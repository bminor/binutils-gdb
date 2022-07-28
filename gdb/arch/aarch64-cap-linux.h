/* Common target-dependent Linux functionality for AArch64.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef ARCH_AARCH64_CAP_LINUX_H
#define ARCH_AARCH64_CAP_LINUX_H

#include "gdbsupport/common-defs.h"
#include "gdbsupport/byte-vector.h"

/* The HWCAP values are not being used to identify Morello anymore, but are
   kept here for reference in case someone has a need for them.  */
/* Morello HWCAP bit V1, before Linux Kernel 5.18.  */
#define HWCAP2_MORELLO_V1 (1 << 19)
/* Morello HWCAP bit V2, after Linux Kernel 5.18.  */
#define HWCAP2_MORELLO_V2 (1UL << 31)

/* Size of the Capability register set.  */
#define AARCH64_LINUX_CREGS_SIZE ((39 * 16) + (2 * 8))

/* 39 128-bit C registers plus 2 64-bit registers.  */
#define AARCH64_MORELLO_REGS_NUM 41
#define AARCH64_C_REGS_NUM 31

/* Capability fault si_code numbers.  */
#define SEGV_CAPTAGERR	      10  /* Capability tag fault */
#define SEGV_CAPSEALEDERR     11  /* Capability sealed fault */
#define SEGV_CAPBOUNDSERR     12  /* Capability bounds fault */
#define SEGV_CAPPERMERR	      13  /* Capability permission fault */
#define SEGV_CAPSTORETAGERR   14  /* Capability tag store fault */

#define MORELLO_TAG_GRANULE_SIZE 16
#define MORELLO_TAG_BIT_SIZE 1

/* Header for NT_MEMTAG notes.  */
struct __attribute__ ((packed)) tag_dump_header
{
  uint16_t format;

  uint64_t start_vma;
  uint64_t end_vma;

  union
  {
    struct tag_dump_fmt
    {
      uint16_t granule_byte_size;
      uint16_t tag_bit_size;
      uint16_t __unused;
    } cheri;
  } u;

  // Other formats may be added here later.
};

#endif /* ARCH_AARCH64_CAP_LINUX_H */
