/* ELF executable support for BFD.
   Copyright 1991, 1992, 1993 Free Software Foundation, Inc.

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

*/

#include <string.h>		/* For strrchr and friends */
#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libelf.h"

#define Elf_External_Ehdr	NAME(Elf,External_Ehdr)
#define Elf_External_Sym	NAME(Elf,External_Sym)
#define Elf_External_Shdr	NAME(Elf,External_Shdr)
#define Elf_External_Phdr	NAME(Elf,External_Phdr)
#define Elf_External_Rel	NAME(Elf,External_Rel)
#define Elf_External_Rela	NAME(Elf,External_Rela)

#define elf_symbol_type		NAME(elf,symbol_type)

#define elf_core_file_failing_command	NAME(bfd_elf,core_file_failing_command)
#define elf_core_file_failing_signal	NAME(bfd_elf,core_file_failing_signal)
#define elf_core_file_matches_executable_p NAME(bfd_elf,core_file_matches_executable_p)
#define elf_object_p			NAME(bfd_elf,object_p)
#define elf_core_file_p			NAME(bfd_elf,core_file_p)
#define elf_mkobject			NAME(bfd_elf,mkobject)
#define elf_write_object_contents	NAME(bfd_elf,write_object_contents)
#define elf_get_symtab_upper_bound	NAME(bfd_elf,get_symtab_upper_bound)
#define elf_get_reloc_upper_bound	NAME(bfd_elf,get_reloc_upper_bound)
#define elf_canonicalize_reloc		NAME(bfd_elf,canonicalize_reloc)
#define elf_get_symtab			NAME(bfd_elf,get_symtab)
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
#define elf_get_sect_thunk		NAME(bfd_elf,get_sect_thunk)
#define elf_hash			NAME(bfd_elf,hash)
#define elf_new_section_hook		NAME(bfd_elf,new_section_hook)


#ifdef HAVE_PROCFS		/* Some core file support requires host /proc files */
#include <sys/procfs.h>
#else
#define bfd_prstatus(abfd, descdata, descsz, filepos)	/* Define away */
#define bfd_fpregset(abfd, descdata, descsz, filepos)	/* Define away */
#define bfd_prpsinfo(abfd, descdata, descsz, filepos)	/* Define away */
#endif

#ifndef INLINE
#if __GNUC__ >= 2
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

/* Forward declarations of static functions */

static char * elf_read PARAMS ((bfd *, long, int));

static struct sec * section_from_elf_index PARAMS ((bfd *, int));

static int elf_section_from_bfd_section PARAMS ((bfd *, struct sec *));

static boolean elf_slurp_symbol_table PARAMS ((bfd *, asymbol **));

static char * elf_get_str_section PARAMS ((bfd *, unsigned int));

static int elf_symbol_from_bfd_symbol PARAMS ((bfd *,
					       struct symbol_cache_entry **));

static void elf_map_symbols PARAMS ((bfd *));

/* Some private data is stashed away for future use using the tdata pointer
   in the bfd structure.  */

struct elf_obj_tdata
{
  Elf_Internal_Ehdr elf_header[1];	/* Actual data, but ref like ptr */
  Elf_Internal_Shdr *elf_sect_ptr;
  struct strtab *strtab_ptr;
  int symtab_section;
  int num_locals;
  int num_globals;
  int *symtab_map;
  void *prstatus;		/* The raw /proc prstatus structure */
  void *prpsinfo;		/* The raw /proc prpsinfo structure */
  Elf_External_Sym *raw_syms;
  Elf_Internal_Sym *internal_syms;
  elf_symbol_type *symbols;
};

#define elf_tdata(bfd)		((bfd) -> tdata.elf_obj_data)
#define elf_elfheader(bfd)	(elf_tdata(bfd) -> elf_header)
#define elf_elfsections(bfd)	(elf_tdata(bfd) -> elf_sect_ptr)
#define elf_shstrtab(bfd)	(elf_tdata(bfd) -> strtab_ptr)
#define elf_onesymtab(bfd)	(elf_tdata(bfd) -> symtab_section)
#define elf_num_locals(bfd)	(elf_tdata(bfd) -> num_locals)
#define elf_num_globals(bfd)	(elf_tdata(bfd) -> num_globals)
#define elf_symtab_map(bfd)	(elf_tdata(bfd) -> symtab_map)
#define core_prpsinfo(bfd)	(elf_tdata(bfd) -> prpsinfo)
#define core_prstatus(bfd)	(elf_tdata(bfd) -> prstatus)
#define obj_symbols(bfd)	(elf_tdata(bfd) -> symbols)
#define obj_raw_syms(bfd)	(elf_tdata(bfd) -> raw_syms)
#define obj_internal_syms(bfd)	(elf_tdata(bfd) -> internal_syms)


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
DEFUN (elf_swap_symbol_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Sym * src AND
       Elf_Internal_Sym * dst)
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
DEFUN (elf_swap_symbol_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Sym * src AND
       Elf_External_Sym * dst)
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
DEFUN (elf_swap_ehdr_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Ehdr * src AND
       Elf_Internal_Ehdr * dst)
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
DEFUN (elf_swap_ehdr_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Ehdr * src AND
       Elf_External_Ehdr * dst)
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
DEFUN (elf_swap_shdr_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Shdr * src AND
       Elf_Internal_Shdr * dst)
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
DEFUN (elf_swap_shdr_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Shdr * src AND
       Elf_External_Shdr * dst)
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
DEFUN (elf_swap_phdr_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Phdr * src AND
       Elf_Internal_Phdr * dst)
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

/* ... */

static void
DEFUN (elf_swap_phdr_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Phdr * src AND
       Elf_External_Phdr * dst)
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
static void
DEFUN (elf_swap_reloc_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Rel * src AND
       Elf_Internal_Rel * dst)
{
  dst->r_offset = get_word (abfd, (bfd_byte *) src->r_offset);
  dst->r_info = get_word (abfd, (bfd_byte *) src->r_info);
}

static void
DEFUN (elf_swap_reloca_in, (abfd, src, dst),
       bfd * abfd AND
       Elf_External_Rela * src AND
       Elf_Internal_Rela * dst)
{
  dst->r_offset = get_word (abfd, (bfd_byte *) src->r_offset);
  dst->r_info = get_word (abfd, (bfd_byte *) src->r_info);
  dst->r_addend = get_word (abfd, (bfd_byte *) src->r_addend);
}

/* Translate an ELF reloc from internal format to external format. */
static void
DEFUN (elf_swap_reloc_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Rel * src AND
       Elf_External_Rel * dst)
{
  put_word (abfd, src->r_offset, dst->r_offset);
  put_word (abfd, src->r_info, dst->r_info);
}

static void
DEFUN (elf_swap_reloca_out, (abfd, src, dst),
       bfd * abfd AND
       Elf_Internal_Rela * src AND
       Elf_External_Rela * dst)
{
  put_word (abfd, src->r_offset, dst->r_offset);
  put_word (abfd, src->r_info, dst->r_info);
  put_word (abfd, src->r_addend, dst->r_addend);
}

/*
INTERNAL_FUNCTION
	bfd_elf_find_section

SYNOPSIS
	struct elf_internal_shdr *bfd_elf32_find_section (bfd *abfd, char *name);

DESCRIPTION
	Helper functions for GDB to locate the string tables.
	Since BFD hides string tables from callers, GDB needs to use an
	internal hook to find them.  Sun's .stabstr, in particular,
	isn't even pointed to by the .stab section, so ordinary
	mechanisms wouldn't work to find it, even if we had some.
*/

struct elf_internal_shdr *
DEFUN (NAME (bfd_elf,find_section), (abfd, name),
       bfd * abfd AND
       char *name)
{
  Elf_Internal_Shdr *i_shdrp;
  Elf_Internal_Shdr *gotit = NULL;
  char *shstrtab;
  unsigned int max;
  unsigned int i;

  i_shdrp = elf_elfsections (abfd);
  if (i_shdrp != NULL)
    {
      shstrtab = elf_get_str_section (abfd, elf_elfheader (abfd)->e_shstrndx);
      if (shstrtab != NULL)
	{
	  max = elf_elfheader (abfd)->e_shnum;
	  for (i = 1; i < max; i++)
	    {
	      if (!strcmp (&shstrtab[i_shdrp[i].sh_name], name))
		{
		  gotit = &i_shdrp[i];
		}
	    }
	}
    }
  return gotit;
}

/* End of GDB support.  */

static char *
DEFUN (elf_get_str_section, (abfd, shindex),
       bfd * abfd AND
       unsigned int shindex)
{
  Elf_Internal_Shdr *i_shdrp;
  char *shstrtab = NULL;
  unsigned int offset;
  unsigned int shstrtabsize;

  i_shdrp = elf_elfsections (abfd);
  if (i_shdrp != NULL)
    {
      shstrtab = i_shdrp[shindex].rawdata;
      if (shstrtab == NULL)
	{
	  /* No cached one, attempt to read, and cache what we read. */
	  offset = i_shdrp[shindex].sh_offset;
	  shstrtabsize = i_shdrp[shindex].sh_size;
	  shstrtab = elf_read (abfd, offset, shstrtabsize);
	  i_shdrp[shindex].rawdata = (void *) shstrtab;
	}
    }
  return shstrtab;
}

static char *
DEFUN (elf_string_from_elf_section, (abfd, shindex, strindex),
       bfd * abfd AND
       unsigned int shindex AND
       unsigned int strindex)
{
  Elf_Internal_Shdr *hdr;

  if (strindex == 0)
    return "";

  hdr = elf_elfsections (abfd) + shindex;

  if (!hdr->rawdata
      && elf_get_str_section (abfd, shindex) == NULL)
    return NULL;

  return ((char *) hdr->rawdata) + strindex;
}

static INLINE char *
elf_string_from_elf_strtab (abfd, strindex)
     bfd *abfd;
     int strindex;
{
  return elf_string_from_elf_section (abfd, elf_elfheader (abfd)->e_shstrndx,
				      strindex);
}

/* Create a new bfd section from an ELF section header. */

