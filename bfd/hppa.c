/* bfd back-end for HP PA-RISC SOM objects.
   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

This file is part of BFD, the Binary File Descriptor library.

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

#ifdef HOST_HPPAHPUX

#include "libbfd.h"
#include "libhppa.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>
#include <sys/user.h>           /* After a.out.h  */
#include <sys/file.h>
#include <errno.h>
 
struct container {
  struct header f;
  struct som_exec_auxhdr e;
};

static bfd_target *
hppa_object_setup (abfd, file_hdrp, aux_hdrp)
     bfd *abfd;
     struct header *file_hdrp;
     struct som_exec_auxhdr *aux_hdrp;
{
  struct container *rawptr;
  struct header *f;
  struct hppa_data_struct *rawptr1;
  asection *text, *data, *bss;

  rawptr = (struct container *) bfd_zalloc (abfd, sizeof (struct container));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  rawptr1 = (struct hppa_data_struct *) bfd_zalloc (abfd, sizeof (struct hppa_data_struct));
  if (rawptr1 == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  
  abfd->tdata.hppa_data = rawptr1;
  obj_file_hdr (abfd) = &rawptr->f;
  obj_aux_hdr (abfd) = &rawptr->e;
  *obj_file_hdr (abfd) = *file_hdrp;
  *obj_aux_hdr (abfd) = *aux_hdrp;

  /* Set the file flags */
  abfd->flags = NO_FLAGS;
  if (file_hdrp->entry_offset)
    abfd->flags |= HAS_RELOC;
  if (file_hdrp->symbol_total)
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;

  bfd_get_start_address (abfd) = aux_hdrp->exec_entry;

  obj_pa_symbols (abfd) = (hppa_symbol_type *)NULL;
  bfd_get_symcount (abfd) = file_hdrp->symbol_total;

  bfd_default_set_arch_mach(abfd, bfd_arch_hppa, 0);

  /* create the sections.  This is raunchy, but bfd_close wants to reclaim
     them */

  text = bfd_make_section(abfd, ".text");
  data = bfd_make_section(abfd, ".data");
  bss = bfd_make_section(abfd, ".bss");

  text->_raw_size = aux_hdrp->exec_tsize;
  data->_raw_size = aux_hdrp->exec_dsize;
  bss->_raw_size = aux_hdrp->exec_bsize;

  text->flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS);
  data->flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS);
  bss->flags = SEC_ALLOC;

  /* The virtual memory addresses of the sections */                    
  text->vma = aux_hdrp->exec_tmem;                          
  data->vma = aux_hdrp->exec_dmem;                          
  bss->vma = aux_hdrp->exec_bfill;                           
                                                                        
  /* The file offsets of the sections */                                
  text->filepos = aux_hdrp->exec_tfile;                      
  data->filepos = aux_hdrp->exec_dfile;                      
                                                                       
  /* The file offsets of the relocation info */                         
  text->rel_filepos = 0;                  
  data->rel_filepos = 0;                  
                                                                        
  /* The file offsets of the string table and symbol table.  */         
  obj_sym_filepos (abfd) = file_hdrp->symbol_location;                  
  bfd_get_symcount (abfd) = file_hdrp->symbol_total;
  obj_str_filepos (abfd) = file_hdrp->symbol_strings_location;           
  obj_stringtab_size (abfd) = file_hdrp->symbol_strings_size;

  return abfd->xvec;
}

/* Create a new BFD section for NAME.  If NAME already exists, then create a
   new unique name, with NAME as the prefix.  This exists because SOM .o files
   created by the native compiler can have a $CODE$ section for each
   subroutine.
 */

static asection *
make_unique_section (abfd, name, num)
     bfd *abfd;
     CONST char *name;
     int num;
{
  asection *sect;
  char *newname;
  char altname[100];

  sect = bfd_make_section (abfd, name);
  while (!sect)
    {
      sprintf(altname, "%s-%d", name, num++);
      sect = bfd_make_section (abfd, altname);
    }

  newname = bfd_alloc (abfd, strlen(sect->name) + 1);
  strcpy (newname, sect->name);

  sect->name = newname;
  return sect;
}

/* Convert all of the space and subspace info into BFD sections.  Each space
   contains a number of subspaces, which in turn describe the mapping between
   regions of the exec file, and the address space that the program runs in.
   BFD sections which correspond to spaces will overlap the sections for the
   associated subspaces.  */

