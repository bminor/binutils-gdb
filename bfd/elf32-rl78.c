/* Renesas RL78 specific support for 32-bit ELF.
   Copyright (C) 2011
   Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bfd_stdint.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/rl78.h"
#include "libiberty.h"

#define valid_16bit_address(v) ((v) <= 0x0ffff || (v) >= 0xf0000)

#define RL78REL(n,sz,bit,shift,complain,pcrel)				     \
  HOWTO (R_RL78_##n, shift, sz, bit, pcrel, 0, complain_overflow_ ## complain, \
	 bfd_elf_generic_reloc, "R_RL78_" #n, FALSE, 0, ~0, FALSE)

/* Note that the relocations around 0x7f are internal to this file;
   feel free to move them as needed to avoid conflicts with published
   relocation numbers.  */

static reloc_howto_type rl78_elf_howto_table [] =
{
  RL78REL (NONE,         0,  0, 0, dont,     FALSE),
  RL78REL (DIR32,        2, 32, 0, signed,   FALSE),
  RL78REL (DIR24S,       2, 24, 0, signed,   FALSE),
  RL78REL (DIR16,        1, 16, 0, dont,     FALSE),
  RL78REL (DIR16U,       1, 16, 0, unsigned, FALSE),
  RL78REL (DIR16S,       1, 16, 0, signed,   FALSE),
  RL78REL (DIR8,         0,  8, 0, dont,     FALSE),
  RL78REL (DIR8U,        0,  8, 0, unsigned, FALSE),
  RL78REL (DIR8S,        0,  8, 0, signed,   FALSE),
  RL78REL (DIR24S_PCREL, 2, 24, 0, signed,   TRUE),
  RL78REL (DIR16S_PCREL, 1, 16, 0, signed,   TRUE),
  RL78REL (DIR8S_PCREL,  0,  8, 0, signed,   TRUE),
  RL78REL (DIR16UL,      1, 16, 2, unsigned, FALSE),
  RL78REL (DIR16UW,      1, 16, 1, unsigned, FALSE),
  RL78REL (DIR8UL,       0,  8, 2, unsigned, FALSE),
  RL78REL (DIR8UW,       0,  8, 1, unsigned, FALSE),
  RL78REL (DIR32_REV,    1, 16, 0, dont,     FALSE),
  RL78REL (DIR16_REV,    1, 16, 0, dont,     FALSE),
  RL78REL (DIR3U_PCREL,  0,  3, 0, dont,     TRUE),

  EMPTY_HOWTO (0x13),
  EMPTY_HOWTO (0x14),
  EMPTY_HOWTO (0x15),
  EMPTY_HOWTO (0x16),
  EMPTY_HOWTO (0x17),
  EMPTY_HOWTO (0x18),
  EMPTY_HOWTO (0x19),
  EMPTY_HOWTO (0x1a),
  EMPTY_HOWTO (0x1b),
  EMPTY_HOWTO (0x1c),
  EMPTY_HOWTO (0x1d),
  EMPTY_HOWTO (0x1e),
  EMPTY_HOWTO (0x1f),

  EMPTY_HOWTO (0x20),
  EMPTY_HOWTO (0x21),
  EMPTY_HOWTO (0x22),
  EMPTY_HOWTO (0x23),
  EMPTY_HOWTO (0x24),
  EMPTY_HOWTO (0x25),
  EMPTY_HOWTO (0x26),
  EMPTY_HOWTO (0x27),
  EMPTY_HOWTO (0x28),
  EMPTY_HOWTO (0x29),
  EMPTY_HOWTO (0x2a),
  EMPTY_HOWTO (0x2b),
  EMPTY_HOWTO (0x2c),
  EMPTY_HOWTO (0x2d),

  EMPTY_HOWTO (0x2e),
  EMPTY_HOWTO (0x2f),
  EMPTY_HOWTO (0x30),
  EMPTY_HOWTO (0x31),
  EMPTY_HOWTO (0x32),
  EMPTY_HOWTO (0x33),
  EMPTY_HOWTO (0x34),
  EMPTY_HOWTO (0x35),
  EMPTY_HOWTO (0x36),
  EMPTY_HOWTO (0x37),
  EMPTY_HOWTO (0x38),
  EMPTY_HOWTO (0x39),
  EMPTY_HOWTO (0x3a),
  EMPTY_HOWTO (0x3b),
  EMPTY_HOWTO (0x3c),
  EMPTY_HOWTO (0x3d),
  EMPTY_HOWTO (0x3e),
  EMPTY_HOWTO (0x3f),
  EMPTY_HOWTO (0x40),

  RL78REL (ABS32,        2, 32, 0, dont,     FALSE),
  RL78REL (ABS24S,       2, 24, 0, signed,   FALSE),
  RL78REL (ABS16,        1, 16, 0, dont,     FALSE),
  RL78REL (ABS16U,       1, 16, 0, unsigned, FALSE),
  RL78REL (ABS16S,       1, 16, 0, signed,   FALSE),
  RL78REL (ABS8,         0,  8, 0, dont,     FALSE),
  RL78REL (ABS8U,        0,  8, 0, unsigned, FALSE),
  RL78REL (ABS8S,        0,  8, 0, signed,   FALSE),
  RL78REL (ABS24S_PCREL, 2, 24, 0, signed,   TRUE),
  RL78REL (ABS16S_PCREL, 1, 16, 0, signed,   TRUE),
  RL78REL (ABS8S_PCREL,  0,  8, 0, signed,   TRUE),
  RL78REL (ABS16UL,      1, 16, 0, unsigned, FALSE),
  RL78REL (ABS16UW,      1, 16, 0, unsigned, FALSE),
  RL78REL (ABS8UL,       0,  8, 0, unsigned, FALSE),
  RL78REL (ABS8UW,       0,  8, 0, unsigned, FALSE),
  RL78REL (ABS32_REV,    2, 32, 0, dont,     FALSE),
  RL78REL (ABS16_REV,    1, 16, 0, dont,     FALSE),

#define STACK_REL_P(x) ((x) <= R_RL78_ABS16_REV && (x) >= R_RL78_ABS32)

  EMPTY_HOWTO (0x52),
  EMPTY_HOWTO (0x53),
  EMPTY_HOWTO (0x54),
  EMPTY_HOWTO (0x55),
  EMPTY_HOWTO (0x56),
  EMPTY_HOWTO (0x57),
  EMPTY_HOWTO (0x58),
  EMPTY_HOWTO (0x59),
  EMPTY_HOWTO (0x5a),
  EMPTY_HOWTO (0x5b),
  EMPTY_HOWTO (0x5c),
  EMPTY_HOWTO (0x5d),
  EMPTY_HOWTO (0x5e),
  EMPTY_HOWTO (0x5f),
  EMPTY_HOWTO (0x60),
  EMPTY_HOWTO (0x61),
  EMPTY_HOWTO (0x62),
  EMPTY_HOWTO (0x63),
  EMPTY_HOWTO (0x64),
  EMPTY_HOWTO (0x65),
  EMPTY_HOWTO (0x66),
  EMPTY_HOWTO (0x67),
  EMPTY_HOWTO (0x68),
  EMPTY_HOWTO (0x69),
  EMPTY_HOWTO (0x6a),
  EMPTY_HOWTO (0x6b),
  EMPTY_HOWTO (0x6c),
  EMPTY_HOWTO (0x6d),
  EMPTY_HOWTO (0x6e),
  EMPTY_HOWTO (0x6f),
  EMPTY_HOWTO (0x70),
  EMPTY_HOWTO (0x71),
  EMPTY_HOWTO (0x72),
  EMPTY_HOWTO (0x73),
  EMPTY_HOWTO (0x74),
  EMPTY_HOWTO (0x75),
  EMPTY_HOWTO (0x76),
  EMPTY_HOWTO (0x77),

  EMPTY_HOWTO (0x78),
  EMPTY_HOWTO (0x79),
  EMPTY_HOWTO (0x7a),
  EMPTY_HOWTO (0x7b),
  EMPTY_HOWTO (0x7c),
  EMPTY_HOWTO (0x7d),
  EMPTY_HOWTO (0x7e),
  EMPTY_HOWTO (0x7f),

  RL78REL (SYM,       2, 32, 0, dont, FALSE),
  RL78REL (OPneg,     2, 32, 0, dont, FALSE),
  RL78REL (OPadd,     2, 32, 0, dont, FALSE),
  RL78REL (OPsub,     2, 32, 0, dont, FALSE),
  RL78REL (OPmul,     2, 32, 0, dont, FALSE),
  RL78REL (OPdiv,     2, 32, 0, dont, FALSE),
  RL78REL (OPshla,    2, 32, 0, dont, FALSE),
  RL78REL (OPshra,    2, 32, 0, dont, FALSE),
  RL78REL (OPsctsize, 2, 32, 0, dont, FALSE),
  EMPTY_HOWTO (0x89),
  EMPTY_HOWTO (0x8a),
  EMPTY_HOWTO (0x8b),
  EMPTY_HOWTO (0x8c),
  RL78REL (OPscttop,  2, 32, 0, dont, FALSE),
  EMPTY_HOWTO (0x8e),
  EMPTY_HOWTO (0x8f),
  RL78REL (OPand,     2, 32, 0, dont, FALSE),
  RL78REL (OPor,      2, 32, 0, dont, FALSE),
  RL78REL (OPxor,     2, 32, 0, dont, FALSE),
  RL78REL (OPnot,     2, 32, 0, dont, FALSE),
  RL78REL (OPmod,     2, 32, 0, dont, FALSE),
  RL78REL (OPromtop,  2, 32, 0, dont, FALSE),
  RL78REL (OPramtop,  2, 32, 0, dont, FALSE)
};

