/* ELF executable support for BFD.
   Copyright 1991, 1992, 1993, 1994 Free Software Foundation, Inc.

   Written by Fred Fish @ Cygnus Support, from information published
   in "UNIX System V Release 4, Programmers Guide: ANSI C and
   Programming Support Tools".  Sufficient support for gdb.

   Rewritten by Mark Eichin @ Cygnus Support, from information
   published in "System V Application Binary Interface", chapters 4
   and 5, as well as the various "Processor Supplement" documents
   derived from it. Added support for assembler and other object file
   utilities.  Further work done by Ken Raeburn (Cygnus Support), Michael
   Meissner (Open Software Foundation), and Peter Hoogenboom (University
   of Utah) to finish and extend this.

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

/* Problems and other issues to resolve.

   (1)	BFD expects there to be some fixed number of "sections" in
        the object file.  I.E. there is a "section_count" variable in the
	bfd structure which contains the number of sections.  However, ELF
	supports multiple "views" of a file.  In particular, with current
	implementations, executable files typically have two tables, a
	program header table and a section header table, both of which
	partition the executable.

	In ELF-speak, the "linking view" of the file uses the section header
	table to access "sections" within the file, and the "execution view"
	uses the program header table to access "segments" within the file.
	"Segments" typically may contain all the data from one or more
	"sections".

	Note that the section header table is optional in ELF executables,
	but it is this information that is most useful to gdb.  If the
	section header table is missing, then gdb should probably try
	to make do with the program header table.  (FIXME)

   (2)  The code in this file is compiled twice, once in 32-bit mode and
	once in 64-bit mode.  More of it should be made size-independent
	and moved into elf.c.

   (3)	ELF section symbols are handled rather sloppily now.  This should
	be cleaned up, and ELF section symbols reconciled with BFD section
	symbols.
 */

#include <string.h>		/* For strrchr and friends */
#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libelf.h"

/* Renaming structures, typedefs, macros and functions to be size-specific.  */
#define Elf_External_Ehdr	NAME(Elf,External_Ehdr)
#define Elf_External_Sym	NAME(Elf,External_Sym)
#define Elf_External_Shdr	NAME(Elf,External_Shdr)
#define Elf_External_Phdr	NAME(Elf,External_Phdr)
#define Elf_External_Rel	NAME(Elf,External_Rel)
#define Elf_External_Rela	NAME(Elf,External_Rela)

#define elf_core_file_failing_command	NAME(bfd_elf,core_file_failing_command)
#define elf_core_file_failing_signal	NAME(bfd_elf,core_file_failing_signal)
#define elf_core_file_matches_executable_p \
  NAME(bfd_elf,core_file_matches_executable_p)
#define elf_object_p			NAME(bfd_elf,object_p)
#define elf_core_file_p			NAME(bfd_elf,core_file_p)
#define elf_get_symtab_upper_bound	NAME(bfd_elf,get_symtab_upper_bound)
#define elf_get_dynamic_symtab_upper_bound \
  NAME(bfd_elf,get_dynamic_symtab_upper_bound)
#define elf_get_reloc_upper_bound	NAME(bfd_elf,get_reloc_upper_bound)
#define elf_canonicalize_reloc		NAME(bfd_elf,canonicalize_reloc)
#define elf_get_symtab			NAME(bfd_elf,get_symtab)
#define elf_canonicalize_dynamic_symtab \
  NAME(bfd_elf,canonicalize_dynamic_symtab)
#define elf_make_empty_symbol		NAME(bfd_elf,make_empty_symbol)
#define elf_get_symbol_info		NAME(bfd_elf,get_symbol_info)
#define elf_print_symbol		NAME(bfd_elf,print_symbol)
#define elf_get_lineno			NAME(bfd_elf,get_lineno)
#define elf_set_arch_mach		NAME(bfd_elf,set_arch_mach)
#define elf_find_nearest_line		NAME(bfd_elf,find_nearest_line)
#define elf_sizeof_headers		NAME(bfd_elf,sizeof_headers)
#define elf_set_section_contents	NAME(bfd_elf,set_section_contents)
#define elf_no_info_to_howto		NAME(bfd_elf,no_info_to_howto)
#define elf_no_info_to_howto_rel	NAME(bfd_elf,no_info_to_howto_rel)
#define elf_new_section_hook		NAME(bfd_elf,new_section_hook)
#define write_relocs			NAME(bfd_elf,_write_relocs)
#define elf_find_section		NAME(bfd_elf,find_section)

#if ARCH_SIZE == 64
#define ELF_R_INFO(X,Y)	ELF64_R_INFO(X,Y)
#define ELF_R_SYM(X)	ELF64_R_SYM(X)
#define ELFCLASS	ELFCLASS64
#define FILE_ALIGN	8
#endif
#if ARCH_SIZE == 32
#define ELF_R_INFO(X,Y)	ELF32_R_INFO(X,Y)
#define ELF_R_SYM(X)	ELF32_R_SYM(X)
#define ELFCLASS	ELFCLASS32
#define FILE_ALIGN	4
#endif

static int shstrtab_length_fixed;

struct elf_sect_data
  {
    int reloc_sec;
    /* more? */
  };

/* Forward declarations of static functions */

static asection *section_from_elf_index PARAMS ((bfd *, unsigned int));

static int elf_section_from_bfd_section PARAMS ((bfd *, struct sec *));

static long elf_slurp_symbol_table PARAMS ((bfd *, asymbol **, boolean));

static int elf_symbol_from_bfd_symbol PARAMS ((bfd *,
					     struct symbol_cache_entry **));

static boolean elf_map_symbols PARAMS ((bfd *));
static boolean swap_out_syms PARAMS ((bfd *));

static boolean bfd_section_from_shdr PARAMS ((bfd *, unsigned int shindex));

#ifdef DEBUG
static void elf_debug_section PARAMS ((char *, int, Elf_Internal_Shdr *));
static void elf_debug_file PARAMS ((Elf_Internal_Ehdr *));
#endif

#define elf_string_from_elf_strtab(abfd,strindex) \
     elf_string_from_elf_section(abfd,elf_elfheader(abfd)->e_shstrndx,strindex)


/* Structure swapping routines */

/* Should perhaps use put_offset, put_word, etc.  For now, the two versions
   can be handled by explicitly specifying 32 bits or "the long type".  */
#if ARCH_SIZE == 64
#define put_word	bfd_h_put_64
#define get_word	bfd_h_get_64
#endif
#if ARCH_SIZE == 32
#define put_word	bfd_h_put_32
#define get_word	bfd_h_get_32
#endif

/* Translate an ELF symbol in external format into an ELF symbol in internal
   format. */

static void
elf_swap_symbol_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Sym *src;
     Elf_Internal_Sym *dst;
{
  dst->st_name = bfd_h_get_32 (abfd, (bfd_byte *) src->st_name);
  dst->st_value = get_word (abfd, (bfd_byte *) src->st_value);
  dst->st_size = get_word (abfd, (bfd_byte *) src->st_size);
  dst->st_info = bfd_h_get_8 (abfd, (bfd_byte *) src->st_info);
  dst->st_other = bfd_h_get_8 (abfd, (bfd_byte *) src->st_other);
  dst->st_shndx = bfd_h_get_16 (abfd, (bfd_byte *) src->st_shndx);
}

/* Translate an ELF symbol in internal format into an ELF symbol in external
   format. */

static void
elf_swap_symbol_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Sym *src;
     Elf_External_Sym *dst;
{
  bfd_h_put_32 (abfd, src->st_name, dst->st_name);
  put_word (abfd, src->st_value, dst->st_value);
  put_word (abfd, src->st_size, dst->st_size);
  bfd_h_put_8 (abfd, src->st_info, dst->st_info);
  bfd_h_put_8 (abfd, src->st_other, dst->st_other);
  bfd_h_put_16 (abfd, src->st_shndx, dst->st_shndx);
}


/* Translate an ELF file header in external format into an ELF file header in
   internal format. */

static void
elf_swap_ehdr_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Ehdr *src;
     Elf_Internal_Ehdr *dst;
{
  memcpy (dst->e_ident, src->e_ident, EI_NIDENT);
  dst->e_type = bfd_h_get_16 (abfd, (bfd_byte *) src->e_type);
  dst->e_machine = bfd_h_get_16 (abfd, (bfd_byte *) src->e_machine);
  dst->e_version = bfd_h_get_32 (abfd, (bfd_byte *) src->e_version);
  dst->e_entry = get_word (abfd, (bfd_byte *) src->e_entry);
  dst->e_phoff = get_word (abfd, (bfd_byte *) src->e_phoff);
  dst->e_shoff = get_word (abfd, (bfd_byte *) src->e_shoff);
  dst->e_flags = bfd_h_get_32 (abfd, (bfd_byte *) src->e_flags);
  dst->e_ehsize = bfd_h_get_16 (abfd, (bfd_byte *) src->e_ehsize);
  dst->e_phentsize = bfd_h_get_16 (abfd, (bfd_byte *) src->e_phentsize);
  dst->e_phnum = bfd_h_get_16 (abfd, (bfd_byte *) src->e_phnum);
  dst->e_shentsize = bfd_h_get_16 (abfd, (bfd_byte *) src->e_shentsize);
  dst->e_shnum = bfd_h_get_16 (abfd, (bfd_byte *) src->e_shnum);
  dst->e_shstrndx = bfd_h_get_16 (abfd, (bfd_byte *) src->e_shstrndx);
}

/* Translate an ELF file header in internal format into an ELF file header in
   external format. */

static void
elf_swap_ehdr_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Ehdr *src;
     Elf_External_Ehdr *dst;
{
  memcpy (dst->e_ident, src->e_ident, EI_NIDENT);
  /* note that all elements of dst are *arrays of unsigned char* already... */
  bfd_h_put_16 (abfd, src->e_type, dst->e_type);
  bfd_h_put_16 (abfd, src->e_machine, dst->e_machine);
  bfd_h_put_32 (abfd, src->e_version, dst->e_version);
  put_word (abfd, src->e_entry, dst->e_entry);
  put_word (abfd, src->e_phoff, dst->e_phoff);
  put_word (abfd, src->e_shoff, dst->e_shoff);
  bfd_h_put_32 (abfd, src->e_flags, dst->e_flags);
  bfd_h_put_16 (abfd, src->e_ehsize, dst->e_ehsize);
  bfd_h_put_16 (abfd, src->e_phentsize, dst->e_phentsize);
  bfd_h_put_16 (abfd, src->e_phnum, dst->e_phnum);
  bfd_h_put_16 (abfd, src->e_shentsize, dst->e_shentsize);
  bfd_h_put_16 (abfd, src->e_shnum, dst->e_shnum);
  bfd_h_put_16 (abfd, src->e_shstrndx, dst->e_shstrndx);
}


/* Translate an ELF section header table entry in external format into an
   ELF section header table entry in internal format. */

static void
elf_swap_shdr_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Shdr *src;
     Elf_Internal_Shdr *dst;
{
  dst->sh_name = bfd_h_get_32 (abfd, (bfd_byte *) src->sh_name);
  dst->sh_type = bfd_h_get_32 (abfd, (bfd_byte *) src->sh_type);
  dst->sh_flags = get_word (abfd, (bfd_byte *) src->sh_flags);
  dst->sh_addr = get_word (abfd, (bfd_byte *) src->sh_addr);
  dst->sh_offset = get_word (abfd, (bfd_byte *) src->sh_offset);
  dst->sh_size = get_word (abfd, (bfd_byte *) src->sh_size);
  dst->sh_link = bfd_h_get_32 (abfd, (bfd_byte *) src->sh_link);
  dst->sh_info = bfd_h_get_32 (abfd, (bfd_byte *) src->sh_info);
  dst->sh_addralign = get_word (abfd, (bfd_byte *) src->sh_addralign);
  dst->sh_entsize = get_word (abfd, (bfd_byte *) src->sh_entsize);
  /* we haven't done any processing on it yet, so... */
  dst->rawdata = (void *) 0;
}

/* Translate an ELF section header table entry in internal format into an
   ELF section header table entry in external format. */

static void
elf_swap_shdr_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Shdr *src;
     Elf_External_Shdr *dst;
{
  /* note that all elements of dst are *arrays of unsigned char* already... */
  bfd_h_put_32 (abfd, src->sh_name, dst->sh_name);
  bfd_h_put_32 (abfd, src->sh_type, dst->sh_type);
  put_word (abfd, src->sh_flags, dst->sh_flags);
  put_word (abfd, src->sh_addr, dst->sh_addr);
  put_word (abfd, src->sh_offset, dst->sh_offset);
  put_word (abfd, src->sh_size, dst->sh_size);
  bfd_h_put_32 (abfd, src->sh_link, dst->sh_link);
  bfd_h_put_32 (abfd, src->sh_info, dst->sh_info);
  put_word (abfd, src->sh_addralign, dst->sh_addralign);
  put_word (abfd, src->sh_entsize, dst->sh_entsize);
}


/* Translate an ELF program header table entry in external format into an
   ELF program header table entry in internal format. */

static void
elf_swap_phdr_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Phdr *src;
     Elf_Internal_Phdr *dst;
{
  dst->p_type = bfd_h_get_32 (abfd, (bfd_byte *) src->p_type);
  dst->p_flags = bfd_h_get_32 (abfd, (bfd_byte *) src->p_flags);
  dst->p_offset = get_word (abfd, (bfd_byte *) src->p_offset);
  dst->p_vaddr = get_word (abfd, (bfd_byte *) src->p_vaddr);
  dst->p_paddr = get_word (abfd, (bfd_byte *) src->p_paddr);
  dst->p_filesz = get_word (abfd, (bfd_byte *) src->p_filesz);
  dst->p_memsz = get_word (abfd, (bfd_byte *) src->p_memsz);
  dst->p_align = get_word (abfd, (bfd_byte *) src->p_align);
}

static void
elf_swap_phdr_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Phdr *src;
     Elf_External_Phdr *dst;
{
  /* note that all elements of dst are *arrays of unsigned char* already... */
  bfd_h_put_32 (abfd, src->p_type, dst->p_type);
  put_word (abfd, src->p_offset, dst->p_offset);
  put_word (abfd, src->p_vaddr, dst->p_vaddr);
  put_word (abfd, src->p_paddr, dst->p_paddr);
  put_word (abfd, src->p_filesz, dst->p_filesz);
  put_word (abfd, src->p_memsz, dst->p_memsz);
  bfd_h_put_32 (abfd, src->p_flags, dst->p_flags);
  put_word (abfd, src->p_align, dst->p_align);
}

/* Translate an ELF reloc from external format to internal format. */
static INLINE void
elf_swap_reloc_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Rel *src;
     Elf_Internal_Rel *dst;
{
  dst->r_offset = get_word (abfd, (bfd_byte *) src->r_offset);
  dst->r_info = get_word (abfd, (bfd_byte *) src->r_info);
}

static INLINE void
elf_swap_reloca_in (abfd, src, dst)
     bfd *abfd;
     Elf_External_Rela *src;
     Elf_Internal_Rela *dst;
{
  dst->r_offset = get_word (abfd, (bfd_byte *) src->r_offset);
  dst->r_info = get_word (abfd, (bfd_byte *) src->r_info);
  dst->r_addend = get_word (abfd, (bfd_byte *) src->r_addend);
}

/* Translate an ELF reloc from internal format to external format. */
static INLINE void
elf_swap_reloc_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Rel *src;
     Elf_External_Rel *dst;
{
  put_word (abfd, src->r_offset, dst->r_offset);
  put_word (abfd, src->r_info, dst->r_info);
}

