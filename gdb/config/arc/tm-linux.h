#include "config/tm-linux.h"

#define CONFIG_OSABI		GDB_OSABI_LINUX

/* Do nothing.  */
#define CONFIG_INIT_TDEP	{}

enum arc700_linux_regnums
  {
    /* Regnums 0..26 are R0..R26 */
    ARC_BTA_REGNUM	=	27,
    ARC_LP_START_REGNUM =	28,
    ARC_LP_END_REGNUM   =	29,
    ARC_LP_COUNT_REGNUM =	30,
    ARC_STATUS32_REGNUM =	31,
    ARC_BLINK_REGNUM    =	32,
    ARC_FP_REGNUM 	=	33,
    ARC_SP_REGNUM 	=	34,
    ARC_EFA_REGNUM      =	35,
    ARC_RET_REGNUM      =	36,
    ARC_ORIG_R8_REGNUM  =	37,
    ARC_STOP_PC_REGNUM  =	38
  };

#define ARC_NR_REGS	39

/* Pseudo-regs.  */
#define ARC_ILINK1_REGNUM   	(NUM_REGS)
#define ARC_ILINK2_REGNUM   	(NUM_REGS+1)
#define ARC_ERET_REGNUM     	(NUM_REGS+2)
#define ARC_STATUS32_L1_REGNUM 	(NUM_REGS+3)
#define ARC_STATUS32_L2_REGNUM 	(NUM_REGS+4)
#define ARC_ERSTATUS_REGNUM 	(NUM_REGS+5)

#define ARC_NR_PSEUDO_REGS	6
