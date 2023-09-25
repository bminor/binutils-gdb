/* Opcode table for the ARC.
   Copyright (C) 1994-2023 Free Software Foundation, Inc.

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

/* ARC NPS400 Support: The ARC NPS400 core is an ARC700 with some
   custom instructions.  All NPS400 features are built into all ARC
   target builds as this reduces the chances that regressions might
   creep in.  */

/* Insert RA register into a 32-bit opcode, with checks.  */

static unsigned long long
insert_ra_chk (unsigned long long  insn,
	       long long value,
	       const char **errmsg)
{
  if (value == 60)
    *errmsg = _("LP_COUNT register cannot be used as destination register");

  return insn | (value & 0x3F);
}

/* Insert RB register into a 32-bit opcode.  */

static unsigned long long
insert_rb (unsigned long long  insn,
	   long long value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x07) << 24) | (((value >> 3) & 0x07) << 12);
}

/* Insert RB register into a push(d)l/pop(d)l instruction.  */

static unsigned long long
insert_rbb (unsigned long long  insn,
	    long long value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x07) << 8) | (((value >> 3) & 0x07) << 1);
}

/* Insert RB register with checks.  */

static unsigned long long
insert_rb_chk (unsigned long long  insn,
	       long long value,
	       const char **errmsg)
{
  if (value == 60)
    *errmsg = _("LP_COUNT register cannot be used as destination register");

  return insn | ((value & 0x07) << 24) | (((value >> 3) & 0x07) << 12);
}

/* Insert a floating point register into fs2 slot.  */

static unsigned long long
insert_fs2 (unsigned long long  insn,
	    long long value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x07) << 24) | (((value >> 3) & 0x03) << 12);
}

/* Insert address writeback mode for 128-bit loads.  */

static unsigned long long
insert_qq (unsigned long long  insn,
	   long long value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x01) << 11) | ((value & 0x02) << (6-1));
}

static long long
extract_rb (unsigned long long insn,
	    bool *invalid)
{
  int value = (((insn >> 12) & 0x07) << 3) | ((insn >> 24) & 0x07);

  if (value == 0x3e && invalid)
    *invalid = true; /* A limm operand, it should be extracted in a
			different way.  */

  return value;
}

static long long
extract_rbb (unsigned long long insn,
	    bool *invalid)
{
  int value = (((insn >> 1) & 0x07) << 3) | ((insn >> 8) & 0x07);

  if (value == 0x3e && invalid)
    *invalid = true; /* A limm operand, it should be extracted in a
			different way.  */

  return value;
}

/* Extract the floating point register number from fs2 slot.  */

static long long
extract_fs2 (unsigned long long insn,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  long long value;
  value = (((insn >> 12) & 0x03) << 3) | ((insn >> 24) & 0x07);
  return value;
}

/* Extract address writeback mode for 128-bit loads.  */

static long long
extract_qq (unsigned long long  insn,
	    bool *       invalid ATTRIBUTE_UNUSED)
{
  long long value;
  value = ((insn & 0x800) >> 11) | ((insn & 0x40) >> (6 - 1));
  return value;
}

static unsigned long long
insert_rad (unsigned long long insn,
	    long long value,
	    const char **errmsg)
{
  if (value & 0x01)
    *errmsg = _("cannot use odd number destination register");
  if (value == 60)
    *errmsg = _("LP_COUNT register cannot be used as destination register");

  return insn | (value & 0x3F);
}

static unsigned long long
insert_rcd (unsigned long long insn,
	    long long value,
	    const char **errmsg)
{
  if (value & 0x01)
    *errmsg = _("cannot use odd number source register");

  return insn | ((value & 0x3F) << 6);
}

static unsigned long long
insert_rbd (unsigned long long insn,
	    long long value,
	    const char **errmsg)
{
  if (value & 0x01)
    *errmsg = _("cannot use odd number source register");
  if (value == 60)
    *errmsg = _("LP_COUNT register cannot be used as destination register");

  return insn | ((value & 0x07) << 24) | (((value >> 3) & 0x07) << 12);
}

/* Dummy insert ZERO operand function.  */

static unsigned long long
insert_za (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value)
    *errmsg = _("operand is not zero");
  return insn;
}

/* Insert Y-bit in bbit/br instructions.  This function is called only
   when solving fixups.  */

static unsigned long long
insert_Ybit (unsigned long long insn,
	     long long value,
	     const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value > 0)
    insn |= 0x08;

  return insn;
}

/* Insert Y-bit in bbit/br instructions.  This function is called only
   when solving fixups.  */

static unsigned long long
insert_NYbit (unsigned long long insn,
	      long long value,
	      const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value < 0)
    insn |= 0x08;

  return insn;
}

/* Insert H register into a 16-bit opcode.  */

static unsigned long long
insert_rhv1 (unsigned long long insn,
	     long long value,
	     const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn |= ((value & 0x07) << 5) | ((value >> 3) & 0x07);
}

static long long
extract_rhv1 (unsigned long long insn,
	      bool *invalid ATTRIBUTE_UNUSED)
{
  int value = ((insn & 0x7) << 3) | ((insn >> 5) & 0x7);

  return value;
}

