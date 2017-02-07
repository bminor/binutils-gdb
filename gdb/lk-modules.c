/* Handle Linux kernel modules as shared libraries.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#include "common/filestuff.h"
#include "filenames.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_regex.h"
#include "lk-lists.h"
#include "lk-low.h"
#include "lk-modules.h"
#include "objfiles.h"
#include "observer.h"
#include "readline/readline.h"
#include "solib.h"
#include "solist.h"
#include "utils.h"

#include <unordered_map>
#include <string>

struct target_so_ops *lk_modules_so_ops = NULL;

/* Info for single section type.  */

struct lm_info_sec
{
  CORE_ADDR start;
  CORE_ADDR offset;
  unsigned int size;
};

/* Link map info to include in an allocated so_list entry.  */

struct lm_info
{
  CORE_ADDR base;
  unsigned int size;

  struct lm_info_sec text;
  struct lm_info_sec init_text;
  struct lm_info_sec ro_data;
  struct lm_info_sec rw_data;
  struct lm_info_sec percpu;
};

/* Check if debug info for module NAME are loaded.  */

bool
lk_modules_debug_info_loaded (const std::string &name)
{
  struct so_list *so;

  for (so = master_so_list (); so; so = so->next)
    {
      if (name == so->so_original_name)
	return (so->symbols_loaded && objfile_has_symbols (so->objfile));
    }

  return false;
}

/* Replace tags, like '$release', with corresponding data in
   solib_search_path.

   Known tags:
   $release	Linux kernel release, same as 'uname -r'

   Returns the expanded path.  */

static std::string
lk_modules_expand_search_path ()
{
  char release[LK_UTS_NAME_LEN + 1];
  CORE_ADDR utsname;

  utsname = LK_ADDR (init_uts_ns) + LK_OFFSET (uts_namespace, name);
  read_memory_string (utsname + LK_OFFSET (utsname, release),
		      release, LK_UTS_NAME_LEN);
  release[LK_UTS_NAME_LEN] = '\0';

  std::string search_path = get_solib_search_path ();
  substitute_path_component (search_path, "$release", release);

  return search_path;
}

/* With kernel modules there is the problem that the kernel only stores
   the modules name but not the path from wich it was loaded from.
   Thus we need to map the name to a path GDB can read from.  We use file
   modules.order to do so.  It is created by kbuild containing the order in
   which the modules appear in the Makefile and is also used by modprobe.
   The drawback of this method is that it needs the modules.order file and
   all relative paths, starting from <solib-search-path>, must be exactly the
   same as decribed in it.  */

/* Open file <solib-search-path>/modules.order and return its file
   pointer.  */

FILE *
lk_modules_open_mod_order ()
{
  FILE *mod_order;
  std::string filename = concat_path (lk_modules_expand_search_path (),
				      "modules.order");
  mod_order = gdb_fopen_cloexec (filename.c_str (), "r");

  if (!mod_order)
    {
      error (_("\
Can not find file module.order at %s \
to load module symbol files.\n\
Please check if solib-search-path is set correctly."),
	     filename.c_str ());
    }

  return mod_order;
}

/* Build map between module name and path to binary file by reading file
   modules.order.  Returns unordered_map with module name as key and its
   path as value.  */

std::unordered_map<std::string, std::string>
lk_modules_build_path_map ()
{
  std::unordered_map<std::string, std::string> umap;
  FILE *mod_order;
  struct cleanup *old_chain;
  char line[SO_NAME_MAX_PATH_SIZE + 1];

  mod_order = lk_modules_open_mod_order ();
  old_chain = make_cleanup_fclose (mod_order);

  line[SO_NAME_MAX_PATH_SIZE] = '\0';
  std::string search_path = lk_modules_expand_search_path ();
  while (fgets (line, SO_NAME_MAX_PATH_SIZE, mod_order))
    {
      /* Remove trailing newline.  */
      line[strlen (line) - 1] = '\0';

      std::string name = lbasename (line);

      /* 3 = strlen (".ko").  */
      if (!endswith (name.c_str (), ".ko")
	  || name.length () >= LK_MODULE_NAME_LEN + 3)
	continue;

      name = name.substr (0, name.length () - 3);

      /* Kernel modules are named after the files they are stored in with
	 all minus '-' replaced by underscore '_'.  Do the same to enable
	 mapping.  */
      for (size_t p = name.find('-'); p != std::string::npos;
	   p = name.find ('-', p + 1))
	name[p] = '_';

      umap[name] = concat_path(search_path, line);
    }

  do_cleanups (old_chain);
  return umap;
}