/* Map BFD reloc types to RL78 ELF reloc types.  */

struct rl78_reloc_map
{
  bfd_reloc_code_real_type  bfd_reloc_val;
  unsigned int              rl78_reloc_val;
};

static const struct rl78_reloc_map rl78_reloc_map [] =
{
  { BFD_RELOC_NONE,		R_RL78_NONE },
  { BFD_RELOC_8,		R_RL78_DIR8S },
  { BFD_RELOC_16,		R_RL78_DIR16S },
  { BFD_RELOC_24,		R_RL78_DIR24S },
  { BFD_RELOC_32,		R_RL78_DIR32 },
  { BFD_RELOC_RL78_16_OP,	R_RL78_DIR16 },
  { BFD_RELOC_RL78_DIR3U_PCREL,	R_RL78_DIR3U_PCREL },
  { BFD_RELOC_8_PCREL,		R_RL78_DIR8S_PCREL },
  { BFD_RELOC_16_PCREL,		R_RL78_DIR16S_PCREL },
  { BFD_RELOC_24_PCREL,		R_RL78_DIR24S_PCREL },
  { BFD_RELOC_RL78_8U,		R_RL78_DIR8U },
  { BFD_RELOC_RL78_16U,		R_RL78_DIR16U },
  { BFD_RELOC_RL78_SYM,		R_RL78_SYM },
  { BFD_RELOC_RL78_OP_SUBTRACT,	R_RL78_OPsub },
  { BFD_RELOC_RL78_OP_NEG,	R_RL78_OPneg },
  { BFD_RELOC_RL78_OP_AND,	R_RL78_OPand },
  { BFD_RELOC_RL78_OP_SHRA,	R_RL78_OPshra },
  { BFD_RELOC_RL78_ABS8,	R_RL78_ABS8 },
  { BFD_RELOC_RL78_ABS16,	R_RL78_ABS16 },
  { BFD_RELOC_RL78_ABS16_REV,	R_RL78_ABS16_REV },
  { BFD_RELOC_RL78_ABS32,	R_RL78_ABS32 },
  { BFD_RELOC_RL78_ABS32_REV,	R_RL78_ABS32_REV },
  { BFD_RELOC_RL78_ABS16UL,	R_RL78_ABS16UL },
  { BFD_RELOC_RL78_ABS16UW,	R_RL78_ABS16UW },
  { BFD_RELOC_RL78_ABS16U,	R_RL78_ABS16U }
};

