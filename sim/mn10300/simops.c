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


/* mov imm8, dn */
void OP_8000 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)] = SEXT8 (insn & 0xff);
}

/* mov dm, dn */
void OP_80 ()
{
  State.regs[REG_D0 + (insn & 0x3)] = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
}

/* mov dm, an */
void OP_F1E0 ()
{
  State.regs[REG_A0 + (insn & 0x3)] = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
}

/* mov am, dn */
void OP_F1D0 ()
{
  State.regs[REG_D0 + (insn & 0x3)] = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
}

/* mov imm8, an */
void OP_9000 ()
{
  State.regs[REG_A0 + ((insn & 0x300) >> 8)] = insn & 0xff;
}

/* mov am, an */
void OP_90 ()
{
  State.regs[REG_A0 + (insn & 0x3)] = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
}

/* mov sp, an */
void OP_3C ()
{
  State.regs[REG_A0 + (insn & 0x3)] = State.regs[REG_SP];
}

/* mov am, sp */
void OP_F2F0 ()
{
  State.regs[REG_SP] = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
}

/* mov psw, dn */
void OP_F2E4 ()
{
  State.regs[REG_D0 + (insn & 0x3)] = PSW;
}

/* mov dm, psw */
void OP_F2F3 ()
{
  PSW = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
}

/* mov mdr, dn */
void OP_F2E0 ()
{
  State.regs[REG_D0 + (insn & 0x3)] = State.regs[REG_MDR];
}

/* mov dm, mdr */
void OP_F2F2 ()
{
  State.regs[REG_MDR] = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
}

/* mov (am), dn */
void OP_70 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)]
    = load_mem (State.regs[REG_A0 + (insn & 0x3)], 4);
}

/* mov (d8,am), dn */
void OP_F80000 ()
{
  State.regs[REG_D0 + ((insn & 0xc00) >> 10)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
		 + SEXT8 (insn & 0xff)), 4);
}

/* mov (d16,am), dn */
void OP_FA000000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + SEXT16 (insn & 0xffff)), 4);
}

/* mov (d32,am), dn */
void OP_FC000000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + ((insn & 0xffff) << 16) | extension), 4);
}

/* mov (d8,sp), dn */
void OP_5800 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem (State.regs[REG_SP] + (insn & 0xff), 4);
}

/* mov (d16,sp), dn */
void OP_FAB40000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (insn & 0xffff), 4);
}

/* mov (d32,sp), dn */
void OP_FCB40000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 4);
}

/* mov (di,am), dn */
void OP_F300 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_A0 + (insn & 0x3)]
		 + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 4);
}

/* mov (abs16), dn */
void OP_300000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = load_mem ((insn & 0xffff), 4);
}

/* mov (abs32), dn */
void OP_FCA40000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem ((((insn & 0xffff) << 16) + extension), 4);
}

/* mov (am), an */
void OP_F000 ()
{
  State.regs[REG_A0 + ((insn & 0xc) >> 2)]
    = load_mem (State.regs[REG_A0 + (insn & 0x3)], 4);
}

/* mov (d8,am), an */
void OP_F82000 ()
{
  State.regs[REG_A0 + ((insn & 0xc00) >> 10)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
		 + SEXT8 (insn & 0xff)), 4);
}

/* mov (d16,am), an */
void OP_FA200000 ()
{
  State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + SEXT16 (insn & 0xffff)), 4);
}

/* mov (d32,am), an */
void OP_FC200000 ()
{
  State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + ((insn & 0xffff) << 16) + extension), 4);
}

/* mov (d8,sp), an */
void OP_5C00 ()
{
  State.regs[REG_A0 + ((insn & 0x300) >> 8)]
    = load_mem (State.regs[REG_SP] + (insn & 0xff), 4);
}

/* mov (d16,sp), an */
void OP_FAB00000 ()
{
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (insn & 0xffff), 4);
}

/* mov (d32,sp), an */
void OP_FCB00000 ()
{
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 4);
}

/* mov (di,am), an */
void OP_F380 ()
{
  State.regs[REG_A0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_A0 + (insn & 0x3)]
		 + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 4);
}

/* mov (abs16), an */
void OP_FAA00000 ()
{
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = load_mem ((insn & 0xffff), 4);
}

/* mov (abs32), an */
void OP_FCA00000 ()
{
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
    = load_mem ((((insn & 0xffff) << 16) + extension), 4);
}

/* mov (d8,am), sp */
void OP_F8F000 ()
{
  State.regs[REG_SP]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
		 + SEXT8 (insn & 0xff)), 4);
}

/* mov dm, (an) */
void OP_60 ()
{
  store_mem (State.regs[REG_A0 + (insn & 0x3)], 4,
	     State.regs[REG_D0 + ((insn & 0xc) >> 2)]);
}

/* mov dm, (d8,an) */
void OP_F81000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
	      + SEXT8 (insn & 0xff)), 4,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* mov dm (d16,an) */
