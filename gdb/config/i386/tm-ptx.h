// OBSOLETE /* Target machine definitions for GDB on a Sequent Symmetry under ptx
// OBSOLETE    with Weitek 1167 and i387 support.
// OBSOLETE 
// OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 2000,
// OBSOLETE    2003 Free Software Foundation, Inc.
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
// OBSOLETE #ifndef TM_PTX_H
// OBSOLETE #define TM_PTX_H 1
// OBSOLETE 
// OBSOLETE /* I don't know if this will work for cross-debugging, even if you do get
// OBSOLETE    a copy of the right include file.  */
// OBSOLETE 
// OBSOLETE #include <sys/reg.h>
// OBSOLETE 
// OBSOLETE #ifdef SEQUENT_PTX4
// OBSOLETE #include "i386/tm-i386.h"
// OBSOLETE #else /* !SEQUENT_PTX4 */
// OBSOLETE #include "i386/tm-i386.h"
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* Amount PC must be decremented by after a breakpoint.  This is often the
// OBSOLETE    number of bytes in BREAKPOINT but not always (such as now). */
// OBSOLETE 
// OBSOLETE #undef DECR_PC_AFTER_BREAK
// OBSOLETE #define DECR_PC_AFTER_BREAK 0
// OBSOLETE 
// OBSOLETE /* Number of machine registers */
// OBSOLETE 
// OBSOLETE #undef  NUM_REGS
// OBSOLETE #define NUM_REGS 49
// OBSOLETE 
// OBSOLETE /* Initializer for an array of names of registers.  There should be at least
// OBSOLETE    NUM_REGS strings in this initializer.  Any excess ones are simply ignored.
// OBSOLETE    The order of the first 8 registers must match the compiler's numbering
// OBSOLETE    scheme (which is the same as the 386 scheme) and also regmap in the various
// OBSOLETE    *-nat.c files. */
// OBSOLETE 
// OBSOLETE #undef REGISTER_NAME
// OBSOLETE #define REGISTER_NAMES { "eax",  "ecx",    "edx",  "ebx",  \
// OBSOLETE 			 "esp",  "ebp",    "esi",  "edi",  \
// OBSOLETE 			 "eip",  "eflags", "st0",  "st1",  \
// OBSOLETE 			 "st2",  "st3",    "st4",  "st5",  \
// OBSOLETE 			 "st6",  "st7",    "fp1",  "fp2",  \
// OBSOLETE 			 "fp3",  "fp4",    "fp5",  "fp6",  \
// OBSOLETE 			 "fp7",  "fp8",    "fp9",  "fp10", \
// OBSOLETE 			 "fp11", "fp12",   "fp13", "fp14", \
// OBSOLETE 			 "fp15", "fp16",   "fp17", "fp18", \
// OBSOLETE 			 "fp19", "fp20",   "fp21", "fp22", \
// OBSOLETE 			 "fp23", "fp24",   "fp25", "fp26", \
// OBSOLETE 			 "fp27", "fp28",   "fp29", "fp30", \
// OBSOLETE 			 "fp31" }
// OBSOLETE 
// OBSOLETE /* Register numbers of various important registers.
// OBSOLETE    Note that some of these values are "real" register numbers,
// OBSOLETE    and correspond to the general registers of the machine,
// OBSOLETE    and some are "phony" register numbers which are too large
// OBSOLETE    to be actual register numbers as far as the user is concerned
// OBSOLETE    but do serve to get the desired values when passed to read_register.  */
// OBSOLETE 
// OBSOLETE #define EAX_REGNUM	0
// OBSOLETE #define ECX_REGNUM	1
// OBSOLETE #define EDX_REGNUM	2
// OBSOLETE #define EBX_REGNUM	3
// OBSOLETE 
// OBSOLETE #define ESP_REGNUM	4
// OBSOLETE #define EBP_REGNUM	5
// OBSOLETE 
// OBSOLETE #define ESI_REGNUM	6
// OBSOLETE #define EDI_REGNUM	7
// OBSOLETE 
// OBSOLETE #define EIP_REGNUM	8
// OBSOLETE #define EFLAGS_REGNUM	9
// OBSOLETE 
// OBSOLETE #define ST0_REGNUM	10
// OBSOLETE #define ST1_REGNUM	11
// OBSOLETE #define ST2_REGNUM	12
// OBSOLETE #define ST3_REGNUM	13
// OBSOLETE 
// OBSOLETE #define ST4_REGNUM	14
// OBSOLETE #define ST5_REGNUM	15
// OBSOLETE #define ST6_REGNUM	16
// OBSOLETE #define ST7_REGNUM	17
// OBSOLETE 
// OBSOLETE #define FP1_REGNUM 18		/* first 1167 register */
// OBSOLETE /* Get %fp2 - %fp31 by addition, since they are contiguous */
// OBSOLETE 
// OBSOLETE #undef  SP_REGNUM
// OBSOLETE #define SP_REGNUM ESP_REGNUM	/* Contains address of top of stack */
// OBSOLETE #undef  FP_REGNUM
// OBSOLETE #define FP_REGNUM EBP_REGNUM	/* Contains address of executing stack frame */
// OBSOLETE #undef  PC_REGNUM
// OBSOLETE #define PC_REGNUM EIP_REGNUM	/* Contains program counter */
// OBSOLETE #undef  PS_REGNUM
// OBSOLETE #define PS_REGNUM EFLAGS_REGNUM	/* Contains processor status */
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * For ptx, this is a little bit bizarre, since the register block
// OBSOLETE  * is below the u area in memory.  This means that blockend here ends
// OBSOLETE  * up being negative (for the call from coredep.c) since the value in
// OBSOLETE  * u.u_ar0 will be less than KERNEL_U_ADDR (and coredep.c passes us
// OBSOLETE  * u.u_ar0 - KERNEL_U_ADDR in blockend).  Since we also define
// OBSOLETE  * FETCH_INFERIOR_REGISTERS (and supply our own functions for that),
// OBSOLETE  * the core file case will be the only use of this function.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno) \
// OBSOLETE { (addr) = ptx_register_u_addr((blockend), (regno)); }
// OBSOLETE 
// OBSOLETE extern int ptx_register_u_addr (int, int);
// OBSOLETE 
// OBSOLETE /* Total amount of space needed to store our copies of the machine's
// OBSOLETE    register state, the array `registers'.  10 i*86 registers, 8 i387
// OBSOLETE    registers, and 31 Weitek 1167 registers */
// OBSOLETE 
// OBSOLETE #undef  REGISTER_BYTES
// OBSOLETE #define REGISTER_BYTES ((10 * 4) + (8 * 10) + (31 * 4))
// OBSOLETE 
// OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  */
// OBSOLETE 
// OBSOLETE #undef  DEPRECATED_MAX_REGISTER_RAW_SIZE
// OBSOLETE #define DEPRECATED_MAX_REGISTER_RAW_SIZE 10
// OBSOLETE 
// OBSOLETE /* Nonzero if register N requires conversion
// OBSOLETE    from raw format to virtual format.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_CONVERTIBLE
// OBSOLETE #define REGISTER_CONVERTIBLE(N) \
// OBSOLETE ((N < ST0_REGNUM) ? 0 : \
// OBSOLETE  (N < FP1_REGNUM) ? 1 : \
// OBSOLETE  0)
// OBSOLETE 
// OBSOLETE /* Convert data from raw format for register REGNUM
// OBSOLETE    to virtual format for register REGNUM.  */
// OBSOLETE extern const struct floatformat floatformat_i387_ext;	/* from floatformat.h */
// OBSOLETE 
// OBSOLETE #undef REGISTER_CONVERT_TO_VIRTUAL
// OBSOLETE #define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO)	\
// OBSOLETE ((REGNUM < ST0_REGNUM) ?  (void)memcpy ((TO), (FROM), 4) : \
// OBSOLETE  (REGNUM < FP1_REGNUM) ? (void)floatformat_to_double(&floatformat_i387_ext, \
// OBSOLETE 						       (FROM),(TO)) : \
// OBSOLETE  (void)memcpy ((TO), (FROM), 4))
// OBSOLETE 
// OBSOLETE /* Convert data from virtual format for register REGNUM
// OBSOLETE    to raw format for register REGNUM.  */
// OBSOLETE 
// OBSOLETE #undef REGISTER_CONVERT_TO_RAW
// OBSOLETE #define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO)	\
// OBSOLETE ((REGNUM < ST0_REGNUM) ?  (void)memcpy ((TO), (FROM), 4) : \
// OBSOLETE  (REGNUM < FP1_REGNUM) ? (void)floatformat_from_double(&floatformat_i387_ext, \
// OBSOLETE 						       (FROM),(TO)) : \
// OBSOLETE  (void)memcpy ((TO), (FROM), 4))
// OBSOLETE 
// OBSOLETE /* Return the GDB type object for the "standard" data type
// OBSOLETE    of data in register N.  */
// OBSOLETE /*
// OBSOLETE  * Note: the 1167 registers (the last line, builtin_type_float) are
// OBSOLETE  * generally used in pairs, with each pair being treated as a double.
// OBSOLETE  * It it also possible to use them singly as floats.  I'm not sure how
// OBSOLETE  * in gdb to treat the register pair pseudo-doubles. -fubar
// OBSOLETE  */
// OBSOLETE #undef REGISTER_VIRTUAL_TYPE
// OBSOLETE #define REGISTER_VIRTUAL_TYPE(N) \
// OBSOLETE ((N < ST0_REGNUM) ? builtin_type_int : \
// OBSOLETE  (N < FP1_REGNUM) ? builtin_type_double : \
// OBSOLETE  builtin_type_float)
// OBSOLETE 
// OBSOLETE /* Extract from an array REGBUF containing the (raw) register state
// OBSOLETE    a function return value of type TYPE, and copy that, in virtual format,
// OBSOLETE    into VALBUF.  */
// OBSOLETE 
// OBSOLETE #undef  DEPRECATED_EXTRACT_RETURN_VALUE
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
// OBSOLETE   symmetry_extract_return_value(TYPE, REGBUF, VALBUF)
// OBSOLETE 
// OBSOLETE #endif /* ifndef TM_PTX_H */
