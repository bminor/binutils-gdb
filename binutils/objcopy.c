/* objcopy.c -- copy object file from input to output, optionally massaging it.
   Copyright (C) 1991, 92, 93, 94 Free Software Foundation, Inc.

   This file is part of GNU Binutils.

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

#include "bfd.h"
#include "sysdep.h"
#include "bucomm.h"
#include <getopt.h>
#include "libiberty.h"

static bfd_vma parse_vma PARAMS ((const char *, const char *));
static void setup_section PARAMS ((bfd *, asection *, PTR));
static void copy_section PARAMS ((bfd *, asection *, PTR));
static void mark_symbols_used_in_relocations PARAMS ((bfd *, asection *, PTR));

#define nonfatal(s) {bfd_nonfatal(s); status = 1; return;}

static asymbol **isympp = NULL;	/* Input symbols */
static asymbol **osympp = NULL;	/* Output symbols that survive stripping */

/* If `copy_byte' >= 0, copy only that byte of every `interleave' bytes.  */
static int copy_byte = -1;
static int interleave = 4;

static boolean verbose;		/* Print file and target names. */
static int status = 0;		/* Exit status.  */

enum strip_action
  {
    strip_undef,
    strip_none,			/* don't strip */
    strip_debug,		/* strip all debugger symbols */
    strip_all			/* strip all symbols */
  };

/* Which symbols to remove. */
static enum strip_action strip_symbols;

enum locals_action
  {
    locals_undef,
    locals_start_L,		/* discard locals starting with L */
    locals_all			/* discard all locals */
  };

/* Which local symbols to remove.  Overrides strip_all.  */
static enum locals_action discard_locals;

/* Structure used to hold lists of sections and actions to take.  */

struct section_list
{
  /* Next section to adjust.  */
  struct section_list *next;
  /* Section name.  */
  const char *name;
  /* Whether this entry was used.  */
  boolean used;
  /* Remaining fields only used if not on remove_sections list.  */
  /* Whether to adjust or set VMA.  */
  boolean adjust;
  /* Amount to adjust by or set to.  */
  bfd_vma val;
};

/* List of sections to remove.  */

static struct section_list *remove_sections;

/* Adjustments to the start address.  */
static bfd_vma adjust_start = 0;
static boolean set_start_set = false;
static bfd_vma set_start;

/* Adjustments to section VMA's.  */
static bfd_vma adjust_section_vma = 0;
static struct section_list *adjust_sections;

/* Options to handle if running as "strip".  */

static struct option strip_options[] =
{
  {"discard-all", no_argument, 0, 'x'},
  {"discard-locals", no_argument, 0, 'X'},
  {"format", required_argument, 0, 'F'}, /* Obsolete */
  {"help", no_argument, 0, 'h'},
  {"input-format", required_argument, 0, 'I'}, /* Obsolete */
  {"input-target", required_argument, 0, 'I'},
  {"output-format", required_argument, 0, 'O'},	/* Obsolete */
  {"output-target", required_argument, 0, 'O'},
  {"remove-section", required_argument, 0, 'R'},
  {"strip-all", no_argument, 0, 's'},
  {"strip-debug", no_argument, 0, 'S'},
  {"target", required_argument, 0, 'F'},
  {"verbose", no_argument, 0, 'v'},
  {"version", no_argument, 0, 'V'},
  {0, no_argument, 0, 0}
};

/* Options to handle if running as "objcopy".  */

/* 150 isn't special; it's just an arbitrary non-ASCII char value.  */

#define OPTION_ADJUST_START 150
#define OPTION_ADJUST_VMA (OPTION_ADJUST_START + 1)
#define OPTION_ADJUST_SECTION_VMA (OPTION_ADJUST_VMA + 1)
#define OPTION_ADJUST_WARNINGS (OPTION_ADJUST_SECTION_VMA + 1)
#define OPTION_NO_ADJUST_WARNINGS (OPTION_ADJUST_WARNINGS + 1)
#define OPTION_SET_START (OPTION_NO_ADJUST_WARNINGS + 1)

