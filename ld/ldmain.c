/* Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain steve@cygnus.com

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include "bfd.h"
#include "sysdep.h"

#include "config.h"
#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldwrite.h"
#include "ldgram.h"
#include "ldsym.h"
#include "ldlang.h"
#include "ldemul.h"
#include "ldlex.h"
#include "ldfile.h"
#include "ldindr.h"
#include "ldwarn.h"
#include "ldctor.h"
#include "lderror.h"

/* Somewhere above, sys/stat.h got included . . . . */
#if !defined(S_ISDIR) && defined(S_IFDIR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#include <string.h>

static char *get_emulation ();
static void set_scripts_dir ();

/* IMPORTS */
extern boolean lang_has_input_file;
extern boolean force_make_executable;
extern boolean relaxing;
extern boolean had_script;

/* EXPORTS */

char *default_target;
char *output_filename = "a.out";

/* Name this program was invoked by.  */
char *program_name;

/* The file that we're creating */
bfd *output_bfd = 0;

/* set if -y on the command line */
int had_y;

/* The local symbol prefix */
char lprefix = 'L';

/* Set by -G argument, for MIPS ECOFF target.  */
int g_switch_value = 8;

/* Count the number of global symbols multiply defined.  */
int multiple_def_count;

/* Count the number of symbols defined through common declarations.
   This count is referenced in symdef_library, linear_library, and
   modified by enter_global_ref.

   It is incremented when a symbol is created as a common, and
   decremented when the common declaration is overridden

   Another way of thinking of it is that this is a count of
   all ldsym_types with a ->scoms field */

unsigned int commons_pending;

/* Count the number of global symbols referenced and not defined.
   common symbols are not included in this count.   */

unsigned int undefined_global_sym_count;

/* Nonzero means print names of input files as processed.  */
boolean trace_files;

/* Nonzero means same, but note open failures, too.  */
boolean trace_file_tries;

/* 1 => write load map.  */
boolean write_map;

#ifdef GNU960
/* Indicates whether output file will be b.out (default) or coff */
enum target_flavour output_flavor = BFD_BOUT_FORMAT;
#endif

/* A count of the total number of local symbols ever seen - by adding
 the symbol_count field of each newly read afile.*/

unsigned int total_symbols_seen;

/* A count of the number of read files - the same as the number of elements
 in file_chain
 */
unsigned int total_files_seen;

args_type command_line;

ld_config_type config;

