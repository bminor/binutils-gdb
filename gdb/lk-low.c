/* Basic Linux kernel support, architecture independent.

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

#include "block.h"
#include "exceptions.h"
#include "frame.h"
#include "gdbarch.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "gdbtypes.h"
#include "inferior.h"
#include "lk-lists.h"
#include "lk-low.h"
#include "lk-modules.h"
#include "objfiles.h"
#include "observer.h"
#include "solib.h"
#include "target.h"
#include "value.h"

#include <algorithm>

struct target_ops *linux_kernel_ops = NULL;

/* Initialize a private data entry for an address, where NAME is the name
   of the symbol, i.e. variable name in Linux, ALIAS the name used to
   retrieve the entry from hashtab, and SILENT a flag to determine if
   errors should be ignored.

   Returns a pointer to the new entry.  In case of an error, either returns
   NULL (SILENT = TRUE) or throws an error (SILENT = FALSE).  If SILENT = TRUE
   the caller is responsible to check for errors.

   Do not use directly, use LK_DECLARE_* macros defined in lk-low.h instead.  */

struct lk_private_data *
lk_init_addr (const char *name, const char *alias, int silent)
{
  struct lk_private_data *data;
  struct bound_minimal_symbol bmsym;
  void **new_slot;
  void *old_slot;

  if ((old_slot = lk_find (alias)) != NULL)
    return (struct lk_private_data *) old_slot;

  bmsym = lookup_minimal_symbol (name, NULL, NULL);

  if (bmsym.minsym == NULL)
    {
      if (!silent)
	error (_("Could not find address %s.  Aborting."), alias);
      return NULL;
    }

  data = XCNEW (struct lk_private_data);
  data->alias = alias;
  data->data.addr = BMSYMBOL_VALUE_ADDRESS (bmsym);

  new_slot = lk_find_slot (alias);
  *new_slot = data;

  return data;
}

/* Same as lk_init_addr but for structs.  */

struct lk_private_data *
lk_init_struct (const char *name, const char *alias, int silent)
{
  struct lk_private_data *data;
  const struct block *global;
  const struct symbol *sym;
  struct type *type;
  void **new_slot;
  void *old_slot;

  if ((old_slot = lk_find (alias)) != NULL)
    return (struct lk_private_data *) old_slot;

  global = block_global_block(get_selected_block (0));
  sym = lookup_symbol (name, global, STRUCT_DOMAIN, NULL).symbol;

  if (sym != NULL)
    {
      type = SYMBOL_TYPE (sym);
      goto out;
    }

  /*  Chek for "typedef struct { ... } name;"-like definitions.  */
  sym = lookup_symbol (name, global, VAR_DOMAIN, NULL).symbol;
  if (sym == NULL)
    goto error;

  type = check_typedef (SYMBOL_TYPE (sym));

  if (TYPE_CODE (type) == TYPE_CODE_STRUCT)
    goto out;

error:
  if (!silent)
    error (_("Could not find %s.  Aborting."), alias);

  return NULL;

out:
  data = XCNEW (struct lk_private_data);
  data->alias = alias;
  data->data.type = type;

  new_slot = lk_find_slot (alias);
  *new_slot = data;

  return data;
}

/* Nearly the same as lk_init_addr, with the difference that two names are
   needed, i.e. the struct name S_NAME containing the field with name
   F_NAME.  */

struct lk_private_data *
lk_init_field (const char *s_name, const char *f_name,
	       const char *s_alias, const char *f_alias,
	       int silent)
{
  struct lk_private_data *data;
  struct lk_private_data *parent;
  struct field *first, *last, *field;
  void **new_slot;
  void *old_slot;

  if ((old_slot = lk_find (f_alias)) != NULL)
    return (struct lk_private_data *) old_slot;

  parent = lk_find (s_alias);
  if (parent == NULL)
    {
      parent = lk_init_struct (s_name, s_alias, silent);

      /* Only SILENT == true needed, as otherwise lk_init_struct would throw
	 an error.  */
      if (parent == NULL)
	return NULL;
    }

  first = TYPE_FIELDS (parent->data.type);
  last = first + TYPE_NFIELDS (parent->data.type);
  for (field = first; field < last; field ++)
    {
      if (streq (field->name, f_name))
	break;
    }

  if (field == last)
    {
      if (!silent)
	error (_("Could not find field %s->%s.  Aborting."), s_alias, f_name);
      return NULL;
    }

  data = XCNEW (struct lk_private_data);
  data->alias = f_alias;
  data->data.field = field;

  new_slot = lk_find_slot (f_alias);
  *new_slot = data;

  return data;
}

