/* BFD back-end for ARM WINCE PE files.
   Copyright 2006, 2007, 2008 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#define TARGET_UNDERSCORE    0
#define USER_LABEL_PREFIX    ""

#define TARGET_LITTLE_SYM    arm_wince_pe_little_vec
#define TARGET_LITTLE_NAME   "pe-arm-wince-little"
#define TARGET_BIG_SYM       arm_wince_pe_big_vec
#define TARGET_BIG_NAME      "pe-arm-wince-big"

#define bfd_arm_allocate_interworking_sections \
  bfd_arm_wince_pe_allocate_interworking_sections
#define bfd_arm_get_bfd_for_interworking \
  bfd_arm_wince_pe_get_bfd_for_interworking
#define bfd_arm_process_before_allocation \
  bfd_arm_wince_pe_process_before_allocation

#define LOCAL_LABEL_PREFIX "."

#include "sysdep.h"
#include "bfd.h"

#undef  bfd_pe_print_pdata
#define	bfd_pe_print_pdata   pe_print_ce_compressed_pdata
extern bfd_boolean pe_print_ce_compressed_pdata (bfd *, void *);

#include "pe-arm.c"

typedef struct sym_cache
{
  int symcount;
  asymbol **syms;
} sym_cache;

static asymbol **
slurp_symtab (bfd *abfd, sym_cache *psc)
{
  asymbol **sy = NULL;
  long storage;

  if (!(bfd_get_file_flags (abfd) & HAS_SYMS))
    {
      psc->symcount = 0;
      return NULL;
    }

  storage = bfd_get_symtab_upper_bound (abfd);
  if (storage < 0)
    return NULL;
  if (storage)
    sy = bfd_malloc (storage);

  psc->symcount = bfd_canonicalize_symtab (abfd, sy);
  if (psc->symcount < 0)
    return NULL;
  return sy;
}

static const char *
my_symbol_for_address (bfd *abfd, bfd_vma func, sym_cache *psc)
{
  int i;

  if (psc->syms == 0)
    psc->syms = slurp_symtab (abfd, psc);

  for (i = 0; i < psc->symcount; i++)
    {
      if (psc->syms[i]->section->vma + psc->syms[i]->value == func)
	return psc->syms[i]->name;
    }

  return NULL;
}

static void
cleanup_syms (sym_cache *psc)
{
  psc->symcount = 0;
  free (psc->syms);
  psc->syms = NULL;
}

/* This is the version for "compressed" pdata.  */

bfd_boolean
pe_print_ce_compressed_pdata (bfd * abfd, void * vfile)
{
# define PDATA_ROW_SIZE	(2 * 4)
  FILE *file = (FILE *) vfile;
  bfd_byte *data = 0;
  asection *section = bfd_get_section_by_name (abfd, ".pdata");
  bfd_size_type datasize = 0;
  bfd_size_type i;
  bfd_size_type start, stop;
  int onaline = PDATA_ROW_SIZE;
  struct sym_cache sym_cache = {0, 0} ;

  if (section == NULL
      || coff_section_data (abfd, section) == NULL
      || pei_section_data (abfd, section) == NULL)
    return TRUE;

  stop = pei_section_data (abfd, section)->virt_size;
  if ((stop % onaline) != 0)
    fprintf (file,
	     _("Warning, .pdata section size (%ld) is not a multiple of %d\n"),
	     (long) stop, onaline);

  fprintf (file,
	   _("\nThe Function Table (interpreted .pdata section contents)\n"));

  fprintf (file, _("\
 vma:\t\tBegin    Prolog   Function Flags    Exception EH\n\
     \t\tAddress  Length   Length   32b exc  Handler   Data\n"));

  datasize = section->size;
  if (datasize == 0)
    return TRUE;

  if (! bfd_malloc_and_get_section (abfd, section, &data))
    {
      if (data != NULL)
	free (data);
      return FALSE;
    }

  start = 0;

  for (i = start; i < stop; i += onaline)
    {
      bfd_vma begin_addr;
      bfd_vma other_data;
      bfd_vma prolog_length, function_length;
      int flag32bit, exception_flag;
      bfd_byte *tdata = 0;
      asection *tsection;

      if (i + PDATA_ROW_SIZE > stop)
	break;

      begin_addr = GET_PDATA_ENTRY (abfd, data + i     );
      other_data = GET_PDATA_ENTRY (abfd, data + i +  4);

      if (begin_addr == 0 && other_data == 0)
	/* We are probably into the padding of the section now.  */
	break;

      prolog_length = (other_data & 0x000000FF);
      function_length = (other_data & 0x3FFFFF00) >> 8;
      flag32bit = (int)((other_data & 0x40000000) >> 30);
      exception_flag = (int)((other_data & 0x80000000) >> 31);

      fputc (' ', file);
      fprintf_vma (file, i + section->vma); fputc ('\t', file);
      fprintf_vma (file, begin_addr); fputc (' ', file);
      fprintf_vma (file, prolog_length); fputc (' ', file);
      fprintf_vma (file, function_length); fputc (' ', file);
      fprintf (file, "%2d  %2d   ", flag32bit, exception_flag);

      /* Get the exception handler's address and the data passed from the
         .text section. This is really the data that belongs with the .pdata
         but got "compressed" out for the ARM and SH4 architectures.  */
      tsection = bfd_get_section_by_name (abfd, ".text");
      if (tsection && coff_section_data (abfd, tsection)
	  && pei_section_data (abfd, tsection))
	{
	  if (bfd_malloc_and_get_section (abfd, tsection, & tdata))
	    {
	      int xx = (begin_addr - 8) - tsection->vma;

	      tdata = bfd_malloc (8);
	      if (bfd_get_section_contents (abfd, tsection, tdata, (bfd_vma) xx, 8))
		{
		  bfd_vma eh, eh_data;

		  eh = bfd_get_32 (abfd, tdata);
		  eh_data = bfd_get_32 (abfd, tdata + 4);
		  fprintf (file, "%08x  ", (unsigned int) eh);
		  fprintf (file, "%08x", (unsigned int) eh_data);
		  if (eh != 0)
		    {
		      const char *s = my_symbol_for_address (abfd, eh, &sym_cache);

		      if (s)
			fprintf (file, " (%s) ", s);
		    }
		}
	      free (tdata);
	    }
	  else
	    {
	      if (tdata)
		free (tdata);
	    }
	}

      fprintf (file, "\n");
    }

  free (data);

  cleanup_syms (& sym_cache);

  return TRUE;
#undef PDATA_ROW_SIZE
}
