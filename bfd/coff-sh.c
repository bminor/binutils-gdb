/* BFD back-end for Hitachi Super-H COFF binaries.
   Copyright 1993, 1994 Free Software Foundation, Inc.
   Contributed by Cygnus Support.
   Written by Steve Chamberlain, <sac@cygnus.com>.

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

#include "bfd.h"
#include "sysdep.h"
#include "obstack.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "coff/sh.h"
#include "coff/internal.h"
#include "libcoff.h"

static bfd_reloc_status_type sh_reloc();


static reloc_howto_type r_imm32 =
  {R_SH_IMM32,  0, 2, 32, false, 0, 
     complain_overflow_bitfield, sh_reloc,"r_imm32",    true, 0xffffffff,0xffffffff, false};


/*#define SELECT_RELOC(x,y) x->howto = (&r_imm32)*/


#define BADMAG(x) SHBADMAG(x)
#define SH 1			/* Customize coffcode.h */

#define __A_MAGIC_SET__

/* Code to swap in the reloc */
#if 0
#define SWAP_IN_RELOC_OFFSET   bfd_h_get_32
#define SWAP_OUT_RELOC_OFFSET bfd_h_put_32
#endif
#define SWAP_OUT_RELOC_EXTRA(abfd, src, dst) \
  dst->r_stuff[0] = 'S'; \
  dst->r_stuff[1] = 'C';

/* Code to turn a r_type into a howto ptr, uses the above howto table
   */
static long
get_symbol_value (symbol)       
     asymbol *symbol;
{                                             
  long relocation = 0;

  if (bfd_is_com_section (symbol->section))
  {
    relocation = 0;                           
  }
  else 
  {                                      
    relocation = symbol->value +
     symbol->section->output_section->vma +
      symbol->section->output_offset;
  }                                           

  return(relocation);
}

static void
rtype2howto (internal, dst)
     arelent * internal;
     struct internal_reloc *dst;
{
  switch (dst->r_type)
    {
    default:
      abort();
    case R_SH_IMM32:

      break;
    }
}

#define RTYPE2HOWTO(x,y) ((x)->howto = &r_imm32)

/* Perform any necessaru magic to the addend in a reloc entry */

#if 0
#define CALC_ADDEND(abfd, symbol, ext_reloc, cache_ptr) \
 cache_ptr->addend =  ext_reloc.r_offset;

#endif


/* Compute the addend of a reloc.  If the reloc is to a common symbol,
   the object file contains the value of the common symbol.  By the
   time this is called, the linker may be using a different symbol
   from a different object file with a different value.  Therefore, we
   hack wildly to locate the original symbol from this file so that we
   can make the correct adjustment.  This macro sets coffsym to the
   symbol from the original file, and uses it to set the addend value
   correctly.  If this is not a common symbol, the usual addend
   calculation is done, except that an additional tweak is needed for
   PC relative relocs.
   FIXME: This macro refers to symbols and asect; these are from the
   calling function, not the macro arguments.  */

#define CALC_ADDEND(abfd, ptr, reloc, cache_ptr)		\
  {								\
    coff_symbol_type *coffsym = (coff_symbol_type *) NULL;	\
    if (ptr && bfd_asymbol_bfd (ptr) != abfd)			\
      coffsym = (obj_symbols (abfd)				\
	         + (cache_ptr->sym_ptr_ptr - symbols));		\
    else if (ptr)						\
      coffsym = coff_symbol_from (abfd, ptr);			\
    if (coffsym != (coff_symbol_type *) NULL			\
	&& coffsym->native->u.syment.n_scnum == 0)		\
      cache_ptr->addend = - coffsym->native->u.syment.n_value;	\
    else if (ptr && bfd_asymbol_bfd (ptr) == abfd		\
	     && ptr->section != (asection *) NULL)		\
      cache_ptr->addend = - (ptr->section->vma + ptr->value);	\
    else							\
      cache_ptr->addend = 0;					\
  }

#if 0
#define RELOC_PROCESSING(relent,reloc,symbols,abfd,section) \
 reloc_processing(relent, reloc, symbols, abfd, section)
#endif

/* this function is in charge of performing all the 29k relocations */

