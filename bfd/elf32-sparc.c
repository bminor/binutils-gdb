/* SPARC-specific support for 32-bit ELF
   Copyright 1993 Free Software Foundation, Inc.

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
#include "bfdlink.h"
#include "libbfd.h"
#include "libelf.h"

static CONST struct reloc_howto_struct *bfd_elf32_bfd_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));
static void elf_info_to_howto
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
static boolean elf32_sparc_create_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean elf32_sparc_adjust_dynamic_symbol
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));
static boolean elf32_sparc_allocate_dynamic_section
  PARAMS ((bfd *, const char *));
static boolean elf32_sparc_size_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean elf32_sparc_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, Elf_Internal_Sym *, asection **, char *));
static boolean elf32_sparc_finish_dynamic_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct elf_link_hash_entry *,
	   Elf_Internal_Sym *));
static boolean elf32_sparc_finish_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));

enum reloc_type
  {
    R_SPARC_NONE = 0,
    R_SPARC_8,		R_SPARC_16,		R_SPARC_32, 
    R_SPARC_DISP8,	R_SPARC_DISP16,		R_SPARC_DISP32, 
    R_SPARC_WDISP30,	R_SPARC_WDISP22,
    R_SPARC_HI22,	R_SPARC_22,
    R_SPARC_13,		R_SPARC_LO10,
    R_SPARC_GOT10,	R_SPARC_GOT13,		R_SPARC_GOT22,
    R_SPARC_PC10,	R_SPARC_PC22,
    R_SPARC_WPLT30,
    R_SPARC_COPY,
    R_SPARC_GLOB_DAT,	R_SPARC_JMP_SLOT,
    R_SPARC_RELATIVE,
    R_SPARC_UA32,
    R_SPARC_max
  };

#if 0
static CONST char *CONST reloc_type_names[] =
{
  "R_SPARC_NONE",
  "R_SPARC_8",		"R_SPARC_16",		"R_SPARC_32",
  "R_SPARC_DISP8",	"R_SPARC_DISP16",	"R_SPARC_DISP32",
  "R_SPARC_WDISP30",	"R_SPARC_WDISP22",
  "R_SPARC_HI22",	"R_SPARC_22",
  "R_SPARC_13",		"R_SPARC_LO10",
  "R_SPARC_GOT10",	"R_SPARC_GOT13",	"R_SPARC_GOT22",
  "R_SPARC_PC10",	"R_SPARC_PC22",
  "R_SPARC_WPLT30",
  "R_SPARC_COPY",
  "R_SPARC_GLOB_DAT",	"R_SPARC_JMP_SLOT",
  "R_SPARC_RELATIVE",
  "R_SPARC_UA32",
};
#endif

static reloc_howto_type elf_sparc_howto_table[] = 
{
  HOWTO(R_SPARC_NONE,    0,0, 0,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_NONE",    false,0,0x00000000,true),
  HOWTO(R_SPARC_8,       0,0, 8,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_8",       false,0,0x000000ff,true),
  HOWTO(R_SPARC_16,      0,1,16,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_16",      false,0,0x0000ffff,true),
  HOWTO(R_SPARC_32,      0,2,32,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_32",      false,0,0xffffffff,true),
  HOWTO(R_SPARC_DISP8,   0,0, 8,true, 0,complain_overflow_signed,  bfd_elf_generic_reloc,"R_SPARC_DISP8",   false,0,0x000000ff,true),
  HOWTO(R_SPARC_DISP16,  0,1,16,true, 0,complain_overflow_signed,  bfd_elf_generic_reloc,"R_SPARC_DISP16",  false,0,0x0000ffff,true),
  HOWTO(R_SPARC_DISP32,  0,2,32,true, 0,complain_overflow_signed,  bfd_elf_generic_reloc,"R_SPARC_DISP32",  false,0,0x00ffffff,true),
  HOWTO(R_SPARC_WDISP30, 2,2,30,true, 0,complain_overflow_signed,  bfd_elf_generic_reloc,"R_SPARC_WDISP30", false,0,0x3fffffff,true),
  HOWTO(R_SPARC_WDISP22, 2,2,22,true, 0,complain_overflow_signed,  bfd_elf_generic_reloc,"R_SPARC_WDISP22", false,0,0x003fffff,true),
  HOWTO(R_SPARC_HI22,   10,2,22,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_HI22",    false,0,0x003fffff,true),
  HOWTO(R_SPARC_22,      0,2,22,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_22",      false,0,0x003fffff,true),
  HOWTO(R_SPARC_13,      0,2,13,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_13",      false,0,0x00001fff,true),
  HOWTO(R_SPARC_LO10,    0,2,10,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_LO10",    false,0,0x000003ff,true),
  HOWTO(R_SPARC_GOT10,   0,2,10,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_GOT10",   false,0,0x000003ff,true),
  HOWTO(R_SPARC_GOT13,   0,2,13,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_GOT13",   false,0,0x00001fff,true),
  HOWTO(R_SPARC_GOT22,  10,2,22,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_GOT22",   false,0,0x003fffff,true),
  HOWTO(R_SPARC_PC10,    0,2,10,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_PC10",    false,0,0x000003ff,true),
  HOWTO(R_SPARC_PC22,    0,2,22,false,0,complain_overflow_bitfield,bfd_elf_generic_reloc,"R_SPARC_PC22",    false,0,0x003fffff,true),
  HOWTO(R_SPARC_WPLT30,  0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_WPLT30",  false,0,0x00000000,true),
  HOWTO(R_SPARC_COPY,    0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_COPY",    false,0,0x00000000,true),
  HOWTO(R_SPARC_GLOB_DAT,0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_GLOB_DAT",false,0,0x00000000,true),
  HOWTO(R_SPARC_JMP_SLOT,0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_JMP_SLOT",false,0,0x00000000,true),
  HOWTO(R_SPARC_RELATIVE,0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_RELATIVE",false,0,0x00000000,true),
  HOWTO(R_SPARC_UA32,    0,0,00,false,0,complain_overflow_dont,    bfd_elf_generic_reloc,"R_SPARC_UA32",    false,0,0x00000000,true),
};

struct elf_reloc_map {
  unsigned char bfd_reloc_val;
  unsigned char elf_reloc_val;
};

static CONST struct elf_reloc_map sparc_reloc_map[] =
{
  { BFD_RELOC_NONE, R_SPARC_NONE, },
  { BFD_RELOC_16, R_SPARC_16, },
  { BFD_RELOC_8, R_SPARC_8 },
  { BFD_RELOC_8_PCREL, R_SPARC_DISP8 },
  { BFD_RELOC_CTOR, R_SPARC_32 }, /* @@ Assumes 32 bits.  */
  { BFD_RELOC_32, R_SPARC_32 },
  { BFD_RELOC_32_PCREL, R_SPARC_DISP32 },
  { BFD_RELOC_HI22, R_SPARC_HI22 },
  { BFD_RELOC_LO10, R_SPARC_LO10, },
  { BFD_RELOC_32_PCREL_S2, R_SPARC_WDISP30 },
  { BFD_RELOC_SPARC22, R_SPARC_22 },
  { BFD_RELOC_SPARC13, R_SPARC_13 },
  { BFD_RELOC_SPARC_GOT10, R_SPARC_GOT10 },
  { BFD_RELOC_SPARC_GOT13, R_SPARC_GOT13 },
  { BFD_RELOC_SPARC_GOT22, R_SPARC_GOT22 },
  { BFD_RELOC_SPARC_PC10, R_SPARC_PC10 },
  { BFD_RELOC_SPARC_PC22, R_SPARC_PC22 },
  { BFD_RELOC_SPARC_WPLT30, R_SPARC_WPLT30 },
  { BFD_RELOC_SPARC_COPY, R_SPARC_COPY },
  { BFD_RELOC_SPARC_GLOB_DAT, R_SPARC_GLOB_DAT },
  { BFD_RELOC_SPARC_JMP_SLOT, R_SPARC_JMP_SLOT },
  { BFD_RELOC_SPARC_RELATIVE, R_SPARC_RELATIVE },
  { BFD_RELOC_SPARC_WDISP22, R_SPARC_WDISP22 },
  /*  { BFD_RELOC_SPARC_UA32, R_SPARC_UA32 }, not used?? */
};

