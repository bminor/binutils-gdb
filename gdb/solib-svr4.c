/* Handle SunOS and SVR4 shared libraries for GDB, the GNU Debugger.
   Copyright 1990, 91, 92, 93, 94, 95, 96, 98, 1999, 2000
   Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#define _SYSCALL32	/* for Sparc64 cross Sparc32 */
#include "defs.h"


#include <sys/types.h>
#include <signal.h>
#include "gdb_string.h"
#include <sys/param.h>
#include <fcntl.h>

#ifndef SVR4_SHARED_LIBS
 /* SunOS shared libs need the nlist structure.  */
#include <a.out.h>
#else
#include "elf/external.h"
#endif

#ifdef HAVE_LINK_H
#include <link.h>
#endif

#include "symtab.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbcore.h"
#include "command.h"
#include "target.h"
#include "frame.h"
#include "gdb_regex.h"
#include "inferior.h"
#include "environ.h"
#include "language.h"
#include "gdbcmd.h"

#include "solist.h"
#include "solib-svr4.h"

/* Link map info to include in an allocated so_list entry */

struct lm_info
  {
    /* Pointer to copy of link map from inferior.  The type is char *
       rather than void *, so that we may use byte offsets to find the
       various fields without the need for a cast.  */
    char *lm;
  };

/* On SVR4 systems, a list of symbols in the dynamic linker where
   GDB can try to place a breakpoint to monitor shared library
   events.

   If none of these symbols are found, or other errors occur, then
   SVR4 systems will fall back to using a symbol as the "startup
   mapping complete" breakpoint address.  */

#ifdef SVR4_SHARED_LIBS
static char *solib_break_names[] =
{
  "r_debug_state",
  "_r_debug_state",
  "_dl_debug_state",
  "rtld_db_dlactivity",
  NULL
};
#endif

#define BKPT_AT_SYMBOL 1

#if defined (BKPT_AT_SYMBOL) && defined (SVR4_SHARED_LIBS)
static char *bkpt_names[] =
{
#ifdef SOLIB_BKPT_NAME
  SOLIB_BKPT_NAME,		/* Prefer configured name if it exists. */
#endif
  "_start",
  "main",
  NULL
};
#endif

/* Symbols which are used to locate the base of the link map structures. */

#ifndef SVR4_SHARED_LIBS
static char *debug_base_symbols[] =
{
  "_DYNAMIC",
  "_DYNAMIC__MGC",
  NULL
};
#endif

static char *main_name_list[] =
{
  "main_$main",
  NULL
};


/* Fetch (and possibly build) an appropriate link_map_offsets structure
   for native targets using struct definitions from link.h.  */

struct link_map_offsets *
default_svr4_fetch_link_map_offsets (void)
{
#ifdef HAVE_LINK_H
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = 0;
#if defined (HAVE_STRUCT_LINK_MAP32)
  static struct link_map_offsets lmo32;
  static struct link_map_offsets *lmp32 = 0;
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif
#define fieldsize(TYPE, MEMBER) (sizeof (((TYPE *)0)->MEMBER))

  if (lmp == 0)
    {
      lmp = &lmo;

#ifdef SVR4_SHARED_LIBS
      lmo.r_debug_size = sizeof (struct r_debug);

      lmo.r_map_offset = offsetof (struct r_debug, r_map);
      lmo.r_map_size = fieldsize (struct r_debug, r_map);

      lmo.link_map_size = sizeof (struct link_map);

      lmo.l_addr_offset = offsetof (struct link_map, l_addr);
      lmo.l_addr_size = fieldsize (struct link_map, l_addr);

      lmo.l_next_offset = offsetof (struct link_map, l_next);
      lmo.l_next_size = fieldsize (struct link_map, l_next);

      lmo.l_prev_offset = offsetof (struct link_map, l_prev);
      lmo.l_prev_size = fieldsize (struct link_map, l_prev);

      lmo.l_name_offset = offsetof (struct link_map, l_name);
      lmo.l_name_size = fieldsize (struct link_map, l_name);
#else /* !SVR4_SHARED_LIBS */
      lmo.link_map_size = sizeof (struct link_map);

      lmo.l_addr_offset = offsetof (struct link_map, lm_addr);
      lmo.l_addr_size = fieldsize (struct link_map, lm_addr);

      lmo.l_next_offset = offsetof (struct link_map, lm_next);
      lmo.l_next_size = fieldsize (struct link_map, lm_next);

      lmo.l_name_offset = offsetof (struct link_map, lm_name);
      lmo.l_name_size = fieldsize (struct link_map, lm_name);
#endif /* SVR4_SHARED_LIBS */
    }

#if defined (HAVE_STRUCT_LINK_MAP32)
  if (lmp32 == 0)
    {
      lmp32 = &lmo32;

      lmo32.r_debug_size = sizeof (struct r_debug32);

      lmo32.r_map_offset = offsetof (struct r_debug32, r_map);
      lmo32.r_map_size = fieldsize (struct r_debug32, r_map);

      lmo32.link_map_size = sizeof (struct link_map32);

      lmo32.l_addr_offset = offsetof (struct link_map32, l_addr);
      lmo32.l_addr_size = fieldsize (struct link_map32, l_addr);

      lmo32.l_next_offset = offsetof (struct link_map32, l_next);
      lmo32.l_next_size = fieldsize (struct link_map32, l_next);

      lmo32.l_prev_offset = offsetof (struct link_map32, l_prev);
      lmo32.l_prev_size = fieldsize (struct link_map32, l_prev);

      lmo32.l_name_offset = offsetof (struct link_map32, l_name);
      lmo32.l_name_size = fieldsize (struct link_map32, l_name);
    }
#endif /* defined (HAVE_STRUCT_LINK_MAP32) */

#if defined (HAVE_STRUCT_LINK_MAP32)
  if (bfd_get_arch_size (exec_bfd) == 32)
    return lmp32;
  else
#endif
    return lmp;

#else

  internal_error ("default_svr4_fetch_link_map_offsets called without HAVE_LINK_H defined.");
  return 0;

#endif /* HAVE_LINK_H */
}

