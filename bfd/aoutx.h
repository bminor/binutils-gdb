/* BFD semi-generic back-end for a.out binaries.
   Copyright 1990, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
   Written by Cygnus Support.

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

/*
SECTION
	a.out backends


DESCRIPTION

	BFD supports a number of different flavours of a.out format,
	though the major differences are only the sizes of the
	structures on disk, and the shape of the relocation
	information.

	The support is split into a basic support file @file{aoutx.h}
	and other files which derive functions from the base. One
	derivation file is @file{aoutf1.h} (for a.out flavour 1), and
	adds to the basic a.out functions support for sun3, sun4, 386
	and 29k a.out files, to create a target jump vector for a
	specific target.

	This information is further split out into more specific files
	for each machine, including @file{sunos.c} for sun3 and sun4,
	@file{newsos3.c} for the Sony NEWS, and @file{demo64.c} for a
	demonstration of a 64 bit a.out format.

	The base file @file{aoutx.h} defines general mechanisms for
	reading and writing records to and from disk and various
	other methods which BFD requires. It is included by
	@file{aout32.c} and @file{aout64.c} to form the names
	<<aout_32_swap_exec_header_in>>, <<aout_64_swap_exec_header_in>>, etc.

	As an example, this is what goes on to make the back end for a
	sun4, from @file{aout32.c}:

|	#define ARCH_SIZE 32
|	#include "aoutx.h"

	Which exports names:

|	...
|	aout_32_canonicalize_reloc
|	aout_32_find_nearest_line
|	aout_32_get_lineno
|	aout_32_get_reloc_upper_bound
|	...

	from @file{sunos.c}:

|	#define ARCH 32
|	#define TARGET_NAME "a.out-sunos-big"
|	#define VECNAME    sunos_big_vec
|	#include "aoutf1.h"

	requires all the names from @file{aout32.c}, and produces the jump vector

|	sunos_big_vec

	The file @file{host-aout.c} is a special case.  It is for a large set
	of hosts that use ``more or less standard'' a.out files, and
	for which cross-debugging is not interesting.  It uses the
	standard 32-bit a.out support routines, but determines the
	file offsets and addresses of the text, data, and BSS
	sections, the machine architecture and machine type, and the
	entry point address, in a host-dependent manner.  Once these
	values have been determined, generic code is used to handle
	the  object file.

	When porting it to run on a new system, you must supply:

|        HOST_PAGE_SIZE
|        HOST_SEGMENT_SIZE
|        HOST_MACHINE_ARCH       (optional)
|        HOST_MACHINE_MACHINE    (optional)
|        HOST_TEXT_START_ADDR
|        HOST_STACK_END_ADDR

	in the file @file{../include/sys/h-@var{XXX}.h} (for your host).  These
	values, plus the structures and macros defined in @file{a.out.h} on
	your host system, will produce a BFD target that will access
	ordinary a.out files on your host. To configure a new machine
	to use @file{host-aout.c}, specify:

|	TDEFAULTS = -DDEFAULT_VECTOR=host_aout_big_vec
|	TDEPFILES= host-aout.o trad-core.o

	in the @file{config/@var{XXX}.mt} file, and modify @file{configure.in}
	to use the
	@file{@var{XXX}.mt} file (by setting "<<bfd_target=XXX>>") when your
	configuration is selected.

*/

/* Some assumptions:
   * Any BFD with D_PAGED set is ZMAGIC, and vice versa.
     Doesn't matter what the setting of WP_TEXT is on output, but it'll
     get set on input.
   * Any BFD with D_PAGED clear and WP_TEXT set is NMAGIC.
   * Any BFD with both flags clear is OMAGIC.
   (Just want to make these explicit, so the conditions tested in this
   file make sense if you're more familiar with a.out than with BFD.)  */

#define KEEPIT flags
#define KEEPITTYPE int

#include <string.h>		/* For strchr and friends */
#include "bfd.h"
#include <sysdep.h>
#include "bfdlink.h"

#include "libaout.h"
#include "libbfd.h"
#include "aout/aout64.h"
#include "aout/stab_gnu.h"
#include "aout/ar.h"

static boolean aout_get_external_symbols PARAMS ((bfd *));

/*
SUBSECTION
	Relocations

DESCRIPTION
	The file @file{aoutx.h} provides for both the @emph{standard}
	and @emph{extended} forms of a.out relocation records.

	The standard records contain only an
	address, a symbol index, and a type field. The extended records
	(used on 29ks and sparcs) also have a full integer for an
	addend.

*/
#define CTOR_TABLE_RELOC_IDX 2

#define howto_table_ext NAME(aout,ext_howto_table)
#define howto_table_std NAME(aout,std_howto_table)

reloc_howto_type howto_table_ext[] =
{
  /* type           rs   size bsz  pcrel bitpos ovrf                  sf name          part_inpl readmask setmask pcdone */
  HOWTO(RELOC_8,      0,  0,  	8,  false, 0, complain_overflow_bitfield,0,"8",        false, 0,0x000000ff, false),
  HOWTO(RELOC_16,     0,  1, 	16, false, 0, complain_overflow_bitfield,0,"16",       false, 0,0x0000ffff, false),
  HOWTO(RELOC_32,     0,  2, 	32, false, 0, complain_overflow_bitfield,0,"32",       false, 0,0xffffffff, false),
  HOWTO(RELOC_DISP8,  0,  0, 	8,  true,  0, complain_overflow_signed,0,"DISP8", 	false, 0,0x000000ff, false),
  HOWTO(RELOC_DISP16, 0,  1, 	16, true,  0, complain_overflow_signed,0,"DISP16", 	false, 0,0x0000ffff, false),
  HOWTO(RELOC_DISP32, 0,  2, 	32, true,  0, complain_overflow_signed,0,"DISP32", 	false, 0,0xffffffff, false),
  HOWTO(RELOC_WDISP30,2,  2, 	30, true,  0, complain_overflow_signed,0,"WDISP30", 	false, 0,0x3fffffff, false),
  HOWTO(RELOC_WDISP22,2,  2, 	22, true,  0, complain_overflow_signed,0,"WDISP22", 	false, 0,0x003fffff, false),
  HOWTO(RELOC_HI22,   10, 2, 	22, false, 0, complain_overflow_bitfield,0,"HI22",	false, 0,0x003fffff, false),
  HOWTO(RELOC_22,     0,  2, 	22, false, 0, complain_overflow_bitfield,0,"22",       false, 0,0x003fffff, false),
  HOWTO(RELOC_13,     0,  2, 	13, false, 0, complain_overflow_bitfield,0,"13",       false, 0,0x00001fff, false),
  HOWTO(RELOC_LO10,   0,  2, 	10, false, 0, complain_overflow_dont,0,"LO10",     false, 0,0x000003ff, false),
  HOWTO(RELOC_SFA_BASE,0, 2, 	32, false, 0, complain_overflow_bitfield,0,"SFA_BASE", false, 0,0xffffffff, false),
  HOWTO(RELOC_SFA_OFF13,0,2, 	32, false, 0, complain_overflow_bitfield,0,"SFA_OFF13",false, 0,0xffffffff, false),
  HOWTO(RELOC_BASE10, 0,  2, 	16, false, 0, complain_overflow_bitfield,0,"BASE10",   false, 0,0x0000ffff, false),
  HOWTO(RELOC_BASE13, 0,  2,	13, false, 0, complain_overflow_bitfield,0,"BASE13",   false, 0,0x00001fff, false),
  HOWTO(RELOC_BASE22, 0,  2,	0,  false, 0, complain_overflow_bitfield,0,"BASE22",   false, 0,0x00000000, false),
  HOWTO(RELOC_PC10,   0,  2,	10, false, 0, complain_overflow_bitfield,0,"PC10",	false, 0,0x000003ff, false),
  HOWTO(RELOC_PC22,   0,  2,	22, false, 0, complain_overflow_bitfield,0,"PC22",	false, 0,0x003fffff, false),
  HOWTO(RELOC_JMP_TBL,0,  2,	32, false, 0, complain_overflow_bitfield,0,"JMP_TBL",	false, 0,0xffffffff, false),
  HOWTO(RELOC_SEGOFF16,0, 2,	0,  false, 0, complain_overflow_bitfield,0,"SEGOFF16",	false, 0,0x00000000, false),
  HOWTO(RELOC_GLOB_DAT,0, 2,	0,  false, 0, complain_overflow_bitfield,0,"GLOB_DAT",	false, 0,0x00000000, false),
  HOWTO(RELOC_JMP_SLOT,0, 2,	0,  false, 0, complain_overflow_bitfield,0,"JMP_SLOT",	false, 0,0x00000000, false),
  HOWTO(RELOC_RELATIVE,0, 2,	0,  false, 0, complain_overflow_bitfield,0,"RELATIVE",	false, 0,0x00000000, false),
};

/* Convert standard reloc records to "arelent" format (incl byte swap).  */

reloc_howto_type howto_table_std[] = {
  /* type              rs size bsz  pcrel bitpos ovrf                     sf name     part_inpl readmask  setmask    pcdone */
HOWTO( 0,	       0,  0,  	8,  false, 0, complain_overflow_bitfield,0,"8",		true, 0x000000ff,0x000000ff, false),
HOWTO( 1,	       0,  1, 	16, false, 0, complain_overflow_bitfield,0,"16",	true, 0x0000ffff,0x0000ffff, false),
HOWTO( 2,	       0,  2, 	32, false, 0, complain_overflow_bitfield,0,"32",	true, 0xffffffff,0xffffffff, false),
HOWTO( 3,	       0,  4, 	64, false, 0, complain_overflow_bitfield,0,"64",	true, 0xdeaddead,0xdeaddead, false),
HOWTO( 4,	       0,  0, 	8,  true,  0, complain_overflow_signed,  0,"DISP8",	true, 0x000000ff,0x000000ff, false),
HOWTO( 5,	       0,  1, 	16, true,  0, complain_overflow_signed,  0,"DISP16",	true, 0x0000ffff,0x0000ffff, false),
HOWTO( 6,	       0,  2, 	32, true,  0, complain_overflow_signed,  0,"DISP32",	true, 0xffffffff,0xffffffff, false),
HOWTO( 7,	       0,  4, 	64, true,  0, complain_overflow_signed,  0,"DISP64",	true, 0xfeedface,0xfeedface, false),
{ -1 },
HOWTO( 9,	       0,  1,   16, false, 0, complain_overflow_bitfield,0,"BASE16",	false,0xffffffff,0xffffffff, false),
HOWTO(10,	       0,  2,   32, false, 0, complain_overflow_bitfield,0,"BASE32",	false,0xffffffff,0xffffffff, false),
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
  HOWTO(16,	       0,  2,	 0, false, 0, complain_overflow_bitfield,0,"JMP_TABLE", false,         0,0x00000000, false),
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 },
  HOWTO(32,	       0,  2,	 0, false, 0, complain_overflow_bitfield,0,"RELATIVE",  false,         0,0x00000000, false),
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
{ -1 },
  HOWTO(40,	       0,  2,	 0, false, 0, complain_overflow_bitfield,0,"BASEREL",   false,         0,0x00000000, false),
};

#define TABLE_SIZE(TABLE)	(sizeof(TABLE)/sizeof(TABLE[0]))

CONST struct reloc_howto_struct *
NAME(aout,reloc_type_lookup) (abfd,code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
#define EXT(i,j)	case i: return &howto_table_ext[j]
#define STD(i,j)	case i: return &howto_table_std[j]
  int ext = obj_reloc_entry_size (abfd) == RELOC_EXT_SIZE;
  if (code == BFD_RELOC_CTOR)
    switch (bfd_get_arch_info (abfd)->bits_per_address)
      {
      case 32:
	code = BFD_RELOC_32;
	break;
      case 64:
	code = BFD_RELOC_64;
	break;
      }
  if (ext)
    switch (code)
      {
	EXT (BFD_RELOC_32, 2);
	EXT (BFD_RELOC_HI22, 8);
	EXT (BFD_RELOC_LO10, 11);
	EXT (BFD_RELOC_32_PCREL_S2, 6);
	EXT (BFD_RELOC_SPARC_WDISP22, 7);
	EXT (BFD_RELOC_SPARC13, 10);
	EXT (BFD_RELOC_SPARC_BASE13, 15);
      default: return (CONST struct reloc_howto_struct *) 0;
      }
  else
    /* std relocs */
    switch (code)
      {
	STD (BFD_RELOC_16, 1);
	STD (BFD_RELOC_32, 2);
	STD (BFD_RELOC_8_PCREL, 4);
	STD (BFD_RELOC_16_PCREL, 5);
	STD (BFD_RELOC_32_PCREL, 6);
	STD (BFD_RELOC_16_BASEREL, 9);
	STD (BFD_RELOC_32_BASEREL, 10);
      default: return (CONST struct reloc_howto_struct *) 0;
      }
}

/*
SUBSECTION
	Internal entry points

DESCRIPTION
	@file{aoutx.h} exports several routines for accessing the
	contents of an a.out file, which are gathered and exported in
	turn by various format specific files (eg sunos.c).

*/

/*
FUNCTION
	 aout_@var{size}_swap_exec_header_in

SYNOPSIS
	void aout_@var{size}_swap_exec_header_in,
           (bfd *abfd,
            struct external_exec *raw_bytes,
            struct internal_exec *execp);

DESCRIPTION
	Swap the information in an executable header @var{raw_bytes} taken
	from a raw byte stream memory image into the internal exec header
	structure @var{execp}.
*/

#ifndef NAME_swap_exec_header_in
void
NAME(aout,swap_exec_header_in) (abfd, raw_bytes, execp)
     bfd *abfd;
     struct external_exec *raw_bytes;
     struct internal_exec *execp;
{
  struct external_exec *bytes = (struct external_exec *)raw_bytes;

  /* The internal_exec structure has some fields that are unused in this
     configuration (IE for i960), so ensure that all such uninitialized
     fields are zero'd out.  There are places where two of these structs
     are memcmp'd, and thus the contents do matter. */
  memset ((PTR) execp, 0, sizeof (struct internal_exec));
  /* Now fill in fields in the execp, from the bytes in the raw data.  */
  execp->a_info   = bfd_h_get_32 (abfd, bytes->e_info);
  execp->a_text   = GET_WORD (abfd, bytes->e_text);
  execp->a_data   = GET_WORD (abfd, bytes->e_data);
  execp->a_bss    = GET_WORD (abfd, bytes->e_bss);
  execp->a_syms   = GET_WORD (abfd, bytes->e_syms);
  execp->a_entry  = GET_WORD (abfd, bytes->e_entry);
  execp->a_trsize = GET_WORD (abfd, bytes->e_trsize);
  execp->a_drsize = GET_WORD (abfd, bytes->e_drsize);
}
#define NAME_swap_exec_header_in NAME(aout,swap_exec_header_in)
#endif

/*
FUNCTION
	aout_@var{size}_swap_exec_header_out

SYNOPSIS
	void aout_@var{size}_swap_exec_header_out
	  (bfd *abfd,
	   struct internal_exec *execp,
	   struct external_exec *raw_bytes);

DESCRIPTION
	Swap the information in an internal exec header structure
	@var{execp} into the buffer @var{raw_bytes} ready for writing to disk.
*/
void
NAME(aout,swap_exec_header_out) (abfd, execp, raw_bytes)
     bfd *abfd;
     struct internal_exec *execp;
     struct external_exec *raw_bytes;
{
  struct external_exec *bytes = (struct external_exec *)raw_bytes;

  /* Now fill in fields in the raw data, from the fields in the exec struct. */
  bfd_h_put_32 (abfd, execp->a_info  , bytes->e_info);
  PUT_WORD (abfd, execp->a_text  , bytes->e_text);
  PUT_WORD (abfd, execp->a_data  , bytes->e_data);
  PUT_WORD (abfd, execp->a_bss   , bytes->e_bss);
  PUT_WORD (abfd, execp->a_syms  , bytes->e_syms);
  PUT_WORD (abfd, execp->a_entry , bytes->e_entry);
  PUT_WORD (abfd, execp->a_trsize, bytes->e_trsize);
  PUT_WORD (abfd, execp->a_drsize, bytes->e_drsize);
}

/* Make all the section for an a.out file.  */

boolean
NAME(aout,make_sections) (abfd)
     bfd *abfd;
{
  if (obj_textsec (abfd) == (asection *) NULL
      && bfd_make_section (abfd, ".text") == (asection *) NULL)
    return false;
  if (obj_datasec (abfd) == (asection *) NULL
      && bfd_make_section (abfd, ".data") == (asection *) NULL)
    return false;
  if (obj_bsssec (abfd) == (asection *) NULL
      && bfd_make_section (abfd, ".bss") == (asection *) NULL)
    return false;
  return true;
}

/*
FUNCTION
	aout_@var{size}_some_aout_object_p

SYNOPSIS
	bfd_target *aout_@var{size}_some_aout_object_p
	 (bfd *abfd,
	  bfd_target *(*callback_to_real_object_p)());

DESCRIPTION
	Some a.out variant thinks that the file open in @var{abfd}
	checking is an a.out file.  Do some more checking, and set up
	for access if it really is.  Call back to the calling
	environment's "finish up" function just before returning, to
	handle any last-minute setup.
*/

bfd_target *
NAME(aout,some_aout_object_p) (abfd, execp, callback_to_real_object_p)
     bfd *abfd;
     struct internal_exec *execp;
     bfd_target *(*callback_to_real_object_p) PARAMS ((bfd *));
{
  struct aout_data_struct *rawptr, *oldrawptr;
  bfd_target *result;

  rawptr = (struct aout_data_struct  *) bfd_zalloc (abfd, sizeof (struct aout_data_struct ));
  if (rawptr == NULL) {
    bfd_set_error (bfd_error_no_memory);
    return 0;
  }

  oldrawptr = abfd->tdata.aout_data;
  abfd->tdata.aout_data = rawptr;

  /* Copy the contents of the old tdata struct.
     In particular, we want the subformat, since for hpux it was set in
     hp300hpux.c:swap_exec_header_in and will be used in
     hp300hpux.c:callback.  */
  if (oldrawptr != NULL)
    *abfd->tdata.aout_data = *oldrawptr;

  abfd->tdata.aout_data->a.hdr = &rawptr->e;
  *(abfd->tdata.aout_data->a.hdr) = *execp;	/* Copy in the internal_exec struct */
  execp = abfd->tdata.aout_data->a.hdr;

  /* Set the file flags */
  abfd->flags = NO_FLAGS;
  if (execp->a_drsize || execp->a_trsize)
    abfd->flags |= HAS_RELOC;
  /* Setting of EXEC_P has been deferred to the bottom of this function */
  if (execp->a_syms)
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;
  if (N_DYNAMIC(*execp))
    abfd->flags |= DYNAMIC;

  if (N_MAGIC (*execp) == ZMAGIC)
    {
      abfd->flags |= D_PAGED|WP_TEXT;
      adata(abfd).magic = z_magic;
    }
  else if (N_MAGIC (*execp) == NMAGIC)
    {
      abfd->flags |= WP_TEXT;
      adata(abfd).magic = n_magic;
    }
  else
    adata(abfd).magic = o_magic;

  bfd_get_start_address (abfd) = execp->a_entry;

  obj_aout_symbols (abfd) = (aout_symbol_type *)NULL;
  bfd_get_symcount (abfd) = execp->a_syms / sizeof (struct external_nlist);

  /* The default relocation entry size is that of traditional V7 Unix.  */
  obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;

  /* The default symbol entry size is that of traditional Unix. */
  obj_symbol_entry_size (abfd) = EXTERNAL_NLIST_SIZE;

  obj_aout_external_syms (abfd) = NULL;
  obj_aout_external_strings (abfd) = NULL;
  obj_aout_sym_hashes (abfd) = NULL;

  if (! NAME(aout,make_sections) (abfd))
    return NULL;

  obj_datasec (abfd)->_raw_size = execp->a_data;
  obj_bsssec (abfd)->_raw_size = execp->a_bss;

  /* If this object is dynamically linked, we assume that both
     sections have relocs.  This does no real harm, even though it may
     not be true.  */
  obj_textsec (abfd)->flags =
    (execp->a_trsize != 0 || (abfd->flags & DYNAMIC) != 0
     ? (SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_HAS_CONTENTS | SEC_RELOC)
     : (SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_HAS_CONTENTS));
  obj_datasec (abfd)->flags =
    (execp->a_drsize != 0 || (abfd->flags & DYNAMIC) != 0
     ? (SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS | SEC_RELOC)
     : (SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS));
  obj_bsssec (abfd)->flags = SEC_ALLOC;

#ifdef THIS_IS_ONLY_DOCUMENTATION
  /* The common code can't fill in these things because they depend
     on either the start address of the text segment, the rounding
     up of virtual addresses between segments, or the starting file
     position of the text segment -- all of which varies among different
     versions of a.out.  */

  /* Call back to the format-dependent code to fill in the rest of the
     fields and do any further cleanup.  Things that should be filled
     in by the callback:  */

  struct exec *execp = exec_hdr (abfd);

  obj_textsec (abfd)->size = N_TXTSIZE(*execp);
  obj_textsec (abfd)->raw_size = N_TXTSIZE(*execp);
  /* data and bss are already filled in since they're so standard */

  /* The virtual memory addresses of the sections */
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);
  obj_datasec (abfd)->vma = N_DATADDR(*execp);
  obj_bsssec  (abfd)->vma = N_BSSADDR(*execp);

  /* The file offsets of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF(*execp);
  obj_datasec (abfd)->filepos = N_DATOFF(*execp);

  /* The file offsets of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TRELOFF(*execp);
  obj_datasec (abfd)->rel_filepos = N_DRELOFF(*execp);

  /* The file offsets of the string table and symbol table.  */
  obj_str_filepos (abfd) = N_STROFF (*execp);
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);

  /* Determine the architecture and machine type of the object file.  */
  switch (N_MACHTYPE (*exec_hdr (abfd))) {
  default:
    abfd->obj_arch = bfd_arch_obscure;
    break;
  }

  adata(abfd)->page_size = PAGE_SIZE;
  adata(abfd)->segment_size = SEGMENT_SIZE;
  adata(abfd)->exec_bytes_size = EXEC_BYTES_SIZE;

  return abfd->xvec;

  /* The architecture is encoded in various ways in various a.out variants,
     or is not encoded at all in some of them.  The relocation size depends
     on the architecture and the a.out variant.  Finally, the return value
     is the bfd_target vector in use.  If an error occurs, return zero and
     set bfd_error to the appropriate error code.

     Formats such as b.out, which have additional fields in the a.out
     header, should cope with them in this callback as well.  */
