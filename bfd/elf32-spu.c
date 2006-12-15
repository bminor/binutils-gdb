/* SPU specific support for 32-bit ELF

   Copyright 2006 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/spu.h"
#include "elf32-spu.h"

/* We use RELA style relocs.  Don't define USE_REL.  */

static bfd_reloc_status_type spu_elf_rel9 (bfd *, arelent *, asymbol *,
					   void *, asection *,
					   bfd *, char **);

/* Values of type 'enum elf_spu_reloc_type' are used to index this
   array, so it must be declared in the order of that type.  */

static reloc_howto_type elf_howto_table[] = {
  HOWTO (R_SPU_NONE,       0, 0,  0, FALSE,  0, complain_overflow_dont,
	 bfd_elf_generic_reloc, "SPU_NONE",
	 FALSE, 0, 0x00000000, FALSE),
  HOWTO (R_SPU_ADDR10,     4, 2, 10, FALSE, 14, complain_overflow_bitfield,
	 bfd_elf_generic_reloc, "SPU_ADDR10",
	 FALSE, 0, 0x00ffc000, FALSE),
  HOWTO (R_SPU_ADDR16,     2, 2, 16, FALSE,  7, complain_overflow_bitfield,
	 bfd_elf_generic_reloc, "SPU_ADDR16",
	 FALSE, 0, 0x007fff80, FALSE),
  HOWTO (R_SPU_ADDR16_HI, 16, 2, 16, FALSE,  7, complain_overflow_bitfield,
	 bfd_elf_generic_reloc, "SPU_ADDR16_HI",
	 FALSE, 0, 0x007fff80, FALSE),
  HOWTO (R_SPU_ADDR16_LO,  0, 2, 16, FALSE,  7, complain_overflow_dont,
	 bfd_elf_generic_reloc, "SPU_ADDR16_LO",
	 FALSE, 0, 0x007fff80, FALSE),
  HOWTO (R_SPU_ADDR18,     0, 2, 18, FALSE,  7, complain_overflow_bitfield,
	 bfd_elf_generic_reloc, "SPU_ADDR18",
	 FALSE, 0, 0x01ffff80, FALSE),
  HOWTO (R_SPU_ADDR32,   0, 2, 32, FALSE,  0, complain_overflow_dont,
	 bfd_elf_generic_reloc, "SPU_ADDR32",
	 FALSE, 0, 0xffffffff, FALSE),
  HOWTO (R_SPU_REL16,      2, 2, 16,  TRUE,  7, complain_overflow_bitfield,
	 bfd_elf_generic_reloc, "SPU_REL16",
	 FALSE, 0, 0x007fff80, TRUE),
  HOWTO (R_SPU_ADDR7,      0, 2,  7, FALSE, 14, complain_overflow_dont,
	 bfd_elf_generic_reloc, "SPU_ADDR7",
	 FALSE, 0, 0x001fc000, FALSE),
  HOWTO (R_SPU_REL9,       2, 2,  9,  TRUE,  0, complain_overflow_signed,
	 spu_elf_rel9,          "SPU_REL9",
	 FALSE, 0, 0x0180007f, TRUE),
  HOWTO (R_SPU_REL9I,      2, 2,  9,  TRUE,  0, complain_overflow_signed,
	 spu_elf_rel9,          "SPU_REL9I",
	 FALSE, 0, 0x0000c07f, TRUE),
  HOWTO (R_SPU_ADDR10I,    0, 2, 10, FALSE, 14, complain_overflow_signed,
	 bfd_elf_generic_reloc, "SPU_ADDR10I",
	 FALSE, 0, 0x00ffc000, FALSE),
  HOWTO (R_SPU_ADDR16I,    0, 2, 16, FALSE,  7, complain_overflow_signed,
	 bfd_elf_generic_reloc, "SPU_ADDR16I",
	 FALSE, 0, 0x007fff80, FALSE),
  HOWTO (R_SPU_REL32,   0, 2, 32, TRUE,  0, complain_overflow_dont,
	 bfd_elf_generic_reloc, "SPU_REL32",
	 FALSE, 0, 0xffffffff, TRUE),
};

static struct bfd_elf_special_section const spu_elf_special_sections[] = {
  { ".toe", 4, 0, SHT_NOBITS, SHF_ALLOC },
  { NULL, 0, 0, 0, 0 }
};

static enum elf_spu_reloc_type
spu_elf_bfd_to_reloc_type (bfd_reloc_code_real_type code)
{
  switch (code)
    {
    default:
      return R_SPU_NONE;
    case BFD_RELOC_SPU_IMM10W:
      return R_SPU_ADDR10;
    case BFD_RELOC_SPU_IMM16W:
      return R_SPU_ADDR16;
    case BFD_RELOC_SPU_LO16:
      return R_SPU_ADDR16_LO;
    case BFD_RELOC_SPU_HI16:
      return R_SPU_ADDR16_HI;
    case BFD_RELOC_SPU_IMM18:
      return R_SPU_ADDR18;
    case BFD_RELOC_SPU_PCREL16:
      return R_SPU_REL16;
    case BFD_RELOC_SPU_IMM7:
      return R_SPU_ADDR7;
    case BFD_RELOC_SPU_IMM8:
      return R_SPU_NONE;
    case BFD_RELOC_SPU_PCREL9a:
      return R_SPU_REL9;
    case BFD_RELOC_SPU_PCREL9b:
      return R_SPU_REL9I;
    case BFD_RELOC_SPU_IMM10:
      return R_SPU_ADDR10I;
    case BFD_RELOC_SPU_IMM16:
      return R_SPU_ADDR16I;
    case BFD_RELOC_32:
      return R_SPU_ADDR32;
    case BFD_RELOC_32_PCREL:
      return R_SPU_REL32;
    }
}

static void
spu_elf_info_to_howto (bfd *abfd ATTRIBUTE_UNUSED,
		       arelent *cache_ptr,
		       Elf_Internal_Rela *dst)
{
  enum elf_spu_reloc_type r_type;

  r_type = (enum elf_spu_reloc_type) ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < R_SPU_max);
  cache_ptr->howto = &elf_howto_table[(int) r_type];
}

static reloc_howto_type *
spu_elf_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			   bfd_reloc_code_real_type code)
{
  return elf_howto_table + spu_elf_bfd_to_reloc_type (code);
}

/* Apply R_SPU_REL9 and R_SPU_REL9I relocs.  */