/* Macro to extract an address from a solib structure.
   When GDB is configured for some 32-bit targets (e.g. Solaris 2.7
   sparc), BFD is configured to handle 64-bit targets, so CORE_ADDR is
   64 bits.  We have to extract only the significant bits of addresses
   to get the right address when accessing the core file BFD.  */

#define SOLIB_EXTRACT_ADDRESS(MEMBER) \
	extract_address (&(MEMBER), sizeof (MEMBER))

/* local data declarations */

#ifndef SVR4_SHARED_LIBS

/* NOTE: converted the macros LM_ADDR, LM_NEXT, LM_NAME and
   IGNORE_FIRST_LINK_MAP_ENTRY into functions (see below).
   MVS, June 2000  */

static struct link_dynamic dynamic_copy;
static struct link_dynamic_2 ld_2_copy;
static struct ld_debug debug_copy;
static CORE_ADDR debug_addr;
static CORE_ADDR flag_addr;

#endif /* !SVR4_SHARED_LIBS */

/* link map access functions */

static CORE_ADDR
LM_ADDR (struct so_list *so)
{
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();

  return extract_address (so->lm_info->lm + lmo->l_addr_offset, lmo->l_addr_size);
}

static CORE_ADDR
LM_NEXT (struct so_list *so)
{
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();

  return extract_address (so->lm_info->lm + lmo->l_next_offset, lmo->l_next_size);
}

static CORE_ADDR
LM_NAME (struct so_list *so)
{
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();

  return extract_address (so->lm_info->lm + lmo->l_name_offset, lmo->l_name_size);
}

#ifndef SVR4_SHARED_LIBS

static int 
IGNORE_FIRST_LINK_MAP_ENTRY (struct so_list *so)
{
  return 0;
}

#else /* SVR4_SHARED_LIBS */

static int
IGNORE_FIRST_LINK_MAP_ENTRY (struct so_list *so)
{
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();

  return extract_address (so->lm_info->lm + lmo->l_prev_offset,
                          lmo->l_prev_size) == 0;
}

#endif /* !SVR4_SHARED_LIBS */


static CORE_ADDR debug_base;	/* Base of dynamic linker structures */
static CORE_ADDR breakpoint_addr;	/* Address where end bkpt is set */

/* Local function prototypes */

static int match_main (char *);

/* If non-zero, this is a prefix that will be added to the front of the name
   shared libraries with an absolute filename for loading.  */
static char *solib_absolute_prefix = NULL;

/* If non-empty, this is a search path for loading non-absolute shared library
   symbol files.  This takes precedence over the environment variables PATH
   and LD_LIBRARY_PATH.  */
static char *solib_search_path = NULL;


#ifndef SVR4_SHARED_LIBS

/* Allocate the runtime common object file.  */

static void
allocate_rt_common_objfile (void)
{
  struct objfile *objfile;
  struct objfile *last_one;

  objfile = (struct objfile *) xmalloc (sizeof (struct objfile));
  memset (objfile, 0, sizeof (struct objfile));
  objfile->md = NULL;
  obstack_specify_allocation (&objfile->psymbol_cache.cache, 0, 0,
			      xmalloc, free);
  obstack_specify_allocation (&objfile->psymbol_obstack, 0, 0, xmalloc,
			      free);
  obstack_specify_allocation (&objfile->symbol_obstack, 0, 0, xmalloc,
			      free);
  obstack_specify_allocation (&objfile->type_obstack, 0, 0, xmalloc,
			      free);
  objfile->name = mstrsave (objfile->md, "rt_common");

  /* Add this file onto the tail of the linked list of other such files. */

  objfile->next = NULL;
  if (object_files == NULL)
    object_files = objfile;
  else
    {
      for (last_one = object_files;
	   last_one->next;
	   last_one = last_one->next);
      last_one->next = objfile;
    }

  rt_common_objfile = objfile;
}

/* Read all dynamically loaded common symbol definitions from the inferior
   and put them into the minimal symbol table for the runtime common
   objfile.  */

static void
solib_add_common_symbols (CORE_ADDR rtc_symp)
{
  struct rtc_symb inferior_rtc_symb;
  struct nlist inferior_rtc_nlist;
  int len;
  char *name;

  /* Remove any runtime common symbols from previous runs.  */

  if (rt_common_objfile != NULL && rt_common_objfile->minimal_symbol_count)
    {
      obstack_free (&rt_common_objfile->symbol_obstack, 0);
      obstack_specify_allocation (&rt_common_objfile->symbol_obstack, 0, 0,
				  xmalloc, free);
      rt_common_objfile->minimal_symbol_count = 0;
      rt_common_objfile->msymbols = NULL;
    }

  init_minimal_symbol_collection ();
  make_cleanup_discard_minimal_symbols ();

  while (rtc_symp)
    {
      read_memory (rtc_symp,
		   (char *) &inferior_rtc_symb,
		   sizeof (inferior_rtc_symb));
      read_memory (SOLIB_EXTRACT_ADDRESS (inferior_rtc_symb.rtc_sp),
		   (char *) &inferior_rtc_nlist,
		   sizeof (inferior_rtc_nlist));
      if (inferior_rtc_nlist.n_type == N_COMM)
	{
	  /* FIXME: The length of the symbol name is not available, but in the
	     current implementation the common symbol is allocated immediately
	     behind the name of the symbol. */
	  len = inferior_rtc_nlist.n_value - inferior_rtc_nlist.n_un.n_strx;

	  name = xmalloc (len);
	  read_memory (SOLIB_EXTRACT_ADDRESS (inferior_rtc_nlist.n_un.n_name),
		       name, len);

	  /* Allocate the runtime common objfile if necessary. */
	  if (rt_common_objfile == NULL)
	    allocate_rt_common_objfile ();

	  prim_record_minimal_symbol (name, inferior_rtc_nlist.n_value,
				      mst_bss, rt_common_objfile);
	  free (name);
	}
      rtc_symp = SOLIB_EXTRACT_ADDRESS (inferior_rtc_symb.rtc_next);
    }

  /* Install any minimal symbols that have been collected as the current
     minimal symbols for the runtime common objfile.  */

  install_minimal_symbols (rt_common_objfile);
}

