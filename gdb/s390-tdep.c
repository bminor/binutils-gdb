/* Target-dependent code for GDB, the GNU debugger.

   Copyright 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   Contributed by D.J. Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
   for IBM Deutschland Entwicklung GmbH, IBM Corporation.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "defs.h"
#include "arch-utils.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "tm.h"
#include "../bfd/bfd.h"
#include "floatformat.h"
#include "regcache.h"
#include "reggroups.h"
#include "regset.h"
#include "value.h"
#include "gdb_assert.h"
#include "dis-asm.h"

#include "s390-tdep.h"


/* The tdep structure.  */

struct gdbarch_tdep
{
  /* ABI version.  */
  enum { ABI_LINUX_S390, ABI_LINUX_ZSERIES } abi;

  /* Core file register sets.  */
  const struct regset *gregset;
  int sizeof_gregset;

  const struct regset *fpregset;
  int sizeof_fpregset;
};


/* Register information.  */

struct s390_register_info
{
  char *name;
  struct type **type;
};

static struct s390_register_info s390_register_info[S390_NUM_TOTAL_REGS] = 
{
  /* Program Status Word.  */
  { "pswm", &builtin_type_long },
  { "pswa", &builtin_type_long },

  /* General Purpose Registers.  */
  { "r0", &builtin_type_long },
  { "r1", &builtin_type_long },
  { "r2", &builtin_type_long },
  { "r3", &builtin_type_long },
  { "r4", &builtin_type_long },
  { "r5", &builtin_type_long },
  { "r6", &builtin_type_long },
  { "r7", &builtin_type_long },
  { "r8", &builtin_type_long },
  { "r9", &builtin_type_long },
  { "r10", &builtin_type_long },
  { "r11", &builtin_type_long },
  { "r12", &builtin_type_long },
  { "r13", &builtin_type_long },
  { "r14", &builtin_type_long },
  { "r15", &builtin_type_long },

  /* Access Registers.  */
  { "acr0", &builtin_type_int },
  { "acr1", &builtin_type_int },
  { "acr2", &builtin_type_int },
  { "acr3", &builtin_type_int },
  { "acr4", &builtin_type_int },
  { "acr5", &builtin_type_int },
  { "acr6", &builtin_type_int },
  { "acr7", &builtin_type_int },
  { "acr8", &builtin_type_int },
  { "acr9", &builtin_type_int },
  { "acr10", &builtin_type_int },
  { "acr11", &builtin_type_int },
  { "acr12", &builtin_type_int },
  { "acr13", &builtin_type_int },
  { "acr14", &builtin_type_int },
  { "acr15", &builtin_type_int },

  /* Floating Point Control Word.  */
  { "fpc", &builtin_type_int },

  /* Floating Point Registers.  */
  { "f0", &builtin_type_double },
  { "f1", &builtin_type_double },
  { "f2", &builtin_type_double },
  { "f3", &builtin_type_double },
  { "f4", &builtin_type_double },
  { "f5", &builtin_type_double },
  { "f6", &builtin_type_double },
  { "f7", &builtin_type_double },
  { "f8", &builtin_type_double },
  { "f9", &builtin_type_double },
  { "f10", &builtin_type_double },
  { "f11", &builtin_type_double },
  { "f12", &builtin_type_double },
  { "f13", &builtin_type_double },
  { "f14", &builtin_type_double },
  { "f15", &builtin_type_double },

  /* Pseudo registers.  */
  { "pc", &builtin_type_void_func_ptr },
  { "cc", &builtin_type_int },
};

/* Return the name of register REGNUM.  */
static const char *
s390_register_name (int regnum)
{
  gdb_assert (regnum >= 0 && regnum < S390_NUM_TOTAL_REGS);
  return s390_register_info[regnum].name;
}

/* Return the GDB type object for the "standard" data type of data in
   register REGNUM. */
static struct type *
s390_register_type (struct gdbarch *gdbarch, int regnum)
{
  gdb_assert (regnum >= 0 && regnum < S390_NUM_TOTAL_REGS);
  return *s390_register_info[regnum].type;
}

/* DWARF Register Mapping.  */

static int s390_dwarf_regmap[] =
{
  /* General Purpose Registers.  */
  S390_R0_REGNUM, S390_R1_REGNUM, S390_R2_REGNUM, S390_R3_REGNUM,
  S390_R4_REGNUM, S390_R5_REGNUM, S390_R6_REGNUM, S390_R7_REGNUM,
  S390_R8_REGNUM, S390_R9_REGNUM, S390_R10_REGNUM, S390_R11_REGNUM,
  S390_R12_REGNUM, S390_R13_REGNUM, S390_R14_REGNUM, S390_R15_REGNUM,

  /* Floating Point Registers.  */
  S390_F0_REGNUM, S390_F2_REGNUM, S390_F4_REGNUM, S390_F6_REGNUM,
  S390_F1_REGNUM, S390_F3_REGNUM, S390_F5_REGNUM, S390_F7_REGNUM,
  S390_F8_REGNUM, S390_F10_REGNUM, S390_F12_REGNUM, S390_F14_REGNUM,
  S390_F9_REGNUM, S390_F11_REGNUM, S390_F13_REGNUM, S390_F15_REGNUM,

  /* Control Registers (not mapped).  */
  -1, -1, -1, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, -1, -1, -1, 

  /* Access Registers.  */
  S390_A0_REGNUM, S390_A1_REGNUM, S390_A2_REGNUM, S390_A3_REGNUM,
  S390_A4_REGNUM, S390_A5_REGNUM, S390_A6_REGNUM, S390_A7_REGNUM,
  S390_A8_REGNUM, S390_A9_REGNUM, S390_A10_REGNUM, S390_A11_REGNUM,
  S390_A12_REGNUM, S390_A13_REGNUM, S390_A14_REGNUM, S390_A15_REGNUM,

  /* Program Status Word.  */
  S390_PSWM_REGNUM,
  S390_PSWA_REGNUM
};

/* Convert DWARF register number REG to the appropriate register
   number used by GDB.  */
static int
s390_dwarf_reg_to_regnum (int reg)
{
  int regnum = -1;

  if (reg >= 0 || reg < ARRAY_SIZE (s390_dwarf_regmap))
    regnum = s390_dwarf_regmap[reg];

  if (regnum == -1)
    warning ("Unmapped DWARF Register #%d encountered\n", reg);

  return regnum;
}

/* Pseudo registers - PC and condition code.  */

static void
s390_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
			   int regnum, void *buf)
{
  ULONGEST val;

  switch (regnum)
    {
    case S390_PC_REGNUM:
      regcache_raw_read_unsigned (regcache, S390_PSWA_REGNUM, &val);
      store_unsigned_integer (buf, 4, val & 0x7fffffff);
      break;

    case S390_CC_REGNUM:
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &val);
      store_unsigned_integer (buf, 4, (val >> 12) & 3);
      break;

    default:
      internal_error (__FILE__, __LINE__, "invalid regnum");
    }
}

static void
s390_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
			    int regnum, const void *buf)
{
  ULONGEST val, psw;

  switch (regnum)
    {
    case S390_PC_REGNUM:
      val = extract_unsigned_integer (buf, 4);
      regcache_raw_read_unsigned (regcache, S390_PSWA_REGNUM, &psw);
      psw = (psw & 0x80000000) | (val & 0x7fffffff);
      regcache_raw_write_unsigned (regcache, S390_PSWA_REGNUM, psw);
      break;

    case S390_CC_REGNUM:
      val = extract_unsigned_integer (buf, 4);
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &psw);
      psw = (psw & ~((ULONGEST)3 << 12)) | ((val & 3) << 12);
      regcache_raw_write_unsigned (regcache, S390_PSWM_REGNUM, psw);
      break;

    default:
      internal_error (__FILE__, __LINE__, "invalid regnum");
    }
}

static void
s390x_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
			    int regnum, void *buf)
{
  ULONGEST val;

  switch (regnum)
    {
    case S390_PC_REGNUM:
      regcache_raw_read (regcache, S390_PSWA_REGNUM, buf);
      break;

    case S390_CC_REGNUM:
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &val);
      store_unsigned_integer (buf, 4, (val >> 44) & 3);
      break;

    default:
      internal_error (__FILE__, __LINE__, "invalid regnum");
    }
}

static void
s390x_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
			     int regnum, const void *buf)
{
  ULONGEST val, psw;

  switch (regnum)
    {
    case S390_PC_REGNUM:
      regcache_raw_write (regcache, S390_PSWA_REGNUM, buf);
      break;

    case S390_CC_REGNUM:
      val = extract_unsigned_integer (buf, 4);
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &psw);
      psw = (psw & ~((ULONGEST)3 << 44)) | ((val & 3) << 44);
      regcache_raw_write_unsigned (regcache, S390_PSWM_REGNUM, psw);
      break;

    default:
      internal_error (__FILE__, __LINE__, "invalid regnum");
    }
}

/* 'float' values are stored in the upper half of floating-point
   registers, even though we are otherwise a big-endian platform.  */

static int
s390_convert_register_p (int regno, struct type *type)
{
  return (regno >= S390_F0_REGNUM && regno <= S390_F15_REGNUM)
	 && TYPE_LENGTH (type) < 8;
}

static void
s390_register_to_value (struct frame_info *frame, int regnum,
                        struct type *valtype, void *out)
{
  char in[8];
  int len = TYPE_LENGTH (valtype);
  gdb_assert (len < 8);

  get_frame_register (frame, regnum, in);
  memcpy (out, in, len);
}

static void
s390_value_to_register (struct frame_info *frame, int regnum,
                        struct type *valtype, const void *in)
{
  char out[8];
  int len = TYPE_LENGTH (valtype);
  gdb_assert (len < 8);

  memset (out, 0, 8);
  memcpy (out, in, len);
  put_frame_register (frame, regnum, out);
}

/* Register groups.  */

static int
s390_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
			  struct reggroup *group)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Registers displayed via 'info regs'.  */
  if (group == general_reggroup)
    return (regnum >= S390_R0_REGNUM && regnum <= S390_R15_REGNUM)
	   || regnum == S390_PC_REGNUM
	   || regnum == S390_CC_REGNUM;

  /* Registers displayed via 'info float'.  */
  if (group == float_reggroup)
    return (regnum >= S390_F0_REGNUM && regnum <= S390_F15_REGNUM)
	   || regnum == S390_FPC_REGNUM;

  /* Registers that need to be saved/restored in order to
     push or pop frames.  */
  if (group == save_reggroup || group == restore_reggroup)
    return regnum != S390_PSWM_REGNUM && regnum != S390_PSWA_REGNUM;

  return default_register_reggroup_p (gdbarch, regnum, group);
}


/* Core file register sets.  */

int s390_regmap_gregset[S390_NUM_REGS] =
{
  /* Program Status Word.  */
  0x00, 0x04,
  /* General Purpose Registers.  */
  0x08, 0x0c, 0x10, 0x14,
  0x18, 0x1c, 0x20, 0x24,
  0x28, 0x2c, 0x30, 0x34,
  0x38, 0x3c, 0x40, 0x44,
  /* Access Registers.  */
  0x48, 0x4c, 0x50, 0x54,
  0x58, 0x5c, 0x60, 0x64,
  0x68, 0x6c, 0x70, 0x74,
  0x78, 0x7c, 0x80, 0x84,
  /* Floating Point Control Word.  */
  -1,
  /* Floating Point Registers.  */
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
};

int s390x_regmap_gregset[S390_NUM_REGS] =
{
  0x00, 0x08,
  /* General Purpose Registers.  */
  0x10, 0x18, 0x20, 0x28,
  0x30, 0x38, 0x40, 0x48,
  0x50, 0x58, 0x60, 0x68,
  0x70, 0x78, 0x80, 0x88,
  /* Access Registers.  */
  0x90, 0x94, 0x98, 0x9c,
  0xa0, 0xa4, 0xa8, 0xac,
  0xb0, 0xb4, 0xb8, 0xbc,
  0xc0, 0xc4, 0xc8, 0xcc,
  /* Floating Point Control Word.  */
  -1,
  /* Floating Point Registers.  */
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
};