void OP_FA100000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + SEXT16 (insn & 0xffff)), 4,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov dm (d32,an) */
void OP_FC100000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + ((insn & 0xffff) << 16) + extension), 4,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov dm, (d8,sp) */
void OP_4200 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xff), 4,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* mov dm, (d16,sp) */
void OP_FA910000 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xffff), 4,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov dm, (d32,sp) */
void OP_FC910000 ()
{
  store_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 4,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov dm, (di,an) */
void OP_F340 ()
{
  store_mem ((State.regs[REG_A0 + (insn & 0x3)]
	      + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 4,
	     State.regs[REG_D0 + ((insn & 0x300) >> 8)]);
}

/* mov dm, (abs16) */
void OP_10000 ()
{
  store_mem ((insn & 0xffff), 4, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov dm, (abs32) */
void OP_FC810000 ()
{
  store_mem ((((insn & 0xffff) << 16) + extension), 4, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (an) */
void OP_F010 ()
{
  store_mem (State.regs[REG_A0 + (insn & 0x3)], 4,
	     State.regs[REG_A0 + ((insn & 0xc) >> 2)]);
}

/* mov am, (d8,an) */
void OP_F83000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
	      + SEXT8 (insn & 0xff)), 4,
	     State.regs[REG_A0 + ((insn & 0xc00) >> 10)]);
}

/* mov am, (d16,an) */
void OP_FA300000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 17)]
	      + SEXT16 (insn & 0xffff)), 4,
	     State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (d32,an) */
void OP_FC300000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 17)]
	      + ((insn & 0xffff) << 16) + extension), 4,
	     State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (d8,sp) */
void OP_4300 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xff), 4,
	     State.regs[REG_A0 + ((insn & 0xc00) >> 10)]);
}

/* mov am, (d16,sp) */
void OP_FA900000 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xffff), 4,
	     State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (d32,sp) */
void OP_FC900000 ()
{
  store_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 4,
	     State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (di,an) */
void OP_F3C0 ()
{
  store_mem ((State.regs[REG_A0 + (insn & 0x3)]
	      + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 4,
	     State.regs[REG_A0 + ((insn & 0x300) >> 8)]);
}

/* mov am, (abs16) */
void OP_FA800000 ()
{
  store_mem ((insn & 0xffff), 4, State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov am, (abs32) */
void OP_FC800000 ()
{
  store_mem ((((insn & 0xffff) << 16) + extension), 4, State.regs[REG_A0 + ((insn & 0xc0000) >> 18)]);
}

/* mov sp, (d8,an) */
void OP_F8F400 ()
{
  store_mem (State.regs[REG_A0 + ((insn & 0x300) >> 8)] + SEXT8 (insn & 0xff),
	     4, State.regs[REG_SP]);
}

/* mov imm16, dn */
void OP_2C0000 ()
{
  unsigned long value;

  value = SEXT16 (insn & 0xffff);
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = value;
}

/* mov imm32,dn */
void OP_FCCC0000 ()
{
  unsigned long value;

  value = (insn & 0xffff) << 16 | extension;
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = value;
}

/* mov imm16, an */
void OP_240000 ()
{
  unsigned long value;

  value = insn & 0xffff;
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;
}

/* mov imm32, an */
void OP_FCDC0000 ()
{
  unsigned long value;

  value = (insn & 0xffff) << 16 | extension;
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;
}

/* movbu (am), dn */
void OP_F040 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)]
    = load_mem (State.regs[REG_A0 + (insn & 0x3)], 1);
}

/* movbu (d8,am), dn */
void OP_F84000 ()
{
  State.regs[REG_D0 + ((insn & 0xc00) >> 10)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
		 + SEXT8 (insn & 0xff)), 1);
}

/* movbu (d16,am), dn */
void OP_FA400000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + SEXT16 (insn & 0xffff)), 1);
}

/* movbu (d32,am), dn */
void OP_FC400000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + ((insn & 0xffff) << 16) + extension), 1);
}

/* movbu (d8,sp), dn */
void OP_F8B800 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_SP] + (insn & 0xff)), 1);
}

/* movbu (d16,sp), dn */
void OP_FAB80000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem ((State.regs[REG_SP] + (insn & 0xffff)), 1);
}

/* movbu (d32,sp), dn */
void OP_FCB80000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 1);
}

/* movbu (di,am), dn */
void OP_F400 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_A0 + (insn & 0x3)]
		 + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 1);
}

/* movbu (abs16), dn */
void OP_340000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = load_mem ((insn & 0xffff), 1);
}

/* movbu (abs32), dn */
void OP_FCA80000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem ((((insn & 0xffff) << 16) + extension), 1);
}

/* movbu dm, (an) */
void OP_F050 ()
{
  store_mem (State.regs[REG_A0 + (insn & 0x3)], 1,
	     State.regs[REG_D0 + ((insn & 0xc) >> 2)]);
}

/* movbu dm, (d8,an) */
void OP_F85000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
	      + SEXT8 (insn & 0xff)), 1,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movbu dm, (d16,an) */
void OP_FA500000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + SEXT16 (insn & 0xffff)), 1,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movbu dm, (d32,an) */
void OP_FC500000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + ((insn & 0xffff) << 16) + extension), 1,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movbu dm, (d8,sp) */
void OP_F89200 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xff), 1,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movbu dm, (d16,sp) */
void OP_FA920000 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xffff), 2,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movbu dm (d32,sp) */
void OP_FC920000 ()
{
  store_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 2,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movbu dm, (di,an) */
void OP_F440 ()
{
  store_mem ((State.regs[REG_A0 + (insn & 0x3)]
	      + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 1,
	     State.regs[REG_D0 + ((insn & 0x300) >> 8)]);
}

/* movbu dm, (abs16) */
void OP_20000 ()
{
  store_mem ((insn & 0xffff), 1, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movbu dm, (abs32) */
void OP_FC820000 ()
{
  store_mem ((((insn & 0xffff) << 16) + extension), 1, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu (am), dn */
void OP_F060 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)]
    = load_mem (State.regs[REG_A0 + (insn & 0x3)], 2);
}

/* movhu (d8,am), dn */
void OP_F86000 ()
{
  State.regs[REG_D0 + ((insn & 0xc00) >> 10)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
		 + SEXT8 (insn & 0xff)), 2);
}

/* movhu (d16,am), dn */
void OP_FA600000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + SEXT16 (insn & 0xffff)), 2);
}