static INLINE void
elf_swap_reloca_out (abfd, src, dst)
     bfd *abfd;
     Elf_Internal_Rela *src;
     Elf_External_Rela *dst;
{
  put_word (abfd, src->r_offset, dst->r_offset);
  put_word (abfd, src->r_info, dst->r_info);
  put_word (abfd, src->r_addend, dst->r_addend);
}



/* String table creation/manipulation routines */

static struct strtab *
bfd_new_strtab (abfd)
     bfd *abfd;
{
  struct strtab *ss;

  ss = (struct strtab *) malloc (sizeof (struct strtab));
  if (!ss)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }
  ss->tab = malloc (1);
  if (!ss->tab)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }
  *ss->tab = 0;
  ss->nentries = 0;
  ss->length = 1;

  return ss;
}

static int
bfd_add_to_strtab (abfd, ss, str)
     bfd *abfd;
     struct strtab *ss;
     CONST char *str;
{
  /* should search first, but for now: */
  /* include the trailing NUL */
  int ln = strlen (str) + 1;

  /* should this be using obstacks? */
  ss->tab = realloc (ss->tab, ss->length + ln);

  BFD_ASSERT (ss->tab != 0);	/* FIXME */
  strcpy (ss->tab + ss->length, str);
  ss->nentries++;
  ss->length += ln;

  return ss->length - ln;
}

static int
bfd_add_2_to_strtab (abfd, ss, str, str2)
     bfd *abfd;
     struct strtab *ss;
     char *str;
     CONST char *str2;
{
  /* should search first, but for now: */
  /* include the trailing NUL */
  int ln = strlen (str) + strlen (str2) + 1;

  /* should this be using obstacks? */
  if (ss->length)
    ss->tab = realloc (ss->tab, ss->length + ln);
  else
    ss->tab = malloc (ln);

  BFD_ASSERT (ss->tab != 0);	/* FIXME */
  strcpy (ss->tab + ss->length, str);
  strcpy (ss->tab + ss->length + strlen (str), str2);
  ss->nentries++;
  ss->length += ln;

  return ss->length - ln;
}


/* ELF .o/exec file reading */

/* Create a new bfd section from an ELF section header. */

static boolean
bfd_section_from_shdr (abfd, shindex)
     bfd *abfd;
     unsigned int shindex;
{
  Elf_Internal_Shdr *hdr = elf_elfsections (abfd)[shindex];
  Elf_Internal_Ehdr *ehdr = elf_elfheader (abfd);
  asection *newsect;
  char *name;

  name = elf_string_from_elf_strtab (abfd, hdr->sh_name);

  switch (hdr->sh_type)
    {

    case SHT_NULL:
      /* inactive section. Throw it away. */
      return true;

    case SHT_PROGBITS:
    case SHT_DYNAMIC:
      /* Bits that get saved. This one is real. */
      if (hdr->rawdata == NULL)
	{
	  newsect = bfd_make_section_anyway (abfd, name);
	  if (newsect == NULL)
	    return false;

	  newsect->filepos = hdr->sh_offset;
	  newsect->flags |= SEC_HAS_CONTENTS;
	  newsect->vma = hdr->sh_addr;
	  newsect->_raw_size = hdr->sh_size;
	  newsect->alignment_power = bfd_log2 (hdr->sh_addralign);

	  if (hdr->sh_flags & SHF_ALLOC)
	    {
	      newsect->flags |= SEC_ALLOC;
	      newsect->flags |= SEC_LOAD;
	    }

	  if (!(hdr->sh_flags & SHF_WRITE))
	    newsect->flags |= SEC_READONLY;

	  if (hdr->sh_flags & SHF_EXECINSTR)
	    newsect->flags |= SEC_CODE;	/* FIXME: may only contain SOME code */
	  else if (newsect->flags & SEC_ALLOC)
	    newsect->flags |= SEC_DATA;

	  /* The debugging sections appear to recognized only by name,
	     not any sort of flag.  */
	  if (strncmp (name, ".debug", sizeof ".debug" - 1) == 0
	      || strncmp (name, ".line", sizeof ".line" - 1) == 0
	      || strncmp (name, ".stab", sizeof ".stab" - 1) == 0)
	    newsect->flags |= SEC_DEBUGGING;

	  hdr->rawdata = (PTR) newsect;
	}
      return true;

    case SHT_NOBITS:
      /* Bits that get saved. This one is real. */
      if (hdr->rawdata == NULL)
	{
	  newsect = bfd_make_section_anyway (abfd, name);
	  if (newsect == NULL)
	    return false;

	  newsect->vma = hdr->sh_addr;
	  newsect->_raw_size = hdr->sh_size;
	  newsect->filepos = hdr->sh_offset;	/* fake */
	  newsect->alignment_power = bfd_log2 (hdr->sh_addralign);
	  if (hdr->sh_flags & SHF_ALLOC)
	    newsect->flags |= SEC_ALLOC;

	  if (!(hdr->sh_flags & SHF_WRITE))
	    newsect->flags |= SEC_READONLY;

	  /* FIXME: This section is empty.  Does it really make sense
	     to set SEC_CODE for it?  */
	  if (hdr->sh_flags & SHF_EXECINSTR)
	    newsect->flags |= SEC_CODE;	/* FIXME: may only contain SOME code */

	  hdr->rawdata = (PTR) newsect;
	}
      return true;

    case SHT_SYMTAB:		/* A symbol table */
      if (elf_onesymtab (abfd) == shindex)
	return true;

      BFD_ASSERT (hdr->sh_entsize == sizeof (Elf_External_Sym));
      BFD_ASSERT (elf_onesymtab (abfd) == 0);
      elf_onesymtab (abfd) = shindex;
      elf_tdata (abfd)->symtab_hdr = *hdr;
      elf_elfsections (abfd)[shindex] = &elf_tdata (abfd)->symtab_hdr;
      abfd->flags |= HAS_SYMS;
      return true;

    case SHT_DYNSYM:		/* A dynamic symbol table */
      if (elf_dynsymtab (abfd) == shindex)
	return true;

      BFD_ASSERT (hdr->sh_entsize == sizeof (Elf_External_Sym));
      BFD_ASSERT (elf_dynsymtab (abfd) == 0);
      elf_dynsymtab (abfd) = shindex;
      elf_tdata (abfd)->dynsymtab_hdr = *hdr;
      elf_elfsections (abfd)[shindex] = &elf_tdata (abfd)->dynsymtab_hdr;
      abfd->flags |= HAS_SYMS;
      return true;

    case SHT_STRTAB:		/* A string table */
      if (hdr->rawdata)
	return true;
      if (ehdr->e_shstrndx == shindex)
	{
	  elf_tdata (abfd)->shstrtab_hdr = *hdr;
	  elf_elfsections (abfd)[shindex] = &elf_tdata (abfd)->shstrtab_hdr;
	  hdr->rawdata = (PTR) & elf_tdata (abfd)->shstrtab_hdr;
	  return true;
	}
      {
	unsigned int i;

	for (i = 1; i < ehdr->e_shnum; i++)
	  {
	    Elf_Internal_Shdr *hdr2 = elf_elfsections (abfd)[i];
	    if (hdr2->sh_link == shindex)
	      {
		if (! bfd_section_from_shdr (abfd, i))
		  return false;
		if (elf_onesymtab (abfd) == i)
		  {
		    elf_tdata (abfd)->strtab_hdr = *hdr;
		    elf_elfsections (abfd)[shindex] = &elf_tdata (abfd)->strtab_hdr;
		    return true;
		  }
		if (elf_dynsymtab (abfd) == i)
		  {
		    elf_tdata (abfd)->dynstrtab_hdr = *hdr;
		    elf_elfsections (abfd)[shindex] = &elf_tdata (abfd)->dynstrtab_hdr;
		    return true;
		  }
#if 0 /* Not handling other string tables specially right now.  */
		hdr2 = elf_elfsections (abfd)[i];	/* in case it moved */
		/* We have a strtab for some random other section.  */
		newsect = (asection *) hdr2->rawdata;
		if (!newsect)
		  break;
		hdr->rawdata = (PTR) newsect;
		hdr2 = &elf_section_data (newsect)->str_hdr;
		*hdr2 = *hdr;
		elf_elfsections (abfd)[shindex] = hdr2;
#endif
	      }
	  }
      }

      newsect = bfd_make_section_anyway (abfd, name);
      if (newsect == NULL)
	return false;

      newsect->flags = SEC_HAS_CONTENTS;
      hdr->rawdata = (PTR) newsect;
      newsect->_raw_size = hdr->sh_size;
      newsect->alignment_power = bfd_log2 (hdr->sh_addralign);
      newsect->vma = hdr->sh_addr;
      newsect->filepos = hdr->sh_offset;

      if (hdr->sh_flags & SHF_ALLOC)
	newsect->flags |= SEC_ALLOC | SEC_LOAD;
      if (!(hdr->sh_flags & SHF_WRITE))
	newsect->flags |= SEC_READONLY;
      if (hdr->sh_flags & SHF_EXECINSTR)
	newsect->flags |= SEC_CODE;
      else if (newsect->flags & SEC_ALLOC)
	newsect->flags |= SEC_DATA;

      /* Check for debugging string tables.  */
      if (strncmp (name, ".debug", sizeof ".debug" - 1) == 0
	  || strncmp (name, ".stab", sizeof ".stab" - 1) == 0)
	newsect->flags |= SEC_DEBUGGING;

      return true;

    case SHT_REL:
    case SHT_RELA:
      /* *These* do a lot of work -- but build no sections!
	 The spec says there can be multiple strtabs, but only one symtab,
	 but there can be lots of REL* sections. */
      /* FIXME:  The above statement is wrong!  There are typically at least
	 two symbol tables in a dynamically linked executable, ".dynsym"
	 which is the dynamic linkage symbol table and ".symtab", which is
	 the "traditional" symbol table.  -fnf */

      {
	asection *target_sect;
	Elf_Internal_Shdr *hdr2;
	int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

	/* Don't allow REL relocations on a machine that uses RELA and
	   vice versa.  */
	/* @@ Actually, the generic ABI does suggest that both might be
	   used in one file.  But the four ABI Processor Supplements I
	   have access to right now all specify that only one is used on
	   each of those architectures.  It's conceivable that, e.g., a
	   bunch of absolute 32-bit relocs might be more compact in REL
	   form even on a RELA machine...  */
	BFD_ASSERT (!(use_rela_p && (hdr->sh_type == SHT_REL)));
	BFD_ASSERT (!(!use_rela_p && (hdr->sh_type == SHT_RELA)));
	BFD_ASSERT (hdr->sh_entsize ==
		    (use_rela_p
		     ? sizeof (Elf_External_Rela)
		     : sizeof (Elf_External_Rel)));

	if (! bfd_section_from_shdr (abfd, hdr->sh_info)     /* target */
	    || ! bfd_section_from_shdr (abfd, hdr->sh_link)) /* symbol table */
	  return false;
	target_sect = section_from_elf_index (abfd, hdr->sh_info);
	if (target_sect == NULL
	    || elf_section_data (target_sect) == NULL)
	  return false;

	hdr2 = &elf_section_data (target_sect)->rel_hdr;
	*hdr2 = *hdr;
	elf_elfsections (abfd)[shindex] = hdr2;
	target_sect->reloc_count = hdr->sh_size / hdr->sh_entsize;
	target_sect->flags |= SEC_RELOC;
	target_sect->relocation = 0;
	target_sect->rel_filepos = hdr->sh_offset;
	abfd->flags |= HAS_RELOC;
	return true;
      }
      break;

    case SHT_HASH:
#if 0
      fprintf (stderr, "Dynamic Linking sections not yet supported.\n");
      BFD_FAIL ();
#endif
      break;

    case SHT_NOTE:
#if 0
      fprintf (stderr, "Note Sections not yet supported.\n");
      BFD_FAIL ();
#endif
      break;

    case SHT_SHLIB:
#if 0
      fprintf (stderr, "SHLIB Sections not supported (and non conforming.)\n");
#endif
      return true;

    default:
      /* Check for any processor-specific section types.  */
      {
	struct elf_backend_data *bed = get_elf_backend_data (abfd);

	if (bed->elf_backend_section_from_shdr)
	  (*bed->elf_backend_section_from_shdr) (abfd, hdr, name);
      }
      break;
    }

  return true;
}

boolean
elf_new_section_hook (abfd, sec)
     bfd *abfd
      ;
     asection *sec;
{
  struct bfd_elf_section_data *sdata;

  sdata = (struct bfd_elf_section_data *) bfd_alloc (abfd, sizeof (*sdata));
  if (!sdata)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  sec->used_by_bfd = (PTR) sdata;
  memset (sdata, 0, sizeof (*sdata));
  return true;
}

/* Create a new bfd section from an ELF program header.

   Since program segments have no names, we generate a synthetic name
   of the form segment<NUM>, where NUM is generally the index in the
   program header table.  For segments that are split (see below) we
   generate the names segment<NUM>a and segment<NUM>b.

   Note that some program segments may have a file size that is different than
   (less than) the memory size.  All this means is that at execution the
   system must allocate the amount of memory specified by the memory size,
   but only initialize it with the first "file size" bytes read from the
   file.  This would occur for example, with program segments consisting
   of combined data+bss.

   To handle the above situation, this routine generates TWO bfd sections
   for the single program segment.  The first has the length specified by
   the file size of the segment, and the second has the length specified
   by the difference between the two sizes.  In effect, the segment is split
   into it's initialized and uninitialized parts.

 */

static boolean
bfd_section_from_phdr (abfd, hdr, index)
     bfd *abfd;
     Elf_Internal_Phdr *hdr;
     int index;
{
  asection *newsect;
  char *name;
  char namebuf[64];
  int split;

  split = ((hdr->p_memsz > 0) &&
	   (hdr->p_filesz > 0) &&
	   (hdr->p_memsz > hdr->p_filesz));
  sprintf (namebuf, split ? "segment%da" : "segment%d", index);
  name = bfd_alloc (abfd, strlen (namebuf) + 1);
  if (!name)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  strcpy (name, namebuf);
  newsect = bfd_make_section (abfd, name);
  if (newsect == NULL)
    return false;
  newsect->vma = hdr->p_vaddr;
  newsect->_raw_size = hdr->p_filesz;
  newsect->filepos = hdr->p_offset;
  newsect->flags |= SEC_HAS_CONTENTS;
  if (hdr->p_type == PT_LOAD)
    {
      newsect->flags |= SEC_ALLOC;
      newsect->flags |= SEC_LOAD;
      if (hdr->p_flags & PF_X)
	{
	  /* FIXME: all we known is that it has execute PERMISSION,
	     may be data. */
	  newsect->flags |= SEC_CODE;
	}
    }
  if (!(hdr->p_flags & PF_W))
    {
      newsect->flags |= SEC_READONLY;
    }

  if (split)
    {
      sprintf (namebuf, "segment%db", index);
      name = bfd_alloc (abfd, strlen (namebuf) + 1);
      if (!name)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      strcpy (name, namebuf);
      newsect = bfd_make_section (abfd, name);
      if (newsect == NULL)
	return false;
      newsect->vma = hdr->p_vaddr + hdr->p_filesz;
      newsect->_raw_size = hdr->p_memsz - hdr->p_filesz;
      if (hdr->p_type == PT_LOAD)
	{
	  newsect->flags |= SEC_ALLOC;
	  if (hdr->p_flags & PF_X)
	    newsect->flags |= SEC_CODE;
	}
      if (!(hdr->p_flags & PF_W))
	newsect->flags |= SEC_READONLY;
    }

  return true;
}

/* Begin processing a given object.

   First we validate the file by reading in the ELF header and checking
   the magic number.  */