static struct option copy_options[] =
{
  {"adjust-start", required_argument, 0, OPTION_ADJUST_START},
  {"adjust-vma", required_argument, 0, OPTION_ADJUST_VMA},
  {"adjust-section-vma", required_argument, 0, OPTION_ADJUST_SECTION_VMA},
  {"adjust-warnings", no_argument, 0, OPTION_ADJUST_WARNINGS},
  {"byte", required_argument, 0, 'b'},
  {"discard-all", no_argument, 0, 'x'},
  {"discard-locals", no_argument, 0, 'X'},
  {"format", required_argument, 0, 'F'}, /* Obsolete */
  {"help", no_argument, 0, 'h'},
  {"input-format", required_argument, 0, 'I'}, /* Obsolete */
  {"input-target", required_argument, 0, 'I'},
  {"interleave", required_argument, 0, 'i'},
  {"no-adjust-warnings", no_argument, 0, OPTION_NO_ADJUST_WARNINGS},
  {"output-format", required_argument, 0, 'O'},	/* Obsolete */
  {"output-target", required_argument, 0, 'O'},
  {"remove-section", required_argument, 0, 'R'},
  {"set-start", required_argument, 0, OPTION_SET_START},
  {"strip-all", no_argument, 0, 'S'},
  {"strip-debug", no_argument, 0, 'g'},
  {"target", required_argument, 0, 'F'},
  {"verbose", no_argument, 0, 'v'},
  {"version", no_argument, 0, 'V'},
  {0, no_argument, 0, 0}
};

/* IMPORTS */
extern char *program_name;
extern char *program_version;

/* This flag distinguishes between strip and objcopy:
   1 means this is 'strip'; 0 means this is 'objcopy'.
   -1 means if we should use argv[0] to decide. */
extern int is_strip;


static void
copy_usage (stream, status)
     FILE *stream;
     int status;
{
  fprintf (stream, "\
Usage: %s [-vVSgxX] [-I bfdname] [-O bfdname] [-F bfdname] [-b byte]\n\
       [-R section] [-i interleave] [--interleave=interleave] [--byte=byte]\n\
       [--input-target=bfdname] [--output-target=bfdname] [--target=bfdname]\n\
       [--strip-all] [--strip-debug] [--discard-all] [--discard-locals]\n\
       [--remove-section=section] [--set-start=val] [--adjust-start=incr]\n\
       [--adjust-vma=incr] [--adjust-section-vma=section{=,+,-}val]\n\
       [--adjust-warnings] [--no-adjust-warnings] [--verbose] [--version]\n\
       [--help] in-file [out-file]\n",
	   program_name);
  exit (status);
}

static void
strip_usage (stream, status)
     FILE *stream;
     int status;
{
  fprintf (stream, "\
Usage: %s [-vVsSgxX] [-I bfdname] [-O bfdname] [-F bfdname] [-R section]\n\
       [--input-target=bfdname] [--output-target=bfdname] [--target=bfdname]\n\
       [--strip-all] [--strip-debug] [--discard-all] [--discard-locals]\n\
       [--remove-section=section] [--verbose] [--version] [--help] file...\n",
	   program_name);
  exit (status);
}

/* Parse a string into a VMA, with a fatal error if it can't be
   parsed.  */

static bfd_vma
parse_vma (s, arg)
     const char *s;
     const char *arg;
{
  bfd_vma ret;
  const char *end;

  ret = bfd_scan_vma (s, &end, 0);
  if (*end != '\0')
    {
      fprintf (stderr, "%s: %s: bad number: %s\n", program_name, arg, s);
      exit (1);
    }
  return ret;
}

/* Return the name of a temporary file in the same directory as FILENAME.  */

static char *
make_tempname (filename)
     char *filename;
{
  static char template[] = "stXXXXXX";
  char *tmpname;
  char *slash = strrchr (filename, '/');

  if (slash != (char *) NULL)
    {
      *slash = 0;
      tmpname = xmalloc (strlen (filename) + sizeof (template) + 1);
      strcpy (tmpname, filename);
      strcat (tmpname, "/");
      strcat (tmpname, template);
      mktemp (tmpname);
      *slash = '/';
    }
  else
    {
      tmpname = xmalloc (sizeof (template));
      strcpy (tmpname, template);
      mktemp (tmpname);
    }
  return tmpname;
}

/* Choose which symbol entries to copy; put the result in OSYMS.
   We don't copy in place, because that confuses the relocs.
   Return the number of symbols to print.  */

