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
#define WITH_WATCHPOINTS 1

#include "sim-basics.h"

typedef address_word sim_cia;

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
 ((unsigned_word) \
  (((e) & ((uword64) 1 << ((b) - 1))) \
   ? (((e) & (((uword64) 1 << (b)) - 1)) - ((uword64)1 << (b))) \
   : ((e) & (((((uword64) 1 << ((b) - 1)) - 1) << 1) | 1))))

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

/* Macro to update FPSR condition-code field. This is complicated by
   the fact that there is a hole in the index range of the bits within
   the FCSR register. Also, the number of bits visible depends on the
   MIPS ISA version being supported. */

#define SETFCC(cc,v) {\
  int bit = ((cc == 0) ? 23 : (24 + (cc)));\
  FCSR = ((FCSR & ~(1 << bit)) | ((v) << bit));\
}
#define GETFCC(cc) (((((cc) == 0) ? (FCSR & (1 << 23)) : (FCSR & (1 << (24 + (cc))))) != 0) ? 1 : 0)

/* This should be the COC1 value at the start of the preceding
   instruction: */
#define PREVCOC1() ((STATE & simPCOC1) ? 1 : 0)

#if 1
#define SizeFGR() (WITH_TARGET_WORD_BITSIZE)
#else
/* They depend on the CPU being simulated */
#define SizeFGR() ((WITH_TARGET_WORD_BITSIZE == 64 && ((SR & status_FR) == 1)) ? 64 : 32)
#endif

/* Standard FCRS bits: */
#define IR (0) /* Inexact Result */
#define UF (1) /* UnderFlow */
#define OF (2) /* OverFlow */
#define DZ (3) /* Division by Zero */
#define IO (4) /* Invalid Operation */
#define UO (5) /* Unimplemented Operation */

/* Get masks for individual flags: */
#if 1 /* SAFE version */
#define FP_FLAGS(b)  (((unsigned)(b) < 5) ? (1 << ((b) + 2)) : 0)
#define FP_ENABLE(b) (((unsigned)(b) < 5) ? (1 << ((b) + 7)) : 0)
#define FP_CAUSE(b)  (((unsigned)(b) < 6) ? (1 << ((b) + 12)) : 0)
#else
#define FP_FLAGS(b)  (1 << ((b) + 2))
#define FP_ENABLE(b) (1 << ((b) + 7))
#define FP_CAUSE(b)  (1 << ((b) + 12))
#endif

#define FP_FS         (1 << 24) /* MIPS III onwards : Flush to Zero */

#define FP_MASK_RM    (0x3)
#define FP_SH_RM      (0)
#define FP_RM_NEAREST (0) /* Round to nearest        (Round) */
#define FP_RM_TOZERO  (1) /* Round to zero           (Trunc) */
#define FP_RM_TOPINF  (2) /* Round to Plus infinity  (Ceil) */
#define FP_RM_TOMINF  (3) /* Round to Minus infinity (Floor) */
#define GETRM()       (int)((FCSR >> FP_SH_RM) & FP_MASK_RM)



/* Integer ALU operations: */

#include "sim-alu.h"

#define ALU32_END(ANS) \
  if (ALU32_HAD_OVERFLOW) \
    SignalExceptionIntegerOverflow (); \
  (ANS) = ALU32_OVERFLOW_RESULT


#define ALU64_END(ANS) \
  if (ALU64_HAD_OVERFLOW) \
    SignalExceptionIntegerOverflow (); \
  (ANS) = ALU64_OVERFLOW_RESULT;

/* start-sanitize-r5900 */

#define BYTES_IN_MMI_REGS       (sizeof(signed_word) + sizeof(signed_word))
#define HALFWORDS_IN_MMI_REGS   (BYTES_IN_MMI_REGS/2)
#define WORDS_IN_MMI_REGS       (BYTES_IN_MMI_REGS/4)
#define DOUBLEWORDS_IN_MMI_REGS (BYTES_IN_MMI_REGS/8)