/* Map cpu number CPU to the original PTID from target beneath.  */

static ptid_t
lk_cpu_to_old_ptid (const int cpu)
{
  struct lk_ptid_map *ptid_map;

  for (ptid_map = LK_PRIVATE->old_ptid; ptid_map;
       ptid_map = ptid_map->next)
    {
      if (ptid_map->cpu == cpu)
	return ptid_map->old_ptid;
    }

  error (_("Could not map CPU %d to original PTID.  Aborting."), cpu);
}

/* Helper functions to read and return basic types at a given ADDRess.  */

/* Read and return the integer value at address ADDR.  */

int
lk_read_int (CORE_ADDR addr)
{
  size_t int_size = lk_builtin_type_size (int);
  enum bfd_endian endian = gdbarch_byte_order (current_inferior ()->gdbarch);
  return read_memory_integer (addr, int_size, endian);
}

/* Read and return the unsigned integer value at address ADDR.  */

unsigned int
lk_read_uint (CORE_ADDR addr)
{
  size_t uint_size = lk_builtin_type_size (unsigned_int);
  enum bfd_endian endian = gdbarch_byte_order (current_inferior ()->gdbarch);
  return read_memory_integer (addr, uint_size, endian);
}

/* Read and return the long integer value at address ADDR.  */

LONGEST
lk_read_long (CORE_ADDR addr)
{
  size_t long_size = lk_builtin_type_size (long);
  enum bfd_endian endian = gdbarch_byte_order (current_inferior ()->gdbarch);
  return read_memory_integer (addr, long_size, endian);
}

/* Read and return the unsigned long integer value at address ADDR.  */

ULONGEST
lk_read_ulong (CORE_ADDR addr)
{
  size_t ulong_size = lk_builtin_type_size (unsigned_long);
  enum bfd_endian endian = gdbarch_byte_order (current_inferior ()->gdbarch);
  return read_memory_unsigned_integer (addr, ulong_size, endian);
}

/* Read and return the address value at address ADDR.  */

CORE_ADDR
lk_read_addr (CORE_ADDR addr)
{
  return (CORE_ADDR) lk_read_ulong (addr);
}

/* Reads a bitmap at a given ADDRess of size SIZE (in bits). Allocates and
   returns an array of ulongs.  The caller is responsible to free the array
   after it is no longer needed.  */

ULONGEST *
lk_read_bitmap (CORE_ADDR addr, size_t size)
{
  ULONGEST *bitmap;
  size_t ulong_size, len;

  ulong_size = lk_builtin_type_size (unsigned_long);
  len = LK_DIV_ROUND_UP (size, ulong_size * LK_BITS_PER_BYTE);
  bitmap = XNEWVEC (ULONGEST, len);

  for (size_t i = 0; i < len; i++)
    bitmap[i] = lk_read_ulong (addr + i * ulong_size);

  return bitmap;
}

/* Return the next set bit in bitmap BITMAP of size SIZE (in bits)
   starting from bit (index) BIT.  Return SIZE when the end of the bitmap
   was reached.  To iterate over all set bits use macro
   LK_BITMAP_FOR_EACH_SET_BIT defined in lk-low.h.  */

size_t
lk_bitmap_find_next_bit (ULONGEST *bitmap, size_t size, size_t bit)
{
  size_t ulong_size, bits_per_ulong, elt;

  ulong_size = lk_builtin_type_size (unsigned_long);
  bits_per_ulong = ulong_size * LK_BITS_PER_BYTE;
  elt = bit / bits_per_ulong;

  while (bit < size)
    {
      /* FIXME: Explain why using lsb0 bit order.  */
      if (bitmap[elt] & (1UL << (bit % bits_per_ulong)))
	return bit;

      bit++;
      if (bit % bits_per_ulong == 0)
	elt++;
    }

  return size;
}

