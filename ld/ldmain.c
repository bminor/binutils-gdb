/* Copyright (C) 1991 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* 
 *  Written by Steve Chamberlain steve@cygnus.com
 *
 * $Id$ 
 *
 *
 */


#include "sysdep.h"
#include "bfd.h"

#include "config.h"
#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldwrite.h"
#include "ldgram.h"
#include "ldsym.h"
#include "ldlang.h"
#include "ld-emul.h"
#include "ldlex.h"
#include "ldfile.h"

/* IMPORTS */
extern boolean lang_has_input_file;
extern boolean trace_files;
/* EXPORTS */

char *default_target;
char *output_filename = "a.out";
/* Name this program was invoked by.  */
char *program_name;

/* The file that we're creating */
bfd *output_bfd;

extern boolean option_v;

/* The local symbol prefix */
char lprefix = 'L';

/* Count the number of global symbols multiply defined.  */
int multiple_def_count;


/* Count the number of symbols defined through common declarations.
   This count is referenced in symdef_library, linear_library, and
   modified by enter_global_ref.

   It is incremented when a symbol is created as a common, and
   decremented when the common declaration is overridden

   Another way of thinking of it is that this is a count of
   all ldsym_types with a ->scoms field
*/
unsigned int commons_pending;


/* Count the number of global symbols referenced and not defined. 
   common symbols are not included in this count.
  */

unsigned int undefined_global_sym_count;



/* Count the number of warning symbols encountered. */
int warning_count;

/* have we had a load script ? */
extern boolean had_script;



/* Nonzero means print names of input files as processed.  */
boolean trace_files;



/* 1 => write load map.  */
boolean write_map;


int unix_relocate;








/* Force the make_executable to be output, even if there are non-fatal
   errors */
boolean force_make_executable;


/* A count of the total number of local symbols ever seen - by adding
 the symbol_count field of each newly read afile.*/


unsigned int total_symbols_seen;

/* A count of the number of read files - the same as the number of elements
 in file_chain
 */
unsigned int total_files_seen;


/* IMPORTS */
args_type command_line;
ld_config_type config;
int
main (argc, argv)
     char **argv;
     int argc;
{
  char *emulation;
  program_name = argv[0];
  output_filename = "a.out";

#ifdef GNU960
  check_v960( argc, argv );
#endif

  emulation =  (char *) getenv(EMULATION_ENVIRON); 

  /* Initialize the data about options.  */

  trace_files = false;


  write_map = false;
  config.relocateable_output = false;
  unix_relocate = 0;
  command_line.force_common_definition = false;

  ldfile_add_arch("");
  ldfile_add_library_path("./");
  config.make_executable = true;
  force_make_executable = false;


  /* Initialize the cumulative counts of symbols.  */
  undefined_global_sym_count = 0;
  warning_count = 0;
  multiple_def_count = 0;
  commons_pending = 0;

  config.magic_demand_paged = true ;
  config.make_executable = true;

#ifdef GNU960
  ldemul_choose_mode(LNK960_EMULATION_NAME);
#else
  if (emulation == (char *)NULL) {
    emulation= DEFAULT_EMULATION;
  }


  ldemul_choose_mode(emulation);
#endif

  default_target =  ldemul_choose_target();

  lang_init();
  ldemul_before_parse();
  lang_has_input_file = false;
  parse_args(argc, argv);
  lang_final(); 
  if (trace_files) {
    info("%P: mode %s\n", emulation);
  }
  if (lang_has_input_file == false) {
    info("%P%F: No input files\n");
  }

  ldemul_after_parse();

  lang_process();




  /* Print error messages for any missing symbols, for any warning
     symbols, and possibly multiple definitions */

  /* Print a map, if requested.  */

  if (write_map) {
    ldsym_print_symbol_table ();
    lang_map(stdout);
  }


  if (config.relocateable_output) {
    output_bfd->flags &=  ~( D_PAGED);
    output_bfd->flags |= EXEC_P;
    ldwrite();
    bfd_close(output_bfd);
  }
  else {
    output_bfd->flags |= EXEC_P;

    ldwrite();
    bfd_close(output_bfd);
    if (config.make_executable == false && force_make_executable == false) {
      unlink(output_filename);
    }
    return (!config.make_executable);
  }

  return(0);
} /* main() */


