/* BFD back-end for CISCO crash dumps.

Copyright 1994 Free Software Foundation, Inc.

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
#include "libbfd.h"

#define CRASH_INFO (0xffc)
#define CRASH_MAGIC 0xdead1234

typedef enum {
    CRASH_REASON_NOTCRASHED = 0,
    CRASH_REASON_EXCEPTION = 1,
    CRASH_REASON_CORRUPT = 2,
  } crashreason;

struct crashinfo_external
{
  char magic[4];			/* Magic number */
  char version[4];		/* Version number */
  char reason[4];		/* Crash reason */
  char cpu_vector[4];		/* CPU vector for exceptions */
  char registers[4];		/* Pointer to saved registers */
  char rambase[4];		/* Base of RAM (not in V1 crash info) */
};

struct cisco_core_struct
{
  int sig;
};

static bfd_target *
cisco_core_file_p (abfd)
     bfd *abfd;
{
  char buf[4];
  unsigned int crashinfo_offset;
  struct crashinfo_external crashinfo;
  int nread;
  unsigned int rambase;
  sec_ptr asect;
  struct stat statbuf;

  if (bfd_seek (abfd, CRASH_INFO, SEEK_SET) != 0)
    return NULL;

  nread = bfd_read (buf, 1, 4, abfd);
  if (nread != 4)
    {
      /* Maybe the file is too small (FIXME: what about other errors).  */
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }
  crashinfo_offset = bfd_get_32 (abfd, buf);

  if (bfd_seek (abfd, crashinfo_offset, SEEK_SET) != 0)
    return NULL;

  nread = bfd_read (&crashinfo, 1, sizeof (crashinfo), abfd);
  if (nread != sizeof (crashinfo))
    {
      /* Maybe the file is too small (FIXME: what about other errors).  */
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  if (bfd_stat (abfd, &statbuf) < 0)
    {
      bfd_set_error (bfd_error_system_call);
      return NULL;
    }

  if (bfd_get_32 (abfd, crashinfo.magic) != CRASH_MAGIC)
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  switch (bfd_get_32 (abfd, crashinfo.version))
    {
    case 0:
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    case 1:
      rambase = 0;
      break;
    default:
    case 2:
      rambase = bfd_get_32 (abfd, crashinfo.rambase);
      break;
    }

  /* OK, we believe you.  You're a core file.  */

  abfd->tdata.cisco_core_data = (struct cisco_core_struct *)
    bfd_zmalloc (abfd, sizeof (struct cisco_core_struct));
  if (abfd->tdata.cisco_core_data == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return NULL;
    }

  switch ((crashreason) bfd_get_32 (abfd, crashinfo.reason))
    {
    case CRASH_REASON_NOTCRASHED:
      /* Crash file probably came from write core.  */
      abfd->tdata.cisco_core_data->sig = 0;
      break;
    case CRASH_REASON_CORRUPT:
      /* The crash context area was corrupt -- proceed with caution.
	 We have no way of passing this information back to the caller.  */
      abfd->tdata.cisco_core_data->sig = 0;
      break;
    case CRASH_REASON_EXCEPTION:
      /* Crash occured due to CPU exception.
	 FIXME: convert cpu_vector to a signal number.  */
      abfd->tdata.cisco_core_data->sig = 0;
      break;
    default:
      /* Unknown crash reason.  */
      abfd->tdata.cisco_core_data->sig = 0;
      break;
    }

  abfd->sections = NULL;
  abfd->section_count = 0;

  asect = (asection *) bfd_zmalloc (sizeof (asection));
  if (asect == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  asect->name = ".reg";
  asect->flags = SEC_ALLOC | SEC_HAS_CONTENTS;
  /* This can be bigger than the real size.  Set it to the size of the whole
     core file.  */
  asect->_raw_size = statbuf.st_size;
  asect->vma = 0;
  asect->filepos = bfd_get_32 (abfd, crashinfo.registers) - rambase;
  asect->next = abfd->sections;
  abfd->sections = asect;
  ++abfd->section_count;

  /* There is only one section containing data from the target system's RAM.
     We call it .data.  */
  asect = (asection *) bfd_zmalloc (sizeof (asection));
  if (asect == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      goto error_return;
    }
  asect->name = ".data";
  asect->flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
  /* The size of memory is the size of the core file itself.  */
  asect->_raw_size = statbuf.st_size;
  asect->vma = rambase;
  asect->filepos = 0;
  asect->next = abfd->sections;
  abfd->sections = asect;
  ++abfd->section_count;

  return abfd->xvec;

 error_return:
  {
    sec_ptr nextsect;
    for (asect = abfd->sections; asect != NULL;)
      {
	nextsect = asect->next;
	free (asect);
	asect = nextsect;
      }
    free (abfd->tdata);
    return NULL;
  }
}

char *
cisco_core_file_failing_command (abfd)
     bfd *abfd;
{
  return NULL;
}

int
cisco_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return abfd->tdata.cisco_core_data->sig;
}

boolean
cisco_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd;
     bfd *exec_bfd;
{
  return true;
}

/* No archive file support via this BFD */
#define	cisco_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	cisco_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	cisco_slurp_armap			bfd_false
#define	cisco_slurp_extended_name_table	bfd_true
#define	cisco_write_armap			(boolean (*) PARAMS	\
    ((bfd *arch, unsigned int elength, struct orl *map, \
      unsigned int orl_count, int stridx))) bfd_false