static unsigned int
filter_symbols (abfd, osyms, isyms, symcount)
     bfd *abfd;
     asymbol **osyms, **isyms;
     long symcount;
{
  register asymbol **from = isyms, **to = osyms;
  long src_count = 0, dst_count = 0;

  for (; src_count < symcount; src_count++)
    {
      asymbol *sym = from[src_count];
      flagword flags = sym->flags;
      int keep;

      if ((flags & BSF_GLOBAL)	/* Keep if external.  */
	  || (flags & BSF_KEEP)	/* Keep if used in a relocation.  */
	  || bfd_is_und_section (bfd_get_section (sym))
	  || bfd_is_com_section (bfd_get_section (sym)))
	keep = 1;
      else if ((flags & BSF_DEBUGGING) != 0)	/* Debugging symbol.  */
	keep = strip_symbols != strip_debug;
      else			/* Local symbol.  */
	keep = discard_locals != locals_all
	  && (discard_locals != locals_start_L ||
	      ! bfd_is_local_label (abfd, sym));
      if (keep)
	to[dst_count++] = sym;
    }

  return dst_count;
}

/* Keep only every `copy_byte'th byte in MEMHUNK, which is *SIZE bytes long.
   Adjust *SIZE.  */

void
filter_bytes (memhunk, size)
     char *memhunk;
     bfd_size_type *size;
{
  char *from = memhunk + copy_byte, *to = memhunk, *end = memhunk + *size;

  for (; from < end; from += interleave)
    *to++ = *from;
  *size /= interleave;
}

/* Copy object file IBFD onto OBFD.  */

static void
copy_object (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  bfd_vma start;
  long symcount;

  if (!bfd_set_format (obfd, bfd_get_format (ibfd)))
    {
      nonfatal (bfd_get_filename (obfd));
    }

  if (verbose)
    printf ("copy from %s(%s) to %s(%s)\n",
	    bfd_get_filename(ibfd), bfd_get_target(ibfd),
	    bfd_get_filename(obfd), bfd_get_target(obfd));

  if (set_start_set)
    start = set_start;
  else
    {
      /* As a special hack make it easier to generate a raw binary
         file, we default the starting address to zero for the binary
         output format.  */
      if (strcmp (bfd_get_target (obfd), "binary") == 0)
	start = 0;
      else
	start = bfd_get_start_address (ibfd);
    }
  start += adjust_start;

  if (!bfd_set_start_address (obfd, start)
      || !bfd_set_file_flags (obfd,
			      (bfd_get_file_flags (ibfd)
			       & bfd_applicable_file_flags (obfd))))
    {
      nonfatal (bfd_get_filename (ibfd));
    }

  /* Copy architecture of input file to output file */
  if (!bfd_set_arch_mach (obfd, bfd_get_arch (ibfd),
			  bfd_get_mach (ibfd)))
    {
      fprintf (stderr, "Output file cannot represent architecture %s\n",
	       bfd_printable_arch_mach (bfd_get_arch (ibfd),
					bfd_get_mach (ibfd)));
    }
  if (!bfd_set_format (obfd, bfd_get_format (ibfd)))
    {
      nonfatal (bfd_get_filename(ibfd));
    }

  if (isympp)
    free (isympp);
  if (osympp != isympp)
    free (osympp);

  /* bfd mandates that all output sections be created and sizes set before
     any output is done.  Thus, we traverse all sections multiple times.  */
  bfd_map_over_sections (ibfd, setup_section, (void *) obfd);

  /* Symbol filtering must happen after the output sections have
     been created, but before their contents are set.  */
  if (strip_symbols == strip_all && discard_locals == locals_undef)
    {
      osympp = isympp = NULL;
      symcount = 0;
    }
  else
    {
      long symsize;

      symsize = bfd_get_symtab_upper_bound (ibfd);
      if (symsize < 0)
	{
	  nonfatal (bfd_get_filename (ibfd));
	}

      osympp = isympp = (asymbol **) xmalloc (symsize);
      symcount = bfd_canonicalize_symtab (ibfd, isympp);
      if (symcount < 0)
	{
	  nonfatal (bfd_get_filename (ibfd));
	}

      if (strip_symbols == strip_debug || discard_locals != locals_undef)
	{
	  /* Mark symbols used in output relocations so that they
	     are kept, even if they are local labels or static symbols.

	     Note we iterate over the input sections examining their
	     relocations since the relocations for the output sections
	     haven't been set yet.  mark_symbols_used_in_relocations will
	     ignore input sections which have no corresponding output
	     section.  */
	  bfd_map_over_sections (ibfd,
				 mark_symbols_used_in_relocations,
				 (PTR)isympp);
	  osympp = (asymbol **) xmalloc (symcount * sizeof (asymbol *));
	  symcount = filter_symbols (ibfd, osympp, isympp, symcount);
	}
    }

  bfd_set_symtab (obfd, osympp, symcount);

  /* This has to happen after the symbol table has been set.  */
  bfd_map_over_sections (ibfd, copy_section, (void *) obfd);

  /* Allow the BFD backend to copy any private data it understands
     from the input BFD to the output BFD.  This is done last to
     permit the routine to look at the filtered symbol table, which is
     important for the ECOFF code at least.  */
  if (!bfd_copy_private_bfd_data (ibfd, obfd))
    {
      fprintf (stderr, "%s: %s: error copying private BFD data: %s\n",
	       program_name, bfd_get_filename (obfd),
	       bfd_errmsg (bfd_get_error ()));
      status = 1;
      return;
    }
}