/* movhu (d32,am), dn */
void OP_FC600000 ()
{
  State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]
    = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
		 + ((insn & 0xffff) << 16) + extension), 2);
}

/* movhu (d8,sp) dn */
void OP_F8BC00 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_SP] + (insn & 0xff)), 2);
}

/* movhu (d16,sp), dn */
void OP_FABC0000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem ((State.regs[REG_SP] + (insn & 0xffff)), 2);
}

/* movhu (d32,sp), dn */
void OP_FCBC0000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 2);
}

/* movhu (di,am), dn */
void OP_F480 ()
{
  State.regs[REG_D0 + ((insn & 0x300) >> 8)]
    = load_mem ((State.regs[REG_A0 + (insn & 0x3)]
		 + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 2);
}

/* movhu (abs16), dn */
void OP_380000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = load_mem ((insn & 0xffff), 2);
}

/* movhu (abs32), dn */
void OP_FCAC0000 ()
{
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    = load_mem ((((insn & 0xffff) << 16) + extension), 2);
}

/* movhu dm, (an) */
void OP_F070 ()
{
  store_mem (State.regs[REG_A0 + (insn & 0x3)], 2,
	     State.regs[REG_D0 + ((insn & 0xc) >> 2)]);
}

/* movhu dm, (d8,an) */
void OP_F87000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x300) >> 8)]
	      + SEXT8 (insn & 0xff)), 2,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movhu dm, (d16,an) */
void OP_FA700000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + SEXT16 (insn & 0xffff)), 2,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu dm, (d32,an) */
void OP_FC700000 ()
{
  store_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
	      + ((insn & 0xffff) << 16) + extension), 2,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu dm,(d8,sp) */
void OP_F89300 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xff), 2,
	     State.regs[REG_D0 + ((insn & 0xc00) >> 10)]);
}

/* movhu dm,(d16,sp) */
void OP_FA930000 ()
{
  store_mem (State.regs[REG_SP] + (insn & 0xffff), 2,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu dm,(d32,sp) */
void OP_FC930000 ()
{
  store_mem (State.regs[REG_SP] + (((insn & 0xffff) << 16) + extension), 2,
	     State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu dm, (di,an) */
void OP_F4C0 ()
{
  store_mem ((State.regs[REG_A0 + (insn & 0x3)]
	      + State.regs[REG_D0 + ((insn & 0xc) >> 2)]), 2,
	     State.regs[REG_D0 + ((insn & 0x300) >> 8)]);
}

/* movhu dm, (abs16) */
void OP_30000 ()
{
  store_mem ((insn & 0xffff), 2, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* movhu dm, (abs32) */
void OP_FC830000 ()
{
  store_mem ((((insn & 0xffff) << 16) + extension), 2, State.regs[REG_D0 + ((insn & 0xc0000) >> 18)]);
}

/* ext dn */
void OP_F2D0 ()
{
  if (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000)
    State.regs[REG_MDR] = -1;
  else
    State.regs[REG_MDR] = 0;
}

/* extb dn */
void OP_10 ()
{
  State.regs[REG_D0 + (insn & 0x3)] = SEXT8 (State.regs[REG_D0 + (insn & 0x3)]);
}

/* extbu dn */
void OP_14 ()
{
  State.regs[REG_D0 + (insn & 0x3)] &= 0xff;
}

/* exth dn */
void OP_18 ()
{
  State.regs[REG_D0 + (insn & 0x3)]
    = SEXT16 (State.regs[REG_D0 + (insn & 0x3)]);
}

/* exthu dn */
void OP_1C ()
{
  State.regs[REG_D0 + (insn & 0x3)] &= 0xffff;
}

/* movm (sp), reg_list */
void OP_CE00 ()
{
  unsigned long sp = State.regs[REG_SP];
  unsigned long mask;

  mask = insn & 0xff;

  if (mask & 0x8)
    {
      sp += 4;
      State.regs[REG_LAR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_LIR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_MDR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x10)
    {
      State.regs[REG_A0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x20)
    {
      State.regs[REG_A0 + 2] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x40)
    {
      State.regs[REG_D0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x80)
    {
      State.regs[REG_D0 + 2] = load_mem (sp, 4);
      sp += 4;
    }

  /* And make sure to update the stack pointer.  */
  State.regs[REG_SP] = sp;
}

/* movm reg_list, (sp) */
void OP_CF00 ()
{
  unsigned long sp = State.regs[REG_SP];
  unsigned long mask;

  mask = insn & 0xff;

  if (mask & 0x80)
    {
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_D0 + 2]);
    }

  if (mask & 0x40)
    {
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_D0 + 3]);
    }

  if (mask & 0x20)
    {
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_A0 + 2]);
    }

  if (mask & 0x10)
    {
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_A0 + 3]);
    }

  if (mask & 0x8)
    {
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_D0]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_D0 + 1]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_A0]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_A0 + 1]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_MDR]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_LIR]);
      sp -= 4;
      store_mem (sp, 4, State.regs[REG_LAR]);
      sp -= 4;
    }

  /* And make sure to update the stack pointer.  */
  State.regs[REG_SP] = sp;
}

/* clr dn */
void OP_0 ()
{
  State.regs[REG_D0 + ((insn & 0xc) >> 2)] = 0;

  PSW |= PSW_Z;
  PSW &= ~(PSW_V | PSW_C | PSW_N);
}