/* Returns the Hamming weight, i.e. number of set bits, of bitmap BITMAP
   with size SIZE (in bits).  */

size_t
lk_bitmap_hweight (ULONGEST *bitmap, size_t size)
{
  size_t ulong_size, bit, bits_per_ulong, elt, retval;

  ulong_size = lk_builtin_type_size (unsigned_long);
  bits_per_ulong = ulong_size * LK_BITS_PER_BYTE;
  elt = bit = 0;
  retval = 0;

  while (bit < size)
    {
      if (bitmap[elt] & (1 << bit % bits_per_ulong))
	retval++;

      bit++;
      if (bit % bits_per_ulong == 0)
	elt++;
    }

  return retval;
}

/* Provide the per_cpu_offset of cpu CPU.  See comment in lk-low.h for
   details.  */

CORE_ADDR
lk_get_percpu_offset (unsigned int cpu)
{
  size_t ulong_size = lk_builtin_type_size (unsigned_long);
  CORE_ADDR percpu_elt;

  /* Give the architecture a chance to overwrite default behaviour.  */
  if (LK_HOOK->get_percpu_offset)
      return LK_HOOK->get_percpu_offset (cpu);

  percpu_elt = LK_ADDR (__per_cpu_offset) + (ulong_size * cpu);
  return lk_read_addr (percpu_elt);
}


/* Test if a given task TASK is running.  See comment in lk-low.h for
   details.  */

unsigned int
lk_task_running (CORE_ADDR task)
{
  ULONGEST *cpu_online_mask;
  size_t size;
  unsigned int cpu;
  struct cleanup *old_chain;

  size = LK_BITMAP_SIZE (cpumask);
  cpu_online_mask = lk_read_bitmap (LK_ADDR (cpu_online_mask), size);
  old_chain = make_cleanup (xfree, cpu_online_mask);

  LK_BITMAP_FOR_EACH_SET_BIT (cpu_online_mask, size, cpu)
    {
      CORE_ADDR rq;
      CORE_ADDR curr;

      rq = LK_ADDR (runqueues) + lk_get_percpu_offset (cpu);
      curr = lk_read_addr (rq + LK_OFFSET (rq, curr));

      if (curr == task)
	break;
    }

  if (cpu == size)
    cpu = LK_CPU_INVAL;

  do_cleanups (old_chain);
  return cpu;
}

/* Update running tasks with information from struct rq->curr. */

static void
lk_update_running_tasks ()
{
  ULONGEST *cpu_online_mask;
  size_t size;
  unsigned int cpu;
  struct cleanup *old_chain;

  size = LK_BITMAP_SIZE (cpumask);
  cpu_online_mask = lk_read_bitmap (LK_ADDR (cpu_online_mask), size);
  old_chain = make_cleanup (xfree, cpu_online_mask);

  LK_BITMAP_FOR_EACH_SET_BIT (cpu_online_mask, size, cpu)
    {
      struct thread_info *tp;
      CORE_ADDR rq, curr;
      LONGEST pid, inf_pid;
      ptid_t new_ptid, old_ptid;

      rq = LK_ADDR (runqueues) + lk_get_percpu_offset (cpu);
      curr = lk_read_addr (rq + LK_OFFSET (rq, curr));
      pid = lk_read_int (curr + LK_OFFSET (task_struct, pid));
      inf_pid = current_inferior ()->pid;

      new_ptid = ptid_build (inf_pid, pid, curr);
      old_ptid = lk_cpu_to_old_ptid (cpu); /* FIXME not suitable for
					      running targets? */

      tp = find_thread_ptid (old_ptid);
      if (tp && tp->state != THREAD_EXITED)
	thread_change_ptid (old_ptid, new_ptid);
    }
  do_cleanups (old_chain);
}

/* Update sleeping tasks by walking the task_structs starting from
   init_task.  */

