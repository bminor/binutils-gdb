/* Register buffer sufficient to hold all raw registers

   Copyright 2002 Free Software Foundation, Inc.

   This file is part of GDB.

   Contributed by Red Hat, Inc.

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

#include "defs.h"
#include "regbuf.h"
#include "gdb_assert.h"

extern void _initialize_regbuf (void);

/* Per-architecture object describing the layout of a register buffer.
   Computed once when the architecture is created */

struct gdbarch_data *regbuf_data_handle;

struct regbuf_descr
{
  struct gdbarch *gdbarch;
  /* Size of the register buffer, over-allocate making room for both
     real and pseudo-registers.  */
  /* FIXME: cagney/2002-05-11: This over-allocation shouldn't be
     necessary, only some targets store values in the pseudo-register
     section and we want to be sure that GDB won't trash memory.  */
  long sizeof_registers;
  /* Total number of registers in the buffer.  */
  int nr_registers;
  /* Offset into the register buffer for each register.  */
  long *register_offset;
  /* Size, in ``bytes'', of a register.  */
  long *sizeof_register;
};

static struct regbuf_descr *
regbuf_descr (struct gdbarch *gdbarch)
{
  int i;
  /* FIXME: cagney/2002-05-11: gdbarch_data() should take that
     ``gdbarch'' as a parameter.  */
  struct regbuf_descr *descr = gdbarch_data (gdbarch,
					     regbuf_data_handle);
  if (descr != NULL)
    return descr;
  
  descr = XMALLOC (struct regbuf_descr);
  descr->gdbarch = gdbarch;
  /* FIXME: cagney/2002-05-11: Shouldn't be including pseudo-registers
     in the register buffer.  Unfortunatly some architectures are
     storing pseudo register values in the raw register buffer.  */
  descr->nr_registers = NUM_REGS + NUM_PSEUDO_REGS;
  /* FIXME: cagney/2002-05-11: Instead of using REGISTER_BYTE() this
     should compute, at runtime, an offset table describing where each
     register is in the register table.  This currently isn't possible
     because some targets overlap registers, ulgh!  */
  descr->register_offset = XCALLOC (descr->nr_registers, long);
  descr->sizeof_register = XCALLOC (descr->nr_registers, long);
  for (i = 0; i < descr->nr_registers; i++)
    {
      descr->register_offset[i] = REGISTER_BYTE (i);
      descr->sizeof_register[i] = REGISTER_RAW_SIZE (i);
    }
  /* Come up with the real size of the registers buffer.  */
  descr->sizeof_registers = REGISTER_BYTES; /* OK use.  */
  for (i = 0; i < descr->nr_registers; i++)
    {
      long regend;
      /* Keep extending the buffer so that there is always enough
         space for all registers.  The comparison is necessary since
         legacy code is free to put registers in random places in the
         buffer separated by holes.  Once REGISTER_BYTE() is killed
         this can be greatly simplified.  */
      /* FIXME: cagney/2001-12-04: This code shouldn't need to use
         REGISTER_BYTE().  Unfortunatly, legacy code likes to lay the
         buffer out so that certain registers just happen to overlap.
         Ulgh!  New targets use gdbarch's register read/write and
         entirely avoid this uglyness.  */
      regend = descr->register_offset[i] + descr->sizeof_register[i];
      if (descr->sizeof_registers < regend)
	descr->sizeof_registers = regend;
    }
  set_gdbarch_data (gdbarch, regbuf_data_handle, descr);
  return descr;
}

static void
xfree_regbuf (struct gdbarch *gdbarch, void *ptr)
{
  struct regbuf_descr *descr = ptr;
  if (descr == NULL)
    return;
  xfree (descr->register_offset);
  xfree (descr->sizeof_register);
  xfree (descr);
}

/* For moment, ``struct regbuf'' is just a character buffer.  */

struct regbuf
{
  struct regbuf_descr *descr;
  char *registers;
  char *valid_p;
};

struct regbuf *
regbuf_xmalloc (struct gdbarch *gdbarch)
{
  struct regbuf_descr *descr = regbuf_descr (gdbarch);
  struct regbuf *regbuf = XMALLOC (struct regbuf);
  regbuf->descr = descr;
  regbuf->registers = xmalloc (descr->sizeof_registers);
  regbuf->valid_p = xmalloc (descr->nr_registers);
  return regbuf;
}

void
regbuf_xfree (struct regbuf *regbuf)
{
  gdb_assert (regbuf != NULL);
  xfree (regbuf->registers);
  xfree (regbuf->valid_p);
  xfree (regbuf);
}

void
do_regbuf_xfree (void *buf)
{
  regbuf_xfree (buf);
}

struct regbuf *
regbuf_xmalloc_with_cleanup (struct gdbarch *gdbarch)
{
  struct regbuf *regbuf = regbuf_xmalloc (gdbarch);
  make_cleanup (do_regbuf_xfree, regbuf);
  return regbuf;
}

struct regbuf *
regbuf_dup (struct regbuf *regbuf)
{
  struct regbuf *newbuf = regbuf_xmalloc (regbuf->descr->gdbarch);
  memcpy (newbuf->registers, regbuf->registers,
	  regbuf->descr->sizeof_registers);
  memcpy (newbuf->valid_p, regbuf->valid_p, regbuf->descr->nr_registers);
  return newbuf;
}

int
regbuf_valid_p (struct regbuf *regbuf, int regnum)
{
  gdb_assert (regnum != NULL);
  gdb_assert (regnum >= 0 && regnum < NUM_REGS + NUM_PSEUDO_REGS);
  return regbuf->valid_p[regnum];
}

void
regbuf_read (struct regbuf *regbuf, int regnum, void *buf)
{
  gdb_assert (regbuf != NULL);
  gdb_assert (regnum >= 0 && regnum < regbuf->descr->nr_registers);
  memcpy (buf, regbuf->registers + regbuf->descr->register_offset[regnum],
	  regbuf->descr->sizeof_register[regnum]);
}

void
regbuf_write (struct regbuf *regbuf, int regnum, const void *buf)
{
  gdb_assert (regbuf != NULL);
  gdb_assert (regnum >= 0 && regnum < regbuf->descr->nr_registers);
  memcpy (regbuf->registers + regbuf->descr->register_offset[regnum], buf,
	  regbuf->descr->sizeof_register[regnum]);
  regbuf->valid_p[regnum] = 1;
}

CORE_ADDR
regbuf_read_as_address (struct regbuf *regbuf, int regnum)
{
  char *buf;
  gdb_assert (regbuf != NULL);
  gdb_assert (regnum >= 0 && regnum < regbuf->descr->nr_registers);
  buf = alloca (regbuf->descr->sizeof_register[regnum]);
  regbuf_read (regbuf, regnum, buf);
  return extract_address (buf, regbuf->descr->sizeof_register[regnum]);
}

char *
grub_around_regbuf_for_registers (struct regbuf *regbuf)
{
  return regbuf->registers;
}

char *
grub_around_regbuf_for_register_valid (struct regbuf *regbuf)
{
  return regbuf->valid_p;
}

void
_initialize_regbuf (void)
{
  regbuf_data_handle = register_gdbarch_data (NULL, xfree_regbuf);
}
