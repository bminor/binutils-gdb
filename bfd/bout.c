/* BFD back-end for i960 b.out binaries */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */

#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"


#include "bout.h"


#include "stab.gnu.h"
#include "liba.out.h"		/* BFD a.out internal data structures */
#include "archures.h"

/* Align an address by rounding it up to a power of two.  It leaves the
   address unchanged if align == 0 (2^0 = alignment of 1 byte) */
#define	i960_align(addr, align)	\
	( ((addr) + ((1<<(align))-1)) & (-1 << (align)))

extern uint64_type _do_getl64 (), _do_getb64 ();
extern uint32_type _do_getl32 (), _do_getb32 ();
extern uint16_type _do_getl16 (), _do_getb16 ();
extern void _do_putl64 (), _do_putb64 ();
extern void _do_putl32 (), _do_putb32 ();
extern void _do_putl16 (), _do_putb16 ();


PROTO (static boolean, b_out_squirt_out_relocs,(bfd *abfd, asection *section));
PROTO (static bfd_target *, b_out_callback, (bfd *));

PROTO (boolean, aout_slurp_symbol_table, (bfd *abfd));
PROTO (void , aout_write_syms, ());

PROTO (static void, swap_exec_header, (bfd *abfd, struct exec *execp));

static bfd_target *
b_out_little_object_p (abfd)
     bfd *abfd;
{
  unsigned char magicbytes[LONG_SIZE];
  struct exec anexec;
  
  if (bfd_read ((PTR)magicbytes, 1, LONG_SIZE, abfd) != LONG_SIZE) {
    bfd_error = system_call_error;
    return 0;
  }
  anexec.a_magic = _do_getl32 (magicbytes);

  if (N_BADMAG (anexec)) {
    bfd_error = wrong_format;
    return 0;
  }
  return some_aout_object_p (abfd, b_out_callback);
}

static bfd_target *
b_out_big_object_p (abfd)
     bfd *abfd;
{
  unsigned char magicbytes[LONG_SIZE];
  struct exec anexec;

  if (bfd_read ((PTR)magicbytes, 1, LONG_SIZE, abfd) != LONG_SIZE) {
    bfd_error = system_call_error;
    return 0;
  }

  anexec.a_magic = _do_getb32 (magicbytes);

  if (N_BADMAG (anexec)) {
    bfd_error = wrong_format;
    return 0;
  }
  return some_aout_object_p (abfd, b_out_callback);
}

/* Finish up the opening of a b.out file for reading.  Fill in all the
   fields that are not handled by common code.  */

