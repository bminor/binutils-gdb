/* X86-64 specific support for 64-bit ELF
   Copyright 2000 Free Software Foundation, Inc.
   Contributed by Jan Hubicka <jh@suse.cz>.

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

#include "elf/x86-64.h"

/* We use only the RELA entries.  */
#define USE_RELA

/* In case we're on a 32-bit machine, construct a 64-bit "-1" value.  */
#define MINUS_ONE (~ (bfd_vma) 0)

/* The relocation "howto" table.  Order of fields:
   type, size, bitsize, pc_relative, complain_on_overflow, special_function,
   name, partial_inplace, src_mask, dst_pack, pcrel_offset  */
static reloc_howto_type x86_64_elf_howto_table[] =
{
  HOWTO(R_X86_64_NONE,          0,0, 0,false,0,complain_overflow_dont,    0, "R_X86_64_NONE",   false,0x00000000,0x00000000,false),
  HOWTO(R_X86_64_64,    0,4,64,false,0,complain_overflow_bitfield,0, "R_X86_64_64",     false,MINUS_ONE ,MINUS_ONE ,false),
  HOWTO(R_X86_64_PC32,          0,4,32,true ,0,complain_overflow_signed  ,0, "R_X86_64_PC32",   false,0xffffffff,0xffffffff,true),
  HOWTO(R_X86_64_GOT32,         0,4,32,false,0,complain_overflow_signed  ,0, "R_X86_64_GOT32",  false,0xffffffff,0xffffffff,false),
  HOWTO(R_X86_64_PLT32,         0,4,32,true ,0,complain_overflow_signed  ,0, "R_X86_64_PLT32",  false,0xffffffff,0xffffffff,true),
  HOWTO(R_X86_64_COPY,     0,4,32,false,0,complain_overflow_bitfield,0, "R_X86_64_COPY",   false,0xffffffff,0xffffffff,false),
  HOWTO(R_X86_64_GLOB_DAT, 0,4,64,false,0,complain_overflow_bitfield,0,"R_X86_64_GLOB_DAT",false,MINUS_ONE ,MINUS_ONE ,false),
  HOWTO(R_X86_64_RELATIVE ,0,4,64,false,0,complain_overflow_bitfield,0,"R_X86_64_RELATIVE",false,MINUS_ONE ,MINUS_ONE ,false),
  HOWTO(R_X86_64_JUMP_SLOT,0,4,64,false,0,complain_overflow_bitfield,0,"R_X86_64_JUMP_SLOT",false,MINUS_ONE,MINUS_ONE ,false),
  HOWTO(R_X86_64_GOTPCREL, 0,4,32,true, 0,complain_overflow_signed  ,0, "R_X86_64_PCREL",  false,0xffffffff,0xffffffff,true),
  HOWTO(R_X86_64_32,    0,4,32,false,0,complain_overflow_unsigned,0, "R_X86_64_32",     false,0xffffffff,0xffffffff,false),
  HOWTO(R_X86_64_32S,   0,4,32,false,0,complain_overflow_signed,  0, "R_X86_64_32S",    false,0xffffffff,0xffffffff,false),
  HOWTO(R_X86_64_16,    0,1,16,false,0,complain_overflow_bitfield,0, "R_X86_64_16",     false,0xffff    ,0xffff,    false),
  HOWTO(R_X86_64_PC16,          0,1,16,true ,0,complain_overflow_bitfield,0, "R_X86_64_PC16",   false,0xffff    ,0xffff,    true),
  HOWTO(R_X86_64_8,     0,0, 8,false,0,complain_overflow_signed  ,0, "R_X86_64_8",      false,0xff      ,0xff,      false),
  HOWTO(R_X86_64_PC8,   0,0, 8,true ,0,complain_overflow_signed  ,0, "R_X86_64_PC8",    false,0xff      ,0xff,      true),
};

/* Map BFD relocs to the x86_64 elf relocs.  */
struct elf_reloc_map {
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static CONST struct elf_reloc_map x86_64_reloc_map[] =
{
  { BFD_RELOC_NONE,           R_X86_64_NONE, },
  { BFD_RELOC_64,             R_X86_64_64,   },
  { BFD_RELOC_32_PCREL,               R_X86_64_PC32, },
  { BFD_RELOC_X86_64_GOT32,   R_X86_64_GOT32,},
  { BFD_RELOC_X86_64_PLT32,   R_X86_64_PLT32,},
  { BFD_RELOC_X86_64_COPY,    R_X86_64_COPY, },
  { BFD_RELOC_X86_64_GLOB_DAT,        R_X86_64_GLOB_DAT, },
  { BFD_RELOC_X86_64_JUMP_SLOT,       R_X86_64_JUMP_SLOT, },
  { BFD_RELOC_X86_64_RELATIVE,        R_X86_64_RELATIVE, },
  { BFD_RELOC_X86_64_GOTPCREL,        R_X86_64_GOTPCREL, },
  { BFD_RELOC_32,             R_X86_64_32, },
  { BFD_RELOC_X86_64_32S,     R_X86_64_32S, },
  { BFD_RELOC_16,             R_X86_64_16, },
  { BFD_RELOC_16_PCREL,               R_X86_64_PC16, },
  { BFD_RELOC_8,              R_X86_64_8, },
  { BFD_RELOC_8_PCREL,                R_X86_64_PC8, },
};


static reloc_howto_type *elf64_x86_64_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));
static void elf64_x86_64_info_to_howto
  PARAMS ((bfd *, arelent *, Elf64_Internal_Rela *));