static bfd_reloc_status_type
spu_elf_rel9 (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
	      void *data, asection *input_section,
	      bfd *output_bfd, char **error_message)
{
  bfd_size_type octets;
  bfd_vma val;
  long insn;

  /* If this is a relocatable link (output_bfd test tells us), just
     call the generic function.  Any adjustment will be done at final
     link time.  */
  if (output_bfd != NULL)
    return bfd_elf_generic_reloc (abfd, reloc_entry, symbol, data,
				  input_section, output_bfd, error_message);

  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_outofrange;
  octets = reloc_entry->address * bfd_octets_per_byte (abfd);

  /* Get symbol value.  */
  val = 0;
  if (!bfd_is_com_section (symbol->section))
    val = symbol->value;
  if (symbol->section->output_section)
    val += symbol->section->output_section->vma;

  val += reloc_entry->addend;

  /* Make it pc-relative.  */
  val -= input_section->output_section->vma + input_section->output_offset;

  val >>= 2;
  if (val + 256 >= 512)
    return bfd_reloc_overflow;

  insn = bfd_get_32 (abfd, (bfd_byte *) data + octets);

  /* Move two high bits of value to REL9I and REL9 position.
     The mask will take care of selecting the right field.  */
  val = (val & 0x7f) | ((val & 0x180) << 7) | ((val & 0x180) << 16);
  insn &= ~reloc_entry->howto->dst_mask;
  insn |= val & reloc_entry->howto->dst_mask;
  bfd_put_32 (abfd, insn, (bfd_byte *) data + octets);
  return bfd_reloc_ok;
}

static bfd_boolean
spu_elf_new_section_hook (bfd *abfd, asection *sec)
{
  if (!sec->used_by_bfd)
    {
      struct _spu_elf_section_data *sdata;

      sdata = bfd_zalloc (abfd, sizeof (*sdata));
      if (sdata == NULL)
	return FALSE;
      sec->used_by_bfd = sdata;
    }

  return _bfd_elf_new_section_hook (abfd, sec);
}

/* Specially mark defined symbols named _EAR_* with BSF_KEEP so that
   strip --strip-unneeded will not remove them.  */

static void
spu_elf_backend_symbol_processing (bfd *abfd ATTRIBUTE_UNUSED, asymbol *sym)
{
  if (sym->name != NULL
      && sym->section != bfd_abs_section_ptr
      && strncmp (sym->name, "_EAR_", 5) == 0)
    sym->flags |= BSF_KEEP;
}

/* SPU ELF linker hash table.  */

struct spu_link_hash_table
{
  struct elf_link_hash_table elf;

  /* The stub hash table.  */
  struct bfd_hash_table stub_hash_table;

  /* Shortcuts to overlay sections.  */
  asection *stub;
  asection *ovtab;

  struct elf_link_hash_entry *ovly_load;

  /* An array of two output sections per overlay region, chosen such that
     the first section vma is the overlay buffer vma (ie. the section has
     the lowest vma in the group that occupy the region), and the second
     section vma+size specifies the end of the region.  We keep pointers
     to sections like this because section vmas may change when laying
     them out.  */
  asection **ovl_region;

  /* Number of overlay buffers.  */
  unsigned int num_buf;

  /* Total number of overlays.  */
  unsigned int num_overlays;

  /* Set if we should emit symbols for stubs.  */
  unsigned int emit_stub_syms:1;

  /* Set if we want stubs on calls out of overlay regions to
     non-overlay regions.  */
  unsigned int non_overlay_stubs : 1;

  /* Set on error.  */
  unsigned int stub_overflow : 1;
};

#define spu_hash_table(p) \
  ((struct spu_link_hash_table *) ((p)->hash))

struct spu_stub_hash_entry
{
  struct bfd_hash_entry root;

  /* Destination of this stub.  */
  asection *target_section;
  bfd_vma target_off;

  /* Offset of entry in stub section.  */
  bfd_vma off;

  /* Offset from this stub to stub that loads the overlay index.  */
  bfd_vma delta;
};

/* Create an entry in a spu stub hash table.  */

static struct bfd_hash_entry *
stub_hash_newfunc (struct bfd_hash_entry *entry,
		   struct bfd_hash_table *table,
		   const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry = bfd_hash_allocate (table, sizeof (struct spu_stub_hash_entry));
      if (entry == NULL)
	return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = bfd_hash_newfunc (entry, table, string);
  if (entry != NULL)
    {
      struct spu_stub_hash_entry *sh = (struct spu_stub_hash_entry *) entry;

      sh->target_section = NULL;
      sh->target_off = 0;
      sh->off = 0;
      sh->delta = 0;
    }

  return entry;
}

/* Create a spu ELF linker hash table.  */

static struct bfd_link_hash_table *
spu_elf_link_hash_table_create (bfd *abfd)
{
  struct spu_link_hash_table *htab;

  htab = bfd_malloc (sizeof (*htab));
  if (htab == NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&htab->elf, abfd,
				      _bfd_elf_link_hash_newfunc,
				      sizeof (struct elf_link_hash_entry)))
    {
      free (htab);
      return NULL;
    }

  /* Init the stub hash table too.  */
  if (!bfd_hash_table_init (&htab->stub_hash_table, stub_hash_newfunc,
			    sizeof (struct spu_stub_hash_entry)))
    return NULL;

  memset (&htab->stub, 0,
	  sizeof (*htab) - offsetof (struct spu_link_hash_table, stub));

  return &htab->elf.root;
}

/* Free the derived linker hash table.  */

static void
spu_elf_link_hash_table_free (struct bfd_link_hash_table *hash)
{
  struct spu_link_hash_table *ret = (struct spu_link_hash_table *) hash;

  bfd_hash_table_free (&ret->stub_hash_table);
  _bfd_generic_link_hash_table_free (hash);
}

/* Find the symbol for the given R_SYMNDX in IBFD and set *HP and *SYMP
   to (hash, NULL) for global symbols, and (NULL, sym) for locals.  Set
   *SYMSECP to the symbol's section.  *LOCSYMSP caches local syms.  */

static bfd_boolean
get_sym_h (struct elf_link_hash_entry **hp,
	   Elf_Internal_Sym **symp,
	   asection **symsecp,
	   Elf_Internal_Sym **locsymsp,
	   unsigned long r_symndx,
	   bfd *ibfd)
{
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (ibfd)->symtab_hdr;

  if (r_symndx >= symtab_hdr->sh_info)
    {
      struct elf_link_hash_entry **sym_hashes = elf_sym_hashes (ibfd);
      struct elf_link_hash_entry *h;

      h = sym_hashes[r_symndx - symtab_hdr->sh_info];
      while (h->root.type == bfd_link_hash_indirect
	     || h->root.type == bfd_link_hash_warning)
	h = (struct elf_link_hash_entry *) h->root.u.i.link;

      if (hp != NULL)
	*hp = h;

      if (symp != NULL)
	*symp = NULL;

      if (symsecp != NULL)
	{
	  asection *symsec = NULL;
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    symsec = h->root.u.def.section;
	  *symsecp = symsec;
	}
    }
  else
    {
      Elf_Internal_Sym *sym;
      Elf_Internal_Sym *locsyms = *locsymsp;

      if (locsyms == NULL)
	{
	  locsyms = (Elf_Internal_Sym *) symtab_hdr->contents;
	  if (locsyms == NULL)
	    locsyms = bfd_elf_get_elf_syms (ibfd, symtab_hdr,
					    symtab_hdr->sh_info,
					    0, NULL, NULL, NULL);
	  if (locsyms == NULL)
	    return FALSE;
	  *locsymsp = locsyms;
	}
      sym = locsyms + r_symndx;

      if (hp != NULL)
	*hp = NULL;

      if (symp != NULL)
	*symp = sym;

      if (symsecp != NULL)
	{
	  asection *symsec = NULL;
	  if ((sym->st_shndx != SHN_UNDEF
	       && sym->st_shndx < SHN_LORESERVE)
	      || sym->st_shndx > SHN_HIRESERVE)
	    symsec = bfd_section_from_elf_index (ibfd, sym->st_shndx);
	  *symsecp = symsec;
	}
    }
  return TRUE;
}

