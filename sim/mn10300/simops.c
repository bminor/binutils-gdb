#include "config.h"

#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "mn10300_sim.h"
#include "simops.h"
#include "sys/syscall.h"
#include "bfd.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>

enum op_types {
  OP_UNKNOWN,
};

#ifdef DEBUG
static void trace_input PARAMS ((char *name, enum op_types type, int size));
static void trace_output PARAMS ((enum op_types result));
static int init_text_p = 0;
static asection *text;
static bfd_vma text_start;
static bfd_vma text_end;
extern bfd *exec_bfd;

#ifndef SIZE_INSTRUCTION
#define SIZE_INSTRUCTION 6
#endif

#ifndef SIZE_OPERANDS
#define SIZE_OPERANDS 16
#endif

#ifndef SIZE_VALUES
#define SIZE_VALUES 11
#endif

#ifndef SIZE_LOCATION
#define SIZE_LOCATION 40
#endif

static void
trace_input (name, type, size)
     char *name;
     enum op_types type;
     int size;
{
}

static void
trace_output (result)
     enum op_types result;
{
}

#else
#define trace_input(NAME, IN1, IN2)
#define trace_output(RESULT)
#endif


/* mov */
void OP_8000 ()
{
}

/* mov */
void OP_80 ()
{
}

/* mov */
void OP_F1E0 ()
{
}

/* mov */
void OP_F1D0 ()
{
}

/* mov */
void OP_9000 ()
{
}

/* mov */
void OP_90 ()
{
}

/* mov sp, an*/
void OP_3C ()
{
  State.regs[REG_A0 + (insn & 0x3)] = State.regs[REG_SP];
}

/* mov am, sp*/
void OP_F2F0 ()
{
  State.regs[REG_SP] = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
}

/* mov */
void OP_F2E4 ()
{
}

/* mov */
void OP_F2F3 ()
{
}

/* mov */
void OP_F2E0 ()
{
}

/* mov */
void OP_F2F2 ()
{
}

/* mov */
void OP_70 ()
{
}

/* mov */
void OP_F80000 ()
{
}

/* mov */
void OP_FA000000 ()
{
}

/* mov */
void OP_FC000000 ()
{
}

/* mov */
void OP_5800 ()
{
}

/* mov */
void OP_FAB40000 ()
{
}

/* mov */
void OP_FCB40000 ()
{
}

/* mov */
void OP_F300 ()
{
}

/* mov */
void OP_300000 ()
{
}

/* mov */
void OP_FCA40000 ()
{
}

/* mov */
void OP_F000 ()
{
}

/* mov */
void OP_F82000 ()
{
}

/* mov */
void OP_FA200000 ()
{
}

/* mov */
void OP_FC200000 ()
{
}

/* mov */
void OP_5C00 ()
{
}

/* mov */
void OP_FAB00000 ()
{
}

/* mov */
void OP_FCB00000 ()
{
}

/* mov */
void OP_F380 ()
{
}

/* mov */
void OP_FAA00000 ()
{
}

/* mov */
void OP_FCA00000 ()
{
}

/* mov */
void OP_F8F000 ()
{
}

/* mov */
void OP_60 ()
{
}

/* mov */
void OP_F81000 ()
{
}

/* mov */
void OP_FA100000 ()
{
}

/* mov */
void OP_FC100000 ()
{
}

/* mov */
void OP_4200 ()
{
}

/* mov */
void OP_FA910000 ()
{
}

/* mov */
void OP_FC910000 ()
{
}

/* mov */
void OP_F340 ()
{
}

/* mov */
void OP_10000 ()
{
}

/* mov */
void OP_FC810000 ()
{
}

/* mov */
void OP_F010 ()
{
}

/* mov */
void OP_F83000 ()
{
}

/* mov */
void OP_FA300000 ()
{
}

/* mov */
void OP_FC300000 ()
{
}

/* mov */
void OP_4300 ()
{
}

/* mov */
void OP_FA900000 ()
{
}

/* mov */
void OP_FC900000 ()
{
}

/* mov */
void OP_F3C0 ()
{
}

/* mov */
void OP_FA800000 ()
{
}

/* mov */
void OP_FC800000 ()
{
}

/* mov */
void OP_F8F400 ()
{
}

/* mov */
void OP_2C0000 ()
{
}