/* add dm,dn */
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
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

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
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add am, dn */
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
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

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
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, dn */
void OP_2800 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x300) >> 8)];
  imm = SEXT8 (insn & 0xff);
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0x300) >> 8)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm16, dn */
void OP_FAC00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm32,dn */
void OP_FCC00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 + imm;
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, an */
void OP_2000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x300) >> 8)];
  imm = SEXT8 (insn & 0xff);
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0x300) >> 8)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm16, an */
void OP_FAD00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x30000) >> 16)];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm32, an */
void OP_FCD00000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 + imm;
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm8, sp */
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
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

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
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* add imm32, sp */
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
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* addc dm,dn */
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
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* sub dm, dn */
void OP_F100 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_D0 + (insn & 0x3)] = value;
}

/* sub dm, an */
void OP_F120 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_A0 + (insn & 0x3)] = value;
}

/* sub am, dn */
void OP_F110 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_D0 + (insn & 0x3)] = value;
}

/* sub am, an */
void OP_F130 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_A0 + (insn & 0x3)] = value;
}

/* sub imm32, dn */
void OP_FCC40000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] = value;
}

/* sub imm32, an */
void OP_FCD40000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_A0 + ((insn & 0x30000) >> 16)] = value;
}

/* subc dm, dn */
void OP_F180 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_D0 + (insn & 0x3)];
  value = reg2 - reg1 - ((PSW & PSW_C) != 0);

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
  State.regs[REG_D0 + (insn & 0x3)] = value;
}

/* mul dm, dn */
void OP_F240 ()
{
  unsigned long long temp;
  int n, z;

  temp = (State.regs[REG_D0 + (insn & 0x3)]
          *  State.regs[REG_D0 + ((insn & 0xc) >> 2)]);
  State.regs[REG_D0 + (insn & 0x3)] = temp & 0xffffffff;
  State.regs[REG_MDR] = temp & 0xffffffff00000000LL;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* mulu dm, dn */
void OP_F250 ()
{
  unsigned long long temp;
  int n, z;

  temp = (State.regs[REG_D0 + (insn & 0x3)]
          *  State.regs[REG_D0 + ((insn & 0xc) >> 2)]);
  State.regs[REG_D0 + (insn & 0x3)] = temp & 0xffffffff;
  State.regs[REG_MDR] = temp & 0xffffffff00000000LL;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* div dm, dn */
void OP_F260 ()
{
  long long temp;
  int n, z;

  temp = State.regs[REG_MDR];
  temp <<= 32;
  temp |= State.regs[REG_D0 + (insn & 0x3)];
  State.regs[REG_MDR] = temp % (long)State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  temp /= (long)State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  State.regs[REG_D0 + (insn & 0x3)] = temp & 0xffffffff;
  State.regs[REG_MDR] = temp & 0xffffffff00000000LL;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* divu dm, dn */
void OP_F270 ()
{
  unsigned long long temp;
  int n, z;

  temp = State.regs[REG_MDR];
  temp <<= 32;
  temp |= State.regs[REG_D0 + (insn & 0x3)];
  State.regs[REG_MDR] = temp % State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  temp /= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  State.regs[REG_D0 + (insn & 0x3)] = temp & 0xffffffff;
  State.regs[REG_MDR] = temp & 0xffffffff00000000LL;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
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
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

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
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

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
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

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
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

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
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp am, an */
void OP_B0 ()
{
  int z, c, n, v;
  unsigned long reg1, reg2, value;

  reg1 = State.regs[REG_A0 + ((insn & 0xc) >> 2)];
  reg2 = State.regs[REG_A0 + (insn & 0x3)];
  value = reg2 - reg1;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 > reg2);
  v = ((reg2 & 0x80000000) != (reg1 & 0x80000000)
       && (reg2 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp imm16, dn */
void OP_FAC80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  imm = SEXT16 (insn & 0xffff);
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp imm32, dn */
void OP_FCC80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp imm16, an */
void OP_FAD80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x30000) >> 16)];
  imm = insn & 0xffff;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* cmp imm32, an */
void OP_FCD80000 ()
{
  int z, c, n, v;
  unsigned long reg1, imm, value;

  reg1 = State.regs[REG_A0 + ((insn & 0x30000) >> 16)];
  imm = ((insn & 0xffff) << 16) | extension;
  value = reg1 - imm;

  z = (value == 0);
  n = (value & 0x80000000);
  c = (reg1 < imm);
  v = ((reg1 & 0x80000000) != (imm & 0x80000000)
       && (reg1 & 0x80000000) != (value & 0x80000000));

  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | ( n ? PSW_N : 0)
	  | (c ? PSW_C : 0) | (v ? PSW_V : 0));
}

/* and dm, dn */
void OP_F200 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)] &= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* and imm8, dn */
void OP_F8E000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x300) >> 8)] &= (insn & 0xff);
  z = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* and imm16, dn */
void OP_FAE00000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] &= (insn & 0xffff);
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* and imm32, dn */
void OP_FCE00000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    &= ((insn & 0xffff) << 16 | extension);
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* and imm16, psw */
void OP_FAFC0000 ()
{
  PSW &= (insn & 0xffff);
}

/* or dm, dn*/
void OP_F210 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)] |= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* or imm8, dn */
void OP_F8E400 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x300) >> 8)] |= insn & 0xff;
  z = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* or imm16, dn*/
void OP_FAE40000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] |= insn & 0xffff;
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* or imm32, dn */
void OP_FCE40000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    |= ((insn & 0xffff) << 16 | extension);
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* or imm16,psw */
void OP_FAFD0000 ()
{
  PSW |= (insn & 0xffff);
}

