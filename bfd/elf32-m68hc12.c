/* Motorola 68HC12-specific support for 32-bit ELF
   Copyright 1999, 2000, 2002 Free Software Foundation, Inc.
   Contributed by Stephane Carrez (stcarrez@nerim.fr)
   (Heavily copied from the D10V port by Martin Hunt (hunt@cygnus.com))

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/m68hc11.h"
#include "opcode/m68hc11.h"

static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));
static void m68hc11_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));

static bfd_reloc_status_type m68hc11_elf_ignore_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static bfd_reloc_status_type m68hc12_elf_special_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static int m68hc12_addr_is_banked PARAMS ((bfd_vma));
static bfd_vma m68hc12_phys_addr PARAMS ((bfd_vma));
static bfd_vma m68hc12_phys_page PARAMS ((bfd_vma));

/* GC mark and sweep.  */
static asection *elf32_m68hc11_gc_mark_hook
  PARAMS ((asection *, struct bfd_link_info *, Elf_Internal_Rela *,
	   struct elf_link_hash_entry *, Elf_Internal_Sym *));
static bfd_boolean elf32_m68hc11_gc_sweep_hook
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));

bfd_boolean _bfd_m68hc12_elf_merge_private_bfd_data PARAMS ((bfd *, bfd *));
bfd_boolean _bfd_m68hc12_elf_set_private_flags PARAMS ((bfd *, flagword));
bfd_boolean _bfd_m68hc12_elf_print_private_bfd_data PARAMS ((bfd *, PTR));



/* Use REL instead of RELA to save space */
#define USE_REL	1

/* The Motorola 68HC11 microcontroler only addresses 64Kb.
   We must handle 8 and 16-bit relocations.  The 32-bit relocation
   is defined but not used except by gas when -gstabs is used (which
   is wrong).

   The 68HC12 microcontroler has a memory bank switching system
   with a 16Kb window in the 64Kb address space.  The extended memory
   is mapped in the 16Kb window (at 0x8000).  The page register controls
   which 16Kb bank is mapped.  The call/rtc instructions take care of
   bank switching in function calls/returns.

   For GNU Binutils to work, we consider there is a physical memory
   at 0..0x0ffff and a kind of virtual memory above that.  Symbols
   in virtual memory have their addresses treated in a special way
   when disassembling and when linking.

   For the linker to work properly, we must always relocate the virtual
   memory as if it is mapped at 0x8000.  When a 16-bit relocation is
   made in the virtual memory, we check that it does not cross the
   memory bank where it is used.  This would involve a page change
   which would be wrong.  The 24-bit relocation is for that and it
   treats the address as a physical address + page number.


					Banked
					Address Space
                                        |               |       Page n
					+---------------+ 0x1010000
                                        |               |
                                        | jsr _foo      |
                                        | ..            |       Page 3
                                        | _foo:         |
					+---------------+ 0x100C000
					|	        |
                                        | call _bar     |
					| ..	        |	Page 2
					| _bar:	        |
					+---------------+ 0x1008000
				/------>|	        |
				|	| call _foo     |	Page 1
				|	|       	|
				|	+---------------+ 0x1004000
      Physical			|	|	        |
      Address Space		|	|	        |	Page 0
				|	|	        |
    +-----------+ 0x00FFFF	|	+---------------+ 0x1000000
    |		|		|
    | call _foo	|		|
    |		|		|
    +-----------+ 0x00BFFF -+---/
    |		|           |
    |		|	    |
    |		| 16K	    |
    |		|	    |
    +-----------+ 0x008000 -+
    |		|
    |		|
    =		=
    |		|
    |		|
    +-----------+ 0000


   The 'call _foo' must be relocated with page 3 and 16-bit address
   mapped at 0x8000.

   The 3-bit and 16-bit PC rel relocation is only used by 68HC12.  */