/* Build a name for an entry in the stub hash table.  The input section
   id isn't really necessary but we add that in for consistency with
   ppc32 and ppc64 stub names.  We can't use a local symbol name
   because ld -r might generate duplicate local symbols.  */

static char *
spu_stub_name (const asection *input_sec,
	       const asection *sym_sec,
	       const struct elf_link_hash_entry *h,
	       const Elf_Internal_Rela *rel)
{
  char *stub_name;
  bfd_size_type len;

  if (h)
    {
      len = 8 + 1 + strlen (h->root.root.string) + 1 + 8 + 1;
      stub_name = bfd_malloc (len);
      if (stub_name == NULL)
	return stub_name;

      sprintf (stub_name, "%08x.%s+%x",
	       input_sec->id & 0xffffffff,
	       h->root.root.string,
	       (int) rel->r_addend & 0xffffffff);
      len -= 8;
    }
  else
    {
      len = 8 + 1 + 8 + 1 + 8 + 1 + 8 + 1;
      stub_name = bfd_malloc (len);
      if (stub_name == NULL)
	return stub_name;

      sprintf (stub_name, "%08x.%x:%x+%x",
	       input_sec->id & 0xffffffff,
	       sym_sec->id & 0xffffffff,
	       (int) ELF32_R_SYM (rel->r_info) & 0xffffffff,
	       (int) rel->r_addend & 0xffffffff);
      len = strlen (stub_name);
    }

  if (stub_name[len - 2] == '+'
      && stub_name[len - 1] == '0'
      && stub_name[len] == 0)
    stub_name[len - 2] = 0;

  return stub_name;
}

/* Create the note section if not already present.  This is done early so
   that the linker maps the sections to the right place in the output.  */

bfd_boolean
spu_elf_create_sections (bfd *output_bfd, struct bfd_link_info *info)
{
  bfd *ibfd;

  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->next)
    if (bfd_get_section_by_name (ibfd, SPU_PTNOTE_SPUNAME) != NULL)
      break;

  if (ibfd == NULL)
    {
      /* Make SPU_PTNOTE_SPUNAME section.  */
      asection *s;
      size_t name_len;
      size_t size;
      bfd_byte *data;
      flagword flags;

      ibfd = info->input_bfds;
      flags = SEC_LOAD | SEC_READONLY | SEC_HAS_CONTENTS | SEC_IN_MEMORY;
      s = bfd_make_section_anyway_with_flags (ibfd, SPU_PTNOTE_SPUNAME, flags);
      if (s == NULL
	  || !bfd_set_section_alignment (ibfd, s, 4))
	return FALSE;

      name_len = strlen (bfd_get_filename (output_bfd)) + 1;
      size = 12 + ((sizeof (SPU_PLUGIN_NAME) + 3) & -4);
      size += (name_len + 3) & -4;

      if (!bfd_set_section_size (ibfd, s, size))
	return FALSE;

      data = bfd_zalloc (ibfd, size);
      if (data == NULL)
	return FALSE;

      bfd_put_32 (ibfd, sizeof (SPU_PLUGIN_NAME), data + 0);
      bfd_put_32 (ibfd, name_len, data + 4);
      bfd_put_32 (ibfd, 1, data + 8);
      memcpy (data + 12, SPU_PLUGIN_NAME, sizeof (SPU_PLUGIN_NAME));
      memcpy (data + 12 + ((sizeof (SPU_PLUGIN_NAME) + 3) & -4),
	      bfd_get_filename (output_bfd), name_len);
      s->contents = data;
    }

  return TRUE;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
