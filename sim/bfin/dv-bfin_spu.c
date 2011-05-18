/* Blackfin System Protection Unit (SPU) model.

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
#include "dv-bfin_spu.h"

struct bfin_spu
{
  bu32 base;

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 ctl;
  bu32 stat;
  bu32 _pad0[0x100 - 2];
  bu32 wp[86];
};
#define mmr_base()      offsetof(struct bfin_spu, ctl)
#define mmr_offset(mmr) (offsetof(struct bfin_spu, mmr) - mmr_base())
#define mmr_idx(mmr)    (mmr_offset (mmr) / 4)

static const char * const mmr_names[] =
{
  "SPU_CTL", "SPU_STAT",
};
#define mmr_name(off) (mmr_names[(off) / 4] ? : "<INV>")

static unsigned
bfin_spu_io_write_buffer (struct hw *me, const void *source,
			  int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_spu *spu = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);

  mmr_off = addr - spu->base;
  valuep = (void *)((unsigned long)spu + mmr_base() + mmr_off);

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
bfin_spu_io_read_buffer (struct hw *me, void *dest,
			 int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_spu *spu = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr - spu->base;
  valuep = (void *)((unsigned long)spu + mmr_base() + mmr_off);

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

static const struct hw_port_descriptor bfin_spu_ports[] =
{
  { "spu", 0, 0, output_port, },
  { NULL, 0, 0, 0, },
};

static void
attach_bfin_spu_regs (struct hw *me, struct bfin_spu *spu)
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

  if (attach_size != BFIN_MMR_SPU_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_SPU_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  spu->base = attach_address;
}

static void
bfin_spu_finish (struct hw *me)
{
  struct bfin_spu *spu;

  spu = HW_ZALLOC (me, struct bfin_spu);

  set_hw_data (me, spu);
  set_hw_io_read_buffer (me, bfin_spu_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_spu_io_write_buffer);
  set_hw_ports (me, bfin_spu_ports);

  attach_bfin_spu_regs (me, spu);

  /* Initialize the SPU.  */
  spu->ctl  = 0x000000AD;
}

const struct hw_descriptor dv_bfin_spu_descriptor[] =
{
  {"bfin_spu", bfin_spu_finish,},
  {NULL, NULL},
};
