#define WITH_CORE
#define WITH_MODULO_MEMORY 1
#define WITH_WATCHPOINTS 1
#define WITH_TARGET_WORD_MSB 31

#include "sim-basics.h"

#include <signal.h>
typedef address_word sim_cia;


/* This simulator doesn't cache state */
#define SIM_ENGINE_HALT_HOOK(sd,last_cpu,cia) while (0)
#define SIM_ENGINE_RESTART_HOOK(sd,last_cpu,cia) while (0)

/* Get the number of instructions.  FIXME: must be a more elegant way
   of doing this.  */
#include "itable.h"
#define MAX_INSNS (nr_itable_entries)
#define INSN_NAME(i) itable[(i)].name

#include "sim-base.h"

#include "simops.h"
#include "bfd.h"


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
} v850_regs;

struct _sim_cpu
{
  /* ... simulator specific members ... */
  v850_regs reg;
  reg_t psw_mask;               /* only allow non-reserved bits to be set */
  sim_event *pending_nmi;
  /* ... base type ... */
  sim_cpu_base base;
};

#define CPU_CIA(CPU) ((CPU)->reg.pc)

struct sim_state {
  sim_cpu cpu[MAX_NR_PROCESSORS];
#if (WITH_SMP)
#define STATE_CPU(sd,n) (&(sd)->cpu[n])
#else
#define STATE_CPU(sd,n) (&(sd)->cpu[0])
#endif
#if 0
  SIM_ADDR rom_size;
  SIM_ADDR low_end;
  SIM_ADDR high_start;
  SIM_ADDR high_base;
  void *mem;
#endif
  sim_state_base base;
};

/* For compatibility, until all functions converted to passing
   SIM_DESC as an argument */
extern SIM_DESC simulator;


#define V850_ROM_SIZE 0x8000
#define V850_LOW_END 0x200000
#define V850_HIGH_START 0xffe000


#define SIG_V850_EXIT	-1	/* indication of a normal exit */


/* Because we are still using the old semantic table, provide compat
   macro's that store the instruction where the old simops expects
   it. */

extern uint32 OP[4];
#if 0
OP[0] = inst & 0x1f;           /* RRRRR -> reg1 */
OP[1] = (inst >> 11) & 0x1f;   /* rrrrr -> reg2 */
OP[2] = (inst >> 16) & 0xffff; /* wwwww -> reg3 OR imm16 */
OP[3] = inst;
#endif

#define SAVE_1 \
PC = cia; \
OP[0] = instruction_0 & 0x1f; \
OP[1] = (instruction_0 >> 11) & 0x1f; \
OP[2] = 0; \
OP[3] = instruction_0

#define COMPAT_1(CALL) \
SAVE_1; \
PC += (CALL); \
nia = PC

#define SAVE_2 \
PC = cia; \
OP[0] = instruction_0 & 0x1f; \
OP[1] = (instruction_0 >> 11) & 0x1f; \
OP[2] = instruction_1; \
OP[3] = (instruction_1 << 16) | instruction_0

#define COMPAT_2(CALL) \
SAVE_2; \
PC += (CALL); \
nia = PC


/* new */
#define GR  ((CPU)->reg.regs)
#define SR  ((CPU)->reg.sregs)

/* old */
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
#define CTPC  (SR[16])
#define CTPSW (SR[17])
/* end-sanitize-v850e */
#define DBPC  (State.sregs[18])
#define DBPSW (State.sregs[19])
/* start-sanitize-v850e */
#define CTBP  (State.sregs[20])
/* end-sanitize-v850e */

/* start-sanitize-v850eq */
#define PSW_US BIT32 (8)
/* end-sanitize-v850eq */
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

/* sign-extend a 9-bit number */
#define SEXT9(x)	((((x)&0x1ff)^(~0xff))+0x100)

/* sign-extend a 22-bit number */
#define SEXT22(x)	((((x)&0x3fffff)^(~0x1fffff))+0x200000)