static reloc_howto_type elf_m68hc11_howto_table[] = {
  /* This reloc does nothing.  */
  HOWTO (R_M68HC11_NONE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_NONE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 8 bit absolute relocation */
  HOWTO (R_M68HC11_8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_8",		/* name */
	 FALSE,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 8 bit absolute relocation (upper address) */
  HOWTO (R_M68HC11_HI8,		/* type */
	 8,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_HI8",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 8 bit absolute relocation (upper address) */
  HOWTO (R_M68HC11_LO8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_LO8",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 8 bit PC-rel relocation */
  HOWTO (R_M68HC11_PCREL_8,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_PCREL_8",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 16 bit absolute relocation */
  HOWTO (R_M68HC11_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont /*bitfield */ ,	/* complain_on_overflow */
	 m68hc12_elf_special_reloc,	/* special_function */
	 "R_M68HC12_16",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  This one is never used for the
     code relocation.  It's used by gas for -gstabs generation.  */
  HOWTO (R_M68HC11_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_32",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 3 bit absolute relocation */
  HOWTO (R_M68HC11_3B,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 3,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_4B",	/* name */
	 FALSE,			/* partial_inplace */
	 0x003,			/* src_mask */
	 0x003,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 16 bit PC-rel relocation */
  HOWTO (R_M68HC11_PCREL_16,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_M68HC12_PCREL_16",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy */
  HOWTO (R_M68HC11_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_M68HC11_GNU_VTINHERIT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage */
  HOWTO (R_M68HC11_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn,	/* special_function */
	 "R_M68HC11_GNU_VTENTRY",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 24 bit relocation */
  HOWTO (R_M68HC11_24,	        /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 m68hc12_elf_special_reloc,	/* special_function */
	 "R_M68HC12_24",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 16-bit low relocation */
  HOWTO (R_M68HC11_LO16,        /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 m68hc12_elf_special_reloc,/* special_function */
	 "R_M68HC12_LO16",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A page relocation */
  HOWTO (R_M68HC11_PAGE,        /* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 m68hc12_elf_special_reloc,/* special_function */
	 "R_M68HC12_PAGE",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ff,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (14),
  EMPTY_HOWTO (15),
  EMPTY_HOWTO (16),
  EMPTY_HOWTO (17),
  EMPTY_HOWTO (18),
  EMPTY_HOWTO (19),

  /* Mark beginning of a jump instruction (any form).  */
  HOWTO (R_M68HC11_RL_JUMP,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 m68hc11_elf_ignore_reloc,	/* special_function */
	 "R_M68HC12_RL_JUMP",	/* name */
	 TRUE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE),                 /* pcrel_offset */

  /* Mark beginning of Gcc relaxation group instruction.  */
  HOWTO (R_M68HC11_RL_GROUP,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 m68hc11_elf_ignore_reloc,	/* special_function */
	 "R_M68HC12_RL_GROUP",	/* name */
	 TRUE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 TRUE),                 /* pcrel_offset */
};

/* Map BFD reloc types to M68HC11 ELF reloc types.  */

struct m68hc11_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct m68hc11_reloc_map m68hc11_reloc_map[] = {
  {BFD_RELOC_NONE, R_M68HC11_NONE,},
  {BFD_RELOC_8, R_M68HC11_8},
  {BFD_RELOC_M68HC11_HI8, R_M68HC11_HI8},
  {BFD_RELOC_M68HC11_LO8, R_M68HC11_LO8},
  {BFD_RELOC_8_PCREL, R_M68HC11_PCREL_8},
  {BFD_RELOC_16_PCREL, R_M68HC11_PCREL_16},
  {BFD_RELOC_16, R_M68HC11_16},
  {BFD_RELOC_32, R_M68HC11_32},
  {BFD_RELOC_M68HC11_3B, R_M68HC11_3B},

  {BFD_RELOC_VTABLE_INHERIT, R_M68HC11_GNU_VTINHERIT},
  {BFD_RELOC_VTABLE_ENTRY, R_M68HC11_GNU_VTENTRY},

  {BFD_RELOC_M68HC11_LO16, R_M68HC11_LO16},
  {BFD_RELOC_M68HC11_PAGE, R_M68HC11_PAGE},
  {BFD_RELOC_M68HC11_24, R_M68HC11_24},

  {BFD_RELOC_M68HC11_RL_JUMP, R_M68HC11_RL_JUMP},
  {BFD_RELOC_M68HC11_RL_GROUP, R_M68HC11_RL_GROUP},
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (m68hc11_reloc_map) / sizeof (struct m68hc11_reloc_map);
       i++)
    {
      if (m68hc11_reloc_map[i].bfd_reloc_val == code)
	return &elf_m68hc11_howto_table[m68hc11_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* This function is used for relocs which are only used for relaxing,
   which the linker should otherwise ignore.  */

static bfd_reloc_status_type
m68hc11_elf_ignore_reloc (abfd, reloc_entry, symbol, data, input_section,
                          output_bfd, error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry;
     asymbol *symbol ATTRIBUTE_UNUSED;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  if (output_bfd != NULL)
    reloc_entry->address += input_section->output_offset;
  return bfd_reloc_ok;
}

static int
m68hc12_addr_is_banked (addr)
     bfd_vma addr;
{
   return (addr >= M68HC12_BANK_VIRT) ? 1 : 0;
}

/* Return the physical address seen by the processor, taking
   into account banked memory.  */
static bfd_vma
m68hc12_phys_addr (addr)
     bfd_vma addr;
{
  if (addr < M68HC12_BANK_VIRT)
    return addr;

  /* Map the address to the memory bank.  */
  addr -= M68HC12_BANK_VIRT;
  addr &= M68HC12_BANK_MASK;
  addr += M68HC12_BANK_BASE;
  return addr;
}

/* Return the page number corresponding to an address in banked memory.  */
static bfd_vma
m68hc12_phys_page (addr)
     bfd_vma addr;
{
  if (addr < M68HC12_BANK_VIRT)
    return 0;

  /* Map the address to the memory bank.  */
  addr -= M68HC12_BANK_VIRT;
  addr >>= M68HC12_BANK_SHIFT;
  addr &= M68HC12_BANK_PAGE_MASK;
  return addr;
}

static bfd_reloc_status_type
m68hc12_elf_special_reloc (abfd, reloc_entry, symbol, data, input_section,
                           output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  reloc_howto_type *howto;
  bfd_vma relocation;
  bfd_vma phys_addr;
  bfd_vma phys_page;
  bfd_vma insn_page;
  bfd_vma insn_addr;

  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    return bfd_reloc_continue;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  /* Compute relocation.  */
  relocation = (symbol->value
		+ symbol->section->output_section->vma
		+ symbol->section->output_offset);
  relocation += reloc_entry->addend;
  relocation += bfd_get_16 (abfd, (bfd_byte*) data + reloc_entry->address);

  /* Do the memory bank mapping.  */
  phys_addr = m68hc12_phys_addr (relocation);
  phys_page = m68hc12_phys_page (relocation);

  howto = reloc_entry->howto;
  if (howto->complain_on_overflow != complain_overflow_dont
      && (phys_addr & (((bfd_vma) -1) << 16)))
     return bfd_reloc_overflow;

  switch (howto->type)
    {
    case R_M68HC11_16:
          /* Get virtual address of instruction having the relocation.  */
       insn_addr = input_section->output_section->vma
          + input_section->output_offset
          + reloc_entry->address;

      insn_page = m68hc12_phys_page (insn_addr);

      if (m68hc12_addr_is_banked (relocation)
          && m68hc12_addr_is_banked (insn_addr)
          && phys_page != insn_page)
         {
            *error_message = _("address is not in the same bank");
            return bfd_reloc_dangerous;
         }
      if (m68hc12_addr_is_banked (relocation)
          && !m68hc12_addr_is_banked (insn_addr))
         {
            *error_message = _("reference to a banked address in "
                               "the normal address space");
            return bfd_reloc_dangerous;
         }

    case R_M68HC11_LO16:
      bfd_put_16 (abfd, phys_addr, (bfd_byte*) data + reloc_entry->address);
      break;

    case R_M68HC11_24:
      bfd_put_16 (abfd, phys_addr, (bfd_byte*) data + reloc_entry->address);
      bfd_put_8 (abfd, phys_page, (bfd_byte*) data + reloc_entry->address + 2);
      break;

    case R_M68HC11_PAGE:
      bfd_put_8 (abfd, phys_page, (bfd_byte*) data + reloc_entry->address);
      break;

    default:
       abort ();
       break;
    }

  return bfd_reloc_ok;
}

/* Set the howto pointer for an M68HC11 ELF reloc.  */

static void
m68hc11_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_M68HC11_max);
  cache_ptr->howto = &elf_m68hc11_howto_table[r_type];
}

static asection *
elf32_m68hc11_gc_mark_hook (sec, info, rel, h, sym)
     asection *sec;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rel;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  if (h != NULL)
    {
      switch (ELF32_R_TYPE (rel->r_info))
	{
	default:
	  switch (h->root.type)
	    {
	    case bfd_link_hash_defined:
	    case bfd_link_hash_defweak:
	      return h->root.u.def.section;

	    case bfd_link_hash_common:
	      return h->root.u.c.p->section;

	    default:
	      break;
	    }
	}
    }
  else
    return bfd_section_from_elf_index (sec->owner, sym->st_shndx);

  return NULL;
}

static bfd_boolean
elf32_m68hc11_gc_sweep_hook (abfd, info, sec, relocs)
     bfd *abfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     asection *sec ATTRIBUTE_UNUSED;
     const Elf_Internal_Rela *relocs ATTRIBUTE_UNUSED;
{
  /* We don't use got and plt entries for 68hc11/68hc12.  */
  return TRUE;
}


static bfd_boolean
m68hc12_elf_set_mach_from_flags (abfd)
     bfd *abfd;
{
  flagword flags = elf_elfheader (abfd)->e_flags;

  switch (flags & EF_M68HC11_MACH_MASK)
    {
    case EF_M68HC12_MACH:
      bfd_default_set_arch_mach (abfd, bfd_arch_m68hc12, bfd_mach_m6812);
      break;
    case EF_M68HCS12_MACH:
      bfd_default_set_arch_mach (abfd, bfd_arch_m68hc12, bfd_mach_m6812s);
      break;
    case EF_M68HC11_GENERIC:
      bfd_default_set_arch_mach (abfd, bfd_arch_m68hc12,
                                 bfd_mach_m6812_default);
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

/* Set and control ELF flags in ELF header.  */

bfd_boolean
_bfd_m68hc12_elf_set_private_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  BFD_ASSERT (!elf_flags_init (abfd)
	      || elf_elfheader (abfd)->e_flags == flags);

  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = TRUE;
  return m68hc12_elf_set_mach_from_flags (abfd);
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */

bfd_boolean
_bfd_m68hc12_elf_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword old_flags;
  flagword new_flags;
  bfd_boolean ok = TRUE;

  /* Check if we have the same endianess */
  if (!_bfd_generic_verify_endian_match (ibfd, obfd))
    return FALSE;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return TRUE;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;

  if (! elf_flags_init (obfd))
    {
      elf_flags_init (obfd) = TRUE;
      elf_elfheader (obfd)->e_flags = new_flags;
      elf_elfheader (obfd)->e_ident[EI_CLASS]
	= elf_elfheader (ibfd)->e_ident[EI_CLASS];

      if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
	  && bfd_get_arch_info (obfd)->the_default)
	{
	  if (! bfd_set_arch_mach (obfd, bfd_get_arch (ibfd),
				   bfd_get_mach (ibfd)))
	    return FALSE;
	}

      return TRUE;
    }

  /* Check ABI compatibility.  */
  if ((new_flags & E_M68HC11_I32) != (old_flags & E_M68HC11_I32))
    {
      (*_bfd_error_handler)
	(_("%s: linking files compiled for 16-bit integers (-mshort) "
           "and others for 32-bit integers"),
	 bfd_archive_filename (ibfd));
      ok = FALSE;
    }
  if ((new_flags & E_M68HC11_F64) != (old_flags & E_M68HC11_F64))
    {
      (*_bfd_error_handler)
	(_("%s: linking files compiled for 32-bit double (-fshort-double) "
           "and others for 64-bit double"),
	 bfd_archive_filename (ibfd));
      ok = FALSE;
    }

  /* Processor compatibility.  */
  if (!EF_M68HC11_CAN_MERGE_MACH (new_flags, old_flags))
    {
      (*_bfd_error_handler)
	(_("%s: linking files compiled for HCS12 with "
           "others compiled for HC12"),
	 bfd_archive_filename (ibfd));
      ok = FALSE;
    }
  new_flags = ((new_flags & ~EF_M68HC11_MACH_MASK)
               | (EF_M68HC11_MERGE_MACH (new_flags, old_flags)));

  elf_elfheader (obfd)->e_flags = new_flags;

  /* Warn about any other mismatches */
  new_flags &= ~(EF_M68HC11_ABI | EF_M68HC11_MACH_MASK);
  old_flags &= ~(EF_M68HC11_ABI | EF_M68HC11_MACH_MASK);
  if (new_flags != old_flags)
    {
      (*_bfd_error_handler)
	(_("%s: uses different e_flags (0x%lx) fields than previous modules (0x%lx)"),
	 bfd_archive_filename (ibfd), (unsigned long) new_flags,
	 (unsigned long) old_flags);
      ok = FALSE;
    }

  if (! ok)
    {
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  return TRUE;
}

bfd_boolean
_bfd_m68hc12_elf_print_private_bfd_data (abfd, ptr)
     bfd *abfd;
     PTR ptr;
{
  FILE *file = (FILE *) ptr;

  BFD_ASSERT (abfd != NULL && ptr != NULL);

  /* Print normal ELF private data.  */
  _bfd_elf_print_private_bfd_data (abfd, ptr);

  /* xgettext:c-format */
  fprintf (file, _("private flags = %lx:"), elf_elfheader (abfd)->e_flags);

  if (elf_elfheader (abfd)->e_flags & E_M68HC11_I32)
    fprintf (file, _("[abi=32-bit int,"));
  else
    fprintf (file, _("[abi=16-bit int,"));

  if (elf_elfheader (abfd)->e_flags & E_M68HC11_F64)
    fprintf (file, _(" 64-bit double,"));
  else
    fprintf (file, _(" 32-bit double,"));

  if (elf_elfheader (abfd)->e_flags & EF_M68HCS12_MACH)
    fprintf (file, _(" cpu=HCS12]"));
  else
    fprintf (file, _(" cpu=HC12]"));    
  fputc ('\n', file);

  return TRUE;
}

#define ELF_ARCH		bfd_arch_m68hc12
#define ELF_MACHINE_CODE	EM_68HC12
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bfd_elf32_m68hc12_vec
#define TARGET_BIG_NAME		"elf32-m68hc12"

#define elf_info_to_howto	0
#define elf_info_to_howto_rel	m68hc11_info_to_howto_rel
#define elf_backend_gc_mark_hook     elf32_m68hc11_gc_mark_hook
#define elf_backend_gc_sweep_hook    elf32_m68hc11_gc_sweep_hook
#define elf_backend_object_p		m68hc12_elf_set_mach_from_flags
#define elf_backend_final_write_processing	0
/* Disabled as this backend uses the generic linker.  */
#define elf_backend_can_gc_sections		0

#define bfd_elf32_bfd_merge_private_bfd_data \
					_bfd_m68hc12_elf_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags	_bfd_m68hc12_elf_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data \
					_bfd_m68hc12_elf_print_private_bfd_data

#include "elf32-target.h"
