/* Blackfin Clock Generation Unit (CGU) model.

   Copyright (C) 2010-2016 Free Software Foundation, Inc.
   Contributed by Analog Devices, Inc.

   This file is part of simulators.

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

#include "config.h"

#include "sim-main.h"
#include "machs.h"
#include "devices.h"
#include "dv-bfin_cgu.h"

/* The CGU is made up of sub-devices (PLL/PCU), but for simplicity,
   we have just one device model for all the pieces.  */

struct bfin_cgu
{
  bu32 base;

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 ctl;
  bu32 stat;
  bu32 div;
  bu32 clkoutsel;
  bu32 eng;
};
#define mmr_base()      offsetof(struct bfin_cgu, ctl)
#define mmr_offset(mmr) (offsetof(struct bfin_cgu, mmr) - mmr_base())

static const char * const mmr_names[] =
{
  "CGU_CTL", "CGU_STAT", "CGU_DIV", "CGU_CLKOUTSEL", "CGU_ENG",
};
#define mmr_name(off) mmr_names[(off) / 4]

static unsigned
bfin_cgu_io_write_buffer (struct hw *me, const void *source,
			  int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_cgu *cgu = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);

  mmr_off = addr - cgu->base;
  valuep = (void *)((unsigned long)cgu + mmr_base() + mmr_off);

  HW_TRACE_WRITE ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, true);

  switch (mmr_off)
    {
    default:
      *valuep = value;
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_cgu_io_read_buffer (struct hw *me, void *dest,
			 int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_cgu *cgu = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr - cgu->base;
  valuep = (void *)((unsigned long)cgu + mmr_base() + mmr_off);

  HW_TRACE_READ ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, false);

  switch (mmr_off)
    {
    default:
      dv_store_4 (dest, *valuep);
      break;
    }

  return nr_bytes;
}

static const struct hw_port_descriptor bfin_cgu_ports[] =
{
  { "cgu", 0, 0, output_port, },
  { NULL, 0, 0, 0, },
};

static void
attach_bfin_cgu_regs (struct hw *me, struct bfin_cgu *cgu)
{
  address_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain three addr/size entries");

  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &attach_space, &attach_address, me);
  hw_unit_size_to_attach_size (hw_parent (me), &reg.size, &attach_size, me);

  if (attach_size != BFIN_MMR_CGU_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_CGU_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  cgu->base = attach_address;
}

static void
bfin_cgu_finish (struct hw *me)
{
  struct bfin_cgu *cgu;

  cgu = HW_ZALLOC (me, struct bfin_cgu);

  set_hw_data (me, cgu);
  set_hw_io_read_buffer (me, bfin_cgu_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_cgu_io_write_buffer);
  set_hw_ports (me, bfin_cgu_ports);

  attach_bfin_cgu_regs (me, cgu);

  /* Initialize the CGU.  */
  cgu->ctl  = 0x00001000;
  cgu->stat = 0x000003F5;
  cgu->div  = 0x04084844;
}

const struct hw_descriptor dv_bfin_cgu_descriptor[] =
{
  {"bfin_cgu", bfin_cgu_finish,},
  {NULL, NULL},
};
