/* Matsushita 10300 specific support for 32-bit ELF
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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

static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static void mn10300_info_to_howto
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));
static boolean mn10300_elf_relax_delete_bytes
  PARAMS ((bfd *, asection *, bfd_vma, int));
static boolean mn10300_elf_symbol_address_p
  PARAMS ((bfd *, asection *, Elf32_External_Sym *, bfd_vma));



/* We have to use RELA instructions since md_apply_fix3 in the assembler
   does absolutely nothing.  */
#define USE_RELA

enum reloc_type
{
  R_MN10300_NONE = 0,
  R_MN10300_32,
  R_MN10300_16,
  R_MN10300_8,
  R_MN10300_PCREL32,
  R_MN10300_PCREL16,
  R_MN10300_PCREL8,
  R_MN10300_MAX
};

static reloc_howto_type elf_mn10300_howto_table[] =
{
  /* Dummy relocation.  Does nothing.  */
  HOWTO (R_MN10300_NONE,
	 0,
	 2,
	 16,
	 false,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_NONE",
	 false,
	 0,
	 0,
	 false),
  /* Standard 32 bit reloc.  */
  HOWTO (R_MN10300_32,
	 0,
	 2,
	 32,
	 false,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_32",
	 false,
	 0xffffffff,
	 0xffffffff,
	 false),
  /* Standard 16 bit reloc.  */
  HOWTO (R_MN10300_16,
	 0,
	 1,
	 16,
	 false,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_16",
	 false,
	 0xffff,
	 0xffff,
	 false),
  /* Standard 8 bit reloc.  */
  HOWTO (R_MN10300_8,
	 0,
	 0,
	 8,
	 false,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_8",
	 false,
	 0xff,
	 0xff,
	 false),
  /* Standard 32bit pc-relative reloc.  */
  HOWTO (R_MN10300_PCREL32,
	 0,
	 2,
	 32,
	 true,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_PCREL32",
	 false,
	 0xffffffff,
	 0xffffffff,
	 true),
  /* Standard 16bit pc-relative reloc.  */
  HOWTO (R_MN10300_PCREL16,
	 0,
	 1,
	 16,
	 true,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_PCREL16",
	 false,
	 0xffff,
	 0xffff,
	 true),
  /* Standard 8 pc-relative reloc.  */
  HOWTO (R_MN10300_PCREL8,
	 0,
	 0,
	 8,
	 true,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_MN10300_PCREL8",
	 false,
	 0xff,
	 0xff,
	 true),
};

struct mn10300_reloc_map
{
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static const struct mn10300_reloc_map mn10300_reloc_map[] =
{
  { BFD_RELOC_NONE, R_MN10300_NONE, },
  { BFD_RELOC_32, R_MN10300_32, },
  { BFD_RELOC_16, R_MN10300_16, },
  { BFD_RELOC_8, R_MN10300_8, },
  { BFD_RELOC_32_PCREL, R_MN10300_PCREL32, },
  { BFD_RELOC_16_PCREL, R_MN10300_PCREL16, },
  { BFD_RELOC_8_PCREL, R_MN10300_PCREL8, },
};

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  unsigned int i;

  for (i = 0;
       i < sizeof (mn10300_reloc_map) / sizeof (struct mn10300_reloc_map);
       i++)
    {
      if (mn10300_reloc_map[i].bfd_reloc_val == code)
	return &elf_mn10300_howto_table[mn10300_reloc_map[i].elf_reloc_val];
    }

  return NULL;
}

/* Set the howto pointer for an MN10300 ELF reloc.  */

static void
mn10300_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rela *dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_MN10300_MAX);
  cache_ptr->howto = &elf_mn10300_howto_table[r_type];
}

/* Perform a relocation as part of a final link.  */
static bfd_reloc_status_type
mn10300_elf_final_link_relocate (howto, input_bfd, output_bfd,
				 input_section, contents, offset, value,
				 addend, info, sym_sec, is_local)
     reloc_howto_type *howto;
     bfd *input_bfd;
     bfd *output_bfd;
     asection *input_section;
     bfd_byte *contents;
     bfd_vma offset;
     bfd_vma value;
     bfd_vma addend;
     struct bfd_link_info *info;
     asection *sym_sec;
     int is_local;
{
  unsigned long r_type = howto->type;
  bfd_byte *hit_data = contents + offset;

  switch (r_type)
    {

    case R_MN10300_NONE:
      return bfd_reloc_ok;

    case R_MN10300_32:
      value += addend;
      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_MN10300_16:
      value += addend;

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_MN10300_8:
      value += addend;

      if ((long)value > 0x7fff || (long)value < -0x8000)
	return bfd_reloc_overflow;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_MN10300_PCREL8:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
      value += addend;

      if ((long)value > 0xff || (long)value < -0x100)
        return bfd_reloc_overflow;

      bfd_put_8 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_MN10300_PCREL16:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
      value += addend;

      if ((long)value > 0xffff || (long)value < -0x10000)
        return bfd_reloc_overflow;

      bfd_put_16 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    case R_MN10300_PCREL32:
      value -= (input_section->output_section->vma
		+ input_section->output_offset);
      value -= offset;
      value += addend;

      bfd_put_32 (input_bfd, value, hit_data);
      return bfd_reloc_ok;

    default:
      return bfd_reloc_notsupported;
    }
}