/* Allocate and fill a copy of struct lm_info for module at address MOD.  */

struct lm_info *
lk_modules_read_lm_info (CORE_ADDR mod)
{
  struct lm_info *lmi = XNEW (struct lm_info);
  struct cleanup *old_chain = make_cleanup (xfree, lmi);

  if (LK_FIELD (module, module_core)) /* linux -4.4 */
    {
      lmi->base = lk_read_addr (mod + LK_OFFSET (module, module_core));
      lmi->size = lk_read_addr (mod + LK_OFFSET (module, core_size));

      lmi->text.start = lmi->base;
      lmi->text.offset = LK_HOOK->get_module_text_offset (mod);
      lmi->text.size = lk_read_uint (mod + LK_OFFSET (module, core_text_size));

      lmi->ro_data.start = lmi->base + lmi->text.size;
      lmi->ro_data.offset = 0;
      lmi->ro_data.size =  lk_read_uint (mod + LK_OFFSET (module,
							  core_ro_size));
    }
  else /* linux 4.5+ */
    {
      CORE_ADDR mod_core = mod + LK_OFFSET (module, core_layout);

      lmi->base = lk_read_addr (mod_core
				+ LK_OFFSET (module_layout, base));
      lmi->size = lk_read_uint (mod_core
				+ LK_OFFSET (module_layout, size));

      lmi->text.start = lmi->base;
      lmi->text.offset = LK_HOOK->get_module_text_offset (mod);
      lmi->text.size = lk_read_uint (mod_core
				     + LK_OFFSET (module_layout, text_size));

      lmi->ro_data.start = lmi->base + lmi->text.size;
      lmi->ro_data.offset = 0;
      lmi->ro_data.size =  lk_read_uint (mod_core
					 + LK_OFFSET (module_layout, ro_size));
    }

  lmi->rw_data.start = lmi->base + lmi->ro_data.size;
  lmi->rw_data.offset = 0;
  lmi->rw_data.size = lmi->size - lmi->ro_data.size;

  lmi->init_text.start = lk_read_addr (mod + LK_OFFSET (module, init));
  lmi->init_text.offset = 0;

  lmi->percpu.start = lk_read_addr (mod + LK_OFFSET (module, percpu));
  lmi->percpu.size = lk_read_uint (mod + LK_OFFSET (module, percpu_size));
  lmi->percpu.offset = 0;

  discard_cleanups (old_chain);
  return lmi;
}

/* Function for current_sos hook.  */

struct so_list *
lk_modules_current_sos (void)
{
  CORE_ADDR modules, next;
  FILE *mod_order;
  struct so_list *list = NULL;
  std::unordered_map<std::string, std::string> umap;

  umap = lk_modules_build_path_map ();
  modules = LK_ADDR (modules);
  lk_list_for_each (next, modules, module, list)
    {
      char name[LK_MODULE_NAME_LEN];
      CORE_ADDR mod, name_addr;

      mod = LK_CONTAINER_OF (next, module, list);
      name_addr = mod + LK_OFFSET (module, name);
      read_memory_string (name_addr, name, LK_MODULE_NAME_LEN);

      if (umap.count (name))
	{
	  struct so_list *newso = XCNEW (struct so_list);

	  newso->next = list;
	  list = newso;
	  newso->lm_info = lk_modules_read_lm_info (mod);
	  strncpy (newso->so_original_name, name, SO_NAME_MAX_PATH_SIZE);
	  strncpy (newso->so_name, umap[name].c_str (), SO_NAME_MAX_PATH_SIZE);
	  newso->pspace = current_program_space;
	}
    }

  return list;
}

