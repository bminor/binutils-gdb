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
 *  $Id$ 
 *
 *
*/

/* 
   Written by Steve Chamberlain steve@cygnus.com
 
   All symbol handling for the linker
 */


#include "sysdep.h"
#include "bfd.h"

#include "ld.h"
#include "ldsym.h"
#include "ldmisc.h"
#include "ldlang.h"
/* IMPORT */

extern bfd *output_bfd;
/* Head and tail of global symbol table chronological list */

ldsym_type *symbol_head = (ldsym_type *)NULL;
ldsym_type **symbol_tail_ptr = &symbol_head;

/*
  incremented for each symbol in the ldsym_type table
  no matter what flavour it is 
*/
unsigned int global_symbol_count;

/* IMPORTS */

extern boolean option_longmap ;

/* LOCALS */
#define	TABSIZE	1009
static ldsym_type *global_symbol_hash_table[TABSIZE];

/* Compute the hash code for symbol name KEY.  */

int
hash_string (key)
     char *key;
{
  register char *cp;
  register int k;

  cp = key;
  k = 0;
  while (*cp)
    k = (((k << 1) + (k >> 14)) ^ (*cp++)) & 0x3fff;

  return k;
}

/* Get the symbol table entry for the global symbol named KEY.
   Create one if there is none.  */
ldsym_type *
ldsym_get (key)
     char *key;
{
  register int hashval;
  register ldsym_type *bp;

  /* Determine the proper bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */

  for (bp = global_symbol_hash_table[hashval]; bp; bp = bp->link)
    if (! strcmp (key, bp->name))
      return bp;

  /* Nothing was found; create a new symbol table entry.  */

  bp = (ldsym_type *) ldmalloc (sizeof (ldsym_type));
  bp->srefs_chain = (asymbol **)NULL;
  bp->sdefs_chain = (asymbol **)NULL;
  bp->scoms_chain = (asymbol **)NULL;
  bp->name = (char *) ldmalloc (strlen (key) + 1);
  strcpy (bp->name, key);




  /* Add the entry to the bucket.  */

  bp->link = global_symbol_hash_table[hashval];
  global_symbol_hash_table[hashval] = bp;

  /* Keep the chronological list up to date too */
  *symbol_tail_ptr = bp;
  symbol_tail_ptr = &bp->next;
  bp->next = 0;
  global_symbol_count++;

  return bp;
}

/* Like `ldsym_get' but return 0 if the symbol is not already known.  */

ldsym_type *
ldsym_get_soft (key)
     char *key;
{
  register int hashval;
  register ldsym_type *bp;

  /* Determine which bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */

  for (bp = global_symbol_hash_table[hashval]; bp; bp = bp->link)
    if (! strcmp (key, bp->name))
      return bp;

  return 0;
}





static void
list_file_locals (entry)
lang_input_statement_type *entry;
{
  asymbol **q;
  fprintf (stderr, "\nLocal symbols of ");
  info("%I", entry);
  fprintf (stderr, ":\n\n");
  if (entry->asymbols) {
    for (q = entry->asymbols; *q; q++) 
      {
	asymbol *p = *q;
	/* If this is a definition,
	   update it if necessary by this file's start address.  */
	if (p->flags & BSF_LOCAL)
	 info("  %V %s\n",p->value, p->name);
      }
  }
}


static void
print_file_stuff(f)
lang_input_statement_type *f;
{
  fprintf (stderr, "  %s", f->filename);
  fprintf (stderr, " ");
  if (f->just_syms_flag) 
    {
      fprintf (stderr, " symbols only\n");
    }
  else 
    {
      asection *s;
      if (option_longmap) {
	for (s = f->the_bfd->sections;
	     s != (asection *)NULL;
	     s = s->next) {
	  fprintf (stderr, "%08lx %08x 2**%2ud %s\n",
		   s->output_offset,
		   (unsigned)s->size, s->alignment_power, s->name);
	}
      }
      else {	      
	for (s = f->the_bfd->sections;
	     s != (asection *)NULL;
	     s = s->next) {
	  fprintf (stderr, "%s %lx(%x) ",
		   s->name,
		   s->output_offset,
		   (unsigned)   s->size);
	}
	fprintf (stderr, "hex \n");
      }
    }
}

void
ldsym_print_symbol_table ()
{
  fprintf (stderr, "\nFiles:\n\n");

  lang_for_each_file(print_file_stuff);

  fprintf (stderr, "\nGlobal symbols:\n\n");
  {
    register ldsym_type *sp;

    for (sp = symbol_head; sp; sp = sp->next)
      {
	if (sp->sdefs_chain) 
	  {
	    asymbol *defsym = *(sp->sdefs_chain);
	    asection *defsec = bfd_get_section(defsym);
	    fprintf(stderr,"%08lx ",defsym->value);
	    if (defsec)
	      {
		fprintf(stderr,"%08lx ",defsym->value+defsec->vma);
		fprintf(stderr,
			"%7s",
			bfd_section_name(output_bfd,
					 defsec));

	      }
	    else 
	      {
		fprintf(stderr,"         .......");
	      }

	  }	
	else {
	  fprintf(stderr,"undefined");
	}


	if (sp->scoms_chain) {
	  fprintf(stderr, " common size %5lu    %s",
		  (*(sp->scoms_chain))->value, sp->name);
	}
	if (sp->sdefs_chain) {
	  fprintf(stderr, " symbol def %08lx    %s",
		  (*(sp->sdefs_chain))->value,
		  sp->name);
	}
	else {
	  fprintf(stderr, " undefined    %s",
		  sp->name);
	}
	fprintf(stderr, "\n");

      }
  }
  lang_for_each_file(list_file_locals);
}