/* xor dm, dn*/
void OP_F220 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)] ^= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* xor imm16, dn */
void OP_FAE80000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)] ^= insn & 0xffff;
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* xor imm32, dn */
void OP_FCE80000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x30000) >> 16)]
    ^= ((insn & 0xffff) << 16 | extension);
  z = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x30000) >> 16)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* not dn */
void OP_F230 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)] = ~State.regs[REG_D0 + (insn & 0x3)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* btst imm8, dn */
void OP_F8EC00 ()
{
  unsigned long temp;
  int z, n;

  temp = State.regs[REG_D0 + ((insn & 0x300) >> 8)];
  temp &= (insn & 0xff);
  n = (temp & 0x80000000) != 0;
  z = (temp == 0);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0) | (n ? PSW_N : 0);
}

/* btst imm16, dn */
void OP_FAEC0000 ()
{
  unsigned long temp;
  int z, n;

  temp = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  temp &= (insn & 0xffff);
  n = (temp & 0x80000000) != 0;
  z = (temp == 0);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0) | (n ? PSW_N : 0);
}

/* btst imm32, dn */
void OP_FCEC0000 ()
{
  unsigned long temp;
  int z, n;

  temp = State.regs[REG_D0 + ((insn & 0x30000) >> 16)];
  temp &= ((insn & 0xffff) << 16 | extension);
  n = (temp & 0x80000000) != 0;
  z = (temp == 0);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0) | (n ? PSW_N : 0);
}

/* btst imm8,(abs32) */
void OP_FE020000 ()
{
  unsigned long temp;
  int n, z;

  temp = load_mem (((insn & 0xffff) << 16) | (extension >> 8), 1);
  temp &= (extension & 0xff);
  n = (temp & 0x80000000) != 0;
  z = (temp == 0);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0) | (n ? PSW_N : 0);
}

/* btst imm8,(d8,an) */
void OP_FAF80000 ()
{
  unsigned long temp;
  int n, z;

  temp = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
                    + SEXT8 ((insn & 0xff00) >> 8)), 1);
  temp &= (insn & 0xff);
  n = (temp & 0x80000000) != 0;
  z = (temp == 0);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0) | (n ? PSW_N : 0);
}

