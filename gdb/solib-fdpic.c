/* Handle fdpic shared libraries for GDB, the GNU Debugger.

   Copyright (C) 2015 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#include "defs.h"
#include "gdb_string.h"
#include "inferior.h"
#include "gdbcore.h"
#include "solib.h"
#include "solist.h"
#include "arm-tdep.h"
#include "objfiles.h"
#include "symtab.h"
#include "language.h"
#include "command.h"
#include "gdbcmd.h"
#include "elf-bfd.h"
#include "exceptions.h"
#include <elf.h>

/*
  TODO:
  - implement endianess support
  - fetch_loadmap shortcut (use a two steps procedure)
  - remove_solib_event_breakpoints not called due to bug in gdb
*/

struct elf32_fdpic_loadseg
{
  Elf32_Addr addr;
  Elf32_Addr p_vaddr;
  Elf32_Word p_memsz;
};
struct elf32_fdpic_loadmap
{
  Elf32_Half version;
  Elf32_Half nsegs;
  struct elf32_fdpic_loadseg segs[/*nsegs*/];
};
struct elf32_fdpic_loadaddr
{
  struct elf32_fdpic_loadmap *map;
  void *got_value;
};
struct link_map
{
  struct elf32_fdpic_loadaddr l_addr;
  char *l_name;   /* Absolute file name object was found in.  */
  char *l_ld;     /* Dynamic section of the shared object.  */
  struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */
};

struct lm_info
{
  struct elf32_fdpic_loadmap *map;
  CORE_ADDR got_value;
  CORE_ADDR lm_addr;
};

/* Per pspace fdpic specific data.  */
struct fdpic_info
{
  struct lm_info *main_executable_lm_info;
  int is_static_binary;
  CORE_ADDR lm_base_cache;
  CORE_ADDR main_lm_addr;
  int enable_break2_done;
  CORE_ADDR interp_text_sect_low;
  CORE_ADDR interp_text_sect_high;
  CORE_ADDR interp_plt_sect_low;
  CORE_ADDR interp_plt_sect_high;
};

/* Flag which indicates whether internal debug messages should be printed.  */
static int solib_fdpic_debug = 0;

/* Per-program-space data key.  */
static const struct program_space_data *solib_fdpic_pspace_data;

/* Helper function for gdb_bfd_lookup_symbol.  */
static int
cmp_name (asymbol *sym, void *data)
{
  return (strcmp (sym->name, (const char *) data) == 0);
}

static CORE_ADDR
displacement_from_map (struct elf32_fdpic_loadmap *map, CORE_ADDR addr)
{
  int seg;

  for (seg = 0; seg < map->nsegs; seg++)
    {
      if (map->segs[seg].p_vaddr <= addr
	  && addr < map->segs[seg].p_vaddr + map->segs[seg].p_memsz)
	return map->segs[seg].addr - map->segs[seg].p_vaddr;
    }

  return 0;
}

static void
fdpic_pspace_data_cleanup (struct program_space *pspace, void *arg)
{
  struct fdpic_info *info;

  info = program_space_data (pspace, solib_fdpic_pspace_data);
  if (info && info->main_executable_lm_info)
    {
      xfree (info->main_executable_lm_info->map);
      xfree (info->main_executable_lm_info);
    }
  xfree (info);
}

static struct fdpic_info *
get_fdpic_info (void)
{
  struct fdpic_info *info;

  info = program_space_data (current_program_space, solib_fdpic_pspace_data);
  if (info != NULL)
    return info;

  info = XZALLOC (struct fdpic_info);
  set_program_space_data (current_program_space, solib_fdpic_pspace_data, info);

  info->main_executable_lm_info = NULL;
  info->enable_break2_done = 0;
  info->is_static_binary = 0;

  return info;
}

