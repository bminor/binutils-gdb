/* All symbol handling for the linker
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain steve@cygnus.com

This file is part of GLD, the Gnu Linker.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
   We keep a hash table of global symbols. Each entry in a hash table
   is called an ldsym_type. Each has three chains; a pointer to a
   chain of definitions for the symbol (hopefully one long), a pointer
   to a chain of references to the symbol, and a pointer to a chain of
   common symbols. Each pointer points into the canonical symbol table
   provided by bfd, each one of which points to an asymbol. During
   linkage, the linker uses the udata field to point to the next entry
   in a canonical table....


   ld_sym
			|          |
   +----------+		+----------+
   | defs     |      a canonical symbol table
   +----------+         +----------+
   | refs     | ----->  | one entry|  -----> asymbol
   +----------+		+----------+	   |   	     |
   | coms     |		|          |	   +---------+
   +----------+		+----------+	   | udata   |-----> another canonical
					   +---------+		symbol



   It is very simple to make all the symbol pointers point to the same
   definition - just run down the chain and make the asymbols pointers
   within the canonical table point to the asymbol attacthed to the
   definition of the symbol.

*/

#include "bfd.h"
#include "sysdep.h"

#include "ld.h"
#include "ldsym.h"
#include "ldmisc.h"
#include "ldlang.h"
/* IMPORT */
extern int symbol_truncate;
extern bfd *output_bfd;
extern strip_symbols_type strip_symbols;
extern discard_locals_type discard_locals;
/* Head and tail of global symbol table chronological list */

ldsym_type *symbol_head = (ldsym_type *) NULL;
ldsym_type **symbol_tail_ptr = &symbol_head;
CONST char *keepsyms_file;
int kept_syms;

extern ld_config_type config;

struct obstack global_sym_obstack;
#define obstack_chunk_alloc ldmalloc
#define obstack_chunk_free  free

/*
  incremented for each symbol in the ldsym_type table
  no matter what flavour it is
*/
unsigned int global_symbol_count;

/* IMPORTS */

/* LOCALS */
#define	TABSIZE	1009
static ldsym_type *global_symbol_hash_table[TABSIZE];

/* Compute the hash code for symbol name KEY.  */
static
#ifdef __GNUC__
  __inline
#endif

int
hash_string (key)
     CONST char *key;
{
  register CONST char *cp;
  register int k;
  register int l = 0;
  cp = key;
  k = 0;
  while (*cp && l < symbol_truncate)
    {
      k = (((k << 1) + (k >> 14)) ^ (*cp++)) & 0x3fff;
      l++;
    }
  return k;
}

static
#ifdef __GNUC__
  __inline
#endif
  ldsym_type *
search (key, hashval)
     CONST char *key;
     int hashval;
{
  ldsym_type *bp;
  for (bp = global_symbol_hash_table[hashval]; bp; bp = bp->link)
    if (!strncmp (key, bp->name, symbol_truncate))
      {
	if (bp->flags & SYM_INDIRECT)
	  {
	    /* Use the symbol we're aliased to instead */
	    return (ldsym_type *) (bp->sdefs_chain);
	  }
	return bp;
      }
  return 0;
}


/* Get the symbol table entry for the global symbol named KEY.
   Create one if there is none.  */