#endif /* SVR4_SHARED_LIBS */


#ifdef SVR4_SHARED_LIBS

static CORE_ADDR bfd_lookup_symbol (bfd *, char *);

/*

   LOCAL FUNCTION

   bfd_lookup_symbol -- lookup the value for a specific symbol

   SYNOPSIS

   CORE_ADDR bfd_lookup_symbol (bfd *abfd, char *symname)

   DESCRIPTION

   An expensive way to lookup the value of a single symbol for
   bfd's that are only temporary anyway.  This is used by the
   shared library support to find the address of the debugger
   interface structures in the shared library.

   Note that 0 is specifically allowed as an error return (no
   such symbol).
 */

static CORE_ADDR
bfd_lookup_symbol (bfd *abfd, char *symname)
{
  unsigned int storage_needed;
  asymbol *sym;
  asymbol **symbol_table;
  unsigned int number_of_symbols;
  unsigned int i;
  struct cleanup *back_to;
  CORE_ADDR symaddr = 0;

  storage_needed = bfd_get_symtab_upper_bound (abfd);

  if (storage_needed > 0)
    {
      symbol_table = (asymbol **) xmalloc (storage_needed);
      back_to = make_cleanup (free, (PTR) symbol_table);
      number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

      for (i = 0; i < number_of_symbols; i++)
	{
	  sym = *symbol_table++;
	  if (STREQ (sym->name, symname))
	    {
	      /* Bfd symbols are section relative. */
	      symaddr = sym->value + sym->section->vma;
	      break;
	    }
	}
      do_cleanups (back_to);
    }

  if (symaddr)
    return symaddr;

  /* On FreeBSD, the dynamic linker is stripped by default.  So we'll
     have to check the dynamic string table too.  */

  storage_needed = bfd_get_dynamic_symtab_upper_bound (abfd);

  if (storage_needed > 0)
    {
      symbol_table = (asymbol **) xmalloc (storage_needed);
      back_to = make_cleanup (free, (PTR) symbol_table);
      number_of_symbols = bfd_canonicalize_dynamic_symtab (abfd, symbol_table);

      for (i = 0; i < number_of_symbols; i++)
	{
	  sym = *symbol_table++;
	  if (STREQ (sym->name, symname))
	    {
	      /* Bfd symbols are section relative. */
	      symaddr = sym->value + sym->section->vma;
	      break;
	    }
	}
      do_cleanups (back_to);
    }

  return symaddr;
}

#ifdef HANDLE_SVR4_EXEC_EMULATORS

/*
   Solaris BCP (the part of Solaris which allows it to run SunOS4
   a.out files) throws in another wrinkle. Solaris does not fill
   in the usual a.out link map structures when running BCP programs,
   the only way to get at them is via groping around in the dynamic
   linker.
   The dynamic linker and it's structures are located in the shared
   C library, which gets run as the executable's "interpreter" by
   the kernel.

   Note that we can assume nothing about the process state at the time
   we need to find these structures.  We may be stopped on the first
   instruction of the interpreter (C shared library), the first
   instruction of the executable itself, or somewhere else entirely
   (if we attached to the process for example).
 */

static char *debug_base_symbols[] =
{
  "r_debug",			/* Solaris 2.3 */
  "_r_debug",			/* Solaris 2.1, 2.2 */
  NULL
};

static int look_for_base (int, CORE_ADDR);

/*

   LOCAL FUNCTION

   look_for_base -- examine file for each mapped address segment

   SYNOPSYS

   static int look_for_base (int fd, CORE_ADDR baseaddr)

   DESCRIPTION

   This function is passed to proc_iterate_over_mappings, which
   causes it to get called once for each mapped address space, with
   an open file descriptor for the file mapped to that space, and the
   base address of that mapped space.

   Our job is to find the debug base symbol in the file that this
   fd is open on, if it exists, and if so, initialize the dynamic
   linker structure base address debug_base.

   Note that this is a computationally expensive proposition, since
   we basically have to open a bfd on every call, so we specifically
   avoid opening the exec file.
 */

static int
look_for_base (int fd, CORE_ADDR baseaddr)
{
  bfd *interp_bfd;
  CORE_ADDR address = 0;
  char **symbolp;

  /* If the fd is -1, then there is no file that corresponds to this
     mapped memory segment, so skip it.  Also, if the fd corresponds
     to the exec file, skip it as well. */

  if (fd == -1
      || (exec_bfd != NULL
	  && fdmatch (fileno ((FILE *) (exec_bfd->iostream)), fd)))
    {
      return (0);
    }

  /* Try to open whatever random file this fd corresponds to.  Note that
     we have no way currently to find the filename.  Don't gripe about
     any problems we might have, just fail. */

  if ((interp_bfd = bfd_fdopenr ("unnamed", gnutarget, fd)) == NULL)
    {
      return (0);
    }
  if (!bfd_check_format (interp_bfd, bfd_object))
    {
      /* FIXME-leak: on failure, might not free all memory associated with
         interp_bfd.  */
      bfd_close (interp_bfd);
      return (0);
    }

  /* Now try to find our debug base symbol in this file, which we at
     least know to be a valid ELF executable or shared library. */

  for (symbolp = debug_base_symbols; *symbolp != NULL; symbolp++)
    {
      address = bfd_lookup_symbol (interp_bfd, *symbolp);
      if (address != 0)
	{
	  break;
	}
    }
  if (address == 0)
    {
      /* FIXME-leak: on failure, might not free all memory associated with
         interp_bfd.  */
      bfd_close (interp_bfd);
      return (0);
    }

  /* Eureka!  We found the symbol.  But now we may need to relocate it
     by the base address.  If the symbol's value is less than the base
     address of the shared library, then it hasn't yet been relocated
     by the dynamic linker, and we have to do it ourself.  FIXME: Note
     that we make the assumption that the first segment that corresponds
     to the shared library has the base address to which the library
     was relocated. */

  if (address < baseaddr)
    {
      address += baseaddr;
    }
  debug_base = address;
  /* FIXME-leak: on failure, might not free all memory associated with
     interp_bfd.  */
  bfd_close (interp_bfd);
  return (1);
}
#endif /* HANDLE_SVR4_EXEC_EMULATORS */