static struct elf32_fdpic_loadmap *
decode_loadmap (gdb_byte *buf)
{
  struct elf32_fdpic_loadmap *input = (struct elf32_fdpic_loadmap *) buf;
  struct elf32_fdpic_loadmap *res;
  /* TODO: implement endianess support.  */

  if (input->version != 0 || input->nsegs <= 0)
    return NULL;

  res = xmalloc (sizeof (struct elf32_fdpic_loadmap)
		 + input->nsegs * sizeof (struct elf32_fdpic_loadseg));
  if (res)
    {
      int i;

      res->version = input->version;
      res->nsegs = input->nsegs;
      for (i = 0; i < input->nsegs; i++)
	{
	  res->segs[i].addr = input->segs[i].addr;
	  res->segs[i].p_vaddr = input->segs[i].p_vaddr;
	  res->segs[i].p_memsz = input->segs[i].p_memsz;
	}
    }

  return res;
}

static struct elf32_fdpic_loadmap *
fetch_loadmap (CORE_ADDR ldmaddr)
{
  gdb_byte buf[sizeof (struct elf32_fdpic_loadmap)
	       + 2 * sizeof (struct elf32_fdpic_loadseg)];
  /* TODO: Here we take a shortcut since nsegs is always 2...  */

  if (target_read_memory (ldmaddr, buf, sizeof (buf)))
    return NULL;

  return decode_loadmap (buf);
}


static void
dump_loadmap (struct elf32_fdpic_loadmap *loadmap)
{
  if (solib_fdpic_debug && loadmap)
    {
      int i;
      fprintf_unfiltered (gdb_stdlog,
			  "*** LOADMAP : %d segments detected\n", loadmap->nsegs);

      for (i = 0; i < loadmap->nsegs; i++)
	fprintf_unfiltered (gdb_stdlog,
			    " - seg[%d] : %s map to %s for %d bytes\n", i,
			    hex_string_custom (loadmap->segs[i].p_vaddr, 8),
			    hex_string_custom (loadmap->segs[i].addr, 8),
			    loadmap->segs[i].p_memsz);
    }
}

static struct elf32_fdpic_loadmap *
fdpic_get_initial_loadmaps (int is_interpreter)
{
  gdb_byte *buf;
  struct elf32_fdpic_loadmap *res;

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog, "  %s : %d\n", __FUNCTION__, is_interpreter);

  /* Read raw loadmap.  */
  if (0 >= target_read_alloc (&current_target, TARGET_OBJECT_FDPIC,
			      is_interpreter ? "interp" : "exec",
			      (gdb_byte**) &buf))
    {
      if (!is_interpreter)
	error (_("Error reading FDPIC exec loadmap"));
      return NULL;
    }
  else if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"   - Successfully loaded %s loadmap\n",
			is_interpreter ? "interpreter" : "executable");

  res = decode_loadmap (buf);
  xfree (buf);
  dump_loadmap (res);

  return res;
}

static void
fdpic_relocate_main_executable (void)
{
  struct elf32_fdpic_loadmap *loadmap;
  struct cleanup *old_chain;
  struct section_offsets *new_offsets;
  int changed;
  struct obj_section *osect;
  struct fdpic_info *info = get_fdpic_info ();

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog, " %s\n", __FUNCTION__);

  /* Get interpreter loadmap to see if we have a static or dynamic binary.  */
  loadmap = fdpic_get_initial_loadmaps (1);
  if (!loadmap)
    info->is_static_binary = 1;
  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"*** %s binary detected\n",
			info->is_static_binary
			? "Statically linked" : "Dynamically linked");
  xfree (loadmap);

  if (info->main_executable_lm_info)
    {
      xfree (info->main_executable_lm_info->map);
      xfree (info->main_executable_lm_info);
    }
  info->main_executable_lm_info = xcalloc (1, sizeof (struct lm_info));
  info->main_executable_lm_info->map = fdpic_get_initial_loadmaps (0);

  /* Now relocate sections.  */
  new_offsets = xcalloc (symfile_objfile->num_sections,
			 sizeof (struct section_offsets));
  old_chain = make_cleanup (xfree, new_offsets);
  changed = 0;

  /* Compute new offset for all sections.  */
  ALL_OBJFILE_OSECTIONS (symfile_objfile, osect)
    {
      CORE_ADDR orig_addr, addr, offset;
      int osect_idx;
      int seg;

      osect_idx = osect->the_bfd_section->index;
      /* Current address of section.  */
      addr = obj_section_addr (osect);
      /* Offset from where this section started.  */
      offset = ANOFFSET (symfile_objfile->section_offsets, osect_idx);
      /* Original address prior to any past relocations.  */
      orig_addr = addr - offset;

      loadmap = info->main_executable_lm_info->map;
      for (seg = 0; seg < loadmap->nsegs; seg++)
	{
	  if (loadmap->segs[seg].p_vaddr <= orig_addr
	      && orig_addr < loadmap->segs[seg].p_vaddr + loadmap->segs[seg].p_memsz)
	    {
	      new_offsets->offsets[osect_idx]
		= loadmap->segs[seg].addr - loadmap->segs[seg].p_vaddr;

	      if (solib_fdpic_debug > 1)
		fprintf_unfiltered (gdb_stdlog,
				    "section %s[%d] relocate from %s to %s [%d]\n",
				    osect->the_bfd_section->name,
				    osect_idx,
				    hex_string_custom ((int)orig_addr, 8),
				    hex_string_custom ((int) (orig_addr
							      + new_offsets->offsets[osect_idx]),
						       8),
				    seg);

	      if (new_offsets->offsets[osect_idx] != offset)
		changed = 1;
	      break;
	    }
	}
    }
  /* Relocate if we have modifications.  */
  if (changed)
    objfile_relocate (symfile_objfile, new_offsets);
  do_cleanups (old_chain);
}