static struct bfd_link_hash_table *elf64_x86_64_link_hash_table_create
  PARAMS ((bfd *));
static boolean elf64_x86_64_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
         Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));

/* Given a BFD reloc type, return a HOWTO structure.  */
static reloc_howto_type *
elf64_x86_64_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  unsigned int i;
  for (i = 0; i < sizeof (x86_64_reloc_map) / sizeof (struct elf_reloc_map);
       i++)
    {
      if (x86_64_reloc_map[i].bfd_reloc_val == code)
      return &x86_64_elf_howto_table[(int)
                                     x86_64_reloc_map[i].elf_reloc_val];
    }
  return 0;
}


/* Given an x86_64 ELF reloc type, fill in an arelent structure.  */
static void
elf64_x86_64_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf64_Internal_Rela *dst;
{
  unsigned r_type;

  r_type = ELF64_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_X86_64_max);
  cache_ptr->howto = &x86_64_elf_howto_table[r_type];
  BFD_ASSERT (r_type == cache_ptr->howto->type);
}

/* Hash table functions - these will be used by dynamic linking code and
   right now they are needed to keep ld happy.  */

/* x86_64  ELF linker hash table.  */

struct elf64_x86_64_link_hash_table
{
  struct elf_link_hash_table root;
};


/* Get the X86-64 ELF linker hash table from a link_info structure.  */

#define elf64_x86_64_hash_table(p) \
  ((struct elf64_x86_64_link_hash_table *) ((p)->hash))

/* Create an X86-64 ELF linker hash table.  */

static struct bfd_link_hash_table *
elf64_x86_64_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct elf64_x86_64_link_hash_table *ret;

  ret = ((struct elf64_x86_64_link_hash_table *)
       bfd_alloc (abfd, sizeof (struct elf64_x86_64_link_hash_table)));
  if (ret == (struct elf64_x86_64_link_hash_table *) NULL)
    return NULL;

  if (! _bfd_elf_link_hash_table_init (&ret->root, abfd,
                                     _bfd_elf_link_hash_newfunc))
    {
      bfd_release (abfd, ret);
      return NULL;
    }

  return &ret->root.root;
}

boolean
elf64_x86_64_elf_object_p (abfd)
     bfd *abfd;
{
  /* Set the right machine number for an x86-64 elf64 file.  */
  bfd_default_set_arch_mach (abfd, bfd_arch_i386, bfd_mach_x86_64);
  return true;
}

/* Relocate an x86_64 ELF section.  */

static boolean
elf64_x86_64_relocate_section (output_bfd, info, input_bfd, input_section,
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
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_vma *local_got_offsets;
  asection *sreloc;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  local_got_offsets = elf_local_got_offsets (input_bfd);

  sreloc = NULL;
  if (dynobj != NULL)
    abort ();

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      unsigned int indx;

      r_type = ELF64_R_TYPE (rel->r_info);

      if ((indx = (unsigned) r_type) >= R_X86_64_max)
      {
        bfd_set_error (bfd_error_bad_value);
        return false;
      }
      howto = x86_64_elf_howto_table + indx;

      r_symndx = ELF64_R_SYM (rel->r_info);

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
            if (sec->output_section == NULL)
              {
                (*_bfd_error_handler)
                  (_("%s: warning: unresolvable relocation against symbol `%s' from %s section"),
                   bfd_get_filename (input_bfd), h->root.root.string,
                   bfd_get_section_name (input_bfd, input_section));
                relocation = 0;
              }
            else
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
                    input_section, rel->r_offset,
                    (!info->shared || info->no_undefined
                     || ELF_ST_VISIBILITY (h->other)))))
              return false;
            relocation = 0;
          }
      }
      /* This function should support shared objects, but don't.  */
      if (info->shared)
      abort();

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
                                  contents, rel->r_offset,
                                  relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
      {
        switch (r)
          {
          default:
          case bfd_reloc_outofrange:
            abort ();
          case bfd_reloc_overflow:
            {
              const char *name;

              if (h != NULL)
                name = h->root.root.string;
              else
                {
                  name = bfd_elf_string_from_elf_section (input_bfd,
                                                          symtab_hdr->sh_link,
                                                          sym->st_name);
                  if (name == NULL)
                    return false;
                  if (*name == '\0')
                    name = bfd_section_name (input_bfd, sec);
                }
              if (! ((*info->callbacks->reloc_overflow)
                     (info, name, howto->name, (bfd_vma) 0,
                      input_bfd, input_section, rel->r_offset)))
                return false;
            }
            break;
          }
      }
    }
  return true;
}


#define TARGET_LITTLE_SYM             bfd_elf64_x86_64_vec
#define TARGET_LITTLE_NAME            "elf64-x86-64"
#define ELF_ARCH                      bfd_arch_i386
#define ELF_MACHINE_CODE              EM_X86_64
#define ELF_MAXPAGESIZE                       0x100000
#define elf_info_to_howto             elf64_x86_64_info_to_howto
#define bfd_elf64_bfd_reloc_type_lookup       elf64_x86_64_reloc_type_lookup
#define elf_backend_object_p            elf64_x86_64_elf_object_p
#define elf_backend_relocate_section  elf64_x86_64_relocate_section
#define bfd_elf64_bfd_link_hash_table_create elf64_x86_64_link_hash_table_create

#include "elf64-target.h"