#endif				/* DOCUMENTATION */

  result = (*callback_to_real_object_p)(abfd);

  /* Now that the segment addresses have been worked out, take a better
     guess at whether the file is executable.  If the entry point
     is within the text segment, assume it is.  (This makes files
     executable even if their entry point address is 0, as long as
     their text starts at zero.)

     At some point we should probably break down and stat the file and
     declare it executable if (one of) its 'x' bits are on...  */
  if ((execp->a_entry >= obj_textsec(abfd)->vma) &&
      (execp->a_entry < obj_textsec(abfd)->vma + obj_textsec(abfd)->_raw_size))
    abfd->flags |= EXEC_P;
  if (result)
    {
#if 0 /* These should be set correctly anyways.  */
      abfd->sections = obj_textsec (abfd);
      obj_textsec (abfd)->next = obj_datasec (abfd);
      obj_datasec (abfd)->next = obj_bsssec (abfd);
#endif
    }
  else
    {
      free (rawptr);
      abfd->tdata.aout_data = oldrawptr;
    }
  return result;
}

/*
FUNCTION
	aout_@var{size}_mkobject

SYNOPSIS
	boolean aout_@var{size}_mkobject, (bfd *abfd);

DESCRIPTION
	Initialize BFD @var{abfd} for use with a.out files.
*/

boolean
NAME(aout,mkobject) (abfd)
     bfd *abfd;
{
  struct aout_data_struct  *rawptr;

  bfd_set_error (bfd_error_system_call);

  /* Use an intermediate variable for clarity */
  rawptr = (struct aout_data_struct *)bfd_zalloc (abfd, sizeof (struct aout_data_struct ));

  if (rawptr == NULL) {
    bfd_set_error (bfd_error_no_memory);
    return false;
  }

  abfd->tdata.aout_data = rawptr;
  exec_hdr (abfd) = &(rawptr->e);

  obj_textsec (abfd) = (asection *)NULL;
  obj_datasec (abfd) = (asection *)NULL;
  obj_bsssec (abfd) = (asection *)NULL;

  return true;
}


/*
FUNCTION
	aout_@var{size}_machine_type

SYNOPSIS
	enum machine_type  aout_@var{size}_machine_type
	 (enum bfd_architecture arch,
	  unsigned long machine));

DESCRIPTION
	Keep track of machine architecture and machine type for
	a.out's. Return the <<machine_type>> for a particular
	architecture and machine, or <<M_UNKNOWN>> if that exact architecture
	and machine can't be represented in a.out format.

	If the architecture is understood, machine type 0 (default)
	is always understood.
*/

enum machine_type
NAME(aout,machine_type) (arch, machine)
     enum bfd_architecture arch;
     unsigned long machine;
{
  enum machine_type arch_flags;

  arch_flags = M_UNKNOWN;

  switch (arch) {
  case bfd_arch_sparc:
    if (machine == 0)	arch_flags = M_SPARC;
    break;

  case bfd_arch_m68k:
    switch (machine) {
    case 0:		arch_flags = M_68010; break;
    case 68000:		arch_flags = M_UNKNOWN;	break;
    case 68010:		arch_flags = M_68010; break;
    case 68020:		arch_flags = M_68020; break;
    default:		arch_flags = M_UNKNOWN; break;
    }
    break;

  case bfd_arch_i386:
    if (machine == 0)	arch_flags = M_386;
    break;

  case bfd_arch_a29k:
    if (machine == 0)	arch_flags = M_29K;
    break;

  case bfd_arch_mips:
    switch (machine) {
    case 0:
    case 2000:
    case 3000:          arch_flags = M_MIPS1; break;
    case 4000:
    case 4400:
    case 6000:          arch_flags = M_MIPS2; break;
    default:            arch_flags = M_UNKNOWN; break;
    }
    break;

  default:
    arch_flags = M_UNKNOWN;
  }
  return arch_flags;
}


/*
FUNCTION
	aout_@var{size}_set_arch_mach

SYNOPSIS
	boolean aout_@var{size}_set_arch_mach,
	 (bfd *,
	  enum bfd_architecture arch,
	  unsigned long machine));

DESCRIPTION
	Set the architecture and the machine of the BFD @var{abfd} to the
	values @var{arch} and @var{machine}.  Verify that @var{abfd}'s format
	can support the architecture required.
*/

boolean
NAME(aout,set_arch_mach) (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  if (! bfd_default_set_arch_mach (abfd, arch, machine))
    return false;

  if (arch != bfd_arch_unknown &&
      NAME(aout,machine_type) (arch, machine) == M_UNKNOWN)
    return false;		/* We can't represent this type */

  /* Determine the size of a relocation entry */
  switch (arch) {
  case bfd_arch_sparc:
  case bfd_arch_a29k:
  case bfd_arch_mips:
    obj_reloc_entry_size (abfd) = RELOC_EXT_SIZE;
    break;
  default:
    obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
    break;
  }

  return (*aout_backend_info(abfd)->set_sizes) (abfd);
}

static void
adjust_o_magic (abfd, execp)
     bfd *abfd;
     struct internal_exec *execp;
{
  file_ptr pos = adata (abfd).exec_bytes_size;
  bfd_vma vma = 0;
  int pad = 0;

  /* Text.  */
  obj_textsec(abfd)->filepos = pos;
  pos += obj_textsec(abfd)->_raw_size;
  vma += obj_textsec(abfd)->_raw_size;

  /* Data.  */
  if (!obj_datasec(abfd)->user_set_vma)
    {
#if 0	    /* ?? Does alignment in the file image really matter? */
      pad = align_power (vma, obj_datasec(abfd)->alignment_power) - vma;
#endif
      obj_textsec(abfd)->_raw_size += pad;
      pos += pad;
      vma += pad;
      obj_datasec(abfd)->vma = vma;
    }
  obj_datasec(abfd)->filepos = pos;
  pos += obj_datasec(abfd)->_raw_size;
  vma += obj_datasec(abfd)->_raw_size;

  /* BSS.  */
  if (!obj_bsssec(abfd)->user_set_vma)
    {
#if 0
      pad = align_power (vma, obj_bsssec(abfd)->alignment_power) - vma;
#endif
      obj_datasec(abfd)->_raw_size += pad;
      pos += pad;
      vma += pad;
      obj_bsssec(abfd)->vma = vma;
    }
  obj_bsssec(abfd)->filepos = pos;

  /* Fix up the exec header.  */
  execp->a_text = obj_textsec(abfd)->_raw_size;
  execp->a_data = obj_datasec(abfd)->_raw_size;
  execp->a_bss = obj_bsssec(abfd)->_raw_size;
  N_SET_MAGIC (*execp, OMAGIC);
}

static void
adjust_z_magic (abfd, execp)
     bfd *abfd;
     struct internal_exec *execp;
{
  bfd_size_type data_pad, text_pad;
  file_ptr text_end;
  CONST struct aout_backend_data *abdp;
  int ztih;			/* Nonzero if text includes exec header.  */
  
  abdp = aout_backend_info (abfd);

  /* Text.  */
  ztih = abdp && abdp->text_includes_header;
  obj_textsec(abfd)->filepos = (ztih
				? adata(abfd).exec_bytes_size
				: adata(abfd).page_size);
  if (! obj_textsec(abfd)->user_set_vma)
    /* ?? Do we really need to check for relocs here?  */
    obj_textsec(abfd)->vma = ((abfd->flags & HAS_RELOC)
			      ? 0
			      : (ztih
				 ? (abdp->default_text_vma
				    + adata(abfd).exec_bytes_size)
				 : abdp->default_text_vma));
  /* Could take strange alignment of text section into account here?  */
  
  /* Find start of data.  */
  text_end = obj_textsec(abfd)->filepos + obj_textsec(abfd)->_raw_size;
  text_pad = BFD_ALIGN (text_end, adata(abfd).page_size) - text_end;
  obj_textsec(abfd)->_raw_size += text_pad;
  text_end += text_pad;

  /* Data.  */
  if (!obj_datasec(abfd)->user_set_vma)
    {
      bfd_vma vma;
      vma = obj_textsec(abfd)->vma + obj_textsec(abfd)->_raw_size;
      obj_datasec(abfd)->vma = BFD_ALIGN (vma, adata(abfd).segment_size);
    }
  if (abdp && abdp->zmagic_mapped_contiguous)
    {
      text_pad = (obj_datasec(abfd)->vma
		  - obj_textsec(abfd)->vma
		  - obj_textsec(abfd)->_raw_size);
      obj_textsec(abfd)->_raw_size += text_pad;
    }
  obj_datasec(abfd)->filepos = (obj_textsec(abfd)->filepos
				+ obj_textsec(abfd)->_raw_size);
  
  /* Fix up exec header while we're at it.  */
  execp->a_text = obj_textsec(abfd)->_raw_size;
  if (ztih && (!abdp || (abdp && !abdp->exec_header_not_counted)))
    execp->a_text += adata(abfd).exec_bytes_size;
  N_SET_MAGIC (*execp, ZMAGIC);

  /* Spec says data section should be rounded up to page boundary.  */
  obj_datasec(abfd)->_raw_size
    = align_power (obj_datasec(abfd)->_raw_size,
		   obj_bsssec(abfd)->alignment_power);
  execp->a_data = BFD_ALIGN (obj_datasec(abfd)->_raw_size,
			     adata(abfd).page_size);
  data_pad = execp->a_data - obj_datasec(abfd)->_raw_size;

  /* BSS.  */
  if (!obj_bsssec(abfd)->user_set_vma)
    obj_bsssec(abfd)->vma = (obj_datasec(abfd)->vma
			     + obj_datasec(abfd)->_raw_size);
  /* If the BSS immediately follows the data section and extra space
     in the page is left after the data section, fudge data
     in the header so that the bss section looks smaller by that
     amount.  We'll start the bss section there, and lie to the OS.
     (Note that a linker script, as well as the above assignment,
     could have explicitly set the BSS vma to immediately follow
     the data section.)  */
  if (align_power (obj_bsssec(abfd)->vma, obj_bsssec(abfd)->alignment_power)
      == obj_datasec(abfd)->vma + obj_datasec(abfd)->_raw_size)
    execp->a_bss = (data_pad > obj_bsssec(abfd)->_raw_size) ? 0 :
      obj_bsssec(abfd)->_raw_size - data_pad;
  else
    execp->a_bss = obj_bsssec(abfd)->_raw_size;
}

static void
adjust_n_magic (abfd, execp)
     bfd *abfd;
     struct internal_exec *execp;
{
  file_ptr pos = adata(abfd).exec_bytes_size;
  bfd_vma vma = 0;
  int pad;
  
  /* Text.  */
  obj_textsec(abfd)->filepos = pos;
  if (!obj_textsec(abfd)->user_set_vma)
    obj_textsec(abfd)->vma = vma;
  else
    vma = obj_textsec(abfd)->vma;
  pos += obj_textsec(abfd)->_raw_size;
  vma += obj_textsec(abfd)->_raw_size;

  /* Data.  */
  obj_datasec(abfd)->filepos = pos;
  if (!obj_datasec(abfd)->user_set_vma)
    obj_datasec(abfd)->vma = BFD_ALIGN (vma, adata(abfd).segment_size);
  vma = obj_datasec(abfd)->vma;
  
  /* Since BSS follows data immediately, see if it needs alignment.  */
  vma += obj_datasec(abfd)->_raw_size;
  pad = align_power (vma, obj_bsssec(abfd)->alignment_power) - vma;
  obj_datasec(abfd)->_raw_size += pad;
  pos += obj_datasec(abfd)->_raw_size;

  /* BSS.  */
  if (!obj_bsssec(abfd)->user_set_vma)
    obj_bsssec(abfd)->vma = vma;
  else
    vma = obj_bsssec(abfd)->vma;

  /* Fix up exec header.  */
  execp->a_text = obj_textsec(abfd)->_raw_size;
  execp->a_data = obj_datasec(abfd)->_raw_size;
  execp->a_bss = obj_bsssec(abfd)->_raw_size;
  N_SET_MAGIC (*execp, NMAGIC);
}

boolean
NAME(aout,adjust_sizes_and_vmas) (abfd, text_size, text_end)
     bfd *abfd;
     bfd_size_type *text_size;
     file_ptr *text_end;
{
  struct internal_exec *execp = exec_hdr (abfd);

  if (! NAME(aout,make_sections) (abfd))
    return false;

  if (adata(abfd).magic != undecided_magic) return true;

  obj_textsec(abfd)->_raw_size =
    align_power(obj_textsec(abfd)->_raw_size,
		obj_textsec(abfd)->alignment_power);

  *text_size = obj_textsec (abfd)->_raw_size;
  /* Rule (heuristic) for when to pad to a new page.  Note that there
     are (at least) two ways demand-paged (ZMAGIC) files have been
     handled.  Most Berkeley-based systems start the text segment at
     (PAGE_SIZE).  However, newer versions of SUNOS start the text
     segment right after the exec header; the latter is counted in the
     text segment size, and is paged in by the kernel with the rest of
     the text. */

  /* This perhaps isn't the right way to do this, but made it simpler for me
     to understand enough to implement it.  Better would probably be to go
     right from BFD flags to alignment/positioning characteristics.  But the
     old code was sloppy enough about handling the flags, and had enough
     other magic, that it was a little hard for me to understand.  I think
     I understand it better now, but I haven't time to do the cleanup this
     minute.  */

  if (abfd->flags & D_PAGED)
    /* Whether or not WP_TEXT is set -- let D_PAGED override.  */
    /* @@ What about QMAGIC?  */
    adata(abfd).magic = z_magic;
  else if (abfd->flags & WP_TEXT)
    adata(abfd).magic = n_magic;
  else
    adata(abfd).magic = o_magic;

#ifdef BFD_AOUT_DEBUG /* requires gcc2 */
#if __GNUC__ >= 2
  fprintf (stderr, "%s text=<%x,%x,%x> data=<%x,%x,%x> bss=<%x,%x,%x>\n",
	   ({ char *str;
	      switch (adata(abfd).magic) {
	      case n_magic: str = "NMAGIC"; break;
	      case o_magic: str = "OMAGIC"; break;
	      case z_magic: str = "ZMAGIC"; break;
	      default: abort ();
	      }
	      str;
	    }),
	   obj_textsec(abfd)->vma, obj_textsec(abfd)->_raw_size,
	   	obj_textsec(abfd)->alignment_power,
	   obj_datasec(abfd)->vma, obj_datasec(abfd)->_raw_size,
	   	obj_datasec(abfd)->alignment_power,
	   obj_bsssec(abfd)->vma, obj_bsssec(abfd)->_raw_size,
	   	obj_bsssec(abfd)->alignment_power);
#endif
#endif

  switch (adata(abfd).magic)
    {
    case o_magic:
      adjust_o_magic (abfd, execp);
      break;
    case z_magic:
      adjust_z_magic (abfd, execp);
      break;
    case n_magic:
      adjust_n_magic (abfd, execp);
      break;
    default:
      abort ();
    }

#ifdef BFD_AOUT_DEBUG
  fprintf (stderr, "       text=<%x,%x,%x> data=<%x,%x,%x> bss=<%x,%x>\n",
	   obj_textsec(abfd)->vma, obj_textsec(abfd)->_raw_size,
	   	obj_textsec(abfd)->filepos,
	   obj_datasec(abfd)->vma, obj_datasec(abfd)->_raw_size,
	   	obj_datasec(abfd)->filepos,
	   obj_bsssec(abfd)->vma, obj_bsssec(abfd)->_raw_size);
#endif

  return true;
}

/*
FUNCTION
	aout_@var{size}_new_section_hook

SYNOPSIS
        boolean aout_@var{size}_new_section_hook,
	   (bfd *abfd,
	    asection *newsect));

DESCRIPTION
	Called by the BFD in response to a @code{bfd_make_section}
	request.
*/
boolean
NAME(aout,new_section_hook) (abfd, newsect)
     bfd *abfd;
     asection *newsect;
{
  /* align to double at least */
  newsect->alignment_power = bfd_get_arch_info(abfd)->section_align_power;


  if (bfd_get_format (abfd) == bfd_object)
  {
    if (obj_textsec(abfd) == NULL && !strcmp(newsect->name, ".text")) {
	obj_textsec(abfd)= newsect;
	newsect->target_index = N_TEXT;
	return true;
      }

    if (obj_datasec(abfd) == NULL && !strcmp(newsect->name, ".data")) {
	obj_datasec(abfd) = newsect;
	newsect->target_index = N_DATA;
	return true;
      }

    if (obj_bsssec(abfd) == NULL && !strcmp(newsect->name, ".bss")) {
	obj_bsssec(abfd) = newsect;
	newsect->target_index = N_BSS;
	return true;
      }

  }

  /* We allow more than three sections internally */
  return true;
}

boolean
NAME(aout,set_section_contents) (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  file_ptr text_end;
  bfd_size_type text_size;

  if (abfd->output_has_begun == false)
      {
	if (NAME(aout,adjust_sizes_and_vmas) (abfd,
					      &text_size,
					      &text_end) == false)
	  return false;
      }

  /* regardless, once we know what we're doing, we might as well get going */
  if (section != obj_bsssec(abfd))
      {
	if (bfd_seek (abfd, section->filepos + offset, SEEK_SET) != 0)
	  return false;

	if (count) {
	  return (bfd_write ((PTR)location, 1, count, abfd) == count) ?
	    true : false;
	}
	return true;
      }
  return true;
}

