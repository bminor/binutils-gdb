/* ELF support for BFD.
   Copyright (C) 1991 Free Software Foundation, Inc.

   Written by Fred Fish @ Cygnus Support, from information published
   in "UNIX System V Release 4, Programmers Guide: ANSI C and
   Programming Support Tools".

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


	/****************************************

	  		WARNING

	This is only a partial ELF implementation,
	incorporating only those parts that are
	required to get gdb up and running.  It is
	expected that it will be expanded to a full
	ELF implementation at some future date.

	Unimplemented stubs call abort() to ensure
	that they get proper attention if they are
	ever called.  The stubs are here since
	this version was hacked from the COFF
	version, and thus they will probably
	go away or get expanded appropriately in a
	future version.

	fnf@cygnus.com

	*****************************************/


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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "elf/common.h"
#include "elf/internal.h"
#include "elf/external.h"

#ifdef HAVE_PROCFS	/* Some core file support requires host /proc files */
#include <sys/procfs.h>
#else
#define bfd_prstatus(abfd, descdata, descsz, filepos)	/* Define away */
#define bfd_fpregset(abfd, descdata, descsz, filepos)	/* Define away */
#define bfd_prpsinfo(abfd, descdata, descsz, filepos)	/* Define away */
#endif

/* Forward data declarations */

extern bfd_target elf_little_vec, elf_big_vec;

/* Currently the elf_symbol_type struct just contains the generic bfd
   symbol structure. */

typedef struct
{
  asymbol symbol;
} elf_symbol_type;

/* Some private data is stashed away for future use using the tdata pointer
   in the bfd structure.  This information is different for ELF core files
   and other ELF files. */

typedef struct elf_core_tdata_struct
{
  void *prstatus;		/* The raw /proc prstatus structure */
  void *prpsinfo;		/* The raw /proc prpsinfo structure */
} elf_core_tdata;

#define core_prpsinfo(bfd) (((bfd)->tdata.elf_core_data) -> prpsinfo)
#define core_prstatus(bfd) (((bfd)->tdata.elf_core_data) -> prstatus)


typedef struct elf_obj_tdata_struct
{
  file_ptr symtab_filepos;	/* Offset to start of ELF symtab section */
  long symtab_filesz;		/* Size of ELF symtab section */
  file_ptr strtab_filepos;	/* Offset to start of ELF string tbl section */
  long strtab_filesz;		/* Size of ELF string tbl section */
} elf_obj_tdata;

#define elf_tdata(bfd)		((bfd) -> tdata.elf_obj_data)
#define elf_symtab_filepos(bfd)	(elf_tdata(bfd) -> symtab_filepos)
#define elf_symtab_filesz(bfd)	(elf_tdata(bfd) -> symtab_filesz)
#define elf_strtab_filepos(bfd)	(elf_tdata(bfd) -> strtab_filepos)
#define elf_strtab_filesz(bfd)	(elf_tdata(bfd) -> strtab_filesz)

/* Translate an ELF symbol in external format into an ELF symbol in internal
   format. */

static void
DEFUN(elf_swap_symbol_in,(abfd, src, dst),
      bfd               *abfd AND
      Elf_External_Sym *src AND
      Elf_Internal_Sym *dst)
{
  dst -> st_name = bfd_h_get_32 (abfd, (bfd_byte *) src -> st_name);
  dst -> st_value = bfd_h_get_32 (abfd, (bfd_byte *) src -> st_value);
  dst -> st_size = bfd_h_get_32 (abfd, (bfd_byte *) src -> st_size);
  dst -> st_info = bfd_h_get_8 (abfd, (bfd_byte *) src -> st_info);
  dst -> st_other = bfd_h_get_8 (abfd, (bfd_byte *) src -> st_other);
  dst -> st_shndx = bfd_h_get_16 (abfd, (bfd_byte *) src -> st_shndx);
}


/* Translate an ELF file header in external format into an ELF file header in
   internal format. */

static void
DEFUN(elf_swap_ehdr_in,(abfd, src, dst),
      bfd               *abfd AND
      Elf_External_Ehdr *src AND
      Elf_Internal_Ehdr *dst)
{
  memcpy (dst -> e_ident, src -> e_ident, EI_NIDENT);
  dst -> e_type = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_type);
  dst -> e_machine = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_machine);
  dst -> e_version = bfd_h_get_32 (abfd, (bfd_byte *) src -> e_version);
  dst -> e_entry = bfd_h_get_32 (abfd, (bfd_byte *) src -> e_entry);
  dst -> e_phoff = bfd_h_get_32 (abfd, (bfd_byte *) src -> e_phoff);
  dst -> e_shoff = bfd_h_get_32 (abfd, (bfd_byte *) src -> e_shoff);
  dst -> e_flags = bfd_h_get_32 (abfd, (bfd_byte *) src -> e_flags);
  dst -> e_ehsize = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_ehsize);
  dst -> e_phentsize = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_phentsize);
  dst -> e_phnum = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_phnum);
  dst -> e_shentsize = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_shentsize);
  dst -> e_shnum = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_shnum);
  dst -> e_shstrndx = bfd_h_get_16 (abfd, (bfd_byte *) src -> e_shstrndx);
}