static INLINE boolean
elf_file_p (x_ehdrp)
     Elf_External_Ehdr *x_ehdrp;
{
  return ((x_ehdrp->e_ident[EI_MAG0] == ELFMAG0)
	  && (x_ehdrp->e_ident[EI_MAG1] == ELFMAG1)
	  && (x_ehdrp->e_ident[EI_MAG2] == ELFMAG2)
	  && (x_ehdrp->e_ident[EI_MAG3] == ELFMAG3));
}

/* Check to see if the file associated with ABFD matches the target vector
   that ABFD points to.

   Note that we may be called several times with the same ABFD, but different
   target vectors, most of which will not match.  We have to avoid leaving
   any side effects in ABFD, or any data it points to (like tdata), if the
   file does not match the target vector.

   FIXME:  There is memory leak if we are called more than once with the same
   ABFD, and that bfd already has tdata allocated, since we allocate more tdata
   and the old tdata is orphaned.  Since it's in the bfd obstack, there isn't
   much we can do about this except possibly rewrite the code.  There are
   also other bfd_allocs that may be the source of memory leaks as well. */

bfd_target *
elf_object_p (abfd)
     bfd *abfd;
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_External_Shdr x_shdr;	/* Section header table entry, external form */
  Elf_Internal_Shdr *i_shdrp;	/* Section header table, internal form */
  unsigned int shindex;
  char *shstrtab;		/* Internal copy of section header stringtab */
  struct elf_backend_data *ebd;
  struct elf_obj_tdata *preserved_tdata = elf_tdata (abfd);

  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      if (bfd_get_error () != bfd_error_system_call)
	goto got_wrong_format_error;
      else
	goto got_no_match;
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry, and it must have a
     section header table (FIXME: See comments re sections at top of this
     file). */

  if ((elf_file_p (&x_ehdr) == false) ||
      (x_ehdr.e_ident[EI_VERSION] != EV_CURRENT) ||
      (x_ehdr.e_ident[EI_CLASS] != ELFCLASS))
    goto got_wrong_format_error;

  /* Check that file's byte order matches xvec's */
  switch (x_ehdr.e_ident[EI_DATA])
    {
    case ELFDATA2MSB:		/* Big-endian */
      if (!abfd->xvec->header_byteorder_big_p)
	goto got_wrong_format_error;
      break;
    case ELFDATA2LSB:		/* Little-endian */
      if (abfd->xvec->header_byteorder_big_p)
	goto got_wrong_format_error;
      break;
    case ELFDATANONE:		/* No data encoding specified */
    default:			/* Unknown data encoding specified */
      goto got_wrong_format_error;
    }

  /* Allocate an instance of the elf_obj_tdata structure and hook it up to
     the tdata pointer in the bfd.  FIXME:  memory leak, see above. */

  elf_tdata (abfd) =
    (struct elf_obj_tdata *) bfd_zalloc (abfd, sizeof (struct elf_obj_tdata));
  if (elf_tdata (abfd) == NULL)
    goto got_no_memory_error;

  /* Now that we know the byte order, swap in the rest of the header */
  i_ehdrp = elf_elfheader (abfd);
  elf_swap_ehdr_in (abfd, &x_ehdr, i_ehdrp);
#if DEBUG & 1
  elf_debug_file (i_ehdrp);
#endif

  /* If there is no section header table, we're hosed. */
  if (i_ehdrp->e_shoff == 0)
    goto got_wrong_format_error;

  /* As a simple sanity check, verify that the what BFD thinks is the
     size of each section header table entry actually matches the size
     recorded in the file. */
  if (i_ehdrp->e_shentsize != sizeof (x_shdr))
    goto got_wrong_format_error;

  ebd = get_elf_backend_data (abfd);

  /* Check that the ELF e_machine field matches what this particular
     BFD format expects.  */
  if (ebd->elf_machine_code != i_ehdrp->e_machine)
    {
      bfd_target **target_ptr;

      if (ebd->elf_machine_code != EM_NONE)
	goto got_wrong_format_error;

      /* This is the generic ELF target.  Let it match any ELF target
	 for which we do not have a specific backend.  */
      for (target_ptr = bfd_target_vector; *target_ptr != NULL; target_ptr++)
	{
	  struct elf_backend_data *back;

	  if ((*target_ptr)->flavour != bfd_target_elf_flavour)
	    continue;
	  back = (struct elf_backend_data *) (*target_ptr)->backend_data;
	  if (back->elf_machine_code == i_ehdrp->e_machine)
	    {
	      /* target_ptr is an ELF backend which matches this
		 object file, so reject the generic ELF target.  */
	      goto got_wrong_format_error;
	    }
	}
    }


  /* Set the flags and architecture before calling the backend so that
     it can override them.  */
  if (i_ehdrp->e_type == ET_EXEC)
    abfd->flags |= EXEC_P;
  else if (i_ehdrp->e_type == ET_DYN)
    abfd->flags |= DYNAMIC;

  bfd_default_set_arch_mach (abfd, ebd->arch, 0);

  /* Remember the entry point specified in the ELF file header. */
  bfd_get_start_address (abfd) = i_ehdrp->e_entry;

  /* Let the backend double check the format and override global
     information.  */
  if (ebd->elf_backend_object_p)
    {
      if ((*ebd->elf_backend_object_p) (abfd) == false)
	goto got_wrong_format_error;
    }

  /* Allocate space for a copy of the section header table in
     internal form, seek to the section header table in the file,
     read it in, and convert it to internal form.  */
  i_shdrp = (Elf_Internal_Shdr *)
    bfd_alloc (abfd, sizeof (*i_shdrp) * i_ehdrp->e_shnum);
  elf_elfsections (abfd) =
    (Elf_Internal_Shdr **) bfd_alloc (abfd, sizeof (i_shdrp) * i_ehdrp->e_shnum);
  if (!i_shdrp || !elf_elfsections (abfd))
    goto got_no_memory_error;
  if (bfd_seek (abfd, i_ehdrp->e_shoff, SEEK_SET) == -1)
    goto got_no_match;
  for (shindex = 0; shindex < i_ehdrp->e_shnum; shindex++)
    {
      if (bfd_read ((PTR) & x_shdr, sizeof x_shdr, 1, abfd) != sizeof (x_shdr))
	goto got_no_match;
      elf_swap_shdr_in (abfd, &x_shdr, i_shdrp + shindex);
      elf_elfsections (abfd)[shindex] = i_shdrp + shindex;

      /* If this is a .dynamic section, mark the object file as being
	 dynamically linked.  */
      if (i_shdrp[shindex].sh_type == SHT_DYNAMIC)
	abfd->flags |= DYNAMIC;
    }
  if (i_ehdrp->e_shstrndx)
    {
      if (! bfd_section_from_shdr (abfd, i_ehdrp->e_shstrndx))
	goto got_no_match;
    }

  /* Read in the string table containing the names of the sections.  We
     will need the base pointer to this table later. */
  /* We read this inline now, so that we don't have to go through
     bfd_section_from_shdr with it (since this particular strtab is
     used to find all of the ELF section names.) */

  shstrtab = elf_get_str_section (abfd, i_ehdrp->e_shstrndx);
  if (!shstrtab)
    goto got_wrong_format_error;

  /* Once all of the section headers have been read and converted, we
     can start processing them.  Note that the first section header is
     a dummy placeholder entry, so we ignore it.

     We also watch for the symbol table section and remember the file
     offset and section size for both the symbol table section and the
     associated string table section. */

  for (shindex = 1; shindex < i_ehdrp->e_shnum; shindex++)
    {
      if (! bfd_section_from_shdr (abfd, shindex))
	goto got_no_match;
    }

  return (abfd->xvec);

  /* If we are going to use goto's to avoid duplicating error setting
     and return(NULL) code, then this at least makes it more maintainable. */

got_wrong_format_error:
  bfd_set_error (bfd_error_wrong_format);
  goto got_no_match;
got_no_memory_error:
  bfd_set_error (bfd_error_no_memory);
  goto got_no_match;
got_no_match:
  elf_tdata (abfd) = preserved_tdata;
  return (NULL);
}


/* ELF .o/exec file writing */

/* Takes a bfd and a symbol, returns a pointer to the elf specific area
   of the symbol if there is one.  */
static INLINE elf_symbol_type *
elf_symbol_from (ignore_abfd, symbol)
     bfd *ignore_abfd;
     asymbol *symbol;
{
  if (symbol->the_bfd->xvec->flavour != bfd_target_elf_flavour)
    return 0;

  if (symbol->the_bfd->tdata.elf_obj_data == (struct elf_obj_tdata *) NULL)
    return 0;

  return (elf_symbol_type *) symbol;
}

/* Create ELF output from BFD sections.

   Essentially, just create the section header and forget about the program
   header for now.  */

static void
elf_make_sections (abfd, asect, obj)
     bfd *abfd;
     asection *asect;
     PTR obj;
{
  /* most of what is in bfd_shdr_from_section goes in here... */
  /* and all of these sections generate at *least* one ELF section. */
  Elf_Internal_Shdr *this_hdr;
  this_hdr = &elf_section_data (asect)->this_hdr;

  this_hdr->sh_addr = asect->vma;
  this_hdr->sh_size = asect->_raw_size;
  /* contents already set by elf_set_section_contents */

  if (asect->flags & SEC_RELOC)
    {
      /* emit a reloc section, and thus strtab and symtab... */
      Elf_Internal_Shdr *rela_hdr;
      int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

      rela_hdr = &elf_section_data (asect)->rel_hdr;

      /* orelocation has the data, reloc_count has the count... */
      if (use_rela_p)
	{
	  rela_hdr->sh_type = SHT_RELA;
	  rela_hdr->sh_entsize = sizeof (Elf_External_Rela);
	}
      else
	/* REL relocations */
	{
	  rela_hdr->sh_type = SHT_REL;
	  rela_hdr->sh_entsize = sizeof (Elf_External_Rel);
	}
      rela_hdr->sh_flags = 0;
      rela_hdr->sh_addr = 0;
      rela_hdr->sh_offset = 0;

      /* FIXME: Systems I've checked use an alignment of 4, but it is
	 possible that some systems use a different alignment.  */
      rela_hdr->sh_addralign = 4;

      rela_hdr->size = 0;
    }
  if (asect->flags & SEC_ALLOC)
    {
      this_hdr->sh_flags |= SHF_ALLOC;
      if (asect->flags & SEC_LOAD)
	{
	  /* @@ Do something with sh_type? */
	}
    }
  else
    {
      /* If this section is not part of the program image during
	 execution, leave the address fields at 0.  */
      this_hdr->sh_addr = 0;
      asect->vma = 0;
    }
  if (!(asect->flags & SEC_READONLY))
    this_hdr->sh_flags |= SHF_WRITE;

  if (asect->flags & SEC_CODE)
    this_hdr->sh_flags |= SHF_EXECINSTR;
}

void
write_relocs (abfd, sec, xxx)
     bfd *abfd;
     asection *sec;
     PTR xxx;
{
  Elf_Internal_Shdr *rela_hdr;
  Elf_External_Rela *outbound_relocas;
  Elf_External_Rel *outbound_relocs;
  int idx;
  int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;
  asymbol *last_sym = 0;
  int last_sym_idx = 9999999;	/* should always be written before use */

  if ((sec->flags & SEC_RELOC) == 0)
    return;
  /* Flags are sometimes inconsistent.  */
  if (sec->reloc_count == 0)
    return;

  rela_hdr = &elf_section_data (sec)->rel_hdr;

  rela_hdr->sh_size = rela_hdr->sh_entsize * sec->reloc_count;
  rela_hdr->contents = (void *) bfd_alloc (abfd, rela_hdr->sh_size);
  if (!rela_hdr->contents)
    {
      bfd_set_error (bfd_error_no_memory);
      abort ();			/* FIXME */
    }

  /* orelocation has the data, reloc_count has the count... */
  if (use_rela_p)
    {
      outbound_relocas = (Elf_External_Rela *) rela_hdr->contents;

      for (idx = 0; idx < sec->reloc_count; idx++)
	{
	  Elf_Internal_Rela dst_rela;
	  Elf_External_Rela *src_rela;
	  arelent *ptr;
	  asymbol *sym;
	  int n;

	  ptr = sec->orelocation[idx];
	  src_rela = outbound_relocas + idx;
	  if (!(abfd->flags & EXEC_P))
	    dst_rela.r_offset = ptr->address - sec->vma;
	  else
	    dst_rela.r_offset = ptr->address;

	  sym = *ptr->sym_ptr_ptr;
	  if (sym == last_sym)
	    n = last_sym_idx;
	  else
	    {
	      last_sym = sym;
	      last_sym_idx = n = elf_symbol_from_bfd_symbol (abfd, &sym);
	    }
	  dst_rela.r_info = ELF_R_INFO (n, ptr->howto->type);

	  dst_rela.r_addend = ptr->addend;
	  elf_swap_reloca_out (abfd, &dst_rela, src_rela);
	}
    }
  else
    /* REL relocations */
    {
      outbound_relocs = (Elf_External_Rel *) rela_hdr->contents;

      for (idx = 0; idx < sec->reloc_count; idx++)
	{
	  Elf_Internal_Rel dst_rel;
	  Elf_External_Rel *src_rel;
	  arelent *ptr;
	  int n;
	  asymbol *sym;

	  ptr = sec->orelocation[idx];
	  sym = *ptr->sym_ptr_ptr;
	  src_rel = outbound_relocs + idx;
	  if (!(abfd->flags & EXEC_P))
	    dst_rel.r_offset = ptr->address - sec->vma;
	  else
	    dst_rel.r_offset = ptr->address;

	  if (sym == last_sym)
	    n = last_sym_idx;
	  else
	    {
	      last_sym = sym;
	      last_sym_idx = n = elf_symbol_from_bfd_symbol (abfd, &sym);
	    }
	  dst_rel.r_info = ELF_R_INFO (n, ptr->howto->type);

	  elf_swap_reloc_out (abfd, &dst_rel, src_rel);
	}
    }
}

static void
fix_up_strtabs (abfd, asect, obj)
     bfd *abfd;
     asection *asect;
     PTR obj;
{
  Elf_Internal_Shdr *this_hdr = &elf_section_data (asect)->this_hdr;
  int this_idx = elf_section_data (asect)->this_idx;

  /* @@ Check flags!  */
  if (!strncmp (asect->name, ".stab", 5)
      && !strcmp ("str", asect->name + strlen (asect->name) - 3))
    {
      size_t len = strlen (asect->name) + 1;
      char *s = (char *) malloc (len);
      if (s == NULL)
	/* FIXME: Should deal more gracefully with errors.  */
	abort ();
      strcpy (s, asect->name);
      s[len - 4] = 0;
      asect = bfd_get_section_by_name (abfd, s);
      free (s);
      if (!asect)
	abort ();
      elf_section_data (asect)->this_hdr.sh_link = this_idx;
      /* @@ Assuming 32 bits!  */
      elf_section_data (asect)->this_hdr.sh_entsize = 0xc;

      this_hdr->sh_type = SHT_STRTAB;
    }
}