static void
lk_update_sleeping_tasks ()
{
  CORE_ADDR init_task, task, thread;
  int inf_pid;

  inf_pid = current_inferior ()->pid;
  init_task = LK_ADDR (init_task);

  lk_list_for_each_container (task, init_task, task_struct, tasks)
    {
      lk_list_for_each_container (thread, task, task_struct, thread_group)
	{
	  int pid;
	  ptid_t ptid;
	  struct thread_info *tp;

	  pid = lk_read_int (thread + LK_OFFSET (task_struct, pid));
	  ptid = ptid_build (inf_pid, pid, thread);

	  tp = find_thread_ptid (ptid);
	  if (tp == NULL || tp->state == THREAD_EXITED)
	    add_thread (ptid);
	}
    }
}

/* Function for targets to_update_thread_list hook.  */

static void
lk_update_thread_list (struct target_ops *target)
{
  prune_threads ();
  lk_update_running_tasks ();
  lk_update_sleeping_tasks ();
}

/* Function for targets to_fetch_registers hook.  */

static void
lk_fetch_registers (struct target_ops *target,
		    struct regcache *regcache, int regnum)
{
  CORE_ADDR task;
  unsigned int cpu;

  task = (CORE_ADDR) ptid_get_tid (regcache_get_ptid (regcache));
  cpu = lk_task_running (task);

  /* Let the target beneath fetch registers of running tasks.  */
  if (cpu != LK_CPU_INVAL)
    {
      struct cleanup *old_inferior_ptid;

      old_inferior_ptid = save_inferior_ptid ();
      inferior_ptid = lk_cpu_to_old_ptid (cpu);
      linux_kernel_ops->beneath->to_fetch_registers (target, regcache, regnum);
      do_cleanups (old_inferior_ptid);
    }
  else
    {
      struct gdbarch *gdbarch;
      unsigned int i;

      LK_HOOK->get_registers (task, target, regcache, regnum);

      /* Mark all registers not found as unavailable.  */
      gdbarch = get_regcache_arch (regcache);
      for (i = 0; i < gdbarch_num_regs (gdbarch); i++)
	{
	  if (regcache_register_status (regcache, i) == REG_UNKNOWN)
	    regcache_raw_supply (regcache, i, NULL);
	}
    }
}

/* Function for targets to_pid_to_str hook.  Marks running tasks with an
   asterisk "*".  */

static char *
lk_pid_to_str (struct target_ops *target, ptid_t ptid)
{
  static char buf[64];
  long pid;
  CORE_ADDR task;

  pid = ptid_get_lwp (ptid);
  task = (CORE_ADDR) ptid_get_tid (ptid);

  xsnprintf (buf, sizeof (buf), "PID: %5li%s, 0x%s",
	     pid, ((lk_task_running (task) != LK_CPU_INVAL) ? "*" : ""),
	     phex (task, lk_builtin_type_size (unsigned_long)));

  return buf;
}

/* Function for targets to_thread_name hook.  */

static const char *
lk_thread_name (struct target_ops *target, struct thread_info *ti)
{
  static char buf[LK_TASK_COMM_LEN + 1];
  char tmp[LK_TASK_COMM_LEN + 1];
  CORE_ADDR task, comm;
  size_t size;

  size = std::min ((unsigned int) LK_TASK_COMM_LEN,
		   LK_ARRAY_LEN(LK_FIELD (task_struct, comm)));

  task = (CORE_ADDR) ptid_get_tid (ti->ptid);
  comm = task + LK_OFFSET (task_struct, comm);
  read_memory (comm, (gdb_byte *) tmp, size);

  xsnprintf (buf, sizeof (buf), "%-16s", tmp);

  return buf;
}

/* Translate a kernel virtual address ADDR to a physical address.  */

CORE_ADDR
lk_kvtop (CORE_ADDR addr)
{
  CORE_ADDR pgd = lk_read_addr (LK_ADDR (init_mm)
				+ LK_OFFSET (mm_struct, pgd));
  return LK_HOOK->vtop (pgd, addr);
}

/* Restore current_target to TARGET.  */
static void
restore_current_target (void *target)
{
  current_target.beneath = (struct target_ops *) target;
}

/* Function for targets to_xfer_partial hook.  */

