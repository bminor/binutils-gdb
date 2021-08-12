/* Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

   This file is part of GDB.

   Based on MIPS target.

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

#include "gdbsupport/common-defs.h"
#include "nat/gdb_ptrace.h"
#include "loongarch-linux-watch.h"

/* Assuming usable watch registers REGS, return the irwmask of
   register N.  */

uint8_t
loongarch_linux_watch_get_irwmask (struct pt_watch_regs *regs, int n)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      return regs->la32[n].irwmask & IRW_MASK;
    case pt_watch_style_la64:
      return regs->la64[n].irwmask & IRW_MASK;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, return the irwstat of
   register N.  */

uint8_t
loongarch_linux_watch_get_irwstat (struct pt_watch_regs *regs, int n)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      return regs->la32[n].irwstat & IRW_MASK;
    case pt_watch_style_la64:
      return regs->la64[n].irwstat & IRW_MASK;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, return the num_valid.  */

uint32_t
loongarch_linux_watch_get_num_valid (struct pt_watch_regs *regs)
{
  return regs->num_valid;
}

/* Assuming usable watch registers REGS, return the addr of
   register N.  */

CORE_ADDR
loongarch_linux_watch_get_addr (struct pt_watch_regs *regs, int n)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      return regs->la32[n].addr;
    case pt_watch_style_la64:
      return regs->la64[n].addr;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, set addr of register N to
   VALUE.  */

void
loongarch_linux_watch_set_addr (struct pt_watch_regs *regs, int n,
				CORE_ADDR value)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      /*  The cast will never throw away bits as 64 bit addresses can
	  never be used on a 32 bit kernel.  */
      regs->la32[n].addr = (uint32_t) value;
      break;
    case pt_watch_style_la64:
      regs->la64[n].addr = value;
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, return the mask of
   register N.  */

CORE_ADDR
loongarch_linux_watch_get_mask (struct pt_watch_regs *regs, int n)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      return regs->la32[n].mask;
    case pt_watch_style_la64:
      return regs->la64[n].mask;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, set mask of register N to
   VALUE.  */

void
loongarch_linux_watch_set_mask (struct pt_watch_regs *regs, int n,
				CORE_ADDR value)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      regs->la32[n].mask = value;
      break;
    case pt_watch_style_la64:
      regs->la64[n].mask = value;
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, return the irw of
   register N.  */

uint8_t
loongarch_linux_watch_get_irw (struct pt_watch_regs *regs, int n)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      return regs->la32[n].irw;
    case pt_watch_style_la64:
      return regs->la64[n].irw;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Assuming usable watch registers REGS, set irw of register N to
   VALUE.  */

void
loongarch_linux_watch_set_irw (struct pt_watch_regs *regs, int n,
			       uint8_t value)
{
  switch (regs->style)
    {
    case pt_watch_style_la32:
      regs->la32[n].irw = value;
      break;
    case pt_watch_style_la64:
      regs->la64[n].irw = value;
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _ ("Unrecognized watch register style"));
    }
}

/* Read the watch registers of process LWPID and store it in
   WATCH_READBACK.  Save true to *WATCH_READBACK_VALID if watch
   registers are valid.  Return 1 if watch registers are usable.
   Cached information is used unless FORCE is true.  */

int
loongarch_linux_read_watch_registers (long lwpid,
				      struct pt_watch_regs *watch_readback,
				      int *watch_readback_valid, int force)
{
  if (force || *watch_readback_valid == 0)
    {
      if (ptrace (PTRACE_GET_WATCH_REGS, lwpid, watch_readback, NULL) == -1)
	{
	  *watch_readback_valid = -1;
	  return 0;
	}
      if (watch_readback->num_valid == 0)
	{
	  *watch_readback_valid = -1;
	  return 0;
	}
      /* Watch registers appear to be usable.  */
      *watch_readback_valid = 1;
    }
  return (*watch_readback_valid == 1) ? 1 : 0;
}

/* Convert GDB's TYPE to an IRW mask.  */

uint32_t
loongarch_linux_watch_type_to_irw (enum target_hw_bp_type type)
{
  switch (type)
    {
    case hw_write:
      return W_MASK;
    case hw_read:
      return R_MASK;
    case hw_access:
      return (W_MASK | R_MASK);
    case hw_execute:
      return I_MASK;
    default:
      return 0;
    }
}

/* Set any low order bits in MASK that are not set.  */

static CORE_ADDR
fill_mask (CORE_ADDR mask)
{
  CORE_ADDR f = 1;

  while (f && f < mask)
    {
      mask |= f;
      f <<= 1;
    }
  return mask;
}

/* Try to add a single watch to the specified registers REGS.  The
   address of added watch is ADDR, the length is LEN, and the mask
   is IRW.  Return 1 on success, 0 on failure.  */

int
loongarch_linux_watch_try_one_watch (struct pt_watch_regs *regs,
				     CORE_ADDR addr, int len, uint32_t irw)
{
  CORE_ADDR base_addr, last_byte;
  CORE_ADDR mask_bits, t_addr, t_mask;
  uint8_t t_irw;
  int i;

  if (len <= 0)
    return 0;

  last_byte = addr + len - 1;
  mask_bits = fill_mask (addr ^ last_byte);
  base_addr = addr & ~mask_bits;

  /* Check to see if it is covered by current registers.  */
  for (i = 0; i < loongarch_linux_watch_get_num_valid (regs); i++)
    {
      t_addr = loongarch_linux_watch_get_addr (regs, i);
      t_irw = loongarch_linux_watch_get_irw (regs, i);
      if (t_addr != 0 && irw == ((uint32_t) t_irw & irw))
	{
	  t_mask = loongarch_linux_watch_get_mask (regs, i);
	  if (addr >= t_addr && last_byte <= (t_addr + t_mask))
	    return 1;
	}
    }
  /* Try to find an empty register.  */
  for (i = 0; i < loongarch_linux_watch_get_num_valid (regs); i++)
    {
      t_addr = loongarch_linux_watch_get_addr (regs, i);
      if (t_addr == 0
	  && irw == (loongarch_linux_watch_get_irwmask (regs, i) & irw))
	{
	  /* It fits, we'll take it.  */
	  loongarch_linux_watch_set_addr (regs, i, base_addr);
	  loongarch_linux_watch_set_mask (regs, i, mask_bits);
	  loongarch_linux_watch_set_irw (regs, i, irw);
	  return 1;
	}
    }
  /* It didn't fit anywhere, we failed.  */
  return 0;
}

/* Fill in the watch registers REGS with the currently cached
   watches CURRENT_WATCHES.  */

void
loongarch_linux_watch_populate_regs (
  struct loongarch_watchpoint *current_watches, struct pt_watch_regs *regs)
{
  struct loongarch_watchpoint *w;
  int i;

  /* Clear them out.  */
  for (i = 0; i < loongarch_linux_watch_get_num_valid (regs); i++)
    {
      loongarch_linux_watch_set_addr (regs, i, 0);
      loongarch_linux_watch_set_mask (regs, i, 0);
      loongarch_linux_watch_set_irw (regs, i, 0);
    }

  w = current_watches;
  while (w)
    {
      uint32_t irw = loongarch_linux_watch_type_to_irw (w->type);

      i = loongarch_linux_watch_try_one_watch (regs, w->addr, w->len, irw);
      /* They must all fit, because we previously calculated that they
	 would.  */
      gdb_assert (i);
      w = w->next;
    }
}
