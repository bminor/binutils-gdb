/* sparc64 trap definitions
   Copyright (C) 1999 Cygnus Solutions.  */

#ifndef TRAP64_H
#define TRAP64_H

/* D1.2.4 page 107 */

typedef enum
{
  TRAP64_POWER_ON_RESET = 1,
  TRAP64_WATCHDOG_RESET = 2,
  TRAP64_EXTERNALLY_INITIATED_RESET = 3,
  TRAP64_SOFTWARE_INITIATED_RESET = 4,
  TRAP64_RED_STATE_EXCEPTION = 5,
  TRAP64_INSN_ACCESS_EXCEPTION = 8,
  TRAP64_INSN_ACCESS_MMU_MISS = 9,
  TRAP64_INSN_ACCESS_ERROR = 10,
  TRAP64_ILLEGAL_INSN = 16,
  TRAP64_PRIVILEDGED_OPCODE = 17,
  TRAP64_UNIMPLEMENTED_LDD = 18,
  TRAP64_UNIMPLEMENTED_STD = 19,
  TRAP64_FP_DISABLED = 32,
  TRAP64_FP_EXCEPTION_IEEE_754 = 33,
  TRAP64_FP_EXCEPTION_OTHER = 34,
  TRAP64_TAG_OVERFLOW = 35,
  TRAP64_CLEAN_WINDOW = 36,
  TRAP64_DIVISION_BY_ZERO = 40,
  TRAP64_INTERNAL_PROCESSOR_ERROR = 41,
  TRAP64_DATA_ACCESS_EXCEPTION = 48,
  TRAP64_DATA_ACCESS_MMU_MISS = 49,
  TRAP64_DATA_ACCESS_ERROR = 50,
  TRAP64_DATA_ACCESS_PROTECTION = 51,
  TRAP64_MEM_ADDRESS_NOT_ALIGNED = 52,
  TRAP64_LDDF_MEM_ADDRESS_NOT_ALIGNED = 53,	/* impdep # 109 */
  TRAP64_STDF_MEM_ADDRESS_NOT_ALIGNED = 54,	/* impdep # 110 */
  TRAP64_PRIVILEDGED_ACTION = 55,
  TRAP64_LDQF_MEM_ADDRESS_NOT_ALIGNED = 56,	/* impdep # 111 */
  TRAP64_STQF_MEM_ADDRESS_NOT_ALIGNED = 57,	/* impdep # 112 */
  TRAP64_ASYNC_DATA_ERROR = 64,
  TRAP64_INTERRUPT_LEVEL_0 = 65,		/* n = 1..15 */
  TRAP64_IMPDEP_EXCEPTION_0 = 96,		/* n = 0..31 */

  /* IMPDEP codes used by the simulator in ENVIRONMENT_USER.  */
  TRAP64_SIM_UNIMPLEMENTED_OPCODE = 124,
  TRAP64_SIM_RESERVED_INSN = 125,
  TRAP64_SIM_SPILL = 126,
  TRAP64_SIM_FILL = 127,

  TRAP64_SPILL_0_NORMAL = 128,			/* n = 0..7 */
  TRAP64_SPILL_0_OTHER = 160,			/* n = 0..7 */
  TRAP64_FILL_0_NORMAL = 192,			/* n = 0..7 */
  TRAP64_FILL_0_OTHER = 224,			/* n = 0..7 */
  TRAP64_INSTRUCTION = 256,			/* n = 0..127 */
  TRAP64_BREAKPOINT = 257,			/* convention */
  TRAP64_MAX = 0x17f
} TRAP64_TYPE;

#define MAX_NUM_TRAPS	1024

#define TRAP64_TABLE_SIZE (32 * MAX_NUM_TRAPS)	/* in bytes */

/* We record the fact that the cpu is in error state by setting TL to be
   something greater than MAXTL, usually MAXTL+1.  */

#define ERROR_STATE_P()	(GET_TL () > MAXTL)

#if 0
fastint trap (trap_type_e);
int trap_priority (trap_type_e);

fastint reserved (void);
fastint deprecated (void);
fastint not_impl (void);
fastint illegal (void);
fastint priviledged (void);
fastint unimp_fpop (void);
fastint fp_disabled (void);

/* When running user level programs, we supply all the necessary trap handlers.
   These handlers run on the host, not in the emulation environment.  */

typedef fastint (trap_handler_t) (void);
#endif

#endif /* TRAP64_H */