#define BYTES_IN_MIPS_REGS       (sizeof(signed_word))
#define HALFWORDS_IN_MIPS_REGS   (BYTES_IN_MIPS_REGS/2)
#define WORDS_IN_MIPS_REGS       (BYTES_IN_MIPS_REGS/4)
#define DOUBLEWORDS_IN_MIPS_REGS (BYTES_IN_MIPS_REGS/8)

/* SUB_REG_FETCH - return as lvalue some sub-part of a "register"
   T  - type of the sub part
   TC - # of T's in the mips part of the "register"
   I  - index (from 0) of desired sub part
   A  - low part of "register"
   A1 - high part of register
*/
#define SUB_REG_FETCH(T,TC,A,A1,I) \
(*(((I) < (TC) ? (T*)(A) : (T*)(A1)) \
   + (CURRENT_HOST_BYTE_ORDER == BIG_ENDIAN \
      ? ((TC) - 1 - (I) % (TC)) \
      : ((I) % (TC)) \
      ) \
   ) \
 )

/* 
GPR_<type>(R,I) - return, as lvalue, the I'th <type> of general register R 
            where <type> has two letters:
                  1 is S=signed or U=unsigned
                  2 is B=byte H=halfword W=word D=doubleword 
*/

#define SUB_REG_SB(A,A1,I) SUB_REG_FETCH(signed8,  BYTES_IN_MIPS_REGS,       A, A1, I)
#define SUB_REG_SH(A,A1,I) SUB_REG_FETCH(signed16, HALFWORDS_IN_MIPS_REGS,   A, A1, I)
#define SUB_REG_SW(A,A1,I) SUB_REG_FETCH(signed32, WORDS_IN_MIPS_REGS,       A, A1, I)
#define SUB_REG_SD(A,A1,I) SUB_REG_FETCH(signed64, DOUBLEWORDS_IN_MIPS_REGS, A, A1, I)

#define SUB_REG_UB(A,A1,I) SUB_REG_FETCH(unsigned8,  BYTES_IN_MIPS_REGS,       A, A1, I)
#define SUB_REG_UH(A,A1,I) SUB_REG_FETCH(unsigned16, HALFWORDS_IN_MIPS_REGS,   A, A1, I)
#define SUB_REG_UW(A,A1,I) SUB_REG_FETCH(unsigned32, WORDS_IN_MIPS_REGS,       A, A1, I)
#define SUB_REG_UD(A,A1,I) SUB_REG_FETCH(unsigned64, DOUBLEWORDS_IN_MIPS_REGS, A, A1, I)
  
#define GPR_SB(R,I) SUB_REG_SB(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_SH(R,I) SUB_REG_SH(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_SW(R,I) SUB_REG_SW(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_SD(R,I) SUB_REG_SD(&REGISTERS[R], &REGISTERS1[R], I)

#define GPR_UB(R,I) SUB_REG_UB(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_UH(R,I) SUB_REG_UH(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_UW(R,I) SUB_REG_UW(&REGISTERS[R], &REGISTERS1[R], I)
#define GPR_UD(R,I) SUB_REG_UD(&REGISTERS[R], &REGISTERS1[R], I)


#define RS_SB(I) SUB_REG_SB(&rs_reg, &rs_reg1, I)
#define RS_SH(I) SUB_REG_SH(&rs_reg, &rs_reg1, I)
#define RS_SW(I) SUB_REG_SW(&rs_reg, &rs_reg1, I)
#define RS_SD(I) SUB_REG_SD(&rs_reg, &rs_reg1, I)

#define RS_UB(I) SUB_REG_UB(&rs_reg, &rs_reg1, I)
#define RS_UH(I) SUB_REG_UH(&rs_reg, &rs_reg1, I)
#define RS_UW(I) SUB_REG_UW(&rs_reg, &rs_reg1, I)
#define RS_UD(I) SUB_REG_UD(&rs_reg, &rs_reg1, I)

