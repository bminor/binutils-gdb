/* Parameters for execution on an HP PA-RISC machine running OSF1, for GDB.
   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).  */

/* For OSF1 (Should be close if not identical to BSD, but I haven't
   tested it yet):

   The signal context structure pointer is always saved at the base
   of the frame + 0x4.

   We get the PC & SP directly from the sigcontext structure itself.
   For other registers we have to dive in a little deeper: 

   The hardware save state pointer is at offset 0x10 within the 
   signal context structure.

   Within the hardware save state, registers are found in the same order
   as the register numbers in GDB. */

#define FRAME_SAVED_PC_IN_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + 0x4, 4); \
  *(TMP) = read_memory_integer (*(TMP) + 0x18, 4); \
}

#define FRAME_BASE_BEFORE_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + 0x4, 4); \
  *(TMP) = read_memory_integer (*(TMP) + 0x8, 4); \
}

#define FRAME_FIND_SAVED_REGS_IN_SIGTRAMP(FRAME, FSR) \
{ \
  int i; \
  CORE_ADDR TMP; \
  TMP = read_memory_integer ((FRAME)->frame + 0x4, 4); \
  TMP = read_memory_integer (TMP + 0x10, 4); \
  for (i = 0; i < NUM_REGS; i++) \
    { \
      if (i == SP_REGNUM) \
	(FSR)->regs[SP_REGNUM] = read_memory_integer (TMP + SP_REGNUM * 4, 4); \
      else \
	(FSR)->regs[i] = TMP + i * 4; \
    } \
}

/* OSF1 needs an extra trap.  I assume for the emulator startup (?!?) */
#define START_INFERIOR_TRAPS_EXPECTED 3

/* OSF1 does not need the pc space queue restored.  */
#define NO_PC_SPACE_QUEUE_RESTORE

/* It's mostly just the common stuff.  */
#include "pa/tm-hppa.h"