enum target_xfer_status
lk_xfer_partial (struct target_ops *ops, enum target_object object,
		 const char *annex, gdb_byte *readbuf,
		 const gdb_byte *writebuf, ULONGEST offset, ULONGEST len,
		 ULONGEST *xfered_len)
{
  enum target_xfer_status ret_val;
  struct cleanup *old_chain = make_cleanup (restore_current_target, ops);

  current_target.beneath = ops->beneath;

  if (LK_HOOK->is_kvaddr (offset))
    offset = lk_kvtop (offset);

  ret_val =  ops->beneath->to_xfer_partial (ops->beneath, object, annex,
					    readbuf, writebuf, offset, len,
					    xfered_len);
  do_cleanups (old_chain);
  return ret_val;
}

/* Functions to initialize and free target_ops and its private data.  As well
   as functions for targets to_open/close/detach hooks.  */

/* Check if OBFFILE is a Linux kernel.  */

static int
lk_is_linux_kernel (struct objfile *objfile)
{
  int ok = 0;

  if (objfile == NULL || !(objfile->flags & OBJF_MAINLINE))
    return 0;

  ok += lookup_minimal_symbol ("linux_banner", NULL, objfile).minsym != NULL;
  ok += lookup_minimal_symbol ("_stext", NULL, objfile).minsym != NULL;
  ok += lookup_minimal_symbol ("_etext", NULL, objfile).minsym != NULL;

  return (ok > 2);
}

/* Initialize struct lk_private.  */

static void
lk_init_private ()
{
  linux_kernel_ops->to_data = XCNEW (struct lk_private);
  LK_PRIVATE->hooks = XCNEW (struct lk_private_hooks);
  LK_PRIVATE->data = htab_create_alloc (31, (htab_hash) lk_hash_private_data,
					(htab_eq) lk_private_data_eq, NULL,
					xcalloc, xfree);
}

/* Initialize architecture independent private data.  Must be called
   _after_ symbol tables were initialized.  */

/* FIXME: throw error more fine-grained.  */
/* FIXME: make independent of compile options.  */

static void
lk_init_private_data ()
{
  if (LK_PRIVATE->data != NULL)
    htab_empty (LK_PRIVATE->data);

  LK_DECLARE_FIELD (task_struct, tasks);
  LK_DECLARE_FIELD (task_struct, pid);
  LK_DECLARE_FIELD (task_struct, tgid);
  LK_DECLARE_FIELD (task_struct, thread_group);
  LK_DECLARE_FIELD (task_struct, comm);
  LK_DECLARE_FIELD (task_struct, thread);

  LK_DECLARE_FIELD (list_head, next);
  LK_DECLARE_FIELD (list_head, prev);

  LK_DECLARE_FIELD (rq, curr);

  LK_DECLARE_FIELD (cpumask, bits);

  LK_DECLARE_FIELD (mm_struct, pgd);

  LK_DECLARE_FIELD (pgd_t, pgd);

  LK_DECLARE_FIELD (module, list);
  LK_DECLARE_FIELD (module, name);
  LK_DECLARE_FIELD (module, source_list);
  LK_DECLARE_FIELD (module, arch);
  LK_DECLARE_FIELD (module, init);
  LK_DECLARE_FIELD (module, percpu);
  LK_DECLARE_FIELD (module, percpu_size);

  /* Module offset moved to new struct module_layout with linux 4.5.
     It must be checked in code which of this fields exist.  */
  if (LK_DECLARE_FIELD_SILENT (module_layout, base)) /* linux 4.5+ */
    {
      LK_DECLARE_FIELD (module, init_layout);
      LK_DECLARE_FIELD (module, core_layout);

      LK_DECLARE_FIELD (module_layout, size);
      LK_DECLARE_FIELD (module_layout, text_size);
      LK_DECLARE_FIELD (module_layout, ro_size);
    }
  else if (LK_DECLARE_FIELD_SILENT (module, module_core)) /* linux -4.4 */
    {
      LK_DECLARE_FIELD (module, init_size);
      LK_DECLARE_FIELD (module, core_size);

      LK_DECLARE_FIELD (module, core_text_size);
      LK_DECLARE_FIELD (module, core_ro_size);
    }
  else
    {
      error (_("Could not find module base.  Aborting."));
    }

  LK_DECLARE_FIELD (module_use, source_list);
  LK_DECLARE_FIELD (module_use, source);

  LK_DECLARE_FIELD (uts_namespace, name);

  LK_DECLARE_STRUCT_ALIAS (new_utsname, utsname);
  LK_DECLARE_STRUCT_ALIAS (old_utsname, utsname);
  LK_DECLARE_STRUCT_ALIAS (oldold_utsname, utsname);
  if (LK_STRUCT (utsname) == NULL)
    error (_("Could not find struct utsname.  Aborting."));
  LK_DECLARE_FIELD (utsname, version);
  LK_DECLARE_FIELD (utsname, release);

  LK_DECLARE_ADDR (init_task);
  LK_DECLARE_ADDR (runqueues);
  LK_DECLARE_ADDR (__per_cpu_offset);
  LK_DECLARE_ADDR (init_mm);
  LK_DECLARE_ADDR (modules);
  LK_DECLARE_ADDR (init_uts_ns);

  LK_DECLARE_ADDR_ALIAS (__cpu_online_mask, cpu_online_mask);	/* linux 4.5+ */
  LK_DECLARE_ADDR_ALIAS (cpu_online_bits, cpu_online_mask);	/* linux -4.4 */
  if (LK_ADDR (cpu_online_mask) == -1)
    error (_("Could not find address cpu_online_mask.  Aborting."));
}