/* Classify stabs symbols */

#define sym_in_text_section(sym) \
  (((sym)->type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_TEXT)

#define sym_in_data_section(sym) \
  (((sym)->type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_DATA)

#define sym_in_bss_section(sym) \
  (((sym)->type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_BSS)

/* Symbol is undefined if type is N_UNDF|N_EXT and if it has
  zero in the "value" field.  Nonzeroes there are fortrancommon
  symbols.  */
#define sym_is_undefined(sym) \
  ((sym)->type == (N_UNDF | N_EXT) && (sym)->symbol.value == 0)

/* Symbol is a global definition if N_EXT is on and if it has
  a nonzero type field.  */
#define sym_is_global_defn(sym) \
  (((sym)->type & N_EXT) && (sym)->type & N_TYPE)

/* Symbol is debugger info if any bits outside N_TYPE or N_EXT
  are on.  */
#define sym_is_debugger_info(sym) \
  (((sym)->type & ~(N_EXT | N_TYPE)) || (sym)->type == N_FN)

#define sym_is_fortrancommon(sym)       \
  (((sym)->type == (N_EXT)) && (sym)->symbol.value != 0)

/* Symbol is absolute if it has N_ABS set */
#define sym_is_absolute(sym) \
  (((sym)->type  & N_TYPE)== N_ABS)


#define sym_is_indirect(sym) \
  (((sym)->type & N_ABS)== N_ABS)

/* Read the external symbols from an a.out file.  */

static boolean
aout_get_external_symbols (abfd)
     bfd *abfd;
{
  if (obj_aout_external_syms (abfd) == (struct external_nlist *) NULL)
    {
      bfd_size_type count;
      struct external_nlist *syms;

      count = exec_hdr (abfd)->a_syms / EXTERNAL_NLIST_SIZE;

      /* We allocate using malloc to make the values easy to free
	 later on.  If we put them on the obstack it might not be
	 possible to free them.  */
      syms = ((struct external_nlist *)
	      malloc ((size_t) count * EXTERNAL_NLIST_SIZE));
      if (syms == (struct external_nlist *) NULL && count != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}

      if (bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET) != 0
	  || (bfd_read (syms, 1, exec_hdr (abfd)->a_syms, abfd)
	      != exec_hdr (abfd)->a_syms))
	{
	  free (syms);
	  return false;
	}

      obj_aout_external_syms (abfd) = syms;
      obj_aout_external_sym_count (abfd) = count;
    }
      
  if (obj_aout_external_strings (abfd) == NULL)
    {
      unsigned char string_chars[BYTES_IN_WORD];
      bfd_size_type stringsize;
      char *strings;

      /* Get the size of the strings.  */
      if (bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET) != 0
	  || (bfd_read ((PTR) string_chars, BYTES_IN_WORD, 1, abfd)
	      != BYTES_IN_WORD))
	return false;
      stringsize = GET_WORD (abfd, string_chars);

      strings = (char *) malloc ((size_t) stringsize + 1);
      if (strings == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}

      /* Skip space for the string count in the buffer for convenience
	 when using indexes.  */
      if (bfd_read (strings + BYTES_IN_WORD, 1, stringsize - BYTES_IN_WORD,
		    abfd)
	  != stringsize - BYTES_IN_WORD)
	{
	  free (strings);
	  return false;
	}

      /* Sanity preservation.  */
      strings[stringsize] = '\0';

      obj_aout_external_strings (abfd) = strings;
      obj_aout_external_string_size (abfd) = stringsize;
    }

  return true;
}

/* Only in their own functions for ease of debugging; when sym flags have
  stabilised these should be inlined into their (single) caller */

static boolean
translate_from_native_sym_flags (sym_pointer, cache_ptr, abfd)
     struct external_nlist *sym_pointer;
     aout_symbol_type * cache_ptr;
     bfd * abfd;
{
  cache_ptr->symbol.section = 0;
  switch (cache_ptr->type & N_TYPE)
    {
    case N_SETA: case N_SETA | N_EXT:
    case N_SETT: case N_SETT | N_EXT:
    case N_SETD: case N_SETD | N_EXT:
    case N_SETB: case N_SETB | N_EXT:
      {
	char *copy = bfd_alloc (abfd, strlen (cache_ptr->symbol.name) + 1);
	asection *section;
	asection *into_section;
	arelent_chain *reloc = (arelent_chain *) bfd_alloc (abfd, sizeof (arelent_chain));

	if (!copy || !reloc)
	  {
	    bfd_set_error (bfd_error_no_memory);
	    return false;
	  }

	strcpy (copy, cache_ptr->symbol.name);

	/* Make sure that this bfd has a section with the right contructor
	   name */
	section = bfd_get_section_by_name (abfd, copy);
	if (!section)
	  section = bfd_make_section (abfd, copy);

	/* Build a relocation entry for the constructor */
	switch ((cache_ptr->type & N_TYPE))
	  {
	  case N_SETA: case N_SETA | N_EXT:
	    into_section = &bfd_abs_section;
	    cache_ptr->type = N_ABS;
	    break;
	  case N_SETT: case N_SETT | N_EXT:
	    into_section = (asection *) obj_textsec (abfd);
	    cache_ptr->type = N_TEXT;
	    break;
	  case N_SETD: case N_SETD | N_EXT:
	    into_section = (asection *) obj_datasec (abfd);
	    cache_ptr->type = N_DATA;
	    break;
	  case N_SETB: case N_SETB | N_EXT:
	    into_section = (asection *) obj_bsssec (abfd);
	    cache_ptr->type = N_BSS;
	    break;
	  default:
	    bfd_set_error (bfd_error_bad_value);
	    return false;
	  }

	/* Build a relocation pointing into the constuctor section
	   pointing at the symbol in the set vector specified */

	reloc->relent.addend = cache_ptr->symbol.value;
	cache_ptr->symbol.section = into_section->symbol->section;
	reloc->relent.sym_ptr_ptr = into_section->symbol_ptr_ptr;


	/* We modify the symbol to belong to a section depending upon the
	   name of the symbol - probably __CTOR__ or __DTOR__ but we don't
	   really care, and add to the size of the section to contain a
	   pointer to the symbol. Build a reloc entry to relocate to this
	   symbol attached to this section.  */

	section->flags = SEC_CONSTRUCTOR | SEC_RELOC;


	section->reloc_count++;
	section->alignment_power = 2;

	reloc->next = section->constructor_chain;
	section->constructor_chain = reloc;
	reloc->relent.address = section->_raw_size;
	section->_raw_size += sizeof (int *);

	reloc->relent.howto
	  = (obj_reloc_entry_size(abfd) == RELOC_EXT_SIZE
	     ? howto_table_ext : howto_table_std)
	    + CTOR_TABLE_RELOC_IDX;
	cache_ptr->symbol.flags |= BSF_CONSTRUCTOR;
      }
      break;
    default:
      if (cache_ptr->type == N_WARNING)
	{
	  /* This symbol is the text of a warning message, the next symbol
	     is the symbol to associate the warning with */
	  cache_ptr->symbol.flags = BSF_DEBUGGING | BSF_WARNING;

	  /* @@ Stuffing pointers into integers is a no-no.
	     We can usually get away with it if the integer is
	     large enough though.  */
	  if (sizeof (cache_ptr + 1) > sizeof (bfd_vma))
	    abort ();
	  cache_ptr->symbol.value = (bfd_vma) ((cache_ptr + 1));

          /* We don't use a warning symbol's section, but we need
	     it to be nonzero for the sanity check below, so
	     pick one arbitrarily.  */
	  cache_ptr->symbol.section = &bfd_abs_section;

	  /* We furgle with the next symbol in place.
	     We don't want it to be undefined, we'll trample the type */
	  (sym_pointer + 1)->e_type[0] = 0xff;
	  break;
	}
      if ((cache_ptr->type | N_EXT) == (N_INDR | N_EXT))
	{
	  /* Two symbols in a row for an INDR message. The first symbol
	     contains the name we will match, the second symbol contains
	     the name the first name is translated into. It is supplied to
	     us undefined. This is good, since we want to pull in any files
	     which define it */
	  cache_ptr->symbol.flags = BSF_DEBUGGING | BSF_INDIRECT;

	  /* @@ Stuffing pointers into integers is a no-no.
	     We can usually get away with it if the integer is
	     large enough though.  */
	  if (sizeof (cache_ptr + 1) > sizeof (bfd_vma))
	    abort ();

	  cache_ptr->symbol.value = (bfd_vma) ((cache_ptr + 1));
	  cache_ptr->symbol.section = &bfd_ind_section;
	}

      else if (sym_is_debugger_info (cache_ptr))
	{
	  cache_ptr->symbol.flags = BSF_DEBUGGING;
	  /* Work out the section correct for this symbol */
	  switch (cache_ptr->type & N_TYPE)
	    {
	    case N_TEXT:
	    case N_FN:
	      cache_ptr->symbol.section = obj_textsec (abfd);
	      cache_ptr->symbol.value -= obj_textsec (abfd)->vma;
	      break;
	    case N_DATA:
	      cache_ptr->symbol.value -= obj_datasec (abfd)->vma;
	      cache_ptr->symbol.section = obj_datasec (abfd);
	      break;
	    case N_BSS:
	      cache_ptr->symbol.section = obj_bsssec (abfd);
	      cache_ptr->symbol.value -= obj_bsssec (abfd)->vma;
	      break;
	    default:
	    case N_ABS:
	      cache_ptr->symbol.section = &bfd_abs_section;
	      break;
	    }
	}
      else
	{

	  if (sym_is_fortrancommon (cache_ptr))
	    {
	      cache_ptr->symbol.flags = 0;
	      cache_ptr->symbol.section = &bfd_com_section;
	    }
	  else
	    {


	    }

	  /* In a.out, the value of a symbol is always relative to the
	   * start of the file, if this is a data symbol we'll subtract
	   * the size of the text section to get the section relative
	   * value. If this is a bss symbol (which would be strange)
	   * we'll subtract the size of the previous two sections
	   * to find the section relative address.
	   */

	  if (sym_in_text_section (cache_ptr))
	    {
	      cache_ptr->symbol.value -= obj_textsec (abfd)->vma;
	      cache_ptr->symbol.section = obj_textsec (abfd);
	    }
	  else if (sym_in_data_section (cache_ptr))
	    {
	      cache_ptr->symbol.value -= obj_datasec (abfd)->vma;
	      cache_ptr->symbol.section = obj_datasec (abfd);
	    }
	  else if (sym_in_bss_section (cache_ptr))
	    {
	      cache_ptr->symbol.section = obj_bsssec (abfd);
	      cache_ptr->symbol.value -= obj_bsssec (abfd)->vma;
	    }
	  else if (sym_is_undefined (cache_ptr))
	    {
	      cache_ptr->symbol.flags = 0;
	      cache_ptr->symbol.section = &bfd_und_section;
	    }
	  else if (sym_is_absolute (cache_ptr))
	    {
	      cache_ptr->symbol.section = &bfd_abs_section;
	    }

	  if (sym_is_global_defn (cache_ptr))
	    {
	      cache_ptr->symbol.flags = BSF_GLOBAL | BSF_EXPORT;
	    }
	  else if (! sym_is_undefined (cache_ptr))
	    {
	      cache_ptr->symbol.flags = BSF_LOCAL;
	    }
	}
    }
  if (cache_ptr->symbol.section == 0)
    abort ();
  return true;
}


static boolean
translate_to_native_sym_flags (sym_pointer, cache_ptr, abfd)
     struct external_nlist *sym_pointer;
     asymbol *cache_ptr;
     bfd *abfd;
{
  bfd_vma value = cache_ptr->value;

  /* mask out any existing type bits in case copying from one section
     to another */
  sym_pointer->e_type[0] &= ~N_TYPE;

  /* We attempt to order these tests by decreasing frequency of success,
     according to tcov when linking the linker.  */
  if (bfd_get_output_section(cache_ptr) == &bfd_abs_section) {
    sym_pointer->e_type[0] |= N_ABS;
  }
  else if (bfd_get_output_section(cache_ptr) == obj_textsec (abfd)) {
    sym_pointer->e_type[0] |= N_TEXT;
  }
  else if (bfd_get_output_section(cache_ptr) == obj_datasec (abfd)) {
    sym_pointer->e_type[0] |= N_DATA;
  }
  else if (bfd_get_output_section(cache_ptr) == obj_bsssec (abfd)) {
    sym_pointer->e_type[0] |= N_BSS;
  }
  else if (bfd_get_output_section(cache_ptr) == &bfd_und_section) {
    sym_pointer->e_type[0] = (N_UNDF | N_EXT);
  }
  else if (bfd_get_output_section(cache_ptr) == &bfd_ind_section) {
    sym_pointer->e_type[0] = N_INDR;
  }
  else if (bfd_get_output_section(cache_ptr) == NULL) {
    /* Protect the bfd_is_com_section call.
       This case occurs, e.g., for the *DEBUG* section of a COFF file.  */
    bfd_set_error (bfd_error_nonrepresentable_section);
    return false;
  }
  else if (bfd_is_com_section (bfd_get_output_section (cache_ptr))) {
    sym_pointer->e_type[0] = (N_UNDF | N_EXT);
  }
  else {
    bfd_set_error (bfd_error_nonrepresentable_section);
    return false;
  }

  /* Turn the symbol from section relative to absolute again */

  value +=  cache_ptr->section->output_section->vma  + cache_ptr->section->output_offset ;


  if (cache_ptr->flags & (BSF_WARNING)) {
    sym_pointer->e_type[0] = N_WARNING;
    (sym_pointer+1)->e_type[0] = 1;
  }

  if (cache_ptr->flags & BSF_DEBUGGING) {
    sym_pointer->e_type[0] = ((aout_symbol_type *)cache_ptr)->type;
  }
  else if (cache_ptr->flags & (BSF_GLOBAL | BSF_EXPORT)) {
    sym_pointer->e_type[0] |= N_EXT;
  }
  if (cache_ptr->flags & BSF_CONSTRUCTOR) {
    int type = ((aout_symbol_type *)cache_ptr)->type;
    switch (type)
      {
      case N_ABS:	type = N_SETA; break;
      case N_TEXT:	type = N_SETT; break;
      case N_DATA:	type = N_SETD; break;
      case N_BSS:	type = N_SETB; break;
      }
    sym_pointer->e_type[0] = type;
  }

  PUT_WORD(abfd, value, sym_pointer->e_value);

  return true;
}

/* Native-level interface to symbols. */


asymbol *
NAME(aout,make_empty_symbol) (abfd)
     bfd *abfd;
{
  aout_symbol_type  *new =
    (aout_symbol_type *)bfd_zalloc (abfd, sizeof (aout_symbol_type));
  if (!new)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

/* Translate a set of internal symbols into external symbols.  */

boolean
NAME(aout,translate_symbol_table) (abfd, in, ext, count, str, strsize, dynamic)
     bfd *abfd;
     aout_symbol_type *in;
     struct external_nlist *ext;
     bfd_size_type count;
     char *str;
     bfd_size_type strsize;
     boolean dynamic;
{
  struct external_nlist *ext_end;

  ext_end = ext + count;
  for (; ext < ext_end; ext++, in++)
    {
      bfd_vma x;

      x = GET_WORD (abfd, ext->e_strx);
      in->symbol.the_bfd = abfd;

      /* For the normal symbols, the zero index points at the number
	 of bytes in the string table but is to be interpreted as the
	 null string.  For the dynamic symbols, the number of bytes in
	 the string table is stored in the __DYNAMIC structure and the
	 zero index points at an actual string.  */
      if (x == 0 && ! dynamic)
	in->symbol.name = "";
      else if (x < strsize)
	in->symbol.name = str + x;
      else
	return false;

      in->symbol.value = GET_SWORD (abfd,  ext->e_value);
      in->desc = bfd_h_get_16 (abfd, ext->e_desc);
      in->other = bfd_h_get_8 (abfd, ext->e_other);
      in->type = bfd_h_get_8 (abfd,  ext->e_type);
      in->symbol.udata = 0;

      if (!translate_from_native_sym_flags (ext, in, abfd))
	return false;

      if (dynamic)
	in->symbol.flags |= BSF_DYNAMIC;
    }

  return true;
}

/* We read the symbols into a buffer, which is discarded when this
   function exits.  We read the strings into a buffer large enough to
   hold them all plus all the cached symbol entries. */

boolean
NAME(aout,slurp_symbol_table) (abfd)
     bfd *abfd;
{
  struct external_nlist *old_external_syms;
  aout_symbol_type *cached;
  size_t cached_size;

  /* If there's no work to be done, don't do any */
  if (obj_aout_symbols (abfd) != (aout_symbol_type *) NULL)
    return true;

  old_external_syms = obj_aout_external_syms (abfd);

  if (! aout_get_external_symbols (abfd))
    return false;

  if (obj_aout_external_sym_count (abfd) == 0)
    {
      bfd_set_error (bfd_error_no_symbols);
      return false;
    }

  cached_size = (obj_aout_external_sym_count (abfd)
		 * sizeof (aout_symbol_type));
  cached = (aout_symbol_type *) malloc (cached_size);
  memset (cached, 0, cached_size);

  if (cached == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  /* Convert from external symbol information to internal.  */
  if (! (NAME(aout,translate_symbol_table)
	 (abfd, cached,
	  obj_aout_external_syms (abfd),
	  obj_aout_external_sym_count (abfd),
	  obj_aout_external_strings (abfd),
	  obj_aout_external_string_size (abfd),
	  false)))
    {
      free (cached);
      return false;
    }

  bfd_get_symcount (abfd) = obj_aout_external_sym_count (abfd);

  obj_aout_symbols (abfd) = cached;

  /* It is very likely that anybody who calls this function will not
     want the external symbol information, so if it was allocated
     because of our call to aout_get_external_symbols, we free it up
     right away to save space.  */
  if (old_external_syms == (struct external_nlist *) NULL
      && obj_aout_external_syms (abfd) != (struct external_nlist *) NULL)
    {
      free (obj_aout_external_syms (abfd));
      obj_aout_external_syms (abfd) = NULL;
    }

  return true;
}

/* Possible improvements:
   + look for strings matching trailing substrings of other strings
   + better data structures?  balanced trees?
   + smaller per-string or per-symbol data?  re-use some of the symbol's
     data fields?
   + also look at reducing memory use elsewhere -- maybe if we didn't have to
     construct the entire symbol table at once, we could get by with smaller
     amounts of VM?  (What effect does that have on the string table
     reductions?)
   + rip this out of here, put it into its own file in bfd or libiberty, so
     coff and elf can use it too.  I'll work on this soon, but have more
     pressing tasks right now.

   A hash table might(?) be more efficient for handling exactly the cases that
   are handled now, but for trailing substring matches, I think we want to
   examine the `nearest' values (reverse-)lexically, not merely impose a strict
   order, nor look only for exact-match or not-match.  I don't think a hash
   table would be very useful for that, and I don't feel like fleshing out two
   completely different implementations.  [raeburn:930419.0331EDT] */

struct stringtab_entry {
  /* Hash value for this string.  Only useful so long as we aren't doing
     substring matches.  */
  unsigned int hash;

  /* Next node to look at, depending on whether the hash value of the string
     being searched for is less than or greater than the hash value of the
     current node.  For now, `equal to' is lumped in with `greater than', for
     space efficiency.  It's not a common enough case to warrant another field
     to be used for all nodes.  */
  struct stringtab_entry *less;
  struct stringtab_entry *greater;

  /* The string itself.  */
  CONST char *string;

  /* The index allocated for this string.  */
  bfd_size_type index;

#ifdef GATHER_STATISTICS
  /* How many references have there been to this string?  (Not currently used;
     could be dumped out for anaylsis, if anyone's interested.)  */
  unsigned long count;
#endif

  /* Next node in linked list, in suggested output order.  */
  struct stringtab_entry *next_to_output;
};

struct stringtab_data {
  /* Tree of string table entries.  */
  struct stringtab_entry *strings;

  /* Fudge factor used to center top node of tree.  */
  int hash_zero;

  /* Next index value to issue.  */
  bfd_size_type index;

  /* Index used for empty strings.  Cached here because checking for them
     is really easy, and we can avoid searching the tree.  */
  bfd_size_type empty_string_index;

  /* These fields indicate the two ends of a singly-linked list that indicates
     the order strings should be written out in.  Use this order, and no
     seeking will need to be done, so output efficiency should be maximized. */
  struct stringtab_entry **end;
  struct stringtab_entry *output_order;

#ifdef GATHER_STATISTICS
  /* Number of strings which duplicate strings already in the table.  */
  unsigned long duplicates;

  /* Number of bytes saved by not having to write all the duplicate strings. */
  unsigned long bytes_saved;

  /* Number of zero-length strings.  Currently, these all turn into
     references to the null byte at the end of the first string.  In some
     cases (possibly not all?  explore this...), it should be possible to
     simply write out a zero index value.  */
  unsigned long empty_strings;

  /* Number of times the hash values matched but the strings were different.
     Note that this includes the number of times the other string(s) occurs, so
     there may only be two strings hashing to the same value, even if this
     number is very large.  */
  unsigned long bad_hash_matches;

  /* Null strings aren't counted in this one.
     This will probably only be nonzero if we've got an input file
     which was produced by `ld -r' (i.e., it's already been processed
     through this code).  Under some operating systems, native tools
     may make all empty strings have the same index; but the pointer
     check won't catch those, because to get to that stage we'd already
     have to compute the checksum, which requires reading the string,
     so we short-circuit that case with empty_string_index above.  */
  unsigned long pointer_matches;

  /* Number of comparisons done.  I figure with the algorithms in use below,
     the average number of comparisons done (per symbol) should be roughly
     log-base-2 of the number of unique strings.  */
  unsigned long n_compares;
#endif
};

/* Some utility functions for the string table code.  */

/* For speed, only hash on the first this many bytes of strings.
   This number was chosen by profiling ld linking itself, with -g.  */
#define HASHMAXLEN 25

#define HASH_CHAR(c) (sum ^= sum >> 20, sum ^= sum << 7, sum += (c))

static INLINE unsigned int
hash (string, len)
     unsigned char *string;
     register unsigned int len;
{
  register unsigned int sum = 0;

  if (len > HASHMAXLEN)
    {
      HASH_CHAR (len);
      len = HASHMAXLEN;
    }

  while (len--)
    {
      HASH_CHAR (*string++);
    }
  return sum;
}

static INLINE void
stringtab_init (tab)
     struct stringtab_data *tab;
{
  tab->strings = 0;
  tab->output_order = 0;
  tab->hash_zero = 0;
  tab->end = &tab->output_order;

  /* Initial string table length includes size of length field.  */
  tab->index = BYTES_IN_WORD;
  tab->empty_string_index = -1;
#ifdef GATHER_STATISTICS
  tab->duplicates = 0;
  tab->empty_strings = 0;
  tab->bad_hash_matches = 0;
  tab->pointer_matches = 0;
  tab->bytes_saved = 0;
  tab->n_compares = 0;
#endif
}

static INLINE int
compare (entry, str, hash)
     struct stringtab_entry *entry;
     CONST char *str;
     unsigned int hash;
{
  return hash - entry->hash;
}

#ifdef GATHER_STATISTICS
/* Don't want to have to link in math library with all bfd applications...  */
static INLINE double
log2 (num)
     int num;
{
  double d = num;
  int n = 0;
  while (d >= 2.0)
    n++, d /= 2.0;
  return ((d > 1.41) ? 0.5 : 0) + n;
}
#endif

/* Main string table routines.  */
/* Returns index in string table.  Whether or not this actually adds an
   entry into the string table should be irrelevant -- it just has to
   return a valid index.  */
static bfd_size_type
add_to_stringtab (abfd, str, tab)
     bfd *abfd;
     CONST char *str;
     struct stringtab_data *tab;
{
  struct stringtab_entry **ep;
  register struct stringtab_entry *entry;
  unsigned int hashval, len;

  if (str[0] == 0)
    {
      bfd_size_type index;
      CONST bfd_size_type minus_one = -1;

#ifdef GATHER_STATISTICS
      tab->empty_strings++;
#endif
      index = tab->empty_string_index;
      if (index != minus_one)
	{
	got_empty:
#ifdef GATHER_STATISTICS
	  tab->bytes_saved++;
	  tab->duplicates++;
#endif
	  return index;
	}

      /* Need to find it.  */
      entry = tab->strings;
      if (entry)
	{
	  index = entry->index + strlen (entry->string);
	  tab->empty_string_index = index;
	  goto got_empty;
	}
      len = 0;
    }
  else
    len = strlen (str);

  /* The hash_zero value is chosen such that the first symbol gets a value of
     zero.  With a balanced tree, this wouldn't be very useful, but without it,
     we might get a more even split at the top level, instead of skewing it
     badly should hash("/usr/lib/crt0.o") (or whatever) be far from zero. */
  hashval = hash (str, len) ^ tab->hash_zero;
  ep = &tab->strings;
  if (!*ep)
    {
      tab->hash_zero = hashval;
      hashval = 0;
      goto add_it;
    }

  while (*ep)
    {
      register int cmp;

      entry = *ep;
#ifdef GATHER_STATISTICS
      tab->n_compares++;
#endif
      cmp = compare (entry, str, hashval);
      /* The not-equal cases are more frequent, so check them first.  */
      if (cmp > 0)
	ep = &entry->greater;
      else if (cmp < 0)
	ep = &entry->less;
      else
	{
	  if (entry->string == str)
	    {
#ifdef GATHER_STATISTICS
	      tab->pointer_matches++;
#endif
	      goto match;
	    }
	  /* Compare the first bytes to save a function call if they
	     don't match.  */
	  if (entry->string[0] == str[0] && !strcmp (entry->string, str))
	    {
	    match:
#ifdef GATHER_STATISTICS
	      entry->count++;
	      tab->bytes_saved += len + 1;
	      tab->duplicates++;
#endif
	      /* If we're in the linker, and the new string is from a new
		 input file which might have already had these reductions
		 run over it, we want to keep the new string pointer.  I
		 don't think we're likely to see any (or nearly as many,
		 at least) cases where a later string is in the same location
		 as an earlier one rather than this one.  */
	      entry->string = str;
	      return entry->index;
	    }
#ifdef GATHER_STATISTICS
	  tab->bad_hash_matches++;
#endif
	  ep = &entry->greater;
	}
    }

  /* If we get here, nothing that's in the table already matched.
     EP points to the `next' field at the end of the chain; stick a
     new entry on here.  */
 add_it:
  entry = (struct stringtab_entry *)
    bfd_alloc_by_size_t (abfd, sizeof (struct stringtab_entry));
  if (!entry)
    {
      bfd_set_error (bfd_error_no_memory);
      abort();			/* FIXME */
    }

  entry->less = entry->greater = 0;
  entry->hash = hashval;
  entry->index = tab->index;
  entry->string = str;
  entry->next_to_output = 0;
#ifdef GATHER_STATISTICS
  entry->count = 1;
#endif

  BFD_ASSERT (*tab->end == 0);
  *(tab->end) = entry;
  tab->end = &entry->next_to_output;
  BFD_ASSERT (*tab->end == 0);

  {
    tab->index += len + 1;
    if (len == 0)
      tab->empty_string_index = entry->index;
  }
  BFD_ASSERT (*ep == 0);
  *ep = entry;
  return entry->index;
}

static boolean
emit_strtab (abfd, tab)
     bfd *abfd;
     struct stringtab_data *tab;
{
  struct stringtab_entry *entry;
#ifdef GATHER_STATISTICS
  int count = 0;
#endif

  /* Be sure to put string length into correct byte ordering before writing
     it out.  */
  char buffer[BYTES_IN_WORD];

  PUT_WORD (abfd, tab->index, (unsigned char *) buffer);
  if (bfd_write ((PTR) buffer, 1, BYTES_IN_WORD, abfd) != BYTES_IN_WORD)
    return false;

  for (entry = tab->output_order; entry; entry = entry->next_to_output)
    {
      size_t len = strlen (entry->string) + 1;

      if (bfd_write ((PTR) entry->string, 1, len, abfd) != len)
	return false;

#ifdef GATHER_STATISTICS
      count++;
#endif
    }

#ifdef GATHER_STATISTICS
  /* Short form only, for now.
     To do:  Specify output file.  Conditionalize on environment?  Detailed
     analysis if desired.  */
  {
    int n_syms = bfd_get_symcount (abfd);

    fprintf (stderr, "String table data for output file:\n");
    fprintf (stderr, "  %8d symbols output\n", n_syms);
    fprintf (stderr, "  %8d duplicate strings\n", tab->duplicates);
    fprintf (stderr, "  %8d empty strings\n", tab->empty_strings);
    fprintf (stderr, "  %8d unique strings output\n", count);
    fprintf (stderr, "  %8d pointer matches\n", tab->pointer_matches);
    fprintf (stderr, "  %8d bytes saved\n", tab->bytes_saved);
    fprintf (stderr, "  %8d bad hash matches\n", tab->bad_hash_matches);
    fprintf (stderr, "  %8d hash-val comparisons\n", tab->n_compares);
    if (n_syms)
      {
	double n_compares = tab->n_compares;
	double avg_compares = n_compares / n_syms;
	/* The second value here should usually be near one.  */
	fprintf (stderr,
		 "\t    average %f comparisons per symbol (%f * log2 nstrings)\n",
		 avg_compares, avg_compares / log2 (count));
      }
  }
#endif

/* Old code:
  unsigned int count;
  generic = bfd_get_outsymbols(abfd);
  for (count = 0; count < bfd_get_symcount(abfd); count++)
    {
      asymbol *g = *(generic++);

      if (g->name)
	{
	  size_t length = strlen(g->name)+1;
	  bfd_write((PTR)g->name, 1, length, abfd);
	}
      g->KEEPIT = (KEEPITTYPE) count;
    } */

  return true;
}

boolean
NAME(aout,write_syms) (abfd)
     bfd *abfd;
{
  unsigned int count ;
  asymbol **generic = bfd_get_outsymbols (abfd);
  struct stringtab_data strtab;

  stringtab_init (&strtab);

  for (count = 0; count < bfd_get_symcount (abfd); count++)
    {
      asymbol *g = generic[count];
      struct external_nlist nsp;

      if (g->name)
	PUT_WORD (abfd, add_to_stringtab (abfd, g->name, &strtab),
		  (unsigned char *) nsp.e_strx);
      else
	PUT_WORD (abfd, 0, (unsigned char *)nsp.e_strx);

      if (bfd_asymbol_flavour(g) == abfd->xvec->flavour)
	{
	  bfd_h_put_16(abfd, aout_symbol(g)->desc,  nsp.e_desc);
	  bfd_h_put_8(abfd, aout_symbol(g)->other,  nsp.e_other);
	  bfd_h_put_8(abfd, aout_symbol(g)->type,  nsp.e_type);
	}
      else
	{
	  bfd_h_put_16(abfd,0, nsp.e_desc);
	  bfd_h_put_8(abfd, 0, nsp.e_other);
	  bfd_h_put_8(abfd, 0, nsp.e_type);
	}

      if (! translate_to_native_sym_flags (&nsp, g, abfd))
	return false;

      if (bfd_write((PTR)&nsp,1,EXTERNAL_NLIST_SIZE, abfd)
	  != EXTERNAL_NLIST_SIZE)
	return false;

      /* NB: `KEEPIT' currently overlays `flags', so set this only
	 here, at the end.  */
      g->KEEPIT = count;
    }

  return emit_strtab (abfd, &strtab);
}


long
NAME(aout,get_symtab) (abfd, location)
     bfd *abfd;
     asymbol **location;
{
    unsigned int counter = 0;
    aout_symbol_type *symbase;

    if (!NAME(aout,slurp_symbol_table)(abfd))
      return -1;

    for (symbase = obj_aout_symbols(abfd); counter++ < bfd_get_symcount (abfd);)
      *(location++) = (asymbol *)( symbase++);
    *location++ =0;
    return bfd_get_symcount (abfd);
}


/* Standard reloc stuff */
/* Output standard relocation information to a file in target byte order. */

void
NAME(aout,swap_std_reloc_out) (abfd, g, natptr)
     bfd *abfd;
     arelent *g;
     struct reloc_std_external *natptr;
{
  int r_index;
  asymbol *sym = *(g->sym_ptr_ptr);
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  asection *output_section = sym->section->output_section;

  PUT_WORD(abfd, g->address, natptr->r_address);

  r_length = g->howto->size ;	/* Size as a power of two */
  r_pcrel  = (int) g->howto->pc_relative; /* Relative to PC? */
  /* XXX This relies on relocs coming from a.out files.  */
  r_baserel = (g->howto->type & 8) != 0;
  r_jmptable = (g->howto->type & 16) != 0;
  r_relative = (g->howto->type & 32) != 0;

#if 0
  /* For a standard reloc, the addend is in the object file.  */
  r_addend = g->addend + (*(g->sym_ptr_ptr))->section->output_section->vma;
#endif

  /* name was clobbered by aout_write_syms to be symbol index */

  /* If this relocation is relative to a symbol then set the
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here
     */


  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section)
    {
      if (bfd_abs_section.symbol == sym)
      {
	/* Whoops, looked like an abs symbol, but is really an offset
	   from the abs section */
	r_index = 0;
	r_extern = 0;
       }
      else
      {
	/* Fill in symbol */
	r_extern = 1;
	r_index =  stoi((*(g->sym_ptr_ptr))->KEEPIT);

      }
    }
  else
    {
      /* Just an ordinary section */
      r_extern = 0;
      r_index  = output_section->target_index;
    }

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
      natptr->r_index[0] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[2] = r_index;
      natptr->r_type[0] =
       (r_extern?    RELOC_STD_BITS_EXTERN_BIG: 0)
	| (r_pcrel?     RELOC_STD_BITS_PCREL_BIG: 0)
	 | (r_baserel?   RELOC_STD_BITS_BASEREL_BIG: 0)
	  | (r_jmptable?  RELOC_STD_BITS_JMPTABLE_BIG: 0)
	   | (r_relative?  RELOC_STD_BITS_RELATIVE_BIG: 0)
	    | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG);
    } else {
	natptr->r_index[2] = r_index >> 16;
	natptr->r_index[1] = r_index >> 8;
	natptr->r_index[0] = r_index;
	natptr->r_type[0] =
	 (r_extern?    RELOC_STD_BITS_EXTERN_LITTLE: 0)
	  | (r_pcrel?     RELOC_STD_BITS_PCREL_LITTLE: 0)
	   | (r_baserel?   RELOC_STD_BITS_BASEREL_LITTLE: 0)
	    | (r_jmptable?  RELOC_STD_BITS_JMPTABLE_LITTLE: 0)
	     | (r_relative?  RELOC_STD_BITS_RELATIVE_LITTLE: 0)
	      | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE);
      }
}