/* Insert H register into a 16-bit opcode.  */

static unsigned long long
insert_rhv2 (unsigned long long insn,
	     long long value,
	     const char **errmsg)
{
  if (value == 0x1E)
    *errmsg = _("register R30 is a limm indicator");
  else if (value < 0 || value > 31)
    *errmsg = _("register out of range");
  return insn |= ((value & 0x07) << 5) | ((value >> 3) & 0x03);
}

static long long
extract_rhv2 (unsigned long long insn,
	      bool *invalid ATTRIBUTE_UNUSED)
{
  int value = ((insn >> 5) & 0x07) | ((insn & 0x03) << 3);

  return value;
}

static unsigned long long
insert_r0 (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 0)
    *errmsg = _("register must be R0");
  return insn;
}

static long long
extract_r0 (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  return 0;
}


static unsigned long long
insert_r1 (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 1)
    *errmsg = _("register must be R1");
  return insn;
}

static long long
extract_r1 (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool* invalid ATTRIBUTE_UNUSED)
{
  return 1;
}

static unsigned long long
insert_r2 (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 2)
    *errmsg = _("register must be R2");
  return insn;
}

static long long
extract_r2 (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  return 2;
}

static unsigned long long
insert_r3 (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 3)
    *errmsg = _("register must be R3");
  return insn;
}

static long long
extract_r3 (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  return 3;
}

static unsigned long long
insert_sp (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 28)
    *errmsg = _("register must be SP");
  return insn;
}

static long long
extract_sp (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  return 28;
}

static unsigned long long
insert_gp (unsigned long long insn,
	   long long value,
	   const char **errmsg)
{
  if (value != 26
      && value != 30)
    *errmsg = _("register must be GP");
  return insn;
}

static long long
extract_gp (unsigned long long insn ATTRIBUTE_UNUSED,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  return 26;
}

static unsigned long long
insert_pcl (unsigned long long insn,
	    long long value,
	    const char **errmsg)
{
  if (value != 63)
    *errmsg = _("register must be PCL");
  return insn;
}

static long long
extract_pcl (unsigned long long insn ATTRIBUTE_UNUSED,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  return 63;
}

static unsigned long long
insert_blink (unsigned long long insn,
	      long long value,
	      const char **errmsg)
{
  if (value != 31)
    *errmsg = _("register must be BLINK");
  return insn;
}

static long long
extract_blink (unsigned long long insn ATTRIBUTE_UNUSED,
	       bool *invalid ATTRIBUTE_UNUSED)
{
  return 31;
}

static unsigned long long
insert_ilink1 (unsigned long long insn,
	       long long value,
	       const char **errmsg)
{
  if (value != 29)
    *errmsg = _("register must be ILINK1");
  return insn;
}

static long long
extract_ilink1 (unsigned long long insn ATTRIBUTE_UNUSED,
		bool *invalid ATTRIBUTE_UNUSED)
{
  return 29;
}

static unsigned long long
insert_ilink2 (unsigned long long insn,
	       long long value,
	       const char **errmsg)
{
  if (value != 30)
    *errmsg = _("register must be ILINK2");
  return insn;
}

static long long
extract_ilink2 (unsigned long long insn ATTRIBUTE_UNUSED,
		bool *invalid ATTRIBUTE_UNUSED)
{
  return 30;
}

static unsigned long long
insert_ras (unsigned long long insn,
	    long long value,
	    const char **errmsg)
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
      *errmsg = _("register must be either r0-r3 or r12-r15");
      break;
    }
  return insn;
}

static long long
extract_ras (unsigned long long insn,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  int value = insn & 0x07;

  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned long long
insert_rbs (unsigned long long insn,
	    long long value,
	    const char **errmsg)
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
      *errmsg = _("register must be either r0-r3 or r12-r15");
      break;
    }
  return insn;
}

static long long
extract_rbs (unsigned long long insn,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 8) & 0x07;

  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned long long
insert_rcs (unsigned long long insn,
	    long long value,
	    const char **errmsg)
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
      *errmsg = _("register must be either r0-r3 or r12-r15");
      break;
    }
  return insn;
}

static long long
extract_rcs (unsigned long long insn,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 5) & 0x07;

  if (value > 3)
    return (value + 8);
  else
    return value;
}

static unsigned long long
insert_simm3s (unsigned long long insn,
	       long long value,
	       const char **errmsg)
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
      *errmsg = _("accepted values are from -1 to 6");
      break;
    }

  insn |= tmp << 8;
  return insn;
}

static long long
extract_simm3s (unsigned long long insn,
		bool *invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 8) & 0x07;

  if (value == 7)
    return -1;
  else
    return value;
}

static unsigned long long
insert_rrange (unsigned long long insn,
	       long long value,
	       const char **errmsg)
{
  int reg1 = (value >> 16) & 0xFFFF;
  int reg2 = value & 0xFFFF;

  if (reg1 != 13)
    *errmsg = _("first register of the range should be r13");
  else if (reg2 < 13 || reg2 > 26)
    *errmsg = _("last register of the range doesn't fit");
  else
    insn |= ((reg2 - 12) & 0x0F) << 1;
  return insn;
}

