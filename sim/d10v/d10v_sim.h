#include "config.h"
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include "ansidecl.h"
#include "callback.h"
#include "opcode/d10v.h"
#include "bfd.h"

#define DEBUG_TRACE		0x00000001
#define DEBUG_VALUES		0x00000002
#define DEBUG_LINE_NUMBER	0x00000004
#define DEBUG_MEMSIZE		0x00000008
#define DEBUG_INSTRUCTION	0x00000010

#ifndef	DEBUG
#define	DEBUG (DEBUG_TRACE | DEBUG_VALUES | DEBUG_LINE_NUMBER)
#endif

extern int d10v_debug;

#include "sim-types.h"
typedef unsigned8 uint8;
typedef unsigned16 uint16;
typedef signed16 int16;
typedef unsigned32 uint32;
typedef signed32 int32;
typedef unsigned64 uint64;
typedef signed64 int64;

/* FIXME: D10V defines */
typedef uint16 reg_t;

struct simops 
{
  long opcode;
  int  is_long;
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
  INS_UNKNOWN,			/* unknown instruction */
  INS_COND_TRUE,		/* # times EXExxx executed other instruction */
  INS_COND_FALSE,		/* # times EXExxx did not execute other instruction */
  INS_COND_JUMP,		/* # times JUMP skipped other instruction */
  INS_CYCLES,			/* # cycles */
  INS_LONG,			/* long instruction (both containers, ie FM == 11) */
  INS_LEFTRIGHT,		/* # times instruction encoded as L -> R (ie, FM == 01) */
  INS_RIGHTLEFT,		/* # times instruction encoded as L <- R (ie, FM == 10) */
  INS_PARALLEL,			/* # times instruction encoded as L || R (ie, RM == 00) */

  INS_LEFT,			/* normal left instructions */
  INS_LEFT_PARALLEL,		/* left side of || */
  INS_LEFT_COND_TEST,		/* EXExx test on left side */
  INS_LEFT_COND_EXE,		/* execution after EXExxx test on right side succeeded */
  INS_LEFT_NOPS,		/* NOP on left side */

  INS_RIGHT,			/* normal right instructions */
  INS_RIGHT_PARALLEL,		/* right side of || */
  INS_RIGHT_COND_TEST,		/* EXExx test on right side */
  INS_RIGHT_COND_EXE,		/* execution after EXExxx test on left side succeeded */
  INS_RIGHT_NOPS,		/* NOP on right side */

  INS_MAX
};

extern unsigned long ins_type_counters[ (int)INS_MAX ];

struct _state
{
  reg_t regs[16];		/* general-purpose registers */
  reg_t cregs[16];		/* control registers */
  int64 a[2];			/* accumulators */
  uint8 SM;
  uint8 EA;
  uint8 DB;
  uint8 DM;
  uint8 IE;
  uint8 RP;
  uint8 MD;
  uint8 FX;
  uint8 ST;
  uint8 F0;
  uint8 F1;
  uint8 C;
  uint8 exe;
  int	exception;
  int	pc_changed;
  /* everything below this line is not reset by sim_create_inferior() */
  uint8 *imem;
  uint8 *dmem;
  uint8 *umem[128];
  enum _ins_type ins_type;
} State;

extern host_callback *d10v_callback;
extern uint16 OP[4];
extern struct simops Simops[];
extern asection *text;
extern bfd_vma text_start;
extern bfd_vma text_end;
extern bfd *prog_bfd;

enum
{
  PSW_CR = 0,
  BPSW_CR = 1,
  PC_CR = 2,
  BPC_CR = 3,
  RPT_C_CR = 7,
  RPT_S_CR = 8,
  RPT_E_CR = 9,
  MOD_S_CR = 10,
  MOD_E_CR = 11,
  IBA_CR = 14,
};

enum
{
  PSW_SM_BIT = 0x8000,
  PSW_EA_BIT = 0x2000,
  PSW_DB_BIT = 0x1000,
  PSW_DM_BIT = 0x0800,
  PSW_IE_BIT = 0x0400,
  PSW_RP_BIT = 0x0200,
  PSW_MD_BIT = 0x0100,
  PSW_FX_BIT = 0x0080,
  PSW_ST_BIT = 0x0040,
  PSW_F0_BIT = 0x0008,
  PSW_F1_BIT = 0x0004,
  PSW_C_BIT = 0x0001,
};