/* bset dm, (an) */
void OP_F080 ()
{
  unsigned long temp;
  int z;

  temp = load_mem (State.regs[REG_A0 + (insn & 3)], 1);
  z = (temp & State.regs[REG_D0 + ((insn & 0xc) >> 2)]) == 0;
  temp |= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  store_mem (State.regs[REG_A0 + (insn & 3)], 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* bset imm8, (abs32) */
void OP_FE000000 ()
{
  unsigned long temp;
  int z;

  temp = load_mem (((insn & 0xffff) << 16 | (extension >> 8)), 1);
  z = (temp & (extension & 0xff)) == 0;
  temp |= (extension & 0xff);
  store_mem ((((insn & 0xffff) << 16) | (extension >> 8)), 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* bset imm8,(d8,an) */
void OP_FAF00000 ()
{
  unsigned long temp;
  int z;

  temp = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
                    + SEXT8 ((insn & 0xff00) >> 8)), 1);
  z = (temp & (insn & 0xff)) == 0;
  temp |= (insn & 0xff);
  store_mem (State.regs[REG_A0 + ((insn & 30000)>> 16)], 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* bclr dm, (an) */
void OP_F090 ()
{
  unsigned long temp;
  int z;

  temp = load_mem (State.regs[REG_A0 + (insn & 3)], 1);
  z = (temp & State.regs[REG_D0 + ((insn & 0xc) >> 2)]) == 0;
  temp = ~temp & State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  store_mem (State.regs[REG_A0 + (insn & 3)], 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* bclr imm8, (abs32) */
void OP_FE010000 ()
{
  unsigned long temp;
  int z;

  temp = load_mem (((insn & 0xffff) << 16) | (extension >> 8), 1);
  z = (temp & (extension & 0xff)) == 0;
  temp = ~temp & (extension & 0xff);
  store_mem (((insn & 0xffff) << 16) | (extension >> 8), 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* bclr imm8,(d8,an) */
void OP_FAF40000 ()
{
  unsigned long temp;
  int z;

  temp = load_mem ((State.regs[REG_A0 + ((insn & 0x30000) >> 16)]
                    + SEXT8 ((insn & 0xff00) >> 8)), 1);
  z = (temp & (insn & 0xff)) == 0;
  temp = ~temp & (insn & 0xff);
  store_mem (State.regs[REG_A0 + ((insn & 30000)>> 16)], 1, temp);
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= (z ? PSW_Z : 0);
}

/* asr dm, dn */
void OP_F2B0 ()
{
  long temp;
  int z, n, c;

  temp = State.regs[REG_D0 + (insn & 0x3)];
  c = temp & 1;
  temp >>= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  State.regs[REG_D0 + (insn & 0x3)] = temp;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* asr imm8, dn */
void OP_F8C800 ()
{
  long temp;
  int z, n, c;

  temp = State.regs[REG_D0 + ((insn & 0x300) >> 8)];
  c = temp & 1;
  temp >>= (insn & 0xff);
  State.regs[REG_D0 + ((insn & 0x300) >> 8)] = temp;
  z = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* lsr dm, dn */
void OP_F2A0 ()
{
  int z, n, c;

  c = State.regs[REG_D0 + (insn & 0x3)] & 1;
  State.regs[REG_D0 + (insn & 0x3)]
    >>= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* lsr dm, dn */
void OP_F8C400 ()
{
  int z, n, c;

  c = State.regs[REG_D0 + ((insn & 0x300) >> 8)] & 1;
  State.regs[REG_D0 + ((insn & 0x300) >> 8)] >>=  (insn & 0xff);
  z = (State.regs[REG_D0 + ((insn & 0x3) >> 8)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x3) >> 8)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* asl dm, dn */
void OP_F290 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)]
    <<= State.regs[REG_D0 + ((insn & 0xc) >> 2)];
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* asl imm8, dn */
void OP_F8C000 ()
{
  int n, z;

  State.regs[REG_D0 + ((insn & 0x300) >> 8)] <<= (insn & 0xff);
  z = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] == 0);
  n = (State.regs[REG_D0 + ((insn & 0x300) >> 8)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* asl2 dn */
void OP_54 ()
{
  int n, z;

  State.regs[REG_D0 + (insn & 0x3)] <<= 2;
  z = (State.regs[REG_D0 + (insn & 0x3)] == 0);
  n = (State.regs[REG_D0 + (insn & 0x3)] & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0));
}

/* ror dn */
void OP_F284 ()
{
  unsigned long value;
  int c,n,z;

  value = State.regs[REG_D0 + (insn & 0x3)];
  if (value & 0x1)
    c = 1;

  value >>= 1;
  value |= ((PSW & PSW_C) != 0) ? 0x80000000 : 0;
  State.regs[REG_D0 + (insn & 0x3)] = value;
  z = (value == 0);
  n = (value & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* rol dn */
void OP_F280 ()
{
  unsigned long value;
  int c,n,z;

  value = State.regs[REG_D0 + (insn & 0x3)];
  if (value & 0x80000000)
    c = 1;

  value <<= 1;
  value |= ((PSW & PSW_C) != 0);
  State.regs[REG_D0 + (insn & 0x3)] = value;
  z = (value == 0);
  n = (value & 0x80000000) != 0;
  PSW &= ~(PSW_Z | PSW_N | PSW_C | PSW_V);
  PSW |= ((z ? PSW_Z : 0) | (n ? PSW_N : 0) | (c ? PSW_C : 0));
}

/* beq label:8 */
void OP_C800 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (PSW & PSW_Z)
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bne label:8 */
void OP_C900 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!(PSW & PSW_Z))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bgt label:8 */
void OP_C100 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!((PSW & PSW_Z)
        || (((PSW & PSW_N) != 0) ^ (PSW & PSW_V) != 0)))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bge label:8 */
void OP_C200 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!(((PSW & PSW_N) != 0) ^ (PSW & PSW_V) != 0))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* ble label:8 */
void OP_C300 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if ((PSW & PSW_Z)
       || (((PSW & PSW_N) != 0) ^ (PSW & PSW_V) != 0))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* blt label:8 */
void OP_C000 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (((PSW & PSW_N) != 0) ^ (PSW & PSW_V) != 0)
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bhi label:8 */
void OP_C500 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!(((PSW & PSW_C) != 0) || (PSW & PSW_Z) != 0))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bcc label:8 */
void OP_C600 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (!(PSW & PSW_C))
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bls label:8 */
void OP_C700 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (((PSW & PSW_C) != 0) || (PSW & PSW_Z) != 0)
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bcs label:8 */
void OP_C400 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  if (PSW & PSW_C)
    State.pc += SEXT8 (insn & 0xff) - 2;
}

/* bvc label:8 */
void OP_F8E800 ()
{
  /* The dispatching code will add 3 after we return, so
     we subtract two here to make things right.  */
  if (!(PSW & PSW_V))
    State.pc += SEXT8 (insn & 0xff) - 3;
}

/* bvs label:8 */
void OP_F8E900 ()
{
  /* The dispatching code will add 3 after we return, so
     we subtract two here to make things right.  */
  if (PSW & PSW_V)
    State.pc += SEXT8 (insn & 0xff) - 3;
}

/* bnc label:8 */
void OP_F8EA00 ()
{
  /* The dispatching code will add 3 after we return, so
     we subtract two here to make things right.  */
  if (!(PSW & PSW_N))
    State.pc += SEXT8 (insn & 0xff) - 3;
}

/* bns label:8 */
void OP_F8EB00 ()
{
  /* The dispatching code will add 3 after we return, so
     we subtract two here to make things right.  */
  if (PSW & PSW_N)
    State.pc += SEXT8 (insn & 0xff) - 3;
}

/* bra label:8 */
void OP_CA00 ()
{
  /* The dispatching code will add 2 after we return, so
     we subtract two here to make things right.  */
  State.pc += SEXT8 (insn & 0xff) - 2;
}

/* leq */
void OP_D8 ()
{
 abort ();
}

/* lne */
void OP_D9 ()
{
 abort ();
}

/* lgt */
void OP_D1 ()
{
 abort ();
}

/* lge */
void OP_D2 ()
{
 abort ();
}

/* lle */
void OP_D3 ()
{
 abort ();
}

/* llt */
void OP_D0 ()
{
 abort ();
}

/* lhi */
void OP_D5 ()
{
 abort ();
}

/* lcc */
void OP_D6 ()
{
 abort ();
}

/* lls */
void OP_D7 ()
{
 abort ();
}

/* lcs */
void OP_D4 ()
{
 abort ();
}

/* lra */
void OP_DA ()
{
 abort ();
}

/* setlb */
void OP_DB ()
{
 abort ();
}

/* jmp (an) */
void OP_F0F4 ()
{
  State.pc = State.regs[REG_A0 + (insn & 0x3)] - 2;
}

/* jmp label:16 */
void OP_CC0000 ()
{
  State.pc += SEXT16 (insn & 0xffff) - 3;
}

/* jmp label:32 */
void OP_DC000000 ()
{
  State.pc += (((insn & 0xffffff) << 8) | extension) - 5;
}

/* call label:16,reg_list,imm8 */
void OP_CD000000 ()
{
  unsigned int next_pc, sp, adjust;
  unsigned long mask;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 2;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = (next_pc & 0xff00) >> 8;
  State.mem[sp+2] = (next_pc & 0xff0000) >> 16;
  State.mem[sp+3] = (next_pc & 0xff000000) >> 24;

  mask = insn & 0xff;

  adjust = 0;
  if (mask & 0x80)
    {
      adjust -= 4;
      State.regs[REG_D0 + 2] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x40)
    {
      adjust -= 4;
      State.regs[REG_D0 + 3] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x20)
    {
      adjust -= 4;
      State.regs[REG_A0 + 2] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x10)
    {
      adjust -= 4;
      State.regs[REG_A0 + 3] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x8)
    {
      adjust -= 4;
      State.regs[REG_D0] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_D0 + 1] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_A0] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_A0 + 1] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_MDR] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_LIR] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_LAR] = load_mem (sp + adjust, 4);
      adjust -= 4;
    }

  /* And make sure to update the stack pointer.  */
  State.regs[REG_SP] -= extension;
  State.regs[REG_MDR] = next_pc;
  State.pc += SEXT16 ((insn & 0xffff00) >> 8) - 5;
}

/* call label:32,reg_list,imm8*/
void OP_DD000000 ()
{
  unsigned int next_pc, sp, adjust;
  unsigned long mask;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 2;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = (next_pc & 0xff00) >> 8;
  State.mem[sp+2] = (next_pc & 0xff0000) >> 16;
  State.mem[sp+3] = (next_pc & 0xff000000) >> 24;

  mask = (extension & 0xff00) >> 8;

  adjust = 0;
  if (mask & 0x80)
    {
      adjust -= 4;
      State.regs[REG_D0 + 2] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x40)
    {
      adjust -= 4;
      State.regs[REG_D0 + 3] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x20)
    {
      adjust -= 4;
      State.regs[REG_A0 + 2] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x10)
    {
      adjust -= 4;
      State.regs[REG_A0 + 3] = load_mem (sp + adjust, 4);
    }

  if (mask & 0x8)
    {
      adjust -= 4;
      State.regs[REG_D0] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_D0 + 1] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_A0] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_A0 + 1] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_MDR] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_LIR] = load_mem (sp + adjust, 4);
      adjust -= 4;
      State.regs[REG_LAR] = load_mem (sp + adjust, 4);
      adjust -= 4;
    }

  /* And make sure to update the stack pointer.  */
  State.regs[REG_SP] -= (extension & 0xff);
  State.regs[REG_MDR] = next_pc;
  State.pc += (((insn & 0xffffff) << 8) | ((extension & 0xff0000) >> 16)) - 7;
}

