/* x86 specific support for ELF
   Copyright (C) 2017 Free Software Foundation, Inc.

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

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "bfd_stdint.h"
#include "hashtab.h"

#define PLT_CIE_LENGTH		20
#define PLT_FDE_LENGTH		36
#define PLT_FDE_START_OFFSET	4 + PLT_CIE_LENGTH + 8
#define PLT_FDE_LEN_OFFSET	4 + PLT_CIE_LENGTH + 12

#define ABI_64_P(abfd) \
  (get_elf_backend_data (abfd)->s->elfclass == ELFCLASS64)

/* If ELIMINATE_COPY_RELOCS is non-zero, the linker will try to avoid
   copying dynamic variables from a shared lib into an app's dynbss
   section, and instead use a dynamic relocation to point into the
   shared lib.  */
#define ELIMINATE_COPY_RELOCS 1

#define elf_x86_hash_table(p, id) \
  (is_elf_hash_table ((p)->hash) \
   && elf_hash_table_id ((struct elf_link_hash_table *) ((p)->hash)) == (id) \
    ? ((struct elf_x86_link_hash_table *) ((p)->hash)) : NULL)

/* Will references to this symbol always be local in this object?  */
#define SYMBOL_REFERENCES_LOCAL_P(INFO, H) \
  _bfd_x86_elf_link_symbol_references_local ((INFO), (H))

/* Is a undefined weak symbol which is resolved to 0.  Reference to an
   undefined weak symbol is resolved to 0 when building executable if
   it isn't dynamic and
   1. Has non-GOT/non-PLT relocations in text section.  Or
   2. Has no GOT/PLT relocation.
   Local undefined weak symbol is always resolved to 0.
 */
#define UNDEFINED_WEAK_RESOLVED_TO_ZERO(INFO, EH) \
  ((EH)->elf.root.type == bfd_link_hash_undefweak		 \
   && (SYMBOL_REFERENCES_LOCAL_P ((INFO), &(EH)->elf)		 \
       || (bfd_link_executable (INFO)				 \
	   && (!(EH)->has_got_reloc				 \
	       || (EH)->has_non_got_reloc))))

/* Should copy relocation be generated for a symbol.  Don't generate
   copy relocation against a protected symbol defined in a shared
   object with GNU_PROPERTY_NO_COPY_ON_PROTECTED.  */
#define SYMBOL_NO_COPYRELOC(INFO, EH) \
  ((EH)->def_protected \
   && ((EH)->elf.root.type == bfd_link_hash_defined \
       || (EH)->elf.root.type == bfd_link_hash_defweak) \
   && elf_has_no_copy_on_protected ((EH)->elf.root.u.def.section->owner) \
   && ((EH)->elf.root.u.def.section->owner->flags & DYNAMIC) != 0 \
   && ((EH)->elf.root.u.def.section->flags & SEC_CODE) == 0)

/* x86 ELF linker hash entry.  */

struct elf_x86_link_hash_entry
{
  struct elf_link_hash_entry elf;

  /* Track dynamic relocs copied for this symbol.  */
  struct elf_dyn_relocs *dyn_relocs;

  unsigned char tls_type;

  /* TRUE if symbol has GOT or PLT relocations.  */
  unsigned int has_got_reloc : 1;

  /* TRUE if symbol has non-GOT/non-PLT relocations in text sections.  */
  unsigned int has_non_got_reloc : 1;

  /* Don't call finish_dynamic_symbol on this symbol.  */
  unsigned int no_finish_dynamic_symbol : 1;

  /* TRUE if symbol is __tls_get_addr.  */
  unsigned int tls_get_addr : 1;

  /* TRUE if symbol is defined as a protected symbol.  */
  unsigned int def_protected : 1;

  /* 0: Symbol references are unknown.
     1: Symbol references aren't local.
     2: Symbol references are local.
   */
  unsigned int local_ref : 2;

  /* TRUE if symbol is defined by linker.  */
  unsigned int linker_def : 1;

  /* TRUE if symbol is referenced by R_386_GOTOFF relocation.  This is
     only used by i386.  */
  unsigned int gotoff_ref : 1;