/* Extended stuff */
/* Output extended relocation information to a file in target byte order. */

void
NAME(aout,swap_ext_reloc_out) (abfd, g, natptr)
     bfd *abfd;
     arelent *g;
     register struct reloc_ext_external *natptr;
{
  int r_index;
  int r_extern;
  unsigned int r_type;
  unsigned int r_addend;
  asymbol *sym = *(g->sym_ptr_ptr);
  asection *output_section = sym->section->output_section;

  PUT_WORD (abfd, g->address, natptr->r_address);

  r_type = (unsigned int) g->howto->type;

  r_addend = g->addend + (*(g->sym_ptr_ptr))->section->output_section->vma;

  /* If this relocation is relative to a symbol then set the
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here.  */

  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section)
  {
    if (bfd_abs_section.symbol == sym)
    {
      /* Whoops, looked like an abs symbol, but is really an offset
	 from the abs section */
      r_index = 0;
      r_extern = 0;
     }
    else
    {
      r_extern = 1;
      r_index =  stoi((*(g->sym_ptr_ptr))->KEEPIT);
    }
  }
  else
  {
    /* Just an ordinary section */
    r_extern = 0;
    r_index  = output_section->target_index;
  }

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    natptr->r_index[0] = r_index >> 16;
    natptr->r_index[1] = r_index >> 8;
    natptr->r_index[2] = r_index;
    natptr->r_type[0] =
      ((r_extern? RELOC_EXT_BITS_EXTERN_BIG: 0)
       | (r_type << RELOC_EXT_BITS_TYPE_SH_BIG));
  } else {
    natptr->r_index[2] = r_index >> 16;
    natptr->r_index[1] = r_index >> 8;
    natptr->r_index[0] = r_index;
    natptr->r_type[0] =
     (r_extern? RELOC_EXT_BITS_EXTERN_LITTLE: 0)
      | (r_type << RELOC_EXT_BITS_TYPE_SH_LITTLE);
  }

  PUT_WORD (abfd, r_addend, natptr->r_addend);
}

/* BFD deals internally with all things based from the section they're
   in. so, something in 10 bytes into a text section  with a base of
   50 would have a symbol (.text+10) and know .text vma was 50.

   Aout keeps all it's symbols based from zero, so the symbol would
   contain 60. This macro subs the base of each section from the value
   to give the true offset from the section */


#define MOVE_ADDRESS(ad)       						\
  if (r_extern) {							\
   /* undefined symbol */						\
     cache_ptr->sym_ptr_ptr = symbols + r_index;			\
     cache_ptr->addend = ad;						\
     } else {								\
    /* defined, section relative. replace symbol with pointer to    	\
       symbol which points to section  */				\
    switch (r_index) {							\
    case N_TEXT:							\
    case N_TEXT | N_EXT:						\
      cache_ptr->sym_ptr_ptr  = obj_textsec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad  - su->textsec->vma;			\
      break;								\
    case N_DATA:							\
    case N_DATA | N_EXT:						\
      cache_ptr->sym_ptr_ptr  = obj_datasec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad - su->datasec->vma;			\
      break;								\
    case N_BSS:								\
    case N_BSS | N_EXT:							\
      cache_ptr->sym_ptr_ptr  = obj_bsssec(abfd)->symbol_ptr_ptr;	\
      cache_ptr->addend = ad - su->bsssec->vma;				\
      break;								\
    default:								\
    case N_ABS:								\
    case N_ABS | N_EXT:							\
     cache_ptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;	\
      cache_ptr->addend = ad;						\
      break;								\
    }									\
  }     								\

void
NAME(aout,swap_ext_reloc_in) (abfd, bytes, cache_ptr, symbols)
     bfd *abfd;
     struct reloc_ext_external *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
{
  int r_index;
  int r_extern;
  unsigned int r_type;
  struct aoutdata *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = (GET_SWORD (abfd, bytes->r_address));

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    r_index =  (bytes->r_index[0] << 16)
	     | (bytes->r_index[1] << 8)
	     |  bytes->r_index[2];
    r_extern = (0 != (bytes->r_type[0] & RELOC_EXT_BITS_EXTERN_BIG));
    r_type   =       (bytes->r_type[0] & RELOC_EXT_BITS_TYPE_BIG)
				      >> RELOC_EXT_BITS_TYPE_SH_BIG;
  } else {
    r_index =  (bytes->r_index[2] << 16)
	     | (bytes->r_index[1] << 8)
	     |  bytes->r_index[0];
    r_extern = (0 != (bytes->r_type[0] & RELOC_EXT_BITS_EXTERN_LITTLE));
    r_type   =       (bytes->r_type[0] & RELOC_EXT_BITS_TYPE_LITTLE)
				      >> RELOC_EXT_BITS_TYPE_SH_LITTLE;
  }

  cache_ptr->howto =  howto_table_ext + r_type;
  MOVE_ADDRESS(GET_SWORD(abfd, bytes->r_addend));
}