static bfd_target *
b_out_callback (abfd)
     bfd *abfd;
{
  struct exec anexec;
  struct exec *execp = &anexec;
  unsigned long bss_start;

  /* Reread the exec header, because the common code didn't get all of
     our extended header.  */

  if (bfd_seek (abfd, 0L, SEEK_SET) < 0) {
    bfd_error = system_call_error;
    return 0;
  }

  /* FIXME, needs to be hacked for character array read-in ala sunos.c.  */
  if (bfd_read ((PTR) execp, 1, sizeof (struct exec), abfd)
      != sizeof (struct exec)) {
    bfd_error = wrong_format;
    return 0;
  }

  swap_exec_header (abfd, execp);

  /* Architecture and machine type */
  abfd->obj_arch = bfd_arch_i960;	/* B.out only used on i960 */
  abfd->obj_machine = bfd_mach_i960_core;	/* Default */
/* FIXME:  Set real machine type from file here */

  /* The positions of the string table and symbol table.  */
  obj_str_filepos (abfd) = N_STROFF (anexec);
  obj_sym_filepos (abfd) = N_SYMOFF (anexec);

  /* The alignments of the sections */
  obj_textsec (abfd)->alignment_power = execp->a_talign;
  obj_datasec (abfd)->alignment_power = execp->a_dalign;
  obj_bsssec  (abfd)->alignment_power = execp->a_balign;

  /* The starting addresses of the sections.  */
  obj_textsec (abfd)->vma = anexec.a_tload;
  obj_datasec (abfd)->vma = anexec.a_dload;
  bss_start = anexec.a_dload + anexec.a_data; /* BSS = end of data section */
  obj_bsssec (abfd)->vma = i960_align (bss_start, anexec.a_balign);

  /* The file positions of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF(anexec);
  obj_datasec (abfd)->filepos = N_DATOFF(anexec);

  /* The file positions of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TROFF(anexec);
  obj_datasec (abfd)->rel_filepos =  N_DROFF(anexec);

  return abfd->xvec;
}


static boolean
b_out_mkobject (abfd)
     bfd *abfd;
{
  PTR  rawptr;

  bfd_error = system_call_error;

  /* Use an intermediate variable for clarity */
  rawptr = (PTR) zalloc (sizeof (struct aoutdata) + sizeof (struct exec));

  if (rawptr == (PTR)NULL) {
    bfd_error = no_memory;
    return false;
  }

  set_tdata(abfd, (struct aoutdata *) rawptr);
  exec_hdr (abfd) = (struct exec *) ( (char*)rawptr + sizeof (struct aoutdata));

  /* For simplicity's sake we just make all the sections right here. */
  obj_textsec (abfd) = (asection *)NULL;
  obj_datasec (abfd) = (asection *)NULL;
  obj_bsssec (abfd) = (asection *)NULL;

  bfd_make_section (abfd, ".text");
  bfd_make_section (abfd, ".data");
  bfd_make_section (abfd, ".bss");

  return true;
}

static boolean
b_out_write_object_contents (abfd)
     bfd *abfd;
{
  struct exec swapped_hdr;

  exec_hdr (abfd)->a_magic = BMAGIC;

  exec_hdr (abfd)->a_text = obj_textsec (abfd)->size;
  exec_hdr (abfd)->a_data = obj_datasec (abfd)->size;
  exec_hdr (abfd)->a_bss = obj_bsssec (abfd)->size;
  exec_hdr (abfd)->a_syms = bfd_get_symcount (abfd) * sizeof (struct nlist);
  exec_hdr (abfd)->a_entry = bfd_get_start_address (abfd);
  exec_hdr (abfd)->a_trsize = ((obj_textsec (abfd)->reloc_count) *
                               sizeof (struct relocation_info));
  exec_hdr (abfd)->a_drsize = ((obj_datasec (abfd)->reloc_count) *
                               sizeof (struct relocation_info));

  exec_hdr (abfd)->a_talign = obj_textsec (abfd)->alignment_power;
  exec_hdr (abfd)->a_dalign = obj_datasec (abfd)->alignment_power;
  exec_hdr (abfd)->a_balign = obj_bsssec (abfd)->alignment_power;

  exec_hdr (abfd)->a_tload = obj_textsec (abfd)->vma;
  exec_hdr (abfd)->a_dload = obj_datasec (abfd)->vma;

  /* FIXME, turn the header into bytes here, to avoid problems with
     sizes and alignments of its fields.  */
  swapped_hdr = *exec_hdr(abfd);
  swap_exec_header (abfd, &swapped_hdr);

  bfd_seek (abfd, 0L, SEEK_SET);
  bfd_write ((PTR) &swapped_hdr, 1, sizeof (struct exec), abfd);

  /* Now write out reloc info, followed by syms and strings */
  if (bfd_get_symcount (abfd) != 0) 
    {
      bfd_seek (abfd,
		(long)(N_SYMOFF(*exec_hdr(abfd))), SEEK_SET);

      aout_write_syms (abfd);

      bfd_seek (abfd,	(long)(N_TROFF(*exec_hdr(abfd))), SEEK_SET);

      if (!b_out_squirt_out_relocs (abfd, obj_textsec (abfd))) return false;
      bfd_seek (abfd, (long)(N_DROFF(*exec_hdr(abfd))), SEEK_SET);

      if (!b_out_squirt_out_relocs (abfd, obj_datasec (abfd))) return false;
    }
  return true;
}