static long long
extract_rrange (unsigned long long insn,
		bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn >> 1) & 0x0F;
}

static unsigned long long
insert_r13el (unsigned long long insn,
	      long long int value,
	      const char **errmsg)
{
  if (value != 13)
    {
      *errmsg = _("invalid register number, should be fp");
      return insn;
    }

  insn |= 0x02;
  return insn;
}

static unsigned long long
insert_fpel (unsigned long long insn,
	     long long value,
	     const char **errmsg)
{
  if (value != 27)
    {
      *errmsg = _("invalid register number, should be fp");
      return insn;
    }

  insn |= 0x0100;
  return insn;
}

static long long
extract_fpel (unsigned long long insn,
	      bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn & 0x0100) ? 27 : -1;
}

static unsigned long long
insert_blinkel (unsigned long long insn,
		long long value,
		const char **errmsg)
{
  if (value != 31)
    {
      *errmsg = _("invalid register number, should be blink");
      return insn;
    }

  insn |= 0x0200;
  return insn;
}

static long long
extract_blinkel (unsigned long long insn,
		 bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn & 0x0200) ? 31 : -1;
}

static unsigned long long
insert_pclel (unsigned long long insn,
	      long long value,
	      const char **errmsg)
{
  if (value != 63)
    {
      *errmsg = _("invalid register number, should be pcl");
      return insn;
    }

  insn |= 0x0400;
  return insn;
}

static long long
extract_pclel (unsigned long long insn,
	       bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn & 0x0400) ? 63 : -1;
}

#define INSERT_W6

/* mask = 00000000000000000000111111000000
   insn = 00011bbb000000000BBBwwwwwwDaaZZ1.  */

static unsigned long long
insert_w6 (unsigned long long insn,
	   long long value,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 0) & 0x003f) << 6;

  return insn;
}

#define EXTRACT_W6

/* mask = 00000000000000000000111111000000.  */

static long long
extract_w6 (unsigned long long insn,
	    bool *invalid ATTRIBUTE_UNUSED)
{
  int value = 0;

  value |= ((insn >> 6) & 0x003f) << 0;

  /* Extend the sign.  */
  int signbit = 1 << 5;
  value = (value ^ signbit) - signbit;

  return value;
}

#define INSERT_G_S

/* mask = 0000011100022000
   insn = 01000ggghhhGG0HH.  */

static unsigned long long
insert_g_s (unsigned long long insn,
	    long long value,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 0) & 0x0007) << 8;
  insn |= ((value >> 3) & 0x0003) << 3;

  return insn;
}

#define EXTRACT_G_S

/* mask = 0000011100022000.  */

static long long
extract_g_s (unsigned long long insn,
	     bool *invalid ATTRIBUTE_UNUSED)
{
  int value = 0;
  int signbit = 1 << (6 - 1);

  value |= ((insn >> 8) & 0x0007) << 0;
  value |= ((insn >> 3) & 0x0003) << 3;

  /* Extend the sign.  */
  value = (value ^ signbit) - signbit;

  return value;
}

/* ARC NPS400 Support: See comment near head of file.  */
#define MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS(NAME,OFFSET)          \
static unsigned long long					 \
 insert_nps_3bit_reg_at_##OFFSET##_##NAME			 \
 (unsigned long long insn,					 \
  long long value,						 \
  const char **errmsg)						 \
{								 \
  switch (value)						 \
    {								 \
    case 0:							 \
    case 1:							 \
    case 2:							 \
    case 3:							 \
      insn |= value << (OFFSET);				 \
      break;							 \
    case 12:							 \
    case 13:							 \
    case 14:							 \
    case 15:							 \
      insn |= (value - 8) << (OFFSET);				 \
      break;							 \
    default:							 \
      *errmsg = _("register must be either r0-r3 or r12-r15");   \
      break;							 \
    }								 \
  return insn;							 \
}								 \
								 \
static long long						 \
extract_nps_3bit_reg_at_##OFFSET##_##NAME			 \
  (unsigned long long insn,					 \
   bool *invalid ATTRIBUTE_UNUSED)				 \
{								 \
  int value = (insn >> (OFFSET)) & 0x07;			 \
  if (value > 3)						 \
    value += 8;							 \
  return value;							 \
}								 \

MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (dst,8)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (dst,24)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (dst,40)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (dst,56)

MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (src2,5)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (src2,21)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (src2,37)
MAKE_3BIT_REG_INSERT_EXTRACT_FUNCS (src2,53)

static unsigned long long
insert_nps_bitop_size_2b (unsigned long long insn,
			  long long value,
			  const char **errmsg)
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
      *errmsg = _("invalid size, should be 1, 2, 4, or 8");
      break;
    }

  insn |= value << 10;
  return insn;
}

static long long
extract_nps_bitop_size_2b (unsigned long long insn,
			   bool *invalid ATTRIBUTE_UNUSED)
{
  return  1 << ((insn >> 10) & 0x3);
}

static unsigned long long
insert_nps_bitop_uimm8 (unsigned long long insn,
			long long value,
			const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value >> 5) & 7) << 12;
  insn |= (value & 0x1f);
  return insn;
}

