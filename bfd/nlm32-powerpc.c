/* Support for 32-bit PowerPC NLM (NetWare Loadable Module)
   Copyright (C) 1994 Free Software Foundation, Inc.

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
#include "libbfd.h"

#define ARCH_SIZE 32

#include "nlm/ppc-ext.h"
#define Nlm_External_Fixed_Header	Nlm32_powerpc_External_Fixed_Header

#include "libnlm.h"

static boolean nlm_powerpc_backend_object_p
  PARAMS ((bfd *));
static boolean nlm_powerpc_write_prefix
  PARAMS ((bfd *));
static boolean nlm_powerpc_read_reloc
  PARAMS ((bfd *, nlmNAME(symbol_type) *, asection **, arelent *));
static boolean nlm_powerpc_mangle_relocs
  PARAMS ((bfd *, asection *, PTR, bfd_vma, bfd_size_type));
static boolean nlm_powerpc_read_import
  PARAMS ((bfd *, nlmNAME(symbol_type) *));
static boolean nlm_powerpc_write_import
  PARAMS ((bfd *, asection *, arelent *));
static boolean nlm_powerpc_write_external
  PARAMS ((bfd *, bfd_size_type, asymbol *, struct reloc_and_sec *));

/* PowerPC NLM's have a prefix header before the standard NLM.  This
   function reads it in, verifies the version, and seeks the bfd to
   the location before the regular NLM header.  */

static boolean
nlm_powerpc_backend_object_p (abfd)
     bfd *abfd;
{
  struct nlm32_powerpc_external_prefix_header s;

  if (bfd_read ((PTR) &s, sizeof s, 1, abfd) != sizeof s)
    return false;

  if (memcmp (s.signature, NLM32_POWERPC_SIGNATURE, sizeof s.signature) != 0
      || bfd_h_get_32 (abfd, s.headerVersion) != NLM32_POWERPC_HEADER_VERSION)
    return false;

  return true;
}

/* Write out the prefix.  */

static boolean
nlm_powerpc_write_prefix (abfd)
     bfd *abfd;
{
  struct nlm32_powerpc_external_prefix_header s;

  memset (&s, 0, sizeof s);
  memcpy (s.signature, NLM32_POWERPC_SIGNATURE, sizeof s.signature);
  bfd_h_put_32 (abfd, (bfd_vma) NLM32_POWERPC_HEADER_VERSION, s.headerVersion);
  bfd_h_put_32 (abfd, (bfd_vma) 0, s.origins);

  /* FIXME: What should we do about the date?  */

  if (bfd_write ((PTR) &s, sizeof s, 1, abfd) != sizeof s)
    return false;

  return true;
}

/* How to process the various reloc types.  PowerPC NLMs use XCOFF
   reloc types, and I have just copied the XCOFF reloc table here.  */

static reloc_howto_type nlm_powerpc_howto_table[] =
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

#define HOWTO_COUNT (sizeof nlm_powerpc_howto_table		\
		     / sizeof nlm_powerpc_howto_table[0])

/* Read a PowerPC NLM reloc.  */

