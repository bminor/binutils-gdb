/* IBM RS/6000 "XCOFF" back-end for BFD.
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.
   Written by Metin G. Ozisik, Mimi Phûông-Thåo Võ, and John Gilmore.
   Archive support from Damon A. Permezel.
   Contributed by IBM Corporation and Cygnus Support.

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

/* This port currently only handles reading object files -- no archive
   support, no core files, and no writing.  FIXME.  */

/* Internalcoff.h and coffcode.h modify themselves based on this flag.  */
#define RS6000COFF_C 1

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "internalcoff.h"
#include "coff-rs6000.h"
#include "libcoff.h"

/* The main body of code is in coffcode.h.  */
#include "coffcode.h"


#if 0
/* These are not yet ready for prime time.  */
#define coff_core_file_matches_executable_p  \
				     rs6000coff_core_file_matches_executable_p
#define	coff_get_section_contents	rs6000coff_get_section_contents
#define coff_openr_next_archived_file	rs6000coff_openr_next_archived_file
#define coff_write_armap		rs6000coff_write_armap
#define	coff_stat_arch_elt		rs6000coff_stat_arch_elt
#define	coff_snarf_ar_hdr		rs6000coff_snarf_ar_hdr
#define	coff_mkarchive			rs6000coff_mkarchive

static bfd_target *rs6000coff_archive_p ();
static bfd_target *rs6000coff_core_p ();
static bfd_target *rs6000coff_object_p ();
static bfd_target *rs6000coff_real_object_p ();
#endif

