/* BFD back-end for AArch64 COFF files.
   Copyright (C) 2021-2023 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */


#ifndef COFF_WITH_peAArch64
#define COFF_WITH_peAArch64
#endif

/* Note we have to make sure not to include headers twice.
   Not all headers are wrapped in #ifdef guards, so we define
   PEI_HEADERS to prevent double including here.  */
#ifndef PEI_HEADERS
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "coff/aarch64.h"
#include "coff/internal.h"
#include "coff/pe.h"
#include "libcoff.h"
#include "libiberty.h"
#endif

#include "libcoff.h"

static bfd_reloc_status_type
coff_aarch64_addr64_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			   arelent *reloc_entry,
			   asymbol *symbol ATTRIBUTE_UNUSED,
			   void *data,
			   asection *input_section ATTRIBUTE_UNUSED,
			   bfd *output_bfd ATTRIBUTE_UNUSED,
			   char **error_message ATTRIBUTE_UNUSED)
{
  uint64_t val = reloc_entry->addend;

  bfd_putl64 (val, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_addr32_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			   arelent *reloc_entry,
			   asymbol *symbol ATTRIBUTE_UNUSED,
			   void *data,
			   asection *input_section ATTRIBUTE_UNUSED,
			   bfd *output_bfd ATTRIBUTE_UNUSED,
			   char **error_message ATTRIBUTE_UNUSED)
{
  uint64_t val;

  if ((int64_t) reloc_entry->addend > 0x7fffffff
      || (int64_t) reloc_entry->addend < -0x7fffffff)
    return bfd_reloc_overflow;

  val = reloc_entry->addend;

  bfd_putl32 ((uint32_t) val, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_branch26_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			     arelent *reloc_entry,
			     asymbol *symbol ATTRIBUTE_UNUSED,
			     void *data,
			     asection *input_section ATTRIBUTE_UNUSED,
			     bfd *output_bfd ATTRIBUTE_UNUSED,
			     char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend;

  if (param > 0x7ffffff || param < -0x8000000)
    return bfd_reloc_overflow;

  op &= 0xfc000000;
  op |= (param >> 2) & 0x3ffffff;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_rel21_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			  arelent *reloc_entry,
			  asymbol *symbol ATTRIBUTE_UNUSED,
			  void *data,
			  asection *input_section ATTRIBUTE_UNUSED,
			  bfd *output_bfd ATTRIBUTE_UNUSED,
			  char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend;

  if (param > 0xfffff || param < -0x100000)
    return bfd_reloc_overflow;

  op &= 0x9f00001f;
  op |= (param & 0x1ffffc) << 3;
  op |= (param & 0x3) << 29;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_po12l_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			  arelent *reloc_entry,
			  asymbol *symbol ATTRIBUTE_UNUSED,
			  void *data,
			  asection *input_section ATTRIBUTE_UNUSED,
			  bfd *output_bfd ATTRIBUTE_UNUSED,
			  char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;
  uint8_t shift;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend & 0xfff;

  if ((op & 0xff800000) == 0x3d800000)
    {
      /* LDR / STR with q register */
      shift = 4;
    }
  else
    {
      /* top two bits represent how much addend should be shifted */
      shift = op >> 30;
    }

  if (param & ((1 << shift) - 1))
    return bfd_reloc_overflow;

  param >>= shift;

  op &= 0xffc003ff;
  op |= param << 10;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_branch19_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			     arelent *reloc_entry,
			     asymbol *symbol ATTRIBUTE_UNUSED,
			     void *data,
			     asection *input_section ATTRIBUTE_UNUSED,
			     bfd *output_bfd ATTRIBUTE_UNUSED,
			     char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend;

  if (param > 0xfffff || param < -0x100000)
    return bfd_reloc_overflow;

  op &= 0xff00001f;
  op |= ((param >> 2) & 0x7ffff) << 5;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_branch14_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			     arelent *reloc_entry,
			     asymbol *symbol ATTRIBUTE_UNUSED,
			     void *data,
			     asection *input_section ATTRIBUTE_UNUSED,
			     bfd *output_bfd ATTRIBUTE_UNUSED,
			     char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend;

  if (param > 0x7fff || param < -0x8000)
    return bfd_reloc_overflow;

  op &= 0xfff8001f;
  op |= ((param >> 2) & 0x3fff) << 5;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_po12a_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			  arelent *reloc_entry,
			  asymbol *symbol ATTRIBUTE_UNUSED,
			  void *data,
			  asection *input_section ATTRIBUTE_UNUSED,
			  bfd *output_bfd ATTRIBUTE_UNUSED,
			  char **error_message ATTRIBUTE_UNUSED)
{
  uint32_t op;
  int32_t param;

  op = bfd_getl32 (data + reloc_entry->address);
  param = reloc_entry->addend;

  op &= 0xffc003ff;
  op |= (param & 0xfff) << 10;

  bfd_putl32 (op, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_addr32nb_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			     arelent *reloc_entry,
			     asymbol *symbol ATTRIBUTE_UNUSED,
			     void *data,
			     asection *input_section ATTRIBUTE_UNUSED,
			     bfd *output_bfd ATTRIBUTE_UNUSED,
			     char **error_message ATTRIBUTE_UNUSED)
{
  uint64_t val;

  if ((int64_t) reloc_entry->addend > 0x7fffffff
      || (int64_t) reloc_entry->addend < -0x7fffffff)
    return bfd_reloc_overflow;

  val = reloc_entry->addend;

  bfd_putl32 ((uint32_t) val, data + reloc_entry->address);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
coff_aarch64_secrel_reloc (bfd *abfd ATTRIBUTE_UNUSED,
			   arelent *reloc_entry,
			   asymbol *symbol ATTRIBUTE_UNUSED,
			   void *data,
			   asection *input_section ATTRIBUTE_UNUSED,
			   bfd *output_bfd ATTRIBUTE_UNUSED,
			   char **error_message ATTRIBUTE_UNUSED)
{
  bfd_putl32 (reloc_entry->addend, data + reloc_entry->address);

  return bfd_reloc_ok;
}

/* In case we're on a 32-bit machine, construct a 64-bit "-1" value.  */
#define MINUS_ONE (~ (bfd_vma) 0)

static const reloc_howto_type arm64_reloc_howto_abs = HOWTO(IMAGE_REL_ARM64_ABSOLUTE, 0, 1, 0, false, 0,
	 complain_overflow_dont,
	 NULL, "IMAGE_REL_ARM64_ABSOLUTE",
	 false, 0, 0, false);

static const reloc_howto_type arm64_reloc_howto_64 = HOWTO(IMAGE_REL_ARM64_ADDR64, 0, 8, 64, false, 0,
	 complain_overflow_bitfield,
	 coff_aarch64_addr64_reloc, "IMAGE_REL_ARM64_ADDR64",
	 false, MINUS_ONE, MINUS_ONE, false);

static const reloc_howto_type arm64_reloc_howto_32 = HOWTO (IMAGE_REL_ARM64_ADDR32, 0, 4, 32, false, 0,
	 complain_overflow_bitfield,
	 coff_aarch64_addr32_reloc, "IMAGE_REL_ARM64_ADDR32",
	 false, 0xffffffff, 0xffffffff, false);

static const reloc_howto_type arm64_reloc_howto_32_pcrel = HOWTO (IMAGE_REL_ARM64_REL32, 0, 4, 32, true, 0,
	 complain_overflow_bitfield,
	 NULL, "IMAGE_REL_ARM64_REL32",
	 false, 0xffffffff, 0xffffffff, true);

static const reloc_howto_type arm64_reloc_howto_branch26 = HOWTO (IMAGE_REL_ARM64_BRANCH26, 0, 4, 26, true, 0,
	 complain_overflow_bitfield,
	 coff_aarch64_branch26_reloc, "IMAGE_REL_ARM64_BRANCH26",
	 false, 0x03ffffff, 0x03ffffff, true);

static const reloc_howto_type arm64_reloc_howto_page21 = HOWTO (IMAGE_REL_ARM64_PAGEBASE_REL21, 12, 4, 21, true, 0,
	 complain_overflow_signed,
	 coff_aarch64_rel21_reloc, "IMAGE_REL_ARM64_PAGEBASE_REL21",
	 false, 0x1fffff, 0x1fffff, false);

static const reloc_howto_type arm64_reloc_howto_lo21 = HOWTO (IMAGE_REL_ARM64_REL21, 0, 4, 21, true, 0,
	 complain_overflow_signed,
	 coff_aarch64_rel21_reloc, "IMAGE_REL_ARM64_REL21",
	 false, 0x1fffff, 0x1fffff, true);

static const reloc_howto_type arm64_reloc_howto_pgoff12l = HOWTO (IMAGE_REL_ARM64_PAGEOFFSET_12L, 1, 4, 12, true, 0,
	 complain_overflow_signed,
	 coff_aarch64_po12l_reloc, "IMAGE_REL_ARM64_PAGEOFFSET_12L",
	 false, 0xffe, 0xffe, true);

static const reloc_howto_type arm64_reloc_howto_branch19 = HOWTO (IMAGE_REL_ARM64_BRANCH19, 2, 4, 19, true, 0,
	 complain_overflow_signed,
	 coff_aarch64_branch19_reloc, "IMAGE_REL_ARM64_BRANCH19",
	 false, 0x7ffff, 0x7ffff, true);

static const reloc_howto_type arm64_reloc_howto_branch14 = HOWTO (IMAGE_REL_ARM64_BRANCH14, 2, 4, 14, true, 0,
	 complain_overflow_signed,
	 coff_aarch64_branch14_reloc, "IMAGE_REL_ARM64_BRANCH14",
	 false, 0x3fff, 0x3fff, true);

static const reloc_howto_type arm64_reloc_howto_pgoff12a = HOWTO (IMAGE_REL_ARM64_PAGEOFFSET_12A, 2, 4, 12, true, 10,
	 complain_overflow_dont,
	 coff_aarch64_po12a_reloc, "IMAGE_REL_ARM64_PAGEOFFSET_12A",
	 false, 0x3ffc00, 0x3ffc00, false);

static const reloc_howto_type arm64_reloc_howto_32nb = HOWTO (IMAGE_REL_ARM64_ADDR32NB, 0, 4, 32, false, 0,
	 complain_overflow_bitfield,
	 coff_aarch64_addr32nb_reloc, "IMAGE_REL_ARM64_ADDR32NB",
	 false, 0xffffffff, 0xffffffff, false);

static const reloc_howto_type arm64_reloc_howto_secrel = HOWTO (IMAGE_REL_ARM64_SECREL, 0, 4, 32, false, 0,
	 complain_overflow_bitfield,
	 coff_aarch64_secrel_reloc, "IMAGE_REL_ARM64_SECREL",
	 false, 0xffffffff, 0xffffffff, false);

static const reloc_howto_type* const arm64_howto_table[] = {
     &arm64_reloc_howto_abs,
     &arm64_reloc_howto_64,
     &arm64_reloc_howto_32,
     &arm64_reloc_howto_32_pcrel,
     &arm64_reloc_howto_branch26,
     &arm64_reloc_howto_page21,
     &arm64_reloc_howto_lo21,
     &arm64_reloc_howto_pgoff12l,
     &arm64_reloc_howto_branch19,
     &arm64_reloc_howto_branch14,
     &arm64_reloc_howto_pgoff12a,
     &arm64_reloc_howto_32nb,
     &arm64_reloc_howto_secrel
};

#ifndef NUM_ELEM
#define NUM_ELEM(a) ((sizeof (a)) / sizeof ((a)[0]))
#endif

#define NUM_RELOCS NUM_ELEM (arm64_howto_table)

#define coff_bfd_reloc_type_lookup		coff_aarch64_reloc_type_lookup
#define coff_bfd_reloc_name_lookup		coff_aarch64_reloc_name_lookup

static reloc_howto_type *
coff_aarch64_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code)
{
  switch (code)
  {
  case BFD_RELOC_64:
    return &arm64_reloc_howto_64;
  case BFD_RELOC_32:
    return &arm64_reloc_howto_32;
  case BFD_RELOC_32_PCREL:
    return &arm64_reloc_howto_32_pcrel;
  case BFD_RELOC_AARCH64_CALL26:
  case BFD_RELOC_AARCH64_JUMP26:
    return &arm64_reloc_howto_branch26;
  case BFD_RELOC_AARCH64_ADR_HI21_PCREL:
  case BFD_RELOC_AARCH64_ADR_HI21_NC_PCREL:
    return &arm64_reloc_howto_page21;
  case BFD_RELOC_AARCH64_TSTBR14:
    return &arm64_reloc_howto_branch14;
  case BFD_RELOC_AARCH64_ADR_LO21_PCREL:
    return &arm64_reloc_howto_lo21;
  case BFD_RELOC_AARCH64_ADD_LO12:
    return &arm64_reloc_howto_pgoff12a;
  case BFD_RELOC_AARCH64_LDST8_LO12:
  case BFD_RELOC_AARCH64_LDST16_LO12:
  case BFD_RELOC_AARCH64_LDST32_LO12:
  case BFD_RELOC_AARCH64_LDST64_LO12:
  case BFD_RELOC_AARCH64_LDST128_LO12:
    return &arm64_reloc_howto_pgoff12l;
  case BFD_RELOC_AARCH64_BRANCH19:
    return &arm64_reloc_howto_branch19;
  case BFD_RELOC_RVA:
    return &arm64_reloc_howto_32nb;
  case BFD_RELOC_32_SECREL:
    return &arm64_reloc_howto_secrel;
  default:
    BFD_FAIL ();
    return NULL;
  }

  return NULL;
}

static reloc_howto_type *
coff_aarch64_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    const char *r_name)
{
	unsigned int i;

	for (i = 0; i < NUM_RELOCS; i++)
	  if (arm64_howto_table[i]->name != NULL
	    && strcasecmp (arm64_howto_table[i]->name, r_name) == 0)
	    return arm64_howto_table[i];

  return NULL;
}

#define COFF_DEFAULT_SECTION_ALIGNMENT_POWER  2
#define COFF_PAGE_SIZE			      0x1000

static reloc_howto_type *
coff_aarch64_rtype_lookup (unsigned int code)
{
  switch (code)
  {
    case IMAGE_REL_ARM64_ABSOLUTE:
      return &arm64_reloc_howto_abs;
    case IMAGE_REL_ARM64_ADDR64:
      return &arm64_reloc_howto_64;
    case IMAGE_REL_ARM64_ADDR32:
      return &arm64_reloc_howto_32;
    case IMAGE_REL_ARM64_REL32:
      return &arm64_reloc_howto_32_pcrel;
    case IMAGE_REL_ARM64_BRANCH26:
      return &arm64_reloc_howto_branch26;
    case IMAGE_REL_ARM64_PAGEBASE_REL21:
      return &arm64_reloc_howto_page21;
    case IMAGE_REL_ARM64_REL21:
      return &arm64_reloc_howto_lo21;
    case IMAGE_REL_ARM64_PAGEOFFSET_12L:
      return &arm64_reloc_howto_pgoff12l;
    case IMAGE_REL_ARM64_BRANCH19:
      return &arm64_reloc_howto_branch19;
    case IMAGE_REL_ARM64_BRANCH14:
      return &arm64_reloc_howto_branch14;
    case IMAGE_REL_ARM64_PAGEOFFSET_12A:
      return &arm64_reloc_howto_pgoff12a;
    case IMAGE_REL_ARM64_ADDR32NB:
      return &arm64_reloc_howto_32nb;
    case IMAGE_REL_ARM64_SECREL:
      return &arm64_reloc_howto_secrel;
    default:
      BFD_FAIL ();
      return NULL;
  }

  return NULL;
}

#define RTYPE2HOWTO(cache_ptr, dst)				\
  ((cache_ptr)->howto =	coff_aarch64_rtype_lookup((dst)->r_type))

#define SELECT_RELOC(x,howto) { (x).r_type = (howto)->type; }

#ifndef bfd_pe_print_pdata
#define bfd_pe_print_pdata      NULL
#endif

#ifdef COFF_WITH_PE
/* Return TRUE if this relocation should
   appear in the output .reloc section.  */

static bool
in_reloc_p (bfd * abfd ATTRIBUTE_UNUSED,
            reloc_howto_type * howto)
{
  return !howto->pc_relative;
}
#endif

static bool
coff_pe_aarch64_relocate_section (bfd *output_bfd,
				  struct bfd_link_info *info,
				  bfd *input_bfd,
				  asection *input_section,
				  bfd_byte *contents,
				  struct internal_reloc *relocs,
				  struct internal_syment *syms,
				  asection **sections)
{
  struct internal_reloc *rel;
  struct internal_reloc *relend;

  if (bfd_link_relocatable (info))
    return true;

  rel = relocs;
  relend = rel + input_section->reloc_count;

  /* The addend for a relocation is stored in the immediate bits of each
     opcode.  So for each relocation, we need to extract the immediate value,
     use this to calculate what it should be for the symbol, and rewrite the
     opcode into the section stream.  */

  for (; rel < relend; rel++)
    {
      long symndx;
      struct coff_link_hash_entry *h;
      bfd_vma sym_value;
      asection *sec = NULL;
      uint64_t dest_vma;

      /* skip trivial relocations */
      if (rel->r_type == IMAGE_REL_ARM64_ADDR32
	  || rel->r_type == IMAGE_REL_ARM64_ADDR64
	  || rel->r_type == IMAGE_REL_ARM64_ABSOLUTE)
	continue;

      symndx = rel->r_symndx;
      sym_value = syms[symndx].n_value;

      h = obj_coff_sym_hashes (input_bfd)[symndx];

      if (h && h->root.type == bfd_link_hash_defined)
	{
	  sec = h->root.u.def.section;
	  sym_value = h->root.u.def.value;
	}
      else
	{
	  sec = sections[symndx];
	}

      if (!sec)
	continue;

      if (bfd_is_und_section (sec))
	continue;

      if (discarded_section (sec))
	continue;

      dest_vma = sec->output_section->vma + sec->output_offset + sym_value;

      if (symndx < 0
	  || (unsigned long) symndx >= obj_raw_syment_count (input_bfd))
	continue;

      switch (rel->r_type)
	{
	case IMAGE_REL_ARM64_ADDR32NB:
	  {
	    uint64_t val;
	    int32_t addend;

	    addend = bfd_getl32 (contents + rel->r_vaddr);

	    dest_vma += addend;

	    val = dest_vma;
	    val -= pe_data (output_bfd)->pe_opthdr.ImageBase;

	    if (val > 0xffffffff)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_ADDR32NB", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    bfd_putl32 (val, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_BRANCH26:
	  {
	    uint64_t cur_vma;
	    uint32_t opcode;
	    int64_t addend, val;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = (opcode & 0x3ffffff) << 2;

	    if (addend & 0x8000000)
	      addend |= 0xfffffffff0000000;

	    dest_vma += addend;
	    cur_vma = input_section->output_section->vma
		      + input_section->output_offset
		      + rel->r_vaddr;

	    val = (dest_vma >> 2) - (cur_vma >> 2);

	    if (val > 0x1ffffff || val < -0x2000000)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_BRANCH26", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    opcode &= 0xfc000000;
	    opcode |= val & 0x3ffffff;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_BRANCH19:
	  {
	    uint64_t cur_vma;
	    uint32_t opcode;
	    int64_t addend, val;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = (opcode & 0xffffe0) >> 3;

	    if (addend & 0x100000)
	      addend |= 0xffffffffffe00000;

	    dest_vma += addend;
	    cur_vma = input_section->output_section->vma
		      + input_section->output_offset
		      + rel->r_vaddr;

	    val = (dest_vma >> 2) - (cur_vma >> 2);

	    if (val > 0x3ffff || val < -0x40000)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_BRANCH19", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    opcode &= 0xff00001f;
	    opcode |= (val & 0x7ffff) << 5;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_BRANCH14:
	  {
	    uint64_t cur_vma;
	    uint32_t opcode;
	    int64_t addend, val;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = (opcode & 0x7ffe0) >> 3;

	    if (addend & 0x8000)
	      addend |= 0xffffffffffff0000;

	    dest_vma += addend;
	    cur_vma = input_section->output_section->vma
		      + input_section->output_offset
		      + rel->r_vaddr;

	    val = (dest_vma >> 2) - (cur_vma >> 2);

	    if (val > 0x1fff || val < -0x2000)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_BRANCH14", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    opcode &= 0xfff8001f;
	    opcode |= (val & 0x3fff) << 5;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_PAGEBASE_REL21:
	  {
	    uint64_t cur_vma;
	    uint32_t opcode;
	    int64_t addend, val;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = ((opcode & 0xffffe0) >> 3)
		     | ((opcode & 0x60000000) >> 29);

	    if (addend & 0x100000)
	      addend |= 0xffffffffffe00000;

	    dest_vma += addend;
	    cur_vma = input_section->output_section->vma
		      + input_section->output_offset
		      + rel->r_vaddr;

	    val = (dest_vma >> 12) - (cur_vma >> 12);

	    if (val > 0xfffff || val < -0x100000)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_PAGEBASE_REL21", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    opcode &= 0x9f00001f;
	    opcode |= (val & 0x3) << 29;
	    opcode |= (val & 0x1ffffc) << 3;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_REL21:
	  {
	    uint64_t cur_vma;
	    uint32_t opcode;
	    int64_t addend, val;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = ((opcode & 0xffffe0) >> 3)
		     | ((opcode & 0x60000000) >> 29);

	    if (addend & 0x100000)
	      addend |= 0xffffffffffe00000;

	    dest_vma += addend;
	    cur_vma = input_section->output_section->vma
		      + input_section->output_offset
		      + rel->r_vaddr;

	    val = dest_vma - cur_vma;

	    if (val > 0xfffff || val < -0x100000)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_REL21", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    opcode &= 0x9f00001f;
	    opcode |= (val & 0x3) << 29;
	    opcode |= (val & 0x1ffffc) << 3;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_PAGEOFFSET_12L:
	  {
	    uint32_t opcode, val;
	    uint8_t shift;
	    int32_t addend;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = (opcode & 0x3ffc00) >> 10;

	    if ((opcode & 0xff800000) == 0x3d800000)
	      {
		/* LDR / STR with q register */
		shift = 4;
	      }
	    else
	      {
		/* top two bits represent how much addend should be shifted */
		shift = opcode >> 30;
	      }

	    addend <<= shift;

	    dest_vma += addend;

	    /* only interested in bottom 12 bits */
	    val = dest_vma & 0xfff;

	    if (val & ((1 << shift) - 1))
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_PAGEOFFSET_12L", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    val >>= shift;

	    opcode &= 0xffc003ff;
	    opcode |= val << 10;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_PAGEOFFSET_12A:
	  {
	    uint32_t opcode, val;
	    int32_t addend;

	    opcode = bfd_getl32 (contents + rel->r_vaddr);

	    addend = (opcode & 0x3ffc00) >> 10;

	    dest_vma += addend;

	    /* only interested in bottom 12 bits */
	    val = dest_vma & 0xfff;

	    opcode &= 0xffc003ff;
	    opcode |= val << 10;

	    bfd_putl32 (opcode, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	case IMAGE_REL_ARM64_SECREL:
	  {
	    uint64_t val;
	    int32_t addend;

	    addend = bfd_getl32 (contents + rel->r_vaddr);

	    val = sec->output_offset + sym_value + addend;

	    if (val > 0xffffffff)
	      (*info->callbacks->reloc_overflow)
		(info, h ? &h->root : NULL, syms[symndx]._n._n_name,
		"IMAGE_REL_ARM64_SECREL", addend, input_bfd,
		input_section, rel->r_vaddr - input_section->vma);

	    bfd_putl32 (val, contents + rel->r_vaddr);
	    rel->r_type = IMAGE_REL_ARM64_ABSOLUTE;

	    break;
	  }

	default:
	  info->callbacks->einfo (_("%F%P: Unhandled relocation type %u\n"),
				  rel->r_type);
	  BFD_FAIL ();
	  return false;
	}
    }

  return _bfd_coff_generic_relocate_section (output_bfd, info, input_bfd,
					     input_section, contents,
					     relocs, syms, sections);
}

#define coff_relocate_section coff_pe_aarch64_relocate_section

#include "coffcode.h"

/* Prevent assertion in md_apply_fix by forcing use_rela_p on for new
   sections.  */
static bool
coff_aarch64_new_section_hook (bfd *abfd, asection *section)
{
  if (!coff_new_section_hook (abfd, section))
    return false;

  section->use_rela_p = 1;

  return true;
}

#define coff_aarch64_close_and_cleanup coff_close_and_cleanup
#define coff_aarch64_bfd_free_cached_info coff_bfd_free_cached_info
#define coff_aarch64_get_section_contents coff_get_section_contents
#define coff_aarch64_get_section_contents_in_window coff_get_section_contents_in_window

/* Target vectors.  */
const bfd_target
#ifdef TARGET_SYM
  TARGET_SYM =
#else
# error "target symbol name not specified"
#endif
{
#ifdef TARGET_NAME
  TARGET_NAME,
#else
# error "target name not specified"
#endif
  bfd_target_coff_flavour,
  BFD_ENDIAN_LITTLE,		/* Data byte order is little.  */
  BFD_ENDIAN_LITTLE,		/* Header byte order is little.  */

  (HAS_RELOC | EXEC_P		/* Object flags.  */
   | HAS_LINENO | HAS_DEBUG
   | HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED | BFD_COMPRESS | BFD_DECOMPRESS),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC /* Section flags.  */
#if defined(COFF_WITH_PE)
   | SEC_LINK_ONCE | SEC_LINK_DUPLICATES | SEC_READONLY | SEC_DEBUGGING
#endif
   | SEC_CODE | SEC_DATA | SEC_EXCLUDE ),

#ifdef TARGET_UNDERSCORE
  TARGET_UNDERSCORE,		/* Leading underscore.  */
#else
  0,				/* Leading underscore.  */
#endif
  '/',				/* Ar_pad_char.  */
  15,				/* Ar_max_namelen.  */
  0,				/* match priority.  */
  TARGET_KEEP_UNUSED_SECTION_SYMBOLS, /* keep unused section symbols.  */

  /* Data conversion functions.  */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* Data.  */
  /* Header conversion functions.  */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
  bfd_getl32, bfd_getl_signed_32, bfd_putl32,
  bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* Hdrs.  */

  /* Note that we allow an object file to be treated as a core file as well.  */
  {				/* bfd_check_format.  */
    _bfd_dummy_target,
    coff_object_p,
    bfd_generic_archive_p,
    coff_object_p
  },
  {				/* bfd_set_format.  */
    _bfd_bool_bfd_false_error,
    coff_mkobject,
    _bfd_generic_mkarchive,
    _bfd_bool_bfd_false_error
  },
  {				/* bfd_write_contents.  */
    _bfd_bool_bfd_false_error,
    coff_write_object_contents,
    _bfd_write_archive_contents,
    _bfd_bool_bfd_false_error
  },

  BFD_JUMP_TABLE_GENERIC (coff_aarch64),
  BFD_JUMP_TABLE_COPY (coff),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_archive_coff),
  BFD_JUMP_TABLE_SYMBOLS (coff),
  BFD_JUMP_TABLE_RELOCS (coff),
  BFD_JUMP_TABLE_WRITE (coff),
  BFD_JUMP_TABLE_LINK (coff),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  COFF_SWAP_TABLE
};
