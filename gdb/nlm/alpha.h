/* Register values.  All of these values *MUST* agree with tm.h */
#define RA_REGNUM 26		/* Contains return address value */
#define SP_REGNUM 30		/* Contains address of top of stack */
#define PC_REGNUM 64		/* Contains program counter */
#define FP_REGNUM 65		/* Virtual frame pointer */
#define V0_REGNUM 0		/* Function integer return value */
#define NUM_REGS 66		/* Number of machine registers */
#define REGISTER_BYTES (NUM_REGS * 8) /* Total size of registers array */

#define ExceptionPC ExceptionRegs[SF_REG_PC].lo
#define DECR_PC_AFTER_BREAK 0	/* NT's Palcode gets this right! */
#define BREAKPOINT {0x80, 0, 0, 0} /* call_pal bpt */
extern unsigned char breakpoint_insn[];
#define BREAKPOINT_SIZE 4

/*#define flush_i_cache() asm("call_pal 0x86")*/

/* Temporary defs until DEC gets their netware includes straightened out */

#include "altdebug.h"		/* Get StackFrame structure */

#define isspace(C) (C == '\n' || C == '\r' || C == ' ' || C == '\t')
int Breakpoint (int __arg);

typedef unsigned long ULONG;