static reloc_howto_type *
rl78_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
			bfd_reloc_code_real_type code)
{
  unsigned int i;

  if (code == BFD_RELOC_RL78_32_OP)
    return rl78_elf_howto_table + R_RL78_DIR32;

  for (i = ARRAY_SIZE (rl78_reloc_map); --i;)
    if (rl78_reloc_map [i].bfd_reloc_val == code)
      return rl78_elf_howto_table + rl78_reloc_map[i].rl78_reloc_val;

  return NULL;
}

static reloc_howto_type *
rl78_reloc_name_lookup (bfd * abfd ATTRIBUTE_UNUSED, const char * r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (rl78_elf_howto_table); i++)
    if (rl78_elf_howto_table[i].name != NULL
	&& strcasecmp (rl78_elf_howto_table[i].name, r_name) == 0)
      return rl78_elf_howto_table + i;

  return NULL;
}

/* Set the howto pointer for an RL78 ELF reloc.  */

static void
rl78_info_to_howto_rela (bfd *               abfd ATTRIBUTE_UNUSED,
			 arelent *           cache_ptr,
			 Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_RL78_max);
  cache_ptr->howto = rl78_elf_howto_table + r_type;
}

static bfd_vma
get_symbol_value (const char *            name,
		  bfd_reloc_status_type * status,
		  struct bfd_link_info *  info,
		  bfd *                   input_bfd,
		  asection *              input_section,
		  int			  offset)
{
  bfd_vma value = 0;
  struct bfd_link_hash_entry * h;

  h = bfd_link_hash_lookup (info->hash, name, FALSE, FALSE, TRUE);

  if (h == NULL
      || (h->type != bfd_link_hash_defined
	  && h->type != bfd_link_hash_defweak))
    * status = info->callbacks->undefined_symbol
      (info, name, input_bfd, input_section, offset, TRUE);
  else
    value = (h->u.def.value
	     + h->u.def.section->output_section->vma
	     + h->u.def.section->output_offset);

  return value;
}

static bfd_vma
get_romstart (bfd_reloc_status_type * status,
	      struct bfd_link_info *  info,
	      bfd *                   abfd,
	      asection *              sec,
	      int		      offset)
{
  static bfd_boolean cached = FALSE;
  static bfd_vma     cached_value = 0;

  if (!cached)
    {
      cached_value = get_symbol_value ("_start", status, info, abfd, sec, offset);
      cached = TRUE;
    }
  return cached_value;
}

static bfd_vma
get_ramstart (bfd_reloc_status_type * status,
	      struct bfd_link_info *  info,
	      bfd *                   abfd,
	      asection *              sec,
	      int		      offset)
{
  static bfd_boolean cached = FALSE;
  static bfd_vma     cached_value = 0;

  if (!cached)
    {
      cached_value = get_symbol_value ("__datastart", status, info, abfd, sec, offset);
      cached = TRUE;
    }
  return cached_value;
}

#define NUM_STACK_ENTRIES 16
static int32_t rl78_stack [ NUM_STACK_ENTRIES ];
static unsigned int rl78_stack_top;

#define RL78_STACK_PUSH(val)			\
  do						\
    {						\
      if (rl78_stack_top < NUM_STACK_ENTRIES)	\
        rl78_stack [rl78_stack_top ++] = (val);	\
      else					\
        r = bfd_reloc_dangerous;		\
    }						\
  while (0)

#define RL78_STACK_POP(dest)			\
  do						\
    {						\
      if (rl78_stack_top > 0)			\
        (dest) = rl78_stack [-- rl78_stack_top];	\
      else					\
        (dest) = 0, r = bfd_reloc_dangerous;	\
    }						\
  while (0)

