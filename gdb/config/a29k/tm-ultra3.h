/* OBSOLETE /* Parameters for NYU Ultracomputer 29000 target, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1990, 1991, 1993 Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by David Wood @ New York University (wood@nyu.edu).  */
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
/* OBSOLETE /* This file includes tm-a29k.h, but predefines REGISTER_NAMES and */
/* OBSOLETE    related macros.  The file supports a a29k running our flavor of */
/* OBSOLETE    Unix on our Ultra3 PE Boards.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Byte order is configurable, but this machine runs big-endian.  */ */
/* OBSOLETE #define TARGET_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer. */
/* OBSOLETE  */ */
/* OBSOLETE #define NUM_REGS   (EXO_REGNUM + 1) */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES { 						 \ */
/* OBSOLETE  "gr1",									 \ */
/* OBSOLETE  "gr64", "gr65", "gr66", "gr67", "gr68", "gr69", "gr70", "gr71", "gr72", \ */
/* OBSOLETE  "gr73", "gr74", "gr75", "gr76", "gr77", "gr78", "gr79", "gr80", "gr81", \ */
/* OBSOLETE  "gr82", "gr83", "gr84", "gr85", "gr86", "gr87", "gr88", "gr89", "gr90", \ */
/* OBSOLETE  "gr91", "gr92", "gr93", "gr94", "gr95",				 \ */
/* OBSOLETE  "gr96", "gr97", "gr98", "gr99", "gr100", "gr101", "gr102", "gr103", "gr104", \ */
/* OBSOLETE  "gr105", "gr106", "gr107", "gr108", "gr109", "gr110", "gr111", "gr112", \ */
/* OBSOLETE  "gr113", "gr114", "gr115", "gr116", "gr117", "gr118", "gr119", "gr120", \ */
/* OBSOLETE  "gr121", "gr122", "gr123", "gr124", "gr125", "gr126", "gr127",		 \ */
/* OBSOLETE  "lr0", "lr1", "lr2", "lr3", "lr4", "lr5", "lr6", "lr7", "lr8", "lr9",   \ */
/* OBSOLETE  "lr10", "lr11", "lr12", "lr13", "lr14", "lr15", "lr16", "lr17", "lr18", \ */
/* OBSOLETE  "lr19", "lr20", "lr21", "lr22", "lr23", "lr24", "lr25", "lr26", "lr27", \ */
/* OBSOLETE  "lr28", "lr29", "lr30", "lr31", "lr32", "lr33", "lr34", "lr35", "lr36", \ */
/* OBSOLETE  "lr37", "lr38", "lr39", "lr40", "lr41", "lr42", "lr43", "lr44", "lr45", \ */
/* OBSOLETE  "lr46", "lr47", "lr48", "lr49", "lr50", "lr51", "lr52", "lr53", "lr54", \ */
/* OBSOLETE  "lr55", "lr56", "lr57", "lr58", "lr59", "lr60", "lr61", "lr62", "lr63", \ */
/* OBSOLETE  "lr64", "lr65", "lr66", "lr67", "lr68", "lr69", "lr70", "lr71", "lr72", \ */
/* OBSOLETE  "lr73", "lr74", "lr75", "lr76", "lr77", "lr78", "lr79", "lr80", "lr81", \ */
/* OBSOLETE  "lr82", "lr83", "lr84", "lr85", "lr86", "lr87", "lr88", "lr89", "lr90", \ */
/* OBSOLETE  "lr91", "lr92", "lr93", "lr94", "lr95", "lr96", "lr97", "lr98", "lr99", \ */
/* OBSOLETE  "lr100", "lr101", "lr102", "lr103", "lr104", "lr105", "lr106", "lr107", \ */
/* OBSOLETE  "lr108", "lr109", "lr110", "lr111", "lr112", "lr113", "lr114", "lr115", \ */
/* OBSOLETE  "lr116", "lr117", "lr118", "lr119", "lr120", "lr121", "lr122", "lr123", \ */
/* OBSOLETE  "lr124", "lr125", "lr126", "lr127",					 \ */
/* OBSOLETE  "vab", "ops", "cps", "cfg", "cha", "chd", "chc", "rbp", "tmc", "tmr",	 \ */
/* OBSOLETE  "pc0", "pc1", "pc2", "mmu", "lru",					 \ */
/* OBSOLETE  "ipc", "ipa", "ipb", "q", "alu", "bp", "fc", "cr",			 \ */
/* OBSOLETE  "fpe", "int", "fps", "exo" } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE #ifdef KERNEL_DEBUGGING */
/* OBSOLETE #define	PADDR_U_REGNUM	22	/* gr86 */ */
/* OBSOLETE #define	RETURN_REGNUM	GR64_REGNUM */
/* OBSOLETE #else */
/* OBSOLETE #define	RETURN_REGNUM	GR96_REGNUM */
/* OBSOLETE #endif /* KERNEL_DEBUGGING */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Should rename all GR96_REGNUM to RETURN_REGNUM */ */
/* OBSOLETE #define GR1_REGNUM 	(0) */
/* OBSOLETE #define GR64_REGNUM	1 */
/* OBSOLETE #define GR96_REGNUM 	(GR64_REGNUM + 32) */
/* OBSOLETE /* This needs to be the memory stack pointer, not the register stack pointer, */
/* OBSOLETE    to make call_function work right.  */ */
/* OBSOLETE #define SP_REGNUM 	MSP_REGNUM */
/* OBSOLETE  */
/* OBSOLETE #define FP_REGNUM 	(LR0_REGNUM + 1)	/* lr1 */ */
/* OBSOLETE /* Large Return Pointer  */ */
/* OBSOLETE #define LRP_REGNUM (123 - 96 + RETURN_REGNUM) */
/* OBSOLETE /* Static link pointer   */ */
/* OBSOLETE #define SLP_REGNUM (124 - 96 + RETURN_REGNUM) */
/* OBSOLETE /* Memory Stack Pointer.  */ */
/* OBSOLETE #define MSP_REGNUM (125 - 96 + RETURN_REGNUM) */
/* OBSOLETE /* Register allocate bound.  */ */
/* OBSOLETE #define RAB_REGNUM (126 - 96 + RETURN_REGNUM) */
/* OBSOLETE /* Register Free Bound.  */ */
/* OBSOLETE #define RFB_REGNUM (127 - 96 + RETURN_REGNUM) */
/* OBSOLETE /* Register Stack Pointer.  */ */
/* OBSOLETE #define RSP_REGNUM GR1_REGNUM */
/* OBSOLETE #define LR0_REGNUM ( 32 +  GR96_REGNUM) */
/* OBSOLETE  */
/* OBSOLETE /* Protected Special registers */ */
/* OBSOLETE #define VAB_REGNUM (LR0_REGNUM +  128) */
/* OBSOLETE #define OPS_REGNUM (VAB_REGNUM + 1) */
/* OBSOLETE #define CPS_REGNUM (VAB_REGNUM + 2) */
/* OBSOLETE #define CFG_REGNUM (VAB_REGNUM + 3) */
/* OBSOLETE #define CHA_REGNUM (VAB_REGNUM + 4) */
/* OBSOLETE #define CHD_REGNUM (VAB_REGNUM + 5) */
/* OBSOLETE #define CHC_REGNUM (VAB_REGNUM + 6) */
/* OBSOLETE #define RBP_REGNUM (VAB_REGNUM + 7) */
/* OBSOLETE #define TMC_REGNUM (VAB_REGNUM + 8) */
/* OBSOLETE #define TMR_REGNUM (VAB_REGNUM + 9) */
/* OBSOLETE #define NPC_REGNUM (VAB_REGNUM + 10)	/* pc0 */ */
/* OBSOLETE #define PC_REGNUM  (VAB_REGNUM + 11)	/* pc1 */ */
/* OBSOLETE #define PC2_REGNUM (VAB_REGNUM + 12)	/* pc2 */ */
/* OBSOLETE #define MMU_REGNUM (VAB_REGNUM + 13) */
/* OBSOLETE #define LRU_REGNUM (VAB_REGNUM + 14) */
/* OBSOLETE 	/* Register sequence gap */ */
/* OBSOLETE /* Unprotected Special registers */ */
/* OBSOLETE #define IPC_REGNUM (LRU_REGNUM + 1) */
/* OBSOLETE #define IPA_REGNUM (IPC_REGNUM + 1) */
/* OBSOLETE #define IPB_REGNUM (IPC_REGNUM + 2) */
/* OBSOLETE #define Q_REGNUM   (IPC_REGNUM + 3) */
/* OBSOLETE #define ALU_REGNUM (IPC_REGNUM + 4) */
/* OBSOLETE #define PS_REGNUM  ALU_REGNUM */
/* OBSOLETE #define BP_REGNUM  (IPC_REGNUM + 5) */
/* OBSOLETE #define FC_REGNUM  (IPC_REGNUM + 6) */
/* OBSOLETE #define CR_REGNUM  (IPC_REGNUM + 7) */
/* OBSOLETE 	/* Register sequence gap */ */
/* OBSOLETE #define FPE_REGNUM (CR_REGNUM  + 1) */
/* OBSOLETE #define INT_REGNUM (FPE_REGNUM + 1) */
/* OBSOLETE #define FPS_REGNUM (FPE_REGNUM + 2) */
/* OBSOLETE 	/* Register sequence gap */ */
/* OBSOLETE #define EXO_REGNUM (FPS_REGNUM + 1) */
/* OBSOLETE  */
/* OBSOLETE /* Special register #x.  */ */
/* OBSOLETE #define SR_REGNUM(x) \ */
/* OBSOLETE   ((x) < 15  ? VAB_REGNUM + (x)					 \ */
/* OBSOLETE    : (x) >= 128 && (x) < 136 ? IPC_REGNUM + (x-128)		 \ */
/* OBSOLETE    : (x) >= 160 && (x) < 163 ? FPE_REGNUM + (x-160)		 \ */
/* OBSOLETE    : (x) == 164 ? EXO_REGNUM                                     \ */
/* OBSOLETE    : (error ("Internal error in SR_REGNUM"), 0)) */
/* OBSOLETE  */
/* OBSOLETE #ifndef KERNEL_DEBUGGING */
/* OBSOLETE /* */
/* OBSOLETE  * This macro defines the register numbers (from REGISTER_NAMES) that */
/* OBSOLETE  * are effectively unavailable to the user through ptrace().  It allows  */
/* OBSOLETE  * us to include the whole register set in REGISTER_NAMES (inorder to  */
/* OBSOLETE  * better support remote debugging).  If it is used in  */
/* OBSOLETE  * fetch/store_inferior_registers() gdb will not complain about I/O errors  */
/* OBSOLETE  * on fetching these registers.  If all registers in REGISTER_NAMES */
/* OBSOLETE  * are available, then return false (0). */
/* OBSOLETE  */ */
/* OBSOLETE #define CANNOT_STORE_REGISTER(regno)		\ */
/* OBSOLETE                   (((regno)>=GR64_REGNUM && (regno)<GR64_REGNUM+32) ||	\ */
/* OBSOLETE                    ((regno)==VAB_REGNUM) ||	\ */
/* OBSOLETE 		   ((regno)==OPS_REGNUM) ||	\ */
/* OBSOLETE                    ((regno)>=CFG_REGNUM && (regno)<=TMR_REGNUM)     ||	\ */
/* OBSOLETE                    ((regno)==MMU_REGNUM) ||	\ */
/* OBSOLETE 		   ((regno)==LRU_REGNUM) ||	\ */
/* OBSOLETE                    ((regno)>=ALU_REGNUM) ||	\ */
/* OBSOLETE                    ((regno)==CR_REGNUM)  ||	\ */
/* OBSOLETE 		   ((regno)==EXO_REGNUM)) */
/* OBSOLETE #define CANNOT_FETCH_REGISTER(regno)	CANNOT_STORE_REGISTER(regno) */
/* OBSOLETE #endif /* KERNEL_DEBUGGING */ */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Converts an sdb register number to an internal gdb register number. */
/* OBSOLETE  * Currently under gcc, gr96->0...gr128->31...lr0->32...lr127->159, or... */
/* OBSOLETE  *                      gr64->0...gr95->31, lr0->32...lr127->159. */
/* OBSOLETE  */ */
/* OBSOLETE #define SDB_REG_TO_REGNUM(value) (((value)<32) ? ((value)+RETURN_REGNUM) : \ */
/* OBSOLETE 		 		                 ((value)-32+LR0_REGNUM)) */
/* OBSOLETE  */
/* OBSOLETE #ifdef KERNEL_DEBUGGING */
/* OBSOLETE   /* ublock virtual address as defined in our sys/param.h */ */
/* OBSOLETE   /* FIXME: Should get this from sys/param.h */ */
/* OBSOLETE #define UVADDR	((32*0x100000)-8192) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* */
/* OBSOLETE  * Are we in sigtramp(), needed in infrun.c.  Specific to ultra3, because */
/* OBSOLETE  * we take off the leading '_'. */
/* OBSOLETE  */ */
/* OBSOLETE #if !defined(KERNEL_DEBUGGING) */
/* OBSOLETE #ifdef SYM1 */
/* OBSOLETE #define IN_SIGTRAMP(pc, name) (name && STREQ ("sigtramp", name)) */
/* OBSOLETE #else */
/* OBSOLETE Need to define */
/* OBSOLETE IN_SIGTRAMP () for sym2. */
/* OBSOLETE #endif */
/* OBSOLETE #endif				/* !KERNEL_DEBUGGING */ */
/* OBSOLETE  */
/* OBSOLETE #include "a29k/tm-a29k.h" */
/* OBSOLETE  */
/* OBSOLETE /**** The following are definitions that override those in tm-a29k.h ****/ */
/* OBSOLETE  */
/* OBSOLETE /* This sequence of words is the instructions */
/* OBSOLETE    mtsrim cr, 15 */
/* OBSOLETE    loadm 0, 0, lr2, msp     ; load first 16 words of arguments into registers */
/* OBSOLETE    add msp, msp, 16 * 4     ; point to the remaining arguments */
/* OBSOLETE    CONST_INSN: */
/* OBSOLETE    const gr96,inf */
/* OBSOLETE    consth gr96,inf */
/* OBSOLETE    calli lr0, gr96 */
/* OBSOLETE    aseq 0x40,gr1,gr1   ; nop */
/* OBSOLETE    asneq 0x50,gr1,gr1  ; breakpoint */
/* OBSOLETE    When KERNEL_DEBUGGIN is defined, msp -> gr93, gr96 -> gr64, */
/* OBSOLETE    7d  -> 5d,    60  -> 40 */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE /* Position of the "const" instruction within CALL_DUMMY in bytes.  */ */
/* OBSOLETE #undef CALL_DUMMY */
/* OBSOLETE #if TARGET_BYTE_ORDER == HOST_BYTE_ORDER */
/* OBSOLETE #ifdef KERNEL_DEBUGGING		/* gr96 -> gr64 */ */
/* OBSOLETE #define CALL_DUMMY {0x0400870f, 0x3600825d, 0x155d5d40, 0x03ff40ff,    \ */
/* OBSOLETE                     0x02ff40ff, 0xc8008040, 0x70400101, 0x72500101} */
/* OBSOLETE #else */
/* OBSOLETE #define CALL_DUMMY {0x0400870f, 0x3600827d, 0x157d7d40, 0x03ff60ff,    \ */
/* OBSOLETE                     0x02ff60ff, 0xc8008060, 0x70400101, 0x72500101} */
/* OBSOLETE #endif				/* KERNEL_DEBUGGING */ */
/* OBSOLETE #else				/* Byte order differs.  */ */
/* OBSOLETE you lose */
/* OBSOLETE #endif				/* Byte order differs.  */ */
/* OBSOLETE  */
/* OBSOLETE #if !defined(KERNEL_DEBUGGING) */
/* OBSOLETE #ifdef SYM1 */
/* OBSOLETE #undef  DECR_PC_AFTER_BREAK */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0	/* Sym1 kernel does the decrement */ */
/* OBSOLETE #else */
/* OBSOLETE ->"ULTRA3 running other than sym1 OS" !; */
/* OBSOLETE #endif */
/* OBSOLETE #endif /* !KERNEL_DEBUGGING */ */
