/* Generic GNU/Linux target using traditional ptrace register access.

   Copyright (C) 1988-2018 Free Software Foundation, Inc.

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
#include "linux-nat-trad.h"

#include "nat/gdb_ptrace.h"
#include "inf-ptrace.h"

/* Pointer to a function that returns the offset within the user area
   where a particular register is stored.  */
static CORE_ADDR (*inf_ptrace_register_u_offset)(struct gdbarch *, int, int);

/* Fetch register REGNUM from the inferior.  */

static void
inf_ptrace_fetch_register (struct regcache *regcache, int regnum)
{
  struct gdbarch *gdbarch = regcache->arch ();
  CORE_ADDR addr;
  size_t size;
  PTRACE_TYPE_RET *buf;
  pid_t pid;
  int i;

  /* This isn't really an address, but ptrace thinks of it as one.  */
  addr = inf_ptrace_register_u_offset (gdbarch, regnum, 0);
  if (addr == (CORE_ADDR)-1
      || gdbarch_cannot_fetch_register (gdbarch, regnum))
    {
      regcache_raw_supply (regcache, regnum, NULL);
      return;
    }

  pid = get_ptrace_pid (regcache_get_ptid (regcache));

  size = register_size (gdbarch, regnum);
  gdb_assert ((size % sizeof (PTRACE_TYPE_RET)) == 0);
  buf = (PTRACE_TYPE_RET *) alloca (size);

  /* Read the register contents from the inferior a chunk at a time.  */
  for (i = 0; i < size / sizeof (PTRACE_TYPE_RET); i++)
    {
      errno = 0;
      buf[i] = ptrace (PT_READ_U, pid, (PTRACE_TYPE_ARG3)(uintptr_t)addr, 0);
      if (errno != 0)
	error (_("Couldn't read register %s (#%d): %s."),
	       gdbarch_register_name (gdbarch, regnum),
	       regnum, safe_strerror (errno));

      addr += sizeof (PTRACE_TYPE_RET);
    }
  regcache_raw_supply (regcache, regnum, buf);
}

/* Fetch register REGNUM from the inferior.  If REGNUM is -1, do this
   for all registers.  */

static void
inf_ptrace_fetch_registers (struct target_ops *ops,
			    struct regcache *regcache, int regnum)
{
  if (regnum == -1)
    for (regnum = 0;
	 regnum < gdbarch_num_regs (regcache->arch ());
	 regnum++)
      inf_ptrace_fetch_register (regcache, regnum);
  else
    inf_ptrace_fetch_register (regcache, regnum);
}

/* Store register REGNUM into the inferior.  */

static void
inf_ptrace_store_register (const struct regcache *regcache, int regnum)
{
  struct gdbarch *gdbarch = regcache->arch ();
  CORE_ADDR addr;
  size_t size;
  PTRACE_TYPE_RET *buf;
  pid_t pid;
  int i;

  /* This isn't really an address, but ptrace thinks of it as one.  */
  addr = inf_ptrace_register_u_offset (gdbarch, regnum, 1);
  if (addr == (CORE_ADDR)-1
      || gdbarch_cannot_store_register (gdbarch, regnum))
    return;

  pid = get_ptrace_pid (regcache_get_ptid (regcache));

  size = register_size (gdbarch, regnum);
  gdb_assert ((size % sizeof (PTRACE_TYPE_RET)) == 0);
  buf = (PTRACE_TYPE_RET *) alloca (size);

  /* Write the register contents into the inferior a chunk at a time.  */
  regcache_raw_collect (regcache, regnum, buf);
  for (i = 0; i < size / sizeof (PTRACE_TYPE_RET); i++)
    {
      errno = 0;
      ptrace (PT_WRITE_U, pid, (PTRACE_TYPE_ARG3)(uintptr_t)addr, buf[i]);
      if (errno != 0)
	error (_("Couldn't write register %s (#%d): %s."),
	       gdbarch_register_name (gdbarch, regnum),
	       regnum, safe_strerror (errno));

      addr += sizeof (PTRACE_TYPE_RET);
    }
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers.  */

static void
inf_ptrace_store_registers (struct target_ops *ops,
			    struct regcache *regcache, int regnum)
{
  if (regnum == -1)
    for (regnum = 0;
	 regnum < gdbarch_num_regs (regcache->arch ());
	 regnum++)
      inf_ptrace_store_register (regcache, regnum);
  else
    inf_ptrace_store_register (regcache, regnum);
}

/* Create a "traditional" Linux/ptrace target.  REGISTER_U_OFFSET
   should be a function returning the offset within the user area
   where a particular register is stored.  */

struct target_ops *
linux_trad_target (CORE_ADDR (*register_u_offset)(struct gdbarch *, int, int))
{
  struct target_ops *t = inf_ptrace_target();

  gdb_assert (register_u_offset);
  inf_ptrace_register_u_offset = register_u_offset;
  t->to_fetch_registers = inf_ptrace_fetch_registers;
  t->to_store_registers = inf_ptrace_store_registers;

  linux_target_install_ops (t);

  return t;
}
