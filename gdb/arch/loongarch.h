/*
   Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef GDB_LOONGARCH_ARCH_H
#define GDB_LOONGARCH_ARCH_H

#include "elf/loongarch.h"
#include "opcode/loongarch.h"

extern unsigned int loongarch_debug;

struct target_desc;

extern const char *loongarch_expedite_regs[];

extern struct target_desc *loongarch_get_base_target_description (int rlen);

extern struct target_desc *
loongarch_create_target_description (int rlen, int fpu32, int fpu64, int lbt,
				     int lsx, int lasx);

#endif
