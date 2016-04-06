/* Opcode table for the ARC.
   Copyright (C) 1994-2016 Free Software Foundation, Inc.

   Contributed by Claudiu Zissulescu (claziss@synopsys.com)

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include "bfd.h"
#include "opcode/arc.h"
#include "opintl.h"
#include "libiberty.h"

/* ARC NPS400 Support: The ARC NPS400 core is an ARC700 with some custom
   instructions.  Support for this target is available when binutils is
   configured and built for the 'arc*-mellanox-*-*' target.  As far as
   possible all ARC NPS400 features are built into all ARC target builds as
   this reduces the chances that regressions might creep in.  */

/* Insert RB register into a 32-bit opcode.  */
static unsigned
insert_rb (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x07) << 24) | (((value >> 3) & 0x07) << 12);
}

static int
extract_rb (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (((insn >> 12) & 0x07) << 3) | ((insn >> 24) & 0x07);

  if (value == 0x3e && invalid)
    *invalid = TRUE; /* A limm operand, it should be extracted in a
			different way.  */

  return value;
}

static unsigned
insert_rad (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value & 0x01)
    *errmsg = _("Improper register value.");

  return insn | (value & 0x3F);
}

static unsigned
insert_rcd (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value & 0x01)
    *errmsg = _("Improper register value.");

  return insn | ((value & 0x3F) << 6);
}

/* Dummy insert ZERO operand function.  */

static unsigned
insert_za (unsigned insn,
	   int value,
	   const char **errmsg)
{
  if (value)
    *errmsg = _("operand is not zero");
  return insn;
}

/* Insert Y-bit in bbit/br instructions.  This function is called only
   when solving fixups.  */

static unsigned
insert_Ybit (unsigned insn,
	     int value,
	     const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value > 0)
    insn |= 0x08;

  return insn;
}

/* Insert Y-bit in bbit/br instructions.  This function is called only
   when solving fixups.  */

static unsigned
insert_NYbit (unsigned insn,
	      int value,
	      const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value < 0)
    insn |= 0x08;

  return insn;
}

/* Insert H register into a 16-bit opcode.  */

static unsigned
insert_rhv1 (unsigned insn,
	     int value,
	     const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn |= ((value & 0x07) << 5) | ((value >> 3) & 0x07);
}

static int
extract_rhv1 (unsigned insn ATTRIBUTE_UNUSED,
	      bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = 0;

  return value;
}

/* Insert H register into a 16-bit opcode.  */

static unsigned
insert_rhv2 (unsigned insn,
	     int value,
	     const char **errmsg)
{
  if (value == 0x1E)
    *errmsg =
      _("Register R30 is a limm indicator for this type of instruction.");
  return insn |= ((value & 0x07) << 5) | ((value >> 3) & 0x03);
}

static int
extract_rhv2 (unsigned insn ATTRIBUTE_UNUSED,
	      bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = ((insn >> 5) & 0x07) | ((insn & 0x03) << 3);

  return value;
}

static unsigned
insert_r0 (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 0)
    *errmsg = _("Register must be R0.");
  return insn;
}

static int
extract_r0 (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 0;
}


static unsigned
insert_r1 (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 1)
    *errmsg = _("Register must be R1.");
  return insn;
}

static int
extract_r1 (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 1;
}

static unsigned
insert_r2 (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 2)
    *errmsg = _("Register must be R2.");
  return insn;
}

static int
extract_r2 (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 2;
}

static unsigned
insert_r3 (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 3)
    *errmsg = _("Register must be R3.");
  return insn;
}

static int
extract_r3 (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 3;
}

static unsigned
insert_sp (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 28)
    *errmsg = _("Register must be SP.");
  return insn;
}

static int
extract_sp (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 28;
}

static unsigned
insert_gp (unsigned insn,
	   int value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 26)
    *errmsg = _("Register must be GP.");
  return insn;
}

static int
extract_gp (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 26;
}

static unsigned
insert_pcl (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 63)
    *errmsg = _("Register must be PCL.");
  return insn;
}

static int
extract_pcl (unsigned insn ATTRIBUTE_UNUSED,
	     bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 63;
}

static unsigned
insert_blink (unsigned insn,
	      int value,
	      const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 31)
    *errmsg = _("Register must be BLINK.");
  return insn;
}

static int
extract_blink (unsigned insn ATTRIBUTE_UNUSED,
	       bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 31;
}

static unsigned
insert_ilink1 (unsigned insn,
	       int value,
	       const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 29)
    *errmsg = _("Register must be ILINK1.");
  return insn;
}

static int
extract_ilink1 (unsigned insn ATTRIBUTE_UNUSED,
		bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 29;
}

static unsigned
insert_ilink2 (unsigned insn,
	       int value,
	       const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 30)
    *errmsg = _("Register must be ILINK2.");
  return insn;
}

static int
extract_ilink2 (unsigned insn ATTRIBUTE_UNUSED,
		bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return 30;
}

static unsigned
insert_ras (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      insn |= value;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      insn |= (value - 8);
      break;
    default:
      *errmsg = _("Register must be either r0-r3 or r12-r15.");
      break;
    }
  return insn;
}

static int
extract_ras (unsigned insn ATTRIBUTE_UNUSED,
	     bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = insn & 0x07;
  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned
insert_rbs (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      insn |= value << 8;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      insn |= ((value - 8)) << 8;
      break;
    default:
      *errmsg = _("Register must be either r0-r3 or r12-r15.");
      break;
    }
  return insn;
}

static int
extract_rbs (unsigned insn ATTRIBUTE_UNUSED,
	     bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 8) & 0x07;
  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned
insert_rcs (unsigned insn,
	    int value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      insn |= value << 5;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      insn |= ((value - 8)) << 5;
      break;
    default:
      *errmsg = _("Register must be either r0-r3 or r12-r15.");
      break;
    }
  return insn;
}

static int
extract_rcs (unsigned insn ATTRIBUTE_UNUSED,
	     bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 5) & 0x07;
  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned
insert_simm3s (unsigned insn,
	       int value,
	       const char **errmsg ATTRIBUTE_UNUSED)
{
  int tmp = 0;
  switch (value)
    {
    case -1:
      tmp = 0x07;
      break;
    case 0:
      tmp = 0x00;
      break;
    case 1:
      tmp = 0x01;
      break;
    case 2:
      tmp = 0x02;
      break;
    case 3:
      tmp = 0x03;
      break;
    case 4:
      tmp = 0x04;
      break;
    case 5:
      tmp = 0x05;
      break;
    case 6:
      tmp = 0x06;
      break;
    default:
      *errmsg = _("Accepted values are from -1 to 6.");
      break;
    }

  insn |= tmp << 8;
  return insn;
}

static int
extract_simm3s (unsigned insn ATTRIBUTE_UNUSED,
		bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 8) & 0x07;
  if (value == 7)
    return -1;
  else
    return value;
}

static unsigned
insert_rrange (unsigned insn,
	       int value,
	       const char **errmsg ATTRIBUTE_UNUSED)
{
  int reg1 = (value >> 16) & 0xFFFF;
  int reg2 = value & 0xFFFF;
  if (reg1 != 13)
    {
      *errmsg = _("First register of the range should be r13.");
      return insn;
    }
  if (reg2 < 13 || reg2 > 26)
    {
      *errmsg = _("Last register of the range doesn't fit.");
      return insn;
    }
  insn |= ((reg2 - 12) & 0x0F) << 1;
  return insn;
}

static int
extract_rrange (unsigned insn  ATTRIBUTE_UNUSED,
		bfd_boolean * invalid  ATTRIBUTE_UNUSED)
{
  return (insn >> 1) & 0x0F;
}

static unsigned
insert_fpel (unsigned insn,
	     int value,
	     const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 27)
    {
      *errmsg = _("Invalid register number, should be fp.");
      return insn;
    }

  insn |= 0x0100;
  return insn;
}

static int
extract_fpel (unsigned insn  ATTRIBUTE_UNUSED,
	      bfd_boolean * invalid  ATTRIBUTE_UNUSED)
{
  return (insn & 0x0100) ? 27 : -1;
}