/* Relocate an MN10300 ELF section.  */
static boolean
mn10300_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			      contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);
      howto = elf_mn10300_howto_table + r_type;

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
             anything, unless the reloc is against a section symbol,
             in which case we have to adjust according to where the
             section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections[r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset)))
		return false;
	      relocation = 0;
	    }
	}

      r = mn10300_elf_final_link_relocate (howto, input_bfd, output_bfd,
					   input_section,
					   contents, rel->r_offset,
					   relocation, rel->r_addend,
					   info, sec, h == NULL);

      if (r != bfd_reloc_ok)
	{
	  const char *name;
	  const char *msg = (const char *)0;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (! ((*info->callbacks->reloc_overflow)
		     (info, name, howto->name, (bfd_vma) 0,
		      input_bfd, input_section, rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_undefined:
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, name, input_bfd, input_section,
		      rel->r_offset)))
		return false;
	      break;

	    case bfd_reloc_outofrange:
	      msg = "internal error: out of range error";
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = "internal error: unsupported relocation error";
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = "internal error: dangerous error";
	      goto common_error;

	    default:
	      msg = "internal error: unknown error";
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, name, input_bfd, input_section,
		     rel->r_offset)))
		return false;
	      break;
	    }
	}
    }

  return true;
}

/* This function handles relaxing for the mn10300.

   There's quite a few relaxing opportunites available on the mn10300:

	* calls:32 -> calls:16 					   2 bytes
	- call:32  -> call:16					   2 bytes

	* jmp:24 -> jmp:16					   2 bytes
	* jmp:16 -> bra:8					   1 byte

		* If the previous instruction is a conditional branch
		around the jump/bra, we may be able to reverse its condition
		and change its target to the jump's target.  The jump/bra
		can then be deleted.				   2 bytes

	XXX These are mn10200 relaxing things
	* mov abs24 -> mov abs16	2 byte savings

	* Most instructions which accept imm24 can relax to imm16  2 bytes
	- Most instructions which accept imm16 can relax to imm8   1 byte

	* Most instructions which accept d24 can relax to d16	   2 bytes
	- Most instructions which accept d16 can relax to d8	   1 byte

	abs24, imm24, d24 all look the same at the reloc level.  It
	might make the code simpler if we had different relocs for
	the various relaxable operand types.
   
	We don't handle imm16->imm8 or d16->d8 as they're very rare
	and somewhat more difficult to support.  */

