/* BFD back-end for Intel 960 b.out binaries.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
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

/* $Id$ */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#include "bout.h"

#include "aout/stab_gnu.h"
#include "libaout.h"		/* BFD a.out internal data structures */

PROTO (static boolean, b_out_squirt_out_relocs,(bfd *abfd, asection *section));
PROTO (static bfd_target *, b_out_callback, (bfd *));

PROTO (boolean, aout_32_slurp_symbol_table, (bfd *abfd));
PROTO (void , aout_32_write_syms, ());

/* Swaps the information in an executable header taken from a raw byte
   stream memory image, into the internal exec_header structure.  */

PROTO(void, bout_swap_exec_header_in,
      (bfd *abfd,
      struct external_exec *raw_bytes,
      struct internal_exec *execp));
	 
void
DEFUN(bout_swap_exec_header_in,(abfd, raw_bytes, execp),
      bfd *abfd AND
      struct external_exec *raw_bytes AND
      struct internal_exec *execp)
{
  struct external_exec *bytes = (struct external_exec *)raw_bytes;

  /* Now fill in fields in the execp, from the bytes in the raw data.  */
  execp->a_info   = bfd_h_get_32 (abfd, bytes->e_info);
  execp->a_text   = GET_WORD (abfd, bytes->e_text);
  execp->a_data   = GET_WORD (abfd, bytes->e_data);
  execp->a_bss    = GET_WORD (abfd, bytes->e_bss);
  execp->a_syms   = GET_WORD (abfd, bytes->e_syms);
  execp->a_entry  = GET_WORD (abfd, bytes->e_entry);
  execp->a_trsize = GET_WORD (abfd, bytes->e_trsize);
  execp->a_drsize = GET_WORD (abfd, bytes->e_drsize);
  execp->a_tload  = GET_WORD (abfd, bytes->e_tload);
  execp->a_dload  = GET_WORD (abfd, bytes->e_dload);
  execp->a_talign = bytes->e_talign[0];
  execp->a_dalign = bytes->e_dalign[0];
  execp->a_balign = bytes->e_balign[0];
}

/* Swaps the information in an internal exec header structure into the
   supplied buffer ready for writing to disk.  */

PROTO(void, bout_swap_exec_header_out,
	  (bfd *abfd,
	   struct internal_exec *execp,
	   struct external_exec *raw_bytes));
void
DEFUN(bout_swap_exec_header_out,(abfd, execp, raw_bytes),
     bfd *abfd AND
     struct internal_exec *execp AND 
     struct external_exec *raw_bytes)
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
  PUT_WORD (abfd, execp->a_tload , bytes->e_tload);
  PUT_WORD (abfd, execp->a_dload , bytes->e_dload);
  bytes->e_talign[0] = execp->a_talign;
  bytes->e_dalign[0] = execp->a_dalign;
  bytes->e_balign[0] = execp->a_balign;
  bytes->e_unused[0] = 0;		/* Clean structs are godly structs */
}


static bfd_target *
b_out_object_p (abfd)
     bfd *abfd;
{
  struct internal_exec anexec;
  struct external_exec exec_bytes;

  if (bfd_read ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd)
      != EXEC_BYTES_SIZE) {
    bfd_error = wrong_format;
    return 0;
  }

  anexec.a_info = bfd_h_get_32 (abfd, exec_bytes.e_info);

  if (N_BADMAG (anexec)) {
    bfd_error = wrong_format;
    return 0;
  }

  bout_swap_exec_header_in (abfd, &exec_bytes, &anexec);
  return aout_32_some_aout_object_p (abfd, &anexec, b_out_callback);
}


/* Finish up the opening of a b.out file for reading.  Fill in all the
   fields that are not handled by common code.  */

