/* Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef NAT_AARCH64_CAP_LINUX_H
#define NAT_AARCH64_CAP_LINUX_H

/* If we don't find PTRACE_POKECAP and PTRACE_PEEKCAP, we need to define
   our own structures for reading capability registers and capabilities, as we
   may be building GDB without the Morello kernel headers.  */
#if !defined(PTRACE_POKECAP) && !defined(PTRACE_PEEKCAP)
/* Struct defining the layout of the capability register set.  */
struct user_morello_state {
	/* General capability registers.  */
	unsigned __int128 cregs[31];
	/* Capability program counter.  */
	unsigned __int128 pcc;
	/* Capability stack pointer.  */
	unsigned __int128 csp;
	/* Default data capability.  */
	unsigned __int128 ddc;
	unsigned __int128 ctpidr;
	unsigned __int128 rcsp;
	unsigned __int128 rddc;
	unsigned __int128 rctpidr;
	/* Compartment ID register.  */
	unsigned __int128 cid;
	/* Bitmap storing the tags of all the capability registers.
	   The tag for register <reg> is stored at bit index
	   MORELLO_PT_TAG_MAP_REG_BIT(<reg>) in tag_map.  */
       uint64_t	tag_map;
	/* Capability control register.  */
       uint64_t	cctlr;
};

/* Struct used to read/write capabilities using ptrace.  */
struct user_cap {
  unsigned __int128 val;
  uint8_t tag;
  uint8_t __reserved[15];
};

/* Morello-specific requests.  */
#define PTRACE_PEEKCAP 12
#define PTRACE_POKECAP 13
#endif

/* From thread TID, read a capability from memory at ADDRESS and store
   it into CAP.

   Return true if successful and false otherwise.  */

extern bool aarch64_linux_read_capability (int tid, CORE_ADDR address,
					   user_cap &cap);

/* For thread TID, write capability CAP to the memory address ADDRESS.

   Return true if successful and false otherwise.  */

extern bool aarch64_linux_write_capability (int tid, CORE_ADDR address,
					    const user_cap &cap);

/* Return true if the target supports Morello features (NT_ARM_MORELLO register
   set, PTRACE_PEEKCAP and PTRACE_POKECAP).  If it does, it means we are
   dealing with a Morello Linux Kernel.  This is needed because we can't
   rely on the HWCAP2_MORELLO value anymore, given it has changed.

   Return false otherwise.  */

extern bool aarch64_supports_morello (int tid);

#endif /* NAT_AARCH64_CAP_LINUX_H */
