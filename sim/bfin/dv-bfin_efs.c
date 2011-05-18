/* Blackfin Electronic Fuse Serial (EFS) model.
   aka Fuse Serial Number (FSN)

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
#include "devices.h"
#include "dv-bfin_efs.h"

struct bfin_efs
{
  bu32 base;

  /* The actual fuse storage of 256bits.  */
  bu32 mem[8];

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 ctl;
  bu32 _pad0;
  bu32 dat[8];
  bu32 pgm[8];
  bu32 _pad1[13];
  bu32 pgm_timing;
};
#define mmr_base()      offsetof(struct bfin_efs, ctl)
#define mmr_offset(mmr) (offsetof(struct bfin_efs, mmr) - mmr_base())
#define mmr_idx(mmr)    (mmr_offset (mmr) / 4)

static const char * const mmr_names[] =
{
  "EFS_CTL",
  [mmr_idx (dat[0])] = "EFS_DAT0", "EFS_DAT1", "EFS_DAT2",  "EFS_DAT3",
  "EFS_DAT4", "EFS_DAT5", "EFS_DAT6", "EFS_DAT7",
  [mmr_idx (pgm[0])] = "EFS_PGM0", "EFS_PGM1", "EFS_PGM2",  "EFS_PGM3",
  "EFS_PGM4", "EFS_PGM5", "EFS_PGM6", "EFS_PGM7",
  [mmr_idx (pgm_timing)] = "EFS_PGM_TIMING",
};
#define mmr_name(off) (mmr_names[(off) / 4] ? : "<INV>")

static unsigned
bfin_efs_io_write_buffer (struct hw *me, const void *source, int space,
			  address_word addr, unsigned nr_bytes)
{
  struct bfin_efs *efs = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);

  mmr_off = addr - efs->base;
  valuep = (void *)((unsigned long)efs + mmr_base() + mmr_off);

  HW_TRACE_WRITE ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, true);

  switch (mmr_off)
    {
    case mmr_offset(ctl):
      *valuep = value;

      /* XXX: What if someone does READ|PGM ?  What order does the
	      the hardware process things ?  */

      if (value & PGM)
	{
	  int i;

	  /* Only program if the cells aren't locked.  */
	  if (!(efs->mem[7] & (1 << 31)))
	    for (i = 0; i < 8; ++i)
	      efs->mem[i] |= efs->pgm[i];

	  *valuep &= PGM;
	}

      if (value & READ)
	{
	  memcpy (efs->dat, efs->mem, sizeof (efs->mem));
	  *valuep &= READ;
	}

      break;
    case mmr_offset(pgm[0]) ... mmr_offset(pgm[7]):
    case mmr_offset(pgm_timing):
      *valuep = value;
      break;
    case mmr_offset(dat[0]) ... mmr_offset(dat[7]):
      /* XXX: Discard writes ... should this error ?  */
      break;
    default:
      dv_bfin_mmr_invalid (me, addr, nr_bytes, true);
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_efs_io_read_buffer (struct hw *me, void *dest, int space,
			 address_word addr, unsigned nr_bytes)
{
  struct bfin_efs *efs = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr - efs->base;
  valuep = (void *)((unsigned long)efs + mmr_base() + mmr_off);

  HW_TRACE_READ ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, false);

  switch (mmr_off)
    {
    case mmr_offset(ctl):
    case mmr_offset(dat[0]) ... mmr_offset(dat[7]):
    case mmr_offset(pgm[0]) ... mmr_offset(pgm[7]):
    case mmr_offset(pgm_timing):
      dv_store_4 (dest, *valuep);
      break;
    default:
      dv_bfin_mmr_invalid (me, addr, nr_bytes, false);
      break;
    }

  return nr_bytes;
}

static void
attach_bfin_efs_regs (struct hw *me, struct bfin_efs *efs)
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

  if (attach_size != BFIN_MMR_EFS_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_EFS_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  efs->base = attach_address;
}

static void
bfin_efs_finish (struct hw *me)
{
  struct bfin_efs *efs;

  efs = HW_ZALLOC (me, struct bfin_efs);

  set_hw_data (me, efs);
  set_hw_io_read_buffer (me, bfin_efs_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_efs_io_write_buffer);

  attach_bfin_efs_regs (me, efs);
}

const struct hw_descriptor dv_bfin_efs_descriptor[] =
{
  {"bfin_efs", bfin_efs_finish,},
  {NULL, NULL},
};