static unsigned
insert_blinkel (unsigned insn,
		int value,
		const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 31)
    {
      *errmsg = _("Invalid register number, should be blink.");
      return insn;
    }

  insn |= 0x0200;
  return insn;
}

static int
extract_blinkel (unsigned insn  ATTRIBUTE_UNUSED,
		 bfd_boolean * invalid  ATTRIBUTE_UNUSED)
{
  return (insn & 0x0200) ? 31 : -1;
}

static unsigned
insert_pclel (unsigned insn,
	      int value,
	      const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value != 63)
    {
      *errmsg = _("Invalid register number, should be pcl.");
      return insn;
    }

  insn |= 0x0400;
  return insn;
}

static int
extract_pclel (unsigned insn  ATTRIBUTE_UNUSED,
	       bfd_boolean * invalid  ATTRIBUTE_UNUSED)
{
  return (insn & 0x0400) ? 63 : -1;
}

#define INSERT_W6
/* mask = 00000000000000000000111111000000
   insn = 00011bbb000000000BBBwwwwwwDaaZZ1.  */
static unsigned
insert_w6 (unsigned insn ATTRIBUTE_UNUSED,
	   int value ATTRIBUTE_UNUSED,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 0) & 0x003f) << 6;

  return insn;
}

#define EXTRACT_W6
/* mask = 00000000000000000000111111000000.  */
static int
extract_w6 (unsigned insn ATTRIBUTE_UNUSED,
	    bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  unsigned value = 0;

  value |= ((insn >> 6) & 0x003f) << 0;

  return value;
}

#define INSERT_G_S
/* mask = 0000011100022000
   insn = 01000ggghhhGG0HH.  */
static unsigned
insert_g_s (unsigned insn ATTRIBUTE_UNUSED,
	    int value ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 0) & 0x0007) << 8;
  insn |= ((value >> 3) & 0x0003) << 3;

  return insn;
}

#define EXTRACT_G_S
/* mask = 0000011100022000.  */
static int
extract_g_s (unsigned insn ATTRIBUTE_UNUSED,
	     bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = 0;

  value |= ((insn >> 8) & 0x0007) << 0;
  value |= ((insn >> 3) & 0x0003) << 3;

  /* Extend the sign.  */
  int signbit = 1 << (6 - 1);
  value = (value ^ signbit) - signbit;

  return value;
}

/* ARC NPS400 Support: See comment near head of file.  */
static unsigned
insert_nps_3bit_dst (unsigned insn ATTRIBUTE_UNUSED,
                     int value ATTRIBUTE_UNUSED,
                     const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      insn |= value << 24;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      insn |= (value - 8) << 24;
      break;
    default:
      *errmsg = _("Register must be either r0-r3 or r12-r15.");
      break;
    }
  return insn;
}

static int
extract_nps_3bit_dst (unsigned insn ATTRIBUTE_UNUSED,
                      bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 24) & 0x07;
  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned
insert_nps_3bit_src2 (unsigned insn ATTRIBUTE_UNUSED,
                      int value ATTRIBUTE_UNUSED,
                      const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      insn |= value << 21;
      break;
    case 12:
    case 13:
    case 14:
    case 15:
      insn |= (value - 8) << 21;
      break;
    default:
      *errmsg = _("Register must be either r0-r3 or r12-r15.");
      break;
    }
  return insn;
}

static int
extract_nps_3bit_src2 (unsigned insn ATTRIBUTE_UNUSED,
                       bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 21) & 0x07;
  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned
insert_nps_bitop_size (unsigned insn ATTRIBUTE_UNUSED,
                      int value ATTRIBUTE_UNUSED,
                      const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value < 1 || value > 32)
    {
      *errmsg = _("Invalid bit size, should be between 1 and 32 inclusive.");
      return insn;
    }

  --value;
  insn |= ((value & 0x1f) << 10);
  return insn;
}

static int
extract_nps_bitop_size (unsigned insn ATTRIBUTE_UNUSED,
                       bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 10) & 0x1f) + 1;
}

static unsigned
insert_nps_bitop_size_2b (unsigned insn ATTRIBUTE_UNUSED,
                          int value ATTRIBUTE_UNUSED,
                          const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 1:
      value = 0;
      break;
    case 2:
      value = 1;
      break;
    case 4:
      value = 2;
      break;
    case 8:
      value = 3;
      break;
    default:
      value = 0;
      *errmsg = _("Invalid size, should be 1, 2, 4, or 8.");
      break;
    }

  insn |= value << 10;
  return insn;
}

static int
extract_nps_bitop_size_2b (unsigned insn ATTRIBUTE_UNUSED,
                           bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return  1 << ((insn >> 10) & 0x3);
}

static unsigned
insert_nps_bitop_uimm8 (unsigned insn ATTRIBUTE_UNUSED,
                        int value ATTRIBUTE_UNUSED,
                        const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 5) & 7) << 12;
  insn |= (value & 0x1f);
  return insn;
}

static int
extract_nps_bitop_uimm8 (unsigned insn ATTRIBUTE_UNUSED,
                         bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return (((insn >> 12) & 0x7) << 5) | (insn & 0x1f);
}

static unsigned
insert_nps_rflt_uimm6 (unsigned insn ATTRIBUTE_UNUSED,
                       int value ATTRIBUTE_UNUSED,
                       const char **errmsg ATTRIBUTE_UNUSED)
{
  switch (value)
    {
    case 1:
    case 2:
    case 4:
      break;

    default:
      *errmsg = _("invalid immediate, must be 1, 2, or 4");
      value = 0;
    }

  insn |= (value << 6);
  return insn;
}

static int
extract_nps_rflt_uimm6 (unsigned insn ATTRIBUTE_UNUSED,
                         bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return (insn >> 6) & 0x3f;
}

static unsigned
insert_nps_dst_pos_and_size (unsigned insn ATTRIBUTE_UNUSED,
                             int value ATTRIBUTE_UNUSED,
                             const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value & 0x1f) | (((32 - value - 1) & 0x1f) << 10));
  return insn;
}

static int
extract_nps_dst_pos_and_size (unsigned insn ATTRIBUTE_UNUSED,
                              bfd_boolean * invalid ATTRIBUTE_UNUSED)
{
  return (insn & 0x1f);
}

/* Include the generic extract/insert functions.  Order is important
   as some of the functions present in the .h may be disabled via
   defines.  */
#include "arc-fxi.h"

/* The flag operands table.

   The format of the table is
   NAME CODE BITS SHIFT FAVAIL.  */