/*

   LOCAL FUNCTION

   elf_locate_base -- locate the base address of dynamic linker structs
   for SVR4 elf targets.

   SYNOPSIS

   CORE_ADDR elf_locate_base (void)

   DESCRIPTION

   For SVR4 elf targets the address of the dynamic linker's runtime
   structure is contained within the dynamic info section in the
   executable file.  The dynamic section is also mapped into the
   inferior address space.  Because the runtime loader fills in the
   real address before starting the inferior, we have to read in the
   dynamic info section from the inferior address space.
   If there are any errors while trying to find the address, we
   silently return 0, otherwise the found address is returned.

 */

static CORE_ADDR
elf_locate_base (void)
{
  sec_ptr dyninfo_sect;
  int dyninfo_sect_size;
  CORE_ADDR dyninfo_addr;
  char *buf;
  char *bufend;
  int arch_size;

  /* Find the start address of the .dynamic section.  */
  dyninfo_sect = bfd_get_section_by_name (exec_bfd, ".dynamic");
  if (dyninfo_sect == NULL)
    return 0;
  dyninfo_addr = bfd_section_vma (exec_bfd, dyninfo_sect);

  /* Read in .dynamic section, silently ignore errors.  */
  dyninfo_sect_size = bfd_section_size (exec_bfd, dyninfo_sect);
  buf = alloca (dyninfo_sect_size);
  if (target_read_memory (dyninfo_addr, buf, dyninfo_sect_size))
    return 0;

  /* Find the DT_DEBUG entry in the the .dynamic section.
     For mips elf we look for DT_MIPS_RLD_MAP, mips elf apparently has
     no DT_DEBUG entries.  */

  arch_size = bfd_get_arch_size (exec_bfd);
  if (arch_size == -1)	/* failure */
    return 0;

  if (arch_size == 32)
    { /* 32-bit elf */
      for (bufend = buf + dyninfo_sect_size;
	   buf < bufend;
	   buf += sizeof (Elf32_External_Dyn))
	{
	  Elf32_External_Dyn *x_dynp = (Elf32_External_Dyn *) buf;
	  long dyn_tag;
	  CORE_ADDR dyn_ptr;

	  dyn_tag = bfd_h_get_32 (exec_bfd, (bfd_byte *) x_dynp->d_tag);
	  if (dyn_tag == DT_NULL)
	    break;
	  else if (dyn_tag == DT_DEBUG)
	    {
	      dyn_ptr = bfd_h_get_32 (exec_bfd, 
				      (bfd_byte *) x_dynp->d_un.d_ptr);
	      return dyn_ptr;
	    }
#ifdef DT_MIPS_RLD_MAP
	  else if (dyn_tag == DT_MIPS_RLD_MAP)
	    {
	      char *pbuf;

	      pbuf = alloca (TARGET_PTR_BIT / HOST_CHAR_BIT);
	      /* DT_MIPS_RLD_MAP contains a pointer to the address
		 of the dynamic link structure.  */
	      dyn_ptr = bfd_h_get_32 (exec_bfd, 
				      (bfd_byte *) x_dynp->d_un.d_ptr);
	      if (target_read_memory (dyn_ptr, pbuf, sizeof (pbuf)))
		return 0;
	      return extract_unsigned_integer (pbuf, sizeof (pbuf));
	    }
#endif
	}
    }
  else /* 64-bit elf */
    {
      for (bufend = buf + dyninfo_sect_size;
	   buf < bufend;
	   buf += sizeof (Elf64_External_Dyn))
	{
	  Elf64_External_Dyn *x_dynp = (Elf64_External_Dyn *) buf;
	  long dyn_tag;
	  CORE_ADDR dyn_ptr;

	  dyn_tag = bfd_h_get_64 (exec_bfd, (bfd_byte *) x_dynp->d_tag);
	  if (dyn_tag == DT_NULL)
	    break;
	  else if (dyn_tag == DT_DEBUG)
	    {
	      dyn_ptr = bfd_h_get_64 (exec_bfd, 
				      (bfd_byte *) x_dynp->d_un.d_ptr);
	      return dyn_ptr;
	    }
	}
    }

  /* DT_DEBUG entry not found.  */
  return 0;
}

#endif /* SVR4_SHARED_LIBS */

/*

   LOCAL FUNCTION

   locate_base -- locate the base address of dynamic linker structs

   SYNOPSIS

   CORE_ADDR locate_base (void)

   DESCRIPTION

   For both the SunOS and SVR4 shared library implementations, if the
   inferior executable has been linked dynamically, there is a single
   address somewhere in the inferior's data space which is the key to
   locating all of the dynamic linker's runtime structures.  This
   address is the value of the debug base symbol.  The job of this
   function is to find and return that address, or to return 0 if there
   is no such address (the executable is statically linked for example).

   For SunOS, the job is almost trivial, since the dynamic linker and
   all of it's structures are statically linked to the executable at
   link time.  Thus the symbol for the address we are looking for has
   already been added to the minimal symbol table for the executable's
   objfile at the time the symbol file's symbols were read, and all we
   have to do is look it up there.  Note that we explicitly do NOT want
   to find the copies in the shared library.

   The SVR4 version is a bit more complicated because the address
   is contained somewhere in the dynamic info section.  We have to go
   to a lot more work to discover the address of the debug base symbol.
   Because of this complexity, we cache the value we find and return that
   value on subsequent invocations.  Note there is no copy in the
   executable symbol tables.

 */