static void
elf_fake_sections (abfd, asect, obj)
     bfd *abfd;
     asection *asect;
     PTR obj;
{
  /* most of what is in bfd_shdr_from_section goes in here... */
  /* and all of these sections generate at *least* one ELF section. */

  Elf_Internal_Shdr *this_hdr;
  this_hdr = &elf_section_data (asect)->this_hdr;
  this_hdr->sh_name =
    bfd_add_to_strtab (abfd, elf_shstrtab (abfd), asect->name);
  /* We need to log the type *now* so that elf_section_from_bfd_section
     can find us... have to set rawdata too. */
  this_hdr->rawdata = (void *) asect;
  this_hdr->sh_addralign = 1 << asect->alignment_power;
  if ((asect->flags & SEC_ALLOC) && (asect->flags & SEC_LOAD))
    this_hdr->sh_type = SHT_PROGBITS;
  else if ((asect->flags & SEC_ALLOC) && ((asect->flags & SEC_LOAD) == 0))
    {
      BFD_ASSERT (strcmp (asect->name, ".bss") == 0
		  || strcmp (asect->name, ".sbss") == 0);
      this_hdr->sh_type = SHT_NOBITS;
    }
  /* FIXME I am not sure how to detect a .note section from the flags
     word of an `asection'.  */
  else if (!strcmp (asect->name, ".note"))
    this_hdr->sh_type = SHT_NOTE;
  else
    this_hdr->sh_type = SHT_PROGBITS;

  this_hdr->sh_flags = 0;
  this_hdr->sh_addr = 0;
  this_hdr->sh_size = 0;
  this_hdr->sh_entsize = 0;
  this_hdr->sh_info = 0;
  this_hdr->sh_link = 0;
  this_hdr->sh_offset = 0;
  this_hdr->size = 0;

  /* Now, check for processor-specific section types.  */
  {
    struct elf_backend_data *bed = get_elf_backend_data (abfd);

    if (bed->elf_backend_fake_sections)
      (*bed->elf_backend_fake_sections) (abfd, this_hdr, asect);
  }

  {
    /* Emit a strtab and symtab, and possibly a reloc section.  */
    Elf_Internal_Shdr *rela_hdr;

    /* Note that only one symtab is used, so just remember it
       for now.  */

    if (asect->flags & SEC_RELOC)
      {
	int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

	rela_hdr = &elf_section_data (asect)->rel_hdr;
	rela_hdr->sh_name =
	  bfd_add_2_to_strtab (abfd, elf_shstrtab (abfd),
			       use_rela_p ? ".rela" : ".rel",
			       asect->name);
	rela_hdr->sh_type = use_rela_p ? SHT_RELA : SHT_REL;
	rela_hdr->sh_entsize = (use_rela_p
				? sizeof (Elf_External_Rela)
				: sizeof (Elf_External_Rel));

	rela_hdr->sh_flags = 0;
	rela_hdr->sh_addr = 0;
	rela_hdr->sh_size = 0;
	rela_hdr->sh_offset = 0;

	/* FIXME: Systems I've checked use an alignment of 4, but some
	   systems may use a different alignment.  */
	rela_hdr->sh_addralign = 4;

	rela_hdr->size = 0;
      }
  }
  if (asect->flags & SEC_ALLOC)
    {
      this_hdr->sh_flags |= SHF_ALLOC;
      if (asect->flags & SEC_LOAD)
	{
	  /* @@ Do something with sh_type?  */
	}
    }
  if (!(asect->flags & SEC_READONLY))
    this_hdr->sh_flags |= SHF_WRITE;
  if (asect->flags & SEC_CODE)
    this_hdr->sh_flags |= SHF_EXECINSTR;
}

/* Map symbol from it's internal number to the external number, moving
   all local symbols to be at the head of the list.  */

static INLINE int
sym_is_global (abfd, sym)
     bfd *abfd;
     asymbol *sym;
{
  /* If the backend has a special mapping, use it.  */
  if (get_elf_backend_data (abfd)->elf_backend_sym_is_global)
    return ((*get_elf_backend_data (abfd)->elf_backend_sym_is_global)
	    (abfd, sym));

  if (sym->flags & (BSF_GLOBAL | BSF_WEAK))
    {
      if (sym->flags & BSF_LOCAL)
	abort ();
      return 1;
    }
  if (sym->section == 0)
    {
      /* Is this valid?  */
      abort ();

      return 1;
    }
  if (sym->section == &bfd_und_section)
    return 1;
  if (bfd_is_com_section (sym->section))
    return 1;
  if (sym->flags & (BSF_LOCAL | BSF_SECTION_SYM | BSF_FILE))
    return 0;
  return 0;
}

static boolean
elf_map_symbols (abfd)
     bfd *abfd;
{
  int symcount = bfd_get_symcount (abfd);
  asymbol **syms = bfd_get_outsymbols (abfd);
  asymbol **sect_syms;
  int num_locals = 0;
  int num_globals = 0;
  int num_locals2 = 0;
  int num_globals2 = 0;
  int max_index = 0;
  int num_sections = 0;
  Elf_Sym_Extra *sym_extra;
  int idx;
  asection *asect;

#ifdef DEBUG
  fprintf (stderr, "elf_map_symbols\n");
  fflush (stderr);
#endif

  /* Add local symbols for each section for which there are relocs.
     FIXME: How can we tell which sections have relocs at this point?
     Will reloc_count always be accurate?  Actually, I think most ELF
     targets create section symbols for all sections anyhow.  */
  for (asect = abfd->sections; asect; asect = asect->next)
    {
      if (max_index < asect->index)
	max_index = asect->index;
    }

  max_index++;
  elf_num_section_syms (abfd) = max_index;
  sect_syms = (asymbol **) bfd_zalloc (abfd, max_index * sizeof (asymbol *));
  elf_section_syms (abfd) = sect_syms;

  if (sect_syms == 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  for (asect = abfd->sections; asect; asect = asect->next)
    {
      asymbol *sym = bfd_make_empty_symbol (abfd);
      if (!sym)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      sym->the_bfd = abfd;
      sym->name = asect->name;
      sym->value = asect->vma;
      sym->flags = BSF_SECTION_SYM;
      sym->section = asect;
      sect_syms[asect->index] = sym;
      num_sections++;
#ifdef DEBUG
      fprintf (stderr,
	       "creating section symbol, name = %s, value = 0x%.8lx, index = %d, section = 0x%.8lx\n",
	       asect->name, (long) asect->vma, asect->index, (long) asect);
#endif
    }

  if (num_sections)
    {
      if (syms)
	syms = (asymbol **) bfd_realloc (abfd, syms,
					 ((symcount + num_sections + 1)
					  * sizeof (asymbol *)));
      else
	syms = (asymbol **) bfd_alloc (abfd,
				   (num_sections + 1) * sizeof (asymbol *));
      if (!syms)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}

      for (asect = abfd->sections; asect; asect = asect->next)
	{
	  if (sect_syms[asect->index])
	    syms[symcount++] = sect_syms[asect->index];
	}

      syms[symcount] = (asymbol *) 0;
      bfd_set_symtab (abfd, syms, symcount);
    }

  elf_sym_extra (abfd) = sym_extra
    = (Elf_Sym_Extra *) bfd_alloc (abfd, symcount * sizeof (Elf_Sym_Extra));
  if (!sym_extra)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  /* Identify and classify all of the symbols.  */
  for (idx = 0; idx < symcount; idx++)
    {
      if (!sym_is_global (abfd, syms[idx]))
	num_locals++;
      else
	num_globals++;
    }

  /* Now provide mapping information.  Add +1 for skipping over the
     dummy symbol.  */
  for (idx = 0; idx < symcount; idx++)
    {
      syms[idx]->udata = (PTR) & sym_extra[idx];
      if (!sym_is_global (abfd, syms[idx]))
	sym_extra[idx].elf_sym_num = 1 + num_locals2++;
      else
	sym_extra[idx].elf_sym_num = 1 + num_locals + num_globals2++;
    }

  elf_num_locals (abfd) = num_locals;
  elf_num_globals (abfd) = num_globals;
  return true;
}

static boolean assign_section_numbers ();
static boolean assign_file_positions_except_relocs ();

static boolean
elf_compute_section_file_positions (abfd)
     bfd *abfd;
{
  bfd_map_over_sections (abfd, elf_fake_sections, 0);

  if (!assign_section_numbers (abfd))
    return false;

  bfd_map_over_sections (abfd, elf_make_sections, 0);

  bfd_map_over_sections (abfd, fix_up_strtabs, 0);	/* .stab/.stabstr &c */

  if (swap_out_syms (abfd) == false)
    return false;

  if (!assign_file_positions_except_relocs (abfd))
    return false;

  return true;
}

static boolean
elf_write_phdrs (abfd, i_ehdrp, i_phdrp, phdr_cnt)
     bfd *abfd;
     Elf_Internal_Ehdr *i_ehdrp;
     Elf_Internal_Phdr *i_phdrp;
     unsigned short phdr_cnt;
{
  /* first program header entry goes after the file header */
  int outbase = i_ehdrp->e_phoff;
  unsigned int i;
  Elf_External_Phdr x_phdr;

  for (i = 0; i < phdr_cnt; i++)
    {
      elf_swap_phdr_out (abfd, i_phdrp + i, &x_phdr);
      if (bfd_seek (abfd, outbase, SEEK_SET) != 0
	  || (bfd_write ((PTR) & x_phdr, sizeof (x_phdr), 1, abfd)
	      != sizeof (x_phdr)))
	return false;
      outbase += sizeof (x_phdr);
    }

  return true;
}

static const Elf_Internal_Shdr null_shdr;

/* Assign all ELF section numbers.  The dummy first section is handled here
   too.  The link/info pointers for the standard section types are filled
   in here too, while we're at it.  (Link pointers for .stab sections are
   not filled in here.)  */
static boolean
assign_section_numbers (abfd)
     bfd *abfd;
{
  struct elf_obj_tdata *t = elf_tdata (abfd);
  asection *sec;
  int section_number = 1;
  int i;
  Elf_Internal_Shdr **i_shdrp;

  t->shstrtab_hdr.sh_size = elf_shstrtab (abfd)->length;
  t->shstrtab_hdr.contents = (void *) elf_shstrtab (abfd)->tab;
  shstrtab_length_fixed = 1;

  t->shstrtab_section = section_number++;
  elf_elfheader (abfd)->e_shstrndx = t->shstrtab_section;
  if (abfd->symcount)
    {
      t->symtab_section = section_number++;
      t->strtab_section = section_number++;
      t->symtab_hdr.sh_link = t->strtab_section;
    }
  for (sec = abfd->sections; sec; sec = sec->next)
    {
      struct bfd_elf_section_data *d = elf_section_data (sec);
      d->this_idx = section_number++;
      if (sec->flags & SEC_RELOC)
	{
	  d->rel_idx = section_number++;
	  d->rel_hdr.sh_link = t->symtab_section;
	  d->rel_hdr.sh_info = d->this_idx;
	}
      else
	d->rel_idx = 0;
      /* No handling for per-section string tables currently.  */
    }
  elf_elfheader (abfd)->e_shnum = section_number;

  /* Set up the list of section header pointers, in agreement with the
     indices.  */
  i_shdrp = (Elf_Internal_Shdr **)
    bfd_alloc (abfd, section_number * sizeof (Elf_Internal_Shdr *));
  if (!i_shdrp)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  elf_elfsections (abfd) = i_shdrp;
  for (i = 0; i < section_number; i++)
    i_shdrp[i] = 0;

  i_shdrp[0] = (Elf_Internal_Shdr *) & null_shdr;
  i_shdrp[t->shstrtab_section] = &t->shstrtab_hdr;
  if (abfd->symcount)
    {
      i_shdrp[t->symtab_section] = &t->symtab_hdr;
      i_shdrp[t->strtab_section] = &t->strtab_hdr;
    }
  for (sec = abfd->sections; sec; sec = sec->next)
    {
      struct bfd_elf_section_data *d = elf_section_data (sec);
      i_shdrp[d->this_idx] = &d->this_hdr;
      if (d->rel_idx)
	i_shdrp[d->rel_idx] = &d->rel_hdr;
    }
  /* Make sure we got everything.... */
  for (i = 0; i < section_number; i++)
    if (i_shdrp[i] == 0)
      abort ();
  return true;
}

static INLINE file_ptr
assign_file_position_for_section (i_shdrp, offset)
     Elf_Internal_Shdr *i_shdrp;
     file_ptr offset;
{
  int align;

  if (i_shdrp->sh_addralign != 0)
    align = i_shdrp->sh_addralign;
  else
    align = 1;
  i_shdrp->sh_offset = offset = BFD_ALIGN (offset, align);
  if (i_shdrp->rawdata != NULL)
    ((asection *) i_shdrp->rawdata)->filepos = offset;
  if (i_shdrp->sh_type != SHT_NOBITS)
    offset += i_shdrp->sh_size;
  return offset;
}

static INLINE file_ptr
align_file_position (off)
     file_ptr off;
{
  return (off + FILE_ALIGN - 1) & ~(FILE_ALIGN - 1);
}

static INLINE file_ptr
assign_file_positions_for_symtab_and_strtabs (abfd, off)
     bfd *abfd;
     file_ptr off;
{
  struct elf_obj_tdata *t = elf_tdata (abfd);

  off = align_file_position (off);
  off = assign_file_position_for_section (&t->symtab_hdr, off);
  off = assign_file_position_for_section (&t->shstrtab_hdr, off);
  off = assign_file_position_for_section (&t->strtab_hdr, off);
  return off;
}

struct seg_info
{
  bfd_vma low, mem_size;
  file_ptr file_size;
  int start_pos;
  int sh_flags;
  struct seg_info *next;
};