spu_elf_gc_mark_hook (asection *sec,
		      struct bfd_link_info *info ATTRIBUTE_UNUSED,
		      Elf_Internal_Rela *rel ATTRIBUTE_UNUSED,
		      struct elf_link_hash_entry *h,
		      Elf_Internal_Sym *sym)
{
  if (h != NULL)
    {
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
  else
    return bfd_section_from_elf_index (sec->owner, sym->st_shndx);

  return NULL;
}

/* qsort predicate to sort sections by vma.  */

static int
sort_sections (const void *a, const void *b)
{
  const asection *const *s1 = a;
  const asection *const *s2 = b;
  bfd_signed_vma delta = (*s1)->vma - (*s2)->vma;

  if (delta != 0)
    return delta < 0 ? -1 : 1;

  return (*s1)->index - (*s2)->index;
}

/* Identify overlays in the output bfd, and number them.  */

bfd_boolean
spu_elf_find_overlays (bfd *output_bfd, struct bfd_link_info *info)
{
  struct spu_link_hash_table *htab = spu_hash_table (info);
  asection **alloc_sec;
  unsigned int i, n, ovl_index, num_buf;
  asection *s;
  bfd_vma ovl_end;

  if (output_bfd->section_count < 2)
    return FALSE;

  alloc_sec = bfd_malloc (output_bfd->section_count * sizeof (*alloc_sec));
  if (alloc_sec == NULL)
    return FALSE;

  /* Pick out all the alloced sections.  */
  for (n = 0, s = output_bfd->sections; s != NULL; s = s->next)
    if ((s->flags & SEC_ALLOC) != 0
	&& (s->flags & (SEC_LOAD | SEC_THREAD_LOCAL)) != SEC_THREAD_LOCAL
	&& s->size != 0)
      alloc_sec[n++] = s;

  if (n == 0)
    {
      free (alloc_sec);
      return FALSE;
    }

  /* Sort them by vma.  */
  qsort (alloc_sec, n, sizeof (*alloc_sec), sort_sections);

  /* Look for overlapping vmas.  Any with overlap must be overlays.
     Count them.  Also count the number of overlay regions and for
     each region save a section from that region with the lowest vma
     and another section with the highest end vma.  */
  ovl_end = alloc_sec[0]->vma + alloc_sec[0]->size;
  for (ovl_index = 0, num_buf = 0, i = 1; i < n; i++)
    {
      s = alloc_sec[i];
      if (s->vma < ovl_end)
	{
	  asection *s0 = alloc_sec[i - 1];

	  if (spu_elf_section_data (s0)->ovl_index == 0)
	    {
	      spu_elf_section_data (s0)->ovl_index = ++ovl_index;
	      alloc_sec[num_buf * 2] = s0;
	      alloc_sec[num_buf * 2 + 1] = s0;
	      num_buf++;
	    }
	  spu_elf_section_data (s)->ovl_index = ++ovl_index;
	  if (ovl_end < s->vma + s->size)
	    {
	      ovl_end = s->vma + s->size;
	      alloc_sec[num_buf * 2 - 1] = s;
	    }
	}
      else
	ovl_end = s->vma + s->size;
    }

  htab->num_overlays = ovl_index;
  htab->num_buf = num_buf;
  if (ovl_index == 0)
    {
      free (alloc_sec);
      return FALSE;
    }

  alloc_sec = bfd_realloc (alloc_sec, num_buf * 2 * sizeof (*alloc_sec));
  if (alloc_sec == NULL)
    return FALSE;

  htab->ovl_region = alloc_sec;
  return TRUE;
}

/* One of these per stub.  */
#define SIZEOF_STUB1 8
#define ILA_79	0x4200004f		/* ila $79,function_address */
#define BR	0x32000000		/* br stub2 */

/* One of these per overlay.  */
#define SIZEOF_STUB2 8
#define ILA_78	0x4200004e		/* ila $78,overlay_number */
					/* br __ovly_load */
#define NOP	0x40200000

/* Return true for all relative and absolute branch and hint instructions.
   bra   00110000 0..
   brasl 00110001 0..
   br    00110010 0..
   brsl  00110011 0..
   brz   00100000 0..
   brnz  00100001 0..
   brhz  00100010 0..
   brhnz 00100011 0..
   hbra  0001000..
   hbrr  0001001..  */

static bfd_boolean
is_branch (const unsigned char *insn)
{
  return (((insn[0] & 0xec) == 0x20 && (insn[1] & 0x80) == 0)
	  || (insn[0] & 0xfc) == 0x10);
}

struct stubarr {
  struct spu_stub_hash_entry **sh;
  unsigned int count;
};

/* Called via bfd_hash_traverse to set up pointers to all symbols
   in the stub hash table.  */

static bfd_boolean
populate_stubs (struct bfd_hash_entry *bh, void *inf)
{
  struct stubarr *stubs = inf;

  stubs->sh[--stubs->count] = (struct spu_stub_hash_entry *) bh;
  return TRUE;
}

/* qsort predicate to sort stubs by overlay number.  */

static int
sort_stubs (const void *a, const void *b)
{
  const struct spu_stub_hash_entry *const *sa = a;
  const struct spu_stub_hash_entry *const *sb = b;
  int i;
  bfd_signed_vma d;

  i = spu_elf_section_data ((*sa)->target_section->output_section)->ovl_index;
  i -= spu_elf_section_data ((*sb)->target_section->output_section)->ovl_index;
  if (i != 0)
    return i;

  d = ((*sa)->target_section->output_section->vma
       + (*sa)->target_section->output_offset
       + (*sa)->target_off
       - (*sb)->target_section->output_section->vma
       - (*sb)->target_section->output_offset
       - (*sb)->target_off);
  if (d != 0)
    return d < 0 ? -1 : 1;

  /* Two functions at the same address.  Aliases perhaps.  */
  i = strcmp ((*sb)->root.string, (*sa)->root.string);
  BFD_ASSERT (i != 0);
  return i;
}

/* Allocate space for overlay call and return stubs.  */

bfd_boolean
spu_elf_size_stubs (bfd *output_bfd,
		    struct bfd_link_info *info,
		    int non_overlay_stubs,
		    asection **stub,
		    asection **ovtab,
		    asection **toe)
{
  struct spu_link_hash_table *htab = spu_hash_table (info);
  bfd *ibfd;
  struct stubarr stubs;
  unsigned i, group;
  flagword flags;

  htab->non_overlay_stubs = non_overlay_stubs;
  stubs.count = 0;
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link_next)
    {
      extern const bfd_target bfd_elf32_spu_vec;
      Elf_Internal_Shdr *symtab_hdr;
      asection *section;
      Elf_Internal_Sym *local_syms = NULL;

      if (ibfd->xvec != &bfd_elf32_spu_vec)
	continue;

      /* We'll need the symbol table in a second.  */
      symtab_hdr = &elf_tdata (ibfd)->symtab_hdr;
      if (symtab_hdr->sh_info == 0)
	continue;

      /* Walk over each section attached to the input bfd.  */
      for (section = ibfd->sections; section != NULL; section = section->next)
	{
	  Elf_Internal_Rela *internal_relocs, *irelaend, *irela;

	  /* If there aren't any relocs, then there's nothing more to do.  */
	  if ((section->flags & SEC_RELOC) == 0
	      || (section->flags & SEC_ALLOC) == 0
	      || (section->flags & SEC_LOAD) == 0
	      || section->reloc_count == 0)
	    continue;

	  /* If this section is a link-once section that will be
	     discarded, then don't create any stubs.  */
	  if (section->output_section == NULL
	      || section->output_section->owner != output_bfd)
	    continue;

	  /* Get the relocs.  */
	  internal_relocs
	    = _bfd_elf_link_read_relocs (ibfd, section, NULL, NULL,
					 info->keep_memory);
	  if (internal_relocs == NULL)
	    goto error_ret_free_local;

	  /* Now examine each relocation.  */
	  irela = internal_relocs;
	  irelaend = irela + section->reloc_count;
	  for (; irela < irelaend; irela++)
	    {
	      enum elf_spu_reloc_type r_type;
	      unsigned int r_indx;
	      asection *sym_sec;
	      Elf_Internal_Sym *sym;
	      struct elf_link_hash_entry *h;
	      char *stub_name;
	      struct spu_stub_hash_entry *sh;
	      unsigned int sym_type;
	      enum _insn_type { non_branch, branch, call } insn_type;

	      r_type = ELF32_R_TYPE (irela->r_info);
	      r_indx = ELF32_R_SYM (irela->r_info);

	      if (r_type >= R_SPU_max)
		{
		  bfd_set_error (bfd_error_bad_value);
		  goto error_ret_free_internal;
		}

	      /* Determine the reloc target section.  */
	      if (!get_sym_h (&h, &sym, &sym_sec, &local_syms, r_indx, ibfd))
		goto error_ret_free_internal;

	      if (sym_sec == NULL
		  || sym_sec->output_section == NULL
		  || sym_sec->output_section->owner != output_bfd)
		continue;

	      /* Ensure no stubs for user supplied overlay manager syms.  */
	      if (h != NULL
		  && (strcmp (h->root.root.string, "__ovly_load") == 0
		      || strcmp (h->root.root.string, "__ovly_return") == 0))
		continue;

	      insn_type = non_branch;
	      if (r_type == R_SPU_REL16
		  || r_type == R_SPU_ADDR16)
		{
		  unsigned char insn[4];

		  if (!bfd_get_section_contents (ibfd, section, insn,
						 irela->r_offset, 4))
		    goto error_ret_free_internal;

		  if (is_branch (insn))
		    {
		      insn_type = branch;
		      if ((insn[0] & 0xfd) == 0x31)
			insn_type = call;
		    }
		}

	      /* We are only interested in function symbols.  */
	      if (h != NULL)
		sym_type = h->type;
	      else
		sym_type = ELF_ST_TYPE (sym->st_info);
	      if (sym_type != STT_FUNC)
		{
		  /* It's common for people to write assembly and forget
		     to give function symbols the right type.  Handle
		     calls to such symbols, but warn so that (hopefully)
		     people will fix their code.  We need the symbol
		     type to be correct to distinguish function pointer
		     initialisation from other pointer initialisation.  */
		  if (insn_type == call)
		    {
		      const char *sym_name;

		      if (h != NULL)
			sym_name = h->root.root.string;
		      else
			sym_name = bfd_elf_sym_name (sym_sec->owner,
						     symtab_hdr,
						     sym,
						     sym_sec);

		      (*_bfd_error_handler) (_("warning: call to non-function"
					       " symbol %s defined in %B"),
					     sym_name, sym_sec->owner);
		    }
		  else
		    continue;
		}

	      /* Usually, non-overlay sections don't need stubs.  */
	      if (!spu_elf_section_data (sym_sec->output_section)->ovl_index
		  && !non_overlay_stubs)
		continue;

	      /* We need a reference from some other section before
		 we consider that a symbol might need an overlay stub.  */
	      if (spu_elf_section_data (sym_sec->output_section)->ovl_index
		  == spu_elf_section_data (section->output_section)->ovl_index)
		{
		  /* Or we need this to *not* be a branch.  ie. We are
		     possibly taking the address of a function and
		     passing it out somehow.  */
		  if (insn_type != non_branch)
		    continue;
		}

	      stub_name = spu_stub_name (section, sym_sec, h, irela);
	      if (stub_name == NULL)
		goto error_ret_free_internal;

	      sh = (struct spu_stub_hash_entry *)
		bfd_hash_lookup (&htab->stub_hash_table, stub_name,
				 TRUE, FALSE);
	      if (sh == NULL)
		{
		  free (stub_name);
		error_ret_free_internal:
		  if (elf_section_data (section)->relocs != internal_relocs)
		    free (internal_relocs);
		error_ret_free_local:
		  if (local_syms != NULL
		      && (symtab_hdr->contents
			  != (unsigned char *) local_syms))
		    free (local_syms);
		  return FALSE;
		}

	      /* If this entry isn't new, we already have a stub.  */
	      if (sh->target_section != NULL)
		{
		  free (stub_name);
		  continue;
		}

	      sh->target_section = sym_sec;
	      if (h != NULL)
		sh->target_off = h->root.u.def.value;
	      else
		sh->target_off = sym->st_value;
	      sh->target_off += irela->r_addend;

	      stubs.count += 1;
	    }

	  /* We're done with the internal relocs, free them.  */
	  if (elf_section_data (section)->relocs != internal_relocs)
	    free (internal_relocs);
	}

      if (local_syms != NULL
	  && symtab_hdr->contents != (unsigned char *) local_syms)
	{
	  if (!info->keep_memory)
	    free (local_syms);
	  else
	    symtab_hdr->contents = (unsigned char *) local_syms;
	}
    }

  *stub = NULL;
  if (stubs.count == 0)
    return TRUE;

  ibfd = info->input_bfds;
  flags = (SEC_ALLOC | SEC_LOAD | SEC_CODE | SEC_READONLY
	   | SEC_HAS_CONTENTS | SEC_IN_MEMORY);
  htab->stub = bfd_make_section_anyway_with_flags (ibfd, ".stub", flags);
  *stub = htab->stub;
  if (htab->stub == NULL
      || !bfd_set_section_alignment (ibfd, htab->stub, 2))
    return FALSE;

  flags = (SEC_ALLOC | SEC_LOAD
	   | SEC_HAS_CONTENTS | SEC_IN_MEMORY);
  htab->ovtab = bfd_make_section_anyway_with_flags (ibfd, ".ovtab", flags);
  *ovtab = htab->ovtab;
  if (htab->ovtab == NULL
      || !bfd_set_section_alignment (ibfd, htab->stub, 4))
    return FALSE;

  *toe = bfd_make_section_anyway_with_flags (ibfd, ".toe", SEC_ALLOC);
  if (*toe == NULL
      || !bfd_set_section_alignment (ibfd, *toe, 4))
    return FALSE;
  (*toe)->size = 16;

  /* Retrieve all the stubs and sort.  */
  stubs.sh = bfd_malloc (stubs.count * sizeof (*stubs.sh));
  if (stubs.sh == NULL)
    return FALSE;
  i = stubs.count;
  bfd_hash_traverse (&htab->stub_hash_table, populate_stubs, &stubs);
  BFD_ASSERT (stubs.count == 0);

  stubs.count = i;
  qsort (stubs.sh, stubs.count, sizeof (*stubs.sh), sort_stubs);

  /* Now that the stubs are sorted, place them in the stub section.
     Stubs are grouped per overlay
     .	    ila $79,func1
     .	    br 1f
     .	    ila $79,func2
     .	    br 1f
     .
     .
     .	    ila $79,funcn
     .	    nop
     .	1:
     .	    ila $78,ovl_index
     .	    br __ovly_load  */

  group = 0;
  for (i = 0; i < stubs.count; i++)
    {
      if (spu_elf_section_data (stubs.sh[group]->target_section
				->output_section)->ovl_index
	  != spu_elf_section_data (stubs.sh[i]->target_section
				   ->output_section)->ovl_index)
	{
	  htab->stub->size += SIZEOF_STUB2;
	  for (; group != i; group++)
	    stubs.sh[group]->delta
	      = stubs.sh[i - 1]->off - stubs.sh[group]->off;
	}
      if (group == i
	  || ((stubs.sh[i - 1]->target_section->output_section->vma
	       + stubs.sh[i - 1]->target_section->output_offset
	       + stubs.sh[i - 1]->target_off)
	      != (stubs.sh[i]->target_section->output_section->vma
		  + stubs.sh[i]->target_section->output_offset
		  + stubs.sh[i]->target_off)))
	{
	  stubs.sh[i]->off = htab->stub->size;
	  htab->stub->size += SIZEOF_STUB1;
	}
      else
	stubs.sh[i]->off = stubs.sh[i - 1]->off;
    }
  if (group != i)
    htab->stub->size += SIZEOF_STUB2;
  for (; group != i; group++)
    stubs.sh[group]->delta = stubs.sh[i - 1]->off - stubs.sh[group]->off;

 /* htab->ovtab consists of two arrays.
    .	struct {
    .	  u32 vma;
    .	  u32 size;
    .	  u32 file_off;
    .	  u32 buf;
    .	} _ovly_table[];
    .
    .	struct {
    .	  u32 mapped;
    .	} _ovly_buf_table[];  */

  htab->ovtab->alignment_power = 4;
  htab->ovtab->size = htab->num_overlays * 16 + htab->num_buf * 4;

  return TRUE;
}

