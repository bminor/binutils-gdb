/* BFD back-end for AMD 29000 COFF binaries.
   Copyright 1990, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
   Contributed by David Wood at New York University 7/8/91.

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

#define A29K 1

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "coff/a29k.h"
#include "coff/internal.h"
#include "libcoff.h"

static long get_symbol_value PARAMS ((asymbol *));
static bfd_reloc_status_type a29k_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));

#define INSERT_HWORD(WORD,HWORD)	\
    (((WORD) & 0xff00ff00) | (((HWORD) & 0xff00) << 8) | ((HWORD)& 0xff))
#define EXTRACT_HWORD(WORD) \
    (((WORD) & 0x00ff0000) >> 8) | ((WORD)& 0xff)
#define SIGN_EXTEND_HWORD(HWORD) \
    ((HWORD) & 0x8000 ? (HWORD)|0xffff0000 : (HWORD))

/* Provided the symbol, returns the value reffed */
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

/* this function is in charge of performing all the 29k relocations */

static bfd_reloc_status_type
a29k_reloc (abfd, reloc_entry, symbol_in, data, input_section, output_bfd,
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
  static boolean part1_consth_active = false;
  static unsigned long part1_consth_value;

  unsigned long insn;
  unsigned long sym_value;
  unsigned long unsigned_value;
  unsigned short r_type;
  long signed_value;

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
    if (r_type == R_IHIHALF) 
    {
      part1_consth_active = true;
      part1_consth_value  = 0;
    }
    return(bfd_reloc_undefined);
  }

  if ((part1_consth_active) && (r_type != R_IHCONST)) 
  {
    part1_consth_active = false;
    *error_message = (char *) "Missing IHCONST";
    return(bfd_reloc_dangerous);
  }


  sym_value = get_symbol_value(symbol_in);

  switch (r_type) 
  {
   case R_IREL: 	
    insn = bfd_get_32(abfd, hit_data); 
    /* Take the value in the field and sign extend it */
    signed_value = EXTRACT_HWORD(insn);
    signed_value = SIGN_EXTEND_HWORD(signed_value);
    signed_value <<= 2;
    signed_value +=  sym_value + reloc_entry->addend;
    if (((signed_value + reloc_entry->address) & ~0x3ffff) == 0)
    {				/* Absolute jmp/call */
      insn |= (1<<24);		/* Make it absolute */
      signed_value += reloc_entry->address;
      /* FIXME: Should we change r_type to R_IABS */
    } 
    else 
    {
      /* Relative jmp/call, so subtract from the value the
	 address of the place we're coming from */
      signed_value -= (input_section->output_section->vma
		       + input_section->output_offset);
      if (signed_value>0x1ffff || signed_value<-0x20000) 
       return(bfd_reloc_overflow);
    }
    signed_value >>= 2;
    insn = INSERT_HWORD(insn, signed_value);
    bfd_put_32(abfd, insn ,hit_data); 
    break;
   case R_ILOHALF: 
    insn = bfd_get_32(abfd, hit_data); 
    unsigned_value = EXTRACT_HWORD(insn);
    unsigned_value +=  sym_value + reloc_entry->addend;
    insn = INSERT_HWORD(insn, unsigned_value);
    bfd_put_32(abfd, insn, hit_data); 
    break;
   case R_IHIHALF:
    insn = bfd_get_32(abfd, hit_data); 
    /* consth, part 1 
       Just get the symbol value that is referenced */
    part1_consth_active = true;
    part1_consth_value = sym_value + reloc_entry->addend;
    /* Don't modify insn until R_IHCONST */
    break;
   case R_IHCONST:	
    insn = bfd_get_32(abfd, hit_data); 
    /* consth, part 2 
       Now relocate the reference */
    if (part1_consth_active == false) {
      *error_message = (char *) "Missing IHIHALF";
      return(bfd_reloc_dangerous);
    }
    /* sym_ptr_ptr = r_symndx, in coff_slurp_reloc_table() */
    unsigned_value = 0;		/*EXTRACT_HWORD(insn) << 16;*/
    unsigned_value += reloc_entry->addend; /* r_symndx */
    unsigned_value += part1_consth_value;
    unsigned_value = unsigned_value >> 16;
    insn = INSERT_HWORD(insn, unsigned_value);
    part1_consth_active = false;
    bfd_put_32(abfd, insn, hit_data); 
    break;
   case R_BYTE:
    insn = bfd_get_8(abfd, hit_data); 
    unsigned_value = insn + sym_value + reloc_entry->addend;	
    if (unsigned_value & 0xffffff00) {
      fprintf(stderr,"Relocation problem : ");
      fprintf(stderr,"byte value too large in module %s\n",
	      abfd->filename); 
      return(bfd_reloc_overflow);
    }
    bfd_put_8(abfd, unsigned_value, hit_data); 
    break;
   case R_HWORD:
    insn = bfd_get_16(abfd, hit_data); 
    unsigned_value = insn + sym_value + reloc_entry->addend;	
    if (unsigned_value & 0xffff0000) {
      fprintf(stderr,"Relocation problem : ");
      fprintf(stderr,"hword value too large in module %s\n",
	      abfd->filename); 
      return(bfd_reloc_overflow);
    }

    bfd_put_16(abfd, insn, hit_data); 
    break;
   case R_WORD:
    insn = bfd_get_32(abfd, hit_data); 
    insn += sym_value + reloc_entry->addend;  
    bfd_put_32(abfd, insn, hit_data);
    break;
   default:
    *error_message = "Unrecognized reloc";
    return (bfd_reloc_dangerous);
  }


  return(bfd_reloc_ok);	
}