static int
enable_break (void)
{
  asection *interp_sect;

  /* Check we have an entry point.  */
  if (symfile_objfile == NULL)
    {
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog, "enable_break: No symbol file found.\n");
      return 0;
    }

  if (!symfile_objfile->ei.entry_point_p)
    {
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog, "enable_break: Symbol file has no entry point.\n");
      return 0;
    }

  /* Check for the presence of a .interp section.  If there is no
     such section, the executable is statically linked.  */

  interp_sect = bfd_get_section_by_name (exec_bfd, ".interp");
  if (interp_sect == NULL)
    {
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog, "enable_break: No .interp section found.\n");
      return 1;
    }

  create_solib_event_breakpoint (target_gdbarch, symfile_objfile->ei.entry_point);

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog, "enable_break: solib event breakpoint placed at : %s\n",
			hex_string_custom (symfile_objfile->ei.entry_point, 8));
  return 1;
}

static CORE_ADDR
main_got (void)
{
  struct minimal_symbol *got_sym;

  got_sym = lookup_minimal_symbol ("_GLOBAL_OFFSET_TABLE_",
				   NULL, symfile_objfile);
  if (got_sym == 0)
    return 0;

  return SYMBOL_VALUE_ADDRESS (got_sym);
}

static CORE_ADDR
lm_base (void)
{
  enum bfd_endian byte_order = gdbarch_byte_order (target_gdbarch);
  struct minimal_symbol *got_sym;
  CORE_ADDR addr;
  gdb_byte buf[4];
  struct fdpic_info *info = get_fdpic_info ();

  /* One of our assumptions is that the main executable has been relocated.
     Bail out if this has not happened.  (Note that post_create_inferior()
     in infcmd.c will call solib_add prior to solib_create_inferior_hook().
     If we allow this to happen, lm_base_cache will be initialized with
     a bogus value.  */
  if (info->main_executable_lm_info == 0)
    return 0;

  /* If we already have a cached value, return it.  */
  if (info->lm_base_cache)
    return info->lm_base_cache;

  got_sym = lookup_minimal_symbol ("_GLOBAL_OFFSET_TABLE_", NULL, symfile_objfile);
  if (got_sym == 0)
    {
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog,
			    "lm_base: _GLOBAL_OFFSET_TABLE_ not found.\n");
      return 0;
    }
  addr = SYMBOL_VALUE_ADDRESS (got_sym) + 8;

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"lm_base: _GLOBAL_OFFSET_TABLE_ + 8 = %s\n",
			hex_string_custom (addr, 8));

  if (target_read_memory (addr, buf, sizeof buf) != 0)
    return 0;

  info->lm_base_cache = extract_unsigned_integer (buf, sizeof buf, byte_order);

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"lm_base: lm_base_cache = %s\n",
			hex_string_custom (info->lm_base_cache, 8));

  return info->lm_base_cache;
}