static bfd_target *
b_out_callback (abfd)
     bfd *abfd;
{
  struct internal_exec *execp = exec_hdr (abfd);
  unsigned long bss_start;

  /* Architecture and machine type */
  bfd_set_arch_mach(abfd, 
		    bfd_arch_i960, /* B.out only used on i960 */
		    bfd_mach_i960_core /* Default */
		    );

  /* The positions of the string table and symbol table.  */
  obj_str_filepos (abfd) = N_STROFF (*execp);
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);

  /* The alignments of the sections */
  obj_textsec (abfd)->alignment_power = execp->a_talign;
  obj_datasec (abfd)->alignment_power = execp->a_dalign;
  obj_bsssec  (abfd)->alignment_power = execp->a_balign;

  /* The starting addresses of the sections.  */
  obj_textsec (abfd)->vma = execp->a_tload;
  obj_datasec (abfd)->vma = execp->a_dload;

  /* And reload the sizes, since the aout module zaps them */
  obj_textsec (abfd)->_raw_size = execp->a_text;

  bss_start = execp->a_dload + execp->a_data; /* BSS = end of data section */
  obj_bsssec (abfd)->vma = align_power (bss_start, execp->a_balign);

  /* The file positions of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF(*execp);
  obj_datasec (abfd)->filepos = N_DATOFF(*execp);

  /* The file positions of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TROFF(*execp);
  obj_datasec (abfd)->rel_filepos =  N_DROFF(*execp);

  adata(abfd).page_size = 1; /* Not applicable. */
  adata(abfd).segment_size = 1; /* Not applicable. */
  adata(abfd).exec_bytes_size = EXEC_BYTES_SIZE;

  return abfd->xvec;
}

struct bout_data_struct {
    struct aoutdata a;
    struct internal_exec e;
};

static boolean
b_out_mkobject (abfd)
     bfd *abfd;
{
  struct bout_data_struct *rawptr;

  rawptr = (struct bout_data_struct *) bfd_zalloc (abfd, sizeof (struct bout_data_struct));
  if (rawptr == NULL) {
      bfd_error = no_memory;
      return false;
    }

  abfd->tdata.bout_data = rawptr;
  exec_hdr (abfd) = &rawptr->e;

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
  struct external_exec swapped_hdr;

  exec_hdr (abfd)->a_info = BMAGIC;

  exec_hdr (abfd)->a_text = obj_textsec (abfd)->_raw_size;
  exec_hdr (abfd)->a_data = obj_datasec (abfd)->_raw_size;
  exec_hdr (abfd)->a_bss = obj_bsssec (abfd)->_raw_size;
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

  bout_swap_exec_header_out (abfd, exec_hdr (abfd), &swapped_hdr);

  bfd_seek (abfd, 0L, SEEK_SET);
  bfd_write ((PTR) &swapped_hdr, 1, EXEC_BYTES_SIZE, abfd);

  /* Now write out reloc info, followed by syms and strings */
  if (bfd_get_symcount (abfd) != 0) 
    {
      bfd_seek (abfd,
		(long)(N_SYMOFF(*exec_hdr(abfd))), SEEK_SET);

      aout_32_write_syms (abfd);

      bfd_seek (abfd,	(long)(N_TROFF(*exec_hdr(abfd))), SEEK_SET);

      if (!b_out_squirt_out_relocs (abfd, obj_textsec (abfd))) return false;
      bfd_seek (abfd, (long)(N_DROFF(*exec_hdr(abfd))), SEEK_SET);

      if (!b_out_squirt_out_relocs (abfd, obj_datasec (abfd))) return false;
    }
  return true;
}

/** Some reloc hackery */

#define CALLS	 0x66003800	/* Template for 'calls' instruction	*/
#define BAL	 0x0b000000	/* Template for 'bal' instruction	*/
#define BAL_MASK 0x00ffffff