static CONST struct reloc_howto_struct *
bfd_elf32_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int i;
  for (i = 0; i < sizeof (sparc_reloc_map) / sizeof (struct elf_reloc_map); i++)
    {
      if (sparc_reloc_map[i].bfd_reloc_val == code)
	return &elf_sparc_howto_table[(int) sparc_reloc_map[i].elf_reloc_val];
    }
  return 0;
}

static void
elf_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_SPARC_max);
  cache_ptr->howto = &elf_sparc_howto_table[ELF32_R_TYPE(dst->r_info)];
}


/* Functions for the SPARC ELF linker.  */

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */

#define ELF_DYNAMIC_INTERPRETER "/usr/lib/ld.so.1"

/* The nop opcode we use.  */

#define SPARC_NOP 0x01000000

/* The size in bytes of an entry in the procedure linkage table.  */

#define PLT_ENTRY_SIZE 12

/* The first four entries in a procedure linkage table are reserved,
   and the initial contents are unimportant (we zero them out).
   Subsequent entries look like this.  See the SVR4 ABI SPARC
   supplement to see how this works.  */

/* sethi %hi(.-.plt0),%g1.  We fill in the address later.  */
#define PLT_ENTRY_WORD0 0x03000000
/* b,a .plt0.  We fill in the offset later.  */
#define PLT_ENTRY_WORD1 0x30800000
/* nop.  */
#define PLT_ENTRY_WORD2 SPARC_NOP

