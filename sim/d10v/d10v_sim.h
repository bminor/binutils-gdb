#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include "ansidecl.h"
#include "callback.h"
#include "opcode/d10v.h"

#define DEBUG_TRACE		0x00000001
#define DEBUG_VALUES		0x00000002
#define DEBUG_LINE_NUMBER	0x00000004
#define DEBUG_MEMSIZE		0x00000008
#define DEBUG_INSTRUCTION	0x00000010

#ifndef	DEBUG
#define	DEBUG (DEBUG_TRACE | DEBUG_VALUES | DEBUG_LINE_NUMBER)
#endif

extern int d10v_debug;

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

#elif LONG_MAX == 2147483647
typedef unsigned long uint32;
typedef signed long int32;

#else
#error "Neither int nor long is a 32-bit type"
#endif

#if LONG_MAX > 2147483647
typedef unsigned long uint64;
typedef signed long int64;

#elif __GNUC__
typedef unsigned long long uint64;
typedef signed long long int64;

#else
#error "Can't find an appropriate 64-bit type"
#endif

/* FIXME: D10V defines */
typedef uint16 reg_t;

struct simops 
{
  long opcode;
  long mask;
  int format;
  int cycles;
  int unit;
  int exec_type;
  void (*func)();
  int numops;
  int operands[9];
};

enum _ins_type
{
  INS_UNKNOWN,
  INS_LEFT,
  INS_RIGHT,
  INS_LEFT_PARALLEL,
  INS_RIGHT_PARALLEL,
  INS_LONG,
  INS_MAX
};

extern long ins_type_counters[ (int)INS_MAX ];
extern long left_nops, right_nops;

struct _state
{
  reg_t regs[16];		/* general-purpose registers */
  reg_t cregs[16];		/* control registers */
  int64 a[2];			/* accumulators */
  uint8 SM;
  uint8 EA;
  uint8 DB;
  uint8 IE;
  uint8 RP;
  uint8 MD;
  uint8 FX;
  uint8 ST;
  uint8 F0;
  uint8 F1;
  uint8 C;
  uint8 exe;
  uint8 *imem;
  uint8 *dmem;
  uint32 mem_min;
  uint32 mem_max;
  int   exception;
  enum _ins_type ins_type;
} State;

extern host_callback *d10v_callback;
extern uint16 OP[4];
extern struct simops Simops[];

#define PC	(State.cregs[2])
#define PSW	(State.cregs[0])
#define BPSW	(State.cregs[1])
#define BPC	(State.cregs[3])
#define RPT_C	(State.cregs[7])
#define RPT_S	(State.cregs[8])
#define RPT_E	(State.cregs[9])
#define MOD_S	(State.cregs[10])
#define MOD_E	(State.cregs[11])
#define IBA	(State.cregs[14])

#define SIG_D10V_STOP	-1
#define SIG_D10V_EXIT	-2

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

#if defined(__GNUC__) && defined(__OPTIMIZE__) && !defined(NO_ENDIAN_INLINE)
#define ENDIAN_INLINE static __inline__
#include "endian.c"
#undef ENDIAN_INLINE

#else
uint32 get_longword PARAMS ((uint8 *));
uint16 get_word PARAMS ((uint8 *));
int64 get_longlong PARAMS ((uint8 *));
void write_word PARAMS ((uint8 *addr, uint16 data));
void write_longword PARAMS ((uint8 *addr, uint32 data));
void write_longlong PARAMS ((uint8 *addr, int64 data));
#endif

#define SW(addr,data)		write_word((long)(addr)+State.imem,data)
#define RW(x)			get_word((long)(x)+State.imem)
#define SLW(addr,data)  	write_longword((long)(addr)+State.imem,data)
#define RLW(x)			get_longword((long)(x)+State.imem)
#define READ_16(x)		get_word(x)
#define WRITE_16(addr,data)	write_word(addr,data)
#define READ_64(x)		get_longlong(x)
#define WRITE_64(addr,data)	write_longlong(addr,data)
