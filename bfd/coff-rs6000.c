/* BFD back-end for IBM RS/6000 "XCOFF" files.
   Copyright 1990, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
   FIXME: Can someone provide a transliteration of this name into ASCII?
   Using the following chars caused a compiler warning on HIUX (so I replaced
   them with octal escapes), and isn't useful without an understanding of what
   character set it is.
   Written by Metin G. Ozisik, Mimi Ph\373\364ng-Th\345o V\365, 
     and John Gilmore.
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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This port currently only handles reading object files, except when
   compiled on an RS/6000 host.  -- no archive support, no core files.
   In all cases, it does not support writing.

   FIXMEmgo comments are left from Metin Ozisik's original port.  */

/* Internalcoff.h and coffcode.h modify themselves based on this flag.  */
#define RS6000COFF_C 1

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "coff/internal.h"
#include "coff/rs6000.h"
#include "libcoff.h"

/* The main body of code is in coffcode.h.  */

#define COFF_DEFAULT_SECTION_ALIGNMENT_POWER (3)

/* The XCOFF reloc table.  Actually, XCOFF relocations specify the
   bitsize and whether they are signed or not, along with a
   conventional type.  This table is for the types, which are used for
   different algorithms for putting in the reloc.  Many of these
   relocs need special_function entries, which I have not written.  */