static CORE_ADDR
locate_base (void)
{

#ifndef SVR4_SHARED_LIBS

  struct minimal_symbol *msymbol;
  CORE_ADDR address = 0;
  char **symbolp;

  /* For SunOS, we want to limit the search for the debug base symbol to the
     executable being debugged, since there is a duplicate named symbol in the
     shared library.  We don't want the shared library versions. */

  for (symbolp = debug_base_symbols; *symbolp != NULL; symbolp++)
    {
      msymbol = lookup_minimal_symbol (*symbolp, NULL, symfile_objfile);
      if ((msymbol != NULL) && (SYMBOL_VALUE_ADDRESS (msymbol) != 0))
	{
	  address = SYMBOL_VALUE_ADDRESS (msymbol);
	  return (address);
	}
    }
  return (0);

#else /* SVR4_SHARED_LIBS */

  /* Check to see if we have a currently valid address, and if so, avoid
     doing all this work again and just return the cached address.  If
     we have no cached address, try to locate it in the dynamic info
     section for ELF executables.  */

  if (debug_base == 0)
    {
      if (exec_bfd != NULL
	  && bfd_get_flavour (exec_bfd) == bfd_target_elf_flavour)
	debug_base = elf_locate_base ();
#ifdef HANDLE_SVR4_EXEC_EMULATORS
      /* Try it the hard way for emulated executables.  */
      else if (inferior_pid != 0 && target_has_execution)
	proc_iterate_over_mappings (look_for_base);
#endif
    }
  return (debug_base);

#endif /* !SVR4_SHARED_LIBS */

}

/*

   LOCAL FUNCTION

   first_link_map_member -- locate first member in dynamic linker's map

   SYNOPSIS

   static CORE_ADDR first_link_map_member (void)

   DESCRIPTION

   Find the first element in the inferior's dynamic link map, and
   return its address in the inferior.  This function doesn't copy the
   link map entry itself into our address space; current_sos actually
   does the reading.  */

static CORE_ADDR
first_link_map_member (void)
{
  CORE_ADDR lm = 0;

#ifndef SVR4_SHARED_LIBS

  read_memory (debug_base, (char *) &dynamic_copy, sizeof (dynamic_copy));
  if (dynamic_copy.ld_version >= 2)
    {
      /* It is a version that we can deal with, so read in the secondary
         structure and find the address of the link map list from it. */
      read_memory (SOLIB_EXTRACT_ADDRESS (dynamic_copy.ld_un.ld_2),
		   (char *) &ld_2_copy, sizeof (struct link_dynamic_2));
      lm = SOLIB_EXTRACT_ADDRESS (ld_2_copy.ld_loaded);
    }

#else /* SVR4_SHARED_LIBS */
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();
  char *r_map_buf = xmalloc (lmo->r_map_size);
  struct cleanup *cleanups = make_cleanup (free, r_map_buf);

  read_memory (debug_base + lmo->r_map_offset, r_map_buf, lmo->r_map_size);

  lm = extract_address (r_map_buf, lmo->r_map_size);

  /* FIXME:  Perhaps we should validate the info somehow, perhaps by
     checking r_version for a known version number, or r_state for
     RT_CONSISTENT. */

  do_cleanups (cleanups);

#endif /* !SVR4_SHARED_LIBS */

  return (lm);
}

#ifdef SVR4_SHARED_LIBS
/*

  LOCAL FUNCTION

  open_symbol_file_object

  SYNOPSIS

  void open_symbol_file_object (void *from_tty)

  DESCRIPTION

  If no open symbol file, attempt to locate and open the main symbol
  file.  On SVR4 systems, this is the first link map entry.  If its
  name is here, we can open it.  Useful when attaching to a process
  without first loading its symbol file.

  If FROM_TTYP dereferences to a non-zero integer, allow messages to
  be printed.  This parameter is a pointer rather than an int because
  open_symbol_file_object() is called via catch_errors() and
  catch_errors() requires a pointer argument. */

static int
open_symbol_file_object (void *from_ttyp)
{
  CORE_ADDR lm, l_name;
  char *filename;
  int errcode;
  int from_tty = *(int *)from_ttyp;
  struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();
  char *l_name_buf = xmalloc (lmo->l_name_size);
  struct cleanup *cleanups = make_cleanup (free, l_name_buf);

  if (symfile_objfile)
    if (!query ("Attempt to reload symbols from process? "))
      return 0;

  if ((debug_base = locate_base ()) == 0)
    return 0;	/* failed somehow... */

  /* First link map member should be the executable.  */
  if ((lm = first_link_map_member ()) == 0)
    return 0;	/* failed somehow... */

  /* Read address of name from target memory to GDB.  */
  read_memory (lm + lmo->l_name_offset, l_name_buf, lmo->l_name_size);

  /* Convert the address to host format.  */
  l_name = extract_address (l_name_buf, lmo->l_name_size);

  /* Free l_name_buf.  */
  do_cleanups (cleanups);

  if (l_name == 0)
    return 0;		/* No filename.  */

  /* Now fetch the filename from target memory.  */
  target_read_string (l_name, &filename, SO_NAME_MAX_PATH_SIZE - 1, &errcode);

  if (errcode)
    {
      warning ("failed to read exec filename from attached file: %s",
	       safe_strerror (errcode));
      return 0;
    }

  make_cleanup (free, filename);
  /* Have a pathname: read the symbol file.  */
  symbol_file_command (filename, from_tty);

  return 1;
}
#else

static int
open_symbol_file_object (int *from_ttyp)
{
  return 1;
}

#endif /* SVR4_SHARED_LIBS */


/* LOCAL FUNCTION

   current_sos -- build a list of currently loaded shared objects

   SYNOPSIS

   struct so_list *current_sos ()

   DESCRIPTION

   Build a list of `struct so_list' objects describing the shared
   objects currently loaded in the inferior.  This list does not
   include an entry for the main executable file.

   Note that we only gather information directly available from the
   inferior --- we don't examine any of the shared library files
   themselves.  The declaration of `struct so_list' says which fields
   we provide values for.  */

