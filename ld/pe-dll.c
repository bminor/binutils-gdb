/* Routines to help build PEI-format DLLs (Win32 etc)
   Copyright (C) 1998 Free Software Foundation, Inc.
   Written by DJ Delorie <dj@cygnus.com>

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
   along with GLD; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libiberty.h"

#include "ld.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldwrite.h"
#include "ldmisc.h"
#include "ldgram.h"
#include "ldmain.h"
#include "coff/internal.h"
#include "../bfd/libcoff.h"
#include "deffile.h"

/************************************************************************

 This file turns a regular Windows PE image into a DLL.  Because of
 the complexity of this operation, it has been broken down into a
 number of separate modules which are all called by the main function
 at the end of this file.  This function is not re-entrant and is
 normally only called once, so static variables are used to reduce
 the number of parameters and return values required.

 See also: ld/emultempl/pe.em

 ************************************************************************/

/* from emultempl/pe.em */

extern def_file *pe_def_file;
extern int pe_dll_export_everything;
extern int pe_dll_do_default_excludes;
extern int pe_dll_kill_ats;
extern int pe_dll_stdcall_aliases;

/************************************************************************

 static variables and types

 ************************************************************************/

static bfd_vma image_base;

static bfd *filler_bfd;
static struct sec *edata_s, *reloc_s;
static unsigned char *edata_d, *reloc_d;
static int edata_sz, reloc_sz;

/************************************************************************

 Helper functions for qsort.  Relocs must be sorted so that we can write
 them out by pages.

 ************************************************************************/

static int
reloc_sort (va, vb)
     const void *va, *vb;
{
  unsigned long a = *(unsigned long *) va;
  unsigned long b = *(unsigned long *) vb;
  return a - b;
}

static int
pe_export_sort (va, vb)
     const void *va, *vb;
{
  def_file_export *a = (def_file_export *) va;
  def_file_export *b = (def_file_export *) vb;
  return strcmp (a->name, b->name);
}

/************************************************************************

 Read and process the .DEF file

 ************************************************************************/

/* These correspond to the entries in pe_def_file->exports[].  I use
   exported_symbol_sections[i] to tag whether or not the symbol was
   defined, since we can't export symbols we don't have. */

static bfd_vma *exported_symbol_offsets;
static struct sec **exported_symbol_sections;

static int export_table_size;
static int count_exported;
static int count_exported_byname;
static int count_with_ordinals;
static const char *dll_name;
static int min_ordinal, max_ordinal;
static int *exported_symbols;

typedef struct exclude_list_struct
  {
    char *string;
    struct exclude_list_struct *next;
  }
exclude_list_struct;
static struct exclude_list_struct *excludes = 0;

void
pe_dll_add_excludes (new_excludes)
     const char *new_excludes;
{
  char *local_copy;
  char *exclude_string;

  local_copy = xstrdup (new_excludes);

  exclude_string = strtok (local_copy, ",:");
  for (; exclude_string; exclude_string = strtok (NULL, ",:"))
    {
      struct exclude_list_struct *new_exclude;

      new_exclude = ((struct exclude_list_struct *)
		     xmalloc (sizeof (struct exclude_list_struct)));
      new_exclude->string = (char *) xmalloc (strlen (exclude_string) + 1);
      strcpy (new_exclude->string, exclude_string);
      new_exclude->next = excludes;
      excludes = new_exclude;
    }

  free (local_copy);
}

static int
auto_export (d, n)
     def_file *d;
     const char *n;
{
  int i;
  struct exclude_list_struct *ex;
  for (i = 0; i < d->num_exports; i++)
    if (strcmp (d->exports[i].name, n) == 0)
      return 0;
  if (pe_dll_do_default_excludes)
    {
      if (strcmp (n, "DllMain@12") == 0)
	return 0;
      if (strcmp (n, "DllEntryPoint@0") == 0)
	return 0;
      if (strcmp (n, "impure_ptr") == 0)
	return 0;
    }
  for (ex = excludes; ex; ex = ex->next)
    if (strcmp (n, ex->string) == 0)
      return 0;
  return 1;
}