  /* TRUE if a weak symbol with a real definition needs a copy reloc.
     When there is a weak symbol with a real definition, the processor
     independent code will have arranged for us to see the real
     definition first.  We need to copy the needs_copy bit from the
     real definition and check it when allowing copy reloc in PIE.  This
     is only used by x86-64.  */
  unsigned int needs_copy : 1;

  /* Reference count of C/C++ function pointer relocations in read-write
     section which can be resolved at run-time.  */
  bfd_signed_vma func_pointer_refcount;

  /* Information about the GOT PLT entry. Filled when there are both
     GOT and PLT relocations against the same function.  */
  union gotplt_union plt_got;

  /* Information about the second PLT entry.   */
  union gotplt_union plt_second;

  /* Offset of the GOTPLT entry reserved for the TLS descriptor,
     starting at the end of the jump table.  */
  bfd_vma tlsdesc_got;
};

struct elf_x86_lazy_plt_layout
{
  /* The first entry in an absolute lazy procedure linkage table looks
     like this.  */
  const bfd_byte *plt0_entry;
  unsigned int plt0_entry_size;          /* Size of PLT0 entry.  */

  /* Later entries in an absolute lazy procedure linkage table look
     like this.  */
  const bfd_byte *plt_entry;
  unsigned int plt_entry_size;          /* Size of each PLT entry.  */

  /* Offsets into plt0_entry that are to be replaced with GOT[1] and
     GOT[2].  */
  unsigned int plt0_got1_offset;
  unsigned int plt0_got2_offset;

  /* Offset of the end of the PC-relative instruction containing
     plt0_got2_offset.  This is for x86-64 only.  */
  unsigned int plt0_got2_insn_end;

  /* Offsets into plt_entry that are to be replaced with...  */
  unsigned int plt_got_offset;    /* ... address of this symbol in .got. */
  unsigned int plt_reloc_offset;  /* ... offset into relocation table. */
  unsigned int plt_plt_offset;    /* ... offset to start of .plt. */

  /* Length of the PC-relative instruction containing plt_got_offset.
     This is used for x86-64 only.  */
  unsigned int plt_got_insn_size;

  /* Offset of the end of the PC-relative jump to plt0_entry.  This is
     used for x86-64 only.  */
  unsigned int plt_plt_insn_end;

  /* Offset into plt_entry where the initial value of the GOT entry
     points.  */
  unsigned int plt_lazy_offset;

  /* The first entry in a PIC lazy procedure linkage table looks like
     this.  */
  const bfd_byte *pic_plt0_entry;

  /* Subsequent entries in a PIC lazy procedure linkage table look
     like this.  */
  const bfd_byte *pic_plt_entry;

  /* .eh_frame covering the lazy .plt section.  */
  const bfd_byte *eh_frame_plt;
  unsigned int eh_frame_plt_size;
};

struct elf_x86_non_lazy_plt_layout
{
  /* Entries in an absolute non-lazy procedure linkage table look like
     this.  */
  const bfd_byte *plt_entry;
  /* Entries in a PIC non-lazy procedure linkage table look like this.  */
  const bfd_byte *pic_plt_entry;

  unsigned int plt_entry_size;          /* Size of each PLT entry.  */

  /* Offsets into plt_entry that are to be replaced with...  */
  unsigned int plt_got_offset;    /* ... address of this symbol in .got. */

  /* Length of the PC-relative instruction containing plt_got_offset.
     This is used for x86-64 only.  */
  unsigned int plt_got_insn_size;

  /* .eh_frame covering the non-lazy .plt section.  */
  const bfd_byte *eh_frame_plt;
  unsigned int eh_frame_plt_size;
};

struct elf_x86_plt_layout
{
  /* The first entry in a lazy procedure linkage table looks like this.
     This is only used for i386 where absolute PLT0 and PIC PLT0 are
     different.  */
  const bfd_byte *plt0_entry;
  /* Entries in a procedure linkage table look like this.  */
  const bfd_byte *plt_entry;
  unsigned int plt_entry_size;          /* Size of each PLT entry.  */