static long long
extract_nps_bitop_uimm8 (unsigned long long insn,
			 bool *invalid ATTRIBUTE_UNUSED)
{
  return (((insn >> 12) & 0x7) << 5) | (insn & 0x1f);
}

static unsigned long long
insert_nps_rflt_uimm6 (unsigned long long insn,
		       long long value,
		       const char **errmsg)
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

static long long
extract_nps_rflt_uimm6 (unsigned long long insn,
			bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn >> 6) & 0x3f;
}

static unsigned long long
insert_nps_dst_pos_and_size (unsigned long long insn,
			     long long value,
			     const char **errmsg ATTRIBUTE_UNUSED)
{
  insn |= ((value & 0x1f) | (((32 - value - 1) & 0x1f) << 10));
  return insn;
}

static long long
extract_nps_dst_pos_and_size (unsigned long long insn,
			      bool *invalid ATTRIBUTE_UNUSED)
{
  return (insn & 0x1f);
}

static unsigned long long
insert_nps_cmem_uimm16 (unsigned long long insn,
			long long value,
			const char **errmsg)
{
  int top = (value >> 16) & 0xffff;

  if (top != 0x0 && top != NPS_CMEM_HIGH_VALUE)
    *errmsg = _("invalid value for CMEM ld/st immediate");
  insn |= (value & 0xffff);
  return insn;
}

static long long
extract_nps_cmem_uimm16 (unsigned long long insn,
			 bool *invalid ATTRIBUTE_UNUSED)
{
  return (NPS_CMEM_HIGH_VALUE << 16) | (insn & 0xffff);
}

static unsigned long long
insert_nps_imm_offset (unsigned long long insn,
		       long long value,
		       const char **errmsg)
{
  switch (value)
    {
    case 0:
    case 16:
    case 32:
    case 48:
    case 64:
      value = value >> 4;
      break;
    default:
      *errmsg = _("invalid position, should be 0, 16, 32, 48 or 64.");
      value = 0;
    }
  insn |= (value << 10);
  return insn;
}

static long long
extract_nps_imm_offset (unsigned long long insn,
			bool *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 10) & 0x7) * 16;
}

static unsigned long long
insert_nps_imm_entry (unsigned long long insn,
		      long long value,
		      const char **errmsg)
{
  switch (value)
    {
    case 16:
      value = 0;
      break;
    case 32:
      value = 1;
      break;
    case 64:
      value = 2;
      break;
    case 128:
    value = 3;
    break;
    default:
      *errmsg = _("invalid position, should be 16, 32, 64 or 128.");
      value = 0;
    }
  insn |= (value << 2);
  return insn;
}

static long long
extract_nps_imm_entry (unsigned long long insn,
		       bool *invalid ATTRIBUTE_UNUSED)
{
  int imm_entry = ((insn >> 2) & 0x7);
  return (1 << (imm_entry + 4));
}

static unsigned long long
insert_nps_size_16bit (unsigned long long insn,
		       long long value,
		       const char **errmsg)
{
  if ((value < 1) || (value > 64))
    {
      *errmsg = _("invalid size value must be on range 1-64.");
      value = 0;
    }
  value = value & 0x3f;
  insn |= (value << 6);
  return insn;
}

static long long
extract_nps_size_16bit (unsigned long long insn,
			bool *invalid ATTRIBUTE_UNUSED)
{
  return ((insn & 0xfc0) >> 6) ? ((insn & 0xfc0) >> 6) : 64;
}


#define MAKE_SRC_POS_INSERT_EXTRACT_FUNCS(NAME,SHIFT)		       \
 static unsigned long long					       \
 insert_nps_##NAME##_pos (unsigned long long insn,		       \
			 long long value,			       \
			 const char **errmsg)			       \
{								       \
  switch (value)						       \
    {								       \
    case 0:							       \
    case 8:							       \
    case 16:							       \
    case 24:							       \
      value = value / 8;					       \
      break;							       \
    default:							       \
     *errmsg = _("invalid position, should be 0, 8, 16, or 24");       \
     value = 0;							       \
    }								       \
  insn |= (value << SHIFT);					       \
  return insn;							       \
}								       \
								       \
 static long long						       \
 extract_nps_##NAME##_pos (unsigned long long insn,		       \
			   bool *invalid ATTRIBUTE_UNUSED)	       \
 {								       \
   return ((insn >> SHIFT) & 0x3) * 8;				       \
 }

MAKE_SRC_POS_INSERT_EXTRACT_FUNCS (src2, 12)
MAKE_SRC_POS_INSERT_EXTRACT_FUNCS (src1, 10)

#define MAKE_BIAS_INSERT_EXTRACT_FUNCS(NAME,LOWER,UPPER,BITS,BIAS,SHIFT) \
static unsigned long long						\
insert_nps_##NAME (unsigned long long insn,				\
		   long long value,					\
		   const char **errmsg)					\
{									\
  if (value < LOWER || value > UPPER)					\
    {									\
      *errmsg = _("invalid size, value must be "			\
		  #LOWER " to " #UPPER ".");				\
      return insn;							\
    }									\
  value -= BIAS;							\
  insn |= (value << SHIFT);						\
  return insn;								\
}									\
									\
static long long							\
extract_nps_##NAME (unsigned long long insn,				\
		    bool *invalid ATTRIBUTE_UNUSED)			\
{									\
  return ((insn >> SHIFT) & ((1 << BITS) - 1)) + BIAS;			\
}