/* Frees the cpu to old ptid map.  */

static void
lk_free_ptid_map ()
{
  while (LK_PRIVATE->old_ptid)
    {
      struct lk_ptid_map *tmp;

      tmp = LK_PRIVATE->old_ptid;
      LK_PRIVATE->old_ptid = tmp->next;
      XDELETE (tmp);
    }
}

/* Initialize the cpu to old ptid map.  Prefer the arch dependent
   map_running_task_to_cpu hook if provided, else assume that the PID used
   by target beneath is the same as in task_struct PID task_struct.  See
   comment on lk_ptid_map in lk-low.h for details.  */

static void
lk_init_ptid_map ()
{
  struct thread_info *ti;
  ULONGEST *cpu_online_mask;
  size_t size;
  unsigned int cpu;
  struct cleanup *old_chain;

  if (LK_PRIVATE->old_ptid != NULL)
    lk_free_ptid_map ();

  size = LK_BITMAP_SIZE (cpumask);
  cpu_online_mask = lk_read_bitmap (LK_ADDR (cpu_online_mask), size);
  old_chain = make_cleanup (xfree, cpu_online_mask);

  ALL_THREADS (ti)
    {
      struct lk_ptid_map *ptid_map = XCNEW (struct lk_ptid_map);
      CORE_ADDR rq, curr;
      int pid;

      /* Give the architecture a chance to overwrite default behaviour.  */
      if (LK_HOOK->map_running_task_to_cpu)
	{
	  ptid_map->cpu = LK_HOOK->map_running_task_to_cpu (ti);
	}
      else
	{
	  LK_BITMAP_FOR_EACH_SET_BIT (cpu_online_mask, size, cpu)
	    {
	      rq = LK_ADDR (runqueues) + lk_get_percpu_offset (cpu);
	      curr = lk_read_addr (rq + LK_OFFSET (rq, curr));
	      pid = lk_read_int (curr + LK_OFFSET (task_struct, pid));

	      if (pid == ptid_get_lwp (ti->ptid))
		{
		  ptid_map->cpu = cpu;
		  break;
		}
	    }
	  if (cpu == size)
	    error (_("Could not map thread with pid %d, lwp %lu to a cpu."),
		   ti->ptid.pid, ti->ptid.lwp);
	}
      ptid_map->old_ptid = ti->ptid;
      ptid_map->next = LK_PRIVATE->old_ptid;
      LK_PRIVATE->old_ptid = ptid_map;
    }

  do_cleanups (old_chain);
}

/* Initializes all private data and pushes the linux kernel target, if not
   already done.  */