static char *
cat (a, b, c)
     char *a;
     char *b;
     char *c;
{
  size_t size = strlen (a) + strlen (b) + strlen (c);
  char *r = xmalloc (size + 1);

  strcpy (r, a);
  strcat (r, b);
  strcat (r, c);
  return r;
}

/* Read each archive element in turn from IBFD, copy the
   contents to temp file, and keep the temp file handle.  */

static void
copy_archive (ibfd, obfd, output_target)
     bfd *ibfd;
     bfd *obfd;
     char *output_target;
{
  bfd **ptr = &obfd->archive_head;
  bfd *this_element;
  char *dir = make_tempname (bfd_get_filename (obfd));

  /* Make a temp directory to hold the contents.  */
  mkdir (dir, 0700);
  obfd->has_armap = ibfd->has_armap;

  this_element = bfd_openr_next_archived_file (ibfd, NULL);
  ibfd->archive_head = this_element;
  while (this_element != (bfd *) NULL)
    {
      /* Create an output file for this member.  */
      char *output_name = cat (dir, "/", bfd_get_filename(this_element));
      bfd *output_bfd = bfd_openw (output_name, output_target);

      if (output_bfd == (bfd *) NULL)
	{
	  nonfatal (output_name);
	}
      if (!bfd_set_format (obfd, bfd_get_format (ibfd)))
	{
	  nonfatal (bfd_get_filename (obfd));
	}

      if (bfd_check_format (this_element, bfd_object) == true)
	{
	  copy_object (this_element, output_bfd);
	}

      bfd_close (output_bfd);
      /* Open the newly output file and attatch to our list.  */
      output_bfd = bfd_openr (output_name, output_target);

      /* Mark it for deletion.  */
      *ptr = output_bfd;
      ptr = &output_bfd->next;
      this_element->next = bfd_openr_next_archived_file (ibfd, this_element);
      this_element = this_element->next;
    }
  *ptr = (bfd *) NULL;

  if (!bfd_close (obfd))
    {
      nonfatal (bfd_get_filename (obfd));
    }

  /* Delete all the files that we opened.
     Construct their names again, unfortunately, but
     we're about to exit anyway.  */
  for (this_element = ibfd->archive_head;
       this_element != (bfd *) NULL;
       this_element = this_element->next)
    {
      unlink (cat (dir, "/", bfd_get_filename (this_element)));
    }
  rmdir (dir);
  if (!bfd_close (ibfd))
    {
      nonfatal (bfd_get_filename (ibfd));
    }
}

/* The top-level control.  */

static void
copy_file (input_filename, output_filename, input_target, output_target)
     char *input_filename;
     char *output_filename;
     char *input_target;
     char *output_target;
{
  bfd *ibfd;
  char **matching;

  /* To allow us to do "strip *" without dying on the first
     non-object file, failures are nonfatal.  */

  ibfd = bfd_openr (input_filename, input_target);
  if (ibfd == NULL)
    {
      nonfatal (input_filename);
    }

  if (bfd_check_format (ibfd, bfd_archive))
    {
      bfd *obfd;

      /* bfd_get_target does not return the correct value until
         bfd_check_format succeeds.  */
      if (output_target == NULL)
	output_target = bfd_get_target (ibfd);

      obfd = bfd_openw (output_filename, output_target);
      if (obfd == NULL)
	{
	  nonfatal (output_filename);
	}
      copy_archive (ibfd, obfd, output_target);
    }
  else if (bfd_check_format_matches (ibfd, bfd_object, &matching))
    {
      bfd *obfd;

      /* bfd_get_target does not return the correct value until
         bfd_check_format succeeds.  */
      if (output_target == NULL)
	output_target = bfd_get_target (ibfd);

      obfd = bfd_openw (output_filename, output_target);
      if (obfd == NULL)
	{
	  nonfatal (output_filename);
	}

      copy_object (ibfd, obfd);

      if (!bfd_close (obfd))
	{
	  nonfatal (output_filename);
	}

      if (!bfd_close (ibfd))
	{
	  nonfatal (input_filename);
	}
    }
  else
    {
      bfd_nonfatal (input_filename);
      if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	{
	  list_matching_formats (matching);
	  free (matching);
	}
      status = 1;
    }
}