MAKE_BIAS_INSERT_EXTRACT_FUNCS (addb_size,2,32,5,1,5)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (andb_size,1,32,5,1,5)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (fxorb_size,8,32,5,8,5)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (wxorb_size,16,32,5,16,5)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (bitop_size,1,32,5,1,10)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (qcmp_size,1,8,3,1,9)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (bitop1_size,1,32,5,1,20)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (bitop2_size,1,32,5,1,25)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (hash_width,1,32,5,1,6)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (hash_len,1,8,3,1,2)
MAKE_BIAS_INSERT_EXTRACT_FUNCS (index3,4,7,2,4,0)

static long long
extract_nps_qcmp_m3 (unsigned long long insn,
		     bool *invalid)
{
  int m3 = (insn >> 5) & 0xf;
  if (m3 == 0xf)
    *invalid = true;
  return m3;
}

static long long
extract_nps_qcmp_m2 (unsigned long long insn,
		     bool *invalid)
{
  bool tmp_invalid = false;
  int m2 = (insn >> 15) & 0x1;
  int m3 = extract_nps_qcmp_m3 (insn, &tmp_invalid);

  if (m2 == 0 && m3 == 0xf)
    *invalid = true;
  return m2;
}

static long long
extract_nps_qcmp_m1 (unsigned long long insn,
		     bool *invalid)
{
  bool tmp_invalid = false;
  int m1 = (insn >> 14) & 0x1;
  int m2 = extract_nps_qcmp_m2 (insn, &tmp_invalid);
  int m3 = extract_nps_qcmp_m3 (insn, &tmp_invalid);

  if (m1 == 0 && m2 == 0 && m3 == 0xf)
    *invalid = true;
  return m1;
}

static unsigned long long
insert_nps_calc_entry_size (unsigned long long insn,
			    long long value,
			    const char **errmsg)
{
  unsigned pwr;

  if (value < 1 || value > 256)
    {
      *errmsg = _("value out of range 1 - 256");
      return 0;
    }

  for (pwr = 0; (value & 1) == 0; value >>= 1)
    ++pwr;

  if (value != 1)
    {
      *errmsg = _("value must be power of 2");
      return 0;
    }

  return insn | (pwr << 8);
}

static long long
extract_nps_calc_entry_size (unsigned long long insn,
			     bool *invalid ATTRIBUTE_UNUSED)
{
  unsigned entry_size = (insn >> 8) & 0xf;
  return 1 << entry_size;
}

static unsigned long long
insert_nps_bitop_mod4 (unsigned long long insn,
		       long long value,
		       const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x2) << 30) | ((value & 0x1) << 47);
}

static long long
extract_nps_bitop_mod4 (unsigned long long insn,
			bool *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 30) & 0x2) | ((insn >> 47) & 0x1);
}

static unsigned long long
insert_nps_bitop_dst_pos3_pos4 (unsigned long long insn,
				long long value,
				const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (value << 42) | (value << 37);
}

static long long
extract_nps_bitop_dst_pos3_pos4 (unsigned long long insn,
				 bool *invalid)
{
  if (((insn >> 42) & 0x1f) != ((insn >> 37) & 0x1f))
    *invalid = true;
  return ((insn >> 37) & 0x1f);
}

static unsigned long long
insert_nps_bitop_ins_ext (unsigned long long insn,
			  long long value,
			  const char **errmsg)
{
  if (value < 0 || value > 28)
    *errmsg = _("value must be in the range 0 to 28");
  return insn | (value << 20);
}

static long long
extract_nps_bitop_ins_ext (unsigned long long insn,
			   bool *invalid)
{
  int value = (insn >> 20) & 0x1f;

  if (value > 28)
    *invalid = true;
  return value;
}

#define MAKE_1BASED_INSERT_EXTRACT_FUNCS(NAME,SHIFT,UPPER,BITS)		\
static unsigned long long						\
insert_nps_##NAME (unsigned long long insn,				\
		   long long value,					\
		   const char **errmsg)					\
{									\
  if (value < 1 || value > UPPER)					\
    *errmsg = _("value must be in the range 1 to " #UPPER);		\
  if (value == UPPER)							\
    value = 0;								\
  return insn | (value << SHIFT);					\
}									\
									\
static long long							\
extract_nps_##NAME (unsigned long long insn,				\
		    bool *invalid ATTRIBUTE_UNUSED)			\
{									\
  int value = (insn >> SHIFT) & ((1 << BITS) - 1);			\
  if (value == 0)							\
    value = UPPER;							\
  return value;								\
}

MAKE_1BASED_INSERT_EXTRACT_FUNCS (field_size, 6, 8, 3)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (shift_factor, 9, 8, 3)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (bits_to_scramble, 12, 8, 3)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (bdlen_max_len, 5, 256, 8)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (bd_num_buff, 6, 8, 3)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (pmu_num_job, 6, 4, 2)
MAKE_1BASED_INSERT_EXTRACT_FUNCS (proto_size, 16, 64, 6)