/* Translate an ELF section header table entry in external format into an
   ELF section header table entry in internal format. */

static void
DEFUN(elf_swap_shdr_in,(abfd, src, dst),
      bfd               *abfd AND
      Elf_External_Shdr *src AND
      Elf_Internal_Shdr *dst)
{
  dst -> sh_name = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_name);
  dst -> sh_type = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_type);
  dst -> sh_flags = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_flags);
  dst -> sh_addr = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_addr);
  dst -> sh_offset = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_offset);
  dst -> sh_size = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_size);
  dst -> sh_link = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_link);
  dst -> sh_info = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_info);
  dst -> sh_addralign = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_addralign);
  dst -> sh_entsize = bfd_h_get_32 (abfd, (bfd_byte *) src -> sh_entsize);
}


/* Translate an ELF program header table entry in external format into an
   ELF program header table entry in internal format. */

static void
DEFUN(elf_swap_phdr_in,(abfd, src, dst),
      bfd               *abfd AND
      Elf_External_Phdr *src AND
      Elf_Internal_Phdr *dst)
{
  dst -> p_type = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_type);
  dst -> p_offset = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_offset);
  dst -> p_vaddr = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_vaddr);
  dst -> p_paddr = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_paddr);
  dst -> p_filesz = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_filesz);
  dst -> p_memsz = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_memsz);
  dst -> p_flags = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_flags);
  dst -> p_align = bfd_h_get_32 (abfd, (bfd_byte *) src -> p_align);
}


/* Create a new bfd section from an ELF section header. */

