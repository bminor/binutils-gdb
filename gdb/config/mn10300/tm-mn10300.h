/* Parameters for execution on a Matsushita mn10300 processor.
   Copyright 1996, 1997, 1998, 1999, 2000, 2001
   Free Software Foundation, Inc.

   Contributed by Geoffrey Noer <noer@cygnus.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#define GDB_MULTI_ARCH 1

/* The mn10300 is little endian.  */
#define TARGET_BYTE_ORDER_DEFAULT LITTLE_ENDIAN

/* All registers are 32bits (phew!).  */
#if !GDB_MULTI_ARCH
#define REGISTER_SIZE 4
#define MAX_REGISTER_RAW_SIZE 4
#define NUM_REGS 32
#endif

#if !GDB_MULTI_ARCH
#define REGISTER_VIRTUAL_TYPE(REG) builtin_type_int
#endif

#if !GDB_MULTI_ARCH
#define REGISTER_BYTE(REG) ((REG) * 4)
#define REGISTER_VIRTUAL_SIZE(REG) 4
#define REGISTER_RAW_SIZE(REG) 4
#endif

#define D2_REGNUM 2
#define D3_REGNUM 3
#define A2_REGNUM 6
#define A3_REGNUM 7
#define SP_REGNUM 8
#define PC_REGNUM 9
#define MDR_REGNUM 10
#define PSW_REGNUM 11
#define LIR_REGNUM 12
#define LAR_REGNUM 13
#define E0_REGNUM 15

#define INIT_FRAME_PC		/* Not necessary */

extern void mn10300_pop_frame (struct frame_info *);
#define POP_FRAME mn10300_pop_frame (get_current_frame ())

/* Cons up virtual frame pointer for trace */
extern void mn10300_virtual_frame_pointer (CORE_ADDR, long *, long *);
#define TARGET_VIRTUAL_FRAME_POINTER(PC, REGP, OFFP) \
	mn10300_virtual_frame_pointer ((PC), (REGP), (OFFP))