const struct arc_flag_operand arc_flag_operands[] =
{
#define F_NULL	0
  { 0, 0, 0, 0, 0},
#define F_ALWAYS    (F_NULL + 1)
  { "al", 0, 0, 0, 0 },
#define F_RA	    (F_ALWAYS + 1)
  { "ra", 0, 0, 0, 0 },
#define F_EQUAL	    (F_RA + 1)
  { "eq", 1, 5, 0, 1 },
#define F_ZERO	    (F_EQUAL + 1)
  { "z",  1, 5, 0, 0 },
#define F_NOTEQUAL  (F_ZERO + 1)
  { "ne", 2, 5, 0, 1 },
#define F_NOTZERO   (F_NOTEQUAL + 1)
  { "nz", 2, 5, 0, 0 },
#define F_POZITIVE  (F_NOTZERO + 1)
  { "p",  3, 5, 0, 1 },
#define F_PL	    (F_POZITIVE + 1)
  { "pl", 3, 5, 0, 0 },
#define F_NEGATIVE  (F_PL + 1)
  { "n",  4, 5, 0, 1 },
#define F_MINUS	    (F_NEGATIVE + 1)
  { "mi", 4, 5, 0, 0 },
#define F_CARRY	    (F_MINUS + 1)
  { "c",  5, 5, 0, 1 },
#define F_CARRYSET  (F_CARRY + 1)
  { "cs", 5, 5, 0, 0 },
#define F_LOWER	    (F_CARRYSET + 1)
  { "lo", 5, 5, 0, 0 },
#define F_CARRYCLR  (F_LOWER + 1)
  { "cc", 6, 5, 0, 0 },
#define F_NOTCARRY (F_CARRYCLR + 1)
  { "nc", 6, 5, 0, 1 },
#define F_HIGHER   (F_NOTCARRY + 1)
  { "hs", 6, 5, 0, 0 },
#define F_OVERFLOWSET (F_HIGHER + 1)
  { "vs", 7, 5, 0, 0 },
#define F_OVERFLOW (F_OVERFLOWSET + 1)
  { "v",  7, 5, 0, 1 },
#define F_NOTOVERFLOW (F_OVERFLOW + 1)
  { "nv", 8, 5, 0, 1 },
#define F_OVERFLOWCLR (F_NOTOVERFLOW + 1)
  { "vc", 8, 5, 0, 0 },
#define F_GT	   (F_OVERFLOWCLR + 1)
  { "gt", 9, 5, 0, 1 },
#define F_GE	   (F_GT + 1)
  { "ge", 10, 5, 0, 1 },
#define F_LT	   (F_GE + 1)
  { "lt", 11, 5, 0, 1 },
#define F_LE	   (F_LT + 1)
  { "le", 12, 5, 0, 1 },
#define F_HI	   (F_LE + 1)
  { "hi", 13, 5, 0, 1 },
#define F_LS	   (F_HI + 1)
  { "ls", 14, 5, 0, 1 },
#define F_PNZ	   (F_LS + 1)
  { "pnz", 15, 5, 0, 1 },

  /* FLAG.  */
#define F_FLAG     (F_PNZ + 1)
  { "f",  1, 1, 15, 1 },
#define F_FFAKE     (F_FLAG + 1)
  { "f",  0, 0, 0, 1 },

  /* Delay slot.  */
#define F_ND	   (F_FFAKE + 1)
  { "nd", 0, 1, 5, 0 },
#define F_D	   (F_ND + 1)
  { "d",  1, 1, 5, 1 },
#define F_DFAKE	   (F_D + 1)
  { "d",  0, 0, 0, 1 },

  /* Data size.  */
#define F_SIZEB1   (F_DFAKE + 1)
  { "b", 1, 2, 1, 1 },
#define F_SIZEB7   (F_SIZEB1 + 1)
  { "b", 1, 2, 7, 1 },
#define F_SIZEB17  (F_SIZEB7 + 1)
  { "b", 1, 2, 17, 1 },
#define F_SIZEW1   (F_SIZEB17 + 1)
  { "w", 2, 2, 1, 0 },
#define F_SIZEW7   (F_SIZEW1 + 1)
  { "w", 2, 2, 7, 0 },
#define F_SIZEW17  (F_SIZEW7 + 1)
  { "w", 2, 2, 17, 0 },

  /* Sign extension.  */
#define F_SIGN6   (F_SIZEW17 + 1)
  { "x", 1, 1, 6, 1 },
#define F_SIGN16  (F_SIGN6 + 1)
  { "x", 1, 1, 16, 1 },
#define F_SIGNX   (F_SIGN16 + 1)
  { "x", 0, 0, 0, 1 },

  /* Address write-back modes.  */
#define F_A3       (F_SIGNX + 1)
  { "a", 1, 2, 3, 0 },
#define F_A9       (F_A3 + 1)
  { "a", 1, 2, 9, 0 },
#define F_A22      (F_A9 + 1)
  { "a", 1, 2, 22, 0 },
#define F_AW3      (F_A22 + 1)
  { "aw", 1, 2, 3, 1 },
#define F_AW9      (F_AW3 + 1)
  { "aw", 1, 2, 9, 1 },
#define F_AW22     (F_AW9 + 1)
  { "aw", 1, 2, 22, 1 },
#define F_AB3      (F_AW22 + 1)
  { "ab", 2, 2, 3, 1 },
#define F_AB9      (F_AB3 + 1)
  { "ab", 2, 2, 9, 1 },
#define F_AB22     (F_AB9 + 1)
  { "ab", 2, 2, 22, 1 },
#define F_AS3      (F_AB22 + 1)
  { "as", 3, 2, 3, 1 },
#define F_AS9      (F_AS3 + 1)
  { "as", 3, 2, 9, 1 },
#define F_AS22     (F_AS9 + 1)
  { "as", 3, 2, 22, 1 },
#define F_ASFAKE   (F_AS22 + 1)
  { "as", 0, 0, 0, 1 },

  /* Cache bypass.  */
#define F_DI5     (F_ASFAKE + 1)
  { "di", 1, 1, 5, 1 },
#define F_DI11    (F_DI5 + 1)
  { "di", 1, 1, 11, 1 },
#define F_DI15    (F_DI11 + 1)
  { "di", 1, 1, 15, 1 },

  /* ARCv2 specific.  */
#define F_NT     (F_DI15 + 1)
  { "nt", 0, 1, 3, 1},
#define F_T      (F_NT + 1)
  { "t", 1, 1, 3, 1},
#define F_H1     (F_T + 1)
  { "h", 2, 2, 1, 1 },
#define F_H7     (F_H1 + 1)
  { "h", 2, 2, 7, 1 },
#define F_H17    (F_H7 + 1)
  { "h", 2, 2, 17, 1 },

  /* Fake Flags.  */
#define F_NE   (F_H17 + 1)
  { "ne", 0, 0, 0, 1 },

  /* ARC NPS400 Support: See comment near head of file.  */
#define F_NPS_CL (F_NE + 1)
  { "cl", 0, 0, 0, 1 },

#define F_NPS_FLAG (F_NPS_CL + 1)
  { "f", 1, 1, 20, 1 },

#define F_NPS_R     (F_NPS_FLAG + 1)
  { "r",  1, 1, 15, 1 },

#define F_NPS_RW     (F_NPS_R + 1)
  { "rw", 0, 1, 7, 1 },

#define F_NPS_RD     (F_NPS_RW + 1)
  { "rd", 1, 1, 7, 1 },

#define F_NPS_WFT     (F_NPS_RD + 1)
  { "wft", 0, 0, 0, 1 },

#define F_NPS_IE1     (F_NPS_WFT + 1)
  { "ie1", 1, 2, 8, 1 },

#define F_NPS_IE2     (F_NPS_IE1 + 1)
  { "ie2", 2, 2, 8, 1 },

#define F_NPS_IE12     (F_NPS_IE2 + 1)
  { "ie12", 3, 2, 8, 1 },

#define F_NPS_SYNC_RD     (F_NPS_IE12 + 1)
  { "rd", 0, 1, 6, 1 },

#define F_NPS_SYNC_WR     (F_NPS_SYNC_RD + 1)
  { "wr", 1, 1, 6, 1 },

#define F_NPS_HWS_OFF     (F_NPS_SYNC_WR + 1)
  { "off", 0, 0, 0, 1 },

#define F_NPS_HWS_RESTORE     (F_NPS_HWS_OFF + 1)
  { "restore", 0, 0, 0, 1 },

};

const unsigned arc_num_flag_operands = ARRAY_SIZE (arc_flag_operands);

/* Table of the flag classes.

   The format of the table is
   CLASS {FLAG_CODE}.  */