/* Create a section in OBFD with the same name and attributes
   as ISECTION in IBFD.  */

static void
setup_section (ibfd, isection, obfdarg)
     bfd *ibfd;
     sec_ptr isection;
     PTR obfdarg;
{
  bfd *obfd = (bfd *) obfdarg;
  struct section_list *p;
  sec_ptr osection;
  bfd_vma vma;
  char *err;

  if ((bfd_get_section_flags (ibfd, isection) & SEC_DEBUGGING) != 0
      && (strip_symbols == strip_debug
	  || strip_symbols == strip_all
	  || discard_locals == locals_all))
    return;

  for (p = remove_sections; p != NULL; p = p->next)
    {
      if (strcmp (p->name, bfd_section_name (ibfd, isection)) == 0)
	{
	  p->used = true;
	  return;
	}
    }

  osection = bfd_make_section_anyway (obfd, bfd_section_name (ibfd, isection));
  if (osection == NULL)
    {
      err = "making";
      goto loser;
    }

  if (!bfd_set_section_size (obfd,
			     osection,
			     bfd_section_size (ibfd, isection)))
    {
      err = "size";
      goto loser;
    }

  vma = bfd_section_vma (ibfd, isection);
  for (p = adjust_sections; p != NULL; p = p->next)
    {
      if (strcmp (p->name, bfd_section_name (ibfd, isection)) == 0)
	{
	  if (p->adjust)
	    vma += p->val;
	  else
	    vma = p->val;
	  p->used = true;
	  break;
	}
    }
  if (p == NULL)
    vma += adjust_section_vma;

  if (! bfd_set_section_vma (obfd, osection, vma))
    {
      err = "vma";
      goto loser;
    }

  if (bfd_set_section_alignment (obfd,
				 osection,
				 bfd_section_alignment (ibfd, isection))
      == false)
    {
      err = "alignment";
      goto loser;
    }

  if (!bfd_set_section_flags (obfd, osection,
			      bfd_get_section_flags (ibfd, isection)))
    {
      err = "flags";
      goto loser;
    }

  /* This used to be mangle_section; we do here to avoid using
     bfd_get_section_by_name since some formats allow multiple
     sections with the same name.  */
  isection->output_section = osection;
  isection->output_offset = 0;

  /* Allow the BFD backend to copy any private data it understands
     from the input section to the output section.  */
  if (!bfd_copy_private_section_data (ibfd, isection, obfd, osection))
    {
      err = "private data";
      goto loser;
    }

  /* All went well */
  return;

loser:
  fprintf (stderr, "%s: %s: section `%s': error in %s: %s\n",
	   program_name,
	   bfd_get_filename (ibfd), bfd_section_name (ibfd, isection),
	   err, bfd_errmsg (bfd_get_error ()));
  status = 1;
}

/* Copy the data of input section ISECTION of IBFD
   to an output section with the same name in OBFD.
   If stripping then don't copy any relocation info.  */