void
NAME(aout,swap_std_reloc_in) (abfd, bytes, cache_ptr, symbols)
     bfd *abfd;
     struct reloc_std_external *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
{
  int r_index;
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  struct aoutdata  *su = &(abfd->tdata.aout_data->a);
  int howto_idx;

  cache_ptr->address = bfd_h_get_32 (abfd, bytes->r_address);

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    r_index =  (bytes->r_index[0] << 16)
      | (bytes->r_index[1] << 8)
	|  bytes->r_index[2];
    r_extern  = (0 != (bytes->r_type[0] & RELOC_STD_BITS_EXTERN_BIG));
    r_pcrel   = (0 != (bytes->r_type[0] & RELOC_STD_BITS_PCREL_BIG));
    r_baserel = (0 != (bytes->r_type[0] & RELOC_STD_BITS_BASEREL_BIG));
    r_jmptable= (0 != (bytes->r_type[0] & RELOC_STD_BITS_JMPTABLE_BIG));
    r_relative= (0 != (bytes->r_type[0] & RELOC_STD_BITS_RELATIVE_BIG));
    r_length  =       (bytes->r_type[0] & RELOC_STD_BITS_LENGTH_BIG)
      			>> RELOC_STD_BITS_LENGTH_SH_BIG;
  } else {
    r_index =  (bytes->r_index[2] << 16)
      | (bytes->r_index[1] << 8)
	|  bytes->r_index[0];
    r_extern  = (0 != (bytes->r_type[0] & RELOC_STD_BITS_EXTERN_LITTLE));
    r_pcrel   = (0 != (bytes->r_type[0] & RELOC_STD_BITS_PCREL_LITTLE));
    r_baserel = (0 != (bytes->r_type[0] & RELOC_STD_BITS_BASEREL_LITTLE));
    r_jmptable= (0 != (bytes->r_type[0] & RELOC_STD_BITS_JMPTABLE_LITTLE));
    r_relative= (0 != (bytes->r_type[0] & RELOC_STD_BITS_RELATIVE_LITTLE));
    r_length  =       (bytes->r_type[0] & RELOC_STD_BITS_LENGTH_LITTLE)
      			>> RELOC_STD_BITS_LENGTH_SH_LITTLE;
  }

  howto_idx = r_length + 4 * r_pcrel + 8 * r_baserel
	      + 16 * r_jmptable + 32 * r_relative;
  BFD_ASSERT (howto_idx < TABLE_SIZE (howto_table_std));
  cache_ptr->howto =  howto_table_std + howto_idx;
  BFD_ASSERT (cache_ptr->howto->type != -1);

  MOVE_ADDRESS(0);
}

/* Read and swap the relocs for a section.  */

boolean
NAME(aout,slurp_reloc_table) (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  unsigned int count;
  bfd_size_type reloc_size;
  PTR relocs;
  arelent *reloc_cache;
  size_t each_size;
  unsigned int counter = 0;
  arelent *cache_ptr;

  if (asect->relocation)
    return true;

  if (asect->flags & SEC_CONSTRUCTOR)
    return true;

  if (asect == obj_datasec (abfd))
    reloc_size = exec_hdr(abfd)->a_drsize;
  else if (asect == obj_textsec (abfd))
    reloc_size = exec_hdr(abfd)->a_trsize;
  else
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  if (bfd_seek (abfd, asect->rel_filepos, SEEK_SET) != 0)
    return false;

  each_size = obj_reloc_entry_size (abfd);

  count = reloc_size / each_size;

  reloc_cache = (arelent *) malloc ((size_t) (count * sizeof (arelent)));
  if (reloc_cache == NULL && count != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  memset (reloc_cache, 0, count * sizeof (arelent));

  relocs = malloc (reloc_size);
  if (relocs == NULL && reloc_size != 0)
    {
      free (reloc_cache);
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  if (bfd_read (relocs, 1, reloc_size, abfd) != reloc_size)
    {
      free (relocs);
      free (reloc_cache);
      return false;
    }

  cache_ptr = reloc_cache;
  if (each_size == RELOC_EXT_SIZE)
    {
      register struct reloc_ext_external *rptr =
	(struct reloc_ext_external *) relocs;

      for (; counter < count; counter++, rptr++, cache_ptr++)
	NAME(aout,swap_ext_reloc_in) (abfd, rptr, cache_ptr, symbols);
    }
  else
    {
      register struct reloc_std_external *rptr =
	(struct reloc_std_external *) relocs;

      for (; counter < count; counter++, rptr++, cache_ptr++)
	NAME(aout,swap_std_reloc_in) (abfd, rptr, cache_ptr, symbols);
    }

  free (relocs);

  asect->relocation = reloc_cache;
  asect->reloc_count = cache_ptr - reloc_cache;

  return true;
}

/* Write out a relocation section into an object file.  */

boolean
NAME(aout,squirt_out_relocs) (abfd, section)
     bfd *abfd;
     asection *section;
{
  arelent **generic;
  unsigned char *native, *natptr;
  size_t each_size;

  unsigned int count = section->reloc_count;
  size_t natsize;

  if (count == 0) return true;

  each_size = obj_reloc_entry_size (abfd);
  natsize = each_size * count;
  native = (unsigned char *) bfd_zalloc (abfd, natsize);
  if (!native) {
    bfd_set_error (bfd_error_no_memory);
    return false;
  }

  generic = section->orelocation;

  if (each_size == RELOC_EXT_SIZE)
    {
      for (natptr = native;
	   count != 0;
	   --count, natptr += each_size, ++generic)
	NAME(aout,swap_ext_reloc_out) (abfd, *generic, (struct reloc_ext_external *)natptr);
    }
  else
    {
      for (natptr = native;
	   count != 0;
	   --count, natptr += each_size, ++generic)
	NAME(aout,swap_std_reloc_out)(abfd, *generic, (struct reloc_std_external *)natptr);
    }

  if ( bfd_write ((PTR) native, 1, natsize, abfd) != natsize) {
    bfd_release(abfd, native);
    return false;
  }
  bfd_release (abfd, native);

  return true;
}

/* This is stupid.  This function should be a boolean predicate */
long
NAME(aout,canonicalize_reloc) (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *tblptr = section->relocation;
  unsigned int count;

  if (!(tblptr || NAME(aout,slurp_reloc_table)(abfd, section, symbols)))
    return -1;

  if (section->flags & SEC_CONSTRUCTOR) {
    arelent_chain *chain = section->constructor_chain;
    for (count = 0; count < section->reloc_count; count ++) {
      *relptr ++ = &chain->relent;
      chain = chain->next;
    }
  }
  else {
    tblptr = section->relocation;

    for (count = 0; count++ < section->reloc_count;)
      {
	*relptr++ = tblptr++;
      }
  }
  *relptr = 0;

  return section->reloc_count;
}

long
NAME(aout,get_reloc_upper_bound) (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (bfd_get_format (abfd) != bfd_object) {
    bfd_set_error (bfd_error_invalid_operation);
    return -1;
  }
  if (asect->flags & SEC_CONSTRUCTOR) {
    return (sizeof (arelent *) * (asect->reloc_count+1));
  }

  if (asect == obj_datasec (abfd))
    return (sizeof (arelent *)
	    * ((exec_hdr(abfd)->a_drsize / obj_reloc_entry_size (abfd))
	       + 1));

  if (asect == obj_textsec (abfd))
    return (sizeof (arelent *)
	    * ((exec_hdr(abfd)->a_trsize / obj_reloc_entry_size (abfd))
	       + 1));

  bfd_set_error (bfd_error_invalid_operation);
  return -1;
}


long
NAME(aout,get_symtab_upper_bound) (abfd)
     bfd *abfd;
{
  if (!NAME(aout,slurp_symbol_table)(abfd))
    return -1;

  return (bfd_get_symcount (abfd)+1) * (sizeof (aout_symbol_type *));
}

/*ARGSUSED*/
 alent *
NAME(aout,get_lineno) (ignore_abfd, ignore_symbol)
     bfd *ignore_abfd;
     asymbol *ignore_symbol;
{
return (alent *)NULL;
}

/*ARGSUSED*/
void
NAME(aout,get_symbol_info) (ignore_abfd, symbol, ret)
     bfd *ignore_abfd;
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);

  if (ret->type == '?')
    {
      int type_code = aout_symbol(symbol)->type & 0xff;
      CONST char *stab_name = aout_stab_name(type_code);
      static char buf[10];

      if (stab_name == NULL)
	{
	  sprintf(buf, "(%d)", type_code);
	  stab_name = buf;
	}
      ret->type = '-';
      ret->stab_other = (unsigned)(aout_symbol(symbol)->other & 0xff);
      ret->stab_desc = (unsigned)(aout_symbol(symbol)->desc & 0xffff);
      ret->stab_name = stab_name;
    }
}

/*ARGSUSED*/
void
NAME(aout,print_symbol) (ignore_abfd, afile, symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *)afile;

  switch (how) {
  case bfd_print_symbol_name:
    if (symbol->name)
      fprintf(file,"%s", symbol->name);
    break;
  case bfd_print_symbol_more:
    fprintf(file,"%4x %2x %2x",(unsigned)(aout_symbol(symbol)->desc & 0xffff),
	    (unsigned)(aout_symbol(symbol)->other & 0xff),
	    (unsigned)(aout_symbol(symbol)->type));
    break;
  case bfd_print_symbol_all:
    {
   CONST char *section_name = symbol->section->name;


      bfd_print_symbol_vandf((PTR)file,symbol);

      fprintf(file," %-5s %04x %02x %02x",
	      section_name,
	      (unsigned)(aout_symbol(symbol)->desc & 0xffff),
	      (unsigned)(aout_symbol(symbol)->other & 0xff),
	      (unsigned)(aout_symbol(symbol)->type  & 0xff));
      if (symbol->name)
        fprintf(file," %s", symbol->name);
    }
    break;
  }
}

/*
 provided a BFD, a section and an offset into the section, calculate
 and return the name of the source file and the line nearest to the
 wanted location.
*/

boolean
NAME(aout,find_nearest_line)
     (abfd, section, symbols, offset, filename_ptr, functionname_ptr, line_ptr)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *line_ptr;
{
  /* Run down the file looking for the filename, function and linenumber */
  asymbol **p;
  static  char buffer[100];
  static  char filename_buffer[200];
  CONST char *directory_name = NULL;
  CONST char *main_file_name = NULL;
  CONST char *current_file_name = NULL;
  CONST char *line_file_name = NULL; /* Value of current_file_name at line number. */
  bfd_vma high_line_vma = ~0;
  bfd_vma low_func_vma = 0;
  asymbol *func = 0;
  *filename_ptr = abfd->filename;
  *functionname_ptr = 0;
  *line_ptr = 0;
  if (symbols != (asymbol **)NULL) {
    for (p = symbols; *p; p++) {
      aout_symbol_type  *q = (aout_symbol_type *)(*p);
    next:
      switch (q->type){
      case N_SO:
	main_file_name = current_file_name = q->symbol.name;
	/* Look ahead to next symbol to check if that too is an N_SO. */
	p++;
	if (*p == NULL)
	  break;
	q = (aout_symbol_type *)(*p);
	if (q->type != (int)N_SO)
	  goto next;

	/* Found a second N_SO  First is directory; second is filename. */
	directory_name = current_file_name;
	main_file_name = current_file_name = q->symbol.name;
	if (obj_textsec(abfd) != section)
	  goto done;
	break;
      case N_SOL:
	current_file_name = q->symbol.name;
	break;

      case N_SLINE:

      case N_DSLINE:
      case N_BSLINE:
	/* We'll keep this if it resolves nearer than the one we have already */
	if (q->symbol.value >= offset &&
	    q->symbol.value < high_line_vma) {
	  *line_ptr = q->desc;
	  high_line_vma = q->symbol.value;
	  line_file_name = current_file_name;
	}
	break;
      case N_FUN:
	{
	  /* We'll keep this if it is nearer than the one we have already */
	  if (q->symbol.value >= low_func_vma &&
	      q->symbol.value <= offset) {
	    low_func_vma = q->symbol.value;
	    func = (asymbol *)q;
	  }
	  if (*line_ptr && func) {
	    CONST char *function = func->name;
	    char *p;

	    /* The caller expects a symbol name.  We actually have a
	       function name, without the leading underscore.  Put the
	       underscore back in, so that the caller gets a symbol
	       name.  */
	    if (bfd_get_symbol_leading_char (abfd) == '\0')
	      strncpy (buffer, function, sizeof (buffer) - 1);
	    else
	      {
		buffer[0] = bfd_get_symbol_leading_char (abfd);
		strncpy (buffer + 1, function, sizeof (buffer) - 2);
	      }
	    buffer[sizeof(buffer)-1] = 0;
	    /* Have to remove : stuff */
	    p = strchr(buffer,':');
	    if (p != NULL) { *p = '\0'; }
	    *functionname_ptr = buffer;
	    goto done;

	  }
	}
	break;
      }
    }
  }

 done:
  if (*line_ptr)
    main_file_name = line_file_name;
  if (main_file_name) {
      if (main_file_name[0] == '/' || directory_name == NULL)
	  *filename_ptr = main_file_name;
      else {
	  sprintf(filename_buffer, "%.140s%.50s",
		  directory_name, main_file_name);
	  *filename_ptr = filename_buffer;
      }
  }
  return true;

}

/*ARGSUSED*/
int
NAME(aout,sizeof_headers) (abfd, execable)
     bfd *abfd;
     boolean execable;
{
  return adata(abfd).exec_bytes_size;
}

/* Free all information we have cached for this BFD.  We can always
   read it again later if we need it.  */

boolean
NAME(aout,bfd_free_cached_info) (abfd)
     bfd *abfd;
{
  asection *o;

  if (bfd_get_format (abfd) != bfd_object)
    return true;

#define FREE(x) if (x != NULL) { free (x); x = NULL; }
  FREE (obj_aout_symbols (abfd));
  FREE (obj_aout_external_syms (abfd));
  FREE (obj_aout_external_strings (abfd));
  for (o = abfd->sections; o != (asection *) NULL; o = o->next)
    FREE (o->relocation);
#undef FREE

  return true;
}

/* a.out link code.  */

/* a.out linker hash table entries.  */

struct aout_link_hash_entry
{
  struct bfd_link_hash_entry root;
  /* Symbol index in output file.  */
  int indx;
};

/* a.out linker hash table.  */

struct aout_link_hash_table
{
  struct bfd_link_hash_table root;
};

static struct bfd_hash_entry *aout_link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *entry,
	   struct bfd_hash_table *table,
	   const char *string));
static boolean aout_link_add_object_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean aout_link_check_archive_element
  PARAMS ((bfd *, struct bfd_link_info *, boolean *));
static boolean aout_link_free_symbols PARAMS ((bfd *));
static boolean aout_link_check_ar_symbols
  PARAMS ((bfd *, struct bfd_link_info *, boolean *pneeded));
static boolean aout_link_add_symbols
  PARAMS ((bfd *, struct bfd_link_info *));

/* Routine to create an entry in an a.out link hash table.  */

static struct bfd_hash_entry *
aout_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct aout_link_hash_entry *ret = (struct aout_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct aout_link_hash_entry *) NULL)
    ret = ((struct aout_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct aout_link_hash_entry)));
  if (ret == (struct aout_link_hash_entry *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return (struct bfd_hash_entry *) ret;
    }

  /* Call the allocation method of the superclass.  */
  ret = ((struct aout_link_hash_entry *)
	 _bfd_link_hash_newfunc ((struct bfd_hash_entry *) ret,
				 table, string));
  if (ret)
    /* Set local fields.  */
    ret->indx = -1;

  return (struct bfd_hash_entry *) ret;
}

/* Create an a.out link hash table.  */

struct bfd_link_hash_table *
NAME(aout,link_hash_table_create) (abfd)
     bfd *abfd;
{
  struct aout_link_hash_table *ret;

  ret = ((struct aout_link_hash_table *)
	 malloc (sizeof (struct aout_link_hash_table)));
  if (ret == (struct aout_link_hash_table *) NULL)
      {
	bfd_set_error (bfd_error_no_memory);
	return (struct bfd_link_hash_table *) NULL;
      }
  if (! _bfd_link_hash_table_init (&ret->root, abfd,
				   aout_link_hash_newfunc))
    {
      free (ret);
      return (struct bfd_link_hash_table *) NULL;
    }
  return &ret->root;
}

/* Look up an entry in an a.out link hash table.  */

#define aout_link_hash_lookup(table, string, create, copy, follow) \
  ((struct aout_link_hash_entry *) \
   bfd_link_hash_lookup (&(table)->root, (string), (create), (copy), (follow)))

/* Traverse an a.out link hash table.  */

#define aout_link_hash_traverse(table, func, info)			\
  (bfd_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct bfd_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the a.out link hash table from the info structure.  This is
   just a cast.  */

#define aout_hash_table(p) ((struct aout_link_hash_table *) ((p)->hash))

/* Given an a.out BFD, add symbols to the global hash table as
   appropriate.  */

boolean
NAME(aout,link_add_symbols) (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  switch (bfd_get_format (abfd))
    {
    case bfd_object:
      return aout_link_add_object_symbols (abfd, info);
    case bfd_archive:
      return _bfd_generic_link_add_archive_symbols
	(abfd, info, aout_link_check_archive_element);
    default:
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }
}

/* Add symbols from an a.out object file.  */

static boolean
aout_link_add_object_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  if (! aout_get_external_symbols (abfd))
    return false;
  if (! aout_link_add_symbols (abfd, info))
    return false;
  if (! info->keep_memory)
    {
      if (! aout_link_free_symbols (abfd))
	return false;
    }
  return true;
}

/* Check a single archive element to see if we need to include it in
   the link.  *PNEEDED is set according to whether this element is
   needed in the link or not.  This is called from
   _bfd_generic_link_add_archive_symbols.  */

static boolean
aout_link_check_archive_element (abfd, info, pneeded)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean *pneeded;
{
  if (! aout_get_external_symbols (abfd))
    return false;

  if (! aout_link_check_ar_symbols (abfd, info, pneeded))
    return false;

  if (*pneeded)
    {
      if (! aout_link_add_symbols (abfd, info))
	return false;
    }

  /* We keep around the symbols even if we aren't going to use this
     object file, because we may want to reread it.  This doesn't
     waste too much memory, because it isn't all that common to read
     an archive element but not need it.  */
  if (! info->keep_memory)
    {
      if (! aout_link_free_symbols (abfd))
	return false;
    }

  return true;
}

/* Free up the internal symbols read from an a.out file.  */

static boolean
aout_link_free_symbols (abfd)
     bfd *abfd;
{
  if (obj_aout_external_syms (abfd) != (struct external_nlist *) NULL)
    {
      free ((PTR) obj_aout_external_syms (abfd));
      obj_aout_external_syms (abfd) = (struct external_nlist *) NULL;
    }
  if (obj_aout_external_strings (abfd) != (char *) NULL)
    {
      free ((PTR) obj_aout_external_strings (abfd));
      obj_aout_external_strings (abfd) = (char *) NULL;
    }
  return true;
}

/* Look through the internal symbols to see if this object file should
   be included in the link.  We should include this object file if it
   defines any symbols which are currently undefined.  If this object
   file defines a common symbol, then we may adjust the size of the
   known symbol but we do not include the object file in the link
   (unless there is some other reason to include it).  */