static boolean
DEFUN (bfd_section_from_shdr, (abfd, shindex),
       bfd * abfd AND
       unsigned int shindex)
{
  Elf_Internal_Shdr *i_shdrp = elf_elfsections (abfd);
  Elf_Internal_Shdr *hdr = i_shdrp + shindex;
  asection *newsect;
  char *name;

  name = elf_string_from_elf_strtab (abfd, hdr->sh_name);

  switch (hdr->sh_type)
    {

    case SHT_NULL:
      /* inactive section. Throw it away. */
      return true;

    case SHT_PROGBITS:
      /* Bits that get saved. This one is real. */
      if (!hdr->rawdata)
	{
	  newsect = bfd_make_section (abfd, name);
	  if (newsect != NULL)
	    {
	      newsect->vma = hdr->sh_addr;
	      newsect->_raw_size = hdr->sh_size;
	      newsect->filepos = hdr->sh_offset;	/* so we can read back the bits */
	      newsect->flags |= SEC_HAS_CONTENTS;
	      newsect->alignment_power = hdr->sh_addralign;

	      if (hdr->sh_flags & SHF_ALLOC)
		{
		  newsect->flags |= SEC_ALLOC;
		  newsect->flags |= SEC_LOAD;
		}

	      if (!(hdr->sh_flags & SHF_WRITE))
		newsect->flags |= SEC_READONLY;

	      if (hdr->sh_flags & SHF_EXECINSTR)
		newsect->flags |= SEC_CODE;	/* FIXME: may only contain SOME code */
	      else
		newsect->flags |= SEC_DATA;

	      hdr->rawdata = (void *) newsect;
	    }
	  else
	    hdr->rawdata = (void *) bfd_get_section_by_name (abfd, name);
	}
      return true;

    case SHT_NOBITS:
      /* Bits that get saved. This one is real. */
      if (!hdr->rawdata)
	{
	  newsect = bfd_make_section (abfd, name);
	  if (newsect != NULL)
	    {
	      newsect->vma = hdr->sh_addr;
	      newsect->_raw_size = hdr->sh_size;
	      newsect->filepos = hdr->sh_offset;	/* fake */
	      newsect->alignment_power = hdr->sh_addralign;
	      if (hdr->sh_flags & SHF_ALLOC)
		newsect->flags |= SEC_ALLOC;

	      if (!(hdr->sh_flags & SHF_WRITE))
		newsect->flags |= SEC_READONLY;

	      if (hdr->sh_flags & SHF_EXECINSTR)
		newsect->flags |= SEC_CODE;	/* FIXME: may only contain SOME code */
	      else
		newsect->flags |= SEC_DATA;

	      hdr->rawdata = (void *) newsect;
	    }
	}
      return true;

    case SHT_SYMTAB:		/* A symbol table */
      BFD_ASSERT (hdr->sh_entsize == sizeof (Elf_External_Sym));
      elf_onesymtab (abfd) = shindex;
      abfd->flags |= HAS_SYMS;
      return true;

    case SHT_STRTAB:		/* A string table */
      if (!strcmp (name, ".strtab") || !strcmp (name, ".shstrtab"))
	return true;

      if (!hdr->rawdata)
	{
	  newsect = bfd_make_section (abfd, name);
	  if (newsect)
	    newsect->flags = SEC_HAS_CONTENTS;
	}

      return true;

    case SHT_REL:
    case SHT_RELA:
      /* *these* do a lot of work -- but build no sections! */
      /* the spec says there can be multiple strtabs, but only one symtab */
      /* but there can be lots of REL* sections. */
      /* FIXME:  The above statement is wrong!  There are typically at least
       two symbol tables in a dynamically linked executable, ".dynsym"
       which is the dynamic linkage symbol table and ".symtab", which is
       the "traditional" symbol table.  -fnf */

      {
	asection *target_sect;
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
		     ? sizeof (Elf32_External_Rela)
		     : sizeof (Elf32_External_Rel)));

	bfd_section_from_shdr (abfd, hdr->sh_link);	/* symbol table */
	bfd_section_from_shdr (abfd, hdr->sh_info);	/* target */
	target_sect = section_from_elf_index (abfd, hdr->sh_info);
	if (target_sect == NULL)
	  return false;

#if 0
	/* FIXME:  We are only prepared to read one symbol table, so
	 do NOT read the dynamic symbol table since it is only a
	 subset of the full symbol table.  Also see comment above. -fnf */
	if (!elf_slurp_symbol_table (abfd, i_shdrp + hdr->sh_link))
	  return false;
#endif

	target_sect->reloc_count = hdr->sh_size / hdr->sh_entsize;
	target_sect->flags |= SEC_RELOC;
	target_sect->relocation = 0;
	target_sect->rel_filepos = hdr->sh_offset;
	return true;
      }
      break;

    case SHT_HASH:
    case SHT_DYNAMIC:
    case SHT_DYNSYM:		/* could treat this like symtab... */
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
      break;
    }

  return true;
}

boolean
DEFUN (elf_new_section_hook, (abfd, sec),
       bfd *abfd
       AND asection *sec)
{
  sec->symbol->name = "";
  return true;
}

static struct strtab *
DEFUN (bfd_new_strtab, (abfd),
       bfd * abfd)
{
  struct strtab *ss;

  ss = (struct strtab *) bfd_xmalloc (sizeof (struct strtab));
  ss->tab = bfd_xmalloc (1);
  BFD_ASSERT (ss->tab != 0);
  *ss->tab = 0;
  ss->nentries = 0;
  ss->length = 1;

  return ss;
}

static int
DEFUN (bfd_add_to_strtab, (abfd, ss, str),
       bfd * abfd AND
       struct strtab *ss AND
       CONST char *str)
{
  /* should search first, but for now: */
  /* include the trailing NUL */
  int ln = strlen (str) + 1;

  /* should this be using obstacks? */
  ss->tab = realloc (ss->tab, ss->length + ln);

  BFD_ASSERT (ss->tab != 0);
  strcpy (ss->tab + ss->length, str);
  ss->nentries++;
  ss->length += ln;

  return ss->length - ln;
}

static int
DEFUN (bfd_add_2_to_strtab, (abfd, ss, str, str2),
       bfd * abfd AND
       struct strtab *ss AND
       char *str AND
       CONST char *str2)
{
  /* should search first, but for now: */
  /* include the trailing NUL */
  int ln = strlen (str) + strlen (str2) + 1;

  /* should this be using obstacks? */
  if (ss->length)
    ss->tab = realloc (ss->tab, ss->length + ln);
  else
    ss->tab = bfd_xmalloc (ln);

  BFD_ASSERT (ss->tab != 0);
  strcpy (ss->tab + ss->length, str);
  strcpy (ss->tab + ss->length + strlen (str), str2);
  ss->nentries++;
  ss->length += ln;

  return ss->length - ln;
}

/* Create a new ELF section from a bfd section. */

#if 0 /* not used */
static boolean
DEFUN (bfd_shdr_from_section, (abfd, hdr, shstrtab, indx),
       bfd * abfd AND
       Elf_Internal_Shdr * hdr AND
       struct strtab *shstrtab AND
       int indx)
{
  asection *sect;
  int ndx;

  sect = abfd->sections;
  for (ndx = indx; --ndx;)
    {
      sect = sect->next;
    }
  hdr[indx].sh_name = bfd_add_to_strtab (abfd, shstrtab,
					 bfd_section_name (abfd, sect));
  hdr[indx].sh_addr = sect->vma;
  hdr[indx].sh_size = sect->_raw_size;
  hdr[indx].sh_addralign = sect->alignment_power;
  hdr[indx].sh_flags = 0;
  /* these need to be preserved on */
  hdr[indx].sh_link = 0;
  hdr[indx].sh_info = 0;
  hdr[indx].sh_entsize = 0;

  hdr[indx].sh_type = 0;
  if (sect->flags & SEC_RELOC)
    {
      int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;
      hdr[indx].sh_type = use_rela_p ? SHT_RELA : SHT_REL;
    }

  if (sect->flags & SEC_HAS_CONTENTS)
    {
      hdr[indx].sh_offset = sect->filepos;
      hdr[indx].sh_size = sect->_raw_size;
    }
  if (sect->flags & SEC_ALLOC)
    {
      hdr[indx].sh_flags |= SHF_ALLOC;
      if (sect->flags & SEC_LOAD)
	{
	  /* do something with sh_type ? */
	}
    }
  if (!(sect->flags & SEC_READONLY))
    hdr[indx].sh_flags |= SHF_WRITE;

  if (sect->flags & SEC_CODE)
    hdr[indx].sh_flags |= SHF_EXECINSTR;

  return true;
}
#endif

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
DEFUN (bfd_section_from_phdr, (abfd, hdr, index),
       bfd * abfd AND
       Elf_Internal_Phdr * hdr AND
       int index)
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
  strcpy (name, namebuf);
  newsect = bfd_make_section (abfd, name);
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
      strcpy (name, namebuf);
      newsect = bfd_make_section (abfd, name);
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

#ifdef HAVE_PROCFS

static void
DEFUN (bfd_prstatus, (abfd, descdata, descsz, filepos),
       bfd * abfd AND
       char *descdata AND
       int descsz AND
       long filepos)
{
  asection *newsect;
  prstatus_t *status = (prstatus_t *) 0;

  if (descsz == sizeof (prstatus_t))
    {
      newsect = bfd_make_section (abfd, ".reg");
      newsect->_raw_size = sizeof (status->pr_reg);
      newsect->filepos = filepos + (long) &status->pr_reg;
      newsect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
      newsect->alignment_power = 2;
      if ((core_prstatus (abfd) = bfd_alloc (abfd, descsz)) != NULL)
	{
	  memcpy (core_prstatus (abfd), descdata, descsz);
	}
    }
}

/* Stash a copy of the prpsinfo structure away for future use. */

static void
DEFUN (bfd_prpsinfo, (abfd, descdata, descsz, filepos),
       bfd * abfd AND
       char *descdata AND
       int descsz AND
       long filepos)
{
  asection *newsect;

  if (descsz == sizeof (prpsinfo_t))
    {
      if ((core_prpsinfo (abfd) = bfd_alloc (abfd, descsz)) != NULL)
	{
	  memcpy (core_prpsinfo (abfd), descdata, descsz);
	}
    }
}

static void
DEFUN (bfd_fpregset, (abfd, descdata, descsz, filepos),
       bfd * abfd AND
       char *descdata AND
       int descsz AND
       long filepos)
{
  asection *newsect;

  newsect = bfd_make_section (abfd, ".reg2");
  newsect->_raw_size = descsz;
  newsect->filepos = filepos;
  newsect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
  newsect->alignment_power = 2;
}

#endif /* HAVE_PROCFS */

/* Return a pointer to the args (including the command name) that were
   seen by the program that generated the core dump.  Note that for
   some reason, a spurious space is tacked onto the end of the args
   in some (at least one anyway) implementations, so strip it off if
   it exists. */