/* Create dynamic sections when linking against a dynamic object.  */

static boolean
elf32_sparc_create_dynamic_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  flagword flags;
  register asection *s;
  struct elf_link_hash_entry *h;

  /* We need to create .plt, .rela.plt, .got, .dynbss, and .rela.bss
     sections.  */

  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY;

  s = bfd_make_section (abfd, ".plt");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_CODE)
      || ! bfd_set_section_alignment (abfd, s, 2))
    return false;

  /* Define the symbol _PROCEDURE_LINKAGE_TABLE_ at the start of the
     .plt section.  */
  h = NULL;
  if (! (_bfd_generic_link_add_one_symbol
	 (info, abfd, "_PROCEDURE_LINKAGE_TABLE_", BSF_GLOBAL, s, (bfd_vma) 0,
	  (const char *) NULL, false, get_elf_backend_data (abfd)->collect,
	  (struct bfd_link_hash_entry **) &h)))
    return false;
  h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;

  /* The first four entries in .plt are reserved.  */
  s->_raw_size = 4 * PLT_ENTRY_SIZE;

  s = bfd_make_section (abfd, ".rela.plt");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, 2))
    return false;

  s = bfd_make_section (abfd, ".got");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags)
      || ! bfd_set_section_alignment (abfd, s, 2))
    return false;

  /* Define the symbol _GLOBAL_OFFSET_TABLE_ at the start of the .got
     section.  We don't do this in the linker script because we don't
     want to define the symbol if we are not creating a global offset
     table.  FIXME: The Solaris linker puts _GLOBAL_OFFSET_TABLE_ at
     the start of the .got section, but when using the small PIC model
     the .got is accessed using a signed 13 bit offset.  Shouldn't
     _GLOBAL_OFFSET_TABLE_ be located at .got + 4096?  */
  h = NULL;
  if (! (_bfd_generic_link_add_one_symbol
	 (info, abfd, "_GLOBAL_OFFSET_TABLE_", BSF_GLOBAL, s, (bfd_vma) 0,
	  (const char *) NULL, false, get_elf_backend_data (abfd)->collect,
	  (struct bfd_link_hash_entry **) &h)))
    return false;
  h->elf_link_hash_flags |= ELF_LINK_HASH_DEF_REGULAR;

  /* The first global offset table entry is reserved.  */
  s->_raw_size += 4;

  /* The .dynbss section is a place to put symbols which are defined
     by dynamic objects, are referenced by regular objects, and are
     not functions.  We must allocate space for them in the process
     image and use a R_SPARC_COPY reloc to tell the dynamic linker to
     initialize them at run time.  The linker script puts the .dynbss
     section into the .bss section of the final image.  */
  s = bfd_make_section (abfd, ".dynbss");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, SEC_ALLOC))
    return false;

  /* The .rela.bss section holds copy relocs.  */
  s = bfd_make_section (abfd, ".rela.bss");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags | SEC_READONLY)
      || ! bfd_set_section_alignment (abfd, s, 2))
    return false;

  return true;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  The current definition is in some section of the
   dynamic object, but we're not including those sections.  We have to
   change the definition to something the rest of the link can
   understand.  */

