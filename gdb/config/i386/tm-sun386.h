/* OBSOLETE /* Parameters for a Sun 386i target machine, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1986, 1987, 1991, 1992, 1993, 1994, 1995, 1999, 2000 */
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
/* OBSOLETE #if !defined (TM_SUN386_H) */
/* OBSOLETE #define TM_SUN386_H 1 */
/* OBSOLETE  */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE #include "i386/tm-i386.h" */
/* OBSOLETE  */
/* OBSOLETE #ifndef sun386 */
/* OBSOLETE #define sun386 */
/* OBSOLETE #endif */
/* OBSOLETE #define SUNOS4 */
/* OBSOLETE #define USE_MACHINE_REG_H */
/* OBSOLETE  */
/* OBSOLETE /* Perhaps some day this will work even without the following #define */ */
/* OBSOLETE #define COFF_ENCAPSULATE */
/* OBSOLETE  */
/* OBSOLETE #ifdef COFF_ENCAPSULATE */
/* OBSOLETE /* Avoid conflicts between our include files and <sys/exec.h> */
/* OBSOLETE    (maybe not needed anymore).  */ */
/* OBSOLETE #define _EXEC_ */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* sun386 ptrace seems unable to change the frame pointer */ */
/* OBSOLETE #define PTRACE_FP_BUG */
/* OBSOLETE  */
/* OBSOLETE /* Address of end of stack space.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STACK_END_ADDR 0xfc000000 */
/* OBSOLETE  */
/* OBSOLETE /* Number of machine registers */ */
/* OBSOLETE  */
/* OBSOLETE #undef  NUM_REGS */
/* OBSOLETE #define NUM_REGS 35 */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers.  There should be NUM_REGS */
/* OBSOLETE    strings in this initializer.  The order of the first 8 registers must match */
/* OBSOLETE    the compiler's numbering scheme (which is the same as the 386 scheme) also, */
/* OBSOLETE    this table must match regmap in i386-pinsn.c. */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_NAMES */
/* OBSOLETE #define REGISTER_NAMES { "gs", "fs", "es", "ds",		\ */
/* OBSOLETE 			 "edi", "esi", "ebp", "esp",		\ */
/* OBSOLETE 			 "ebx", "edx", "ecx", "eax",		\ */
/* OBSOLETE 			 "retaddr", "trapnum", "errcode", "ip",	\ */
/* OBSOLETE 			 "cs", "ps", "sp", "ss",		\ */
/* OBSOLETE 			 "fst0", "fst1", "fst2", "fst3",	\ */
/* OBSOLETE 			 "fst4", "fst5", "fst6", "fst7",	\ */
/* OBSOLETE 			 "fctrl", "fstat", "ftag", "fip",	\ */
/* OBSOLETE 			 "fcs", "fopoff", "fopsel"		\ */
/* OBSOLETE 			 } */
/* OBSOLETE  */
/* OBSOLETE /* Register numbers of various important registers. */
/* OBSOLETE    Note that some of these values are "real" register numbers, */
/* OBSOLETE    and correspond to the general registers of the machine, */
/* OBSOLETE    and some are "phony" register numbers which are too large */
/* OBSOLETE    to be actual register numbers as far as the user is concerned */
/* OBSOLETE    but do serve to get the desired values when passed to read_register.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  FP_REGNUM */
/* OBSOLETE #define FP_REGNUM   6		/* (ebp) Contains address of executing stack frame */ */
/* OBSOLETE #undef  SP_REGNUM */
/* OBSOLETE #define SP_REGNUM  18		/* (usp) Contains address of top of stack */ */
/* OBSOLETE #undef  PS_REGNUM */
/* OBSOLETE #define PS_REGNUM  17		/* (ps)  Contains processor status */ */
/* OBSOLETE #undef  PC_REGNUM */
/* OBSOLETE #define PC_REGNUM  15		/* (eip) Contains program counter */ */
/* OBSOLETE #undef  FP0_REGNUM */
/* OBSOLETE #define FP0_REGNUM 20		/* Floating point register 0 */ */
/* OBSOLETE #undef  FPC_REGNUM */
/* OBSOLETE #define FPC_REGNUM 28		/* 80387 control register */ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_BYTES */
/* OBSOLETE #define REGISTER_BYTES (20*4+8*10+7*4) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_BYTE */
/* OBSOLETE #define REGISTER_BYTE(N) \ */
/* OBSOLETE  ((N) >= FPC_REGNUM ? (((N) - FPC_REGNUM) * 4) + 160	\ */
/* OBSOLETE   : (N) >= FP0_REGNUM ? (((N) - FP0_REGNUM) * 10) + 80	\ */
/* OBSOLETE   : (N) * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_RAW_SIZE */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) (((unsigned)((N) - FP0_REGNUM)) < 8 ? 10 : 4) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the program's representation */
/* OBSOLETE    for register N. */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_VIRTUAL_SIZE */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) (((unsigned)((N) - FP0_REGNUM)) < 8 ? 8 : 4) */
/* OBSOLETE  */
/* OBSOLETE /* Nonzero if register N requires conversion */
/* OBSOLETE    from raw format to virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_CONVERTIBLE */
/* OBSOLETE #define REGISTER_CONVERTIBLE(N) (((unsigned)((N) - FP0_REGNUM)) < 8) */
/* OBSOLETE  */
/* OBSOLETE /* Convert data from raw format for register REGNUM in buffer FROM */
/* OBSOLETE    to virtual format with type TYPE in buffer TO.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_CONVERT_TO_VIRTUAL */
/* OBSOLETE #define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \ */
/* OBSOLETE { \ */
/* OBSOLETE   double val; \ */
/* OBSOLETE   i387_to_double ((FROM), (char *)&val); \ */
/* OBSOLETE   store_floating ((TO), TYPE_LENGTH (TYPE), val); \ */
/* OBSOLETE } */
/* OBSOLETE extern void i387_to_double (char *, char *); */
/* OBSOLETE  */
/* OBSOLETE /* Convert data from virtual format with type TYPE in buffer FROM */
/* OBSOLETE    to raw format for register REGNUM in buffer TO.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_CONVERT_TO_RAW */
/* OBSOLETE #define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \ */
/* OBSOLETE { \ */
/* OBSOLETE   double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \ */
/* OBSOLETE   double_to_i387((char *)&val, (TO)); \ */
/* OBSOLETE } */
/* OBSOLETE extern void double_to_i387 (char *, char *); */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  REGISTER_VIRTUAL_TYPE */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \ */
/* OBSOLETE  (((unsigned)((N) - FP0_REGNUM)) < 8 ? builtin_type_double : builtin_type_int) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  EXTRACT_RETURN_VALUE */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE   memcpy (VALBUF, REGBUF + REGISTER_BYTE (TYPE_CODE (TYPE) == TYPE_CODE_FLT ? FP0_REGNUM : 11), TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  STORE_RETURN_VALUE */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE   write_register_bytes (REGISTER_BYTE (TYPE_CODE (TYPE) == TYPE_CODE_FLT ? FP0_REGNUM : 11), VALBUF, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame */
/* OBSOLETE    (its caller).  */ */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a frame's nominal address */
/* OBSOLETE    and produces the frame's chain-pointer. */ */
/* OBSOLETE  */
/* OBSOLETE #undef  FRAME_CHAIN */
/* OBSOLETE #define FRAME_CHAIN(thisframe) \ */
/* OBSOLETE   (!inside_entry_file ((thisframe)->pc) ? \ */
/* OBSOLETE    read_memory_integer ((thisframe)->frame, 4) :\ */
/* OBSOLETE    0) */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame.  */ */
/* OBSOLETE  */
/* OBSOLETE /* A macro that tells us whether the function invocation represented */
/* OBSOLETE    by FI does not have a frame on the stack associated with it.  If it */
/* OBSOLETE    does not, FRAMELESS is set to 1, else 0.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  FRAMELESS_FUNCTION_INVOCATION */
/* OBSOLETE #define FRAMELESS_FUNCTION_INVOCATION(FI) \ */
/* OBSOLETE      (frameless_look_for_prologue (FI)) */
/* OBSOLETE  */
/* OBSOLETE #undef  FRAME_SAVED_PC */
/* OBSOLETE #define FRAME_SAVED_PC(FRAME) (read_memory_integer ((FRAME)->frame + 4, 4)) */
/* OBSOLETE  */
/* OBSOLETE /* Insert the specified number of args and function address */
/* OBSOLETE    into a call sequence of the above form stored at DUMMYNAME.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef  FIX_CALL_DUMMY */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   \ */
/* OBSOLETE { \ */
/* OBSOLETE 	*(int *)((char *)(dummyname) + 1) = (int)(fun) - (pc) - 5; \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #endif /* !defined (TM_SUN386_H) */ */