/*      type	   rightshift
		       size
			  bitsize
			       pc-relative
				     bitpos
					 absolute
					     complain_on_overflow
						  special_function
						    relocation name
							       partial_inplace 
								      src_mask
*/

/*FIXME: I'm not real sure about this table */
static reloc_howto_type howto_table[] = 
{
  {R_ABS,     0, 3, 32, false, 0, complain_overflow_bitfield,a29k_reloc,"ABS",     true, 0xffffffff,0xffffffff, false},
  {1},  {2},  {3},   {4},  {5},  {6},  {7},  {8},  {9}, {10},
  {11}, {12}, {13}, {14}, {15}, {16}, {17}, {18}, {19}, {20},
  {21}, {22}, {23},
  {R_IREL,    0, 3, 32, true,  0, complain_overflow_signed,a29k_reloc,"IREL",    true, 0xffffffff,0xffffffff, false},
  {R_IABS,    0, 3, 32, false, 0, complain_overflow_bitfield, a29k_reloc,"IABS",    true, 0xffffffff,0xffffffff, false},
  {R_ILOHALF, 0, 3, 16, true,  0, complain_overflow_signed, a29k_reloc,"ILOHALF", true, 0x0000ffff,0x0000ffff, false},
  {R_IHIHALF, 0, 3, 16, true,  16, complain_overflow_signed, a29k_reloc,"IHIHALF", true, 0xffff0000,0xffff0000, false},
  {R_IHCONST, 0, 3, 16, true,  0, complain_overflow_signed, a29k_reloc,"IHCONST", true, 0xffff0000,0xffff0000, false},
  {R_BYTE,    0, 0, 8, false, 0, complain_overflow_bitfield, a29k_reloc,"BYTE",    true, 0x000000ff,0x000000ff, false},
  {R_HWORD,   0, 1, 16, false, 0, complain_overflow_bitfield, a29k_reloc,"HWORD",   true, 0x0000ffff,0x0000ffff, false},
  {R_WORD,    0, 2, 32, false, 0, complain_overflow_bitfield, a29k_reloc,"WORD",    true, 0xffffffff,0xffffffff, false},
};

#define BADMAG(x) A29KBADMAG(x)

#define RELOC_PROCESSING(relent, reloc, symbols, abfd, section) \
 reloc_processing(relent, reloc, symbols, abfd, section)

static void
reloc_processing (relent,reloc, symbols, abfd, section)
     arelent *relent;
     struct internal_reloc *reloc;
     asymbol **symbols;
     bfd *abfd;
     asection *section;
{
    static bfd_vma ihihalf_vaddr = (bfd_vma) -1;

    relent->address = reloc->r_vaddr;		
    relent->howto = howto_table + reloc->r_type;
    if (reloc->r_type == R_IHCONST) 
    {		
      /* The address of an R_IHCONST should always be the address of
	 the immediately preceding R_IHIHALF.  relocs generated by gas
	 are correct, but relocs generated by High C are different (I
	 can't figure out what the address means for High C).  We can
	 handle both gas and High C by ignoring the address here, and
	 simply reusing the address saved for R_IHIHALF.  */
        if (ihihalf_vaddr == (bfd_vma) -1)
	  abort ();
	relent->address = ihihalf_vaddr;
	ihihalf_vaddr = (bfd_vma) -1;
	relent->addend = reloc->r_symndx;		
	relent->sym_ptr_ptr= bfd_abs_section_ptr->symbol_ptr_ptr;
    }
    else 
    {
      asymbol *ptr;
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
      if (reloc->r_type == R_IHIHALF)
	ihihalf_vaddr = relent->address;
      else if (ihihalf_vaddr != (bfd_vma) -1)
	abort ();
  }
}

#include "coffcode.h"

const bfd_target a29kcoff_big_vec =
{
  "coff-a29k-big",		/* name */
  bfd_target_coff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC /* section flags */
   | SEC_LOAD | SEC_RELOC  
   | SEC_READONLY ),
  '_',				/* leading underscore */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  2,				/* minimum section alignment */
  /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32,   bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16,
  /* hdrs */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32,   bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16,

 {
	    
   _bfd_dummy_target,
   coff_object_p,
   bfd_generic_archive_p,
   _bfd_dummy_target
  },
 {
   bfd_false,
   coff_mkobject,
   _bfd_generic_mkarchive,
   bfd_false
  },
 {
   bfd_false,
   coff_write_object_contents,
   _bfd_write_archive_contents,
   bfd_false
  },

     BFD_JUMP_TABLE_GENERIC (coff),
     BFD_JUMP_TABLE_COPY (coff),
     BFD_JUMP_TABLE_CORE (_bfd_nocore),
     BFD_JUMP_TABLE_ARCHIVE (_bfd_archive_coff),
     BFD_JUMP_TABLE_SYMBOLS (coff),
     BFD_JUMP_TABLE_RELOCS (coff),
     BFD_JUMP_TABLE_WRITE (coff),
     BFD_JUMP_TABLE_LINK (coff),
     BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  COFF_SWAP_TABLE
 };
