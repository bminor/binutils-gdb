/* BFD back-end for HP/UX core files.
   Copyright 1993 Free Software Foundation, Inc.
   Written by Stu Grossman, Cygnus Support.
   Converted to back-end form by Ian Lance Taylor, Cygnus SUpport

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

/* This file can only be compiled on systems which use HP/UX style
   core files.  In the config/XXXXXX.mh file for such a system add
      HDEFINES=-DHPUX_CORE
      HDEPFILES=hpux-core.o
   */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#if defined (HOST_HPPAHPUX) || defined (HOST_HP300HPUX)

/* FIXME: sys/core.h doesn't exist for HPUX version 7.  HPUX version
   5, 6, and 7 core files seem to be standard trad-core.c type core
   files; can we just use trad-core.c in addition to this file?  */

#include <sys/core.h>
#include <sys/utsname.h>

#endif /* HOST_HPPAHPUX */

#ifdef HOST_HPPABSD

/* Not a very swift place to put it, but that's where the BSD port
   puts them.  */
#include "/hpux/usr/include/sys/core.h"

#endif /* HOST_HPPABSD */

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>
#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#include <errno.h>

/* These are stored in the bfd's tdata */

struct hpux_core_struct 
{
  int sig;
  char cmd[MAXCOMLEN + 1];
  asection *data_section;
  asection *stack_section;
  asection *reg_section;
};

#define core_hdr(bfd) ((bfd)->tdata.hpux_core_data)
#define core_signal(bfd) (core_hdr(bfd)->sig)
#define core_command(bfd) (core_hdr(bfd)->cmd)
#define core_datasec(bfd) (core_hdr(bfd)->data_section)
#define core_stacksec(bfd) (core_hdr(bfd)->stack_section)
#define core_regsec(bfd) (core_hdr(bfd)->reg_section)

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

static asymbol *
hpux_core_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  new->the_bfd = abfd;
  return new;
}

static bfd_target *
hpux_core_core_file_p (abfd)
     bfd *abfd;
{
  core_hdr (abfd) = (struct hpux_core_struct *)
    bfd_zalloc (abfd, sizeof (struct hpux_core_struct));
  if (!core_hdr (abfd))
    return NULL;

  while (1)
    {
      int val;
      struct corehead core_header;

      val = bfd_read ((void *) &core_header, 1, sizeof core_header, abfd);
      if (val <= 0)
	break;
      switch (core_header.type)
	{
	case CORE_KERNEL:
	case CORE_FORMAT:
	  bfd_seek (abfd, core_header.len, SEEK_CUR);	/* Just skip this */
	  break;
	case CORE_EXEC:
	  {
	    struct proc_exec proc_exec;
	    bfd_read ((void *) &proc_exec, 1, core_header.len, abfd);
	    strncpy (core_command (abfd), proc_exec.cmd, MAXCOMLEN + 1);
	  }
	  break;
	case CORE_PROC:
	  {
	    struct proc_info proc_info;
	    core_regsec (abfd) = make_bfd_asection (abfd, ".reg",
					       SEC_ALLOC + SEC_HAS_CONTENTS,
						    core_header.len,
				(int) &proc_info - (int) &proc_info.hw_regs,
						    2);
	    bfd_read (&proc_info, 1, core_header.len, abfd);
	    core_signal (abfd) = proc_info.sig;
	  }
	  if (!core_regsec (abfd))
	    return NULL;
	  break;
	case CORE_DATA:
	  core_datasec (abfd) = make_bfd_asection (abfd, ".data",
				    SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS,
						   core_header.len,
						   core_header.addr,
						   2);
	  if (!core_datasec (abfd))
	    return NULL;
	  bfd_seek (abfd, core_header.len, SEEK_CUR);
	  break;
	case CORE_STACK:
	  core_stacksec (abfd) = make_bfd_asection (abfd, ".stack",
				    SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS,
						    core_header.len,
						    core_header.addr,
						    2);
	  if (!core_stacksec (abfd))
	    return NULL;
	  bfd_seek (abfd, core_header.len, SEEK_CUR);
	  break;
	default:
	  /* Falling into here is an error and should prevent this
	     target from matching.  That way systems which use hpux
	     cores along with other formats can still work.  */
	  return 0;
	}
    }

  /* OK, we believe you.  You're a core file (sure, sure).  */

  return abfd->xvec;
}

static char *
hpux_core_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

/* ARGSUSED */
static int
hpux_core_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

/* ARGSUSED */
static boolean
hpux_core_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;			/* FIXME, We have no way of telling at this point */
}

/* No archive file support via this BFD */
#define	hpux_core_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	hpux_core_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	hpux_core_slurp_armap			bfd_false
#define	hpux_core_slurp_extended_name_table	bfd_true
#define	hpux_core_write_armap			(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	hpux_core_truncate_arname		bfd_dont_truncate_arname

#define	hpux_core_close_and_cleanup		bfd_generic_close_and_cleanup
#define	hpux_core_set_section_contents		(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_false
#define	hpux_core_get_section_contents		bfd_generic_get_section_contents
#define	hpux_core_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	hpux_core_get_symtab_upper_bound	bfd_0u
#define	hpux_core_get_symtab			(unsigned int (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0u
#define	hpux_core_get_reloc_upper_bound		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0u
#define	hpux_core_canonicalize_reloc		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0u
#define	hpux_core_print_symbol			(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	hpux_core_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	hpux_core_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	hpux_core_set_arch_mach			(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	hpux_core_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	hpux_core_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define hpux_core_bfd_debug_info_start		bfd_void
#define hpux_core_bfd_debug_info_end		bfd_void
#define hpux_core_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define hpux_core_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define hpux_core_bfd_relax_section		bfd_generic_relax_section
#define hpux_core_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define hpux_core_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define hpux_core_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define hpux_core_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define hpux_core_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)

/* If somebody calls any byte-swapping routines, shoot them.  */
void
swap_abort()
{
  abort(); /* This way doesn't require any declaration for ANSI to fuck up */
}
#define	NO_GET	((bfd_vma (*) PARAMS ((         bfd_byte *))) swap_abort )
#define	NO_PUT	((void    (*) PARAMS ((bfd_vma, bfd_byte *))) swap_abort )
#define	NO_SIGNED_GET ((bfd_signed_vma (*) PARAMS ((bfd_byte *))) swap_abort )

bfd_target hpux_core_vec =
  {
    "hpux-core",
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
     hpux_core_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(hpux_core),
    (PTR) 0			/* backend_data */
};