static boolean
DEFUN(bfd_section_from_shdr, (abfd, hdr, shstrtab),
      bfd               *abfd AND
      Elf_Internal_Shdr *hdr AND
      char		*shstrtab)
{
  asection *newsect;
  char *name;

  name = hdr -> sh_name ? shstrtab + hdr -> sh_name : "unnamed";
  newsect = bfd_make_section (abfd, name);
  newsect -> vma = hdr -> sh_addr;
  newsect -> _raw_size = hdr -> sh_size;
  if (!(hdr -> sh_type == SHT_NOBITS))
    {
      newsect -> filepos = hdr -> sh_offset;
      newsect -> flags |= SEC_HAS_CONTENTS;
    }
  if (hdr -> sh_flags & SHF_ALLOC)
    {
      newsect -> flags |= SEC_ALLOC;
      if (hdr -> sh_type != SHT_NOBITS)
	{
	  newsect -> flags |= SEC_LOAD;
	}
    }
  if (!(hdr -> sh_flags & SHF_WRITE))
    {
      newsect -> flags |= SEC_READONLY;
    }
  if (hdr -> sh_flags & SHF_EXECINSTR)
    {
      newsect -> flags |= SEC_CODE;	/* FIXME: may only contain SOME code */
    }
  else
    {
      newsect -> flags |= SEC_DATA;
    }
  if (hdr -> sh_type == SHT_SYMTAB)
    {
      abfd -> flags |= HAS_SYMS;
    }

  return (true);
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
DEFUN(bfd_section_from_phdr, (abfd, hdr, index),
      bfd               *abfd AND
      Elf_Internal_Phdr *hdr AND
      int		 index)
{
  asection *newsect;
  char *name;
  char namebuf[64];
  int split;

  split = ((hdr -> p_memsz > 0) &&
	   (hdr -> p_filesz > 0) &&
	   (hdr -> p_memsz > hdr -> p_filesz));
  sprintf (namebuf, split ? "segment%da" : "segment%d", index);
  name = bfd_alloc (abfd, strlen (namebuf) + 1);
  (void) strcpy (name, namebuf);
  newsect = bfd_make_section (abfd, name);
  newsect -> vma = hdr -> p_vaddr;
  newsect -> _raw_size = hdr -> p_filesz;
  newsect -> filepos = hdr -> p_offset;
  newsect -> flags |= SEC_HAS_CONTENTS;
  if (hdr -> p_type == PT_LOAD)
    {
      newsect -> flags |= SEC_ALLOC;
      newsect -> flags |= SEC_LOAD;
      if (hdr -> p_flags & PF_X)
	{
	  /* FIXME: all we known is that it has execute PERMISSION,
	     may be data. */
	  newsect -> flags |= SEC_CODE;
	}
    }
  if (!(hdr -> p_flags & PF_W))
    {
      newsect -> flags |= SEC_READONLY;
    }

  if (split)
    {
      sprintf (namebuf, "segment%db", index);
      name = bfd_alloc (abfd, strlen (namebuf) + 1);
      (void) strcpy (name, namebuf);
      newsect = bfd_make_section (abfd, name);
      newsect -> vma = hdr -> p_vaddr + hdr -> p_filesz;
      newsect -> _raw_size = hdr -> p_memsz - hdr -> p_filesz;
      if (hdr -> p_type == PT_LOAD)
	{
	  newsect -> flags |= SEC_ALLOC;
	  if (hdr -> p_flags & PF_X)
	    {
	      newsect -> flags |= SEC_CODE;
	    }
	}
      if (!(hdr -> p_flags & PF_W))
	{
	  newsect -> flags |= SEC_READONLY;
	}
    }

  return (true);
}

#ifdef HAVE_PROCFS

static void
DEFUN(bfd_prstatus,(abfd, descdata, descsz, filepos),
      bfd	*abfd AND
      char	*descdata AND
      int	 descsz AND
      long	 filepos)
{
  asection *newsect;

  if (descsz == sizeof (prstatus_t))
    {
      newsect = bfd_make_section (abfd, ".reg");
      newsect -> _raw_size = sizeof (gregset_t);
      newsect -> filepos = filepos + (long) (((prstatus_t *)0) -> pr_reg);
      newsect -> flags = SEC_ALLOC | SEC_HAS_CONTENTS;
      newsect -> alignment_power = 2;
      if ((core_prstatus (abfd) = bfd_alloc (abfd, descsz)) != NULL)
	{
	  memcpy (core_prstatus (abfd), descdata, descsz);
	}
    }
}

/* Stash a copy of the prpsinfo structure away for future use. */

static void
DEFUN(bfd_prpsinfo,(abfd, descdata, descsz, filepos),
      bfd	*abfd AND
      char	*descdata AND
      int	 descsz AND
      long	 filepos)
{
  asection *newsect;

  if (descsz == sizeof (prpsinfo_t))
    {
      if ((core_prpsinfo (abfd) = bfd_alloc (abfd, descsz)) != NULL)
	{
	  bcopy (descdata, core_prpsinfo (abfd), descsz);
	}
    }
}

static void
DEFUN(bfd_fpregset,(abfd, descdata, descsz, filepos),
      bfd	*abfd AND
      char	*descdata AND
      int	 descsz AND
      long	 filepos)
{
  asection *newsect;

  if (descsz == sizeof (fpregset_t))
    {
      newsect = bfd_make_section (abfd, ".reg2");
      newsect -> _raw_size = sizeof (fpregset_t);
      newsect -> filepos = filepos;
      newsect -> flags = SEC_ALLOC | SEC_HAS_CONTENTS;
      newsect -> alignment_power = 2;
    }
}

#endif	/* HAVE_PROCFS */

/* Return a pointer to the args (including the command name) that were
   seen by the program that generated the core dump.  Note that for
   some reason, a spurious space is tacked onto the end of the args
   in some (at least one anyway) implementations, so strip it off if
   it exists. */

char *
DEFUN(elf_core_file_failing_command, (abfd),
     bfd *abfd)
{
#ifdef HAVE_PROCFS
  if (core_prpsinfo (abfd))
    {
      prpsinfo_t *p = core_prpsinfo (abfd);
      char *scan = p -> pr_psargs;
      while (*scan++) {;}
      scan -= 2;
      if ((scan > p -> pr_psargs) && (*scan == ' '))
	{
	  *scan = '\000';
	}
      return (p -> pr_psargs);
    }
#endif
  return (NULL);
}

/* Return the number of the signal that caused the core dump.  Presumably,
   since we have a core file, we got a signal of some kind, so don't bother
   checking the other process status fields, just return the signal number.
   */

static int
DEFUN(elf_core_file_failing_signal, (abfd),
      bfd *abfd)
{
#ifdef HAVE_PROCFS
  if (core_prstatus (abfd))
    {
      return (((prstatus_t *)(core_prstatus (abfd))) -> pr_cursig);
    }
#endif
  return (-1);
}

/* Check to see if the core file could reasonably be expected to have
   come for the current executable file.  Note that by default we return
   true unless we find something that indicates that there might be a
   problem.
   */

static boolean
DEFUN(elf_core_file_matches_executable_p, (core_bfd, exec_bfd),
      bfd *core_bfd AND
      bfd *exec_bfd)
{
#ifdef HAVE_PROCFS
  char *corename;
  char *execname;
#endif

  /* First, xvecs must match since both are ELF files for the same target. */

  if (core_bfd->xvec != exec_bfd->xvec)
    {
      bfd_error = system_call_error;
      return (false);
    }

#ifdef HAVE_PROCFS

  /* If no prpsinfo, just return true.  Otherwise, grab the last component
     of the exec'd pathname from the prpsinfo. */

  if (core_prpsinfo (core_bfd))
    {
      corename = (((struct prpsinfo *) core_prpsinfo (core_bfd)) -> pr_fname);
    }  
  else
    {
      return (true);
    }

  /* Find the last component of the executable pathname. */

  if ((execname = strrchr (exec_bfd -> filename, '/')) != NULL)
    {
      execname++;
    }
  else
    {
      execname = (char *) exec_bfd -> filename;
    }

  /* See if they match */

  return (strcmp (execname, corename) ? false : true);

#else

  return (true);

#endif	/* HAVE_PROCFS */
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
DEFUN(elf_corefile_note, (abfd, hdr),
      bfd               *abfd AND
      Elf_Internal_Phdr *hdr)
{
  Elf_External_Note *x_note_p;	/* Elf note, external form */
  Elf_Internal_Note i_note;	/* Elf note, internal form */
  char *buf = NULL;		/* Entire note segment contents */
  char *namedata;		/* Name portion of the note */
  char *descdata;		/* Descriptor portion of the note */
  char *sectname;		/* Name to use for new section */
  long filepos;			/* File offset to descriptor data */
  asection *newsect;

  if (hdr -> p_filesz > 0
      && (buf = (char *)malloc(hdr -> p_filesz)) != NULL
      && bfd_seek (abfd, hdr -> p_offset, SEEK_SET) != -1L
      && bfd_read ((PTR) buf, hdr -> p_filesz, 1, abfd) == hdr -> p_filesz)
    {
      x_note_p = (Elf_External_Note *) buf;
      while ((char *) x_note_p < (buf + hdr -> p_filesz))
	{
	  i_note.namesz = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p -> namesz);
	  i_note.descsz = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p -> descsz);
	  i_note.type = bfd_h_get_32 (abfd, (bfd_byte *) x_note_p -> type);
	  namedata = x_note_p -> name;
	  descdata = namedata + BFD_ALIGN (i_note.namesz, 4);
	  filepos = hdr -> p_offset + (descdata - buf);
	  switch (i_note.type) {
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
	      newsect -> _raw_size = i_note.descsz;
	      newsect -> filepos = filepos;
	      newsect -> flags = SEC_ALLOC | SEC_HAS_CONTENTS;
	      newsect -> alignment_power = 2;
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
DEFUN(elf_read, (abfd, offset, size),
      bfd	*abfd AND
      long	offset AND
      int	size)
{
  char *buf;

  if ((buf = bfd_alloc (abfd, size)) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  if (bfd_seek (abfd, offset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (NULL);
    }
  if (bfd_read ((PTR) buf, size, 1, abfd) != size)
    {
      bfd_error = system_call_error;
      return (NULL);
    }
  return (buf);
}

/* Begin processing a given object.

   First we validate the file by reading in the ELF header and checking
   the magic number.

   */

static bfd_target *
DEFUN (elf_object_p, (abfd), bfd *abfd)
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr i_ehdr;	/* Elf file header, internal form */
  Elf_External_Shdr *x_shdr;	/* Section header table, external form */
  Elf_Internal_Shdr *i_shdr;	/* Section header table, internal form */
  int shindex;
  char *shstrtab;		/* Internal copy of section header stringtab */
  int shstrtabsize;		/* Size of section header string table */
  Elf_Off offset;		/* Temp place to stash file offsets */
  
  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) &x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      bfd_error = system_call_error;
      return (NULL);
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry, and it must have a
     section header table (FIXME: See comments re sections at top of this
     file). */

  if (x_ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
      x_ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
      x_ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
      x_ehdr.e_ident[EI_MAG3] != ELFMAG3)
    {
wrong:
      bfd_error = wrong_format;
      return (NULL);
    }

  /* FIXME, Check EI_VERSION here !  */

  switch (x_ehdr.e_ident[EI_CLASS]) {
  case ELFCLASSNONE:			/* address size not specified */
    goto wrong;			/* No support if can't tell address size */
  case ELFCLASS32:			/* 32-bit addresses */
    break;
  case ELFCLASS64:			/* 64-bit addresses */
    goto wrong;			/* FIXME: 64 bits not yet supported */
  default:
    goto wrong;			/* No support if unknown address class */
  }

  /* Switch xvec to match the specified byte order.  */
  switch (x_ehdr.e_ident[EI_DATA]) {
  case ELFDATA2MSB:			/* Big-endian */ 
    abfd->xvec = &elf_big_vec;
    break;
  case ELFDATA2LSB:			/* Little-endian */
    abfd->xvec = &elf_little_vec;
    break;
  case ELFDATANONE:			/* No data encoding specified */
  default:				/* Unknown data encoding specified */
    goto wrong;
  }
  
  /* Allocate an instance of the elf_obj_tdata structure and hook it up to
     the tdata pointer in the bfd. */

  if ((abfd -> tdata.elf_obj_data = 
       (elf_obj_tdata*) bfd_zalloc (abfd, sizeof (elf_obj_tdata))) 
      == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }

  /* Now that we know the byte order, swap in the rest of the header */
  elf_swap_ehdr_in (abfd, &x_ehdr, &i_ehdr);

  /* If there is no section header table, we're hosed. */
  if (i_ehdr.e_shoff == 0)
    goto wrong;

  if (i_ehdr.e_type == ET_EXEC || i_ehdr.e_type == ET_DYN)
    {
      abfd -> flags |= EXEC_P;
    }

  /* Allocate space for copies of the section header table in external
     and internal form, seek to the section header table in the file,
     read it in, and convert it to internal form.  As a simple sanity
     check, verify that the what BFD thinks is the size of each section
     header table entry actually matches the size recorded in the file. */

  if (i_ehdr.e_shentsize != sizeof (*x_shdr))
    goto wrong;
  if ((x_shdr = (Elf_External_Shdr *)
	bfd_alloc (abfd, sizeof (*x_shdr) * i_ehdr.e_shnum)) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  if ((i_shdr = (Elf_Internal_Shdr *)
	bfd_alloc (abfd, sizeof (*i_shdr) * i_ehdr.e_shnum)) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  if (bfd_seek (abfd, i_ehdr.e_shoff, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (NULL);
    }
  for (shindex = 0; shindex < i_ehdr.e_shnum; shindex++)
    {
      if (bfd_read ((PTR) (x_shdr + shindex), sizeof (*x_shdr), 1, abfd)
	  != sizeof (*x_shdr))
	{
	  bfd_error = system_call_error;
	  return (NULL);
	}
      elf_swap_shdr_in (abfd, x_shdr + shindex, i_shdr + shindex);
    }

  /* Read in the string table containing the names of the sections.  We
     will need the base pointer to this table later. */

  shstrtabsize = i_shdr[i_ehdr.e_shstrndx].sh_size;
  offset = i_shdr[i_ehdr.e_shstrndx].sh_offset;
  if ((shstrtab = elf_read (abfd, offset, shstrtabsize)) == NULL)
    {
      return (NULL);
    }

  /* Once all of the section headers have been read and converted, we
     can start processing them.  Note that the first section header is
     a dummy placeholder entry, so we ignore it.

     We also watch for the symbol table section and remember the file
     offset and section size for both the symbol table section and the
     associated string table section. */

  for (shindex = 1; shindex < i_ehdr.e_shnum; shindex++)
    {
      Elf_Internal_Shdr *hdr = i_shdr + shindex;
      bfd_section_from_shdr (abfd, hdr, shstrtab);
      if (hdr -> sh_type == SHT_SYMTAB)
	{
	  elf_symtab_filepos(abfd) = hdr -> sh_offset;
	  elf_symtab_filesz(abfd) = hdr -> sh_size;
	  elf_strtab_filepos(abfd) = (i_shdr + hdr -> sh_link) -> sh_offset;
	  elf_strtab_filesz(abfd) = (i_shdr + hdr -> sh_link) -> sh_size;
	}
    }

  /* Remember the entry point specified in the ELF file header. */

  bfd_get_start_address (abfd) = i_ehdr.e_entry;

  return (abfd->xvec);
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

static bfd_target *
DEFUN (elf_core_file_p, (abfd), bfd *abfd)
{
  Elf_External_Ehdr x_ehdr;	/* Elf file header, external form */
  Elf_Internal_Ehdr i_ehdr;	/* Elf file header, internal form */
  Elf_External_Phdr *x_phdr;	/* Program header table, external form */
  Elf_Internal_Phdr *i_phdr;	/* Program header table, internal form */
  int phindex;
  
  /* Read in the ELF header in external format.  */

  if (bfd_read ((PTR) &x_ehdr, sizeof (x_ehdr), 1, abfd) != sizeof (x_ehdr))
    {
      bfd_error = system_call_error;
      return (NULL);
    }

  /* Now check to see if we have a valid ELF file, and one that BFD can
     make use of.  The magic number must match, the address size ('class')
     and byte-swapping must match our XVEC entry, and it must have a
     program header table (FIXME: See comments re segments at top of this
     file). */

  if (x_ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
      x_ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
      x_ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
      x_ehdr.e_ident[EI_MAG3] != ELFMAG3)
    {
wrong:
      bfd_error = wrong_format;
      return (NULL);
    }

  /* FIXME, Check EI_VERSION here !  */

  switch (x_ehdr.e_ident[EI_CLASS]) {
  case ELFCLASSNONE:			/* address size not specified */
    goto wrong;			/* No support if can't tell address size */
  case ELFCLASS32:			/* 32-bit addresses */
    break;
  case ELFCLASS64:			/* 64-bit addresses */
    goto wrong;			/* FIXME: 64 bits not yet supported */
  default:
    goto wrong;			/* No support if unknown address class */
  }

  /* Switch xvec to match the specified byte order.  */
  switch (x_ehdr.e_ident[EI_DATA]) {
  case ELFDATA2MSB:			/* Big-endian */ 
    abfd->xvec = &elf_big_vec;
    break;
  case ELFDATA2LSB:			/* Little-endian */
    abfd->xvec = &elf_little_vec;
    break;
  case ELFDATANONE:			/* No data encoding specified */
  default:				/* Unknown data encoding specified */
    goto wrong;
  }
  
  /* Now that we know the byte order, swap in the rest of the header */
  elf_swap_ehdr_in (abfd, &x_ehdr, &i_ehdr);

  /* If there is no program header, or the type is not a core file, then
     we are hosed. */
  if (i_ehdr.e_phoff == 0 || i_ehdr.e_type != ET_CORE)
    goto wrong;

  /* Allocate an instance of the elf_core_tdata structure and hook it up to
     the tdata pointer in the bfd. */

  if ((abfd -> tdata.elf_core_data =
       (elf_core_tdata *) bfd_zalloc (abfd, sizeof (elf_core_tdata))) 
      == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }

  /* Allocate space for copies of the program header table in external
     and internal form, seek to the program header table in the file,
     read it in, and convert it to internal form.  As a simple sanity
     check, verify that the what BFD thinks is the size of each program
     header table entry actually matches the size recorded in the file. */

  if (i_ehdr.e_phentsize != sizeof (*x_phdr))
    goto wrong;
  if ((x_phdr = (Elf_External_Phdr *)
	bfd_alloc (abfd, sizeof (*x_phdr) * i_ehdr.e_phnum)) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  if ((i_phdr = (Elf_Internal_Phdr *)
	bfd_alloc (abfd, sizeof (*i_phdr) * i_ehdr.e_phnum)) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  if (bfd_seek (abfd, i_ehdr.e_phoff, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (NULL);
    }
  for (phindex = 0; phindex < i_ehdr.e_phnum; phindex++)
    {
      if (bfd_read ((PTR) (x_phdr + phindex), sizeof (*x_phdr), 1, abfd)
	  != sizeof (*x_phdr))
	{
	  bfd_error = system_call_error;
	  return (NULL);
	}
      elf_swap_phdr_in (abfd, x_phdr + phindex, i_phdr + phindex);
    }

  /* Once all of the program headers have been read and converted, we
     can start processing them. */

  for (phindex = 0; phindex < i_ehdr.e_phnum; phindex++)
    {
      bfd_section_from_phdr (abfd, i_phdr + phindex, phindex);
      if ((i_phdr + phindex) -> p_type == PT_NOTE)
	{
	  elf_corefile_note (abfd, i_phdr + phindex);
	}
    }

  /* Remember the entry point specified in the ELF file header. */

  bfd_get_start_address (abfd) = i_ehdr.e_entry;

  return (abfd->xvec);
}

static boolean
DEFUN (elf_mkobject, (abfd), bfd *abfd)
{
  fprintf (stderr, "elf_mkobject unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static boolean
DEFUN (elf_write_object_contents, (abfd), bfd *abfd)
{
  fprintf (stderr, "elf_write_object_contents unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

/* Given an index of a section, retrieve a pointer to it.  Note
   that for our purposes, sections are indexed by {1, 2, ...} with
   0 being an illegal index. */

static struct sec *
DEFUN (section_from_bfd_index, (abfd, index),
       bfd            *abfd AND
       int             index)
{
  if (index > 0)
    {
      struct sec *answer = abfd -> sections;
      while (--index > 0)
	{
	  answer = answer -> next;
	}
      return (answer);
    }
  return (NULL);
}

static boolean
DEFUN (elf_slurp_symbol_table, (abfd), bfd *abfd)
{
  int symcount;		/* Number of external ELF symbols */
  char *strtab;		/* Buffer for raw ELF string table section */
  asymbol *sym;		/* Pointer to current bfd symbol */
  asymbol *symbase;	/* Buffer for generated bfd symbols */
  asymbol **vec;	/* Pointer to current bfd symbol pointer */
  Elf_Internal_Sym i_sym;
  Elf_External_Sym x_sym;

  if (bfd_get_outsymbols (abfd) != NULL)
    {
      return (true);
    }

  /* Slurp in the string table.  We will keep it around permanently, as
     long as the bfd is in use, since we will end up setting up pointers
     into it for the names of all the symbols. */

  strtab = elf_read (abfd, elf_strtab_filepos(abfd), elf_strtab_filesz(abfd));
  if (strtab == NULL)
    {
      return (false);
    }

  /* Read each raw ELF symbol, converting from external ELF form to
     internal ELF form, and then using the information to create a
     canonical bfd symbol table entry.

     Note that be allocate the initial bfd canonical symbol buffer
     based on a one-to-one mapping of the ELF symbols to canonical
     symbols.  However, it is likely that not all the ELF symbols will
     be used, so there will be some space leftover at the end.  Once
     we know how many symbols we actual generate, we realloc the buffer
     to the correct size and then build the pointer vector. */

  if (bfd_seek (abfd, elf_symtab_filepos (abfd), SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  symcount = elf_symtab_filesz(abfd) / sizeof (Elf_External_Sym);
  sym = symbase = (asymbol *) bfd_zalloc (abfd, symcount * sizeof (asymbol));

  while (symcount-- > 0)
    {
      if (bfd_read ((PTR) &x_sym, sizeof (x_sym), 1, abfd) != sizeof (x_sym))
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      elf_swap_symbol_in (abfd, &x_sym, &i_sym);
      if (i_sym.st_name > 0)
	{
	  sym -> the_bfd = abfd;
	  sym -> name = strtab + i_sym.st_name;
	  sym -> value = i_sym.st_value;
	  if (i_sym.st_shndx > 0 && i_sym.st_shndx < SHN_LORESERV)
	    {
	      /* Note:  This code depends upon there being an ordered
		 one-for-one mapping of ELF sections to bfd sections. */
	      sym -> section = section_from_bfd_index (abfd, i_sym.st_shndx);
	    }
	  else if (i_sym.st_shndx == SHN_ABS)
	    {
/*	      sym -> flags |= BSF_ABSOLUTE; OBSOLETE */
	    }
	  else if (i_sym.st_shndx == SHN_COMMON)
	    {
	      sym -> section = &bfd_com_section;
	    }
	  switch (ELF_ST_BIND (i_sym.st_info))
	    {
	      case STB_LOCAL:
		sym -> flags |= BSF_LOCAL;
	        break;
	      case STB_GLOBAL:
	        sym -> flags |= (BSF_GLOBAL | BSF_EXPORT);
	        break;
	      case STB_WEAK:
		sym -> flags |= BSF_WEAK;
	        break;
	    }
	  sym++;
	}
    }

  bfd_get_symcount(abfd) = symcount = sym - symbase;
  sym = symbase = (asymbol *)
    bfd_realloc (abfd, symbase, symcount * sizeof (asymbol));
  bfd_get_outsymbols(abfd) = vec = (asymbol **)
    bfd_alloc (abfd, symcount * sizeof (asymbol *));

  while (symcount-- > 0)
    {
      *vec++ = sym++;
    }

  return (true);
}

/* Return the number of bytes required to hold the symtab vector.

   Note that we base it on the count plus 1, since we will null terminate
   the vector allocated based on this size. */

static unsigned int
DEFUN (elf_get_symtab_upper_bound, (abfd), bfd *abfd)
{
  unsigned int symtab_size = 0;

  if (elf_slurp_symbol_table (abfd))
    {
      symtab_size = (bfd_get_symcount (abfd) + 1) * (sizeof (asymbol));
    }
  return (symtab_size);
}

static unsigned int
elf_get_reloc_upper_bound (abfd, asect)
bfd            *abfd;
sec_ptr         asect;
{
  fprintf (stderr, "elf_get_reloc_upper_bound unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static unsigned int
elf_canonicalize_reloc (abfd, section, relptr, symbols)
bfd            *abfd;
sec_ptr         section;
arelent       **relptr;
asymbol       **symbols;
{
  fprintf (stderr, "elf_canonicalize_reloc unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

static unsigned int
DEFUN (elf_get_symtab, (abfd, alocation),
       bfd            *abfd AND
       asymbol       **alocation)
{
  unsigned int symcount;
  asymbol **vec;

  if (!elf_slurp_symbol_table (abfd))
    {
      return (0);
    }
  else
    {
      symcount = bfd_get_symcount (abfd);
      vec = bfd_get_outsymbols (abfd);
      while (symcount-- > 0)
	{
	  *alocation++ = *vec++;
	}
      *alocation++ = NULL;
      return (bfd_get_symcount (abfd));
    }
}

static asymbol *
DEFUN (elf_make_empty_symbol, (abfd),
       bfd *abfd)
{
  elf_symbol_type *new;

  new = (elf_symbol_type *) bfd_zalloc (abfd, sizeof (elf_symbol_type));
  if (new == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }
  else
    {
      new -> symbol.the_bfd = abfd;
      return (&new -> symbol);
    }
}

static void 
DEFUN (elf_print_symbol,(ignore_abfd, filep, symbol, how),
      bfd            *ignore_abfd AND
      PTR           filep AND
      asymbol        *symbol AND
      bfd_print_symbol_type how)
{
  fprintf (stderr, "elf_print_symbol unimplemented\n");
  fflush (stderr);
  abort ();
}

static alent *
DEFUN (elf_get_lineno,(ignore_abfd, symbol),
      bfd            *ignore_abfd AND
      asymbol        *symbol)
{
  fprintf (stderr, "elf_get_lineno unimplemented\n");
  fflush (stderr);
  abort ();
  return (NULL);
}

static boolean
DEFUN (elf_set_arch_mach,(abfd, arch, machine),
      bfd            *abfd AND
      enum bfd_architecture arch AND
      unsigned long   machine)
{
  fprintf (stderr, "elf_set_arch_mach unimplemented\n");
  fflush (stderr);
  /* Allow any architecture to be supported by the elf backend */
  return  bfd_default_set_arch_mach(abfd, arch, machine);
}

static boolean
DEFUN (elf_find_nearest_line,(abfd,
			      section,
			      symbols,
			      offset,
			      filename_ptr,
			      functionname_ptr,
			      line_ptr),
      bfd            *abfd AND
      asection       *section AND
      asymbol       **symbols AND
      bfd_vma         offset AND
      CONST char      **filename_ptr AND
      CONST char       **functionname_ptr AND
      unsigned int   *line_ptr)
{
  fprintf (stderr, "elf_find_nearest_line unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static int 
DEFUN (elf_sizeof_headers, (abfd, reloc),
      bfd *abfd AND
      boolean reloc)
{
  fprintf (stderr, "elf_sizeof_headers unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

/* This structure contains everything that BFD knows about a target.
   It includes things like its byte order, name, what routines to call
   to do various operations, etc.  Every BFD points to a target structure
   with its "xvec" member.

   There are two such structures here:  one for big-endian machines and
   one for little-endian machines.   */

/* Archives are generic or unimplemented.  */
#define elf_slurp_armap			bfd_false
#define elf_slurp_extended_name_table	_bfd_slurp_extended_name_table
#define elf_truncate_arname		bfd_dont_truncate_arname
#define elf_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define elf_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	elf_write_armap			(PROTO (boolean, (*),		\
     (bfd *arch, unsigned int elength, struct orl *map, unsigned int orl_count,	\
      int stridx))) bfd_false

/* Ordinary section reading and writing */
#define elf_new_section_hook		_bfd_dummy_new_section_hook
#define	elf_get_section_contents	bfd_generic_get_section_contents
#define	elf_set_section_contents	bfd_generic_set_section_contents
#define	elf_close_and_cleanup		bfd_generic_close_and_cleanup

#define elf_bfd_debug_info_start	bfd_void
#define elf_bfd_debug_info_end		bfd_void
#define elf_bfd_debug_info_accumulate	(PROTO(void,(*),(bfd*, struct sec *))) bfd_void
#define elf_bfd_get_relocated_section_contents \
 bfd_generic_get_relocated_section_contents
#define elf_bfd_relax_section bfd_generic_relax_section
bfd_target elf_big_vec =
{
  /* name: identify kind of target */
  "elf-big",

  /* flavour: general indication about file */
  bfd_target_elf_flavour,

  /* byteorder_big_p: data is big endian */
  true,

  /* header_byteorder_big_p: header is also big endian */
  true,

  /* object_flags: mask of all file flags */
  (HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS |
   DYNAMIC | WP_TEXT),
  
  /* section_flags: mask of all section flags */
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY |
   SEC_DATA), 

  /* ar_pad_char: pad character for filenames within an archive header
     FIXME:  this really has nothing to do with ELF, this is a characteristic
     of the archiver and/or os and should be independently tunable */
  '/',

  /* ar_max_namelen: maximum number of characters in an archive header
     FIXME:  this really has nothing to do with ELF, this is a characteristic
     of the archiver and should be independently tunable.  This value is
     a WAG (wild a** guess) */
  15,

  /* align_power_min: minimum alignment restriction for any section
     FIXME:  this value may be target machine dependent */
  3,

  /* Routines to byte-swap various sized integers from the data sections */
  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16,

  /* Routines to byte-swap various sized integers from the file headers */
  _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16,

  /* bfd_check_format: check the format of a file being read */
  { _bfd_dummy_target,		/* unknown format */
    elf_object_p,		/* assembler/linker output (object file) */
    bfd_generic_archive_p,	/* an archive */
    elf_core_file_p		/* a core file */
  },

  /* bfd_set_format: set the format of a file being written */
  { bfd_false,
    elf_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },

  /* bfd_write_contents: write cached information into a file being written */
  { bfd_false,
    elf_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false
  },

  /* Initialize a jump table with the standard macro.  All names start
     with "elf" */
  JUMP_TABLE(elf),

  /* SWAP_TABLE */
  NULL, NULL, NULL
};

bfd_target elf_little_vec =
{
  /* name: identify kind of target */
  "elf-little",

  /* flavour: general indication about file */
  bfd_target_elf_flavour,

  /* byteorder_big_p: data is big endian */
  false,		/* Nope -- this one's little endian */

  /* header_byteorder_big_p: header is also big endian */
  false,		/* Nope -- this one's little endian */

  /* object_flags: mask of all file flags */
  (HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS |
   DYNAMIC | WP_TEXT),
  
  /* section_flags: mask of all section flags */
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY |
   SEC_DATA), 

  /* ar_pad_char: pad character for filenames within an archive header
     FIXME:  this really has nothing to do with ELF, this is a characteristic
     of the archiver and/or os and should be independently tunable */
  '/',

  /* ar_max_namelen: maximum number of characters in an archive header
     FIXME:  this really has nothing to do with ELF, this is a characteristic
     of the archiver and should be independently tunable.  This value is
     a WAG (wild a** guess) */
  15,

  /* align_power_min: minimum alignment restriction for any section
     FIXME:  this value may be target machine dependent */
  3,

  /* Routines to byte-swap various sized integers from the data sections */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16,

  /* Routines to byte-swap various sized integers from the file headers */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16,

  /* bfd_check_format: check the format of a file being read */
  { _bfd_dummy_target,		/* unknown format */
    elf_object_p,		/* assembler/linker output (object file) */
    bfd_generic_archive_p,	/* an archive */
    elf_core_file_p		/* a core file */
  },

  /* bfd_set_format: set the format of a file being written */
  { bfd_false,
    elf_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },

  /* bfd_write_contents: write cached information into a file being written */
  { bfd_false,
    elf_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false
  },

  /* Initialize a jump table with the standard macro.  All names start
     with "elf" */
  JUMP_TABLE(elf),

  /* SWAP_TABLE */
  NULL, NULL, NULL
};