/* Relocate an RL78 ELF section.
   There is some attempt to make this function usable for many architectures,
   both USE_REL and USE_RELA ['twould be nice if such a critter existed],
   if only to serve as a learning tool.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
rl78_elf_relocate_section
    (bfd *                   output_bfd,
     struct bfd_link_info *  info,
     bfd *                   input_bfd,
     asection *              input_section,
     bfd_byte *              contents,
     Elf_Internal_Rela *     relocs,
     Elf_Internal_Sym *      local_syms,
     asection **             local_sections)
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  Elf_Internal_Rela *           rel;
  Elf_Internal_Rela *           relend;
  bfd *dynobj;
  asection *splt;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  dynobj = elf_hash_table (info)->dynobj;
  splt = NULL;
  if (dynobj != NULL)
    splt = bfd_get_section_by_name (dynobj, ".plt");

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *           howto;
      unsigned long                r_symndx;
      Elf_Internal_Sym *           sym;
      asection *                   sec;
      struct elf_link_hash_entry * h;
      bfd_vma                      relocation;
      bfd_reloc_status_type        r;
      const char *                 name = NULL;
      bfd_boolean                  unresolved_reloc = TRUE;
      int                          r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);

      howto  = rl78_elf_howto_table + ELF32_R_TYPE (rel->r_info);
      h      = NULL;
      sym    = NULL;
      sec    = NULL;
      relocation = 0;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, & sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (sym->st_name == 0) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  bfd_boolean warned;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes, h,
				   sec, relocation, unresolved_reloc,
				   warned);

	  name = h->root.root.string;
	}

      if (sec != NULL && elf_discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, relend, howto, contents);

      if (info->relocatable)
	{
	  /* This is a relocatable link.  We don't have to change
             anything, unless the reloc is against a section symbol,
             in which case we have to adjust according to where the
             section symbol winds up in the output section.  */
	  if (sym != NULL && ELF_ST_TYPE (sym->st_info) == STT_SECTION)
	    rel->r_addend += sec->output_offset;
	  continue;
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_RL78_DIR16S:
	  {
	    bfd_vma *plt_offset;

	    if (h != NULL)
	      plt_offset = &h->plt.offset;
	    else
	      plt_offset = elf_local_got_offsets (input_bfd) + r_symndx;

	    /*	    printf("%s: rel %x plt %d\n", h ? h->root.root.string : "(none)",
		    relocation, *plt_offset);*/
	    if (valid_16bit_address (relocation))
	      {
	        /* If the symbol is in range for a 16-bit address, we should
		   have deallocated the plt entry in relax_section.  */
	        BFD_ASSERT (*plt_offset == (bfd_vma) -1);
	      }
	    else
	      {
		/* If the symbol is out of range for a 16-bit address,
		   we must have allocated a plt entry.  */
		BFD_ASSERT (*plt_offset != (bfd_vma) -1);

		/* If this is the first time we've processed this symbol,
		   fill in the plt entry with the correct symbol address.  */
		if ((*plt_offset & 1) == 0)
		  {
		    unsigned int x;

		    x = 0x000000ec;  /* br !!abs24 */
		    x |= (relocation << 8) & 0xffffff00;
		    bfd_put_32 (input_bfd, x, splt->contents + *plt_offset);
		    *plt_offset |= 1;
		  }

		relocation = (splt->output_section->vma
			      + splt->output_offset
			      + (*plt_offset & -2));
		if (name)
		{
		  char *newname = bfd_malloc (strlen(name)+5);
		  strcpy (newname, name);
		  strcat(newname, ".plt");
		  _bfd_generic_link_add_one_symbol (info,
						    input_bfd,
						    newname,
						    BSF_FUNCTION | BSF_WEAK,
						    splt,
						    (*plt_offset & -2),
						    0,
						    1,
						    0,
						    0);
		}
	      }
	  }
	  break;
	}

      if (h != NULL && h->root.type == bfd_link_hash_undefweak)
	/* If the symbol is undefined and weak
	   then the relocation resolves to zero.  */
	relocation = 0;
      else
	{
	  if (howto->pc_relative)
	    {
	      relocation -= (input_section->output_section->vma
			     + input_section->output_offset
			     + rel->r_offset);
	      relocation -= bfd_get_reloc_size (howto);
	    }

	  relocation += rel->r_addend;
	}

      r = bfd_reloc_ok;