const struct arc_flag_class arc_flag_classes[] =
{
#define C_EMPTY     0
  { F_CLASS_NONE, { F_NULL } },

#define C_CC	    (C_EMPTY + 1)
  { F_CLASS_OPTIONAL | F_CLASS_EXTEND,
    { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL,
      F_NOTZERO, F_POZITIVE, F_PL, F_NEGATIVE, F_MINUS,
      F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
      F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW,
      F_NOTOVERFLOW, F_OVERFLOWCLR, F_GT, F_GE, F_LT,
      F_LE, F_HI, F_LS, F_PNZ, F_NULL } },

#define C_AA_ADDR3  (C_CC + 1)
#define C_AA27	    (C_CC + 1)
  { F_CLASS_OPTIONAL, { F_A3, F_AW3, F_AB3, F_AS3, F_NULL } },
#define C_AA_ADDR9  (C_AA_ADDR3 + 1)
#define C_AA21	     (C_AA_ADDR3 + 1)
  { F_CLASS_OPTIONAL, { F_A9, F_AW9, F_AB9, F_AS9, F_NULL } },
#define C_AA_ADDR22 (C_AA_ADDR9 + 1)
#define C_AA8	   (C_AA_ADDR9 + 1)
  { F_CLASS_OPTIONAL, { F_A22, F_AW22, F_AB22, F_AS22, F_NULL } },

#define C_F	    (C_AA_ADDR22 + 1)
  { F_CLASS_OPTIONAL, { F_FLAG, F_NULL } },
#define C_FHARD	    (C_F + 1)
  { F_CLASS_OPTIONAL, { F_FFAKE, F_NULL } },

#define C_T	    (C_FHARD + 1)
  { F_CLASS_OPTIONAL, { F_NT, F_T, F_NULL } },
#define C_D	    (C_T + 1)
  { F_CLASS_OPTIONAL, { F_ND, F_D, F_NULL } },

#define C_DHARD	    (C_D + 1)
  { F_CLASS_OPTIONAL, { F_DFAKE, F_NULL } },

#define C_DI20	    (C_DHARD + 1)
  { F_CLASS_OPTIONAL, { F_DI11, F_NULL }},
#define C_DI16	    (C_DI20 + 1)
  { F_CLASS_OPTIONAL, { F_DI15, F_NULL }},
#define C_DI26	    (C_DI16 + 1)
  { F_CLASS_OPTIONAL, { F_DI5, F_NULL }},

#define C_X25	    (C_DI26 + 1)
  { F_CLASS_OPTIONAL, { F_SIGN6, F_NULL }},
#define C_X15	   (C_X25 + 1)
  { F_CLASS_OPTIONAL, { F_SIGN16, F_NULL }},
#define C_XHARD	   (C_X15 + 1)
#define C_X	   (C_X15 + 1)
  { F_CLASS_OPTIONAL, { F_SIGNX, F_NULL }},

#define C_ZZ13	      (C_X + 1)
  { F_CLASS_OPTIONAL, { F_SIZEB17, F_SIZEW17, F_H17, F_NULL}},
#define C_ZZ23	      (C_ZZ13 + 1)
  { F_CLASS_OPTIONAL, { F_SIZEB7, F_SIZEW7, F_H7, F_NULL}},
#define C_ZZ29	      (C_ZZ23 + 1)
  { F_CLASS_OPTIONAL, { F_SIZEB1, F_SIZEW1, F_H1, F_NULL}},

#define C_AS	    (C_ZZ29 + 1)
  { F_CLASS_OPTIONAL, { F_ASFAKE, F_NULL}},

#define C_NE	    (C_AS + 1)
  { F_CLASS_OPTIONAL, { F_NE, F_NULL}},

  /* ARC NPS400 Support: See comment near head of file.  */
#define C_NPS_CL     (C_NE + 1)
  { F_CLASS_REQUIRED, { F_NPS_CL, F_NULL}},

#define C_NPS_F     (C_NPS_CL + 1)
  { F_CLASS_OPTIONAL, { F_NPS_FLAG, F_NULL}},

#define C_NPS_R     (C_NPS_F + 1)
  { F_CLASS_OPTIONAL, { F_NPS_R, F_NULL}},

#define C_NPS_SCHD_RW     (C_NPS_R + 1)
  { F_CLASS_REQUIRED, { F_NPS_RW, F_NPS_RD, F_NULL}},

#define C_NPS_SCHD_TRIG     (C_NPS_SCHD_RW + 1)
  { F_CLASS_REQUIRED, { F_NPS_WFT, F_NULL}},

#define C_NPS_SCHD_IE     (C_NPS_SCHD_TRIG + 1)
  { F_CLASS_OPTIONAL, { F_NPS_IE1, F_NPS_IE2, F_NPS_IE12, F_NULL}},

#define C_NPS_SYNC     (C_NPS_SCHD_IE + 1)
  { F_CLASS_REQUIRED, { F_NPS_SYNC_RD, F_NPS_SYNC_WR, F_NULL}},

#define C_NPS_HWS_OFF     (C_NPS_SYNC + 1)
  { F_CLASS_REQUIRED, { F_NPS_HWS_OFF, F_NULL}},

#define C_NPS_HWS_RESTORE     (C_NPS_HWS_OFF + 1)
  { F_CLASS_REQUIRED, { F_NPS_HWS_RESTORE, F_NULL}},

};

const unsigned char flags_none[] = { 0 };
const unsigned char flags_f[]    = { C_F };
const unsigned char flags_cc[]   = { C_CC };
const unsigned char flags_ccf[]  = { C_CC, C_F };

/* The operands table.

   The format of the operands table is:

   BITS SHIFT DEFAULT_RELOC FLAGS INSERT_FUN EXTRACT_FUN.  */
