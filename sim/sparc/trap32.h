/* sparc32 trap definitions
   Copyright (C) 1999 Cygnus Solutions.  */

#ifndef TRAP32_H
#define TRAP32_H

typedef enum
{
  /* reset */
  TRAP32_RESET = 0,
  /* unable to fetch instruction (e.g. bad mapping) */
  TRAP32_INSTRUCTION_ACCESS = 1,
  /* illegal instruction */
  TRAP32_ILLEGAL_INSN = 2,
  /* priviledged instruction */
  TRAP32_PRIVILEDGED_INSN = 3,
  /* FPU disabled */
  TRAP32_FP_DIS = 4,
  /* window overflow */
  TRAP32_WINDOW_OVERFLOW = 5,
  /* window underflow */
  TRAP32_WINDOW_UNDERFLOW = 6,
  /* unaligned memory access */
  TRAP32_UNALIGNED_ADDR = 7,
  /* unable to access memory (e.g. bad mapping) */
  TRAP32_DATA_ACCESS = 9,
  /* tag overflow (taddcctv, tsubcctv insns) */
  TRAP32_TAG_OVERFLOW = 10,
  /* coprocessor disabled */
  TRAP32_CP_DIS = 36,

  /* Implementation dependent trap types.  */
  /* ??? Borrowed concept from v9.  */
  TRAP32_IMPDEP_EXCEPTION_0 = 96,		/* n = 0..31 */

  /* IMPDEP codes used by the simulator in ENVIRONMENT_USER.  */
  TRAP32_SIM_UNIMPLEMENTED_OPCODE = 124,
  TRAP32_SIM_RESERVED_INSN = 125,
  TRAP32_SIM_SPILL = 126,
  TRAP32_SIM_FILL = 127,

  /* Traps via the "trap" instruction.  */
  TRAP32_INSTRUCTION = 128,			/* n = 0..127 */
  /* System calls.  */
  TRAP32_SYSCALL = 128,
  /* The breakpoint trap, "ta 1".  */
  TRAP32_BREAKPOINT = 129,
  /* Divide by zero.  */
  TRAP32_DIVIDE_0 = 130,
  /* Flush register windows, "ta 3".  */
  TRAP32_FLUSH_REGWIN = 131,

  TRAP32_MAX = 0xff
} TRAP32_TYPE;

/* FIXME: revisit */
#define MAX_NUM_TRAPS	256

#define TRAP32_TABLE_SIZE (32 * MAX_NUM_TRAPS)	/* in bytes */

/* The libgloss _exit routine sets a magic number in %o1 to tell us its
   him.  This lets us defer tinkering with libgloss and remain compatible
   with erc32.  */
#define LIBGLOSS_EXIT_MAGIC 0xdeadd00d

void sparc32_window_overflow (SIM_CPU *, IADDR pc_);
void sparc32_window_underflow (SIM_CPU *, IADDR pc_);

void sparc32_hw_trap (SIM_CPU *, IADDR pc_, TRAP32_TYPE trap_);
IADDR sparc32_sw_trap (SIM_CPU *, IADDR pc_, SI rs1_, SI rs2_simm13_);

IADDR sparc32_do_rett (SIM_CPU *, IADDR pc_, SI rs1_, SI rs2_simm13_);

#endif /* TRAP32_H */
