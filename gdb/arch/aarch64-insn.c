/* Copyright (C) 2009-2015 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

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

#include "common-defs.h"
#include "aarch64-insn.h"

/* Toggle this file's internal debugging dump.  */
int aarch64_debug = 0;

/* Extract a signed value from a bit field within an instruction
   encoding.

   INSN is the instruction opcode.

   WIDTH specifies the width of the bit field to extract (in bits).

   OFFSET specifies the least significant bit of the field where bits
   are numbered zero counting from least to most significant.  */

static int32_t
extract_signed_bitfield (uint32_t insn, unsigned width, unsigned offset)
{
  unsigned shift_l = sizeof (int32_t) * 8 - (offset + width);
  unsigned shift_r = sizeof (int32_t) * 8 - width;

  return ((int32_t) insn << shift_l) >> shift_r;
}

/* Determine if specified bits within an instruction opcode matches a
   specific pattern.

   INSN is the instruction opcode.

   MASK specifies the bits within the opcode that are to be tested
   agsinst for a match with PATTERN.  */

static int
decode_masked_match (uint32_t insn, uint32_t mask, uint32_t pattern)
{
  return (insn & mask) == pattern;
}

/* Decode an opcode if it represents an ADRP instruction.

   ADDR specifies the address of the opcode.
   INSN specifies the opcode to test.
   RD receives the 'rd' field from the decoded instruction.

   Return 1 if the opcodes matches and is decoded, otherwise 0.  */

int
aarch64_decode_adrp (CORE_ADDR addr, uint32_t insn, unsigned *rd)
{
  if (decode_masked_match (insn, 0x9f000000, 0x90000000))
    {
      *rd = (insn >> 0) & 0x1f;

      if (aarch64_debug)
	{
	  debug_printf ("decode: 0x%s 0x%x adrp x%u, #?\n",
			core_addr_to_string_nz (addr), insn, *rd);
	}
      return 1;
    }
  return 0;
}

/* Decode an opcode if it represents an branch immediate or branch
   and link immediate instruction.

   ADDR specifies the address of the opcode.
   INSN specifies the opcode to test.
   IS_BL receives the 'op' bit from the decoded instruction.
   OFFSET receives the immediate offset from the decoded instruction.

   Return 1 if the opcodes matches and is decoded, otherwise 0.  */

int
aarch64_decode_b (CORE_ADDR addr, uint32_t insn, int *is_bl,
		  int32_t *offset)
{
  /* b  0001 01ii iiii iiii iiii iiii iiii iiii */
  /* bl 1001 01ii iiii iiii iiii iiii iiii iiii */
  if (decode_masked_match (insn, 0x7c000000, 0x14000000))
    {
      *is_bl = (insn >> 31) & 0x1;
      *offset = extract_signed_bitfield (insn, 26, 0) << 2;

      if (aarch64_debug)
	{
	  debug_printf ("decode: 0x%s 0x%x %s 0x%s\n",
			core_addr_to_string_nz (addr), insn,
			*is_bl ? "bl" : "b",
			core_addr_to_string_nz (addr + *offset));
	}

      return 1;
    }
  return 0;
}

/* Decode an opcode if it represents a conditional branch instruction.

   ADDR specifies the address of the opcode.
   INSN specifies the opcode to test.
   COND receives the branch condition field from the decoded
   instruction.
   OFFSET receives the immediate offset from the decoded instruction.

   Return 1 if the opcodes matches and is decoded, otherwise 0.  */

int
aarch64_decode_bcond (CORE_ADDR addr, uint32_t insn, unsigned *cond,
		      int32_t *offset)
{
  /* b.cond  0101 0100 iiii iiii iiii iiii iii0 cccc */
  if (decode_masked_match (insn, 0xff000010, 0x54000000))
    {
      *cond = (insn >> 0) & 0xf;
      *offset = extract_signed_bitfield (insn, 19, 5) << 2;

      if (aarch64_debug)
	{
	  debug_printf ("decode: 0x%s 0x%x b<%u> 0x%s\n",
			core_addr_to_string_nz (addr), insn, *cond,
			core_addr_to_string_nz (addr + *offset));
	}
      return 1;
    }
  return 0;
}

/* Decode an opcode if it represents a CBZ or CBNZ instruction.

   ADDR specifies the address of the opcode.
   INSN specifies the opcode to test.
   IS64 receives the 'sf' field from the decoded instruction.
   IS_CBNZ receives the 'op' field from the decoded instruction.
   RN receives the 'rn' field from the decoded instruction.
   OFFSET receives the 'imm19' field from the decoded instruction.

   Return 1 if the opcodes matches and is decoded, otherwise 0.  */

int
aarch64_decode_cb (CORE_ADDR addr, uint32_t insn, int *is64, int *is_cbnz,
		   unsigned *rn, int32_t *offset)
{
  /* cbz  T011 010o iiii iiii iiii iiii iiir rrrr */
  /* cbnz T011 010o iiii iiii iiii iiii iiir rrrr */
  if (decode_masked_match (insn, 0x7e000000, 0x34000000))
    {
      *rn = (insn >> 0) & 0x1f;
      *is64 = (insn >> 31) & 0x1;
      *is_cbnz = (insn >> 24) & 0x1;
      *offset = extract_signed_bitfield (insn, 19, 5) << 2;

      if (aarch64_debug)
	{
	  debug_printf ("decode: 0x%s 0x%x %s 0x%s\n",
			core_addr_to_string_nz (addr), insn,
			*is_cbnz ? "cbnz" : "cbz",
			core_addr_to_string_nz (addr + *offset));
	}
      return 1;
    }
  return 0;
}

/* Decode an opcode if it represents a TBZ or TBNZ instruction.

   ADDR specifies the address of the opcode.
   INSN specifies the opcode to test.
   IS_TBNZ receives the 'op' field from the decoded instruction.
   BIT receives the bit position field from the decoded instruction.
   RT receives 'rt' field from the decoded instruction.
   IMM receives 'imm' field from the decoded instruction.

   Return 1 if the opcodes matches and is decoded, otherwise 0.  */

int
aarch64_decode_tb (CORE_ADDR addr, uint32_t insn, int *is_tbnz,
		   unsigned *bit, unsigned *rt, int32_t *imm)
{
  /* tbz  b011 0110 bbbb biii iiii iiii iiir rrrr */
  /* tbnz B011 0111 bbbb biii iiii iiii iiir rrrr */
  if (decode_masked_match (insn, 0x7e000000, 0x36000000))
    {
      *rt = (insn >> 0) & 0x1f;
      *is_tbnz = (insn >> 24) & 0x1;
      *bit = ((insn >> (31 - 4)) & 0x20) | ((insn >> 19) & 0x1f);
      *imm = extract_signed_bitfield (insn, 14, 5) << 2;

      if (aarch64_debug)
	{
	  debug_printf ("decode: 0x%s 0x%x %s x%u, #%u, 0x%s\n",
			core_addr_to_string_nz (addr), insn,
			*is_tbnz ? "tbnz" : "tbz", *rt, *bit,
			core_addr_to_string_nz (addr + *imm));
	}
      return 1;
    }
  return 0;
}
