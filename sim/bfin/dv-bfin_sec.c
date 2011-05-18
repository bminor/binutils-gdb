/* Blackfin System Event Controller (SEC) model.

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
#include "dv-bfin_sec.h"
#include "dv-bfin_cec.h"

/* The SEC is made up of sub-devices (SCI/SFI/SSI), but for simplicity,
   we have just one device model for all the pieces.  */

struct bfin_sec
{
  /* We assume first element is the base.  */
  bu32 base;

  /* Order after here is important -- matches hardware MMR layout.  */

  /* SEC: System Event Controller */
  bu32 gctl;
  bu32 gstat;
  bu32 raise;
  bu32 end;

  /* SFI: System Fault Interface */
  bu32 fctl;
  bu32 fstat;
  bu32 fsid;
  bu32 fend;
  bu32 fdly;
  bu32 fdly_cur;
  bu32 fsrdly;
  bu32 fsrdly_cur;
  bu32 fcopp;
  bu32 fcopp_cur;
  bu32 _pad0[0xf2];

  /* SCI: System Core Interface */
  struct
  {
    bu32 cctl;
    bu32 cstat;
    bu32 cpnd;
    bu32 cact;
    bu32 cpmsk;
    bu32 cgmsk;
    bu32 cplvl;
    bu32 csid;
    bu32 _pad[8];
  } sci[2];
  bu32 _pad1[0xe8];

  /* SSI: System Source Interface */
  struct
  {
    bu32 sctl;
    bu32 sstat;
  } ssi[140];
};
#define mmr_base()      offsetof(struct bfin_sec, gctl)
#define mmr_offset(mmr) (offsetof(struct bfin_sec, mmr) - mmr_base())
#define mmr_idx(mmr)    (mmr_offset (mmr) / 4)

static const char * const mmr_names[] =
{
  /* SEC */
  [mmr_idx (gctl)] =
  "SEC_GCTL", "SEC_GSTAT", "SEC_RAISE", "SEC_END",
  /* SFI */
  [mmr_idx (fctl)] =
  "SEC_FCTL", "SEC_FSTAT", "SEC_FSID", "SEC_FEND",
  "SEC_FDLY", "SEC_FDLY_CUR", "SEC_FSRDLY",
  "SEC_FSRDLY_CUR", "SEC_FCOPP", "SEC_FCOPP_CUR",
  /* SCI */
  [mmr_idx (sci[0].cctl)] =
  "SEC_CCTL0", "SEC_CSTAT0", "SEC_CPND0", "SEC_CACT0",
  "SEC_CPMSK0", "SEC_CGMSK0", "SEC_CPLVL0", "SEC_CSID0",
  [mmr_idx (sci[1].cctl)] =
  "SEC_CCTL1", "SEC_CSTAT1", "SEC_CPND1", "SEC_CACT1",
  "SEC_CPMSK1", "SEC_CGMSK1", "SEC_CPLVL1", "SEC_CSID1",
  /* SSI */
};
#define mmr_name(off) (mmr_names[(off) / 4] ? : "<INV>")

