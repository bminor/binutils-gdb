#define SIM_HAVE_FLATMEM 1

#include "sim-basics.h"

typedef address_word sim_cia;

/* This simulator doesn't cache state */
#define SIM_ENGINE_HALT_HOOK(sd,last_cpu,cia) while (0)
#define SIM_ENGINE_RESTART_HOOK(sd,last_cpu,cia) while (0)

#include "sim-base.h"

typedef signed8 int8;
typedef unsigned8 uint8;
typedef signed16 int16;
typedef unsigned16 uint16;
typedef signed32 int32;
typedef unsigned32 uint32;
typedef unsigned32 reg_t;


/* The current state of the processor; registers, memory, etc.  */

typedef struct _v850_regs {
  reg_t regs[32];		/* general-purpose registers */
  reg_t sregs[32];		/* system registers, including psw */
  reg_t pc;
  int dummy_mem;		/* where invalid accesses go */
  int exception;
  int pending_nmi;
} v850_regs;

struct _sim_cpu
{
  /* ... simulator specific members ... */
  v850_regs reg;
  /* ... base type ... */
  sim_cpu_base base;
};

struct sim_state {
  sim_cpu cpu[MAX_NR_PROCESSORS];
#if (WITH_SMP)
#define STATE_CPU(sd,n) (&(sd)->cpu[n])
#else
#define STATE_CPU(sd,n) (&(sd)->cpu[0])
#endif
  SIM_ADDR rom_size;
  SIM_ADDR low_end;
  SIM_ADDR high_start;
  SIM_ADDR high_base;
  sim_state_base base;
};

/* For compatibility, until all functions converted to passing
   SIM_DESC as an argument */
extern SIM_DESC simulator;


#define V850_ROM_SIZE 0x8000
#define V850_LOW_END 0x200000
#define V850_HIGH_START 0xffe000


#define DEBUG_TRACE		0x00000001
#define DEBUG_VALUES		0x00000002

extern int v850_debug;

#define SIG_V850_EXIT	-1	/* indication of a normal exit */

extern uint32 OP[4];
extern struct simops Simops[];

#define State    (STATE_CPU (simulator, 0)->reg)
#define PC	(State.pc)
#define SP	(State.regs[3])
#define EP	(State.regs[30])

#define EIPC  (State.sregs[0])
#define EIPSW (State.sregs[1])
#define FEPC  (State.sregs[2])
#define FEPSW (State.sregs[3])
#define ECR   (State.sregs[4])
#define PSW   (State.sregs[5])
/* start-sanitize-v850e */
#define CTPC  (State.sregs[16])
#define CTPSW (State.sregs[17])
/* end-sanitize-v850e */
#define DBPC  (State.sregs[18])
#define DBPSW (State.sregs[19])
/* start-sanitize-v850e */
#define CTBP  (State.sregs[20])
/* end-sanitize-v850e */

#define PSW_NP 0x80
#define PSW_EP 0x40
#define PSW_ID 0x20
#define PSW_SAT 0x10
#define PSW_CY 0x8
#define PSW_OV 0x4
#define PSW_S 0x2
#define PSW_Z 0x1

#define SEXT3(x)	((((x)&0x7)^(~0x3))+0x4)	

/* sign-extend a 4-bit number */
#define SEXT4(x)	((((x)&0xf)^(~0x7))+0x8)	

/* sign-extend a 5-bit number */
#define SEXT5(x)	((((x)&0x1f)^(~0xf))+0x10)	

/* sign-extend an 8-bit number */
#define SEXT8(x)	((((x)&0xff)^(~0x7f))+0x80)

/* sign-extend a 9-bit number */
#define SEXT9(x)	((((x)&0x1ff)^(~0xff))+0x100)

/* sign-extend a 16-bit number */
#define SEXT16(x)	((((x)&0xffff)^(~0x7fff))+0x8000)

/* sign-extend a 22-bit number */
#define SEXT22(x)	((((x)&0x3fffff)^(~0x1fffff))+0x200000)

/* sign-extend a 32-bit number */
#define SEXT32(x)	((((x)&0xffffffffLL)^(~0x7fffffffLL))+0x80000000LL)

/* sign extend a 40 bit number */
#define SEXT40(x)	((((x)&0xffffffffffLL)^(~0x7fffffffffLL))+0x8000000000LL)

/* sign extend a 44 bit number */
#define SEXT44(x)	((((x)&0xfffffffffffLL)^(~0x7ffffffffffLL))+0x80000000000LL)

/* sign extend a 60 bit number */
#define SEXT60(x)	((((x)&0xfffffffffffffffLL)^(~0x7ffffffffffffffLL))+0x800000000000000LL)

/* No sign extension */
#define NOP(x)		(x)

#if 0
#define MAX32	0x7fffffffLL
#define MIN32	0xff80000000LL
#define MASK32	0xffffffffLL
#define MASK40	0xffffffffffLL
#endif

#define INC_ADDR(x,i)	x = ((State.MD && x == MOD_E) ? MOD_S : (x)+(i))

#define RLW(x) load_mem (x, 4)

#ifdef _WIN32
#ifndef SIGTRAP
#define SIGTRAP 5
#endif
#ifndef SIGQUIT
#define SIGQUIT 3
#endif
#endif

/* Function declarations.  */

uint32 get_word PARAMS ((uint8 *));
uint16 get_half PARAMS ((uint8 *));
uint8 get_byte PARAMS ((uint8 *));
void put_word PARAMS ((uint8 *, uint32));
void put_half PARAMS ((uint8 *, uint16));
void put_byte PARAMS ((uint8 *, uint8));

extern uint32 load_mem PARAMS ((SIM_ADDR addr, int len));
extern void store_mem PARAMS ((SIM_ADDR addr, int len, uint32 data));

extern uint8 *map PARAMS ((SIM_ADDR addr));