#define RT_SB(I) SUB_REG_SB(&rt_reg, &rt_reg1, I)
#define RT_SH(I) SUB_REG_SH(&rt_reg, &rt_reg1, I)
#define RT_SW(I) SUB_REG_SW(&rt_reg, &rt_reg1, I)
#define RT_SD(I) SUB_REG_SD(&rt_reg, &rt_reg1, I)

#define RT_UB(I) SUB_REG_UB(&rt_reg, &rt_reg1, I)
#define RT_UH(I) SUB_REG_UH(&rt_reg, &rt_reg1, I)
#define RT_UW(I) SUB_REG_UW(&rt_reg, &rt_reg1, I)
#define RT_UD(I) SUB_REG_UD(&rt_reg, &rt_reg1, I)



#define LO_SB(I) SUB_REG_SB(&LO, &LO1, I)
#define LO_SH(I) SUB_REG_SH(&LO, &LO1, I)
#define LO_SW(I) SUB_REG_SW(&LO, &LO1, I)
#define LO_SD(I) SUB_REG_SD(&LO, &LO1, I)

#define LO_UB(I) SUB_REG_UB(&LO, &LO1, I)
#define LO_UH(I) SUB_REG_UH(&LO, &LO1, I)
#define LO_UW(I) SUB_REG_UW(&LO, &LO1, I)
#define LO_UD(I) SUB_REG_UD(&LO, &LO1, I)

#define HI_SB(I) SUB_REG_SB(&HI, &HI1, I)
#define HI_SH(I) SUB_REG_SH(&HI, &HI1, I)
#define HI_SW(I) SUB_REG_SW(&HI, &HI1, I)
#define HI_SD(I) SUB_REG_SD(&HI, &HI1, I)

#define HI_UB(I) SUB_REG_UB(&HI, &HI1, I)
#define HI_UH(I) SUB_REG_UH(&HI, &HI1, I)
#define HI_UW(I) SUB_REG_UW(&HI, &HI1, I)
#define HI_UD(I) SUB_REG_UD(&HI, &HI1, I)

/* end-sanitize-r5900 */




struct _sim_cpu {


  /* The following are internal simulator state variables: */
  sim_cia cia;
#define CPU_CIA(CPU) (PC)
  address_word ipc; /* internal Instruction PC */
  address_word dspc;  /* delay-slot PC */
#define IPC ((STATE_CPU (sd,0))->ipc)
#define DSPC ((STATE_CPU (sd,0))->dspc)

  /* Issue a delay slot instruction immediatly by re-calling
     idecode_issue */
#define DELAY_SLOT(TARGET) \
  do { \
    address_word target = (TARGET); \
    instruction_word delay_insn; \
    sim_events_slip (sd, 1); \
    PC = CIA + 4; \
    STATE |= simDELAYSLOT; \
    delay_insn = IMEM (PC); \
    idecode_issue (sd, delay_insn, (PC)); \
    STATE &= !simDELAYSLOT; \
    PC = target; \
  } while (0)
#define NULLIFY_NEXT_INSTRUCTION() \
  do { \
    sim_events_slip (sd, 1); \
    NIA = CIA + 4; \
  } while (0)