static void
lk_try_push_target ()
{
  struct gdbarch *gdbarch;

  gdbarch = current_inferior ()->gdbarch;
  if (!(gdbarch && gdbarch_lk_init_private_p (gdbarch)))
    error (_("Linux kernel debugging not supported on %s."),
	   gdbarch_bfd_arch_info (gdbarch)->printable_name);

  lk_init_private ();
  lk_init_private_data ();
  gdbarch_lk_init_private (gdbarch);
  /* Check for required arch hooks.  */
  gdb_assert (LK_HOOK->get_registers);
  gdb_assert (LK_HOOK->is_kvaddr);
  gdb_assert (LK_HOOK->vtop);
  gdb_assert (LK_HOOK->get_module_text_offset);

  lk_init_ptid_map ();
  lk_update_thread_list (linux_kernel_ops);

  if (!target_is_pushed (linux_kernel_ops))
    push_target (linux_kernel_ops);

  set_solib_ops (gdbarch, lk_modules_so_ops);
}

/* Function for targets to_open hook.  */

static void
lk_open (const char *args, int from_tty)
{
  struct objfile *objfile;

  if (target_is_pushed (linux_kernel_ops))
    {
      printf_unfiltered (_("Linux kernel target already pushed.  Aborting\n"));
      return;
    }

  for (objfile = current_program_space->objfiles; objfile;
       objfile = objfile->next)
    {
      if (lk_is_linux_kernel (objfile)
	  && ptid_get_pid (inferior_ptid) != 0)
	{
	  lk_try_push_target ();
	  return;
	}
    }
  printf_unfiltered (_("Could not find a valid Linux kernel object file.  "
		       "Aborting.\n"));
}

/* Function for targets to_close hook.  Deletes all private data.  */

static void
lk_close (struct target_ops *ops)
{
  htab_delete (LK_PRIVATE->data);
  lk_free_ptid_map ();
  XDELETE (LK_PRIVATE->hooks);

  XDELETE (LK_PRIVATE);
  linux_kernel_ops->to_data = NULL;
}

/* Function for targets to_detach hook.  */

static void
lk_detach (struct target_ops *t, const char *args, int from_tty)
{
  struct target_ops *beneath = linux_kernel_ops->beneath;

  unpush_target (linux_kernel_ops);
  reinit_frame_cache ();
  if (from_tty)
    printf_filtered (_("Linux kernel target detached.\n"));

  beneath->to_detach (beneath, args, from_tty);
}

/* Function for new objfile observer.  */

static void
lk_observer_new_objfile (struct objfile *objfile)
{
  if (lk_is_linux_kernel (objfile)
      && ptid_get_pid (inferior_ptid) != 0)
    lk_try_push_target ();
}

/* Function for inferior created observer.  */

static void
lk_observer_inferior_created (struct target_ops *ops, int from_tty)
{
  struct objfile *objfile;

  if (ptid_get_pid (inferior_ptid) == 0)
    return;

  for (objfile = current_inferior ()->pspace->objfiles; objfile;
       objfile = objfile->next)
    {
      if (lk_is_linux_kernel (objfile))
	{
	  lk_try_push_target ();
	  return;
	}
    }
}

/* Initialize linux kernel target.  */

static void
init_linux_kernel_ops (void)
{
  struct target_ops *t;

  if (linux_kernel_ops != NULL)
    return;

  t = XCNEW (struct target_ops);
  t->to_shortname = "linux-kernel";
  t->to_longname = "linux kernel support";
  t->to_doc = "Adds support to debug the Linux kernel";

  /* set t->to_data = struct lk_private in lk_init_private.  */

  t->to_open = lk_open;
  t->to_close = lk_close;
  t->to_detach = lk_detach;
  t->to_fetch_registers = lk_fetch_registers;
  t->to_update_thread_list = lk_update_thread_list;
  t->to_pid_to_str = lk_pid_to_str;
  t->to_thread_name = lk_thread_name;
  t->to_xfer_partial = lk_xfer_partial;

  t->to_stratum = thread_stratum;
  t->to_magic = OPS_MAGIC;

  linux_kernel_ops = t;

  add_target (t);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_linux_kernel;

void
_initialize_linux_kernel (void)
{
  init_linux_kernel_ops ();

  observer_attach_new_objfile (lk_observer_new_objfile);
  observer_attach_inferior_created (lk_observer_inferior_created);
}
