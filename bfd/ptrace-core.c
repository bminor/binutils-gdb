/* BFD backend for core files which use the ptrace_user structure
   Copyright 1993 Free Software Foundation, Inc.
   The structure of this file is based on trad-core.c written by John Gilmore
   of Cygnus Support.
   Modified to work with the ptrace_user structure by Kevin A. Buettner.
   (Longterm it may be better to merge this file with trad-core.c)

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   To use this file on a particular host, configure the host with these
   parameters in the config/h-HOST file:

	HDEFINES=-DPTRACE_CORE
	HDEPFILES=ptrace-core.o

*/

#ifdef PTRACE_CORE

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ptrace.h>


struct trad_core_struct
  {
    asection *data_section;
    asection *stack_section;
    asection *reg_section;
    struct ptrace_user u;
  } *rawptr;

#define core_upage(bfd) (&((bfd)->tdata.trad_core_data->u))
#define core_datasec(bfd) ((bfd)->tdata.trad_core_data->data_section)
#define core_stacksec(bfd) ((bfd)->tdata.trad_core_data->stack_section)
#define core_regsec(bfd) ((bfd)->tdata.trad_core_data->reg_section)

/* forward declarations */

bfd_target *	ptrace_unix_core_file_p PARAMS ((bfd *abfd));
char *		ptrace_unix_core_file_failing_command PARAMS ((bfd *abfd));
int		ptrace_unix_core_file_failing_signal PARAMS ((bfd *abfd));
boolean		ptrace_unix_core_file_matches_executable_p
			 PARAMS ((bfd *core_bfd, bfd *exec_bfd));

/* ARGSUSED */
bfd_target *
ptrace_unix_core_file_p (abfd)
     bfd *abfd;

{
  int val;
  struct ptrace_user u;

  val = bfd_read ((void *)&u, 1, sizeof u, abfd);
  if (val != sizeof u || u.pt_magic != _BCS_PTRACE_MAGIC 
      || u.pt_rev != _BCS_PTRACE_REV)
    {
      /* Too small to be a core file */
      bfd_set_error (bfd_error_wrong_format);
      return 0;
    }

  /* OK, we believe you.  You're a core file (sure, sure).  */

  /* Allocate both the upage and the struct core_data at once, so
     a single free() will free them both.  */
  rawptr = (struct trad_core_struct *)
		bfd_zalloc (abfd, sizeof (struct trad_core_struct));

  if (rawptr == NULL) {
    bfd_set_error (bfd_error_no_memory);
    return 0;
  }
  
  abfd->tdata.trad_core_data = rawptr;

  rawptr->u = u; /*Copy the uarea into the tdata part of the bfd */

  /* Create the sections.  This is raunchy, but bfd_close wants to free
     them separately.  */

  core_stacksec(abfd) = (asection *) bfd_zmalloc (sizeof (asection));
  if (core_stacksec (abfd) == NULL) {
  loser:
    bfd_set_error (bfd_error_no_memory);
    free ((void *)rawptr);
    return 0;
  }
  core_datasec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_datasec (abfd) == NULL) {
  loser1:
    free ((void *)core_stacksec (abfd));
    goto loser;
  }
  core_regsec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_regsec (abfd) == NULL) {
    free ((void *)core_datasec (abfd));
    goto loser1;
  }

  core_stacksec (abfd)->name = ".stack";
  core_datasec (abfd)->name = ".data";
  core_regsec (abfd)->name = ".reg";

  /* FIXME:  Need to worry about shared memory, library data, and library
     text.  I don't think that any of these things are supported on the
     system on which I am developing this for though. */


  core_stacksec (abfd)->flags = SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS;
  core_datasec (abfd)->flags = SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS;
  core_regsec (abfd)->flags = SEC_ALLOC + SEC_HAS_CONTENTS;

  core_datasec (abfd)->_raw_size =  u.pt_dsize;
  core_stacksec (abfd)->_raw_size = u.pt_ssize;
  core_regsec (abfd)->_raw_size = sizeof(u);

  core_datasec (abfd)->vma = u.pt_o_data_start;
  core_stacksec (abfd)->vma = USRSTACK - u.pt_ssize;
  core_regsec (abfd)->vma = 0 - sizeof(u);	/* see trad-core.c */

  core_datasec (abfd)->filepos = (int) u.pt_dataptr;
  core_stacksec (abfd)->filepos = (int) (u.pt_dataptr + u.pt_dsize);
  core_regsec (abfd)->filepos = 0; /* Register segment is ptrace_user */

  /* Align to word at least */
  core_stacksec (abfd)->alignment_power = 2;
  core_datasec (abfd)->alignment_power = 2;
  core_regsec (abfd)->alignment_power = 2;

  abfd->sections = core_stacksec (abfd);
  core_stacksec (abfd)->next = core_datasec (abfd);
  core_datasec (abfd)->next = core_regsec (abfd);
  abfd->section_count = 3;

  return abfd->xvec;
}