char *
DEFUN (elf_core_file_failing_command, (abfd),
       bfd * abfd)
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
DEFUN (elf_core_file_failing_signal, (abfd),
       bfd * abfd)
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
DEFUN (elf_core_file_matches_executable_p, (core_bfd, exec_bfd),
       bfd * core_bfd AND
       bfd * exec_bfd)
{
#ifdef HAVE_PROCFS
  char *corename;
  char *execname;
#endif

  /* First, xvecs must match since both are ELF files for the same target. */

  if (core_bfd->xvec != exec_bfd->xvec)
    {
      bfd_error = system_call_error;
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
DEFUN (elf_corefile_note, (abfd, hdr),
       bfd * abfd AND
       Elf_Internal_Phdr * hdr)
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
      && (buf = (char *) bfd_xmalloc (hdr->p_filesz)) != NULL
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
	      bfd_prstatus (abfd, descdata, i_note.descsz, filepos);
	      sectname = ".prstatus";
	      break;
	    case NT_FPREGSET:
	      /* process descdata as fpregset info */
	      bfd_fpregset (abfd, descdata, i_note.descsz, filepos);
	      sectname = ".fpregset";
	      break;
	    case NT_PRPSINFO:
	      /* process descdata as prpsinfo */
	      bfd_prpsinfo (abfd, descdata, i_note.descsz, filepos);
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
  return true;

}


/* Read a specified number of bytes at a specified offset in an ELF
   file, into a newly allocated buffer, and return a pointer to the
   buffer. */

static char *
DEFUN (elf_read, (abfd, offset, size),
       bfd * abfd AND
       long offset AND
       int size)
{
  char *buf;

  if ((buf = bfd_alloc (abfd, size)) == NULL)
    {
      bfd_error = no_memory;
      return NULL;
    }
  if (bfd_seek (abfd, offset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return NULL;
    }
  if (bfd_read ((PTR) buf, size, 1, abfd) != size)
    {
      bfd_error = system_call_error;
      return NULL;
    }
  return buf;
}

/* Begin processing a given object.

   First we validate the file by reading in the ELF header and checking
   the magic number.  */

static INLINE boolean
DEFUN (elf_file_p, (x_ehdrp), Elf_External_Ehdr * x_ehdrp)
{
  return ((x_ehdrp->e_ident[EI_MAG0] == ELFMAG0)
	  && (x_ehdrp->e_ident[EI_MAG1] == ELFMAG1)
	  && (x_ehdrp->e_ident[EI_MAG2] == ELFMAG2)
	  && (x_ehdrp->e_ident[EI_MAG3] == ELFMAG3));
}

bfd_target *
DEFUN (elf_object_p, (abfd), bfd * abfd)
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_External_Shdr x_shdr;	/* Section header table entry, external form */
  Elf_Internal_Shdr *i_shdrp;	/* Section header table, internal form */
  int shindex;
  char *shstrtab;		/* Internal copy of section header stringtab */
  struct elf_backend_data *ebd;	/* Use to get ELF_ARCH stored in xvec */

  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      bfd_error = system_call_error;
      return NULL;
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry, and it must have a
     section header table (FIXME: See comments re sections at top of this
     file). */

  if (elf_file_p (&x_ehdr) == false)
    {
    wrong:
      bfd_error = wrong_format;
      return NULL;
    }

  if (x_ehdr.e_ident[EI_VERSION] != EV_CURRENT)
    goto wrong;

  {
    unsigned int class = 0x12345;
#if ARCH_SIZE == 64
    class = ELFCLASS64;
#endif
#if ARCH_SIZE == 32
    class = ELFCLASS32;
#endif
    if (x_ehdr.e_ident[EI_CLASS] != class)
      goto wrong;
  }

  /* Switch xvec to match the specified byte order.  */
  switch (x_ehdr.e_ident[EI_DATA])
    {
    case ELFDATA2MSB:		/* Big-endian */
      if (!abfd->xvec->header_byteorder_big_p)
	goto wrong;
      break;
    case ELFDATA2LSB:		/* Little-endian */
      if (abfd->xvec->header_byteorder_big_p)
	goto wrong;
      break;
    case ELFDATANONE:		/* No data encoding specified */
    default:			/* Unknown data encoding specified */
      goto wrong;
    }

  /* Allocate an instance of the elf_obj_tdata structure and hook it up to
     the tdata pointer in the bfd. */

  if (NULL == (elf_tdata (abfd) = (struct elf_obj_tdata *)
	       bfd_zalloc (abfd, sizeof (struct elf_obj_tdata))))
    {
      bfd_error = no_memory;
      return NULL;
    }

  /* FIXME:  Any `wrong' exits below here will leak memory (tdata).  */

  /* Now that we know the byte order, swap in the rest of the header */
  i_ehdrp = elf_elfheader (abfd);
  elf_swap_ehdr_in (abfd, &x_ehdr, i_ehdrp);

  /* If there is no section header table, we're hosed. */
  if (i_ehdrp->e_shoff == 0)
    goto wrong;

  if (i_ehdrp->e_type == ET_EXEC || i_ehdrp->e_type == ET_DYN)
    abfd->flags |= EXEC_P;

  /* Retrieve the architecture information from the xvec and verify
     that it matches the machine info stored in the ELF header.
     This allows us to resolve ambiguous formats that might not
     otherwise be distinguishable. */

  ebd = get_elf_backend_data (abfd);
  switch (i_ehdrp->e_machine)
    {
    default:
    case EM_NONE:
    case EM_M32:
      /* Arguably EM_M32 should be bfd_arch_obscure, but then we would
	 need both an elf32-obscure target and an elf32-unknown target.
	 The distinction is probably not worth worrying about.  */
      if (ebd->arch != bfd_arch_unknown)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_unknown, 0);
      break;
    case EM_SPARC:
      if (ebd->arch != bfd_arch_sparc)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_sparc, 0);
      break;
    case EM_386:
      if (ebd->arch != bfd_arch_i386)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_i386, 0);
      break;
    case EM_68K:
      if (ebd->arch != bfd_arch_m68k)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_m68k, 0);
      break;
    case EM_88K:
      if (ebd->arch != bfd_arch_m88k)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_m88k, 0);
      break;
    case EM_860:
      if (ebd->arch != bfd_arch_i860)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_i860, 0);
      break;
    case EM_MIPS:
      if (ebd->arch != bfd_arch_mips)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_mips, 0);
      break;
    case EM_HPPA:
      if (ebd->arch != bfd_arch_hppa)
	goto wrong;
      bfd_default_set_arch_mach (abfd, bfd_arch_hppa, 0);
      break;
    }

  /* Allocate space for a copy of the section header table in
     internal form, seek to the section header table in the file,
     read it in, and convert it to internal form.  As a simple sanity
     check, verify that the what BFD thinks is the size of each section
     header table entry actually matches the size recorded in the file. */

  if (i_ehdrp->e_shentsize != sizeof (x_shdr))
    goto wrong;
  i_shdrp = (Elf_Internal_Shdr *)
    bfd_alloc (abfd, sizeof (*i_shdrp) * i_ehdrp->e_shnum);
  if (!i_shdrp)
    {
      bfd_error = no_memory;
      return NULL;
    }
  if (bfd_seek (abfd, i_ehdrp->e_shoff, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return NULL;
    }
  for (shindex = 0; shindex < i_ehdrp->e_shnum; shindex++)
    {
      if (bfd_read ((PTR) & x_shdr, sizeof x_shdr, 1, abfd)
	  != sizeof (x_shdr))
	{
	  bfd_error = system_call_error;
	  return NULL;
	}
      elf_swap_shdr_in (abfd, &x_shdr, i_shdrp + shindex);
    }

  elf_elfsections (abfd) = i_shdrp;

  /* Read in the string table containing the names of the sections.  We
     will need the base pointer to this table later. */
  /* We read this inline now, so that we don't have to go through
     bfd_section_from_shdr with it (since this particular strtab is
     used to find all of the ELF section names.) */

  shstrtab = elf_get_str_section (abfd, i_ehdrp->e_shstrndx);
  if (!shstrtab)
    return NULL;

  /* Once all of the section headers have been read and converted, we
     can start processing them.  Note that the first section header is
     a dummy placeholder entry, so we ignore it.

     We also watch for the symbol table section and remember the file
     offset and section size for both the symbol table section and the
     associated string table section. */

  for (shindex = 1; shindex < i_ehdrp->e_shnum; shindex++)
    {
      bfd_section_from_shdr (abfd, shindex);
    }

  /* Remember the entry point specified in the ELF file header. */

  bfd_get_start_address (abfd) = i_ehdrp->e_entry;

  return abfd->xvec;
}

/*
  Takes a bfd and a symbol, returns a pointer to the elf specific area
  of the symbol if there is one.
  */
static INLINE elf_symbol_type *
DEFUN (elf_symbol_from, (ignore_abfd, symbol),
       bfd * ignore_abfd AND
       asymbol * symbol)
{
  if (symbol->the_bfd->xvec->flavour != bfd_target_elf_flavour)
    return 0;

  if (symbol->the_bfd->tdata.elf_obj_data == (struct elf_obj_tdata *) NULL)
    return 0;

  return (elf_symbol_type *) symbol;
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
DEFUN (elf_core_file_p, (abfd), bfd * abfd)
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_External_Phdr x_phdr;	/* Program header table entry, external form */
  Elf_Internal_Phdr *i_phdrp;	/* Program header table, internal form */
  unsigned int phindex;

  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      bfd_error = system_call_error;
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
      bfd_error = wrong_format;
      return NULL;
    }

  /* FIXME, Check EI_VERSION here !  */

  switch (x_ehdr.e_ident[EI_CLASS])
    {
    case ELFCLASSNONE:		/* address size not specified */
      goto wrong;		/* No support if can't tell address size */
    case ELFCLASS32:		/* 32-bit addresses */
      break;
    case ELFCLASS64:		/* 64-bit addresses */
      goto wrong;		/* FIXME: 64 bits not yet supported */
    default:
      goto wrong;		/* No support if unknown address class */
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
      bfd_error = no_memory;
      return NULL;
    }

  /* FIXME, `wrong' returns from this point onward, leak memory.  */

  /* Now that we know the byte order, swap in the rest of the header */
  i_ehdrp = elf_elfheader (abfd);
  elf_swap_ehdr_in (abfd, &x_ehdr, i_ehdrp);

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
      bfd_error = no_memory;
      return NULL;
    }
  if (bfd_seek (abfd, i_ehdrp->e_phoff, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return NULL;
    }
  for (phindex = 0; phindex < i_ehdrp->e_phnum; phindex++)
    {
      if (bfd_read ((PTR) & x_phdr, sizeof (x_phdr), 1, abfd)
	  != sizeof (x_phdr))
	{
	  bfd_error = system_call_error;
	  return NULL;
	}
      elf_swap_phdr_in (abfd, &x_phdr, i_phdrp + phindex);
    }

  /* Once all of the program headers have been read and converted, we
     can start processing them. */

  for (phindex = 0; phindex < i_ehdrp->e_phnum; phindex++)
    {
      bfd_section_from_phdr (abfd, i_phdrp + phindex, phindex);
      if ((i_phdrp + phindex)->p_type == PT_NOTE)
	{
	  elf_corefile_note (abfd, i_phdrp + phindex);
	}
    }

  /* Remember the entry point specified in the ELF file header. */

  bfd_get_start_address (abfd) = i_ehdrp->e_entry;

  return abfd->xvec;
}

boolean
DEFUN (elf_mkobject, (abfd), bfd * abfd)
{
  /* this just does initialization */
  /* coff_mkobject zalloc's space for tdata.coff_obj_data ... */
  elf_tdata (abfd) = (struct elf_obj_tdata *)
    bfd_zalloc (abfd, sizeof (struct elf_obj_tdata));
  if (elf_tdata (abfd) == 0)
    {
      bfd_error = no_memory;
      return false;
    }
  /* since everything is done at close time, do we need any
     initialization? */

  return true;
}

