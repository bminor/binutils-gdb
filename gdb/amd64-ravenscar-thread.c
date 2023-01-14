/* Ravenscar x86-64 target support.

   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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
#include "regcache.h"
#include "amd64-tdep.h"
#include "inferior.h"
#include "ravenscar-thread.h"
#include "amd64-ravenscar-thread.h"

struct amd64_ravenscar_ops : public ravenscar_arch_ops
{
  void fetch_registers (struct regcache *regcache, int regnum) override;
  void store_registers (struct regcache *regcache, int regnum) override;

private:

  /* Return the offset of the register in the context buffer.  */
  int register_offset (struct gdbarch *arch, int regnum);
};

/* x86-64 Ravenscar stores registers as:

   type Context_Buffer is record
      RIP    : System.Address;
      RFLAGS : EFLAGS;
      RSP    : System.Address;

      RBX    : System.Address;
      RBP    : System.Address;
      R12    : System.Address;
      R13    : System.Address;
      R14    : System.Address;
      R15    : System.Address;
   end record;
*/
static const int register_layout[] =
{
  AMD64_RIP_REGNUM,
  AMD64_EFLAGS_REGNUM,
  AMD64_RSP_REGNUM,
  AMD64_RBX_REGNUM,
  AMD64_RBP_REGNUM,
  AMD64_R12_REGNUM,
  AMD64_R13_REGNUM,
  AMD64_R14_REGNUM,
  AMD64_R15_REGNUM,
};

int
amd64_ravenscar_ops::register_offset (struct gdbarch *arch, int regnum)
{
  for (int i = 0; i < ARRAY_SIZE (register_layout); ++i)
    if (register_layout[i] == regnum)
      return i * 8;
  /* Not saved.  */
  return -1;
}

/* Supply register REGNUM, which has been saved at REGISTER_ADDR, to
   the regcache.  */

static void
supply_register_at_address (struct regcache *regcache, int regnum,
                            CORE_ADDR register_addr)
{
  struct gdbarch *gdbarch = regcache->arch ();
  int buf_size = register_size (gdbarch, regnum);
  gdb_byte *buf;

  buf = (gdb_byte *) alloca (buf_size);
  read_memory (register_addr, buf, buf_size);
  regcache->raw_supply (regnum, buf);
}

void
amd64_ravenscar_ops::fetch_registers (struct regcache *regcache, int regnum)
{
  struct gdbarch *gdbarch = regcache->arch ();
  const int num_regs = gdbarch_num_regs (gdbarch);
  int current_regnum;
  CORE_ADDR current_address;
  CORE_ADDR thread_descriptor_address;

  /* The tid is the thread_id field, which is a pointer to the thread.  */
  thread_descriptor_address = (CORE_ADDR) inferior_ptid.tid ();

  /* Read registers.  */
  for (current_regnum = 0; current_regnum < num_regs; current_regnum++)
    {
      int offset = register_offset (gdbarch, current_regnum);

      if (offset != -1)
        {
          current_address = thread_descriptor_address + offset;
          supply_register_at_address (regcache, current_regnum,
                                      current_address);
        }
    }
}

void
amd64_ravenscar_ops::store_registers (struct regcache *regcache, int regnum)
{
  struct gdbarch *gdbarch = regcache->arch ();
  int buf_size = register_size (gdbarch, regnum);
  gdb_byte buf[buf_size];
  CORE_ADDR register_address;

  int offset = register_offset (gdbarch, regnum);
  if (offset != -1)
    {
      register_address = inferior_ptid.tid () + offset;

      regcache->raw_collect (regnum, buf);
      write_memory (register_address,
		    buf,
		    buf_size);
    }
}

/* The ravenscar_arch_ops vector for AMD64 targets.  */

static struct amd64_ravenscar_ops amd64_ravenscar_ops;

/* Register amd64_ravenscar_ops in GDBARCH.  */

void
register_amd64_ravenscar_ops (struct gdbarch *gdbarch)
{
  set_gdbarch_ravenscar_ops (gdbarch, &amd64_ravenscar_ops);
}