static bfd_reloc_status_type
sh_reloc (abfd, reloc_entry, symbol_in, data, input_section, output_bfd,
	    error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol_in;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  /* the consth relocation comes in two parts, we have to remember
     the state between calls, in these variables */
  unsigned long insn;
  unsigned long sym_value;
  unsigned short r_type;

  unsigned long addr = reloc_entry->address ; /*+ input_section->vma*/
  bfd_byte  *hit_data =addr + (bfd_byte *)(data);
	
  r_type = reloc_entry->howto->type;

  if (output_bfd) {
    /* Partial linking - do nothing */
    reloc_entry->address += input_section->output_offset;
    return bfd_reloc_ok;
  }

  if (symbol_in != NULL
      && bfd_is_und_section (symbol_in->section))
    {
      /* Keep the state machine happy in case we're called again */
      return (bfd_reloc_undefined);
    }


  sym_value = get_symbol_value(symbol_in);

  switch (r_type) 
    {
    case R_SH_IMM32:
      /* We ignore the previous contents ! */
      insn = sym_value + reloc_entry->addend;  
      bfd_put_32(abfd, insn, hit_data);
      break;
    default:
      *error_message = "Unrecognized reloc";
      return (bfd_reloc_dangerous);
    }


  return(bfd_reloc_ok);	
}

/* The reloc processing routine for the optimized COFF linker.  */

static boolean
coff_sh_relocate_section (output_bfd, info, input_bfd, input_section,
			    contents, relocs, syms, sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     struct internal_reloc *relocs;
     struct internal_syment *syms;
     asection **sections;
{
  struct internal_reloc *rel;
  struct internal_reloc *relend;

  /* If we are performing a relocateable link, we don't need to do a
     thing.  The caller will take care of adjusting the reloc
     addresses and symbol indices.  */
  if (info->relocateable)
    return true;


  rel = relocs;
  relend = rel + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      long symndx;
      bfd_byte *loc;
      struct coff_link_hash_entry *h;
      struct internal_syment *sym;
      asection *sec;
      bfd_vma val;
      bfd_reloc_status_type rstat;

      symndx = rel->r_symndx;
      loc = contents + rel->r_vaddr - input_section->vma;

      if (symndx == -1)
	h = NULL;
      else
	h = obj_coff_sym_hashes (input_bfd)[symndx];

      sym = NULL;
      sec = NULL;
      val = 0;


      if (h == NULL)
	{
	  if (symndx == -1)
	    sec = bfd_abs_section_ptr;
	  else
	    {
	      sym = syms + symndx;
	      sec = sections[symndx];
	      val = (sec->output_section->vma
		     + sec->output_offset
		     + sym->n_value
		     - sec->vma 
		     - sec->vma);
	    }
	}
      else
	{
	  if (h->root.type == bfd_link_hash_defined)
	    {
	      sec = h->root.u.def.section;
	      val = (h->root.u.def.value
		     + sec->output_section->vma
		     + sec->output_offset);
	    }
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd, input_section,
		      rel->r_vaddr - input_section->vma)))
		return false;
	    }
	}

      switch (rel->r_type)
	{
	default:
	  bfd_set_error (bfd_error_bad_value);
	  return false;

	case R_SH_IMM32:
	  {
	    long x = bfd_get_32 (input_bfd, loc);
	    x += val;
	    bfd_put_32 (input_bfd, x, loc);

	  }

	  break;
	}
    }     

  return true;
}

static void 
reloc_processing (relent, reloc, symbols, abfd, section)
     arelent * relent;
     struct internal_reloc *reloc;
     asymbol ** symbols;
     bfd * abfd;
     asection * section;
{
  asymbol *ptr;
  relent->address = reloc->r_vaddr;
  relent->howto = &r_imm32;


  relent->sym_ptr_ptr = symbols + obj_convert(abfd)[reloc->r_symndx];

  ptr = *(relent->sym_ptr_ptr);

  if (ptr 
      && bfd_asymbol_bfd(ptr) == abfd		

      && ((ptr->flags & BSF_OLD_COMMON)== 0))	
    {						
      relent->addend = 0;
    }						
  else
    {					
      relent->addend = 0;			
    }			
  relent->address-= section->vma;

}

#define coff_relocate_section coff_sh_relocate_section

#include "coffcode.h"

const bfd_target shcoff_vec =
{
  "coff-sh",			/* name */
  bfd_target_coff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | BFD_IS_RELAXABLE ),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC),	/* section flags */
  '_',				/* leading symbol underscore */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  2,				/* minimum section alignment */
     bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
     bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (coff),
     BFD_JUMP_TABLE_COPY (coff),
     BFD_JUMP_TABLE_CORE (_bfd_nocore),
     BFD_JUMP_TABLE_ARCHIVE (_bfd_archive_coff),
     BFD_JUMP_TABLE_SYMBOLS (coff),
     BFD_JUMP_TABLE_RELOCS (coff),
     BFD_JUMP_TABLE_WRITE (coff),
     BFD_JUMP_TABLE_LINK (coff),
     BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

    COFF_SWAP_TABLE,
};