/* sign extend a 40 bit number */
#define SEXT40(x)	((((x) & UNSIGNED64 (0xffffffffff)) \
			  ^ (~UNSIGNED64 (0x7fffffffff))) \
			 + UNSIGNED64 (0x8000000000))

/* sign extend a 44 bit number */
#define SEXT44(x)	((((x) & UNSIGNED64 (0xfffffffffff)) \
			  ^ (~ UNSIGNED64 (0x7ffffffffff))) \
			 + UNSIGNED64 (0x80000000000))

/* sign extend a 60 bit number */
#define SEXT60(x)	((((x) & UNSIGNED64 (0xfffffffffffffff)) \
			  ^ (~ UNSIGNED64 (0x7ffffffffffffff))) \
			 + UNSIGNED64 (0x800000000000000))

/* No sign extension */
#define NOP(x)		(x)

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

#define IMEM(EA) \
sim_core_read_aligned_2 (STATE_CPU (sd, 0), \
			 PC, sim_core_execute_map, (EA))

#define IMEM_IMMED(EA,N) \
sim_core_read_aligned_2 (STATE_CPU (sd, 0), \
			 PC, sim_core_execute_map, (EA) + (N) * 2)

#define load_mem(ADDR,LEN) \
sim_core_read_unaligned_##LEN (STATE_CPU (simulator, 0), \
			       PC, sim_core_read_map, (ADDR))

#define store_mem(ADDR,LEN,DATA) \
sim_core_write_unaligned_##LEN (STATE_CPU (simulator, 0), \
				PC, sim_core_write_map, (ADDR), (DATA))


/* compare cccc field against PSW */
int condition_met (unsigned code);


/* Debug/tracing calls */

enum op_types
{
  OP_UNKNOWN,
  OP_NONE,
  OP_TRAP,
  OP_REG,
  OP_REG_REG,
  OP_REG_REG_CMP,
  OP_REG_REG_MOVE,
  OP_IMM_REG,
  OP_IMM_REG_CMP,
  OP_IMM_REG_MOVE,
  OP_COND_BR,
  OP_LOAD16,
  OP_STORE16,
  OP_LOAD32,
  OP_STORE32,
  OP_JUMP,
  OP_IMM_REG_REG,
  OP_UIMM_REG_REG,
  OP_IMM16_REG_REG,
  OP_UIMM16_REG_REG,
  OP_BIT,
  OP_EX1,
  OP_EX2,
  OP_LDSR,
  OP_STSR,
/* start-sanitize-v850e */
  OP_BIT_CHANGE,
  OP_REG_REG_REG,
  OP_REG_REG3,
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
  OP_IMM_REG_REG_REG,
  OP_PUSHPOP1,
  OP_PUSHPOP2,
  OP_PUSHPOP3,
/* end-sanitize-v850eq */
};

#ifdef DEBUG
void trace_input PARAMS ((char *name, enum op_types type, int size));
void trace_output PARAMS ((enum op_types result));
void trace_result PARAMS ((int has_result, unsigned32 result));

extern int trace_num_values;
extern unsigned32 trace_values[];
extern unsigned32 trace_pc;
extern const char *trace_name;
extern const char *trace_module;

#define TRACE_ALU_INPUT0() \
do { \
  if (TRACE_ALU_P (CPU)) { \
    trace_module = "alu"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_num_values = 0; \
  } \
} while (0)

#define TRACE_ALU_INPUT1(IN1) \
do { \
  if (TRACE_ALU_P (CPU)) { \
    trace_module = "alu"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN1); \
    trace_num_values = 1; \
  } \
} while (0)

#define TRACE_ALU_INPUT2(IN1, IN2) \
do { \
  if (TRACE_ALU_P (CPU)) { \
    trace_module = "alu"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN1); \
    trace_values[1] = (IN2); \
    trace_num_values = 2; \
  } \
} while (0)

#define TRACE_ALU_INPUT3(IN0, IN1, IN2) \
do { \
  if (TRACE_ALU_P (CPU)) { \
    trace_module = "alu"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN0); \
    trace_values[1] = (IN1); \
    trace_values[2] = (IN2); \
    trace_num_values = 3; \
  } \
} while (0)