#define RANGE(a,b) if (a > (long) relocation || (long) relocation > b) r = bfd_reloc_overflow
#define ALIGN(m)   if (relocation & m) r = bfd_reloc_other;
#define OP(i)      (contents[rel->r_offset + (i)])

      /* Opcode relocs are always big endian.  Data relocs are bi-endian.  */
      switch (r_type)
	{
	case R_RL78_NONE:
	  break;

	case R_RL78_DIR8S_PCREL:
	  RANGE (-128, 127);
	  OP (0) = relocation;
	  break;

	case R_RL78_DIR8S:
	  RANGE (-128, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_DIR8U:
	  RANGE (0, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_DIR16S_PCREL:
	  RANGE (-32768, 32767);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_DIR16S:
	  if ((relocation & 0xf0000) == 0xf0000)
	    relocation &= 0xffff;
	  RANGE (-32768, 65535);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_DIR16U:
	  RANGE (0, 65536);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_DIR16:
	  RANGE (-32768, 65536);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_DIR16_REV:
	  RANGE (-32768, 65536);
	  OP (1) = relocation;
	  OP (0) = relocation >> 8;
	  break;

	case R_RL78_DIR3U_PCREL:
	  RANGE (3, 10);
	  OP (0) &= 0xf8;
	  OP (0) |= relocation & 0x07;
	  break;

	case R_RL78_DIR24S_PCREL:
	  RANGE (-0x800000, 0x7fffff);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  OP (2) = relocation >> 16;
	  break;

	case R_RL78_DIR24S:
	  RANGE (-0x800000, 0x7fffff);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  OP (2) = relocation >> 16;
	  break;

	case R_RL78_DIR32:
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  OP (2) = relocation >> 16;
	  OP (3) = relocation >> 24;
	  break;

	case R_RL78_DIR32_REV:
	  OP (3) = relocation;
	  OP (2) = relocation >> 8;
	  OP (1) = relocation >> 16;
	  OP (0) = relocation >> 24;
	  break;

	  /* Complex reloc handling:  */

	case R_RL78_ABS32:
	  RL78_STACK_POP (relocation);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  OP (2) = relocation >> 16;
	  OP (3) = relocation >> 24;
	  break;

	case R_RL78_ABS32_REV:
	  RL78_STACK_POP (relocation);
	  OP (3) = relocation;
	  OP (2) = relocation >> 8;
	  OP (1) = relocation >> 16;
	  OP (0) = relocation >> 24;
	  break;

	case R_RL78_ABS24S_PCREL:
	case R_RL78_ABS24S:
	  RL78_STACK_POP (relocation);
	  RANGE (-0x800000, 0x7fffff);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  OP (2) = relocation >> 16;
	  break;

	case R_RL78_ABS16:
	  RL78_STACK_POP (relocation);
	  RANGE (-32768, 65535);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_ABS16_REV:
	  RL78_STACK_POP (relocation);
	  RANGE (-32768, 65535);
	  OP (1) = relocation;
	  OP (0) = relocation >> 8;
	  break;

	case R_RL78_ABS16S_PCREL:
	case R_RL78_ABS16S:
	  RL78_STACK_POP (relocation);
	  RANGE (-32768, 32767);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_ABS16U:
	  RL78_STACK_POP (relocation);
	  RANGE (0, 65536);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_ABS16UL:
	  RL78_STACK_POP (relocation);
	  relocation >>= 2;
	  RANGE (0, 65536);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_ABS16UW:
	  RL78_STACK_POP (relocation);
	  relocation >>= 1;
	  RANGE (0, 65536);
	  OP (0) = relocation;
	  OP (1) = relocation >> 8;
	  break;

	case R_RL78_ABS8:
	  RL78_STACK_POP (relocation);
	  RANGE (-128, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_ABS8U:
	  RL78_STACK_POP (relocation);
	  RANGE (0, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_ABS8UL:
	  RL78_STACK_POP (relocation);
	  relocation >>= 2;
	  RANGE (0, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_ABS8UW:
	  RL78_STACK_POP (relocation);
	  relocation >>= 1;
	  RANGE (0, 255);
	  OP (0) = relocation;
	  break;

	case R_RL78_ABS8S_PCREL:
	case R_RL78_ABS8S:
	  RL78_STACK_POP (relocation);
	  RANGE (-128, 127);
	  OP (0) = relocation;
	  break;

	case R_RL78_SYM:
	  if (r_symndx < symtab_hdr->sh_info)
	    RL78_STACK_PUSH (sec->output_section->vma
			   + sec->output_offset
			   + sym->st_value
			   + rel->r_addend);
	  else
	    {
	      if (h != NULL
		  && (h->root.type == bfd_link_hash_defined
		      || h->root.type == bfd_link_hash_defweak))
		RL78_STACK_PUSH (h->root.u.def.value
			       + sec->output_section->vma
			       + sec->output_offset
			       + rel->r_addend);
	      else
		_bfd_error_handler (_("Warning: RL78_SYM reloc with an unknown symbol"));
	    }
	  break;

	case R_RL78_OPneg:
	  {
	    int32_t tmp;

	    RL78_STACK_POP (tmp);
	    tmp = - tmp;
	    RL78_STACK_PUSH (tmp);
	  }
	  break;

	case R_RL78_OPadd:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 += tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPsub:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp2 -= tmp1;
	    RL78_STACK_PUSH (tmp2);
	  }
	  break;

	case R_RL78_OPmul:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 *= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPdiv:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 /= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPshla:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 <<= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPshra:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 >>= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPsctsize:
	  RL78_STACK_PUSH (input_section->size);
	  break;

	case R_RL78_OPscttop:
	  RL78_STACK_PUSH (input_section->output_section->vma);
	  break;

	case R_RL78_OPand:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 &= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPor:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 |= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPxor:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 ^= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPnot:
	  {
	    int32_t tmp;

	    RL78_STACK_POP (tmp);
	    tmp = ~ tmp;
	    RL78_STACK_PUSH (tmp);
	  }
	  break;

	case R_RL78_OPmod:
	  {
	    int32_t tmp1, tmp2;

	    RL78_STACK_POP (tmp2);
	    RL78_STACK_POP (tmp1);
	    tmp1 %= tmp2;
	    RL78_STACK_PUSH (tmp1);
	  }
	  break;

	case R_RL78_OPromtop:
	  RL78_STACK_PUSH (get_romstart (&r, info, input_bfd, input_section, rel->r_offset));
	  break;

	case R_RL78_OPramtop:
	  RL78_STACK_PUSH (get_ramstart (&r, info, input_bfd, input_section, rel->r_offset));
	  break;

	default:
	  r = bfd_reloc_notsupported;
	  break;
	}

      if (r != bfd_reloc_ok)
	{
	  const char * msg = NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      /* Catch the case of a missing function declaration
		 and emit a more helpful error message.  */
	      if (r_type == R_RL78_DIR24S_PCREL)
		msg = _("%B(%A): error: call to undefined function '%s'");
	      else
		r = info->callbacks->reloc_overflow
		  (info, (h ? &h->root : NULL), name, howto->name, (bfd_vma) 0,
		   input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      r = info->callbacks->undefined_symbol
		(info, name, input_bfd, input_section, rel->r_offset,
		 TRUE);
	      break;

	    case bfd_reloc_other:
	      msg = _("%B(%A): warning: unaligned access to symbol '%s' in the small data area");
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("%B(%A): internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("%B(%A): internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("%B(%A): internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("%B(%A): internal error: unknown error");
	      break;
	    }

	  if (msg)
	    _bfd_error_handler (msg, input_bfd, input_section, name);

	  if (! r)
	    return FALSE;
	}
    }

  return TRUE;
}

/* Function to set the ELF flag bits.  */

static bfd_boolean
rl78_elf_set_private_flags (bfd * abfd, flagword flags)
{
  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = TRUE;
  return TRUE;
}

static bfd_boolean no_warn_mismatch = FALSE;

void bfd_elf32_rl78_set_target_flags (bfd_boolean);

void
bfd_elf32_rl78_set_target_flags (bfd_boolean user_no_warn_mismatch)
{
  no_warn_mismatch = user_no_warn_mismatch;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */

static bfd_boolean
rl78_elf_merge_private_bfd_data (bfd * ibfd, bfd * obfd)
{
  flagword old_flags;
  flagword new_flags;
  bfd_boolean error = FALSE;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;

  if (!elf_flags_init (obfd))
    {
      /* First call, no flags set.  */
      elf_flags_init (obfd) = TRUE;
      elf_elfheader (obfd)->e_flags = new_flags;
    }

  return !error;
}

static bfd_boolean
rl78_elf_print_private_bfd_data (bfd * abfd, void * ptr)
{
  FILE * file = (FILE *) ptr;
  flagword flags;

  BFD_ASSERT (abfd != NULL && ptr != NULL);

  /* Print normal ELF private data.  */
  _bfd_elf_print_private_bfd_data (abfd, ptr);

  flags = elf_elfheader (abfd)->e_flags;
  fprintf (file, _("private flags = 0x%lx:"), (long) flags);

  fputc ('\n', file);
  return TRUE;
}

/* Return the MACH for an e_flags value.  */

static int
elf32_rl78_machine (bfd * abfd)
{
  if ((elf_elfheader (abfd)->e_flags & EF_RL78_CPU_MASK) == EF_RL78_CPU_RL78)
    return bfd_mach_rl78;

  return 0;
}

static bfd_boolean
rl78_elf_object_p (bfd * abfd)
{
  bfd_default_set_arch_mach (abfd, bfd_arch_rl78,
			     elf32_rl78_machine (abfd));
  return TRUE;
}
 
#ifdef DEBUG
void
rl78_dump_symtab (bfd * abfd, void * internal_syms, void * external_syms)
{
  size_t locsymcount;
  Elf_Internal_Sym * isymbuf;
  Elf_Internal_Sym * isymend;
  Elf_Internal_Sym * isym;
  Elf_Internal_Shdr * symtab_hdr;
  bfd_boolean free_internal = FALSE, free_external = FALSE;
  char * st_info_str;
  char * st_info_stb_str;
  char * st_other_str;
  char * st_shndx_str;

  if (! internal_syms)
    {
      internal_syms = bfd_malloc (1000);
      free_internal = 1;
    }
  if (! external_syms)
    {
      external_syms = bfd_malloc (1000);
      free_external = 1;
    }

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  locsymcount = symtab_hdr->sh_size / get_elf_backend_data (abfd)->s->sizeof_sym;
  if (free_internal)
    isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
				    symtab_hdr->sh_info, 0,
				    internal_syms, external_syms, NULL);
  else
    isymbuf = internal_syms;
  isymend = isymbuf + locsymcount;

  for (isym = isymbuf ; isym < isymend ; isym++)
    {
      switch (ELF_ST_TYPE (isym->st_info))
	{
	case STT_FUNC: st_info_str = "STT_FUNC";
	case STT_SECTION: st_info_str = "STT_SECTION";
	case STT_FILE: st_info_str = "STT_FILE";
	case STT_OBJECT: st_info_str = "STT_OBJECT";
	case STT_TLS: st_info_str = "STT_TLS";
	default: st_info_str = "";
	}
      switch (ELF_ST_BIND (isym->st_info))
	{
	case STB_LOCAL: st_info_stb_str = "STB_LOCAL";
	case STB_GLOBAL: st_info_stb_str = "STB_GLOBAL";
	default: st_info_stb_str = "";
	}
      switch (ELF_ST_VISIBILITY (isym->st_other))
	{
	case STV_DEFAULT: st_other_str = "STV_DEFAULT";
	case STV_INTERNAL: st_other_str = "STV_INTERNAL";
	case STV_PROTECTED: st_other_str = "STV_PROTECTED";
	default: st_other_str = "";
	}
      switch (isym->st_shndx)
	{
	case SHN_ABS: st_shndx_str = "SHN_ABS";
	case SHN_COMMON: st_shndx_str = "SHN_COMMON";
	case SHN_UNDEF: st_shndx_str = "SHN_UNDEF";
	default: st_shndx_str = "";
	}

      printf ("isym = %p st_value = %lx st_size = %lx st_name = (%lu) %s "
	      "st_info = (%d) %s %s st_other = (%d) %s st_shndx = (%d) %s\n",
	      isym,
	      (unsigned long) isym->st_value,
	      (unsigned long) isym->st_size,
	      isym->st_name,
	      bfd_elf_string_from_elf_section (abfd, symtab_hdr->sh_link,
					       isym->st_name),
	      isym->st_info, st_info_str, st_info_stb_str,
	      isym->st_other, st_other_str,
	      isym->st_shndx, st_shndx_str);
    }
  if (free_internal)
    free (internal_syms);
  if (free_external)
    free (external_syms);
}

char *
rl78_get_reloc (long reloc)
{
  if (0 <= reloc && reloc < R_RL78_max)
    return rl78_elf_howto_table[reloc].name;
  return "";
}
#endif /* DEBUG */


/* support PLT for 16-bit references to 24-bit functions.  */

/* We support 16-bit pointers to code above 64k by generating a thunk
   below 64k containing a JMP instruction to the final address.  */
 
static bfd_boolean
rl78_elf_check_relocs
    (bfd *                     abfd,
     struct bfd_link_info *    info,
     asection *                sec,
     const Elf_Internal_Rela * relocs)
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  const Elf_Internal_Rela *     rel;
  const Elf_Internal_Rela *     rel_end;
  bfd_vma *local_plt_offsets;
  asection *splt;
  bfd *dynobj;

  if (info->relocatable)
    return TRUE;
 
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  local_plt_offsets = elf_local_got_offsets (abfd);
  splt = NULL;
  dynobj = elf_hash_table(info)->dynobj;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;
      bfd_vma *offset;
 
      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	}
 
      switch (ELF32_R_TYPE (rel->r_info))
        {
	  /* This relocation describes a 16-bit pointer to a function.
	     We may need to allocate a thunk in low memory; reserve memory
	     for it now.  */
	case R_RL78_DIR16S:
	  if (dynobj == NULL)
	    elf_hash_table (info)->dynobj = dynobj = abfd;
	  if (splt == NULL)
	    {
	      splt = bfd_get_section_by_name (dynobj, ".plt");
	      if (splt == NULL)
		{
		  flagword flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
				    | SEC_IN_MEMORY | SEC_LINKER_CREATED
				    | SEC_READONLY | SEC_CODE);
		  splt = bfd_make_section_with_flags (dynobj, ".plt", flags);
		  if (splt == NULL
		      || ! bfd_set_section_alignment (dynobj, splt, 1))
		    return FALSE;
		}
	    }

	  if (h != NULL)
	    offset = &h->plt.offset;
	  else
	    {
	      if (local_plt_offsets == NULL)
		{
		  size_t size;
		  unsigned int i;

		  size = symtab_hdr->sh_info * sizeof (bfd_vma);
		  local_plt_offsets = (bfd_vma *) bfd_alloc (abfd, size);
		  if (local_plt_offsets == NULL)
		    return FALSE;
		  elf_local_got_offsets (abfd) = local_plt_offsets;

		  for (i = 0; i < symtab_hdr->sh_info; i++)
		    local_plt_offsets[i] = (bfd_vma) -1;
		}
	      offset = &local_plt_offsets[r_symndx];
	    }

	  if (*offset == (bfd_vma) -1)
	    {
	      *offset = splt->size;
	      splt->size += 4;
	    }
	  break;
        }
    }
 
  return TRUE;
}