static boolean
map_program_segments (abfd)
     bfd *abfd;
{
  Elf_Internal_Shdr **i_shdrpp = elf_elfsections (abfd);
  Elf_Internal_Ehdr *i_ehdrp = elf_elfheader (abfd);
  Elf_Internal_Shdr *i_shdrp;
  Elf_Internal_Phdr *phdr;
  char *done = NULL;
  unsigned int i, n_left = 0;
  file_ptr lowest_offset = 0;
  struct seg_info *seg = NULL;

  done = (char *) malloc (i_ehdrp->e_shnum);
  if (done == NULL && i_ehdrp->e_shnum != 0)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  memset (done, 0, i_ehdrp->e_shnum);
  for (i = 1; i < i_ehdrp->e_shnum; i++)
    {
      i_shdrp = i_shdrpp[i];
      /* If it's going to be mapped in, it's been assigned a position.  */
      if (i_shdrp->sh_offset + 1 == 0)
	{
	  /* Well, not really, but we won't process it here.   */
	  done[i] = 1;
	  continue;
	}
      if (i_shdrp->sh_offset < lowest_offset
	  || lowest_offset == 0)
	lowest_offset = i_shdrp->sh_offset;
      /* Only interested in PROGBITS or NOBITS for generating segments.  */
      switch (i_shdrp->sh_type)
	{
	case SHT_PROGBITS:
	case SHT_NOBITS:
	  break;
	default:
	  done[i] = 1;
	}
      if (!done[i])
	n_left++;
    }
  while (n_left)
    {
      bfd_vma lowest_vma = -1, high;
      int low_sec = 0;
      int mem_size;
      int file_size = 0;
      struct seg_info *snew;
      struct seg_info **s_ptr;

      for (i = 1; i < i_ehdrp->e_shnum; i++)
	{
	  i_shdrp = i_shdrpp[i];
	  if (!done[i] && i_shdrp->sh_addr < lowest_vma)
	    {
	      lowest_vma = i_shdrp->sh_addr;
	      low_sec = i;
	    }
	}
      if (low_sec == 0)
	abort ();
      /* So now we know the lowest vma of any unassigned sections; start
	 a segment there.  */
      snew = (struct seg_info *) bfd_alloc (abfd, sizeof (struct seg_info));
      if (!snew)
	{
	  bfd_set_error (bfd_error_no_memory);
	  goto error_return;
	}
      s_ptr = &seg;
      while (*s_ptr != (struct seg_info *) NULL)
	s_ptr = &(*s_ptr)->next;
      *s_ptr = snew;
      snew->next = NULL;
      snew->low = lowest_vma;
      i_shdrp = i_shdrpp[low_sec];
      snew->start_pos = i_shdrp->sh_offset;
      snew->sh_flags = i_shdrp->sh_flags;
      done[low_sec] = 1, n_left--;
      mem_size = i_shdrp->sh_size;
      high = lowest_vma + i_shdrp->sh_size;

      if (i_shdrp->sh_type == SHT_PROGBITS)
	file_size = i_shdrp->sh_size;

      for (i = 1; i < i_ehdrp->e_shnum; i++)
	{
	  file_ptr f1;

	  if (done[i])
	    continue;
	  i_shdrp = i_shdrpp[i];
	  /* position of next byte on disk */
	  f1 = snew->start_pos + file_size;
	  if (i_shdrp->sh_type == SHT_PROGBITS)
	    {
	      if (i_shdrp->sh_offset - f1 != i_shdrp->sh_addr - high)
		continue;
	      if (file_size != mem_size)
		break;
	    }
	  else
	    /* sh_type == NOBITS */
	    {
	      /* If the section in question has no contents in the disk
		 file, we really don't care where it supposedly starts.
		 But we don't want to bother merging it into this segment
		 if it doesn't start on this memory page.  */
	      bfd_vma page1, page2;
	      bfd_vma maxpagesize = get_elf_backend_data (abfd)->maxpagesize;

	      /* page number in address space of current end of snew */
	      page1 = (high - 1 + maxpagesize - 1) / maxpagesize;
	      /* page number in address space of start of this section */
	      page2 = (i_shdrp->sh_addr + maxpagesize - 1) / maxpagesize;

	      if (page1 != page2)
		continue;
	    }
	  done[i] = 1, n_left--;
	  if (i_shdrp->sh_type == SHT_PROGBITS)
	    file_size = i_shdrp->sh_offset + i_shdrp->sh_size - snew->start_pos;
	  mem_size = i_shdrp->sh_addr + i_shdrp->sh_size - snew->low;
	  high = i_shdrp->sh_addr + i_shdrp->sh_size;
	  i = 0;
	}
      snew->file_size = file_size;
      snew->mem_size = mem_size;
    }
  /* Now do something with the list of segments we've built up.  */
  {
    bfd_vma maxpagesize = get_elf_backend_data (abfd)->maxpagesize;
    struct seg_info *s;
    int n_segs = 0;
    int sz;

    for (s = seg; s; s = s->next)
      {
	n_segs++;
      }
    i_ehdrp->e_phentsize = sizeof (Elf_External_Phdr);
    sz = sizeof (Elf_External_Phdr) * n_segs;
    if (align_file_position (i_ehdrp->e_ehsize) + sz <= lowest_offset)
      i_ehdrp->e_phoff = align_file_position (i_ehdrp->e_ehsize);
    else
      {
	i_ehdrp->e_phoff = align_file_position (elf_tdata (abfd)->next_file_pos);
	elf_tdata (abfd)->next_file_pos = i_ehdrp->e_phoff + sz;
      }
    phdr = (Elf_Internal_Phdr *) bfd_alloc (abfd,
				       n_segs * sizeof (Elf_Internal_Phdr));
    if (!phdr)
      {
	bfd_set_error (bfd_error_no_memory);
	abort ();		/* FIXME */
      }
    elf_tdata (abfd)->phdr = phdr;
    while (seg)
      {
	phdr->p_type = PT_LOAD;	/* only type we really support so far */
	phdr->p_offset = seg->start_pos;
	phdr->p_vaddr = seg->low;
	phdr->p_paddr = 0;
	phdr->p_filesz = seg->file_size;
	phdr->p_memsz = seg->mem_size;
	phdr->p_flags = PF_R;
	phdr->p_align = maxpagesize;	/* ? */
	if (seg->sh_flags & SHF_WRITE)
	  /* SysVr4 ELF docs say "data segments normally have read, write,
	     and execute permissions."  */
	  phdr->p_flags |= (PF_W | PF_X);
	if (seg->sh_flags & SHF_EXECINSTR)
	  phdr->p_flags |= PF_X;
	phdr++;
	seg = seg->next;
      }
    i_ehdrp->e_phnum = n_segs;
  }
  elf_write_phdrs (abfd, i_ehdrp, elf_tdata (abfd)->phdr, i_ehdrp->e_phnum);
  if (done != NULL)
    free (done);
  return true;
error_return:
  if (done != NULL)
    free (done);
  return false;
}

static boolean
assign_file_positions_except_relocs (abfd)
     bfd *abfd;
{
  /* For now, we ignore the possibility of having program segments, which
     may require some alignment in the file.  That'll require padding, and
     some interesting calculations to optimize file space usage.

     Also, since the application may change the list of relocations for
     a given section, we don't figure them in here.  We'll put them at the
     end of the file, at positions computed during bfd_close.

     The order, for now: <ehdr> <shdr> <sec1> <sec2> <sec3> ... <rel1> ...
     or:                 <ehdr> <phdr> <sec1> <sec2> ... <shdr> <rel1> ... */

  struct elf_obj_tdata *t = elf_tdata (abfd);
  file_ptr off;
  unsigned int i;
  Elf_Internal_Shdr **i_shdrpp = elf_elfsections (abfd);
  Elf_Internal_Shdr *i_shdrp;
  Elf_Internal_Ehdr *i_ehdrp = elf_elfheader (abfd);
  int exec_p = (abfd->flags & EXEC_P) != 0;
  bfd_vma maxpagesize = get_elf_backend_data (abfd)->maxpagesize;

  /* Everything starts after the ELF file header.  */
  off = i_ehdrp->e_ehsize;

  if (!exec_p)
    {
      /* Section headers.  */
      off = align_file_position (off);
      i_ehdrp->e_shoff = off;
      off += i_ehdrp->e_shnum * i_ehdrp->e_shentsize;
      off = assign_file_positions_for_symtab_and_strtabs (abfd, off);
    }
  for (i = 1; i < i_ehdrp->e_shnum; i++)
    {
      /* The symtab and strtab sections are placed by
	 assign_file_positions_for_symtab_and_strtabs.  */
      if (i == t->symtab_section
	  || i == t->strtab_section
	  || i == t->shstrtab_section)
	continue;

      i_shdrp = i_shdrpp[i];
      if (i_shdrp->sh_type == SHT_REL || i_shdrp->sh_type == SHT_RELA)
	{
	  i_shdrp->sh_offset = -1;
	  continue;
	}
      if (exec_p)
	{
	  if (maxpagesize == 0)
	    maxpagesize = 1;	/* make the arithmetic work */
	  /* This isn't necessarily going to give the best packing, if the
	     segments require padding between them, but since that isn't
	     usually the case, this'll do.  */
	  if ((i_shdrp->sh_flags & SHF_ALLOC) == 0)
	    {
	      i_shdrp->sh_offset = -1;
	      continue;
	    }
	  /* Blindly assume that the segments are ordered optimally.  With
	     the default LD script, they will be.  */
	  if (i_shdrp->sh_type != SHT_NOBITS)
	    {
	      /* need big unsigned type */
	      bfd_vma addtl_off;
	      addtl_off = i_shdrp->sh_addr - off;
	      addtl_off = addtl_off % maxpagesize;
	      if (addtl_off)
		{
		  off += addtl_off;
		}
	    }
	}
      off = assign_file_position_for_section (i_shdrp, off);

      if (exec_p
	  && i_shdrp->sh_type == SHT_NOBITS
	  && (i == i_ehdrp->e_shnum
	      || i_shdrpp[i + 1]->sh_type != SHT_NOBITS))
	{
	  /* Skip to the next page to ensure that when the file is
	     loaded the bss section is loaded with zeroes.  I don't
	     know if this is required on all platforms, but it
	     shouldn't really hurt.  */
	  off = BFD_ALIGN (off, maxpagesize);
	}

      if (exec_p
	  && (abfd->flags & D_PAGED) != 0
	  && get_elf_backend_data (abfd)->maxpagesize > 1
	  && i_shdrp->sh_type == SHT_PROGBITS
	  && (i_shdrp->sh_flags & SHF_ALLOC)
	  && (i_shdrp->sh_offset - i_shdrp->sh_addr) % get_elf_backend_data (abfd)->maxpagesize != 0)
	abort ();
    }
  if (exec_p)
    {
      elf_tdata (abfd)->next_file_pos = off;
      if (!map_program_segments (abfd))
	return false;
      off = elf_tdata (abfd)->next_file_pos;

      /* Section headers.  */
      off = align_file_position (off);
      i_ehdrp->e_shoff = off;
      off += i_ehdrp->e_shnum * i_ehdrp->e_shentsize;

      off = assign_file_positions_for_symtab_and_strtabs (abfd, off);

      for (i = 1; i < i_ehdrp->e_shnum; i++)
	{
	  i_shdrp = i_shdrpp[i];
	  if (i_shdrp->sh_offset + 1 == 0
	      && i_shdrp->sh_type != SHT_REL
	      && i_shdrp->sh_type != SHT_RELA)
	    off = assign_file_position_for_section (i_shdrp, off);
	}
    }
  elf_tdata (abfd)->next_file_pos = off;
  return true;
}

static boolean
prep_headers (abfd)
     bfd *abfd;
{
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_Internal_Phdr *i_phdrp = 0;	/* Program header table, internal form */
  Elf_Internal_Shdr **i_shdrp;	/* Section header table, internal form */
  int count;
  struct strtab *shstrtab;

  i_ehdrp = elf_elfheader (abfd);
  i_shdrp = elf_elfsections (abfd);

  shstrtab = bfd_new_strtab (abfd);
  if (!shstrtab)
    return false;

  elf_shstrtab (abfd) = shstrtab;

  i_ehdrp->e_ident[EI_MAG0] = ELFMAG0;
  i_ehdrp->e_ident[EI_MAG1] = ELFMAG1;
  i_ehdrp->e_ident[EI_MAG2] = ELFMAG2;
  i_ehdrp->e_ident[EI_MAG3] = ELFMAG3;

  i_ehdrp->e_ident[EI_CLASS] = ELFCLASS;
  i_ehdrp->e_ident[EI_DATA] =
    abfd->xvec->byteorder_big_p ? ELFDATA2MSB : ELFDATA2LSB;
  i_ehdrp->e_ident[EI_VERSION] = EV_CURRENT;

  for (count = EI_PAD; count < EI_NIDENT; count++)
    i_ehdrp->e_ident[count] = 0;

  i_ehdrp->e_type = (abfd->flags & EXEC_P) ? ET_EXEC : ET_REL;
  switch (bfd_get_arch (abfd))
    {
    case bfd_arch_unknown:
      i_ehdrp->e_machine = EM_NONE;
      break;
    case bfd_arch_sparc:
      i_ehdrp->e_machine = EM_SPARC;
      /* start-sanitize-v9 */
#if ARCH_SIZE == 64
      i_ehdrp->e_machine = EM_SPARC64;
#endif
      /* end-sanitize-v9 */
      break;
    case bfd_arch_i386:
      i_ehdrp->e_machine = EM_386;
      break;
    case bfd_arch_m68k:
      i_ehdrp->e_machine = EM_68K;
      break;
    case bfd_arch_m88k:
      i_ehdrp->e_machine = EM_88K;
      break;
    case bfd_arch_i860:
      i_ehdrp->e_machine = EM_860;
      break;
    case bfd_arch_mips:	/* MIPS Rxxxx */
      i_ehdrp->e_machine = EM_MIPS;	/* only MIPS R3000 */
      break;
    case bfd_arch_hppa:
      i_ehdrp->e_machine = EM_HPPA;
      break;
    case bfd_arch_powerpc:
      i_ehdrp->e_machine = EM_CYGNUS_POWERPC;
      break;
      /* also note that EM_M32, AT&T WE32100 is unknown to bfd */
    default:
      i_ehdrp->e_machine = EM_NONE;
    }
  i_ehdrp->e_version = EV_CURRENT;
  i_ehdrp->e_ehsize = sizeof (Elf_External_Ehdr);

  /* no program header, for now. */
  i_ehdrp->e_phoff = 0;
  i_ehdrp->e_phentsize = 0;
  i_ehdrp->e_phnum = 0;

  /* each bfd section is section header entry */
  i_ehdrp->e_entry = bfd_get_start_address (abfd);
  i_ehdrp->e_shentsize = sizeof (Elf_External_Shdr);

  /* if we're building an executable, we'll need a program header table */
  if (abfd->flags & EXEC_P)
    {
      /* it all happens later */
#if 0
      i_ehdrp->e_phentsize = sizeof (Elf_External_Phdr);

      /* elf_build_phdrs() returns a (NULL-terminated) array of
	 Elf_Internal_Phdrs */
      i_phdrp = elf_build_phdrs (abfd, i_ehdrp, i_shdrp, &i_ehdrp->e_phnum);
      i_ehdrp->e_phoff = outbase;
      outbase += i_ehdrp->e_phentsize * i_ehdrp->e_phnum;
#endif
    }
  else
    {
      i_ehdrp->e_phentsize = 0;
      i_phdrp = 0;
      i_ehdrp->e_phoff = 0;
    }

  elf_tdata (abfd)->symtab_hdr.sh_name = bfd_add_to_strtab (abfd, shstrtab,
							    ".symtab");
  elf_tdata (abfd)->strtab_hdr.sh_name = bfd_add_to_strtab (abfd, shstrtab,
							    ".strtab");
  elf_tdata (abfd)->shstrtab_hdr.sh_name = bfd_add_to_strtab (abfd, shstrtab,
							      ".shstrtab");
  return true;
}

