// OBSOLETE /* Target machine definitions for GDB on a Sequent Symmetry under dynix 3.0,
// OBSOLETE    with Weitek 1167 and i387 support.
// OBSOLETE 
// OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 2003 Free
// OBSOLETE    Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    Symmetry version by Jay Vosburgh (fubar@sequent.com).
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #ifndef TM_SYMMETRY_H
// OBSOLETE #define TM_SYMMETRY_H 1
// OBSOLETE 
// OBSOLETE #include "regcache.h"
// OBSOLETE #include "doublest.h"
// OBSOLETE 
// OBSOLETE /* I don't know if this will work for cross-debugging, even if you do get
// OBSOLETE    a copy of the right include file.  */
// OBSOLETE #include <machine/reg.h>
// OBSOLETE 
// OBSOLETE #include "i386/tm-i386.h"
// OBSOLETE 
// OBSOLETE /* Amount PC must be decremented by after a breakpoint.  This is often the
// OBSOLETE    number of bytes in BREAKPOINT but not always (such as now). */
// OBSOLETE 
// OBSOLETE #undef DECR_PC_AFTER_BREAK
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* Number of machine registers */
// OBSOLETE 
// OBSOLETE #undef NUM_REGS
// OBSOLETE #define NUM_REGS 49
// OBSOLETE 
// OBSOLETE /* Initializer for an array of names of registers.
// OBSOLETE    There should be NUM_REGS strings in this initializer.  */
// OBSOLETE 
// OBSOLETE /* Initializer for an array of names of registers.  There should be at least
// OBSOLETE    NUM_REGS strings in this initializer.  Any excess ones are simply ignored.
// OBSOLETE    Symmetry registers are in this weird order to match the register numbers
// OBSOLETE    in the symbol table entries.  If you change the order, things will probably
// OBSOLETE    break mysteriously for no apparent reason.  Also note that the st(0)...
// OBSOLETE    st(7) 387 registers are represented as st0...st7.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_NAME
// OBSOLETE #define REGISTER_NAMES {     "eax",  "edx",  "ecx",   "st0",  "st1", \
// OBSOLETE 			     "ebx",  "esi",  "edi",   "st2",  "st3", \
// OBSOLETE 			     "st4",  "st5",  "st6",   "st7",  "esp", \
// OBSOLETE 			     "ebp",  "eip",  "eflags","fp1",  "fp2", \
// OBSOLETE 			     "fp3",  "fp4",  "fp5",   "fp6",  "fp7", \
// OBSOLETE 			     "fp8",  "fp9",  "fp10",  "fp11", "fp12", \
// OBSOLETE 			     "fp13", "fp14", "fp15",  "fp16", "fp17", \
// OBSOLETE 			     "fp18", "fp19", "fp20",  "fp21", "fp22", \
// OBSOLETE 			     "fp23", "fp24", "fp25",  "fp26", "fp27", \
// OBSOLETE 			     "fp28", "fp29", "fp30",  "fp31" }
// OBSOLETE 
// OBSOLETE /* Register numbers of various important registers.
// OBSOLETE    Note that some of these values are "real" register numbers,
// OBSOLETE    and correspond to the general registers of the machine,
// OBSOLETE    and some are "phony" register numbers which are too large
// OBSOLETE    to be actual register numbers as far as the user is concerned
// OBSOLETE    but do serve to get the desired values when passed to read_register.  */
// OBSOLETE 
// OBSOLETE #define EAX_REGNUM	0
// OBSOLETE #define EDX_REGNUM	1
// OBSOLETE #define ECX_REGNUM	2
// OBSOLETE #define ST0_REGNUM	3
// OBSOLETE #define ST1_REGNUM	4
// OBSOLETE #define EBX_REGNUM	5
// OBSOLETE #define ESI_REGNUM	6
// OBSOLETE #define EDI_REGNUM	7
// OBSOLETE #define ST2_REGNUM	8
// OBSOLETE #define ST3_REGNUM	9
// OBSOLETE 
// OBSOLETE #define ST4_REGNUM	10
// OBSOLETE #define ST5_REGNUM	11
// OBSOLETE #define ST6_REGNUM	12
// OBSOLETE #define ST7_REGNUM	13
// OBSOLETE 
// OBSOLETE #define FP1_REGNUM 18		/* first 1167 register */
// OBSOLETE /* Get %fp2 - %fp31 by addition, since they are contiguous */
// OBSOLETE 
// OBSOLETE #undef  SP_REGNUM
// OBSOLETE #define SP_REGNUM 14		/* (usp) Contains address of top of stack */
// OBSOLETE #define ESP_REGNUM 14
// OBSOLETE #undef  FP_REGNUM
// OBSOLETE #define FP_REGNUM 15		/* (ebp) Contains address of executing stack frame */
// OBSOLETE #define EBP_REGNUM 15
// OBSOLETE #undef  PC_REGNUM
// OBSOLETE #define PC_REGNUM 16		/* (eip) Contains program counter */
// OBSOLETE #define EIP_REGNUM 16
// OBSOLETE #undef  PS_REGNUM
// OBSOLETE #define PS_REGNUM 17		/* (ps)  Contains processor status */
// OBSOLETE #define EFLAGS_REGNUM 17
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Following macro translates i386 opcode register numbers to Symmetry
// OBSOLETE  * register numbers.  This is used by i386_frame_find_saved_regs.
// OBSOLETE  *
// OBSOLETE  *           %eax  %ecx  %edx  %ebx  %esp  %ebp  %esi  %edi
// OBSOLETE  * i386        0     1     2     3     4     5     6     7
// OBSOLETE  * Symmetry    0     2     1     5    14    15     6     7
// OBSOLETE  *
// OBSOLETE  */
// OBSOLETE #define I386_REGNO_TO_SYMMETRY(n) \
// OBSOLETE ((n)==0?0 :(n)==1?2 :(n)==2?1 :(n)==3?5 :(n)==4?14 :(n)==5?15 :(n))
// OBSOLETE 
// OBSOLETE /* The magic numbers below are offsets into u_ar0 in the user struct.
// OBSOLETE  * They live in <machine/reg.h>.  Gdb calls this macro with blockend
// OBSOLETE  * holding u.u_ar0 - KERNEL_U_ADDR.  Only the registers listed are
// OBSOLETE  * saved in the u area (along with a few others that aren't useful
// OBSOLETE  * here.  See <machine/reg.h>).
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno) \
// OBSOLETE { struct user foo;	/* needed for finding fpu regs */ \
// OBSOLETE switch (regno) { \
// OBSOLETE     case 0: \
// OBSOLETE       addr = blockend + EAX * sizeof(int); break; \
// OBSOLETE   case 1: \
// OBSOLETE       addr = blockend + EDX * sizeof(int); break; \
// OBSOLETE   case 2: \
// OBSOLETE       addr = blockend + ECX * sizeof(int); break; \
// OBSOLETE   case 3:			/* st(0) */ \
// OBSOLETE       addr = ((int)&foo.u_fpusave.fpu_stack[0][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 4:			/* st(1) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[1][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 5: \
// OBSOLETE       addr = blockend + EBX * sizeof(int); break; \
// OBSOLETE   case 6: \
// OBSOLETE       addr = blockend + ESI * sizeof(int); break; \
// OBSOLETE   case 7: \
// OBSOLETE       addr = blockend + EDI * sizeof(int); break; \
// OBSOLETE   case 8:			/* st(2) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[2][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 9:			/* st(3) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[3][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 10:			/* st(4) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[4][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 11:			/* st(5) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[5][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 12:			/* st(6) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[6][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 13:			/* st(7) */ \
// OBSOLETE       addr = ((int) &foo.u_fpusave.fpu_stack[7][0] - (int)&foo); \
// OBSOLETE       break; \
// OBSOLETE   case 14: \
// OBSOLETE       addr = blockend + ESP * sizeof(int); break; \
// OBSOLETE   case 15: \
// OBSOLETE       addr = blockend + EBP * sizeof(int); break; \
// OBSOLETE   case 16: \
// OBSOLETE       addr = blockend + EIP * sizeof(int); break; \
// OBSOLETE   case 17: \
// OBSOLETE       addr = blockend + FLAGS * sizeof(int); break; \
// OBSOLETE   case 18:			/* fp1 */ \
// OBSOLETE   case 19:			/* fp2 */ \
// OBSOLETE   case 20:			/* fp3 */ \
// OBSOLETE   case 21:			/* fp4 */ \
// OBSOLETE   case 22:			/* fp5 */ \
// OBSOLETE   case 23:			/* fp6 */ \
// OBSOLETE   case 24:			/* fp7 */ \
// OBSOLETE   case 25:			/* fp8 */ \
// OBSOLETE   case 26:			/* fp9 */ \
// OBSOLETE   case 27:			/* fp10 */ \
// OBSOLETE   case 28:			/* fp11 */ \
// OBSOLETE   case 29:			/* fp12 */ \
// OBSOLETE   case 30:			/* fp13 */ \
// OBSOLETE   case 31:			/* fp14 */ \
// OBSOLETE   case 32:			/* fp15 */ \
// OBSOLETE   case 33:			/* fp16 */ \
// OBSOLETE   case 34:			/* fp17 */ \
// OBSOLETE   case 35:			/* fp18 */ \
// OBSOLETE   case 36:			/* fp19 */ \
// OBSOLETE   case 37:			/* fp20 */ \
// OBSOLETE   case 38:			/* fp21 */ \
// OBSOLETE   case 39:			/* fp22 */ \
// OBSOLETE   case 40:			/* fp23 */ \
// OBSOLETE   case 41:			/* fp24 */ \
// OBSOLETE   case 42:			/* fp25 */ \
// OBSOLETE   case 43:			/* fp26 */ \
// OBSOLETE   case 44:			/* fp27 */ \
// OBSOLETE   case 45:			/* fp28 */ \
// OBSOLETE   case 46:			/* fp29 */ \
// OBSOLETE   case 47:			/* fp30 */ \
// OBSOLETE   case 48:			/* fp31 */ \
// OBSOLETE      addr = ((int) &foo.u_fpasave.fpa_regs[(regno)-18] - (int)&foo); \
// OBSOLETE   } \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Total amount of space needed to store our copies of the machine's
// OBSOLETE    register state, the array `registers'.  10 i*86 registers, 8 i387
// OBSOLETE    registers, and 31 Weitek 1167 registers */
// OBSOLETE 
// OBSOLETE #undef  REGISTER_BYTES
// OBSOLETE #define REGISTER_BYTES ((10 * 4) + (8 * 10) + (31 * 4))
// OBSOLETE 
// OBSOLETE /* Nonzero if register N requires conversion
// OBSOLETE    from raw format to virtual format.  */
// OBSOLETE 
// OBSOLETE #undef  REGISTER_CONVERTIBLE
// OBSOLETE #define REGISTER_CONVERTIBLE(N) \
// OBSOLETE (((N) < 3) ? 0 : \
// OBSOLETE ((N) < 5) ? 1  : \
// OBSOLETE ((N) < 8) ? 0  : \
// OBSOLETE ((N) < 14) ? 1 : \
// OBSOLETE     0)
// OBSOLETE 
// OBSOLETE #include "floatformat.h"
// OBSOLETE 
// OBSOLETE /* Convert data from raw format for register REGNUM in buffer FROM
// OBSOLETE    to virtual format with type TYPE in buffer TO.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_CONVERT_TO_VIRTUAL
// OBSOLETE #define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
// OBSOLETE { \
// OBSOLETE   DOUBLEST val; \
// OBSOLETE   floatformat_to_doublest (&floatformat_i387_ext, (FROM), &val); \
// OBSOLETE   deprecated_store_floating ((TO), TYPE_LENGTH (TYPE), val); \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Convert data from virtual format with type TYPE in buffer FROM
// OBSOLETE    to raw format for register REGNUM in buffer TO.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_CONVERT_TO_RAW
// OBSOLETE #define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
// OBSOLETE { \
// OBSOLETE   DOUBLEST val = deprecated_extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
// OBSOLETE   floatformat_from_doublest (&floatformat_i387_ext, &val, (TO)); \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Return the GDB type object for the "standard" data type
// OBSOLETE    of data in register N.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_VIRTUAL_TYPE
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \
// OBSOLETE ((N < 3) ? builtin_type_int : \
// OBSOLETE (N < 5) ? builtin_type_double : \
// OBSOLETE (N < 8) ? builtin_type_int : \
// OBSOLETE (N < 14) ? builtin_type_double : \
// OBSOLETE     builtin_type_int)
// OBSOLETE 
// OBSOLETE /* Store the address of the place in which to copy the structure the
// OBSOLETE    subroutine will return.  This is called from call_function.
// OBSOLETE    Native cc passes the address in eax, gcc (up to version 2.5.8)
// OBSOLETE    passes it on the stack.  gcc should be fixed in future versions to
// OBSOLETE    adopt native cc conventions.  */
// OBSOLETE 
// OBSOLETE #undef  DEPRECATED_PUSH_ARGUMENTS
// OBSOLETE #undef  STORE_STRUCT_RETURN
// OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) write_register(0, (ADDR))
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    a function return value of type TYPE, and copy that, in virtual format,
// OBSOLETE    into VALBUF.  */
// OBSOLETE 
// OBSOLETE #undef  DEPRECATED_EXTRACT_RETURN_VALUE
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
// OBSOLETE   symmetry_extract_return_value(TYPE, REGBUF, VALBUF)
// OBSOLETE 
// OBSOLETE /* The following redefines make backtracing through sigtramp work.
// OBSOLETE    They manufacture a fake sigtramp frame and obtain the saved pc in sigtramp
// OBSOLETE    from the sigcontext structure which is pushed by the kernel on the
// OBSOLETE    user stack, along with a pointer to it.  */
// OBSOLETE 
// OBSOLETE #define IN_SIGTRAMP(pc, name) ((name) && STREQ ("_sigcode", name))
// OBSOLETE 
// OBSOLETE /* Offset to saved PC in sigcontext, from <signal.h>.  */
// OBSOLETE #define SIGCONTEXT_PC_OFFSET 16
// OBSOLETE 
// OBSOLETE #endif /* ifndef TM_SYMMETRY_H */
