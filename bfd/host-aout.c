/* BFD backend for local host's a.out binaries
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Cygnus Support.  Probably John Gilmore's fault.

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

#include <ansidecl.h>
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"

#include <a.out.h>
#include "libaout.h"           /* BFD a.out internal data structures */

#include "trad-core.h"		/* Traditional Unix core files */

/*======== This next section is stolen from ../include/a.out.gnu.h
  ======== for all the losing Unix systems that don't provide these
  ======== macros.  

  When porting to a new system, you must supply:

	HOST_PAGE_SIZE
	HOST_SEGMENT_SIZE
	HOST_MACHINE_ARCH	(optional)
	HOST_MACHINE_MACHINE	(optional)
	HOST_TEXT_START_ADDR
	HOST_STACK_END_ADDR

  in the ../include/h-systemname.h file.  */

#define	PAGE_SIZE	HOST_PAGE_SIZE
#define	SEGMENT_SIZE	HOST_SEGMENT_SIZE
#define	TEXT_START_ADDR	HOST_TEXT_START_ADDR
#define	STACK_END_ADDR	HOST_STACK_END_ADDR

/*======== Stolen section begins below.  =================================*/

#define	a_info	a_magic		/* Old traditional Unix */

#define N_MAGIC(exec) ((exec).a_info & 0xffff)
#define N_SET_MAGIC(exec, magic) \
	((exec).a_info = (((exec).a_info & 0xffff0000) | ((magic) & 0xffff)))

/* Virtual Address of text segment from the a.out file.  For OMAGIC,
   (almost always "unlinked .o's" these days), should be zero.
   For linked files, should reflect reality if we know it.  */

#ifndef N_TXTADDR
#define N_TXTADDR(x)	(N_MAGIC(x)==OMAGIC? 0 : TEXT_START_ADDR)
#endif

#ifndef N_BADMAG
#define N_BADMAG(x)	  (N_MAGIC(x) != OMAGIC		\
			&& N_MAGIC(x) != NMAGIC		\
  			&& N_MAGIC(x) != ZMAGIC)
#endif

/* This complexity is for encapsulated COFF support */
#ifndef _N_HDROFF
#define _N_HDROFF(x)	(SEGMENT_SIZE - sizeof (struct exec))
#endif

#ifndef N_TXTOFF
#define N_TXTOFF(x)	(N_MAGIC(x) == ZMAGIC ?	\
				_N_HDROFF((x)) + sizeof (struct exec) :	\
				sizeof (struct exec))
#endif


#ifndef N_DATOFF
#define N_DATOFF(x)	( N_TXTOFF(x) + (x).a_text )
#endif

#ifndef N_TRELOFF
#define N_TRELOFF(x)	( N_DATOFF(x) + (x).a_data )
#endif

#ifndef N_DRELOFF
#define N_DRELOFF(x)	( N_TRELOFF(x) + (x).a_trsize )
#endif

#ifndef N_SYMOFF
#define N_SYMOFF(x)	( N_DRELOFF(x) + (x).a_drsize )
#endif

#ifndef N_STROFF
#define N_STROFF(x)	( N_SYMOFF(x) + (x).a_syms )
#endif

/* Address of text segment in memory after it is loaded.  */
#ifndef N_TXTADDR
#define	N_TXTADDR(x)	0
#endif

#ifndef N_DATADDR
#define N_DATADDR(x) \
    (N_MAGIC(x)==OMAGIC? (N_TXTADDR(x)+(x).a_text) \
     :  (SEGMENT_SIZE + ((N_TXTADDR(x)+(x).a_text-1) & ~(SEGMENT_SIZE-1))))
#endif

/* Address of bss segment in memory after it is loaded.  */
#ifndef N_BSSADDR
#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)
#endif


static bfd_target *NAME(host_aout,callback) ();

/*SUPPRESS558*/
/*SUPPRESS529*/

bfd_target *
DEFUN(NAME(host_aout,object_p), (abfd),
     bfd *abfd)
{
  unsigned char magicbuf[4];	/* Raw bytes of magic number from file */
  unsigned long magic;		/* Swapped magic number */

  bfd_error = system_call_error;

  if (bfd_read ((PTR)magicbuf, 1, sizeof (magicbuf), abfd) !=
      sizeof (magicbuf))
    return 0;
  magic = bfd_h_get_32 (abfd, magicbuf);

  if (N_BADMAG (*((struct exec *) &magic))) return 0;

  return NAME(aout,some_aout_object_p) (abfd, NAME(host_aout,callback));
}

/* Set parameters about this a.out file that are machine-dependent.
   This routine is called from NAME(some_aout_object_p) just before it returns.
   */

static bfd_target *
DEFUN(NAME(host_aout,callback), (abfd),
     bfd *abfd)
{
  /* exec_hdr (abfd), a "struct internal_exec *", is just an abstraction,
     as far as the BFD a.out layer cares.  We use it as a "struct exec *".
     This routine moves any data from the exec header,
     which is needed by the BFD code, out to places known to BFD.  This
     allows the rest of the BFD code to not know or care about the structure
     of exec_hdr (abfd).  */
  struct exec *execp = (struct exec *)exec_hdr (abfd);

  /* The virtual memory addresses of the sections */
  obj_datasec (abfd)->vma = N_DATADDR(*execp);
  obj_bsssec (abfd)->vma = N_BSSADDR(*execp);
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);

  /* The file offsets of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF(*execp);
  obj_datasec (abfd)->filepos = N_DATOFF(*execp);

  /* The file offsets of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TRELOFF(*execp);
  obj_datasec (abfd)->rel_filepos = N_DRELOFF(*execp);

  /* The file offsets of the string table and symbol table.  */
  obj_str_filepos (abfd) = N_STROFF (*execp);
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);