static void
swap_exec_header (abfd, execp)
     bfd *abfd;
     struct exec *execp;
{
#define swapme(field)	field = bfd_h_get_32 (abfd, (unsigned char *)&field);
  swapme (execp->a_magic);
  swapme (execp->a_text);
  swapme (execp->a_data);
  swapme (execp->a_bss);
  swapme (execp->a_syms);
  swapme (execp->a_entry);
  swapme (execp->a_trsize);
  swapme (execp->a_drsize);
  swapme (execp->a_tload);
  swapme (execp->a_dload);
  /* talign, dalign, and balign are one-byte fields and don't swap.  */
#undef swapme
}

/** Some reloc hackery */

#define CALLS	 0x66003800	/* Template for 'calls' instruction	*/
#define BAL	 0x0b000000	/* Template for 'bal' instruction	*/
#define BAL_MASK 0x00ffffff

static bfd_reloc_status_enum_type 
callj_callback(abfd, reloc_entry, symbol_in, data, input_section)
bfd *abfd;
arelent *reloc_entry;
asymbol *symbol_in;
unsigned char *data;
asection *input_section;
{
  int  word = bfd_get_32(abfd, data+reloc_entry->address);
  aout_symbol_type  *symbol = aout_symbol(symbol_in);

  if (IS_OTHER(symbol->other)) {
    /* Call to a system procedure - replace code with system
       procedure number 
       */

    word = CALLS | (symbol->other - 1);
    bfd_put_32(abfd, word,  data+reloc_entry->address); /* replace */
    return bfd_reloc_ok;
  }
    
  if (IS_CALLNAME(symbol->other)) {
    aout_symbol_type *balsym = symbol+1;
    /* The next symbol should be an N_BALNAME */
    BFD_ASSERT(IS_BALNAME(balsym->other));

    /* We are calling a leaf - so replace the call instruction
       with a bal */
  
    word = BAL |
      (((word & BAL_MASK) +
 	balsym->symbol.section->output_offset +
 	balsym->symbol.section->output_section->vma+
	balsym->symbol.value + reloc_entry->addend - 
	( input_section->output_section->vma + input_section->output_offset))
       & BAL_MASK);

    bfd_put_32(abfd, word,  data+reloc_entry->address); /* replace */
    return bfd_reloc_ok;
  }
  return bfd_reloc_continue;

}
/* type rshift size  bitsize  	pcrel	bitpos  absolute overflow check*/


static reloc_howto_type howto_reloc_callj =
HOWTO( 3, 0, 2, 24, true, 0, true, true, callj_callback,"callj", true, 0x00ffffff, 0x00ffffff,false);
static  reloc_howto_type howto_reloc_abs32 =
HOWTO(1, 0, 2, 32, false, 0, true, true,0,"abs32", true, 0xffffffff,0xffffffff,false);
static reloc_howto_type howto_reloc_pcrel24 =
HOWTO(2, 0, 2, 24, true, 0, true, true,0,"pcrel24", true, 0x00ffffff,0x00ffffff,false);

/* Allocate enough room for all the reloc entries, plus pointers to them all */

