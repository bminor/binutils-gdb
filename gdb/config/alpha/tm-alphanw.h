/* GDB target definitions for Alpha running Netware. */

/* Alpha/Netware uses NT PALcode, which causes call_pal bpt to give the PC of the
   actual breakpoint, not PC+4, (like the OSF1 PALcode).  */

#define DECR_PC_AFTER_BREAK 0

#define VM_MIN_ADDRESS ((CORE_ADDR)0)

#include "alpha/tm-alpha.h"

/* Stop backtracing when we wander into main.  */

#define FRAME_CHAIN_VALID_ALTERNATE