static unsigned long long
insert_nps_min_hofs (unsigned long long insn,
		     long long value,
		     const char **errmsg)
{
  if (value < 0 || value > 240)
    *errmsg = _("value must be in the range 0 to 240");
  if ((value % 16) != 0)
    *errmsg = _("value must be a multiple of 16");
  value = value / 16;
  return insn | (value << 6);
}

static long long
extract_nps_min_hofs (unsigned long long insn,
		      bool *invalid ATTRIBUTE_UNUSED)
{
  int value = (insn >> 6) & 0xF;
  return value * 16;
}

#define MAKE_INSERT_NPS_ADDRTYPE(NAME, VALUE)			       \
  static unsigned long long					       \
insert_nps_##NAME (unsigned long long insn,			       \
		   long long value,				       \
		   const char **errmsg)				       \
{								       \
  if (value != ARC_NPS400_ADDRTYPE_##VALUE)			       \
    *errmsg = _("invalid address type for operand");		       \
  return insn;							       \
}								       \
								       \
static long long						       \
extract_nps_##NAME (unsigned long long insn ATTRIBUTE_UNUSED,	       \
		    bool *invalid ATTRIBUTE_UNUSED)		       \
{								       \
  return ARC_NPS400_ADDRTYPE_##VALUE;				       \
}

MAKE_INSERT_NPS_ADDRTYPE (bd, BD)
MAKE_INSERT_NPS_ADDRTYPE (jid, JID)
MAKE_INSERT_NPS_ADDRTYPE (lbd, LBD)
MAKE_INSERT_NPS_ADDRTYPE (mbd, MBD)
MAKE_INSERT_NPS_ADDRTYPE (sd, SD)
MAKE_INSERT_NPS_ADDRTYPE (sm, SM)
MAKE_INSERT_NPS_ADDRTYPE (xa, XA)
MAKE_INSERT_NPS_ADDRTYPE (xd, XD)
MAKE_INSERT_NPS_ADDRTYPE (cd, CD)
MAKE_INSERT_NPS_ADDRTYPE (cbd, CBD)
MAKE_INSERT_NPS_ADDRTYPE (cjid, CJID)
MAKE_INSERT_NPS_ADDRTYPE (clbd, CLBD)
MAKE_INSERT_NPS_ADDRTYPE (cm, CM)
MAKE_INSERT_NPS_ADDRTYPE (csd, CSD)
MAKE_INSERT_NPS_ADDRTYPE (cxa, CXA)
MAKE_INSERT_NPS_ADDRTYPE (cxd, CXD)

static unsigned long long
insert_nps_rbdouble_64 (unsigned long long insn,
			long long value,
			const char **errmsg)
{
  if (value < 0 || value > 31)
    *errmsg = _("value must be in the range 0 to 31");
  return insn | (value << 43) | (value << 48);
}


static long long
extract_nps_rbdouble_64 (unsigned long long insn,
			 bool *invalid)
{
  int value1 = (insn >> 43) & 0x1F;
  int value2 = (insn >> 48) & 0x1F;

  if (value1 != value2)
    *invalid = true;

  return value1;
}

static unsigned long long
insert_nps_misc_imm_offset (unsigned long long insn,
			    long long value,
			    const char **errmsg)
{
  if (value & 0x3)
    {
      *errmsg = _("invalid position, should be one of: 0,4,8,...124.");
      value = 0;
    }
  insn |= (value << 6);
  return insn;
}

static long long int
extract_nps_misc_imm_offset (unsigned long long insn,
			     bool *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 8) & 0x1f) * 4;
}

static long long int
extract_uimm12_20 (unsigned long long insn ATTRIBUTE_UNUSED,
		   bool *invalid ATTRIBUTE_UNUSED)
{
  int value = 0;

  value |= ((insn >> 6) & 0x003f) << 0;
  value |= ((insn >> 0) & 0x003f) << 6;

  return value;
}

/* Include the generic extract/insert functions.  Order is important
   as some of the functions present in the .h may be disabled via
   defines.  */
#include "arc-fxi.h"

/* The flag operands enum. */
#define FLAG(NAME, NAMESTR, CODE, BITS, SHIFT, FAVAIL) \
		F_##NAME,
enum arc_flag_operand_enum {
	F_INVALID = -1,
	F_NULL = 0,
#include "arc-flag.def"
	F_SIZE,
};
#undef FLAG

/* The flag operands name. */
#define FLAG(NAME, NAMESTR, CODE, BITS, SHIFT, FAVAIL) \
		"F_" #NAME,
const char *flag_operand_name[F_SIZE] = {
	"F_NULL",
#include "arc-flag.def"
};
#undef FLAG

/* The flag operands table.

   The format of the table is
   NAME CODE BITS SHIFT FAVAIL.  */