static boolean
elf32_sparc_adjust_dynamic_symbol (info, h)
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
{
  bfd *dynobj;
  asection *s;
  unsigned int power_of_two;

  dynobj = elf_hash_table (info)->dynobj;

  /* Make sure we know what is going on here.  */
  BFD_ASSERT (dynobj != NULL
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_REF_REGULAR) != 0
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0
	      && h->root.type == bfd_link_hash_defined
	      && (bfd_get_flavour (h->root.u.def.section->owner)
		  == bfd_target_elf_flavour)
	      && (elf_elfheader (h->root.u.def.section->owner)->e_type
		  == ET_DYN)
	      && h->root.u.def.section->output_section == NULL);

  /* If this is a function, put it in the procedure linkage table.  We
     will fill in the contents of the procedure linkage table later
     (although we could actually do it here).  */
  if (h->type == STT_FUNC)
    {
      s = bfd_get_section_by_name (dynobj, ".plt");
      BFD_ASSERT (s != NULL);

      /* The procedure linkage table has a maximum size.  */
      if (s->_raw_size >= 0x400000)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}

      /* Set the symbol to this location in the .plt.  */
      h->root.u.def.section = s;
      h->root.u.def.value = s->_raw_size;

      /* Make room for this entry.  */
      s->_raw_size += PLT_ENTRY_SIZE;

      /* We also need to make an entry in the .rela.plt section.  */

      s = bfd_get_section_by_name (dynobj, ".rela.plt");
      BFD_ASSERT (s != NULL);
      s->_raw_size += sizeof (Elf32_External_Rela);

      return true;
    }

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->weakdef != NULL)
    {
      BFD_ASSERT (h->weakdef->root.type == bfd_link_hash_defined);
      h->root.u.def.section = h->weakdef->root.u.def.section;
      h->root.u.def.value = h->weakdef->root.u.def.value;
      h->copy_offset = (bfd_vma) -1;
      return true;
    }

  /* This is a reference to a symbol defined by a dynamic object which
     is not a function.  We must allocate it in our .dynbss section,
     which will become part of the .bss section of the executable.
     There will be an entry for this symbol in the .dynsym section.
     The dynamic object will contain position independent code, so all
     references from the dynamic object to this symbol will go through
     the global offset table.  The dynamic linker will use the .dynsym
     entry to determine the address it must put in the global offset
     table, so both the dynamic object and the regular object will
     refer to the same memory location for the variable.  */

  s = bfd_get_section_by_name (dynobj, ".dynbss");
  BFD_ASSERT (s != NULL);

  /* If the symbol is currently defined in the .bss section of the
     dynamic object, then it is OK to simply initialize it to zero.
     If the symbol is in some other section, we must generate a
     R_SPARC_COPY reloc to tell the dynamic linker to copy the initial
     value out of the dynamic object and into the runtime process
     image.  We need to remember the offset into the .rel.bss section
     we are going to use.  */
  if ((h->root.u.def.section->flags & SEC_LOAD) == 0)
    h->copy_offset = (bfd_vma) -1;
  else
    {
      asection *srel;

      srel = bfd_get_section_by_name (dynobj, ".rela.bss");
      BFD_ASSERT (srel != NULL);
      h->copy_offset = srel->_raw_size;
      srel->_raw_size += sizeof (Elf32_External_Rela);
    }

  /* We need to figure out the alignment required for this symbol.  I
     have no idea how ELF linkers handle this.  */
  power_of_two = bfd_log2 (h->size);
  if (power_of_two > 3)
    power_of_two = 3;

  /* Apply the required alignment.  */
  s->_raw_size = BFD_ALIGN (s->_raw_size,
			    (bfd_size_type) (1 << power_of_two));
  if (power_of_two > bfd_get_section_alignment (dynobj, s))
    {
      if (! bfd_set_section_alignment (dynobj, s, power_of_two))
	return false;
    }

  /* Define the symbol as being at this point in the section.  */
  h->root.u.def.section = s;
  h->root.u.def.value = s->_raw_size;

  /* Increment the section size to make room for the symbol.  */
  s->_raw_size += h->size;

  return true;
}