static void
copy_section (ibfd, isection, obfdarg)
     bfd *ibfd;
     sec_ptr isection;
     PTR obfdarg;
{
  bfd *obfd = (bfd *) obfdarg;
  struct section_list *p;
  arelent **relpp;
  long relcount;
  sec_ptr osection;
  bfd_size_type size;

  if ((bfd_get_section_flags (ibfd, isection) & SEC_DEBUGGING) != 0
      && (strip_symbols == strip_debug
	  || strip_symbols == strip_all
	  || discard_locals == locals_all))
    {
      return;
    }

  for (p = remove_sections; p != NULL; p = p->next)
    if (strcmp (p->name, bfd_section_name (ibfd, isection)) == 0)
      return;

  osection = isection->output_section;
  size = bfd_get_section_size_before_reloc (isection);

  if (size == 0 || osection == 0)
    return;

  if (strip_symbols == strip_all)
    bfd_set_reloc (obfd, osection, (arelent **) NULL, 0);
  else
    {
      long relsize;

      relsize = bfd_get_reloc_upper_bound (ibfd, isection);
      if (relsize < 0)
	{
	  nonfatal (bfd_get_filename (ibfd));
	}
      if (relsize == 0)
	bfd_set_reloc (obfd, osection, (arelent **) NULL, 0);
      else
	{
	  relpp = (arelent **) xmalloc (relsize);
	  relcount = bfd_canonicalize_reloc (ibfd, isection, relpp, isympp);
	  if (relcount < 0)
	    {
	      nonfatal (bfd_get_filename (ibfd));
	    }
	  bfd_set_reloc (obfd, osection, relpp, relcount);
	}
    }

  isection->_cooked_size = isection->_raw_size;
  isection->reloc_done = true;

  if (bfd_get_section_flags (ibfd, isection) & SEC_HAS_CONTENTS)
    {
      PTR memhunk = (PTR) xmalloc ((unsigned) size);

      if (!bfd_get_section_contents (ibfd, isection, memhunk, (file_ptr) 0,
				     size))
	{
	  nonfatal (bfd_get_filename (ibfd));
	}

      if (copy_byte >= 0) 
        {
	  filter_bytes (memhunk, &size);
              /* The section has gotten smaller. */
          if (!bfd_set_section_size (obfd, osection, size))
            nonfatal (bfd_get_filename (obfd));
        }

      if (!bfd_set_section_contents (obfd, osection, memhunk, (file_ptr) 0,
				     size))
	{
	  nonfatal (bfd_get_filename (obfd));
	}
      free (memhunk);
    }
}

/* Mark all the symbols which will be used in output relocations with
   the BSF_KEEP flag so that those symbols will not be stripped.

   Ignore relocations which will not appear in the output file.  */

static void
mark_symbols_used_in_relocations (ibfd, isection, symbolsarg)
     bfd *ibfd;
     sec_ptr isection;
     PTR symbolsarg;
{
  asymbol **symbols = (asymbol **) symbolsarg;
  long relsize;
  arelent **relpp;
  long relcount, i;

  /* Ignore an input section with no corresponding output section.  */
  if (isection->output_section == NULL)
    return;

  relsize = bfd_get_reloc_upper_bound (ibfd, isection);
  if (relsize < 0)
    bfd_fatal (bfd_get_filename (ibfd));

  relpp = (arelent **) xmalloc (relsize);
  relcount = bfd_canonicalize_reloc (ibfd, isection, relpp, symbols);
  if (relcount < 0)
    bfd_fatal (bfd_get_filename (ibfd));

  /* Examine each symbol used in a relocation.  If it's not one of the
     special bfd section symbols, then mark it with BSF_KEEP.  */
  for (i = 0; i < relcount; i++)
    {
      if (*relpp[i]->sym_ptr_ptr != bfd_com_section_ptr->symbol
	  && *relpp[i]->sym_ptr_ptr != bfd_abs_section_ptr->symbol
	  && *relpp[i]->sym_ptr_ptr != bfd_und_section_ptr->symbol)
	(*relpp[i]->sym_ptr_ptr)->flags |= BSF_KEEP;
    }

  if (relpp != NULL)
    free (relpp);
}

/* The number of bytes to copy at once.  */
#define COPY_BUF 8192

/* Copy file FROM to file TO, performing no translations.
   Return 0 if ok, -1 if error.  */

static int
simple_copy (from, to)
     char *from, *to;
{
  int fromfd, tofd, nread;
  char buf[COPY_BUF];

  fromfd = open (from, O_RDONLY);
  if (fromfd < 0)
    return -1;
  tofd = open (to, O_WRONLY | O_CREAT | O_TRUNC);
  if (tofd < 0)
    {
      close (fromfd);
      return -1;
    }
  while ((nread = read (fromfd, buf, sizeof buf)) > 0)
    {
      if (write (tofd, buf, nread) != nread)
	{
	  close (fromfd);
	  close (tofd);
	  return -1;
	}
    }
  close (fromfd);
  close (tofd);
  if (nread < 0)
    return -1;
  return 0;
}

#ifndef S_ISLNK
#ifdef S_IFLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#else
#define S_ISLNK(m) 0
#define lstat stat
#endif
#endif

/* Rename FROM to TO, copying if TO is a link.
   Assumes that TO already exists, because FROM is a temp file.
   Return 0 if ok, -1 if error.  */