static struct so_list *
svr4_current_sos (void)
{
  CORE_ADDR lm;
  struct so_list *head = 0;
  struct so_list **link_ptr = &head;

  /* Make sure we've looked up the inferior's dynamic linker's base
     structure.  */
  if (! debug_base)
    {
      debug_base = locate_base ();

      /* If we can't find the dynamic linker's base structure, this
	 must not be a dynamically linked executable.  Hmm.  */
      if (! debug_base)
	return 0;
    }

  /* Walk the inferior's link map list, and build our list of
     `struct so_list' nodes.  */
  lm = first_link_map_member ();  
  while (lm)
    {
      struct link_map_offsets *lmo = SVR4_FETCH_LINK_MAP_OFFSETS ();
      struct so_list *new
	= (struct so_list *) xmalloc (sizeof (struct so_list));
      struct cleanup *old_chain = make_cleanup (free, new);

      memset (new, 0, sizeof (*new));

      new->lm_info = xmalloc (sizeof (struct lm_info));
      make_cleanup (free, new->lm_info);

      new->lm_info->lm = xmalloc (lmo->link_map_size);
      make_cleanup (free, new->lm_info->lm);
      memset (new->lm_info->lm, 0, lmo->link_map_size);

      read_memory (lm, new->lm_info->lm, lmo->link_map_size);

      lm = LM_NEXT (new);

      /* For SVR4 versions, the first entry in the link map is for the
         inferior executable, so we must ignore it.  For some versions of
         SVR4, it has no name.  For others (Solaris 2.3 for example), it
         does have a name, so we can no longer use a missing name to
         decide when to ignore it. */
      if (IGNORE_FIRST_LINK_MAP_ENTRY (new))
	free_so (new);
      else
	{
	  int errcode;
	  char *buffer;

	  /* Extract this shared object's name.  */
	  target_read_string (LM_NAME (new), &buffer,
			      SO_NAME_MAX_PATH_SIZE - 1, &errcode);
	  if (errcode != 0)
	    {
	      warning ("current_sos: Can't read pathname for load map: %s\n",
		       safe_strerror (errcode));
	    }
	  else
	    {
	      strncpy (new->so_name, buffer, SO_NAME_MAX_PATH_SIZE - 1);
	      new->so_name[SO_NAME_MAX_PATH_SIZE - 1] = '\0';
	      free (buffer);
	      strcpy (new->so_original_name, new->so_name);
	    }

	  /* If this entry has no name, or its name matches the name
	     for the main executable, don't include it in the list.  */
	  if (! new->so_name[0]
	      || match_main (new->so_name))
	    free_so (new);
	  else
	    {
	      new->next = 0;
	      *link_ptr = new;
	      link_ptr = &new->next;
	    }
	}

      discard_cleanups (old_chain);
    }

  return head;
}


/* On some systems, the only way to recognize the link map entry for
   the main executable file is by looking at its name.  Return
   non-zero iff SONAME matches one of the known main executable names.  */

static int
match_main (char *soname)
{
  char **mainp;

  for (mainp = main_name_list; *mainp != NULL; mainp++)
    {
      if (strcmp (soname, *mainp) == 0)
	return (1);
    }

  return (0);
}


#ifdef SVR4_SHARED_LIBS

/* Return 1 if PC lies in the dynamic symbol resolution code of the
   SVR4 run time loader.  */

static CORE_ADDR interp_text_sect_low;
static CORE_ADDR interp_text_sect_high;
static CORE_ADDR interp_plt_sect_low;
static CORE_ADDR interp_plt_sect_high;

int
in_svr4_dynsym_resolve_code (CORE_ADDR pc)
{
  return ((pc >= interp_text_sect_low && pc < interp_text_sect_high)
	  || (pc >= interp_plt_sect_low && pc < interp_plt_sect_high)
	  || in_plt_section (pc, NULL));
}
#endif

/*

   LOCAL FUNCTION

   disable_break -- remove the "mapping changed" breakpoint

   SYNOPSIS

   static int disable_break ()

   DESCRIPTION

   Removes the breakpoint that gets hit when the dynamic linker
   completes a mapping change.

 */

#ifndef SVR4_SHARED_LIBS

static int
disable_break (void)
{
  int status = 1;

  int in_debugger = 0;

  /* Read the debugger structure from the inferior to retrieve the
     address of the breakpoint and the original contents of the
     breakpoint address.  Remove the breakpoint by writing the original
     contents back. */

  read_memory (debug_addr, (char *) &debug_copy, sizeof (debug_copy));

  /* Set `in_debugger' to zero now. */

  write_memory (flag_addr, (char *) &in_debugger, sizeof (in_debugger));

  breakpoint_addr = SOLIB_EXTRACT_ADDRESS (debug_copy.ldd_bp_addr);
  write_memory (breakpoint_addr, (char *) &debug_copy.ldd_bp_inst,
		sizeof (debug_copy.ldd_bp_inst));

  /* For the SVR4 version, we always know the breakpoint address.  For the
     SunOS version we don't know it until the above code is executed.
     Grumble if we are stopped anywhere besides the breakpoint address. */

  if (stop_pc != breakpoint_addr)
    {
      warning ("stopped at unknown breakpoint while handling shared libraries");
    }

  return (status);
}

#endif /* #ifdef SVR4_SHARED_LIBS */

/*

   LOCAL FUNCTION

   enable_break -- arrange for dynamic linker to hit breakpoint

   SYNOPSIS

   int enable_break (void)

   DESCRIPTION

   Both the SunOS and the SVR4 dynamic linkers have, as part of their
   debugger interface, support for arranging for the inferior to hit
   a breakpoint after mapping in the shared libraries.  This function
   enables that breakpoint.

   For SunOS, there is a special flag location (in_debugger) which we
   set to 1.  When the dynamic linker sees this flag set, it will set
   a breakpoint at a location known only to itself, after saving the
   original contents of that place and the breakpoint address itself,
   in it's own internal structures.  When we resume the inferior, it
   will eventually take a SIGTRAP when it runs into the breakpoint.
   We handle this (in a different place) by restoring the contents of
   the breakpointed location (which is only known after it stops),
   chasing around to locate the shared libraries that have been
   loaded, then resuming.

   For SVR4, the debugger interface structure contains a member (r_brk)
   which is statically initialized at the time the shared library is
   built, to the offset of a function (_r_debug_state) which is guaran-
   teed to be called once before mapping in a library, and again when
   the mapping is complete.  At the time we are examining this member,
   it contains only the unrelocated offset of the function, so we have
   to do our own relocation.  Later, when the dynamic linker actually
   runs, it relocates r_brk to be the actual address of _r_debug_state().

   The debugger interface structure also contains an enumeration which
   is set to either RT_ADD or RT_DELETE prior to changing the mapping,
   depending upon whether or not the library is being mapped or unmapped,
   and then set to RT_CONSISTENT after the library is mapped/unmapped.
 */