static boolean
swap_out_syms (abfd)
     bfd *abfd;
{
  if (!elf_map_symbols (abfd))
    return false;

  /* Dump out the symtabs. */
  {
    int symcount = bfd_get_symcount (abfd);
    asymbol **syms = bfd_get_outsymbols (abfd);
    struct strtab *stt = bfd_new_strtab (abfd);
    Elf_Internal_Shdr *symtab_hdr;
    Elf_Internal_Shdr *symstrtab_hdr;
    Elf_External_Sym *outbound_syms;
    int idx;

    if (!stt)
      return false;
    symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
    symtab_hdr->sh_type = SHT_SYMTAB;
    symtab_hdr->sh_entsize = sizeof (Elf_External_Sym);
    symtab_hdr->sh_size = symtab_hdr->sh_entsize * (symcount + 1);
    symtab_hdr->sh_info = elf_num_locals (abfd) + 1;

    /* FIXME: Systems I've checked use 4 byte alignment for .symtab,
       but it is possible that there are systems which use a different
       alignment.  */
    symtab_hdr->sh_addralign = 4;

    /* see assert in elf_fake_sections that supports this: */
    symstrtab_hdr = &elf_tdata (abfd)->strtab_hdr;
    symstrtab_hdr->sh_type = SHT_STRTAB;

    outbound_syms = (Elf_External_Sym *)
      bfd_alloc (abfd, (1 + symcount) * sizeof (Elf_External_Sym));
    if (!outbound_syms)
      {
	bfd_set_error (bfd_error_no_memory);
	return false;
      }
    /* now generate the data (for "contents") */
    {
      /* Fill in zeroth symbol and swap it out.  */
      Elf_Internal_Sym sym;
      sym.st_name = 0;
      sym.st_value = 0;
      sym.st_size = 0;
      sym.st_info = 0;
      sym.st_other = 0;
      sym.st_shndx = SHN_UNDEF;
      elf_swap_symbol_out (abfd, &sym, outbound_syms);
    }
    for (idx = 0; idx < symcount; idx++)
      {
	Elf_Internal_Sym sym;
	bfd_vma value = syms[idx]->value;

	if (syms[idx]->flags & BSF_SECTION_SYM)
	  /* Section symbols have no names.  */
	  sym.st_name = 0;
	else
	  sym.st_name = bfd_add_to_strtab (abfd, stt, syms[idx]->name);

	if (bfd_is_com_section (syms[idx]->section))
	  {
	    /* ELF common symbols put the alignment into the `value' field,
	       and the size into the `size' field.  This is backwards from
	       how BFD handles it, so reverse it here.  */
	    sym.st_size = value;
	    /* Should retrieve this from somewhere... */
	    sym.st_value = 16;
	    sym.st_shndx = elf_section_from_bfd_section (abfd,
							 syms[idx]->section);
	  }
	else
	  {
	    asection *sec = syms[idx]->section;
	    elf_symbol_type *type_ptr;
	    int shndx;

	    if (sec->output_section)
	      {
		value += sec->output_offset;
		sec = sec->output_section;
	      }
	    value += sec->vma;
	    sym.st_value = value;
	    type_ptr = elf_symbol_from (abfd, syms[idx]);
	    sym.st_size = type_ptr ? type_ptr->internal_elf_sym.st_size : 0;
	    sym.st_shndx = shndx = elf_section_from_bfd_section (abfd, sec);
	    if (shndx == -1)
	      {
		asection *sec2;
		/* Writing this would be a hell of a lot easier if we had
		   some decent documentation on bfd, and knew what to expect
		   of the library, and what to demand of applications.  For
		   example, it appears that `objcopy' might not set the
		   section of a symbol to be a section that is actually in
		   the output file.  */
		sec2 = bfd_get_section_by_name (abfd, sec->name);
		BFD_ASSERT (sec2 != 0);
		sym.st_shndx = shndx = elf_section_from_bfd_section (abfd, sec2);
		BFD_ASSERT (shndx != -1);
	      }
	  }

	if (bfd_is_com_section (syms[idx]->section))
	  sym.st_info = ELF_ST_INFO (STB_GLOBAL, STT_OBJECT);
	else if (syms[idx]->section == &bfd_und_section)
	  sym.st_info = ELF_ST_INFO (STB_GLOBAL, STT_NOTYPE);
	else if (syms[idx]->flags & BSF_SECTION_SYM)
	  sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_SECTION);
	else if (syms[idx]->flags & BSF_FILE)
	  sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_FILE);
	else
	  {
	    int bind = STB_LOCAL;
	    int type = STT_OBJECT;
	    unsigned int flags = syms[idx]->flags;

	    if (flags & BSF_LOCAL)
	      bind = STB_LOCAL;
	    else if (flags & BSF_WEAK)
	      bind = STB_WEAK;
	    else if (flags & BSF_GLOBAL)
	      bind = STB_GLOBAL;

	    if (flags & BSF_FUNCTION)
	      type = STT_FUNC;

	    sym.st_info = ELF_ST_INFO (bind, type);
	  }

	sym.st_other = 0;
	elf_swap_symbol_out (abfd, &sym,
			     (outbound_syms
			      + elf_sym_extra (abfd)[idx].elf_sym_num));
      }

    symtab_hdr->contents = (PTR) outbound_syms;
    symstrtab_hdr->contents = (PTR) stt->tab;
    symstrtab_hdr->sh_size = stt->length;
    symstrtab_hdr->sh_type = SHT_STRTAB;

    symstrtab_hdr->sh_flags = 0;
    symstrtab_hdr->sh_addr = 0;
    symstrtab_hdr->sh_entsize = 0;
    symstrtab_hdr->sh_link = 0;
    symstrtab_hdr->sh_info = 0;
    symstrtab_hdr->sh_addralign = 1;
    symstrtab_hdr->size = 0;
  }

  /* put the strtab out too... */
  {
    Elf_Internal_Shdr *this_hdr;

    this_hdr = &elf_tdata (abfd)->shstrtab_hdr;
    this_hdr->contents = (PTR) elf_shstrtab (abfd)->tab;
    this_hdr->sh_size = elf_shstrtab (abfd)->length;
    this_hdr->sh_type = SHT_STRTAB;
    this_hdr->sh_flags = 0;
    this_hdr->sh_addr = 0;
    this_hdr->sh_entsize = 0;
    this_hdr->sh_addralign = 1;
    this_hdr->size = 0;
  }
  return true;
}

static boolean
write_shdrs_and_ehdr (abfd)
     bfd *abfd;
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_External_Shdr *x_shdrp;	/* Section header table, external form */
  Elf_Internal_Shdr **i_shdrp;	/* Section header table, internal form */
  unsigned int count;
  struct strtab *shstrtab;

  i_ehdrp = elf_elfheader (abfd);
  i_shdrp = elf_elfsections (abfd);
  shstrtab = elf_shstrtab (abfd);

  /* swap the header before spitting it out... */

#if DEBUG & 1
  elf_debug_file (i_ehdrp);
#endif
  elf_swap_ehdr_out (abfd, i_ehdrp, &x_ehdr);
  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0
      || (bfd_write ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd)
	  != sizeof (x_ehdr)))
    return false;

  /* at this point we've concocted all the ELF sections... */
  x_shdrp = (Elf_External_Shdr *)
    bfd_alloc (abfd, sizeof (*x_shdrp) * (i_ehdrp->e_shnum));
  if (!x_shdrp)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  for (count = 0; count < i_ehdrp->e_shnum; count++)
    {
#if DEBUG & 2
      elf_debug_section (shstrtab->tab + i_shdrp[count]->sh_name, count,
			 i_shdrp[count]);
#endif
      elf_swap_shdr_out (abfd, i_shdrp[count], x_shdrp + count);
    }
  if (bfd_seek (abfd, (file_ptr) i_ehdrp->e_shoff, SEEK_SET) != 0
      || (bfd_write ((PTR) x_shdrp, sizeof (*x_shdrp), i_ehdrp->e_shnum, abfd)
	  != sizeof (*x_shdrp) * i_ehdrp->e_shnum))
    return false;

  /* need to dump the string table too... */

  return true;
}

static void
assign_file_positions_for_relocs (abfd)
     bfd *abfd;
{
  file_ptr off = elf_tdata (abfd)->next_file_pos;
  unsigned int i;
  Elf_Internal_Shdr **shdrpp = elf_elfsections (abfd);
  Elf_Internal_Shdr *shdrp;
  for (i = 1; i < elf_elfheader (abfd)->e_shnum; i++)
    {
      shdrp = shdrpp[i];
      if (shdrp->sh_type != SHT_REL && shdrp->sh_type != SHT_RELA)
	continue;
      off = align_file_position (off);
      off = assign_file_position_for_section (shdrp, off);
    }
  elf_tdata (abfd)->next_file_pos = off;
}

boolean
NAME(bfd_elf,write_object_contents) (abfd)
     bfd *abfd;
{
  struct elf_backend_data *bed = get_elf_backend_data (abfd);
  Elf_Internal_Ehdr *i_ehdrp;
  Elf_Internal_Shdr **i_shdrp;
  unsigned int count;

  /* We don't know how to write dynamic objects.  Specifically, we
     don't know how to construct the program header.  */
  if ((abfd->flags & DYNAMIC) != 0)
    {
      fprintf (stderr, "Writing ELF dynamic objects is not supported\n");
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  if (abfd->output_has_begun == false)
    {
      /* Do any elf backend specific processing first.  */
      if (bed->elf_backend_begin_write_processing)
	(*bed->elf_backend_begin_write_processing) (abfd);

      if (prep_headers (abfd) == false)
	return false;
      if (elf_compute_section_file_positions (abfd) == false)
	return false;
      abfd->output_has_begun = true;
    }

  i_shdrp = elf_elfsections (abfd);
  i_ehdrp = elf_elfheader (abfd);

  bfd_map_over_sections (abfd, write_relocs, (PTR) 0);
  assign_file_positions_for_relocs (abfd);

  /* After writing the headers, we need to write the sections too... */
  for (count = 1; count < i_ehdrp->e_shnum; count++)
    {
      if (bed->elf_backend_section_processing)
	(*bed->elf_backend_section_processing) (abfd, i_shdrp[count]);
      if (i_shdrp[count]->contents)
	{
	  if (bfd_seek (abfd, i_shdrp[count]->sh_offset, SEEK_SET) != 0
	      || (bfd_write (i_shdrp[count]->contents, i_shdrp[count]->sh_size,
			     1, abfd)
		  != i_shdrp[count]->sh_size))
	    return false;
	}
    }

  if (bed->elf_backend_final_write_processing)
    (*bed->elf_backend_final_write_processing) (abfd);

  return write_shdrs_and_ehdr (abfd);
}

/* Given an index of a section, retrieve a pointer to it.  Note
   that for our purposes, sections are indexed by {1, 2, ...} with
   0 being an illegal index. */

/* In the original, each ELF section went into exactly one BFD
   section. This doesn't really make sense, so we need a real mapping.
   The mapping has to hide in the Elf_Internal_Shdr since asection
   doesn't have anything like a tdata field... */

static asection *
section_from_elf_index (abfd, index)
     bfd *abfd;
     unsigned int index;
{
  /* @@ Is bfd_com_section really correct in all the places it could
     be returned from this routine?  */

  if (index == SHN_ABS)
    return &bfd_com_section;	/* not abs? */
  if (index == SHN_COMMON)
    return &bfd_com_section;

  if (index > elf_elfheader (abfd)->e_shnum)
    return NULL;

  {
    Elf_Internal_Shdr *hdr = elf_elfsections (abfd)[index];

    switch (hdr->sh_type)
      {
	/* ELF sections that map to BFD sections */
      case SHT_PROGBITS:
      case SHT_NOBITS:
	if (hdr->rawdata == NULL)
	  {
	    if (! bfd_section_from_shdr (abfd, index))
	      return NULL;
	  }
	return (struct sec *) hdr->rawdata;

      default:
	return (struct sec *) &bfd_abs_section;
      }
  }
}

/* given a section, search the header to find them... */
static int
elf_section_from_bfd_section (abfd, asect)
     bfd *abfd;
     struct sec *asect;
{
  Elf_Internal_Shdr **i_shdrp = elf_elfsections (abfd);
  int index;
  Elf_Internal_Shdr *hdr;
  int maxindex = elf_elfheader (abfd)->e_shnum;

  if (asect == &bfd_abs_section)
    return SHN_ABS;
  if (asect == &bfd_com_section)
    return SHN_COMMON;
  if (asect == &bfd_und_section)
    return SHN_UNDEF;

  for (index = 0; index < maxindex; index++)
    {
      hdr = i_shdrp[index];
      switch (hdr->sh_type)
	{
	  /* ELF sections that map to BFD sections */
	case SHT_PROGBITS:
	case SHT_NOBITS:
	case SHT_NOTE:
	  if (hdr->rawdata)
	    {
	      if (((struct sec *) (hdr->rawdata)) == asect)
		return index;
	    }
	  break;

	case SHT_STRTAB:
	  /* fix_up_strtabs will generate STRTAB sections with names
	     of .stab*str.  */
	  if (!strncmp (asect->name, ".stab", 5)
	      && !strcmp ("str", asect->name + strlen (asect->name) - 3))
	    {
	      if (hdr->rawdata)
		{
		  if (((struct sec *) (hdr->rawdata)) == asect)
		    return index;
		}
	      break;
	    }
	  /* FALL THROUGH */
	default:
	  {
	    struct elf_backend_data *bed = get_elf_backend_data (abfd);

	    if (bed->elf_backend_section_from_bfd_section)
	      {
		int retval;

		retval = index;
		if ((*bed->elf_backend_section_from_bfd_section)
		    (abfd, hdr, asect, &retval))
		  return retval;
	      }
	  }
	  break;
	}
    }
  return -1;
}

/* given a symbol, return the bfd index for that symbol.  */
static int
elf_symbol_from_bfd_symbol (abfd, asym_ptr_ptr)
     bfd *abfd;
     struct symbol_cache_entry **asym_ptr_ptr;
{
  struct symbol_cache_entry *asym_ptr = *asym_ptr_ptr;
  int idx;
  flagword flags = asym_ptr->flags;

  /* When gas creates relocations against local labels, it creates its
     own symbol for the section, but does put the symbol into the
     symbol chain, so udata is 0.  When the linker is generating
     relocatable output, this section symbol may be for one of the
     input sections rather than the output section.  */
  if (asym_ptr->udata == (PTR) 0
      && (flags & BSF_SECTION_SYM)
      && asym_ptr->section)
    {
      int indx;

      if (asym_ptr->section->output_section != NULL)
	indx = asym_ptr->section->output_section->index;
      else
	indx = asym_ptr->section->index;
      if (elf_section_syms (abfd)[indx])
	asym_ptr->udata = elf_section_syms (abfd)[indx]->udata;
    }

  if (asym_ptr->udata)
    idx = ((Elf_Sym_Extra *) asym_ptr->udata)->elf_sym_num;
  else
    {
      abort ();
    }

#if DEBUG & 4
  {

    fprintf (stderr,
	     "elf_symbol_from_bfd_symbol 0x%.8lx, name = %s, sym num = %d, flags = 0x%.8lx %s\n",
     (long) asym_ptr, asym_ptr->name, idx, flags, elf_symbol_flags (flags));
    fflush (stderr);
  }
#endif

  return idx;
}

static long
elf_slurp_symbol_table (abfd, symptrs, dynamic)
     bfd *abfd;
     asymbol **symptrs;		/* Buffer for generated bfd symbols */
     boolean dynamic;
{
  Elf_Internal_Shdr *hdr;
  long symcount;		/* Number of external ELF symbols */
  elf_symbol_type *sym;		/* Pointer to current bfd symbol */
  elf_symbol_type *symbase;	/* Buffer for generated bfd symbols */
  Elf_Internal_Sym i_sym;
  Elf_External_Sym *x_symp = NULL;

  /* Read each raw ELF symbol, converting from external ELF form to
     internal ELF form, and then using the information to create a
     canonical bfd symbol table entry.

     Note that we allocate the initial bfd canonical symbol buffer
     based on a one-to-one mapping of the ELF symbols to canonical
     symbols.  We actually use all the ELF symbols, so there will be no
     space left over at the end.  When we have all the symbols, we
     build the caller's pointer vector. */

  if (dynamic)
    hdr = &elf_tdata (abfd)->dynsymtab_hdr;
  else
    hdr = &elf_tdata (abfd)->symtab_hdr;
  if (bfd_seek (abfd, hdr->sh_offset, SEEK_SET) == -1)
    return -1;

  symcount = hdr->sh_size / sizeof (Elf_External_Sym);

  if (symcount == 0)
    sym = symbase = NULL;
  else
    {
      long i;

      if (bfd_seek (abfd, hdr->sh_offset, SEEK_SET) == -1)
	return -1;

      symbase = ((elf_symbol_type *)
		 bfd_zalloc (abfd, symcount * sizeof (elf_symbol_type)));
      if (symbase == (elf_symbol_type *) NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return -1;
	}
      sym = symbase;

      /* Temporarily allocate room for the raw ELF symbols.  */
      x_symp = ((Elf_External_Sym *)
		malloc (symcount * sizeof (Elf_External_Sym)));
      if (x_symp == NULL && symcount != 0)
	{
	  bfd_set_error (bfd_error_no_memory);
	  goto error_return;
	}

      if (bfd_read ((PTR) x_symp, sizeof (Elf_External_Sym), symcount, abfd)
	  != symcount * sizeof (Elf_External_Sym))
	goto error_return;
      /* Skip first symbol, which is a null dummy.  */
      for (i = 1; i < symcount; i++)
	{
	  elf_swap_symbol_in (abfd, x_symp + i, &i_sym);
	  memcpy (&sym->internal_elf_sym, &i_sym, sizeof (Elf_Internal_Sym));
#ifdef ELF_KEEP_EXTSYM
	  memcpy (&sym->native_elf_sym, x_symp + i, sizeof (Elf_External_Sym));
#endif
	  sym->symbol.the_bfd = abfd;

	  sym->symbol.name = elf_string_from_elf_section (abfd, hdr->sh_link,
							  i_sym.st_name);

	  sym->symbol.value = i_sym.st_value;

	  if (i_sym.st_shndx > 0 && i_sym.st_shndx < SHN_LORESERV)
	    {
	      sym->symbol.section = section_from_elf_index (abfd,
							    i_sym.st_shndx);
	      if (sym->symbol.section == NULL)
		goto error_return;
	    }
	  else if (i_sym.st_shndx == SHN_ABS)
	    {
	      sym->symbol.section = &bfd_abs_section;
	    }
	  else if (i_sym.st_shndx == SHN_COMMON)
	    {
	      sym->symbol.section = &bfd_com_section;
	      /* Elf puts the alignment into the `value' field, and
		 the size into the `size' field.  BFD wants to see the
		 size in the value field, and doesn't care (at the
		 moment) about the alignment.  */
	      sym->symbol.value = i_sym.st_size;
	    }
	  else if (i_sym.st_shndx == SHN_UNDEF)
	    {
	      sym->symbol.section = &bfd_und_section;
	    }
	  else
	    sym->symbol.section = &bfd_abs_section;

	  sym->symbol.value -= sym->symbol.section->vma;

	  switch (ELF_ST_BIND (i_sym.st_info))
	    {
	    case STB_LOCAL:
	      sym->symbol.flags |= BSF_LOCAL;
	      break;
	    case STB_GLOBAL:
	      sym->symbol.flags |= BSF_GLOBAL;
	      break;
	    case STB_WEAK:
	      sym->symbol.flags |= BSF_WEAK;
	      break;
	    }

	  switch (ELF_ST_TYPE (i_sym.st_info))
	    {
	    case STT_SECTION:
	      sym->symbol.flags |= BSF_SECTION_SYM | BSF_DEBUGGING;
	      break;
	    case STT_FILE:
	      sym->symbol.flags |= BSF_FILE | BSF_DEBUGGING;
	      break;
	    case STT_FUNC:
	      sym->symbol.flags |= BSF_FUNCTION;
	      break;
	    }

	  if (dynamic)
	    sym->symbol.flags |= BSF_DYNAMIC;

	  /* Do some backend-specific processing on this symbol.  */
	  {
	    struct elf_backend_data *ebd = get_elf_backend_data (abfd);
	    if (ebd->elf_backend_symbol_processing)
	      (*ebd->elf_backend_symbol_processing) (abfd, &sym->symbol);
	  }

	  sym++;
	}
    }

  /* Do some backend-specific processing on this symbol table.  */
  {
    struct elf_backend_data *ebd = get_elf_backend_data (abfd);
    if (ebd->elf_backend_symbol_table_processing)
      (*ebd->elf_backend_symbol_table_processing) (abfd, symbase, symcount);
  }

  /* We rely on the zalloc to clear out the final symbol entry.  */

  symcount = sym - symbase;

  /* Fill in the user's symbol pointer vector if needed.  */
  if (symptrs)
    {
      long l = symcount;

      sym = symbase;
      while (l-- > 0)
	{
	  *symptrs++ = &sym->symbol;
	  sym++;
	}
      *symptrs = 0;		/* Final null pointer */
    }

  if (x_symp != NULL)
    free (x_symp);
  return symcount;
error_return:
  if (x_symp != NULL)
    free (x_symp);
  return -1;
}

/* Return the number of bytes required to hold the symtab vector.

   Note that we base it on the count plus 1, since we will null terminate
   the vector allocated based on this size.  However, the ELF symbol table
   always has a dummy entry as symbol #0, so it ends up even.  */

long
elf_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  long symcount;
  long symtab_size;
  Elf_Internal_Shdr *hdr = &elf_tdata (abfd)->symtab_hdr;

  symcount = hdr->sh_size / sizeof (Elf_External_Sym);
  symtab_size = (symcount - 1 + 1) * (sizeof (asymbol *));

  return symtab_size;
}