static void
enable_break_failure_warning (void)
{
  warning (_("Unable to find dynamic linker breakpoint function.\n"
	     "GDB will be unable to debug shared library initializers\n"
	     "and track explicitly loaded dynamic code."));
}

static int
enable_break2 (void)
{
  asection *interp_sect;
  struct fdpic_info *info = get_fdpic_info ();

  if (exec_bfd == NULL)
    return 0;

  if (!target_has_execution)
    return 0;

  if (info->enable_break2_done)
    return 1;

  info->interp_text_sect_low = 0;
  info->interp_text_sect_high = 0;
  info->interp_plt_sect_low = 0;
  info->interp_plt_sect_high = 0;

  interp_sect = bfd_get_section_by_name (exec_bfd, ".interp");
  if (interp_sect)
    {
      unsigned int interp_sect_size;
      gdb_byte *buf;
      bfd *tmp_bfd = NULL;
      CORE_ADDR addr;
      struct elf32_fdpic_loadmap *ldm;
      volatile struct gdb_exception ex;

      /* Read the contents of the .interp section into a local buffer;
	 the contents specify the dynamic linker this program uses.  */
      interp_sect_size = bfd_section_size (exec_bfd, interp_sect);
      buf = alloca (interp_sect_size);
      bfd_get_section_contents (exec_bfd, interp_sect, buf, 0, interp_sect_size);

      /* Now we need to figure out where the dynamic linker was
	 loaded so that we can load its symbols and place a breakpoint
	 in the dynamic linker itself.  */

      TRY_CATCH (ex, RETURN_MASK_ALL)
	{
	  tmp_bfd = solib_bfd_open (buf);
	}
      if (tmp_bfd == NULL)
	{
	  enable_break_failure_warning ();
	  return 0;
	}
      ldm = fdpic_get_initial_loadmaps (1);

      /* Record the relocated start and end address of the dynamic linker
	 text and plt section for dsbt_in_dynsym_resolve_code.  */
      interp_sect = bfd_get_section_by_name (tmp_bfd, ".text");
      if (interp_sect)
	{
	  info->interp_text_sect_low = bfd_section_vma (tmp_bfd, interp_sect);
	  info->interp_text_sect_low
	    += displacement_from_map (ldm, info->interp_text_sect_low);
	  info->interp_text_sect_high
	    = info->interp_text_sect_low + bfd_section_size (tmp_bfd, interp_sect);
	}
      interp_sect = bfd_get_section_by_name (tmp_bfd, ".plt");
      if (interp_sect)
	{
	  info->interp_plt_sect_low = bfd_section_vma (tmp_bfd, interp_sect);
	  info->interp_plt_sect_low
	    += displacement_from_map (ldm, info->interp_plt_sect_low);
	  info->interp_plt_sect_high
	    = info->interp_plt_sect_low + bfd_section_size (tmp_bfd, interp_sect);
	}

      /* Got load address of _dl_debug_addr.  */
      addr = gdb_bfd_lookup_symbol (tmp_bfd, cmp_name, "_dl_debug_addr");
      if (addr == 0)
	{
	  warning (_("Could not find symbol _dl_debug_addr in dynamic linker"));
	  enable_break_failure_warning ();
	  xfree (ldm);
	  bfd_close (tmp_bfd);
	  return 0;
	}
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog,
			    "enable_break: _dl_debug_addr (prior to relocation) = %s\n",
			    hex_string_custom (addr, 8));
      addr += displacement_from_map (ldm, addr);
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog,
			    "enable_break: _dl_debug_addr (after relocation) = %s\n",
			    hex_string_custom (addr, 8));

      /* Read structure.  */
      if (target_read_memory (addr, (gdb_byte *) &addr, sizeof (addr)) != 0)
	warning (_("Unable to fetch contents of _dl_debug_addr "
		   "(at address %s) from dynamic linker"),
		 hex_string_custom (addr, 8));

      /* If it's zero, then the ldso hasn't initialized yet, and so
	 there are no shared libs yet loaded.  */
      if (addr == 0)
	{
	  if (solib_fdpic_debug)
	    fprintf_unfiltered (gdb_stdlog,
				"enable_break: ldso not yet initialized\n");
	  /* Do not warn, but mark to run again.  */
	  xfree (ldm);
	  bfd_close (tmp_bfd);
	  return 0;
	}
      /* Fetch the r_brk field.  It's 8 bytes from the start of dl_debug_addr.  */
      if (target_read_memory (addr + 8, (gdb_byte *) &addr, sizeof (addr)) != 0)
	{
	  warning (_("Unable to fetch _dl_debug_addr->r_brk(at address %s) from dynamic linker"),
		   hex_string_custom (addr + 8, 8));
	  enable_break_failure_warning ();
	  xfree (ldm);
	  bfd_close (tmp_bfd);
	  return 0;
	}
      /* At this point we have funcptr but we need to break on function code address.  */
      if (target_read_memory (addr, (gdb_byte *) &addr, sizeof (addr)) != 0)
	{
	  warning (_("Unable to fetch _dl_debug_addr->.r_brk entry point (at address %s) from dynamic linker"),
		   hex_string_custom (addr, 8));
	  enable_break_failure_warning ();
	  xfree (ldm);
	  bfd_close (tmp_bfd);
	  return 0;
	}

      /* We're done with the temporary bfd.  */
      bfd_close (tmp_bfd);

      /* We're also done with the loadmap.  */
      xfree (ldm);

      /* Remove all the solib event breakpoints.  Their addresses
	 may have changed since the last time we ran the program.  */
      /* TODO: we disable the removal of solib event bp since it may crash gdb when
       * enable_break2 is called in solib event context.  Since bp memory is returned
       * but read later in bpstat_stop_status() after handle_solib_event () call,
       * and if bp memory is reallocated and modified then we can have a crash.  */
      //remove_solib_event_breakpoints ();

      /* Now (finally!) create the solib breakpoint.  */
      /* In case we have a thumb entry then reset lower bits.  */
      addr &= ~1;
      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog,
			    "put solib breakpoint at %s\n",
			    hex_string_custom ((int) addr, 8));
      create_solib_event_breakpoint (target_gdbarch, addr);

      info->enable_break2_done = 1;

      return 1;
    }
  /* Tell the user we couldn't set a dynamic linker breakpoint.  */
  enable_break_failure_warning ();

  /* Failure return.  */
  return 0;
}