int s390_regmap_fpregset[S390_NUM_REGS] =
{
  /* Program Status Word.  */
  -1, -1,
  /* General Purpose Registers.  */
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  /* Access Registers.  */
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  /* Floating Point Control Word.  */
  0x00,
  /* Floating Point Registers.  */
  0x08, 0x10, 0x18, 0x20,
  0x28, 0x30, 0x38, 0x40,
  0x48, 0x50, 0x58, 0x60,
  0x68, 0x70, 0x78, 0x80,
};

/* Supply register REGNUM from the register set REGSET to register cache 
   REGCACHE.  If REGNUM is -1, do this for all registers in REGSET.  */
static void
s390_supply_regset (const struct regset *regset, struct regcache *regcache,
		    int regnum, const void *regs, size_t len)
{
  const int *offset = regset->descr;
  int i;

  for (i = 0; i < S390_NUM_REGS; i++)
    {
      if ((regnum == i || regnum == -1) && offset[i] != -1)
	regcache_raw_supply (regcache, i, (const char *)regs + offset[i]);
    }
}

static const struct regset s390_gregset = {
  s390_regmap_gregset, 
  s390_supply_regset
};

static const struct regset s390x_gregset = {
  s390x_regmap_gregset, 
  s390_supply_regset
};

static const struct regset s390_fpregset = {
  s390_regmap_fpregset, 
  s390_supply_regset
};

/* Return the appropriate register set for the core section identified
   by SECT_NAME and SECT_SIZE.  */
const struct regset *
s390_regset_from_core_section (struct gdbarch *gdbarch,
			       const char *sect_name, size_t sect_size)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (strcmp (sect_name, ".reg") == 0 && sect_size == tdep->sizeof_gregset)
    return tdep->gregset;

  if (strcmp (sect_name, ".reg2") == 0 && sect_size == tdep->sizeof_fpregset)
    return tdep->fpregset;

  return NULL;
}


#define GDB_TARGET_IS_ESAME (TARGET_ARCHITECTURE->mach == bfd_mach_s390_64)
#define S390_GPR_SIZE      (GDB_TARGET_IS_ESAME ? 8 : 4)
#define S390_FPR_SIZE      (8)
#define S390_MAX_INSTR_SIZE (6)
#define S390_SYSCALL_OPCODE (0x0a)
#define S390_SYSCALL_SIZE   (2)
#define S390_SIGCONTEXT_SREGS_OFFSET (8)
#define S390X_SIGCONTEXT_SREGS_OFFSET (8)
#define S390_SIGREGS_FP0_OFFSET       (144)
#define S390X_SIGREGS_FP0_OFFSET      (216)
#define S390_UC_MCONTEXT_OFFSET    (256)
#define S390X_UC_MCONTEXT_OFFSET   (344)
#define S390_SIGNAL_FRAMESIZE  (GDB_TARGET_IS_ESAME ? 160:96)
#define s390_NR_sigreturn          119
#define s390_NR_rt_sigreturn       173



struct frame_extra_info
{
  int initialised;
  int good_prologue;
  CORE_ADDR function_start;
  CORE_ADDR skip_prologue_function_start;
  CORE_ADDR saved_pc_valid;
  CORE_ADDR saved_pc;
  CORE_ADDR sig_fixed_saved_pc_valid;
  CORE_ADDR sig_fixed_saved_pc;
  CORE_ADDR frame_pointer_saved_pc;	/* frame pointer needed for alloca */
  CORE_ADDR stack_bought_valid;
  CORE_ADDR stack_bought;	/* amount we decrement the stack pointer by */
  CORE_ADDR sigcontext;
};


static CORE_ADDR s390_frame_saved_pc_nofix (struct frame_info *fi);

static int
s390_readinstruction (bfd_byte instr[], CORE_ADDR at)
{
  int instrlen;

  static int s390_instrlen[] = {
    2,
    4,
    4,
    6
  };
  if (target_read_memory (at, &instr[0], 2))
    return -1;
  instrlen = s390_instrlen[instr[0] >> 6];
  if (instrlen > 2)
    {
      if (target_read_memory (at + 2, &instr[2], instrlen - 2))
        return -1;
    }
  return instrlen;
}

static void
s390_memset_extra_info (struct frame_extra_info *fextra_info)
{
  memset (fextra_info, 0, sizeof (struct frame_extra_info));
}


/* Prologue analysis.  */

/* When we analyze a prologue, we're really doing 'abstract
   interpretation' or 'pseudo-evaluation': running the function's code
   in simulation, but using conservative approximations of the values
   it would have when it actually runs.  For example, if our function
   starts with the instruction:

      ahi r1, 42     # add halfword immediate 42 to r1

   we don't know exactly what value will be in r1 after executing this
   instruction, but we do know it'll be 42 greater than its original
   value.

   If we then see an instruction like:

      ahi r1, 22     # add halfword immediate 22 to r1

   we still don't know what r1's value is, but again, we can say it is
   now 64 greater than its original value.

   If the next instruction were:

      lr r2, r1      # set r2 to r1's value

   then we can say that r2's value is now the original value of r1
   plus 64.  And so on.

   Of course, this can only go so far before it gets unreasonable.  If
   we wanted to be able to say anything about the value of r1 after
   the instruction:

      xr r1, r3      # exclusive-or r1 and r3, place result in r1

   then things would get pretty complex.  But remember, we're just
   doing a conservative approximation; if exclusive-or instructions
   aren't relevant to prologues, we can just say r1's value is now
   'unknown'.  We can ignore things that are too complex, if that loss
   of information is acceptable for our application.

   Once you've reached an instruction that you don't know how to
   simulate, you stop.  Now you examine the state of the registers and
   stack slots you've kept track of.  For example:

   - To see how large your stack frame is, just check the value of sp;
     if it's the original value of sp minus a constant, then that
     constant is the stack frame's size.  If the sp's value has been
     marked as 'unknown', then that means the prologue has done
     something too complex for us to track, and we don't know the
     frame size.

   - To see whether we've saved the SP in the current frame's back
     chain slot, we just check whether the current value of the back
     chain stack slot is the original value of the sp.

   Sure, this takes some work.  But prologue analyzers aren't
   quick-and-simple pattern patching to recognize a few fixed prologue
   forms any more; they're big, hairy functions.  Along with inferior
   function calls, prologue analysis accounts for a substantial
   portion of the time needed to stabilize a GDB port.  So I think
   it's worthwhile to look for an approach that will be easier to
   understand and maintain.  In the approach used here:

   - It's easier to see that the analyzer is correct: you just see
     whether the analyzer properly (albiet conservatively) simulates
     the effect of each instruction.

   - It's easier to extend the analyzer: you can add support for new
     instructions, and know that you haven't broken anything that
     wasn't already broken before.

   - It's orthogonal: to gather new information, you don't need to
     complicate the code for each instruction.  As long as your domain
     of conservative values is already detailed enough to tell you
     what you need, then all the existing instruction simulations are
     already gathering the right data for you.

   A 'struct prologue_value' is a conservative approximation of the
   real value the register or stack slot will have.  */

struct prologue_value {

  /* What sort of value is this?  This determines the interpretation
     of subsequent fields.  */
  enum {

    /* We don't know anything about the value.  This is also used for
       values we could have kept track of, when doing so would have
       been too complex and we don't want to bother.  The bottom of
       our lattice.  */
    pv_unknown,

    /* A known constant.  K is its value.  */
    pv_constant,

    /* The value that register REG originally had *UPON ENTRY TO THE
       FUNCTION*, plus K.  If K is zero, this means, obviously, just
       the value REG had upon entry to the function.  REG is a GDB
       register number.  Before we start interpreting, we initialize
       every register R to { pv_register, R, 0 }.  */
    pv_register,

  } kind;

  /* The meanings of the following fields depend on 'kind'; see the
     comments for the specific 'kind' values.  */
  int reg;
  CORE_ADDR k;
};


/* Set V to be unknown.  */
static void
pv_set_to_unknown (struct prologue_value *v)
{
  v->kind = pv_unknown;
}


/* Set V to the constant K.  */
static void
pv_set_to_constant (struct prologue_value *v, CORE_ADDR k)
{
  v->kind = pv_constant;
  v->k = k;
}


/* Set V to the original value of register REG, plus K.  */
static void
pv_set_to_register (struct prologue_value *v, int reg, CORE_ADDR k)
{
  v->kind = pv_register;
  v->reg = reg;
  v->k = k;
}


/* If one of *A and *B is a constant, and the other isn't, swap the
   pointers as necessary to ensure that *B points to the constant.
   This can reduce the number of cases we need to analyze in the
   functions below.  */
static void
pv_constant_last (struct prologue_value **a,
                  struct prologue_value **b)
{
  if ((*a)->kind == pv_constant
      && (*b)->kind != pv_constant)
    {
      struct prologue_value *temp = *a;
      *a = *b;
      *b = temp;
    }
}


/* Set SUM to the sum of A and B.  SUM, A, and B may point to the same
   'struct prologue_value' object.  */
static void
pv_add (struct prologue_value *sum,
        struct prologue_value *a,
        struct prologue_value *b)
{
  pv_constant_last (&a, &b);

  /* We can handle adding constants to registers, and other constants.  */
  if (b->kind == pv_constant
      && (a->kind == pv_register
          || a->kind == pv_constant))
    {
      sum->kind = a->kind;
      sum->reg = a->reg;    /* not meaningful if a is pv_constant, but
                               harmless */
      sum->k = a->k + b->k;
    }

  /* Anything else we don't know how to add.  We don't have a
     representation for, say, the sum of two registers, or a multiple
     of a register's value (adding a register to itself).  */
  else
    sum->kind = pv_unknown;
}


/* Add the constant K to V.  */
static void
pv_add_constant (struct prologue_value *v, CORE_ADDR k)
{
  struct prologue_value pv_k;

  /* Rather than thinking of all the cases we can and can't handle,
     we'll just let pv_add take care of that for us.  */
  pv_set_to_constant (&pv_k, k);
  pv_add (v, v, &pv_k);
}


/* Subtract B from A, and put the result in DIFF.

   This isn't quite the same as negating B and adding it to A, since
   we don't have a representation for the negation of anything but a
   constant.  For example, we can't negate { pv_register, R1, 10 },
   but we do know that { pv_register, R1, 10 } minus { pv_register,
   R1, 5 } is { pv_constant, <ignored>, 5 }.

   This means, for example, that we can subtract two stack addresses;
   they're both relative to the original SP.  Since the frame pointer
   is set based on the SP, its value will be the original SP plus some
   constant (probably zero), so we can use its value just fine.  */
static void
pv_subtract (struct prologue_value *diff,
             struct prologue_value *a,
             struct prologue_value *b)
{
  pv_constant_last (&a, &b);

  /* We can subtract a constant from another constant, or from a
     register.  */
  if (b->kind == pv_constant
      && (a->kind == pv_register
          || a->kind == pv_constant))
    {
      diff->kind = a->kind;
      diff->reg = a->reg;    /* not always meaningful, but harmless */
      diff->k = a->k - b->k;
    }

  /* We can subtract a register from itself, yielding a constant.  */
  else if (a->kind == pv_register
           && b->kind == pv_register
           && a->reg == b->reg)
    {
      diff->kind = pv_constant;
      diff->k = a->k - b->k;
    }

  /* We don't know how to subtract anything else.  */
  else
    diff->kind = pv_unknown;
}


/* Set AND to the logical and of A and B.  */
static void
pv_logical_and (struct prologue_value *and,
                struct prologue_value *a,
                struct prologue_value *b)
{
  pv_constant_last (&a, &b);

  /* We can 'and' two constants.  */
  if (a->kind == pv_constant
      && b->kind == pv_constant)
    {
      and->kind = pv_constant;
      and->k = a->k & b->k;
    }

  /* We can 'and' anything with the constant zero.  */
  else if (b->kind == pv_constant
           && b->k == 0)
    {
      and->kind = pv_constant;
      and->k = 0;
    }
  
  /* We can 'and' anything with ~0.  */
  else if (b->kind == pv_constant
           && b->k == ~ (CORE_ADDR) 0)
    *and = *a;

  /* We can 'and' a register with itself.  */
  else if (a->kind == pv_register
           && b->kind == pv_register
           && a->reg == b->reg
           && a->k == b->k)
    *and = *a;