static bfd_reloc_status_type 
callj_callback(abfd, reloc_entry, symbol_in, data, input_section)
bfd *abfd;
arelent *reloc_entry;
asymbol *symbol_in;
PTR data;
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
  register struct relocation_info *rptr;
  unsigned int counter ;
  arelent *cache_ptr ;
  int extern_mask, pcrel_mask, callj_mask;

  unsigned int count;
  size_t  reloc_size;
  struct relocation_info *relocs;
  arelent *reloc_cache;

  if (asect->relocation) return true;
  if (!aout_32_slurp_symbol_table (abfd)) return false;

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
  bfd_seek (abfd, (long)(asect->rel_filepos),  SEEK_SET);
  count = reloc_size / sizeof (struct relocation_info);

  relocs = (struct relocation_info *) bfd_xmalloc (reloc_size);
  if (!relocs) {
      bfd_error = no_memory;
      return false;
    }
  reloc_cache = (arelent *) bfd_xmalloc ((count+1) * sizeof (arelent));
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


  
  if (abfd->xvec->header_byteorder_big_p) {
      /* big-endian bit field allocation order */
      pcrel_mask  = 0x80;
      extern_mask = 0x10;
      callj_mask  = 0x02;
    } else {
	/* little-endian bit field allocation order */
	pcrel_mask  = 0x01;
	extern_mask = 0x08;
	callj_mask  = 0x40;
      }

  for (rptr = relocs, cache_ptr = reloc_cache, counter = 0;
       counter < count;
       counter++, rptr++, cache_ptr++) 
  {
    unsigned char *raw = (unsigned char *)rptr;
    unsigned int symnum;
    cache_ptr->address = bfd_h_get_32 (abfd, raw + 0);
    if (abfd->xvec->header_byteorder_big_p) 
    {
      symnum = (raw[4] << 16) | (raw[5] << 8) | raw[6];
    } 
    else
    {
      symnum = (raw[6] << 16) | (raw[5] << 8) | raw[4];
    }

    if (raw[7] & extern_mask) 
    {
	/* if this is set then the r_index is a index into the symbol table;
	 * if the bit is not set then r_index contains a section map.
	 * we either fill in the sym entry with a pointer to the symbol,
	 * or point to the correct section
	 */
	cache_ptr->sym_ptr_ptr = symbols + symnum;
	cache_ptr->addend = 0;
      } else 
      {
	  /* in a.out symbols are relative to the beginning of the
	   * file rather than sections ?
	   * (look in translate_from_native_sym_flags)
	   * the reloc entry addend has added to it the offset into the
	   * file of the data, so subtract the base to make the reloc
	   * section relative */
	  cache_ptr->sym_ptr_ptr = (asymbol **)NULL;
	  switch (symnum) 
	  {
	    case N_TEXT:
	    case N_TEXT | N_EXT:
	      cache_ptr->sym_ptr_ptr = obj_textsec(abfd)->symbol_ptr_ptr;
	      cache_ptr->addend = - obj_textsec(abfd)->vma;
	      break;
	    case N_DATA:
	    case N_DATA | N_EXT:
	      cache_ptr->sym_ptr_ptr = obj_datasec(abfd)->symbol_ptr_ptr;
	      cache_ptr->addend = - obj_datasec(abfd)->vma;
	      break;
	    case N_BSS:
	    case N_BSS | N_EXT:
	      cache_ptr->sym_ptr_ptr = obj_bsssec(abfd)->symbol_ptr_ptr;
	      cache_ptr->addend =  - obj_bsssec(abfd)->vma;
	      break;
	    case N_ABS:
	    case N_ABS | N_EXT:
	      bfd_assert(0);
	      break;
	    default:
	      bfd_assert(0);
	      break;
	    }
	
	}

    /* the i960 only has a few relocation types:
       abs 32-bit and pcrel 24bit.   except for callj's!  */
    if (raw[7] & callj_mask)
     cache_ptr->howto = &howto_reloc_callj;
    else if ( raw[7] & pcrel_mask)
     cache_ptr->howto = &howto_reloc_pcrel24;
    else
     cache_ptr->howto = &howto_reloc_abs32;
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
  int r_extern;
  int r_idx;
  int r_addend;
  
  unsigned int count = section->reloc_count;
  struct relocation_info *native, *natptr;
  size_t natsize = count * sizeof (struct relocation_info);
  int extern_mask, pcrel_mask,  len_2, callj_mask;
  if (count == 0) return true;
  generic   = section->orelocation;
  native = ((struct relocation_info *) bfd_xmalloc (natsize));
  if (!native) {
      bfd_error = no_memory;
      return false;
    }

  if (abfd->xvec->header_byteorder_big_p) 
  {
    /* Big-endian bit field allocation order */
    pcrel_mask  = 0x80;
    extern_mask = 0x10;
    len_2       = 0x40;
    callj_mask  = 0x02;
  } 
else 
  {
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
    asymbol *sym = *(g->sym_ptr_ptr);
      
    asection *output_section = sym->section->output_section;
    bfd_h_put_32(abfd, g->address, raw);  
    /* Find a type in the output format which matches the input howto - 
     * at the moment we assume input format == output format FIXME!!
     */
    /* FIXME:  Need callj stuff here, and to check the howto entries to
       be sure they are real for this architecture.  */
    if (g->howto== &howto_reloc_callj) 
    {
      raw[7] = callj_mask + pcrel_mask + len_2;
    }
    else if (g->howto == &howto_reloc_pcrel24) 
    {
      raw[7] = pcrel_mask +len_2;
    }
    else {
	raw[7] = len_2;
      }
    if (output_section == &bfd_abs_section) 
    {
      r_extern = 0;
      r_idx = N_ABS;
      r_addend += sym->value;
    }
    else if (output_section == &bfd_com_section 
	     || output_section == &bfd_und_section) 
    {
      /* Fill in symbol */
      r_extern = 1;
      r_idx =  stoi((*(g->sym_ptr_ptr))->flags);
    }
    else 
    {
      /* Just an ordinary section */
      r_extern = 0;
      r_idx  = output_section->target_index;      
    }

    if (abfd->xvec->header_byteorder_big_p) {
	raw[4] = (unsigned char) (r_idx >> 16);
	raw[5] = (unsigned char) (r_idx >>  8);
	raw[6] = (unsigned char) (r_idx     );
      } else {
	  raw[6] = (unsigned char) (r_idx >> 16);
	  raw[5] = (unsigned char) (r_idx>>  8);
	  raw[4] = (unsigned char) (r_idx     );
	}  
if (r_extern)
    raw[7] |= extern_mask; 
  }

  if (bfd_write ((PTR) native, 1, natsize, abfd) != natsize) {
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
        (obj_textsec (abfd)->_cooked_size == 0) || (obj_datasec (abfd)->_cooked_size == 0)*/) {
      bfd_error = invalid_operation;
      return false;
    }

    obj_textsec (abfd)->filepos = sizeof(struct internal_exec);
    obj_datasec(abfd)->filepos = obj_textsec(abfd)->filepos 
                                +  obj_textsec (abfd)->_cooked_size;

  }
  /* regardless, once we know what we're doing, we might as well get going */
  bfd_seek (abfd, section->filepos + offset, SEEK_SET);

  if (count != 0) {
    return (bfd_write ((PTR)location, 1, count, abfd) == count) ?true:false;
  }
  return true;
}