/* Relocate target_section SEC to section type LMI_SEC.  Helper function for
   lk_modules_relocate_section_addresses.  */

void
lk_modules_relocate_sec (struct target_section *sec,
			 struct lm_info_sec *lmi_sec)
{
  unsigned int alignment = 1;

 alignment = 1 << sec->the_bfd_section->alignment_power;

  /* Adjust offset to section alignment.  */
  if (lmi_sec->offset % alignment != 0)
    lmi_sec->offset += alignment - (lmi_sec->offset % alignment);

  sec->addr += lmi_sec->start + lmi_sec->offset;
  sec->endaddr += lmi_sec->start + lmi_sec->offset;
  lmi_sec->offset += sec->endaddr - sec->addr;
}

/* Function for relocate_section_addresses hook.  */

void
lk_modules_relocate_section_addresses (struct so_list *so,
				       struct target_section *sec)
{
  struct lm_info *lmi = so->lm_info;
  unsigned int flags = sec->the_bfd_section->flags;
  const char *name = sec->the_bfd_section->name;

  if (streq (name, ".modinfo") || streq (name, "__versions"))
      return;

  /* FIXME: Make dependent on module state, i.e. only map .init sections if
   * state is MODULE_STATE_COMING.  */
  if (startswith (name, ".init"))
    lk_modules_relocate_sec (sec, &lmi->init_text);
  else if (endswith (name, ".percpu"))
    lk_modules_relocate_sec (sec, &lmi->percpu);
  else if (flags & SEC_CODE)
    lk_modules_relocate_sec (sec, &lmi->text);
  else if (flags & SEC_READONLY)
    lk_modules_relocate_sec (sec, &lmi->ro_data);
  else if (flags & SEC_ALLOC)
    lk_modules_relocate_sec (sec, &lmi->rw_data);

  /* Set address range to be displayed with info shared.
     size = text + (ro + rw) data without .init sections.  */
  if (so->addr_low == so->addr_high)
    {
      so->addr_low = lmi->base;
      so->addr_high = lmi->base + lmi->size;
    }
}

/* Function for free_so hook.  */

void
lk_modules_free_so (struct so_list *so)
{
  xfree (so->lm_info);
}

/* Function for clear_so hook.  */

void
lk_modules_clear_so (struct so_list *so)
{
  if (so->lm_info != NULL)
    memset (so->lm_info, 0, sizeof (struct lm_info));
}

/* Function for clear_solib hook.  */

void
lk_modules_clear_solib ()
{
  /* Nothing to do.  */
}

/* Function for clear_create_inferior_hook hook.  */

void
lk_modules_create_inferior_hook (int from_tty)
{
  /* Nothing to do.  */
}

/* Function for clear_create_inferior_hook hook.  */

int
lk_modules_in_dynsym_resolve_code (CORE_ADDR pc)
{
  return 0;
}

/* Function for same hook.  */

int
lk_modules_same (struct so_list *gdb, struct so_list *inf)
{
  return streq (gdb->so_name, inf->so_name);
}

/* Initialize linux modules solib target.  */

void
init_lk_modules_so_ops (void)
{
  struct target_so_ops *t;

  if (lk_modules_so_ops != NULL)
    return;

  t = XCNEW (struct target_so_ops);
  t->relocate_section_addresses = lk_modules_relocate_section_addresses;
  t->free_so = lk_modules_free_so;
  t->clear_so = lk_modules_clear_so;
  t->clear_solib = lk_modules_clear_solib;
  t->solib_create_inferior_hook = lk_modules_create_inferior_hook;
  t->current_sos = lk_modules_current_sos;
  t->bfd_open = solib_bfd_open;
  t->in_dynsym_resolve_code = lk_modules_in_dynsym_resolve_code;
  t->same = lk_modules_same;

  lk_modules_so_ops = t;
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_lk_modules;

void
_initialize_lk_modules (void)
{
  init_lk_modules_so_ops ();
}