void
Q_read_entry_symbols (desc, entry)
     bfd *desc;
     struct lang_input_statement_struct *entry;
{
  if (entry->asymbols == (asymbol **)NULL) {
    size_t table_size = get_symtab_upper_bound(desc);
    entry->asymbols = (asymbol **)ldmalloc(table_size);

    entry->symbol_count =  bfd_canonicalize_symtab(desc, entry->asymbols) ;
  }
}


/*
 * turn this item into a reference 
 */
static void
refize(sp, nlist_p)
ldsym_type *sp;
asymbol **nlist_p;
{
  asymbol *sym = *nlist_p;
  sym->value = 0;
  sym->flags = BSF_UNDEFINED;
  sym->section = (asection *)NULL;
  sym->udata =(PTR)( sp->srefs_chain);
  sp->srefs_chain = nlist_p;
}
/*
This function is called for each name which is seen which has a global
scope. It enters the name into the global symbol table in the correct
symbol on the correct chain. Remember that each ldsym_type has three
chains attatched, one of all definitions of a symbol, one of all
references of a symbol and one of all common definitions of a symbol.

When the function is over, the supplied is left connected to the bfd
to which is was born, with its udata field pointing to the next member
on the chain in which it has been inserted.

A certain amount of jigery pokery is necessary since commons come
along and upset things, we only keep one item in the common chain; the
one with the biggest size seen sofar. When another common comes along
it either bumps the previous definition into the ref chain, since it
is bigger, or gets turned into a ref on the spot since the one on the
common chain is already bigger. If a real definition comes along then
the common gets bumped off anyway.

Whilst all this is going on we keep a count of the number of multiple
definitions seen, undefined global symbols and pending commons.
*/


void
Q_enter_global_ref (nlist_p)
     asymbol **nlist_p;

{
  asymbol *sym = *nlist_p;
  CONST char *name = sym->name;
  ldsym_type *sp = ldsym_get (name);

  flagword this_symbol_flags = sym->flags;


  ASSERT(sym->udata == 0);

  /* Just place onto correct chain */
  if (flag_is_common(this_symbol_flags)) {
    /* If we have a definition of this symbol already then
     * this common turns into a reference. Also we only
     * ever point to the largest common, so if we
     * have a common, but it's bigger that the new symbol
     * the turn this into a reference too.
     */
    if (sp->sdefs_chain)  
      {
	/* This is a common symbol, but we already have a definition
	   for it, so just link it into the ref chain as if
	   it were a reference
	   */
	refize(sp, nlist_p);
      }
    else  if (sp->scoms_chain) {
      /* If we have a previous common, keep only the biggest */
      if ( (*(sp->scoms_chain))->value > sym->value) {
	/* other common is bigger, throw this one away */
	refize(sp, nlist_p);
      }
      else if (sp->scoms_chain != nlist_p) {
	/* other common is smaller, throw that away */
	refize(sp, sp->scoms_chain);
	sp->scoms_chain = nlist_p;
      }
    }
    else {
      /* This is the first time we've seen a common, so
       * remember it - if it was undefined before, we know it's defined now
       */
      if (sp->srefs_chain)
	undefined_global_sym_count--;

      commons_pending++;
      sp->scoms_chain = nlist_p;
    }
  }

  else if (flag_is_defined(this_symbol_flags)) {
    /* This is the definition of a symbol, add to def chain */
    if (sp->sdefs_chain && (*(sp->sdefs_chain))->section != sym->section) {
      /* Multiple definition */
      asymbol *sy = *(sp->sdefs_chain);
      lang_input_statement_type *stat = (lang_input_statement_type *) sy->the_bfd->usrdata;
      lang_input_statement_type *stat1 = (lang_input_statement_type *) sym->the_bfd->usrdata;
      asymbol ** stat1_symbols  = stat1 ? stat1->asymbols: 0;
      asymbol ** stat_symbols = stat ? stat->asymbols:0;
      
      multiple_def_count++;
      info("%C: multiple definition of `%T'\n",
	   sym->the_bfd,
	   sym->section,
	   stat1_symbols,
	   sym->value,
	   sym);
	   
      info("%C: first seen here\n",
	   sy->the_bfd,
	   sy->section,
	   stat_symbols,
	   sy->value);
    }
    else {
      sym->udata =(PTR)( sp->sdefs_chain);
      sp->sdefs_chain = nlist_p;
    }
    /* A definition overrides a common symbol */
    if (sp->scoms_chain) {
      refize(sp, sp->scoms_chain);
      sp->scoms_chain = 0;
      commons_pending--;
    }
    else if (sp->srefs_chain) {
      /* If previously was undefined, then remember as defined */
      undefined_global_sym_count--;
    }
  }
  else {
    if (sp->scoms_chain == (asymbol **)NULL 
	&& sp->srefs_chain == (asymbol **)NULL 
	&& sp->sdefs_chain == (asymbol **)NULL) {
      /* And it's the first time we've seen it */
      undefined_global_sym_count++;

    }

    refize(sp, nlist_p);
  }

  ASSERT(sp->sdefs_chain == 0 || sp->scoms_chain == 0);
  ASSERT(sp->scoms_chain ==0 || (*(sp->scoms_chain))->udata == 0);


}