long
elf_get_dynamic_symtab_upper_bound (abfd)
     bfd *abfd;
{
  long symcount;
  long symtab_size;
  Elf_Internal_Shdr *hdr = &elf_tdata (abfd)->dynsymtab_hdr;

  symcount = hdr->sh_size / sizeof (Elf_External_Sym);
  symtab_size = (symcount - 1 + 1) * (sizeof (asymbol *));

  return symtab_size;
}

/*
	This function return the number of bytes required to store the
	relocation information associated with section <<sect>>
	attached to bfd <<abfd>>

*/
long
elf_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (asect->flags & SEC_RELOC)
    {
      /* either rel or rela */
      return elf_section_data (asect)->rel_hdr.sh_size;
    }
  else
    return 0;
}

static boolean
elf_slurp_reloca_table (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  Elf_External_Rela *native_relocs;
  arelent *reloc_cache;
  arelent *cache_ptr;

  unsigned int idx;

  if (asect->relocation)
    return true;
  if (asect->reloc_count == 0)
    return true;
  if (asect->flags & SEC_CONSTRUCTOR)
    return true;

  if (bfd_seek (abfd, asect->rel_filepos, SEEK_SET) != 0)
    return false;
  native_relocs = (Elf_External_Rela *)
    bfd_alloc (abfd, asect->reloc_count * sizeof (Elf_External_Rela));
  if (!native_relocs)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  if (bfd_read ((PTR) native_relocs,
		sizeof (Elf_External_Rela), asect->reloc_count, abfd)
      != sizeof (Elf_External_Rela) * asect->reloc_count)
    return false;

  reloc_cache = (arelent *)
    bfd_alloc (abfd, (size_t) (asect->reloc_count * sizeof (arelent)));

  if (!reloc_cache)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  for (idx = 0; idx < asect->reloc_count; idx++)
    {
      Elf_Internal_Rela dst;
      Elf_External_Rela *src;

      cache_ptr = reloc_cache + idx;
      src = native_relocs + idx;
      elf_swap_reloca_in (abfd, src, &dst);

#ifdef RELOC_PROCESSING
      RELOC_PROCESSING (cache_ptr, &dst, symbols, abfd, asect);
#else
      if (asect->flags & SEC_RELOC)
	{
	  /* relocatable, so the offset is off of the section */
	  cache_ptr->address = dst.r_offset + asect->vma;
	}
      else
	{
	  /* non-relocatable, so the offset a virtual address */
	  cache_ptr->address = dst.r_offset;
	}

      /* ELF_R_SYM(dst.r_info) is the symbol table offset.  An offset
	 of zero points to the dummy symbol, which was not read into
	 the symbol table SYMBOLS.  */
      if (ELF_R_SYM (dst.r_info) == 0)
	cache_ptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;
      else
	{
	  asymbol *s;

	  cache_ptr->sym_ptr_ptr = symbols + ELF_R_SYM (dst.r_info) - 1;

	  /* Translate any ELF section symbol into a BFD section
	     symbol.  */
	  s = *(cache_ptr->sym_ptr_ptr);
	  if (s->flags & BSF_SECTION_SYM)
	    {
	      cache_ptr->sym_ptr_ptr = s->section->symbol_ptr_ptr;
	      s = *cache_ptr->sym_ptr_ptr;
	      if (s->name == 0 || s->name[0] == 0)
		abort ();
	    }
	}
      cache_ptr->addend = dst.r_addend;

      /* Fill in the cache_ptr->howto field from dst.r_type */
      {
	struct elf_backend_data *ebd = get_elf_backend_data (abfd);
	(*ebd->elf_info_to_howto) (abfd, cache_ptr, &dst);
      }
#endif
    }

  asect->relocation = reloc_cache;
  return true;
}

#ifdef DEBUG
static void
elf_debug_section (str, num, hdr)
     char *str;
     int num;
     Elf_Internal_Shdr *hdr;
{
  fprintf (stderr, "\nSection#%d '%s' 0x%.8lx\n", num, str, (long) hdr);
  fprintf (stderr,
	   "sh_name      = %ld\tsh_type      = %ld\tsh_flags     = %ld\n",
	   (long) hdr->sh_name,
	   (long) hdr->sh_type,
	   (long) hdr->sh_flags);
  fprintf (stderr,
	   "sh_addr      = %ld\tsh_offset    = %ld\tsh_size      = %ld\n",
	   (long) hdr->sh_addr,
	   (long) hdr->sh_offset,
	   (long) hdr->sh_size);
  fprintf (stderr,
	   "sh_link      = %ld\tsh_info      = %ld\tsh_addralign = %ld\n",
	   (long) hdr->sh_link,
	   (long) hdr->sh_info,
	   (long) hdr->sh_addralign);
  fprintf (stderr, "sh_entsize   = %ld\n",
	   (long) hdr->sh_entsize);
  fprintf (stderr, "rawdata      = 0x%.8lx\n", (long) hdr->rawdata);
  fprintf (stderr, "contents     = 0x%.8lx\n", (long) hdr->contents);
  fprintf (stderr, "size         = %ld\n", (long) hdr->size);
  fflush (stderr);
}

static void
elf_debug_file (ehdrp)
     Elf_Internal_Ehdr *ehdrp;
{
  fprintf (stderr, "e_entry      = 0x%.8lx\n", (long) ehdrp->e_entry);
  fprintf (stderr, "e_phoff      = %ld\n", (long) ehdrp->e_phoff);
  fprintf (stderr, "e_phnum      = %ld\n", (long) ehdrp->e_phnum);
  fprintf (stderr, "e_phentsize  = %ld\n", (long) ehdrp->e_phentsize);
  fprintf (stderr, "e_shoff      = %ld\n", (long) ehdrp->e_shoff);
  fprintf (stderr, "e_shnum      = %ld\n", (long) ehdrp->e_shnum);
  fprintf (stderr, "e_shentsize  = %ld\n", (long) ehdrp->e_shentsize);
}
#endif

static boolean
elf_slurp_reloc_table (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  Elf_External_Rel *native_relocs;
  arelent *reloc_cache;
  arelent *cache_ptr;
  Elf_Internal_Shdr *data_hdr;
  bfd_vma data_off;
  unsigned long data_max;
  char buf[4];			/* FIXME -- might be elf64 */

  unsigned int idx;

  if (asect->relocation)
    return true;
  if (asect->reloc_count == 0)
    return true;
  if (asect->flags & SEC_CONSTRUCTOR)
    return true;

  if (bfd_seek (abfd, asect->rel_filepos, SEEK_SET) != 0)
    return false;
  native_relocs = (Elf_External_Rel *)
    bfd_alloc (abfd, asect->reloc_count * sizeof (Elf_External_Rel));
  if (!native_relocs)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  if (bfd_read ((PTR) native_relocs,
		sizeof (Elf_External_Rel), asect->reloc_count, abfd)
      != sizeof (Elf_External_Rel) * asect->reloc_count)
    return false;

  reloc_cache = (arelent *)
    bfd_alloc (abfd, (size_t) (asect->reloc_count * sizeof (arelent)));

  if (!reloc_cache)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }

  /* Get the offset of the start of the segment we are relocating to read in
     the implicit addend.  */
  data_hdr = &elf_section_data (asect)->this_hdr;
  data_off = data_hdr->sh_offset;
  data_max = data_hdr->sh_size - sizeof (buf) + 1;

#if DEBUG & 2
  elf_debug_section ("data section", -1, data_hdr);
#endif

  for (idx = 0; idx < asect->reloc_count; idx++)
    {
#ifdef RELOC_PROCESSING
      Elf_Internal_Rel dst;
      Elf_External_Rel *src;

      cache_ptr = reloc_cache + idx;
      src = native_relocs + idx;
      elf_swap_reloc_in (abfd, src, &dst);

      RELOC_PROCESSING (cache_ptr, &dst, symbols, abfd, asect);
#else
      Elf_Internal_Rel dst;
      Elf_External_Rel *src;

      cache_ptr = reloc_cache + idx;
      src = native_relocs + idx;

      elf_swap_reloc_in (abfd, src, &dst);

      if (asect->flags & SEC_RELOC)
	{
	  /* relocatable, so the offset is off of the section */
	  cache_ptr->address = dst.r_offset + asect->vma;
	}
      else
	{
	  /* non-relocatable, so the offset a virtual address */
	  cache_ptr->address = dst.r_offset;
	}

      /* ELF_R_SYM(dst.r_info) is the symbol table offset.  An offset
	 of zero points to the dummy symbol, which was not read into
	 the symbol table SYMBOLS.  */
      if (ELF_R_SYM (dst.r_info) == 0)
	cache_ptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;
      else
	{
	  asymbol *s;

	  cache_ptr->sym_ptr_ptr = symbols + ELF_R_SYM (dst.r_info) - 1;

	  /* Translate any ELF section symbol into a BFD section
	     symbol.  */
	  s = *(cache_ptr->sym_ptr_ptr);
	  if (s->flags & BSF_SECTION_SYM)
	    {
	      cache_ptr->sym_ptr_ptr = s->section->symbol_ptr_ptr;
	      s = *cache_ptr->sym_ptr_ptr;
	      if (s->name == 0 || s->name[0] == 0)
		abort ();
	    }
	}
      BFD_ASSERT (dst.r_offset <= data_max);
      cache_ptr->addend = 0;

      /* Fill in the cache_ptr->howto field from dst.r_type */
      {
	struct elf_backend_data *ebd = get_elf_backend_data (abfd);
	(*ebd->elf_info_to_howto_rel) (abfd, cache_ptr, &dst);
      }
#endif
    }

  asect->relocation = reloc_cache;
  return true;
}

long
elf_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *tblptr = section->relocation;
  unsigned int count = 0;
  int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

  /* snarfed from coffcode.h */
  if (use_rela_p)
    {
      if (! elf_slurp_reloca_table (abfd, section, symbols))
	return -1;
    }
  else
    {
      if (! elf_slurp_reloc_table (abfd, section, symbols))
	return -1;
    }

  tblptr = section->relocation;

  for (; count++ < section->reloc_count;)
    *relptr++ = tblptr++;

  *relptr = 0;
  return section->reloc_count;
}

long
elf_get_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  long symcount = elf_slurp_symbol_table (abfd, alocation, false);

  if (symcount >= 0)
    bfd_get_symcount (abfd) = symcount;
  return symcount;
}

long
elf_canonicalize_dynamic_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  return elf_slurp_symbol_table (abfd, alocation, true);
}

asymbol *
elf_make_empty_symbol (abfd)
     bfd *abfd;
{
  elf_symbol_type *newsym;

  newsym = (elf_symbol_type *) bfd_zalloc (abfd, sizeof (elf_symbol_type));
  if (!newsym)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }
  else
    {
      newsym->symbol.the_bfd = abfd;
      return &newsym->symbol;
    }
}

void
elf_get_symbol_info (ignore_abfd, symbol, ret)
     bfd *ignore_abfd;
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);
}

void
elf_print_symbol (ignore_abfd, filep, symbol, how)
     bfd *ignore_abfd;
     PTR filep;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) filep;
  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    case bfd_print_symbol_more:
      fprintf (file, "elf ");
      fprintf_vma (file, symbol->value);
      fprintf (file, " %lx", (long) symbol->flags);
      break;
    case bfd_print_symbol_all:
      {
	CONST char *section_name;
	section_name = symbol->section ? symbol->section->name : "(*none*)";
	bfd_print_symbol_vandf ((PTR) file, symbol);
	fprintf (file, " %s\t%s",
		 section_name,
		 symbol->name);
      }
      break;
    }

}