/* Exported functions.  */
/* Find the global pointer for the given function address ADDR.  */
CORE_ADDR
fdpic_find_global_pointer (CORE_ADDR addr)
{
  struct so_list *so;

  so = master_so_list ();
  while (so)
    {
      int seg;
      struct elf32_fdpic_loadmap *map;

      map = so->lm_info->map;

      for (seg = 0; seg < map->nsegs; seg++)
	{
	  if (map->segs[seg].addr <= addr
	      && addr < map->segs[seg].addr + map->segs[seg].p_memsz)
	    return so->lm_info->got_value;
	}

      so = so->next;
    }

  /* Didn't find it in any of the shared objects.
     So assume it's in the main executable.  */
  return main_got ();
}

/* Shared object operations.  */
static int
fdpic_in_dynsym_resolve_code (CORE_ADDR pc)
{
  struct fdpic_info *info = get_fdpic_info ();
  int res;

  res = ((pc >= info->interp_text_sect_low && pc < info->interp_text_sect_high)
	 || (pc >= info->interp_plt_sect_low && pc < info->interp_plt_sect_high)
	 || in_plt_section (pc, NULL)
	 || in_gnu_ifunc_stub (pc));

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"%s\n : pc = %s => %d\n", __FUNCTION__,
			hex_string_custom ((int)pc, 8), res);

  return res;
}

static int
fdpic_open_symbol_file_object (void *from_ttyp)
{
  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"%s : from_ttyp = %s\n", __FUNCTION__,
			hex_string_custom ((long) from_ttyp, 16));

  /* Unimplemented.  */
  return 0;
}