static boolean
b_out_slurp_reloc_table (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  unsigned int count;
  size_t  reloc_size;
  struct relocation_info *relocs;
  arelent *reloc_cache;

  if (asect->relocation) return true;
  if (!aout_slurp_symbol_table (abfd)) return false;

  if (asect == obj_datasec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_drsize;
    goto doit;
  }

  if (asect == obj_textsec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_trsize;
    goto doit;
  }

  bfd_error = invalid_operation;
  return false;

 doit:
  bfd_seek (abfd, (long)(asect->rel_filepos), SEEK_SET);
  count = reloc_size / sizeof (struct relocation_info);

  relocs = (struct relocation_info *) malloc (reloc_size);
  if (!relocs) {
    bfd_error = no_memory;
    return false;
  }
  reloc_cache = (arelent *) malloc ((count+1) * sizeof (arelent));
  if (!reloc_cache) {
    free ((char*)relocs);
    bfd_error = no_memory;
    return false;
  }

  if (bfd_read ((PTR) relocs, 1, reloc_size, abfd) != reloc_size) {
    bfd_error = system_call_error;
    free (reloc_cache);
    free (relocs);
    return false;
  }

  {
    register struct relocation_info *rptr = relocs;
    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;
    int extern_mask, pcrel_mask, callj_mask;
  
    if (abfd->xvec->header_byteorder_big_p) {
      /* Big-endian bit field allocation order */
      pcrel_mask  = 0x80;
      extern_mask = 0x10;
      callj_mask  = 0x02;
    } else {
      /* Little-endian bit field allocation order */
      pcrel_mask  = 0x01;
      extern_mask = 0x08;
      callj_mask  = 0x40;
    }

    for (; counter < count; counter++, rptr++, cache_ptr++) 
      {
	unsigned char *raw = (unsigned char *)rptr;
	unsigned int symnum;
	cache_ptr->address = bfd_h_get_32 (abfd, raw + 0);
 	if (abfd->xvec->header_byteorder_big_p) {
	  symnum = (raw[4] << 16) | (raw[5] << 8) | raw[6];
 	} else {
	  symnum = (raw[6] << 16) | (raw[5] << 8) | raw[4];
	}

	if (raw[7] & extern_mask) {
	  /* If this is set then the r_index is a index into the symbol table;
	   * if the bit is not set then r_index contains a section map.
	   * We either fill in the sym entry with a pointer to the symbol,
	   * or point to the correct section
	   */
	  cache_ptr->sym_ptr_ptr = symbols + symnum;
	  cache_ptr->addend = 0;
	  cache_ptr->section = (asection*)NULL;
	} else {
	  /* In a.out symbols are relative to the beginning of the
	   * file rather than sections ?
	   * (look in translate_from_native_sym_flags)
	   * The reloc entry addend has added to it the offset into the
	   * file of the data, so subtract the base to make the reloc
	   * section relative */
	  cache_ptr->sym_ptr_ptr = (asymbol **)NULL;
	  switch (symnum) {
	  case N_TEXT:
	  case N_TEXT | N_EXT:
	    cache_ptr->section = obj_textsec(abfd);
	    cache_ptr->addend = -obj_textsec(abfd)->vma;
	    break;
	  case N_DATA:
	  case N_DATA | N_EXT:
	    cache_ptr->section = obj_datasec(abfd);
	    cache_ptr->addend = - obj_datasec(abfd)->vma;
	    break;
	  case N_BSS:
	  case N_BSS | N_EXT:
	    cache_ptr->section = obj_bsssec(abfd);
	    cache_ptr->addend =  - obj_bsssec(abfd)->vma;
	    break;
	  case N_ABS:
	  case N_ABS | N_EXT:
	    BFD_ASSERT(0);
	    break;
	  default:
	    BFD_ASSERT(0);
	    break;
	  }
	
	}

 	/* The i960 only has a few relocation types:
	   abs 32-bit and pcrel 24bit.   Except for callj's!  */
	if (raw[7] & callj_mask)
	  cache_ptr->howto = &howto_reloc_callj;
	else if ( raw[7] & pcrel_mask)
	  cache_ptr->howto = &howto_reloc_pcrel24;
	else
	  cache_ptr->howto = &howto_reloc_abs32;
      }
  }

  free (relocs);
  asect->relocation = reloc_cache;
  asect->reloc_count = count;
  return true;
}