static void
process_def_file (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  int i, j;
  struct bfd_link_hash_entry *blhe;
  bfd *b;
  struct sec *s;
  def_file_export *e=0;

  if (!pe_def_file)
    pe_def_file = def_file_empty ();

  /* First, run around to all the objects looking for the .drectve
     sections, and push those into the def file too */

  for (b = info->input_bfds; b; b = b->link_next)
    {
      s = bfd_get_section_by_name (b, ".drectve");
      if (s)
	{
	  int size = bfd_get_section_size_before_reloc (s);
	  char *buf = xmalloc (size);
	  bfd_get_section_contents (b, s, buf, 0, size);
	  def_file_add_directive (pe_def_file, buf, size);
	  free (buf);
	}
    }

  /* Now, maybe export everything else the default way */

  if (pe_dll_export_everything || pe_def_file->num_exports == 0)
    {
      for (b = info->input_bfds; b; b = b->link_next)
	{
	  asymbol **symbols;
	  int nsyms, symsize;

	  symsize = bfd_get_symtab_upper_bound (b);
	  symbols = (asymbol **) xmalloc (symsize);
	  nsyms = bfd_canonicalize_symtab (b, symbols);

	  for (j = 0; j < nsyms; j++)
	    {
	      if ((symbols[j]->flags & (BSF_FUNCTION | BSF_GLOBAL))
		  == (BSF_FUNCTION | BSF_GLOBAL))
		{
		  const char *sn = symbols[j]->name;
		  if (*sn == '_')
		    sn++;
		  if (auto_export (pe_def_file, sn))
		    def_file_add_export (pe_def_file, sn, 0, -1);
		}
	    }
	}
    }

#undef NE
#define NE pe_def_file->num_exports

  /* Canonicalize the export list */

  if (pe_dll_kill_ats)
    {
      for (i = 0; i < NE; i++)
	{
	  if (strchr (pe_def_file->exports[i].name, '@'))
	    {
	      /* This will preserve internal_name, which may have been pointing
	         to the same memory as name, or might not have */
	      char *tmp = xstrdup (pe_def_file->exports[i].name);
	      *(strchr (tmp, '@')) = 0;
	      pe_def_file->exports[i].name = tmp;
	    }
	}
    }

  if (pe_dll_stdcall_aliases)
    {
      for (i = 0; i < NE; i++)
	{
	  if (strchr (pe_def_file->exports[i].name, '@'))
	    {
	      char *tmp = xstrdup (pe_def_file->exports[i].name);
	      *(strchr (tmp, '@')) = 0;
	      if (auto_export (pe_def_file, tmp))
		def_file_add_export (pe_def_file, tmp,
				     pe_def_file->exports[i].internal_name, -1);
	      else
		free (tmp);
	    }
	}
    }

  e = pe_def_file->exports; /* convenience, but watch out for it changing */

  exported_symbol_offsets = (bfd_vma *) xmalloc (NE * sizeof (bfd_vma));
  exported_symbol_sections = (struct sec **) xmalloc (NE * sizeof (struct sec *));

  memset (exported_symbol_sections, 0, NE * sizeof (struct sec *));
  max_ordinal = 0;
  min_ordinal = 65536;
  count_exported = 0;
  count_exported_byname = 0;
  count_with_ordinals = 0;

  qsort (pe_def_file->exports, NE, sizeof (pe_def_file->exports[0]), pe_export_sort);
  for (i = 0, j = 0; i < NE; i++)
    {
      if (i > 0 && strcmp (e[i].name, e[i - 1].name) == 0)
	{
	  /* This is a duplicate */
	  if (e[j - 1].ordinal != -1
	      && e[i].ordinal != -1
	      && e[j - 1].ordinal != e[i].ordinal)
	    {
	      /* xgettext:c-format */
	      einfo (_("%XError, duplicate EXPORT with oridinals: %s (%d vs %d)\n"),
		     e[j - 1].name, e[j - 1].ordinal, e[i].ordinal);
	    }
	  else
	    {
	      /* xgettext:c-format */
	      einfo (_("Warning, duplicate EXPORT: %s\n"),
		     e[j - 1].name);
	    }
	  if (e[i].ordinal)
	    e[j - 1].ordinal = e[i].ordinal;
	  e[j - 1].flag_private |= e[i].flag_private;
	  e[j - 1].flag_constant |= e[i].flag_constant;
	  e[j - 1].flag_noname |= e[i].flag_noname;
	  e[j - 1].flag_data |= e[i].flag_data;
	}
      else
	{
	  if (i != j)
	    e[j] = e[i];
	  j++;
	}
    }
  pe_def_file->num_exports = j;	/* == NE */

  for (i = 0; i < NE; i++)
    {
      char *name = (char *) xmalloc (strlen (pe_def_file->exports[i].internal_name) + 2);
      *name = '_';
      strcpy (name + 1, pe_def_file->exports[i].internal_name);

      blhe = bfd_link_hash_lookup (info->hash,
				   name,
				   false, false, true);

      if (blhe && (blhe->type == bfd_link_hash_defined))
	{
	  count_exported++;
	  if (!pe_def_file->exports[i].flag_noname)
	    count_exported_byname++;
	  exported_symbol_offsets[i] = blhe->u.def.value;
	  exported_symbol_sections[i] = blhe->u.def.section;
	  if (pe_def_file->exports[i].ordinal != -1)
	    {
	      if (max_ordinal < pe_def_file->exports[i].ordinal)
		max_ordinal = pe_def_file->exports[i].ordinal;
	      if (min_ordinal > pe_def_file->exports[i].ordinal)
		min_ordinal = pe_def_file->exports[i].ordinal;
	      count_with_ordinals++;
	    }
	}
      else if (blhe && blhe->type == bfd_link_hash_undefined)
	{
	  /* xgettext:c-format */
	  einfo (_("%XCannot export %s: symbol not defined\n"),
		 pe_def_file->exports[i].internal_name);
	}
      else if (blhe)
	{
	  /* xgettext:c-format */
	  einfo (_("%XCannot export %s: symbol wrong type (%d vs %d)\n"),
		 pe_def_file->exports[i].internal_name,
		 blhe->type, bfd_link_hash_defined);
	}
      else
	{
	  /* xgettext:c-format */
	  einfo (_("%XCannot export %s: symbol not found\n"),
		 pe_def_file->exports[i].internal_name);
	}
      free(name);
    }
}