  /* State of the simulator */
  unsigned int state;
  unsigned int dsstate;
#define STATE ((STATE_CPU (sd,0))->state)
#define DSSTATE ((STATE_CPU (sd,0))->dsstate)

/* Flags in the "state" variable: */
#define simHALTEX       (1 << 2)  /* 0 = run; 1 = halt on exception */
#define simHALTIN       (1 << 3)  /* 0 = run; 1 = halt on interrupt */
#define simTRACE        (1 << 8)  /* 0 = do nothing; 1 = trace address activity */
#define simPCOC0        (1 << 17) /* COC[1] from current */
#define simPCOC1        (1 << 18) /* COC[1] from previous */
#define simDELAYSLOT    (1 << 24) /* 0 = do nothing; 1 = delay slot entry exists */
#define simSKIPNEXT     (1 << 25) /* 0 = do nothing; 1 = skip instruction */
#define simSIGINT	(1 << 28)  /* 0 = do nothing; 1 = SIGINT has occured */
#define simJALDELAYSLOT	(1 << 29) /* 1 = in jal delay slot */




/* This is nasty, since we have to rely on matching the register
   numbers used by GDB. Unfortunately, depending on the MIPS target
   GDB uses different register numbers. We cannot just include the
   relevant "gdb/tm.h" link, since GDB may not be configured before
   the sim world, and also the GDB header file requires too much other
   state. */

#ifndef TM_MIPS_H
#define LAST_EMBED_REGNUM (89)
#define NUM_REGS (LAST_EMBED_REGNUM + 1)
/* start-sanitize-r5900 */
#undef NUM_REGS
#define NUM_REGS (128)
/* end-sanitize-r5900 */
#endif

/* To keep this default simulator simple, and fast, we use a direct
   vector of registers. The internal simulator engine then uses
   manifests to access the correct slot. */

  unsigned_word registers[LAST_EMBED_REGNUM + 1];
  int register_widths[NUM_REGS];
#define REGISTERS       ((STATE_CPU (sd,0))->registers)

#define GPR     (&REGISTERS[0])
#define FGRIDX  (38)
#define FGR     (&REGISTERS[FGRIDX])
#define LO      (REGISTERS[33])
#define HI      (REGISTERS[34])
#define PC      (REGISTERS[37])
#define CAUSE   (REGISTERS[36])
#define SRIDX   (32)
#define SR      (REGISTERS[SRIDX])      /* CPU status register */
#define FCR0IDX  (71)
#define FCR0    (REGISTERS[FCR0IDX])    /* really a 32bit register */
#define FCR31IDX (70)
#define FCR31   (REGISTERS[FCR31IDX])   /* really a 32bit register */
#define FCSR    (FCR31)
#define Debug	(REGISTERS[86])
#define DEPC	(REGISTERS[87])
#define EPC	(REGISTERS[88])
#define COCIDX  (LAST_EMBED_REGNUM + 2) /* special case : outside the normal range */

/* The following are pseudonyms for standard registers */
#define ZERO    (REGISTERS[0])
#define V0      (REGISTERS[2])
#define A0      (REGISTERS[4])
#define A1      (REGISTERS[5])
#define A2      (REGISTERS[6])
#define A3      (REGISTERS[7])
#define SP      (REGISTERS[29])
#define RA      (REGISTERS[31])

  /* Keep the current format state for each register: */
  FP_formats fpr_state[32];
#define FPR_STATE ((STATE_CPU (sd, 0))->fpr_state)


  /* Slots for delayed register updates. For the moment we just have a
     fixed number of slots (rather than a more generic, dynamic
     system). This keeps the simulator fast. However, we only allow
     for the register update to be delayed for a single instruction
     cycle. */
#define PSLOTS (5) /* Maximum number of instruction cycles */
  int pending_in;
  int pending_out;
  int pending_total;
  int pending_slot_count[PSLOTS];
  int pending_slot_reg[PSLOTS];
  unsigned_word pending_slot_value[PSLOTS];
#define PENDING_IN ((STATE_CPU (sd, 0))->pending_in)
#define PENDING_OUT ((STATE_CPU (sd, 0))->pending_out)
#define PENDING_TOTAL ((STATE_CPU (sd, 0))->pending_total)
#define PENDING_SLOT_COUNT ((STATE_CPU (sd, 0))->pending_slot_count)
#define PENDING_SLOT_REG ((STATE_CPU (sd, 0))->pending_slot_reg)
#define PENDING_SLOT_VALUE ((STATE_CPU (sd, 0))->pending_slot_value)

