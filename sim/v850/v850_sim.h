#include <stdio.h>
#include <ctype.h>
#include "ansidecl.h"
#include "opcode/v850.h"

/* FIXME: host defines */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

/* FIXME: V850 defines */
typedef uint32 reg_t;

struct simops 
{
  long opcode;
  long mask;
  void (*func)();
  int numops;
  int operands[9];
};

struct _state
{
  reg_t regs[32];		/* general-purpose registers */
  reg_t sregs[32];		/* system regsiters, including psw */
  reg_t pc;
  uint8 *imem;
  uint8 *dmem;
  int   exception;
} State;

extern uint16 OP[4];
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

#define SEXT3(x)	((((x)&0x7)^(~3))+4)	

/* sign-extend a 4-bit number */
#define SEXT4(x)	((((x)&0xf)^(~7))+8)	

/* sign-extend an 8-bit number */
#define SEXT8(x)	((((x)&0xff)^(~0x7f))+0x80)

/* sign-extend a 16-bit number */
#define SEXT16(x)	((((x)&0xffff)^(~0x7fff))+0x8000)

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

#define	RB(x)	(*((uint8 *)((x)+State.imem)))
#define SB(addr,data)	( RB(addr) = (data & 0xff))

#ifdef WORDS_BIGENDIAN

#define RW(x)			(*((uint16 *)((x)+State.imem)))
#define RLW(x)			(*((uint32 *)((x)+State.imem)))
#define SW(addr,data)		RW(addr)=data
#define READ_16(x)		(*((int16 *)(x)))
#define WRITE_16(addr,data)	(*(int16 *)(addr)=data)	
#define READ_64(x)		(*((int64 *)(x)))
#define WRITE_64(addr,data)	(*(int64 *)(addr)=data)	

#else

uint32 get_longword PARAMS ((uint8 *));
uint16 get_word PARAMS ((uint8 *));
int64 get_longlong PARAMS ((uint8 *));
void write_word PARAMS ((uint8 *addr, uint16 data));
void write_longlong PARAMS ((uint8 *addr, int64 data));

#define SW(addr,data)		write_word((long)(addr)+State.imem,data)
#define RW(x)			get_word((long)(x)+State.imem)
#define RLW(x)			get_longword((long)(x)+State.imem)
#define READ_16(x)		get_word(x)
#define WRITE_16(addr,data)	write_word(addr,data)
#define READ_64(x)		get_longlong(x)
#define WRITE_64(addr,data)	write_longlong(addr,data)

#endif /* not WORDS_BIGENDIAN */