static boolean 
mn10300_elf_relax_section (abfd, sec, link_info, again)
     bfd *abfd;
     asection *sec;
     struct bfd_link_info *link_info;
     boolean *again;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs;
  Elf_Internal_Rela *free_relocs = NULL;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  bfd_byte *free_contents = NULL;
  Elf32_External_Sym *extsyms = NULL;
  Elf32_External_Sym *free_extsyms = NULL;

  /* Assume nothing changes.  */
  *again = false;

  /* We don't have to do anything for a relocateable link, if
     this section does not have relocs, or if this is not a
     code section.  */
  if (link_info->relocateable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0
      || (sec->flags & SEC_CODE) == 0)
    return true;

  /* If this is the first time we have been called for this section,
     initialize the cooked size.  */
  if (sec->_cooked_size == 0)
    sec->_cooked_size = sec->_raw_size;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  /* Get a copy of the native relocations.  */
  internal_relocs = (_bfd_elf32_link_read_relocs
		     (abfd, sec, (PTR) NULL, (Elf_Internal_Rela *) NULL,
		      link_info->keep_memory));
  if (internal_relocs == NULL)
    goto error_return;
  if (! link_info->keep_memory)
    free_relocs = internal_relocs;

  /* Walk through them looking for relaxing opportunities.  */
  irelend = internal_relocs + sec->reloc_count;
  for (irel = internal_relocs; irel < irelend; irel++)
    {
      bfd_vma symval;

      /* If this isn't something that can be relaxed, then ignore
	 this reloc.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_NONE
	  || ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_8
	  || ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_MAX)
	continue;

      /* Get the section contents if we haven't done so already.  */
      if (contents == NULL)
	{
	  /* Get cached copy if it exists.  */
	  if (elf_section_data (sec)->this_hdr.contents != NULL)
	    contents = elf_section_data (sec)->this_hdr.contents;
	  else
	    {
	      /* Go get them off disk.  */
	      contents = (bfd_byte *) bfd_malloc (sec->_raw_size);
	      if (contents == NULL)
		goto error_return;
	      free_contents = contents;

	      if (! bfd_get_section_contents (abfd, sec, contents,
					      (file_ptr) 0, sec->_raw_size))
		goto error_return;
	    }
	}

      /* Read the local symbols if we haven't done so already.  */
      if (extsyms == NULL)
	{
	  /* Get cached copy if it exists.  */
	  if (symtab_hdr->contents != NULL)
	    extsyms = (Elf32_External_Sym *) symtab_hdr->contents;
	  else
	    {
	      /* Go get them off disk.  */
	      extsyms = ((Elf32_External_Sym *)
			 bfd_malloc (symtab_hdr->sh_info
				     * sizeof (Elf32_External_Sym)));
	      if (extsyms == NULL)
		goto error_return;
	      free_extsyms = extsyms;
	      if (bfd_seek (abfd, symtab_hdr->sh_offset, SEEK_SET) != 0
		  || (bfd_read (extsyms, sizeof (Elf32_External_Sym),
				symtab_hdr->sh_info, abfd)
		      != (symtab_hdr->sh_info * sizeof (Elf32_External_Sym))))
		goto error_return;
	    }
	}

      /* Get the value of the symbol referred to by the reloc.  */
      if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
	{
	  Elf_Internal_Sym isym;
	  asection *sym_sec;

	  /* A local symbol.  */
	  bfd_elf32_swap_symbol_in (abfd,
				    extsyms + ELF32_R_SYM (irel->r_info),
				    &isym);

	  sym_sec = bfd_section_from_elf_index (abfd, isym.st_shndx);
	  symval = (isym.st_value
		    + sym_sec->output_section->vma
		    + sym_sec->output_offset);
	}
      else
	{
	  unsigned long indx;
	  struct elf_link_hash_entry *h;

	  /* An external symbol.  */
	  indx = ELF32_R_SYM (irel->r_info) - symtab_hdr->sh_info;
	  h = elf_sym_hashes (abfd)[indx];
	  BFD_ASSERT (h != NULL);
	  if (h->root.type != bfd_link_hash_defined
	      && h->root.type != bfd_link_hash_defweak)
	    {
	      /* This appears to be a reference to an undefined
                 symbol.  Just ignore it--it will be caught by the
                 regular reloc processing.  */
	      continue;
	    }

	  symval = (h->root.u.def.value
		    + h->root.u.def.section->output_section->vma
		    + h->root.u.def.section->output_offset);
	}

      /* For simplicity of coding, we are going to modify the section
	 contents, the section relocs, and the BFD symbol table.  We
	 must tell the rest of the code not to free up this
	 information.  It would be possible to instead create a table
	 of changes which have to be made, as is done in coff-mips.c;
	 that would be more work, but would require less memory when
	 the linker is run.  */


      /* Try to turn a 32bit pc-relative branch/call into a 16bit pc-relative
	 branch/call.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_PCREL32)
	{
	  bfd_vma value = symval;

	  /* Deal with pc-relative gunk.  */
	  value -= (sec->output_section->vma + sec->output_offset);
	  value -= irel->r_offset;
	  value += irel->r_addend;

	  /* See if the value will fit in 16 bits, note the high value is
	     0x7fff + 2 as the target will be two bytes closer if we are
	     able to relax.  */
	  if ((long)value < 0x8001 && (long)value > -0x8000)
	    {
	      unsigned char code;

	      /* Get the opcode.  */
	      code = bfd_get_8 (abfd, contents + irel->r_offset - 1);

	      if (code != 0xdc && code != 0xff)
		continue;

	      /* Note that we've changed the relocs, section contents, etc.  */
	      elf_section_data (sec)->relocs = internal_relocs;
	      free_relocs = NULL;

	      elf_section_data (sec)->this_hdr.contents = contents;
	      free_contents = NULL;

	      symtab_hdr->contents = (bfd_byte *) extsyms;
	      free_extsyms = NULL;

	      /* Fix the opcode.  */
	      if (code == 0xdc)
		bfd_put_8 (abfd, 0xcc, contents + irel->r_offset - 1);
	      else if (code == 0xff)
		bfd_put_8 (abfd, 0xfa, contents + irel->r_offset - 2);

	      /* Fix the relocation's type.  */
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
					   R_MN10300_PCREL16);

	      /* Delete two bytes of data.  */
	      if (!mn10300_elf_relax_delete_bytes (abfd, sec,
						   irel->r_offset + 1, 2))
		goto error_return;

	      /* That will change things, so, we should relax again.
		 Note that this is not required, and it may be slow.  */
	      *again = true;
	    }
	}

      /* Try to turn a 16bit pc-relative branch into a 8bit pc-relative
	 branch.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_PCREL16)
	{
	  bfd_vma value = symval;

	  /* Deal with pc-relative gunk.  */
	  value -= (sec->output_section->vma + sec->output_offset);
	  value -= irel->r_offset;
	  value += irel->r_addend;

	  /* See if the value will fit in 8 bits, note the high value is
	     0x7f + 1 as the target will be one bytes closer if we are
	     able to relax.  */
	  if ((long)value < 0x80 && (long)value > -0x80)
	    {
	      unsigned char code;

	      /* Get the opcode.  */
	      code = bfd_get_8 (abfd, contents + irel->r_offset - 1);

	      if (code != 0xcc)
		continue;

	      /* Note that we've changed the relocs, section contents, etc.  */
	      elf_section_data (sec)->relocs = internal_relocs;
	      free_relocs = NULL;

	      elf_section_data (sec)->this_hdr.contents = contents;
	      free_contents = NULL;

	      symtab_hdr->contents = (bfd_byte *) extsyms;
	      free_extsyms = NULL;

	      /* Fix the opcode.  */
	      bfd_put_8 (abfd, 0xca, contents + irel->r_offset - 1);

	      /* Fix the relocation's type.  */
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
					   R_MN10300_PCREL8);

	      /* Delete one byte of data.  */
	      if (!mn10300_elf_relax_delete_bytes (abfd, sec,
						   irel->r_offset + 1, 1))
		goto error_return;

	      /* That will change things, so, we should relax again.
		 Note that this is not required, and it may be slow.  */
	      *again = true;
	    }
	}

      /* Try to eliminate an unconditional 8 bit pc-relative branch
	 which immediately follows a conditional 8 bit pc-relative
	 branch around the unconditional branch.

	    original:		new:
	    bCC lab1		bCC' lab2
	    bra lab2
	   lab1:	       lab1:


	 This happens when the bCC can't reach lab2 at assembly time,
	 but due to other relaxations it can reach at link time.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_PCREL8)
	{
	  Elf_Internal_Rela *nrel;
	  bfd_vma value = symval;
	  unsigned char code;

	  /* Deal with pc-relative gunk.  */
	  value -= (sec->output_section->vma + sec->output_offset);
	  value -= irel->r_offset;
	  value += irel->r_addend;

	  /* Do nothing if this reloc is the last byte in the section.  */
	  if (irel->r_offset == sec->_cooked_size)
	    continue;

	  /* See if the next instruction is an unconditional pc-relative
	     branch, more often than not this test will fail, so we
	     test it first to speed things up.  */
	  code = bfd_get_8 (abfd, contents + irel->r_offset + 1);
	  if (code != 0xca)
	    continue;

	  /* Also make sure the next relocation applies to the next
	     instruction and that it's a pc-relative 8 bit branch.  */
	  nrel = irel + 1;
	  if (nrel == irelend
	      || irel->r_offset + 2 != nrel->r_offset
	      || ELF32_R_TYPE (nrel->r_info) != (int) R_MN10300_PCREL8)
	    continue;

	  /* Make sure our destination immediately follows the
	     unconditional branch.  */
	  if (symval != (sec->output_section->vma + sec->output_offset
			 + irel->r_offset + 3))
	    continue;

	  /* Now make sure we are a conditional branch.  This may not
	     be necessary, but why take the chance. 

	     Note these checks assume that R_MN10300_PCREL8 relocs
	     only occur on bCC and bCCx insns.  If they occured
	     elsewhere, we'd need to know the start of this insn
	     for this check to be accurate.  */
	  code = bfd_get_8 (abfd, contents + irel->r_offset - 1);
	  if (code != 0xc0 && code != 0xc1 && code != 0xc2
	      && code != 0xc3 && code != 0xc4 && code != 0xc5
	      && code != 0xc6 && code != 0xc7 && code != 0xc8
	      && code != 0xc9 && code != 0xe8 && code != 0xe9
	      && code != 0xea && code != 0xeb)
	    continue;

	  /* We also have to be sure there is no symbol/label
	     at the unconditional branch.  */
	  if (mn10300_elf_symbol_address_p (abfd, sec, extsyms,
					    irel->r_offset + 1))
	    continue;

	  /* Note that we've changed the relocs, section contents, etc.  */
	  elf_section_data (sec)->relocs = internal_relocs;
	  free_relocs = NULL;

	  elf_section_data (sec)->this_hdr.contents = contents;
	  free_contents = NULL;

	  symtab_hdr->contents = (bfd_byte *) extsyms;
	  free_extsyms = NULL;

	  /* Reverse the condition of the first branch.  */
	  switch (code)
	    {
	      case 0xc8:
		code = 0xc9;
		break;
	      case 0xc9:
		code = 0xc8;
		break;
	      case 0xc0:
		code = 0xc2;
		break;
	      case 0xc2:
		code = 0xc0;
		break;
	      case 0xc3:
		code = 0xc1;
		break;
	      case 0xc1:
		code = 0xc3;
		break;
	      case 0xc4:
		code = 0xc6;
		break;
	      case 0xc6:
		code = 0xc4;
		break;
	      case 0xc7:
		code = 0xc5;
		break;
	      case 0xc5:
		code = 0xc7;
		break;
	      case 0xe8:
		code = 0xe9;
		break;
	      case 0x9d:
		code = 0xe8;
		break;
	      case 0xea:
		code = 0xeb;
		break;
	      case 0xeb:
		code = 0xea;
		break;
	    }
	  bfd_put_8 (abfd, code, contents + irel->r_offset - 1);
	  
	  /* Set the reloc type and symbol for the first branch
	     from the second branch.  */
	  irel->r_info = nrel->r_info;

	  /* Make the reloc for the second branch a null reloc.  */
	  nrel->r_info = ELF32_R_INFO (ELF32_R_SYM (nrel->r_info),
				       R_MN10300_NONE);

	  /* Delete two bytes of data.  */
	  if (!mn10300_elf_relax_delete_bytes (abfd, sec,
					       irel->r_offset + 1, 2))
	    goto error_return;

	  /* That will change things, so, we should relax again.
	     Note that this is not required, and it may be slow.  */
	  *again = true;
	}

      /* Try to turn a 32bit immediate, displacement or absolute address
	 into a 16bit immediate, displacement or absolute address.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MN10300_32)
	{
	  bfd_vma value = symval;

	  /* See if the value will fit in 16 bits. 
	     We allow any 16bit match here.  We prune those we can't
	     handle below.  */
	  if ((long)value < 0x7fff && (long)value > -0x8000)
	    {
	      unsigned char code;

	      /* Most insns which have 32bit operands are 6 bytes long;
		 exceptions are pcrel insns and bit insns.
		 
		 We handle pcrel insns above.  We don't bother trying
		 to handle the bit insns here.

		 The first byte of the remaining insns will be 0xfc.  */

	      /* Get the first opcode.  */
	      code = bfd_get_8 (abfd, contents + irel->r_offset - 2);

	      if (code != 0xfc)
		continue;

	      /* Get the second opcode.  */
	      code = bfd_get_8 (abfd, contents + irel->r_offset - 1);

	      if ((code & 0xf0) < 0x80)
		switch (code & 0xf0)
		  {
		  /* mov (d32,am),dn   -> mov (d32,am),dn
		     mov dm,(d32,am)   -> mov dn,(d32,am)
		     mov (d32,am),an   -> mov (d32,am),an
		     mov dm,(d32,am)   -> mov dn,(d32,am)
		     movbu (d32,am),dn -> movbu (d32,am),dn
		     movbu dm,(d32,am) -> movbu dn,(d32,am)
		     movhu (d32,am),dn -> movhu (d32,am),dn
		     movhu dm,(d32,am) -> movhu dn,(d32,am) */
		  case 0x00:
		  case 0x10:
		  case 0x20:
		  case 0x30:
		  case 0x40:
		  case 0x50:
		  case 0x60:
		  case 0x70:
		    /* Not safe if the high bit is on as relaxing may
		       move the value out of high mem and thus not fit
		       in a signed 16bit value.  */
		    if (code == 0xcc
			&& (value & 0x8000))
		      continue;

		    /* Note that we've changed the relocation contents, etc.  */
		    elf_section_data (sec)->relocs = internal_relocs;
		    free_relocs = NULL;

		    elf_section_data (sec)->this_hdr.contents = contents;
		    free_contents = NULL;

		    symtab_hdr->contents = (bfd_byte *) extsyms;
		    free_extsyms = NULL;

		    /* Fix the opcode.  */
		    bfd_put_8 (abfd, 0xfa, contents + irel->r_offset - 2);
		    bfd_put_8 (abfd, code, contents + irel->r_offset - 1);

		    /* Fix the relocation's type.  */
		    irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		    /* Delete two bytes of data.  */
		    if (!mn10300_elf_relax_delete_bytes (abfd, sec,
							 irel->r_offset + 2, 2))
		      goto error_return;

		    /* That will change things, so, we should relax again.
		       Note that this is not required, and it may be slow.  */
		    *again = true;
		    break;
		  }
	      else if ((code & 0xf0) == 0x80
		       || (code & 0xf0) == 0x90)
	        switch (code & 0xf3)
		  {
		  /* mov dn,(abs32)   -> mov dn,(abs16)
		     movbu dn,(abs32) -> movbu dn,(abs16)
		     movhu dn,(abs32) -> movhu dn,(abs16)  */
		  case 0x81:
		  case 0x82:
		  case 0x83:
		    /* Note that we've changed the relocation contents, etc.  */
		    elf_section_data (sec)->relocs = internal_relocs;
		    free_relocs = NULL;

		    elf_section_data (sec)->this_hdr.contents = contents;
		    free_contents = NULL;

		    symtab_hdr->contents = (bfd_byte *) extsyms;
		    free_extsyms = NULL;

		    if ((code & 0xf3) == 0x81)
		      code = 0x01 + (code & 0x0c);
		    else if ((code & 0xf3) == 0x82)
		      code = 0x02 + (code & 0x0c);
		    else if ((code & 0xf3) == 0x83)
		      code = 0x03 + (code & 0x0c);
		    else
		      abort ();

		    /* Fix the opcode.  */
		    bfd_put_8 (abfd, code, contents + irel->r_offset - 2);

		    /* Fix the relocation's type.  */
		    irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		    /* The opcode got shorter too, so we have to fix the
		       addend and offset too!  */
		    irel->r_offset -= 1;

		    /* Delete three bytes of data.  */
		    if (!mn10300_elf_relax_delete_bytes (abfd, sec,
							 irel->r_offset + 1, 3))
		      goto error_return;

		    /* That will change things, so, we should relax again.
		       Note that this is not required, and it may be slow.  */
		    *again = true;
		    break;

		  /* mov am,(abs32)    -> mov am,(abs16)
		     mov am,(d32,sp)   -> mov am,(d16,sp)
		     mov dm,(d32,sp)   -> mov dm,(d32,sp)
		     movbu dm,(d32,sp) -> movbu dm,(d32,sp)
		     movhu dm,(d32,sp) -> movhu dm,(d32,sp) */
		  case 0x80:
		  case 0x90:
		  case 0x91:
		  case 0x92:
		  case 0x93:
		    /* Note that we've changed the relocation contents, etc.  */
		    elf_section_data (sec)->relocs = internal_relocs;
		    free_relocs = NULL;

		    elf_section_data (sec)->this_hdr.contents = contents;
		    free_contents = NULL;

		    symtab_hdr->contents = (bfd_byte *) extsyms;
		    free_extsyms = NULL;

		    /* Fix the opcode.  */
		    bfd_put_8 (abfd, 0xfa, contents + irel->r_offset - 2);
		    bfd_put_8 (abfd, code, contents + irel->r_offset - 1);

		    /* Fix the relocation's type.  */
		    irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		    /* Delete two bytes of data.  */
		    if (!mn10300_elf_relax_delete_bytes (abfd, sec,
							 irel->r_offset + 2, 2))
		      goto error_return;

		    /* That will change things, so, we should relax again.
		       Note that this is not required, and it may be slow.  */
		    *again = true;
		    break;
		  }
	      else if ((code & 0xf0) < 0xf0)
	        switch (code & 0xfc)
		  {
		  /* mov imm32,dn     -> mov imm16,dn
		     mov imm32,an     -> mov imm16,an
		     mov (abs32),dn   -> mov (abs16),dn
		     movbu (abs32),dn -> movbu (abs16),dn
		     movhu (abs32),dn -> movhu (abs16),dn  */
		  case 0xcc:
		  case 0xdc:
		  case 0xa4:
		  case 0xa8:
		  case 0xac:
		    /* Not safe if the high bit is on as relaxing may
		       move the value out of high mem and thus not fit
		       in a signed 16bit value.  */
		    if (code == 0xcc
			&& (value & 0x8000))
		      continue;

		    /* Note that we've changed the relocation contents, etc.  */
		    elf_section_data (sec)->relocs = internal_relocs;
		    free_relocs = NULL;

		    elf_section_data (sec)->this_hdr.contents = contents;
		    free_contents = NULL;

		    symtab_hdr->contents = (bfd_byte *) extsyms;
		    free_extsyms = NULL;

		    if ((code & 0xfc) == 0xcc)
		      code = 0x2c + (code & 0x03);
		    else if ((code & 0xfc) == 0xdc)
		      code = 0x24 + (code & 0x03);
		    else if ((code & 0xfc) == 0xa4)
		      code = 0x30 + (code & 0x03);
		    else if ((code & 0xfc) == 0xa8)
		      code = 0x34 + (code & 0x03);
		    else if ((code & 0xfc) == 0xac)
		      code = 0x38 + (code & 0x03);
		    else
		      abort ();

		    /* Fix the opcode.  */
		    bfd_put_8 (abfd, code, contents + irel->r_offset - 2);

		    /* Fix the relocation's type.  */
		    irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		    /* The opcode got shorter too, so we have to fix the
		       addend and offset too!  */
		    irel->r_offset -= 1;

		    /* Delete three bytes of data.  */
		    if (!mn10300_elf_relax_delete_bytes (abfd, sec,
							 irel->r_offset + 1, 3))
		      goto error_return;

		    /* That will change things, so, we should relax again.
		       Note that this is not required, and it may be slow.  */
		    *again = true;
		    break;

		  /* mov (abs32),an    -> mov (abs16),an
		     mov (d32,sp),an   -> mov (d32,sp),an
		     mov (d32,sp),dn   -> mov (d32,sp),dn
		     movbu (d32,sp),dn -> movbu (d32,sp),dn
		     movhu (d32,sp),dn -> movhu (d32,sp),dn
		     add imm32,dn      -> add imm16,dn
		     cmp imm32,dn      -> cmp imm16,dn
		     add imm32,an      -> add imm16,an
		     cmp imm32,an      -> cmp imm16,an
		     and imm32,dn      -> and imm32,dn
		     or imm32,dn       -> or imm32,dn
		     xor imm32,dn      -> xor imm32,dn
		     btst imm32,dn     -> btst imm32,dn */

		  case 0xa0:
		  case 0xb0:
		  case 0xb1:
		  case 0xb2:
		  case 0xb3:
		  case 0xc0:
		  case 0xc8:

		  case 0xd0:
#if 1
		  case 0xd8:
#endif
		  case 0xe0:
		  case 0xe1:
		  case 0xe2:
		  case 0xe3:
		    /* Note that we've changed the relocation contents, etc.  */
		    elf_section_data (sec)->relocs = internal_relocs;
		    free_relocs = NULL;

		    elf_section_data (sec)->this_hdr.contents = contents;
		    free_contents = NULL;

		    symtab_hdr->contents = (bfd_byte *) extsyms;
		    free_extsyms = NULL;

		    /* Fix the opcode.  */
		    bfd_put_8 (abfd, 0xfa, contents + irel->r_offset - 2);
		    bfd_put_8 (abfd, code, contents + irel->r_offset - 1);

		    /* Fix the relocation's type.  */
		    irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		    /* Delete two bytes of data.  */
		    if (!mn10300_elf_relax_delete_bytes (abfd, sec,
							 irel->r_offset + 2, 2))
		      goto error_return;

		    /* That will change things, so, we should relax again.
		       Note that this is not required, and it may be slow.  */
		    *again = true;
		    break;
		  }
	      else if (code == 0xfe)
		{
		  /* add imm32,sp -> add imm16,sp  */

		  /* Note that we've changed the relocation contents, etc.  */
		  elf_section_data (sec)->relocs = internal_relocs;
		  free_relocs = NULL;

		  elf_section_data (sec)->this_hdr.contents = contents;
		  free_contents = NULL;

		  symtab_hdr->contents = (bfd_byte *) extsyms;
		  free_extsyms = NULL;

		  /* Fix the opcode.  */
		  bfd_put_8 (abfd, 0xfa, contents + irel->r_offset - 2);
		  bfd_put_8 (abfd, 0xfe, contents + irel->r_offset - 1);

		  /* Fix the relocation's type.  */
		  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
						 R_MN10300_16);

		  /* Delete two bytes of data.  */
		  if (!mn10300_elf_relax_delete_bytes (abfd, sec,
						       irel->r_offset + 2, 2))
		    goto error_return;

		  /* That will change things, so, we should relax again.
		     Note that this is not required, and it may be slow.  */
		  *again = true;
		  break;
		}
	    }
	}
    }

  if (free_relocs != NULL)
    {
      free (free_relocs);
      free_relocs = NULL;
    }

  if (free_contents != NULL)
    {
      if (! link_info->keep_memory)
	free (free_contents);
      else
	{
	  /* Cache the section contents for elf_link_input_bfd.  */
	  elf_section_data (sec)->this_hdr.contents = contents;
	}
      free_contents = NULL;
    }

  if (free_extsyms != NULL)
    {
      if (! link_info->keep_memory)
	free (free_extsyms);
      else
	{
	  /* Cache the symbols for elf_link_input_bfd.  */
	  symtab_hdr->contents = extsyms;
	}
      free_extsyms = NULL;
    }

  return true;

 error_return:
  if (free_relocs != NULL)
    free (free_relocs);
  if (free_contents != NULL)
    free (free_contents);
  if (free_extsyms != NULL)
    free (free_extsyms);
  return false;
}

