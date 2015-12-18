/* Copyright (C) 1995-2015 Free Software Foundation, Inc.

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

/* Correct in either endianness.  */
#define arm_abi_breakpoint 0xef9f0001UL

/* For new EABI binaries.  We recognize it regardless of which ABI
   is used for gdbserver, so single threaded debugging should work
   OK, but for multi-threaded debugging we only insert the current
   ABI's breakpoint instruction.  For now at least.  */
#define arm_eabi_breakpoint 0xe7f001f0UL

#ifndef __ARM_EABI__
static const unsigned long arm_breakpoint = arm_abi_breakpoint;
#else
static const unsigned long arm_breakpoint = arm_eabi_breakpoint;
#endif

#define arm_breakpoint_len 4
static const unsigned short thumb_breakpoint = 0xde01;
#define thumb_breakpoint_len 2
static const unsigned short thumb2_breakpoint[] = { 0xf7f0, 0xa000 };
#define thumb2_breakpoint_len 4

extern struct regs_info regs_info_aarch32;

void arm_fill_gregset (struct regcache *regcache, void *buf);
void arm_store_gregset (struct regcache *regcache, const void *buf);
void arm_fill_vfpregset_num (struct regcache *regcache, void *buf, int num);
void arm_store_vfpregset_num (struct regcache *regcache, const void *buf,
			      int num);

int arm_breakpoint_kind_from_pc (CORE_ADDR *pcptr);
const gdb_byte *arm_sw_breakpoint_from_kind (int kind , int *size);
int arm_breakpoint_kind_from_current_state (CORE_ADDR *pcptr);
int arm_breakpoint_at (CORE_ADDR where);

void initialize_low_arch_aarch32 (void);

void init_registers_arm_with_neon (void);
int arm_is_thumb_mode (void);

extern const struct target_desc *tdesc_arm_with_neon;
