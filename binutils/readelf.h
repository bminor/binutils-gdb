/* readelf.h -- This file contains structures similar to those found in
   include/elf/internal.h.  These structures however, match the layout of
   the information in the file, rather than being optimised for internal
   storage. 

   Copyright (C) 1998 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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

#ifndef _READELF_H
#define _READELF_H

#include "bfd.h"
#include "elf/common.h"

/* The type 'bfd_vma' is used in places where the 64 bit version of the
   structure has a 64 bit wide field, and the 32 bit version has a 32 bit wide
   field.  It does not necessarily imply that the field contains an address.  */

#define EI_NIDENT	16

/* The ELF file header.  This appears at the start of every ELF file.  */
typedef struct
{
  unsigned char		e_ident [EI_NIDENT];	/* Magic number and other info */
  unsigned short	e_type;			/* Object file type */
  unsigned short	e_machine;		/* Architecture */
  unsigned long		e_version;		/* Object file version */
  bfd_vma		e_entry;		/* Entry point virtual address */
  bfd_vma		e_phoff;		/* Program header table file offset */
  bfd_vma		e_shoff;		/* Section header table file offset */
  unsigned long		e_flags;		/* Processor-specific flags */
  unsigned short	e_ehsize;		/* ELF header size in bytes */
  unsigned short	e_phentsize;		/* Program header table entry size */
  unsigned short	e_phnum;		/* Program header table entry count */
  unsigned short	e_shentsize;		/* Section header table entry size */
  unsigned short	e_shnum;		/* Section header table entry count */
  unsigned short	e_shstrndx;		/* Section header string table index */
}
Elf_Ehdr;

/* Program Segment Header.  (Might be wrong for 64 bit ELF formats).  */
typedef struct
{
  unsigned long		p_type;			/* Segment type */
  bfd_vma		p_offset;		/* Segment file offset */
  bfd_vma		p_vaddr;		/* Segment virtual address */
  bfd_vma		p_paddr;		/* Segment physical address */
  unsigned long		p_filesz;		/* Segment size in file */
  unsigned long		p_memsz;		/* Segment size in memory */
  unsigned long		p_flags;		/* Segment flags */
  unsigned long		p_align;		/* Segment alignment */
}
Elf_Phdr;

/* Section header.  */
typedef struct
{
  unsigned long		sh_name;		/* Section name (string tbl index) */
  unsigned long		sh_type;		/* Section type */
  bfd_vma		sh_flags;		/* Section flags */
  bfd_vma		sh_addr;		/* Section virtual addr at execution */
  bfd_vma		sh_offset;		/* Section file offset */
  unsigned long		sh_size;		/* Section size in bytes */
  unsigned long		sh_link;		/* Link to another section */
  unsigned long		sh_info;		/* Additional section information */
  bfd_vma		sh_addralign;		/* Section alignment */
  bfd_vma		sh_entsize;		/* Entry size if section holds table */
}
Elf_Shdr;

/* Symbol table entry.  (Might be incorrect for 64 bit ELF formats) */
typedef struct
{
  unsigned long		st_name;		/* Symbol name (string tbl index) */
  bfd_vma 		st_value;		/* Symbol value */
  bfd_vma		st_size;		/* Symbol size */
  unsigned char		st_info;		/* Symbol type and binding */
  unsigned char		st_other;		/* No defined meaning, 0 */
  unsigned short	st_shndx;		/* Section index */
}
Elf_Sym;

/* Version definition sections.  */
typedef struct
{
  unsigned short 	vd_version;	/* Version number of structure.  */
  unsigned short 	vd_flags;	/* Flags (VER_FLG_*).  */
  unsigned short 	vd_ndx;		/* Version index.  */
  unsigned short 	vd_cnt;		/* Number of verdaux entries.  */
  unsigned long	 	vd_hash;	/* Hash of name.  */
  unsigned long	 	vd_aux;		/* Offset to verdaux entries.  */
  unsigned long	 	vd_next;	/* Offset to next verdef.  */
}
Elf_Verdef;

/* Auxialiary version information.  */
typedef struct
{
  unsigned long 	vda_name;	/* String table offset of name.  */
  unsigned long 	vda_next;	/* Offset to next verdaux.  */
}
Elf_Verdaux;

/* Version dependency section.  */
typedef struct
{
  unsigned short 	vn_version;	/* Version number of structure.  */
  unsigned short 	vn_cnt;		/* Number of vernaux entries.  */
  unsigned long	 	vn_file;	/* String table offset of library name.  */
  unsigned long	 	vn_aux;		/* Offset to vernaux entries.  */
  unsigned long	 	vn_next;	/* Offset to next verneed.  */
}
Elf_Verneed;

/* Auxiliary needed version information.  */
typedef struct
{
  unsigned long	 	vna_hash;	/* Hash of dependency name.  */
  unsigned short 	vna_flags;	/* Flags (VER_FLG_*).  */
  unsigned short 	vna_other;	/* Unused.  */
  unsigned long	 	vna_name;	/* String table offset to version name.  */
  unsigned long	 	vna_next;	/* Offset to next vernaux.  */
}
Elf_Vernaux;

/* Relocation table entry without addend.  */
typedef struct
{
  bfd_vma		r_offset;	/* Location at which to apply the action */
  bfd_vma		r_info;		/* index and type of relocation */
}
Elf_Rel;

/* Relocation table entry with addend.  */
typedef struct
{
  bfd_vma		r_offset;	/* Location at which to apply the action */
  bfd_vma		r_info;		/* Index and Type of relocation */
  bfd_signed_vma	r_addend;	/* Constant addend used to compute value */
}
Elf_Rela;

/* Dynamic section entry.  */
typedef struct
{
  signed long		d_tag;		/* Dynamic entry type */
  union
    {
      bfd_vma 		d_val;		/* Integer value */
      bfd_vma 		d_ptr;		/* Address value */
    } d_un;
}
Elf_Dyn;


#define DT_VERSIONTAGIDX(tag)	(DT_VERNEEDNUM - (tag))	/* Reverse order! */

#endif /* _READELF_H */