  /* The following are not used for MIPS IV onwards: */
#define PENDING_FILL(r,v) {\
/* printf("DBG: FILL BEFORE pending_in = %d, pending_out = %d, pending_total = %d\n",PENDING_IN,PENDING_OUT,PENDING_TOTAL); */\
                            if (PENDING_SLOT_REG[PENDING_IN] != (LAST_EMBED_REGNUM + 1))\
                             sim_io_eprintf(sd,"Attempt to over-write pending value\n");\
                            PENDING_SLOT_COUNT[PENDING_IN] = 2;\
                            PENDING_SLOT_REG[PENDING_IN] = (r);\
                            PENDING_SLOT_VALUE[PENDING_IN] = (uword64)(v);\
/*printf("DBG: FILL        reg %d value = 0x%s\n",(r),pr_addr(v));*/\
                            PENDING_TOTAL++;\
                            PENDING_IN++;\
                            if (PENDING_IN == PSLOTS)\
                             PENDING_IN = 0;\
/*printf("DBG: FILL AFTER  pending_in = %d, pending_out = %d, pending_total = %d\n",PENDING_IN,PENDING_OUT,PENDING_TOTAL);*/\
                          }


  /* LLBIT = Load-Linked bit. A bit of "virtual" state used by atomic
     read-write instructions. It is set when a linked load occurs. It
     is tested and cleared by the conditional store. It is cleared
     (during other CPU operations) when a store to the location would
     no longer be atomic. In particular, it is cleared by exception
     return instructions. */
  int llbit;
#define LLBIT ((STATE_CPU (sd, 0))->llbit)


/* The HIACCESS and LOACCESS counts are used to ensure that
   corruptions caused by using the HI or LO register to close to a
   following operation are spotted. */

  int hiaccess;
  int loaccess;
#define HIACCESS ((STATE_CPU (sd, 0))->hiaccess)
#define LOACCESS ((STATE_CPU (sd, 0))->loaccess)
  /* start-sanitize-r5900 */
  int hi1access;
  int lo1access;
#define HI1ACCESS ((STATE_CPU (sd, 0))->hi1access)
#define LO1ACCESS ((STATE_CPU (sd, 0))->lo1access)
  /* end-sanitize-r5900 */
#if 1
  /* The 4300 and a few other processors have interlocks on hi/lo
     register reads, and hence do not have this problem.  To avoid
     spurious warnings, we just disable this always.  */
#define CHECKHILO(s)
#else
  unsigned_word HLPC;
  /* If either of the preceding two instructions have accessed the HI
     or LO registers, then the values they see should be
     undefined. However, to keep the simulator world simple, we just
     let them use the value read and raise a warning to notify the
     user: */
#define CHECKHILO(s) {\
  if ((HIACCESS != 0) || (LOACCESS != 0)) \
    sim_io_eprintf(sd,"%s over-writing HI and LO registers values (PC = 0x%s HLPC = 0x%s)\n",(s),pr_addr(PC),pr_addr(HLPC));\
}
  /* start-sanitize-r5900 */
#undef CHECKHILO
#define CHECKHILO(s) {\
  if ((HIACCESS != 0) || (LOACCESS != 0) || (HI1ACCESS != 0) || (LO1ACCESS != 0))\
    sim_io_eprintf(sd,"%s over-writing HI and LO registers values (PC = 0x%s HLPC = 0x%s)\n",(s),pr_addr(PC),pr_addr(HLPC));\
}
  /* end-sanitize-r5900 */