/* Delete some bytes from a section while relaxing.  */

static boolean
mn10300_elf_relax_delete_bytes (abfd, sec, addr, count)
     bfd *abfd;
     asection *sec;
     bfd_vma addr;
     int count;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf32_External_Sym *extsyms;
  int shndx;
  bfd_byte *contents;
  Elf_Internal_Rela *irel, *irelend;
  Elf_Internal_Rela *irelalign;
  bfd_vma toaddr;
  Elf32_External_Sym *esym, *esymend;
  struct elf_link_hash_entry **sym_hash, **sym_hash_end;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  extsyms = (Elf32_External_Sym *) symtab_hdr->contents;

  shndx = _bfd_elf_section_from_bfd_section (abfd, sec);

  contents = elf_section_data (sec)->this_hdr.contents;

  /* The deletion must stop at the next ALIGN reloc for an aligment
     power larger than the number of bytes we are deleting.  */

  irelalign = NULL;
  toaddr = sec->_cooked_size;

  irel = elf_section_data (sec)->relocs;
  irelend = irel + sec->reloc_count;

  /* Actually delete the bytes.  */
  memmove (contents + addr, contents + addr + count, toaddr - addr - count);
  sec->_cooked_size -= count;

  /* Adjust all the relocs.  */
  for (irel = elf_section_data (sec)->relocs; irel < irelend; irel++)
    {
      /* Get the new reloc address.  */
      if ((irel->r_offset > addr
	   && irel->r_offset < toaddr))
	irel->r_offset -= count;
    }

  /* Adjust all the symbols.  */
  esym = extsyms;
  esymend = esym + symtab_hdr->sh_info;
  for (; esym < esymend; esym++)
    {
      Elf_Internal_Sym isym;

      bfd_elf32_swap_symbol_in (abfd, esym, &isym);

      if (isym.st_shndx == shndx
	  && isym.st_value > addr
	  && isym.st_value < toaddr)
	{
	  isym.st_value -= count;
	  bfd_elf32_swap_symbol_out (abfd, &isym, esym);
	}
    }

  sym_hash = elf_sym_hashes (abfd);
  sym_hash_end = (sym_hash
		  + (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
		     - symtab_hdr->sh_info));
  for (; sym_hash < sym_hash_end; sym_hash++)
    {
      if (((*sym_hash)->root.type == bfd_link_hash_defined
	   || (*sym_hash)->root.type == bfd_link_hash_defweak)
	  && (*sym_hash)->root.u.def.section == sec
	  && (*sym_hash)->root.u.def.value > addr
	  && (*sym_hash)->root.u.def.value < toaddr)
	{
	  (*sym_hash)->root.u.def.value -= count;
	}
    }

  return true;
}

