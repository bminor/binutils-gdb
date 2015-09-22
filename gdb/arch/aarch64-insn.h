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

#endif