#define TRACE_ALU_RESULT(RESULT) \
do { \
  if (TRACE_ALU_P (CPU)) { \
    trace_result (1, (RESULT)); \
  } \
} while (0)

#define TRACE_BRANCH0() \
do { \
  if (TRACE_BRANCH_P (CPU)) { \
    trace_module = "branch"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_num_values = 0; \
    trace_result (1, (nia)); \
  } \
} while (0)

#define TRACE_BRANCH1(IN1) \
do { \
  if (TRACE_BRANCH_P (CPU)) { \
    trace_module = "branch"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN1); \
    trace_num_values = 1; \
    trace_result (1, (nia)); \
  } \
} while (0)

#define TRACE_BRANCH2(IN1, IN2) \
do { \
  if (TRACE_BRANCH_P (CPU)) { \
    trace_module = "branch"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN1); \
    trace_values[1] = (IN2); \
    trace_num_values = 2; \
    trace_result (1, (nia)); \
  } \
} while (0)

#define TRACE_BRANCH3(IN1, IN2, IN3) \
do { \
  if (TRACE_BRANCH_P (CPU)) { \
    trace_module = "branch"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (IN1); \
    trace_values[1] = (IN2); \
    trace_values[2] = (IN3); \
    trace_num_values = 3; \
    trace_result (1, (nia)); \
  } \
} while (0)

#define TRACE_LD(ADDR,RESULT) \
do { \
  if (TRACE_MEMORY_P (CPU)) { \
    trace_module = "memory"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (ADDR); \
    trace_num_values = 1; \
    trace_result (1, (RESULT)); \
  } \
} while (0)

/* start-sanitize-v850e */
#define TRACE_LD_NAME(NAME, ADDR,RESULT) \
do { \
  if (TRACE_MEMORY_P (CPU)) { \
    trace_module = "memory"; \
    trace_pc = cia; \
    trace_name = (NAME); \
    trace_values[0] = (ADDR); \
    trace_num_values = 1; \
    trace_result (1, (RESULT)); \
  } \
} while (0)

/* end-sanitize-v850e */
#define TRACE_ST(ADDR,RESULT) \
do { \
  if (TRACE_MEMORY_P (CPU)) { \
    trace_module = "memory"; \
    trace_pc = cia; \
    trace_name = itable[MY_INDEX].name; \
    trace_values[0] = (ADDR); \
    trace_num_values = 1; \
    trace_result (1, (RESULT)); \
  } \
} while (0)

#else
#define trace_input(NAME, IN1, IN2)
#define trace_output(RESULT)
#define trace_result(HAS_RESULT, RESULT)

#define TRACE_ALU_INPUT0()
#define TRACE_ALU_INPUT1(IN0)
#define TRACE_ALU_INPUT2(IN0, IN1)
#define TRACE_ALU_INPUT2(IN0, IN1)
#define TRACE_ALU_INPUT2(IN0, IN1 INS2)
#define TRACE_ALU_RESULT(RESULT)

#define TRACE_BRANCH0()
#define TRACE_BRANCH1(IN1)
#define TRACE_BRANCH2(IN1, IN2)
#define TRACE_BRANCH2(IN1, IN2, IN3)

#define TRACE_LD(ADDR,RESULT)
#define TRACE_ST(ADDR,RESULT)

#endif


/* start-sanitize-v850eq */
extern void divun ( unsigned int       N,
		    unsigned long int  als,
		    unsigned long int  sfi,
		    unsigned long int *  quotient_ptr,
		    unsigned long int *  remainder_ptr,
		    boolean *          overflow_ptr
		    );
extern void divn ( unsigned int       N,
		   unsigned long int  als,
		   unsigned long int  sfi,
		   signed long int *  quotient_ptr,
		   signed long int *  remainder_ptr,
		   boolean *          overflow_ptr
		   );
/* end-sanitize-v850eq */
/* start-sanitize-v850e */
extern int type1_regs[];
extern int type2_regs[];
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
extern int type3_regs[];
/* end-sanitize-v850eq */