extern lang_output_section_statement_type *create_object_symbols;
extern char lprefix;
static asymbol **
write_file_locals(output_buffer)
asymbol **output_buffer;
{
LANG_FOR_EACH_INPUT_STATEMENT(entry)
    {
      /* Run trough the symbols and work out what to do with them */
      unsigned int i;

      /* Add one for the filename symbol if needed */
      if (create_object_symbols 
	  != (lang_output_section_statement_type *)NULL) {
	asection *s;
	for (s = entry->the_bfd->sections;
	     s != (asection *)NULL;
	     s = s->next) {
	  if (s->output_section == create_object_symbols->bfd_section) {
	    /* Add symbol to this section */
	    asymbol * newsym  =
	      (asymbol *)bfd_make_empty_symbol(entry->the_bfd);
	    newsym->name = entry->local_sym_name;
	    /* The symbol belongs to the output file's text section */

	    /* The value is the start of this section in the output file*/
	    newsym->value  = 0;
	    newsym->flags = BSF_LOCAL;
	    newsym->section = s;
	    *output_buffer++ = newsym;
	    break;
	  }
	}
      }
      for (i = 0; i < entry->symbol_count; i++) 
	{
	  asymbol *p = entry->asymbols[i];

	  if (flag_is_global(p->flags) || flag_is_absolute(p->flags))
	    {
	      /* We are only interested in outputting 
		 globals at this stage in special circumstances */
	      if (p->the_bfd == entry->the_bfd 
		  && flag_is_not_at_end(p->flags)) {
		/* And this is one of them */
		*(output_buffer++) = p;
		p->flags |= BSF_KEEP;
	      }
	    }
	  else {
	    if (flag_is_ordinary_local(p->flags)) 
	      {
		if (discard_locals == DISCARD_ALL)
		  {  }
		else if (discard_locals == DISCARD_L &&
			 (p->name[0] == lprefix)) 
		  {  }
		else if (p->flags ==  BSF_WARNING) 
		  {  }
		else 
		  { *output_buffer++ = p; }
	      }
	    else if (flag_is_debugger(p->flags)) 
	      {
		/* Only keep the debugger symbols if no stripping required */
		if (strip_symbols == STRIP_NONE) {
		  *output_buffer++ = p;
		}
	      }
	    else if (flag_is_undefined(p->flags)) 
	      { /* This must be global */
	      }
	    else if (flag_is_common(p->flags)) {
	   /* And so must this */
	    } 
	    else if (p->flags & BSF_CTOR) {
	      /* Throw it away */
	    }
else
	      {
		FAIL();
	      }
	  }
	}


    }
  return output_buffer;
}


static asymbol **
write_file_globals(symbol_table)
asymbol **symbol_table;
{
  FOR_EACH_LDSYM(sp)
    {
      if (sp->sdefs_chain != (asymbol **)NULL) {
	asymbol *bufp = (*(sp->sdefs_chain));

	if ((bufp->flags & BSF_KEEP) ==0) {
	  ASSERT(bufp != (asymbol *)NULL);

	  bufp->name = sp->name;

	  if (sp->scoms_chain != (asymbol **)NULL)	

	    {
	      /* 
		 defined as common but not allocated, this happens
		 only with -r and not -d, write out a common
		 definition
		 */
	      bufp = *(sp->scoms_chain);
	    }
	  *symbol_table++ = bufp;
	}
      }
      else if (sp->scoms_chain != (asymbol **)NULL) {
	/* This symbol is a common - just output */
	asymbol *bufp = (*(sp->scoms_chain));
	*symbol_table++ = bufp;
      }
      else if (sp->srefs_chain != (asymbol **)NULL) {
	/* This symbol is undefined but has a reference */
	asymbol *bufp = (*(sp->srefs_chain));
	*symbol_table++ = bufp;
      }
      else {
	/*
	   This symbol has neither defs nor refs, it must have come
	   from the command line, since noone has used it it has no
	   data attatched, so we'll ignore it 
	   */
      }
    }
  return symbol_table;
}



void
ldsym_write()
{
  if (strip_symbols != STRIP_ALL) {
    /* We know the maximum size of the symbol table -
       it's the size of all the global symbols ever seen +
       the size of all the symbols from all the files +
       the number of files (for the per file symbols)
       +1 (for the null at the end)
       */
    extern unsigned int total_files_seen;
    extern unsigned int total_symbols_seen;

    asymbol **  symbol_table =  (asymbol **) 
      ldmalloc ((size_t)(global_symbol_count +
			 total_files_seen +
			 total_symbols_seen + 1) *     sizeof (asymbol *));
    asymbol ** tablep = write_file_locals(symbol_table);

    tablep = write_file_globals(tablep);

    *tablep =  (asymbol *)NULL;
    bfd_set_symtab(output_bfd, symbol_table, (unsigned)( tablep - symbol_table));
  }
}

/*
return true if the supplied symbol name is not in the 
linker symbol table
*/
boolean 
ldsym_undefined(sym)
char *sym;
{
  ldsym_type *from_table = ldsym_get_soft(sym);
  if (from_table != (ldsym_type *)NULL) {
    if (from_table->sdefs_chain != (asymbol **)NULL) return false;
  }
  return true;
}