char *
ptrace_unix_core_file_failing_command (abfd)
     bfd *abfd;
{
  char *com = abfd->tdata.trad_core_data->u.pt_comm;
  if (*com)
    return com;
  else
    return 0;
}

/* ARGSUSED */
int
ptrace_unix_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return abfd->tdata.trad_core_data->u.pt_sigframe.sig_num;
}

/* ARGSUSED */
boolean
ptrace_unix_core_file_matches_executable_p  (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  /* FIXME: Use pt_timdat field of the ptrace_user structure to match 
     the date of the executable */
  return true;
}

/* No archive file support via this BFD */
#define	ptrace_unix_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	ptrace_unix_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	ptrace_unix_slurp_armap			bfd_false
#define	ptrace_unix_slurp_extended_name_table	bfd_true
#define	ptrace_unix_write_armap			(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	ptrace_unix_truncate_arname		bfd_dont_truncate_arname
#define	aout_32_openr_next_archived_file	bfd_generic_openr_next_archived_file

#define	ptrace_unix_close_and_cleanup		bfd_generic_close_and_cleanup
#define	ptrace_unix_set_section_contents		(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_generic_set_section_contents
#define	ptrace_unix_get_section_contents		bfd_generic_get_section_contents
#define	ptrace_unix_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	ptrace_unix_get_symtab_upper_bound	bfd_0l
#define	ptrace_unix_get_symtab			(long (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0l
#define	ptrace_unix_get_reloc_upper_bound	(long (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0l
#define	ptrace_unix_canonicalize_reloc		(long (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0l
#define	ptrace_unix_make_empty_symbol		(struct symbol_cache_entry * \
	(*) PARAMS ((bfd *))) bfd_false
#define	ptrace_unix_print_symbol			(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	ptrace_unix_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	ptrace_unix_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	ptrace_unix_set_arch_mach			(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	ptrace_unix_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	ptrace_unix_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define ptrace_unix_bfd_debug_info_start		bfd_void
#define ptrace_unix_bfd_debug_info_end		bfd_void
#define ptrace_unix_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define ptrace_unix_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define ptrace_unix_bfd_relax_section		bfd_generic_relax_section
#define ptrace_unix_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define ptrace_unix_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define ptrace_unix_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define ptrace_unix_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define ptrace_unix_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define ptrace_unix_bfd_copy_private_section_data \
  ((boolean (*) PARAMS ((bfd *, asection *, bfd *, asection *))) bfd_false)
#define ptrace_unix_bfd_copy_private_bfd_data \
  ((boolean (*) PARAMS ((bfd *, bfd *))) bfd_false)
#define ptrace_unix_bfd_is_local_label \
  ((boolean (*) PARAMS ((bfd *, bfd *))) bfd_false)

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

bfd_target ptrace_core_vec =
  {
    "trad-core",
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
     ptrace_unix_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(ptrace_unix),
    (PTR) 0			/* backend_data */
};

#endif /* PTRACE_CORE */