alent *
elf_get_lineno (ignore_abfd, symbol)
     bfd *ignore_abfd;
     asymbol *symbol;
{
  fprintf (stderr, "elf_get_lineno unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
  return NULL;
}

boolean
elf_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  /* If this isn't the right architecture for this backend, and this
     isn't the generic backend, fail.  */
  if (arch != get_elf_backend_data (abfd)->arch
      && arch != bfd_arch_unknown
      && get_elf_backend_data (abfd)->arch != bfd_arch_unknown)
    return false;

  return bfd_default_set_arch_mach (abfd, arch, machine);
}

boolean
elf_find_nearest_line (abfd,
		       section,
		       symbols,
		       offset,
		       filename_ptr,
		       functionname_ptr,
		       line_ptr)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *line_ptr;
{
  return false;
}

int
elf_sizeof_headers (abfd, reloc)
     bfd *abfd;
     boolean reloc;
{
  fprintf (stderr, "elf_sizeof_headers unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
  return 0;
}

boolean
elf_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  Elf_Internal_Shdr *hdr;

  if (abfd->output_has_begun == false)	/* set by bfd.c handler? */
    {
      struct elf_backend_data *bed = get_elf_backend_data (abfd);

      /* Do any elf backend specific processing first.  */
      if (bed->elf_backend_begin_write_processing)
	(*bed->elf_backend_begin_write_processing) (abfd);

      /* do setup calculations (FIXME) */
      if (prep_headers (abfd) == false)
	return false;
      if (elf_compute_section_file_positions (abfd) == false)
	return false;
      abfd->output_has_begun = true;
    }

  hdr = &elf_section_data (section)->this_hdr;

  if (bfd_seek (abfd, hdr->sh_offset + offset, SEEK_SET) == -1)
    return false;
  if (bfd_write (location, 1, count, abfd) != count)
    return false;

  return true;
}

void
elf_no_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  fprintf (stderr, "elf RELA relocation support for target machine unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
}

void
elf_no_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf_Internal_Rel *dst;
{
  fprintf (stderr, "elf REL relocation support for target machine unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
}


/* Core file support */

#ifdef HAVE_PROCFS		/* Some core file support requires host /proc files */
#include <sys/procfs.h>
#else
#define bfd_prstatus(abfd, descdata, descsz, filepos) true
#define bfd_fpregset(abfd, descdata, descsz, filepos) true
#define bfd_prpsinfo(abfd, descdata, descsz, filepos) true
#endif

#ifdef HAVE_PROCFS

static boolean
bfd_prstatus (abfd, descdata, descsz, filepos)
     bfd *abfd;
     char *descdata;
     int descsz;
     long filepos;
{
  asection *newsect;
  prstatus_t *status = (prstatus_t *) 0;

  if (descsz == sizeof (prstatus_t))
    {
      newsect = bfd_make_section (abfd, ".reg");
      if (newsect == NULL)
	return false;
      newsect->_raw_size = sizeof (status->pr_reg);
      newsect->filepos = filepos + (long) &status->pr_reg;
      newsect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
      newsect->alignment_power = 2;
      if ((core_prstatus (abfd) = bfd_alloc (abfd, descsz)) != NULL)
	{
	  memcpy (core_prstatus (abfd), descdata, descsz);
	}
    }
  return true;
}

/* Stash a copy of the prpsinfo structure away for future use. */

static boolean
bfd_prpsinfo (abfd, descdata, descsz, filepos)
     bfd *abfd;
     char *descdata;
     int descsz;
     long filepos;
{
  asection *newsect;

  if (descsz == sizeof (prpsinfo_t))
    {
      if ((core_prpsinfo (abfd) = bfd_alloc (abfd, descsz)) == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      memcpy (core_prpsinfo (abfd), descdata, descsz);
    }
  return true;
}

static boolean
bfd_fpregset (abfd, descdata, descsz, filepos)
     bfd *abfd;
     char *descdata;
     int descsz;
     long filepos;
{
  asection *newsect;

  newsect = bfd_make_section (abfd, ".reg2");
  if (newsect == NULL)
    return false;
  newsect->_raw_size = descsz;
  newsect->filepos = filepos;
  newsect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
  newsect->alignment_power = 2;
  return true;
}

#endif /* HAVE_PROCFS */

/* Return a pointer to the args (including the command name) that were
   seen by the program that generated the core dump.  Note that for
   some reason, a spurious space is tacked onto the end of the args
   in some (at least one anyway) implementations, so strip it off if
   it exists. */

char *
elf_core_file_failing_command (abfd)
     bfd *abfd;
{
#ifdef HAVE_PROCFS
  if (core_prpsinfo (abfd))
    {
      prpsinfo_t *p = core_prpsinfo (abfd);
      char *scan = p->pr_psargs;
      while (*scan++)
	{;
	}
      scan -= 2;
      if ((scan > p->pr_psargs) && (*scan == ' '))
	{
	  *scan = '\000';
	}
      return p->pr_psargs;
    }
#endif
  return NULL;
}

/* Return the number of the signal that caused the core dump.  Presumably,
   since we have a core file, we got a signal of some kind, so don't bother
   checking the other process status fields, just return the signal number.
   */

int
elf_core_file_failing_signal (abfd)
     bfd *abfd;
{
#ifdef HAVE_PROCFS
  if (core_prstatus (abfd))
    {
      return ((prstatus_t *) (core_prstatus (abfd)))->pr_cursig;
    }
#endif
  return -1;
}

/* Check to see if the core file could reasonably be expected to have
   come for the current executable file.  Note that by default we return
   true unless we find something that indicates that there might be a
   problem.
   */

boolean
elf_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd;
     bfd *exec_bfd;
{
#ifdef HAVE_PROCFS
  char *corename;
  char *execname;
#endif

  /* First, xvecs must match since both are ELF files for the same target. */

  if (core_bfd->xvec != exec_bfd->xvec)
    {
      bfd_set_error (bfd_error_system_call);
      return false;
    }

#ifdef HAVE_PROCFS

  /* If no prpsinfo, just return true.  Otherwise, grab the last component
     of the exec'd pathname from the prpsinfo. */

  if (core_prpsinfo (core_bfd))
    {
      corename = (((struct prpsinfo *) core_prpsinfo (core_bfd))->pr_fname);
    }
  else
    {
      return true;
    }

  /* Find the last component of the executable pathname. */

  if ((execname = strrchr (exec_bfd->filename, '/')) != NULL)
    {
      execname++;
    }
  else
    {
      execname = (char *) exec_bfd->filename;
    }

  /* See if they match */

  return strcmp (execname, corename) ? false : true;

#else

  return true;

#endif /* HAVE_PROCFS */
}

/* ELF core files contain a segment of type PT_NOTE, that holds much of
   the information that would normally be available from the /proc interface
   for the process, at the time the process dumped core.  Currently this
   includes copies of the prstatus, prpsinfo, and fpregset structures.

   Since these structures are potentially machine dependent in size and
   ordering, bfd provides two levels of support for them.  The first level,
   available on all machines since it does not require that the host
   have /proc support or the relevant include files, is to create a bfd
   section for each of the prstatus, prpsinfo, and fpregset structures,
   without any interpretation of their contents.  With just this support,
   the bfd client will have to interpret the structures itself.  Even with
   /proc support, it might want these full structures for it's own reasons.

   In the second level of support, where HAVE_PROCFS is defined, bfd will
   pick apart the structures to gather some additional information that
   clients may want, such as the general register set, the name of the
   exec'ed file and its arguments, the signal (if any) that caused the
   core dump, etc.

   */

static boolean
elf_corefile_note (abfd, hdr)
     bfd *abfd;
     Elf_Internal_Phdr *hdr;
{
  Elf_External_Note *x_note_p;	/* Elf note, external form */
  Elf_Internal_Note i_note;	/* Elf note, internal form */
  char *buf = NULL;		/* Entire note segment contents */
  char *namedata;		/* Name portion of the note */
  char *descdata;		/* Descriptor portion of the note */
  char *sectname;		/* Name to use for new section */
  long filepos;			/* File offset to descriptor data */
  asection *newsect;

  if (hdr->p_filesz > 0
      && (buf = (char *) malloc (hdr->p_filesz)) != NULL
      && bfd_seek (abfd, hdr->p_offset, SEEK_SET) != -1
      && bfd_read ((PTR) buf, hdr->p_filesz, 1, abfd) == hdr->p_filesz)
    {
      x_note_p = (Elf_External_Note *) buf;
      while ((char *) x_note_p < (buf + hdr->p_filesz))
	{
	  i_note.namesz = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p->namesz);
	  i_note.descsz = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p->descsz);
	  i_note.type = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p->type);
	  namedata = x_note_p->name;
	  descdata = namedata + BFD_ALIGN (i_note.namesz, 4);
	  filepos = hdr->p_offset + (descdata - buf);
	  switch (i_note.type)
	    {
	    case NT_PRSTATUS:
	      /* process descdata as prstatus info */
	      if (! bfd_prstatus (abfd, descdata, i_note.descsz, filepos))
		return false;
	      sectname = ".prstatus";
	      break;
	    case NT_FPREGSET:
	      /* process descdata as fpregset info */
	      if (! bfd_fpregset (abfd, descdata, i_note.descsz, filepos))
		return false;
	      sectname = ".fpregset";
	      break;
	    case NT_PRPSINFO:
	      /* process descdata as prpsinfo */
	      if (! bfd_prpsinfo (abfd, descdata, i_note.descsz, filepos))
		return false;
	      sectname = ".prpsinfo";
	      break;
	    default:
	      /* Unknown descriptor, just ignore it. */
	      sectname = NULL;
	      break;
	    }
	  if (sectname != NULL)
	    {
	      newsect = bfd_make_section (abfd, sectname);
	      if (newsect == NULL)
		return false;
	      newsect->_raw_size = i_note.descsz;
	      newsect->filepos = filepos;
	      newsect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
	      newsect->alignment_power = 2;
	    }
	  x_note_p = (Elf_External_Note *)
	    (descdata + BFD_ALIGN (i_note.descsz, 4));
	}
    }
  if (buf != NULL)
    {
      free (buf);
    }
  else if (hdr->p_filesz > 0)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  return true;

}

/*  Core files are simply standard ELF formatted files that partition
    the file using the execution view of the file (program header table)
    rather than the linking view.  In fact, there is no section header
    table in a core file.

    The process status information (including the contents of the general
    register set) and the floating point register set are stored in a
    segment of type PT_NOTE.  We handcraft a couple of extra bfd sections
    that allow standard bfd access to the general registers (.reg) and the
    floating point registers (.reg2).

 */

bfd_target *
elf_core_file_p (abfd)
     bfd *abfd;
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_External_Phdr x_phdr;	/* Program header table entry, external form */
  Elf_Internal_Phdr *i_phdrp;	/* Program header table, internal form */
  unsigned int phindex;
  struct elf_backend_data *ebd;

  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      if (bfd_get_error () != bfd_error_system_call)
	bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry, and it must have a
     program header table (FIXME: See comments re segments at top of this
     file). */

  if (elf_file_p (&x_ehdr) == false)
    {
    wrong:
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  /* FIXME, Check EI_VERSION here !  */

  {
#if ARCH_SIZE == 32
    int desired_address_size = ELFCLASS32;
#endif
#if ARCH_SIZE == 64
    int desired_address_size = ELFCLASS64;
#endif

    if (x_ehdr.e_ident[EI_CLASS] != desired_address_size)
      goto wrong;
  }

  /* Switch xvec to match the specified byte order.  */
  switch (x_ehdr.e_ident[EI_DATA])
    {
    case ELFDATA2MSB:		/* Big-endian */
      if (abfd->xvec->byteorder_big_p == false)
	goto wrong;
      break;
    case ELFDATA2LSB:		/* Little-endian */
      if (abfd->xvec->byteorder_big_p == true)
	goto wrong;
      break;
    case ELFDATANONE:		/* No data encoding specified */
    default:			/* Unknown data encoding specified */
      goto wrong;
    }

  /* Allocate an instance of the elf_obj_tdata structure and hook it up to
     the tdata pointer in the bfd. */

  elf_tdata (abfd) =
    (struct elf_obj_tdata *) bfd_zalloc (abfd, sizeof (struct elf_obj_tdata));
  if (elf_tdata (abfd) == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }

  /* FIXME, `wrong' returns from this point onward, leak memory.  */

  /* Now that we know the byte order, swap in the rest of the header */
  i_ehdrp = elf_elfheader (abfd);
  elf_swap_ehdr_in (abfd, &x_ehdr, i_ehdrp);
#if DEBUG & 1
  elf_debug_file (i_ehdrp);
#endif

  ebd = get_elf_backend_data (abfd);

  /* Check that the ELF e_machine field matches what this particular
     BFD format expects.  */
  if (ebd->elf_machine_code != i_ehdrp->e_machine)
    {
      bfd_target **target_ptr;

      if (ebd->elf_machine_code != EM_NONE)
	goto wrong;

      /* This is the generic ELF target.  Let it match any ELF target
	 for which we do not have a specific backend.  */
      for (target_ptr = bfd_target_vector; *target_ptr != NULL; target_ptr++)
	{
	  struct elf_backend_data *back;

	  if ((*target_ptr)->flavour != bfd_target_elf_flavour)
	    continue;
	  back = (struct elf_backend_data *) (*target_ptr)->backend_data;
	  if (back->elf_machine_code == i_ehdrp->e_machine)
	    {
	      /* target_ptr is an ELF backend which matches this
		 object file, so reject the generic ELF target.  */
	      goto wrong;
	    }
	}
    }

  /* If there is no program header, or the type is not a core file, then
     we are hosed. */
  if (i_ehdrp->e_phoff == 0 || i_ehdrp->e_type != ET_CORE)
    goto wrong;

  /* Allocate space for a copy of the program header table in
     internal form, seek to the program header table in the file,
     read it in, and convert it to internal form.  As a simple sanity
     check, verify that the what BFD thinks is the size of each program
     header table entry actually matches the size recorded in the file. */

  if (i_ehdrp->e_phentsize != sizeof (x_phdr))
    goto wrong;
  i_phdrp = (Elf_Internal_Phdr *)
    bfd_alloc (abfd, sizeof (*i_phdrp) * i_ehdrp->e_phnum);
  if (!i_phdrp)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }
  if (bfd_seek (abfd, i_ehdrp->e_phoff, SEEK_SET) == -1)
    return NULL;
  for (phindex = 0; phindex < i_ehdrp->e_phnum; phindex++)
    {
      if (bfd_read ((PTR) & x_phdr, sizeof (x_phdr), 1, abfd)
	  != sizeof (x_phdr))
	return NULL;
      elf_swap_phdr_in (abfd, &x_phdr, i_phdrp + phindex);
    }

  /* Once all of the program headers have been read and converted, we
     can start processing them. */

  for (phindex = 0; phindex < i_ehdrp->e_phnum; phindex++)
    {
      bfd_section_from_phdr (abfd, i_phdrp + phindex, phindex);
      if ((i_phdrp + phindex)->p_type == PT_NOTE)
	{
	  if (! elf_corefile_note (abfd, i_phdrp + phindex))
	    return NULL;
	}
    }

  /* Remember the entry point specified in the ELF file header. */

  bfd_get_start_address (abfd) = i_ehdrp->e_entry;

  return abfd->xvec;
}
