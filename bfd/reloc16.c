/* 8 and 16 bit COFF relocation functions, for BFD.
   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
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
Most of this hacked by  Steve Chamberlain,
			sac@cygnus.com 
*/

/* These routines are used by coff-h8300 and coff-z8k to do
   relocation.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "seclet.h"
#include "obstack.h"
#include "coff/internal.h"
#include "libcoff.h"

extern bfd_error_vector_type bfd_error_vector;

bfd_vma 
DEFUN(bfd_coff_reloc16_get_value,(reloc, seclet),
      arelent  *reloc AND
      bfd_seclet_type *seclet)
{
  bfd_vma value;
  asymbol *symbol = *(reloc->sym_ptr_ptr);
  /* A symbol holds a pointer to a section, and an offset from the
     base of the section.  To relocate, we find where the section will
     live in the output and add that in */

  if (symbol->section == &bfd_und_section)
  {
    /* Ouch, this is an undefined symbol.. */
    bfd_error_vector.undefined_symbol(reloc, seclet);
    value = symbol->value;
  }
  else 
  {
    value = symbol->value +
     symbol->section->output_offset +
      symbol->section->output_section->vma;
  }
  
  
  /* Add the value contained in the relocation */
  value += reloc->addend;
  
  return value;
}

static void
DEFUN(perform_slip,(s, slip, input_section, value),
      asymbol **s AND
      unsigned int slip AND
      asection *input_section AND
      bfd_vma value)
{
  
  /* Find all symbols past this point, and make them know
     what's happened */
  while (*s) 
  {
    asymbol *p = *s;
    if (p->section == input_section) 
    {
      /* This was pointing into this section, so mangle it */
      if (p->value > value)
      {
	p->value -= slip;
      }
    }
    s++;
	
  }    
}
static int 
DEFUN(movb1,(input_section, symbols, r, shrink),
      asection *input_section AND
      asymbol **symbols AND
      arelent *r AND
      unsigned int shrink) 
{
  bfd_vma value = bfd_coff_reloc16_get_value(r,0);
	
  if (value >= 0xff00)
  { 

    /* Change the reloc type from 16bit, possible 8 to 8bit
       possible 16 */
    r->howto = r->howto + 1;	  
    /* The place to relc moves back by one */
    r->address -=1;
	  
    /* This will be two bytes smaller in the long run */
    shrink +=2 ;
    perform_slip(symbols, 2, input_section, r->address - shrink +1);

	  
  }      
  return shrink;      
}

static int 
DEFUN(jmp1,(input_section, symbols, r, shrink),
      asection *input_section AND
      asymbol **symbols AND
      arelent *r AND
      unsigned int shrink) 
{

  
  bfd_vma value = bfd_coff_reloc16_get_value(r, 0);
	
  bfd_vma dot = input_section->output_section->vma +
   input_section->output_offset + r->address;	
  bfd_vma gap;
  
  /* See if the address we're looking at within 127 bytes of where
     we are, if so then we can use a small branch rather than the
     jump we were going to */

  gap = value - (dot - shrink);
  

  if (-120 < (long)gap && (long)gap < 120 )
  { 

    /* Change the reloc type from 16bit, possible 8 to 8bit
       possible 16 */
    r->howto = r->howto + 1;	  
    /* The place to relc moves back by one */
    r->address -=1;
	  
    /* This will be two bytes smaller in the long run */
    shrink +=2 ;
    perform_slip(symbols, 2, input_section, r->address-shrink +1);

	  
  }      
  return shrink;      
}

boolean 
DEFUN(bfd_coff_reloc16_relax_section,(abfd, i, symbols),
      bfd *abfd AND
      asection *i AND
      asymbol **symbols)
{
  
  /* Get enough memory to hold the stuff */
  bfd *input_bfd = i->owner;
  asection *input_section = i;
  int shrink = 0 ;
  boolean new = false;
  
  bfd_size_type reloc_size = bfd_get_reloc_upper_bound(input_bfd,
						       input_section);
  arelent **reloc_vector = (arelent **)bfd_xmalloc(reloc_size);

  /* Get the relocs and think about them */
  if (bfd_canonicalize_reloc(input_bfd, 
			     input_section,
			     reloc_vector,
			     symbols))
  {
    arelent **parent;
    for (parent = reloc_vector; *parent; parent++) 
    {
      arelent *r = *parent;
      switch (r->howto->type) {
	case R_MOVB2:
	case R_JMP2:
	  
	  shrink+=2;
	  break;
	  
	case R_MOVB1:
	  shrink = movb1(input_section, symbols, r, shrink);
	  new = true;
	  
	  break;
	case R_JMP1:
	  shrink = jmp1(input_section, symbols, r, shrink);
	  new = true;
	  
	  break;
	}
    }

  }
  input_section->_cooked_size -= shrink;  
  free((char *)reloc_vector);
  return new;
}

bfd_byte *
DEFUN(bfd_coff_reloc16_get_relocated_section_contents,(in_abfd, seclet, data),
      bfd *in_abfd AND
      bfd_seclet_type *seclet AND
      bfd_byte *data)