static boolean
aout_link_check_ar_symbols (abfd, info, pneeded)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean *pneeded;
{
  register struct external_nlist *p;
  struct external_nlist *pend;
  char *strings;

  *pneeded = false;

  /* Look through all the symbols.  */
  p = obj_aout_external_syms (abfd);
  pend = p + obj_aout_external_sym_count (abfd);
  strings = obj_aout_external_strings (abfd);
  for (; p < pend; p++)
    {
      int type = bfd_h_get_8 (abfd, p->e_type);
      const char *name;
      struct bfd_link_hash_entry *h;

      /* Ignore symbols that are not externally visible.  */
      if ((type & N_EXT) == 0)
	{
	  if (type == N_WARNING
	      || type == N_INDR)
	    ++p;
	  continue;
	}

      name = strings + GET_WORD (abfd, p->e_strx);
      h = bfd_link_hash_lookup (info->hash, name, false, false, true);

      /* We are only interested in symbols that are currently
	 undefined or common.  */
      if (h == (struct bfd_link_hash_entry *) NULL
	  || (h->type != bfd_link_hash_undefined
	      && h->type != bfd_link_hash_common))
	{
	  if (type == (N_INDR | N_EXT))
	    ++p;
	  continue;
	}

      if (type == (N_TEXT | N_EXT)
	  || type == (N_DATA | N_EXT)
	  || type == (N_BSS | N_EXT)
	  || type == (N_ABS | N_EXT)
	  || type == (N_INDR | N_EXT))
	{
	  /* This object file defines this symbol.  We must link it
	     in.  This is true regardless of whether the current
	     definition of the symbol is undefined or common.  If the
	     current definition is common, we have a case in which we
	     have already seen an object file including
	         int a;
	     and this object file from the archive includes
	         int a = 5;
	     In such a case we must include this object file.  */
	  if (! (*info->callbacks->add_archive_element) (info, abfd, name))
	    return false;
	  *pneeded = true;
	  return true;
	}

      if (type == (N_UNDF | N_EXT))
	{
	  bfd_vma value;

	  value = GET_WORD (abfd, p->e_value);
	  if (value != 0)
	    {
	      /* This symbol is common in the object from the archive
		 file.  */
	      if (h->type == bfd_link_hash_undefined)
		{
		  bfd *symbfd;

		  symbfd = h->u.undef.abfd;
		  if (symbfd == (bfd *) NULL)
		    {
		      /* This symbol was created as undefined from
			 outside BFD.  We assume that we should link
			 in the object file.  This is done for the -u
			 option in the linker.  */
		      if (! (*info->callbacks->add_archive_element) (info,
								     abfd,
								     name))
			return false;
		      *pneeded = true;
		      return true;
		    }
		  /* Turn the current link symbol into a common
		     symbol.  It is already on the undefs list.  */
		  h->type = bfd_link_hash_common;
		  h->u.c.size = value;
		  h->u.c.section = bfd_make_section_old_way (symbfd,
							     "COMMON");
		}
	      else
		{
		  /* Adjust the size of the common symbol if
		     necessary.  */
		  if (value > h->u.c.size)
		    h->u.c.size = value;
		}
	    }
	}
    }

  /* We do not need this object file.  */
  return true;
}

/* Add all symbols from an object file to the hash table.  */

static boolean
aout_link_add_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  bfd_size_type sym_count;
  char *strings;
  boolean copy;
  struct aout_link_hash_entry **sym_hash;
  register struct external_nlist *p;
  struct external_nlist *pend;

  sym_count = obj_aout_external_sym_count (abfd);
  strings = obj_aout_external_strings (abfd);
  if (info->keep_memory)
    copy = false;
  else
    copy = true;

  /* We keep a list of the linker hash table entries that correspond
     to particular symbols.  We could just look them up in the hash
     table, but keeping the list is more efficient.  Perhaps this
     should be conditional on info->keep_memory.  */
  sym_hash = ((struct aout_link_hash_entry **)
	      bfd_alloc (abfd,
			 ((size_t) sym_count
			  * sizeof (struct aout_link_hash_entry *))));
  if (!sym_hash)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  obj_aout_sym_hashes (abfd) = sym_hash;

  p = obj_aout_external_syms (abfd);
  pend = p + sym_count;
  for (; p < pend; p++, sym_hash++)
    {
      int type;
      const char *name;
      bfd_vma value;
      asection *section;
      flagword flags;
      const char *string;

      *sym_hash = NULL;

      type = bfd_h_get_8 (abfd, p->e_type);

      /* Ignore debugging symbols.  */
      if ((type & N_STAB) != 0)
	continue;

      /* Ignore symbols that are not external.  */
      if ((type & N_EXT) == 0
	  && type != N_WARNING
	  && type != N_SETA
	  && type != N_SETT
	  && type != N_SETD
	  && type != N_SETB)
	{
	  /* If this is an N_INDR symbol we must skip the next entry,
	     which is the symbol to indirect to (actually, an N_INDR
	     symbol without N_EXT set is pretty useless).  */
	  if (type == N_INDR)
	    {
	      ++p;
	      ++sym_hash;
	    }
	  continue;
	}

      /* Ignore N_FN symbols (these appear to have N_EXT set).  */
      if (type == N_FN)
	continue;

      name = strings + GET_WORD (abfd, p->e_strx);
      value = GET_WORD (abfd, p->e_value);
      flags = BSF_GLOBAL;
      string = NULL;
      switch (type)
	{
	default:
	  abort ();
	case N_UNDF | N_EXT:
	  if (value != 0)
	    section = &bfd_com_section;
	  else
	    section = &bfd_und_section;
	  break;
	case N_ABS | N_EXT:
	  section = &bfd_abs_section;
	  break;
	case N_TEXT | N_EXT:
	  section = obj_textsec (abfd);
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_DATA | N_EXT:
	  section = obj_datasec (abfd);
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_BSS | N_EXT:
	  section = obj_bsssec (abfd);
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_INDR | N_EXT:
	  /* An indirect symbol.  The next symbol is the symbol
	     which this one really is.  */
	  BFD_ASSERT (p + 1 < pend);
	  ++p;
	  string = strings + GET_WORD (abfd, p->e_strx);
	  section = &bfd_ind_section;
	  flags |= BSF_INDIRECT;
	  break;
	case N_COMM | N_EXT:
	  section = &bfd_com_section;
	  break;
	case N_SETA: case N_SETA | N_EXT:
	  section = &bfd_abs_section;
	  flags |= BSF_CONSTRUCTOR;
	  break;
	case N_SETT: case N_SETT | N_EXT:
	  section = obj_textsec (abfd);
	  flags |= BSF_CONSTRUCTOR;
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_SETD: case N_SETD | N_EXT:
	  section = obj_datasec (abfd);
	  flags |= BSF_CONSTRUCTOR;
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_SETB: case N_SETB | N_EXT:
	  section = obj_bsssec (abfd);
	  flags |= BSF_CONSTRUCTOR;
	  value -= bfd_get_section_vma (abfd, section);
	  break;
	case N_WARNING:
	  /* A warning symbol.  The next symbol is the one to warn
	     about.  */
	  BFD_ASSERT (p + 1 < pend);
	  ++p;
	  string = name;
	  name = strings + GET_WORD (abfd, p->e_strx);
	  section = &bfd_und_section;
	  flags |= BSF_WARNING;
	  break;
	}

      if (! (_bfd_generic_link_add_one_symbol
	     (info, abfd, name, flags, section, value, string, copy, false,
	      (struct bfd_link_hash_entry **) sym_hash)))
	return false;

      if (type == (N_INDR | N_EXT) || type == N_WARNING)
	++sym_hash;
    }

  return true;
}

/* During the final link step we need to pass around a bunch of
   information, so we do it in an instance of this structure.  */

struct aout_final_link_info
{
  /* General link information.  */
  struct bfd_link_info *info;
  /* Output bfd.  */
  bfd *output_bfd;
  /* Reloc file positions.  */
  file_ptr treloff, dreloff;
  /* File position of symbols.  */
  file_ptr symoff;
  /* String table.  */
  struct stringtab_data strtab;
};

static boolean aout_link_input_bfd
  PARAMS ((struct aout_final_link_info *, bfd *input_bfd));
static boolean aout_link_write_symbols
  PARAMS ((struct aout_final_link_info *, bfd *input_bfd, int *symbol_map));
static boolean aout_link_write_other_symbol
  PARAMS ((struct aout_link_hash_entry *, PTR));
static boolean aout_link_input_section
  PARAMS ((struct aout_final_link_info *, bfd *input_bfd,
	   asection *input_section, file_ptr *reloff_ptr,
	   bfd_size_type rel_size, int *symbol_map));
static boolean aout_link_input_section_std
  PARAMS ((struct aout_final_link_info *, bfd *input_bfd,
	   asection *input_section, struct reloc_std_external *,
	   bfd_size_type rel_size, bfd_byte *contents, int *symbol_map));
static boolean aout_link_input_section_ext
  PARAMS ((struct aout_final_link_info *, bfd *input_bfd,
	   asection *input_section, struct reloc_ext_external *,
	   bfd_size_type rel_size, bfd_byte *contents, int *symbol_map));
static INLINE asection *aout_reloc_index_to_section
  PARAMS ((bfd *, int));
static boolean aout_link_reloc_link_order
  PARAMS ((struct aout_final_link_info *, asection *,
	   struct bfd_link_order *));

/* Do the final link step.  This is called on the output BFD.  The
   INFO structure should point to a list of BFDs linked through the
   link_next field which can be used to find each BFD which takes part
   in the output.  Also, each section in ABFD should point to a list
   of bfd_link_order structures which list all the input sections for
   the output section.  */

boolean
NAME(aout,final_link) (abfd, info, callback)
     bfd *abfd;
     struct bfd_link_info *info;
     void (*callback) PARAMS ((bfd *, file_ptr *, file_ptr *, file_ptr *));
{
  struct aout_final_link_info aout_info;
  register bfd *sub;
  bfd_size_type text_size;
  file_ptr text_end;
  register struct bfd_link_order *p;
  asection *o;
  boolean have_link_order_relocs;

  aout_info.info = info;
  aout_info.output_bfd = abfd;

  if (! info->relocateable)
    {
      exec_hdr (abfd)->a_trsize = 0;
      exec_hdr (abfd)->a_drsize = 0;
    }
  else
    {
      bfd_size_type trsize, drsize;

      /* Count up the relocation sizes.  */
      trsize = 0;
      drsize = 0;
      for (sub = info->input_bfds; sub != (bfd *) NULL; sub = sub->link_next)
	{
	  if (bfd_get_flavour (abfd) == bfd_target_aout_flavour)
	    {
	      trsize += exec_hdr (sub)->a_trsize;
	      drsize += exec_hdr (sub)->a_drsize;
	    }
	  else
	    {
	      /* FIXME: We need to identify the .text and .data sections
		 and call get_reloc_upper_bound and canonicalize_reloc to
		 work out the number of relocs needed, and then multiply
		 by the reloc size.  */
	      abort ();
	    }
	}
      if (obj_textsec (abfd) != (asection *) NULL)
	trsize += (_bfd_count_link_order_relocs (obj_textsec (abfd)
						 ->link_order_head)
		   * obj_reloc_entry_size (abfd));
      exec_hdr (abfd)->a_trsize = trsize;
      if (obj_datasec (abfd) != (asection *) NULL)
	drsize += (_bfd_count_link_order_relocs (obj_datasec (abfd)
						 ->link_order_head)
		   * obj_reloc_entry_size (abfd));
      exec_hdr (abfd)->a_drsize = drsize;
    }

  exec_hdr (abfd)->a_entry = bfd_get_start_address (abfd);

  /* Adjust the section sizes and vmas according to the magic number.
     This sets a_text, a_data and a_bss in the exec_hdr and sets the
     filepos for each section.  */
  if (! NAME(aout,adjust_sizes_and_vmas) (abfd, &text_size, &text_end))
    return false;

  /* The relocation and symbol file positions differ among a.out
     targets.  We are passed a callback routine from the backend
     specific code to handle this.
     FIXME: At this point we do not know how much space the symbol
     table will require.  This will not work for any (nonstandard)
     a.out target that needs to know the symbol table size before it
     can compute the relocation file positions.  This may or may not
     be the case for the hp300hpux target, for example.  */
  (*callback) (abfd, &aout_info.treloff, &aout_info.dreloff,
	       &aout_info.symoff);
  obj_textsec (abfd)->rel_filepos = aout_info.treloff;
  obj_datasec (abfd)->rel_filepos = aout_info.dreloff;
  obj_sym_filepos (abfd) = aout_info.symoff;

  /* We keep a count of the symbols as we output them.  */
  obj_aout_external_sym_count (abfd) = 0;

  /* We accumulate the string table as we write out the symbols.  */
  stringtab_init (&aout_info.strtab);

  /* The most time efficient way to do the link would be to read all
     the input object files into memory and then sort out the
     information into the output file.  Unfortunately, that will
     probably use too much memory.  Another method would be to step
     through everything that composes the text section and write it
     out, and then everything that composes the data section and write
     it out, and then write out the relocs, and then write out the
     symbols.  Unfortunately, that requires reading stuff from each
     input file several times, and we will not be able to keep all the
     input files open simultaneously, and reopening them will be slow.

     What we do is basically process one input file at a time.  We do
     everything we need to do with an input file once--copy over the
     section contents, handle the relocation information, and write
     out the symbols--and then we throw away the information we read
     from it.  This approach requires a lot of lseeks of the output
     file, which is unfortunate but still faster than reopening a lot
     of files.

     We use the output_has_begun field of the input BFDs to see
     whether we have already handled it.  */
  for (sub = info->input_bfds; sub != (bfd *) NULL; sub = sub->link_next)
    sub->output_has_begun = false;

  have_link_order_relocs = false;
  for (o = abfd->sections; o != (asection *) NULL; o = o->next)
    {
      for (p = o->link_order_head;
	   p != (struct bfd_link_order *) NULL;
	   p = p->next)
	{
	  if (p->type == bfd_indirect_link_order
	      && (bfd_get_flavour (p->u.indirect.section->owner)
		  == bfd_target_aout_flavour))
	    {
	      bfd *input_bfd;

	      input_bfd = p->u.indirect.section->owner;
	      if (! input_bfd->output_has_begun)
		{
		  if (! aout_link_input_bfd (&aout_info, input_bfd))
		    return false;
		  input_bfd->output_has_begun = true;
		}
	    }
	  else if (p->type == bfd_section_reloc_link_order
		   || p->type == bfd_symbol_reloc_link_order)
	    {
	      /* These are handled below.  */
	      have_link_order_relocs = true;
	    }
	  else
	    {
	      if (! _bfd_default_link_order (abfd, info, o, p))
		return false;
	    }
	}
    }

  /* Write out any symbols that we have not already written out.  */
  aout_link_hash_traverse (aout_hash_table (info),
			   aout_link_write_other_symbol,
			   (PTR) &aout_info);

  /* Now handle any relocs we were asked to create by the linker.
     These did not come from any input file.  We must do these after
     we have written out all the symbols, so that we know the symbol
     indices to use.  */
  if (have_link_order_relocs)
    {
      for (o = abfd->sections; o != (asection *) NULL; o = o->next)
	{
	  for (p = o->link_order_head;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      if (p->type == bfd_section_reloc_link_order
		  || p->type == bfd_symbol_reloc_link_order)
		{
		  if (! aout_link_reloc_link_order (&aout_info, o, p))
		    return false;
		}
	    }
	}
    }

  /* Update the header information.  */
  abfd->symcount = obj_aout_external_sym_count (abfd);
  exec_hdr (abfd)->a_syms = abfd->symcount * EXTERNAL_NLIST_SIZE;
  obj_str_filepos (abfd) = obj_sym_filepos (abfd) + exec_hdr (abfd)->a_syms;
  obj_textsec (abfd)->reloc_count =
    exec_hdr (abfd)->a_trsize / obj_reloc_entry_size (abfd);
  obj_datasec (abfd)->reloc_count =
    exec_hdr (abfd)->a_drsize / obj_reloc_entry_size (abfd);

  /* Write out the string table.  */
  if (bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET) != 0)
    return false;
  return emit_strtab (abfd, &aout_info.strtab);
}

/* Link an a.out input BFD into the output file.  */