static boolean
b_out_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  bfd_default_set_arch_mach(abfd, arch, machine);

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
    case 0:
      return true;
    default:
      return false;
    }

  return false;
}

static int 
DEFUN(b_out_sizeof_headers,(ignore_abfd, ignore),
      bfd *ignore_abfd AND
      boolean ignore)
{
  return sizeof(struct internal_exec);
}




/* Build the transfer vectors for Big and Little-Endian B.OUT files.  */

/* We don't have core files.  */
#define	aout_32_core_file_failing_command _bfd_dummy_core_file_failing_command
#define	aout_32_core_file_failing_signal _bfd_dummy_core_file_failing_signal
#define	aout_32_core_file_matches_executable_p	\
				_bfd_dummy_core_file_matches_executable_p

/* We use BSD-Unix generic archive files.  */
#define	aout_32_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_32_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	aout_32_slurp_armap		bfd_slurp_bsd_armap
#define	aout_32_slurp_extended_name_table	bfd_true
#define	aout_32_write_armap		bsd_write_armap
#define	aout_32_truncate_arname		bfd_bsd_truncate_arname

/* We override these routines from the usual a.out file routines.  */
#define	aout_32_canonicalize_reloc	b_out_canonicalize_reloc
#define	aout_32_get_reloc_upper_bound	b_out_get_reloc_upper_bound
#define	aout_32_set_section_contents	b_out_set_section_contents
#define	aout_32_set_arch_mach		b_out_set_arch_mach
#define	aout_32_sizeof_headers		b_out_sizeof_headers

#define aout_32_bfd_debug_info_start		bfd_void
#define aout_32_bfd_debug_info_end		bfd_void
#define aout_32_bfd_debug_info_accumulate	(PROTO(void,(*),(bfd*, struct sec *))) bfd_void

#define aout_32_bfd_get_relocated_section_contents  bfd_generic_get_relocated_section_contents
#define aout_32_bfd_relax_section                   bfd_generic_relax_section
bfd_target b_out_vec_big_host =
{
  "b.out.big",			/* name */
  bfd_target_aout_flavour,
  false,			/* data byte order is little */
  true,				/* hdr byte order is big */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT ),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
     2,				/* minumum alignment power */

_do_getl64, _do_putl64,  _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
_do_getb64, _do_putb64,  _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */
    {_bfd_dummy_target, b_out_object_p, /* bfd_check_format */
       bfd_generic_archive_p, _bfd_dummy_target},
    {bfd_false, b_out_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, b_out_write_object_contents,	/* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(aout_32)
};


bfd_target b_out_vec_little_host =
{
  "b.out.little",		/* name */
  bfd_target_aout_flavour,
  false,			/* data byte order is little */
  false,			/* header byte order is little */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT ),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
     2,				/* minum align */
_do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
_do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16, /* hdrs */
	 
    {_bfd_dummy_target, b_out_object_p, /* bfd_check_format */
       bfd_generic_archive_p, _bfd_dummy_target},
    {bfd_false, b_out_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, b_out_write_object_contents,	/* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE(aout_32)
};