  /* Otherwise, we don't know.  */
  else
    pv_set_to_unknown (and);
}


/* Return non-zero iff A and B are identical expressions.

   This is not the same as asking if the two values are equal; the
   result of such a comparison would have to be a pv_boolean, and
   asking whether two 'unknown' values were equal would give you
   pv_maybe.  Same for comparing, say, { pv_register, R1, 0 } and {
   pv_register, R2, 0}.  Instead, this is asking whether the two
   representations are the same.  */
static int
pv_is_identical (struct prologue_value *a,
                 struct prologue_value *b)
{
  if (a->kind != b->kind)
    return 0;

  switch (a->kind)
    {
    case pv_unknown:
      return 1;
    case pv_constant:
      return (a->k == b->k);
    case pv_register:
      return (a->reg == b->reg && a->k == b->k);
    default:
      gdb_assert (0);
    }
}


/* Return non-zero if A is the original value of register number R
   plus K, zero otherwise.  */
static int
pv_is_register (struct prologue_value *a, int r, CORE_ADDR k)
{
  return (a->kind == pv_register
          && a->reg == r
          && a->k == k);
}


/* A prologue-value-esque boolean type, including "maybe", when we
   can't figure out whether something is true or not.  */
enum pv_boolean {
  pv_maybe,
  pv_definite_yes,
  pv_definite_no,
};


/* Decide whether a reference to SIZE bytes at ADDR refers exactly to
   an element of an array.  The array starts at ARRAY_ADDR, and has
   ARRAY_LEN values of ELT_SIZE bytes each.  If ADDR definitely does
   refer to an array element, set *I to the index of the referenced
   element in the array, and return pv_definite_yes.  If it definitely
   doesn't, return pv_definite_no.  If we can't tell, return pv_maybe.

   If the reference does touch the array, but doesn't fall exactly on
   an element boundary, or doesn't refer to the whole element, return
   pv_maybe.  */
static enum pv_boolean
pv_is_array_ref (struct prologue_value *addr,
                 CORE_ADDR size,
                 struct prologue_value *array_addr,
                 CORE_ADDR array_len, 
                 CORE_ADDR elt_size,
                 int *i)
{
  struct prologue_value offset;

  /* Note that, since ->k is a CORE_ADDR, and CORE_ADDR is unsigned,
     if addr is *before* the start of the array, then this isn't going
     to be negative...  */
  pv_subtract (&offset, addr, array_addr);

  if (offset.kind == pv_constant)
    {
      /* This is a rather odd test.  We want to know if the SIZE bytes
         at ADDR don't overlap the array at all, so you'd expect it to
         be an || expression: "if we're completely before || we're
         completely after".  But with unsigned arithmetic, things are
         different: since it's a number circle, not a number line, the
         right values for offset.k are actually one contiguous range.  */
      if (offset.k <= -size
          && offset.k >= array_len * elt_size)
        return pv_definite_no;
      else if (offset.k % elt_size != 0
               || size != elt_size)
        return pv_maybe;
      else
        {
          *i = offset.k / elt_size;
          return pv_definite_yes;
        }
    }
  else
    return pv_maybe;
}



/* Decoding S/390 instructions.  */

/* Named opcode values for the S/390 instructions we recognize.  Some
   instructions have their opcode split across two fields; those are the
   op1_* and op2_* enums.  */
enum
  {
    op1_aghi = 0xa7,   op2_aghi = 0xb,
    op1_ahi  = 0xa7,   op2_ahi  = 0xa,
    op_ar    = 0x1a,
    op_basr  = 0x0d,
    op1_bras = 0xa7,   op2_bras = 0x5,
    op_l     = 0x58,
    op_la    = 0x41,
    op1_larl = 0xc0,   op2_larl = 0x0,
    op_lgr   = 0xb904,
    op1_lghi = 0xa7,   op2_lghi = 0x9,
    op1_lhi  = 0xa7,   op2_lhi  = 0x8,
    op_lr    = 0x18,
    op_nr    = 0x14,
    op_ngr   = 0xb980,
    op_s     = 0x5b,
    op_st    = 0x50,
    op_std   = 0x60,
    op1_stg  = 0xe3,   op2_stg  = 0x24,
    op_stm   = 0x90,
    op1_stmg = 0xeb,   op2_stmg = 0x24,
    op_lm    = 0x98,
    op1_lmg  = 0xeb,   op2_lmg  = 0x04,
    op_svc   = 0x0a,
  };


/* The functions below are for recognizing and decoding S/390
   instructions of various formats.  Each of them checks whether INSN
   is an instruction of the given format, with the specified opcodes.
   If it is, it sets the remaining arguments to the values of the
   instruction's fields, and returns a non-zero value; otherwise, it
   returns zero.

   These functions' arguments appear in the order they appear in the
   instruction, not in the machine-language form.  So, opcodes always
   come first, even though they're sometimes scattered around the
   instructions.  And displacements appear before base and extension
   registers, as they do in the assembly syntax, not at the end, as
   they do in the machine language.  */
static int
is_ri (bfd_byte *insn, int op1, int op2, unsigned int *r1, int *i2)
{
  if (insn[0] == op1 && (insn[1] & 0xf) == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      /* i2 is a 16-bit signed quantity.  */
      *i2 = (((insn[2] << 8) | insn[3]) ^ 0x8000) - 0x8000;
      return 1;
    }
  else
    return 0;
}


static int
is_ril (bfd_byte *insn, int op1, int op2,
        unsigned int *r1, int *i2)
{
  if (insn[0] == op1 && (insn[1] & 0xf) == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      /* i2 is a signed quantity.  If the host 'int' is 32 bits long,
         no sign extension is necessary, but we don't want to assume
         that.  */
      *i2 = (((insn[2] << 24)
              | (insn[3] << 16)
              | (insn[4] << 8)
              | (insn[5])) ^ 0x80000000) - 0x80000000;
      return 1;
    }
  else
    return 0;
}


static int
is_rr (bfd_byte *insn, int op, unsigned int *r1, unsigned int *r2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r2 = insn[1] & 0xf;
      return 1;
    }
  else
    return 0;
}


static int
is_rre (bfd_byte *insn, int op, unsigned int *r1, unsigned int *r2)
{
  if (((insn[0] << 8) | insn[1]) == op)
    {
      /* Yes, insn[3].  insn[2] is unused in RRE format.  */
      *r1 = (insn[3] >> 4) & 0xf;
      *r2 = insn[3] & 0xf;
      return 1;
    }
  else
    return 0;
}