static int
setup_sections (abfd, file_hdr)
     bfd *abfd;
     struct header *file_hdr;
{
  char *space_strings;
  int space_index;

/* First, read in space names */

  space_strings = alloca (file_hdr->space_strings_size);
  if (!space_strings)
    return 0;

  if (bfd_seek (abfd, file_hdr->space_strings_location, SEEK_SET) < 0)
    return 0;
  if (bfd_read (space_strings, 1, file_hdr->space_strings_size, abfd)
      != file_hdr->space_strings_size)
    return 0;

  /* Loop over all of the space dictionaries, building up sections */

  for (space_index = 0; space_index < file_hdr->space_total; space_index++)
    {
      struct space_dictionary_record space;
      struct subspace_dictionary_record subspace;
      int subspace_index, tmp;
      asection *space_asect;

      /* Read the space dictionary element */
      if (bfd_seek (abfd, file_hdr->space_location
		           + space_index * sizeof space, SEEK_SET) < 0)
	return 0;
      if (bfd_read (&space, 1, sizeof space, abfd) != sizeof space)
	return 0;

      /* Setup the space name string */
      space.name.n_name = space.name.n_strx + space_strings;

      /* Make a section out of it */
      space_asect = make_unique_section (abfd, space.name.n_name, space_index);
      if (!space_asect)
	return 0;

      /* Now, read in the first subspace for this space */
      if (bfd_seek (abfd, file_hdr->subspace_location
		       + space.subspace_index * sizeof subspace,
		SEEK_SET) < 0)
	return 0;
      if (bfd_read (&subspace, 1, sizeof subspace, abfd) != sizeof subspace)
	return 0;
      /* Seek back to the start of the subspaces for loop below */
      if (bfd_seek (abfd, file_hdr->subspace_location
		       + space.subspace_index * sizeof subspace,
		SEEK_SET) < 0)
	return 0;

      /* Setup the section flags as appropriate (this is somewhat bogus, as
	 there isn't a clear mapping between what's in the space record, and
	 what BFD can describe here). */
      if (space.is_loadable)
	space_asect->flags |= SEC_ALLOC;
      if (space.is_defined)
	space_asect->flags |= SEC_LOAD;

      /* Setup the start address and file loc from the first subspace record */
      space_asect->vma = subspace.subspace_start;
      space_asect->filepos = subspace.file_loc_init_value;
      space_asect->alignment_power = subspace.alignment;

      /* Loop over the rest of the subspaces, building up more sections */
      for (subspace_index = 0; subspace_index < space.subspace_quantity;
	   subspace_index++)
	{
	  asection *subspace_asect;

	  /* Read in the next subspace */
	  if (bfd_read (&subspace, 1, sizeof subspace, abfd)
	      != sizeof subspace)
	    return 0;

	  /* Setup the subspace name string */
	  subspace.name.n_name = subspace.name.n_strx + space_strings;

	  /* Make a section out of this subspace */
	  subspace_asect = make_unique_section (abfd, subspace.name.n_name,
						space.subspace_index + subspace_index);

	  if (!subspace_asect)
	    return 0;

	  if (subspace.is_loadable)
	    subspace_asect->flags |= SEC_ALLOC | SEC_LOAD;
	  if (subspace.code_only)
	    subspace_asect->flags |= SEC_CODE;

	  subspace_asect->vma = subspace.subspace_start;
	  subspace_asect->_cooked_size = subspace.subspace_length;
	  subspace_asect->_raw_size = subspace.initialization_length;
	  subspace_asect->alignment_power = subspace.alignment;
	  subspace_asect->filepos = subspace.file_loc_init_value;

	}
      /* Setup the sizes for the space section based upon the info in the
	 last subspace of the space. */
      space_asect->_cooked_size = (subspace.subspace_start - space_asect->vma)
				  + subspace.subspace_length;
      space_asect->_raw_size = (subspace.file_loc_init_value
				- space_asect->filepos)
			       + subspace.initialization_length;
    }
}