static int
enable_break (void)
{
  int success = 0;

#ifndef SVR4_SHARED_LIBS

  int j;
  int in_debugger;

  /* Get link_dynamic structure */

  j = target_read_memory (debug_base, (char *) &dynamic_copy,
			  sizeof (dynamic_copy));
  if (j)
    {
      /* unreadable */
      return (0);
    }

  /* Calc address of debugger interface structure */

  debug_addr = SOLIB_EXTRACT_ADDRESS (dynamic_copy.ldd);

  /* Calc address of `in_debugger' member of debugger interface structure */

  flag_addr = debug_addr + (CORE_ADDR) ((char *) &debug_copy.ldd_in_debugger -
					(char *) &debug_copy);

  /* Write a value of 1 to this member.  */

  in_debugger = 1;
  write_memory (flag_addr, (char *) &in_debugger, sizeof (in_debugger));
  success = 1;

#else /* SVR4_SHARED_LIBS */

#ifdef BKPT_AT_SYMBOL

  struct minimal_symbol *msymbol;
  char **bkpt_namep;
  asection *interp_sect;

  /* First, remove all the solib event breakpoints.  Their addresses
     may have changed since the last time we ran the program.  */
  remove_solib_event_breakpoints ();

#ifdef SVR4_SHARED_LIBS
  interp_text_sect_low = interp_text_sect_high = 0;
  interp_plt_sect_low = interp_plt_sect_high = 0;

  /* Find the .interp section; if not found, warn the user and drop
     into the old breakpoint at symbol code.  */
  interp_sect = bfd_get_section_by_name (exec_bfd, ".interp");
  if (interp_sect)
    {
      unsigned int interp_sect_size;
      char *buf;
      CORE_ADDR load_addr;
      bfd *tmp_bfd;
      CORE_ADDR sym_addr = 0;

      /* Read the contents of the .interp section into a local buffer;
         the contents specify the dynamic linker this program uses.  */
      interp_sect_size = bfd_section_size (exec_bfd, interp_sect);
      buf = alloca (interp_sect_size);
      bfd_get_section_contents (exec_bfd, interp_sect,
				buf, 0, interp_sect_size);

      /* Now we need to figure out where the dynamic linker was
         loaded so that we can load its symbols and place a breakpoint
         in the dynamic linker itself.

         This address is stored on the stack.  However, I've been unable
         to find any magic formula to find it for Solaris (appears to
         be trivial on GNU/Linux).  Therefore, we have to try an alternate
         mechanism to find the dynamic linker's base address.  */
      tmp_bfd = bfd_openr (buf, gnutarget);
      if (tmp_bfd == NULL)
	goto bkpt_at_symbol;

      /* Make sure the dynamic linker's really a useful object.  */
      if (!bfd_check_format (tmp_bfd, bfd_object))
	{
	  warning ("Unable to grok dynamic linker %s as an object file", buf);
	  bfd_close (tmp_bfd);
	  goto bkpt_at_symbol;
	}

      /* We find the dynamic linker's base address by examining the
         current pc (which point at the entry point for the dynamic
         linker) and subtracting the offset of the entry point.  */
      load_addr = read_pc () - tmp_bfd->start_address;

      /* Record the relocated start and end address of the dynamic linker
         text and plt section for in_svr4_dynsym_resolve_code.  */
      interp_sect = bfd_get_section_by_name (tmp_bfd, ".text");
      if (interp_sect)
	{
	  interp_text_sect_low =
	    bfd_section_vma (tmp_bfd, interp_sect) + load_addr;
	  interp_text_sect_high =
	    interp_text_sect_low + bfd_section_size (tmp_bfd, interp_sect);
	}
      interp_sect = bfd_get_section_by_name (tmp_bfd, ".plt");
      if (interp_sect)
	{
	  interp_plt_sect_low =
	    bfd_section_vma (tmp_bfd, interp_sect) + load_addr;
	  interp_plt_sect_high =
	    interp_plt_sect_low + bfd_section_size (tmp_bfd, interp_sect);
	}

      /* Now try to set a breakpoint in the dynamic linker.  */
      for (bkpt_namep = solib_break_names; *bkpt_namep != NULL; bkpt_namep++)
	{
	  sym_addr = bfd_lookup_symbol (tmp_bfd, *bkpt_namep);
	  if (sym_addr != 0)
	    break;
	}

      /* We're done with the temporary bfd.  */
      bfd_close (tmp_bfd);

      if (sym_addr != 0)
	{
	  create_solib_event_breakpoint (load_addr + sym_addr);
	  return 1;
	}

      /* For whatever reason we couldn't set a breakpoint in the dynamic
         linker.  Warn and drop into the old code.  */
    bkpt_at_symbol:
      warning ("Unable to find dynamic linker breakpoint function.\nGDB will be unable to debug shared library initializers\nand track explicitly loaded dynamic code.");
    }
#endif

  /* Scan through the list of symbols, trying to look up the symbol and
     set a breakpoint there.  Terminate loop when we/if we succeed. */

  breakpoint_addr = 0;
  for (bkpt_namep = bkpt_names; *bkpt_namep != NULL; bkpt_namep++)
    {
      msymbol = lookup_minimal_symbol (*bkpt_namep, NULL, symfile_objfile);
      if ((msymbol != NULL) && (SYMBOL_VALUE_ADDRESS (msymbol) != 0))
	{
	  create_solib_event_breakpoint (SYMBOL_VALUE_ADDRESS (msymbol));
	  return 1;
	}
    }

  /* Nothing good happened.  */
  success = 0;

#endif /* BKPT_AT_SYMBOL */

#endif /* !SVR4_SHARED_LIBS */

  return (success);
}

/*

   LOCAL FUNCTION

   special_symbol_handling -- additional shared library symbol handling

   SYNOPSIS

   void special_symbol_handling ()

   DESCRIPTION

   Once the symbols from a shared object have been loaded in the usual
   way, we are called to do any system specific symbol handling that 
   is needed.

   For SunOS4, this consists of grunging around in the dynamic
   linkers structures to find symbol definitions for "common" symbols
   and adding them to the minimal symbol table for the runtime common
   objfile.

 */