/* Return true if a symbol exists at the given address, else return
   false.  */
static boolean
mn10300_elf_symbol_address_p (abfd, sec, extsyms, addr)
     bfd *abfd;
     asection *sec;
     Elf32_External_Sym *extsyms;
     bfd_vma addr;
{
  Elf_Internal_Shdr *symtab_hdr;
  int shndx;
  Elf32_External_Sym *esym, *esymend;
  struct elf_link_hash_entry **sym_hash, **sym_hash_end;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  shndx = _bfd_elf_section_from_bfd_section (abfd, sec);

  /* Examine all the symbols.  */
  esym = extsyms;
  esymend = esym + symtab_hdr->sh_info;
  for (; esym < esymend; esym++)
    {
      Elf_Internal_Sym isym;

      bfd_elf32_swap_symbol_in (abfd, esym, &isym);

      if (isym.st_shndx == shndx
	  && isym.st_value == addr)
	return true;
    }

  sym_hash = elf_sym_hashes (abfd);
  sym_hash_end = (sym_hash
		  + (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
		     - symtab_hdr->sh_info));
  for (; sym_hash < sym_hash_end; sym_hash++)
    {
      if (((*sym_hash)->root.type == bfd_link_hash_defined
	   || (*sym_hash)->root.type == bfd_link_hash_defweak)
	  && (*sym_hash)->root.u.def.section == sec
	  && (*sym_hash)->root.u.def.value == addr)
	return true;
    }
  return false;
}