static void
Q_enter_file_symbols (entry)
lang_input_statement_type *entry;
{
  asymbol **q ;
  entry->common_section =
    bfd_make_section(entry->the_bfd, "COMMON");
  
  ldlang_add_file(entry);


  if (trace_files || option_v) {
    info("%I\n", entry);
  }

  total_symbols_seen += entry->symbol_count;
  total_files_seen ++;
  for (q = entry->asymbols; *q; q++)
    {
      asymbol *p = *q;

      if (flag_is_undefined_or_global_or_common(p->flags))
	{
	  Q_enter_global_ref(q);
	}
      ASSERT(p->flags != 0);
    }
}



/* Searching libraries */

struct lang_input_statement_struct *decode_library_subfile ();
void linear_library (), symdef_library ();

/* Search the library ENTRY, already open on descriptor DESC.
   This means deciding which library members to load,
   making a chain of `struct lang_input_statement_struct' for those members,
   and entering their global symbols in the hash table.  */

void
search_library (entry)
     struct lang_input_statement_struct *entry;
{

  /* No need to load a library if no undefined symbols */
  if (!undefined_global_sym_count) return;

  if (bfd_has_map(entry->the_bfd)) 
    symdef_library (entry);
  else
    linear_library (entry);

}


#ifdef GNU960
static
boolean
gnu960_check_format (abfd, format)
bfd *abfd;
bfd_format format;
{
  boolean retval;

  if ((bfd_check_format(abfd,format) == true) && BFD_COFF_FILE_P(abfd)) {
	return true;
  }
  return false;
}
#endif

void
ldmain_open_file_read_symbol (entry)
struct lang_input_statement_struct *entry;
{
  if (entry->asymbols == (asymbol **)NULL
      &&entry->real == true 
      && entry->filename != (char *)NULL)
    {
      ldfile_open_file (entry);

#ifdef GNU960
      if (gnu960_check_format(entry->the_bfd, bfd_object))
#else
      if (bfd_check_format(entry->the_bfd, bfd_object))
#endif
	{
	  entry->the_bfd->usrdata = (PTR)entry;


	  Q_read_entry_symbols (entry->the_bfd, entry);
	  Q_enter_file_symbols (entry);
	}
#ifdef GNU960
      else if (gnu960_check_format(entry->the_bfd, bfd_archive)) 
#else
      else if (bfd_check_format(entry->the_bfd, bfd_archive)) 
#endif
	{
	  entry->the_bfd->usrdata = (PTR)entry;

	  entry->subfiles = (lang_input_statement_type *)NULL;
	  search_library (entry);
	}
      else 
	{
	  info("%F%B: malformed input file (not rel or archive) \n",
						entry->the_bfd);
	}
    }

}


/* Construct and return a lang_input_statement_struct for a library member.
   The library's lang_input_statement_struct is library_entry,
   and the library is open on DESC.
   SUBFILE_OFFSET is the byte index in the library of this member's header.
   We store the length of the member into *LENGTH_LOC.  */

