#include <stdio.h>
#include <ctype.h>
#include "ansidecl.h"
#include "gdb/callback.h"
#include "opcode/mn10300.h"
#include <limits.h>
#include "gdb/remote-sim.h"
#include "bfd.h"

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif
#endif

extern host_callback *mn10300_callback;
extern SIM_DESC simulator;

#define DEBUG_TRACE		0x00000001
#define DEBUG_VALUES		0x00000002

extern int mn10300_debug;

#if UCHAR_MAX == 255
typedef unsigned char uint8;
typedef signed char int8;
#else
#error "Char is not an 8-bit type"
#endif

#if SHRT_MAX == 32767
typedef unsigned short uint16;
typedef signed short int16;
#else
#error "Short is not a 16-bit type"
#endif

#if INT_MAX == 2147483647

typedef unsigned int uint32;
typedef signed int int32;

#else
#  if LONG_MAX == 2147483647

typedef unsigned long uint32;
typedef signed long int32;

#  else
#  error "Neither int nor long is a 32-bit type"
#  endif
#endif

typedef uint32 reg_t;

struct simops 
{
  long opcode;
  long mask;
  void (*func)();
  int length;
  int format;
  int numops;
  int operands[16];
};

/* The current state of the processor; registers, memory, etc.  */

struct _state
{
  reg_t regs[32];		/* registers, d0-d3, a0-a3, sp, pc, mdr, psw,
				   lir, lar, mdrq, plus some room for processor
				   specific regs.  */
  uint8 *mem;			/* main memory */
  int exception;
  int exited;

  /* All internal state modified by signal_exception() that may need to be
     rolled back for passing moment-of-exception image back to gdb. */
  reg_t exc_trigger_regs[32];
  reg_t exc_suspend_regs[32];
  int exc_suspended;

#define SIM_CPU_EXCEPTION_TRIGGER(SD,CPU,CIA) mn10300_cpu_exception_trigger(SD,CPU,CIA)
#define SIM_CPU_EXCEPTION_SUSPEND(SD,CPU,EXC) mn10300_cpu_exception_suspend(SD,CPU,EXC)
#define SIM_CPU_EXCEPTION_RESUME(SD,CPU,EXC) mn10300_cpu_exception_resume(SD,CPU,EXC)
};

extern struct _state State;
extern uint32 OP[4];
extern struct simops Simops[];

#define PC	(State.regs[REG_PC])
#define SP	(State.regs[REG_SP])

#define PSW (State.regs[11])
#define PSW_Z 0x1
#define PSW_N 0x2
#define PSW_C 0x4
#define PSW_V 0x8
#define PSW_IE LSBIT (11)
#define PSW_LM LSMASK (10, 8)

#define EXTRACT_PSW_LM LSEXTRACTED16 (PSW, 10, 8)
#define INSERT_PSW_LM(l) LSINSERTED16 ((l), 10, 8)

#define REG_D0 0
#define REG_A0 4
#define REG_SP 8
#define REG_PC 9
#define REG_MDR 10
#define REG_PSW 11
#define REG_LIR 12
#define REG_LAR 13
#define REG_MDRQ 14
#define REG_E0 15
#define REG_SSP 23
#define REG_MSP 24
#define REG_USP 25
#define REG_MCRH 26
#define REG_MCRL 27
#define REG_MCVF 28

#ifdef _WIN32
#define SIGTRAP 5
#define SIGQUIT 3
#endif

#define FETCH32(a,b,c,d) \
 ((a)+((b)<<8)+((c)<<16)+((d)<<24))

#define FETCH24(a,b,c) \
 ((a)+((b)<<8)+((c)<<16))

#define FETCH16(a,b) ((a)+((b)<<8))

#define load_byte(ADDR) \
sim_core_read_unaligned_1 (STATE_CPU (simulator, 0), PC, read_map, (ADDR))

#define load_half(ADDR) \
sim_core_read_unaligned_2 (STATE_CPU (simulator, 0), PC, read_map, (ADDR))

#define load_word(ADDR) \
sim_core_read_unaligned_4 (STATE_CPU (simulator, 0), PC, read_map, (ADDR))

#define store_byte(ADDR, DATA) \
sim_core_write_unaligned_1 (STATE_CPU (simulator, 0), \
			    PC, write_map, (ADDR), (DATA))


#define store_half(ADDR, DATA) \
sim_core_write_unaligned_2 (STATE_CPU (simulator, 0), \
			    PC, write_map, (ADDR), (DATA))


#define store_word(ADDR, DATA) \
sim_core_write_unaligned_4 (STATE_CPU (simulator, 0), \
			    PC, write_map, (ADDR), (DATA))

/* Function declarations.  */

uint32 get_word (uint8 *);
uint16 get_half (uint8 *);
uint8 get_byte (uint8 *);
void put_word (uint8 *, uint32);
void put_half (uint8 *, uint16);
void put_byte (uint8 *, uint8);

extern uint8 *map (SIM_ADDR addr);

INLINE_SIM_MAIN (void) genericAdd (unsigned32 source, unsigned32 destReg);
INLINE_SIM_MAIN (void) genericSub (unsigned32 source, unsigned32 destReg);
INLINE_SIM_MAIN (void) genericCmp (unsigned32 leftOpnd, unsigned32 rightOpnd);
INLINE_SIM_MAIN (void) genericOr (unsigned32 source, unsigned32 destReg);
INLINE_SIM_MAIN (void) genericXor (unsigned32 source, unsigned32 destReg);
INLINE_SIM_MAIN (void) genericBtst (unsigned32 leftOpnd, unsigned32 rightOpnd);
INLINE_SIM_MAIN (int) syscall_read_mem (host_callback *cb,
					struct cb_syscall *sc,
					unsigned long taddr,
					char *buf,
					int bytes);
INLINE_SIM_MAIN (int) syscall_write_mem (host_callback *cb,
					 struct cb_syscall *sc,
					 unsigned long taddr,
					 const char *buf,
					 int bytes); 
INLINE_SIM_MAIN (void) do_syscall (void);
void program_interrupt (SIM_DESC sd, sim_cpu *cpu, sim_cia cia, SIM_SIGNAL sig);

void mn10300_cpu_exception_trigger(SIM_DESC sd, sim_cpu* cpu, address_word pc);
void mn10300_cpu_exception_suspend(SIM_DESC sd, sim_cpu* cpu, int exception);
void mn10300_cpu_exception_resume(SIM_DESC sd, sim_cpu* cpu, int exception);