static unsigned
bfin_sec_io_write_buffer (struct hw *me, const void *source, int space,
			  address_word addr, unsigned nr_bytes)
{
  struct bfin_sec *sec = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu16 *value16p;
  bu32 *value32p;
  void *valuep;

  if (nr_bytes == 4)
    value = dv_load_4 (source);
  else
    value = dv_load_2 (source);

  mmr_off = addr - sec->base;
  valuep = (void *)((unsigned long)sec + mmr_base() + mmr_off);
  value16p = valuep;
  value32p = valuep;

  HW_TRACE_WRITE ();

  switch (mmr_off)
    {
    default:
      *value32p = value;
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_sec_io_read_buffer (struct hw *me, void *dest, int space,
			 address_word addr, unsigned nr_bytes)
{
  struct bfin_sec *sec = hw_data (me);
  bu32 mmr_off;
  bu16 *value16p;
  bu32 *value32p;
  void *valuep;

  mmr_off = addr - sec->base;
  valuep = (void *)((unsigned long)sec + mmr_base() + mmr_off);
  value16p = valuep;
  value32p = valuep;

  HW_TRACE_READ ();

  switch (mmr_off)
    {
    default:
      if (nr_bytes == 2)
	dv_store_2 (dest, 0);
      else
	dv_store_4 (dest, 0);
      break;
    }

  return nr_bytes;
}

/* Give each SEC its own base to make it easier to extract the pin at
   runtime.  The pin is used as its bit position in the SEC MMRs.  */
#define ENC(sec, pin) (((sec) << 8) + (pin))
#define DEC_PIN(pin) ((pin) % 0x100)
#define DEC_SEC(pin) ((pin) >> 8)

/* It would be nice to declare just one set of input_ports, and then
   have the device tree instantiate multiple SECs, but the MMR layout
   on the BF54x/BF561 makes this pretty hard to pull off since their
   regs are interwoven in the address space.  */

#define BFIN_SEC_TO_CEC_PORTS \
  { "ivg7",  IVG7,  0, output_port, }, \
  { "ivg8",  IVG8,  0, output_port, }, \
  { "ivg9",  IVG9,  0, output_port, }, \
  { "ivg10", IVG10, 0, output_port, }, \
  { "ivg11", IVG11, 0, output_port, }, \
  { "ivg12", IVG12, 0, output_port, }, \
  { "ivg13", IVG13, 0, output_port, }, \
  { "ivg14", IVG14, 0, output_port, }, \
  { "ivg15", IVG15, 0, output_port, },

#define SEC_PORTS(n) \
  { "int0@"#n,   ENC(n,  0), 0, input_port, }, \
  { "int1@"#n,   ENC(n,  1), 0, input_port, }, \
  { "int2@"#n,   ENC(n,  2), 0, input_port, }, \
  { "int3@"#n,   ENC(n,  3), 0, input_port, }, \
  { "int4@"#n,   ENC(n,  4), 0, input_port, }, \
  { "int5@"#n,   ENC(n,  5), 0, input_port, }, \
  { "int6@"#n,   ENC(n,  6), 0, input_port, }, \
  { "int7@"#n,   ENC(n,  7), 0, input_port, }, \
  { "int8@"#n,   ENC(n,  8), 0, input_port, }, \
  { "int9@"#n,   ENC(n,  9), 0, input_port, }, \
  { "int10@"#n,  ENC(n, 10), 0, input_port, }, \
  { "int11@"#n,  ENC(n, 11), 0, input_port, }, \
  { "int12@"#n,  ENC(n, 12), 0, input_port, }, \
  { "int13@"#n,  ENC(n, 13), 0, input_port, }, \
  { "int14@"#n,  ENC(n, 14), 0, input_port, }, \
  { "int15@"#n,  ENC(n, 15), 0, input_port, }, \
  { "int16@"#n,  ENC(n, 16), 0, input_port, }, \
  { "int17@"#n,  ENC(n, 17), 0, input_port, }, \
  { "int18@"#n,  ENC(n, 18), 0, input_port, }, \
  { "int19@"#n,  ENC(n, 19), 0, input_port, }, \
  { "int20@"#n,  ENC(n, 20), 0, input_port, }, \
  { "int21@"#n,  ENC(n, 21), 0, input_port, }, \
  { "int22@"#n,  ENC(n, 22), 0, input_port, }, \
  { "int23@"#n,  ENC(n, 23), 0, input_port, }, \
  { "int24@"#n,  ENC(n, 24), 0, input_port, }, \
  { "int25@"#n,  ENC(n, 25), 0, input_port, }, \
  { "int26@"#n,  ENC(n, 26), 0, input_port, }, \
  { "int27@"#n,  ENC(n, 27), 0, input_port, }, \
  { "int28@"#n,  ENC(n, 28), 0, input_port, }, \
  { "int29@"#n,  ENC(n, 29), 0, input_port, }, \
  { "int30@"#n,  ENC(n, 30), 0, input_port, }, \
  { "int31@"#n,  ENC(n, 31), 0, input_port, },

static const struct hw_port_descriptor bfin_sec_ports[] =
{
  BFIN_SEC_TO_CEC_PORTS
  SEC_PORTS(0)
  { NULL, 0, 0, 0, },
};

static void
attach_bfin_sec_regs (struct hw *me, struct bfin_sec *sec)
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

  if (attach_size != BFIN_MMR_SEC_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_SEC_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  sec->base = attach_address;
}

static void
bfin_sec_finish (struct hw *me)
{
  struct bfin_sec *sec;
  int i;

  sec = HW_ZALLOC (me, struct bfin_sec);

  set_hw_data (me, sec);
  set_hw_io_read_buffer (me, bfin_sec_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_sec_io_write_buffer);
  set_hw_ports (me, bfin_sec_ports);
  //set_hw_port_event (me, bfin_sec_52x_port_event);

  attach_bfin_sec_regs (me, sec);

  /* Initialize the SEC.  */
  for (i = 0; i < ARRAY_SIZE (sec->sci); ++i)
    {
      sec->sci[i].cpmsk = 0x000000FF;
      sec->sci[i].cplvl = 0x00000007;
    }
}

const struct hw_descriptor dv_bfin_sec_descriptor[] =
{
  {"bfin_sec", bfin_sec_finish,},
  {NULL, NULL},
};
