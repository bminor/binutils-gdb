/* BFD back-end for OSF/1 core files.
   Copyright 1993 Free Software Foundation, Inc.

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

/* This file can only be compiled on systems which use OSF/1 style
   core files.  In the config/XXXXXX.mh file for such a system add
      HDEFINES=-DOSF_CORE
      HDEPFILES=osf-core.o
   */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#include <stdio.h>
#include <string.h>
#include <sys/user.h>
#include <sys/core.h>

/* forward declarations */

static asection *
make_bfd_asection PARAMS ((bfd *, CONST char *, flagword, bfd_size_type,
			   bfd_vma, file_ptr));
static asymbol *
osf_core_make_empty_symbol PARAMS ((bfd *));
static bfd_target *
osf_core_core_file_p PARAMS ((bfd *));
static char *
osf_core_core_file_failing_command PARAMS ((bfd *));
static int
osf_core_core_file_failing_signal PARAMS ((bfd *));
static boolean
osf_core_core_file_matches_executable_p PARAMS ((bfd *, bfd *));
static void
swap_abort PARAMS ((void));

/* These are stored in the bfd's tdata */

struct osf_core_struct 
{
  int sig;
  char cmd[MAXCOMLEN + 1];
};

#define core_hdr(bfd) ((bfd)->tdata.osf_core_data)
#define core_signal(bfd) (core_hdr(bfd)->sig)
#define core_command(bfd) (core_hdr(bfd)->cmd)

static asection *
make_bfd_asection (abfd, name, flags, _raw_size, vma, filepos)
     bfd *abfd;
     CONST char *name;
     flagword flags;
     bfd_size_type _raw_size;
     bfd_vma vma;
     file_ptr filepos;
{
  asection *asect;

  asect = bfd_make_section (abfd, name);
  if (!asect)
    return NULL;

  asect->flags = flags;
  asect->_raw_size = _raw_size;
  asect->vma = vma;
  asect->filepos = filepos;
  asect->alignment_power = 8;

  return asect;
}

static asymbol *
osf_core_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  if (new)
    new->the_bfd = abfd;
  return new;
}

static bfd_target *
osf_core_core_file_p (abfd)
     bfd *abfd;
{
  int val;
  int i;
  char *secname;
  struct core_filehdr core_header;
  int dseccnt = 0;

  val = bfd_read ((PTR)&core_header, 1, sizeof core_header, abfd);
  if (val != sizeof core_header)
    return NULL;

  if (strncmp (core_header.magic, "Core", 4) != 0)
    return NULL;

  core_hdr (abfd) = (struct osf_core_struct *)
    bfd_zalloc (abfd, sizeof (struct osf_core_struct));
  if (!core_hdr (abfd))
    return NULL;

  strncpy (core_command (abfd), core_header.name, MAXCOMLEN + 1);
  core_signal (abfd) = core_header.signo;

  for (i = 0; i < core_header.nscns; i++)
    {
      struct core_scnhdr core_scnhdr;

      val = bfd_read ((PTR)&core_scnhdr, 1, sizeof core_scnhdr, abfd);
      if (val != sizeof core_scnhdr)
	break;

      /* Skip empty sections.  */
      if (core_scnhdr.size == 0 || core_scnhdr.scnptr == 0)
	continue;

      switch (core_scnhdr.scntype)
	{
	case SCNRGN:
	  /* OSF/1 has multiple data sections (data, bss and data/bss sections
	     for shared libraries), but bfd doesn't permit data sections with
	     the same name. Construct a unique section name.  */
	  secname = bfd_alloc (abfd, 40);
	  if (!secname)
	    {
	      bfd_error = no_memory;
	      return NULL;
	    }
	  sprintf (secname, ".data%d", dseccnt++);
	  break;
	case SCNSTACK:
	  secname = ".stack";
	  break;
	case SCNREGS:
	  secname = ".reg";
	  break;
	default:
	  fprintf (stderr, "Unhandled OSF/1 core file section type %d\n",
		   core_scnhdr.scntype);
	  continue;
	}

      if (!make_bfd_asection (abfd, secname,
			      SEC_ALLOC+SEC_LOAD+SEC_HAS_CONTENTS,
			      (bfd_size_type) core_scnhdr.size,
			      (bfd_vma) core_scnhdr.vaddr,
			      (file_ptr) core_scnhdr.scnptr))
	return NULL;
    }

  /* OK, we believe you.  You're a core file (sure, sure).  */

  return abfd->xvec;
}

static char *
osf_core_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

/* ARGSUSED */
static int
osf_core_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

/* ARGSUSED */
static boolean
osf_core_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;		/* FIXME, We have no way of telling at this point */
}

/* No archive file support via this BFD */
#define	osf_core_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	osf_core_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	osf_core_slurp_armap			bfd_false
#define	osf_core_slurp_extended_name_table	bfd_true
#define	osf_core_write_armap			(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	osf_core_truncate_arname		bfd_dont_truncate_arname

#define	osf_core_close_and_cleanup		bfd_generic_close_and_cleanup
#define	osf_core_set_section_contents		(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_generic_set_section_contents
#define	osf_core_get_section_contents		bfd_generic_get_section_contents
#define	osf_core_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	osf_core_get_symtab_upper_bound	bfd_0u
#define	osf_core_get_symtab			(unsigned int (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0u
#define	osf_core_get_reloc_upper_bound		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0u
#define	osf_core_canonicalize_reloc		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0u
#define	osf_core_print_symbol			(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	osf_core_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	osf_core_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	osf_core_set_arch_mach			(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	osf_core_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	osf_core_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define osf_core_bfd_debug_info_start		bfd_void
#define osf_core_bfd_debug_info_end		bfd_void
#define osf_core_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define osf_core_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define osf_core_bfd_relax_section		bfd_generic_relax_section
#define osf_core_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define osf_core_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define osf_core_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define osf_core_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define osf_core_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)

/* If somebody calls any byte-swapping routines, shoot them.  */
static void
swap_abort()
{
  abort(); /* This way doesn't require any declaration for ANSI to fuck up */
}
#define	NO_GET	((bfd_vma (*) PARAMS ((   const bfd_byte *))) swap_abort )
#define	NO_PUT	((void    (*) PARAMS ((bfd_vma, bfd_byte *))) swap_abort )
#define	NO_SIGNED_GET \
  ((bfd_signed_vma (*) PARAMS ((const bfd_byte *))) swap_abort )

bfd_target osf_core_vec =
  {
    "osf-core",
    bfd_target_unknown_flavour,
    true,			/* target byte order */
    true,			/* target headers byte order */
    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
    (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
    0,			                                   /* symbol prefix */
    ' ',						   /* ar_pad_char */
    16,							   /* ar_max_namelen */
    3,							   /* minimum alignment power */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 64 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 32 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 16 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 64 bit hdrs */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 32 bit hdrs */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 16 bit hdrs */

    {				/* bfd_check_format */
     _bfd_dummy_target,		/* unknown format */
     _bfd_dummy_target,		/* object file */
     _bfd_dummy_target,		/* archive */
     osf_core_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(osf_core),
    (PTR) 0			/* backend_data */
};