static int
smart_rename (from, to)
     char *from, *to;
{
  struct stat s;
  int ret = 0;

  if (lstat (to, &s))
    return -1;

  /* Use rename only if TO is not a symbolic link and has
     only one hard link.  */
  if (!S_ISLNK (s.st_mode) && s.st_nlink == 1)
    {
      ret = rename (from, to);
      if (ret == 0)
	{
	  /* Try to preserve the permission bits and ownership of TO.  */
	  chmod (to, s.st_mode & 07777);
	  chown (to, s.st_uid, s.st_gid);
	}
    }
  else
    {
      ret = simple_copy (from, to);
      if (ret == 0)
	unlink (from);
    }
  return ret;
}

static int
strip_main (argc, argv)
     int argc;
     char *argv[];
{
  char *input_target = NULL, *output_target = NULL;
  boolean show_version = false;
  int c, i;

  while ((c = getopt_long (argc, argv, "I:O:F:R:sSgxXVv",
			   strip_options, (int *) 0)) != EOF)
    {
      switch (c)
	{
	case 'I':
	  input_target = optarg;
	  break;
	case 'O':
	  output_target = optarg;
	  break;
	case 'F':
	  input_target = output_target = optarg;
	  break;
	case 'R':
	  {
	    struct section_list *n;

	    n = (struct section_list *) xmalloc (sizeof (struct section_list));
	    n->name = optarg;
	    n->used = false;
	    n->next = remove_sections;
	    remove_sections = n;
	  }
	  break;
	case 's':
	  strip_symbols = strip_all;
	  break;
	case 'S':
	case 'g':
	  strip_symbols = strip_debug;
	  break;
	case 'x':
	  discard_locals = locals_all;
	  break;
	case 'X':
	  discard_locals = locals_start_L;
	  break;
	case 'v':
	  verbose = true;
	  break;
	case 'V':
	  show_version = true;
	  break;
	case 0:
	  break;		/* we've been given a long option */
	case 'h':
	  strip_usage (stdout, 0);
	default:
	  strip_usage (stderr, 1);
	}
    }

  if (show_version)
    {
      printf ("GNU %s version %s\n", program_name, program_version);
      exit (0);
    }

  /* Default is to strip all symbols.  */
  if (strip_symbols == strip_undef && discard_locals == locals_undef)
    strip_symbols = strip_all;

  if (output_target == (char *) NULL)
    output_target = input_target;

  i = optind;
  if (i == argc)
    strip_usage (stderr, 1);

  for (; i < argc; i++)
    {
      int hold_status = status;

      char *tmpname = make_tempname (argv[i]);
      status = 0;
      copy_file (argv[i], tmpname, input_target, output_target);
      if (status == 0)
	{
	  smart_rename (tmpname, argv[i]);
	  status = hold_status;
	}
      else
	unlink (tmpname);
      free (tmpname);
    }

  return 0;
}