static boolean
b_out_squirt_out_relocs (abfd, section)
     bfd *abfd;
     asection *section;
{
  arelent **generic;

  unsigned int count = section->reloc_count;
  struct relocation_info *native, *natptr;
  size_t natsize = count * sizeof (struct relocation_info);
  int extern_mask, pcrel_mask,  len_2, callj_mask;
  if (count == 0) return true;
  generic   = section->orelocation;
  native = ((struct relocation_info *) malloc (natsize));
  if (!native) {
    bfd_error = no_memory;
    return false;
  }

   if (abfd->xvec->header_byteorder_big_p) {
       /* Big-endian bit field allocation order */
       pcrel_mask  = 0x80;
       extern_mask = 0x10;
       len_2       = 0x40;
      callj_mask  = 0x02;
   } else {
       /* Little-endian bit field allocation order */
       pcrel_mask  = 0x01;
       extern_mask = 0x08;
       len_2       = 0x04;
      callj_mask  = 0x40;
   }

  for (natptr = native; count > 0; --count, ++natptr, ++generic) 
    {
      arelent *g = *generic;
      unsigned char *raw = (unsigned char *)natptr;
      unsigned int symnum;

      bfd_h_put_32(abfd, g->address, raw);  
      /* Find a type in the output format which matches the input howto - 
       * at the moment we assume input format == output format FIXME!!
       */
      /* FIXME:  Need callj stuff here, and to check the howto entries to
	 be sure they are real for this architecture.  */
      if (g->howto== &howto_reloc_callj) {
	raw[7] = callj_mask + pcrel_mask + len_2;
      }
      else if (g->howto == &howto_reloc_pcrel24) {
	raw[7] = pcrel_mask +len_2;
      }
      else {
	raw[7] = len_2;
      }
      if (g->sym_ptr_ptr != (asymbol **)NULL) 
	{
	  /* name clobbered by aout_write_syms to be symbol index*/
	  if ((*(g->sym_ptr_ptr))->section) {
	    /* replace the section offset into the addent */
	    g->addend += (*(g->sym_ptr_ptr))->section->vma ;
	  }
	  symnum = stoi((*(g->sym_ptr_ptr))->name);
	  raw[7] |= extern_mask;
	  BFD_ASSERT(g->addend == 0);
	}
      else {
	if (g->section == (asection *)NULL) {
	  symnum = N_ABS;
	  BFD_ASSERT(0);
	}
	else  if(g->section->output_section == obj_textsec(abfd)) {
	  symnum = N_TEXT;
	  BFD_ASSERT(g->addend == 0);
	}
	else if (g->section->output_section == obj_datasec(abfd)) {
	  symnum  = N_DATA;
	  BFD_ASSERT(g->addend + obj_textsec(abfd)->size == 0);
	}
	else if (g->section->output_section == obj_bsssec(abfd)) {
	  symnum = N_BSS;
	  BFD_ASSERT(g->addend + obj_textsec(abfd)->size 
	    + obj_datasec(abfd)->size == 0);

	}
	else {
	  BFD_ASSERT(0);
	}
      }
      if (abfd->xvec->header_byteorder_big_p) {
	raw[4] = (unsigned char) (symnum >> 16);
	raw[5] = (unsigned char) (symnum >>  8);
	raw[6] = (unsigned char) (symnum      );
      } else {
	raw[6] = (unsigned char) (symnum >> 16);
	raw[5] = (unsigned char) (symnum >>  8);
	raw[4] = (unsigned char) (symnum      );
      }  
    }

  if ( bfd_write ((PTR) native, 1, natsize, abfd) != natsize) {
    free((PTR)native);
    return false;
  }
  free ((PTR)native);

  return true;
}

/* This is stupid.  This function should be a boolean predicate */
static unsigned int
b_out_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *tblptr = section->relocation;
  unsigned int count = 0;

 if (!(tblptr || b_out_slurp_reloc_table (abfd, section, symbols))) return 0;
  tblptr = section->relocation;
 if (!tblptr) return 0;

  for (; count++ < section->reloc_count;)
    *relptr++ = tblptr++;

  *relptr = 0;

  return section->reloc_count;
}