/* Functions to handle embedded spu_ovl.o object.  */

static void *
ovl_mgr_open (struct bfd *nbfd ATTRIBUTE_UNUSED, void *stream)
{
  return stream;
}

static file_ptr
ovl_mgr_pread (struct bfd *abfd ATTRIBUTE_UNUSED,
	       void *stream,
	       void *buf,
	       file_ptr nbytes,
	       file_ptr offset)
{
  struct _ovl_stream *os;
  size_t count;
  size_t max;

  os = (struct _ovl_stream *) stream;
  max = (char *) os->end - (char *) os->start;

  if ((ufile_ptr) offset >= max)
    return 0;

  count = nbytes;
  if (count > max - offset)
    count = max - offset;

  memcpy (buf, (char *) os->start + offset, count);
  return count;
}

bfd_boolean
spu_elf_open_builtin_lib (bfd **ovl_bfd, const struct _ovl_stream *stream)
{
  *ovl_bfd = bfd_openr_iovec ("builtin ovl_mgr",
			      "elf32-spu",
			      ovl_mgr_open,
			      (void *) stream,
			      ovl_mgr_pread,
			      NULL,
			      NULL);
  return *ovl_bfd != NULL;
}

/* Fill in the ila and br for a stub.  On the last stub for a group,
   write the stub that sets the overlay number too.  */