/* mov */
void OP_FCCC0000 ()
{
}

/* mov */
void OP_240000 ()
{
}

/* mov imm32, an*/
void OP_FCDC0000 ()
{
  unsigned long value;

  value = (insn & 0xffff) << 16 | extension;
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;
}

/* movbu */
void OP_F040 ()
{
}

/* movbu */
void OP_F84000 ()
{
}

/* movbu */
void OP_FA400000 ()
{
}

/* movbu */
void OP_FC400000 ()
{
}

/* movbu */
void OP_F8B800 ()
{
}

/* movbu */
void OP_FAB80000 ()
{
}

/* movbu */
void OP_FCB80000 ()
{
}

/* movbu */
void OP_F400 ()
{
}

/* movbu */
void OP_340000 ()
{
}

/* movbu */
void OP_FCA80000 ()
{
}

/* movbu dm,(an) */
void OP_F050 ()
{
  store_mem (State.regs[REG_A0 + ((insn & 0xc) >> 2)], 1,
	     State.regs[REG_D0 + (insn & 0x3)]);
}

/* movbu */
void OP_F85000 ()
{
}

/* movbu */
void OP_FA500000 ()
{
}

/* movbu */
void OP_FC500000 ()
{
}

/* movbu */
void OP_F89200 ()
{
}

/* movbu */
void OP_FA920000 ()
{
}

/* movbu */
void OP_FC920000 ()
{
}

/* movbu */
void OP_F440 ()
{
}

/* movbu */
void OP_20000 ()
{
}

/* movbu */
void OP_FC820000 ()
{
}

/* movhu */
void OP_F060 ()
{
}

/* movhu */
void OP_F86000 ()
{
}

/* movhu */
void OP_FA600000 ()
{
}

/* movhu */
void OP_FC600000 ()
{
}

/* movhu */
void OP_F8BC00 ()
{
}

/* movhu */
void OP_FABC0000 ()
{
}

/* movhu */
void OP_FCBC0000 ()
{
}

/* movhu */
void OP_F480 ()
{
}

/* movhu */
void OP_380000 ()
{
}

/* movhu */
void OP_FCAC0000 ()
{
}

/* movhu */
void OP_F070 ()
{
}

/* movhu */
void OP_F87000 ()
{
}

/* movhu */
void OP_FA700000 ()
{
}

/* movhu */
void OP_FC700000 ()
{
}

/* movhu */
void OP_F89300 ()
{
}

/* movhu */
void OP_FA930000 ()
{
}

/* movhu */
void OP_FC930000 ()
{
}

/* movhu */
void OP_F4C0 ()
{
}

/* movhu */
void OP_30000 ()
{
}

/* movhu */
void OP_FC830000 ()
{
}

/* ext */
void OP_F2D0 ()
{
}

/* extb */
void OP_10 ()
{
}

/* extbu */
void OP_14 ()
{
}

/* exth */
void OP_18 ()
{
}

/* exthu */
void OP_1C ()
{
}

/* movm */
void OP_CE00 ()
{
}

/* movm */
void OP_CF00 ()
{
}

/* clr dn */
void OP_0 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)] = 0;

  PSW |= PSW_Z;
  PSW &= ~(PSW_V | PSW_C | PSW_N);
}

/* add dm,dn*/
void OP_E0 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg1 + reg2;
  State.regs[REG_D0 + (insn & 0x3)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add dm, an */
void OP_F160 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg1 + reg2;
  State.regs[REG_A0 + (insn & 0x3)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add am, dn*/
void OP_F150 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg1 + reg2;
  State.regs[REG_D0 + (insn & 0x3)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add am,an */
void OP_F170 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg1 + reg2;
  State.regs[REG_A0 + (insn & 0x3)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, dn */
void OP_2800 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc00) >> 8)];
  imm = SEXT8 (insn & 0xff);
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0xc00) >> 8)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm16, dn */
void OP_FAC00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc0000) >> 16)];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0xc0000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm32,dn */
void OP_FCC00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc0000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0xc0000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, an */
void OP_2000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc00) >> 8)];
  imm = insn & 0xff;
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0xc00) >> 8)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm16, an */
void OP_FAD00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc0000) >> 16)];
  imm = 0xffff;
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0xc0000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm32, an */
void OP_FCD00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc0000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0xc0000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, sp*/
void OP_F8FE00 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_SP];
  imm = SEXT8 (insn & 0xff);
  value = reg1 + imm;
  State.regs[REG_SP] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm16,sp */