static boolean
nlm_powerpc_read_reloc (abfd, sym, secp, rel)
     bfd *abfd;
     nlmNAME(symbol_type) *sym;
     asection **secp;
     arelent *rel;
{
  struct nlm32_powerpc_external_reloc ext;
  bfd_vma l_vaddr;
  unsigned long l_symndx;
  int l_rtype;
  int l_rsecnm;

  /* Read the reloc from the file.  */
  if (bfd_read (&ext, sizeof ext, 1, abfd) != sizeof ext)
    {
      bfd_set_error (bfd_error_system_call);
      return false;
    }

  /* Swap in the fields.  */
  l_vaddr = bfd_h_get_32 (abfd, ext.l_vaddr);
  l_symndx = bfd_h_get_32 (abfd, ext.l_symndx);
  l_rtype = bfd_h_get_16 (abfd, ext.l_rtype);
  l_rsecnm = bfd_h_get_16 (abfd, ext.l_rsecnm);

  /* Work out the arelent fields.  */
  if (sym != NULL)
    {
      /* This is an import.  sym_ptr_ptr is filled in by
	 nlm_canonicalize_reloc.  */
      rel->sym_ptr_ptr = NULL;
    }
  else
    {
      asection *sec;

      if (l_symndx == 0)
	sec = bfd_get_section_by_name (abfd, NLM_CODE_NAME);
      else if (l_symndx == 1)
	sec = bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME);
      else if (l_symndx == 2)
	sec = bfd_get_section_by_name (abfd, NLM_UNINITIALIZED_DATA_NAME);
      else
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      rel->sym_ptr_ptr = sec->symbol_ptr_ptr;
    }

  rel->address = l_vaddr;
  rel->addend = 0;

  BFD_ASSERT ((l_rtype & 0xff) < HOWTO_COUNT);

  rel->howto = nlm_powerpc_howto_table + (l_rtype & 0xff);

  BFD_ASSERT (rel->howto->name != NULL
	      && ((l_rtype & 0x8000) != 0
		  ? (rel->howto->complain_on_overflow
		     == complain_overflow_signed)
		  : (rel->howto->complain_on_overflow
		     == complain_overflow_bitfield))
	      && ((l_rtype >> 8) & 0x1f) == rel->howto->bitsize - 1);

  if (l_rsecnm == 0)
    *secp = bfd_get_section_by_name (abfd, NLM_CODE_NAME);
  else if (l_rsecnm == 1)
    *secp = bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME);
  else
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  return true;
}

/* Mangle PowerPC NLM relocs for output.  */

static boolean
nlm_powerpc_mangle_relocs (abfd, sec, data, offset, count)
     bfd *abfd;
     asection *sec;
     PTR data;
     bfd_vma offset;
     bfd_size_type count;
{
  return true;
}

/* Read a PowerPC NLM import record */

static boolean
nlm_powerpc_read_import (abfd, sym)
     bfd *abfd;
     nlmNAME(symbol_type) *sym;
{
  struct nlm_relent *nlm_relocs;	/* relocation records for symbol */
  bfd_size_type rcount;			/* number of relocs */
  bfd_byte temp[NLM_TARGET_LONG_SIZE];	/* temporary 32-bit value */
  unsigned char symlength;		/* length of symbol name */

  if (bfd_read ((PTR) &symlength, sizeof (symlength), 1, abfd)
      != sizeof (symlength))
    {
      bfd_set_error (bfd_error_system_call);
      return (false);
    }
  sym -> symbol.the_bfd = abfd;
  sym -> symbol.name = bfd_alloc (abfd, symlength + 1);
  if (bfd_read ((PTR) sym -> symbol.name, symlength, 1, abfd)
      != symlength)
    {
      bfd_set_error (bfd_error_system_call);
      return (false);
    }
  sym -> symbol.flags = 0;
  sym -> symbol.value = 0;
  sym -> symbol.section = &bfd_und_section;
  if (bfd_read ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
    {
      bfd_set_error (bfd_error_system_call);
      return (false);
    }
  rcount = bfd_h_get_32 (abfd, temp);
  nlm_relocs = ((struct nlm_relent *)
		bfd_alloc (abfd, rcount * sizeof (struct nlm_relent)));
  if (nlm_relocs == (struct nlm_relent *) NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  sym -> relocs = nlm_relocs;
  sym -> rcnt = 0;
  while (sym -> rcnt < rcount)
    {
      asection *section;
      
      if (nlm_powerpc_read_reloc (abfd, sym, &section,
				  &nlm_relocs -> reloc)
	  == false)
	return false;
      nlm_relocs -> section = section;
      nlm_relocs++;
      sym -> rcnt++;
    }
  return true;
}

/* Write a PowerPC NLM reloc.  */

static boolean
nlm_powerpc_write_import (abfd, sec, rel)
     bfd *abfd;
     asection *sec;
     arelent *rel;
{
  struct nlm32_powerpc_external_reloc ext;
  asymbol *sym;
  asection *symsec;
  unsigned long l_symndx;
  int l_rtype;
  int l_rsecnm;
  const reloc_howto_type *howto;

  bfd_h_put_32 (abfd, rel->address, ext.l_vaddr);

  sym = *rel->sym_ptr_ptr;
  symsec = bfd_get_section (sym);
  if (symsec == &bfd_und_section)
    l_symndx = 0;
  else if (symsec == bfd_get_section_by_name (abfd, NLM_CODE_NAME))
    l_symndx = 0;
  else if (symsec == bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME))
    l_symndx = 1;
  else if (symsec == bfd_get_section_by_name (abfd,
					      NLM_UNINITIALIZED_DATA_NAME))
    l_symndx = 2;
  else
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  bfd_h_put_32 (abfd, (bfd_vma) l_symndx, ext.l_symndx);

  for (howto = nlm_powerpc_howto_table;
       howto < nlm_powerpc_howto_table + HOWTO_COUNT;
       howto++)
    {
      if (howto->rightshift == rel->howto->rightshift
	  && howto->size == rel->howto->size
	  && howto->bitsize == rel->howto->bitsize
	  && howto->pc_relative == rel->howto->pc_relative
	  && howto->bitpos == rel->howto->bitpos
	  && howto->partial_inplace == rel->howto->partial_inplace
	  && howto->src_mask == rel->howto->src_mask
	  && howto->dst_mask == rel->howto->dst_mask
	  && howto->pcrel_offset == rel->howto->pcrel_offset)
	break;
    }
  if (howto >= nlm_powerpc_howto_table + HOWTO_COUNT)
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  l_rtype = howto->type;
  if (howto->complain_on_overflow == complain_overflow_signed)
    l_rtype |= 0x8000;
  l_rtype |= (howto->bitsize - 1) << 8;
  bfd_h_put_16 (abfd, (bfd_vma) l_rtype, ext.l_rtype);

  if (sec == bfd_get_section_by_name (abfd, NLM_CODE_NAME))
    l_rsecnm = 0;
  else if (sec == bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME))
    l_rsecnm = 1;
  else
    {
      bfd_set_error (bfd_error_bad_value);
      return false;
    }

  bfd_h_put_16 (abfd, (bfd_vma) l_rsecnm, ext.l_rsecnm);

  if (bfd_write (&ext, sizeof ext, 1, abfd) != sizeof ext)
    return false;

  return true;
}