const struct arc_operand arc_operands[] =
{
  /* The fields are bits, shift, insert, extract, flags.  The zero
     index is used to indicate end-of-list.  */
#define UNUSED		0
  { 0, 0, 0, 0, 0, 0 },
  /* The plain integer register fields.  Used by 32 bit
     instructions.  */
#define RA		(UNUSED + 1)
  { 6, 0, 0, ARC_OPERAND_IR, 0, 0 },
#define RB		(RA + 1)
  { 6, 12, 0, ARC_OPERAND_IR, insert_rb, extract_rb },
#define RC		(RB + 1)
  { 6, 6, 0, ARC_OPERAND_IR, 0, 0 },
#define RBdup		(RC + 1)
  { 6, 12, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE, insert_rb, extract_rb },

#define RAD		(RBdup + 1)
  { 6, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_TRUNCATE, insert_rad, 0 },
#define RCD		(RAD + 1)
  { 6, 6, 0, ARC_OPERAND_IR | ARC_OPERAND_TRUNCATE, insert_rcd, 0 },

  /* The plain integer register fields.  Used by short
     instructions.  */
#define RA16		(RCD + 1)
#define RA_S		(RCD + 1)
  { 4, 0, 0, ARC_OPERAND_IR, insert_ras, extract_ras },
#define RB16		(RA16 + 1)
#define RB_S		(RA16 + 1)
  { 4, 8, 0, ARC_OPERAND_IR, insert_rbs, extract_rbs },
#define RB16dup		(RB16 + 1)
#define RB_Sdup		(RB16 + 1)
  { 4, 8, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE, insert_rbs, extract_rbs },
#define RC16		(RB16dup + 1)
#define RC_S		(RB16dup + 1)
  { 4, 5, 0, ARC_OPERAND_IR, insert_rcs, extract_rcs },
#define R6H		(RC16 + 1)   /* 6bit register field 'h' used
					by V1 cpus.  */
  { 6, 5, 0, ARC_OPERAND_IR, insert_rhv1, extract_rhv1 },
#define R5H		(R6H + 1)    /* 5bit register field 'h' used
					by V2 cpus.  */
#define RH_S		(R6H + 1)    /* 5bit register field 'h' used
					by V2 cpus.  */
  { 5, 5, 0, ARC_OPERAND_IR, insert_rhv2, extract_rhv2 },
#define R5Hdup		(R5H + 1)
#define RH_Sdup		(R5H + 1)
  { 5, 5, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE,
    insert_rhv2, extract_rhv2 },

#define RG		(R5Hdup + 1)
#define G_S		(R5Hdup + 1)
  { 5, 5, 0, ARC_OPERAND_IR, insert_g_s, extract_g_s },

  /* Fix registers.  */
#define R0		(RG + 1)
#define R0_S		(RG + 1)
  { 0, 0, 0, ARC_OPERAND_IR, insert_r0, extract_r0 },
#define R1		(R0 + 1)
#define R1_S		(R0 + 1)
  { 1, 0, 0, ARC_OPERAND_IR, insert_r1, extract_r1 },
#define R2		(R1 + 1)
#define R2_S		(R1 + 1)
  { 2, 0, 0, ARC_OPERAND_IR, insert_r2, extract_r2 },
#define R3		(R2 + 1)
#define R3_S		(R2 + 1)
  { 2, 0, 0, ARC_OPERAND_IR, insert_r3, extract_r3 },
#define RSP		(R3 + 1)
#define SP_S		(R3 + 1)
  { 5, 0, 0, ARC_OPERAND_IR, insert_sp, extract_sp },
#define SPdup		(RSP + 1)
#define SP_Sdup		(RSP + 1)
  { 5, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE, insert_sp, extract_sp },
#define GP		(SPdup + 1)
#define GP_S		(SPdup + 1)
  { 5, 0, 0, ARC_OPERAND_IR, insert_gp, extract_gp },

#define PCL_S		(GP + 1)
  { 1, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_NCHK, insert_pcl, extract_pcl },

#define BLINK		(PCL_S + 1)
#define BLINK_S		(PCL_S + 1)
  { 5, 0, 0, ARC_OPERAND_IR, insert_blink, extract_blink },

#define ILINK1		(BLINK + 1)
  { 5, 0, 0, ARC_OPERAND_IR, insert_ilink1, extract_ilink1 },
#define ILINK2		(ILINK1 + 1)
  { 5, 0, 0, ARC_OPERAND_IR, insert_ilink2, extract_ilink2 },

  /* Long immediate.  */
#define LIMM		(ILINK2 + 1)
#define LIMM_S		(ILINK2 + 1)
  { 32, 0, BFD_RELOC_ARC_32_ME, ARC_OPERAND_LIMM, insert_limm, 0 },
#define LIMMdup		(LIMM + 1)
  { 32, 0, 0, ARC_OPERAND_LIMM | ARC_OPERAND_DUPLICATE, insert_limm, 0 },

  /* Special operands.  */
#define ZA		(LIMMdup + 1)
#define ZB		(LIMMdup + 1)
#define ZA_S		(LIMMdup + 1)
#define ZB_S		(LIMMdup + 1)
#define ZC_S		(LIMMdup + 1)
  { 0, 0, 0, ARC_OPERAND_UNSIGNED, insert_za, 0 },

#define RRANGE_EL	(ZA + 1)
  { 4, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_NCHK | ARC_OPERAND_TRUNCATE,
    insert_rrange, extract_rrange},
#define FP_EL		(RRANGE_EL + 1)
  { 1, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_IGNORE | ARC_OPERAND_NCHK,
    insert_fpel, extract_fpel },
#define BLINK_EL	(FP_EL + 1)
  { 1, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_IGNORE | ARC_OPERAND_NCHK,
    insert_blinkel, extract_blinkel },
#define PCL_EL		(BLINK_EL + 1)
  { 1, 0, 0, ARC_OPERAND_IR | ARC_OPERAND_IGNORE | ARC_OPERAND_NCHK,
    insert_pclel, extract_pclel },

  /* Fake operand to handle the T flag.  */
#define BRAKET		(PCL_EL + 1)
#define BRAKETdup	(PCL_EL + 1)
  { 0, 0, 0, ARC_OPERAND_FAKE | ARC_OPERAND_BRAKET, 0, 0 },

  /* Fake operand to handle the T flag.  */
#define FKT_T		(BRAKET + 1)
  { 1, 3, 0, ARC_OPERAND_FAKE, insert_Ybit, 0 },
  /* Fake operand to handle the T flag.  */
#define FKT_NT		(FKT_T + 1)
  { 1, 3, 0, ARC_OPERAND_FAKE, insert_NYbit, 0 },

  /* UIMM6_20 mask = 00000000000000000000111111000000.  */
#define UIMM6_20       (FKT_NT + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm6_20, extract_uimm6_20},

  /* SIMM12_20 mask = 00000000000000000000111111222222.  */
#define SIMM12_20	(UIMM6_20 + 1)
  {12, 0, 0, ARC_OPERAND_SIGNED, insert_simm12_20, extract_simm12_20},

  /* SIMM3_5_S mask = 0000011100000000.  */
#define SIMM3_5_S	(SIMM12_20 + 1)
  {3, 0, 0, ARC_OPERAND_SIGNED | ARC_OPERAND_NCHK,
   insert_simm3s, extract_simm3s},

  /* UIMM7_A32_11_S mask = 0000000000011111.  */
#define UIMM7_A32_11_S	     (SIMM3_5_S + 1)
  {7, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_IGNORE, insert_uimm7_a32_11_s,
   extract_uimm7_a32_11_s},

  /* UIMM7_9_S mask = 0000000001111111.  */
#define UIMM7_9_S	(UIMM7_A32_11_S + 1)
  {7, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm7_9_s, extract_uimm7_9_s},

  /* UIMM3_13_S mask = 0000000000000111.  */
#define UIMM3_13_S	 (UIMM7_9_S + 1)
  {3, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm3_13_s, extract_uimm3_13_s},

  /* SIMM11_A32_7_S mask = 0000000111111111.  */
#define SIMM11_A32_7_S	     (UIMM3_13_S + 1)
  {11, 0, BFD_RELOC_ARC_SDA16_LD2, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE, insert_simm11_a32_7_s, extract_simm11_a32_7_s},

  /* UIMM6_13_S mask = 0000000002220111.  */
#define UIMM6_13_S	 (SIMM11_A32_7_S + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm6_13_s, extract_uimm6_13_s},
  /* UIMM5_11_S mask = 0000000000011111.  */
#define UIMM5_11_S	 (UIMM6_13_S + 1)
  {5, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_IGNORE, insert_uimm5_11_s,
   extract_uimm5_11_s},

  /* SIMM9_A16_8 mask = 00000000111111102000000000000000.  */
#define SIMM9_A16_8	  (UIMM5_11_S + 1)
  {9, 0, -SIMM9_A16_8, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_PCREL | ARC_OPERAND_TRUNCATE, insert_simm9_a16_8,
   extract_simm9_a16_8},

  /* UIMM6_8 mask = 00000000000000000000111111000000.	 */
#define UIMM6_8	      (SIMM9_A16_8 + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm6_8, extract_uimm6_8},

  /* SIMM21_A16_5 mask = 00000111111111102222222222000000.  */
#define SIMM21_A16_5	   (UIMM6_8 + 1)
  {21, 0, BFD_RELOC_ARC_S21H_PCREL, ARC_OPERAND_SIGNED
   | ARC_OPERAND_ALIGNED16 | ARC_OPERAND_TRUNCATE,
   insert_simm21_a16_5, extract_simm21_a16_5},

  /* SIMM25_A16_5 mask = 00000111111111102222222222003333.  */
#define SIMM25_A16_5	   (SIMM21_A16_5 + 1)
  {25, 0, BFD_RELOC_ARC_S25H_PCREL, ARC_OPERAND_SIGNED
   | ARC_OPERAND_ALIGNED16 | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL,
   insert_simm25_a16_5, extract_simm25_a16_5},

  /* SIMM10_A16_7_S mask = 0000000111111111.  */
#define SIMM10_A16_7_S	     (SIMM25_A16_5 + 1)
  {10, 0, -SIMM10_A16_7_S, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm10_a16_7_s,
   extract_simm10_a16_7_s},

#define SIMM10_A16_7_Sbis    (SIMM10_A16_7_S + 1)
  {10, 0, -SIMM10_A16_7_Sbis, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE, insert_simm10_a16_7_s, extract_simm10_a16_7_s},

  /* SIMM7_A16_10_S mask = 0000000000111111.  */
#define SIMM7_A16_10_S	     (SIMM10_A16_7_Sbis + 1)
  {7, 0, -SIMM7_A16_10_S, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm7_a16_10_s,
   extract_simm7_a16_10_s},

  /* SIMM21_A32_5 mask = 00000111111111002222222222000000.  */
#define SIMM21_A32_5	   (SIMM7_A16_10_S + 1)
  {21, 0, BFD_RELOC_ARC_S21W_PCREL, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm21_a32_5,
   extract_simm21_a32_5},

  /* SIMM25_A32_5 mask = 00000111111111002222222222003333.  */
#define SIMM25_A32_5	   (SIMM21_A32_5 + 1)
  {25, 0, BFD_RELOC_ARC_S25W_PCREL, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm25_a32_5,
   extract_simm25_a32_5},

  /* SIMM13_A32_5_S mask = 0000011111111111.  */
#define SIMM13_A32_5_S	     (SIMM25_A32_5 + 1)
  {13, 0, BFD_RELOC_ARC_S13_PCREL, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm13_a32_5_s,
   extract_simm13_a32_5_s},

  /* SIMM8_A16_9_S mask = 0000000001111111.  */
#define SIMM8_A16_9_S	    (SIMM13_A32_5_S + 1)
  {8, 0, -SIMM8_A16_9_S, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm8_a16_9_s,
   extract_simm8_a16_9_s},

  /* UIMM3_23 mask = 00000000000000000000000111000000.  */
#define UIMM3_23       (SIMM8_A16_9_S + 1)
  {3, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm3_23, extract_uimm3_23},

  /* UIMM10_6_S mask = 0000001111111111.  */
#define UIMM10_6_S	 (UIMM3_23 + 1)
  {10, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm10_6_s, extract_uimm10_6_s},

  /* UIMM6_11_S mask = 0000002200011110.  */
#define UIMM6_11_S	 (UIMM10_6_S + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm6_11_s, extract_uimm6_11_s},

  /* SIMM9_8 mask = 00000000111111112000000000000000.	 */
#define SIMM9_8	      (UIMM6_11_S + 1)
  {9, 0, BFD_RELOC_ARC_SDA_LDST, ARC_OPERAND_SIGNED | ARC_OPERAND_IGNORE,
   insert_simm9_8, extract_simm9_8},

  /* UIMM10_A32_8_S mask = 0000000011111111.  */
#define UIMM10_A32_8_S	     (SIMM9_8 + 1)
  {10, 0, -UIMM10_A32_8_S, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_uimm10_a32_8_s,
   extract_uimm10_a32_8_s},

  /* SIMM9_7_S mask = 0000000111111111.  */
#define SIMM9_7_S	(UIMM10_A32_8_S + 1)
  {9, 0, BFD_RELOC_ARC_SDA16_LD, ARC_OPERAND_SIGNED, insert_simm9_7_s,
   extract_simm9_7_s},

  /* UIMM6_A16_11_S mask = 0000000000011111.  */
#define UIMM6_A16_11_S	     (SIMM9_7_S + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE  | ARC_OPERAND_IGNORE, insert_uimm6_a16_11_s,
   extract_uimm6_a16_11_s},

  /* UIMM5_A32_11_S mask = 0000020000011000.  */