static bfd_target *
hppa_object_p (abfd)
     bfd *abfd;
{
  struct header file_hdr;
  struct som_exec_auxhdr aux_hdr;

  if (bfd_read ((PTR) &file_hdr, 1, FILE_HDR_SIZE, abfd) != FILE_HDR_SIZE)
    return 0;

  if (!_PA_RISC_ID (file_hdr.system_id))
    {
      bfd_error = wrong_format;
      return 0;
    }

  switch (file_hdr.a_magic)
    {
    case RELOC_MAGIC:	/* I'm not really sure about all of these types... */
    case EXEC_MAGIC:
    case SHARE_MAGIC:
    case DEMAND_MAGIC:
    case DL_MAGIC:
    case SHL_MAGIC:
      break;
    default:
      bfd_error = wrong_format;
      return 0;
    }

  if (file_hdr.version_id != VERSION_ID
      && file_hdr.version_id != NEW_VERSION_ID)
    {
      bfd_error = wrong_format;
      return 0;
    }

  if (bfd_read ((PTR) &aux_hdr, 1, AUX_HDR_SIZE, abfd) != AUX_HDR_SIZE)
    bfd_error = wrong_format;

  if (!setup_sections (abfd, &file_hdr))
    return 0;

  return hppa_object_setup(abfd, &file_hdr, &aux_hdr);
}