static int
is_rs (bfd_byte *insn, int op,
       unsigned int *r1, unsigned int *r3, unsigned int *d2, unsigned int *b2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


static int
is_rse (bfd_byte *insn, int op1, int op2,
        unsigned int *r1, unsigned int *r3, unsigned int *d2, unsigned int *b2)
{
  if (insn[0] == op1
      /* Yes, insn[5].  insn[4] is unused.  */
      && insn[5] == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


static int
is_rx (bfd_byte *insn, int op,
       unsigned int *r1, unsigned int *d2, unsigned int *x2, unsigned int *b2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *x2 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


static int
is_rxe (bfd_byte *insn, int op1, int op2,
        unsigned int *r1, unsigned int *d2, unsigned int *x2, unsigned int *b2)
{
  if (insn[0] == op1
      /* Yes, insn[5].  insn[4] is unused.  */
      && insn[5] == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *x2 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


/* Set ADDR to the effective address for an X-style instruction, like:

        L R1, D2(X2, B2)

   Here, X2 and B2 are registers, and D2 is an unsigned 12-bit
   constant; the effective address is the sum of all three.  If either
   X2 or B2 are zero, then it doesn't contribute to the sum --- this
   means that r0 can't be used as either X2 or B2.

   GPR is an array of general register values, indexed by GPR number,
   not GDB register number.  */
static void
compute_x_addr (struct prologue_value *addr, 
                struct prologue_value *gpr,
                unsigned int d2, unsigned int x2, unsigned int b2)
{
  /* We can't just add stuff directly in addr; it might alias some of
     the registers we need to read.  */
  struct prologue_value result;

  pv_set_to_constant (&result, d2);
  if (x2)
    pv_add (&result, &result, &gpr[x2]);
  if (b2)
    pv_add (&result, &result, &gpr[b2]);

  *addr = result;
}


/* The number of GPR and FPR spill slots in an S/390 stack frame.  We
   track general-purpose registers r2 -- r15, and floating-point
   registers f0, f2, f4, and f6.  */
#define S390_NUM_SPILL_SLOTS (14 + 4)
#define S390_NUM_GPRS 16
#define S390_NUM_FPRS 16


/* If the SIZE bytes at ADDR are a stack slot we're actually tracking,
   return pv_definite_yes and set *STACK to point to the slot.  If
   we're sure that they are not any of our stack slots, then return
   pv_definite_no.  Otherwise, return pv_maybe.
   - GPR is an array indexed by GPR number giving the current values
     of the general-purpose registers.
   - SPILL is an array tracking the spill area of the caller's frame;
     SPILL[i] is the i'th spill slot.  The spill slots are designated
     for r2 -- r15, and then f0, f2, f4, and f6.
   - BACK_CHAIN is the value of the back chain slot; it's only valid
     when the current frame actually has some space for a back chain
     slot --- that is, when the current value of the stack pointer
     (according to GPR) is at least S390_STACK_FRAME_OVERHEAD bytes
     less than its original value.  */
static enum pv_boolean
s390_on_stack (struct prologue_value *addr,
               CORE_ADDR size,
               struct prologue_value *gpr,
               struct prologue_value *spill, 
               struct prologue_value *back_chain,
               struct prologue_value **stack)
{
  struct prologue_value gpr_spill_addr;
  struct prologue_value fpr_spill_addr;
  struct prologue_value back_chain_addr;  
  int i;
  enum pv_boolean b;

  /* Construct the addresses of the spill arrays and the back chain.  */
  pv_set_to_register (&gpr_spill_addr, S390_SP_REGNUM, 2 * S390_GPR_SIZE);
  pv_set_to_register (&fpr_spill_addr, S390_SP_REGNUM, 16 * S390_GPR_SIZE);
  back_chain_addr = gpr[S390_SP_REGNUM - S390_R0_REGNUM];

  /* We have to check for GPR and FPR references using two separate
     calls to pv_is_array_ref, since the GPR and FPR spill slots are
     different sizes.  (SPILL is an array, but the thing it tracks
     isn't really an array.)  */

  /* Was it a reference to the GPR spill array?  */
  b = pv_is_array_ref (addr, size, &gpr_spill_addr, 14, S390_GPR_SIZE, &i);
  if (b == pv_definite_yes)
    {
      *stack = &spill[i];
      return pv_definite_yes;
    }
  if (b == pv_maybe)
    return pv_maybe;

  /* Was it a reference to the FPR spill array?  */
  b = pv_is_array_ref (addr, size, &fpr_spill_addr, 4, S390_FPR_SIZE, &i);
  if (b == pv_definite_yes)
    {
      *stack = &spill[14 + i];
      return pv_definite_yes;
    }
  if (b == pv_maybe)
    return pv_maybe;

  /* Was it a reference to the back chain?
     This isn't quite right.  We ought to check whether we have
     actually allocated any new frame at all.  */
  b = pv_is_array_ref (addr, size, &back_chain_addr, 1, S390_GPR_SIZE, &i);
  if (b == pv_definite_yes)
    {
      *stack = back_chain;
      return pv_definite_yes;
    }
  if (b == pv_maybe)
    return pv_maybe;

  /* All the above queries returned definite 'no's.  */
  return pv_definite_no;
}


/* Do a SIZE-byte store of VALUE to ADDR.  GPR, SPILL, and BACK_CHAIN,
   and the return value are as described for s390_on_stack, above.
   Note that, when this returns pv_maybe, we have to assume that all
   of our memory now contains unknown values.  */
static enum pv_boolean
s390_store (struct prologue_value *addr,
            CORE_ADDR size,
            struct prologue_value *value,
            struct prologue_value *gpr,
            struct prologue_value *spill, 
            struct prologue_value *back_chain)
{
  struct prologue_value *stack;
  enum pv_boolean on_stack
    = s390_on_stack (addr, size, gpr, spill, back_chain, &stack);

  if (on_stack == pv_definite_yes)
    *stack = *value;

  return on_stack;
}
            

/* The current frame looks like a signal delivery frame: the first
   instruction is an 'svc' opcode.  If the next frame is a signal
   handler's frame, set FI's saved register map to point into the
   signal context structure.  */
static void
s390_get_signal_frame_info (struct frame_info *fi)
{
  struct frame_info *next_frame = get_next_frame (fi);

  if (next_frame
      && get_frame_extra_info (next_frame)
      && get_frame_extra_info (next_frame)->sigcontext)
    {
      /* We're definitely backtracing from a signal handler.  */
      CORE_ADDR *saved_regs = deprecated_get_frame_saved_regs (fi);
      CORE_ADDR save_reg_addr = (get_frame_extra_info (next_frame)->sigcontext
                                 + DEPRECATED_REGISTER_BYTE (S390_R0_REGNUM));
      int reg;

      for (reg = 0; reg < S390_NUM_GPRS; reg++)
        {
          saved_regs[S390_R0_REGNUM + reg] = save_reg_addr;
          save_reg_addr += S390_GPR_SIZE;
        }

      save_reg_addr = (get_frame_extra_info (next_frame)->sigcontext
                       + (GDB_TARGET_IS_ESAME ? S390X_SIGREGS_FP0_OFFSET :
                          S390_SIGREGS_FP0_OFFSET));
      for (reg = 0; reg < S390_NUM_FPRS; reg++)
        {
          saved_regs[S390_F0_REGNUM + reg] = save_reg_addr;
          save_reg_addr += S390_FPR_SIZE;
        }
    }
}


static int
s390_get_frame_info (CORE_ADDR start_pc,
                     struct frame_extra_info *fextra_info,
                     struct frame_info *fi,
                     int init_extra_info)
{
  /* Our return value:
     zero if we were able to read all the instructions we wanted, or
     -1 if we got an error trying to read memory.  */
  int result = 0;

  /* The current PC for our abstract interpretation.  */
  CORE_ADDR pc;

  /* The address of the next instruction after that.  */
  CORE_ADDR next_pc;
  
  /* The general-purpose registers.  */
  struct prologue_value gpr[S390_NUM_GPRS];

  /* The floating-point registers.  */
  struct prologue_value fpr[S390_NUM_FPRS];

  /* The register spill stack slots in the caller's frame ---
     general-purpose registers r2 through r15, and floating-point
     registers.  spill[i] is where gpr i+2 gets spilled;
     spill[(14, 15, 16, 17)] is where (f0, f2, f4, f6) get spilled.  */
  struct prologue_value spill[S390_NUM_SPILL_SLOTS];

  /* The value of the back chain slot.  This is only valid if the stack
     pointer is known to be less than its original value --- that is,
     if we have indeed allocated space on the stack.  */
  struct prologue_value back_chain;

  /* The address of the instruction after the last one that changed
     the SP, FP, or back chain.  */
  CORE_ADDR after_last_frame_setup_insn = start_pc;

  /* Set up everything's initial value.  */
  {
    int i;

    for (i = 0; i < S390_NUM_GPRS; i++)
      pv_set_to_register (&gpr[i], S390_R0_REGNUM + i, 0);

    for (i = 0; i < S390_NUM_FPRS; i++)
      pv_set_to_register (&fpr[i], S390_F0_REGNUM + i, 0);

    for (i = 0; i < S390_NUM_SPILL_SLOTS; i++)
      pv_set_to_unknown (&spill[i]);

    pv_set_to_unknown (&back_chain);
  }

  /* Start interpreting instructions, until we hit something we don't
     know how to interpret.  (Ideally, we should stop at the frame's
     real current PC, but at the moment, our callers don't give us
     that info.)  */
  for (pc = start_pc; ; pc = next_pc)
    {
      bfd_byte insn[S390_MAX_INSTR_SIZE];
      int insn_len = s390_readinstruction (insn, pc);

      /* Fields for various kinds of instructions.  */
      unsigned int b2, r1, r2, d2, x2, r3;
      int i2;

      /* The values of SP, FP, and back chain before this instruction,
         for detecting instructions that change them.  */
      struct prologue_value pre_insn_sp, pre_insn_fp, pre_insn_back_chain;

      /* If we got an error trying to read the instruction, report it.  */
      if (insn_len < 0)
        {
          result = -1;
          break;
        }

      next_pc = pc + insn_len;

      pre_insn_sp = gpr[S390_SP_REGNUM - S390_R0_REGNUM];
      pre_insn_fp = gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];
      pre_insn_back_chain = back_chain;

      /* A special case, first --- only recognized as the very first
         instruction of the function, for signal delivery frames:
         SVC i --- system call  */
      if (pc == start_pc
          && is_rr (insn, op_svc, &r1, &r2))
        {
          if (fi)
            s390_get_signal_frame_info (fi);
          break;
        }
        
      /* AHI r1, i2 --- add halfword immediate */
      else if (is_ri (insn, op1_ahi, op2_ahi, &r1, &i2))
        pv_add_constant (&gpr[r1], i2);


      /* AGHI r1, i2 --- add halfword immediate (64-bit version) */
      else if (GDB_TARGET_IS_ESAME
               && is_ri (insn, op1_aghi, op2_aghi, &r1, &i2))
        pv_add_constant (&gpr[r1], i2);

      /* AR r1, r2 -- add register */
      else if (is_rr (insn, op_ar, &r1, &r2))
        pv_add (&gpr[r1], &gpr[r1], &gpr[r2]);

      /* BASR r1, 0 --- branch and save
         Since r2 is zero, this saves the PC in r1, but doesn't branch.  */
      else if (is_rr (insn, op_basr, &r1, &r2)
               && r2 == 0)
        pv_set_to_constant (&gpr[r1], next_pc);

      /* BRAS r1, i2 --- branch relative and save */
      else if (is_ri (insn, op1_bras, op2_bras, &r1, &i2))
        {
          pv_set_to_constant (&gpr[r1], next_pc);
          next_pc = pc + i2 * 2;

          /* We'd better not interpret any backward branches.  We'll
             never terminate.  */
          if (next_pc <= pc)
            break;
        }

      /* L r1, d2(x2, b2) --- load */
      else if (is_rx (insn, op_l, &r1, &d2, &x2, &b2))
        {
          struct prologue_value addr;
          struct prologue_value *stack;

          compute_x_addr (&addr, gpr, d2, x2, b2);

          /* If it's a load from an in-line constant pool, then we can
             simulate that, under the assumption that the code isn't
             going to change between the time the processor actually
             executed it creating the current frame, and the time when
             we're analyzing the code to unwind past that frame.  */
          if (addr.kind == pv_constant
              && start_pc <= addr.k 
              && addr.k < next_pc)
            pv_set_to_constant (&gpr[r1], 
                                read_memory_integer (addr.k, 4));

          /* If it's definitely a reference to something on the stack, 
             we can do that.  */
          else if (s390_on_stack (&addr, 4, gpr, spill, &back_chain, &stack)
                   == pv_definite_yes)
            gpr[r1] = *stack;

          /* Otherwise, we don't know the value.  */
          else
            pv_set_to_unknown (&gpr[r1]);
        }

      /* LA r1, d2(x2, b2) --- load address */
      else if (is_rx (insn, op_la, &r1, &d2, &x2, &b2))
        compute_x_addr (&gpr[r1], gpr, d2, x2, b2);

      /* LARL r1, i2 --- load address relative long */
      else if (GDB_TARGET_IS_ESAME 
               && is_ril (insn, op1_larl, op2_larl, &r1, &i2))
        pv_set_to_constant (&gpr[r1], pc + i2 * 2);

      /* LGR r1, r2 --- load from register */
      else if (GDB_TARGET_IS_ESAME
               && is_rre (insn, op_lgr, &r1, &r2))
        gpr[r1] = gpr[r2];

      /* LHI r1, i2 --- load halfword immediate */
      else if (is_ri (insn, op1_lhi, op2_lhi, &r1, &i2))
        pv_set_to_constant (&gpr[r1], i2);

      /* LGHI r1, i2 --- load halfword immediate --- 64-bit version */
      else if (is_ri (insn, op1_lghi, op2_lghi, &r1, &i2))
        pv_set_to_constant (&gpr[r1], i2);

      /* LR r1, r2 --- load from register */
      else if (is_rr (insn, op_lr, &r1, &r2))
        gpr[r1] = gpr[r2];

      /* NGR r1, r2 --- logical and --- 64-bit version */
      else if (GDB_TARGET_IS_ESAME
               && is_rre (insn, op_ngr, &r1, &r2))
        pv_logical_and (&gpr[r1], &gpr[r1], &gpr[r2]);

      /* NR r1, r2 --- logical and */
      else if (is_rr (insn, op_nr, &r1, &r2))
        pv_logical_and (&gpr[r1], &gpr[r1], &gpr[r2]);

      /* NGR r1, r2 --- logical and --- 64-bit version */
      else if (GDB_TARGET_IS_ESAME
               && is_rre (insn, op_ngr, &r1, &r2))
        pv_logical_and (&gpr[r1], &gpr[r1], &gpr[r2]);

      /* NR r1, r2 --- logical and */
      else if (is_rr (insn, op_nr, &r1, &r2))
        pv_logical_and (&gpr[r1], &gpr[r1], &gpr[r2]);

      /* S r1, d2(x2, b2) --- subtract from memory */
      else if (is_rx (insn, op_s, &r1, &d2, &x2, &b2))
        {
          struct prologue_value addr;
          struct prologue_value value;
          struct prologue_value *stack;

          compute_x_addr (&addr, gpr, d2, x2, b2);

          /* If it's a load from an in-line constant pool, then we can
             simulate that, under the assumption that the code isn't
             going to change between the time the processor actually
             executed it and the time when we're analyzing it.  */
          if (addr.kind == pv_constant
              && start_pc <= addr.k 
              && addr.k < pc)
            pv_set_to_constant (&value, read_memory_integer (addr.k, 4));

          /* If it's definitely a reference to something on the stack,
             we could do that.  */
          else if (s390_on_stack (&addr, 4, gpr, spill, &back_chain, &stack)
                   == pv_definite_yes)
            value = *stack;

          /* Otherwise, we don't know the value.  */
          else
            pv_set_to_unknown (&value);

          pv_subtract (&gpr[r1], &gpr[r1], &value);
        }

      /* ST r1, d2(x2, b2) --- store  */
      else if (is_rx (insn, op_st, &r1, &d2, &x2, &b2))
        {
          struct prologue_value addr;

          compute_x_addr (&addr, gpr, d2, x2, b2);

          /* The below really should be '4', not 'S390_GPR_SIZE'; this
             instruction always stores 32 bits, regardless of the full
             size of the GPR.  */
          if (s390_store (&addr, 4, &gpr[r1], gpr, spill, &back_chain)
              == pv_maybe)
            /* If we can't be sure that it's *not* a store to
               something we're tracing, then we would have to mark all
               our memory as unknown --- after all, it *could* be a
               store to any of them --- so we might as well just stop
               interpreting.  */
            break;
        }

      /* STD r1, d2(x2,b2) --- store floating-point register  */
      else if (is_rx (insn, op_std, &r1, &d2, &x2, &b2))
        {
          struct prologue_value addr;

          compute_x_addr (&addr, gpr, d2, x2, b2);

          if (s390_store (&addr, 8, &fpr[r1], gpr, spill, &back_chain)
              == pv_maybe)
            /* If we can't be sure that it's *not* a store to
               something we're tracing, then we would have to mark all
               our memory as unknown --- after all, it *could* be a
               store to any of them --- so we might as well just stop
               interpreting.  */
            break;
        }

      /* STG r1, d2(x2, b2) --- 64-bit store */
      else if (GDB_TARGET_IS_ESAME
               && is_rxe (insn, op1_stg, op2_stg, &r1, &d2, &x2, &b2))
        {
          struct prologue_value addr;

          compute_x_addr (&addr, gpr, d2, x2, b2);

          /* The below really should be '8', not 'S390_GPR_SIZE'; this
             instruction always stores 64 bits, regardless of the full
             size of the GPR.  */
          if (s390_store (&addr, 8, &gpr[r1], gpr, spill, &back_chain)
              == pv_maybe)
            /* If we can't be sure that it's *not* a store to
               something we're tracing, then we would have to mark all
               our memory as unknown --- after all, it *could* be a
               store to any of them --- so we might as well just stop
               interpreting.  */
            break;
        }

      /* STM r1, r3, d2(b2) --- store multiple */
      else if (is_rs (insn, op_stm, &r1, &r3, &d2, &b2))
        {
          int regnum;
          int offset;
          struct prologue_value addr;

          for (regnum = r1, offset = 0;
               regnum <= r3;
               regnum++, offset += 4)
            {
              compute_x_addr (&addr, gpr, d2 + offset, 0, b2);
              
              if (s390_store (&addr, 4, &gpr[regnum], gpr, spill, &back_chain)
                  == pv_maybe)
                /* If we can't be sure that it's *not* a store to
                   something we're tracing, then we would have to mark all
                   our memory as unknown --- after all, it *could* be a
                   store to any of them --- so we might as well just stop
                   interpreting.  */
                break;
            }

          /* If we left the loop early, we should stop interpreting
             altogether.  */
          if (regnum <= r3)
            break;
        }

      /* STMG r1, r3, d2(b2) --- store multiple, 64-bit */
      else if (GDB_TARGET_IS_ESAME
               && is_rse (insn, op1_stmg, op2_stmg, &r1, &r3, &d2, &b2))
        {
          int regnum;
          int offset;
          struct prologue_value addr;

          for (regnum = r1, offset = 0;
               regnum <= r3;
               regnum++, offset += 8)
            {
              compute_x_addr (&addr, gpr, d2 + offset, 0, b2);
              
              if (s390_store (&addr, 8, &gpr[regnum], gpr, spill, &back_chain)
                  == pv_maybe)
                /* If we can't be sure that it's *not* a store to
                   something we're tracing, then we would have to mark all
                   our memory as unknown --- after all, it *could* be a
                   store to any of them --- so we might as well just stop
                   interpreting.  */
                break;
            }

          /* If we left the loop early, we should stop interpreting
             altogether.  */
          if (regnum <= r3)
            break;
        }

      else
        /* An instruction we don't know how to simulate.  The only
           safe thing to do would be to set every value we're tracking
           to 'unknown'.  Instead, we'll be optimistic: we just stop
           interpreting, and assume that the machine state we've got
           now is good enough for unwinding the stack.  */
        break;

      /* Record the address after the last instruction that changed
         the FP, SP, or backlink.  Ignore instructions that changed
         them back to their original values --- those are probably
         restore instructions.  (The back chain is never restored,
         just popped.)  */
      {
        struct prologue_value *sp = &gpr[S390_SP_REGNUM - S390_R0_REGNUM];
        struct prologue_value *fp = &gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];
        
        if ((! pv_is_identical (&pre_insn_sp, sp)
             && ! pv_is_register (sp, S390_SP_REGNUM, 0))
            || (! pv_is_identical (&pre_insn_fp, fp)
                && ! pv_is_register (fp, S390_FRAME_REGNUM, 0))
            || ! pv_is_identical (&pre_insn_back_chain, &back_chain))
          after_last_frame_setup_insn = next_pc;
      }
    }

  /* Okay, now gpr[], fpr[], spill[], and back_chain reflect the state
     of the machine as of the first instruction we couldn't interpret
     (hopefully the first non-prologue instruction).  */
  {
    /* The size of the frame, or (CORE_ADDR) -1 if we couldn't figure
       that out.  */
    CORE_ADDR frame_size = -1;

    /* The value the SP had upon entry to the function, or
       (CORE_ADDR) -1 if we can't figure that out.  */
    CORE_ADDR original_sp = -1;

    /* Are we using S390_FRAME_REGNUM as a frame pointer register?  */
    int using_frame_pointer = 0;

    /* If S390_FRAME_REGNUM is some constant offset from the SP, then
       that strongly suggests that we're going to use that as our
       frame pointer register, not the SP.  */
    {
      struct prologue_value *fp = &gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];

      if (fp->kind == pv_register
          && fp->reg == S390_SP_REGNUM)
        using_frame_pointer = 1;
    }

    /* If we were given a frame_info structure, we may be able to use
       the frame's base address to figure out the actual value of the
       original SP.  */
    if (fi && get_frame_base (fi))
      {
        int frame_base_regno;
        struct prologue_value *frame_base;

        /* The meaning of the frame base depends on whether the
           function uses a frame pointer register other than the SP or
           not (see s390_read_fp):
           - If the function does use a frame pointer register other
             than the SP, then the frame base is that register's
             value.
           - If the function doesn't use a frame pointer, then the
             frame base is the SP itself.
           We're duplicating some of the logic of s390_fp_regnum here,
           but we don't want to call that, because it would just do
           exactly the same analysis we've already done above.  */
        if (using_frame_pointer)
          frame_base_regno = S390_FRAME_REGNUM;
        else
          frame_base_regno = S390_SP_REGNUM;

        frame_base = &gpr[frame_base_regno - S390_R0_REGNUM];

        /* We know the frame base address; if the value of whatever
           register it came from is a constant offset from the
           original SP, then we can reconstruct the original SP just
           by subtracting off that constant.  */
        if (frame_base->kind == pv_register
            && frame_base->reg == S390_SP_REGNUM)
          original_sp = get_frame_base (fi) - frame_base->k;
      }

    /* If the analysis said that the current SP value is the original
       value less some constant, then that constant is the frame size.  */
    {
      struct prologue_value *sp = &gpr[S390_SP_REGNUM - S390_R0_REGNUM];

      if (sp->kind == pv_register
          && sp->reg == S390_SP_REGNUM)
        frame_size = -sp->k;
    }

    /* If we knew other registers' current values, we could check if
       the analysis said any of those were related to the original SP
       value, too.  But for now, we'll just punt.  */

    /* If the caller passed in an 'extra info' structure, fill in the
       parts we can.  */
    if (fextra_info)
      {
        if (init_extra_info || ! fextra_info->initialised)
          {
            s390_memset_extra_info (fextra_info);
            fextra_info->function_start = start_pc;
            fextra_info->initialised = 1;
          }

        if (frame_size != -1)
          {
            fextra_info->stack_bought_valid = 1;
            fextra_info->stack_bought = frame_size;
          }

        /* Assume everything was okay, and indicate otherwise when we
           find something amiss.  */
        fextra_info->good_prologue = 1;

        if (using_frame_pointer)
          /* Actually, nobody cares about the exact PC, so any
             non-zero value will do here.  */
          fextra_info->frame_pointer_saved_pc = 1;

        /* If we weren't able to find the size of the frame, or find
           the original sp based on actual current register values,
           then we're not going to be able to unwind this frame.

           (If we're just doing prologue analysis to set a breakpoint,
           then frame_size might be known, but original_sp unknown; if
           we're analyzing a real frame which uses alloca, then
           original_sp might be known (from the frame pointer
           register), but the frame size might be unknown.)  */
        if (original_sp == -1 && frame_size == -1)
          fextra_info->good_prologue = 0;
        
        if (fextra_info->good_prologue)
          fextra_info->skip_prologue_function_start
            = after_last_frame_setup_insn;
        else 
          /* If the prologue was too complex for us to make sense of,
             then perhaps it's better to just not skip anything at
             all.  */
          fextra_info->skip_prologue_function_start = start_pc;
      }

    /* Indicate where registers were saved on the stack, if:
       - the caller seems to want to know,
       - the caller provided an actual SP, and
       - the analysis gave us enough information to actually figure it
         out.  */
    if (fi
        && deprecated_get_frame_saved_regs (fi)
        && original_sp != -1)
      {
        int slot_num;
        CORE_ADDR slot_addr;
        CORE_ADDR *saved_regs = deprecated_get_frame_saved_regs (fi);

        /* Scan the spill array; if a spill slot says it holds the
           original value of some register, then record that slot's
           address as the place that register was saved.

           Just for kicks, note that, even if registers aren't saved
           in their officially-sanctioned slots, this will still work
           --- we know what really got put where.  */

        /* First, the slots for r2 -- r15.  */
        for (slot_num = 0, slot_addr = original_sp + 2 * S390_GPR_SIZE;
             slot_num < 14;
             slot_num++, slot_addr += S390_GPR_SIZE)
          {
            struct prologue_value *slot = &spill[slot_num];

            if (slot->kind == pv_register
                && slot->k == 0)
              saved_regs[slot->reg] = slot_addr;
          }

        /* Then, the slots for f0, f2, f4, and f6.  They're a
           different size.  */
        for (slot_num = 14, slot_addr = original_sp + 16 * S390_GPR_SIZE;
             slot_num < S390_NUM_SPILL_SLOTS;
             slot_num++, slot_addr += S390_FPR_SIZE)
          {
            struct prologue_value *slot = &spill[slot_num];
            
            if (slot->kind == pv_register
                && slot->k == 0)
              saved_regs[slot->reg] = slot_addr;
          }

        /* The stack pointer's element of saved_regs[] is special.  */
        saved_regs[S390_SP_REGNUM] = original_sp;
      }
  }

  return result;
}

/* Return true if we are in the functin's epilogue, i.e. after the
   instruction that destroyed the function's stack frame.  */
static int
s390_in_function_epilogue_p (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;

  /* In frameless functions, there's not frame to destroy and thus
     we don't care about the epilogue.

     In functions with frame, the epilogue sequence is a pair of
     a LM-type instruction that restores (amongst others) the
     return register %r14 and the stack pointer %r15, followed
     by a branch 'br %r14' --or equivalent-- that effects the
     actual return.

     In that situation, this function needs to return 'true' in
     exactly one case: when pc points to that branch instruction.

     Thus we try to disassemble the one instructions immediately
     preceeding pc and check whether it is an LM-type instruction
     modifying the stack pointer.

     Note that disassembling backwards is not reliable, so there
     is a slight chance of false positives here ...  */

  bfd_byte insn[6];
  unsigned int r1, r3, b2;
  int d2;

  if (word_size == 4
      && !read_memory_nobpt (pc - 4, insn, 4)
      && is_rs (insn, op_lm, &r1, &r3, &d2, &b2)
      && r3 == S390_SP_REGNUM - S390_R0_REGNUM)
    return 1;

  if (word_size == 8
      && !read_memory_nobpt (pc - 6, insn, 6)
      && is_rse (insn, op1_lmg, op2_lmg, &r1, &r3, &d2, &b2)
      && r3 == S390_SP_REGNUM - S390_R0_REGNUM)
    return 1;

  return 0;
}

static int
s390_check_function_end (CORE_ADDR pc)
{
  bfd_byte instr[S390_MAX_INSTR_SIZE];
  int regidx, instrlen;

  instrlen = s390_readinstruction (instr, pc);
  if (instrlen < 0)
    return -1;
  /* check for BR */
  if (instrlen != 2 || instr[0] != 07 || (instr[1] >> 4) != 0xf)
    return 0;
  regidx = instr[1] & 0xf;
  /* Check for LMG or LG */
  instrlen =
    s390_readinstruction (instr, pc - (GDB_TARGET_IS_ESAME ? 6 : 4));
  if (instrlen < 0)
    return -1;
  if (GDB_TARGET_IS_ESAME)
    {

      if (instrlen != 6 || instr[0] != 0xeb || instr[5] != 0x4)
	return 0;
    }
  else if (instrlen != 4 || instr[0] != 0x98)
    {
      return 0;
    }
  if ((instr[2] >> 4) != 0xf)
    return 0;
  if (regidx == 14)
    return 1;
  instrlen = s390_readinstruction (instr, pc - (GDB_TARGET_IS_ESAME ? 12 : 8));
  if (instrlen < 0)
    return -1;
  if (GDB_TARGET_IS_ESAME)
    {
      /* Check for LG */
      if (instrlen != 6 || instr[0] != 0xe3 || instr[5] != 0x4)
	return 0;
    }
  else
    {
      /* Check for L */
      if (instrlen != 4 || instr[0] != 0x58)
	return 0;
    }
  if (instr[2] >> 4 != 0xf)
    return 0;
  if (instr[1] >> 4 != regidx)
    return 0;
  return 1;
}

static CORE_ADDR
s390_sniff_pc_function_start (CORE_ADDR pc, struct frame_info *fi)
{
  CORE_ADDR function_start, test_function_start;
  int loop_cnt, err, function_end;
  struct frame_extra_info fextra_info;
  function_start = get_pc_function_start (pc);

  if (function_start == 0)
    {
      test_function_start = pc;
      if (test_function_start & 1)
	return 0;		/* This has to be bogus */
      loop_cnt = 0;
      do
	{

	  err =
	    s390_get_frame_info (test_function_start, &fextra_info, fi, 1);
	  loop_cnt++;
	  test_function_start -= 2;
	  function_end = s390_check_function_end (test_function_start);
	}
      while (!(function_end == 1 || err || loop_cnt >= 4096 ||
	       (fextra_info.good_prologue)));
      if (fextra_info.good_prologue)
	function_start = fextra_info.function_start;
      else if (function_end == 1)
	function_start = test_function_start;
    }
  return function_start;
}


static int
s390_frameless_function_invocation (struct frame_info *fi)
{
  struct frame_extra_info fextra_info, *fextra_info_ptr;
  int frameless = 0;

  if (get_next_frame (fi) == NULL)		/* no may be frameless */
    {
      if (get_frame_extra_info (fi))
	fextra_info_ptr = get_frame_extra_info (fi);
      else
	{
	  fextra_info_ptr = &fextra_info;
	  s390_get_frame_info (s390_sniff_pc_function_start (get_frame_pc (fi), fi),
			       fextra_info_ptr, fi, 1);
	}
      frameless = (fextra_info_ptr->stack_bought_valid
                   && fextra_info_ptr->stack_bought == 0);
    }
  return frameless;

}


static int
s390_is_sigreturn (CORE_ADDR pc, struct frame_info *sighandler_fi,
		   CORE_ADDR *sregs, CORE_ADDR *sigcaller_pc)
{
  bfd_byte instr[S390_MAX_INSTR_SIZE];
  int instrlen;
  CORE_ADDR scontext;
  int retval = 0;
  CORE_ADDR orig_sp;
  CORE_ADDR temp_sregs;

  scontext = temp_sregs = 0;

  instrlen = s390_readinstruction (instr, pc);
  if (sigcaller_pc)
    *sigcaller_pc = 0;
  if (((instrlen == S390_SYSCALL_SIZE) &&
       (instr[0] == S390_SYSCALL_OPCODE)) &&
      ((instr[1] == s390_NR_sigreturn) || (instr[1] == s390_NR_rt_sigreturn)))
    {
      if (sighandler_fi)
	{
	  if (s390_frameless_function_invocation (sighandler_fi))
	    orig_sp = get_frame_base (sighandler_fi);
	  else
	    orig_sp = ADDR_BITS_REMOVE ((CORE_ADDR)
					read_memory_integer (get_frame_base (sighandler_fi),
							     S390_GPR_SIZE));
	  if (orig_sp && sigcaller_pc)
	    {
	      scontext = orig_sp + S390_SIGNAL_FRAMESIZE;
	      if (pc == scontext && instr[1] == s390_NR_rt_sigreturn)
		{
		  /* We got a new style rt_signal */
		  /* get address of read ucontext->uc_mcontext */
		  temp_sregs = orig_sp + (GDB_TARGET_IS_ESAME ?
					  S390X_UC_MCONTEXT_OFFSET :
					  S390_UC_MCONTEXT_OFFSET);
		}
	      else
		{
		  /* read sigcontext->sregs */
		  temp_sregs = ADDR_BITS_REMOVE ((CORE_ADDR)
						 read_memory_integer (scontext
								      +
								      (GDB_TARGET_IS_ESAME
								       ?
								       S390X_SIGCONTEXT_SREGS_OFFSET
								       :
								       S390_SIGCONTEXT_SREGS_OFFSET),
								      S390_GPR_SIZE));

		}
	      /* read sigregs->psw.addr */
	      *sigcaller_pc =
		ADDR_BITS_REMOVE ((CORE_ADDR)
				  read_memory_integer (temp_sregs +
						       DEPRECATED_REGISTER_BYTE (S390_PSWA_REGNUM),
						       S390_GPR_SIZE));
	    }
	}
      retval = 1;
    }
  if (sregs)
    *sregs = temp_sregs;
  return retval;
}

/*
  We need to do something better here but this will keep us out of trouble
  for the moment.
  For some reason the blockframe.c calls us with fi->next->fromleaf
  so this seems of little use to us. */
static CORE_ADDR
s390_init_frame_pc_first (int next_fromleaf, struct frame_info *fi)
{
  CORE_ADDR sigcaller_pc;
  CORE_ADDR pc = 0;
  if (next_fromleaf)
    {
      pc = ADDR_BITS_REMOVE (read_register (S390_RETADDR_REGNUM));
      /* fix signal handlers */
    }
  else if (get_next_frame (fi) && get_frame_pc (get_next_frame (fi)))
    pc = s390_frame_saved_pc_nofix (get_next_frame (fi));
  if (pc && get_next_frame (fi) && get_frame_base (get_next_frame (fi))
      && s390_is_sigreturn (pc, get_next_frame (fi), NULL, &sigcaller_pc))
    {
      pc = sigcaller_pc;
    }
  return pc;
}

static void
s390_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  frame_extra_info_zalloc (fi, sizeof (struct frame_extra_info));
  if (get_frame_pc (fi))
    s390_get_frame_info (s390_sniff_pc_function_start (get_frame_pc (fi), fi),
			 get_frame_extra_info (fi), fi, 1);
  else
    s390_memset_extra_info (get_frame_extra_info (fi));
}

/* If saved registers of frame FI are not known yet, read and cache them.
   &FEXTRA_INFOP contains struct frame_extra_info; TDATAP can be NULL,
   in which case the framedata are read.  */

static void
s390_frame_init_saved_regs (struct frame_info *fi)
{

  int quick;

  if (deprecated_get_frame_saved_regs (fi) == NULL)
    {
      /* zalloc memsets the saved regs */
      frame_saved_regs_zalloc (fi);
      if (get_frame_pc (fi))
	{
	  quick = (get_frame_extra_info (fi)
		   && get_frame_extra_info (fi)->initialised
		   && get_frame_extra_info (fi)->good_prologue);
	  s390_get_frame_info (quick
			       ? get_frame_extra_info (fi)->function_start
			       : s390_sniff_pc_function_start (get_frame_pc (fi), fi),
			       get_frame_extra_info (fi), fi, !quick);
	}
    }
}



static CORE_ADDR
s390_frame_saved_pc_nofix (struct frame_info *fi)
{
  if (get_frame_extra_info (fi) && get_frame_extra_info (fi)->saved_pc_valid)
    return get_frame_extra_info (fi)->saved_pc;

  if (deprecated_generic_find_dummy_frame (get_frame_pc (fi),
					   get_frame_base (fi)))
    return deprecated_read_register_dummy (get_frame_pc (fi),
					   get_frame_base (fi), S390_PC_REGNUM);

  s390_frame_init_saved_regs (fi);
  if (get_frame_extra_info (fi))
    {
      get_frame_extra_info (fi)->saved_pc_valid = 1;
      if (get_frame_extra_info (fi)->good_prologue
          && deprecated_get_frame_saved_regs (fi)[S390_RETADDR_REGNUM])
        get_frame_extra_info (fi)->saved_pc
          = ADDR_BITS_REMOVE (read_memory_integer
                              (deprecated_get_frame_saved_regs (fi)[S390_RETADDR_REGNUM],
                               S390_GPR_SIZE));
      else
        get_frame_extra_info (fi)->saved_pc
          = ADDR_BITS_REMOVE (read_register (S390_RETADDR_REGNUM));
      return get_frame_extra_info (fi)->saved_pc;
    }
  return 0;
}

static CORE_ADDR
s390_frame_saved_pc (struct frame_info *fi)
{
  CORE_ADDR saved_pc = 0, sig_pc;

  if (get_frame_extra_info (fi)
      && get_frame_extra_info (fi)->sig_fixed_saved_pc_valid)
    return get_frame_extra_info (fi)->sig_fixed_saved_pc;
  saved_pc = s390_frame_saved_pc_nofix (fi);

  if (get_frame_extra_info (fi))
    {
      get_frame_extra_info (fi)->sig_fixed_saved_pc_valid = 1;
      if (saved_pc)
	{
	  if (s390_is_sigreturn (saved_pc, fi, NULL, &sig_pc))
	    saved_pc = sig_pc;
	}
      get_frame_extra_info (fi)->sig_fixed_saved_pc = saved_pc;
    }
  return saved_pc;
}




/* We want backtraces out of signal handlers so we don't set
   (get_frame_type (thisframe) == SIGTRAMP_FRAME) to 1 */

static CORE_ADDR
s390_frame_chain (struct frame_info *thisframe)
{
  CORE_ADDR prev_fp = 0;

  if (deprecated_generic_find_dummy_frame (get_frame_pc (thisframe),
					   get_frame_base (thisframe)))
    return deprecated_read_register_dummy (get_frame_pc (thisframe),
					   get_frame_base (thisframe),
					   S390_SP_REGNUM);
  else
    {
      int sigreturn = 0;
      CORE_ADDR sregs = 0;
      struct frame_extra_info prev_fextra_info;

      memset (&prev_fextra_info, 0, sizeof (prev_fextra_info));
      if (get_frame_pc (thisframe))
	{
	  CORE_ADDR saved_pc, sig_pc;

	  saved_pc = s390_frame_saved_pc_nofix (thisframe);
	  if (saved_pc)
	    {
	      if ((sigreturn =
		   s390_is_sigreturn (saved_pc, thisframe, &sregs, &sig_pc)))
		saved_pc = sig_pc;
	      s390_get_frame_info (s390_sniff_pc_function_start
				   (saved_pc, NULL), &prev_fextra_info, NULL,
				   1);
	    }
	}
      if (sigreturn)
	{
	  /* read sigregs,regs.gprs[11 or 15] */
	  prev_fp = read_memory_integer (sregs +
					 DEPRECATED_REGISTER_BYTE (S390_R0_REGNUM +
							(prev_fextra_info.
							 frame_pointer_saved_pc
							 ? 11 : 15)),
					 S390_GPR_SIZE);
	  get_frame_extra_info (thisframe)->sigcontext = sregs;
	}
      else
	{
	  if (deprecated_get_frame_saved_regs (thisframe))
	    {
	      int regno;

              if (prev_fextra_info.frame_pointer_saved_pc
                  && deprecated_get_frame_saved_regs (thisframe)[S390_FRAME_REGNUM])
                regno = S390_FRAME_REGNUM;
              else
                regno = S390_SP_REGNUM;

	      if (deprecated_get_frame_saved_regs (thisframe)[regno])
                {
                  /* The SP's entry of `saved_regs' is special.  */
                  if (regno == S390_SP_REGNUM)
                    prev_fp = deprecated_get_frame_saved_regs (thisframe)[regno];
                  else
                    prev_fp =
                      read_memory_integer (deprecated_get_frame_saved_regs (thisframe)[regno],
                                           S390_GPR_SIZE);
                }
	    }
	}
    }
  return ADDR_BITS_REMOVE (prev_fp);
}

/*
  Whether struct frame_extra_info is actually needed I'll have to figure
  out as our frames are similar to rs6000 there is a possibility
  i386 dosen't need it. */

/* Not the most efficent code in the world */
static int
s390_fp_regnum (void)
{
  int regno = S390_SP_REGNUM;
  struct frame_extra_info fextra_info;

  CORE_ADDR pc = ADDR_BITS_REMOVE (read_register (S390_PC_REGNUM));

  s390_get_frame_info (s390_sniff_pc_function_start (pc, NULL), &fextra_info,
		       NULL, 1);
  if (fextra_info.frame_pointer_saved_pc)
    regno = S390_FRAME_REGNUM;
  return regno;
}

static CORE_ADDR
s390_read_fp (void)
{
  return read_register (s390_fp_regnum ());
}


static void
s390_pop_frame_regular (struct frame_info *frame)
{
  int regnum;

  write_register (S390_PC_REGNUM, DEPRECATED_FRAME_SAVED_PC (frame));

  /* Restore any saved registers.  */
  if (deprecated_get_frame_saved_regs (frame))
    {
      for (regnum = 0; regnum < NUM_REGS; regnum++)
        if (deprecated_get_frame_saved_regs (frame)[regnum] != 0)
          {
            ULONGEST value;
            
            value = read_memory_unsigned_integer (deprecated_get_frame_saved_regs (frame)[regnum],
                                                  DEPRECATED_REGISTER_RAW_SIZE (regnum));
            write_register (regnum, value);
          }

      /* Actually cut back the stack.  Remember that the SP's element of
         saved_regs is the old SP itself, not the address at which it is
         saved.  */
      write_register (S390_SP_REGNUM, deprecated_get_frame_saved_regs (frame)[S390_SP_REGNUM]);
    }

  /* Throw away any cached frame information.  */
  flush_cached_frames ();
}


/* Destroy the innermost (Top-Of-Stack) stack frame, restoring the 
   machine state that was in effect before the frame was created. 
   Used in the contexts of the "return" command, and of 
   target function calls from the debugger.  */
static void
s390_pop_frame (void)
{
  /* This function checks for and handles generic dummy frames, and
     calls back to our function for ordinary frames.  */
  generic_pop_current_frame (s390_pop_frame_regular);
}


/* Dummy function calls.  */

/* Return non-zero if TYPE is an integer-like type, zero otherwise.
   "Integer-like" types are those that should be passed the way
   integers are: integers, enums, ranges, characters, and booleans.  */
static int
is_integer_like (struct type *type)
{
  enum type_code code = TYPE_CODE (type);

  return (code == TYPE_CODE_INT
          || code == TYPE_CODE_ENUM
          || code == TYPE_CODE_RANGE
          || code == TYPE_CODE_CHAR
          || code == TYPE_CODE_BOOL);
}

/* Return non-zero if TYPE is a pointer-like type, zero otherwise.
   "Pointer-like" types are those that should be passed the way
   pointers are: pointers and references.  */
static int
is_pointer_like (struct type *type)
{
  enum type_code code = TYPE_CODE (type);

  return (code == TYPE_CODE_PTR
          || code == TYPE_CODE_REF);
}


/* Return non-zero if TYPE is a `float singleton' or `double
   singleton', zero otherwise.

   A `T singleton' is a struct type with one member, whose type is
   either T or a `T singleton'.  So, the following are all float
   singletons:

   struct { float x };
   struct { struct { float x; } x; };
   struct { struct { struct { float x; } x; } x; };

   ... and so on.

   All such structures are passed as if they were floats or doubles,
   as the (revised) ABI says.  */
static int
is_float_singleton (struct type *type)
{
  if (TYPE_CODE (type) == TYPE_CODE_STRUCT && TYPE_NFIELDS (type) == 1)
    {
      struct type *singleton_type = TYPE_FIELD_TYPE (type, 0);
      CHECK_TYPEDEF (singleton_type);

      return (TYPE_CODE (singleton_type) == TYPE_CODE_FLT
	      || is_float_singleton (singleton_type));
    }

  return 0;
}


/* Return non-zero if TYPE is a struct-like type, zero otherwise.
   "Struct-like" types are those that should be passed as structs are:
   structs and unions.

   As an odd quirk, not mentioned in the ABI, GCC passes float and
   double singletons as if they were a plain float, double, etc.  (The
   corresponding union types are handled normally.)  So we exclude
   those types here.  *shrug* */
static int
is_struct_like (struct type *type)
{
  enum type_code code = TYPE_CODE (type);

  return (code == TYPE_CODE_UNION
          || (code == TYPE_CODE_STRUCT && ! is_float_singleton (type)));
}


/* Return non-zero if TYPE is a float-like type, zero otherwise.
   "Float-like" types are those that should be passed as
   floating-point values are.

   You'd think this would just be floats, doubles, long doubles, etc.
   But as an odd quirk, not mentioned in the ABI, GCC passes float and
   double singletons as if they were a plain float, double, etc.  (The
   corresponding union types are handled normally.)  So we include
   those types here.  *shrug* */
static int
is_float_like (struct type *type)
{
  return (TYPE_CODE (type) == TYPE_CODE_FLT
          || is_float_singleton (type));
}


static int
is_power_of_two (unsigned int n)
{
  return ((n & (n - 1)) == 0);
}

/* Return non-zero if TYPE should be passed as a pointer to a copy,
   zero otherwise.  */
static int
s390_function_arg_pass_by_reference (struct type *type)
{
  unsigned length = TYPE_LENGTH (type);
  if (length > 8)
    return 1;

  /* FIXME: All complex and vector types are also returned by reference.  */
  return is_struct_like (type) && !is_power_of_two (length);
}

/* Return non-zero if TYPE should be passed in a float register
   if possible.  */
static int
s390_function_arg_float (struct type *type)
{
  unsigned length = TYPE_LENGTH (type);
  if (length > 8)
    return 0;

  return is_float_like (type);
}

/* Return non-zero if TYPE should be passed in an integer register
   (or a pair of integer registers) if possible.  */
static int
s390_function_arg_integer (struct type *type)
{
  unsigned length = TYPE_LENGTH (type);
  if (length > 8)
    return 0;

   return is_integer_like (type)
	  || is_pointer_like (type)
	  || (is_struct_like (type) && is_power_of_two (length));
}

/* Return ARG, a `SIMPLE_ARG', sign-extended or zero-extended to a full
   word as required for the ABI.  */
static LONGEST
extend_simple_arg (struct value *arg)
{
  struct type *type = VALUE_TYPE (arg);

  /* Even structs get passed in the least significant bits of the
     register / memory word.  It's not really right to extract them as
     an integer, but it does take care of the extension.  */
  if (TYPE_UNSIGNED (type))
    return extract_unsigned_integer (VALUE_CONTENTS (arg),
                                     TYPE_LENGTH (type));
  else
    return extract_signed_integer (VALUE_CONTENTS (arg),
                                   TYPE_LENGTH (type));
}


/* Return the alignment required by TYPE.  */
static int
alignment_of (struct type *type)
{
  int alignment;

  if (is_integer_like (type)
      || is_pointer_like (type)
      || TYPE_CODE (type) == TYPE_CODE_FLT)
    alignment = TYPE_LENGTH (type);
  else if (TYPE_CODE (type) == TYPE_CODE_STRUCT
           || TYPE_CODE (type) == TYPE_CODE_UNION)
    {
      int i;

      alignment = 1;
      for (i = 0; i < TYPE_NFIELDS (type); i++)
        {
          int field_alignment = alignment_of (TYPE_FIELD_TYPE (type, i));

          if (field_alignment > alignment)
            alignment = field_alignment;
        }
    }
  else
    alignment = 1;

  /* Check that everything we ever return is a power of two.  Lots of
     code doesn't want to deal with aligning things to arbitrary
     boundaries.  */
  gdb_assert ((alignment & (alignment - 1)) == 0);

  return alignment;
}


/* Put the actual parameter values pointed to by ARGS[0..NARGS-1] in
   place to be passed to a function, as specified by the "GNU/Linux
   for S/390 ELF Application Binary Interface Supplement".

   SP is the current stack pointer.  We must put arguments, links,
   padding, etc. whereever they belong, and return the new stack
   pointer value.
   
   If STRUCT_RETURN is non-zero, then the function we're calling is
   going to return a structure by value; STRUCT_ADDR is the address of
   a block we've allocated for it on the stack.

   Our caller has taken care of any type promotions needed to satisfy
   prototypes or the old K&R argument-passing rules.  */
static CORE_ADDR
s390_push_dummy_call (struct gdbarch *gdbarch, CORE_ADDR func_addr,
		      struct regcache *regcache, CORE_ADDR bp_addr,
		      int nargs, struct value **args, CORE_ADDR sp,
		      int struct_return, CORE_ADDR struct_addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  ULONGEST orig_sp;
  int i;

  /* If the i'th argument is passed as a reference to a copy, then
     copy_addr[i] is the address of the copy we made.  */
  CORE_ADDR *copy_addr = alloca (nargs * sizeof (CORE_ADDR));

  /* Build the reference-to-copy area.  */
  for (i = 0; i < nargs; i++)
    {
      struct value *arg = args[i];
      struct type *type = VALUE_TYPE (arg);
      unsigned length = TYPE_LENGTH (type);

      if (s390_function_arg_pass_by_reference (type))
        {
          sp -= length;
          sp = align_down (sp, alignment_of (type));
          write_memory (sp, VALUE_CONTENTS (arg), length);
          copy_addr[i] = sp;
        }
    }

  /* Reserve space for the parameter area.  As a conservative
     simplification, we assume that everything will be passed on the
     stack.  Since every argument larger than 8 bytes will be 
     passed by reference, we use this simple upper bound.  */
  sp -= nargs * 8;

  /* After all that, make sure it's still aligned on an eight-byte
     boundary.  */
  sp = align_down (sp, 8);

  /* Finally, place the actual parameters, working from SP towards
     higher addresses.  The code above is supposed to reserve enough
     space for this.  */
  {
    int fr = 0;
    int gr = 2;
    CORE_ADDR starg = sp;

    /* A struct is returned using general register 2.  */
    if (struct_return)
      {
	regcache_cooked_write_unsigned (regcache, S390_R0_REGNUM + gr,
				        struct_addr);
	gr++;
      }

    for (i = 0; i < nargs; i++)
      {
        struct value *arg = args[i];
        struct type *type = VALUE_TYPE (arg);
        unsigned length = TYPE_LENGTH (type);

	if (s390_function_arg_pass_by_reference (type))
	  {
	    if (gr <= 6)
	      {
		regcache_cooked_write_unsigned (regcache, S390_R0_REGNUM + gr,
					        copy_addr[i]);
		gr++;
	      }
	    else
	      {
		write_memory_unsigned_integer (starg, word_size, copy_addr[i]);
		starg += word_size;
	      }
	  }
	else if (s390_function_arg_float (type))
	  {
	    /* The GNU/Linux for S/390 ABI uses FPRs 0 and 2 to pass arguments,
	       the GNU/Linux for zSeries ABI uses 0, 2, 4, and 6.  */
	    if (fr <= (tdep->abi == ABI_LINUX_S390 ? 2 : 6))
	      {
		/* When we store a single-precision value in an FP register,
		   it occupies the leftmost bits.  */
		regcache_cooked_write_part (regcache, S390_F0_REGNUM + fr,
					    0, length, VALUE_CONTENTS (arg));
		fr += 2;
	      }
	    else
	      {
		/* When we store a single-precision value in a stack slot,
		   it occupies the rightmost bits.  */
		starg = align_up (starg + length, word_size);
                write_memory (starg - length, VALUE_CONTENTS (arg), length);
	      }
	  }
	else if (s390_function_arg_integer (type) && length <= word_size)
	  {
	    if (gr <= 6)
	      {
		/* Integer arguments are always extended to word size.  */
		regcache_cooked_write_signed (regcache, S390_R0_REGNUM + gr,
					      extend_simple_arg (arg));
		gr++;
	      }
	    else
	      {
		/* Integer arguments are always extended to word size.  */
		write_memory_signed_integer (starg, word_size,
                                             extend_simple_arg (arg));
                starg += word_size;
	      }
	  }
	else if (s390_function_arg_integer (type) && length == 2*word_size)
	  {
	    if (gr <= 5)
	      {
		regcache_cooked_write (regcache, S390_R0_REGNUM + gr,
				       VALUE_CONTENTS (arg));
		regcache_cooked_write (regcache, S390_R0_REGNUM + gr + 1,
				       VALUE_CONTENTS (arg) + word_size);
		gr += 2;
	      }
	    else
	      {
		/* If we skipped r6 because we couldn't fit a DOUBLE_ARG
		   in it, then don't go back and use it again later.  */
		gr = 7;

		write_memory (starg, VALUE_CONTENTS (arg), length);
		starg += length;
	      }
	  }
	else
	  internal_error (__FILE__, __LINE__, "unknown argument type");
      }
  }

  /* Allocate the standard frame areas: the register save area, the
     word reserved for the compiler (which seems kind of meaningless),
     and the back chain pointer.  */
  sp -= 16*word_size + 32;

  /* Write the back chain pointer into the first word of the stack
     frame.  This is needed to unwind across a dummy frame.  */
  regcache_cooked_read_unsigned (regcache, S390_SP_REGNUM, &orig_sp);
  write_memory_unsigned_integer (sp, word_size, orig_sp);

  /* Store return address.  */
  regcache_cooked_write_unsigned (regcache, S390_RETADDR_REGNUM, bp_addr);
  
  /* Store updated stack pointer.  */
  regcache_cooked_write_unsigned (regcache, S390_SP_REGNUM, sp);

  /* Return stack pointer.  */ 
  return sp;
}

/* Assuming NEXT_FRAME->prev is a dummy, return the frame ID of that
   dummy frame.  The frame ID's base needs to match the TOS value
   returned by push_dummy_call, and the PC match the dummy frame's
   breakpoint.  */
static struct frame_id
s390_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;
  frame_unwind_unsigned_register (next_frame, S390_SP_REGNUM, &sp);
  return frame_id_build (sp, frame_pc_unwind (next_frame));
}

static CORE_ADDR
s390_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  /* Both the 32- and 64-bit ABI's say that the stack pointer should
     always be aligned on an eight-byte boundary.  */
  return (addr & -8);
}