#define UIMM5_A32_11_S	     (UIMM6_A16_11_S + 1)
  {5, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_IGNORE, insert_uimm5_a32_11_s,
   extract_uimm5_a32_11_s},

  /* SIMM11_A32_13_S mask = 0000022222200111.	 */
#define SIMM11_A32_13_S	      (UIMM5_A32_11_S + 1)
  {11, 0, BFD_RELOC_ARC_SDA16_ST2, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED32
   | ARC_OPERAND_TRUNCATE, insert_simm11_a32_13_s, extract_simm11_a32_13_s},

  /* UIMM7_13_S mask = 0000000022220111.  */
#define UIMM7_13_S	 (SIMM11_A32_13_S + 1)
  {7, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm7_13_s, extract_uimm7_13_s},

  /* UIMM6_A16_21 mask = 00000000000000000000011111000000.  */
#define UIMM6_A16_21	   (UIMM7_13_S + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE, insert_uimm6_a16_21, extract_uimm6_a16_21},

  /* UIMM7_11_S mask = 0000022200011110.  */
#define UIMM7_11_S	 (UIMM6_A16_21 + 1)
  {7, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm7_11_s, extract_uimm7_11_s},

  /* UIMM7_A16_20 mask = 00000000000000000000111111000000.  */
#define UIMM7_A16_20	   (UIMM7_11_S + 1)
  {7, 0, -UIMM7_A16_20, ARC_OPERAND_UNSIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_uimm7_a16_20,
   extract_uimm7_a16_20},

  /* SIMM13_A16_20 mask = 00000000000000000000111111222222.  */
#define SIMM13_A16_20	    (UIMM7_A16_20 + 1)
  {13, 0, -SIMM13_A16_20, ARC_OPERAND_SIGNED | ARC_OPERAND_ALIGNED16
   | ARC_OPERAND_TRUNCATE | ARC_OPERAND_PCREL, insert_simm13_a16_20,
   extract_simm13_a16_20},

  /* UIMM8_8_S mask = 0000000011111111.  */
#define UIMM8_8_S	(SIMM13_A16_20 + 1)
  {8, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm8_8_s, extract_uimm8_8_s},

  /* W6 mask = 00000000000000000000111111000000.  */
#define W6	 (UIMM8_8_S + 1)
  {6, 0, 0, ARC_OPERAND_SIGNED, insert_w6, extract_w6},

  /* UIMM6_5_S mask = 0000011111100000.  */
#define UIMM6_5_S	(W6 + 1)
  {6, 0, 0, ARC_OPERAND_UNSIGNED, insert_uimm6_5_s, extract_uimm6_5_s},

  /* ARC NPS400 Support: See comment near head of file.  */
#define NPS_R_DST_3B	(UIMM6_5_S + 1)
  { 3, 24, 0, ARC_OPERAND_IR | ARC_OPERAND_NCHK, insert_nps_3bit_dst, extract_nps_3bit_dst },

#define NPS_R_SRC1_3B	(NPS_R_DST_3B + 1)
  { 3, 24, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE | ARC_OPERAND_NCHK, insert_nps_3bit_dst, extract_nps_3bit_dst },

#define NPS_R_SRC2_3B	(NPS_R_SRC1_3B + 1)
  { 3, 21, 0, ARC_OPERAND_IR | ARC_OPERAND_NCHK, insert_nps_3bit_src2, extract_nps_3bit_src2 },

#define NPS_R_DST	(NPS_R_SRC2_3B + 1)
  { 6, 21, 0, ARC_OPERAND_IR, NULL, NULL },

#define NPS_R_SRC1	(NPS_R_DST + 1)
  { 6, 21, 0, ARC_OPERAND_IR | ARC_OPERAND_DUPLICATE, NULL, NULL },

#define NPS_BITOP_DST_POS	(NPS_R_SRC1 + 1)
  { 5, 5, 0, ARC_OPERAND_UNSIGNED, 0, 0 },

#define NPS_BITOP_SRC_POS	(NPS_BITOP_DST_POS + 1)
  { 5, 0, 0, ARC_OPERAND_UNSIGNED, 0, 0 },

#define NPS_BITOP_SIZE		(NPS_BITOP_SRC_POS + 1)
  { 5, 10, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_NCHK, insert_nps_bitop_size, extract_nps_bitop_size },

#define NPS_BITOP_DST_POS_SZ    (NPS_BITOP_SIZE + 1)
  { 5, 0, 0, ARC_OPERAND_UNSIGNED, insert_nps_dst_pos_and_size, extract_nps_dst_pos_and_size },

#define NPS_BITOP_SIZE_2B	(NPS_BITOP_DST_POS_SZ + 1)
  { 0, 0, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_NCHK, insert_nps_bitop_size_2b, extract_nps_bitop_size_2b },

#define NPS_BITOP_UIMM8		(NPS_BITOP_SIZE_2B + 1)
  { 8, 0, 0, ARC_OPERAND_UNSIGNED, insert_nps_bitop_uimm8, extract_nps_bitop_uimm8 },

#define NPS_UIMM16		(NPS_BITOP_UIMM8 + 1)
  { 16, 0, 0, ARC_OPERAND_UNSIGNED, NULL, NULL },

#define NPS_RFLT_UIMM6		(NPS_UIMM16 + 1)
  { 6, 6, 0, ARC_OPERAND_UNSIGNED | ARC_OPERAND_NCHK, insert_nps_rflt_uimm6, extract_nps_rflt_uimm6 },
};

const unsigned arc_num_operands = ARRAY_SIZE (arc_operands);

const unsigned arc_Toperand = FKT_T;
const unsigned arc_NToperand = FKT_NT;