lang_input_statement_type *
decode_library_subfile (library_entry, subfile_offset)
     struct lang_input_statement_struct *library_entry;
     bfd *subfile_offset;
{
  register struct lang_input_statement_struct *subentry;
  subentry = (struct lang_input_statement_struct *) ldmalloc (sizeof (struct lang_input_statement_struct));
  subentry->filename = subfile_offset -> filename;
  subentry->local_sym_name  = subfile_offset->filename;
  subentry->asymbols = 0;
  subentry->the_bfd = subfile_offset;
  subentry->subfiles = 0;
  subentry->next = 0;
  subentry->superfile = library_entry;
  subentry->is_archive = false;

  subentry->just_syms_flag = false;
  subentry->loaded = false;
  subentry->chain = 0;

  return subentry;
}

boolean  subfile_wanted_p ();
void
clear_syms(entry, offset)
struct lang_input_statement_struct *entry;
file_ptr offset;
{
  carsym *car;
  unsigned long indx = bfd_get_next_mapent(entry->the_bfd,
					   BFD_NO_MORE_SYMBOLS,
					   &car);
  while (indx != BFD_NO_MORE_SYMBOLS) {
    if (car->file_offset == offset) {
      car->name = 0;
    }
    indx = bfd_get_next_mapent(entry->the_bfd, indx, &car);
  }

}

/* Search a library that has a map
 */
void
symdef_library (entry)
     struct lang_input_statement_struct *entry;

{
  register struct lang_input_statement_struct *prev = 0;

  boolean not_finished = true;


  while (not_finished == true)
    {
      carsym *exported_library_name;
      bfd *prev_archive_member_bfd = 0;    

      int idx = bfd_get_next_mapent(entry->the_bfd,
				    BFD_NO_MORE_SYMBOLS,
				    &exported_library_name);

      not_finished = false;

      while (idx != BFD_NO_MORE_SYMBOLS  && undefined_global_sym_count)
	{

	  if (exported_library_name->name) 
	    {

	      ldsym_type *sp =  ldsym_get_soft (exported_library_name->name);

	      /* If we find a symbol that appears to be needed, think carefully
		 about the archive member that the symbol is in.  */
	      /* So - if it exists, and is referenced somewhere and is
		 undefined or */
	      if (sp && sp->srefs_chain && !sp->sdefs_chain)
		{
		  bfd *archive_member_bfd = bfd_get_elt_at_index(entry->the_bfd, idx);
		  struct lang_input_statement_struct *archive_member_lang_input_statement_struct;

#ifdef GNU960
		  if (archive_member_bfd && gnu960_check_format(archive_member_bfd, bfd_object)) 
#else
		  if (archive_member_bfd && bfd_check_format(archive_member_bfd, bfd_object)) 
#endif
		    {

		      /* Don't think carefully about any archive member
			 more than once in a given pass.  */
		      if (prev_archive_member_bfd != archive_member_bfd)
			{

			  prev_archive_member_bfd = archive_member_bfd;

			  /* Read the symbol table of the archive member.  */

			  if (archive_member_bfd->usrdata != (PTR)NULL) {

			    archive_member_lang_input_statement_struct =(lang_input_statement_type *) archive_member_bfd->usrdata;
			  }
			  else {

			    archive_member_lang_input_statement_struct =
			      decode_library_subfile (entry, archive_member_bfd);
			    archive_member_bfd->usrdata = (PTR) archive_member_lang_input_statement_struct;

			  }

	  if (archive_member_lang_input_statement_struct == 0) {
	    info ("%F%I contains invalid archive member %s\n",
		    entry,
		    sp->name);
	  }

			  if (archive_member_lang_input_statement_struct->loaded == false)  
			    {

			      Q_read_entry_symbols (archive_member_bfd, archive_member_lang_input_statement_struct);
			      /* Now scan the symbol table and decide whether to load.  */


			      if (subfile_wanted_p (archive_member_lang_input_statement_struct) == true)

				{
				  /* This member is needed; load it.
				     Since we are loading something on this pass,
				     we must make another pass through the symdef data.  */

				  not_finished = true;

				  Q_enter_file_symbols (archive_member_lang_input_statement_struct);

				  if (prev)
				    prev->chain = archive_member_lang_input_statement_struct;
				  else
				    entry->subfiles = archive_member_lang_input_statement_struct;


				  prev = archive_member_lang_input_statement_struct;


				  /* Clear out this member's symbols from the symdef data
				     so that following passes won't waste time on them.  */
				  clear_syms(entry, exported_library_name->file_offset);
				  archive_member_lang_input_statement_struct->loaded = true;
				}
			    }
			}
		    }
		}
	    }
	  idx = bfd_get_next_mapent(entry->the_bfd, idx, &exported_library_name);
	}
    }
}