#ifdef HOST_MACHINE_ARCH
  abfd->obj_arch = HOST_MACHINE_ARCH;
#endif
#ifdef HOST_MACHINE_MACHINE
  abfd->obj_machine = HOST_MACHINE_MACHINE;
#endif

  obj_reloc_entry_size (abfd) = sizeof (struct relocation_info);
  return abfd->xvec;
}


boolean
DEFUN(NAME(host_aout,mkobject), (abfd),
     bfd *abfd)
{
  /* This struct is just for allocating two things with one zalloc, so
     they will be freed together, without violating alignment constraints. */
  struct aout_exec {
	struct aoutdata	aoutdata;
	struct exec	exec;
  } *rawptr;

  bfd_error = system_call_error;

  /* Use an intermediate variable for clarity */
  rawptr = (struct aout_exec *)bfd_zalloc (abfd, sizeof (struct aout_exec));

  if (rawptr == NULL) {
    bfd_error = no_memory;
    return false;
  }

  set_tdata (abfd, &rawptr->aoutdata);
  /* exec_hdr (abfd), a "struct internal_exec *", is just an abstraction,
     as far as the BFD a.out layer cares.  We use it as a "struct exec *".  */
  exec_hdr (abfd) = (struct internal_exec *) &rawptr->exec;

  /* For simplicity's sake we just make all the sections right here. */

  obj_textsec (abfd) = (asection *)NULL;
  obj_datasec (abfd) = (asection *)NULL;
  obj_bsssec (abfd) = (asection *)NULL;
  bfd_make_section (abfd, ".text");
  bfd_make_section (abfd, ".data");
  bfd_make_section (abfd, ".bss");

  return true;
}

/* Write an object file in host a.out format.
   Section contents have already been written.  We write the
   file header, symbols, and relocation.  */

boolean
DEFUN(NAME(host_aout,write_object_contents), (abfd),
     bfd *abfd)
{
/* This works because we are on the host system */
#define	EXEC_BYTES_SIZE		(sizeof (struct exec))
#define	EXTERNAL_LIST_SIZE	(sizeof (struct nlist))
  size_t data_pad = 0;
  unsigned char exec_bytes[EXEC_BYTES_SIZE];
  struct exec *execp = (struct exec *)exec_hdr (abfd);

  execp->a_text = obj_textsec (abfd)->size;

  WRITE_HEADERS (abfd, execp);
  return true;
}

/* We use BFD generic archive files.  */
#define	aout_32_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_32_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	aout_32_slurp_armap			bfd_slurp_bsd_armap
#define	aout_32_slurp_extended_name_table	bfd_true
#define	aout_32_write_armap			bsd_write_armap
#define	aout_32_truncate_arname			bfd_bsd_truncate_arname
/* #define aout_32_machine_type 			sunos_machine_type */

/* Traditional Unix core files with upage */
#define	aout_32_core_file_failing_command 	trad_unix_core_file_failing_command
#define	aout_32_core_file_failing_signal	trad_unix_core_file_failing_signal
#define	aout_32_core_file_matches_executable_p	trad_unix_core_file_matches_executable_p


#define	aout_64_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_64_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	aout_64_slurp_armap			bfd_slurp_bsd_armap
#define	aout_64_slurp_extended_name_table	bfd_true
#define	aout_64_write_armap			bsd_write_armap
#define	aout_64_truncate_arname			bfd_bsd_truncate_arname
/* #define aout_64_machine_type 			sunos_machine_type */

#define	aout_64_core_file_failing_command 	trad_unix_core_file_failing_command
#define	aout_64_core_file_failing_signal	trad_unix_core_file_failing_signal
#define	aout_64_core_file_matches_executable_p	trad_unix_core_file_matches_executable_p

#define aout_64_bfd_debug_info_start		bfd_void
#define aout_64_bfd_debug_info_end		bfd_void
#define aout_64_bfd_debug_info_accumulate	bfd_void

#define aout_32_bfd_debug_info_start		bfd_void
#define aout_32_bfd_debug_info_end		bfd_void
#define aout_32_bfd_debug_info_accumulate	(PROTO(void,(*),(bfd*, struct sec *))) bfd_void


/* We implement these routines ourselves, rather than using the generic
   a.out versions.  */
#define	aout_write_object_contents	host_write_object_contents

bfd_target host_aout_big_vec =
  {
    "a.out-host-big",
    bfd_target_aout_flavour_enum,
    true,			/* target byte order */
    true,			/* target headers byte order */
    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
    (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
    ' ',						   /* ar_pad_char */
    16,							   /* ar_max_namelen */
    3,							   /* minimum alignment power */
    _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
    _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
    
      {_bfd_dummy_target, NAME(host_aout,object_p),
       bfd_generic_archive_p, trad_unix_core_file_p},
      {bfd_false, NAME(host_aout,mkobject),
       _bfd_generic_mkarchive, bfd_false},
      {bfd_false, NAME(host_aout,write_object_contents), /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},
    
    JUMP_TABLE(JNAME(aout))
};

bfd_target host_aout_little_vec =
  {
    "a.out-host-little",
    bfd_target_aout_flavour_enum,
    false,			/* target byte order */
    false,			/* target headers byte order */
    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
    (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
    ' ',						   /* ar_pad_char */
    16,							   /* ar_max_namelen */
    3,							   /* minimum alignment power */
    _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
    _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
    
      {_bfd_dummy_target, NAME(host_aout,object_p),
       bfd_generic_archive_p, trad_unix_core_file_p},
      {bfd_false, NAME(host_aout,mkobject),
       _bfd_generic_mkarchive, bfd_false},
      {bfd_false, NAME(host_aout,write_object_contents), /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},
    
    JUMP_TABLE(JNAME(aout))
};
