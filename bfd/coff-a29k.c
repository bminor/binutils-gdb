/* AMD 29000 COFF back-end for BFD.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
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

/* $Id$ */

#define A29K 1

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "coff/a29k.h"
#include "coff/internal.h"
#include "libcoff.h"

#define INSERT_HWORD(WORD,HWORD)	\
    (((WORD) & 0xff00ff00) | (((HWORD) & 0xff00) << 8) | ((HWORD)& 0xff))
#define EXTRACT_HWORD(WORD) \
    (((WORD) & 0x00ff0000) >> 8) | ((WORD)& 0xff)
#define SIGN_EXTEND_HWORD(HWORD) \
    ((HWORD) & 0x8000 ? (HWORD)|0xffff0000 : (HWORD))

/* Provided the symbol, returns the value reffed */
static  long
get_symbol_value(symbol)       
asymbol *symbol;
{                                             
  long relocation = 0;

  if (symbol != (asymbol *)NULL) {              
      if (symbol->section == &bfd_com_section) 
      {
	
      relocation = 0;                           
    } else {                                      
      relocation = symbol->value;               
    }                                           
    if (symbol->section != (asection *)NULL) {    
      relocation += symbol->section->output_section->vma +
        	symbol->section->output_offset;           
    }                                             
  }
  return(relocation);
}

/* this function is in charge of performing all the 29k relocations */

static bfd_reloc_status_type
DEFUN(a29k_reloc,(abfd, reloc_entry, symbol_in, data, input_section),
      bfd *abfd AND
      arelent *reloc_entry AND
      asymbol *symbol_in AND
      unsigned char *data AND
      asection *input_section)
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

	
    r_type = reloc_entry->howto->type;

    /* FIXME: Do we need to check for partial linking here */
    if (symbol_in && (symbol_in->section == &bfd_und_section))
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
	fprintf(stderr,"Relocation problem : ");
	fprintf(stderr,"Missing IHCONST in module %s\n",abfd->filename);
	part1_consth_active = false;
	return(bfd_reloc_dangerous);
    }

    insn = bfd_get_32(abfd, data + reloc_entry->address); 
    sym_value = get_symbol_value(symbol_in);

    switch (r_type) 
    {
      case R_IREL: 	
	/* Take the value in the field and sign extend it */
	signed_value = EXTRACT_HWORD(insn) << 2;
	signed_value = SIGN_EXTEND_HWORD(signed_value);
	signed_value +=  sym_value + reloc_entry->addend;
	if ((signed_value&~0x3ffff) == 0) 
	{			/* Absolute jmp/call */
	    insn |= (1<<24);	/* Make it absolute */
	    /* FIXME: Should we change r_type to R_IABS */
	    signed_value /= 2;
	} 
	else 
	{
	    /* Relative jmp/call, so subtract from the value the
	       address of the place we're coming from */
	    signed_value -= reloc_entry->address + 
	     input_section->output_section->vma + 
	      input_section->output_offset;
	    if (signed_value>0x1ffff || signed_value<-0x20000) 
	     return(bfd_reloc_outofrange);

	    signed_value /= 2;
	}
	insn = INSERT_HWORD(insn, signed_value);
	break;
      case R_ILOHALF: 
	unsigned_value = EXTRACT_HWORD(insn);
	unsigned_value +=  sym_value + reloc_entry->addend;
	insn = INSERT_HWORD(insn, unsigned_value);
	break;
      case R_IHIHALF:
	/* consth, part 1 
	  Just get the symbol value that is referenced */
	part1_consth_active = true;
	part1_consth_value = sym_value + reloc_entry->addend;
	/* Don't modify insn until R_IHCONST */
	return(bfd_reloc_ok);
	break;
      case R_IHCONST:	
	/* consth, part 2 
	   Now relocate the reference */
	if (part1_consth_active == false) {
	    fprintf(stderr,"Relocation problem : ");
	    fprintf(stderr,"IHIHALF missing in module %s\n",
		    abfd->filename); 
	    return(bfd_reloc_dangerous);
	}
	/* sym_ptr_ptr = r_symndx, in coff_slurp_reloc_table() */
	unsigned_value = 0;	/*EXTRACT_HWORD(insn) << 16;*/
	unsigned_value += reloc_entry->addend; /* r_symndx */
	unsigned_value += part1_consth_value;
	unsigned_value = unsigned_value >> 16;
	insn = INSERT_HWORD(insn, unsigned_value);
	part1_consth_active = false;
	break;
      case R_BYTE:
	unsigned_value = (insn >> 24) + sym_value + reloc_entry->addend;	
	if (unsigned_value & 0xffffff00) {
	    fprintf(stderr,"Relocation problem : ");
	    fprintf(stderr,"byte value too large in module %s\n",
		    abfd->filename); 
	    return(bfd_reloc_overflow);
	}
	insn = (insn & 0x00ffffff) | (unsigned_value << 24);
	break;
      case R_HWORD:
	unsigned_value = (insn >> 16) + sym_value + reloc_entry->addend;	
	if (unsigned_value & 0xffff0000) {
	    fprintf(stderr,"Relocation problem : ");
	    fprintf(stderr,"hword value too large in module %s\n",
		    abfd->filename); 
	    return(bfd_reloc_overflow);
	}
	insn = (insn & 0x0000ffff) | (unsigned_value<<16); 
	break;
      case R_WORD:
	insn += sym_value + reloc_entry->addend;  
	break;
      default:
	fprintf(stderr,"Relocation problem : ");
	fprintf(stderr,"Unrecognized reloc type %d, in module %s\n",
		r_type,abfd->filename); 
	return (bfd_reloc_dangerous);
    }

    bfd_put_32(abfd, insn, data+reloc_entry->address);
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
#define NA	0	/* Obsolete fields, via the documentation */
static reloc_howto_type howto_table[] = 
{
  {R_ABS,     0, 3, NA, false, NA, NA, true,a29k_reloc,"ABS",     true, 0xffffffff,0xffffffff, false},
  {1},  {2},  {3},   {4},  {5},  {6},  {7},  {8},  {9}, {10},
  {11}, {12}, {13}, {14}, {15}, {16}, {17}, {18}, {19}, {20},
  {21}, {22}, {23},
  {R_IREL,    0, 3, NA, true,  NA, NA, true,a29k_reloc,"IREL",    true, 0xffffffff,0xffffffff, false},
  {R_IABS,    0, 3, NA, false, NA, NA, true,a29k_reloc,"IABS",    true, 0xffffffff,0xffffffff, false},
  {R_ILOHALF, 0, 3, NA, true,  NA, NA, true,a29k_reloc,"ILOHALF", true, 0x0000ffff,0x0000ffff, false},
  {R_IHIHALF, 0, 3, NA, true,  NA, NA, true,a29k_reloc,"IHIHALF", true, 0xffff0000,0xffff0000, false},
  {R_IHCONST, 0, 3, NA, true,  NA, NA, true,a29k_reloc,"IHCONST", true, 0xffff0000,0xffff0000, false},
  {R_BYTE,    0, 0, NA, false, NA, NA, true,a29k_reloc,"BYTE",    true, 0x000000ff,0x000000ff, false},
  {R_HWORD,   0, 1, NA, false, NA, NA, true,a29k_reloc,"HWORD",   true, 0x0000ffff,0x0000ffff, false},
  {R_WORD,    0, 2, NA, false, NA, NA, true,a29k_reloc,"WORD",    true, 0xffffffff,0xffffffff, false},
};
#undef NA

