// OBSOLETE /* Parameters for execution on an HP PA-RISC machine running OSF1, for GDB.
// OBSOLETE    Contributed by the Center for Software Science at the
// OBSOLETE    University of Utah (pa-gdb-bugs@cs.utah.edu).  */
// OBSOLETE 
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* Define offsets to access CPROC stack when it does not have
// OBSOLETE  * a kernel thread.
// OBSOLETE  */
// OBSOLETE #define MACHINE_CPROC_SP_OFFSET 20
// OBSOLETE #define MACHINE_CPROC_PC_OFFSET 16
// OBSOLETE #define MACHINE_CPROC_FP_OFFSET 12
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * Software defined PSW masks.
// OBSOLETE  */
// OBSOLETE #define PSW_SS  0x10000000	/* Kernel managed single step */
// OBSOLETE 
// OBSOLETE /* Thread flavors used in re-setting the T bit.
// OBSOLETE  * @@ this is also bad for cross debugging.
// OBSOLETE  */
// OBSOLETE #define TRACE_FLAVOR		HP800_THREAD_STATE
// OBSOLETE #define TRACE_FLAVOR_SIZE	HP800_THREAD_STATE_COUNT
// OBSOLETE #define TRACE_SET(x,state) \
// OBSOLETE 	((struct hp800_thread_state *)state)->cr22 |= PSW_SS
// OBSOLETE #define TRACE_CLEAR(x,state) \
// OBSOLETE   	((((struct hp800_thread_state *)state)->cr22 &= ~PSW_SS), 1)
// OBSOLETE 
// OBSOLETE /* For OSF1 (Should be close if not identical to BSD, but I haven't
// OBSOLETE    tested it yet):
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
// OBSOLETE /* OSF1 does not need the pc space queue restored.  */
// OBSOLETE #define NO_PC_SPACE_QUEUE_RESTORE
// OBSOLETE 
// OBSOLETE /* The mach kernel uses the recovery counter to implement single
// OBSOLETE    stepping.  While this greatly simplifies the kernel support
// OBSOLETE    necessary for single stepping, it unfortunately does the wrong
// OBSOLETE    thing in the presense of a nullified instruction (gives control
// OBSOLETE    back two insns after the nullifed insn).  This is an artifact
// OBSOLETE    of the HP architecture (recovery counter doesn't tick for
// OBSOLETE    nullified insns).
// OBSOLETE 
// OBSOLETE    Do our best to avoid losing in such situations.  */
// OBSOLETE #define INSTRUCTION_NULLIFIED \
// OBSOLETE (({ \
// OBSOLETE     int ipsw = (int)read_register(IPSW_REGNUM); \
// OBSOLETE     if (ipsw & PSW_N)  \
// OBSOLETE       { \
// OBSOLETE         int pcoqt = (int)read_register(PCOQ_TAIL_REGNUM); \
// OBSOLETE         write_register(PCOQ_HEAD_REGNUM, pcoqt); \
// OBSOLETE         write_register(PCOQ_TAIL_REGNUM, pcoqt + 0x4); \
// OBSOLETE         write_register(IPSW_REGNUM, ipsw & ~(PSW_N | PSW_B | PSW_X)); \
// OBSOLETE         stop_pc = pcoqt; \
// OBSOLETE       } \
// OBSOLETE    }), 0)
// OBSOLETE 
// OBSOLETE /* It's mostly just the common stuff.  */
// OBSOLETE 
// OBSOLETE #include "pa/tm-hppa.h"
