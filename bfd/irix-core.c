/* BFD back-end for Irix core files.
   Copyright 1993 Free Software Foundation, Inc.
   Written by Stu Grossman, Cygnus Support.
   Converted to back-end form by Ian Lance Taylor, Cygnus Support

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

/* This file can only be compiled on systems which use Irix style core
   files (namely, Irix 4 and Irix 5, so far).  In the config/XXXXXX.mh
   file for such a system add
      HDEFINES=-DIRIX_CORE
      HDEPFILES=irix-core.o
   */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#ifdef IRIX_CORE

#include <core.out.h>

struct sgi_core_struct 
{
  int sig;
  char cmd[CORE_NAMESIZE];
};

#define core_hdr(bfd) ((bfd)->tdata.sgi_core_data)
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

  asect = bfd_make_section_anyway (abfd, name);
  if (!asect)
    return NULL;

  asect->flags = flags;
  asect->_raw_size = _raw_size;
  asect->vma = vma;
  asect->filepos = filepos;
  asect->alignment_power = 4;

  return asect;
}

static bfd_target *
irix_core_core_file_p (abfd)
     bfd *abfd;
{
  int val;
  int i;
  char *secname;
  struct coreout coreout;
  struct idesc *idg, *idf, *ids;

  val = bfd_read ((PTR)&coreout, 1, sizeof coreout, abfd);
  if (val != sizeof coreout)
    {
      if (bfd_get_error () != bfd_error_system_call)
	bfd_set_error (bfd_error_wrong_format);
      return 0;
    }

  if (coreout.c_magic != CORE_MAGIC
      || coreout.c_version != CORE_VERSION1)
    return 0;

  core_hdr (abfd) = (struct sgi_core_struct *) bfd_zalloc (abfd, sizeof (struct sgi_core_struct));
  if (!core_hdr (abfd))
    return NULL;

  strncpy (core_command (abfd), coreout.c_name, CORE_NAMESIZE);
  core_signal (abfd) = coreout.c_sigcause;

  if (bfd_seek (abfd, coreout.c_vmapoffset, SEEK_SET) != 0)
    return NULL;

  for (i = 0; i < coreout.c_nvmap; i++)
    {
      struct vmap vmap;

      val = bfd_read ((PTR)&vmap, 1, sizeof vmap, abfd);
      if (val != sizeof vmap)
	break;

      switch (vmap.v_type)
	{
	case VDATA:
	  secname = ".data";
	  break;
	case VSTACK:
	  secname = ".stack";
	  break;
#ifdef VMAPFILE
	case VMAPFILE:
	  secname = ".mapfile";
	  break;
#endif
	default:
	  continue;
	}

      if (!make_bfd_asection (abfd, secname,
			      SEC_ALLOC+SEC_LOAD+SEC_HAS_CONTENTS,
			      vmap.v_len,
			      vmap.v_vaddr,
			      vmap.v_offset,
			      2))
	return NULL;
    }

  /* Make sure that the regs are contiguous within the core file. */

  idg = &coreout.c_idesc[I_GPREGS];
  idf = &coreout.c_idesc[I_FPREGS];
  ids = &coreout.c_idesc[I_SPECREGS];

  if (idg->i_offset + idg->i_len != idf->i_offset
      || idf->i_offset + idf->i_len != ids->i_offset)
    return 0;			/* Can't deal with non-contig regs */

  if (bfd_seek (abfd, idg->i_offset, SEEK_SET) != 0)
    return NULL;

  make_bfd_asection (abfd, ".reg",
		     SEC_ALLOC+SEC_HAS_CONTENTS,
		     idg->i_len + idf->i_len + ids->i_len,
		     0,
		     idg->i_offset);

  /* OK, we believe you.  You're a core file (sure, sure).  */

  return abfd->xvec;
}

static char *
irix_core_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

static int
irix_core_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

static boolean
irix_core_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;			/* XXX - FIXME */
}

static asymbol *
irix_core_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  if (new)
    new->the_bfd = abfd;
  return new;
}

#define	irix_core_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	irix_core_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	irix_core_slurp_armap			bfd_false
#define	irix_core_slurp_extended_name_table	bfd_true
#define	irix_core_write_armap			(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	irix_core_truncate_arname		bfd_dont_truncate_arname

#define	irix_core_close_and_cleanup		bfd_generic_close_and_cleanup
#define	irix_core_set_section_contents		(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_generic_set_section_contents
#define	irix_core_get_section_contents		bfd_generic_get_section_contents
#define	irix_core_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	irix_core_get_symtab_upper_bound	bfd_0l
#define	irix_core_get_symtab			(long (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0l
#define	irix_core_get_reloc_upper_bound		(long (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0l
#define	irix_core_canonicalize_reloc		(long (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0l
#define	irix_core_print_symbol			(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	irix_core_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	irix_core_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	irix_core_set_arch_mach			(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	irix_core_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	irix_core_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define irix_core_bfd_debug_info_start		bfd_void
#define irix_core_bfd_debug_info_end		bfd_void
#define irix_core_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define irix_core_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define irix_core_bfd_relax_section		bfd_generic_relax_section
#define irix_core_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define irix_core_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define irix_core_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define irix_core_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define irix_core_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)

#define irix_core_bfd_copy_private_section_data \
  ((boolean (*) PARAMS ((bfd *, asection *, bfd *, asection *))) bfd_false)
#define irix_core_bfd_copy_private_bfd_data \
  ((boolean (*) PARAMS ((bfd *, bfd *))) bfd_false)
#define irix_core_bfd_is_local_label \
  ((boolean (*) PARAMS ((bfd *, asymbol *))) bfd_false)
#define irix_core_bfd_free_cached_info bfd_true

/* If somebody calls any byte-swapping routines, shoot them.  */
void
swap_abort()
{
  abort(); /* This way doesn't require any declaration for ANSI to fuck up */
}
#define	NO_GET	((bfd_vma (*) PARAMS ((   const bfd_byte *))) swap_abort )
#define	NO_PUT	((void    (*) PARAMS ((bfd_vma, bfd_byte *))) swap_abort )
#define	NO_SIGNED_GET \
  ((bfd_signed_vma (*) PARAMS ((const bfd_byte *))) swap_abort )

bfd_target irix_core_vec =
  {
    "irix-core",
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
     irix_core_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(irix_core),
    (PTR) 0			/* backend_data */
};

#endif /* IRIX_CORE */
