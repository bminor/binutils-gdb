#include <stdio.h>
#include <ctype.h>
#include "ansidecl.h"
#include "callback.h"
#include "opcode/v850.h"
#include <limits.h>
#include "remote-sim.h"

extern host_callback *v850_callback;

#define DEBUG_TRACE		0x00000001
#define DEBUG_VALUES		0x00000002

extern int v850_debug;

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

/* FIXME: V850 defines */
typedef uint32 reg_t;

struct simops 
{
  long opcode;
  long mask;
  void (*func)();
  int numops;
  int operands[6];
};

struct _state
{
  reg_t regs[32];		/* general-purpose registers */
  reg_t sregs[32];		/* system regsiters, including psw */
  reg_t pc;
  uint8 *mem;
  int   exception;
} State;

extern uint32 OP[4];
extern struct simops Simops[];

#define PC	(State.pc)

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

/* sign-extend a 7-bit number */
#define SEXT7(x)	((((x)&0x7f)^(~0x4f))+0x40)

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

#define MAX32	0x7fffffffLL
#define MIN32	0xff80000000LL
#define MASK32	0xffffffffLL
#define MASK40	0xffffffffffLL

#define INC_ADDR(x,i)	x = ((State.MD && x == MOD_E) ? MOD_S : (x)+(i))

/*#define	RB(x)	(*((uint8 *)((x)+State.mem)))*/
/*#define SB(addr,data)	( RB(addr) = (data & 0xff))*/

uint32 get_word PARAMS ((uint8 *));
uint16 get_half PARAMS ((uint8 *));
uint8 get_byte PARAMS ((uint8 *));
void put_word PARAMS ((uint8 *, uint32));
void put_half PARAMS ((uint8 *, uint16));
void put_byte PARAMS ((uint8 *, uint8));

uint32 load_mem PARAMS ((SIM_ADDR addr, int len));
void store_mem PARAMS ((SIM_ADDR addr, int len, uint32 data));

uint8 *map PARAMS ((SIM_ADDR addr));

#define RLW(x) load_mem (x, 4)

#ifdef _WIN32
#define SIGTRAP 5
#define SIGQUIT 3
#endif