static bfd_boolean
write_one_stub (struct bfd_hash_entry *bh, void *inf)
{
  struct spu_stub_hash_entry *ent = (struct spu_stub_hash_entry *) bh;
  struct spu_link_hash_table *htab = inf;
  asection *sec = htab->stub;
  asection *s = ent->target_section;
  unsigned int ovl;
  bfd_vma val;

  val = ent->target_off + s->output_offset + s->output_section->vma;
  bfd_put_32 (sec->owner, ILA_79 + ((val << 7) & 0x01ffff80),
	      sec->contents + ent->off);
  val = ent->delta + 4;
  bfd_put_32 (sec->owner, BR + ((val << 5) & 0x007fff80),
	      sec->contents + ent->off + 4);

  /* If this is the last stub of this group, write stub2.  */
  if (ent->delta == 0)
    {
      bfd_put_32 (sec->owner, NOP,
		  sec->contents + ent->off + 4);

      ovl = spu_elf_section_data (s->output_section)->ovl_index;
      bfd_put_32 (sec->owner, ILA_78 + ((ovl << 7) & 0x01ffff80),
		  sec->contents + ent->off + 8);

      val = (htab->ovly_load->root.u.def.section->output_section->vma
	     + htab->ovly_load->root.u.def.section->output_offset
	     + htab->ovly_load->root.u.def.value
	     - (sec->output_section->vma
		+ sec->output_offset
		+ ent->off + 12));

      if (val + 0x20000 >= 0x40000)
	htab->stub_overflow = TRUE;

      bfd_put_32 (sec->owner, BR + ((val << 5) & 0x007fff80),
		  sec->contents + ent->off + 12);
    }

  if (htab->emit_stub_syms)
    {
      struct elf_link_hash_entry *h;
      size_t len1, len2;
      char *name;

      len1 = sizeof ("ovl_call.") - 1;
      len2 = strlen (ent->root.string);
      name = bfd_malloc (len1 + len2 + 1);
      if (name == NULL)
	return FALSE;
      memcpy (name, ent->root.string, 9);
      memcpy (name + 9, "ovl_call.", len1);
      memcpy (name + 9 + len1, ent->root.string + 9, len2 - 9 + 1);
      h = elf_link_hash_lookup (&htab->elf, name, TRUE, FALSE, FALSE);
      if (h == NULL)
	return FALSE;
      if (h->root.type == bfd_link_hash_new)
	{
	  h->root.type = bfd_link_hash_defined;
	  h->root.u.def.section = sec;
	  h->root.u.def.value = ent->off;
	  h->size = (ent->delta == 0
		     ? SIZEOF_STUB1 + SIZEOF_STUB2 : SIZEOF_STUB1);
	  h->type = STT_FUNC;
	  h->ref_regular = 1;
	  h->def_regular = 1;
	  h->ref_regular_nonweak = 1;
	  h->forced_local = 1;
	  h->non_elf = 0;
	}
    }

  return TRUE;
}

/* Define an STT_OBJECT symbol.  */

static struct elf_link_hash_entry *
define_ovtab_symbol (struct spu_link_hash_table *htab, const char *name)
{
  struct elf_link_hash_entry *h;

  h = elf_link_hash_lookup (&htab->elf, name, TRUE, FALSE, FALSE);
  if (h == NULL)
    return NULL;

  if (h->root.type != bfd_link_hash_defined
      || !h->def_regular)
    {
      h->root.type = bfd_link_hash_defined;
      h->root.u.def.section = htab->ovtab;
      h->type = STT_OBJECT;
      h->ref_regular = 1;
      h->def_regular = 1;
      h->ref_regular_nonweak = 1;
      h->non_elf = 0;
    }
  else
    {
      (*_bfd_error_handler) (_("%B is not allowed to define %s"),
			     h->root.u.def.section->owner,
			     h->root.root.string);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }

  return h;
}

/* Fill in all stubs and the overlay tables.  */