/* This must exist if dynobj is ever set.  */

static bfd_boolean
rl78_elf_finish_dynamic_sections (bfd *abfd ATTRIBUTE_UNUSED,
                                  struct bfd_link_info *info)
{
  bfd *dynobj;
  asection *splt;

  /* As an extra sanity check, verify that all plt entries have
     been filled in.  */

  if ((dynobj = elf_hash_table (info)->dynobj) != NULL
      && (splt = bfd_get_section_by_name (dynobj, ".plt")) != NULL)
    {
      bfd_byte *contents = splt->contents;
      unsigned int i, size = splt->size;
      for (i = 0; i < size; i += 4)
	{
	  unsigned int x = bfd_get_32 (dynobj, contents + i);
	  BFD_ASSERT (x != 0);
	}
    }

  return TRUE;
}

static bfd_boolean
rl78_elf_always_size_sections (bfd *output_bfd ATTRIBUTE_UNUSED,
                               struct bfd_link_info *info)
{
  bfd *dynobj;
  asection *splt;

  if (info->relocatable)
    return TRUE;

  dynobj = elf_hash_table (info)->dynobj;
  if (dynobj == NULL)
    return TRUE;

  splt = bfd_get_section_by_name (dynobj, ".plt");
  BFD_ASSERT (splt != NULL);