/* Allocate contents for a section.  */

static INLINE boolean
elf32_sparc_allocate_dynamic_section (dynobj, name)
     bfd *dynobj;
     const char *name;
{
  register asection *s;

  s = bfd_get_section_by_name (dynobj, name);
  BFD_ASSERT (s != NULL);
  s->contents = (bfd_byte *) bfd_alloc (dynobj, s->_raw_size);
  if (s->contents == NULL && s->_raw_size != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  return true;
}

/* Set the sizes of the dynamic sections.  */

static boolean
elf32_sparc_size_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  bfd *dynobj;
  asection *s;

  dynobj = elf_hash_table (info)->dynobj;
  BFD_ASSERT (dynobj != NULL);

  /* Set the contents of the .interp section to the interpreter.  */
  if (! info->shared)
    {
      s = bfd_get_section_by_name (dynobj, ".interp");
      BFD_ASSERT (s != NULL);
      s->_raw_size = sizeof ELF_DYNAMIC_INTERPRETER;
      s->contents = (unsigned char *) ELF_DYNAMIC_INTERPRETER;
    }

  /* Make space for the trailing nop in .plt.  */
  s = bfd_get_section_by_name (dynobj, ".plt");
  BFD_ASSERT (s != NULL);
  s->_raw_size += 4;

  /* The adjust_dynamic_symbol entry point has determined the sizes of
     the various dynamic sections.  Allocate some memory for them to
     hold contents.  */
  if (! elf32_sparc_allocate_dynamic_section (dynobj, ".plt")
      || ! elf32_sparc_allocate_dynamic_section (dynobj, ".rela.plt")
      || ! elf32_sparc_allocate_dynamic_section (dynobj, ".got")
      || ! elf32_sparc_allocate_dynamic_section (dynobj, ".rela.bss"))
    return false;

  /* Add some entries to the .dynamic section.  We fill in the values
     later, in elf32_sparc_finish_dynamic_sections, but we must add
     the entries now so that we get the correct size for the .dynamic
     section.  The DT_DEBUG entry is filled in by the dynamic linker
     and used by the debugger.  */
  if (! bfd_elf32_add_dynamic_entry (info, DT_DEBUG, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_PLTGOT, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_PLTRELSZ, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_PLTREL, DT_RELA)
      || ! bfd_elf32_add_dynamic_entry (info, DT_JMPREL, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_RELA, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_RELASZ, 0)
      || ! bfd_elf32_add_dynamic_entry (info, DT_RELAENT,
					sizeof (Elf32_External_Rela)))
    return false;

  return true;
}

/* Relocate a SPARC ELF section.  */