{
  /* Get enough memory to hold the stuff */
  bfd *input_bfd = seclet->u.indirect.section->owner;
  asection *input_section = seclet->u.indirect.section;
  bfd_size_type reloc_size = bfd_get_reloc_upper_bound(input_bfd,
						       input_section);
  arelent **reloc_vector = (arelent **)bfd_xmalloc(reloc_size);
  
  /* read in the section */
  bfd_get_section_contents(input_bfd,
			   input_section,
			   data,
			   0,
			   input_section->_raw_size);
  
  
  if (bfd_canonicalize_reloc(input_bfd, 
			     input_section,
			     reloc_vector,
			     seclet->u.indirect.symbols) )
  {
    arelent **parent = reloc_vector;
    arelent *reloc ;
    


    unsigned int dst_address = 0;
    unsigned int src_address = 0;
    unsigned int run;
    unsigned int idx;
    
    /* Find how long a run we can do */
    while (dst_address < seclet->size) 
    {
      
      reloc = *parent;
      if (reloc) 
      {
	/* Note that the relaxing didn't tie up the addresses in the
	   relocation, so we use the original address to work out the
	   run of non-relocated data */
	run = reloc->address - src_address;
	parent++;
	
      }
      else 
      {
	run = seclet->size - dst_address;
      }
      /* Copy the bytes */
      for (idx = 0; idx < run; idx++)
      {
	data[dst_address++] = data[src_address++];
      }
    
      /* Now do the relocation */
    
      if (reloc) 
      {
	switch (reloc->howto->type) 
	{
	case R_JMP2:
	  /* Speciial relaxed type */
	{
	  bfd_vma dot = seclet->offset + dst_address + seclet->u.indirect.section->output_section->vma;	  
	  int   gap = bfd_coff_reloc16_get_value(reloc,seclet)-dot-1;
	  if ((gap & ~0xff  ) != 0 &&((gap & 0xff00)!= 0xff00)) abort();

	  bfd_put_8(in_abfd,gap,   data+dst_address);

	  switch (data[dst_address-1]) 
	  {
	    
	  case 0x5e:
	    /* jsr -> bsr */
	    bfd_put_8(in_abfd, 0x55, data+dst_address-1);
	    break;
	  case 0x5a:	 
	    /* jmp ->bra */
	    bfd_put_8(in_abfd, 0x40, data+dst_address-1);
	    break;
	  
	  default:
	    abort();
	  
	  }
	
	

	     
	  dst_address++;
	  src_address+=3;
  
	  break;
	}

	     
	case R_MOVB2:
	  /* Special relaxed type, there will be a gap between where we
	     get stuff from and where we put stuff to now 
	     
	     for a mov.b @aa:16 -> mov.b @aa:8
	     opcode 0x6a 0x0y offset
	     ->     0x2y off
	     */
	  if (data[dst_address-1] != 0x6a)
	   abort();
	  switch (data[src_address] & 0xf0) 
	  {
	  case 0x00:
	    /* Src is memory */
	    data[dst_address-1] = (data[src_address] & 0xf) | 0x20;
	    break;
	  case 0x80:
	    /* Src is reg */
	    data[dst_address-1] = (data[src_address] & 0xf) | 0x30;
	    break;
	  default:
	    abort();
	  }
	
	  /* the offset must fit ! after all, what was all the relaxing
	     about ? */

	  bfd_put_8(in_abfd, bfd_coff_reloc16_get_value(reloc, seclet),
		    data + dst_address);

	  /* Note the magic - src goes up by two bytes, but dst by only
	     one */
	  dst_address+=1;
	  src_address+=3;
	
	  break;
	  /* PCrel 8 bits */
	case R_PCRBYTE:	  
	{
	  bfd_vma dot = seclet->offset + dst_address + seclet->u.indirect.section->output_section->vma;	  
	  int   gap = bfd_coff_reloc16_get_value(reloc,seclet)-dot;
	  if (gap > 127 || gap < -128) 
	  {
	    bfd_error_vector.reloc_value_truncated(reloc, seclet);
	  }
	  
	  bfd_put_8(in_abfd,gap,   data+dst_address);
	  dst_address++;
	  src_address++;
  
	  break;
	}

	case R_RELBYTE:
	{
	  unsigned  int gap =bfd_coff_reloc16_get_value(reloc,seclet);
	  if (gap > 0xff && gap < ~0xff)
	  {
	    bfd_error_vector.reloc_value_truncated(reloc, seclet);
	  }
	  
	  bfd_put_8(in_abfd, gap, data+dst_address);
	  dst_address+=1;
	  src_address+=1;


	}
	  break; 
	case R_JMP1:
	  /* A relword which would have like to have been a pcrel */
	case R_MOVB1:	
	  /* A relword which would like to have been modified but
	     didn't make it */
	case R_RELWORD:
	  bfd_put_16(in_abfd, bfd_coff_reloc16_get_value(reloc,seclet),
		     data+dst_address);
	  dst_address+=2;
	  src_address+=2;
	  break;
        default:
	  bfd_coff_reloc16_extra_cases (in_abfd, seclet, reloc, data,
					&src_address, &dst_address);
	  break;
	}
      }    
    }
  }
  free((char *)reloc_vector);
  return data;
  
}

