/* Parameters for execution on an HP PA-RISC machine running OSF1, for GDB.
   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).  */

/* It's all just the common stuff.  */
#include "pa/tm-hppa.h"

/* OSF1 needs an extra trap.  I assume for the emulator startup (?!?) */
#define START_INFERIOR_TRAPS_EXPECTED 3