static int
copy_main (argc, argv)
     int argc;
     char *argv[];
{
  char *input_filename = NULL, *output_filename = NULL;
  char *input_target = NULL, *output_target = NULL;
  boolean show_version = false;
  boolean adjust_warn = true;
  int c;
  struct section_list *p;

  while ((c = getopt_long (argc, argv, "b:i:I:s:O:d:F:R:SgxXVv",
			   copy_options, (int *) 0)) != EOF)
    {
      switch (c)
	{
	case 'b':
	  copy_byte = atoi(optarg);
	  if (copy_byte < 0)
	    {
	      fprintf (stderr, "%s: byte number must be non-negative\n",
		       program_name);
	      exit (1);
	    }
	  break;
	case 'i':
	  interleave = atoi(optarg);
	  if (interleave < 1)
	    {
	      fprintf(stderr, "%s: interleave must be positive\n",
		      program_name);
	      exit (1);
	    }
	  break;
	case 'I':
	case 's':		/* "source" - 'I' is preferred */
	  input_target = optarg;
	  break;
	case 'O':
	case 'd':		/* "destination" - 'O' is preferred */
	  output_target = optarg;
	  break;
	case 'F':
	  input_target = output_target = optarg;
	  break;
	case 'R':
	  p = (struct section_list *) xmalloc (sizeof (struct section_list));
	  p->name = optarg;
	  p->used = false;
	  p->next = remove_sections;
	  remove_sections = p;
	  break;
	case 'S':
	  strip_symbols = strip_all;
	  break;
	case 'g':
	  strip_symbols = strip_debug;
	  break;
	case 'x':
	  discard_locals = locals_all;
	  break;
	case 'X':
	  discard_locals = locals_start_L;
	  break;
	case 'v':
	  verbose = true;
	  break;
	case 'V':
	  show_version = true;
	  break;
	case OPTION_ADJUST_START:
	  adjust_start = parse_vma (optarg, "--adjust-start");
	  break;
	case OPTION_ADJUST_SECTION_VMA:
	  {
	    const char *s;
	    int len;
	    char *name;

	    p = (struct section_list *) xmalloc (sizeof (struct section_list));
	    s = strchr (optarg, '=');
	    if (s != NULL)
	      {
		p->adjust = false;
		p->val = parse_vma (s + 1, "--adjust-section-vma");
	      }
	    else
	      {
		s = strchr (optarg, '+');
		if (s == NULL)
		  {
		    s = strchr (optarg, '-');
		    if (s == NULL)
		      {
			fprintf (stderr,
				 "%s: bad format for --adjust-section-vma\n",
				 program_name);
			exit (1);
		      }
		  }
		p->adjust = true;
		p->val = parse_vma (s + 1, "--adjust-section-vma");
		if (*s == '-')
		  p->val = - p->val;
	      }

	    len = s - optarg;
	    name = (char *) xmalloc (len + 1);
	    strncpy (name, optarg, len);
	    name[len] = '\0';
	    p->name = name;

	    p->used = false;

	    p->next = adjust_sections;
	    adjust_sections = p;
	  }
	  break;
	case OPTION_ADJUST_VMA:
	  adjust_section_vma = parse_vma (optarg, "--adjust-vma");
	  adjust_start = adjust_section_vma;
	  break;
	case OPTION_ADJUST_WARNINGS:
	  adjust_warn = true;
	  break;
	case OPTION_NO_ADJUST_WARNINGS:
	  adjust_warn = false;
	  break;
	case OPTION_SET_START:
	  set_start = parse_vma (optarg, "--set-start");
	  set_start_set = true;
	  break;
	case 0:
	  break;		/* we've been given a long option */
	case 'h':
	  copy_usage (stdout, 0);
	default:
	  copy_usage (stderr, 1);
	}
    }

  if (show_version)
    {
      printf ("GNU %s version %s\n", program_name, program_version);
      exit (0);
    }

  if (copy_byte >= interleave)
    {
      fprintf (stderr, "%s: byte number must be less than interleave\n",
	       program_name);
      exit (1);
    }

  if (optind == argc || optind + 2 < argc)
    copy_usage (stderr, 1);

  input_filename = argv[optind];
  if (optind + 1 < argc)
    output_filename = argv[optind + 1];

  /* Default is to strip no symbols.  */
  if (strip_symbols == strip_undef && discard_locals == locals_undef)
    strip_symbols = strip_none;

  if (output_target == (char *) NULL)
    output_target = input_target;

  /* If there is no destination file then create a temp and rename
     the result into the input.  */

  if (output_filename == (char *) NULL)
    {
      char *tmpname = make_tempname (input_filename);
      copy_file (input_filename, tmpname, input_target, output_target);
      if (status == 0)
	smart_rename (tmpname, input_filename);
      else
	unlink (tmpname);
    }
  else
    {
      copy_file (input_filename, output_filename, input_target, output_target);
    }

  if (adjust_warn)
    {
      for (p = adjust_sections; p != NULL; p = p->next)
	{
	  if (! p->used)
	    {
	      fprintf (stderr, "%s: warning: --adjust-section-vma %s%c0x",
		       program_name, p->name,
		       p->adjust ? '=' : '+');
	      fprintf_vma (stderr, p->val);
	      fprintf (stderr, " never used\n");
	    }
	}
    }

  /* We could issue similar warnings for remove_sections, but I don't
     think that would be as useful.  */

  return 0;
}

int
main (argc, argv)
     int argc;
     char *argv[];
{
  program_name = argv[0];
  xmalloc_set_program_name (program_name);
  strip_symbols = strip_undef;
  discard_locals = locals_undef;

  bfd_init ();

  if (is_strip < 0)
    {
      int i = strlen (program_name);
      is_strip = (i >= 5 && strcmp (program_name + i - 5, "strip"));
    }

  if (is_strip)
    strip_main (argc, argv);
  else
    copy_main (argc, argv);

  return status;
}
