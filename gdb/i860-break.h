/* I860 -specific breakpoint stuff.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/* #define BREAKPOINT_DEBUG 1 */

#include "tm.h"

#ifdef i860
void i860_insert_breakpoints();
void i860_dbrk_breakpoints();

#define BREAKPOINT_HERE(b,pc)  ((b->act_addr[0] == pc) || (b->act_addr[1] == pc) ||(b->act_addr[2] == pc) || (b->act_addr[2] == pc))

/* This is the sequence of bytes we insert for a breakpoint.  */

static char break_insn[] = BREAKPOINT;
static char float_insn[] = BREAKFLOAT;

#endif

#define FOPMSK	0x4C000000
#define DOPMSK	0x4C000200
#define FOP860	0x48000000
#define DOP860	0x48000200
 
#define ALN32(a) ((( (int)(a) & 0x0007) == 0x04)	 )
#define ALN64(a) ((( (int)(a) & 0x0007) == 0x00)	 )
/*
#define ISFOP(a) ((((adj_read_memory_integer(a)) & FOPMSK)==FOP860 ))
#define ISCOR(a) ((!(((adj_read_memory_integer(a)) & FOPMSK)==FOP860	)))
#define ISDOP(a) ((((adj_read_memory_integer(a)) & DOPMSK)==DOP860 ))
*/

#define ISFOP(a) ((((adj_read_memory_integer(a)) & FOPMSK)==FOP860 ))
#define ISCOR(a) ((!(((adj_read_memory_integer(a)) & FOPMSK)==FOP860	)))
#define ISDOP(a) ((((adj_read_memory_integer(a)) & DOPMSK)==DOP860 ))

#define ISDIM(a) (\
 ( ISDOP(a) && ALN64(a) && ISCOR(a+4)) || 				     \
 ((ISFOP(a) && ALN64(a) && ISCOR(a+4)) && (ISDOP(a-8) || ISDOP(a-0x10))) ||  \
 ( ISCOR(a) && ALN32(a) && ISDOP(a-4)) ||				     \
 ((ISCOR(a) && ALN32(a) && ISFOP(a+4)) && (ISDOP(a-4) || ISDOP(a-0x0C)))     \
		 )
/*
#define ISDIM(a) (\
 ( ISDOP(a) && ALN64(a) && ISCOR(a+4)) || 				  \
 ((ISFOP(a) && ALN64(a) && ISCOR(a+4)) && (ISDOP(a-8) || ISDOP(a-16))) ||  \
 ( ISCOR(a) && ALN32(a) && ISDOP(a-4)) ||				  \
 ((ISCOR(a) && ALN32(a) && ISFOP(a+4)) && (ISDOP(a-4) || ISDOP(a-12)))     \
		 )
*/
#define FOPADR(a) ( ((int)(a) & 0xFFFFFFF8) )
#define CORADR(a) ((((int)(a) & 0xFFFFFFFC ) | 0x04) )

#define DMNEXT(a) ( ISDIM(a)?(ISDIM(a+1)?(a+2):(a+1)):(a+1) )
#define STDIM (((*(int *)&(registers[REGISTER_BYTE(PS_REGNUM)])) & 0x6000) == 0x2000 )
#define INDIM (((*(int *)&(registers[REGISTER_BYTE(PS_REGNUM)])) & 0x6000) == 0x4000 )
#define ENDIM (((*(int *)&(registers[REGISTER_BYTE(PS_REGNUM)])) & 0x6000) == 0x6000 )

#define	DIM 8
#define	RIM 0x8004
#define SIM 4
#define ADDR_INC(a) (((int)(a) & 0x0FFF))

#define SINGLE_STEP_MODE 1
#define BREAK_MODE	 0