#define	cisco_truncate_arname		bfd_dont_truncate_arname
#define	aout_32_openr_next_archived_file	bfd_generic_openr_next_archived_file

#define	cisco_close_and_cleanup		bfd_generic_close_and_cleanup
#define	cisco_set_section_contents		(boolean (*) PARAMS	\
        ((bfd *abfd, asection *section, PTR data, file_ptr offset,	\
        bfd_size_type count))) bfd_generic_set_section_contents
#define	cisco_get_section_contents		bfd_generic_get_section_contents
#define	cisco_new_section_hook		(boolean (*) PARAMS	\
	((bfd *, sec_ptr))) bfd_true
#define	cisco_get_symtab_upper_bound	bfd_0u
#define	cisco_get_symtab			(unsigned int (*) PARAMS \
        ((bfd *, struct symbol_cache_entry **))) bfd_0u
#define	cisco_get_reloc_upper_bound		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr))) bfd_0u
#define	cisco_canonicalize_reloc		(unsigned int (*) PARAMS \
	((bfd *, sec_ptr, arelent **, struct symbol_cache_entry**))) bfd_0u
#define	cisco_make_empty_symbol		(struct symbol_cache_entry * \
	(*) PARAMS ((bfd *))) bfd_false
#define	cisco_print_symbol			(void (*) PARAMS	\
	((bfd *, PTR, struct symbol_cache_entry  *,			\
	bfd_print_symbol_type))) bfd_false
#define	cisco_get_symbol_info		(void (*) PARAMS	\
	((bfd *, struct symbol_cache_entry  *,			\
	symbol_info *))) bfd_false
#define	cisco_get_lineno			(alent * (*) PARAMS	\
	((bfd *, struct symbol_cache_entry *))) bfd_nullvoidptr
#define	cisco_set_arch_mach			(boolean (*) PARAMS	\
	((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define	cisco_find_nearest_line		(boolean (*) PARAMS	\
        ((bfd *abfd, struct sec  *section,				\
         struct symbol_cache_entry  **symbols,bfd_vma offset,		\
         CONST char **file, CONST char **func, unsigned int *line))) bfd_false
#define	cisco_sizeof_headers		(int (*) PARAMS	\
	((bfd *, boolean))) bfd_0

#define cisco_bfd_debug_info_start		bfd_void
#define cisco_bfd_debug_info_end		bfd_void
#define cisco_bfd_debug_info_accumulate	(void (*) PARAMS	\
	((bfd *, struct sec *))) bfd_void
#define cisco_bfd_get_relocated_section_contents bfd_generic_get_relocated_section_contents
#define cisco_bfd_relax_section		bfd_generic_relax_section
#define cisco_bfd_reloc_type_lookup \
  ((CONST struct reloc_howto_struct *(*) PARAMS ((bfd *, bfd_reloc_code_real_type))) bfd_nullvoidptr)
#define cisco_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)
#define cisco_bfd_link_hash_table_create \
  ((struct bfd_link_hash_table *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define cisco_bfd_link_add_symbols \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define cisco_bfd_final_link \
  ((boolean (*) PARAMS ((bfd *, struct bfd_link_info *))) bfd_false)
#define cisco_bfd_copy_private_section_data \
  ((boolean (*) PARAMS ((bfd *, asection *, bfd *, asection *))) bfd_false)
#define cisco_bfd_copy_private_bfd_data \
  ((boolean (*) PARAMS ((bfd *, bfd *))) bfd_false)
#define cisco_bfd_is_local_label \
  ((boolean (*) PARAMS ((bfd *, asection *))) bfd_false)

/* If somebody calls any byte-swapping routines, shoot them.  */
void
swap_abort()
{
  abort(); /* This way doesn't require any declaration for ANSI to fuck up */
}
#define	NO_GET	((bfd_vma (*) PARAMS ((   const bfd_byte *))) swap_abort )
#define	NO_PUT	((void    (*) PARAMS ((bfd_vma, bfd_byte *))) swap_abort )
#define	NO_SIGNED_GET \
  ((bfd_signed_vma (*) PARAMS ((const bfd_byte *))) swap_abort )

bfd_target trad_core_vec =
  {
    "trad-core",
    bfd_target_unknown_flavour,
    true,			/* target byte order */
    true,			/* target headers byte order */
    (HAS_RELOC | EXEC_P |	/* object flags */
     HAS_LINENO | HAS_DEBUG |
     HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
    (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
    0,			                                   /* symbol prefix */
    ' ',						   /* ar_pad_char */
    16,							   /* ar_max_namelen */
    3,							   /* minimum alignment power */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 64 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 32 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 16 bit data */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 64 bit hdrs */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 32 bit hdrs */
    NO_GET, NO_SIGNED_GET, NO_PUT,	/* 16 bit hdrs */

    {				/* bfd_check_format */
     _bfd_dummy_target,		/* unknown format */
     _bfd_dummy_target,		/* object file */
     _bfd_dummy_target,		/* archive */
     cisco_core_file_p	/* a core file */
    },
    {				/* bfd_set_format */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    {				/* bfd_write_contents */
     bfd_false, bfd_false,
     bfd_false, bfd_false
    },
    
    JUMP_TABLE(cisco),
    (PTR) 0			/* backend_data */
};
