/* MIPS Simulator definition.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

/* This simulator doesn't cache the Current Instruction Address */
#define SIM_ENGINE_HALT_HOOK(SD, LAST_CPU, CIA)
#define SIM_ENGINE_RESUME_HOOK(SD, LAST_CPU, CIA)

#define SIM_HAVE_BIENDIAN
#define SIM_HAVE_FLATMEM


/* hobble some common features for moment */
#define WITH_PROFILE 0
#define WITH_TRACE 0
#define WITH_WATCHPOINTS 1

#include "sim-basics.h"

/* dummy - not used */
typedef int sim_cia;
#define NULL_CIA 0

#include "sim-base.h"


/* Depreciated macros and types for manipulating 64bit values.  Use
   ../common/sim-bits.h and ../common/sim-endian.h macros instead. */

typedef signed64 word64;
typedef unsigned64 uword64;

#define WORD64LO(t)     (unsigned int)((t)&0xFFFFFFFF)
#define WORD64HI(t)     (unsigned int)(((uword64)(t))>>32)
#define SET64LO(t)      (((uword64)(t))&0xFFFFFFFF)
#define SET64HI(t)	(((uword64)(t))<<32)
#define WORD64(h,l)     ((word64)((SET64HI(h)|SET64LO(l))))
#define UWORD64(h,l)     (SET64HI(h)|SET64LO(l))

/* Sign-extend the given value (e) as a value (b) bits long. We cannot
   assume the HI32bits of the operand are zero, so we must perform a
   mask to ensure we can use the simple subtraction to sign-extend. */
#define SIGNEXTEND(e,b) \
  (((e) & ((uword64) 1 << ((b) - 1))) \
   ? (((e) & (((uword64) 1 << (b)) - 1)) - ((uword64)1 << (b))) \
   : ((e) & (((((uword64) 1 << ((b) - 1)) - 1) << 1) | 1)))

/* Check if a value will fit within a halfword: */
#define NOTHALFWORDVALUE(v) ((((((uword64)(v)>>16) == 0) && !((v) & ((unsigned)1 << 15))) || (((((uword64)(v)>>32) == 0xFFFFFFFF) && ((((uword64)(v)>>16) & 0xFFFF) == 0xFFFF)) && ((v) & ((unsigned)1 << 15)))) ? (1 == 0) : (1 == 1))

/* windows always looses */
#include <signal.h>
#ifndef SIGBUS
#define SIGBUS SIGSEGV
#endif
#ifdef _WIN32
#define SIGTRAP 5
#define SIGQUIT 3
#endif


/* FIXME: At present the CPU registers are static */
struct _sim_cpu {
  sim_cpu_base base;
};


/* MIPS specific simulator watch config */

void watch_options_install PARAMS ((SIM_DESC sd));

struct swatch {
  sim_event *pc;
  sim_event *clock;
  sim_event *cycles;
};


/* FIXME: At present much of the simulator is still static */
struct sim_state {

  struct swatch watch;

  sim_cpu cpu[1];
#if (WITH_SMP)
#define STATE_CPU(sd,n) (&(sd)->cpu[n])
#else
#define STATE_CPU(sd,n) (&(sd)->cpu[0])
#endif

  sim_state_base base;
};


/* Exceptions: */

/* NOTE: These numbers depend on the processor architecture being
   simulated: */
#define Interrupt               (0)
#define TLBModification         (1)
#define TLBLoad                 (2)
#define TLBStore                (3)
#define AddressLoad             (4)
#define AddressStore            (5)
#define InstructionFetch        (6)
#define DataReference           (7)
#define SystemCall              (8)
#define BreakPoint              (9)
#define ReservedInstruction     (10)
#define CoProcessorUnusable     (11)
#define IntegerOverflow         (12)    /* Arithmetic overflow (IDT monitor raises SIGFPE) */
#define Trap                    (13)
#define FPE                     (15)
#define DebugBreakPoint         (16)
#define Watch                   (23)

/* The following exception code is actually private to the simulator
   world. It is *NOT* a processor feature, and is used to signal
   run-time errors in the simulator. */
#define SimulatorFault      (0xFFFFFFFF)