#endif


  /* start-sanitize-r5900 */
  /* The R5900 has 128 bit registers, but the hi 64 bits are only
     touched by multimedia (MMI) instructions.  The normal mips
     instructions just use the lower 64 bits.  To avoid changing the
     older parts of the simulator to handle this weirdness, the high
     64 bits of each register are kept in a separate array
     (registers1).  The high 64 bits of any register are by convention
     refered by adding a '1' to the end of the normal register's name.
     So LO still refers to the low 64 bits of the LO register, LO1
     refers to the high 64 bits of that same register.  */

  signed_word registers1[LAST_EMBED_REGNUM + 1];
#define REGISTERS1 ((STATE_CPU (sd, 0))->registers1)
#define GPR1     (&REGISTERS1[0])
#define LO1      (REGISTERS1[32])
#define HI1      (REGISTERS1[33])
#define REGISTER_SA	(124)

  unsigned_word sa;        /* the shift amount register */
#define SA ((STATE_CPU (sd, 0))->sa)

  /* end-sanitize-r5900 */
  /* start-sanitize-vr5400 */

  /* end-sanitize-vr5400 */



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



/* Status information: */

/* TODO : these should be the bitmasks for these bits within the
   status register. At the moment the following are VR4300
   bit-positions: */
#define status_KSU_mask  (0x3)          /* mask for KSU bits */
#define status_KSU_shift (3)            /* shift for field */
#define ksu_kernel       (0x0)
#define ksu_supervisor   (0x1)
#define ksu_user         (0x2)
#define ksu_unknown      (0x3)

#define status_IE	 (1 <<  0)      /* Interrupt enable */
#define status_EXL	 (1 <<  1)	/* Exception level */
#define status_RE        (1 << 25)      /* Reverse Endian in user mode */
#define status_FR        (1 << 26)      /* enables MIPS III additional FP registers */
#define status_SR        (1 << 20)      /* soft reset or NMI */
#define status_BEV       (1 << 22)      /* Location of general exception vectors */
#define status_TS        (1 << 21)      /* TLB shutdown has occurred */
#define status_ERL       (1 <<  2)      /* Error level */
#define status_RP        (1 << 27)      /* Reduced Power mode */

#define cause_BD        ((unsigned)1 << 31)     /* Exception in branch delay slot */

/* NOTE: We keep the following status flags as bit values (1 for true,
   0 for false). This allows them to be used in binary boolean
   operations without worrying about what exactly the non-zero true
   value is. */

/* UserMode */
#define UserMode        ((((SR & status_KSU_mask) >> status_KSU_shift) == ksu_user) ? 1 : 0)

/* BigEndianMem */
/* Hardware configuration. Affects endianness of LoadMemory and
   StoreMemory and the endianness of Kernel and Supervisor mode
   execution. The value is 0 for little-endian; 1 for big-endian. */
#define BigEndianMem    (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN)
/*(state & simBE) ? 1 : 0)*/

/* ByteSwapMem */
/* This is true if the host and target have different endianness.  */
#define ByteSwapMem (CURRENT_TARGET_BYTE_ORDER != CURRENT_HOST_BYTE_ORDER)

/* ReverseEndian */
/* This mode is selected if in User mode with the RE bit being set in
   SR (Status Register). It reverses the endianness of load and store
   instructions. */
#define ReverseEndian   (((SR & status_RE) && UserMode) ? 1 : 0)

/* BigEndianCPU */
/* The endianness for load and store instructions (0=little;1=big). In
   User mode this endianness may be switched by setting the state_RE
   bit in the SR register. Thus, BigEndianCPU may be computed as
   (BigEndianMem EOR ReverseEndian). */
#define BigEndianCPU    (BigEndianMem ^ ReverseEndian) /* Already bits */



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


/* Co-processor accesses */

void cop_lw  PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg, unsigned int memword));
void cop_ld  PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg, uword64 memword));
unsigned int cop_sw PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg));
uword64 cop_sd PARAMS ((SIM_DESC sd, int coproc_num, int coproc_reg));

