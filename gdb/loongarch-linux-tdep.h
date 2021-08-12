/* GNU/Linux on LoongArch target support, prototypes.

   Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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

#ifndef LOONGARCH_LINUX_TDEP_H
#define LOONGARCH_LINUX_TDEP_H

#include <regset.h>

#define ELF_NGREG   45
#define ELF_NFPREG  34

typedef uint64_t loongarch_elf_gregset_t[ELF_NGREG];
extern const struct regset loongarch_elf_gregset;

typedef uint64_t loongarch_elf_fpregset_t[ELF_NFPREG];
extern const struct regset loongarch_elf_fpregset;

/* Regset variable size.  */
extern const struct regset loongarch_elf_cpucfg;

/* 4 SCRs + 4-byte EFLAG + 1-byte x86_top.  */
typedef uint64_t loongarch_elf_lbtregset_t[5];
extern const struct regset loongarch_elf_lbtregset;

typedef uint64_t loongarch_elf_lsxregset_t[32 * 2];
extern const struct regset loongarch_elf_lsxregset;

typedef uint64_t loongarch_elf_lasxregset_t[32 * 4];
extern const struct regset loongarch_elf_lasxregset;

#endif
