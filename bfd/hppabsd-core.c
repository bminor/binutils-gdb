/* BFD back-end for HPPA BSD core files.
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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Written by the Center for Software Science at the University of Utah
   and by Cygnus Support. 

   The core file structure for the Utah 4.3BSD and OSF1 ports on the
   PA is a mix between traditional cores and hpux cores -- just
   different enough that supporting this format would tend to add
   gross hacks to trad-core.c or hpux-core.c.  So instead we keep any
   gross hacks isolated to this file.  */
   

/* This file can only be compiled on systems which use HPPA-BSD style
   core files.  In the config/XXXXXX.mh file for such a system add
   HDEFINES=-DHPPABSD_CORE
   HDEPFILES=hppabsd-core.o

   I would not expect this to be of use to any other host/target, but
   you never know.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#if defined (HOST_HPPABSD)

#include "machine/vmparam.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>
#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#include <errno.h>

static asection *make_bfd_asection PARAMS ((bfd *, CONST char *,
					    flagword, bfd_size_type,
					    bfd_vma, unsigned int));
static asymbol *hppabsd_core_make_empty_symbol PARAMS ((bfd *));
static bfd_target *hppabsd_core_core_file_p PARAMS ((bfd *));
static char *hppabsd_core_core_file_failing_command PARAMS ((bfd *));
static int hppabsd_core_core_file_failing_signal PARAMS ((bfd *));
static boolean hppabsd_core_core_file_matches_executable_p
  PARAMS ((bfd *, bfd *));
static void swap_abort PARAMS ((void));

/* These are stored in the bfd's tdata.  */

struct hppabsd_core_struct
  {
    int sig;
    char cmd[MAXCOMLEN + 1];
    asection *data_section;
    asection *stack_section;
    asection *reg_section;
  };

#define core_hdr(bfd) ((bfd)->tdata.hppabsd_core_data)
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
hppabsd_core_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  if (new)
    new->the_bfd = abfd;
  return new;
}

static bfd_target *
hppabsd_core_core_file_p (abfd)
     bfd *abfd;
{
  int val;
  struct user u;
  struct hppabsd_core_struct *coredata;
  int clicksz;

  /* Try to read in the u-area.  We will need information from this
     to know how to grok the rest of the core structures.  */
  val = bfd_read ((void *) &u, 1, sizeof u, abfd);
  if (val != sizeof u)
    {
      bfd_error = wrong_format;
      return NULL;
    }

  /* Get the page size out of the u structure.  This will be different
     for PA 1.0 machines and PA 1.1 machines.   Yuk!  */
  clicksz = u.u_pcb.pcb_pgsz;

  /* Sanity checks.  Make sure the size of the core file matches the
     the size computed from information within the core itself.  */
  {
    FILE *stream = bfd_cache_lookup (abfd);
    struct stat statbuf;
    if (stream == NULL || fstat (fileno (stream), &statbuf) < 0)
      {
	bfd_error = system_call_error;
	return NULL;
      }
    if (NBPG * (UPAGES + u.u_dsize + u.u_ssize) > statbuf.st_size)
      {
	bfd_error = file_truncated;
	return NULL;
      }
    if (clicksz * (UPAGES + u.u_dsize + u.u_ssize) < statbuf.st_size)
      {
	/* The file is too big.  Maybe it's not a core file
	   or we otherwise have bad values for u_dsize and u_ssize).  */
	bfd_error = wrong_format;
	return NULL;
      }
  }

  /* OK, we believe you.  You're a core file (sure, sure).  */

  coredata = (struct hppabsd_core_struct *)
    bfd_zalloc (abfd, sizeof (struct hppabsd_core_struct));
  if (!coredata)
    {
      bfd_error = no_memory;
      return NULL;
    }

  /* Make the core data and available via the tdata part of the BFD.  */
  abfd->tdata.hppabsd_core_data = coredata;

  /* Create the sections.  */
  core_stacksec (abfd) = make_bfd_asection (abfd, ".stack",
					   SEC_ALLOC + SEC_HAS_CONTENTS,
					   clicksz * u.u_ssize,
					   NBPG * (USIZE + KSTAKSIZE) 
					     + clicksz * u.u_dsize, 2);
  core_stacksec (abfd)->vma = USRSTACK; 

  core_datasec (abfd) = make_bfd_asection (abfd, ".data",
					  SEC_ALLOC + SEC_LOAD
					    + SEC_HAS_CONTENTS,
					  clicksz * u.u_dsize,
					  NBPG * (USIZE + KSTAKSIZE), 2);
  core_datasec (abfd)->vma = UDATASEG;

  core_regsec (abfd) = make_bfd_asection (abfd, ".reg",
					 SEC_ALLOC + SEC_HAS_CONTENTS,
					 KSTAKSIZE * NBPG,
					 NBPG * USIZE, 2);
  core_regsec (abfd)->vma = 0;

  strncpy (core_command (abfd), u.u_comm, MAXCOMLEN + 1);
  core_signal (abfd) = u.u_code;
  return abfd->xvec;
}

static char *
hppabsd_core_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

/* ARGSUSED */
static int
hppabsd_core_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

/* ARGSUSED */
static boolean
hppabsd_core_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  /* There's no way to know this...  */
  return true;
}


/* No archive file support via this BFD */
#define	hppabsd_core_openr_next_archived_file \
	bfd_generic_openr_next_archived_file
#define	hppabsd_core_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	hppabsd_core_slurp_armap		bfd_false
#define	hppabsd_core_slurp_extended_name_table	bfd_true
#define	hppabsd_core_write_armap		(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	hppabsd_core_truncate_arname		bfd_dont_truncate_arname

#define	hppabsd_core_close_and_cleanup		bfd_generic_close_and_cleanup
#define	hppabsd_core_set_section_contents	(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_generic_set_section_contents
#define	hppabsd_core_get_section_contents \
	bfd_generic_get_section_contents
#define	hppabsd_core_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	hppabsd_core_get_symtab_upper_bound	bfd_0u
#define	hppabsd_core_get_symtab			(unsigned int (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0u
#define	hppabsd_core_get_reloc_upper_bound	(unsigned int (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0u
#define	hppabsd_core_canonicalize_reloc		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0u
#define	hppabsd_core_print_symbol		(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	hppabsd_core_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	hppabsd_core_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	hppabsd_core_set_arch_mach		(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	hppabsd_core_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	hppabsd_core_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define hppabsd_core_bfd_debug_info_start	bfd_void
#define hppabsd_core_bfd_debug_info_end		bfd_void
#define hppabsd_core_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define hppabsd_core_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define hppabsd_core_bfd_relax_section		bfd_generic_relax_section
#define hppabsd_core_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define hppabsd_core_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define hppabsd_core_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define hppabsd_core_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define hppabsd_core_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)

/* If somebody calls any byte-swapping routines, shoot them.  */
static void
swap_abort ()
{
  /* This way doesn't require any declaration for ANSI to fuck up.  */
  abort ();	
}

#define	NO_GET	((bfd_vma (*) PARAMS ((   const bfd_byte *))) swap_abort )
#define	NO_PUT	((void    (*) PARAMS ((bfd_vma, bfd_byte *))) swap_abort )
#define	NO_SIGNED_GET \
  ((bfd_signed_vma (*) PARAMS ((const bfd_byte *))) swap_abort )

bfd_target hppabsd_core_vec =
  {
    "hppabsd-core",
    bfd_target_unknown_flavour,
    true,			/* target byte order */
    true,			/* target headers byte order */
    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
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
     hppabsd_core_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(hppabsd_core),
    (PTR) 0			/* backend_data */
};
#endif