#define FLAG(NAME, NAMESTR, CODE, BITS, SHIFT, FAVAIL) \
  [F_##NAME] = { NAMESTR, CODE, BITS, SHIFT, FAVAIL },
const struct arc_flag_operand arc_flag_operands[F_SIZE] =
{
  [F_NULL] = { 0, 0, 0, 0, 0 },
  #include "arc-flag.def"
};
#undef FLAG

const unsigned arc_num_flag_operands = ARRAY_SIZE (arc_flag_operands);

/* Enum of the flag classes. */
#define FLAG_CLASS(NAME, CLASS, INSERT_FN, EXTRACT_FN, ...) \
	  C_##NAME,
enum arc_flag_class_enum {
  C_INVALID = -1,
  #include "arc-flag-classes.def"
  C_SIZE
};
#undef FLAG_CLASS

/* Table of the flag classes.

   The format of the table is
   CLASS {FLAG_CODE}.  */
#define FLAG_CLASS(NAME, CLASS, INSERT_FN, EXTRACT_FN, ...) \
  [C_##NAME] = { \
    .flag_class = CLASS, \
    .insert = INSERT_FN, \
    .extract = EXTRACT_FN, \
    .flags = { __VA_ARGS__, F_NULL } \
  },
const struct arc_flag_class arc_flag_classes[C_SIZE] =
{
  #include "arc-flag-classes.def"
};
#undef FLAG_CLASS

const unsigned char flags_none[] = { 0 };
const unsigned char flags_f[]    = { C_F };
const unsigned char flags_cc[]   = { C_CC };
const unsigned char flags_ccf[]  = { C_CC, C_F };

/* The operands enum. */
#define ARC_OPERAND(NAME, BITS, SHIFT, RELO, FLAGS, INSERT_FN, EXTRACT_FN) \
  NAME,
enum arc_operand_enum {
  ARC_OPERAND_INVALID = -1,
  UNUSED = 0,
#include "arc-operands.def"
  ARC_OPERAND_SIZE
};
#undef ARC_OPERAND

/* The operands name. */
#define ARC_OPERAND(NAME, BITS, SHIFT, RELO, FLAGS, INSERT_FN, EXTRACT_FN) \
  #NAME,
const char *arc_operand_name[ARC_OPERAND_SIZE] = {
  "UNUSED",
#include "arc-operands.def"
};
#undef ARC_OPERAND
/* The operands table.

   The format of the operands table is:

   BITS SHIFT DEFAULT_RELOC FLAGS INSERT_FUN EXTRACT_FUN.  */
#define ARC_OPERAND(NAME, BITS, SHIFT, RELOC, FLAGS, INSERT_FN, EXTRACT_FN) \
    [NAME] = { \
      .bits = BITS, \
      .shift = SHIFT, \
      .default_reloc = RELOC, \
      .flags = FLAGS, \
      .insert = INSERT_FN, \
      .extract = EXTRACT_FN \
    },
const struct arc_operand arc_operands[ARC_OPERAND_SIZE] =
{
  /* The fields are bits, shift, insert, extract, flags.  The zero
     index is used to indicate end-of-list.  */
  [UNUSED] = { 0, 0, 0, 0, 0, 0 },
#include "arc-operands.def"
};
#undef ARC_OPERAND

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

const unsigned char arg_32bit_rc[]   = { RC };
const unsigned char arg_32bit_u6[]   = { UIMM6_20 };
const unsigned char arg_32bit_limm[] = { LIMM };

/* List with special cases instructions and the applicable flags.  */
const struct arc_flag_special arc_flag_special_cases[] =
{
  { "b", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	  F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	  F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	  F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ, F_NJ, F_NM,
	  F_NO_T, F_NULL } },
  { "bl", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	   F_NULL } },
  { "br", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	   F_NULL } },
  { "j", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	  F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	  F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	  F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	  F_NULL } },
  { "jl", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	   F_NULL } },
  { "lp", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	   F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	   F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	   F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	   F_NULL } },
  { "set", { F_ALWAYS, F_RA, F_EQUAL, F_ZERO, F_NOTEQUAL, F_NOTZERO, F_POZITIVE,
	    F_PL, F_NEGATIVE, F_MINUS, F_CARRY, F_CARRYSET, F_LOWER, F_CARRYCLR,
	    F_NOTCARRY, F_HIGHER, F_OVERFLOWSET, F_OVERFLOW, F_NOTOVERFLOW,
	    F_OVERFLOWCLR, F_GT, F_GE, F_LT, F_LE, F_HI, F_LS, F_PNZ,
	    F_NULL } },
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

/* ARC64 pseudo instructions.  */
const struct arc_pseudo_insn arc64_pseudo_insns[] =
{
  { "pushl", "stl", ".aw", 5, { { RC, 0, 0, 0 }, { BRAKET, 1, 0, 1 },
				{ RB, 1, 28, 2 }, { SIMM9_8, 1, -8, 3 },
				{ BRAKETdup, 1, 0, 4} } },
  { "popl", "ldl", ".ab", 5, { { RA, 0, 0, 0 }, { BRAKET, 1, 0, 1 },
			       { RB, 1, 28, 2 }, { SIMM9_8, 1, 8, 3 },
			       { BRAKETdup, 1, 0, 4} } },
};