/* Function return value access.  */

static enum return_value_convention
s390_return_value_convention (struct gdbarch *gdbarch, struct type *type)
{
  int length = TYPE_LENGTH (type);
  if (length > 8)
    return RETURN_VALUE_STRUCT_CONVENTION;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_ARRAY:
      return RETURN_VALUE_STRUCT_CONVENTION;

    default:
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
}

static enum return_value_convention
s390_return_value (struct gdbarch *gdbarch, struct type *type, 
		   struct regcache *regcache, void *out, const void *in)
{
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  int length = TYPE_LENGTH (type);
  enum return_value_convention rvc = 
			s390_return_value_convention (gdbarch, type);
  if (in)
    {
      switch (rvc)
	{
	case RETURN_VALUE_REGISTER_CONVENTION:
	  if (TYPE_CODE (type) == TYPE_CODE_FLT)
	    {
	      /* When we store a single-precision value in an FP register,
		 it occupies the leftmost bits.  */
	      regcache_cooked_write_part (regcache, S390_F0_REGNUM, 
					  0, length, in);
	    }
	  else if (length <= word_size)
	    {
	      /* Integer arguments are always extended to word size.  */
	      if (TYPE_UNSIGNED (type))
		regcache_cooked_write_unsigned (regcache, S390_R2_REGNUM,
			extract_unsigned_integer (in, length));
	      else
		regcache_cooked_write_signed (regcache, S390_R2_REGNUM,
			extract_signed_integer (in, length));
	    }
	  else if (length == 2*word_size)
	    {
	      regcache_cooked_write (regcache, S390_R2_REGNUM, in);
	      regcache_cooked_write (regcache, S390_R3_REGNUM,
				     (const char *)in + word_size);
	    }
	  else
	    internal_error (__FILE__, __LINE__, "invalid return type");
	  break;

	case RETURN_VALUE_STRUCT_CONVENTION:
	  error ("Cannot set function return value.");
	  break;
	}
    }
  else if (out)
    {
      switch (rvc)
	{
	case RETURN_VALUE_REGISTER_CONVENTION:
	  if (TYPE_CODE (type) == TYPE_CODE_FLT)
	    {
	      /* When we store a single-precision value in an FP register,
		 it occupies the leftmost bits.  */
	      regcache_cooked_read_part (regcache, S390_F0_REGNUM, 
					 0, length, out);
	    }
	  else if (length <= word_size)
	    {
	      /* Integer arguments occupy the rightmost bits.  */
	      regcache_cooked_read_part (regcache, S390_R2_REGNUM, 
					 word_size - length, length, out);
	    }
	  else if (length == 2*word_size)
	    {
	      regcache_cooked_read (regcache, S390_R2_REGNUM, out);
	      regcache_cooked_read (regcache, S390_R3_REGNUM,
				    (char *)out + word_size);
	    }
	  else
	    internal_error (__FILE__, __LINE__, "invalid return type");
	  break;

	case RETURN_VALUE_STRUCT_CONVENTION:
	  error ("Function return value unknown.");
	  break;
	}
    }