/* calls (an) */
void OP_F0F0 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 2;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = (next_pc & 0xff00) >> 8;
  State.mem[sp+2] = (next_pc & 0xff0000) >> 16;
  State.mem[sp+3] = (next_pc & 0xff000000) >> 24;
  State.regs[REG_MDR] = next_pc;
  State.pc = State.regs[REG_A0 + (insn & 0x3)] - 2;
}

/* calls label:16 */
void OP_FAFF0000 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 4;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = (next_pc & 0xff00) >> 8;
  State.mem[sp+2] = (next_pc & 0xff0000) >> 16;
  State.mem[sp+3] = (next_pc & 0xff000000) >> 24;
  State.regs[REG_MDR] = next_pc;
  State.pc += SEXT16 (insn & 0xffff) - 4;
}

/* calls label:32 */
void OP_FCFF0000 ()
{
  unsigned int next_pc, sp;

  sp = State.regs[REG_SP];
  next_pc = State.pc + 6;
  State.mem[sp] = next_pc & 0xff;
  State.mem[sp+1] = (next_pc & 0xff00) >> 8;
  State.mem[sp+2] = (next_pc & 0xff0000) >> 16;
  State.mem[sp+3] = (next_pc & 0xff000000) >> 24;
  State.regs[REG_MDR] = next_pc;
  State.pc += (((insn & 0xffff) << 16) | extension) - 6;
}