/*
  Create ELF output from BFD sections.

  Essentially, just create the section header and forget about the program
  header for now.

*/

#if 0 /* not used */
static int
elf_idx_of_sym (abfd, sym)
     bfd *abfd;
     asymbol *sym;
{
  int i;
  for (i = 0; i < abfd->symcount; i++)
    {
      if (sym == (asymbol *) abfd->outsymbols[i])
	{
	  /* sanity check */
	  BFD_ASSERT ((strcmp (sym->name, abfd->outsymbols[i]->name) == 0)
		      || (strlen (sym->name) == 0));
	  return i + 1;
	}
    }
  return STN_UNDEF;
}
#endif

static void
DEFUN (elf_make_sections, (abfd, asect, obj),
       bfd * abfd AND
       asection * asect AND
       PTR obj)
{
  elf_sect_thunk *thunk = (elf_sect_thunk *) obj;
  /* most of what is in bfd_shdr_from_section goes in here... */
  /* and all of these sections generate at *least* one ELF section. */
  int this_section;
  int idx;

  Elf_Internal_Shdr *this_hdr;
  this_section = elf_section_from_bfd_section (abfd, asect);
  this_hdr = &thunk->i_shdrp[this_section];

  this_hdr->sh_addr = asect->vma;
  this_hdr->sh_size = asect->_raw_size;
  /* contents already set by elf_set_section_contents */

  if (asect->flags & SEC_RELOC)
    {
      /* emit a reloc section, and thus strtab and symtab... */
      Elf_Internal_Shdr *rela_hdr;
      Elf_Internal_Shdr *symtab_hdr;
      Elf_External_Rela *outbound_relocas;
      Elf_External_Rel *outbound_relocs;
      int rela_section;
      int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

      symtab_hdr = &thunk->i_shdrp[thunk->symtab_section];

      if (thunk->symtab_section == this_section + 1)
	rela_section = thunk->symtab_section + 2; /* symtab + symstrtab */
      else
	rela_section = this_section + 1;
      rela_hdr = &thunk->i_shdrp[rela_section];
      rela_hdr->sh_link = thunk->symtab_section;
      rela_hdr->sh_info = this_section;

      /* orelocation has the data, reloc_count has the count... */
      if (use_rela_p)
	{
	  rela_hdr->sh_type = SHT_RELA;
	  rela_hdr->sh_entsize = sizeof (Elf_External_Rela);
	  rela_hdr->sh_size = rela_hdr->sh_entsize * asect->reloc_count;
	  outbound_relocas = (Elf_External_Rela *) bfd_alloc (abfd, rela_hdr->sh_size);
	
	  for (idx = 0; idx < asect->reloc_count; idx++)
	    {
	      Elf_Internal_Rela dst_rela;
	      Elf_External_Rela *src_rela;
	      arelent *ptr;
	
	      ptr = asect->orelocation[idx];
	      src_rela = outbound_relocas + idx;
	      if (!(abfd->flags & EXEC_P))
		dst_rela.r_offset = ptr->address - asect->vma;
	      else
		dst_rela.r_offset = ptr->address;
	
	      dst_rela.r_info
		= ELF32_R_INFO (elf_symbol_from_bfd_symbol (abfd, ptr->sym_ptr_ptr),
				ptr->howto->type);
	
	      dst_rela.r_addend = ptr->addend;
	      elf_swap_reloca_out (abfd, &dst_rela, src_rela);
	    }

	  rela_hdr->contents = (void *) outbound_relocas;
	}
      else
	/* REL relocations */
	{
	  rela_hdr->sh_type = SHT_REL;
	  rela_hdr->sh_entsize = sizeof (Elf_External_Rel);
	  rela_hdr->sh_size = rela_hdr->sh_entsize * asect->reloc_count;
	  outbound_relocs = (Elf_External_Rel *)
	    bfd_alloc (abfd, rela_hdr->sh_size);
	
	  for (idx = 0; idx < asect->reloc_count; idx++)
	    {
	      Elf_Internal_Rel dst_rel;
	      Elf_External_Rel *src_rel;
	      arelent *ptr;
	
	      ptr = asect->orelocation[idx];
	      src_rel = outbound_relocs + idx;
	      if (!(abfd->flags & EXEC_P))
		dst_rel.r_offset = ptr->address - asect->vma;
	      else
		dst_rel.r_offset = ptr->address;
	
	      dst_rel.r_info
		= ELF32_R_INFO (elf_symbol_from_bfd_symbol (abfd, ptr->sym_ptr_ptr),
				ptr->howto->type);
	
	      elf_swap_reloc_out (abfd, &dst_rel, src_rel);
	
	      /* Update the addend -- FIXME add 64 bit support.  */
#ifdef DEBUG
	      fprintf (stderr, "Updating addend: 0x%.8lx = %d, this_section = %d\n",
		       (long) ((unsigned char *) (elf_elfsections (abfd)[this_section].contents)
			       + dst_rel.r_offset), ptr->addend, this_section);
#endif
	
	      bfd_put_32 (abfd, ptr->addend,
			  (unsigned char *) (elf_elfsections (abfd)[this_section].contents)
			  + dst_rel.r_offset);
	    }
	  rela_hdr->contents = (void *) outbound_relocs;
	}
    }
  if (asect->flags & SEC_ALLOC)
    {
      this_hdr->sh_flags |= SHF_ALLOC;
      if (asect->flags & SEC_LOAD)
	{
	  /* @@ Do something with sh_type? */
	}
    }
  if (!(asect->flags & SEC_READONLY))
    this_hdr->sh_flags |= SHF_WRITE;

  if (asect->flags & SEC_CODE)
    this_hdr->sh_flags |= SHF_EXECINSTR;
}

static void
fix_up_strtabs (abfd, asect, obj)
     bfd *abfd;
     asection *asect;
     PTR obj;
{
  int this_section = elf_section_from_bfd_section (abfd, asect);
  elf_sect_thunk *thunk = (elf_sect_thunk *) obj;
  Elf_Internal_Shdr *this_hdr = &thunk->i_shdrp[this_section];

  /* @@ Check flags!  */
  if (!strncmp (asect->name, ".stab", 5)
      && strcmp ("str", asect->name + strlen (asect->name) - 3))
    {
      asection *s;
      char strtab[100];		/* @@ fixed size buffer -- eliminate */
      int stridx;

      strcpy (strtab, asect->name);
      strcat (strtab, "str");

      s = bfd_get_section_by_name (abfd, strtab);
#if 0
      fprintf (stderr, "`%s' -> 0x%x\n", strtab, s);
#endif
      if (s)
	{
	  Elf_Internal_Shdr *s2 = thunk->i_shdrp;

	  for (stridx = 0; /* ?? */; s2++, stridx++)
	    {
	      if (!strcmp (strtab, s2->sh_name + elf_shstrtab (abfd)->tab))
		break;
	    }
	}
      else
	{
	  stridx = 0;
#if 0
	  {
	    asection *s2 = abfd->sections;
	    fprintf (stderr, " not in:");
	    while (s2)
	      {
		fprintf (stderr, " %s", s2->name);
		s2 = s2->next;
	      }
	    fprintf (stderr, "\n");
	  }
#endif
	}
      this_hdr->sh_link = stridx;
      /* @@ Assuming 32 bits!  */
      this_hdr->sh_entsize = 0xc;
    }
}