  return rvc;
}


static const unsigned char *
s390_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  static unsigned char breakpoint[] = { 0x0, 0x1 };

  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

/* Advance PC across any function entry prologue instructions to reach some
   "real" code.  */
static CORE_ADDR
s390_skip_prologue (CORE_ADDR pc)
{
  struct frame_extra_info fextra_info;

  s390_get_frame_info (pc, &fextra_info, NULL, 1);
  return fextra_info.skip_prologue_function_start;
}

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */
static CORE_ADDR
s390_saved_pc_after_call (struct frame_info *frame)
{
  return ADDR_BITS_REMOVE (read_register (S390_RETADDR_REGNUM));
}

static CORE_ADDR
s390_addr_bits_remove (CORE_ADDR addr)
{
  return (addr) & 0x7fffffff;
}

static int
s390_address_class_type_flags (int byte_size, int dwarf2_addr_class)
{
  if (byte_size == 4)
    return TYPE_FLAG_ADDRESS_CLASS_1;
  else
    return 0;
}

static const char *
s390_address_class_type_flags_to_name (struct gdbarch *gdbarch, int type_flags)
{
  if (type_flags & TYPE_FLAG_ADDRESS_CLASS_1)
    return "mode32";
  else
    return NULL;
}

static int
s390_address_class_name_to_type_flags (struct gdbarch *gdbarch, const char *name,
				       int *type_flags_ptr)
{
  if (strcmp (name, "mode32") == 0)
    {
      *type_flags_ptr = TYPE_FLAG_ADDRESS_CLASS_1;
      return 1;
    }
  else
    return 0;
}