static boolean
DEFUN(hppa_mkobject,(abfd),
      bfd *abfd)
{ 
  fprintf (stderr, "hppa_mkobject unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

boolean
DEFUN(hppa_write_object_contents,(abfd),
      bfd *abfd)
{
  fprintf (stderr, "hppa_write_object_contents unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static unsigned int
hppa_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  fprintf (stderr, "hppa_get_symtab_upper_bound unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static unsigned int
hppa_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  fprintf (stderr, "hppa_get_reloc_upper_bound unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static unsigned int
hppa_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  fprintf (stderr, "hppa_canonicalize_reloc unimplemented\n");
  fflush (stderr);
  abort ();
}

extern bfd_target hppa_vec;

static unsigned int
hppa_get_symtab (abfd, location)
     bfd *abfd;
     asymbol **location;
{
  fprintf (stderr, "hppa_get_symtab unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static asymbol *
hppa_make_empty_symbol (abfd)
     bfd *abfd;
{
  hppa_symbol_type  *new =
    (hppa_symbol_type *)bfd_zalloc (abfd, sizeof (hppa_symbol_type));
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

static void 
hppa_print_symbol (ignore_abfd, afile,  symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  fprintf (stderr, "hppa_print_symbol unimplemented\n");
  fflush (stderr);
  abort ();
}

static boolean
hppa_new_section_hook (abfd, newsect)
     bfd *abfd;
     asection *newsect;
{
  newsect->alignment_power = 3;

  /* We allow more than three sections internally */
  return true;
}

static boolean
hppa_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  fprintf (stderr, "hppa_set_section_contents unimplimented\n");
  fflush (stderr);
  abort();
  return false;
}

static boolean
hppa_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  fprintf (stderr, "hppa_set_arch_mach unimplemented\n");
  fflush (stderr);
  /* Allow any architecture to be supported by the hppa backend */
  return  bfd_default_set_arch_mach(abfd, arch, machine);
}

static boolean
hppa_find_nearest_line (abfd, section, symbols, offset, filename_ptr,
			functionname_ptr, line_ptr)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *line_ptr;
{
  fprintf (stderr, "hppa_find_nearest_line unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static int
hppa_sizeof_headers (abfd, reloc)
      bfd *abfd;
      boolean reloc;
{
  fprintf (stderr, "hppa_sizeof_headers unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static asection *
make_bfd_asection (abfd, name, flags, _raw_size, vma, alignment_power)
     bfd *abfd;
     CONST char *name;
     flagword flags;
     bfd_size_type _raw_size;
     bfd_vma vma;
     unsigned int alignment_power;
{
  asection *asect;

  asect = bfd_make_section (abfd, name);
  if (!asect)
    return NULL;

  asect->flags = flags;
  asect->_raw_size = _raw_size;
  asect->vma = vma;
  asect->filepos = bfd_tell (abfd);
  asect->alignment_power = alignment_power;

  return asect;
}

static bfd_target *
hppa_core_file_p (abfd)
     bfd *abfd;
{
  core_hdr (abfd) = bfd_zalloc (abfd, sizeof (struct hppa_core_struct));
  if (!core_hdr (abfd))
    return NULL;

  while (1)
    {
      int val;
      struct corehead core_header;

      val = bfd_read ((void *)&core_header, 1, sizeof core_header, abfd);
      if (val <= 0)
	break;
      switch (core_header.type)
	{
	case CORE_KERNEL:
	case CORE_FORMAT:
	  bfd_seek (abfd, core_header.len, SEEK_CUR); /* Just skip this */
	  break;
	case CORE_EXEC:
	  {
	    struct proc_exec proc_exec;
	    bfd_read ((void *)&proc_exec, 1, core_header.len, abfd);
	    strncpy (core_command (abfd), proc_exec.cmd, MAXCOMLEN + 1);
	  }
	  break;
	case CORE_PROC:
	  {
	    struct proc_info proc_info;
	    core_regsec (abfd) = make_bfd_asection (abfd, ".reg",
						    SEC_ALLOC+SEC_HAS_CONTENTS,
						    core_header.len,
						    (int)&proc_info - (int)&proc_info.hw_regs,
						    2);
	    bfd_read (&proc_info, 1, core_header.len, abfd);
	    core_signal (abfd) = proc_info.sig;
	  }
	  if (!core_regsec (abfd))
	    return NULL;
	  break;
	case CORE_DATA:
	  core_datasec (abfd) = make_bfd_asection (abfd, ".data",
						   SEC_ALLOC+SEC_LOAD+SEC_HAS_CONTENTS,
						   core_header.len,
						   core_header.addr,
						   2);
	  if (!core_datasec (abfd))
	    return NULL;
	  bfd_seek (abfd, core_header.len, SEEK_CUR);
	  break;
	case CORE_STACK:
	  core_stacksec (abfd) = make_bfd_asection (abfd, ".stack",
						    SEC_ALLOC+SEC_LOAD+SEC_HAS_CONTENTS,
						    core_header.len,
						    core_header.addr,
						    2);
	  if (!core_stacksec (abfd))
	    return NULL;
	  bfd_seek (abfd, core_header.len, SEEK_CUR);
	  break;
	default:
	  fprintf (stderr, "Unknown HPPA/HPUX core file section type %d\n",
		   core_header.type);
	  bfd_seek (abfd, core_header.len, SEEK_CUR);
	  break;
	}
    }

  /* OK, we believe you.  You're a core file (sure, sure).  */

  return abfd->xvec;
}

static char *
hppa_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

/* ARGSUSED */
static int
hppa_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

/* ARGSUSED */
static boolean
hppa_core_file_matches_executable_p  (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;          /* FIXME, We have no way of telling at this point */
}

#define hppa_bfd_debug_info_start        bfd_void
#define hppa_bfd_debug_info_end          bfd_void
#define hppa_bfd_debug_info_accumulate   (PROTO(void,(*),(bfd*, struct sec *))) bfd_void



#define hppa_openr_next_archived_file    bfd_generic_openr_next_archived_file
#define hppa_generic_stat_arch_elt       bfd_generic_stat_arch_elt
#define hppa_slurp_armap                  bfd_false
#define hppa_slurp_extended_name_table    _bfd_slurp_extended_name_table
#define hppa_truncate_arname              (void (*)())bfd_nullvoidptr
#define hppa_write_armap                  0

#define hppa_get_lineno                   (struct lineno_cache_entry *(*)())bfd_nullvoidptr
#define	hppa_close_and_cleanup	           bfd_generic_close_and_cleanup
#define hppa_get_section_contents          bfd_generic_get_section_contents

#define hppa_bfd_get_relocated_section_contents \
 bfd_generic_get_relocated_section_contents
#define hppa_bfd_relax_section bfd_generic_relax_section
#define hppa_bfd_seclet_link bfd_generic_seclet_link

bfd_target hppa_vec =
{
  "hppa",			/* name */
  bfd_target_hppa_flavour,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_CODE|SEC_DATA|SEC_ROM|SEC_HAS_CONTENTS
   |SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */

   /* leading_symbol_char: is the first char of a user symbol
      predictable, and if so what is it */
   0,
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
    3,				/* minimum alignment */
_do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
_do_getb64, _do_putb64,  _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
  { _bfd_dummy_target,
     hppa_object_p,		/* bfd_check_format */
     bfd_generic_archive_p,
     hppa_core_file_p,
     },
  {
    bfd_false,
    hppa_mkobject, 
    _bfd_generic_mkarchive,
    bfd_false
    },
  {
    bfd_false,
    hppa_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false,
  },
#undef hppa
  JUMP_TABLE(hppa)
};

#endif /* HOST_HPPAHPUX */