const unsigned arc64_num_pseudo_insn =
  sizeof (arc64_pseudo_insns) / sizeof (*arc64_pseudo_insns);

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

  /* add_s c,b,u3 01101bbbccc00uuu.  */
  { "add_s", 0x00006800, 0x0000F818, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RC_S, RB_S, UIMM3_13R_S }, { 0 }},

  /* add<.f> a,b,u6 00100bbb01000000FBBBuuuuuuAAAAAA.  */
  { "add", 0x20400000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, UIMM6_20R }, { C_F }},

  /* add<.f> a,b,limm 00100bbb00000000FBBB111110AAAAAA.  */
  { "add", 0x20000F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, LIMM }, { C_F }},

  /* ld_s c,b,u7 10000bbbcccuuuuu.  */
  { "ld_s", 0x00008000, 0x0000F800, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RC_S, BRAKET, RB_S, UIMM7_A32_11R_S, BRAKETdup }, { 0 }},

  /* ld<.di><.aa><.x><zz> a,b,s9
     00010bbbssssssssSBBBDaaZZXAAAAAA.  */
  { "ld", 0x10000000, 0xF8000000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RA, BRAKET, RB, SIMM9_8R, BRAKETdup },
    { C_ZZ23, C_DI20, C_AA21, C_X25 }},

  /* ld<.di><.aa><.x><zz> a,b,limm 00100bbbaa110ZZXDBBB111110AAAAAA.  */
  { "ld", 0x20300F80, 0xF8380FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RA, BRAKET, RB, LIMM, BRAKETdup },
    { C_ZZ13, C_DI16, C_AA8, C_X15 }},

  /* mov_s b,u8 11011bbbuuuuuuuu.  */
  { "mov_s", 0x0000D800, 0x0000F800, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB_S, UIMM8_8R_S }, { 0 }},

  /* mov<.f> b,s12 00100bbb10001010FBBBssssssSSSSSS.  */
  { "mov", 0x208A0000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, SIMM12_20R }, { C_F }},

  /* mov<.f> b,limm 00100bbb00001010FBBB111110RRRRRR.  */
  { "mov", 0x200A0F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, LIMM }, { C_F }},

  /* sub_s c,b,u3 01101bbbccc01uuu.  */
  { "sub_s", 0x00006808, 0x0000F818, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RC_S, RB_S, UIMM3_13R_S }, { 0 }},

  /* sub<.f> a,b,u6 00100bbb01000010FBBBuuuuuuAAAAAA.  */
  { "sub", 0x20420000, 0xF8FF0000, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, UIMM6_20R }, { C_F }},

  /* sub<.f> a,b,limm 00100bbb00000010FBBB111110AAAAAA.  */
  { "sub", 0x20020F80, 0xF8FF0FC0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RA, RB, LIMM }, { C_F }},

  /* mpy<.f> a,b,u6 00100bbb01011010FBBBuuuuuuAAAAAA.  */
  { "mpy", 0x205A0000, 0xF8FF0000, ARC_OPCODE_ARC700 | ARC_OPCODE_ARCv2EM
    | ARC_OPCODE_ARCv2HS, ARITH, MPY6E, { RA, RB, UIMM6_20R }, { C_F }},

  /* mpy<.f> a,b,limm 00100bbb00011010FBBB111110AAAAAA.  */
  { "mpy", 0x201A0F80, 0xF8FF0FC0, ARC_OPCODE_ARC700 | ARC_OPCODE_ARCv2EM
    | ARC_OPCODE_ARCv2HS, ARITH, MPY6E, { RA, RB, LIMM }, { C_F }},

  /* mov<.f><.cc> b,u6 00100bbb11001010FBBBuuuuuu1QQQQQ.  */
  { "mov", 0x20CA0020, 0xF8FF0020, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, UIMM6_20R }, { C_F, C_CC }},

  /* mov<.f><.cc> b,limm 00100bbb11001010FBBB1111100QQQQQ.  */
  { "mov", 0x20CA0F80, 0xF8FF0FE0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, MEMORY, NONE,
    { RB, LIMM }, { C_F, C_CC }},

  /* add<.f><.cc> b,b,u6 00100bbb11000000FBBBuuuuuu1QQQQQ.  */
  { "add", 0x20C00020, 0xF8FF0020, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RB, RBdup, UIMM6_20R }, { C_F, C_CC }},

  /* add<.f><.cc> b,b,limm 00100bbb11000000FBBB1111100QQQQQ.  */
  { "add", 0x20C00F80, 0xF8FF0FE0, ARC_OPCODE_ARC600 | ARC_OPCODE_ARC700
    | ARC_OPCODE_ARCv2EM | ARC_OPCODE_ARCv2HS, ARITH, NONE,
    { RB, RBdup, LIMM }, { C_F, C_CC }}
};

const unsigned arc_num_relax_opcodes = ARRAY_SIZE (arc_relax_opcodes);

/* Return length of an opcode in bytes.  */

int
arc_opcode_len (const struct arc_opcode *opcode)
{
  if (opcode->mask < 0x10000ull)
    return 2;

  if (opcode->mask < 0x100000000ull)
    return 4;

  if (opcode->mask < 0x1000000000000ull)
    return 6;

  return 8;
}