void OP_FAFE0000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_SP];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 + imm;
  State.regs[REG_SP] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add */
void OP_FCFE0000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_SP];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 + imm;
  State.regs[REG_SP] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* addc */
void OP_F140 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg1 + reg2 + ((PSW & PSW_C) != 0);
  State.regs[REG_D0 + (insn & 0x3)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* sub */
void OP_F100 ()
{
}

/* sub */
void OP_F120 ()
{
}

/* sub */
void OP_F110 ()
{
}

/* sub */
void OP_F130 ()
{
}

/* sub */
void OP_FCC40000 ()
{
}

/* sub */
void OP_FCD40000 ()
{
}

/* subc */
void OP_F180 ()
{
}

/* mul */
void OP_F240 ()
{
}

/* mulu */
void OP_F250 ()
{
}

/* div */
void OP_F260 ()
{
}

/* divu */
void OP_F270 ()
{
}

/* inc dn */
void OP_40 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)] += 1;
}

/* inc an */
void OP_41 ()
{
  State.regs[REG_A0 + ((insn & 0xc) >> 2)] += 1;
}

/* inc4 an */
void OP_50 ()
{
  State.regs[REG_A0 + (insn & 0x3)] += 4;
}

/* cmp imm8, dn */
void OP_A000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x300) >> 8)];
  imm = SEXT8 (insn & 0xff);
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp dm, dn */
void OP_A0 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg1 - reg2;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp dm, an */
void OP_F1A0 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg1 - reg2;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp am, dn */
void OP_F190 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg1 - reg2;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp imm8, an */
void OP_B000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x300) >> 8)];
  imm = insn & 0xff;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp am,an */
void OP_B0 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg1 - reg2;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < reg2);
  v = ((reg2 & 0x8000000) != (reg1 & 0x80000000)
       && (reg2 & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp */
void OP_FAC80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x300) >> 16)];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp */
void OP_FCC80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x300) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp */
void OP_FAD80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x300) >> 16)];
  imm = insn & 0xffff;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp */
void OP_FCD80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x300) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((imm & 0x8000000) != (reg1 & 0x80000000)
       && (imm & 0x8000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* and */
void OP_F200 ()
{
}

/* and */
void OP_F8E000 ()
{
}

/* and */
void OP_FAE00000 ()
{
}

/* and */
void OP_FCE00000 ()
{
}

/* and */
void OP_FAFC0000 ()
{
}

/* or */
void OP_F210 ()
{
}

/* or */
void OP_F8E400 ()
{
}

/* or */
void OP_FAE40000 ()
{
}

/* or */
void OP_FCE40000 ()
{
}

/* or */
void OP_FAFD0000 ()
{
}

/* xor */
void OP_F220 ()
{
}

/* xor */
void OP_FAE80000 ()
{
}

/* xor */
void OP_FCE80000 ()
{
}

/* not */
void OP_F230 ()
{
}

/* btst */
void OP_F8EC00 ()
{
}

/* btst */
void OP_FAEC0000 ()
{
}

/* btst */
void OP_FCEC0000 ()
{
}

/* btst */
void OP_FE020000 ()
{
}

/* btst */
void OP_FAF80000 ()
{
}

/* btst */
void OP_F080 ()
{
}

/* btst */
void OP_FE000000 ()
{
}

/* btst */
void OP_FAF00000 ()
{
}

/* bclr */
void OP_F090 ()
{
}

/* bclr */
void OP_FE010000 ()
{
}

/* bclr */
void OP_FAF40000 ()
{
}

/* asr */
void OP_F2B0 ()
{
}

/* asr */
void OP_F8C800 ()
{
}

/* lsr */
void OP_F2A0 ()
{
}

/* lsr */
void OP_F8C400 ()
{
}

/* asl */
void OP_F290 ()
{
}

/* asl */
void OP_F8C000 ()
{
}

/* asl2 */
void OP_54 ()
{
}

/* ror */
void OP_F284 ()
{
}

/* rol */
void OP_F280 ()
{
}

/* beq */
void OP_C800 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (PSW & PSW_Z)
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bne */
void OP_C900 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!(PSW & PSW_Z))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bgt */
void OP_C100 ()
{
}

/* bge */
void OP_C200 ()
{
}

/* ble */
void OP_C300 ()
{
}

/* blt */
void OP_C000 ()
{
}

/* bhi */
void OP_C500 ()
{
}

/* bcc */
void OP_C600 ()
{
}

/* bls */
void OP_C700 ()
{
}

/* bcs */
void OP_C400 ()
{
}

/* bvc */
void OP_F8E800 ()
{
}

/* bvs */
void OP_F8E900 ()
{
}

/* bnc */
void OP_F8EA00 ()
{
}

/* bns */
void OP_F8EB00 ()
{
}

/* bra */
void OP_CA00 ()
{
}

/* leq */
void OP_D8 ()
{
}

/* lne */
void OP_D9 ()
{
}

/* lgt */
void OP_D1 ()
{
}

/* lge */
void OP_D2 ()
{
}

/* lle */
void OP_D3 ()
{
}

/* llt */
void OP_D0 ()
{
}

/* lhi */
void OP_D5 ()
{
}

/* lcc */
void OP_D6 ()
{
}

/* lls */
void OP_D7 ()
{
}

/* lcs */
void OP_D4 ()
{
}

/* lra */
void OP_DA ()
{
}

/* setlb */
void OP_DB ()
{
}

/* jmp */
void OP_F0F4 ()
{
  State.pc = State.regs[REG_A0 + (insn & 0x3)] - 2;
}

/* jmp */
void OP_CC0000 ()
{
  State.pc += SEXT16 (insn & 0xffff) - 3;
}

/* jmp */
void OP_DC000000 ()
{
  State.pc += (((insn & 0xffffff) << 8) | extension) - 5;
}

/* call */
void OP_CD000000 ()
{
}

/* call */
void OP_DD000000 ()
{
}

/* calls */
void OP_F0F0 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 2;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = next_pc & 0xff00;
  State.mem[sp+2] = next_pc & 0xff0000;
  State.mem[sp+3] = next_pc & 0xff000000;
  State.regs[REG_MDR] = next_pc;
  State.pc = State.regs[REG_A0 + (insn & 0x3)] - 2;
}

/* calls */
void OP_FAFF0000 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 4;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = next_pc & 0xff00;
  State.mem[sp+2] = next_pc & 0xff0000;
  State.mem[sp+3] = next_pc & 0xff000000;
  State.regs[REG_MDR] = next_pc;
  State.pc += SEXT16 (insn & 0xffff) - 4;
}

/* calls */
void OP_FCFF0000 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 6;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = next_pc & 0xff00;
  State.mem[sp+2] = next_pc & 0xff0000;
  State.mem[sp+3] = next_pc & 0xff000000;
  State.regs[REG_MDR] = next_pc;
  State.pc += (((insn & 0xffff) << 16) | extension) - 6;
}

/* ret */
void OP_DF0000 ()
{
}

/* retf */
void OP_DE0000 ()
{
}

/* rets */
void OP_F0FC ()
{
  unsigned int sp;

  sp = State.regs[REG_SP];
  State.pc = (State.mem[sp] | (State.mem[sp+1] << 8)
	      | (State.mem[sp+2] << 16) | (State.mem[sp+3] << 24));
  State.pc -= 2;
}

/* rti */
void OP_F0FD ()
{
}

/* trap */
void OP_F0FE ()
{
}

/* rtm */
void OP_F0FF ()
{
}

/* nop */
void OP_CB ()
{
}

/* putx */
void OP_F500 ()
{
}

/* getx */
void OP_F6F0 ()
{
}

/* mulq */
void OP_F600 ()
{
}

/* mulq */
void OP_F90000 ()
{
}

/* mulq */
void OP_FB000000 ()
{
}

/* mulq */
void OP_FD000000 ()
{
}

/* mulqu */
void OP_F610 ()
{
}

/* mulqu */
void OP_F91400 ()
{
}

/* mulqu */
void OP_FB140000 ()
{
}

/* mulqu */
void OP_FD140000 ()
{
}

/* sat16 */
void OP_F640 ()
{
}

/* sat24 */
void OP_F650 ()
{
}

/* bsch */
void OP_F670 ()
{
}