static boolean
elf32_sparc_relocate_section (output_bfd, info, input_bfd, input_section,
			      contents, relocs, local_syms, local_sections,
			      output_names)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
     char *output_names;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      const reloc_howto_type *howto;
      long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_type = ELF32_R_TYPE (rel->r_info);
      if (r_type < 0 || r_type >= (int) R_SPARC_max)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = elf_sparc_howto_table + r_type;

      r_symndx = ELF32_R_SYM (rel->r_info);

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
	  long indx;

	  indx = r_symndx - symtab_hdr->sh_info;
	  h = sym_hashes[indx];
	  if (h->root.type == bfd_link_hash_defined)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_weak)
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
		    name = output_names + sym->st_name;
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

/* Finish up dynamic symbol handling.  We set the contents of various
   dynamic sections here.  */

static boolean
elf32_sparc_finish_dynamic_symbol (output_bfd, info, h, sym)
     bfd *output_bfd;
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  /* If this symbol is not defined by a dynamic object, or is not
     referenced by a regular object, ignore it.  */
  if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0
      || (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
      || (h->elf_link_hash_flags & ELF_LINK_HASH_REF_REGULAR) == 0)
    {
      /* Mark some specially defined symbols as absolute.  */
      if (strcmp (h->root.root.string, "_DYNAMIC") == 0
	  || strcmp (h->root.root.string, "_GLOBAL_OFFSET_TABLE_") == 0
	  || strcmp (h->root.root.string, "_PROCEDURE_LINKAGE_TABLE_") == 0)
	sym->st_shndx = SHN_ABS;
      return true;
    }

  BFD_ASSERT (h->root.type == bfd_link_hash_defined);
  BFD_ASSERT (h->dynindx != -1);

  if (h->type == STT_FUNC)
    {
      asection *splt;
      asection *srela;
      Elf_Internal_Rela rela;

      splt = h->root.u.def.section;
      BFD_ASSERT (strcmp (bfd_get_section_name (splt->owner, splt), ".plt")
		  == 0);
      srela = bfd_get_section_by_name (splt->owner, ".rela.plt");
      BFD_ASSERT (srela != NULL);

      /* Fill in the entry in the procedure linkage table.  */
      bfd_put_32 (output_bfd,
		  PLT_ENTRY_WORD0 + h->root.u.def.value,
		  splt->contents + h->root.u.def.value);
      bfd_put_32 (output_bfd,
		  (PLT_ENTRY_WORD1
		   + (((- (h->root.u.def.value + 4)) >> 2) & 0x3fffff)),
		  splt->contents + h->root.u.def.value + 4);
      bfd_put_32 (output_bfd, PLT_ENTRY_WORD2,
		  splt->contents + h->root.u.def.value + 8);

      /* Fill in the entry in the .rela.plt section.  */
      rela.r_offset = (splt->output_section->vma
		       + splt->output_offset
		       + h->root.u.def.value);
      rela.r_info = ELF32_R_INFO (h->dynindx, R_SPARC_JMP_SLOT);
      rela.r_addend = 0;
      bfd_elf32_swap_reloca_out (output_bfd, &rela,
				 ((Elf32_External_Rela *) srela->contents
				  + (h->root.u.def.value / PLT_ENTRY_SIZE
				     - 4)));

      /* Mark the symbol as undefined, rather than as defined in the
	 .plt section.  Leave the value alone.  */
      sym->st_shndx = SHN_UNDEF;
    }
  else
    {
      /* This is not a function.  We have already allocated memory for
	 it in the .bss section (via .dynbss).  All we have to do here
	 is create a COPY reloc if required.  */
      if (h->copy_offset != (bfd_vma) -1)
	{
	  asection *s;
	  Elf_Internal_Rela rela;

	  s = bfd_get_section_by_name (h->root.u.def.section->owner,
				       ".rela.bss");
	  BFD_ASSERT (s != NULL);

	  rela.r_offset = (h->root.u.def.value
			   + h->root.u.def.section->output_section->vma
			   + h->root.u.def.section->output_offset);
	  rela.r_info = ELF32_R_INFO (h->dynindx, R_SPARC_COPY);
	  rela.r_addend = 0;
	  bfd_elf32_swap_reloca_out (output_bfd, &rela,
				     ((Elf32_External_Rela *)
				      (s->contents + h->copy_offset)));
	}
    }

  return true;
}