  /* 1 has PLT0.  */
  unsigned int has_plt0;

  /* Offsets into plt_entry that are to be replaced with...  */
  unsigned int plt_got_offset;    /* ... address of this symbol in .got. */

  /* Length of the PC-relative instruction containing plt_got_offset.
     This is only used for x86-64.  */
  unsigned int plt_got_insn_size;

  /* .eh_frame covering the .plt section.  */
  const bfd_byte *eh_frame_plt;
  unsigned int eh_frame_plt_size;
};

/* Values in tls_type of x86 ELF linker hash entry.  */
#define GOT_UNKNOWN	0
#define GOT_NORMAL	1
#define GOT_TLS_GD	2
#define GOT_TLS_IE	4
#define GOT_TLS_IE_POS	5
#define GOT_TLS_IE_NEG	6
#define GOT_TLS_IE_BOTH 7
#define GOT_TLS_GDESC	8
#define GOT_TLS_GD_BOTH_P(type)	\
  ((type) == (GOT_TLS_GD | GOT_TLS_GDESC))
#define GOT_TLS_GD_P(type) \
  ((type) == GOT_TLS_GD || GOT_TLS_GD_BOTH_P (type))
#define GOT_TLS_GDESC_P(type) \
  ((type) == GOT_TLS_GDESC || GOT_TLS_GD_BOTH_P (type))
#define GOT_TLS_GD_ANY_P(type) \
  (GOT_TLS_GD_P (type) || GOT_TLS_GDESC_P (type))

#define elf_x86_hash_entry(ent) \
  ((struct elf_x86_link_hash_entry *)(ent))

/* x86 ELF linker hash table.  */

struct elf_x86_link_hash_table
{
  struct elf_link_hash_table elf;

  /* Short-cuts to get to dynamic linker sections.  */
  asection *interp;
  asection *plt_eh_frame;
  asection *plt_second;
  asection *plt_second_eh_frame;
  asection *plt_got;
  asection *plt_got_eh_frame;

  /* Parameters describing PLT generation, lazy or non-lazy.  */
  struct elf_x86_plt_layout plt;

  /* Parameters describing lazy PLT generation.  */
  const struct elf_x86_lazy_plt_layout *lazy_plt;

  /* Parameters describing non-lazy PLT generation.  */
  const struct elf_x86_non_lazy_plt_layout *non_lazy_plt;

  union
  {
    bfd_signed_vma refcount;
    bfd_vma offset;
  } tls_ld_or_ldm_got;

  /* The amount of space used by the jump slots in the GOT.  */
  bfd_vma sgotplt_jump_table_size;

  /* Small local sym cache.  */
  struct sym_cache sym_cache;

  /* _TLS_MODULE_BASE_ symbol.  */
  struct bfd_link_hash_entry *tls_module_base;

  /* Used by local STT_GNU_IFUNC symbols.  */
  htab_t loc_hash_table;
  void * loc_hash_memory;

  /* The offset into sgot of the GOT entry used by the PLT entry
     above.  */
  bfd_vma tlsdesc_got;

  /* The index of the next R_X86_64_JUMP_SLOT entry in .rela.plt.  */
  bfd_vma next_jump_slot_index;
  /* The index of the next R_X86_64_IRELATIVE entry in .rela.plt.  */
  bfd_vma next_irelative_index;

  /* TRUE if there are dynamic relocs against IFUNC symbols that apply
     to read-only sections.  */
  bfd_boolean readonly_dynrelocs_against_ifunc;

  /* TRUE if this is a VxWorks x86 target.  This is only used for
     i386.  */
  bfd_boolean is_vxworks;

  /* The (unloaded but important) .rel.plt.unloaded section on VxWorks.
     This is used for i386 only.  */
  asection *srelplt2;

  /* The index of the next unused R_386_TLS_DESC slot in .rel.plt.  This
     is only used for i386.  */
  bfd_vma next_tls_desc_index;