/* ret reg_list, imm8 */
void OP_DF0000 ()
{
  unsigned int sp;
  unsigned long mask;

  State.regs[REG_SP] += insn & 0xff;
  State.pc = State.regs[REG_MDR] - 3;
  sp = State.regs[REG_SP];

  mask = (insn & 0xff00) >> 8;

  if (mask & 0x8)
    {
      sp += 4;
      State.regs[REG_LAR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_LIR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_MDR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x10)
    {
      State.regs[REG_A0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x20)
    {
      State.regs[REG_A0 + 2] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x40)
    {
      State.regs[REG_D0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x80)
    {
      State.regs[REG_D0 + 2] = load_mem (sp, 4);
      sp += 4;
    }
}

/* retf reg_list,imm8 */
void OP_DE0000 ()
{
  unsigned int sp;
  unsigned long mask;

  State.regs[REG_SP] += insn & 0xff;
  State.pc = (State.mem[sp] | (State.mem[sp+1] << 8)
	      | (State.mem[sp+2] << 16) | (State.mem[sp+3] << 24));
  State.pc -= 3;

  sp = State.regs[REG_SP];

  mask = (insn & 0xff00) >> 8;

  if (mask & 0x8)
    {
      sp += 4;
      State.regs[REG_LAR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_LIR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_MDR] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_A0] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0 + 1] = load_mem (sp, 4);
      sp += 4;
      State.regs[REG_D0] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x10)
    {
      State.regs[REG_A0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x20)
    {
      State.regs[REG_A0 + 2] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x40)
    {
      State.regs[REG_D0 + 3] = load_mem (sp, 4);
      sp += 4;
    }

  if (mask & 0x80)
    {
      State.regs[REG_D0 + 2] = load_mem (sp, 4);
      sp += 4;
    }
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
  abort ();
}

/* trap */
void OP_F0FE ()
{
  /* We use this for simulated system calls; we may need to change
     it to a reserved instruction if we conflict with uses at
     Matsushita.  */
  int save_errno = errno;	
  errno = 0;

/* Registers passed to trap 0 */

/* Function number.  */
#define FUNC   (load_mem (State.regs[REG_SP] + 4, 4))

/* Parameters.  */
#define PARM1   (load_mem (State.regs[REG_SP] + 8, 4))
#define PARM2   (load_mem (State.regs[REG_SP] + 12, 4))
#define PARM3   (load_mem (State.regs[REG_SP] + 16, 4))

/* Registers set by trap 0 */

#define RETVAL State.regs[0]	/* return value */
#define RETERR State.regs[1]	/* return error code */

/* Turn a pointer in a register into a pointer into real memory. */

#define MEMPTR(x) (State.mem + x)

  switch (FUNC)
    {
#if !defined(__GO32__) && !defined(_WIN32)
      case SYS_fork:
      RETVAL = fork ();
      break;
    case SYS_execve:
      RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2),
		       (char **)MEMPTR (PARM3));
      break;
    case SYS_execv:
      RETVAL = execve (MEMPTR (PARM1), (char **) MEMPTR (PARM2), NULL);
      break;
#endif

    case SYS_read:
      RETVAL = mn10300_callback->read (mn10300_callback, PARM1,
				    MEMPTR (PARM2), PARM3);
      break;
    case SYS_write:
      if (PARM1 == 1)
	RETVAL = (int)mn10300_callback->write_stdout (mn10300_callback,
						   MEMPTR (PARM2), PARM3);
      else
	RETVAL = (int)mn10300_callback->write (mn10300_callback, PARM1,
					    MEMPTR (PARM2), PARM3);
      break;
    case SYS_lseek:
      RETVAL = mn10300_callback->lseek (mn10300_callback, PARM1, PARM2, PARM3);
      break;
    case SYS_close:
      RETVAL = mn10300_callback->close (mn10300_callback, PARM1);
      break;
    case SYS_open:
      RETVAL = mn10300_callback->open (mn10300_callback, MEMPTR (PARM1), PARM2);
      break;
    case SYS_exit:
      /* EXIT - caller can look in PARM1 to work out the 
	 reason */
      if (PARM1 == 0xdead || PARM1 == 0x1)
	State.exception = SIGABRT;
      else
	State.exception = SIGQUIT;
      break;

    case SYS_stat:	/* added at hmsi */
      /* stat system call */
      {
	struct stat host_stat;
	reg_t buf;

	RETVAL = stat (MEMPTR (PARM1), &host_stat);
	
	buf = PARM2;

	/* Just wild-assed guesses.  */
	store_mem (buf, 2, host_stat.st_dev);
	store_mem (buf + 2, 2, host_stat.st_ino);
	store_mem (buf + 4, 4, host_stat.st_mode);
	store_mem (buf + 8, 2, host_stat.st_nlink);
	store_mem (buf + 10, 2, host_stat.st_uid);
	store_mem (buf + 12, 2, host_stat.st_gid);
	store_mem (buf + 14, 2, host_stat.st_rdev);
	store_mem (buf + 16, 4, host_stat.st_size);
	store_mem (buf + 20, 4, host_stat.st_atime);
	store_mem (buf + 28, 4, host_stat.st_mtime);
	store_mem (buf + 36, 4, host_stat.st_ctime);
      }
      break;

    case SYS_chown:
      RETVAL = chown (MEMPTR (PARM1), PARM2, PARM3);
      break;
    case SYS_chmod:
      RETVAL = chmod (MEMPTR (PARM1), PARM2);
      break;
    case SYS_time:
      RETVAL = time (MEMPTR (PARM1));
      break;
    case SYS_times:
      {
	struct tms tms;
	RETVAL = times (&tms);
	store_mem (PARM1, 4, tms.tms_utime);
	store_mem (PARM1 + 4, 4, tms.tms_stime);
	store_mem (PARM1 + 8, 4, tms.tms_cutime);
	store_mem (PARM1 + 12, 4, tms.tms_cstime);
	break;
      }
    case SYS_gettimeofday:
      {
	struct timeval t;
	struct timezone tz;
	RETVAL = gettimeofday (&t, &tz);
	store_mem (PARM1, 4, t.tv_sec);
	store_mem (PARM1 + 4, 4, t.tv_usec);
	store_mem (PARM2, 4, tz.tz_minuteswest);
	store_mem (PARM2 + 4, 4, tz.tz_dsttime);
	break;
      }
    case SYS_utime:
      /* Cast the second argument to void *, to avoid type mismatch
	 if a prototype is present.  */
      RETVAL = utime (MEMPTR (PARM1), (void *) MEMPTR (PARM2));
      break;
    default:
      abort ();
    }
  RETERR = errno;
  errno = save_errno;
}

/* rtm */
void OP_F0FF ()
{
 abort ();
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