  splt->contents = (bfd_byte *) bfd_zalloc (dynobj, splt->size);
  if (splt->contents == NULL)
    return FALSE;

  return TRUE;
}



/* Handle relaxing.  */

/* A subroutine of rl78_elf_relax_section.  If the global symbol H
   is within the low 64k, remove any entry for it in the plt.  */

struct relax_plt_data
{
  asection *splt;
  bfd_boolean *again;
};

static bfd_boolean
rl78_relax_plt_check (struct elf_link_hash_entry *h,
                      PTR xdata)
{
  struct relax_plt_data *data = (struct relax_plt_data *) xdata;

  if (h->plt.offset != (bfd_vma) -1)
    {
      bfd_vma address;

      if (h->root.type == bfd_link_hash_undefined
	  || h->root.type == bfd_link_hash_undefweak)
	address = 0;
      else
	address = (h->root.u.def.section->output_section->vma
		   + h->root.u.def.section->output_offset
		   + h->root.u.def.value);

      if (valid_16bit_address (address))
	{
	  h->plt.offset = -1;
	  data->splt->size -= 4;
	  *data->again = TRUE;
	}
    }

  return TRUE;
}

/* A subroutine of rl78_elf_relax_section.  If the global symbol H
   previously had a plt entry, give it a new entry offset.  */

static bfd_boolean
rl78_relax_plt_realloc (struct elf_link_hash_entry *h,
                        PTR xdata)
{
  bfd_vma *entry = (bfd_vma *) xdata;

  if (h->plt.offset != (bfd_vma) -1)
    {
      h->plt.offset = *entry;
      *entry += 4;
    }

  return TRUE;
}