  /* The offset into splt of the PLT entry for the TLS descriptor
     resolver.  Special values are 0, if not necessary (or not found
     to be necessary yet), and -1 if needed but not determined
     yet.  This is only used for x86-64.  */
  bfd_vma tlsdesc_plt;

  bfd_vma (*r_info) (bfd_vma, bfd_vma);
  bfd_vma (*r_sym) (bfd_vma);
  bfd_boolean (*is_reloc_section) (const char *);
  enum elf_target_id target_id;
  unsigned int sizeof_reloc;
  unsigned int dt_reloc;
  unsigned int dt_reloc_sz;
  unsigned int dt_reloc_ent;
  unsigned int got_entry_size;
  unsigned int pointer_r_type;
  int dynamic_interpreter_size;
  const char *dynamic_interpreter;
  const char *tls_get_addr;
};

struct elf_x86_init_table
{
  /* The lazy PLT layout.  */
  const struct elf_x86_lazy_plt_layout *lazy_plt;

  /* The non-lazy PLT layout.  */
  const struct elf_x86_non_lazy_plt_layout *non_lazy_plt;

  /* The lazy PLT layout for IBT.  */
  const struct elf_x86_lazy_plt_layout *lazy_ibt_plt;

  /* The non-lazy PLT layout for IBT.  */
  const struct elf_x86_non_lazy_plt_layout *non_lazy_ibt_plt;

  /* TRUE if this is a normal x86 target.  */
  bfd_boolean normal_target;

  /* TRUE if this is a VxWorks x86 target.  */
  bfd_boolean is_vxworks;

  bfd_vma (*r_info) (bfd_vma, bfd_vma);
  bfd_vma (*r_sym) (bfd_vma);
};

struct elf_x86_obj_tdata
{
  struct elf_obj_tdata root;

  /* tls_type for each local got entry.  */
  char *local_got_tls_type;

  /* GOTPLT entries for TLS descriptors.  */
  bfd_vma *local_tlsdesc_gotent;
};

enum elf_x86_plt_type
{
  plt_non_lazy = 0,
  plt_lazy = 1 << 0,
  plt_pic = 1 << 1,
  plt_second = 1 << 2,
  plt_unknown = -1
};

struct elf_x86_plt
{
  const char *name;
  asection *sec;
  bfd_byte *contents;
  enum elf_x86_plt_type type;
  unsigned int plt_got_offset;
  unsigned int plt_entry_size;
  unsigned int plt_got_insn_size;	/* Only used for x86-64.  */
  long count;
};

#define elf_x86_tdata(abfd) \
  ((struct elf_x86_obj_tdata *) (abfd)->tdata.any)

#define elf_x86_local_got_tls_type(abfd) \
  (elf_x86_tdata (abfd)->local_got_tls_type)

#define elf_x86_local_tlsdesc_gotent(abfd) \
  (elf_x86_tdata (abfd)->local_tlsdesc_gotent)

#define elf_x86_compute_jump_table_size(htab) \
  ((htab)->elf.srelplt->reloc_count * (htab)->got_entry_size)

#define is_x86_elf(bfd, htab)				\
  (bfd_get_flavour (bfd) == bfd_target_elf_flavour	\
   && elf_tdata (bfd) != NULL				\
   && elf_object_id (bfd) == (htab)->target_id)

extern bfd_boolean _bfd_x86_elf_mkobject
  (bfd *);

extern void _bfd_x86_elf_set_tls_module_base
  (struct bfd_link_info *);

extern bfd_vma _bfd_x86_elf_dtpoff_base
  (struct bfd_link_info *);

extern bfd_boolean _bfd_x86_elf_readonly_dynrelocs
  (struct elf_link_hash_entry *, void *);

extern struct elf_link_hash_entry * _bfd_elf_x86_get_local_sym_hash
  (struct elf_x86_link_hash_table *, bfd *, const Elf_Internal_Rela *,
   bfd_boolean);

extern hashval_t _bfd_x86_elf_local_htab_hash
  (const void *);

extern int _bfd_x86_elf_local_htab_eq
  (const void *, const void *);

extern struct bfd_hash_entry * _bfd_x86_elf_link_hash_newfunc
  (struct bfd_hash_entry *, struct bfd_hash_table *, const char *);