void
main (argc, argv)
     char **argv;
     int argc;
{
  char *emulation;

  program_name = argv[0];

  bfd_init ();

  /* Initialize the data about options.  */


  trace_files = trace_file_tries = false;
  write_map = false;
  config.relocateable_output = false;
  command_line.force_common_definition = false;

  init_bfd_error_vector ();
  ldsym_init ();
  ldfile_add_arch ("");

  set_scripts_dir ();

  config.make_executable = true;
  force_make_executable = false;


  /* Initialize the cumulative counts of symbols.  */
  undefined_global_sym_count = 0;
  multiple_def_count = 0;
  commons_pending = 0;

  config.magic_demand_paged = true;
  config.text_read_only = true;
  config.make_executable = true;

  emulation = get_emulation (argc, argv);
  ldemul_choose_mode (emulation);
  default_target = ldemul_choose_target ();
  lang_init ();
  ldemul_before_parse ();
  lang_has_input_file = false;
  parse_args (argc, argv);

  if (had_script == false)
    {
      /* Read the emulation's appropriate default script.  */
      char *scriptname = ldemul_get_script ();
      /* sizeof counts the terminating NUL.  */
      size_t size = strlen (scriptname) + sizeof ("-T /ldscripts/");
      char *buf = (char *) ldmalloc(size);
      /* The initial slash prevents finding the script in `.' first.  */
      sprintf (buf, "-T /ldscripts/%s", scriptname);
      parse_line (buf, 0);
      free (buf);
    }

  if (config.relocateable_output && command_line.relax)
    {
      einfo ("%P%F: -relax and -r may not be used together\n");
    }
  lang_final ();

  if (lang_has_input_file == false)
    {
      einfo ("%P%F: No input files\n");
    }

  if (trace_files)
    {
      info ("%P: mode %s\n", emulation);
    }

  ldemul_after_parse ();


  if (config.map_filename)
    {
      if (strcmp (config.map_filename, "-") == 0)
	{
	  config.map_file = stdout;
	}
      else
	{
	  config.map_file = fopen (config.map_filename, FOPEN_WT);
	  if (config.map_file == (FILE *) NULL)
	    {
	      einfo ("%P%F: cannot open map file %s: %E\n",
		     config.map_filename);
	    }
	}
    }


  lang_process ();

  /* Print error messages for any missing symbols, for any warning
     symbols, and possibly multiple definitions */


  if (config.text_read_only)
    {
      /* Look for a text section and mark the readonly attribute in it */
      asection *found = bfd_get_section_by_name (output_bfd, ".text");

      if (found != (asection *) NULL)
	{
	  found->flags |= SEC_READONLY;
	}
    }

  if (config.relocateable_output)
    output_bfd->flags &= ~EXEC_P;
  else
    output_bfd->flags |= EXEC_P;

  ldwrite ();

  /* Even if we're producing relocateable output, some non-fatal errors should
     be reported in the exit status.  (What non-fatal errors, if any, do we
     want to ignore for relocateable output?)  */

  if (config.make_executable == false && force_make_executable == false)
    {
      if (trace_files == true)
	{
	  einfo ("%P: Link errors found, deleting executable `%s'\n",
		 output_filename);
	}

      if (output_bfd->iostream)
	fclose ((FILE *) (output_bfd->iostream));

      unlink (output_filename);
      exit (1);
    }
  else
    {
      bfd_close (output_bfd);
    }

  exit (0);
}

/* We need to find any explicitly given emulation in order to initialize the
   state that's needed by the lex&yacc argument parser (parse_args).  */

static char *
get_emulation (argc, argv)
     int argc;
     char **argv;
{
  char *emulation;
  int i;

#ifdef GNU960
  check_v960 (argc, argv);
  emulation = "gld960";
  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-Fcoff"))
	{
	  emulation = "lnk960";
	  output_flavor = BFD_COFF_FORMAT;
	  break;
	}
    }
#else
  emulation = (char *) getenv (EMULATION_ENVIRON);
  if (emulation == NULL)
    emulation = DEFAULT_EMULATION;
#endif

  for (i = 1; i < argc; i++)
    {
      if (!strncmp (argv[i], "-m", 2))
	{
	  if (argv[i][2] == '\0')
	    {
	      /* -m EMUL */
	      if (i < argc - 1)
		{
		  emulation = argv[i + 1];
		  i++;
		}
	      else
		{
		  einfo("%P%F missing argument to -m\n");
		}
	    }
	  else
	    {
	      /* -mEMUL */
	      emulation = &argv[i][2];
	    }
	}
    }

  return emulation;
}

/* If directory DIR contains an "ldscripts" subdirectory,
   add DIR to the library search path and return true,
   else return false.  */

static boolean
check_for_scripts_dir (dir)
     char *dir;
{
  size_t dirlen;
  char *buf;
  struct stat s;
  boolean res;

  dirlen = strlen (dir);
  /* sizeof counts the terminating NUL.  */
  buf = (char *) ldmalloc (dirlen + sizeof("/ldscripts"));
  sprintf (buf, "%s/ldscripts", dir);

  res = stat (buf, &s) == 0 && S_ISDIR (s.st_mode);
  free (buf);
  if (res)
    ldfile_add_library_path (dir);
  return res;
}

/* Set the default directory for finding script files.
   Libraries will be searched for here too, but that's ok.
   We look for the "ldscripts" directory in:

   SCRIPTDIR (passed from Makefile)
   the dir where this program is (for using it from the build tree)
   the dir where this program is/../lib (for installing the tool suite elsewhere) */