static unsigned int
b_out_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (bfd_get_format (abfd) != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  if (asect == obj_datasec (abfd))
    return (sizeof (arelent *) *
	    ((exec_hdr(abfd)->a_drsize / sizeof (struct relocation_info))
	     +1));

  if (asect == obj_textsec (abfd))
    return (sizeof (arelent *) *
	    ((exec_hdr(abfd)->a_trsize / sizeof (struct relocation_info))
	     +1));

  bfd_error = invalid_operation;
  return 0;
}

static boolean
b_out_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     unsigned char *location;
     file_ptr offset;
      int count;
{
  if (abfd->output_has_begun == false) { /* set by bfd.c handler */
    if ((obj_textsec (abfd) == NULL) || (obj_datasec (abfd) == NULL) /*||
        (obj_textsec (abfd)->size == 0) || (obj_datasec (abfd)->size == 0)*/) {
      bfd_error = invalid_operation;
      return false;
    }

    obj_textsec (abfd)->filepos = sizeof(struct exec);
    obj_datasec(abfd)->filepos = obj_textsec(abfd)->filepos 
                                +  obj_textsec (abfd)->size;

  }
  /* regardless, once we know what we're doing, we might as well get going */
  bfd_seek (abfd, section->filepos + offset, SEEK_SET);

  if (count != 0) {
    return (bfd_write ((PTR)location, 1, count, abfd) == count) ?true:false;
  }
  return false;
}

static boolean
b_out_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  abfd->obj_arch = arch;
  abfd->obj_machine = machine;
  if (arch == bfd_arch_unknown)	/* Unknown machine arch is OK */
    return true;
  if (arch == bfd_arch_i960)	/* i960 default is OK */
    switch (machine) {
    case bfd_mach_i960_core:
    case bfd_mach_i960_kb_sb:
    case bfd_mach_i960_mc:
    case bfd_mach_i960_xa:
    case bfd_mach_i960_ca:
    case bfd_mach_i960_ka_sa:
      return true;
    default:
      return false;
    }

  return false;
}

static int 
DEFUN(b_out_sizeof_headers,(abfd, exec),
      bfd *abfd AND
      boolean execable)
{
return sizeof(struct exec);
}

/* Build the transfer vectors for Big and Little-Endian B.OUT files.  */

/* We don't have core files.  */
#define	aout_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define	aout_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define	aout_core_file_matches_executable_p	\
				_bfd_dummy_core_file_matches_executable_p

/* We use BSD-Unix generic archive files.  */
#define	aout_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	aout_slurp_armap		bfd_slurp_bsd_armap
#define	aout_slurp_extended_name_table	bfd_true
#define	aout_write_armap		bsd_write_armap
#define	aout_truncate_arname		bfd_bsd_truncate_arname

/* We override these routines from the usual a.out file routines.  */
#define	aout_canonicalize_reloc		b_out_canonicalize_reloc
#define	aout_get_reloc_upper_bound	b_out_get_reloc_upper_bound
#define	aout_set_section_contents	b_out_set_section_contents
#define	aout_set_arch_mach		b_out_set_arch_mach
#define	aout_sizeof_headers		b_out_sizeof_headers


bfd_target b_out_vec_big_host =
{
  "b.out.big",			/* name */
  bfd_target_aout_flavour_enum,
  false,			/* data byte order is little */
  true,				/* hdr byte order is big */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT ),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */


_do_getl64, _do_putl64,  _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
_do_getb64, _do_putb64,  _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
    {_bfd_dummy_target, b_out_big_object_p, /* bfd_check_format */
       bfd_generic_archive_p, _bfd_dummy_target},
    {bfd_false, b_out_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, b_out_write_object_contents,	/* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(aout)
};


bfd_target b_out_vec_little_host =
{
  "b.out.little",		/* name */
  bfd_target_aout_flavour_enum,
  false,			/* data byte order is little */
  false,			/* header byte order is little */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT ),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
	 
_do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
_do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* hdrs */
	 
    {_bfd_dummy_target, b_out_little_object_p, /* bfd_check_format */
       bfd_generic_archive_p, _bfd_dummy_target},
    {bfd_false, b_out_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, b_out_write_object_contents,	/* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE(aout)
};