static boolean
aout_link_input_bfd (finfo, input_bfd)
     struct aout_final_link_info *finfo;
     bfd *input_bfd;
{
  bfd_size_type sym_count;
  int *symbol_map = NULL;

  BFD_ASSERT (bfd_get_format (input_bfd) == bfd_object);

  /* Get the symbols.  We probably have them already, unless
     finfo->info->keep_memory is false.  */
  if (! aout_get_external_symbols (input_bfd))
    return false;

  sym_count = obj_aout_external_sym_count (input_bfd);
  symbol_map = (int *) malloc ((size_t) sym_count * sizeof (int));
  if (symbol_map == NULL && sym_count != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  /* Write out the symbols and get a map of the new indices.  */
  if (! aout_link_write_symbols (finfo, input_bfd, symbol_map))
    goto error_return;

  /* Relocate and write out the sections.  */
  if (! aout_link_input_section (finfo, input_bfd,
				 obj_textsec (input_bfd),
				 &finfo->treloff,
				 exec_hdr (input_bfd)->a_trsize,
				 symbol_map)
      || ! aout_link_input_section (finfo, input_bfd,
				    obj_datasec (input_bfd),
				    &finfo->dreloff,
				    exec_hdr (input_bfd)->a_drsize,
				    symbol_map))
    goto error_return;

  /* If we are not keeping memory, we don't need the symbols any
     longer.  We still need them if we are keeping memory, because the
     strings in the hash table point into them.  */
  if (! finfo->info->keep_memory)
    {
      if (! aout_link_free_symbols (input_bfd))
	goto error_return;
    }

  if (symbol_map != NULL)
    free (symbol_map);
  return true;
 error_return:
  if (symbol_map != NULL)
    free (symbol_map);
  return false;
}

/* Adjust and write out the symbols for an a.out file.  Set the new
   symbol indices into a symbol_map.  */

static boolean
aout_link_write_symbols (finfo, input_bfd, symbol_map)
     struct aout_final_link_info *finfo;
     bfd *input_bfd;
     int *symbol_map;
{
  bfd *output_bfd;
  bfd_size_type sym_count;
  char *strings;
  enum bfd_link_strip strip;
  enum bfd_link_discard discard;
  struct external_nlist *output_syms = NULL;
  struct external_nlist *outsym;
  register struct external_nlist *sym;
  struct external_nlist *sym_end;
  struct aout_link_hash_entry **sym_hash;
  boolean pass;
  boolean skip_indirect;

  output_bfd = finfo->output_bfd;
  sym_count = obj_aout_external_sym_count (input_bfd);
  strings = obj_aout_external_strings (input_bfd);
  strip = finfo->info->strip;
  discard = finfo->info->discard;
  output_syms = ((struct external_nlist *)
		 malloc ((size_t) (sym_count + 1) * EXTERNAL_NLIST_SIZE));
  if (output_syms == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  outsym = output_syms;

  /* First write out a symbol for this object file, unless we are
     discarding such symbols.  */
  if (strip != strip_all
      && (strip != strip_some
	  || bfd_hash_lookup (finfo->info->keep_hash, input_bfd->filename,
			      false, false) != NULL)
      && discard != discard_all)
    {
      bfd_h_put_8 (output_bfd, N_TEXT, outsym->e_type);
      bfd_h_put_8 (output_bfd, 0, outsym->e_other);
      bfd_h_put_16 (output_bfd, (bfd_vma) 0, outsym->e_desc);
      PUT_WORD (output_bfd,
		add_to_stringtab (output_bfd, input_bfd->filename,
				  &finfo->strtab),
		outsym->e_strx);
      PUT_WORD (output_bfd,
		(bfd_get_section_vma (output_bfd,
				      obj_textsec (input_bfd)->output_section)
		 + obj_textsec (input_bfd)->output_offset),
		outsym->e_value);
      ++obj_aout_external_sym_count (output_bfd);
      ++outsym;
    }

  pass = false;
  skip_indirect = false;
  sym = obj_aout_external_syms (input_bfd);
  sym_end = sym + sym_count;
  sym_hash = obj_aout_sym_hashes (input_bfd);
  for (; sym < sym_end; sym++, sym_hash++, symbol_map++)
    {
      const char *name;
      int type;
      struct aout_link_hash_entry *h;
      boolean skip;
      asection *symsec;
      bfd_vma val = 0;

      *symbol_map = -1;

      type = bfd_h_get_8 (input_bfd, sym->e_type);
      name = strings + GET_WORD (input_bfd, sym->e_strx);

      h = NULL;

      if (pass)
	{
	  /* Pass this symbol through.  It is the target of an
	     indirect or warning symbol.  */
	  val = GET_WORD (input_bfd, sym->e_value);
	  pass = false;
	}
      else if (skip_indirect)
	{
	  /* Skip this symbol, which is the target of an indirect
	     symbol that we have changed to no longer be an indirect
	     symbol.  */
	  skip_indirect = false;
	  continue;
	}
      else
	{
	  struct aout_link_hash_entry *hresolve;

	  /* We have saved the hash table entry for this symbol, if
	     there is one.  Note that we could just look it up again
	     in the hash table, provided we first check that it is an
	     external symbol. */
	  h = *sym_hash;

	  /* If this is an indirect or warning symbol, then change
	     hresolve to the base symbol.  We also change *sym_hash so
	     that the relocation routines relocate against the real
	     symbol.  */
	  hresolve = h;
	  if (h != (struct aout_link_hash_entry *) NULL
	      && (h->root.type == bfd_link_hash_indirect
		  || h->root.type == bfd_link_hash_warning))
	    {
	      hresolve = (struct aout_link_hash_entry *) h->root.u.i.link;
	      while (hresolve->root.type == bfd_link_hash_indirect)
		hresolve = ((struct aout_link_hash_entry *)
			    hresolve->root.u.i.link);
	      *sym_hash = hresolve;
	    }

	  /* If the symbol has already been written out, skip it.  */
	  if (h != (struct aout_link_hash_entry *) NULL
	      && h->root.written)
	    {
	      *symbol_map = h->indx;
	      continue;
	    }

	  /* See if we are stripping this symbol.  */
	  skip = false;
	  switch (strip)
	    {
	    case strip_none:
	      break;
	    case strip_debugger:
	      if ((type & N_STAB) != 0)
		skip = true;
	      break;
	    case strip_some:
	      if (bfd_hash_lookup (finfo->info->keep_hash, name, false, false)
		  == NULL)
		skip = true;
	      break;
	    case strip_all:
	      skip = true;
	      break;
	    }
	  if (skip)
	    {
	      if (h != (struct aout_link_hash_entry *) NULL)
		h->root.written = true;
	      continue;
	    }

	  /* Get the value of the symbol.  */
	  if ((type & N_TYPE) == N_TEXT)
	    symsec = obj_textsec (input_bfd);
	  else if ((type & N_TYPE) == N_DATA)
	    symsec = obj_datasec (input_bfd);
	  else if ((type & N_TYPE) == N_BSS)
	    symsec = obj_bsssec (input_bfd);
	  else if ((type & N_TYPE) == N_ABS)
	    symsec = &bfd_abs_section;
	  else if (((type & N_TYPE) == N_INDR
		    && (hresolve == (struct aout_link_hash_entry *) NULL
			|| (hresolve->root.type != bfd_link_hash_defined
			    && hresolve->root.type != bfd_link_hash_common)))
		   || type == N_WARNING)
	    {
	      /* Pass the next symbol through unchanged.  The
		 condition above for indirect symbols is so that if
		 the indirect symbol was defined, we output it with
		 the correct definition so the debugger will
		 understand it.  */
	      pass = true;
	      val = GET_WORD (input_bfd, sym->e_value);
	      symsec = NULL;
	    }
	  else if ((type & N_STAB) != 0)
	    {
	      val = GET_WORD (input_bfd, sym->e_value);
	      symsec = NULL;
	    }
	  else
	    {
	      /* If we get here with an indirect symbol, it means that
		 we are outputting it with a real definition.  In such
		 a case we do not want to output the next symbol,
		 which is the target of the indirection.  */
	      if ((type & N_TYPE) == N_INDR)
		skip_indirect = true;

	      /* We need to get the value from the hash table.  We use
		 hresolve so that if we have defined an indirect
		 symbol we output the final definition.  */
	      if (h == (struct aout_link_hash_entry *) NULL)
		val = 0;
	      else if (hresolve->root.type == bfd_link_hash_defined)
		{
		  asection *input_section;
		  asection *output_section;

		  /* This case means a common symbol which was turned
		     into a defined symbol.  */
		  input_section = hresolve->root.u.def.section;
		  output_section = input_section->output_section;
		  BFD_ASSERT (output_section == &bfd_abs_section
			      || output_section->owner == output_bfd);
		  val = (hresolve->root.u.def.value
			 + bfd_get_section_vma (output_bfd, output_section)
			 + input_section->output_offset);

		  /* Get the correct type based on the section.  If
		     this is a constructed set, force it to be
		     globally visible.  */
		  if (type == N_SETT
		      || type == N_SETD
		      || type == N_SETB
		      || type == N_SETA)
		    type |= N_EXT;

		  type &=~ N_TYPE;

		  if (output_section == obj_textsec (output_bfd))
		    type |= N_TEXT;
		  else if (output_section == obj_datasec (output_bfd))
		    type |= N_DATA;
		  else if (output_section == obj_bsssec (output_bfd))
		    type |= N_BSS;
		  else
		    type |= N_ABS;
		}
	      else if (hresolve->root.type == bfd_link_hash_common)
		val = hresolve->root.u.c.size;
	      else
		val = 0;

	      symsec = NULL;
	    }
	  if (symsec != (asection *) NULL)
	    val = (symsec->output_section->vma
		   + symsec->output_offset
		   + (GET_WORD (input_bfd, sym->e_value)
		      - symsec->vma));

	  /* If this is a global symbol set the written flag, and if
	     it is a local symbol see if we should discard it.  */
	  if (h != (struct aout_link_hash_entry *) NULL)
	    {
	      h->root.written = true;
	      h->indx = obj_aout_external_sym_count (output_bfd);
	    }
	  else
	    {
	      switch (discard)
		{
		case discard_none:
		  break;
		case discard_l:
		  if (*name == *finfo->info->lprefix
		      && (finfo->info->lprefix_len == 1
			  || strncmp (name, finfo->info->lprefix,
				      finfo->info->lprefix_len) == 0))
		    skip = true;
		  break;
		case discard_all:
		  skip = true;
		  break;
		}
	      if (skip)
		{
		  pass = false;
		  continue;
		}
	    }
	}

      /* Copy this symbol into the list of symbols we are going to
	 write out.  */
      bfd_h_put_8 (output_bfd, type, outsym->e_type);
      bfd_h_put_8 (output_bfd, bfd_h_get_8 (input_bfd, sym->e_other),
		   outsym->e_other);
      bfd_h_put_16 (output_bfd, bfd_h_get_16 (input_bfd, sym->e_desc),
		    outsym->e_desc);
      if (! finfo->info->keep_memory)
	{
	  /* name points into a string table which we are going to
	     free.  If there is a hash table entry, use that string.
	     Otherwise, copy name into memory.  */
	  if (h != (struct aout_link_hash_entry *) NULL)
	    name = (*sym_hash)->root.root.string;
	  else
	    {
	      char *n;

	      n = bfd_alloc (output_bfd, strlen (name) + 1);
	      strcpy (n, name);
	      name = n;
	    }
	}
      PUT_WORD (output_bfd,
		add_to_stringtab (output_bfd, name, &finfo->strtab),
		outsym->e_strx);
      PUT_WORD (output_bfd, val, outsym->e_value);
      *symbol_map = obj_aout_external_sym_count (output_bfd);
      ++obj_aout_external_sym_count (output_bfd);
      ++outsym;
    }

  /* Write out the output symbols we have just constructed.  */
  if (outsym > output_syms)
    {
      bfd_size_type outsym_count;

      if (bfd_seek (output_bfd, finfo->symoff, SEEK_SET) != 0)
	goto error_return;
      outsym_count = outsym - output_syms;
      if (bfd_write ((PTR) output_syms, (bfd_size_type) EXTERNAL_NLIST_SIZE,
		     (bfd_size_type) outsym_count, output_bfd)
	  != outsym_count * EXTERNAL_NLIST_SIZE)
	goto error_return;
      finfo->symoff += outsym_count * EXTERNAL_NLIST_SIZE;
    }

  if (output_syms != NULL)
    free (output_syms);
  return true;
 error_return:
  if (output_syms != NULL)
    free (output_syms);
  return false;
}

/* Write out a symbol that was not associated with an a.out input
   object.  */

static boolean
aout_link_write_other_symbol (h, data)
     struct aout_link_hash_entry *h;
     PTR data;
{
  struct aout_final_link_info *finfo = (struct aout_final_link_info *) data;
  bfd *output_bfd;
  int type;
  bfd_vma val;
  struct external_nlist outsym;

  if (h->root.written)
    return true;

  h->root.written = true;

  if (finfo->info->strip == strip_all
      || (finfo->info->strip == strip_some
	  && bfd_hash_lookup (finfo->info->keep_hash, h->root.root.string,
			      false, false) == NULL))
    return true;

  output_bfd = finfo->output_bfd;

  switch (h->root.type)
    {
    default:
    case bfd_link_hash_new:
      abort ();
      /* Avoid variable not initialized warnings.  */
      return true;
    case bfd_link_hash_undefined:
      type = N_UNDF | N_EXT;
      val = 0;
      break;
    case bfd_link_hash_defined:
      {
	asection *sec;

	sec = h->root.u.def.section;
	BFD_ASSERT (sec == &bfd_abs_section
		    || sec->owner == output_bfd);
	if (sec == obj_textsec (output_bfd))
	  type = N_TEXT | N_EXT;
	else if (sec == obj_datasec (output_bfd))
	  type = N_DATA | N_EXT;
	else if (sec == obj_bsssec (output_bfd))
	  type = N_BSS | N_EXT;
	else
	  type = N_ABS | N_EXT;
	val = (h->root.u.def.value
	       + sec->output_section->vma
	       + sec->output_offset);
      }
      break;
    case bfd_link_hash_common:
      type = N_UNDF | N_EXT;
      val = h->root.u.c.size;
      break;
    case bfd_link_hash_indirect:
    case bfd_link_hash_warning:
      /* FIXME: Ignore these for now.  The circumstances under which
	 they should be written out are not clear to me.  */
      return true;
    }

  bfd_h_put_8 (output_bfd, type, outsym.e_type);
  bfd_h_put_8 (output_bfd, 0, outsym.e_other);
  bfd_h_put_16 (output_bfd, 0, outsym.e_desc);
  PUT_WORD (output_bfd,
	    add_to_stringtab (output_bfd, h->root.root.string, &finfo->strtab),
	    outsym.e_strx);
  PUT_WORD (output_bfd, val, outsym.e_value);

  if (bfd_seek (output_bfd, finfo->symoff, SEEK_SET) != 0
      || bfd_write ((PTR) &outsym, (bfd_size_type) EXTERNAL_NLIST_SIZE,
		    (bfd_size_type) 1, output_bfd) != EXTERNAL_NLIST_SIZE)
    {
      /* FIXME: No way to handle errors.  */
      abort ();
    }

  finfo->symoff += EXTERNAL_NLIST_SIZE;
  h->indx = obj_aout_external_sym_count (output_bfd);
  ++obj_aout_external_sym_count (output_bfd);

  return true;
}

/* Link an a.out section into the output file.  */

static boolean
aout_link_input_section (finfo, input_bfd, input_section, reloff_ptr,
			 rel_size, symbol_map)
     struct aout_final_link_info *finfo;
     bfd *input_bfd;
     asection *input_section;
     file_ptr *reloff_ptr;
     bfd_size_type rel_size;
     int *symbol_map;
{
  bfd_size_type input_size;
  bfd_byte *contents = NULL;
  PTR relocs = NULL;

  /* Get the section contents.  */
  input_size = bfd_section_size (input_bfd, input_section);
  contents = (bfd_byte *) malloc (input_size);
  if (contents == NULL && input_size != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  if (! bfd_get_section_contents (input_bfd, input_section, (PTR) contents,
				  (file_ptr) 0, input_size))
    goto error_return;

  /* Read in the relocs.  */
  relocs = (PTR) malloc (rel_size);
  if (relocs == NULL && rel_size != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  if (bfd_seek (input_bfd, input_section->rel_filepos, SEEK_SET) != 0
      || bfd_read (relocs, 1, rel_size, input_bfd) != rel_size)
    goto error_return;

  /* Relocate the section contents.  */
  if (obj_reloc_entry_size (input_bfd) == RELOC_STD_SIZE)
    {
      if (! aout_link_input_section_std (finfo, input_bfd, input_section,
					 (struct reloc_std_external *) relocs,
					 rel_size, contents, symbol_map))
	goto error_return;
    }
  else
    {
      if (! aout_link_input_section_ext (finfo, input_bfd, input_section,
					 (struct reloc_ext_external *) relocs,
					 rel_size, contents, symbol_map))
	goto error_return;
    }

  /* Write out the section contents.  */
  if (! bfd_set_section_contents (finfo->output_bfd,
				  input_section->output_section,
				  (PTR) contents,
				  input_section->output_offset,
				  input_size))
    goto error_return;

  /* If we are producing relocateable output, the relocs were
     modified, and we now write them out.  */
  if (finfo->info->relocateable)
    {
      if (bfd_seek (finfo->output_bfd, *reloff_ptr, SEEK_SET) != 0)
	goto error_return;
      if (bfd_write (relocs, (bfd_size_type) 1, rel_size, finfo->output_bfd)
	  != rel_size)
	goto error_return;
      *reloff_ptr += rel_size;

      /* Assert that the relocs have not run into the symbols, and
	 that if these are the text relocs they have not run into the
	 data relocs.  */
      BFD_ASSERT (*reloff_ptr <= obj_sym_filepos (finfo->output_bfd)
		  && (reloff_ptr != &finfo->treloff
		      || (*reloff_ptr
			  <= obj_datasec (finfo->output_bfd)->rel_filepos)));
    }

  if (relocs != NULL)
    free (relocs);
  if (contents != NULL)
    free (contents);
  return true;
 error_return:
  if (relocs != NULL)
    free (relocs);
  if (contents != NULL)
    free (contents);
  return false;
}

/* Get the section corresponding to a reloc index.  */

static INLINE asection *
aout_reloc_index_to_section (abfd, indx)
     bfd *abfd;
     int indx;
{
  switch (indx & N_TYPE)
    {
    case N_TEXT:
      return obj_textsec (abfd);
    case N_DATA:
      return obj_datasec (abfd);
    case N_BSS:
      return obj_bsssec (abfd);
    case N_ABS:
    case N_UNDF:
      return &bfd_abs_section;
    default:
      abort ();
    }
}

/* Relocate an a.out section using standard a.out relocs.  */

static boolean
aout_link_input_section_std (finfo, input_bfd, input_section, relocs,
			     rel_size, contents, symbol_map)
     struct aout_final_link_info *finfo;
     bfd *input_bfd;
     asection *input_section;
     struct reloc_std_external *relocs;
     bfd_size_type rel_size;
     bfd_byte *contents;
     int *symbol_map;
{
  bfd *output_bfd;
  boolean relocateable;
  struct external_nlist *syms;
  char *strings;
  struct aout_link_hash_entry **sym_hashes;
  bfd_size_type reloc_count;
  register struct reloc_std_external *rel;
  struct reloc_std_external *rel_end;

  output_bfd = finfo->output_bfd;

  BFD_ASSERT (obj_reloc_entry_size (input_bfd) == RELOC_STD_SIZE);
  BFD_ASSERT (input_bfd->xvec->header_byteorder_big_p
	      == output_bfd->xvec->header_byteorder_big_p);

  relocateable = finfo->info->relocateable;
  syms = obj_aout_external_syms (input_bfd);
  strings = obj_aout_external_strings (input_bfd);
  sym_hashes = obj_aout_sym_hashes (input_bfd);

  reloc_count = rel_size / RELOC_STD_SIZE;
  rel = relocs;
  rel_end = rel + reloc_count;
  for (; rel < rel_end; rel++)
    {
      bfd_vma r_addr;
      int r_index;
      int r_extern;
      int r_pcrel;
      int r_baserel;
      int r_jmptable;
      int r_relative;
      int r_length;
      int howto_idx;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_addr = GET_SWORD (input_bfd, rel->r_address);

      if (input_bfd->xvec->header_byteorder_big_p)
	{
	  r_index   =  ((rel->r_index[0] << 16)
			| (rel->r_index[1] << 8)
			| rel->r_index[2]);
	  r_extern  = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_BIG));
	  r_pcrel   = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_BIG));
	  r_baserel = (0 != (rel->r_type[0] & RELOC_STD_BITS_BASEREL_BIG));
	  r_jmptable= (0 != (rel->r_type[0] & RELOC_STD_BITS_JMPTABLE_BIG));
	  r_relative= (0 != (rel->r_type[0] & RELOC_STD_BITS_RELATIVE_BIG));
	  r_length  = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_BIG)
		       >> RELOC_STD_BITS_LENGTH_SH_BIG);
	}
      else
	{
	  r_index   = ((rel->r_index[2] << 16)
		       | (rel->r_index[1] << 8)
		       | rel->r_index[0]);
	  r_extern  = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_LITTLE));
	  r_pcrel   = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_LITTLE));
	  r_baserel = (0 != (rel->r_type[0] & RELOC_STD_BITS_BASEREL_LITTLE));
	  r_jmptable= (0 != (rel->r_type[0] & RELOC_STD_BITS_JMPTABLE_LITTLE));
	  r_relative= (0 != (rel->r_type[0] & RELOC_STD_BITS_RELATIVE_LITTLE));
	  r_length  = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_LITTLE)
		       >> RELOC_STD_BITS_LENGTH_SH_LITTLE);
	}

      howto_idx = r_length + 4 * r_pcrel + 8 * r_baserel
		  + 16 * r_jmptable + 32 * r_relative;
      BFD_ASSERT (howto_idx < TABLE_SIZE (howto_table_std));

      if (relocateable)
	{
	  /* We are generating a relocateable output file, and must
	     modify the reloc accordingly.  */
	  if (r_extern)
	    {
	      struct aout_link_hash_entry *h;

	      /* If we know the symbol this relocation is against,
		 convert it into a relocation against a section.  This
		 is what the native linker does.  */
	      h = sym_hashes[r_index];
	      if (h != (struct aout_link_hash_entry *) NULL
		  && h->root.type == bfd_link_hash_defined)
		{
		  asection *output_section;

		  /* Change the r_extern value.  */
		  if (output_bfd->xvec->header_byteorder_big_p)
		    rel->r_type[0] &=~ RELOC_STD_BITS_EXTERN_BIG;
		  else
		    rel->r_type[0] &=~ RELOC_STD_BITS_EXTERN_LITTLE;

		  /* Compute a new r_index.  */
		  output_section = h->root.u.def.section->output_section;
		  if (output_section == obj_textsec (output_bfd))
		    r_index = N_TEXT;
		  else if (output_section == obj_datasec (output_bfd))
		    r_index = N_DATA;
		  else if (output_section == obj_bsssec (output_bfd))
		    r_index = N_BSS;
		  else
		    r_index = N_ABS;

		  /* Add the symbol value and the section VMA to the
		     addend stored in the contents.  */
		  relocation = (h->root.u.def.value
				+ output_section->vma
				+ h->root.u.def.section->output_offset);
		}
	      else
		{
		  /* We must change r_index according to the symbol
		     map.  */
		  r_index = symbol_map[r_index];

		  if (r_index == -1)
		    {
		      const char *name;

		      name = strings + GET_WORD (input_bfd,
						 syms[r_index].e_strx);
		      if (! ((*finfo->info->callbacks->unattached_reloc)
			     (finfo->info, name, input_bfd, input_section,
			      r_addr)))
			return false;
		      r_index = 0;
		    }

		  relocation = 0;
		}

	      /* Write out the new r_index value.  */
	      if (output_bfd->xvec->header_byteorder_big_p)
		{
		  rel->r_index[0] = r_index >> 16;
		  rel->r_index[1] = r_index >> 8;
		  rel->r_index[2] = r_index;
		}
	      else
		{
		  rel->r_index[2] = r_index >> 16;
		  rel->r_index[1] = r_index >> 8;
		  rel->r_index[0] = r_index;
		}
	    }
	  else
	    {
	      asection *section;

	      /* This is a relocation against a section.  We must
		 adjust by the amount that the section moved.  */
	      section = aout_reloc_index_to_section (input_bfd, r_index);
	      relocation = (section->output_section->vma
			    + section->output_offset
			    - section->vma);
	    }

	  /* Change the address of the relocation.  */
	  PUT_WORD (output_bfd,
		    r_addr + input_section->output_offset,
		    rel->r_address);

	  /* Adjust a PC relative relocation by removing the reference
	     to the original address in the section and including the
	     reference to the new address.  */
	  if (r_pcrel)
	    relocation -= (input_section->output_section->vma
			   + input_section->output_offset
			   - input_section->vma);

	  if (relocation == 0)
	    r = bfd_reloc_ok;
	  else
	    r = _bfd_relocate_contents (howto_table_std + howto_idx,
					input_bfd, relocation,
					contents + r_addr);
	}
      else
	{
	  /* We are generating an executable, and must do a full
	     relocation.  */
	  if (r_extern)
	    {
	      struct aout_link_hash_entry *h;

	      h = sym_hashes[r_index];
	      if (h != (struct aout_link_hash_entry *) NULL
		  && h->root.type == bfd_link_hash_defined)
		{
		  relocation = (h->root.u.def.value
				+ h->root.u.def.section->output_section->vma
				+ h->root.u.def.section->output_offset);
		}
	      else
		{
		  const char *name;

		  name = strings + GET_WORD (input_bfd, syms[r_index].e_strx);
		  if (! ((*finfo->info->callbacks->undefined_symbol)
			 (finfo->info, name, input_bfd, input_section,
			  r_addr)))
		    return false;
		  relocation = 0;
		}
	    }
	  else
	    {
	      asection *section;

	      section = aout_reloc_index_to_section (input_bfd, r_index);
	      relocation = (section->output_section->vma
			    + section->output_offset
			    - section->vma);
	      if (r_pcrel)
		relocation += input_section->vma;
	    }

	  r = _bfd_final_link_relocate (howto_table_std + howto_idx,
					input_bfd, input_section,
					contents, r_addr, relocation,
					(bfd_vma) 0);
	}

      if (r != bfd_reloc_ok)
	{
	  switch (r)
	    {
	    default:
	    case bfd_reloc_outofrange:
	      abort ();
	    case bfd_reloc_overflow:
	      {
		const char *name;

		if (r_extern)
		  name = strings + GET_WORD (input_bfd,
					     syms[r_index].e_strx);
		else
		  {
		    asection *s;

		    s = aout_reloc_index_to_section (input_bfd, r_index);
		    name = bfd_section_name (input_bfd, s);
		  }
		if (! ((*finfo->info->callbacks->reloc_overflow)
		       (finfo->info, name, howto_table_std[howto_idx].name,
			(bfd_vma) 0, input_bfd, input_section, r_addr)))
		  return false;
	      }
	      break;
	    }
	}
    }

  return true;
}

