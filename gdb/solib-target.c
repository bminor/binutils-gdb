/* Definitions for targets which report shared library events.

   Copyright (C) 2006
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "solist.h"
#include "symtab.h"
#include "symfile.h"
#include "target.h"

#include "gdb_string.h"

struct lm_info
{
  CORE_ADDR textSeg, dataSeg;
};

static struct so_list *solib_start;
static int solibs_fetched;

static struct so_list *
solib_target_current_sos (void)
{
  struct so_list *sop;
  struct so_list *start = NULL;
  struct so_list *last = NULL;

  /* If we have not asked the target for the list of shared libraries
     yet, do it now.  */
  if (!solibs_fetched)
    {
      solibs_fetched = 1;
      target_get_shared_libraries ();
    }

  for (sop = solib_start; sop; sop = sop->next)
    {
      struct so_list *new;

      /* Duplicate the recorded solib.  */
      new = XZALLOC (struct so_list);
      strcpy (new->so_name, sop->so_name);
      strcpy (new->so_original_name, sop->so_original_name);
      new->lm_info = XMALLOC (struct lm_info);
      *new->lm_info = *sop->lm_info;

      /* Add it to the list.  */
      if (!start)
	last = start = new;
      else
	{
	  last->next = new;
	  last = new;
	}
    }

  return start;
}

static void
solib_target_special_symbol_handling (void)
{
  /* Nothing needed.  */
}

static void
solib_target_solib_create_inferior_hook (void)
{
  /* Nothing needed.  */
}

static void
solib_target_clear_solib (void)
{
  struct so_list *sop, *next;

  for (sop = solib_start; sop; sop = next)
    {
      next = sop->next;

      free_so (sop);
    }

  solib_start = NULL;
  solibs_fetched = 0;
}

static void
solib_target_free_so (struct so_list *so)
{
  xfree (so->lm_info);
}

static void
solib_target_relocate_section_addresses (struct so_list *so,
					 struct section_table *sec)
{
  int flags = bfd_get_section_flags (sec->bfd, sec->the_bfd_section);
  CORE_ADDR offset;

  offset = symfile_section_offset_from_segment (sec->bfd, sec->the_bfd_section,
						so->lm_info->textSeg,
						so->lm_info->dataSeg);

  sec->addr += offset;
  sec->endaddr += offset;

  /* If we haven't set these yet, do so now.  If this fails, we may waste some
     cycles uselessly retrying it, but that is rare and harmless.  */
  if (so->addr_low == 0 && so->addr_high == 0)
    {
      CORE_ADDR text_len, data_len;

      symfile_find_segment_lengths (sec->bfd, &text_len, &data_len);

      if (text_len)
	{
	  so->addr_low = so->lm_info->textSeg;
	  so->addr_high = so->addr_low + text_len;
	}
      else if (data_len)
	{
	  so->addr_low = so->lm_info->dataSeg;
	  so->addr_high = so->addr_low + data_len;
	}
    }
}

static int
solib_target_open_symbol_file_object (void *from_ttyp)
{
  /* We can't locate the main symbol file based on the target's
     knowledge; the user has to specify it.  */
  return 0;
}

static int
solib_target_in_dynsym_resolve_code (CORE_ADDR pc)
{
  /* Assume there isn't target dynsym resolution code.  DLL targets
     generally have only import stubs (which GDB treats as "PLT entries"),
     and no runtime binding code.  */
  return 0;
}

static void
solib_target_add_one_solib (char *soname, CORE_ADDR textSeg,
			    CORE_ADDR dataSeg)
{
  struct so_list *new_solib, *so;

  /* We should already have queried the target for shared libraries
     before this point.  If we haven't, we may have just connected;
     we'll be querying shortly.  */
  if (!solibs_fetched)
    return;

  /* Check for duplicates already on the list.  This can happen, for
     instance, if we are stopped at a DLL load event when we first
     connect to a remote target: the DLL will already be in the
     queried list, but also be reported by the initial wait.  */
  for (so = solib_start; so; so = so->next)
    if (strcmp (so->so_name, soname) == 0
	&& so->lm_info->textSeg == textSeg
	&& so->lm_info->dataSeg == dataSeg)
      return;

  new_solib = XZALLOC (struct so_list);
  strncpy (new_solib->so_name, soname, SO_NAME_MAX_PATH_SIZE - 1);
  new_solib->so_name[SO_NAME_MAX_PATH_SIZE - 1] = '\0';
  strncpy (new_solib->so_original_name, soname, SO_NAME_MAX_PATH_SIZE - 1);
  new_solib->so_original_name[SO_NAME_MAX_PATH_SIZE - 1] = '\0';

  new_solib->lm_info = XZALLOC (struct lm_info);
  new_solib->lm_info->textSeg = textSeg;
  new_solib->lm_info->dataSeg = dataSeg;

  if (solib_start == NULL)
    solib_start = new_solib;
  else
    {
      so = solib_start;
      while (so->next)
	so = so->next;
      so->next = new_solib;
    }

  /* We do not trigger symbol reading here; the target will do it,
     after all load events have been processed.  */
}

static void
solib_target_remove_one_solib (char *soname, CORE_ADDR textSeg,
			       CORE_ADDR dataSeg)
{
  struct so_list **slot, *removed;

  /* We should already have queried the target for shared libraries
     before this point.  If we haven't, we may have just connected;
     we'll be querying shortly.  */
  if (!solibs_fetched)
    return;

  slot = &solib_start;
  while (*slot)
    {
      if (textSeg != ~(CORE_ADDR) 0 && textSeg != (*slot)->lm_info->textSeg)
	continue;
      if (dataSeg != ~(CORE_ADDR) 0 && dataSeg != (*slot)->lm_info->dataSeg)
	continue;
      if (soname != NULL && strcmp (soname, (*slot)->so_name) != 0)
	continue;
      break;
    }

  if (*slot == NULL)
    return;

  removed = *slot;
  *slot = removed->next;

  free_so (removed);

  /* We do not trigger symbol unloading here; the target will do it,
     after all unload events have been processed.  */
}

static struct target_so_ops solib_target_so_ops;

extern initialize_file_ftype _initialize_solib_target; /* -Wmissing-prototypes */

void
_initialize_solib_target (void)
{
  solib_target_so_ops.relocate_section_addresses
    = solib_target_relocate_section_addresses;
  solib_target_so_ops.free_so = solib_target_free_so;
  solib_target_so_ops.clear_solib = solib_target_clear_solib;
  solib_target_so_ops.solib_create_inferior_hook
    = solib_target_solib_create_inferior_hook;
  solib_target_so_ops.special_symbol_handling
    = solib_target_special_symbol_handling;
  solib_target_so_ops.current_sos = solib_target_current_sos;
  solib_target_so_ops.open_symbol_file_object
    = solib_target_open_symbol_file_object;
  solib_target_so_ops.in_dynsym_resolve_code
    = solib_target_in_dynsym_resolve_code;
  solib_target_so_ops.add_one_solib = solib_target_add_one_solib;
  solib_target_so_ops.remove_one_solib = solib_target_remove_one_solib;

  current_target_so_ops = &solib_target_so_ops;
}