const unsigned char arg_none[]		 = { 0 };
const unsigned char arg_32bit_rarbrc[]	 = { RA, RB, RC };
const unsigned char arg_32bit_zarbrc[]	 = { ZA, RB, RC };
const unsigned char arg_32bit_rbrbrc[]	 = { RB, RBdup, RC };
const unsigned char arg_32bit_rarbu6[]	 = { RA, RB, UIMM6_20 };
const unsigned char arg_32bit_zarbu6[]	 = { ZA, RB, UIMM6_20 };
const unsigned char arg_32bit_rbrbu6[]	 = { RB, RBdup, UIMM6_20 };
const unsigned char arg_32bit_rbrbs12[]	 = { RB, RBdup, SIMM12_20 };
const unsigned char arg_32bit_ralimmrc[] = { RA, LIMM, RC };
const unsigned char arg_32bit_rarblimm[] = { RA, RB, LIMM };
const unsigned char arg_32bit_zalimmrc[] = { ZA, LIMM, RC };
const unsigned char arg_32bit_zarblimm[] = { ZA, RB, LIMM };

const unsigned char arg_32bit_rbrblimm[] = { RB, RBdup, LIMM };
const unsigned char arg_32bit_ralimmu6[] = { RA, LIMM, UIMM6_20 };
const unsigned char arg_32bit_zalimmu6[] = { ZA, LIMM, UIMM6_20 };

const unsigned char arg_32bit_zalimms12[]  = { ZA, LIMM, SIMM12_20 };
const unsigned char arg_32bit_ralimmlimm[] = { RA, LIMM, LIMMdup };
const unsigned char arg_32bit_zalimmlimm[] = { ZA, LIMM, LIMMdup };

const unsigned char arg_32bit_rbrc[]   = { RB, RC };
const unsigned char arg_32bit_zarc[]   = { ZA, RC };
const unsigned char arg_32bit_rbu6[]   = { RB, UIMM6_20 };
const unsigned char arg_32bit_zau6[]   = { ZA, UIMM6_20 };
const unsigned char arg_32bit_rblimm[] = { RB, LIMM };
const unsigned char arg_32bit_zalimm[] = { ZA, LIMM };

const unsigned char arg_32bit_limmrc[]   = { LIMM, RC };
const unsigned char arg_32bit_limmu6[]   = { LIMM, UIMM6_20 };
const unsigned char arg_32bit_limms12[]  = { LIMM, SIMM12_20 };
const unsigned char arg_32bit_limmlimm[] = { LIMM, LIMMdup };

/* The opcode table.

   The format of the opcode table is:

   NAME OPCODE MASK CPU CLASS SUBCLASS { OPERANDS } { FLAGS }.

   The table is organised such that, where possible, all instructions with
   the same mnemonic are together in a block.  When the assembler searches
   for a suitable instruction the entries are checked in table order, so
   more specific, or specialised cases should appear earlier in the table.

   As an example, consider two instructions 'add a,b,u6' and 'add
   a,b,limm'.  The first takes a 6-bit immediate that is encoded within the
   32-bit instruction, while the second takes a 32-bit immediate that is
   encoded in a follow-on 32-bit, making the total instruction length
   64-bits.  In this case the u6 variant must appear first in the table, as
   all u6 immediates could also be encoded using the 'limm' extension,
   however, we want to use the shorter instruction wherever possible.

   It is possible though to split instructions with the same mnemonic into
   multiple groups.  However, the instructions are still checked in table
   order, even across groups.  The only time that instructions with the
   same mnemonic should be split into different groups is when different
   variants of the instruction appear in different architectures, in which
   case, grouping all instructions from a particular architecture together
   might be preferable to merging the instruction into the main instruction
   table.

   An example of this split instruction groups can be found with the 'sync'
   instruction.  The core arc architecture provides a 'sync' instruction,
   while the nps instruction set extension provides 'sync.rd' and
   'sync.wr'.  The rd/wr flags are instruction flags, not part of the
   mnemonic, so we end up with two groups for the sync instruction, the
   first within the core arc instruction table, and the second within the
   nps extension instructions.  */
const struct arc_opcode arc_opcodes[] =
{
#include "arc-tbl.h"
#include "arc-nps400-tbl.h"
#include "arc-ext-tbl.h"

  { NULL, 0, 0, 0, 0, 0, { 0 }, { 0 } }
};

/* List with special cases instructions and the applicable flags.  */
const struct arc_flag_special arc_flag_special_cases[] =
{
  { "b", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "bl", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	    F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	    F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	    F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "br", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	    F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	    F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	    F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "j", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "jl", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	    F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	    F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	    F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "lp", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	    F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	    F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	    F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "set", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	     F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	     F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	     F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NULL } },
  { "ld", { F_SIZEB17, F_SIZEW17, F_H17, F_NULL } },
  { "st", { F_SIZEB1, F_SIZEW1, F_H1, F_NULL } }
};

const unsigned arc_num_flag_special = ARRAY_SIZE (arc_flag_special_cases);