bfd_boolean
spu_elf_build_stubs (struct bfd_link_info *info, int emit_syms, asection *toe)
{
  struct spu_link_hash_table *htab = spu_hash_table (info);
  struct elf_link_hash_entry *h;
  bfd_byte *p;
  asection *s;
  bfd *obfd;
  unsigned int i;

  htab->emit_stub_syms = emit_syms;
  htab->stub->contents = bfd_zalloc (htab->stub->owner, htab->stub->size);
  if (htab->stub->contents == NULL)
    return FALSE;

  h = elf_link_hash_lookup (&htab->elf, "__ovly_load", FALSE, FALSE, FALSE);
  htab->ovly_load = h;
  BFD_ASSERT (h != NULL
	      && (h->root.type == bfd_link_hash_defined
		  || h->root.type == bfd_link_hash_defweak)
	      && h->def_regular);

  s = h->root.u.def.section->output_section;
  if (spu_elf_section_data (s)->ovl_index)
    {
      (*_bfd_error_handler) (_("%s in overlay section"),
			     h->root.u.def.section->owner);
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  /* Write out all the stubs.  */
  bfd_hash_traverse (&htab->stub_hash_table, write_one_stub, htab);

  if (htab->stub_overflow)
    {
      (*_bfd_error_handler) (_("overlay stub relocation overflow"));
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  htab->ovtab->contents = bfd_zalloc (htab->ovtab->owner, htab->ovtab->size);
  if (htab->ovtab->contents == NULL)
    return FALSE;

  /* Write out _ovly_table.  */
  p = htab->ovtab->contents;
  obfd = htab->ovtab->output_section->owner;
  for (s = obfd->sections; s != NULL; s = s->next)
    {
      unsigned int ovl_index = spu_elf_section_data (s)->ovl_index;

      if (ovl_index != 0)
	{
	  unsigned int lo, hi, mid;
	  unsigned long off = (ovl_index - 1) * 16;
	  bfd_put_32 (htab->ovtab->owner, s->vma, p + off);
	  bfd_put_32 (htab->ovtab->owner, (s->size + 15) & -16, p + off + 4);
	  /* file_off written later in spu_elf_modify_program_headers.  */

	  lo = 0;
	  hi = htab->num_buf;
	  while (lo < hi)
	    {
	      mid = (lo + hi) >> 1;
	      if (htab->ovl_region[2 * mid + 1]->vma
		  + htab->ovl_region[2 * mid + 1]->size <= s->vma)
		lo = mid + 1;
	      else if (htab->ovl_region[2 * mid]->vma > s->vma)
		hi = mid;
	      else
		{
		  bfd_put_32 (htab->ovtab->owner, mid + 1, p + off + 12);
		  break;
		}
	    }
	  BFD_ASSERT (lo < hi);
	}
    }

  /* Write out _ovly_buf_table.  */
  p = htab->ovtab->contents + htab->num_overlays * 16;
  for (i = 0; i < htab->num_buf; i++)
    {
      bfd_put_32 (htab->ovtab->owner, 0, p);
      p += 4;
    }

  h = define_ovtab_symbol (htab, "_ovly_table");
  if (h == NULL)
    return FALSE;
  h->root.u.def.value = 0;
  h->size = htab->num_overlays * 16;

  h = define_ovtab_symbol (htab, "_ovly_table_end");
  if (h == NULL)
    return FALSE;
  h->root.u.def.value = htab->num_overlays * 16;
  h->size = 0;

  h = define_ovtab_symbol (htab, "_ovly_buf_table");
  if (h == NULL)
    return FALSE;
  h->root.u.def.value = htab->num_overlays * 16;
  h->size = htab->num_buf * 4;

  h = define_ovtab_symbol (htab, "_ovly_buf_table_end");
  if (h == NULL)
    return FALSE;
  h->root.u.def.value = htab->num_overlays * 16 + htab->num_buf * 4;
  h->size = 0;

  h = define_ovtab_symbol (htab, "_EAR_");
  if (h == NULL)
    return FALSE;
  h->root.u.def.section = toe;
  h->root.u.def.value = 0;
  h->size = 16;

  return TRUE;
}

/* Apply RELOCS to CONTENTS of INPUT_SECTION from INPUT_BFD.  */

static bfd_boolean
spu_elf_relocate_section (bfd *output_bfd,
			  struct bfd_link_info *info,
			  bfd *input_bfd,
			  asection *input_section,
			  bfd_byte *contents,
			  Elf_Internal_Rela *relocs,
			  Elf_Internal_Sym *local_syms,
			  asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel, *relend;
  struct spu_link_hash_table *htab;
  bfd_boolean ret = TRUE;

  if (info->relocatable)
    return TRUE;

  htab = spu_hash_table (info);
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = (struct elf_link_hash_entry **) (elf_sym_hashes (input_bfd));

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
      const char *sym_name;
      bfd_vma relocation;
      bfd_vma addend;
      bfd_reloc_status_type r;
      bfd_boolean unresolved_reloc;
      bfd_boolean warned;

      r_symndx = ELF32_R_SYM (rel->r_info);
      r_type = ELF32_R_TYPE (rel->r_info);
      howto = elf_howto_table + r_type;
      unresolved_reloc = FALSE;
      warned = FALSE;

      h = NULL;
      sym = NULL;
      sec = NULL;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  sym_name = bfd_elf_sym_name (input_bfd, symtab_hdr, sym, sec);
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);
	}
      else
	{
	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned);
	  sym_name = h->root.root.string;
	}

      if (unresolved_reloc)
	{
	  (*_bfd_error_handler)
	    (_("%B(%s+0x%lx): unresolvable %s relocation against symbol `%s'"),
	     input_bfd,
	     bfd_get_section_name (input_bfd, input_section),
	     (long) rel->r_offset,
	     howto->name,
	     sym_name);
	  ret = FALSE;
	}

      /* If this symbol is in an overlay area, we may need to relocate
	 to the overlay stub.  */
      addend = rel->r_addend;
      if (sec != NULL
	  && sec->output_section != NULL
	  && sec->output_section->owner == output_bfd
	  && (spu_elf_section_data (sec->output_section)->ovl_index != 0
	      || htab->non_overlay_stubs)
	  && !(sec == input_section
	       && is_branch (contents + rel->r_offset)))
	{
	  char *stub_name;
	  struct spu_stub_hash_entry *sh;

	  stub_name = spu_stub_name (input_section, sec, h, rel);
	  if (stub_name == NULL)
	    return FALSE;

	  sh = (struct spu_stub_hash_entry *)
	    bfd_hash_lookup (&htab->stub_hash_table, stub_name, FALSE, FALSE);
	  if (sh != NULL)
	    {
	      relocation = (htab->stub->output_section->vma
			    + htab->stub->output_offset
			    + sh->off);
	      addend = 0;
	    }
	  free (stub_name);
	}

      r = _bfd_final_link_relocate (howto,
				    input_bfd,
				    input_section,
				    contents,
				    rel->r_offset, relocation, addend);

      if (r != bfd_reloc_ok)
	{
	  const char *msg = (const char *) 0;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      if (!((*info->callbacks->reloc_overflow)
		    (info, (h ? &h->root : NULL), sym_name, howto->name,
		     (bfd_vma) 0, input_bfd, input_section, rel->r_offset)))
		return FALSE;
	      break;

	    case bfd_reloc_undefined:
	      if (!((*info->callbacks->undefined_symbol)
		    (info, sym_name, input_bfd, input_section,
		     rel->r_offset, TRUE)))
		return FALSE;
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      goto common_error;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      goto common_error;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous error");
	      goto common_error;

	    default:
	      msg = _("internal error: unknown error");
	      /* fall through */

	    common_error:
	      if (!((*info->callbacks->warning)
		    (info, msg, sym_name, input_bfd, input_section,
		     rel->r_offset)))
		return FALSE;
	      break;
	    }
	}
    }

  return ret;
}

static int spu_plugin = 0;

void
spu_elf_plugin (int val)
{
  spu_plugin = val;
}

/* Set ELF header e_type for plugins.  */

static void
spu_elf_post_process_headers (bfd *abfd,
			      struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
  if (spu_plugin)
    {
      Elf_Internal_Ehdr *i_ehdrp = elf_elfheader (abfd);

      i_ehdrp->e_type = ET_DYN;
    }
}

/* We may add an extra PT_LOAD segment for .toe.  We also need extra
   segments for overlays.  */

static int
spu_elf_additional_program_headers (bfd *abfd, struct bfd_link_info *info)
{
  struct spu_link_hash_table *htab = spu_hash_table (info);
  int extra = htab->num_overlays;
  asection *sec;

  if (extra)
    ++extra;

  sec = bfd_get_section_by_name (abfd, ".toe");
  if (sec != NULL && (sec->flags & SEC_LOAD) != 0)
    ++extra;

  return extra;
}

/* Remove .toe section from other PT_LOAD segments and put it in
   a segment of its own.  Put overlays in separate segments too.  */

