/* OBSOLETE /* Parameters for execution on a Sony/NEWS, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1987, 1989, 1992, 1993, 1994, 1996 */
/* OBSOLETE    Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #define HOST_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE #include <sys/param.h>		/* For INT_MIN */ */
/* OBSOLETE  */
/* OBSOLETE #define HAVE_WAIT_STRUCT */
/* OBSOLETE  */
/* OBSOLETE /* We can't use "isatty" or "fileno" on this machine.  This isn't good, */
/* OBSOLETE    but it will have to do.  */ */
/* OBSOLETE #define ISATTY(FP)	((FP) == stdin || (FP) == stdout) */
/* OBSOLETE  */
/* OBSOLETE /* THis is the amount to subtract from u.u_ar0 */
/* OBSOLETE    to get the offset in the core file of the register values. */ */
/* OBSOLETE  */
/* OBSOLETE #define KERNEL_U_ADDR UADDR */
/* OBSOLETE  */
/* OBSOLETE /* The offsets in this macro are from /usr/include/machine/reg.h */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno)		\ */
/* OBSOLETE {   static char offsets[] = { \ */
/* OBSOLETE 	/*d0-d7:*/1,2,3,4,5,6,7,8, \ */
/* OBSOLETE 	/*a0-a6:*/9,10,11,12,13,14,15, /*sp:*/-4, /*ps:*/0, /*pc:*/-1, \ */
/* OBSOLETE 	/*fp0-fp7:*/19,22,25,28,31,34,37,40, /*fpc:*/16,17,18 }; \ */
/* OBSOLETE 	addr = blockend + 4 * offsets[regno]; \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* NewsOS 3.3 does not define errno in <errno.h>.  */ */
/* OBSOLETE extern int errno; */
/* OBSOLETE  */
/* OBSOLETE /* Interface definitions for kernel debugger KDB.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Use GNU assembler instead of standard assembler */ */
/* OBSOLETE #define USE_GAS */
/* OBSOLETE  */
/* OBSOLETE /* Motorola assembly format */ */
/* OBSOLETE #ifndef USE_GAS */
/* OBSOLETE #define MOTOROLA */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Map machine fault codes into signal numbers. */
/* OBSOLETE    First subtract 0, divide by 4, then index in a table. */
/* OBSOLETE    Faults for which the entry in this table is 0 */
/* OBSOLETE    are not handled by KDB; the program's own trap handler */
/* OBSOLETE    gets to handle then.  */ */
/* OBSOLETE  */
/* OBSOLETE #define FAULT_CODE_ORIGIN 0 */
/* OBSOLETE #define FAULT_CODE_UNITS 4 */
/* OBSOLETE #define FAULT_TABLE    \ */
/* OBSOLETE { 0, 0, 0, 0, SIGTRAP, 0, 0, 0, \ */
/* OBSOLETE   0, SIGTRAP, 0, 0, 0, 0, 0, SIGKILL, \ */
/* OBSOLETE   0, 0, 0, 0, 0, 0, 0, 0, \ */
/* OBSOLETE   SIGILL } */
/* OBSOLETE  */
/* OBSOLETE /* Start running with a stack stretching from BEG to END. */
/* OBSOLETE    BEG and END should be symbols meaningful to the assembler. */
/* OBSOLETE    This is used only for kdb.  */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef MOTOROLA */
/* OBSOLETE #define INIT_STACK(beg, end)  \ */
/* OBSOLETE { asm (".globl end");         \ */
/* OBSOLETE   asm ("move.l $ end, sp");      \ */
/* OBSOLETE   asm ("clr.l fp"); } */
/* OBSOLETE #else */
/* OBSOLETE #define INIT_STACK(beg, end)  \ */
/* OBSOLETE { asm (".globl end");         \ */
/* OBSOLETE   asm ("movel $ end, sp");      \ */
/* OBSOLETE   asm ("clrl fp"); } */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Push the frame pointer register on the stack.  */ */
/* OBSOLETE #ifdef MOTOROLA */
/* OBSOLETE #define PUSH_FRAME_PTR        \ */
/* OBSOLETE   asm ("move.l fp, -(sp)"); */
/* OBSOLETE #else */
/* OBSOLETE #define PUSH_FRAME_PTR        \ */
/* OBSOLETE   asm ("movel fp, -(sp)"); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Copy the top-of-stack to the frame pointer register.  */ */
/* OBSOLETE #ifdef MOTOROLA */
/* OBSOLETE #define POP_FRAME_PTR  \ */
/* OBSOLETE   asm ("move.l (sp), fp"); */
/* OBSOLETE #else */
/* OBSOLETE #define POP_FRAME_PTR  \ */
/* OBSOLETE   asm ("movl (sp), fp"); */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* After KDB is entered by a fault, push all registers */
/* OBSOLETE    that GDB thinks about (all NUM_REGS of them), */
/* OBSOLETE    so that they appear in order of ascending GDB register number. */
/* OBSOLETE    The fault code will be on the stack beyond the last register.  */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef MOTOROLA */
/* OBSOLETE #define PUSH_REGISTERS        \ */
/* OBSOLETE { asm ("clr.w -(sp)");	      \ */
/* OBSOLETE   asm ("pea (10,sp)");	      \ */
/* OBSOLETE   asm ("movem $ 0xfffe,-(sp)"); } */
/* OBSOLETE #else */
/* OBSOLETE #define PUSH_REGISTERS        \ */
/* OBSOLETE { asm ("clrw -(sp)");	      \ */
/* OBSOLETE   asm ("pea 10(sp)");	      \ */
/* OBSOLETE   asm ("movem $ 0xfffe,-(sp)"); } */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* Assuming the registers (including processor status) have been */
/* OBSOLETE    pushed on the stack in order of ascending GDB register number, */
/* OBSOLETE    restore them and return to the address in the saved PC register.  */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef MOTOROLA */
/* OBSOLETE #define POP_REGISTERS          \ */
/* OBSOLETE { asm ("subi.l $8,28(sp)");     \ */
/* OBSOLETE   asm ("movem (sp),$ 0xffff"); \ */
/* OBSOLETE   asm ("rte"); } */
/* OBSOLETE #else */
/* OBSOLETE #define POP_REGISTERS          \ */
/* OBSOLETE { asm ("subil $8,28(sp)");     \ */
/* OBSOLETE   asm ("movem (sp),$ 0xffff"); \ */
/* OBSOLETE   asm ("rte"); } */
/* OBSOLETE #endif */