static struct so_list *
fdpic_current_sos (void)
{
  /* TODO: endianess support.  */
  struct fdpic_info *info = get_fdpic_info ();
  CORE_ADDR lm_addr;
  CORE_ADDR mgot;
  struct so_list *sos_head = NULL;
  struct so_list **sos_next_ptr = &sos_head;

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog, "%s\n", __FUNCTION__);

  /* Make sure that the main executable has been relocated.  This is
     required in order to find the address of the global offset table,
     which in turn is used to find the link map info.  (See lm_base
     for details.)

     Note that the relocation of the main executable is also performed
     by SOLIB_CREATE_INFERIOR_HOOK, however, in the case of core
     files, this hook is called too late in order to be of benefit to
     SOLIB_ADD.  SOLIB_ADD eventually calls this function,
     dsbt_current_sos, and also precedes the call to
     SOLIB_CREATE_INFERIOR_HOOK.   (See post_create_inferior in
     infcmd.c.)  */
  if (info->main_executable_lm_info == 0 && core_bfd != NULL)
    fdpic_relocate_main_executable ();

  /* Check for a static binary.  */
  if (info->is_static_binary)
    return NULL;

  /* Locate the address of the first link map struct.  */
  lm_addr = lm_base ();

  /* Fetch the GOT corresponding to the main executable.  */
  mgot = main_got ();

  while (lm_addr)
    {
      int errcode;
      struct link_map lm_buf;
      char *name_buf;
      struct so_list *sop;
      CORE_ADDR got_addr;

      if (solib_fdpic_debug)
	fprintf_unfiltered (gdb_stdlog,
			    " Reading link map from %s\n",
			    hex_string_custom ((int) lm_addr, 8));

      /* Read link map.  */
      if (target_read_memory (lm_addr, (gdb_byte *) &lm_buf, sizeof (lm_buf)) != 0)
	{
	  warning (_("fdpic_current_sos: Unable to read link map entry."
		     " Shared object chain may be incomplete."));
	  break;
	}

      /* Don't add main executable.  */
      if ((CORE_ADDR) lm_buf.l_addr.got_value != mgot)
	{
	  struct elf32_fdpic_loadmap *loadmap;

	  loadmap = fetch_loadmap ((CORE_ADDR) lm_buf.l_addr.map);
	  if (loadmap == NULL)
	    {
	      warning (_("fdpic_current_sos: Unable to fetch load map."
			 " Shared object chain may be incomplete."));
	      break;
	    }
	  sop = xcalloc (1, sizeof (struct so_list));
	  sop->lm_info = xcalloc (1, sizeof (struct lm_info));
	  sop->lm_info->map = loadmap;
	  sop->lm_info->got_value = (CORE_ADDR) lm_buf.l_addr.got_value;
	  sop->lm_info->lm_addr = (CORE_ADDR) lm_buf.l_addr.map;
	  /* Fetch the name.  */
	  target_read_string ((CORE_ADDR) lm_buf.l_name, &name_buf,
			      SO_NAME_MAX_PATH_SIZE - 1, &errcode);
	  if (errcode != 0)
	    warning (_("Can't read pathname for link map entry: %s."),
		     safe_strerror (errcode));
	  else
	    {
	      if (solib_fdpic_debug)
		fprintf_unfiltered (gdb_stdlog, "  so name %s\n", name_buf);
	      strncpy (sop->so_name, name_buf, SO_NAME_MAX_PATH_SIZE - 1);
	      sop->so_name[SO_NAME_MAX_PATH_SIZE - 1] = '\0';
	      xfree (name_buf);
	      strcpy (sop->so_original_name, sop->so_name);
	    }
	  *sos_next_ptr = sop;
	  sos_next_ptr = &sop->next;
	}
      else
	/* Main executable.  */
	info->main_lm_addr = lm_addr;

      /* Read next entry.  */
      lm_addr = (CORE_ADDR) lm_buf.l_next;
    }

  enable_break2 ();

  return sos_head;
}

static void
fdpic_special_symbol_handling (void)
{
  if (solib_fdpic_debug > 2)
    fprintf_unfiltered (gdb_stdlog, "%s\n", __FUNCTION__);

  /* Nothing to do.  */
}

static void
fdpic_solib_create_inferior_hook (int from_tty)
{
  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"%s\n : from_tty = %d\n", __FUNCTION__, from_tty);

  /* Relocate main executable.  */
  fdpic_relocate_main_executable ();

  /* Enable shared library breakpoints.  */
  if (!enable_break ())
    {
      warning (_("shared library handler failed to enable breakpoint"));
      return;
    }
}