extern struct bfd_link_hash_table * _bfd_x86_elf_link_hash_table_create
  (bfd *);

extern int _bfd_x86_elf_compare_relocs
  (const void *, const void *);

extern bfd_boolean _bfd_x86_elf_link_check_relocs
  (bfd *, struct bfd_link_info *);

extern bfd_boolean _bfd_x86_elf_size_dynamic_sections
  (bfd *, struct bfd_link_info *);

extern bfd_boolean _bfd_x86_elf_always_size_sections
  (bfd *, struct bfd_link_info *);

extern void _bfd_x86_elf_merge_symbol_attribute
  (struct elf_link_hash_entry *, const Elf_Internal_Sym *,
   bfd_boolean, bfd_boolean);

extern void _bfd_x86_elf_copy_indirect_symbol
  (struct bfd_link_info *, struct elf_link_hash_entry *,
   struct elf_link_hash_entry *);

extern bfd_boolean _bfd_x86_elf_fixup_symbol
  (struct bfd_link_info *, struct elf_link_hash_entry *);

extern bfd_boolean _bfd_x86_elf_hash_symbol
  (struct elf_link_hash_entry *);

extern bfd_boolean _bfd_x86_elf_adjust_dynamic_symbol
  (struct bfd_link_info *, struct elf_link_hash_entry *);

extern bfd_boolean _bfd_x86_elf_link_symbol_references_local
  (struct bfd_link_info *, struct elf_link_hash_entry *);

extern asection * _bfd_x86_elf_gc_mark_hook
  (asection *, struct bfd_link_info *, Elf_Internal_Rela *,
   struct elf_link_hash_entry *, Elf_Internal_Sym *);

extern long _bfd_x86_elf_get_synthetic_symtab
  (bfd *, long, long, bfd_vma, struct elf_x86_plt [], asymbol **,
   asymbol **);

extern enum elf_property_kind _bfd_x86_elf_parse_gnu_properties
  (bfd *, unsigned int, bfd_byte *, unsigned int);

extern bfd_boolean _bfd_x86_elf_merge_gnu_properties
  (struct bfd_link_info *, bfd *, elf_property *, elf_property *);

extern bfd * _bfd_x86_elf_link_setup_gnu_properties
  (struct bfd_link_info *, struct elf_x86_init_table *);

#define bfd_elf64_mkobject \
  _bfd_x86_elf_mkobject
#define bfd_elf32_mkobject \
  _bfd_x86_elf_mkobject
#define bfd_elf64_bfd_link_hash_table_create \
  _bfd_x86_elf_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_create \
  _bfd_x86_elf_link_hash_table_create
#define bfd_elf64_bfd_link_check_relocs	\
  _bfd_x86_elf_link_check_relocs
#define bfd_elf32_bfd_link_check_relocs \
  _bfd_x86_elf_link_check_relocs

#define elf_backend_size_dynamic_sections \
  _bfd_x86_elf_size_dynamic_sections
#define elf_backend_always_size_sections \
  _bfd_x86_elf_always_size_sections
#define elf_backend_merge_symbol_attribute \
  _bfd_x86_elf_merge_symbol_attribute
#define elf_backend_copy_indirect_symbol \
  _bfd_x86_elf_copy_indirect_symbol
#define elf_backend_fixup_symbol \
  _bfd_x86_elf_fixup_symbol
#define elf_backend_hash_symbol \
  _bfd_x86_elf_hash_symbol
#define elf_backend_adjust_dynamic_symbol \
  _bfd_x86_elf_adjust_dynamic_symbol
#define elf_backend_gc_mark_hook \
  _bfd_x86_elf_gc_mark_hook
#define elf_backend_omit_section_dynsym \
  ((bfd_boolean (*) (bfd *, struct bfd_link_info *, asection *)) bfd_true)
#define elf_backend_parse_gnu_properties \
  _bfd_x86_elf_parse_gnu_properties
#define elf_backend_merge_gnu_properties \
  _bfd_x86_elf_merge_gnu_properties