static bfd_boolean
spu_elf_modify_segment_map (bfd *abfd, struct bfd_link_info *info)
{
  asection *toe, *s;
  struct elf_segment_map *m;
  unsigned int i;

  if (info == NULL)
    return TRUE;

  toe = bfd_get_section_by_name (abfd, ".toe");
  for (m = elf_tdata (abfd)->segment_map; m != NULL; m = m->next)
    if (m->p_type == PT_LOAD && m->count > 1)
      for (i = 0; i < m->count; i++)
	if ((s = m->sections[i]) == toe
	    || spu_elf_section_data (s)->ovl_index != 0)
	  {
	    struct elf_segment_map *m2;
	    bfd_vma amt;

	    if (i + 1 < m->count)
	      {
		amt = sizeof (struct elf_segment_map);
		amt += (m->count - (i + 2)) * sizeof (m->sections[0]);
		m2 = bfd_zalloc (abfd, amt);
		if (m2 == NULL)
		  return FALSE;
		m2->count = m->count - (i + 1);
		memcpy (m2->sections, m->sections + i + 1,
			m2->count * sizeof (m->sections[0]));
		m2->p_type = PT_LOAD;
		m2->next = m->next;
		m->next = m2;
	      }
	    m->count = 1;
	    if (i != 0)
	      {
		m->count = i;
		amt = sizeof (struct elf_segment_map);
		m2 = bfd_zalloc (abfd, amt);
		if (m2 == NULL)
		  return FALSE;
		m2->p_type = PT_LOAD;
		m2->count = 1;
		m2->sections[0] = s;
		m2->next = m->next;
		m->next = m2;
	      }
	    break;
	  }

  return TRUE;
}

/* Check that all loadable section VMAs lie in the range
   LO .. HI inclusive.  */

asection *
spu_elf_check_vma (bfd *abfd, bfd_vma lo, bfd_vma hi)
{
  struct elf_segment_map *m;
  unsigned int i;

  for (m = elf_tdata (abfd)->segment_map; m != NULL; m = m->next)
    if (m->p_type == PT_LOAD)
      for (i = 0; i < m->count; i++)
	if (m->sections[i]->size != 0
	    && (m->sections[i]->vma < lo
		|| m->sections[i]->vma > hi
		|| m->sections[i]->vma + m->sections[i]->size - 1 > hi))
	  return m->sections[i];

  return NULL;
}

/* Tweak phdrs before writing them out.  */

static int
spu_elf_modify_program_headers (bfd *abfd, struct bfd_link_info *info)
{
  const struct elf_backend_data *bed;
  struct elf_obj_tdata *tdata;
  Elf_Internal_Phdr *phdr, *last;
  struct spu_link_hash_table *htab;
  unsigned int count;
  unsigned int i;

  if (info == NULL)
    return TRUE;

  bed = get_elf_backend_data (abfd);
  tdata = elf_tdata (abfd);
  phdr = tdata->phdr;
  count = tdata->program_header_size / bed->s->sizeof_phdr;
  htab = spu_hash_table (info);
  if (htab->num_overlays != 0)
    {
      struct elf_segment_map *m;
      unsigned int o;

      for (i = 0, m = elf_tdata (abfd)->segment_map; m; ++i, m = m->next)
	if (m->count != 0
	    && (o = spu_elf_section_data (m->sections[0])->ovl_index) != 0)
	  {
	    /* Mark this as an overlay header.  */
	    phdr[i].p_flags |= PF_OVERLAY;

	    if (htab->ovtab != NULL && htab->ovtab->size != 0)
	      {
		bfd_byte *p = htab->ovtab->contents;
		unsigned int off = (o - 1) * 16 + 8;

		/* Write file_off into _ovly_table.  */
		bfd_put_32 (htab->ovtab->owner, phdr[i].p_offset, p + off);
	      }
	  }
    }

  /* Round up p_filesz and p_memsz of PT_LOAD segments to multiples
     of 16.  This should always be possible when using the standard
     linker scripts, but don't create overlapping segments if
     someone is playing games with linker scripts.  */
  last = NULL;
  for (i = count; i-- != 0; )
    if (phdr[i].p_type == PT_LOAD)
      {
	unsigned adjust;

	adjust = -phdr[i].p_filesz & 15;
	if (adjust != 0
	    && last != NULL
	    && phdr[i].p_offset + phdr[i].p_filesz > last->p_offset - adjust)
	  break;

	adjust = -phdr[i].p_memsz & 15;
	if (adjust != 0
	    && last != NULL
	    && phdr[i].p_filesz != 0
	    && phdr[i].p_vaddr + phdr[i].p_memsz > last->p_vaddr - adjust
	    && phdr[i].p_vaddr + phdr[i].p_memsz <= last->p_vaddr)
	  break;

	if (phdr[i].p_filesz != 0)
	  last = &phdr[i];
      }

  if (i == (unsigned int) -1)
    for (i = count; i-- != 0; )
      if (phdr[i].p_type == PT_LOAD)
	{
	unsigned adjust;

	adjust = -phdr[i].p_filesz & 15;
	phdr[i].p_filesz += adjust;

	adjust = -phdr[i].p_memsz & 15;
	phdr[i].p_memsz += adjust;
      }

  return TRUE;
}

/* Arrange for our linker created section to be output.  */

static bfd_boolean
spu_elf_section_processing (bfd *abfd ATTRIBUTE_UNUSED,
			    Elf_Internal_Shdr *i_shdrp)
{
  asection *sec;

  sec = i_shdrp->bfd_section;
  if (sec != NULL
      && (sec->flags & SEC_LINKER_CREATED) != 0
      && sec->name != NULL
      && strcmp (sec->name, SPU_PTNOTE_SPUNAME) == 0)
    i_shdrp->contents = sec->contents;

  return TRUE;
}

#define TARGET_BIG_SYM		bfd_elf32_spu_vec
#define TARGET_BIG_NAME		"elf32-spu"
#define ELF_ARCH		bfd_arch_spu
#define ELF_MACHINE_CODE	EM_SPU
/* This matches the alignment need for DMA.  */
#define ELF_MAXPAGESIZE		0x80
#define elf_backend_rela_normal         1
#define elf_backend_can_gc_sections	1

#define bfd_elf32_bfd_reloc_type_lookup		spu_elf_reloc_type_lookup
#define elf_info_to_howto			spu_elf_info_to_howto
#define elf_backend_gc_mark_hook		spu_elf_gc_mark_hook
#define elf_backend_relocate_section		spu_elf_relocate_section
#define elf_backend_symbol_processing		spu_elf_backend_symbol_processing
#define bfd_elf32_new_section_hook		spu_elf_new_section_hook
#define bfd_elf32_bfd_link_hash_table_create	spu_elf_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_free	spu_elf_link_hash_table_free

#define elf_backend_additional_program_headers	spu_elf_additional_program_headers
#define elf_backend_modify_segment_map		spu_elf_modify_segment_map
#define elf_backend_modify_program_headers	spu_elf_modify_program_headers
#define elf_backend_post_process_headers        spu_elf_post_process_headers
#define elf_backend_section_processing		spu_elf_section_processing
#define elf_backend_special_sections		spu_elf_special_sections

#include "elf32-target.h"