static void
DEFUN (elf_fake_sections, (abfd, asect, obj),
       bfd * abfd AND
       asection * asect AND
       PTR obj)
{
  elf_sect_thunk *thunk = (elf_sect_thunk *) obj;
  /* most of what is in bfd_shdr_from_section goes in here... */
  /* and all of these sections generate at *least* one ELF section. */
  int this_section;

  /* check if we're making a PROGBITS section... */
  /* if ((asect->flags & SEC_ALLOC) && (asect->flags & SEC_LOAD)) */
  /* this was too strict... what *do* we want to check here? */
  if (1)
    {
      Elf_Internal_Shdr *this_hdr;
      this_section = thunk->i_ehdr->e_shnum++;
      this_hdr = &thunk->i_shdrp[this_section];
      this_hdr->sh_name =
	bfd_add_to_strtab (abfd, thunk->shstrtab, asect->name);
      /* we need to log the type *now* so that elf_section_from_bfd_section
	 can find us... have to set rawdata too. */
      this_hdr->rawdata = (void *) asect;
      this_hdr->sh_addralign = asect->alignment_power;
      if ((asect->flags & SEC_ALLOC) && (asect->flags & SEC_LOAD))
	this_hdr->sh_type = SHT_PROGBITS;
      /* @@ Select conditions correctly!  */
      else if (!strcmp (asect->name, ".bss"))
	this_hdr->sh_type = SHT_NOBITS;
      else
	/* what *do* we put here? */
	this_hdr->sh_type = SHT_PROGBITS;


      {
	/* Emit a strtab and symtab, and possibly a reloc section.  */
	Elf_Internal_Shdr *rela_hdr;
	Elf_Internal_Shdr *symtab_hdr;
	Elf_Internal_Shdr *symstrtab_hdr;
	int rela_section;
	int symstrtab_section;

	/* Note that only one symtab is used, so just remember it
	   for now.  */
	if (!thunk->symtab_section)
	  {
	    thunk->symtab_section = thunk->i_ehdr->e_shnum++;
	    symtab_hdr = &thunk->i_shdrp[thunk->symtab_section];
	    symtab_hdr->sh_name =
	      bfd_add_to_strtab (abfd, thunk->shstrtab, ".symtab");
	    symtab_hdr->sh_type = SHT_SYMTAB;
	    symtab_hdr->sh_entsize = sizeof (Elf_External_Sym);

	    symstrtab_section = thunk->i_ehdr->e_shnum++;
	    BFD_ASSERT (symstrtab_section == thunk->symtab_section + 1);
	    symstrtab_hdr = &thunk->i_shdrp[symstrtab_section];
	    symtab_hdr->sh_link = symstrtab_section;
	    symstrtab_hdr->sh_name =
	      bfd_add_to_strtab (abfd, thunk->shstrtab, ".strtab");
	    symstrtab_hdr->sh_type = SHT_STRTAB;

	    symtab_hdr->contents = 0;
	    symstrtab_hdr->contents = 0;
	    symstrtab_hdr->sh_size = 0;
	  }
	else
	  symtab_hdr = &thunk->i_shdrp[thunk->symtab_section];

	if (asect->flags & SEC_RELOC)
	  {
	    int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;

	    rela_section = thunk->i_ehdr->e_shnum++;
	    rela_hdr = &thunk->i_shdrp[rela_section];
	    rela_hdr->sh_name =
	      bfd_add_2_to_strtab (abfd, thunk->shstrtab,
				   use_rela_p ? ".rela" : ".rel",
				   asect->name);
	    rela_hdr->sh_type = use_rela_p ? SHT_RELA : SHT_REL;
	    rela_hdr->sh_link = thunk->symtab_section;
	    rela_hdr->sh_info = this_section;
	    rela_hdr->sh_entsize = sizeof (Elf_External_Rela);
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
}


/*
xxxINTERNAL_FUNCTION
	bfd_elf_locate_sh

xxxSYNOPSIS
	struct elf_internal_shdr *bfd_elf_locate_sh (bfd *abfd,
                                                     struct strtab *strtab,
                                                     struct elf_internal_shdr *shdrp,
                                                     CONST char *name);

xxxDESCRIPTION
	Helper function to locate an ELF section header given the
        name of a BFD section.
*/

struct elfNAME (internal_shdr) *
DEFUN (bfd_elf_locate_sh, (abfd, strtab, shdrp, name),
       bfd * abfd AND
       struct strtab *strtab AND
       struct elfNAME (internal_shdr) *shdrp AND
       CONST char *name)
{
  Elf_Internal_Shdr *gotit = NULL;
  int max, i;

  if (shdrp != NULL && strtab != NULL)
    {
      max = elf_elfheader (abfd)->e_shnum;
      for (i = 1; i < max; i++)
	{
	  if (!strcmp (strtab->tab + shdrp[i].sh_name, name))
	    {
	      gotit = &shdrp[i];
	    }
	}
    }
  return gotit;
}

/* Map symbol from it's internal number to the external number, moving
   all local symbols to be at the head of the list.  */

static INLINE int
sym_is_global (sym)
     asymbol *sym;
{
  if (sym->flags & BSF_GLOBAL)
    {
      if (sym->flags & BSF_LOCAL)
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

static void
DEFUN (elf_map_symbols, (abfd), bfd * abfd)
{
  int symcount = bfd_get_symcount (abfd);
  asymbol **syms = bfd_get_outsymbols (abfd);
  int num_locals = 0;
  int num_globals = 0;
  int num_locals2 = 0;
  int num_globals2 = 0;
  int num_sections = 0;
  int *symtab_map;
  int idx;
  asection *asect;

#ifdef DEBUG
  fprintf (stderr, "elf_map_symbols\n");
  fflush (stderr);
#endif

  /* Add local symbols for each allocated section
     FIXME -- we should only put out symbols for sections that
     are actually relocated against.  */
  for (asect = abfd->sections; asect; asect = asect->next)
    {
      if (asect->flags & (SEC_LOAD | SEC_DATA | SEC_CODE))
	num_sections++;
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

      for (asect = abfd->sections; asect; asect = asect->next)
	{
	  if (asect->flags & (SEC_LOAD | SEC_DATA | SEC_CODE))
	    {
	      asymbol *sym = syms[symcount++] = bfd_make_empty_symbol (abfd);
	      sym->the_bfd = abfd;
	      sym->name = asect->name;
	      sym->value = asect->vma;
	      sym->flags = BSF_SECTION_SYM;
	      sym->section = asect;
	    }
	}

      syms[symcount] = (asymbol *) 0;
      bfd_set_symtab (abfd, syms, symcount);
    }

  elf_symtab_map (abfd) = symtab_map
    = (int *) bfd_alloc (abfd, symcount * sizeof (int *));

  /* Identify and classify all of the symbols.  */
  for (idx = 0; idx < symcount; idx++)
    {
      if (!sym_is_global (syms[idx]))
	num_locals++;
      else
	num_globals++;
    }

  /* Now provide mapping information.  Add +1 for skipping over the
     dummy symbol.  */
  for (idx = 0; idx < symcount; idx++)
    {
      if (!sym_is_global (syms[idx]))
	symtab_map[idx] = 1 + num_locals2++;
      else
	symtab_map[idx] = 1 + num_locals + num_globals2++;
    }

  elf_num_locals (abfd) = num_locals;
  elf_num_globals (abfd) = num_globals;
}


static boolean
DEFUN (elf_compute_section_file_positions, (abfd), bfd * abfd)
{
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_Internal_Shdr *i_shdrp;	/* Section header table, internal form */
  struct strtab *shstrtab;
  int count, maxsections;
  elf_sect_thunk est;

  if (!elf_shstrtab (abfd))
    {
      i_ehdrp = elf_elfheader (abfd);	/* build new header in tdata memory */
      shstrtab = bfd_new_strtab (abfd);

      i_ehdrp->e_ident[EI_MAG0] = ELFMAG0;
      i_ehdrp->e_ident[EI_MAG1] = ELFMAG1;
      i_ehdrp->e_ident[EI_MAG2] = ELFMAG2;
      i_ehdrp->e_ident[EI_MAG3] = ELFMAG3;

      i_ehdrp->e_ident[EI_CLASS] = ELFCLASS32;	/* FIXME: find out from bfd */
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

      /* figure at most each section can have a rel, strtab, symtab */
      maxsections = 4 * bfd_count_sections (abfd) + 2;

      i_ehdrp->e_shoff = i_ehdrp->e_ehsize;

      /* and we'll just have to fix up the offsets later. */
      /* outbase += i_ehdr.e_shentsize * i_ehdr.e_shnum; */

      i_shdrp = (Elf_Internal_Shdr *)
	bfd_alloc (abfd, sizeof (*i_shdrp) * maxsections);
      if (!i_shdrp)
	{
	  bfd_error = no_memory;
	  return false;
	}
      for (count = 0; count < maxsections; count++)
	{
	  i_shdrp[count].rawdata = 0;
	  i_shdrp[count].contents = 0;
	}


      i_shdrp[0].sh_name = 0;
      i_shdrp[0].sh_type = SHT_NULL;
      i_shdrp[0].sh_flags = 0;
      i_shdrp[0].sh_addr = 0;
      i_shdrp[0].sh_offset = 0;
      i_shdrp[0].sh_size = 0;
      i_shdrp[0].sh_link = SHN_UNDEF;
      i_shdrp[0].sh_info = 0;
      i_shdrp[0].sh_addralign = 0;
      i_shdrp[0].sh_entsize = 0;

      i_ehdrp->e_shnum = 1;

      elf_elfsections (abfd) = i_shdrp;
      elf_shstrtab (abfd) = shstrtab;
    }
  est.i_ehdr = elf_elfheader (abfd);
  est.i_shdrp = elf_elfsections (abfd);
  est.shstrtab = elf_shstrtab (abfd);
  est.symtab_section = 0;	/* elf_fake_sections fills it in */

  elf_map_symbols (abfd);
  bfd_map_over_sections (abfd, elf_fake_sections, &est);
  {
    asection *secp;
    asection *psecp = (asection *) 0;
    int idx;
    Elf_Internal_Shdr *shp;

    for (idx = 0, secp = abfd->sections; secp; idx++, secp = secp->next)
      {
	if (psecp == (asection *) 0)
	  {
	    secp->filepos = est.i_ehdr->e_shoff + (est.i_ehdr->e_shnum + 1) * sizeof (Elf_External_Shdr);
	  }
	else
	  {
	    secp->filepos = psecp->filepos + psecp->_cooked_size;
	  }
	shp = bfd_elf_locate_sh (abfd, est.shstrtab, est.i_shdrp, secp->name);
	if (shp)
	  shp->sh_offset = secp->filepos;
	psecp = secp;
      }
  }
  elf_onesymtab (abfd) = est.symtab_section;
  return true;
}

static boolean
DEFUN (elf_write_phdrs, (abfd, i_ehdrp, i_phdrp, phdr_cnt),
       bfd * abfd AND
       Elf_Internal_Ehdr * i_ehdrp AND
       Elf_Internal_Phdr * i_phdrp AND
       Elf32_Half phdr_cnt)
{
  /* first program header entry goes after the file header */
  int outbase = i_ehdrp->e_ehsize;
  int i;
  Elf_External_Phdr x_phdr;

  for (i = 0; i < phdr_cnt; i++)
    {
      elf_swap_phdr_out (abfd, i_phdrp + i, &x_phdr);
      bfd_seek (abfd, outbase, SEEK_SET);
      bfd_write ((PTR) & x_phdr, sizeof (x_phdr), 1, abfd);
      outbase += sizeof (x_phdr);
    }

  return true;
}

static Elf_Internal_Phdr *
DEFUN (elf_build_phdrs, (abfd, i_ehdrp, i_shdrp, phdr_cnt),
       bfd * abfd AND
       Elf_Internal_Ehdr * i_ehdrp AND
       Elf_Internal_Shdr * i_shdrp AND
       Elf32_Half * phdr_cnt)
{
  Elf_Internal_Phdr *phdr_buf;
  int idx;
  /* NOTES:
     1. The program header table is *not* loaded as part
        of the memory image of the program.  If this
	changes later, the PT_PHDR entry must come first.
     2. there is currently no support for program header
     entries of type PT_PHDR, PT_DYNAMIC, PT_INTERP,
     or PT_SHLIB. */

  /* A. Figure out how many program header table entries are needed
       1. PT_LOAD for the text segment
       2. PT_LOAD for the data segment
     Then, reserve space for one more pointer.  This will be NULL
     to indicate the end of the program header table.  */

#ifdef PHDRS_INCLUDED
  *phdr_cnt = 4;
#else
  /* XXX right now, execve() expects exactly 3 PT entries on HPPA-OSF.  */
  *phdr_cnt = 3;
#endif

  phdr_buf = (Elf_Internal_Phdr *) bfd_xmalloc (((*phdr_cnt) + 1)
						*
						sizeof (Elf_Internal_Phdr));

  idx = 0;
#ifdef PHDRS_INCLUDED
  /* B. Fill in the PT_PHDR entry. */

  idx++;
#endif

  /* C. Fill in the PT_LOAD entry for the text segment. */

  phdr_buf[idx].p_type = PT_LOAD;

  /* get virtual/physical address from .text section */
  phdr_buf[idx].p_vaddr = bfd_get_section_by_name (abfd, ".text")->vma;
  phdr_buf[idx].p_paddr = 0;	/* XXX */

  /* Ultimately, we would like the size of the .text load
     segment to be the sum of the following sections:
	the program header table itself
	.interp
	.hash
	.dynsym
	.dynstr
	.rela.bss
	.rela.plt
	.init
	.text
	.fini
	.rodata
     But, right now, it will be the sum of the following sections:
	.text
	.rodata */

  {
    static char *CONST ld_sect_names[] =
    {".text", ".rodata", NULL};
    int i;
    int ld_size = 0;

    for (i = 0; ld_sect_names[i]; i++)
      {
	asection *asect = bfd_get_section_by_name (abfd,
						   ld_sect_names[i]);

	if (asect)
	  ld_size += bfd_section_size (abfd, asect);
      }
    phdr_buf[idx].p_filesz = ld_size;
    /* XXX: need to fix this */
    phdr_buf[idx].p_memsz = ld_size;
  }
  phdr_buf[idx].p_flags = PF_R + PF_X;
  phdr_buf[idx].p_align =
    bfd_get_section_by_name (abfd, ".text")->alignment_power;

  idx++;

  /* D. Fill in the PT_LOAD entry for the data segment.	*/

  phdr_buf[idx].p_type = PT_LOAD;

  /* get virtual/physical address from .data section */
  phdr_buf[idx].p_vaddr = bfd_get_section_by_name (abfd, ".data")->vma;
  phdr_buf[idx].p_paddr = 0;	/* XXX */

  /* Ultimately, we would like the size of the data load segment
     to be the sum of the following sections:
	the PT_DYNAMIC program header table entry
	.plt
	.data
	.data1
	.got
	.dynamic
     But, right now, it will be the sum of the following sections:
	.data */

  {
    static char *CONST ld_sect_names[] =
    {".data", NULL};
    int i;
    int ld_size = 0;

    for (i = 0; ld_sect_names[i]; i++)
      {
	asection *asect = bfd_get_section_by_name (abfd,
						   ld_sect_names[i]);

	if (asect)
	  ld_size += bfd_section_size (abfd, asect);
      }
    phdr_buf[idx].p_filesz = ld_size;
    /* XXX: need to fix this */
    phdr_buf[idx].p_memsz = ld_size;
  }
  phdr_buf[idx].p_flags = PF_R + PF_W + PF_X;
  phdr_buf[idx].p_align
    = bfd_get_section_by_name (abfd, ".data")->alignment_power;

  idx++;

  /* E. Fill in the PT_LOAD entry for the bss segment.	*/

  phdr_buf[idx].p_type = PT_LOAD;

  /* get virtual/physical address from .data section */
  phdr_buf[idx].p_vaddr = bfd_get_section_by_name (abfd, ".bss")->vma;
  phdr_buf[idx].p_paddr = 0;	/* XXX */

  {
    static char *CONST ld_sect_names[] =
    {".bss", NULL};
    int i;
    int ld_size = 0;

    for (i = 0; ld_sect_names[i]; i++)
      {
	asection *asect = bfd_get_section_by_name (abfd,
						   ld_sect_names[i]);

	if (asect)
	  ld_size += bfd_section_size (abfd, asect);
      }
    phdr_buf[idx].p_filesz = 0;
    /* XXX: need to fix this */
    phdr_buf[idx].p_memsz = ld_size;
  }
  phdr_buf[idx].p_flags = PF_R + PF_W + PF_X;
  phdr_buf[idx].p_align
    = bfd_get_section_by_name (abfd, ".bss")->alignment_power;

  idx++;

  /* F. Set up the "end of program header table" sentinel.	*/

  bzero ((char *) (phdr_buf + idx), sizeof (Elf_Internal_Phdr));
  idx++;

  BFD_ASSERT (idx - 1 == *phdr_cnt);

  return phdr_buf;
}

boolean
DEFUN (elf_write_object_contents, (abfd), bfd * abfd)
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */
  Elf_Internal_Phdr *i_phdrp = 0;	/* Program header table, internal form */
  Elf_External_Shdr *x_shdrp;	/* Section header table, external form */
  Elf_Internal_Shdr *i_shdrp;	/* Section header table, internal form */
  asection *nsect;
  elf_sect_thunk est;

  int outbase = 0;
  int count;
  int scnt;
  struct strtab *shstrtab;

  if (abfd->output_has_begun == false)
    {
      elf_compute_section_file_positions (abfd);
      abfd->output_has_begun = true;
    }

  i_ehdrp = elf_elfheader (abfd);
  i_shdrp = elf_elfsections (abfd);
  shstrtab = elf_shstrtab (abfd);

  est.i_ehdr = i_ehdrp;
  est.i_shdrp = i_shdrp;
  est.shstrtab = shstrtab;
  est.symtab_section = elf_onesymtab (abfd);	/* filled in by elf_fake */

  bfd_map_over_sections (abfd, elf_make_sections, &est);
  bfd_map_over_sections (abfd, fix_up_strtabs, &est);

  /* Dump out the symtabs. */
  {
    int symcount = bfd_get_symcount (abfd);
    asymbol **syms = bfd_get_outsymbols (abfd);
    struct strtab *stt = bfd_new_strtab (abfd);
    Elf_Internal_Shdr *symtab_hdr;
    Elf_Internal_Shdr *symstrtab_hdr;
    int symstrtab_section;
    Elf_External_Sym *outbound_syms;
    int idx;

    symtab_hdr = &i_shdrp[est.symtab_section];
    symtab_hdr->sh_type = SHT_SYMTAB;
    symtab_hdr->sh_entsize = sizeof (Elf_External_Sym);
    symtab_hdr->sh_size = symtab_hdr->sh_entsize * (symcount + 1);
    symtab_hdr->sh_info = elf_num_locals (abfd) + 1;

    /* see assert in elf_fake_sections that supports this: */
    symstrtab_section = est.symtab_section + 1;
    symstrtab_hdr = &i_shdrp[symstrtab_section];
    symtab_hdr->sh_link = symstrtab_section;
    symstrtab_hdr->sh_type = SHT_STRTAB;

    outbound_syms = (Elf_External_Sym *)
      bfd_alloc (abfd, (1 + symcount) * sizeof (Elf_External_Sym));
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

	sym.st_name = bfd_add_to_strtab (abfd, stt, syms[idx]->name);

	value += syms[idx]->section->output_section->vma
	  + syms[idx]->section->output_offset;
	sym.st_value = value;

	sym.st_size = (Elf32_Word) (elf_symbol_from (abfd, syms[idx]))->internal_elf_sym.st_size;

	if (syms[idx]->section == &bfd_und_section
	    || bfd_is_com_section (syms[idx]->section))
	  sym.st_info = ELF_ST_INFO (STB_GLOBAL, STT_NOTYPE);
	else if (syms[idx]->flags & BSF_WEAK)
	  sym.st_info = ELF_ST_INFO (STB_WEAK, STT_OBJECT);
	else if (syms[idx]->flags & BSF_SECTION_SYM)
	  sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_SECTION);
	else if (syms[idx]->flags & BSF_FILE)
	  sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_FILE);
	else if (syms[idx]->flags & (BSF_GLOBAL | BSF_EXPORT))
	  {
	    if (syms[idx]->flags & BSF_FUNCTION)
	      sym.st_info = ELF_ST_INFO (STB_GLOBAL, STT_FUNC);
	    else
	      sym.st_info = ELF_ST_INFO (STB_GLOBAL, STT_OBJECT);
	  }
	else if (syms[idx]->flags & BSF_LOCAL)
	  {
	    if (syms[idx]->flags & BSF_FUNCTION)
	      sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_FUNC);
	    else
	      sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_OBJECT);
	  }
	else
	  /* Default to local if flag isn't set at all.  */
	  sym.st_info = ELF_ST_INFO (STB_LOCAL, STT_OBJECT);

	sym.st_other = 0;
	if (syms[idx]->section)
	  sym.st_shndx =
	    elf_section_from_bfd_section (abfd,
					syms[idx]->section->output_section);
	else
	  sym.st_shndx = SHN_UNDEF;

	elf_swap_symbol_out (abfd, &sym,
			     outbound_syms + elf_symtab_map (abfd)[idx]);
      }

    symtab_hdr->contents = (void *) outbound_syms;
    symstrtab_hdr->contents = (void *) stt->tab;
    symstrtab_hdr->sh_size = stt->length;
    symstrtab_hdr->sh_type = SHT_STRTAB;
  }

  /* put the strtab out too... */
  {
    Elf_Internal_Shdr *this_hdr;
    int this_section;

    this_section = i_ehdrp->e_shnum++;
    i_ehdrp->e_shstrndx = this_section;
    this_hdr = &i_shdrp[this_section];
    this_hdr->sh_name = bfd_add_to_strtab (abfd, shstrtab, ".shstrtab");
    this_hdr->sh_type = SHT_STRTAB;
    this_hdr->sh_size = shstrtab->length;
    this_hdr->contents = (void *) shstrtab->tab;
  }

  outbase = i_ehdrp->e_ehsize;

  /* if we're building an executable, we'll need a program header table */
  if (abfd->flags & EXEC_P)
    {
      i_ehdrp->e_phentsize = sizeof (Elf_External_Phdr);

      /* elf_build_phdrs() returns a (NULL-terminated) array of
	 Elf_Internal_Phdrs */
      i_phdrp = elf_build_phdrs (abfd, i_ehdrp, i_shdrp, &i_ehdrp->e_phnum);
      i_ehdrp->e_phoff = i_ehdrp->e_ehsize;
      i_ehdrp->e_shoff = i_ehdrp->e_phoff + (i_ehdrp->e_phentsize
					     * i_ehdrp->e_phnum);
    }

  /* swap the header before spitting it out... */
  elf_swap_ehdr_out (abfd, i_ehdrp, &x_ehdr);
  bfd_seek (abfd, (file_ptr) 0, SEEK_SET);
  bfd_write ((PTR) & x_ehdr, sizeof (x_ehdr), 1, abfd);