static void
fdpic_clear_solib (void)
{
  struct fdpic_info *info = get_fdpic_info ();

  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog, "%s\n", __FUNCTION__);

  info->lm_base_cache = 0;
  info->enable_break2_done = 0;
  info->main_lm_addr = 0;
  info->is_static_binary = 0;
  if (info->main_executable_lm_info != 0)
    {
      xfree (info->main_executable_lm_info->map);
      xfree (info->main_executable_lm_info);
      info->main_executable_lm_info = 0;
    }
}

static void
fdpic_free_so (struct so_list *so)
{
  if (solib_fdpic_debug)
    fprintf_unfiltered (gdb_stdlog,
			"%s : so = %s\n", __FUNCTION__,
			hex_string_custom ((long)so, 16));

  xfree (so->lm_info->map);
  xfree (so->lm_info);
}

static void
fdpic_relocate_section_addresses (struct so_list *so,
				  struct target_section *sec)
{
  int seg;
  struct elf32_fdpic_loadmap *map;

  if (solib_fdpic_debug > 1)
    fprintf_unfiltered (gdb_stdlog,
			"%s : so = %s / sec = %s\n", __FUNCTION__,
			hex_string_custom ((long)so, 16),
			hex_string_custom ((long)sec, 16));

  map = so->lm_info->map;

  for (seg = 0; seg < map->nsegs; seg++)
    {
      if (map->segs[seg].p_vaddr <= sec->addr
	  && sec->addr < map->segs[seg].p_vaddr + map->segs[seg].p_memsz)
	{
	  CORE_ADDR displ = map->segs[seg].addr - map->segs[seg].p_vaddr;

	  sec->addr += displ;
	  sec->endaddr += displ;
	  break;
	}
    }
}

/* Given an objfile, return the address of its link map.  This value is
   needed for TLS support.  */
CORE_ADDR
fdpic_fetch_objfile_link_map (struct objfile *objfile)
{
  struct fdpic_info *info = get_fdpic_info ();
  struct so_list *so;

  /* Cause fdpic_current_sos() to be run if it hasn't been already.  */
  if (info->main_lm_addr == 0)
    solib_add (0, 0, 0, 1);

  /* fdpic_current_sos() will set main_lm_addr for the main executable.  */
  if (objfile == symfile_objfile)
    return info->main_lm_addr;

  /* The other link map addresses may be found by examining the list
     of shared libraries.  */
  for (so = master_so_list (); so; so = so->next)
    {
      if (so->objfile == objfile)
	return so->lm_info->lm_addr;
    }

  /* Not found!  */
  return 0;
}

struct target_so_ops fdpic_so_ops;

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_fdpic_solib;

void
_initialize_fdpic_solib (void)
{

  solib_fdpic_pspace_data
    = register_program_space_data_with_cleanup (fdpic_pspace_data_cleanup);

  fdpic_so_ops.relocate_section_addresses = fdpic_relocate_section_addresses;
  fdpic_so_ops.free_so = fdpic_free_so;
  fdpic_so_ops.clear_solib = fdpic_clear_solib;
  fdpic_so_ops.solib_create_inferior_hook = fdpic_solib_create_inferior_hook;
  fdpic_so_ops.special_symbol_handling = fdpic_special_symbol_handling;
  fdpic_so_ops.current_sos = fdpic_current_sos;
  fdpic_so_ops.open_symbol_file_object = fdpic_open_symbol_file_object;
  fdpic_so_ops.in_dynsym_resolve_code = fdpic_in_dynsym_resolve_code;
  fdpic_so_ops.bfd_open = solib_bfd_open;

  /* Debug this file's internals.  */
  add_setshow_zinteger_cmd ("solib-fdpic", class_maintenance,
			    &solib_fdpic_debug, _("\
Set internal debugging of shared library code for FDPIC."), _("\
Show internal debugging of shared library code for FDPIC."), _("\
When non-zero, FDPIC solib specific internal debugging is enabled."),
			    NULL,
			    NULL,
			    &setdebuglist, &showdebuglist);
}