void
linear_library (entry)
struct lang_input_statement_struct *entry;
{
  boolean more_to_do = true;
  register struct lang_input_statement_struct *prev = 0;

  while (more_to_do) {

    bfd *  archive = bfd_openr_next_archived_file(entry->the_bfd,0);

    more_to_do = false;
    while (archive) {
#ifdef GNU960
      if (gnu960_check_format(archive, bfd_object)) 
#else
      if (bfd_check_format(archive, bfd_object)) 
#endif
	{
	  register struct lang_input_statement_struct *subentry;

	  subentry = decode_library_subfile (entry,
					     archive);

	  archive->usrdata = (PTR) subentry;
	  if (!subentry) return;
	  if (subentry->loaded == false) {
	    Q_read_entry_symbols (archive, subentry);

	    if (subfile_wanted_p (subentry) == true)
	      {
		Q_enter_file_symbols (subentry);

		if (prev)
		  prev->chain = subentry;
		else 
		  entry->subfiles = subentry;
		prev = subentry;

		more_to_do = true;
		subentry->loaded = true;
	      }
	  }
	}
      archive = bfd_openr_next_archived_file(entry->the_bfd,archive);
 
    }

  }
}

  /* ENTRY is an entry for a library member.
     Its symbols have been read into core, but not entered.
     Return nonzero if we ought to load this member.  */

boolean
subfile_wanted_p (entry)
struct lang_input_statement_struct *entry;
{
  asymbol **q;

  for (q = entry->asymbols; *q; q++)
    {
      asymbol *p = *q;

      /* If the symbol has an interesting definition, we could
	 potentially want it.  */

      if (p->flags & BSF_FORT_COMM 
	  || p->flags & BSF_GLOBAL)
	{
	  register ldsym_type *sp = ldsym_get_soft (p->name);


	  /* If this symbol has not been hashed,
	     we can't be looking for it. */
	  if (sp != (ldsym_type *)NULL 
	      && sp->sdefs_chain == (asymbol **)NULL) {
	    if (sp->srefs_chain  != (asymbol **)NULL
		|| sp->scoms_chain != (asymbol **)NULL)
	      {
		/* This is a symbol we are looking for.  It is either
		   not yet defined or common.  */

		if (flag_is_common(p->flags))
		  {
		    /* This libary member has something to
		       say about this element. We should 
		       remember if its a new size  */
		    /* Move something from the ref list to the com list */
		    if(sp->scoms_chain) {
		      /* Already a common symbol, maybe update it */
		      if (p->value > (*(sp->scoms_chain))->value) {
			(*(sp->scoms_chain))->value = p->value;
		      }
		    }
		    else {
		      /* Take a value from the ref chain
			 Here we are moving a symbol from the owning bfd
			 to another bfd. We must set up the
			 common_section portion of the bfd thing */

		      

		      sp->scoms_chain = sp->srefs_chain;
		      sp->srefs_chain =
			(asymbol **)((*(sp->srefs_chain))->udata);
		      (*(sp->scoms_chain))->udata = (PTR)NULL;

		      (*(  sp->scoms_chain))->flags = BSF_FORT_COMM;
		      /* Remember the size of this item */
		      sp->scoms_chain[0]->value = p->value;
		      commons_pending++;
		      undefined_global_sym_count--;
		    } {
		      asymbol *com = *(sp->scoms_chain);
		      if (((lang_input_statement_type *)
			   (com->the_bfd->usrdata))->common_section ==
			  (asection *)NULL) {
			((lang_input_statement_type *)
			 (com->the_bfd->usrdata))->common_section = 
			   bfd_make_section(com->the_bfd, "COMMON");
		      }
		    }
		    ASSERT(p->udata == 0);
		  }
	      
		else {
		  if (write_map)
		    {
		      info("%I needed due to %s\n",entry, sp->name);
		    }
		  return true;
		}
	      }
	  }
	}
    }

  return false;
}