static struct gdbarch *
s390_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;

  /* First see if there is already a gdbarch that can satisfy the request.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* None found: is the request for a s390 architecture? */
  if (info.bfd_arch_info->arch != bfd_arch_s390)
    return NULL;		/* No; then it's not for us.  */

  /* Yes: create a new gdbarch for the specified machine type.  */
  tdep = XCALLOC (1, struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);

  /* NOTE: cagney/2002-12-06: This can be deleted when this arch is
     ready to unwind the PC first (see frame.c:get_prev_frame()).  */
  set_gdbarch_deprecated_init_frame_pc (gdbarch, deprecated_init_frame_pc_default);

  set_gdbarch_believe_pcc_promotion (gdbarch, 0);
  set_gdbarch_char_signed (gdbarch, 0);

  set_gdbarch_deprecated_frame_chain (gdbarch, s390_frame_chain);
  set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, s390_frame_init_saved_regs);
  /* Amount PC must be decremented by after a breakpoint.  This is
     often the number of bytes returned by BREAKPOINT_FROM_PC but not
     always.  */
  set_gdbarch_decr_pc_after_break (gdbarch, 2);
  set_gdbarch_deprecated_pop_frame (gdbarch, s390_pop_frame);
  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_breakpoint_from_pc (gdbarch, s390_breakpoint_from_pc);
  set_gdbarch_skip_prologue (gdbarch, s390_skip_prologue);
  set_gdbarch_deprecated_init_extra_frame_info (gdbarch, s390_init_extra_frame_info);
  set_gdbarch_deprecated_init_frame_pc_first (gdbarch, s390_init_frame_pc_first);
  set_gdbarch_deprecated_target_read_fp (gdbarch, s390_read_fp);
  set_gdbarch_in_function_epilogue_p (gdbarch, s390_in_function_epilogue_p);
  /* This function that tells us whether the function invocation represented
     by FI does not have a frame on the stack associated with it.  If it
     does not, FRAMELESS is set to 1, else 0.  */
  set_gdbarch_deprecated_frameless_function_invocation (gdbarch, s390_frameless_function_invocation);
  /* Return saved PC from a frame */
  set_gdbarch_deprecated_frame_saved_pc (gdbarch, s390_frame_saved_pc);
  /* DEPRECATED_FRAME_CHAIN takes a frame's nominal address and
     produces the frame's chain-pointer. */
  set_gdbarch_deprecated_frame_chain (gdbarch, s390_frame_chain);
  set_gdbarch_deprecated_saved_pc_after_call (gdbarch, s390_saved_pc_after_call);
  set_gdbarch_pc_regnum (gdbarch, S390_PC_REGNUM);
  set_gdbarch_sp_regnum (gdbarch, S390_SP_REGNUM);
  set_gdbarch_deprecated_fp_regnum (gdbarch, S390_SP_REGNUM);
  set_gdbarch_fp0_regnum (gdbarch, S390_F0_REGNUM);
  set_gdbarch_num_regs (gdbarch, S390_NUM_REGS);
  set_gdbarch_num_pseudo_regs (gdbarch, S390_NUM_PSEUDO_REGS);
  set_gdbarch_register_name (gdbarch, s390_register_name);
  set_gdbarch_register_type (gdbarch, s390_register_type);
  set_gdbarch_stab_reg_to_regnum (gdbarch, s390_dwarf_reg_to_regnum);
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, s390_dwarf_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, s390_dwarf_reg_to_regnum);
  set_gdbarch_convert_register_p (gdbarch, s390_convert_register_p);
  set_gdbarch_register_to_value (gdbarch, s390_register_to_value);
  set_gdbarch_value_to_register (gdbarch, s390_value_to_register);
  set_gdbarch_register_reggroup_p (gdbarch, s390_register_reggroup_p);
  set_gdbarch_regset_from_core_section (gdbarch,
                                        s390_regset_from_core_section);

  /* Inferior function calls.  */
  set_gdbarch_push_dummy_call (gdbarch, s390_push_dummy_call);
  set_gdbarch_unwind_dummy_id (gdbarch, s390_unwind_dummy_id);
  set_gdbarch_frame_align (gdbarch, s390_frame_align);
  set_gdbarch_return_value (gdbarch, s390_return_value);

  switch (info.bfd_arch_info->mach)
    {
    case bfd_mach_s390_31:
      tdep->abi = ABI_LINUX_S390;

      tdep->gregset = &s390_gregset;
      tdep->sizeof_gregset = s390_sizeof_gregset;
      tdep->fpregset = &s390_fpregset;
      tdep->sizeof_fpregset = s390_sizeof_fpregset;

      set_gdbarch_addr_bits_remove (gdbarch, s390_addr_bits_remove);
      set_gdbarch_pseudo_register_read (gdbarch, s390_pseudo_register_read);
      set_gdbarch_pseudo_register_write (gdbarch, s390_pseudo_register_write);
      break;
    case bfd_mach_s390_64:
      tdep->abi = ABI_LINUX_ZSERIES;

      tdep->gregset = &s390x_gregset;
      tdep->sizeof_gregset = s390x_sizeof_gregset;
      tdep->fpregset = &s390_fpregset;
      tdep->sizeof_fpregset = s390_sizeof_fpregset;

      set_gdbarch_long_bit (gdbarch, 64);
      set_gdbarch_long_long_bit (gdbarch, 64);
      set_gdbarch_ptr_bit (gdbarch, 64);
      set_gdbarch_pseudo_register_read (gdbarch, s390x_pseudo_register_read);
      set_gdbarch_pseudo_register_write (gdbarch, s390x_pseudo_register_write);
      set_gdbarch_address_class_type_flags (gdbarch,
                                            s390_address_class_type_flags);
      set_gdbarch_address_class_type_flags_to_name (gdbarch,
                                                    s390_address_class_type_flags_to_name);
      set_gdbarch_address_class_name_to_type_flags (gdbarch,
                                                    s390_address_class_name_to_type_flags);
      break;
    }

  set_gdbarch_print_insn (gdbarch, print_insn_s390);

  return gdbarch;
}



extern initialize_file_ftype _initialize_s390_tdep; /* -Wmissing-prototypes */

void
_initialize_s390_tdep (void)
{

  /* Hook us into the gdbarch mechanism.  */
  register_gdbarch_init (bfd_arch_s390, s390_gdbarch_init);
}
