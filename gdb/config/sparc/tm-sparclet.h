// OBSOLETE /* Target machine definitions for GDB for an embedded SPARC.
// OBSOLETE    Copyright 1996, 1997, 2000 Free Software Foundation, Inc.
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
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #define TARGET_SPARCLET 1	/* Still needed for non-multi-arch case */
// OBSOLETE 
// OBSOLETE #include "sparc/tm-sparc.h"
// OBSOLETE 
// OBSOLETE /* Note: we are not defining GDB_MULTI_ARCH for the sparclet target
// OBSOLETE    at this time, because we have not figured out how to detect the
// OBSOLETE    sparclet target from the bfd structure.  */
// OBSOLETE 
// OBSOLETE /* Sparclet regs, for debugging purposes.  */
// OBSOLETE 
// OBSOLETE enum { 
// OBSOLETE   CCSR_REGNUM   = 72,
// OBSOLETE   CCPR_REGNUM   = 73, 
// OBSOLETE   CCCRCR_REGNUM = 74,
// OBSOLETE   CCOR_REGNUM   = 75, 
// OBSOLETE   CCOBR_REGNUM  = 76,
// OBSOLETE   CCIBR_REGNUM  = 77,
// OBSOLETE   CCIR_REGNUM   = 78
// OBSOLETE };
// OBSOLETE 
// OBSOLETE /* Select the sparclet disassembler.  Slightly different instruction set from
// OBSOLETE    the V8 sparc.  */
// OBSOLETE 
// OBSOLETE #undef TM_PRINT_INSN_MACH
// OBSOLETE #define TM_PRINT_INSN_MACH bfd_mach_sparc_sparclet
// OBSOLETE 
// OBSOLETE /* overrides of tm-sparc.h */
// OBSOLETE 
// OBSOLETE #undef TARGET_BYTE_ORDER
// OBSOLETE 
// OBSOLETE /* Sequence of bytes for breakpoint instruction (ta 1). */
// OBSOLETE #undef BREAKPOINT
// OBSOLETE #define BIG_BREAKPOINT {0x91, 0xd0, 0x20, 0x01}
// OBSOLETE #define LITTLE_BREAKPOINT {0x01, 0x20, 0xd0, 0x91}
// OBSOLETE 
// OBSOLETE #if !defined (GDB_MULTI_ARCH) || (GDB_MULTI_ARCH == 0)
// OBSOLETE /*
// OBSOLETE  * The following defines must go away for MULTI_ARCH.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #undef  NUM_REGS		/* formerly "72" */
// OBSOLETE /*                WIN  FP   CPU  CCP  ASR  AWR  APSR */
// OBSOLETE #define NUM_REGS (32 + 32 + 8  + 8  + 8/*+ 32 + 1*/)
// OBSOLETE 
// OBSOLETE #undef  REGISTER_BYTES		/* formerly "(32*4 + 32*4 + 8*4)" */
// OBSOLETE #define REGISTER_BYTES (32*4 + 32*4 + 8*4 + 8*4 + 8*4/* + 32*4 + 1*4*/)
// OBSOLETE 
// OBSOLETE /* Initializer for an array of names of registers.
// OBSOLETE    There should be NUM_REGS strings in this initializer.  */
// OBSOLETE /* Sparclet has no fp! */
// OBSOLETE /* Compiler maps types for floats by number, so can't 
// OBSOLETE    change the numbers here. */
// OBSOLETE 
// OBSOLETE #undef REGISTER_NAMES
// OBSOLETE #define REGISTER_NAMES  \
// OBSOLETE { "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",	\
// OBSOLETE   "o0", "o1", "o2", "o3", "o4", "o5", "o6", "o7",	\
// OBSOLETE   "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",	\
// OBSOLETE   "i0", "i1", "i2", "i3", "i4", "i5", "i6", "i7",	\
// OBSOLETE 							\
// OBSOLETE   "", "", "", "", "", "", "", "", /* no FPU regs */	\
// OBSOLETE   "", "", "", "", "", "", "", "", 			\
// OBSOLETE   "", "", "", "", "", "", "", "", 			\
// OBSOLETE   "", "", "", "", "", "", "", "", 			\
// OBSOLETE 				  /* no CPSR, FPSR */	\
// OBSOLETE   "y", "psr", "wim", "tbr", "pc", "npc", "", "", 	\
// OBSOLETE 							\
// OBSOLETE   "ccsr", "ccpr", "cccrcr", "ccor", "ccobr", "ccibr", "ccir", "", \
// OBSOLETE 								  \
// OBSOLETE   /*       ASR15                 ASR19 (don't display them) */    \
// OBSOLETE   "asr1",  "", "asr17", "asr18", "", "asr20", "asr21", "asr22",   \
// OBSOLETE /*									  \
// OBSOLETE   "awr0",  "awr1",  "awr2",  "awr3",  "awr4",  "awr5",  "awr6",  "awr7",  \
// OBSOLETE   "awr8",  "awr9",  "awr10", "awr11", "awr12", "awr13", "awr14", "awr15", \
// OBSOLETE   "awr16", "awr17", "awr18", "awr19", "awr20", "awr21", "awr22", "awr23", \
// OBSOLETE   "awr24", "awr25", "awr26", "awr27", "awr28", "awr29", "awr30", "awr31", \
// OBSOLETE   "apsr",								  \
// OBSOLETE  */									  \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Remove FP dependant code which was defined in tm-sparc.h */
// OBSOLETE #undef	FP0_REGNUM		/* Floating point register 0 */
// OBSOLETE #undef  FPS_REGNUM		/* Floating point status register */
// OBSOLETE #undef 	CPS_REGNUM		/* Coprocessor status register */
// OBSOLETE 
// OBSOLETE /* sparclet register numbers */
// OBSOLETE #define CCSR_REGNUM 72
// OBSOLETE 
// OBSOLETE #undef DEPRECATED_EXTRACT_RETURN_VALUE
// OBSOLETE #define DEPRECATED_EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF)            \
// OBSOLETE   {                                                                    \
// OBSOLETE     memcpy ((VALBUF),                                                  \
// OBSOLETE 	    (char *)(REGBUF) + REGISTER_RAW_SIZE (O0_REGNUM) * 8 +     \
// OBSOLETE 	    (TYPE_LENGTH(TYPE) >= REGISTER_RAW_SIZE (O0_REGNUM)        \
// OBSOLETE 	     ? 0 : REGISTER_RAW_SIZE (O0_REGNUM) - TYPE_LENGTH(TYPE)), \
// OBSOLETE 	    TYPE_LENGTH(TYPE));                                        \
// OBSOLETE   }
// OBSOLETE #undef DEPRECATED_STORE_RETURN_VALUE
// OBSOLETE #define DEPRECATED_STORE_RETURN_VALUE(TYPE,VALBUF) \
// OBSOLETE   {                                                                    \
// OBSOLETE     /* Other values are returned in register %o0.  */                  \
// OBSOLETE     deprecated_write_register_bytes (REGISTER_BYTE (O0_REGNUM), (VALBUF),         \
// OBSOLETE 			  TYPE_LENGTH (TYPE));                         \
// OBSOLETE   }
// OBSOLETE 
// OBSOLETE #endif /* GDB_MULTI_ARCH */
// OBSOLETE 
// OBSOLETE extern void sparclet_do_registers_info (int regnum, int all);
// OBSOLETE #undef DEPRECATED_DO_REGISTERS_INFO
// OBSOLETE #define DEPRECATED_DO_REGISTERS_INFO(REGNUM,ALL) sparclet_do_registers_info (REGNUM, ALL)
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* Offsets into jmp_buf.  Not defined by Sun, but at least documented in a
// OBSOLETE    comment in <machine/setjmp.h>! */
// OBSOLETE 
// OBSOLETE #define JB_ELEMENT_SIZE 4	/* Size of each element in jmp_buf */
// OBSOLETE 
// OBSOLETE #define JB_ONSSTACK 0
// OBSOLETE #define JB_SIGMASK 1
// OBSOLETE #define JB_SP 2
// OBSOLETE #define JB_PC 3
// OBSOLETE #define JB_NPC 4
// OBSOLETE #define JB_PSR 5
// OBSOLETE #define JB_G1 6
// OBSOLETE #define JB_O0 7
// OBSOLETE #define JB_WBCNT 8
// OBSOLETE 
// OBSOLETE /* Figure out where the longjmp will land.  We expect that we have just entered
// OBSOLETE    longjmp and haven't yet setup the stack frame, so the args are still in the
// OBSOLETE    output regs.  %o0 (O0_REGNUM) points at the jmp_buf structure from which we
// OBSOLETE    extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR.
// OBSOLETE    This routine returns true on success */
// OBSOLETE 
// OBSOLETE extern int get_longjmp_target (CORE_ADDR *);
// OBSOLETE 
// OBSOLETE #define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR)
