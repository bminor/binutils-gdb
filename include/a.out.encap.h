/* Another try at encapsulating bsd object files in coff.
   Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
   Written by Pace Willisson 12/9/88

   This file is obsolete.  It needs to be converted to just define a bunch
   of stuff that BFD can use to do coff-encapsulated files.  --gnu@cygnus.com

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

/*
 * This time, we will only use the coff headers to tell the kernel
 * how to exec the file.  Therefore, the only fields that need to 
 * be filled in are the scnptr and vaddr for the text and data
 * sections, and the vaddr for the bss.  As far as coff is concerned,
 * there is no symbol table, relocation, or line numbers.
 *
 * A normal bsd header (struct exec) is placed after the coff headers,
 * and before the real text.  I defined a the new fields 'a_machtype'
 * and a_flags.  If a_machtype is M_386, and a_flags & A_ENCAP is
 * true, then the bsd header is preceeded by a coff header.  Macros
 * like N_TXTOFF and N_TXTADDR use this field to find the bsd header.
 * 
 * The only problem is to track down the bsd exec header.  The
 * macros HEADER_OFFSET, etc do this.  Look at nm.c, dis.c, etc
 * for examples.
 */
#ifndef A_OUT_ENCAP_H_SEEN
#define A_OUT_ENCAP_H_SEEN

#include "a.out.gnu.h"

#define N_FLAGS_COFF_ENCAPSULATE 0x20 /* coff header precedes bsd header */

/* Describe the COFF header used for encapsulation.  */

struct coffheader
{
  /* filehdr */
  unsigned short f_magic;
  unsigned short f_nscns;
  long f_timdat;
  long f_symptr;
  long f_nsyms;
  unsigned short f_opthdr;
  unsigned short f_flags;
  /* aouthdr */
  short magic;
  short vstamp;
  long tsize;
  long dsize;
  long bsize;
  long entry;
  long text_start;
  long data_start;
  struct coffscn
    {
      char s_name[8];
      long s_paddr;
      long s_vaddr;
      long s_size;
      long s_scnptr;
      long s_relptr;
      long s_lnnoptr;
      unsigned short s_nreloc;
      unsigned short s_nlnno;
      long s_flags;
    } scns[3];		/* text, data, bss */
};

/* Describe some of the parameters of the encapsulation,
   including how to find the encapsulated BSD header.  */

#if TARGET == TARGET_I386
#define COFF_MAGIC 0514 /* I386MAGIC */
#endif
#if TARGET == TARGET_M68K
#define COFF_MAGIC 0520 /* MC68MAGIC */
#endif
#if TARGET == TARGET_SPARC
#define	COFF_MAGIC UNKNOWN!!!	/* Used by TTI */
#endif
#if TARGET == TARGET_AM29K
#define	COFF_MAGIC 0x17A	/* Used by asm29k cross-tools */
#endif

#ifdef COFF_MAGIC
short __header_offset_temp;

/* FIXME, this is dumb.  The same tools can't handle a.outs for different
   architectures, just because COFF_MAGIC is different; so you need a
   separate GNU nm for every architecture!!?  Also note that for
   expediency, this macros accepts COFF_MAGIC in either byte order.
   The right thing to do is to call read_aout_header to handle all this.  */
   
#define HEADER_OFFSET(f) \
	(__header_offset_temp = 0, \
	 fread ((char *)&__header_offset_temp, sizeof (short), 1, (f)), \
	 fseek ((f), -sizeof (short), 1), \
	 (__header_offset_temp==COFF_MAGIC || __header_offset_temp == \
	    ((COFF_MAGIC >> 8)|((COFF_MAGIC&0xFF)<<8)) \
		    ? sizeof(struct coffheader) : 0))

#define HEADER_OFFSET_FD(fd) \
	(__header_offset_temp = 0, \
	 read (fd, (char *)&__header_offset_temp, sizeof (short)), \
	 lseek ((fd), -sizeof (short), 1), \
	 (__header_offset_temp==COFF_MAGIC || __header_offset_temp == \
	    ((COFF_MAGIC >> 8)|((COFF_MAGIC&0xFF)<<8)) \
		    ? sizeof(struct coffheader) : 0))


#else
#define HEADER_OFFSET(f) 0
#define HEADER_OFFSET_FD(fd) 0
#endif

#define HEADER_SEEK(f) (fseek ((f), HEADER_OFFSET((f)), 1))
#define HEADER_SEEK_FD(fd) (lseek ((fd), HEADER_OFFSET_FD((fd)), 1))


/* Describe the characteristics of the BSD header
   that appears inside the encapsulation.  */

#undef _N_HDROFF
#undef N_TXTADDR
#undef N_DATADDR

/* Encapsulated coff files that are linked ZMAGIC have a text segment
   offset just past the header (and a matching TXTADDR), excluding
   the headers from the text segment proper but keeping the physical
   layout and the virtual memory layout page-aligned.

   Non-encapsulated a.out files that are linked ZMAGIC have a text
   segment that starts at 0 and an N_TXTADR similarly offset to 0.
   They too are page-aligned with each other, but they include the
   a.out header as part of the text. 

   The _N_HDROFF gets sizeof struct exec added to it, so we have
   to compensate here.  See <a.out.gnu.h>.  */

#define _N_HDROFF(x) ((N_FLAGS(x) & N_FLAGS_COFF_ENCAPSULATE) ? \
		      sizeof (struct coffheader) : -sizeof (struct exec))

/* Address of text segment in memory after it is loaded.  */
#define N_TXTADDR(x) \
	(TEXT_START_ADDR + \
	((N_FLAGS(x) & N_FLAGS_COFF_ENCAPSULATE) ? \
	 sizeof (struct coffheader) + sizeof (struct exec) : 0))

/* I have no idea what this is doing here.  -- gnu@toad.com  20Mar90
   Perhaps it is to give a size that is acceptable to any machine?  */
#undef SEGMENT_SIZE
#define SEGMENT_SIZE 0x400000

#define N_DATADDR(x) \
	((N_FLAGS(x) & N_FLAGS_COFF_ENCAPSULATE) ? \
	 (SEGMENT_SIZE + ((N_TXTADDR(x)+(x).a_text-1) & ~(SEGMENT_SIZE-1))) : \
	 (N_TXTADDR(x)+(x).a_text))

#endif /* A_OUT_ENCAP_H_SEEN */