static bfd_boolean
rl78_elf_relax_plt_section (bfd *dynobj,
                            asection *splt,
                            struct bfd_link_info *info,
                            bfd_boolean *again)
{
  struct relax_plt_data relax_plt_data;
  bfd *ibfd;

  /* Assume nothing changes.  */
  *again = FALSE;

  if (info->relocatable)
    return TRUE;

  /* We only relax the .plt section at the moment.  */
  if (dynobj != elf_hash_table (info)->dynobj
      || strcmp (splt->name, ".plt") != 0)
    return TRUE;

  /* Quick check for an empty plt.  */
  if (splt->size == 0)
    return TRUE;

  /* Map across all global symbols; see which ones happen to
     fall in the low 64k.  */
  relax_plt_data.splt = splt;
  relax_plt_data.again = again;
  elf_link_hash_traverse (elf_hash_table (info), rl78_relax_plt_check,
			  &relax_plt_data);

  /* Likewise for local symbols, though that's somewhat less convenient
     as we have to walk the list of input bfds and swap in symbol data.  */
  for (ibfd = info->input_bfds; ibfd ; ibfd = ibfd->link_next)
    {
      bfd_vma *local_plt_offsets = elf_local_got_offsets (ibfd);
      Elf_Internal_Shdr *symtab_hdr;
      Elf_Internal_Sym *isymbuf = NULL;
      unsigned int idx;

      if (! local_plt_offsets)
	continue;

      symtab_hdr = &elf_tdata (ibfd)->symtab_hdr;
      if (symtab_hdr->sh_info != 0)
	{
	  isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
	  if (isymbuf == NULL)
	    isymbuf = bfd_elf_get_elf_syms (ibfd, symtab_hdr,
					    symtab_hdr->sh_info, 0,
					    NULL, NULL, NULL);
	  if (isymbuf == NULL)
	    return FALSE;
	}

      for (idx = 0; idx < symtab_hdr->sh_info; ++idx)
	{
	  Elf_Internal_Sym *isym;
	  asection *tsec;
	  bfd_vma address;

	  if (local_plt_offsets[idx] == (bfd_vma) -1)
	    continue;

	  isym = &isymbuf[idx];
	  if (isym->st_shndx == SHN_UNDEF)
	    continue;
	  else if (isym->st_shndx == SHN_ABS)
	    tsec = bfd_abs_section_ptr;
	  else if (isym->st_shndx == SHN_COMMON)
	    tsec = bfd_com_section_ptr;
	  else
	    tsec = bfd_section_from_elf_index (ibfd, isym->st_shndx);

	  address = (tsec->output_section->vma
		     + tsec->output_offset
		     + isym->st_value);
	  if (valid_16bit_address (address))
	    {
	      local_plt_offsets[idx] = -1;
	      splt->size -= 4;
	      *again = TRUE;
	    }
	}

      if (isymbuf != NULL
	  && symtab_hdr->contents != (unsigned char *) isymbuf)
	{
	  if (! info->keep_memory)
	    free (isymbuf);
	  else
	    {
	      /* Cache the symbols for elf_link_input_bfd.  */
	      symtab_hdr->contents = (unsigned char *) isymbuf;
	    }
	}
    }

  /* If we changed anything, walk the symbols again to reallocate
     .plt entry addresses.  */
  if (*again && splt->size > 0)
    {
      bfd_vma entry = 0;

      elf_link_hash_traverse (elf_hash_table (info),
			      rl78_relax_plt_realloc, &entry);

      for (ibfd = info->input_bfds; ibfd ; ibfd = ibfd->link_next)
	{
	  bfd_vma *local_plt_offsets = elf_local_got_offsets (ibfd);
	  unsigned int nlocals = elf_tdata (ibfd)->symtab_hdr.sh_info;
	  unsigned int idx;

	  if (! local_plt_offsets)
	    continue;

	  for (idx = 0; idx < nlocals; ++idx)
	    if (local_plt_offsets[idx] != (bfd_vma) -1)
	      {
	        local_plt_offsets[idx] = entry;
		entry += 4;
	      }
	}
    }

  return TRUE;
}

static bfd_boolean
rl78_elf_relax_section
    (bfd *                  abfd,
     asection *             sec,
     struct bfd_link_info * link_info,
     bfd_boolean *          again)
{
  if (abfd == elf_hash_table (link_info)->dynobj
      && strcmp (sec->name, ".plt") == 0)
    return rl78_elf_relax_plt_section (abfd, sec, link_info, again);

  /* Assume nothing changes.  */
  *again = FALSE;
  return TRUE;
}



#define ELF_ARCH		bfd_arch_rl78
#define ELF_MACHINE_CODE	EM_RL78
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_LITTLE_SYM	bfd_elf32_rl78_vec
#define TARGET_LITTLE_NAME	"elf32-rl78"

#define elf_info_to_howto_rel			NULL
#define elf_info_to_howto			rl78_info_to_howto_rela
#define elf_backend_object_p			rl78_elf_object_p
#define elf_backend_relocate_section		rl78_elf_relocate_section
#define elf_symbol_leading_char                 ('_')
#define elf_backend_can_gc_sections		1

#define bfd_elf32_bfd_reloc_type_lookup		rl78_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		rl78_reloc_name_lookup
#define bfd_elf32_bfd_set_private_flags		rl78_elf_set_private_flags
#define bfd_elf32_bfd_merge_private_bfd_data	rl78_elf_merge_private_bfd_data
#define bfd_elf32_bfd_print_private_bfd_data	rl78_elf_print_private_bfd_data

#define bfd_elf32_bfd_relax_section		rl78_elf_relax_section
#define elf_backend_check_relocs                rl78_elf_check_relocs
#define elf_backend_always_size_sections \
  rl78_elf_always_size_sections
#define elf_backend_finish_dynamic_sections \
  rl78_elf_finish_dynamic_sections

#include "elf32-target.h"