void signal_exception (SIM_DESC sd, int exception, ...);
#define SignalException(exc,instruction) signal_exception (sd, (exc), (instruction))
#define SignalExceptionInterrupt() signal_exception (sd, Interrupt)
#define SignalExceptionInstructionFetch() signal_exception (sd, InstructionFetch)
#define SignalExceptionAddressStore() signal_exception (sd, AddressStore)
#define SignalExceptionAddressLoad() signal_exception (sd, AddressLoad)
#define SignalExceptionSimulatorFault(buf) signal_exception (sd, SimulatorFault, buf)
#define SignalExceptionFPE() signal_exception (sd, FPE)
#define SignalExceptionIntegerOverflow() signal_exception (sd, IntegerOverflow)
#define SignalExceptionCoProcessorUnusable() signal_exception (sd, CoProcessorUnusable)


/* Floating-point operations: */

/* FPU registers must be one of the following types. All other values
   are reserved (and undefined). */
typedef enum {
 fmt_single  = 0,
 fmt_double  = 1,
 fmt_word    = 4,
 fmt_long    = 5,
 /* The following are well outside the normal acceptable format
    range, and are used in the register status vector. */
 fmt_unknown       = 0x10000000,
 fmt_uninterpreted = 0x20000000,
} FP_formats;

unsigned64 value_fpr PARAMS ((SIM_DESC sd, int fpr, FP_formats));
#define ValueFPR(FPR,FMT) value_fpr (sd, (FPR), (FMT))

void store_fpr PARAMS ((SIM_DESC sd, int fpr, FP_formats fmt, unsigned64 value));
#define StoreFPR(FPR,FMT,VALUE) store_fpr (sd, (FPR), (FMT), (VALUE))

int NaN PARAMS ((unsigned64 op, FP_formats fmt));
int Infinity PARAMS ((unsigned64 op, FP_formats fmt));
int Less PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
int Equal PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
unsigned64 AbsoluteValue PARAMS ((unsigned64 op, FP_formats fmt));
unsigned64 Negate PARAMS ((unsigned64 op, FP_formats fmt));
unsigned64 Add PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
unsigned64 Sub PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
unsigned64 Multiply PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
unsigned64 Divide PARAMS ((unsigned64 op1, unsigned64 op2, FP_formats fmt));
unsigned64 Recip PARAMS ((unsigned64 op, FP_formats fmt));
unsigned64 SquareRoot PARAMS ((unsigned64 op, FP_formats fmt));
unsigned64 convert PARAMS ((SIM_DESC sd, int rm, unsigned64 op, FP_formats from, FP_formats to));
#define Convert(rm,op,from,to) convert(sd,rm,op,from,to)


/* Co-processor accesses */

void cop_lw  PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg, unsigned int memword));
void cop_ld  PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg, uword64 memword));
unsigned int cop_sw PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg));
uword64 cop_sd PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg));

#define COP_LW(coproc_num,coproc_reg,memword) cop_lw(sd,coproc_num,coproc_reg,memword)
#define COP_LD(coproc_num,coproc_reg,memword) cop_ld(sd,coproc_num,coproc_reg,memword)
#define COP_SW(coproc_num,coproc_reg) cop_sw(sd,coproc_num,coproc_reg)
#define COP_SD(coproc_num,coproc_reg) cop_sd(sd,coproc_num,coproc_reg)



/* Memory accesses */

int address_translation PARAMS ((SIM_DESC sd, uword64 vAddr, int IorD, int LorS, uword64 *pAddr, int *CCA, int host, int raw));
#define AddressTranslation(vAddr,IorD,LorS,pAddr,CCA,host,raw) \
address_translation(sd, vAddr,IorD,LorS,pAddr,CCA,host,raw)

void load_memory PARAMS ((SIM_DESC sd, uword64* memvalp, uword64* memval1p, int CCA, int AccessLength, uword64 pAddr, uword64 vAddr, int IorD, int raw));
#define LoadMemory(memvalp,memval1p,CCA,AccessLength,pAddr,vAddr,IorD,raw) \
load_memory(sd,memvalp,memval1p,CCA,AccessLength,pAddr,vAddr,IorD,raw)

void store_memory PARAMS ((SIM_DESC sd, int CCA, int AccessLength, uword64 MemElem,     uword64 MemElem1, uword64 pAddr, uword64 vAddr, int raw));
#define StoreMemory(CCA,AccessLength,MemElem,MemElem1,pAddr,vAddr,raw) \
store_memory(sd,CCA,AccessLength,MemElem,MemElem1,pAddr,vAddr,raw)

void cache_op PARAMS ((SIM_DESC sd, int op, uword64 pAddr, uword64 vAddr, unsigned int instruction));
#define CacheOp(op,pAddr,vAddr,instruction) cache_op(sd,op,pAddr,vAddr,instruction)

#endif