#define BADMAG(x) A29KBADMAG(x)

#define RELOC_PROCESSING(relent, reloc, symbols, abfd, section) \
 reloc_processing(relent, reloc, symbols, abfd, section)

static void DEFUN(reloc_processing,(relent,reloc, symbols, abfd, section) ,
	   arelent *relent AND
	   struct internal_reloc *reloc AND
	   asymbol **symbols AND
	   bfd *abfd AND
	   asection *section)
{
    relent->address = reloc->r_vaddr;		
    relent->howto = howto_table + reloc->r_type;
    if (reloc->r_type == R_IHCONST) 
    {		
	relent->addend = reloc->r_symndx;		
	relent->sym_ptr_ptr= 0;
    }
    else 
    {
      asymbol *ptr;
      relent->sym_ptr_ptr = symbols + obj_convert(abfd)[reloc->r_symndx];

      ptr = *(relent->sym_ptr_ptr);

      if (ptr 
	  && ptr->the_bfd == abfd		
	  && ptr->section != (asection *) NULL	
	  && ((ptr->flags & BSF_OLD_COMMON)== 0))	
      {						
	  relent->addend = -(ptr->section->vma + ptr->value);	
      }						
      else
      {					
	  relent->addend = 0;			
      }			
      relent->address-= section->vma;
/*      relent->section = 0;*/
  }
}

#include "coffcode.h"

bfd_target a29kcoff_big_vec =
{
    "coff-a29k-big",		/* name */
    bfd_target_coff_flavour,
    true,			/* data byte order is big */
    true,			/* header byte order is big */

    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

    (SEC_HAS_CONTENTS | SEC_ALLOC /* section flags */
     | SEC_LOAD | SEC_RELOC  
     | SEC_READONLY ),
    '/',			/* ar_pad_char */
    15,				/* ar_max_namelen */
    2,				/* minimum section alignment */
    /* data */
    _do_getb64, _do_putb64, _do_getb32,
    _do_putb32, _do_getb16, _do_putb16,
    /* hdrs */
    _do_getb64, _do_putb64, _do_getb32,
    _do_putb32, _do_getb16, _do_putb16,

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

    JUMP_TABLE(coff),
    COFF_SWAP_TABLE
 };

