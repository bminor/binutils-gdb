// OBSOLETE /* Parameters for execution on an HP PA-RISC level 0 embedded system.
// OBSOLETE    This is based on tm-hppab.h.
// OBSOLETE    Contributed by the Center for Software Science at the
// OBSOLETE    University of Utah (pa-gdb-bugs@cs.utah.edu).  */
// OBSOLETE 
// OBSOLETE #define PA_LEVEL_0		/* Disables touching space regs and fp */
// OBSOLETE 
// OBSOLETE /* All the PRO targets use software floating point at the moment.  */
// OBSOLETE #define SOFT_FLOAT 1
// OBSOLETE 
// OBSOLETE /* It's mostly just the common stuff.  */
// OBSOLETE #include "pa/tm-hppa.h"
// OBSOLETE 
// OBSOLETE #define GDB_TARGET_IS_PA_ELF
