/* Macro defintions for i386.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.

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

/*
 * Changes for 80386 by Pace Willisson (pace@prep.ai.mit.edu)
 * July 1988
 */

#define HOST_BYTE_ORDER LITTLE_ENDIAN

/* I'm running gdb 3.4 under 386/ix 2.0.2, which is a derivative of AT&T's
Sys V/386 3.2.

On some machines, gdb crashes when it's starting up while calling the
vendor's termio tgetent() routine.  It always works when run under
itself (actually, under 3.2, it's not an infinitely recursive bug.)
After some poking around, it appears that depending on the environment
size, or whether you're running YP, or the phase of the moon or something,
the stack is not always long-aligned when main() is called, and tgetent()
takes strong offense at that.  On some machines this bug never appears, but
on those where it does, it occurs quite reliably.  */
#define ALIGN_STACK_ON_STARTUP

/* define USG if you are using sys5 /usr/include's */
#define USG

/* USG systems need these */
#define vfork() fork()
#define MAXPATHLEN 500

#define HAVE_TERMIO

/* Get rid of any system-imposed stack limit if possible.  */

/* #define SET_STACK_LIMIT_HUGE not in sys5 */

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

#define KERNEL_U_ADDR 0xe0000000


#if 0
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
#endif /* 0 */
