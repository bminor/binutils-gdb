// OBSOLETE /* Parameters for execution on an HP PA-RISC machine running BSD, for GDB.
// OBSOLETE    Contributed by the Center for Software Science at the
// OBSOLETE    University of Utah (pa-gdb-bugs@cs.utah.edu).  */
// OBSOLETE 
// OBSOLETE /* For BSD:
// OBSOLETE 
// OBSOLETE    The signal context structure pointer is always saved at the base
// OBSOLETE    of the frame + 0x4.
// OBSOLETE 
// OBSOLETE    We get the PC & SP directly from the sigcontext structure itself.
// OBSOLETE    For other registers we have to dive in a little deeper: 
// OBSOLETE 
// OBSOLETE    The hardware save state pointer is at offset 0x10 within the 
// OBSOLETE    signal context structure.
// OBSOLETE 
// OBSOLETE    Within the hardware save state, registers are found in the same order
// OBSOLETE    as the register numbers in GDB. */
// OBSOLETE 
// OBSOLETE #define FRAME_SAVED_PC_IN_SIGTRAMP(FRAME, TMP) \
// OBSOLETE { \
// OBSOLETE   *(TMP) = read_memory_integer ((FRAME)->frame + 0x4, 4); \
// OBSOLETE   *(TMP) = read_memory_integer (*(TMP) + 0x18, 4); \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #define FRAME_BASE_BEFORE_SIGTRAMP(FRAME, TMP) \
// OBSOLETE { \
// OBSOLETE   *(TMP) = read_memory_integer ((FRAME)->frame + 0x4, 4); \
// OBSOLETE   *(TMP) = read_memory_integer (*(TMP) + 0x8, 4); \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #define FRAME_FIND_SAVED_REGS_IN_SIGTRAMP(FRAME, FSR) \
// OBSOLETE { \
// OBSOLETE   int i; \
// OBSOLETE   CORE_ADDR TMP; \
// OBSOLETE   TMP = read_memory_integer ((FRAME)->frame + 0x4, 4); \
// OBSOLETE   TMP = read_memory_integer (TMP + 0x10, 4); \
// OBSOLETE   for (i = 0; i < NUM_REGS; i++) \
// OBSOLETE     { \
// OBSOLETE       if (i == SP_REGNUM) \
// OBSOLETE 	(FSR)->regs[SP_REGNUM] = read_memory_integer (TMP + SP_REGNUM * 4, 4); \
// OBSOLETE       else \
// OBSOLETE 	(FSR)->regs[i] = TMP + i * 4; \
// OBSOLETE     } \
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* It's mostly just the common stuff.  */
// OBSOLETE #include "pa/tm-hppa.h"