/************************************************************************

 Build the bfd that will contain .edata and .reloc sections

 ************************************************************************/

static void
build_filler_bfd ()
{
  static lang_input_statement_type *filler_file;
  filler_file = lang_add_input_file ("dll stuff",
				     lang_input_file_is_fake_enum,
				     NULL);
  filler_file->the_bfd = filler_bfd = bfd_create ("dll stuff", output_bfd);
  if (filler_bfd == NULL
      || !bfd_set_arch_mach (filler_bfd,
			     bfd_get_arch (output_bfd),
			     bfd_get_mach (output_bfd)))
    {
      einfo ("%X%P: can not create BFD %E\n");
      return;
    }

  edata_s = bfd_make_section_old_way (filler_bfd, ".edata");
  if (edata_s == NULL
      || !bfd_set_section_flags (filler_bfd, edata_s,
				 (SEC_HAS_CONTENTS
				  | SEC_ALLOC
				  | SEC_LOAD
				  | SEC_KEEP
				  | SEC_IN_MEMORY)))
    {
      einfo ("%X%P: can not create .edata section: %E\n");
      return;
    }
  bfd_set_section_size (filler_bfd, edata_s, edata_sz);

  reloc_s = bfd_make_section_old_way (filler_bfd, ".reloc");
  if (reloc_s == NULL
      || !bfd_set_section_flags (filler_bfd, reloc_s,
				 (SEC_HAS_CONTENTS
				  | SEC_ALLOC
				  | SEC_LOAD
				  | SEC_KEEP
				  | SEC_IN_MEMORY)))
    {
      einfo ("%X%P: can not create .reloc section: %E\n");
      return;
    }
  bfd_set_section_size (filler_bfd, reloc_s, 0);

  ldlang_add_file (filler_file);
}

/************************************************************************

 Gather all the exported symbols and build the .edata section

 ************************************************************************/