/* Finish up the dynamic sections.  */

static boolean
elf32_sparc_finish_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  asection *splt;
  asection *sgot;
  asection *sdyn;
  Elf32_External_Dyn *dyncon, *dynconend;

  splt = bfd_get_section_by_name (elf_hash_table (info)->dynobj, ".plt");
  sgot = bfd_get_section_by_name (elf_hash_table (info)->dynobj, ".got");
  sdyn = bfd_get_section_by_name (elf_hash_table (info)->dynobj, ".dynamic");
  BFD_ASSERT (splt != NULL && sgot != NULL && sdyn != NULL);

  dyncon = (Elf32_External_Dyn *) sdyn->contents;
  dynconend = (Elf32_External_Dyn *) (sdyn->contents + sdyn->_raw_size);
  for (; dyncon < dynconend; dyncon++)
    {
      Elf_Internal_Dyn dyn;
      const char *name;
      boolean size;

      bfd_elf32_swap_dyn_in (elf_hash_table (info)->dynobj, dyncon, &dyn);

      switch (dyn.d_tag)
	{
	case DT_PLTGOT:   name = ".plt"; size = false; break;
	case DT_PLTRELSZ: name = ".rela.plt"; size = true; break;
	case DT_JMPREL:   name = ".rela.plt"; size = false; break;
	default:	  name = NULL; size = false; break;
	}

      if (name != NULL)
	{
	  asection *s;

	  s = bfd_get_section_by_name (output_bfd, name);
	  BFD_ASSERT (s != NULL);
	  if (! size)
	    dyn.d_un.d_ptr = s->vma;
	  else
	    {
	      if (s->_cooked_size != 0)
		dyn.d_un.d_val = s->_cooked_size;
	      else
		dyn.d_un.d_val = s->_raw_size;
	    }
	  bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	}
    }

  /* Clear the first four entries in the procedure linkage table, and
     put a nop in the last four bytes.  */
  if (splt->_raw_size > 0)
    {
      memset (splt->contents, 0, 4 * PLT_ENTRY_SIZE);
      bfd_put_32 (output_bfd, SPARC_NOP,
		  splt->contents + splt->_raw_size - 4);
    }

  /* Set the first entry in the global offset table to the address of
     the dynamic section.  */
  if (sgot->_raw_size > 0)
    bfd_put_32 (output_bfd,
		sdyn->output_section->vma + sdyn->output_offset,
		sgot->contents);

  elf_section_data (sgot->output_section)->this_hdr.sh_entsize = 4;
  elf_section_data (splt->output_section)->this_hdr.sh_entsize =
    PLT_ENTRY_SIZE;

  return true;
}

#define TARGET_BIG_SYM	bfd_elf32_sparc_vec
#define TARGET_BIG_NAME	"elf32-sparc"
#define ELF_ARCH	bfd_arch_sparc
#define ELF_MACHINE_CODE EM_SPARC
#define ELF_MAXPAGESIZE 0x10000
#define elf_backend_create_dynamic_sections \
					elf32_sparc_create_dynamic_sections
#define elf_backend_adjust_dynamic_symbol \
					elf32_sparc_adjust_dynamic_symbol
#define elf_backend_size_dynamic_sections \
					elf32_sparc_size_dynamic_sections
#define elf_backend_relocate_section	elf32_sparc_relocate_section
#define elf_backend_finish_dynamic_symbol \
					elf32_sparc_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections \
					elf32_sparc_finish_dynamic_sections

#include "elf32-target.h"