#define COP_LW(coproc_num,coproc_reg,memword) cop_lw(sd,coproc_num,coproc_reg,memword)
#define COP_LD(coproc_num,coproc_reg,memword) cop_ld(sd,coproc_num,coproc_reg,memword)
#define COP_SW(coproc_num,coproc_reg) cop_sw(sd,coproc_num,coproc_reg)
#define COP_SD(coproc_num,coproc_reg) cop_sd(sd,coproc_num,coproc_reg)

void decode_coproc PARAMS ((SIM_DESC sd,unsigned int instruction));
#define DecodeCoproc(instruction) decode_coproc(sd, (instruction))



/* Memory accesses */

/* The following are generic to all versions of the MIPS architecture
   to date: */

/* Memory Access Types (for CCA): */
#define Uncached                (0)
#define CachedNoncoherent       (1)
#define CachedCoherent          (2)
#define Cached                  (3)

#define isINSTRUCTION   (1 == 0) /* FALSE */
#define isDATA          (1 == 1) /* TRUE */
#define isLOAD          (1 == 0) /* FALSE */
#define isSTORE         (1 == 1) /* TRUE */
#define isREAL          (1 == 0) /* FALSE */
#define isRAW           (1 == 1) /* TRUE */
/* The parameter HOST (isTARGET / isHOST) is ignored */
#define isTARGET        (1 == 0) /* FALSE */
/* #define isHOST          (1 == 1) TRUE */

/* The "AccessLength" specifications for Loads and Stores. NOTE: This
   is the number of bytes minus 1. */
#define AccessLength_BYTE       (0)
#define AccessLength_HALFWORD   (1)
#define AccessLength_TRIPLEBYTE (2)
#define AccessLength_WORD       (3)
#define AccessLength_QUINTIBYTE (4)
#define AccessLength_SEXTIBYTE  (5)
#define AccessLength_SEPTIBYTE  (6)
#define AccessLength_DOUBLEWORD (7)
#define AccessLength_QUADWORD   (15)

int address_translation PARAMS ((SIM_DESC sd, address_word vAddr, int IorD, int LorS, address_word *pAddr, int *CCA, int raw));
#define AddressTranslation(vAddr,IorD,LorS,pAddr,CCA,host,raw) \
address_translation(sd, vAddr,IorD,LorS,pAddr,CCA,raw)

void load_memory PARAMS ((SIM_DESC sd, uword64* memvalp, uword64* memval1p, int CCA, int AccessLength, address_word pAddr, address_word vAddr, int IorD, int raw));
#define LoadMemory(memvalp,memval1p,CCA,AccessLength,pAddr,vAddr,IorD,raw) \
load_memory(sd,memvalp,memval1p,CCA,AccessLength,pAddr,vAddr,IorD,raw)

void store_memory PARAMS ((SIM_DESC sd, int CCA, int AccessLength, uword64 MemElem,     uword64 MemElem1, address_word pAddr, address_word vAddr, int raw));
#define StoreMemory(CCA,AccessLength,MemElem,MemElem1,pAddr,vAddr,raw) \
store_memory(sd,CCA,AccessLength,MemElem,MemElem1,pAddr,vAddr,raw)

void cache_op PARAMS ((SIM_DESC sd, int op, address_word pAddr, address_word vAddr, unsigned int instruction));
#define CacheOp(op,pAddr,vAddr,instruction) cache_op(sd,op,pAddr,vAddr,instruction)

void sync_operation PARAMS ((SIM_DESC sd, int stype));
#define SyncOperation(stype) sync_operation (sd, (stype))

void prefetch PARAMS ((SIM_DESC sd, int CCA, address_word pAddr, address_word vAddr, int DATA, int hint));
#define Prefetch(CCA,pAddr,vAddr,DATA,hint) prefetch(sd,CCA,pAddr,vAddr,DATA,hint)

unsigned32 ifetch32 PARAMS ((SIM_DESC sd, address_word cia));
#define IMEM(CIA) ifetch32 (SD, (CIA))


#endif