static void
generate_edata (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  int i, j, next_ordinal;
  int name_table_size = 0;
  const char *dlnp;

  /* First, we need to know how many exported symbols there are,
     and what the range of ordinals is. */

  if (pe_def_file->name)
    {
      dll_name = pe_def_file->name;
    }
  else
    {
      dll_name = abfd->filename;
      for (dlnp = dll_name; *dlnp; dlnp++)
	{
	  if (*dlnp == '\\' || *dlnp == '/' || *dlnp == ':')
	    dll_name = dlnp + 1;
	}
    }

  if (count_with_ordinals && max_ordinal > count_exported)
    {
      if (min_ordinal > max_ordinal - count_exported + 1)
	min_ordinal = max_ordinal - count_exported + 1;
    }
  else
    {
      min_ordinal = 1;
      max_ordinal = count_exported;
    }
  export_table_size = max_ordinal - min_ordinal + 1;

  exported_symbols = (int *) xmalloc (export_table_size * sizeof (int));
  for (i = 0; i < export_table_size; i++)
    exported_symbols[i] = -1;

  /* Now we need to assign ordinals to those that don't have them */
  for (i = 0; i < NE; i++)
    {
      if (exported_symbol_sections[i])
	{
	  if (pe_def_file->exports[i].ordinal != -1)
	    {
	      int ei = pe_def_file->exports[i].ordinal - min_ordinal;
	      int pi = exported_symbols[ei];
	      if (pi != -1)
		{
		  /* xgettext:c-format */
		  einfo (_("%XError, oridinal used twice: %d (%s vs %s)\n"),
			 pe_def_file->exports[i].ordinal,
			 pe_def_file->exports[i].name,
			 pe_def_file->exports[pi].name);
		}
	      exported_symbols[ei] = i;
	    }
	  name_table_size += strlen (pe_def_file->exports[i].name) + 1;
	}
    }

  next_ordinal = min_ordinal;
  for (i = 0; i < NE; i++)
    if (exported_symbol_sections[i])
      if (pe_def_file->exports[i].ordinal == -1)
	{
	  while (exported_symbols[next_ordinal - min_ordinal] != -1)
	    next_ordinal++;
	  exported_symbols[next_ordinal - min_ordinal] = i;
	  pe_def_file->exports[i].ordinal = next_ordinal;
	}

  /* OK, now we can allocate some memory */

  edata_sz = (40		/* directory */
	      + 4 * export_table_size	/* addresses */
	      + 4 * count_exported_byname	/* name ptrs */
	      + 2 * count_exported_byname	/* ordinals */
	      + name_table_size + strlen (dll_name) + 1);
}

static void
fill_edata (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  int i;
  unsigned char *edirectory;
  unsigned long *eaddresses;
  unsigned long *enameptrs;
  unsigned short *eordinals;
  unsigned char *enamestr;

  edata_d = (unsigned char *) xmalloc (edata_sz);

  /* Note use of array pointer math here */
  edirectory = edata_d;
  eaddresses = (unsigned long *) (edata_d + 40);
  enameptrs = eaddresses + export_table_size;
  eordinals = (unsigned short *) (enameptrs + count_exported_byname);
  enamestr = (char *) (eordinals + count_exported_byname);

#define ERVA(ptr) (((unsigned char *)(ptr) - edata_d) + edata_s->output_section->vma - image_base)

  memset (edata_d, 0, 40);
  if (pe_def_file->version_major != -1)
    {
      bfd_put_16 (abfd, pe_def_file->version_major, edata_d + 8);
      bfd_put_16 (abfd, pe_def_file->version_minor, edata_d + 10);
    }
  bfd_put_32 (abfd, ERVA (enamestr), edata_d + 12);
  strcpy (enamestr, dll_name);
  enamestr += strlen (enamestr) + 1;
  bfd_put_32 (abfd, min_ordinal, edata_d + 16);
  bfd_put_32 (abfd, export_table_size, edata_d + 20);
  bfd_put_32 (abfd, count_exported_byname, edata_d + 24);
  bfd_put_32 (abfd, ERVA (eaddresses), edata_d + 28);
  bfd_put_32 (abfd, ERVA (enameptrs), edata_d + 32);
  bfd_put_32 (abfd, ERVA (eordinals), edata_d + 36);

  /* Ok, now for the filling in part */
  for (i = 0; i < export_table_size; i++)
    {
      int s = exported_symbols[i];
      if (s != -1)
	{
	  struct sec *ssec = exported_symbol_sections[s];
	  unsigned long srva = (exported_symbol_offsets[s]
				+ ssec->output_section->vma
				+ ssec->output_offset);

	  bfd_put_32 (abfd, srva - image_base, (void *) (eaddresses + i));
	  if (!pe_def_file->exports[s].flag_noname)
	    {
	      char *ename = pe_def_file->exports[s].name;
	      bfd_put_32 (abfd, ERVA (enamestr), (void *) enameptrs);
	      strcpy (enamestr, ename);
	      enamestr += strlen (enamestr) + 1;
	      bfd_put_16 (abfd, i, (void *) eordinals);
	      enameptrs++;
	    }
	  eordinals++;
	}
    }
}