bfd_target rs6000coff_vec =
{
  "coff-rs6000",		/* name */
  bfd_target_coff_flavour,	
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen??? FIXMEmgo */
  3,				/* default alignment power */

  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, 	/* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(coff),
  COFF_SWAP_TABLE

};


#if 0	/* we don't have include file for this yet */
/* ------------------------------------------------------------------------ */
/*	Support for archive file stuff..				    */
/*	Stolen from Damon A. Parmazel's `bfd' portation.		    */
/* ------------------------------------------------------------------------ */

#include "/usr/include/ar.h"		/* <ar.h> doesn't do it.	*/


#define arch_hdr(bfd) 		\
	((struct ar_hdr *)	\
	 (((struct areltdata *)((bfd)->arelt_data))->arch_header))


static boolean
rs6000coff_mkarchive (abfd)
     bfd *abfd;
{
	bfd_error = invalid_operation;	/* write not supported	*/
}


/* This functions reads an arch header and returns an areltdata pointer, or
   NULL on error.

   Presumes the file pointer is already in the right place (ie pointing
   to the ar_hdr in the file).   Moves the file pointer; on success it
   should be pointing to the front of the file contents; on failure it
   could have been moved arbitrarily.
*/

struct areltdata *
rs6000coff_snarf_ar_hdr (abfd)
     bfd *abfd;
{
	extern int errno;

	struct {
		struct ar_hdr hdr;
		char namebuf[256];
	} h;
	int size;
	struct areltdata *ared;
	unsigned int namelen = 0;
	char *allocptr;

	size = sizeof (h.hdr);
	if (bfd_read(&h.hdr, 1, size, abfd) != size) {
		bfd_error = no_more_archived_files;
		return NULL;
	}
	size  = atoi(h.hdr.ar_namlen);	/* ar_name[] length	*/
	size += size & 1;

	if (bfd_read(&h.hdr._ar_name.ar_name[2], 1, size, abfd) != size) {
		bfd_error = no_more_archived_files;
		return NULL;
	}

	if (strncmp(h.hdr._ar_name.ar_fmag + size, AIAFMAG, 2)) {
		bfd_error = malformed_archive;
		return NULL;
	}

	h.hdr._ar_name.ar_name[size] = 0;	/* terminate filename	*/

	/*
	 * if the filename is NULL, we're (probably) at the end.
	 */
	if (size == 0) {
		bfd_error = no_more_archived_files;
		return NULL;
	}

	size += sizeof (h.hdr);
	allocptr = bfd_zalloc(abfd, sizeof (*ared) + size);

	if (allocptr == NULL) {
		bfd_error = no_memory;
		return NULL;
	}

	ared = (struct areltdata *) allocptr;

	ared->arch_header = (void *) (allocptr + sizeof (struct areltdata));
	memcpy ((char *) ared->arch_header, &h.hdr, size);
	ared->parsed_size = atoi(h.hdr.ar_size);
	ared->filename    = ((AR_HDR*) ared->arch_header)->_ar_name.ar_name;

	return ared;
}

/*
 * xcoff_openr_next_archived_file -	xcoff has nxt/prv seek addrs.
 */
static bfd *
rs6000coff_openr_next_archived_file(archive, last_file)
  bfd *archive, *last_file; 
{
	file_ptr filestart;

	if (!last_file)
		filestart = bfd_ardata(archive)->first_file_filepos;
	else
		filestart = atol(arch_hdr(last_file)->ar_nxtmem);

	return get_elt_at_filepos (archive, filestart);
}


static bfd_target *
rs6000coff_archive_p (abfd)
     bfd *abfd;
{
	struct fl_hdr hdr;
	register struct artdata *art;

	if (bfd_read (&hdr, 1, sizeof (hdr), abfd) != sizeof (hdr)) {
		bfd_error = wrong_format;
		return 0;
	}

	if (strncmp(hdr.fl_magic, AIAMAG, SAIAMAG)) {
		bfd_error = wrong_format;
		return 0;
	}

	/*
	 * bfd_ardata() accesses the bfd->tdata field.
	 */
	abfd->tdata = (void *) bfd_zalloc(abfd, sizeof (*art) + sizeof (hdr));
	if ((art = bfd_ardata (abfd)) == NULL) {
		bfd_error = no_memory;
		return 0;
	}

	art->first_file_filepos = atoi(hdr.fl_fstmoff);
	*(struct fl_hdr *) (1 + art) = hdr;

	/*
	 * slurp in the member table, which I thing is the armap equivalent.
	xcoff_slurp_armap(abfd);
	 */
  
	if (abfd->obj_arch == bfd_arch_unknown)	/* FIXME!!!	*/
		abfd->obj_arch = bfd_arch_rs6000;

	return abfd->xvec;
}


static int
rs6000coff_stat_arch_elt(abfd, buf)
  bfd *abfd;
  struct stat *buf;
{
	struct ar_hdr *hdr;
	char *aloser;
  
	if (abfd->arelt_data == NULL) {
		bfd_error = invalid_operation;
		return -1;
	}
    
	hdr = arch_hdr (abfd);

#define foo(arelt, stelt, size)  \
	buf->stelt = strtol (hdr->arelt, &aloser, size); \
		if (aloser == hdr->arelt) return -1;
  
	foo (ar_date, st_mtime, 10);
	foo (ar_uid, st_uid, 10);
	foo (ar_gid, st_gid, 10);
	foo (ar_mode, st_mode, 8);
	foo (ar_size, st_size, 10);

	return 0;
}

static boolean
rs6000coff_write_armap (arch, elength, map, orl_count, stridx)
  bfd *arch;
  unsigned int elength;
  struct orl *map; 
{
	bfd_error = invalid_operation;
	return false;
}


#endif	/* if 0 */

#if 0	/* not sure if this will work on all hosts yet! */
#ifdef AOUTHDR
#undef AOUTHDR
#endif

/* ------------------------------------------------------------------------ */
/*	Support for core file stuff.. 					    */
/* ------------------------------------------------------------------------ */

#include <sys/user.h>
#include <sys/ldr.h>
#include <sys/core.h>


/* Number of special purpose registers supported bygdb. This value should match
   `tm.h' in gdb directory. Clean this mess up and use the macros in sys/reg.h.
   FIXMEmgo. */

#define	NUM_OF_SPEC_REGS  7
#define	STACK_END_ADDR 0x2ff80000

#define	core_hdr(bfd)		(((Rs6kCorData*)(bfd->tdata))->hdr)
#define	core_datasec(bfd)	(((Rs6kCorData*)(bfd->tdata))->data_section)
#define	core_stacksec(bfd)	(((Rs6kCorData*)(bfd->tdata))->stack_section)
#define	core_regsec(bfd)	(((Rs6kCorData*)(bfd->tdata))->reg_section)
#define	core_reg2sec(bfd)	(((Rs6kCorData*)(bfd->tdata))->reg2_section)

/* These are stored in the bfd's tdata */
typedef struct {
  struct core *hdr;		/* core file header */
  asection *data_section,
  	   *stack_section,
	   *reg_section,	/* section for GPRs and special registers. */
	   *reg2_section;	/* section for FPRs. */
} Rs6kCorData;


/* Decide if a given bfd represents a `core' file or not. There really is no
   magic number or anything like, in rs6000coff. */

static bfd_target *
rs6000coff_core_p (abfd)
     bfd *abfd;
{
  int fd;
  struct core_dump coredata;
  struct stat statbuf;
  char *tmpptr;

  /* Use bfd_xxx routines, rather than O/S primitives to read coredata. FIXMEmgo */
  fd = open (abfd->filename, O_RDONLY);

  fstat (fd, &statbuf);
  read (fd, &coredata, sizeof (struct core_dump));

  close (fd);

  if (coredata.c_tab < (sizeof (coredata.c_u) + (int)&coredata.c_u - (int)&coredata.c_signo) ||
      coredata.c_tab >= statbuf.st_size ||
      (long)coredata.c_stack <= (long)coredata.c_tab ) {
    return NULL;
  }

/*
  If it looks like core file, then.....
  read core file header..... (maybe you've done it above..)
*/

  /* maybe you should alloc space for the whole core chunk over here!! FIXMEmgo */
  tmpptr = (char*)bfd_zalloc (abfd, sizeof (Rs6kCorData));
  set_tdata (abfd, tmpptr);

  /* .stack section. */
  if ((core_stacksec (abfd) = (asection*) bfd_zalloc (abfd, sizeof (asection)))
       == NULL)  {
    bfd_error = no_memory;
    /* bfd_release (abfd, ???? ) */
    return NULL;
  }
  core_stacksec (abfd)->name = ".stack";
  core_stacksec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_stacksec (abfd)->size = coredata.c_size;
  core_stacksec (abfd)->vma = STACK_END_ADDR - coredata.c_size;
  core_stacksec (abfd)->filepos = coredata.c_stack;	/*???? */

  /* .reg section for GPRs and special registers. */
  if ((core_regsec (abfd) = (asection*) bfd_zalloc (abfd, sizeof (asection)))
       == NULL)  {
    bfd_error = no_memory;
    /* bfd_release (abfd, ???? ) */
    return NULL;
  }
  core_regsec (abfd)->name = ".reg";
  core_regsec (abfd)->flags = SEC_ALLOC;
  core_regsec (abfd)->size = (32 + NUM_OF_SPEC_REGS) * 4;
  core_regsec (abfd)->vma = NULL;			/* not used?? */
  core_regsec (abfd)->filepos = 
  	(char*)&coredata.c_u.u_save - (char*)&coredata;

  /* .reg2 section for FPRs (floating point registers). */
  if ((core_reg2sec (abfd) = (asection*) bfd_zalloc (abfd, sizeof (asection)))
       == NULL)  {
    bfd_error = no_memory;
    /* bfd_release (abfd, ???? ) */
    return NULL;
  }
  core_reg2sec (abfd)->name = ".reg2";
  core_reg2sec (abfd)->flags = SEC_ALLOC;
  core_reg2sec (abfd)->size = 8 * 32;			/* 32 FPRs. */
  core_reg2sec (abfd)->vma = NULL;			/* not used?? */
  core_reg2sec (abfd)->filepos = 
  	(char*)&coredata.c_u.u_save.fpr[0] - (char*)&coredata;

  /* set up section chain here. */
  abfd->section_count = 3;
  abfd->sections = core_stacksec (abfd);
  core_stacksec (abfd)->next = core_regsec(abfd);
  core_regsec (abfd)->next = core_reg2sec (abfd);
  core_reg2sec (abfd)->next = NULL;

  return abfd->xvec;				/* this is garbage for now. */
}



/* return `true' if given core is from the given executable.. */
static boolean
rs6000coff_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd;
     bfd *exec_bfd;
{
  FILE *fd;
  struct core_dump coredata;
  struct ld_info ldinfo;
  char pathname [1024];
  char *str1, *str2;

  /* Use bfd_xxx routines, rather than O/S primitives, do error checking!!
  								FIXMEmgo */
  fd = fopen (core_bfd->filename, "r");

  fread (&coredata, sizeof (struct core_dump), 1, fd);
  fseek (fd, (long)coredata.c_tab, 0);
  fread (&ldinfo, (char*)&ldinfo.ldinfo_filename[0] - (char*)&ldinfo.ldinfo_next,
	 1, fd);
  fscanf (fd, "%s", pathname);
  printf ("path: %s\n", pathname);
  
  str1 = strrchr (pathname, '/');
  str2 = strrchr (exec_bfd->filename, '/');

  /* step over character '/' */
  str1 = str1 ? str1+1 : &pathname[0];
  str2 = str2 ? str2+1 : exec_bfd->filename;

  fclose (fd);
  return strcmp (str1, str2);
}


static boolean
rs6000coff_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     int count;
{
    if (count == 0)
	return true;

    /* Reading a core file's sections will be slightly different. For the
       rest of them we can use bfd_generic_get_section_contents () I suppose. */
    /* Make sure this routine works for any bfd and any section. FIXMEmgo. */

    if (abfd->format == bfd_core && strcmp (section->name, ".reg") == 0) {

      struct mstsave mstatus;
      int    regoffset = (char*)&mstatus.gpr[0] - (char*)&mstatus;

      /* Assert that the only way this code will be executed is reading the
         whole section. */
      if (offset || count != (sizeof(mstatus.gpr) + (4 * NUM_OF_SPEC_REGS)))
        printf ("ERROR! in rs6000coff_get_section_contents()\n");

      /* for `.reg' section, `filepos' is a pointer to the `mstsave' structure
         in the core file. */

      /* read GPR's into the location. */
      if ( bfd_seek(abfd, section->filepos + regoffset, SEEK_SET) == -1
	|| bfd_read(location, 1, sizeof (mstatus.gpr), abfd) != sizeof (mstatus.gpr))
	return (false); /* on error */

      /* increment location to the beginning of special registers in the section,
         reset register offset value to the beginning of first special register
	 in mstsave structure, and read special registers. */

      location = (PTR) ((char*)location + sizeof (mstatus.gpr));
      regoffset = (char*)&mstatus.iar - (char*)&mstatus;

      if ( bfd_seek(abfd, section->filepos + regoffset, SEEK_SET) == -1
	|| bfd_read(location, 1, 4 * NUM_OF_SPEC_REGS, abfd) != 
							4 * NUM_OF_SPEC_REGS)
	return (false); /* on error */
      
      /* increment location address, and read the special registers.. */
      /* FIXMEmgo */
      return (true);
    }

    /* else, use default bfd section content transfer. */
    else
      return bfd_generic_get_section_contents 
      			(abfd, section, location, offset, count);
}

#endif /* if 0 - for CORE */
