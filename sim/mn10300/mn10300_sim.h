#include <stdio.h>
#include <ctype.h>
#include "ansidecl.h"
#include "callback.h"
#include "opcode/mn10300.h"
#include <limits.h>
#include "remote-sim.h"

extern host_callback *mn10300_callback;

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
  reg_t regs[12];		/* registers, d0-d3, a0-a3, sp, pc, mdr, psw,
				   lir, lar */
  uint8 *mem;			/* main memory */
  int exception;
} State;

extern uint32 OP[4];
extern struct simops Simops[];

#define PC	(State.regs[9])

#define PSW (State.regs[11])
#define PSW_V 0x1
#define PSW_C 0x2
#define PSW_N 0x4
#define PSW_Z 0x8

#define REG_D0 0
#define REG_A0 4
#define REG_SP 8
#define REG_PC 9
#define REG_MDR 10
#define REG_PSW 11
#define REG_LIR 12
#define REG_LAR 13

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

#define MAX32	0x7fffffffLL
#define MIN32	0xff80000000LL
#define MASK32	0xffffffffLL
#define MASK40	0xffffffffffLL

#define RLW(x) load_mem (x, 4)

#ifdef _WIN32
#define SIGTRAP 5
#define SIGQUIT 3
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