static void
set_scripts_dir ()
{
  char *end, *dir;
  size_t dirlen;

  if (check_for_scripts_dir (SCRIPTDIR))
    return;			/* We've been installed normally.  */

  /* Look for "ldscripts" in the dir where our binary is.  */
  end = strrchr (program_name, '/');
  if (end)
    {
      dirlen = end - program_name;
      /* Make a copy of program_name in dir.
	 Leave room for later "/../lib".  */
      dir = (char *) ldmalloc (dirlen + 8);
      strncpy (dir, program_name, dirlen);
      dir[dirlen] = '\0';
    }
  else
    {
      dirlen = 1;
      dir = (char *) ldmalloc (dirlen + 8);
      strcpy (dir, ".");
    }

  if (check_for_scripts_dir (dir))
    return;			/* Don't free dir.  */

  /* Look for "ldscripts" in <the dir where our binary is>/../lib.  */
  strcpy (dir + dirlen, "/../lib");
  if (check_for_scripts_dir (dir))
    return;

  free (dir);			/* Well, we tried.  */
}

void
read_entry_symbols (desc, entry)
     bfd *desc;
     struct lang_input_statement_struct *entry;
{
  if (entry->asymbols == (asymbol **) NULL)
    {
      bfd_size_type table_size = get_symtab_upper_bound (desc);

      entry->asymbols = (asymbol **) ldmalloc (table_size);
      entry->symbol_count = bfd_canonicalize_symtab (desc, entry->asymbols);
    }
}

/*
 * turn this item into a reference
 */
