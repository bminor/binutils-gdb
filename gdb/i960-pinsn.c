/* i80960 instruction disassembler for GDB.
   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "dis-asm.h"

/* Print the instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes.  */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     GDB_FILE *stream;
{
  disassemble_info info;

  GDB_INIT_DISASSEMBLE_INFO(info, stream);

  return print_insn_i960 (memaddr, &info);
}

/****************************************/
/* MEM format				*/
/****************************************/

struct tabent {
	char	*name;
	char	numops;
};

static int				/* returns instruction length: 4 or 8 */
mem( memaddr, word1, word2, noprint )
    unsigned long memaddr;
    unsigned long word1, word2;
    int noprint;		/* If TRUE, return instruction length, but
				   don't output any text.  */
{
	int i, j;
	int len;
	int mode;
	int offset;
	const char *reg1, *reg2, *reg3;

	/* This lookup table is too sparse to make it worth typing in, but not
	 * so large as to make a sparse array necessary.  We allocate the
	 * table at runtime, initialize all entries to empty, and copy the
	 * real ones in from an initialization table.
	 *
	 * NOTE: In this table, the meaning of 'numops' is:
	 *	 1: single operand
	 *	 2: 2 operands, load instruction
	 *	-2: 2 operands, store instruction
	 */
	static struct tabent *mem_tab = NULL;
/* Opcodes of 0x8X, 9X, aX, bX, and cX must be in the table.  */
#define MEM_MIN	0x80
#define MEM_MAX	0xcf
#define MEM_SIZ	((MEM_MAX-MEM_MIN+1) * sizeof(struct tabent))

	static struct { int opcode; char *name; char numops; } mem_init[] = {
		0x80,	"ldob",	 2,
		0x82,	"stob",	-2,
		0x84,	"bx",	 1,
		0x85,	"balx",	 2,
		0x86,	"callx", 1,
		0x88,	"ldos",	 2,
		0x8a,	"stos",	-2,
		0x8c,	"lda",	 2,
		0x90,	"ld",	 2,
		0x92,	"st",	-2,
		0x98,	"ldl",	 2,
		0x9a,	"stl",	-2,
		0xa0,	"ldt",	 2,
		0xa2,	"stt",	-2,
		0xb0,	"ldq",	 2,
		0xb2,	"stq",	-2,
		0xc0,	"ldib",	 2,
		0xc2,	"stib",	-2,
		0xc8,	"ldis",	 2,
		0xca,	"stis",	-2,
		0,	NULL,	0
	};

	if ( mem_tab == NULL ){
		mem_tab = (struct tabent *) xmalloc( MEM_SIZ );
		memset( mem_tab, '\0', MEM_SIZ );
		for ( i = 0; mem_init[i].opcode != 0; i++ ){
			j = mem_init[i].opcode - MEM_MIN;
			mem_tab[j].name = mem_init[i].name;
			mem_tab[j].numops = mem_init[i].numops;
		}
	}

	i = ((word1 >> 24) & 0xff) - MEM_MIN;
	mode = (word1 >> 10) & 0xf;

	if ( (mem_tab[i].name != NULL)		/* Valid instruction */
	&&   ((mode == 5) || (mode >=12)) ){	/* With 32-bit displacement */
		len = 8;
	} else {
		len = 4;
	}

	if ( noprint ){
		return len;
	}
	abort ();
}

/* Read the i960 instruction at 'memaddr' and return the address of 
   the next instruction after that, or 0 if 'memaddr' is not the
   address of a valid instruction.  The first word of the instruction
   is stored at 'pword1', and the second word, if any, is stored at
   'pword2'.  */

CORE_ADDR
next_insn (memaddr, pword1, pword2)
     unsigned long *pword1, *pword2;
     CORE_ADDR memaddr;
{
  int len;
  char buf[8];

  /* Read the two (potential) words of the instruction at once,
     to eliminate the overhead of two calls to read_memory ().
     FIXME: Loses if the first one is readable but the second is not
     (e.g. last word of the segment).  */

  read_memory (memaddr, buf, 8);
  *pword1 = extract_unsigned_integer (buf, 4);
  *pword2 = extract_unsigned_integer (buf + 4, 4);

  /* Divide instruction set into classes based on high 4 bits of opcode*/

  switch ((*pword1 >> 28) & 0xf)
    {
    case 0x0:
    case 0x1:	/* ctrl */

    case 0x2:
    case 0x3:	/* cobr */

    case 0x5:
    case 0x6:
    case 0x7:	/* reg */
      len = 4;
      break;

    case 0x8:
    case 0x9:
    case 0xa:
    case 0xb:
    case 0xc:
      len = mem (memaddr, *pword1, *pword2, 1);
      break;

    default:	/* invalid instruction */
      len = 0;
      break;
    }

  if (len)
    return memaddr + len;
  else
    return 0;
}