#if 0
  outbase += i_ehdrp->e_phentsize * i_ehdrp->e_phnum;
  outbase += i_ehdrp->e_shentsize * i_ehdrp->e_shnum;
#endif
  /* find the next available location in the file
	  (just beyond the sections that have already been written). */
  outbase = 0;
  for (count = 1; count < i_ehdrp->e_shnum; count++)
    {
      if (i_shdrp[count].sh_offset && outbase < i_shdrp[count].sh_offset)
	outbase = i_shdrp[count].sh_offset + i_shdrp[count].sh_size;
    }

  /* now we fix up the offsets... */
  for (count = 1; count < i_ehdrp->e_shnum; count++)
    {
      if (i_shdrp[count].sh_offset == 0)
	i_shdrp[count].sh_offset = outbase;
      outbase += i_shdrp[count].sh_size;
    }

  /* If we're building an executable, fixup the program header table
     offsets.

     @@ For now, assume that the entries are in a fixed order: text,
     data, bss.  FIXME */

  if (abfd->flags & EXEC_P)
    {
      static char *CONST section_name[] =
      {".text", ".data", ".bss"};

      for (count = 0; count < 3; count++)
	{
	  asection *asect = bfd_get_section_by_name (abfd, section_name[count]);
	  int sh_idx = elf_section_from_bfd_section (abfd, asect);

	  i_phdrp[count].p_offset = i_shdrp[sh_idx].sh_offset;
	}

      /* write out the program header table entries */
      elf_write_phdrs (abfd, i_ehdrp, i_phdrp, i_ehdrp->e_phnum);
    }

  /* at this point we've concocted all the ELF sections... */
  x_shdrp = (Elf_External_Shdr *)
    bfd_alloc (abfd, sizeof (*x_shdrp) * (i_ehdrp->e_shnum));
  if (!x_shdrp)
    {
      bfd_error = no_memory;
      return false;
    }

  for (count = 0, scnt = 0; count < i_ehdrp->e_shnum; count++)
    {
      elf_swap_shdr_out (abfd, i_shdrp + count, x_shdrp + scnt);
      scnt++;
    }
  bfd_write ((PTR) x_shdrp, sizeof (*x_shdrp), i_ehdrp->e_shnum, abfd);
  /* need to dump the string table too... */

  /* after writing the headers, we need to write the sections too... */
  nsect = abfd->sections;
  for (count = 0; count < i_ehdrp->e_shnum; count++)
    {
      if (i_shdrp[count].contents)
	{
	  bfd_seek (abfd, i_shdrp[count].sh_offset, SEEK_SET);
	  bfd_write (i_shdrp[count].contents, i_shdrp[count].sh_size, 1, abfd);
	}
    }

  return true;
}

