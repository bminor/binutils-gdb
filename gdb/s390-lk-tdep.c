/* Target-dependent code for linux-kernel target on S390.
   Copyright (C) 2017 Free Software Foundation, Inc.

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

#include "gdbarch.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "lk-low.h"
#include "osabi.h"
#include "regcache.h"
#include "regset.h"
#include "s390-tdep.h"
#include "s390-lk-tdep.h"

#include "features/s390x-cr-linux64.c"
#include "features/s390x-vxcr-linux64.c"

/* Register maps and sets.  */

static const struct regcache_map_entry s390x_gregmap_lk[] =
  {
    { 10, S390_R6_REGNUM }, /* r0-r5 volatile */
    { -2, REGCACHE_MAP_SKIP, 8 },
    { 1, S390_PSWA_REGNUM }, /* Use r14 for PSWA.  */
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_cr [] =
  {
    { 16, S390_CR0_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_timer [] =
  {
    { 1, S390_TIMER_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_todcmp [] =
  {
    { 1, S390_TODCMP_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_todpreg [] =
  {
    { 1, S390_TODPREG_REGNUM, 4 },
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_prefix [] =
  {
    { 1, S390_PREFIX_REGNUM, 4 },
    { 0 }
  };

const struct regset s390x_gregset_lk = {
  s390x_gregmap_lk,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_cr_regset = {
  s390x_regmap_cr,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_timer_regset = {
  s390x_regmap_timer,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_todcmp_regset = {
  s390x_regmap_todcmp,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_todpreg_regset = {
  s390x_regmap_todpreg,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_prefix_regset = {
  s390x_regmap_prefix,
  regcache_supply_regset,
  regcache_collect_regset
};

/* Function for Linux kernel target get_registers hook.  Supplies gprs for
   task TASK to REGCACHE.  Uses r14 (back jump address) as current pswa.  */

void
s390_lk_get_registers (CORE_ADDR task, struct target_ops *target,
		       struct regcache *regcache, int regnum)
{
  const struct regset *regset;
  CORE_ADDR ksp, gprs, pswa;
  gdb_byte buf[80]; /* 80 = 10 (#registers saved) * 8 (64 bit width) */
  size_t size;

  regset = &s390x_gregset_lk;

  ksp = lk_read_addr (task + LK_OFFSET (task_struct, thread)
		      + LK_OFFSET (thread_struct, ksp));
  gprs = ksp + LK_OFFSET (stack_frame, gprs);
  size = FIELD_SIZE (LK_FIELD (stack_frame, gprs));
  gdb_assert (size <= sizeof (buf));

  read_memory (gprs, buf, size);
  regset->supply_regset (regset, regcache, -1, buf, size);
}

/* Function for Linux kernel target get_percpu_offset hook.  Returns the
   percpu_offset from lowcore for cpu CPU.  */

CORE_ADDR
s390_lk_get_percpu_offset (unsigned int cpu)
{
  CORE_ADDR lowcore_ptr, lowcore;
  size_t ptr_len = lk_builtin_type_size (unsigned_long);

  lowcore_ptr = LK_ADDR (lowcore_ptr) + (ptr_len * cpu);
  lowcore = lk_read_addr (lowcore_ptr);

  return lk_read_addr (lowcore + LK_OFFSET (lowcore, percpu_offset));
}

/* Function for Linux kernel target map_running_task_to_cpu hook.  */

unsigned int
s390_lk_map_running_task_to_cpu (struct thread_info *ti)
{
  struct regcache *regcache;
  enum register_status reg_status;
  CORE_ADDR lowcore;
  unsigned int cpu;

  regcache = get_thread_regcache (ti->ptid);
  reg_status = regcache_raw_read_unsigned (regcache, S390_PREFIX_REGNUM,
					   (ULONGEST *) &lowcore);
  if (reg_status != REG_VALID)
    error (_("Could not find prefix register for thread with pid %d, lwp %li."),
	   ti->ptid.pid, ti->ptid.lwp);

  cpu = lk_read_uint (lowcore + LK_OFFSET (lowcore, cpu_nr));

  return cpu;
}

/* Function for Linux kernel target is_kvaddr hook.  */

int
s390_lk_is_kvaddr (CORE_ADDR addr)
{
  return addr >= LK_ADDR (high_memory);
}

/* Read table entry from TABLE at offset OFFSET.  Helper for s390_lk_vtop.  */

static inline ULONGEST
s390_lk_read_table_entry (CORE_ADDR table, ULONGEST offset)
{
  return lk_read_ulong (table + offset * lk_builtin_type_size (unsigned_long));
}

/* Function for Linux kernel target vtop hook.  Assume 64 bit addresses.  */

CORE_ADDR
s390_lk_vtop (CORE_ADDR table, CORE_ADDR vaddr)
{
  ULONGEST entry, offset;
  CORE_ADDR paddr;
  unsigned int table_type;
  size_t addr_size = lk_builtin_type_size (unsigned_long);

  /* Read first entry in table to get its type.  As the Table-Type bits are
     the same in every table assume Region1-Table.  */
  entry = s390_lk_read_table_entry (table, 0);
  table_type = (entry & S390_LK_RFTE_TT) >> 2;

  switch (table_type)
    {
      case S390_LK_DAT_TT_REGION1:
	{
	  offset = (vaddr & S390_LK_VADDR_RFX) >> 53;
	  entry = s390_lk_read_table_entry (table, offset);

	  /* Do sanity checks.  */
	  if (!entry)
	    warning (_("Trying to translate address 0x%s with empty "\
		       "region-first-table entry."),
		   phex (vaddr, addr_size));
	  else if ((entry & S390_LK_RFTE_TT) >> 2 != S390_LK_DAT_TT_REGION1)
	    warning (_("Trying to translate address 0x%s with corrupt "\
		       "table type in region-first-table entry."),
		     phex (vaddr, addr_size));
	  else if (entry & S390_LK_RFTE_I)
	    warning (_("Translating address 0x%s with invalid bit set at "\
		       "region-first-table entry."),
		     phex (vaddr, addr_size));

	  table = entry & S390_LK_RFTE_O;
	}
	/* fall through */
      case S390_LK_DAT_TT_REGION2:
	{
	  offset = (vaddr & S390_LK_VADDR_RSX) >> 42;
	  entry = s390_lk_read_table_entry (table, offset);

	  /* Do sanity checks.  */
	  if (!entry)
	    warning (_("Trying to translate address 0x%s with empty "\
		       "region-second-table entry."),
		   phex (vaddr, addr_size));
	  else if ((entry & S390_LK_RSTE_TT) >> 2 != S390_LK_DAT_TT_REGION2)
	    warning (_("Trying to translate address 0x%s with corrupt "\
		       "table type in region-second-table entry."),
		     phex (vaddr, addr_size));
	  else if (entry & S390_LK_RSTE_I)
	    warning (_("Translating address 0x%s with invalid bit set at "\
		       "region-second-table entry."),
		     phex (vaddr, addr_size));

	  table = entry & S390_LK_RSTE_O;
	}
	/* fall through */
      case S390_LK_DAT_TT_REGION3:
	{
	  offset = (vaddr & S390_LK_VADDR_RTX) >> 31;
	  entry = s390_lk_read_table_entry (table, offset);

	  /* Do sanity checks.  */
	  if (!entry)
	    warning (_("Trying to translate address 0x%s with empty "\
		       "region-third-table entry."),
		   phex (vaddr, addr_size));
	  else if ((entry & S390_LK_RTTE_TT) >> 2 != S390_LK_DAT_TT_REGION3)
	    warning (_("Trying to translate address 0x%s with corrupt "\
		       "table type in region-third-table entry."),
		     phex (vaddr, addr_size));
	  else if (entry & S390_LK_RTTE_I)
	    warning (_("Translating address 0x%s with invalid bit set at "\
		       "region-third-table entry."),
		     phex (vaddr, addr_size));

	  /* Check for huge page.  */
	  if (entry & S390_LK_RTTE_FC)
	    {
	      paddr = ((entry & S390_LK_RTTE_RFAA)
		       + (vaddr & ~S390_LK_RTTE_RFAA));
	      return paddr;
	    }

	  table = entry & S390_LK_RTTE_O;
	}
	/* fall through */
      case S390_LK_DAT_TT_SEGMENT:
	{
	  offset = (vaddr & S390_LK_VADDR_SX) >> 20;
	  entry = s390_lk_read_table_entry (table, offset);

	  /* Do sanity checks.  */
	  if (!entry)
	    warning (_("Trying to translate address 0x%s with empty "\
		       "segment-table entry."),
		   phex (vaddr, addr_size));
	  else if ((entry & S390_LK_STE_TT) >> 2 != S390_LK_DAT_TT_SEGMENT)
	    warning (_("Trying to translate address 0x%s with corrupt "\
		       "table type in segment-table entry."),
		     phex (vaddr, addr_size));
	  else if (entry & S390_LK_STE_I)
	    warning (_("Translating address 0x%s with invalid bit set at "\
		       "segment-table entry."),
		     phex (vaddr, addr_size));

	  /* Check for large page.  */
	  if (entry & S390_LK_STE_FC)
	    {
	      paddr = ((entry & S390_LK_STE_SFAA)
		       + (vaddr & ~S390_LK_STE_SFAA));
	      return paddr;
	    }

	  table = entry & S390_LK_STE_O;
	  break;
	}
    } /* switch (table_type) */

  offset = (vaddr & S390_LK_VADDR_PX) >> 12;
  entry = s390_lk_read_table_entry (table, offset);

  /* Do sanity checks.  */
  if (!entry)
    warning (_("Trying to translate address 0x%s with empty page-table "\
	       "entry."),
	     phex (vaddr, addr_size));
  else if (entry & S390_LK_PTE_I)
    warning (_("Translating address 0x%s with invalid bit set at page-table "\
	       "entry."),
	     phex (vaddr, addr_size));

  paddr = ((entry & S390_LK_PTE_PFAA) + (vaddr & ~S390_LK_PTE_PFAA));

  return paddr;
}

/* Function for Linux kernel target get_module_text_offset hook.  */

CORE_ADDR
s390_lk_get_module_text_offset (CORE_ADDR mod)
{
  CORE_ADDR offset, mod_arch;

  mod_arch = mod + LK_OFFSET (module, arch);
  offset = lk_read_ulong (mod_arch + LK_OFFSET (mod_arch_specific, got_size));
  offset += lk_read_ulong (mod_arch + LK_OFFSET (mod_arch_specific, plt_size));

  return offset;
}

/* Initialize s390 dependent private data for linux kernel target.  */

static void
s390_lk_init_private (struct gdbarch *gdbarch)
{
  LK_DECLARE_FIELD (stack_frame, gprs);

  LK_DECLARE_FIELD (thread_struct, ksp);

  LK_DECLARE_STRUCT_ALIAS (_lowcore, lowcore); /* linux -4.4 */
  LK_DECLARE_STRUCT_ALIAS (lowcore, lowcore); /* linux 4.5+ */
  if (LK_STRUCT (lowcore) == NULL)
    error (_("Could not find struct lowcore.  Aborting."));
  LK_DECLARE_FIELD (lowcore, percpu_offset);
  LK_DECLARE_FIELD (lowcore, current_pid);
  LK_DECLARE_FIELD (lowcore, cpu_nr);

  LK_DECLARE_FIELD (mod_arch_specific, got_size);
  LK_DECLARE_FIELD (mod_arch_specific, plt_size);

  LK_DECLARE_ADDR (lowcore_ptr);
  LK_DECLARE_ADDR (high_memory);

  LK_HOOK->get_registers = s390_lk_get_registers;
  LK_HOOK->is_kvaddr = s390_lk_is_kvaddr;
  LK_HOOK->vtop = s390_lk_vtop;
  LK_HOOK->get_percpu_offset = s390_lk_get_percpu_offset;
  LK_HOOK->map_running_task_to_cpu = s390_lk_map_running_task_to_cpu;
  LK_HOOK->get_module_text_offset = s390_lk_get_module_text_offset;
}

/* Initialize Linux kernel specific gdbarch hooks.  */

void
s390_gdbarch_lk_init (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* Support linux kernel debugging.  */
  set_gdbarch_lk_init_private (gdbarch, s390_lk_init_private);
}

extern initialize_file_ftype _initialize_s390_lk_tdep; /* -Wmissing-prototypes */

void
_initialize_s390_lk_tdep (void)
{
  /* Initialize the Linux kernel target descriptions.  */
  initialize_tdesc_s390x_cr_linux64 ();
  initialize_tdesc_s390x_vxcr_linux64 ();
}