/* Write a PowerPC NLM external symbol.  */

static boolean
nlm_powerpc_write_external (abfd, count, sym, relocs)
     bfd *abfd;
     bfd_size_type count;
     asymbol *sym;
     struct reloc_and_sec *relocs;
{
  int i;
  bfd_byte len;
  unsigned char temp[NLM_TARGET_LONG_SIZE];

  len = strlen (sym->name);
  if ((bfd_write (&len, sizeof (bfd_byte), 1, abfd) != sizeof(bfd_byte))
      || bfd_write (sym->name, len, 1, abfd) != len)
    {
      bfd_set_error (bfd_error_system_call);
      return false;
    }

  bfd_put_32 (abfd, count, temp);
  if (bfd_write (temp, sizeof(temp), 1, abfd) != sizeof (temp))
    {
      bfd_set_error (bfd_error_system_call);
      return false;
    }

  for (i = 0; i < count; i++)
    {
      if (nlm_powerpc_write_import (abfd, relocs[i].sec,
				    relocs[i].rel) == false)
	return false;
    }

  return true;
}

#include "nlmswap.h"

static const struct nlm_backend_data nlm32_powerpc_backend =
{
  "NetWare PowerPC Module \032",
  sizeof (Nlm32_powerpc_External_Fixed_Header),
  sizeof (struct nlm32_powerpc_external_prefix_header),
  bfd_arch_powerpc,
  0,
  false,
  nlm_powerpc_backend_object_p,
  nlm_powerpc_write_prefix,
  nlm_powerpc_read_reloc,
  nlm_powerpc_mangle_relocs,
  nlm_powerpc_read_import,
  nlm_powerpc_write_import,
  0,	/* set_public_section */
  0,	/* get_public_offset */
  nlm_swap_fixed_header_in,
  nlm_swap_fixed_header_out,
  nlm_powerpc_write_external,
  0,	/* write_export */
};

#define TARGET_BIG_NAME			"nlm32-powerpc"
#define TARGET_BIG_SYM			nlmNAME(powerpc_vec)
#define TARGET_BACKEND_DATA		&nlm32_powerpc_backend

#include "nlm-target.h"