/* Relocate an a.out section using extended a.out relocs.  */

static boolean
aout_link_input_section_ext (finfo, input_bfd, input_section, relocs,
			     rel_size, contents, symbol_map)
     struct aout_final_link_info *finfo;
     bfd *input_bfd;
     asection *input_section;
     struct reloc_ext_external *relocs;
     bfd_size_type rel_size;
     bfd_byte *contents;
     int *symbol_map;
{
  bfd *output_bfd;
  boolean relocateable;
  struct external_nlist *syms;
  char *strings;
  struct aout_link_hash_entry **sym_hashes;
  bfd_size_type reloc_count;
  register struct reloc_ext_external *rel;
  struct reloc_ext_external *rel_end;

  output_bfd = finfo->output_bfd;

  BFD_ASSERT (obj_reloc_entry_size (input_bfd) == RELOC_EXT_SIZE);
  BFD_ASSERT (input_bfd->xvec->header_byteorder_big_p
	      == output_bfd->xvec->header_byteorder_big_p);

  relocateable = finfo->info->relocateable;
  syms = obj_aout_external_syms (input_bfd);
  strings = obj_aout_external_strings (input_bfd);
  sym_hashes = obj_aout_sym_hashes (input_bfd);

  reloc_count = rel_size / RELOC_EXT_SIZE;
  rel = relocs;
  rel_end = rel + reloc_count;
  for (; rel < rel_end; rel++)
    {
      bfd_vma r_addr;
      int r_index;
      int r_extern;
      int r_type;
      bfd_vma r_addend;
      bfd_vma relocation;

      r_addr = GET_SWORD (input_bfd, rel->r_address);

      if (input_bfd->xvec->header_byteorder_big_p)
	{
	  r_index  = ((rel->r_index[0] << 16)
		      | (rel->r_index[1] << 8)
		      | rel->r_index[2]);
	  r_extern = (0 != (rel->r_type[0] & RELOC_EXT_BITS_EXTERN_BIG));
	  r_type   = ((rel->r_type[0] & RELOC_EXT_BITS_TYPE_BIG)
		      >> RELOC_EXT_BITS_TYPE_SH_BIG);
	}
      else
	{
	  r_index  = ((rel->r_index[2] << 16)
		      | (rel->r_index[1] << 8)
		      | rel->r_index[0]);
	  r_extern = (0 != (rel->r_type[0] & RELOC_EXT_BITS_EXTERN_LITTLE));
	  r_type   = ((rel->r_type[0] & RELOC_EXT_BITS_TYPE_LITTLE)
		      >> RELOC_EXT_BITS_TYPE_SH_LITTLE);
	}

      r_addend = GET_SWORD (input_bfd, rel->r_addend);

      BFD_ASSERT (r_type >= 0
		  && r_type < TABLE_SIZE (howto_table_ext));

      if (relocateable)
	{
	  /* We are generating a relocateable output file, and must
	     modify the reloc accordingly.  */
	  if (r_extern)
	    {
	      struct aout_link_hash_entry *h;

	      /* If we know the symbol this relocation is against,
		 convert it into a relocation against a section.  This
		 is what the native linker does.  */
	      h = sym_hashes[r_index];
	      if (h != (struct aout_link_hash_entry *) NULL
		  && h->root.type == bfd_link_hash_defined)
		{
		  asection *output_section;

		  /* Change the r_extern value.  */
		  if (output_bfd->xvec->header_byteorder_big_p)
		    rel->r_type[0] &=~ RELOC_EXT_BITS_EXTERN_BIG;
		  else
		    rel->r_type[0] &=~ RELOC_EXT_BITS_EXTERN_LITTLE;

		  /* Compute a new r_index.  */
		  output_section = h->root.u.def.section->output_section;
		  if (output_section == obj_textsec (output_bfd))
		    r_index = N_TEXT;
		  else if (output_section == obj_datasec (output_bfd))
		    r_index = N_DATA;
		  else if (output_section == obj_bsssec (output_bfd))
		    r_index = N_BSS;
		  else
		    r_index = N_ABS;

		  /* Add the symbol value and the section VMA to the
		     addend.  */
		  relocation = (h->root.u.def.value
				+ output_section->vma
				+ h->root.u.def.section->output_offset);

		  /* Now RELOCATION is the VMA of the final
		     destination.  If this is a PC relative reloc,
		     then ADDEND is the negative of the source VMA.
		     We want to set ADDEND to the difference between
		     the destination VMA and the source VMA, which
		     means we must adjust RELOCATION by the change in
		     the source VMA.  This is done below.  */
		}
	      else
		{
		  /* We must change r_index according to the symbol
		     map.  */
		  r_index = symbol_map[r_index];

		  if (r_index == -1)
		    {
		      const char *name;

		      name = (strings
			      + GET_WORD (input_bfd, syms[r_index].e_strx));
		      if (! ((*finfo->info->callbacks->unattached_reloc)
			     (finfo->info, name, input_bfd, input_section,
			      r_addr)))
			return false;
		      r_index = 0;
		    }

		  relocation = 0;

		  /* If this is a PC relative reloc, then the addend
		     is the negative of the source VMA.  We must
		     adjust it by the change in the source VMA.  This
		     is done below.  */
		}

	      /* Write out the new r_index value.  */
	      if (output_bfd->xvec->header_byteorder_big_p)
		{
		  rel->r_index[0] = r_index >> 16;
		  rel->r_index[1] = r_index >> 8;
		  rel->r_index[2] = r_index;
		}
	      else
		{
		  rel->r_index[2] = r_index >> 16;
		  rel->r_index[1] = r_index >> 8;
		  rel->r_index[0] = r_index;
		}
	    }
	  else
	    {
	      asection *section;

	      /* This is a relocation against a section.  We must
		 adjust by the amount that the section moved.  */
	      section = aout_reloc_index_to_section (input_bfd, r_index);
	      relocation = (section->output_section->vma
			    + section->output_offset
			    - section->vma);

	      /* If this is a PC relative reloc, then the addend is
		 the difference in VMA between the destination and the
		 source.  We have just adjusted for the change in VMA
		 of the destination, so we must also adjust by the
		 change in VMA of the source.  This is done below.  */
	    }

	  /* As described above, we must always adjust a PC relative
	     reloc by the change in VMA of the source.  */
	  if (howto_table_ext[r_type].pc_relative)
	    relocation -= (input_section->output_section->vma
			   + input_section->output_offset
			   - input_section->vma);

	  /* Change the addend if necessary.  */
	  if (relocation != 0)
	    PUT_WORD (output_bfd, r_addend + relocation, rel->r_addend);

	  /* Change the address of the relocation.  */
	  PUT_WORD (output_bfd,
		    r_addr + input_section->output_offset,
		    rel->r_address);
	}
      else
	{
	  bfd_reloc_status_type r;

	  /* We are generating an executable, and must do a full
	     relocation.  */
	  if (r_extern)
	    {
	      struct aout_link_hash_entry *h;

	      h = sym_hashes[r_index];
	      if (h != (struct aout_link_hash_entry *) NULL
		  && h->root.type == bfd_link_hash_defined)
		{
		  relocation = (h->root.u.def.value
				+ h->root.u.def.section->output_section->vma
				+ h->root.u.def.section->output_offset);
		}
	      else
		{
		  const char *name;

		  name = strings + GET_WORD (input_bfd, syms[r_index].e_strx);
		  if (! ((*finfo->info->callbacks->undefined_symbol)
			 (finfo->info, name, input_bfd, input_section,
			  r_addr)))
		    return false;
		  relocation = 0;
		}
	    }
	  else
	    {
	      asection *section;

	      section = aout_reloc_index_to_section (input_bfd, r_index);

	      /* If this is a PC relative reloc, then R_ADDEND is the
		 difference between the two vmas, or
		   old_dest_sec + old_dest_off - (old_src_sec + old_src_off)
		 where
		   old_dest_sec == section->vma
		 and
		   old_src_sec == input_section->vma
		 and
		   old_src_off == r_addr

		 _bfd_final_link_relocate expects RELOCATION +
		 R_ADDEND to be the VMA of the destination minus
		 r_addr (the minus r_addr is because this relocation
		 is not pcrel_offset, which is a bit confusing and
		 should, perhaps, be changed), or
		   new_dest_sec
		 where
		   new_dest_sec == output_section->vma + output_offset
		 We arrange for this to happen by setting RELOCATION to
		   new_dest_sec + old_src_sec - old_dest_sec

		 If this is not a PC relative reloc, then R_ADDEND is
		 simply the VMA of the destination, so we set
		 RELOCATION to the change in the destination VMA, or
		   new_dest_sec - old_dest_sec
		 */
	      relocation = (section->output_section->vma
			    + section->output_offset
			    - section->vma);
	      if (howto_table_ext[r_type].pc_relative)
		relocation += input_section->vma;
	    }

	  r = _bfd_final_link_relocate (howto_table_ext + r_type,
					input_bfd, input_section,
					contents, r_addr, relocation,
					r_addend);
	  if (r != bfd_reloc_ok)
	    {
	      switch (r)
		{
		default:
		case bfd_reloc_outofrange:
		  abort ();
		case bfd_reloc_overflow:
		  {
		    const char *name;

		    if (r_extern)
		      name = strings + GET_WORD (input_bfd,
						 syms[r_index].e_strx);
		    else
		      {
			asection *s;

			s = aout_reloc_index_to_section (input_bfd, r_index);
			name = bfd_section_name (input_bfd, s);
		      }
		    if (! ((*finfo->info->callbacks->reloc_overflow)
			   (finfo->info, name, howto_table_ext[r_type].name,
			    r_addend, input_bfd, input_section, r_addr)))
		      return false;
		  }
		  break;
		}
	    }
	}
    }

  return true;
}

/* Handle a link order which is supposed to generate a reloc.  */

static boolean
aout_link_reloc_link_order (finfo, o, p)
     struct aout_final_link_info *finfo;
     asection *o;
     struct bfd_link_order *p;
{
  struct bfd_link_order_reloc *pr;
  int r_index;
  int r_extern;
  const reloc_howto_type *howto;
  file_ptr *reloff_ptr;
  struct reloc_std_external srel;
  struct reloc_ext_external erel;
  PTR rel_ptr;

  pr = p->u.reloc.p;

  if (p->type == bfd_section_reloc_link_order)
    {
      r_extern = 0;
      if (pr->u.section == &bfd_abs_section)
	r_index = N_ABS | N_EXT;
      else
	{
	  BFD_ASSERT (pr->u.section->owner == finfo->output_bfd);
	  r_index = pr->u.section->target_index;
	}
    }
  else
    {
      struct aout_link_hash_entry *h;

      BFD_ASSERT (p->type == bfd_symbol_reloc_link_order);
      r_extern = 1;
      h = aout_link_hash_lookup (aout_hash_table (finfo->info),
				 pr->u.name, false, false, true);
      if (h != (struct aout_link_hash_entry *) NULL
	  && h->indx == -1)
	r_index = h->indx;
      else
	{
	  if (! ((*finfo->info->callbacks->unattached_reloc)
		 (finfo->info, pr->u.name, (bfd *) NULL,
		  (asection *) NULL, (bfd_vma) 0)))
	    return false;
	  r_index = 0;
	}
    }

  howto = bfd_reloc_type_lookup (finfo->output_bfd, pr->reloc);
  if (howto == (const reloc_howto_type *) NULL)
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  if (o == obj_textsec (finfo->output_bfd))
    reloff_ptr = &finfo->treloff;
  else if (o == obj_datasec (finfo->output_bfd))
    reloff_ptr = &finfo->dreloff;
  else
    abort ();

  if (obj_reloc_entry_size (finfo->output_bfd) == RELOC_STD_SIZE)
    {
      int r_pcrel;
      int r_baserel;
      int r_jmptable;
      int r_relative;
      int r_length;

      r_pcrel = howto->pc_relative;
      r_baserel = (howto->type & 8) != 0;
      r_jmptable = (howto->type & 16) != 0;
      r_relative = (howto->type & 32) != 0;
      r_length = howto->size;

      PUT_WORD (finfo->output_bfd, p->offset, srel.r_address);
      if (finfo->output_bfd->xvec->header_byteorder_big_p)
	{
	  srel.r_index[0] = r_index >> 16;
	  srel.r_index[1] = r_index >> 8;
	  srel.r_index[2] = r_index;
	  srel.r_type[0] =
	    ((r_extern ?     RELOC_STD_BITS_EXTERN_BIG : 0)
	     | (r_pcrel ?    RELOC_STD_BITS_PCREL_BIG : 0)
	     | (r_baserel ?  RELOC_STD_BITS_BASEREL_BIG : 0)
	     | (r_jmptable ? RELOC_STD_BITS_JMPTABLE_BIG : 0)
	     | (r_relative ? RELOC_STD_BITS_RELATIVE_BIG : 0)
	     | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG));
	}
      else
	{
	  srel.r_index[2] = r_index >> 16;
	  srel.r_index[1] = r_index >> 8;
	  srel.r_index[0] = r_index;
	  srel.r_type[0] =
	    ((r_extern ?     RELOC_STD_BITS_EXTERN_LITTLE : 0)
	     | (r_pcrel ?    RELOC_STD_BITS_PCREL_LITTLE : 0)
	     | (r_baserel ?  RELOC_STD_BITS_BASEREL_LITTLE : 0)
	     | (r_jmptable ? RELOC_STD_BITS_JMPTABLE_LITTLE : 0)
	     | (r_relative ? RELOC_STD_BITS_RELATIVE_LITTLE : 0)
	     | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE));
	}

      rel_ptr = (PTR) &srel;

      /* We have to write the addend into the object file, since
	 standard a.out relocs are in place.  It would be more
	 reliable if we had the current contents of the file here,
	 rather than assuming zeroes, but we can't read the file since
	 it was opened using bfd_openw.  */
      if (pr->addend != 0)
	{
	  bfd_size_type size;
	  bfd_reloc_status_type r;
	  bfd_byte *buf;
	  boolean ok;

	  size = bfd_get_reloc_size (howto);
	  buf = (bfd_byte*) bfd_zmalloc (size);
	  if (buf == (bfd_byte *) NULL)
	    {
	      bfd_set_error (bfd_error_no_memory);
	      return false;
	    }
	  r = _bfd_relocate_contents (howto, finfo->output_bfd,
				      pr->addend, buf);
	  switch (r)
	    {
	    case bfd_reloc_ok:
	      break;
	    default:
	    case bfd_reloc_outofrange:
	      abort ();
	    case bfd_reloc_overflow:
	      if (! ((*finfo->info->callbacks->reloc_overflow)
		     (finfo->info,
		      (p->type == bfd_section_reloc_link_order
		       ? bfd_section_name (finfo->output_bfd,
					   pr->u.section)
		       : pr->u.name),
		      howto->name, pr->addend, (bfd *) NULL,
		      (asection *) NULL, (bfd_vma) 0)))
		{
		  free (buf);
		  return false;
		}
	      break;
	    }
	  ok = bfd_set_section_contents (finfo->output_bfd, o,
					 (PTR) buf,
					 (file_ptr) p->offset,
					 size);
	  free (buf);
	  if (! ok)
	    return false;
	}
    }
  else
    {
      PUT_WORD (finfo->output_bfd, p->offset, erel.r_address);

      if (finfo->output_bfd->xvec->header_byteorder_big_p)
	{
	  erel.r_index[0] = r_index >> 16;
	  erel.r_index[1] = r_index >> 8;
	  erel.r_index[2] = r_index;
	  erel.r_type[0] =
	    ((r_extern ? RELOC_EXT_BITS_EXTERN_BIG : 0)
	     | (howto->type << RELOC_EXT_BITS_TYPE_SH_BIG));
	}
      else
	{
	  erel.r_index[2] = r_index >> 16;
	  erel.r_index[1] = r_index >> 8;
	  erel.r_index[0] = r_index;
	  erel.r_type[0] =
	    (r_extern ? RELOC_EXT_BITS_EXTERN_LITTLE : 0)
	      | (howto->type << RELOC_EXT_BITS_TYPE_SH_LITTLE);
	}

      PUT_WORD (finfo->output_bfd, pr->addend, erel.r_addend);

      rel_ptr = (PTR) &erel;
    }

  if (bfd_seek (finfo->output_bfd, *reloff_ptr, SEEK_SET) != 0
      || (bfd_write (rel_ptr, (bfd_size_type) 1,
		     obj_reloc_entry_size (finfo->output_bfd),
		     finfo->output_bfd)
	  != obj_reloc_entry_size (finfo->output_bfd)))
    return false;

  *reloff_ptr += obj_reloc_entry_size (finfo->output_bfd);

  /* Assert that the relocs have not run into the symbols, and that n
     the text relocs have not run into the data relocs.  */
  BFD_ASSERT (*reloff_ptr <= obj_sym_filepos (finfo->output_bfd)
	      && (reloff_ptr != &finfo->treloff
		  || (*reloff_ptr
		      <= obj_datasec (finfo->output_bfd)->rel_filepos)));

  return true;
}
