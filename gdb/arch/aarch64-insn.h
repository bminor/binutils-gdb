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

#ifndef AARCH64_INSN_H
#define AARCH64_INSN_H 1

extern int aarch64_debug;

int aarch64_decode_adr (CORE_ADDR addr, uint32_t insn, int *is_adrp,
			unsigned *rd, int32_t *offset);

int aarch64_decode_b (CORE_ADDR addr, uint32_t insn, int *is_bl,
		      int32_t *offset);

int aarch64_decode_bcond (CORE_ADDR addr, uint32_t insn, unsigned *cond,
			  int32_t *offset);

int aarch64_decode_cb (CORE_ADDR addr, uint32_t insn, int *is64,
		       int *is_cbnz, unsigned *rn, int32_t *offset);

int aarch64_decode_tb (CORE_ADDR addr, uint32_t insn, int *is_tbnz,
		       unsigned *bit, unsigned *rt, int32_t *imm);

int aarch64_decode_ldr_literal (CORE_ADDR addr, uint32_t insn, int *is_w,
				int *is64, unsigned *rt, int32_t *offset);

/* Data passed to each method of aarch64_insn_visitor.  */

struct aarch64_insn_data
{
  /* The instruction address.  */
  CORE_ADDR insn_addr;
};

/* Visit different instructions by different methods.  */

struct aarch64_insn_visitor
{
  /* Visit instruction B/BL OFFSET.  */
  void (*b) (const int is_bl, const int32_t offset,
	     struct aarch64_insn_data *data);

  /* Visit instruction B.COND OFFSET.  */
  void (*b_cond) (const unsigned cond, const int32_t offset,
		  struct aarch64_insn_data *data);

  /* Visit instruction CBZ/CBNZ Rn, OFFSET.  */
  void (*cb) (const int32_t offset, const int is_cbnz,
	      const unsigned rn, int is64,
	      struct aarch64_insn_data *data);

  /* Visit instruction TBZ/TBNZ Rt, #BIT, OFFSET.  */
  void (*tb) (const int32_t offset, int is_tbnz,
	      const unsigned rt, unsigned bit,
	      struct aarch64_insn_data *data);

  /* Visit instruction ADR/ADRP Rd, OFFSET.  */
  void (*adr) (const int32_t offset, const unsigned rd,
	       const int is_adrp, struct aarch64_insn_data *data);

  /* Visit instruction LDR/LDRSW Rt, OFFSET.  */
  void (*ldr_literal) (const int32_t offset, const int is_sw,
		       const unsigned rt, const int is64,
		       struct aarch64_insn_data *data);

  /* Visit instruction INSN of other kinds.  */
  void (*others) (const uint32_t insn, struct aarch64_insn_data *data);
};

void aarch64_relocate_instruction (uint32_t insn,
				   const struct aarch64_insn_visitor *visitor,
				   struct aarch64_insn_data *data);

#endif
