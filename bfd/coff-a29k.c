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

#include <ansidecl.h>
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"
#include "obstack.h"
#include "amdcoff.h"
#include "internalcoff.h"
#include "libcoff.h"

#define INSERT_HWORD(WORD,HWORD)	\
	(((WORD) & 0xff00ff00) | (((HWORD) & 0xff00) << 8) | ((HWORD)& 0xff))
#define EXTRACT_HWORD(WORD) 	(((WORD) & 0x00ff0000) >> 8) | ((WORD) & 0xff)

/* Provided the symbol, returns the value reffed */
static  long
get_symbol_value(symbol)       
asymbol *symbol;
{                                             
  long relocation = 0;

  if (symbol != (asymbol *)NULL) {              
    if (symbol->flags & BSF_FORT_COMM) {        
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

static bfd_reloc_status_enum_type
a29k_reloc(abfd, reloc_entry, symbol_in, data, input_section)
bfd *abfd;
arelent *reloc_entry;
asymbol *symbol_in;
unsigned char *data;
asection *input_section;
{
	static unsigned long part1_consth_active=0;
	static unsigned long part1_consth_value;
	unsigned long insn, value, sym_value; 
	unsigned short r_type;
/*  	bfd_reloc_status_enum_type result;*/
/*  	coff_symbol_type *cs = coffsymbol(symbol_in);*/
	
	r_type = reloc_entry->howto->type;

        /* FIXME: Do we need to check for partial linking here */
        if (symbol_in && (symbol_in->flags & BSF_UNDEFINED)) {
		/* Keep the state machine happy in case we're called again */
		if (r_type == R_IHIHALF) {
			part1_consth_active = 1;
			part1_consth_value  = 0;
		}
                return(bfd_reloc_undefined);
	}

	if ((part1_consth_active) && (r_type != R_IHCONST)) {
		fprintf(stderr,"Relocation problem : ");
		fprintf(stderr,"Missing IHCONST in module %s\n",abfd->filename);
		part1_consth_active = 0;
		return(bfd_reloc_dangerous);
	}

	insn = bfd_get_32(abfd, data + reloc_entry->address); 
	sym_value = get_symbol_value(symbol_in);

	switch (r_type) {
	case R_IREL: 	
		value = EXTRACT_HWORD(insn) << 2;
		value +=  sym_value + reloc_entry->addend;
		if (value <= 0x3ffff) {		/* Absolute jmp/call */
			insn |= 0x01000000;	/* Make it absolute */
			/* FIXME: Should we change r_type to R_IABS */
		} else {			/* Relative jmp/call */
			value -= reloc_entry->address;
			if (value > 0x3ffff) {
				fprintf(stderr,"Relocation problem : ");
				fprintf(stderr,"Jmp/call too far; to %s from %s\n",
					symbol_in->name,abfd->filename); 
				return(bfd_reloc_outofrange);
			}
		}
		value >>= 2;
		insn = INSERT_HWORD(insn,value);
		break;
	case R_ILOHALF: 
		value = EXTRACT_HWORD(insn);
		value +=  sym_value + reloc_entry->addend;
		insn = INSERT_HWORD(insn,value);
		break;
	case R_IHIHALF: 	/* consth, part 1 */
		/* Just get the symbol value that is referenced */
		part1_consth_active = 1;
		part1_consth_value = sym_value + reloc_entry->addend;
		return(bfd_reloc_ok);	/* Don't modify insn until R_IHCONST */
		break;
	case R_IHCONST: 	/* consth, part 2 */
		/* Now relocate the reference */
		if (!part1_consth_active) {
			fprintf(stderr,"Relocation problem : ");
			fprintf(stderr,"IHIHALF missing in module %s\n",
				abfd->filename); 
			part1_consth_active = 0;
			return(bfd_reloc_dangerous);
		}
		/* sym_ptr_ptr = r_symndx, in coff_slurp_reloc_table() */
		value = (unsigned int)reloc_entry->addend; /* r_symndx */
		value += part1_consth_value;
		value >>= 16;
		insn = INSERT_HWORD(insn,value);
		part1_consth_active = 0;
		break;
	case R_BYTE:
		value = (insn >> 24) + sym_value + reloc_entry->addend;	
		if (value & 0xffffff00) {
			fprintf(stderr,"Relocation problem : ");
			fprintf(stderr,"byte value too large in module %s\n",
				abfd->filename); 
			return(bfd_reloc_overflow);
		}
		insn = (insn & 0x00ffffff) | (value << 24);
		break;
	case R_HWORD:
		value = (insn >> 16) + sym_value + reloc_entry->addend;	
		if (value & 0xffff0000) {
			fprintf(stderr,"Relocation problem : ");
			fprintf(stderr,"hword value too large in module %s\n",
				abfd->filename); 
			return(bfd_reloc_overflow);
		}
		insn = (insn & 0x0000ffff) | (value<<16); 
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

#include "coffcode.h"

bfd_target a29kcoff_big_vec =
{
  "coff-a29k-big",		/* name */
  bfd_target_coff_flavour_enum,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC /* section flags */
	| SEC_LOAD | SEC_RELOC  
	| SEC_READONLY ),
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */

     3,				/* minimum section alignment */
_do_getb64, _do_putb64,  _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
_do_getb64, _do_putb64,  _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */


  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

     JUMP_TABLE(coff),
     COFF_SWAP_TABLE
  };

