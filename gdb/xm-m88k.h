/* Copyright (C) 1986, 1987, 1988, 1989, 1990 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This is currently for a 88000 running DGUX.  If other 88k ports are
   done, OS-specific stuff should be moved (see tm-68k.h, for example).  */
/* g++ support is not yet included.  */

#define HOST_BYTE_ORDER BIG_ENDIAN

#if !defined (USG)
#define USG 1
#endif
/* DGUX has bcopy(), etc.  */
#define USG_UTILS 0

#include <sys/param.h>

#define vfork() fork()
#define index strchr
#define rindex strrchr
#define getwd(BUF) getcwd(BUF,MAXPATHLEN);
#define bzero(ptr,count) (memset((ptr),0,(count)))
#define bcopy(src,dst,count) (memcpy((dst),(src),(count)))
#define bcmp(left,right,count) (memcmp((right),(left),(count)))
#ifdef __GNUC__
#define memcpy __builtin_memcpy
/* gcc doesn't have this, at least not gcc 1.92.  */
/* #define memset __builtin_memset */
#define strcmp __builtin_strcmp
#endif

#ifdef DGUX
#define x_foff _x_x._x_offset
#define x_fname _x_name
#define USER ptrace_user
#define _BSD_WAIT_FLAVOR
#endif

#define HAVE_TERMIO


#define USIZE 2048
#define NBPG NBPC
#define UPAGES USIZE

#define HAVE_GETPAGESIZE

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* number of traps that happen between exec'ing the shell
 * to run an inferior, and when we finally get to
 * the inferior code.  This is 2 on most implementations.
 */
#define START_INFERIOR_TRAPS_EXPECTED 2

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

/* Since registers r0 through r31 are stored directly in the struct ptrace_user,
   (for m88k BCS)
   the ptrace_user offsets are sufficient and KERNEL_U_ADDRESS can be 0 */

#define KERNEL_U_ADDR 0

#define REGISTER_U_ADDR(addr, blockend, regno) \
        (addr) = m88k_register_u_addr ((blockend),(regno));

#define HAVE_WAIT_STRUCT

#define FETCH_INFERIOR_REGISTERS

/* Interface definitions for kernel debugger KDB.  */

/* Map machine fault codes into signal numbers.
   First subtract 0, divide by 4, then index in a table.
   Faults for which the entry in this table is 0
   are not handled by KDB; the program's own trap handler
   gets to handle then.  */

#define FAULT_CODE_ORIGIN 0
#define FAULT_CODE_UNITS 4
#define FAULT_TABLE    \
{ 0, 0, 0, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0}

/* Start running with a stack stretching from BEG to END.
   BEG and END should be symbols meaningful to the assembler.
   This is used only for kdb.  */

#define INIT_STACK(beg, end)  {}

/* Push the frame pointer register on the stack.  */
#define PUSH_FRAME_PTR        {}

/* Copy the top-of-stack to the frame pointer register.  */
#define POP_FRAME_PTR  {}

/* After KDB is entered by a fault, push all registers
   that GDB thinks about (all NUM_REGS of them),
   so that they appear in order of ascending GDB register number.
   The fault code will be on the stack beyond the last register.  */

#define PUSH_REGISTERS        {}

/* Assuming the registers (including processor status) have been
   pushed on the stack in order of ascending GDB register number,
   restore them and return to the address in the saved PC register.  */

#define POP_REGISTERS      {}
