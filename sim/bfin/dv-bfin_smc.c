/* Blackfin Static Memory Controller (SMC) model.

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
#include "dv-bfin_smc.h"

struct bfin_smc
{
  bu32 base;
  bu32 bank_base, bank_size;
  struct hw *slaves[4];

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 id, gctl, gstat;
  struct
  {
    bu32 ctl, tim, etim, _pad;
  } b[4];
};
#define mmr_base()      offsetof(struct bfin_smc, id)
#define mmr_offset(mmr) (offsetof(struct bfin_smc, mmr) - mmr_base())
#define mmr_idx(mmr)    (mmr_offset (mmr) / 4)

static const char * const mmr_names[] =
{
  "SMC_ID", "SMC_GCTL", "SMC_GSTAT",
  [mmr_idx (b[0])] = "SMC_B0CTL", "SMC_B0TIM", "SMC_B0ETIM",
  [mmr_idx (b[1])] = "SMC_B1CTL", "SMC_B1TIM", "SMC_B1ETIM",
  [mmr_idx (b[2])] = "SMC_B2CTL", "SMC_B2TIM", "SMC_B2ETIM",
  [mmr_idx (b[3])] = "SMC_B3CTL", "SMC_B3TIM", "SMC_B3ETIM",
};
#define mmr_name(off) (mmr_names[(off) / 4] ? : "<INV>")

static void
bfin_smc_write_bctl (struct hw *me, struct bfin_smc *smc, int i, bu32 bctl)
{
  bu32 ben_old, ben, addr;

  ben_old = smc->b[i].ctl & BANK_EN;
  ben = bctl & BANK_EN;
  addr = smc->bank_base + i * smc->bank_size;

  if (ben_old && !ben)
    {
      HW_TRACE ((me, "detaching bank %i (%#x base)", i, addr));
      sim_core_detach (hw_system (me), NULL, 0, 0, addr);
    }
  else if (!ben_old && ben)
    {
      struct hw *slave = smc->slaves[i];

      HW_TRACE ((me, "attaching bank %i (%#x base) to %s", i, addr,
		 slave ? hw_path (slave) : "<floating pins>"));

      sim_core_attach (hw_system (me), NULL, 0, access_read_write_exec,
		       0, addr, smc->bank_size, 0, slave, NULL);
    }
  else
    HW_TRACE ((me, "leaving bank %i alone (%u -> %u)", i, ben_old, ben));

  smc->b[i].ctl = bctl;
}

static unsigned
bfin_smc_io_write_buffer (struct hw *me, const void *source, int space,
			  address_word addr, unsigned nr_bytes)
{
  struct bfin_smc *smc = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);
  mmr_off = addr - smc->base;
  valuep = (void *)((unsigned long)smc + mmr_base() + mmr_off);

  HW_TRACE_WRITE ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, true);

  switch (mmr_off)
    {
    case mmr_offset(id):
    case mmr_offset(gstat):
      /* XXX: Should generate hwerr ?  */
      break;
    case mmr_offset(gctl):
    case mmr_offset(b[0].tim) ... mmr_offset(b[0].etim):
    case mmr_offset(b[1].tim) ... mmr_offset(b[1].etim):
    case mmr_offset(b[2].tim) ... mmr_offset(b[2].etim):
    case mmr_offset(b[3].tim) ... mmr_offset(b[3].etim):
      *valuep = value;
      break;
    case mmr_offset(b[0].ctl):
      bfin_smc_write_bctl (me, smc, 0, value);
      break;
    case mmr_offset(b[1].ctl):
      bfin_smc_write_bctl (me, smc, 1, value);
      break;
    case mmr_offset(b[2].ctl):
      bfin_smc_write_bctl (me, smc, 2, value);
      break;
    case mmr_offset(b[3].ctl):
      bfin_smc_write_bctl (me, smc, 3, value);
      break;
    default:
      dv_bfin_mmr_invalid (me, addr, nr_bytes, true);
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_smc_io_read_buffer (struct hw *me, void *dest, int space,
			 address_word addr, unsigned nr_bytes)
{
  struct bfin_smc *smc = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr - smc->base;
  valuep = (void *)((unsigned long)smc + mmr_base() + mmr_off);

  HW_TRACE_READ ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, false);

  switch (mmr_off)
    {
    case mmr_offset(id):
    case mmr_offset(gctl):
    case mmr_offset(gstat):
    case mmr_offset(b[0].ctl) ... mmr_offset(b[0].etim):
    case mmr_offset(b[1].ctl) ... mmr_offset(b[1].etim):
    case mmr_offset(b[2].ctl) ... mmr_offset(b[2].etim):
    case mmr_offset(b[3].ctl) ... mmr_offset(b[3].etim):
      dv_store_4 (dest, *valuep);
      break;
    default:
      dv_bfin_mmr_invalid (me, addr, nr_bytes, false);
      break;
    }

  return nr_bytes;
}

static void
bfin_smc_attach_address_callback (struct hw *me,
				  int level,
				  int space,
				  address_word addr,
				  address_word nr_bytes,
				  struct hw *client)
{
  struct bfin_smc *smc = hw_data (me);
  int i;

  HW_TRACE ((me, "attach - level=%d, space=%d, addr=0x%lx, nr_bytes=%lu, client=%s",
	     level, space, (unsigned long) addr, (unsigned long) nr_bytes, hw_path (client)));

  if (addr + nr_bytes > 4)
    hw_abort (me, "smc attaches are done in terms of banks");

  while (nr_bytes--)
    smc->slaves[addr + nr_bytes] = client;

  for (i = 0; i < ARRAY_SIZE (smc->b); ++i)
    bfin_smc_write_bctl (me, smc, i, smc->b[i].ctl);
}

static void
attach_bfin_smc_regs (struct hw *me, struct bfin_smc *smc)
{
  address_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain three addr/size entries");

  if (hw_find_property (me, "type") == NULL)
    hw_abort (me, "Missing \"type\" property");

  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &attach_space, &attach_address, me);
  hw_unit_size_to_attach_size (hw_parent (me), &reg.size, &attach_size, me);

  if (attach_size != BFIN_MMR_SMC_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_SMC_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  smc->base = attach_address;
}

static void
bfin_smc_finish (struct hw *me)
{
  struct bfin_smc *smc;
  int i;

  smc = HW_ZALLOC (me, struct bfin_smc);

  set_hw_data (me, smc);
  set_hw_io_read_buffer (me, bfin_smc_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_smc_io_write_buffer);
  set_hw_attach_address (me, bfin_smc_attach_address_callback);

  attach_bfin_smc_regs (me, smc);

  /* Initialize the SMC.  */
  smc->bank_base = BFIN_SMC_BASE;
  smc->bank_size = 64 * 1024 * 1024;

  smc->id = 0x00000030;
  for (i = 0; i < ARRAY_SIZE (smc->b); ++i)
    {
      smc->b[i].ctl = 0x01000000;
      smc->b[i].tim = 0x01010101;
      smc->b[i].etim = 0x00020200;
      bfin_smc_write_bctl (me, smc, i, smc->b[i].ctl);
    }
}

const struct hw_descriptor dv_bfin_smc_descriptor[] =
{
  {"bfin_smc", bfin_smc_finish,},
  {NULL, NULL},
};