/* See simopsc.:move_to_cr() for registers that can not be read-from
   or assigned-to directly */
#define PC	(State.cregs[PC_CR])
#define PSW	(move_from_cr (PSW_CR))
#define BPSW	(0 + State.cregs[PSW_CR])
#define BPC	(State.cregs[BPC_CR])
#define RPT_C	(State.cregs[RPT_C_CR])
#define RPT_S	(State.cregs[RPT_S_CR])
#define RPT_E	(State.cregs[RPT_E_CR])
#define MOD_S	(0 + State.cregs[MOD_S_CR])
#define MOD_E	(0 + State.cregs[MOD_E_CR])
#define IBA	(State.cregs[IBA_CR])

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
#define SEXT32(x)	((((x)&SIGNED64(0xffffffff))^(~SIGNED64(0x7fffffff)))+SIGNED64(0x80000000))

/* sign extend a 40 bit number */
#define SEXT40(x)	((((x)&SIGNED64(0xffffffffff))^(~SIGNED64(0x7fffffffff)))+SIGNED64(0x8000000000))

/* sign extend a 44 bit number */
#define SEXT44(x)	((((x)&SIGNED64(0xfffffffffff))^(~SIGNED64(0x7ffffffffff)))+SIGNED64(0x80000000000))

/* sign extend a 56 bit number */
#define SEXT56(x)	((((x)&SIGNED64(0xffffffffffffff))^(~SIGNED64(0x7fffffffffffff)))+SIGNED64(0x80000000000000))

/* sign extend a 60 bit number */
#define SEXT60(x)	((((x)&SIGNED64(0xfffffffffffffff))^(~SIGNED64(0x7ffffffffffffff)))+SIGNED64(0x800000000000000))

#define MAX32	SIGNED64(0x7fffffff)
#define MIN32	SIGNED64(0xff80000000)
#define MASK32	SIGNED64(0xffffffff)
#define MASK40	SIGNED64(0xffffffffff)

/* The alignment of MOD_E in the following macro depends upon "i" always being a power of 2. */
#define INC_ADDR(x,i)	x = ((State.MD && x == (MOD_E & ~((i)-1))) ? MOD_S : (x)+(i))

extern uint8 *dmem_addr PARAMS ((uint32));
extern bfd_vma decode_pc PARAMS ((void));

#define	RB(x)	(*(dmem_addr(x)))
#define SB(addr,data)	( RB(addr) = (data & 0xff))

#if defined(__GNUC__) && defined(__OPTIMIZE__) && !defined(NO_ENDIAN_INLINE)
#define ENDIAN_INLINE static __inline__
#include "endian.c"
#undef ENDIAN_INLINE

#else
extern uint32 get_longword PARAMS ((uint8 *));
extern uint16 get_word PARAMS ((uint8 *));
extern int64 get_longlong PARAMS ((uint8 *));
extern void write_word PARAMS ((uint8 *addr, uint16 data));
extern void write_longword PARAMS ((uint8 *addr, uint32 data));
extern void write_longlong PARAMS ((uint8 *addr, int64 data));
#endif

#define SW(addr,data)		write_word(dmem_addr(addr),data)
#define RW(x)			get_word(dmem_addr(x))
#define SLW(addr,data)  	write_longword(dmem_addr(addr),data)
#define RLW(x)			get_longword(dmem_addr(x))
#define READ_16(x)		get_word(x)
#define WRITE_16(addr,data)	write_word(addr,data)
#define READ_64(x)		get_longlong(x)
#define WRITE_64(addr,data)	write_longlong(addr,data)

#define IMAP0			RW(0xff00)
#define IMAP1			RW(0xff02)
#define DMAP			RW(0xff04)
#define SET_IMAP0(x)		SW(0xff00,x)
#define SET_IMAP1(x)		SW(0xff02,x)
#define SET_DMAP(x)		SW(0xff04,x)

#define JMP(x)			{ PC = (x); State.pc_changed = 1; }

#define AE_VECTOR_START 0xffc3
#define RIE_VECTOR_START 0xffc2
#define SDBT_VECTOR_START 0xffd5
#define TRAP_VECTOR_START 0xffc4	/* vector for trap 0 */

extern void move_to_cr PARAMS ((int cr, reg_t val));
extern reg_t move_from_cr PARAMS ((int cr));