ldsym_type *
ldsym_get (key)
     CONST char *key;
{
  register int hashval;
  register ldsym_type *bp;

  /* Determine the proper bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */
  bp = search (key, hashval);
  if (bp)
    {
      return bp;
    }

  /* Nothing was found; create a new symbol table entry.  */

  bp = (ldsym_type *) obstack_alloc (&global_sym_obstack, (bfd_size_type) (sizeof (ldsym_type)));
  bp->srefs_chain = (asymbol **) NULL;
  bp->sdefs_chain = (asymbol **) NULL;
  bp->scoms_chain = (asymbol **) NULL;
  bp->name = obstack_copy (&global_sym_obstack, key, strlen (key) + 1);
  bp->flags = 0;
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
     CONST char *key;
{
  register int hashval;
  /* Determine which bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */
  return search (key, hashval);
}

static asymbol **
process_keepsyms (table, size)
     asymbol **table;
     int size;
{
  struct obstack obstack;
  char *start_of_obstack;
  FILE *ks_file = 0;
  asymbol **out = table;
  asymbol **end = table + size;
  asymbol **sym;

  if (!keepsyms_file || size == 0)
    return end;
  obstack_init (&obstack);
  obstack_alloc (&obstack, 1);
  obstack_finish (&obstack);
  start_of_obstack = obstack_alloc (&obstack, 1);
  ks_file = fopen (keepsyms_file, "r");
  if (!ks_file)
    {
      info_msg ("%X%P: cannot open keep-symbols file `%s'\n", keepsyms_file);
      goto egress;
    }
  errno = 0;

#define KEEP(S) \
  do { asymbol **p = (S), *tmp = *out; *out = *p; *p = tmp; out++; } while (0)

  while (!feof (ks_file) && !ferror (ks_file))
    {
      int c;
      char *ptr;
      int found = 0;

      obstack_free (&obstack, start_of_obstack);
      do
	{
	  c = getc (ks_file);
	  if (c == '\n')
	    c = 0;
	  obstack_1grow (&obstack, c);
	}
      while (c > 0);
      if (c == EOF)
	{
	  if (!feof (ks_file))
	    /* error occurred */
	    {
	      info_msg ("%X%P: error reading keep-symbols file `%s': %E\n",
		    keepsyms_file);
	      out = end;
	      goto egress;
	    }
	  if (obstack_next_free (&obstack) != obstack_base (&obstack) + 1)
	    /* eof in middle of symbol */
	    {
	      info_msg ("%X%P: eof reached mid-line while reading keep-symbols file `%s'\n",
		    keepsyms_file);
	      out = end;
	      goto egress;
	    }
	  /* All okay -- no incomplete lines, EOF reached.  */
	  break;
	}
      ptr = obstack_next_free (&obstack) - 2;
      /* discard trailing trash */
      while (*ptr == ' '
	     || *ptr == '\t')
	*ptr-- = 0;
      ptr = obstack_base (&obstack);
      for (sym = out; sym < end; sym++)
	if (!strncmp ((*sym)->name, ptr, symbol_truncate))
	  {
	    KEEP (sym);
	    found = 1;
	  }
      if (!found)
	info_msg ("%P: symbol `%s' (requested to be kept) not found\n", ptr);
    }
  /* It'd be slightly faster to move this pass above the previous one,
     but that'd mean any symbols preserved in this pass would generate
     warnings if they were also listed in the keepsyms file.  */
  for (sym = out; sym < end; sym++)
    {
      asymbol *s = *sym;
      if (s->section == &bfd_und_section
	  || bfd_is_com_section (s->section)
	  || s->flags & BSF_KEEP_G)
	KEEP (sym);
    }
egress:
  obstack_free (&obstack, start_of_obstack);
  if (ks_file)
    fclose (ks_file);
  return out;
}

static void
list_file_locals (entry)
     lang_input_statement_type *entry;
{
  asymbol **q;
  fprintf (config.map_file, "\nLocal symbols of ");
  minfo ("%I", entry);
  fprintf (config.map_file, ":\n\n");
  if (entry->asymbols)
    {
      for (q = entry->asymbols; *q; q++)
	{
	  asymbol *p = *q;
	  /* If this is a definition,
	   update it if necessary by this file's start address.  */
	  if (p->flags & BSF_LOCAL)
	    info_msg ("  %V %s\n", p->value, p->name);
	}
    }
}


static void
print_file_stuff (f)
     lang_input_statement_type * f;
{
  fprintf (config.map_file, "  %s\n", f->filename);
  if (f->just_syms_flag)
    {
      fprintf (config.map_file, " symbols only\n");
    }
  else
    {
      asection *s;
      if (true)
	{
	  for (s = f->the_bfd->sections;
	       s != (asection *) NULL;
	       s = s->next)
	    {
	      print_address (s->output_offset);
	      if (s->reloc_done)
		{
		  fprintf (config.map_file, " %08x 2**%2ud %s\n",
			   (unsigned) bfd_get_section_size_after_reloc (s),
			   s->alignment_power, s->name);
		}

	      else
		{
		  fprintf (config.map_file, " %08x 2**%2ud %s\n",
			   (unsigned) bfd_get_section_size_before_reloc (s),
			   s->alignment_power, s->name);
		}
	    }
	}
      else
	{
	  for (s = f->the_bfd->sections;
	       s != (asection *) NULL;
	       s = s->next)
	    {
	      fprintf (config.map_file, "%s ", s->name);
	      print_address (s->output_offset);
	      fprintf (config.map_file, "(%x)", (unsigned) bfd_get_section_size_after_reloc (s));
	    }
	  fprintf (config.map_file, "hex \n");
	}
    }
  fprintf (config.map_file, "\n");
}

void
ldsym_print_symbol_table ()
{
  fprintf (config.map_file, "**FILES**\n\n");

  lang_for_each_file (print_file_stuff);

  fprintf (config.map_file, "**GLOBAL SYMBOLS**\n\n");
  fprintf (config.map_file, "offset    section    offset   symbol\n");
  {
    register ldsym_type *sp;

    for (sp = symbol_head; sp; sp = sp->next)
      {
	if (sp->flags & SYM_INDIRECT)
	  {
	    fprintf (config.map_file, "indirect %s to %s\n",
		     sp->name, (((ldsym_type *) (sp->sdefs_chain))->name));
	  }
	else
	  {
	    if (sp->sdefs_chain)
	      {
		asymbol *defsym = *(sp->sdefs_chain);
		asection *defsec = bfd_get_section (defsym);
		print_address (defsym->value);
		if (defsec)
		  {
		    fprintf (config.map_file, "  %-10s",
			     bfd_section_name (output_bfd,
					       defsec));
		    print_space ();
		    print_address (defsym->value + defsec->vma);

		  }
		else
		  {
		    fprintf (config.map_file, "         .......");
		  }

	      }


	    if (sp->scoms_chain)
	      {
		fprintf (config.map_file, "common               ");
		print_address ((*(sp->scoms_chain))->value);
		fprintf (config.map_file, " %s ", sp->name);
	      }
	    else if (sp->sdefs_chain)
	      {
		fprintf (config.map_file, " %s ", sp->name);
	      }
	    else
	      {
		fprintf (config.map_file, "undefined                     ");
		fprintf (config.map_file, "%s ", sp->name);

	      }
	  }
	print_nl ();

      }
  }
}

extern lang_output_section_statement_type *create_object_symbols;
extern char lprefix;
static asymbol **
write_file_locals (output_buffer)
     asymbol **output_buffer;
{
  LANG_FOR_EACH_INPUT_STATEMENT (entry)
  {
    /* Run trough the symbols and work out what to do with them */
    unsigned int i;

    /* Add one for the filename symbol if needed */
    if (create_object_symbols
	!= (lang_output_section_statement_type *) NULL)
      {
	asection *s;
	for (s = entry->the_bfd->sections;
	     s != (asection *) NULL;
	     s = s->next)
	  {
	    if (s->output_section == create_object_symbols->bfd_section)
	      {
		/* Add symbol to this section */
		asymbol *newsym =
		(asymbol *) bfd_make_empty_symbol (entry->the_bfd);
		newsym->name = entry->local_sym_name;
		/* The symbol belongs to the output file's text section */

		/* The value is the start of this section in the output file*/
		newsym->value = 0;
		/* FIXME: Usurping BSF_KEEP_G flag, since it's defined as
		   "used by the linker" and I can't find any other code that
		   uses it.  Should be a cleaner way of doing this (like an
		   "application flags" field in the symbol structure?).  */
		newsym->flags = BSF_LOCAL | BSF_KEEP_G | BSF_FILE;
		newsym->section = s;
		*output_buffer++ = newsym;
		break;
	      }
	  }
      }
    for (i = 0; i < entry->symbol_count; i++)
      {
	asymbol *p = entry->asymbols[i];
	/* FIXME, temporary hack, since not all of ld knows about the new abs section convention */

	if (p->section == 0)
	  p->section = &bfd_abs_section;
	if ((p->flags & BSF_GLOBAL)
	    || (p->flags & BSF_WEAK))
	  {
	    /* If this symbol is marked as occurring now, rather than
	       at the end, output it now.  This is used for COFF C_EXT
	       FCN symbols.  FIXME: There must be a better way.  */
	    if (bfd_asymbol_bfd (p) == entry->the_bfd
		&& (p->flags & BSF_NOT_AT_END))
	      {
		*(output_buffer++) = p;
		p->flags |= BSF_KEEP;
	      }
	  }
	else
	  {
	    if (p->section == &bfd_ind_section)
	      {
		/* Dont think about indirect symbols */
	      }
	    else if (p->flags & BSF_DEBUGGING)
	      {
		/* Only keep the debugger symbols if no stripping required */
		if (strip_symbols == STRIP_NONE)
		  {
		    *output_buffer++ = p;
		  }
	      }
	    else if (p->section == &bfd_und_section
		     || bfd_is_com_section (p->section))
	      {
		/* These must be global.  */
	      }
	    else if (p->flags & BSF_LOCAL)
	      {
		if (discard_locals == DISCARD_ALL)
		  {
		  }
		else if (discard_locals == DISCARD_L &&
			 (p->name[0] == lprefix))
		  {
		  }
		else if (p->flags == BSF_WARNING)
		  {
		  }
		else
		  {
		    *output_buffer++ = p;
		  }
	      }
	    else if (p->flags & BSF_CONSTRUCTOR)
	      {
		if (strip_symbols != STRIP_ALL)
		  {
		    *output_buffer++ = p;
		  }
	      }
	    else
	      {
		FAIL ();
	      }
	  }
      }


  }
  return output_buffer;
}


static asymbol **
write_file_globals (symbol_table)
     asymbol **symbol_table;
{
  FOR_EACH_LDSYM (sp)
  {
    if (sp->flags & SYM_INDIRECT)
      {
	asymbol *bufp = (*(sp->srefs_chain));
	ldsym_type *aliased_to = (ldsym_type *) (sp->sdefs_chain);
	if (aliased_to->sdefs_chain)
	  {
	    asymbol *p = aliased_to->sdefs_chain[0];
	    bufp->value = p->value;
	    bufp->section = p->section;
	    bufp->flags = p->flags;
	  }
	else
	  {
	    bufp->value = 0;
	    bufp->flags = 0;
	    bufp->section = &bfd_und_section;
	  }
	*symbol_table++ = bufp;
      }
    else if ((sp->flags & SYM_INDIRECT) == 0 && sp->sdefs_chain != (asymbol **) NULL)
      {
	asymbol *bufp = (*(sp->sdefs_chain));

	if ((bufp->flags & BSF_KEEP) == 0)
	  {
	    ASSERT (bufp != (asymbol *) NULL);

	    bufp->name = sp->name;

	    if (sp->scoms_chain != (asymbol **) NULL)

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
    else if (sp->scoms_chain != (asymbol **) NULL)
      {
	/* This symbol is a common - just output */
	asymbol *bufp = (*(sp->scoms_chain));
	*symbol_table++ = bufp;
      }
    else if (sp->srefs_chain != (asymbol **) NULL)
      {
	/* This symbol is undefined but has a reference */
	asymbol *bufp = (*(sp->srefs_chain));
	*symbol_table++ = bufp;
      }
    else
      {
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
ldsym_write ()
{
  if (keepsyms_file != 0
      && strip_symbols != STRIP_SOME)
    {
      info_msg ("%P: `-retain-symbols-file' overrides `-s' and `-S'\n");
      strip_symbols = STRIP_SOME;
    }
  if (strip_symbols != STRIP_ALL)
    {
      /* We know the maximum size of the symbol table -
       it's the size of all the global symbols ever seen +
       the size of all the symbols from all the files +
       the number of files (for the per file symbols)
       +1 (for the null at the end)
       */
      extern unsigned int total_files_seen;
      extern unsigned int total_symbols_seen;

      asymbol **symbol_table = (asymbol **)
      ldmalloc ((bfd_size_type) (global_symbol_count +
				 total_files_seen +
			      total_symbols_seen + 1) * sizeof (asymbol *));
      asymbol **tablep = write_file_locals (symbol_table);

      tablep = write_file_globals (tablep);
      tablep = process_keepsyms (symbol_table, tablep - symbol_table);

      *tablep = (asymbol *) NULL;
      bfd_set_symtab (output_bfd, symbol_table, (unsigned) (tablep - symbol_table));
    }
}

/*
return true if the supplied symbol name is not in the
linker symbol table
*/
boolean
ldsym_undefined (sym)
     CONST char *sym;
{
  ldsym_type *from_table = ldsym_get_soft (sym);
  if (from_table != (ldsym_type *) NULL)
    {
      if (from_table->sdefs_chain != (asymbol **) NULL)
	return false;
    }
  return true;
}

void
ldsym_init ()
{
  obstack_begin (&global_sym_obstack, 20000);
}