static reloc_howto_type rs6000coff_howto_table[] =
{
  /* Standard 32 bit relocation.  */
  HOWTO (0,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_POS",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,            /* src_mask */                             
	 0xffffffff,            /* dst_mask */                             
	 false),                /* pcrel_offset */

  /* 32 bit relocation, but store negative value.  */
  HOWTO (1,	                /* type */                                 
	 0,	                /* rightshift */                           
	 -2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_NEG",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,            /* src_mask */                             
	 0xffffffff,            /* dst_mask */                             
	 false),                /* pcrel_offset */

  /* 32 bit PC relative relocation.  */
  HOWTO (2,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 true,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_REL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,            /* src_mask */                             
	 0xffffffff,            /* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* 16 bit TOC relative relocation.  */
  HOWTO (3,	                /* type */                                 
	 0,	                /* rightshift */                           
	 1,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_TOC",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* I don't really know what this is.  */
  HOWTO (4,	                /* type */                                 
	 1,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RTB",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,	        /* src_mask */                             
	 0xffffffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* External TOC relative symbol.  */
  HOWTO (5,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_GL",                /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Local TOC relative symbol.  */
  HOWTO (6,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_TCL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  { 7 },
  
  /* Non modifiable absolute branch.  */
  HOWTO (8,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 26,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_BA",                /* name */                                 
	 true,	                /* partial_inplace */                      
	 0x3fffffc,	        /* src_mask */                             
	 0x3fffffc,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  { 9 },

  /* Non modifiable relative branch.  */
  HOWTO (0xa,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 26,	                /* bitsize */                   
	 true,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_BR",                /* name */                                 
	 true,	                /* partial_inplace */                      
	 0x3fffffc,	        /* src_mask */                             
	 0x3fffffc,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  { 0xb },

  /* Indirect load.  */
  HOWTO (0xc,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RL",                /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Load address.  */
  HOWTO (0xd,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RLA",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  { 0xe },
  
  /* Non-relocating reference.  */
  HOWTO (0xf,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_REF",               /* name */                                 
	 false,	                /* partial_inplace */                      
	 0,		        /* src_mask */                             
	 0,     	   	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  { 0x10 },
  { 0x11 },
  
  /* TOC relative indirect load.  */
  HOWTO (0x12,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_TRL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* TOC relative load address.  */
  HOWTO (0x13,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_TRLA",              /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable relative branch.  */
  HOWTO (0x14,	                /* type */                                 
	 1,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RRTBI",             /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,	        /* src_mask */                             
	 0xffffffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable absolute branch.  */
  HOWTO (0x15,	                /* type */                                 
	 1,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 32,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RRTBA",             /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffffffff,	        /* src_mask */                             
	 0xffffffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable call absolute indirect.  */
  HOWTO (0x16,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_CAI",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable call relative.  */
  HOWTO (0x17,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_REL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable branch absolute.  */
  HOWTO (0x18,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RBA",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable branch absolute.  */
  HOWTO (0x19,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_RBAC",              /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable branch relative.  */
  HOWTO (0x1a,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 26,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_signed, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_REL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false),                /* pcrel_offset */
  
  /* Modifiable branch absolute.  */
  HOWTO (0x1b,	                /* type */                                 
	 0,	                /* rightshift */                           
	 2,	                /* size (0 = byte, 1 = short, 2 = long) */ 
	 16,	                /* bitsize */                   
	 false,	                /* pc_relative */                          
	 0,	                /* bitpos */                               
	 complain_overflow_bitfield, /* complain_on_overflow */
	 0,		        /* special_function */                     
	 "R_REL",               /* name */                                 
	 true,	                /* partial_inplace */                      
	 0xffff,	        /* src_mask */                             
	 0xffff,        	/* dst_mask */                             
	 false)                 /* pcrel_offset */
};

#define RTYPE2HOWTO(cache_ptr, dst) rs6000coff_rtype2howto (cache_ptr, dst)

static void rs6000coff_rtype2howto PARAMS ((arelent *,
					    struct internal_reloc *));

static void
rs6000coff_rtype2howto (relent, internal)
     arelent *relent;
     struct internal_reloc *internal;
{
  relent->howto = rs6000coff_howto_table + internal->r_type;

  /* The r_size field of an XCOFF reloc encodes the bitsize of the
     relocation, as well as indicating whether it is signed or not.
     Doublecheck that the relocation information gathered from the
     type matches this information.  */
  if (relent->howto->bitsize != ((unsigned int) internal->r_size & 0x1f) + 1)
    abort ();
#if 0
  if ((internal->r_size & 0x80) != 0
      ? (relent->howto->complain_on_overflow != complain_overflow_signed)
      : (relent->howto->complain_on_overflow != complain_overflow_bitfield))
    abort ();
#endif
}

#define coff_bfd_reloc_type_lookup rs6000coff_reloc_type_lookup

static reloc_howto_type *rs6000coff_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));

static reloc_howto_type *
rs6000coff_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  switch (code)
    {
    case BFD_RELOC_PPC_B26:
      return &rs6000coff_howto_table[0xa];
    case BFD_RELOC_PPC_BA26:
      return &rs6000coff_howto_table[8];
    case BFD_RELOC_PPC_TOC16:
      return &rs6000coff_howto_table[3];
    case BFD_RELOC_32:
      return &rs6000coff_howto_table[0];
    default:
      return NULL;
    }
}

#define SELECT_RELOC(internal, howto)					\
  {									\
    internal.r_type = howto->type;					\
    internal.r_size =							\
      ((howto->complain_on_overflow == complain_overflow_signed		\
	? 0x80								\
	: 0)								\
       | (howto->bitsize - 1));						\
  }

#define COFF_LONG_FILENAMES

#include "coffcode.h"

#define	coff_archive_p		bfd_generic_archive_p
#define	coff_mkarchive		_bfd_generic_mkarchive

#ifdef HOST_AIX

/* ------------------------------------------------------------------------ */
/*	Support for archive file stuff..				    */
/*	Stolen from Damon A. Permezel's `bfd' portation.		    */
/* ------------------------------------------------------------------------ */

#define rs6000coff_slurp_armap bfd_slurp_coff_armap
#define rs6000coff_slurp_extended_name_table _bfd_slurp_extended_name_table
#define rs6000coff_construct_extended_name_table \
  _bfd_archive_coff_construct_extended_name_table
#define rs6000coff_truncate_arname bfd_dont_truncate_arname
#define rs6000coff_update_armap_timestamp bfd_true

#undef	coff_mkarchive
#define	coff_mkarchive			rs6000coff_mkarchive

#undef	coff_archive_p
#define	coff_archive_p			rs6000coff_archive_p

#include "/usr/include/ar.h"		/* <ar.h> doesn't do it.	*/


#define arch_hdr(bfd) 		\
	((struct ar_hdr *)	\
	 (((struct areltdata *)((bfd)->arelt_data))->arch_header))


static boolean
rs6000coff_mkarchive (abfd)
     bfd *abfd;
{
	bfd_set_error (bfd_error_invalid_operation);	/* write not supported	*/
	return false;
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
	struct {
		struct ar_hdr hdr;
		char namebuf[256];
	} h;
	int size;
	struct areltdata *ared;
	char *allocptr;

	size = sizeof (h.hdr);
	if (bfd_read(&h.hdr, 1, size, abfd) != size) {
	        if (bfd_get_error () != bfd_error_system_call)
		  bfd_set_error (bfd_error_no_more_archived_files);
		return NULL;
	}
	size  = atoi(h.hdr.ar_namlen);	/* ar_name[] length	*/
	size += size & 1;

	if (bfd_read(&h.hdr._ar_name.ar_name[2], 1, size, abfd) != size) {
	        if (bfd_get_error () != bfd_error_system_call)
		  bfd_set_error (bfd_error_no_more_archived_files);
		return NULL;
	}

	if (strncmp(h.hdr._ar_name.ar_fmag + size, AIAFMAG, 2)) {
		bfd_set_error (bfd_error_malformed_archive);
		return NULL;
	}

	h.hdr._ar_name.ar_name[size] = 0;	/* terminate filename	*/

	/*
	 * if the filename is NULL, we're (probably) at the end.
	 */
	if (size == 0) {
		bfd_set_error (bfd_error_no_more_archived_files);
		return NULL;
	}

	size += sizeof (h.hdr);
	allocptr = bfd_zalloc(abfd, sizeof (*ared) + size);

	if (allocptr == NULL) {
		bfd_set_error (bfd_error_no_memory);
		return NULL;
	}

	ared = (struct areltdata *) allocptr;

	ared->arch_header = (void *) (allocptr + sizeof (struct areltdata));
	memcpy ((char *) ared->arch_header, &h.hdr, size);
	ared->parsed_size = atoi(h.hdr.ar_size);
	ared->filename    = ((AR_HDR*) ared->arch_header)->_ar_name.ar_name;

	return ared;
}

/* Stolen directly from archive.c, except it calls rs6000coff_snarf_ar_hdr.
   Why wasn't this part of the transfer vector?  */

bfd *
rs6000coff_get_elt_at_filepos (archive, filepos)
     bfd *archive;
     file_ptr filepos;
{
  struct areltdata *new_areldata;
  bfd *n_nfd;

  n_nfd = _bfd_look_for_bfd_in_cache (archive, filepos);
  if (n_nfd) return n_nfd;

  if (0 != bfd_seek (archive, filepos, SEEK_SET))
    return NULL;

  if ((new_areldata = rs6000coff_snarf_ar_hdr (archive)) == NULL) return NULL;
  
  n_nfd = _bfd_create_empty_archive_element_shell (archive);
  if (n_nfd == NULL) {
    bfd_release (archive, (PTR)new_areldata);
    return NULL;
  }
  n_nfd->origin = bfd_tell (archive);
  n_nfd->arelt_data = (PTR) new_areldata;
  n_nfd->filename = new_areldata->filename;

  if (_bfd_add_bfd_to_archive_cache (archive, filepos, n_nfd))
    return n_nfd;

  /* huh? */
  bfd_release (archive, (PTR)n_nfd);
  bfd_release (archive, (PTR)new_areldata);
  return NULL;
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

	return rs6000coff_get_elt_at_filepos (archive, filestart);
}


static const bfd_target *
rs6000coff_archive_p (abfd)
     bfd *abfd;
{
	struct fl_hdr hdr;
	register struct artdata *art;

	if (bfd_read (&hdr, sizeof (hdr), 1, abfd) != sizeof (hdr)) {
	        if (bfd_get_error () != bfd_error_system_call)
		  bfd_set_error (bfd_error_wrong_format);
		return 0;
	}

	if (strncmp(hdr.fl_magic, AIAMAG, SAIAMAG)) {
		bfd_set_error (bfd_error_wrong_format);
		return 0;
	}

	/*
	 * bfd_ardata() accesses the bfd->tdata field.
	 */
	abfd->tdata.aout_ar_data =
	  (void *) bfd_zalloc(abfd, sizeof (*art) + sizeof (hdr));
	if ((art = bfd_ardata (abfd)) == NULL) {
		bfd_set_error (bfd_error_no_memory);
		return 0;
	}

	art->first_file_filepos = atoi(hdr.fl_fstmoff);
	*(struct fl_hdr *) (1 + art) = hdr;

	/* Someday...
	 * slurp in the member table, which I think is the armap equivalent.
	xcoff_slurp_armap(abfd);
	 */
  
	return abfd->xvec;
}


static int
rs6000coff_generic_stat_arch_elt(abfd, buf)
  bfd *abfd;
  struct stat *buf;
{
	struct ar_hdr *hdr;
	char *aloser;
  
	if (abfd->arelt_data == NULL) {
		bfd_set_error (bfd_error_invalid_operation);
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
#undef foo

	return 0;
}

static boolean
rs6000coff_write_armap (arch, elength, map, orl_count, stridx)
  bfd *arch;
  unsigned int elength;
  struct orl *map; 
  unsigned int orl_count;
  int stridx;
{
	bfd_set_error (bfd_error_invalid_operation);
	return false;
}
#endif	/* HOST_AIX */


#define CORE_FILE_P _bfd_dummy_target

#define coff_core_file_failing_command _bfd_nocore_core_file_failing_command
#define coff_core_file_failing_signal _bfd_nocore_core_file_failing_signal
#define coff_core_file_matches_executable_p \
  _bfd_nocore_core_file_matches_executable_p

#ifdef AIX_CORE
#undef CORE_FILE_P
#define CORE_FILE_P rs6000coff_core_p
extern const bfd_target * rs6000coff_core_p ();
extern boolean rs6000coff_get_section_contents ();
extern boolean rs6000coff_core_file_matches_executable_p ();

#undef	coff_core_file_matches_executable_p
#define coff_core_file_matches_executable_p  \
				     rs6000coff_core_file_matches_executable_p

#undef	coff_get_section_contents
#define	coff_get_section_contents	rs6000coff_get_section_contents
#endif /* AIX_CORE */

#ifdef LYNX_CORE

#undef CORE_FILE_P
#define CORE_FILE_P lynx_core_file_p
extern const bfd_target *lynx_core_file_p PARAMS ((bfd *abfd));

extern boolean lynx_core_file_matches_executable_p PARAMS ((bfd *core_bfd,
							    bfd *exec_bfd));
#undef	coff_core_file_matches_executable_p
#define coff_core_file_matches_executable_p lynx_core_file_matches_executable_p

extern char *lynx_core_file_failing_command PARAMS ((bfd *abfd));
#undef coff_core_file_failing_command
#define coff_core_file_failing_command lynx_core_file_failing_command

extern int lynx_core_file_failing_signal PARAMS ((bfd *abfd));
#undef coff_core_file_failing_signal
#define coff_core_file_failing_signal lynx_core_file_failing_signal

#endif /* LYNX_CORE */

/* The transfer vector that leads the outside world to all of the above. */

const bfd_target rs6000coff_vec =
{
  "aixcoff-rs6000",		/* name */
  bfd_target_coff_flavour,	
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* leading char */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen??? FIXMEmgo */

  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, 	/* bfd_check_format */
     coff_archive_p, CORE_FILE_P},
  {bfd_false, coff_mkobject, coff_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (coff),
     BFD_JUMP_TABLE_COPY (coff),
     BFD_JUMP_TABLE_CORE (coff),
#ifdef HOST_AIX
     BFD_JUMP_TABLE_ARCHIVE (rs6000coff),
#else
     BFD_JUMP_TABLE_ARCHIVE (_bfd_archive_coff),
#endif
     BFD_JUMP_TABLE_SYMBOLS (coff),
     BFD_JUMP_TABLE_RELOCS (coff),
     BFD_JUMP_TABLE_WRITE (coff),
     BFD_JUMP_TABLE_LINK (coff),
     BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  COFF_SWAP_TABLE,
};