static void
svr4_special_symbol_handling (void)
{
#ifndef SVR4_SHARED_LIBS
  int j;

  if (debug_addr == 0)
    {
      /* Get link_dynamic structure */

      j = target_read_memory (debug_base, (char *) &dynamic_copy,
			      sizeof (dynamic_copy));
      if (j)
	{
	  /* unreadable */
	  return;
	}

      /* Calc address of debugger interface structure */
      /* FIXME, this needs work for cross-debugging of core files
         (byteorder, size, alignment, etc).  */

      debug_addr = SOLIB_EXTRACT_ADDRESS (dynamic_copy.ldd);
    }

  /* Read the debugger structure from the inferior, just to make sure
     we have a current copy. */

  j = target_read_memory (debug_addr, (char *) &debug_copy,
			  sizeof (debug_copy));
  if (j)
    return;			/* unreadable */

  /* Get common symbol definitions for the loaded object. */

  if (debug_copy.ldd_cp)
    {
      solib_add_common_symbols (SOLIB_EXTRACT_ADDRESS (debug_copy.ldd_cp));
    }

#endif /* !SVR4_SHARED_LIBS */
}

/*

   GLOBAL FUNCTION

   svr4_solib_create_inferior_hook -- shared library startup support

   SYNOPSIS

   void svr4_solib_create_inferior_hook()

   DESCRIPTION

   When gdb starts up the inferior, it nurses it along (through the
   shell) until it is ready to execute it's first instruction.  At this
   point, this function gets called via expansion of the macro
   SOLIB_CREATE_INFERIOR_HOOK.

   For SunOS executables, this first instruction is typically the
   one at "_start", or a similar text label, regardless of whether
   the executable is statically or dynamically linked.  The runtime
   startup code takes care of dynamically linking in any shared
   libraries, once gdb allows the inferior to continue.

   For SVR4 executables, this first instruction is either the first
   instruction in the dynamic linker (for dynamically linked
   executables) or the instruction at "start" for statically linked
   executables.  For dynamically linked executables, the system
   first exec's /lib/libc.so.N, which contains the dynamic linker,
   and starts it running.  The dynamic linker maps in any needed
   shared libraries, maps in the actual user executable, and then
   jumps to "start" in the user executable.

   For both SunOS shared libraries, and SVR4 shared libraries, we
   can arrange to cooperate with the dynamic linker to discover the
   names of shared libraries that are dynamically linked, and the
   base addresses to which they are linked.

   This function is responsible for discovering those names and
   addresses, and saving sufficient information about them to allow
   their symbols to be read at a later time.

   FIXME

   Between enable_break() and disable_break(), this code does not
   properly handle hitting breakpoints which the user might have
   set in the startup code or in the dynamic linker itself.  Proper
   handling will probably have to wait until the implementation is
   changed to use the "breakpoint handler function" method.

   Also, what if child has exit()ed?  Must exit loop somehow.
 */

void
svr4_solib_create_inferior_hook (void)
{
  /* If we are using the BKPT_AT_SYMBOL code, then we don't need the base
     yet.  In fact, in the case of a SunOS4 executable being run on
     Solaris, we can't get it yet.  current_sos will get it when it needs
     it.  */
#if !(defined (SVR4_SHARED_LIBS) && defined (BKPT_AT_SYMBOL))
  if ((debug_base = locate_base ()) == 0)
    {
      /* Can't find the symbol or the executable is statically linked. */
      return;
    }
#endif

  if (!enable_break ())
    {
      warning ("shared library handler failed to enable breakpoint");
      return;
    }

#if !defined(SVR4_SHARED_LIBS) || defined(_SCO_DS)
  /* SCO and SunOS need the loop below, other systems should be using the
     special shared library breakpoints and the shared library breakpoint
     service routine.

     Now run the target.  It will eventually hit the breakpoint, at
     which point all of the libraries will have been mapped in and we
     can go groveling around in the dynamic linker structures to find
     out what we need to know about them. */

  clear_proceed_status ();
  stop_soon_quietly = 1;
  stop_signal = TARGET_SIGNAL_0;
  do
    {
      target_resume (-1, 0, stop_signal);
      wait_for_inferior ();
    }
  while (stop_signal != TARGET_SIGNAL_TRAP);
  stop_soon_quietly = 0;

#if !defined(_SCO_DS)
  /* We are now either at the "mapping complete" breakpoint (or somewhere
     else, a condition we aren't prepared to deal with anyway), so adjust
     the PC as necessary after a breakpoint, disable the breakpoint, and
     add any shared libraries that were mapped in. */

  if (DECR_PC_AFTER_BREAK)
    {
      stop_pc -= DECR_PC_AFTER_BREAK;
      write_register (PC_REGNUM, stop_pc);
    }

  if (!disable_break ())
    {
      warning ("shared library handler failed to disable breakpoint");
    }

  if (auto_solib_add)
    solib_add ((char *) 0, 0, (struct target_ops *) 0);
#endif /* ! _SCO_DS */
#endif
}

static void
svr4_clear_solib (void)
{
  debug_base = 0;
}

static void
svr4_free_so (struct so_list *so)
{
  free (so->lm_info->lm);
  free (so->lm_info);
}

static void
svr4_relocate_section_addresses (struct so_list *so,
                                 struct section_table *sec)
{
  sec->addr += LM_ADDR (so);
  sec->endaddr += LM_ADDR (so);
}

static struct target_so_ops svr4_so_ops;

void
_initialize_svr4_solib (void)
{
  svr4_so_ops.relocate_section_addresses = svr4_relocate_section_addresses;
  svr4_so_ops.free_so = svr4_free_so;
  svr4_so_ops.clear_solib = svr4_clear_solib;
  svr4_so_ops.solib_create_inferior_hook = svr4_solib_create_inferior_hook;
  svr4_so_ops.special_symbol_handling = svr4_special_symbol_handling;
  svr4_so_ops.current_sos = svr4_current_sos;
  svr4_so_ops.open_symbol_file_object = open_symbol_file_object;

  /* FIXME: Don't do this here.  *_gdbarch_init() should set so_ops. */
  current_target_so_ops = &svr4_so_ops;
}