/* Relocations.  */
const struct arc_reloc_equiv_tab arc_reloc_equiv[] =
{
  { "sda", "ld", { F_ASFAKE, F_H1, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST1 },
  { "sda", "st", { F_ASFAKE, F_H1, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST1 },
  { "sda", "ld", { F_ASFAKE, F_SIZEW7, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST1 },
  { "sda", "st", { F_ASFAKE, F_SIZEW7, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST1 },

  /* Next two entries will cover the undefined behavior ldb/stb with
     address scaling.  */
  { "sda", "ld", { F_ASFAKE, F_SIZEB7, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST },
  { "sda", "st", { F_ASFAKE, F_SIZEB7, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST},

  { "sda", "ld", { F_ASFAKE, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST2 },
  { "sda", "st", { F_ASFAKE, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST2},
  { "sda", "ldd", { F_ASFAKE, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST2 },
  { "sda", "std", { F_ASFAKE, F_NULL },
    BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST2},

  /* Short instructions.  */
  { "sda", 0, { F_NULL }, BFD_RELOC_ARC_SDA16_LD, BFD_RELOC_ARC_SDA16_LD },
  { "sda", 0, { F_NULL }, -SIMM10_A16_7_Sbis, BFD_RELOC_ARC_SDA16_LD1 },
  { "sda", 0, { F_NULL }, BFD_RELOC_ARC_SDA16_LD2, BFD_RELOC_ARC_SDA16_LD2 },
  { "sda", 0, { F_NULL }, BFD_RELOC_ARC_SDA16_ST2, BFD_RELOC_ARC_SDA16_ST2 },

  { "sda", 0, { F_NULL }, BFD_RELOC_ARC_32_ME, BFD_RELOC_ARC_SDA32_ME },
  { "sda", 0, { F_NULL }, BFD_RELOC_ARC_SDA_LDST, BFD_RELOC_ARC_SDA_LDST },

  { "plt", 0, { F_NULL }, BFD_RELOC_ARC_S25H_PCREL,
    BFD_RELOC_ARC_S25H_PCREL_PLT },
  { "plt", 0, { F_NULL }, BFD_RELOC_ARC_S21H_PCREL,
    BFD_RELOC_ARC_S21H_PCREL_PLT },
  { "plt", 0, { F_NULL }, BFD_RELOC_ARC_S25W_PCREL,
    BFD_RELOC_ARC_S25W_PCREL_PLT },
  { "plt", 0, { F_NULL }, BFD_RELOC_ARC_S21W_PCREL,
    BFD_RELOC_ARC_S21W_PCREL_PLT },

  { "plt", 0, { F_NULL }, BFD_RELOC_ARC_32_ME, BFD_RELOC_ARC_PLT32 }
};

const unsigned arc_num_equiv_tab = ARRAY_SIZE (arc_reloc_equiv);

const struct arc_pseudo_insn arc_pseudo_insns[] =
{
  { "push", "st", ".aw", 5, { { RC, 0, 0, 0 }, { BRAKET, 1, 0, 1 },
			      { RB, 1, 28, 2 }, { SIMM9_8, 1, -4, 3 },
			      { BRAKETdup, 1, 0, 4} } },
  { "pop", "ld", ".ab", 5, { { RA, 0, 0, 0 }, { BRAKET, 1, 0, 1 },
			     { RB, 1, 28, 2 }, { SIMM9_8, 1, 4, 3 },
			     { BRAKETdup, 1, 0, 4} } },

  { "brgt", "brlt", NULL, 3, { { RB, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brgt", "brge", NULL, 3, { { RB, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brgt", "brlt", NULL, 3, { { RB, 0, 0, 1 }, { LIMM, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brgt", "brlt", NULL, 3, { { LIMM, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brgt", "brge", NULL, 3, { { LIMM, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },

  { "brhi", "brlo", NULL, 3, { { RB, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brhi", "brhs", NULL, 3, { { RB, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brhi", "brlo", NULL, 3, { { RB, 0, 0, 1 }, { LIMM, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brhi", "brlo", NULL, 3, { { LIMM, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brhi", "brhs", NULL, 3, { { LIMM, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },

  { "brle", "brge", NULL, 3, { { RB, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brle", "brlt", NULL, 3, { { RB, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brle", "brge", NULL, 3, { { RB, 0, 0, 1 }, { LIMM, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brle", "brge", NULL, 3, { { LIMM, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brle", "brlt", NULL, 3, { { LIMM, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },

  { "brls", "brhs", NULL, 3, { { RB, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brls", "brlo", NULL, 3, { { RB, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brls", "brhs", NULL, 3, { { RB, 0, 0, 1 }, { LIMM, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brls", "brhs", NULL, 3, { { LIMM, 0, 0, 1 }, { RC, 0, 0, 0 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
  { "brls", "brlo", NULL, 3, { { LIMM, 0, 0, 0 }, { UIMM6_8, 0, 1, 1 },
			       { SIMM9_A16_8, 0, 0, 2 } } },
};

const unsigned arc_num_pseudo_insn =
  sizeof (arc_pseudo_insns) / sizeof (*arc_pseudo_insns);

const struct arc_aux_reg arc_aux_regs[] =
{
#undef DEF
#define DEF(ADDR, CPU, SUBCLASS, NAME)		\
  { ADDR, CPU, SUBCLASS, #NAME, sizeof (#NAME)-1 },

#include "arc-regs.h"

#undef DEF
};

const unsigned arc_num_aux_regs = ARRAY_SIZE (arc_aux_regs);

/* NOTE: The order of this array MUST be consistent with 'enum
   arc_rlx_types' located in tc-arc.h!  */
const struct arc_opcode arc_relax_opcodes[] =
{
  { NULL, 0x0, 0x0, 0x0, ARITH, NONE, { UNUSED }, { 0 } },

  /* bl_s s13 11111sssssssssss.  */
  { "bl_s", 0x0000F800, 0x0000F800, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, BRANCH, NONE,
    { SIMM13_A32_5_S }, { 0 }},

  /* bl<.d> s25 00001sssssssss10SSSSSSSSSSNRtttt.  */
  { "bl", 0x08020000, 0xF8030000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, BRANCH, NONE,
    { SIMM25_A32_5 }, { C_D }},

  /* b_s s10 1111000sssssssss.  */
  { "b_s", 0x0000F000, 0x0000FE00, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, BRANCH, NONE,
    { SIMM10_A16_7_S }, { 0 }},

  /* b<.d> s25 00000ssssssssss1SSSSSSSSSSNRtttt.  */
  { "b", 0x00010000, 0xF8010000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, BRANCH, NONE,
    { SIMM25_A16_5 }, { C_D }},

  /* add_s c,b,u3 01101bbbccc00uuu.  Wants UIMM3_13_S_PCREL.  */
  { "add_s", 0x00006800, 0x0000F818, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RC_S, RB_S, UIMM3_13_S }, { 0 }},

  /* add<.f> a,b,u6 00100bbb01000000FBBBuuuuuuAAAAAA.  Wants
     UIMM6_20_PCREL.  */
  { "add", 0x20400000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, UIMM6_20 }, { C_F }},

  /* add<.f> a,b,limm 00100bbb00000000FBBB111110AAAAAA.  */
  { "add", 0x20000F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, LIMM }, { C_F }},

  /* ld_s c,b,u7 10000bbbcccuuuuu.  Wants UIMM7_A32_11_S_PCREL.  */
  { "ld_s", 0x00008000, 0x0000F800, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RC_S, BRAKET, RB_S, UIMM7_A32_11_S, BRAKETdup }, { 0 }},

  /* ld<.di><.aa><.x><zz> a,b,s9
     00010bbbssssssssSBBBDaaZZXAAAAAA.  Wants SIMM9_8_PCREL.  */
  { "ld", 0x10000000, 0xF8000000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RA, BRAKET, RB, SIMM9_8, BRAKETdup },
    { C_ZZ23, C_DI20, C_AA21, C_X25 }},

  /* ld<.di><.aa><.x><zz> a,b,limm 00100bbbaa110ZZXDBBB111110AAAAAA.  */
  { "ld", 0x20300F80, 0xF8380FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RA, BRAKET, RB, LIMM, BRAKETdup },
    { C_ZZ13, C_DI16, C_AA8, C_X15 }},

  /* mov_s b,u8 11011bbbuuuuuuuu.  Wants UIMM8_8_S_PCREL.  */
  { "mov_s", 0x0000D800, 0x0000F800, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB_S, UIMM8_8_S }, { 0 }},

  /* mov<.f> b,s12 00100bbb10001010FBBBssssssSSSSSS.  Wants
     SIMM12_20_PCREL.  */
  { "mov", 0x208A0000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, SIMM12_20 }, { C_F }},

  /* mov<.f> b,limm 00100bbb00001010FBBB111110RRRRRR.  */
  { "mov", 0x200A0F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, LIMM }, { C_F }},

  /* sub_s c,b,u3 01101bbbccc01uuu.  UIMM3_13_S_PCREL.  */
  { "sub_s", 0x00006808, 0x0000F818, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RC_S, RB_S, UIMM3_13_S }, { 0 }},

  /* sub<.f> a,b,u6 00100bbb01000010FBBBuuuuuuAAAAAA.
     UIMM6_20_PCREL.  */
  { "sub", 0x20420000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, UIMM6_20 }, { C_F }},

  /* sub<.f> a,b,limm 00100bbb00000010FBBB111110AAAAAA.  */
  { "sub", 0x20020F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, LIMM }, { C_F }},

  /* mpy<.f> a,b,u6 00100bbb01011010FBBBuuuuuuAAAAAA.
     UIMM6_20_PCREL.  */
  { "mpy", 0x205A0000, 0xF8FF0000, ARC_OPCODE_ARC700 | ARC_OPCODE_ARCv2EM
    | ARC_OPCODE_ARCv2HS, ARITH, MPY6E, { RA, RB, UIMM6_20 }, { C_F }},

  /* mpy<.f> a,b,limm 00100bbb00011010FBBB111110AAAAAA.  */
  { "mpy", 0x201A0F80, 0xF8FF0FC0, ARC_OPCODE_ARC700 | ARC_OPCODE_ARCv2EM
    | ARC_OPCODE_ARCv2HS, ARITH, MPY6E, { RA, RB, LIMM }, { C_F }},

  /* mov<.f><.cc> b,u6 00100bbb11001010FBBBuuuuuu1QQQQQ.
     UIMM6_20_PCREL.  */
  { "mov", 0x20CA0020, 0xF8FF0020, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, UIMM6_20 }, { C_F, C_CC }},

  /* mov<.f><.cc> b,limm 00100bbb11001010FBBB1111100QQQQQ.  */
  { "mov", 0x20CA0F80, 0xF8FF0FE0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, LIMM }, { C_F, C_CC }},

  /* add<.f><.cc> b,b,u6 00100bbb11000000FBBBuuuuuu1QQQQQ.
     UIMM6_20_PCREL.  */
  { "add", 0x20C00020, 0xF8FF0020, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RB, RBdup, UIMM6_20 }, { C_F, C_CC }},

  /* add<.f><.cc> b,b,limm 00100bbb11000000FBBB1111100QQQQQ.  */
  { "add", 0x20C00F80, 0xF8FF0FE0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RB, RBdup, LIMM }, { C_F, C_CC }}
};

const unsigned arc_num_relax_opcodes = ARRAY_SIZE (arc_relax_opcodes);