/* Given an index of a section, retrieve a pointer to it.  Note
   that for our purposes, sections are indexed by {1, 2, ...} with
   0 being an illegal index. */

/* In the original, each ELF section went into exactly one BFD
   section. This doesn't really make sense, so we need a real mapping.
   The mapping has to hide in the Elf_Internal_Shdr since asection
   doesn't have anything like a tdata field... */

static struct sec *
DEFUN (section_from_elf_index, (abfd, index),
       bfd * abfd AND
       int index)
{
  /* @@ Is bfd_com_section really correct in all the places it could
     be returned from this routine?  */

  if (index == SHN_ABS)
    return &bfd_com_section;	/* not abs? */
  if (index == SHN_COMMON)
    return &bfd_com_section;

  {
    Elf_Internal_Shdr *i_shdrp = elf_elfsections (abfd);
    Elf_Internal_Shdr *hdr = i_shdrp + index;

    switch (hdr->sh_type)
      {
	/* ELF sections that map to BFD sections */
      case SHT_PROGBITS:
      case SHT_NOBITS:
	if (!hdr->rawdata)
	  bfd_section_from_shdr (abfd, index);
	if (!hdr->rawdata)
	  abort ();
	return (struct sec *) hdr->rawdata;

      default:
	return (struct sec *) &bfd_abs_section;
      }
  }
}

/* given a section, search the header to find them... */
static int
DEFUN (elf_section_from_bfd_section, (abfd, asect),
       bfd * abfd AND
       struct sec *asect)
{
  Elf_Internal_Shdr *i_shdrp = elf_elfsections (abfd);
  int index;
  Elf_Internal_Shdr *hdr;
  int maxindex = elf_elfheader (abfd)->e_shnum;

  if (asect == &bfd_abs_section)
    return SHN_ABS;
  if (asect == &bfd_com_section)
    return SHN_COMMON;

  for (index = 0; index < maxindex; index++)
    {
      hdr = &i_shdrp[index];
      switch (hdr->sh_type)
	{
	  /* ELF sections that map to BFD sections */
	case SHT_PROGBITS:
	case SHT_NOBITS:
	  if (hdr->rawdata)
	    {
	      if (((struct sec *) (hdr->rawdata)) == asect)
		return index;
	    }
	  break;
	default:
	  break;
	}
    }
  return 0;
}

/* given a symbol, return the bfd index for that symbol.  */
static int
DEFUN (elf_symbol_from_bfd_symbol, (abfd, asym_ptr_ptr),
       bfd * abfd AND
       struct symbol_cache_entry **asym_ptr_ptr)
{
  struct symbol_cache_entry *asym_ptr = *asym_ptr_ptr;
  CONST char *name = asym_ptr->name;
  int idx;
  int symcount = bfd_get_symcount (abfd);
  asymbol **syms = bfd_get_outsymbols (abfd);

  /* FIXME -- there has to be a better way than linear search.  */
  for (idx = 0; idx < symcount; idx++)
    {
      if (syms[idx] == asym_ptr || (name == syms[idx]->name && name))
	break;
    }

  BFD_ASSERT (idx < symcount);
  idx = elf_symtab_map (abfd)[idx];

#ifdef DEBUG
  {
    flagword flags = asym_ptr->flags;

    fprintf (stderr,
	     "elf_symbol_from_bfd_symbol 0x%.8lx, name = %s, sym num = %d, flags =",
	     (long) asym_ptr, asym_ptr->name, idx);

    if (flags == BSF_NO_FLAGS)
      fprintf (stderr, " none");

    if (flags & BSF_LOCAL)
      fprintf (stderr, " local");

    if (flags & BSF_GLOBAL)
      fprintf (stderr, " global");

    if (flags & BSF_IMPORT)
      fprintf (stderr, " import");

    if (flags & BSF_EXPORT)
      fprintf (stderr, " export");

    if (flags & BSF_UNDEFINED_OBS)
      fprintf (stderr, " undefined_obs");

    if (flags & BSF_FORT_COMM_OBS)
      fprintf (stderr, " fort_comm_obs");

    if (flags & BSF_DEBUGGING)
      fprintf (stderr, " debugging");

    if (flags & BSF_KEEP)
      fprintf (stderr, " keep");

    if (flags & BSF_KEEP_G)
      fprintf (stderr, " keep_g");

    if (flags & BSF_WEAK)
      fprintf (stderr, " weak");

    if (flags & BSF_CTOR)
      fprintf (stderr, " ctor");

    if (flags & BSF_SECTION_SYM)
      fprintf (stderr, " section_sym");

    if (flags & BSF_OLD_COMMON)
      fprintf (stderr, " old_common");

    if (flags & BSF_NOT_AT_END)
      fprintf (stderr, " not_at_end");

    if (flags & BSF_CONSTRUCTOR)
      fprintf (stderr, " constructor");

    if (flags & BSF_WARNING)
      fprintf (stderr, " warning");

    if (flags & BSF_INDIRECT)
      fprintf (stderr, " indirect");

    if (flags & BSF_FILE)
      fprintf (stderr, " file");

    if (flags & BSF_FUNCTION)
      fprintf (stderr, " function");

    putc ('\n', stderr);
    fflush (stderr);
  }
#endif

  return idx;
}