/* This is a version of bfd_generic_get_relocated_section_contents
   which uses mn10300_elf_relocate_section.  */

static bfd_byte *
mn10300_elf_get_relocated_section_contents (output_bfd, link_info, link_order,
					    data, relocateable, symbols)
     bfd *output_bfd;
     struct bfd_link_info *link_info;
     struct bfd_link_order *link_order;
     bfd_byte *data;
     boolean relocateable;
     asymbol **symbols;
{
  Elf_Internal_Shdr *symtab_hdr;
  asection *input_section = link_order->u.indirect.section;
  bfd *input_bfd = input_section->owner;
  asection **sections = NULL;
  Elf_Internal_Rela *internal_relocs = NULL;
  Elf32_External_Sym *external_syms = NULL;
  Elf_Internal_Sym *internal_syms = NULL;

  /* We only need to handle the case of relaxing, or of having a
     particular set of section contents, specially.  */
  if (relocateable
      || elf_section_data (input_section)->this_hdr.contents == NULL)
    return bfd_generic_get_relocated_section_contents (output_bfd, link_info,
						       link_order, data,
						       relocateable,
						       symbols);

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;

  memcpy (data, elf_section_data (input_section)->this_hdr.contents,
	  input_section->_raw_size);

  if ((input_section->flags & SEC_RELOC) != 0
      && input_section->reloc_count > 0)
    {
      Elf_Internal_Sym *isymp;
      asection **secpp;
      Elf32_External_Sym *esym, *esymend;

      if (symtab_hdr->contents != NULL)
	external_syms = (Elf32_External_Sym *) symtab_hdr->contents;
      else
	{
	  external_syms = ((Elf32_External_Sym *)
			   bfd_malloc (symtab_hdr->sh_info
				       * sizeof (Elf32_External_Sym)));
	  if (external_syms == NULL && symtab_hdr->sh_info > 0)
	    goto error_return;
	  if (bfd_seek (input_bfd, symtab_hdr->sh_offset, SEEK_SET) != 0
	      || (bfd_read (external_syms, sizeof (Elf32_External_Sym),
			    symtab_hdr->sh_info, input_bfd)
		  != (symtab_hdr->sh_info * sizeof (Elf32_External_Sym))))
	    goto error_return;
	}

      internal_relocs = (_bfd_elf32_link_read_relocs
			 (input_bfd, input_section, (PTR) NULL,
			  (Elf_Internal_Rela *) NULL, false));
      if (internal_relocs == NULL)
	goto error_return;

      internal_syms = ((Elf_Internal_Sym *)
		       bfd_malloc (symtab_hdr->sh_info
				   * sizeof (Elf_Internal_Sym)));
      if (internal_syms == NULL && symtab_hdr->sh_info > 0)
	goto error_return;

      sections = (asection **) bfd_malloc (symtab_hdr->sh_info
					   * sizeof (asection *));
      if (sections == NULL && symtab_hdr->sh_info > 0)
	goto error_return;

      isymp = internal_syms;
      secpp = sections;
      esym = external_syms;
      esymend = esym + symtab_hdr->sh_info;
      for (; esym < esymend; ++esym, ++isymp, ++secpp)
	{
	  asection *isec;

	  bfd_elf32_swap_symbol_in (input_bfd, esym, isymp);

	  if (isymp->st_shndx == SHN_UNDEF)
	    isec = bfd_und_section_ptr;
	  else if (isymp->st_shndx > 0 && isymp->st_shndx < SHN_LORESERVE)
	    isec = bfd_section_from_elf_index (input_bfd, isymp->st_shndx);
	  else if (isymp->st_shndx == SHN_ABS)
	    isec = bfd_abs_section_ptr;
	  else if (isymp->st_shndx == SHN_COMMON)
	    isec = bfd_com_section_ptr;
	  else
	    {
	      /* Who knows?  */
	      isec = NULL;
	    }

	  *secpp = isec;
	}

      if (! mn10300_elf_relocate_section (output_bfd, link_info, input_bfd,
				     input_section, data, internal_relocs,
				     internal_syms, sections))
	goto error_return;

      if (sections != NULL)
	free (sections);
      sections = NULL;
      if (internal_syms != NULL)
	free (internal_syms);
      internal_syms = NULL;
      if (external_syms != NULL && symtab_hdr->contents == NULL)
	free (external_syms);
      external_syms = NULL;
      if (internal_relocs != elf_section_data (input_section)->relocs)
	free (internal_relocs);
      internal_relocs = NULL;
    }

  return data;

 error_return:
  if (internal_relocs != NULL
      && internal_relocs != elf_section_data (input_section)->relocs)
    free (internal_relocs);
  if (external_syms != NULL && symtab_hdr->contents == NULL)
    free (external_syms);
  if (internal_syms != NULL)
    free (internal_syms);
  if (sections != NULL)
    free (sections);
  return NULL;
}



#define TARGET_LITTLE_SYM	bfd_elf32_mn10300_vec
#define TARGET_LITTLE_NAME	"elf32-mn10300"
#define ELF_ARCH		bfd_arch_mn10300
#define ELF_MACHINE_CODE	EM_CYGNUS_MN10300
#define ELF_MAXPAGESIZE		0x1000

#define elf_info_to_howto	mn10300_info_to_howto
#define elf_info_to_howto_rel	0
#define elf_backend_relocate_section mn10300_elf_relocate_section
#define bfd_elf32_bfd_relax_section	mn10300_elf_relax_section
#define bfd_elf32_bfd_get_relocated_section_contents \
				mn10300_elf_get_relocated_section_contents

#define elf_symbol_leading_char '_'

#include "elf32-target.h"