/************************************************************************

 Gather all the relocations and build the .reloc section

 ************************************************************************/

static void
generate_reloc (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{

  /* for .reloc stuff */
  bfd_vma *reloc_addresses;
  int total_relocs = 0;
  int i, j;
  unsigned long sec_page = (unsigned long) (-1);
  unsigned long page_ptr, page_count;
  int bi;
  bfd *b;
  struct sec *s;

  total_relocs = 0;
  for (b = info->input_bfds; b; b = b->link_next)
    for (s = b->sections; s; s = s->next)
      total_relocs += s->reloc_count;

  reloc_addresses = (unsigned long *) xmalloc (total_relocs * sizeof (unsigned long));

  total_relocs = 0;
  bi = 0;
  for (bi = 0, b = info->input_bfds; b; bi++, b = b->link_next)
    {
      arelent **relocs;
      int relsize, nrelocs, i;

      for (s = b->sections; s; s = s->next)
	{
	  unsigned long sec_vma = s->output_section->vma + s->output_offset;
	  asymbol **symbols;
	  int nsyms, symsize;

	  /* if it's not loaded, we don't need to relocate it this way */
	  if (!(s->output_section->flags & SEC_LOAD))
	    continue;

	  /* I don't know why there would be a reloc for these, but I've
	     seen it happen - DJ */
	  if (s->output_section == &bfd_abs_section)
	    continue;

	  if (s->output_section->vma == 0)
	    {
	      /* Huh?  Shouldn't happen, but punt if it does */
	      einfo ("DJ: zero vma section reloc detected: `%s' #%d f=%d\n",
		     s->output_section->name, s->output_section->index,
		     s->output_section->flags);
	      continue;
	    }

	  symsize = bfd_get_symtab_upper_bound (b);
	  symbols = (asymbol **) xmalloc (symsize);
	  nsyms = bfd_canonicalize_symtab (b, symbols);

	  relsize = bfd_get_reloc_upper_bound (b, s);
	  relocs = (arelent **) xmalloc ((size_t) relsize);
	  nrelocs = bfd_canonicalize_reloc (b, s, relocs, symbols);

	  for (i = 0; i < nrelocs; i++)
	    {
	      if (!relocs[i]->howto->pc_relative
		  && relocs[i]->howto->type != R_IMAGEBASE)
		{
		  switch (relocs[i]->howto->bitsize)
		    {
		    case 32:
		      reloc_addresses[total_relocs++] = sec_vma + relocs[i]->address;
		      break;
		    default:
		      /* xgettext:c-format */
		      einfo (_("%XError: %d-bit reloc in dll\n"),
			     relocs[i]->howto->bitsize);
		      break;
		    }
		}
	    }
	  free (relocs);
	  /* Warning: the allocated symbols are remembered in BFD and reused
	     later, so don't free them! */
	  /* free(symbols); */
	}
    }

  /* At this point, we have total_relocs relocation addresses in
     reloc_addresses, which are all suitable for the .reloc section.
     We must now create the new sections. */

  qsort (reloc_addresses, total_relocs, sizeof (unsigned long), reloc_sort);

  for (i = 0; i < total_relocs; i++)
    {
      unsigned long this_page = (reloc_addresses[i] >> 12);
      if (this_page != sec_page)
	{
	  reloc_sz = (reloc_sz + 3) & ~3;	/* 4-byte align */
	  reloc_sz += 8;
	  sec_page = this_page;
	}
      reloc_sz += 2;
    }
  reloc_sz = (reloc_sz + 3) & ~3;	/* 4-byte align */

  reloc_d = (unsigned char *) xmalloc (reloc_sz);

  sec_page = (unsigned long) (-1);
  reloc_sz = 0;
  page_ptr = (unsigned long) (-1);
  page_count = 0;
  for (i = 0; i < total_relocs; i++)
    {
      unsigned long rva = reloc_addresses[i] - image_base;
      unsigned long this_page = (rva & ~0xfff);
      if (this_page != sec_page)
	{
	  while (reloc_sz & 3)
	    reloc_d[reloc_sz++] = 0;
	  if (page_ptr != (unsigned long) (-1))
	    bfd_put_32 (abfd, reloc_sz - page_ptr, reloc_d + page_ptr + 4);
	  bfd_put_32 (abfd, this_page, reloc_d + reloc_sz);
	  page_ptr = reloc_sz;
	  reloc_sz += 8;
	  sec_page = this_page;
	  page_count = 0;
	}
      bfd_put_16 (abfd, (rva & 0xfff) + 0x3000, reloc_d + reloc_sz);
      reloc_sz += 2;
      page_count++;
    }
  while (reloc_sz & 3)
    reloc_d[reloc_sz++] = 0;
  if (page_ptr != (unsigned long) (-1))
    bfd_put_32 (abfd, reloc_sz - page_ptr, reloc_d + page_ptr + 4);
  while (reloc_sz < reloc_s->_raw_size)
    reloc_d[reloc_sz++] = 0;
}

/************************************************************************

 Given the exiting def_file structure, print out a .DEF file that
 corresponds to it.

 ************************************************************************/

static void
quoteput (s, f, needs_quotes)
     char *s;
     FILE * f;
     int needs_quotes;
{
  char *cp;
  for (cp = s; *cp; cp++)
    if (*cp == '\'' || *cp == '"' || *cp == '\\' || isspace (*cp) || *cp == ','
	|| *cp == ';')
      needs_quotes = 1;
  if (needs_quotes)
    {
      putc ('"', f);
      while (*s)
	{
	  if (*s == '"' || *s == '\\')
	    putc ('\\', f);
	  putc (*s, f);
	  s++;
	}
      putc ('"', f);
    }
  else
    fputs (s, f);
}

void
pe_dll_generate_def_file (pe_out_def_filename)
     char *pe_out_def_filename;
{
  int i;
  FILE *out = fopen (pe_out_def_filename, "w");
  if (out == NULL)
    {
      /* xgettext:c-format */
      einfo (_("%s: Can't open output def file %s\n"),
	     program_name, pe_out_def_filename);
    }

  if (pe_def_file)
    {
      if (pe_def_file->name)
	{
	  if (pe_def_file->is_dll)
	    fprintf (out, "LIBRARY ");
	  else
	    fprintf (out, "NAME ");
	  quoteput (pe_def_file->name, out, 1);
	  if (pe_def_file->base_address != (bfd_vma) (-1))
	    fprintf (out, " BASE=0x%x", pe_def_file->base_address);
	  fprintf (out, "\n");
	}

      if (pe_def_file->description)
	{
	  fprintf (out, "DESCRIPTION ");
	  quoteput (pe_def_file->description, out, 1);
	  fprintf (out, "\n");
	}

      if (pe_def_file->version_minor)
	fprintf (out, "VERSION %d.%d\n", pe_def_file->version_major,
		 pe_def_file->version_minor);
      else
	fprintf (out, "VERSION %d\n", pe_def_file->version_major);

      if (pe_def_file->stack_reserve != -1 || pe_def_file->heap_reserve != -1)
	fprintf (out, "\n");

      if (pe_def_file->stack_commit != -1)
	fprintf (out, "STACKSIZE 0x%x,0x%x\n",
		 pe_def_file->stack_reserve, pe_def_file->stack_commit);
      else if (pe_def_file->stack_reserve != -1)
	fprintf (out, "STACKSIZE 0x%x\n", pe_def_file->stack_reserve);
      if (pe_def_file->heap_commit != -1)
	fprintf (out, "HEAPSIZE 0x%x,0x%x\n",
		 pe_def_file->heap_reserve, pe_def_file->heap_commit);
      else if (pe_def_file->heap_reserve != -1)
	fprintf (out, "HEAPSIZE 0x%x\n", pe_def_file->heap_reserve);

      if (pe_def_file->num_section_defs > 0)
	{
	  fprintf (out, "\nSECTIONS\n\n");
	  for (i = 0; i < pe_def_file->num_section_defs; i++)
	    {
	      fprintf (out, "    ");
	      quoteput (pe_def_file->section_defs[i].name, out, 0);
	      if (pe_def_file->section_defs[i].class)
		{
		  fprintf (out, " CLASS ");
		  quoteput (pe_def_file->section_defs[i].class, out, 0);
		}
	      if (pe_def_file->section_defs[i].flag_read)
		fprintf (out, " READ");
	      if (pe_def_file->section_defs[i].flag_write)
		fprintf (out, " WRITE");
	      if (pe_def_file->section_defs[i].flag_execute)
		fprintf (out, " EXECUTE");
	      if (pe_def_file->section_defs[i].flag_shared)
		fprintf (out, " SHARED");
	      fprintf (out, "\n");
	    }
	}

      if (pe_def_file->num_exports > 0)
	{
	  fprintf (out, "\nEXPORTS\n\n");
	  for (i = 0; i < pe_def_file->num_exports; i++)
	    {
	      def_file_export *e = pe_def_file->exports + i;
	      fprintf (out, "    ");
	      quoteput (e->name, out, 0);
	      if (e->internal_name && strcmp (e->internal_name, e->name))
		{
		  fprintf (out, " = ");
		  quoteput (e->internal_name, out, 0);
		}
	      if (e->ordinal != -1)
		fprintf (out, " @%d", e->ordinal);
	      if (e->flag_private)
		fprintf (out, " PRIVATE");
	      if (e->flag_constant)
		fprintf (out, " CONSTANT");
	      if (e->flag_noname)
		fprintf (out, " NONAME");
	      if (e->flag_data)
		fprintf (out, " DATA");

	      fprintf (out, "\n");
	    }
	}

      if (pe_def_file->num_imports > 0)
	{
	  fprintf (out, "\nIMPORTS\n\n");
	  for (i = 0; i < pe_def_file->num_imports; i++)
	    {
	      def_file_import *im = pe_def_file->imports + i;
	      fprintf (out, "    ");
	      if (im->internal_name
		  && (!im->name || strcmp (im->internal_name, im->name)))
		{
		  quoteput (im->internal_name, out, 0);
		  fprintf (out, " = ");
		}
	      quoteput (im->module->name, out, 0);
	      fprintf (out, ".");
	      if (im->name)
		quoteput (im->name, out, 0);
	      else
		fprintf (out, "%d", im->ordinal);
	      fprintf (out, "\n");
	    }
	}
    }
  else
    fprintf (out, _("; no contents available\n"));

  if (fclose (out) == EOF)
    {
      /* xgettext:c-format */
      einfo (_("%P: Error closing file `%s'\n"), pe_out_def_filename);
    }
}

/************************************************************************

 These are the main functions, called from the emulation.  The first
 is called after the bfds are read, so we can guess at how much space
 we need.  The second is called after everything is placed, so we
 can put the right values in place.

 ************************************************************************/

void
pe_dll_build_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  process_def_file (abfd, info);

  generate_edata (abfd, info);
  build_filler_bfd ();
}

void
pe_dll_fill_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  image_base = pe_data (abfd)->pe_opthdr.ImageBase;

  generate_reloc (abfd, info);
  if (reloc_sz > 0)
    {
      bfd_set_section_size (filler_bfd, reloc_s, reloc_sz);

      /* Resize the sections.  */
      lang_size_sections (stat_ptr->head, abs_output_section,
			  &stat_ptr->head, 0, (bfd_vma) 0, false);

      /* Redo special stuff.  */
      ldemul_after_allocation ();

      /* Do the assignments again.  */
      lang_do_assignments (stat_ptr->head,
			   abs_output_section,
			   (fill_type) 0, (bfd_vma) 0);
    }

  fill_edata (abfd, info);

  pe_data (abfd)->dll = 1;

  edata_s->contents = edata_d;
  reloc_s->contents = reloc_d;
}