static boolean
DEFUN (elf_slurp_symbol_table, (abfd, symptrs),
       bfd * abfd AND
       asymbol ** symptrs)	/* Buffer for generated bfd symbols */
{
  Elf_Internal_Shdr *i_shdrp = elf_elfsections (abfd);
  Elf_Internal_Shdr *hdr = i_shdrp + elf_onesymtab (abfd);
  int symcount;			/* Number of external ELF symbols */
  int i;
  elf_symbol_type *sym;		/* Pointer to current bfd symbol */
  elf_symbol_type *symbase;	/* Buffer for generated bfd symbols */
  Elf_Internal_Sym i_sym;
  Elf_External_Sym *x_symp;

  /* this is only valid because there is only one symtab... */
  /* FIXME:  This is incorrect, there may also be a dynamic symbol
     table which is a subset of the full symbol table.  We either need
     to be prepared to read both (and merge them) or ensure that we
     only read the full symbol table.  Currently we only get called to
     read the full symbol table.  -fnf */
  if (bfd_get_outsymbols (abfd) != NULL)
    {
      return true;
    }

  /* Read each raw ELF symbol, converting from external ELF form to
     internal ELF form, and then using the information to create a
     canonical bfd symbol table entry.

     Note that we allocate the initial bfd canonical symbol buffer
     based on a one-to-one mapping of the ELF symbols to canonical
     symbols.  We actually use all the ELF symbols, so there will be no
     space left over at the end.  When we have all the symbols, we
     build the caller's pointer vector. */

  if (bfd_seek (abfd, hdr->sh_offset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return false;
    }

  symcount = hdr->sh_size / sizeof (Elf_External_Sym);
  symbase = (elf_symbol_type *) bfd_zalloc (abfd, symcount * sizeof (elf_symbol_type));
  sym = symbase;

  /* Temporarily allocate room for the raw ELF symbols.  */
  x_symp = (Elf_External_Sym *) bfd_xmalloc (symcount * sizeof (Elf_External_Sym));

  if (bfd_read ((PTR) x_symp, sizeof (Elf_External_Sym), symcount, abfd)
      != symcount * sizeof (Elf_External_Sym))
    {
      free ((PTR) x_symp);
      bfd_error = system_call_error;
      return false;
    }
  /* Skip first symbol, which is a null dummy.  */
  for (i = 1; i < symcount; i++)
    {
      elf_swap_symbol_in (abfd, x_symp + i, &i_sym);
      memcpy (&sym->internal_elf_sym, &i_sym, sizeof (Elf_Internal_Sym));
      memcpy (&sym->native_elf_sym, x_symp + i, sizeof (Elf_External_Sym));
      sym->symbol.the_bfd = abfd;

      sym->symbol.name = elf_string_from_elf_section (abfd, hdr->sh_link,
						      i_sym.st_name);

      sym->symbol.value = i_sym.st_value;
#if 0
      /* FIXME -- this is almost certainly bogus.  It's from Pace
	 Willisson's hasty Solaris support, to pass the sizes of
	 object files or functions down into GDB via the back door, to
	 circumvent some other kludge in how Sun hacked stabs.  --
	 gnu@cygnus.com */
      /* XXX Size is now stored via a pointer in an elf_symbol_type;
	 should fix gdb then turn this off.  */
      sym->symbol.udata = (PTR) i_sym.st_size;
      /* FIXME -- end of bogosity.  */
#endif
      if (i_sym.st_shndx > 0 && i_sym.st_shndx < SHN_LORESERV)
	{
	  sym->symbol.section = section_from_elf_index (abfd, i_sym.st_shndx);
	}
      else if (i_sym.st_shndx == SHN_ABS)
	{
	  sym->symbol.section = &bfd_abs_section;
	}
      else if (i_sym.st_shndx == SHN_COMMON)
	{
	  sym->symbol.section = &bfd_com_section;
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
	  sym->symbol.flags |= (BSF_GLOBAL | BSF_EXPORT);
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
      /* Is this a definition of $global$?  If so, keep it because it will be
	 needd if any relocations are performed.  */
      if (!strcmp (sym->symbol.name, "$global$")
	  && sym->symbol.section != &bfd_und_section)
	{
	  /* @@ Why is this referring to backend data and not a field of
	     abfd?  FIXME */
	  struct elf_backend_data *be_data = (struct elf_backend_data *) abfd->xvec->backend_data;

	  be_data->global_sym = sym;
	}
      sym++;
    }

  /* We rely on the zalloc to clear out the final symbol entry.  */

  obj_raw_syms (abfd) = x_symp;

  bfd_get_symcount (abfd) = symcount = sym - symbase;

  /* Fill in the user's symbol pointer vector if needed.  */
  if (symptrs)
    {
      sym = symbase;
      while (symcount-- > 0)
	{
	  *symptrs++ = &sym->symbol;
	  sym++;
	}
      *symptrs = 0;		/* Final null pointer */
    }

  return true;
}

/* Return the number of bytes required to hold the symtab vector.

   Note that we base it on the count plus 1, since we will null terminate
   the vector allocated based on this size.  However, the ELF symbol table
   always has a dummy entry as symbol #0, so it ends up even.  */

unsigned int
DEFUN (elf_get_symtab_upper_bound, (abfd), bfd * abfd)
{
  unsigned int symcount;
  unsigned int symtab_size = 0;
  Elf_Internal_Shdr *i_shdrp;
  Elf_Internal_Shdr *hdr;

  i_shdrp = elf_elfsections (abfd);
  if (i_shdrp != NULL)
    {
      hdr = i_shdrp + elf_onesymtab (abfd);
      symcount = hdr->sh_size / sizeof (Elf_External_Sym);
      symtab_size = (symcount - 1 + 1) * (sizeof (asymbol));
    }
  return symtab_size;
}

/*
	This function return the number of bytes required to store the
	relocation information associated with section <<sect>>
	attached to bfd <<abfd>>

*/
unsigned int
elf_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (asect->flags & SEC_RELOC)
    {
      /* either rel or rela */
      return asect->_raw_size;
    }
  else
    return 0;
}

static boolean
DEFUN (elf_slurp_reloca_table, (abfd, asect, symbols),
       bfd * abfd AND
       sec_ptr asect AND
       asymbol ** symbols)
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

  bfd_seek (abfd, asect->rel_filepos, SEEK_SET);
  native_relocs = (Elf_External_Rela *)
    bfd_alloc (abfd, asect->reloc_count * sizeof (Elf_External_Rela));
  bfd_read ((PTR) native_relocs,
	    sizeof (Elf_External_Rela), asect->reloc_count, abfd);

  reloc_cache = (arelent *)
    bfd_alloc (abfd, (size_t) (asect->reloc_count * sizeof (arelent)));

  if (!reloc_cache)
    {
      bfd_error = no_memory;
      return false;
    }

  for (idx = 0; idx < asect->reloc_count; idx++)
    {
#ifdef RELOC_PROCESSING
      Elf_Internal_Rela dst;
      Elf_External_Rela *src;

      cache_ptr = reloc_cache + idx;
      src = native_relocs + idx;
      elf_swap_reloca_in (abfd, src, &dst);

      RELOC_PROCESSING (cache_ptr, &dst, symbols, abfd, asect);
#else
      Elf_Internal_Rela dst;
      Elf_External_Rela *src;

      cache_ptr = reloc_cache + idx;
      src = native_relocs + idx;

      elf_swap_reloca_in (abfd, src, &dst);

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
      /* ELF_R_SYM(dst.r_info) is the symbol table offset; subtract 1
	 because the first entry is NULL.  */
      cache_ptr->sym_ptr_ptr = symbols + ELF32_R_SYM (dst.r_info) - 1;
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
  fprintf (stderr, "sh_name      = %ld\n", (long) hdr->sh_name);
  fprintf (stderr, "sh_type      = %ld\n", (long) hdr->sh_type);
  fprintf (stderr, "sh_flags     = %ld\n", (long) hdr->sh_flags);
  fprintf (stderr, "sh_addr      = %ld\n", (long) hdr->sh_addr);
  fprintf (stderr, "sh_offset    = %ld\n", (long) hdr->sh_offset);
  fprintf (stderr, "sh_size      = %ld\n", (long) hdr->sh_size);
  fprintf (stderr, "sh_link      = %ld\n", (long) hdr->sh_link);
  fprintf (stderr, "sh_info      = %ld\n", (long) hdr->sh_info);
  fprintf (stderr, "sh_addralign = %ld\n", (long) hdr->sh_addralign);
  fprintf (stderr, "sh_entsize   = %ld\n", (long) hdr->sh_entsize);
  fprintf (stderr, "rawdata      = 0x%.8lx\n", (long) hdr->rawdata);
  fprintf (stderr, "contents     = 0x%.8lx\n", (long) hdr->contents);
  fprintf (stderr, "size         = %ld\n", (long) hdr->size);
  fflush (stderr);
}

#endif

static boolean
DEFUN (elf_slurp_reloc_table, (abfd, asect, symbols),
       bfd * abfd AND
       sec_ptr asect AND
       asymbol ** symbols)
{
  Elf_External_Rel *native_relocs;
  arelent *reloc_cache;
  arelent *cache_ptr;
  Elf_Internal_Shdr *i_shdrp;
  Elf_Internal_Shdr *data_hdr;
  ElfNAME (Off) data_off;
  ElfNAME (Word) data_max;
  char buf[4];			/* FIXME -- might be elf64 */

  unsigned int idx;

  if (asect->relocation)
    return true;
  if (asect->reloc_count == 0)
    return true;
  if (asect->flags & SEC_CONSTRUCTOR)
    return true;

  bfd_seek (abfd, asect->rel_filepos, SEEK_SET);
  native_relocs = (Elf_External_Rel *)
    bfd_alloc (abfd, asect->reloc_count * sizeof (Elf_External_Rel));
  bfd_read ((PTR) native_relocs,
	    sizeof (Elf_External_Rel), asect->reloc_count, abfd);

  reloc_cache = (arelent *)
    bfd_alloc (abfd, (size_t) (asect->reloc_count * sizeof (arelent)));

  if (!reloc_cache)
    {
      bfd_error = no_memory;
      return false;
    }

  /* Get the offset of the start of the segment we are relocating to read in
     the implicit addend.  */
  i_shdrp = elf_elfsections (abfd);
  data_hdr = i_shdrp + elf_section_from_bfd_section (abfd, asect);
  data_off = data_hdr->sh_offset;
  data_max = data_hdr->sh_size - sizeof (buf) + 1;

#ifdef DEBUG
  elf_debug_section ("data section", data_hdr - i_shdrp, data_hdr);
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
      /* ELF_R_SYM(dst.r_info) is the symbol table offset...
         -1 is to skip the dummy symbol table entry */
      cache_ptr->sym_ptr_ptr = symbols + ELF32_R_SYM (dst.r_info) - 1;
      BFD_ASSERT (dst.r_offset <= data_max);
      if (bfd_seek (abfd, data_off + dst.r_offset, SEEK_SET) != 0
	  || bfd_read ((PTR) buf, sizeof (buf), 1, abfd) != sizeof (buf))
	{
	  bfd_error = system_call_error;
	  return false;
	}

      cache_ptr->addend = (*abfd->xvec->bfd_getx_signed_32) ((bfd_byte *) buf);

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

unsigned int
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
    elf_slurp_reloca_table (abfd, section, symbols);
  else
    elf_slurp_reloc_table (abfd, section, symbols);

  tblptr = section->relocation;
  if (!tblptr)
    return 0;

  for (; count++ < section->reloc_count;)
    *relptr++ = tblptr++;

  *relptr = 0;
  return section->reloc_count;
}

unsigned int
DEFUN (elf_get_symtab, (abfd, alocation),
       bfd * abfd AND
       asymbol ** alocation)
{

  if (!elf_slurp_symbol_table (abfd, alocation))
    return 0;
  else
    return bfd_get_symcount (abfd);
}

asymbol *
DEFUN (elf_make_empty_symbol, (abfd),
       bfd * abfd)
{
  elf_symbol_type *newsym;

  newsym = (elf_symbol_type *) bfd_zalloc (abfd, sizeof (elf_symbol_type));
  if (!newsym)
    {
      bfd_error = no_memory;
      return NULL;
    }
  else
    {
      newsym->symbol.the_bfd = abfd;
      return &newsym->symbol;
    }
}

void
DEFUN (elf_get_symbol_info, (ignore_abfd, symbol, ret),
       bfd * ignore_abfd AND
       asymbol * symbol AND
       symbol_info * ret)
{
  bfd_symbol_info (symbol, ret);
}

void
DEFUN (elf_print_symbol, (ignore_abfd, filep, symbol, how),
       bfd * ignore_abfd AND
       PTR filep AND
       asymbol * symbol AND
       bfd_print_symbol_type how)
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
DEFUN (elf_get_lineno, (ignore_abfd, symbol),
       bfd * ignore_abfd AND
       asymbol * symbol)
{
  fprintf (stderr, "elf_get_lineno unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
  return NULL;
}

boolean
DEFUN (elf_set_arch_mach, (abfd, arch, machine),
       bfd * abfd AND
       enum bfd_architecture arch AND
       unsigned long machine)
{
  /* Allow any architecture to be supported by the elf backend */
  switch (arch)
    {
    case bfd_arch_unknown:	/* EM_NONE */
    case bfd_arch_sparc:	/* EM_SPARC */
    case bfd_arch_i386:	/* EM_386 */
    case bfd_arch_m68k:	/* EM_68K */
    case bfd_arch_m88k:	/* EM_88K */
    case bfd_arch_i860:	/* EM_860 */
    case bfd_arch_mips:	/* EM_MIPS (MIPS R3000) */
    case bfd_arch_hppa:	/* EM_HPPA (HP PA_RISC) */
      return bfd_default_set_arch_mach (abfd, arch, machine);
    default:
      return false;
    }
}

boolean
DEFUN (elf_find_nearest_line, (abfd,
			       section,
			       symbols,
			       offset,
			       filename_ptr,
			       functionname_ptr,
			       line_ptr),
       bfd * abfd AND
       asection * section AND
       asymbol ** symbols AND
       bfd_vma offset AND
       CONST char **filename_ptr AND
       CONST char **functionname_ptr AND
       unsigned int *line_ptr)
{
  fprintf (stderr, "elf_find_nearest_line unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
  return false;
}

int
DEFUN (elf_sizeof_headers, (abfd, reloc),
       bfd * abfd AND
       boolean reloc)
{
  fprintf (stderr, "elf_sizeof_headers unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
  return 0;
}

boolean
DEFUN (elf_set_section_contents, (abfd, section, location, offset, count),
       bfd * abfd AND
       sec_ptr section AND
       PTR location AND
       file_ptr offset AND
       bfd_size_type count)
{
  int dest_sect;

  if (abfd->output_has_begun == false)	/* set by bfd.c handler? */
    {
      /* do setup calculations (FIXME) */
      elf_compute_section_file_positions (abfd);
      abfd->output_has_begun = true;
    }

  dest_sect = elf_section_from_bfd_section (abfd, section);
  if (!dest_sect)
    return false;

  if (bfd_seek (abfd, elf_elfsections (abfd)[dest_sect].sh_offset + offset, SEEK_SET) == -1)
    return false;
  if (bfd_write (location, 1, count, abfd) != count)
    return false;
  return true;
}

void
DEFUN (elf_no_info_to_howto, (abfd, cache_ptr, dst),
       bfd * abfd AND
       arelent * cache_ptr AND
       Elf_Internal_Rela * dst)
{
  fprintf (stderr, "elf RELA relocation support for target machine unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
}

void
DEFUN (elf_no_info_to_howto_rel, (abfd, cache_ptr, dst),
       bfd * abfd AND
       arelent * cache_ptr AND
       Elf_Internal_Rel * dst)
{
  fprintf (stderr, "elf REL relocation support for target machine unimplemented\n");
  fflush (stderr);
  BFD_FAIL ();
}

boolean
DEFUN (elf_get_sect_thunk, (abfd, est),
       bfd * abfd AND
       elf_sect_thunk * est)
{
  if (est == (elf_sect_thunk *) NULL)
    return false;

  est->i_ehdr = elf_elfheader (abfd);
  est->i_shdrp = elf_elfsections (abfd);
  est->shstrtab = elf_shstrtab (abfd);
  est->symtab_section = elf_onesymtab (abfd);	/* filled in by elf_fake */

  return true;
}