void
refize (sp, nlist_p)
     ldsym_type *sp;
     asymbol **nlist_p;
{
  asymbol *sym = *nlist_p;

  sym->value = 0;
  sym->flags = 0;
  sym->section = &bfd_und_section;
  sym->udata = (PTR) (sp->srefs_chain);
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
enter_global_ref (nlist_p, name)
     asymbol ** nlist_p;	/* pointer into symbol table from incoming bfd */
     CONST char *name; /* name of symbol in linker table */
{
  asymbol *sym = *nlist_p;
  ldsym_type *sp;

  /* Lookup the name from the incoming bfd's symbol table in the
     linker's global symbol table */


  flagword this_symbol_flags = sym->flags;

  sp = ldsym_get (name);


  /* If this symbol already has udata, it means that something strange
     has happened.

     The strange thing is that we've had an undefined symbol resolved by
     an alias, but the thing the alias defined wasn't in the file. So
     the symbol got a udata entry, but the file wasn't loaded.  Then
     later on the file was loaded, but we don't need to do this
     processing again */


  if (sym->udata)
    return;


  if (flag_is_constructor (this_symbol_flags))
    {
      /* Add this constructor to the list we keep */
      ldlang_add_constructor (sp);
      /* Turn any commons into refs */
      if (sp->scoms_chain != (asymbol **) NULL)
	{
	  refize (sp, sp->scoms_chain);
	  sp->scoms_chain = 0;
	}


    }
  else
    {
      if (bfd_is_com_section (sym->section))
	{
	  /* If we have a definition of this symbol already then
	 this common turns into a reference. Also we only
	 ever point to the largest common, so if we
	 have a common, but it's bigger that the new symbol
	 the turn this into a reference too. */
	  if (sp->sdefs_chain)
	    {
	      /* This is a common symbol, but we already have a definition
	   for it, so just link it into the ref chain as if
	   it were a reference  */
	      refize (sp, nlist_p);
	    }
	  else if (sp->scoms_chain)
	    {
	      /* If we have a previous common, keep only the biggest */
	      if ((*(sp->scoms_chain))->value > sym->value)
		{
		  /* other common is bigger, throw this one away */
		  refize (sp, nlist_p);
		}
	      else if (sp->scoms_chain != nlist_p)
		{
		  /* other common is smaller, throw that away */
		  refize (sp, sp->scoms_chain);
		  sp->scoms_chain = nlist_p;
		}
	    }
	  else
	    {
	      /* This is the first time we've seen a common, so remember it
	   - if it was undefined before, we know it's defined now. If
	   the symbol has been marked as really being a constructor,
	   then treat this as a ref
	   */
	      if (sp->flags & SYM_CONSTRUCTOR)
		{
		  /* Turn this into a ref */
		  refize (sp, nlist_p);
		}
	      else
		{
		  /* treat like a common */
		  if (sp->srefs_chain)
		    undefined_global_sym_count--;

		  commons_pending++;
		  sp->scoms_chain = nlist_p;
		}
	    }
	}

      else if (sym->section != &bfd_und_section)
	{
	  /* This is the definition of a symbol, add to def chain */
	  if (sp->sdefs_chain && (*(sp->sdefs_chain))->section != sym->section)
	    {
	      /* Multiple definition */
	      asymbol *sy = *(sp->sdefs_chain);
	      lang_input_statement_type *stat =
	      (lang_input_statement_type *) bfd_asymbol_bfd (sy)->usrdata;
	      lang_input_statement_type *stat1 =
	      (lang_input_statement_type *) bfd_asymbol_bfd (sym)->usrdata;
	      asymbol **stat1_symbols = stat1 ? stat1->asymbols : 0;
	      asymbol **stat_symbols = stat ? stat->asymbols : 0;

	      multiple_def_count++;
	      einfo ("%X%C: multiple definition of `%T'\n",
		     bfd_asymbol_bfd (sym), sym->section, stat1_symbols, sym->value, sym);

	      einfo ("%X%C: first seen here\n",
		bfd_asymbol_bfd (sy), sy->section, stat_symbols, sy->value);
	    }
	  else
	    {
	      sym->udata = (PTR) (sp->sdefs_chain);
	      sp->sdefs_chain = nlist_p;
	    }
	  /* A definition overrides a common symbol */
	  if (sp->scoms_chain)
	    {
	      refize (sp, sp->scoms_chain);
	      sp->scoms_chain = 0;
	      commons_pending--;
	    }
	  else if (sp->srefs_chain && relaxing == false)
	    {
	      /* If previously was undefined, then remember as defined */
	      undefined_global_sym_count--;
	    }
	}
      else
	{
	  if (sp->scoms_chain == (asymbol **) NULL
	      && sp->srefs_chain == (asymbol **) NULL
	      && sp->sdefs_chain == (asymbol **) NULL)
	    {
	      /* And it's the first time we've seen it */
	      undefined_global_sym_count++;

	    }

	  refize (sp, nlist_p);
	}
    }

  ASSERT (sp->sdefs_chain == 0 || sp->scoms_chain == 0);
  ASSERT (sp->scoms_chain == 0 || (*(sp->scoms_chain))->udata == 0);


}

static void
enter_file_symbols (entry)
     lang_input_statement_type *entry;
{
  asymbol **q;

  entry->common_section =
    bfd_make_section_old_way (entry->the_bfd, "COMMON");
  entry->common_section->flags = SEC_NEVER_LOAD;
  ldlang_add_file (entry);


  if (trace_files || trace_file_tries)
    {
      info ("%I\n", entry);
    }

  total_symbols_seen += entry->symbol_count;
  total_files_seen++;
  if (entry->symbol_count)
    {
      for (q = entry->asymbols; *q; q++)
	{
	  asymbol *p = *q;

	  if (had_y && p->name)
	    {
	      /* look up the symbol anyway to see if the trace bit was
	   set */
	      ldsym_type *s = ldsym_get (p->name);
	      if (s->flags & SYM_Y)
		{
		  einfo ("%B: %s %T\n", entry->the_bfd,
			 p->section == &bfd_und_section ? "reference to" : "definition of ",
			 p);
		}
	    }

	  if (p->section == &bfd_ind_section)
	    {
	      add_indirect (q);
	    }
	  else if (p->flags & BSF_WARNING)
	    {
	      add_warning (p);
	    }
	  else if (p->section == &bfd_und_section
		   || (p->flags & BSF_GLOBAL)
		   || bfd_is_com_section (p->section)
		   || (p->flags & BSF_CONSTRUCTOR))

	    {

	      asymbol *p = *q;

	      if (p->flags & BSF_INDIRECT)
		{
		  add_indirect (q);
		}
	      else if (p->flags & BSF_WARNING)
		{
		  add_warning (p);
		}
	      else if (p->section == &bfd_und_section
		       || (p->flags & BSF_GLOBAL)
		       || bfd_is_com_section (p->section)
		       || (p->flags & BSF_CONSTRUCTOR))
		{
		  enter_global_ref (q, p->name);
		}

	    }

	}
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
  if (!undefined_global_sym_count)
    return;

  if (bfd_has_map (entry->the_bfd))
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

  if ((bfd_check_format (abfd, format) == true)
      && (abfd->xvec->flavour == output_flavor))
    {
      return true;
    }


  return false;
}

#endif

void
ldmain_open_file_read_symbol (entry)
     struct lang_input_statement_struct *entry;
{
  if (entry->asymbols == (asymbol **) NULL
      && entry->real == true
      && entry->filename != (char *) NULL)
    {
      ldfile_open_file (entry);


#ifdef GNU960
      if (gnu960_check_format (entry->the_bfd, bfd_object))
#else
      if (bfd_check_format (entry->the_bfd, bfd_object))
#endif
	{
	  entry->the_bfd->usrdata = (PTR) entry;


	  read_entry_symbols (entry->the_bfd, entry);

	  /* look through the sections in the file and see if any of them
	     are constructors */
	  ldlang_check_for_constructors (entry);

	  enter_file_symbols (entry);
	}
#ifdef GNU960
      else if (gnu960_check_format (entry->the_bfd, bfd_archive))
#else
      else if (bfd_check_format (entry->the_bfd, bfd_archive))
#endif
	{
	  entry->the_bfd->usrdata = (PTR) entry;

	  entry->subfiles = (lang_input_statement_type *) NULL;
	  search_library (entry);
	}
      else
	{
	  einfo ("%F%B: malformed input file (not rel or archive) \n",
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


  /* First, check if we already have a loaded
     lang_input_statement_struct  for this library subfile.  If so,
     just return it.  Otherwise, allocate some space and build a new one. */

  if (subfile_offset->usrdata
      && ((struct lang_input_statement_struct *) subfile_offset->usrdata)->
      loaded == true)
    {
      subentry = (struct lang_input_statement_struct *) subfile_offset->usrdata;
    }
  else
    {
      subentry =
	(struct lang_input_statement_struct *)
	ldmalloc ((bfd_size_type) (sizeof (struct lang_input_statement_struct)));

      subentry->filename = subfile_offset->filename;
      subentry->local_sym_name = subfile_offset->filename;
      subentry->asymbols = 0;
      subentry->the_bfd = subfile_offset;
      subentry->subfiles = 0;
      subentry->next = 0;
      subentry->superfile = library_entry;
      subentry->is_archive = false;

      subentry->just_syms_flag = false;
      subentry->loaded = false;
      subentry->chain = 0;
    }
  return subentry;
}

boolean subfile_wanted_p ();
void
clear_syms (entry, offset)
     struct lang_input_statement_struct *entry;
     file_ptr offset;
{
  carsym *car;
  unsigned long indx = bfd_get_next_mapent (entry->the_bfd,
					    BFD_NO_MORE_SYMBOLS,
					    &car);

  while (indx != BFD_NO_MORE_SYMBOLS)
    {
      if (car->file_offset == offset)
	{
	  car->name = 0;
	}
      indx = bfd_get_next_mapent (entry->the_bfd, indx, &car);
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

      int idx = bfd_get_next_mapent (entry->the_bfd,
				     BFD_NO_MORE_SYMBOLS,
				     &exported_library_name);

      not_finished = false;

      while (idx != BFD_NO_MORE_SYMBOLS && undefined_global_sym_count)
	{

	  if (exported_library_name->name)
	    {

	      ldsym_type *sp = ldsym_get_soft (exported_library_name->name);

	      /* If we find a symbol that appears to be needed, think carefully
	         about the archive member that the symbol is in.  */
	      /* So - if it exists, and is referenced somewhere and is
	         undefined or */
	      if (sp && sp->srefs_chain && !sp->sdefs_chain)
		{
		  bfd *archive_member_bfd = bfd_get_elt_at_index (entry->the_bfd, idx);
		  struct lang_input_statement_struct *archive_member_lang_input_statement_struct;

#ifdef GNU960
		  if (archive_member_bfd && gnu960_check_format (archive_member_bfd, bfd_object))
#else
		  if (archive_member_bfd && bfd_check_format (archive_member_bfd, bfd_object))
#endif
		    {

		      /* Don't think carefully about any archive member
		         more than once in a given pass.  */
		      if (prev_archive_member_bfd != archive_member_bfd)
			{

			  prev_archive_member_bfd = archive_member_bfd;

			  /* Read the symbol table of the archive member.  */

			  if (archive_member_bfd->usrdata != (PTR) NULL)
			    {

			      archive_member_lang_input_statement_struct = (lang_input_statement_type *) archive_member_bfd->usrdata;
			    }
			  else
			    {

			      archive_member_lang_input_statement_struct =
				decode_library_subfile (entry, archive_member_bfd);
			      archive_member_bfd->usrdata = (PTR) archive_member_lang_input_statement_struct;

			    }

			  if (archive_member_lang_input_statement_struct == 0)
			    {
			      einfo ("%F%I contains invalid archive member %s\n",
				     entry, sp->name);
			    }

			  if (archive_member_lang_input_statement_struct->loaded == false)
			    {

			      read_entry_symbols (archive_member_bfd, archive_member_lang_input_statement_struct);
			      /* Now scan the symbol table and decide whether to load.  */


			      if (subfile_wanted_p (archive_member_lang_input_statement_struct) == true)

				{
				  /* This member is needed; load it.
			             Since we are loading something on this pass,
			             we must make another pass through the symdef data.  */

				  not_finished = true;

				  enter_file_symbols (archive_member_lang_input_statement_struct);

				  if (prev)
				    prev->chain = archive_member_lang_input_statement_struct;
				  else
				    entry->subfiles = archive_member_lang_input_statement_struct;


				  prev = archive_member_lang_input_statement_struct;


				  /* Clear out this member's symbols from the symdef data
			             so that following passes won't waste time on them.  */
				  clear_syms (entry, exported_library_name->file_offset);
				  archive_member_lang_input_statement_struct->loaded = true;
				}
			    }
			}
		    }
		}
	    }
	  idx = bfd_get_next_mapent (entry->the_bfd, idx, &exported_library_name);
	}
    }
}

void
linear_library (entry)
     struct lang_input_statement_struct *entry;
{
  boolean more_to_do = true;
  register struct lang_input_statement_struct *prev = 0;

  if (entry->complained == false)
    {
      if (entry->the_bfd->xvec->flavour != bfd_target_ieee_flavour)

	{
	  /* IEEE can use table of contents, so this message is bogus */
	  einfo ("%P: library %s has bad table of contents, rerun ranlib\n",
		 entry->the_bfd->filename);
	}
      entry->complained = true;

    }
  while (more_to_do)
    {

      bfd *archive = bfd_openr_next_archived_file (entry->the_bfd, 0);

      more_to_do = false;
      while (archive)
	{
	  /* Don't check this file if it's already been read in
	     once */

	  if (!archive->usrdata ||
	      !((lang_input_statement_type *) (archive->usrdata))->loaded)
	    {
#ifdef GNU960
	      if (gnu960_check_format (archive, bfd_object))
#else
	      if (bfd_check_format (archive, bfd_object))
#endif
		{
		  register struct lang_input_statement_struct *subentry;

		  subentry = decode_library_subfile (entry,
						     archive);

		  archive->usrdata = (PTR) subentry;
		  if (!subentry)
		    return;
		  if (subentry->loaded == false)
		    {
		      read_entry_symbols (archive, subentry);

		      if (subfile_wanted_p (subentry) == true)
			{
			  enter_file_symbols (subentry);

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
	    }
	  archive = bfd_openr_next_archived_file (entry->the_bfd, archive);

	}

    }
}

/* ENTRY is an entry for a file inside an archive
    Its symbols have been read into core, but not entered into the
    linker ymbol table
    Return nonzero if we ought to load this file */

boolean
subfile_wanted_p (entry)
     struct lang_input_statement_struct *entry;
{
  asymbol **q;

  if (entry->symbol_count == 0)
    return false;

  for (q = entry->asymbols; *q; q++)
    {
      asymbol *p = *q;

      /* If the symbol has an interesting definition, we could
	 potentially want it.  */

      if (p->flags & BSF_INDIRECT)
	{
	  /**	add_indirect(q);*/
	}

      if (bfd_is_com_section (p->section)
	  || (p->flags & BSF_GLOBAL)
	  || (p->flags & BSF_INDIRECT))
	{
	  register ldsym_type *sp = ldsym_get_soft (p->name);

	  /* If this symbol has not been hashed,
	     we can't be looking for it. */
	  if (sp != (ldsym_type *) NULL
	      && sp->sdefs_chain == (asymbol **) NULL)
	    {
	      if (sp->srefs_chain != (asymbol **) NULL
		  || sp->scoms_chain != (asymbol **) NULL)
		{
		  /* This is a symbol we are looking for.  It is
		     either not yet defined or common.  If this is a
		     common symbol, then if the symbol in the object
		     file is common, we need to combine sizes.  But if
		     we already have a common symbol, and the symbol
		     in the object file is not common, we don't want
		     the object file: it is providing a definition for
		     a symbol that we already have a definition for
		     (this is the else condition below).  */
		  if (bfd_is_com_section (p->section))
		    {

		      /* If the symbol in the table is a constructor, we won't to
		         anything fancy with it */
		      if ((sp->flags & SYM_CONSTRUCTOR) == 0)
			{
			  /* This libary member has something to
		             say about this element. We should
		             remember if its a new size  */
			  /* Move something from the ref list to the com list */
			  if (sp->scoms_chain)
			    {
			      /* Already a common symbol, maybe update it */
			      if (p->value > (*(sp->scoms_chain))->value)
				{
				  (*(sp->scoms_chain))->value = p->value;
				}
			    }
			  else
			    {
			      /* Take a value from the ref chain
				 Here we are moving a symbol from the owning bfd
				 to another bfd. We must set up the
				 common_section portion of the bfd thing */



			      sp->scoms_chain = sp->srefs_chain;
			      sp->srefs_chain =
				(asymbol **) ((*(sp->srefs_chain))->udata);
			      (*(sp->scoms_chain))->udata = (PTR) NULL;

			      (*(sp->scoms_chain))->section = p->section;
			      (*(sp->scoms_chain))->flags = 0;
			      /* Remember the size of this item */
			      sp->scoms_chain[0]->value = p->value;
			      commons_pending++;
			      undefined_global_sym_count--;
			    }
			  {
			    asymbol *com = *(sp->scoms_chain);

			    if (((lang_input_statement_type *)
				 (bfd_asymbol_bfd (com)->usrdata))->common_section ==
				(asection *) NULL)
			      {
				((lang_input_statement_type *)
				 (bfd_asymbol_bfd (com)->usrdata))->common_section =
				  bfd_make_section_old_way (bfd_asymbol_bfd (com), "COMMON");
			      }
			  }
			}
		      ASSERT (p->udata == 0);
		    }
		  else if (sp->scoms_chain == (asymbol **) NULL)
		    {
		      if (write_map)
			{
			  info ("%I needed due to %s\n", entry, sp->name);
			}
		      return true;
		    }
		}
	    }
	}
    }

  return false;
}

void
add_ysym (text)
     char *text;
{
  ldsym_type *lookup = ldsym_get (text);
  lookup->flags |= SYM_Y;
  had_y = 1;
}
