/* to sanitize this file, grep -v v9 < sparc.h > clean-sparc.h */

/* Table of opcodes for the sparc.
	Copyright 1989, 1991, 1992 Free Software Foundation, Inc.

This file is part of the BFD library.

BFD is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

BFD is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.	*/

/* FIXME-someday: perhaps the ,a's and such should be embedded in the
   instruction's name rather than the args.  This would make gas faster, pinsn
   slower, but would mess up some macros a bit.  xoxorich. */

#include <stdio.h>
#include "ansidecl.h"
#define BFD_EMIT_TABLE
#include "opcode/sparc.h"

const char *architecture_pname[] = {
	"v6",
	"v7",
	"v8",
	"sparclite",
	"v9",
	NULL,
};

/* v9: This file is correct for SPARC Version 9 Draft 1.0.4.  */

#define COND(x)		(((x)&0xf)<<25)
#define MCOND(x,i_or_f)	((((i_or_f)&1)<<18)|(((x)>>11)&(0xf<<14)))	/* v9 */

#define CONDA	(COND(0x8))
#define CONDCC	(COND(0xd))
#define CONDCS	(COND(0x5))
#define CONDE	(COND(0x1))
#define CONDG	(COND(0xa))
#define CONDGE	(COND(0xb))
#define CONDGU	(COND(0xc))
#define CONDL	(COND(0x3))
#define CONDLE	(COND(0x2))
#define CONDLEU	(COND(0x4))
#define CONDN	(COND(0x0))
#define CONDNE	(COND(0x9))
#define CONDNEG	(COND(0x6))
#define CONDPOS	(COND(0xe))
#define CONDVC	(COND(0xf))
#define CONDVS	(COND(0x7))

#define CONDNZ	CONDNE
#define CONDZ	CONDE
#define CONDGEU	CONDCC
#define CONDLU	CONDCS

#define FCONDA		(COND(0x8))
#define FCONDE		(COND(0x9))
#define FCONDG		(COND(0x6))
#define FCONDGE		(COND(0xb))
#define FCONDL		(COND(0x4))
#define FCONDLE		(COND(0xd))
#define FCONDLG		(COND(0x2))
#define FCONDN		(COND(0x0))
#define FCONDNE		(COND(0x1))
#define FCONDO		(COND(0xf))
#define FCONDU		(COND(0x7))
#define FCONDUE		(COND(0xa))
#define FCONDUG		(COND(0x5))
#define FCONDUGE	(COND(0xc))
#define FCONDUL		(COND(0x3))
#define FCONDULE	(COND(0xe))

#define FCONDNZ	FCONDNE
#define FCONDZ	FCONDE

#define ICC (0)	/* v9 */
#define XCC (1<<11) /* v9 */
#define FCC(x)	(((x)&0x3)<<11) /* v9 */
#define FBFCC(x)	(((x)&0x3)<<20)	/* v9 */

/* The order of the opcodes in the table is significant:
	
	* The assembler requires that all instances of the same mnemonic must
	be consecutive.	If they aren't, the assembler will bomb at runtime.

	* The disassembler should not care about the order of the opcodes.

*/

struct sparc_opcode sparc_opcodes[] = {

{ "ld",	F3(3, 0x00, 0), F3(~3, ~0x00, ~0),		"[1+2],d", 0, v6 },
{ "ld",	F3(3, 0x00, 0), F3(~3, ~0x00, ~0)|RS2_G0,	"[1],d", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1),		"[1+i],d", 0, v6 },
{ "ld",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1),		"[i+1],d", 0, v6 },
{ "ld",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ld",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ld [rs1+0],d */
{ "ld",	F3(3, 0x20, 0), F3(~3, ~0x20, ~0),		"[1+2],g", 0, v6 },
{ "ld",	F3(3, 0x20, 0), F3(~3, ~0x20, ~0)|RS2_G0,	"[1],g", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	F3(3, 0x20, 1), F3(~3, ~0x20, ~1),		"[1+i],g", 0, v6 },
{ "ld",	F3(3, 0x20, 1), F3(~3, ~0x20, ~1),		"[i+1],g", 0, v6 },
{ "ld",	F3(3, 0x20, 1), F3(~3, ~0x20, ~1)|RS1_G0,	"[i],g", 0, v6 },
{ "ld",	F3(3, 0x20, 1), F3(~3, ~0x20, ~1)|SIMM13(~0),	"[1],g", 0, v6 }, /* ld [rs1+0],d */

{ "ld",	F3(3, 0x21, 0), F3(~3, ~0x21, ~0)|RD(~0),	"[1+2],F", 0, v6 },
{ "ld",	F3(3, 0x21, 0), F3(~3, ~0x21, ~0)|RS2_G0|RD(~0),"[1],F", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	F3(3, 0x21, 1), F3(~3, ~0x21, ~1)|RD(~0),	"[1+i],F", 0, v6 },
{ "ld",	F3(3, 0x21, 1), F3(~3, ~0x21, ~1)|RD(~0),	"[i+1],F", 0, v6 },
{ "ld",	F3(3, 0x21, 1), F3(~3, ~0x21, ~1)|RS1_G0|RD(~0),"[i],F", 0, v6 },
{ "ld",	F3(3, 0x21, 1), F3(~3, ~0x21, ~1)|SIMM13(~0)|RD(~0),"[1],F", 0, v6 }, /* ld [rs1+0],d */

/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "ld",	F3(3, 0x30, 0), F3(~3, ~0x30, ~0),		"[1+2],D", F_ALIAS, v6 },
{ "ld",	F3(3, 0x30, 0), F3(~3, ~0x30, ~0)|RS2_G0,	"[1],D", F_ALIAS, v6 }, /* ld [rs1+%g0],d */
{ "ld",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1),		"[1+i],D", F_ALIAS, v6 },
{ "ld",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1),		"[i+1],D", F_ALIAS, v6 },
{ "ld",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1)|RS1_G0,	"[i],D", F_ALIAS, v6 },
{ "ld",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1)|SIMM13(~0),	"[1],D", F_ALIAS, v6 }, /* ld [rs1+0],d */
{ "ld",	F3(3, 0x31, 0), F3(~3, ~0x31, ~0),		"[1+2],C", 0, v6 },
{ "ld",	F3(3, 0x31, 0), F3(~3, ~0x31, ~0)|RS2_G0,	"[1],C", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	F3(3, 0x31, 1), F3(~3, ~0x31, ~1),		"[1+i],C", 0, v6 },
{ "ld",	F3(3, 0x31, 1), F3(~3, ~0x31, ~1),		"[i+1],C", 0, v6 },
{ "ld",	F3(3, 0x31, 1), F3(~3, ~0x31, ~1)|RS1_G0,	"[i],C", 0, v6 },
{ "ld",	F3(3, 0x31, 1), F3(~3, ~0x31, ~1)|SIMM13(~0),	"[1],C", 0, v6 }, /* ld [rs1+0],d */

/* The v9 LDUW is the same as the old 'ld' opcode, it is not the same as the
   'ld' pseudo-op in v9.  */
{ "lduw",	F3(3, 0x00, 0), F3(~3, ~0x00, ~0),		"[1+2],d", F_ALIAS, v9 },
{ "lduw",	F3(3, 0x00, 0), F3(~3, ~0x00, ~0)|RS2_G0,	"[1],d", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1),		"[1+i],d", F_ALIAS, v9 },
{ "lduw",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1),		"[i+1],d", F_ALIAS, v9 },
{ "lduw",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1)|RS1_G0,	"[i],d", F_ALIAS, v9 },
{ "lduw",	F3(3, 0x00, 1), F3(~3, ~0x00, ~1)|SIMM13(~0),	"[1],d", F_ALIAS, v9 }, /* ld [rs1+0],d */

{ "ldd",	F3(3, 0x03, 0), F3(~3, ~0x03, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "ldd",	F3(3, 0x03, 0), F3(~3, ~0x03, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	F3(3, 0x03, 1), F3(~3, ~0x03, ~1),		"[1+i],d", 0, v6 },
{ "ldd",	F3(3, 0x03, 1), F3(~3, ~0x03, ~1),		"[i+1],d", 0, v6 },
{ "ldd",	F3(3, 0x03, 1), F3(~3, ~0x03, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ldd",	F3(3, 0x03, 1), F3(~3, ~0x03, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ldd [rs1+0],d */
{ "ldd",	F3(3, 0x23, 0), F3(~3, ~0x23, ~0)|ASI(~0),	"[1+2],H", 0, v6 },
{ "ldd",	F3(3, 0x23, 0), F3(~3, ~0x23, ~0)|ASI_RS2(~0),	"[1],H", 0, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	F3(3, 0x23, 1), F3(~3, ~0x23, ~1),		"[1+i],H", 0, v6 },
{ "ldd",	F3(3, 0x23, 1), F3(~3, ~0x23, ~1),		"[i+1],H", 0, v6 },
{ "ldd",	F3(3, 0x23, 1), F3(~3, ~0x23, ~1)|RS1_G0,	"[i],H", 0, v6 },
{ "ldd",	F3(3, 0x23, 1), F3(~3, ~0x23, ~1)|SIMM13(~0),	"[1],H", 0, v6 }, /* ldd [rs1+0],d */
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "ldd",	F3(3, 0x33, 0), F3(~3, ~0x33, ~0)|ASI(~0),	"[1+2],D", F_ALIAS, v6 },
{ "ldd",	F3(3, 0x33, 0), F3(~3, ~0x33, ~0)|ASI_RS2(~0),	"[1],D", F_ALIAS, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1),		"[1+i],D", F_ALIAS, v6 },
{ "ldd",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1),		"[i+1],D", F_ALIAS, v6 },
{ "ldd",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1)|RS1_G0,	"[i],D", F_ALIAS, v6 },
{ "ldd",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1)|SIMM13(~0),	"[1],D", F_ALIAS, v6 }, /* ldd [rs1+0],d */

{ "ldq",	F3(3, 0x22, 0), F3(~3, ~0x22, ~0)|ASI(~0),	"[1+2],J", 0, v9 },
{ "ldq",	F3(3, 0x22, 0), F3(~3, ~0x22, ~0)|ASI_RS2(~0),	"[1],J", 0, v9 }, /* ldd [rs1+%g0],d */
{ "ldq",	F3(3, 0x22, 1), F3(~3, ~0x22, ~1),		"[1+i],J", 0, v9 },
{ "ldq",	F3(3, 0x22, 1), F3(~3, ~0x22, ~1),		"[i+1],J", 0, v9 },
{ "ldq",	F3(3, 0x22, 1), F3(~3, ~0x22, ~1)|RS1_G0,	"[i],J", 0, v9 },
{ "ldq",	F3(3, 0x22, 1), F3(~3, ~0x22, ~1)|SIMM13(~0),	"[1],J", 0, v9 }, /* ldd [rs1+0],d */

{ "ldsb",	F3(3, 0x09, 0), F3(~3, ~0x09, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "ldsb",	F3(3, 0x09, 0), F3(~3, ~0x09, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* ldsb [rs1+%g0],d */
{ "ldsb",	F3(3, 0x09, 1), F3(~3, ~0x09, ~1),		"[1+i],d", 0, v6 },
{ "ldsb",	F3(3, 0x09, 1), F3(~3, ~0x09, ~1),		"[i+1],d", 0, v6 },
{ "ldsb",	F3(3, 0x09, 1), F3(~3, ~0x09, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ldsb",	F3(3, 0x09, 1), F3(~3, ~0x09, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ldsb [rs1+0],d */

{ "ldsh",	F3(3, 0x0a, 0), F3(~3, ~0x0a, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* ldsh [rs1+%g0],d */
{ "ldsh",	F3(3, 0x0a, 0), F3(~3, ~0x0a, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "ldsh",	F3(3, 0x0a, 1), F3(~3, ~0x0a, ~1),		"[1+i],d", 0, v6 },
{ "ldsh",	F3(3, 0x0a, 1), F3(~3, ~0x0a, ~1),		"[i+1],d", 0, v6 },
{ "ldsh",	F3(3, 0x0a, 1), F3(~3, ~0x0a, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ldsh",	F3(3, 0x0a, 1), F3(~3, ~0x0a, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ldsh [rs1+0],d */

{ "ldstub",	F3(3, 0x0d, 0), F3(~3, ~0x0d, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "ldstub",	F3(3, 0x0d, 0), F3(~3, ~0x0d, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* ldstub [rs1+%g0],d */
{ "ldstub",	F3(3, 0x0d, 1), F3(~3, ~0x0d, ~1),		"[1+i],d", 0, v6 },
{ "ldstub",	F3(3, 0x0d, 1), F3(~3, ~0x0d, ~1),		"[i+1],d", 0, v6 },
{ "ldstub",	F3(3, 0x0d, 1), F3(~3, ~0x0d, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ldstub",	F3(3, 0x0d, 1), F3(~3, ~0x0d, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ldstub [rs1+0],d */

{ "ldsw",	F3(3, 0x08, 0), F3(~3, ~0x08, ~0)|ASI(~0),	"[1+2],d", 0, v9 },
{ "ldsw",	F3(3, 0x08, 0), F3(~3, ~0x08, ~0)|ASI_RS2(~0),	"[1],d", 0, v9 }, /* ldsw [rs1+%g0],d */
{ "ldsw",	F3(3, 0x08, 1), F3(~3, ~0x08, ~1),		"[1+i],d", 0, v9 },
{ "ldsw",	F3(3, 0x08, 1), F3(~3, ~0x08, ~1),		"[i+1],d", 0, v9 },
{ "ldsw",	F3(3, 0x08, 1), F3(~3, ~0x08, ~1)|RS1_G0,	"[i],d", 0, v9 },
{ "ldsw",	F3(3, 0x08, 1), F3(~3, ~0x08, ~1)|SIMM13(~0),	"[1],d", 0, v9 }, /* ldsw [rs1+0],d */

{ "ldub",	F3(3, 0x01, 0), F3(~3, ~0x01, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "ldub",	F3(3, 0x01, 0), F3(~3, ~0x01, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* ldub [rs1+%g0],d */
{ "ldub",	F3(3, 0x01, 1), F3(~3, ~0x01, ~1),		"[1+i],d", 0, v6 },
{ "ldub",	F3(3, 0x01, 1), F3(~3, ~0x01, ~1),		"[i+1],d", 0, v6 },
{ "ldub",	F3(3, 0x01, 1), F3(~3, ~0x01, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "ldub",	F3(3, 0x01, 1), F3(~3, ~0x01, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* ldub [rs1+0],d */

{ "lduh",	F3(3, 0x02, 0), F3(~3, ~0x02, ~0)|ASI(~0),	"[1+2],d", 0, v6 },
{ "lduh",	F3(3, 0x02, 0), F3(~3, ~0x02, ~0)|ASI_RS2(~0),	"[1],d", 0, v6 }, /* lduh [rs1+%g0],d */
{ "lduh",	F3(3, 0x02, 1), F3(~3, ~0x02, ~1),		"[1+i],d", 0, v6 },
{ "lduh",	F3(3, 0x02, 1), F3(~3, ~0x02, ~1),		"[i+1],d", 0, v6 },
{ "lduh",	F3(3, 0x02, 1), F3(~3, ~0x02, ~1)|RS1_G0,	"[i],d", 0, v6 },
{ "lduh",	F3(3, 0x02, 1), F3(~3, ~0x02, ~1)|SIMM13(~0),	"[1],d", 0, v6 }, /* lduh [rs1+0],d */

{ "ldx",	F3(3, 0x0b, 0), F3(~3, ~0x0b, ~0)|ASI(~0),	"[1+2],d", 0, v9 },
{ "ldx",	F3(3, 0x0b, 0), F3(~3, ~0x0b, ~0)|ASI_RS2(~0),	"[1],d", 0, v9 }, /* ldx [rs1+%g0],d */
{ "ldx",	F3(3, 0x0b, 1), F3(~3, ~0x0b, ~1),		"[1+i],d", 0, v9 },
{ "ldx",	F3(3, 0x0b, 1), F3(~3, ~0x0b, ~1),		"[i+1],d", 0, v9 },
{ "ldx",	F3(3, 0x0b, 1), F3(~3, ~0x0b, ~1)|RS1_G0,	"[i],d", 0, v9 },
{ "ldx",	F3(3, 0x0b, 1), F3(~3, ~0x0b, ~1)|SIMM13(~0),	"[1],d", 0, v9 }, /* ldx [rs1+0],d */

{ "ldx",	F3(3, 0x21, 0)|RD(1), F3(~3, ~0x21, ~0)|RD(~1),	"[1+2],F", 0, v9 },
{ "ldx",	F3(3, 0x21, 0)|RD(1), F3(~3, ~0x21, ~0)|RS2_G0|RD(~1),	"[1],F", 0, v9 }, /* ld [rs1+%g0],d */
{ "ldx",	F3(3, 0x21, 1)|RD(1), F3(~3, ~0x21, ~1)|RD(~1),	"[1+i],F", 0, v9 },
{ "ldx",	F3(3, 0x21, 1)|RD(1), F3(~3, ~0x21, ~1)|RD(~1),	"[i+1],F", 0, v9 },
{ "ldx",	F3(3, 0x21, 1)|RD(1), F3(~3, ~0x21, ~1)|RS1_G0|RD(~1),	"[i],F", 0, v9 },
{ "ldx",	F3(3, 0x21, 1)|RD(1), F3(~3, ~0x21, ~1)|SIMM13(~0)|RD(~1),"[1],F", 0, v9 }, /* ld [rs1+0],d */

{ "lda",	F3(3, 0x10, 0), F3(~3, ~0x10, ~0),		"[1+2]A,d", 0, v6 },
{ "lda",	F3(3, 0x10, 0), F3(~3, ~0x10, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* lda [rs1+%g0],d */
{ "lda",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1),		"[1+i]o,d", 0, v9 },
{ "lda",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1),		"[i+1]o,d", 0, v9 },
{ "lda",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "lda",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */
{ "lda",	F3(3, 0x30, 0), F3(~3, ~0x30, ~0),		"[1+2]A,g", 0, v9 },
{ "lda",	F3(3, 0x30, 0), F3(~3, ~0x30, ~0)|RS2_G0,	"[1]A,g", 0, v9 }, /* lda [rs1+%g0],d */
{ "lda",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1),		"[1+i]o,g", 0, v9 },
{ "lda",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1),		"[i+1]o,g", 0, v9 },
{ "lda",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1)|RS1_G0,	"[i]o,g", 0, v9 },
{ "lda",	F3(3, 0x30, 1), F3(~3, ~0x30, ~1)|SIMM13(~0),	"[1]o,g", 0, v9 }, /* ld [rs1+0],d */

{ "ldda",	F3(3, 0x13, 0), F3(~3, ~0x13, ~0),		"[1+2]A,d", 0, v6 },
{ "ldda",	F3(3, 0x13, 0), F3(~3, ~0x13, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* ldda [rs1+%g0],d */
{ "ldda",	F3(3, 0x13, 1), F3(~3, ~0x13, ~1),		"[1+i]o,d", 0, v9 },
{ "ldda",	F3(3, 0x13, 1), F3(~3, ~0x13, ~1),		"[i+1]o,d", 0, v9 },
{ "ldda",	F3(3, 0x13, 1), F3(~3, ~0x13, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldda",	F3(3, 0x13, 1), F3(~3, ~0x13, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "ldda",	F3(3, 0x33, 0), F3(~3, ~0x33, ~0),		"[1+2]A,H", 0, v9 },
{ "ldda",	F3(3, 0x33, 0), F3(~3, ~0x33, ~0)|RS2_G0,	"[1]A,H", 0, v9 }, /* ldda [rs1+%g0],d */
{ "ldda",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1),		"[1+i]o,H", 0, v9 },
{ "ldda",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1),		"[i+1]o,H", 0, v9 },
{ "ldda",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1)|RS1_G0,	"[i]o,H", 0, v9 },
{ "ldda",	F3(3, 0x33, 1), F3(~3, ~0x33, ~1)|SIMM13(~0),	"[1]o,H", 0, v9 }, /* ld [rs1+0],d */

{ "ldqa",	F3(3, 0x32, 0), F3(~3, ~0x32, ~0),		"[1+2]A,J", 0, v9 },
{ "ldqa",	F3(3, 0x32, 0), F3(~3, ~0x32, ~0)|RS2_G0,	"[1]A,J", 0, v9 }, /* ldd [rs1+%g0],d */
{ "ldqa",	F3(3, 0x32, 1), F3(~3, ~0x32, ~1),		"[1+i]o,J", 0, v9 },
{ "ldqa",	F3(3, 0x32, 1), F3(~3, ~0x32, ~1),		"[i+1]o,J", 0, v9 },
{ "ldqa",	F3(3, 0x32, 1), F3(~3, ~0x32, ~1)|RS1_G0,	"[i]o,J", 0, v9 },
{ "ldqa",	F3(3, 0x32, 1), F3(~3, ~0x32, ~1)|SIMM13(~0),	"[1]o,J", 0, v9 }, /* ldd [rs1+0],d */

{ "ldsba",	F3(3, 0x19, 0), F3(~3, ~0x19, ~0),		"[1+2]A,d", 0, v6 },
{ "ldsba",	F3(3, 0x19, 0), F3(~3, ~0x19, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* ldsba [rs1+%g0],d */
{ "ldsba",	F3(3, 0x19, 1), F3(~3, ~0x19, ~1),		"[1+i]o,d", 0, v9 },
{ "ldsba",	F3(3, 0x19, 1), F3(~3, ~0x19, ~1),		"[i+1]o,d", 0, v9 },
{ "ldsba",	F3(3, 0x19, 1), F3(~3, ~0x19, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldsba",	F3(3, 0x19, 1), F3(~3, ~0x19, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "ldsha",	F3(3, 0x1a, 0), F3(~3, ~0x1a, ~0),		"[1+2]A,d", 0, v6 },
{ "ldsha",	F3(3, 0x1a, 0), F3(~3, ~0x1a, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* ldsha [rs1+%g0],d */
{ "ldsha",	F3(3, 0x1a, 1), F3(~3, ~0x1a, ~1),		"[1+i]o,d", 0, v9 },
{ "ldsha",	F3(3, 0x1a, 1), F3(~3, ~0x1a, ~1),		"[i+1]o,d", 0, v9 },
{ "ldsha",	F3(3, 0x1a, 1), F3(~3, ~0x1a, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldsha",	F3(3, 0x1a, 1), F3(~3, ~0x1a, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "ldstuba",	F3(3, 0x1d, 0), F3(~3, ~0x1d, ~0),		"[1+2]A,d", 0, v6 },
{ "ldstuba",	F3(3, 0x1d, 0), F3(~3, ~0x1d, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* ldstuba [rs1+%g0],d */
{ "ldstuba",	F3(3, 0x1d, 1), F3(~3, ~0x1d, ~1),		"[1+i]o,d", 0, v9 },
{ "ldstuba",	F3(3, 0x1d, 1), F3(~3, ~0x1d, ~1),		"[i+1]o,d", 0, v9 },
{ "ldstuba",	F3(3, 0x1d, 1), F3(~3, ~0x1d, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldstuba",	F3(3, 0x1d, 1), F3(~3, ~0x1d, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "ldswa",	F3(3, 0x18, 0), F3(~3, ~0x18, ~0),		"[1+2]A,d", 0, v9 },
{ "ldswa",	F3(3, 0x18, 0), F3(~3, ~0x18, ~0)|RS2_G0,	"[1]A,d", 0, v9 }, /* lda [rs1+%g0],d */
{ "ldswa",	F3(3, 0x18, 1), F3(~3, ~0x18, ~1),		"[1+i]o,d", 0, v9 },
{ "ldswa",	F3(3, 0x18, 1), F3(~3, ~0x18, ~1),		"[i+1]o,d", 0, v9 },
{ "ldswa",	F3(3, 0x18, 1), F3(~3, ~0x18, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldswa",	F3(3, 0x18, 1), F3(~3, ~0x18, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "lduba",	F3(3, 0x11, 0), F3(~3, ~0x11, ~0),		"[1+2]A,d", 0, v6 },
{ "lduba",	F3(3, 0x11, 0), F3(~3, ~0x11, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* lduba [rs1+%g0],d */
{ "lduba",	F3(3, 0x11, 1), F3(~3, ~0x11, ~1),		"[1+i]o,d", 0, v9 },
{ "lduba",	F3(3, 0x11, 1), F3(~3, ~0x11, ~1),		"[i+1]o,d", 0, v9 },
{ "lduba",	F3(3, 0x11, 1), F3(~3, ~0x11, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "lduba",	F3(3, 0x11, 1), F3(~3, ~0x11, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "lduha",	F3(3, 0x12, 0), F3(~3, ~0x12, ~0),		"[1+2]A,d", 0, v6 },
{ "lduha",	F3(3, 0x12, 0), F3(~3, ~0x12, ~0)|RS2_G0,	"[1]A,d", 0, v6 }, /* lduha [rs1+%g0],d */
{ "lduha",	F3(3, 0x12, 1), F3(~3, ~0x12, ~1),		"[1+i]o,d", 0, v9 },
{ "lduha",	F3(3, 0x12, 1), F3(~3, ~0x12, ~1),		"[i+1]o,d", 0, v9 },
{ "lduha",	F3(3, 0x12, 1), F3(~3, ~0x12, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "lduha",	F3(3, 0x12, 1), F3(~3, ~0x12, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "lduwa",	F3(3, 0x10, 0), F3(~3, ~0x10, ~0),		"[1+2]A,d", F_ALIAS, v9 }, /* lduwa === lda */
{ "lduwa",	F3(3, 0x10, 0), F3(~3, ~0x10, ~0)|RS2_G0,	"[1]A,d", F_ALIAS, v9 }, /* lda [rs1+%g0],d */
{ "lduwa",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1),		"[1+i]o,d", F_ALIAS, v9 },
{ "lduwa",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1),		"[i+1]o,d", F_ALIAS, v9 },
{ "lduwa",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1)|RS1_G0,	"[i]o,d", F_ALIAS, v9 },
{ "lduwa",	F3(3, 0x10, 1), F3(~3, ~0x10, ~1)|SIMM13(~0),	"[1]o,d", F_ALIAS, v9 }, /* ld [rs1+0],d */

{ "ldxa",	F3(3, 0x1b, 0), F3(~3, ~0x1b, ~0),		"[1+2]A,d", 0, v9 }, /* lduwa === lda */
{ "ldxa",	F3(3, 0x1b, 0), F3(~3, ~0x1b, ~0)|RS2_G0,	"[1]A,d", 0, v9 }, /* lda [rs1+%g0],d */
{ "ldxa",	F3(3, 0x1b, 1), F3(~3, ~0x1b, ~1),		"[1+i]o,d", 0, v9 },
{ "ldxa",	F3(3, 0x1b, 1), F3(~3, ~0x1b, ~1),		"[i+1]o,d", 0, v9 },
{ "ldxa",	F3(3, 0x1b, 1), F3(~3, ~0x1b, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "ldxa",	F3(3, 0x1b, 1), F3(~3, ~0x1b, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* ld [rs1+0],d */

{ "st",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI(~0),		"d,[1+2]", 0, v6 },
{ "st",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI_RS2(~0),		"d,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),			"d,[1+i]", 0, v6 },
{ "st",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),			"d,[i+1]", 0, v6 },
{ "st",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RS1_G0,		"d,[i]", 0, v6 },
{ "st",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|SIMM13(~0),		"d,[1]", 0, v6 }, /* st d,[rs1+0] */
{ "st",	F3(3, 0x24, 0), F3(~3, ~0x24, ~0)|ASI(~0),		"g,[1+2]", 0, v6 },
{ "st",	F3(3, 0x24, 0), F3(~3, ~0x24, ~0)|ASI_RS2(~0),		"g,[1]", 0, v6 }, /* st d[rs1+%g0] */
{ "st",	F3(3, 0x24, 1), F3(~3, ~0x24, ~1),			"g,[1+i]", 0, v6 },
{ "st",	F3(3, 0x24, 1), F3(~3, ~0x24, ~1),			"g,[i+1]", 0, v6 },
{ "st",	F3(3, 0x24, 1), F3(~3, ~0x24, ~1)|RS1_G0,		"g,[i]", 0, v6 },
{ "st",	F3(3, 0x24, 1), F3(~3, ~0x24, ~1)|SIMM13(~0),		"g,[1]", 0, v6 }, /* st d,[rs1+0] */
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "st",	F3(3, 0x34, 0), F3(~3, ~0x34, ~0)|ASI(~0),		"D,[1+2]", F_ALIAS, v6 },
{ "st",	F3(3, 0x34, 0), F3(~3, ~0x34, ~0)|ASI_RS2(~0),		"D,[1]", F_ALIAS, v6 }, /* st d,[rs1+%g0] */
{ "st",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1),			"D,[1+i]", F_ALIAS, v6 },
{ "st",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1),			"D,[i+1]", F_ALIAS, v6 },
{ "st",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1)|RS1_G0,		"D,[i]", F_ALIAS, v6 },
{ "st",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1)|SIMM13(~0),		"D,[1]", F_ALIAS, v6 }, /* st d,[rs1+0] */
{ "st",	F3(3, 0x35, 0), F3(~3, ~0x35, ~0)|ASI(~0),		"C,[1+2]", 0, v6 },
{ "st",	F3(3, 0x35, 0), F3(~3, ~0x35, ~0)|ASI_RS2(~0),		"C,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	F3(3, 0x35, 1), F3(~3, ~0x35, ~1),			"C,[1+i]", 0, v6 },
{ "st",	F3(3, 0x35, 1), F3(~3, ~0x35, ~1),			"C,[i+1]", 0, v6 },
{ "st",	F3(3, 0x35, 1), F3(~3, ~0x35, ~1)|RS1_G0,		"C,[i]", 0, v6 },
{ "st",	F3(3, 0x35, 1), F3(~3, ~0x35, ~1)|SIMM13(~0),		"C,[1]", 0, v6 }, /* st d,[rs1+0] */

{ "st",	F3(3, 0x25, 0), F3(~3, ~0x25, ~0)|RD_G0|ASI(~0),	"F,[1+2]", 0, v6 },
{ "st",	F3(3, 0x25, 0), F3(~3, ~0x25, ~0)|RD_G0|ASI_RS2(~0),	"F,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	F3(3, 0x25, 1), F3(~3, ~0x25, ~1)|RD_G0,		"F,[1+i]", 0, v6 },
{ "st",	F3(3, 0x25, 1), F3(~3, ~0x25, ~1)|RD_G0,		"F,[i+1]", 0, v6 },
{ "st",	F3(3, 0x25, 1), F3(~3, ~0x25, ~1)|RD_G0|RS1_G0,		"F,[i]", 0, v6 },
{ "st",	F3(3, 0x25, 1), F3(~3, ~0x25, ~1)|RD_G0|SIMM13(~0),	"F,[1]", 0, v6 }, /* st d,[rs1+0] */

  /* The v9 STW/STUW/STSW are the same as the old 'st' opcode, they are not
     the same as the 'st' psuedo-op in v9.  */
{ "stw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stuw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stuw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stuw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stuw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stuw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stuw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stsw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 }, /* stsw === st */
{ "stsw",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stsw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stsw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stsw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stsw",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "sta",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0),		"d,[1+2]A", 0, v6 },
{ "sta",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0)|RS2(~0),	"d,[1]A", 0, v6 }, /* sta d,[rs1+%g0] */
{ "sta",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[1+i]o", 0, v9 },
{ "sta",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[i+1]o", 0, v9 },
{ "sta",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|RS1_G0,	"d,[i]o", 0, v9 },
{ "sta",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|SIMM13(~0),	"d,[1]o", 0, v9 }, /* st d,[rs1+0] */

{ "sta",	F3(3, 0x34, 0), F3(~3, ~0x34, ~0),		"g,[1+2]A", 0, v9 },
{ "sta",	F3(3, 0x34, 0), F3(~3, ~0x34, ~0)|RS2(~0),	"g,[1]A", 0, v9 }, /* sta d,[rs1+%g0] */
{ "sta",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1),		"g,[1+i]o", 0, v9 },
{ "sta",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1),		"g,[i+1]o", 0, v9 },
{ "sta",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1)|RS1_G0,	"g,[i]o", 0, v9 },
{ "sta",	F3(3, 0x34, 1), F3(~3, ~0x34, ~1)|SIMM13(~0),	"g,[1]o", 0, v9 }, /* st d,[rs1+0] */

{ "stwa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stwa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */
{ "stwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stswa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stswa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */
{ "stswa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stswa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stswa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stswa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stuwa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stuwa",	F3(3, 0x14, 0), F3(~3, ~0x14, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */
{ "stuwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stuwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stuwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stuwa",	F3(3, 0x14, 1), F3(~3, ~0x14, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI(~0),	"d,[1+2]", 0, v6 },
{ "stb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI_RS2(~0),	"d,[1]", 0, v6 }, /* stb d,[rs1+%g0] */
{ "stb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[1+i]", 0, v6 },
{ "stb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[i+1]", 0, v6 },
{ "stb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RS1_G0,	"d,[i]", 0, v6 },
{ "stb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|SIMM13(~0),	"d,[1]", 0, v6 }, /* stb d,[rs1+0] */

{ "stub",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stub",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+%g0] */
{ "stub",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stub",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stub",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stub",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "stsb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stsb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+%g0] */
{ "stsb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stsb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stsb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stsb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "stba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0),		"d,[1+2]A", 0, v6 },
{ "stba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0)|RS2(~0),	"d,[1]A", 0, v6 }, /* stba d,[rs1+%g0] */
{ "stba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[1+i]o", 0, v9 },
{ "stba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[i+1]o", 0, v9 },
{ "stba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|RS1_G0,	"d,[i]o", 0, v9 },
{ "stba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|SIMM13(~0),	"d,[1]o", 0, v9 }, /* stb d,[rs1+0] */

{ "stsba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stsba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* stba d,[rs1+%g0] */
{ "stsba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stsba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stsba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stsba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "stuba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stuba",	F3(3, 0x15, 0), F3(~3, ~0x15, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* stba d,[rs1+%g0] */
{ "stuba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stuba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stuba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stuba",	F3(3, 0x15, 1), F3(~3, ~0x15, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "std",	F3(3, 0x07, 0), F3(~3, ~0x07, ~0)|ASI(~0),	"d,[1+2]", 0, v6 },
{ "std",	F3(3, 0x07, 0), F3(~3, ~0x07, ~0)|ASI_RS2(~0),	"d,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	F3(3, 0x07, 1), F3(~3, ~0x07, ~1),		"d,[1+i]", 0, v6 },
{ "std",	F3(3, 0x07, 1), F3(~3, ~0x07, ~1),		"d,[i+1]", 0, v6 },
{ "std",	F3(3, 0x07, 1), F3(~3, ~0x07, ~1)|RS1_G0,	"d,[i]", 0, v6 },
{ "std",	F3(3, 0x07, 1), F3(~3, ~0x07, ~1)|SIMM13(~0),	"d,[1]", 0, v6 }, /* std d,[rs1+0] */
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "std",	F3(3, 0x26, 0), F3(~3, ~0x26, ~0)|ASI(~0),	"q,[1+2]", F_ALIAS, v6 },
{ "std",	F3(3, 0x26, 0), F3(~3, ~0x26, ~0)|ASI_RS2(~0),	"q,[1]", F_ALIAS, v6 }, /* std d,[rs1+%g0] */
{ "std",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1),		"q,[1+i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1),		"q,[i+1]", F_ALIAS, v6 },
{ "std",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1)|RS1_G0,	"q,[i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1)|SIMM13(~0),	"q,[1]", F_ALIAS, v6 }, /* std d,[rs1+0] */
{ "std",	F3(3, 0x27, 0), F3(~3, ~0x27, ~0)|ASI(~0),	"H,[1+2]", 0, v6 },
{ "std",	F3(3, 0x27, 0), F3(~3, ~0x27, ~0)|ASI_RS2(~0),	"H,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	F3(3, 0x27, 1), F3(~3, ~0x27, ~1),		"H,[1+i]", 0, v6 },
{ "std",	F3(3, 0x27, 1), F3(~3, ~0x27, ~1),		"H,[i+1]", 0, v6 },
{ "std",	F3(3, 0x27, 1), F3(~3, ~0x27, ~1)|RS1_G0,	"H,[i]", 0, v6 },
{ "std",	F3(3, 0x27, 1), F3(~3, ~0x27, ~1)|SIMM13(~0),	"H,[1]", 0, v6 }, /* std d,[rs1+0] */
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "std",	F3(3, 0x36, 0), F3(~3, ~0x36, ~0)|ASI(~0),	"Q,[1+2]", F_ALIAS, v6 },
{ "std",	F3(3, 0x36, 0), F3(~3, ~0x36, ~0)|ASI_RS2(~0),	"Q,[1]", F_ALIAS, v6 }, /* std d,[rs1+%g0] */
{ "std",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1),		"Q,[1+i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1),		"Q,[i+1]", F_ALIAS, v6 },
{ "std",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1)|RS1_G0,	"Q,[i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1)|SIMM13(~0),	"Q,[1]", F_ALIAS, v6 }, /* std d,[rs1+0] */
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "std",	F3(3, 0x37, 0), F3(~3, ~0x37, ~0)|ASI(~0),	"D,[1+2]", F_ALIAS, v6 },
{ "std",	F3(3, 0x37, 0), F3(~3, ~0x37, ~0)|ASI_RS2(~0),	"D,[1]", F_ALIAS, v6 }, /* std d,[rs1+%g0] */
{ "std",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1),		"D,[1+i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1),		"D,[i+1]", F_ALIAS, v6 },
{ "std",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1)|RS1_G0,	"D,[i]", F_ALIAS, v6 },
{ "std",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1)|SIMM13(~0),	"D,[1]", F_ALIAS, v6 }, /* std d,[rs1+0] */

{ "stda",	F3(3, 0x17, 0), F3(~3, ~0x17, ~0),		"d,[1+2]A", 0, v6 },
{ "stda",	F3(3, 0x17, 0), F3(~3, ~0x17, ~0)|RS2(~0),	"d,[1]A", 0, v6 }, /* stda d,[rs1+%g0] */
{ "stda",	F3(3, 0x17, 1), F3(~3, ~0x17, ~1),		"d,[1+i]o", 0, v9 },
{ "stda",	F3(3, 0x17, 1), F3(~3, ~0x17, ~1),		"d,[i+1]o", 0, v9 },
{ "stda",	F3(3, 0x17, 1), F3(~3, ~0x17, ~1)|RS1_G0,	"d,[i]o", 0, v9 },
{ "stda",	F3(3, 0x17, 1), F3(~3, ~0x17, ~1)|SIMM13(~0),	"d,[1]o", 0, v9 }, /* std d,[rs1+0] */
{ "stda",	F3(3, 0x37, 0), F3(~3, ~0x37, ~0),		"H,[1+2]A", 0, v9 },
{ "stda",	F3(3, 0x37, 0), F3(~3, ~0x37, ~0)|RS2(~0),	"H,[1]A", 0, v9 }, /* stda d,[rs1+%g0] */
{ "stda",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1),		"H,[1+i]o", 0, v9 },
{ "stda",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1),		"H,[i+1]o", 0, v9 },
{ "stda",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1)|RS1_G0,	"H,[i]o", 0, v9 },
{ "stda",	F3(3, 0x37, 1), F3(~3, ~0x37, ~1)|SIMM13(~0),	"H,[1]o", 0, v9 }, /* std d,[rs1+0] */

{ "sth",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI(~0),	"d,[1+2]", 0, v6 },
{ "sth",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI_RS2(~0),	"d,[1]", 0, v6 }, /* sth d,[rs1+%g0] */
{ "sth",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[1+i]", 0, v6 },
{ "sth",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[i+1]", 0, v6 },
{ "sth",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RS1_G0,	"d,[i]", 0, v6 },
{ "sth",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|SIMM13(~0),	"d,[1]", 0, v6 }, /* sth d,[+] */

{ "stsh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stsh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+%g0] */
{ "stsh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stsh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stsh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stsh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+0] */

{ "stuh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI(~0),	"d,[1+2]", F_ALIAS, v9 },
{ "stuh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|ASI_RS2(~0),	"d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+%g0] */
{ "stuh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[1+i]", F_ALIAS, v9 },
{ "stuh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1),		"d,[i+1]", F_ALIAS, v9 },
{ "stuh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RS1_G0,	"d,[i]", F_ALIAS, v9 },
{ "stuh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|SIMM13(~0),	"d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+0] */

{ "stha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0),		"d,[1+2]A", 0, v6 },
{ "stha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0)|RS2(~0),	"d,[1]A", 0, v6 }, /* stha ,[+%] */
{ "stha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[1+i]o", 0, v9 },
{ "stha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[i+1]o", 0, v9 },
{ "stha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|RS1_G0,	"d,[i]o", 0, v9 },
{ "stha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|SIMM13(~0),	"d,[1]o", 0, v9 }, /* sth d,[+] */

{ "stsha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stsha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* stha d,[rs1+%g0] */
{ "stsha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stsha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stsha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stsha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* sth d,[+] */

{ "stuha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0),		"d,[1+2]A", F_ALIAS, v9 },
{ "stuha",	F3(3, 0x16, 0), F3(~3, ~0x16, ~0)|RS2(~0),	"d,[1]A", F_ALIAS, v9 }, /* stha d,[rs1+%g0] */
{ "stuha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[1+i]o", F_ALIAS, v9 },
{ "stuha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1),		"d,[i+1]o", F_ALIAS, v9 },
{ "stuha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|RS1_G0,	"d,[i]o", F_ALIAS, v9 },
{ "stuha",	F3(3, 0x16, 1), F3(~3, ~0x16, ~1)|SIMM13(~0),	"d,[1]o", F_ALIAS, v9 }, /* sth d,[+] */

{ "stx",	F3(3, 0x0e, 0), F3(~3, ~0x0e, ~0)|ASI(~0),	"d,[1+2]", 0, v9 },
{ "stx",	F3(3, 0x0e, 0), F3(~3, ~0x0e, ~0)|ASI_RS2(~0),	"d,[1]", 0, v9 }, /* stx d,[rs1+%g0] */
{ "stx",	F3(3, 0x0e, 1), F3(~3, ~0x0e, ~1),		"d,[1+i]", 0, v9 },
{ "stx",	F3(3, 0x0e, 1), F3(~3, ~0x0e, ~1),		"d,[i+1]", 0, v9 },
{ "stx",	F3(3, 0x0e, 1), F3(~3, ~0x0e, ~1)|RS1_G0,	"d,[i]", 0, v9 },
{ "stx",	F3(3, 0x0e, 1), F3(~3, ~0x0e, ~1)|SIMM13(~0),	"d,[1]", 0, v9 }, /* stx d,[rs1+0] */

{ "stx",	F3(3, 0x25, 0)|RD(1), F3(~3, ~0x25, ~0)|ASI(~0)|RD(~1),	"F,[1+2]", 0, v9 },
{ "stx",	F3(3, 0x25, 0)|RD(1), F3(~3, ~0x25, ~0)|ASI_RS2(~0)|RD(~1),"F,[1]", 0, v9 }, /* st d,[rs1+%g0] */
{ "stx",	F3(3, 0x25, 1)|RD(1), F3(~3, ~0x25, ~1)|RD(~1),		"F,[1+i]", 0, v9 },
{ "stx",	F3(3, 0x25, 1)|RD(1), F3(~3, ~0x25, ~1)|RD(~1),		"F,[i+1]", 0, v9 },
{ "stx",	F3(3, 0x25, 1)|RD(1), F3(~3, ~0x25, ~1)|RS1_G0|RD(~1),	"F,[i]", 0, v9 },
{ "stx",	F3(3, 0x25, 1)|RD(1), F3(~3, ~0x25, ~1)|SIMM13(~0)|RD(~1),"F,[1]", 0, v9 }, /* st d,[rs1+0] */

{ "stxa",	F3(3, 0x1e, 0), F3(~3, ~0x1e, ~0),		"d,[1+2]A", 0, v9 },
{ "stxa",	F3(3, 0x1e, 0), F3(~3, ~0x1e, ~0)|RS2(~0),	"d,[1]A", 0, v9 }, /* stha d,[rs1+%g0] */
{ "stxa",	F3(3, 0x1e, 1), F3(~3, ~0x1e, ~1),		"d,[1+i]o", 0, v9 },
{ "stxa",	F3(3, 0x1e, 1), F3(~3, ~0x1e, ~1),		"d,[i+1]o", 0, v9 },
{ "stxa",	F3(3, 0x1e, 1), F3(~3, ~0x1e, ~1)|RS1_G0,	"d,[i]o", 0, v9 },
{ "stxa",	F3(3, 0x1e, 1), F3(~3, ~0x1e, ~1)|SIMM13(~0),	"d,[1]o", 0, v9 }, /* stx d,[rs1+0] */

{ "stq",	F3(3, 0x26, 0), F3(~3, ~0x26, ~0)|ASI(~0),	"J,[1+2]", 0, v9 },
{ "stq",	F3(3, 0x26, 0), F3(~3, ~0x26, ~0)|ASI_RS2(~0),	"J,[1]", 0, v9 }, /* st d[rs1+%g0] */
{ "stq",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1),		"J,[1+i]", 0, v9 },
{ "stq",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1),		"J,[i+1]", 0, v9 },
{ "stq",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1)|RS1_G0,	"J,[i]", 0, v9 },
{ "stq",	F3(3, 0x26, 1), F3(~3, ~0x26, ~1)|SIMM13(~0),	"J,[1]", 0, v9 }, /* st d,[rs1+0] */

{ "stqa",	F3(3, 0x36, 0), F3(~3, ~0x36, ~0)|ASI(~0),	"J,[1+2]A", 0, v9 },
{ "stqa",	F3(3, 0x36, 0), F3(~3, ~0x36, ~0)|ASI_RS2(~0),	"J,[1]A", 0, v9 }, /* st d[rs1+%g0] */
{ "stqa",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1),		"J,[1+i]o", 0, v9 },
{ "stqa",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1),		"J,[i+1]o", 0, v9 },
{ "stqa",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1)|RS1_G0,	"J,[i]o", 0, v9 },
{ "stqa",	F3(3, 0x36, 1), F3(~3, ~0x36, ~1)|SIMM13(~0),	"J,[1]o", 0, v9 }, /* st d,[rs1+0] */

{ "swap",	F3(3, 0x0f, 0), F3(~3, ~0x0f, ~0)|ASI(~0),	"[1+2],d", 0, v7 },
{ "swap",	F3(3, 0x0f, 0), F3(~3, ~0x0f, ~0)|ASI_RS2(~0),	"[1],d", 0, v7 }, /* swap [rs1+%g0],d */
{ "swap",	F3(3, 0x0f, 1), F3(~3, ~0x0f, ~1),		"[1+i],d", 0, v7 },
{ "swap",	F3(3, 0x0f, 1), F3(~3, ~0x0f, ~1),		"[i+1],d", 0, v7 },
{ "swap",	F3(3, 0x0f, 1), F3(~3, ~0x0f, ~1)|RS1_G0,	"[i],d", 0, v7 },
{ "swap",	F3(3, 0x0f, 1), F3(~3, ~0x0f, ~1)|SIMM13(~0),	"[1],d", 0, v7 }, /* swap [rs1+0],d */

{ "swapa",	F3(3, 0x1f, 0), F3(~3, ~0x1f, ~0),		"[1+2]A,d", 0, v7 },
{ "swapa",	F3(3, 0x1f, 0), F3(~3, ~0x1f, ~0)|RS2(~0),	"[1]A,d", 0, v7 }, /* swapa [rs1+%g0],d */
{ "swapa",	F3(3, 0x1f, 1), F3(~3, ~0x1f, ~1),		"[1+i]o,d", 0, v9 },
{ "swapa",	F3(3, 0x1f, 1), F3(~3, ~0x1f, ~1),		"[i+1]o,d", 0, v9 },
{ "swapa",	F3(3, 0x1f, 1), F3(~3, ~0x1f, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "swapa",	F3(3, 0x1f, 1), F3(~3, ~0x1f, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* swap [rs1+0],d */

{ "restore",	F3(2, 0x3d, 0), F3(~2, ~0x3d, ~0)|ASI(~0),			"1,2,d", 0, v6 },
{ "restore",	F3(2, 0x3d, 0), F3(~2, ~0x3d, ~0)|RD_G0|RS1_G0|ASI_RS2(~0),	"", 0, v6 }, /* restore %g0,%g0,%g0 */
{ "restore",	F3(2, 0x3d, 1), F3(~2, ~0x3d, ~1),				"1,i,d", 0, v6 },
{ "restore",	F3(2, 0x3d, 1), F3(~2, ~0x3d, ~1)|RD_G0|RS1_G0|SIMM13(~0),	"", 0, v6 }, /* restore %g0,0,%g0 */

{ "rett",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|RD_G0|ASI(~0),	"1+2", F_DELAYED, v6 }, /* rett rs1+rs2 */
{ "rett",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|RD_G0|ASI_RS2(~0),	"1", F_DELAYED, v6 },	/* rett rs1,%g0 */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD_G0,	"1+i", F_DELAYED, v6 }, /* rett rs1+X */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD_G0,	"i+1", F_DELAYED, v6 }, /* rett X+rs1 */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD_G0|RS1_G0,"i", F_DELAYED, v6 }, /* rett X+rs1 */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD_G0|RS1_G0,	"i", F_DELAYED, v6 },	/* rett X */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD_G0|SIMM13(~0),	"1", F_DELAYED, v6 },	/* rett rs1+0 */

{ "save",	F3(2, 0x3c, 0), F3(~2, ~0x3c, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "save",	F3(2, 0x3c, 1), F3(~2, ~0x3c, ~1),		"1,i,d", 0, v6 },
{ "save",	0x81e00000,	~0x81e00000,	"", F_ALIAS, v6 },

{ "ret",  F3(2, 0x38, 1)|RS1(0x1f)|SIMM13(8), F3(~2, ~0x38, ~1)|SIMM13(~8),	       "", F_DELAYED, v6 }, /* jmpl %i7+8,%g0 */
{ "retl", F3(2, 0x38, 1)|RS1(0x0f)|SIMM13(8), F3(~2, ~0x38, ~1)|RS1(~0x0f)|SIMM13(~8), "", F_DELAYED, v6 }, /* jmpl %o7+8,%g0 */

{ "jmpl",	F3(2, 0x38, 0), F3(~2, ~0x38, ~0)|ASI(~0),	"1+2,d", F_DELAYED, v6 },
{ "jmpl",	F3(2, 0x38, 0), F3(~2, ~0x38, ~0)|ASI_RS2(~0),	"1,d", F_DELAYED, v6 }, /* jmpl rs1+%g0,d */
{ "jmpl",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1)|SIMM13(~0),	"1,d", F_DELAYED, v6 }, /* jmpl rs1+0,d */
{ "jmpl",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1)|RS1_G0,	"i,d", F_DELAYED, v6 }, /* jmpl %g0+i,d */
{ "jmpl",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1),		"1+i,d", F_DELAYED, v6 },
{ "jmpl",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1),		"i+1,d", F_DELAYED, v6 },

{ "done",	F3(2, 0x3e, 1)|RD(0), F3(~2, ~0x3e, ~1)|RD(~0)|RS1_G0|SIMM13(~0),	"", 0, v9 },
{ "retry",	F3(2, 0x3e, 1)|RD(1), F3(~2, ~0x3e, ~1)|RD(~1)|RS1_G0|SIMM13(~0),	"", 0, v9 },
{ "saved",	F3(2, 0x31, 1)|RD(0), F3(~2, ~0x31, ~1)|RD(~0)|RS1_G0|SIMM13(~0),	"", 0, v9 },
{ "restored",	F3(2, 0x31, 1)|RD(1), F3(~2, ~0x31, ~1)|RD(~1)|RS1_G0|SIMM13(~0),	"", 0, v9 },
{ "sigm",	F3(2, 0x30, 0)|RD(0xf), F3(~2, ~0x30, ~0)|RD(~0xf)|RS1_G0|SIMM13(~0),	"", 0, v9 },

{ "flush",	F3(2, 0x3b, 0), F3(~2, ~0x3b, ~0)|ASI(~0),	"1+2", 0, v8 },
{ "flush",	F3(2, 0x3b, 0), F3(~2, ~0x3b, ~0)|ASI_RS2(~0),	"1", 0, v8 }, /* flush rs1+%g0 */
{ "flush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1)|SIMM13(~0),	"1", 0, v8 }, /* flush rs1+0 */
{ "flush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1)|RS1_G0,	"i", 0, v8 }, /* flush %g0+i */
{ "flush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1),		"1+i", 0, v8 },
{ "flush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1),		"i+1", 0, v8 },

/* IFLUSH was renamed to FLUSH in v8.  */
{ "iflush",	F3(2, 0x3b, 0), F3(~2, ~0x3b, ~0)|ASI(~0),	"1+2", F_ALIAS, v6 },
{ "iflush",	F3(2, 0x3b, 0), F3(~2, ~0x3b, ~0)|ASI_RS2(~0),	"1", F_ALIAS, v6 }, /* flush rs1+%g0 */
{ "iflush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1)|SIMM13(~0),	"1", F_ALIAS, v6 }, /* flush rs1+0 */
{ "iflush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1)|RS1_G0,	"i", F_ALIAS, v6 },
{ "iflush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1),		"1+i", F_ALIAS, v6 },
{ "iflush",	F3(2, 0x3b, 1), F3(~2, ~0x3b, ~1),		"i+1", F_ALIAS, v6 },

{ "return",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|ASI(~0),	"1+2", 0, v9 },
{ "return",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|ASI_RS2(~0),	"1", 0, v9 }, /* return rs1+%g0 */
{ "return",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|SIMM13(~0),	"1", 0, v9 }, /* return rs1+0 */
{ "return",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RS1_G0,	"i", 0, v9 }, /* return %g0+i */
{ "return",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1),		"1+i", 0, v9 },
{ "return",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1),		"i+1", 0, v9 },

{ "flushw",	F3(2, 0x2b, 0), F3(~2, ~0x2b, ~0)|RD_G0|RS1_G0|ASI_RS2(~0),	"", 0, v9 },

{ "membar",	F3(2, 0x28, 1)|RS1(0xf), F3(~2, ~0x28, ~1)|RD_G0|RS1(~0xf)|ASI(~0), "K", 0, v9 },
{ "stbar",	F3(2, 0x28, 0)|RS1(0xf), F3(~2, ~0x28, ~0)|RD_G0|RS1(~0xf)|SIMM13(~0),	"", 0, v8 },

{ "prefetch",	F3(3, 0x2d, 0), F3(~3, ~0x2d, ~0),		"[1+2],*", 0, v9 },
{ "prefetch",	F3(3, 0x2d, 0), F3(~3, ~0x2d, ~0)|RS2_G0,	"[1],*", 0, v9 }, /* prefetch [rs1+%g0],prefetch_fcn */
{ "prefetch",	F3(3, 0x2d, 1), F3(~3, ~0x2d, ~1),		"[1+i],*", 0, v9 },
{ "prefetch",	F3(3, 0x2d, 1), F3(~3, ~0x2d, ~1),		"[i+1],*", 0, v9 },
{ "prefetch",	F3(3, 0x2d, 1), F3(~3, ~0x2d, ~1)|RS1_G0,	"[i],*", 0, v9 },
{ "prefetch",	F3(3, 0x2d, 1), F3(~3, ~0x2d, ~1)|SIMM13(~0),	"[1],*", 0, v9 }, /* prefetch [rs1+0],prefetch_fcn */
{ "prefetcha",	F3(3, 0x3d, 0), F3(~3, ~0x3d, ~0),		"[1+2]A,*", 0, v9 },
{ "prefetcha",	F3(3, 0x3d, 0), F3(~3, ~0x3d, ~0)|RS2_G0,	"[1]A,*", 0, v9 }, /* prefetcha [rs1+%g0],prefetch_fcn */
{ "prefetcha",	F3(3, 0x3d, 1), F3(~3, ~0x3d, ~1),		"[1+i]o,d", 0, v9 },
{ "prefetcha",	F3(3, 0x3d, 1), F3(~3, ~0x3d, ~1),		"[i+1]o,d", 0, v9 },
{ "prefetcha",	F3(3, 0x3d, 1), F3(~3, ~0x3d, ~1)|RS1_G0,	"[i]o,d", 0, v9 },
{ "prefetcha",	F3(3, 0x3d, 1), F3(~3, ~0x3d, ~1)|SIMM13(~0),	"[1]o,d", 0, v9 }, /* prefetcha [rs1+0],d */

 /* The 1<<12 is a long story.  It is necessary.  For more info, please contact rich@cygnus.com */
{ "sll",	F3(2, 0x25, 0), F3(~2, ~0x25, ~0)|(1<<12)|ASI(~0),	"1,2,d", 0, v6 },
{ "sll",	F3(2, 0x25, 1), F3(~2, ~0x25, ~1)|(1<<12),		"1,i,d", 0, v6 },
{ "sra",	F3(2, 0x27, 0), F3(~2, ~0x27, ~0)|(1<<12)|ASI(~0),	"1,2,d", 0, v6 },
{ "sra",	F3(2, 0x27, 1), F3(~2, ~0x27, ~1)|(1<<12),		"1,i,d", 0, v6 },
{ "srl",	F3(2, 0x26, 0), F3(~2, ~0x26, ~0)|(1<<12)|ASI(~0),	"1,2,d", 0, v6 },
{ "srl",	F3(2, 0x26, 1), F3(~2, ~0x26, ~1)|(1<<12),		"1,i,d", 0, v6 },

{ "sllx",	F3(2, 0x25, 0)|(1<<12), F3(~2, ~0x25, ~0)|(ASI(~0)^(1<<12)),	"1,2,d", 0, v9 },
{ "sllx",	F3(2, 0x25, 1)|(1<<12), F3(~2, ~0x25, ~1),		"1,i,d", 0, v9 },
{ "srax",	F3(2, 0x27, 0)|(1<<12), F3(~2, ~0x27, ~0)|(ASI(~0)^(1<<12)),	"1,2,d", 0, v9 },
{ "srax",	F3(2, 0x27, 1)|(1<<12), F3(~2, ~0x27, ~1),		"1,i,d", 0, v9 },
{ "srlx",	F3(2, 0x26, 0)|(1<<12), F3(~2, ~0x26, ~0)|(ASI(~0)^(1<<12)),	"1,2,d", 0, v9 },
{ "srlx",	F3(2, 0x26, 1)|(1<<12), F3(~2, ~0x26, ~1),		"1,i,d", 0, v9 },

{ "mulscc",	F3(2, 0x24, 0), F3(~2, ~0x24, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "mulscc",	F3(2, 0x24, 1), F3(~2, ~0x24, ~1),		"1,i,d", 0, v6 },

{ "divscc",	F3(2, 0x1d, 0), F3(~2, ~0x1d, ~0)|ASI(~0),	"1,2,d", 0, sparclite },
{ "divscc",	F3(2, 0x1d, 1), F3(~2, ~0x1d, ~1),		"1,i,d", 0, sparclite },

{ "scan",	F3(2, 0x2c, 0), F3(~2, ~0x2c, ~0)|ASI(~0),	"1,2,d", 0, sparclite },
{ "scan",	F3(2, 0x2c, 1), F3(~2, ~0x2c, ~1),		"1,i,d", 0, sparclite },

{ "popc",	F3(2, 0x2e, 0), F3(~2, ~0x2e, ~0)|RS2_G0|ASI(~0),"2,d", 0, v9 },
{ "popc",	F3(2, 0x2e, 1), F3(~2, ~0x2e, ~1)|RS2_G0,	"i,d", 0, v9 },

{ "clr",	F3(2, 0x02, 0), F3(~2, ~0x02, ~0)|RD_G0|RS1_G0|ASI_RS2(~0),	"d", F_ALIAS, v6 }, /* or %g0,%g0,d */
{ "clr",	F3(2, 0x02, 1), F3(~2, ~0x02, ~1)|RS1_G0|SIMM13(~0),		"d", F_ALIAS, v6 }, /* or %g0,0,d	*/
{ "clr",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|RD_G0|ASI(~0),		"[1+2]", F_ALIAS, v6 },
{ "clr",	F3(3, 0x04, 0), F3(~3, ~0x04, ~0)|RD_G0|ASI_RS2(~0),		"[1]", F_ALIAS, v6 }, /* st %g0,[rs1+%g0] */
{ "clr",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RD_G0,			"[1+i]", F_ALIAS, v6 },
{ "clr",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RD_G0,			"[i+1]", F_ALIAS, v6 },
{ "clr",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RD_G0|RS1_G0,		"[i]", F_ALIAS, v6 },
{ "clr",	F3(3, 0x04, 1), F3(~3, ~0x04, ~1)|RD_G0|SIMM13(~0),		"[1]", F_ALIAS, v6 }, /* st %g0,[rs1+0] */

{ "clrb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|RD_G0|ASI(~0),	"[1+2]", F_ALIAS, v6 },
{ "clrb",	F3(3, 0x05, 0), F3(~3, ~0x05, ~0)|RD_G0|ASI_RS2(~0),	"[1]", F_ALIAS, v6 }, /* stb %g0,[rs1+%g0] */
{ "clrb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RD_G0,		"[1+i]", F_ALIAS, v6 },
{ "clrb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RD_G0,		"[i+1]", F_ALIAS, v6 },
{ "clrb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RD_G0|RS1_G0,	"[i]", F_ALIAS, v6 },
{ "clrb",	F3(3, 0x05, 1), F3(~3, ~0x05, ~1)|RD_G0|SIMM13(~0),	"[1]", F_ALIAS, v6 }, /* clrb [rs1+0],d */

{ "clrh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|RD_G0|ASI(~0),	"[1+2]", F_ALIAS, v6 },
{ "clrh",	F3(3, 0x06, 0), F3(~3, ~0x06, ~0)|RD_G0|ASI_RS2(~0),	"[1]", F_ALIAS, v6 }, /* sth %g0,[rs1+%g0] */
{ "clrh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RD_G0,		"[1+i]", F_ALIAS, v6 },
{ "clrh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RD_G0,		"[i+1]", F_ALIAS, v6 },
{ "clrh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RD_G0|RS1_G0,	"[i]", F_ALIAS, v6 },
{ "clrh",	F3(3, 0x06, 1), F3(~3, ~0x06, ~1)|RD_G0|SIMM13(~0),	"[1]", F_ALIAS, v6 }, /* clrb [rs1+0],d */

{ "orcc",	F3(2, 0x12, 0), F3(~2, ~0x12, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "orcc",	F3(2, 0x12, 1), F3(~2, ~0x12, ~1),		"1,i,d", 0, v6 },
{ "orcc",	F3(2, 0x12, 1), F3(~2, ~0x12, ~1),		"i,1,d", 0, v6 },

/* This is not a commutative instruction.  */
{ "orncc",	F3(2, 0x16, 0), F3(~2, ~0x16, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "orncc",	F3(2, 0x16, 1), F3(~2, ~0x16, ~1),		"1,i,d", 0, v6 },

/* This is not a commutative instruction.  */
{ "orn",	F3(2, 0x06, 0), F3(~2, ~0x06, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "orn",	F3(2, 0x06, 1), F3(~2, ~0x06, ~1),		"1,i,d", 0, v6 },

{ "tst",	F3(2, 0x12, 0), F3(~2, ~0x12, ~0)|RD_G0|ASI_RS2(~0),	"1", 0, v6 }, /* orcc rs1, %g0, %g0 */
{ "tst",	F3(2, 0x12, 0), F3(~2, ~0x12, ~0)|RD_G0|RS1_G0|ASI(~0),	"2", 0, v6 }, /* orcc %g0, rs2, %g0 */
{ "tst",	F3(2, 0x12, 1), F3(~2, ~0x12, ~1)|RD_G0|SIMM13(~0),	"1", 0, v6 }, /* orcc rs1, 0, %g0 */

{ "wr",	F3(2, 0x30, 0),		F3(~2, ~0x30, ~0)|ASI(~0),		"1,2,m", 0, v8 }, /* wr r,r,%asrX */
{ "wr",	F3(2, 0x30, 0),		F3(~2, ~0x30, ~0)|RD_G0|ASI(~0),	"1,2,y", 0, v6 }, /* wr r,r,%y */
{ "wr",	F3(2, 0x30, 1),		F3(~2, ~0x30, ~1),			"1,i,m", 0, v8 }, /* wr r,i,%asrX */
{ "wr",	F3(2, 0x30, 1),		F3(~2, ~0x30, ~1)|RD_G0,		"1,i,y", 0, v6 }, /* wr r,i,%y */
{ "wr",	F3(2, 0x31, 0),		F3(~2, ~0x31, ~0)|RD_G0|ASI(~0),	"1,2,p", 0, v6 }, /* wr r,r,%psr */
{ "wr",	F3(2, 0x31, 1),		F3(~2, ~0x31, ~1)|RD_G0,		"1,i,p", 0, v6 }, /* wr r,i,%psr */
{ "wr",	F3(2, 0x32, 0),		F3(~2, ~0x32, ~0)|RD_G0|ASI(~0),	"1,2,w", 0, v6 }, /* wr r,r,%wim */
{ "wr",	F3(2, 0x32, 1),		F3(~2, ~0x32, ~1)|RD_G0,		"1,i,w", 0, v6 }, /* wr r,i,%wim */
{ "wr",	F3(2, 0x33, 0),		F3(~2, ~0x33, ~0)|RD_G0|ASI(~0),	"1,2,t", 0, v6 }, /* wr r,r,%tbr */
{ "wr",	F3(2, 0x33, 0),		F3(~2, ~0x33, ~0)|RD_G0|ASI(~0),	"1,2,o", 0, v9 }, /* wr r,r,%asi */
{ "wr",	F3(2, 0x33, 1),		F3(~2, ~0x33, ~1)|RD_G0,		"1,i,t", 0, v6 }, /* wr r,i,%tbr */
{ "wr",	F3(2, 0x33, 1),		F3(~2, ~0x33, ~1)|RD_G0,		"1,i,o", 0, v9 }, /* wr r,i,%asi */

{ "wr", F3(2, 0x30, 0)|RD(0), F3(~2, ~0x30, ~0)|RD(~0)|ASI(~0), "1,2,y", 0, v9 },
{ "wr", F3(2, 0x30, 1)|RD(0), F3(~2, ~0x30, ~1)|RD(~0),		"1,i,y", 0, v9 },
{ "wr", F3(2, 0x30, 0)|RD(2), F3(~2, ~0x30, ~0)|RD(~2)|ASI(~0), "1,2,E", 0, v9 },
{ "wr", F3(2, 0x30, 1)|RD(2), F3(~2, ~0x30, ~1)|RD(~2),		"1,i,E", 0, v9 },
{ "wr", F3(2, 0x30, 0)|RD(3), F3(~2, ~0x30, ~0)|RD(~3)|ASI(~0), "1,2,o", 0, v9 },
{ "wr", F3(2, 0x30, 1)|RD(3), F3(~2, ~0x30, ~1)|RD(~3),		"1,i,o", 0, v9 },
{ "wr", F3(2, 0x30, 0)|RD(6), F3(~2, ~0x30, ~0)|RD(~6)|ASI(~0), "1,2,s", 0, v9 },
{ "wr", F3(2, 0x30, 1)|RD(6), F3(~2, ~0x30, ~1)|RD(~6),		"1,i,s", 0, v9 },

{ "rd",	F3(2, 0x28, 0),			F3(~2, ~0x28, ~0)|SIMM13(~0),		"M,d", 0, v8 }, /* rd %asr1,r */
{ "rd",	F3(2, 0x28, 0),			F3(~2, ~0x28, ~0)|RS1_G0|SIMM13(~0),	"y,d", 0, v6 }, /* rd %y,r */
{ "rd",	F3(2, 0x29, 0),			F3(~2, ~0x29, ~0)|RS1_G0|SIMM13(~0),	"p,d", 0, v6 }, /* rd %psr,r */
{ "rd",	F3(2, 0x2a, 0),			F3(~2, ~0x2a, ~0)|RS1_G0|SIMM13(~0),	"w,d", 0, v6 }, /* rd %wim,r */
{ "rd",	F3(2, 0x2b, 0),			F3(~2, ~0x2b, ~0)|RS1_G0|SIMM13(~0),	"t,d", 0, v6 }, /* rd %tbr,r */

{ "rd",	F3(2, 0x28, 0)|RS1(2),		F3(~2, ~0x28, ~0)|RS1(~2)|SIMM13(~0),	"E,d", 0, v9 }, /* rd %ccr,r */
{ "rd",	F3(2, 0x28, 0)|RS1(3),		F3(~2, ~0x28, ~0)|RS1(~3)|SIMM13(~0),	"o,d", 0, v9 }, /* rd %asi,r */
{ "rd",	F3(2, 0x28, 0)|RS1(4),		F3(~2, ~0x28, ~0)|RS1(~4)|SIMM13(~0),	"W,d", 0, v9 }, /* rd %tick,r */
{ "rd",	F3(2, 0x28, 0)|RS1(5),		F3(~2, ~0x28, ~0)|RS1(~5)|SIMM13(~0),	"P,d", 0, v9 }, /* rd %pc,r */
{ "rd",	F3(2, 0x2b, 0)|RS1(6),		F3(~2, ~0x2b, ~0)|RS1(~6)|SIMM13(~0),	"s,d", 0, v9 }, /* rd %fprs,r */

{ "rdpr",	F3(2, 0x2a, 0),		F3(~2, ~0x2a, ~0)|SIMM13(~0),	"?,d", 0, v9 }, /* rdpr %priv_reg,r */
{ "wrpr",	F3(2, 0x32, 0),		F3(~2, ~0x32, ~0)|SIMM13(~0),	"1,2,!", 0, v9 },
{ "wrpr",	F3(2, 0x32, 0),		F3(~2, ~0x32, ~0)|SIMM13(~0),	"1,!", 0, v9 },
{ "wrpr",	F3(2, 0x32, 0),		F3(~2, ~0x32, ~0)|SIMM13(~0),	"!", 0, v9 },
{ "wrpr",	F3(2, 0x32, 1),		F3(~2, ~0x32, ~1),	"1,i,!", 0, v9 },
{ "wrpr",	F3(2, 0x32, 1),		F3(~2, ~0x32, ~1),	"i,1,!", 0, v9 },
{ "wrpr",	F3(2, 0x32, 1),		F3(~2, ~0x32, ~1),	"i,!", 0, v9 },

{ "mov",	F3(2, 0x30, 0),		F3(~2, ~0x30, ~0)|ASI(~0),		"1,2,m", F_ALIAS, v8 }, /* wr r,r,%asrX */
{ "mov",	F3(2, 0x30, 0),		F3(~2, ~0x30, ~0)|RD_G0|ASI(~0),	"1,2,y", F_ALIAS, v6 }, /* wr r,r,%y */
{ "mov",	F3(2, 0x30, 1),		F3(~2, ~0x30, ~1),			"1,i,m", F_ALIAS, v8 }, /* wr r,i,%asrX */
{ "mov",	F3(2, 0x30, 1),		F3(~2, ~0x30, ~1)|RD_G0,		"1,i,y", F_ALIAS, v6 }, /* wr r,i,%y */
{ "mov",	F3(2, 0x31, 0),		F3(~2, ~0x31, ~0)|RD_G0|ASI(~0),	"1,2,p", F_ALIAS, v6 }, /* wr r,r,%psr */
{ "mov",	F3(2, 0x31, 1),		F3(~2, ~0x31, ~1)|RD_G0,		"1,i,p", F_ALIAS, v6 }, /* wr r,i,%psr */
{ "mov",	F3(2, 0x32, 0),		F3(~2, ~0x32, ~0)|RD_G0|ASI(~0),	"1,2,w", F_ALIAS, v6 }, /* wr r,r,%wim */
{ "mov",	F3(2, 0x32, 1),		F3(~2, ~0x32, ~1)|RD_G0,		"1,i,w", F_ALIAS, v6 }, /* wr r,i,%wim */
{ "mov",	F3(2, 0x33, 0),		F3(~2, ~0x33, ~0)|RD_G0|ASI(~0),	"1,2,t", F_ALIAS, v6 }, /* wr r,r,%tbr */
{ "mov",	F3(2, 0x33, 1),		F3(~2, ~0x33, ~1)|RD_G0,		"1,i,t", F_ALIAS, v6 }, /* wr r,i,%tbr */

{ "mov",	F3(2, 0x28, 0),		 F3(~2, ~0x28, ~0)|SIMM13(~0),			"M,d", F_ALIAS, v8 }, /* rd %asr1,r */
{ "mov",	F3(2, 0x28, 0),		 F3(~2, ~0x28, ~0)|RS1_G0|SIMM13(~0),		"y,d", F_ALIAS, v6 }, /* rd %y,r */
{ "mov",	F3(2, 0x29, 0),		 F3(~2, ~0x29, ~0)|RS1_G0|SIMM13(~0),		"p,d", F_ALIAS, v6 }, /* rd %psr,r */
{ "mov",	F3(2, 0x2a, 0),		 F3(~2, ~0x2a, ~0)|RS1_G0|SIMM13(~0),		"w,d", F_ALIAS, v6 }, /* rd %wim,r */
{ "mov",	F3(2, 0x2b, 0),		 F3(~2, ~0x2b, ~0)|RS1_G0|SIMM13(~0),		"t,d", F_ALIAS, v6 }, /* rd %tbr,r */

{ "mov",	F3(2, 0x30, 0), F3(~2, ~0x30, ~0)|ASI_RS2(~0),	"1,y", F_ALIAS, v6 }, /* wr rs1,%g0,%y */
{ "mov",	F3(2, 0x30, 1), F3(~2, ~0x30, ~1),		"i,y", F_ALIAS, v6 },
{ "mov",	F3(2, 0x30, 1), F3(~2, ~0x30, ~1)|SIMM13(~0),	"1,y", F_ALIAS, v6 }, /* wr rs1,0,%y */
{ "mov",	F3(2, 0x31, 0), F3(~2, ~0x31, ~0)|ASI_RS2(~0),	"1,p", F_ALIAS, v6 }, /* wr rs1,%g0,%psr */
{ "mov",	F3(2, 0x31, 1), F3(~2, ~0x31, ~1),		"i,p", F_ALIAS, v6 },
{ "mov",	F3(2, 0x31, 1), F3(~2, ~0x31, ~1)|SIMM13(~0),	"1,p", F_ALIAS, v6 }, /* wr rs1,0,%psr */
{ "mov",	F3(2, 0x32, 0), F3(~2, ~0x32, ~0)|ASI_RS2(~0),	"1,w", F_ALIAS, v6 }, /* wr rs1,%g0,%wim */
{ "mov",	F3(2, 0x32, 1), F3(~2, ~0x32, ~1),		"i,w", F_ALIAS, v6 },
{ "mov",	F3(2, 0x32, 1), F3(~2, ~0x32, ~1)|SIMM13(~0),	"1,w", F_ALIAS, v6 }, /* wr rs1,0,%wim */
{ "mov",	F3(2, 0x33, 0), F3(~2, ~0x33, ~0)|ASI_RS2(~0),	"1,t", F_ALIAS, v6 }, /* wr rs1,%g0,%tbr */
{ "mov",	F3(2, 0x33, 1), F3(~2, ~0x33, ~1),		"i,t", F_ALIAS, v6 },
{ "mov",	F3(2, 0x33, 1), F3(~2, ~0x33, ~1)|SIMM13(~0),	"1,t", F_ALIAS, v6 }, /* wr rs1,0,%tbr */

{ "mov",	F3(2, 0x02, 0), F3(~2, ~0x02, ~0)|RS1_G0|ASI(~0),	"2,d", 0, v6 }, /* or %g0,rs2,d */
{ "mov",	F3(2, 0x02, 1), F3(~2, ~0x02, ~1)|RS1_G0,		"i,d", 0, v6 }, /* or %g0,i,d	*/
{ "mov",        F3(2, 0x02, 0), F3(~2, ~0x02, ~0)|ASI_RS2(~0),		"1,d", 0, v6 }, /* or rs1,%g0,d   */
{ "mov",        F3(2, 0x02, 1), F3(~2, ~0x02, ~1)|SIMM13(~0),		"1,d", 0, v6 }, /* or rs1,0,d */

{ "or",	F3(2, 0x02, 0), F3(~2, ~0x02, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "or",	F3(2, 0x02, 1), F3(~2, ~0x02, ~1),		"1,i,d", 0, v6 },
{ "or",	F3(2, 0x02, 1), F3(~2, ~0x02, ~1),		"i,1,d", 0, v6 },

{ "bset",	F3(2, 0x02, 0), F3(~2, ~0x02, ~0)|ASI(~0),	"2,r", F_ALIAS, v6 },	/* or rd,rs2,rd */
{ "bset",	F3(2, 0x02, 1), F3(~2, ~0x02, ~1),		"i,r", F_ALIAS, v6 },	/* or rd,i,rd */

/* This is not a commutative instruction.  */
{ "andn",	F3(2, 0x05, 0), F3(~2, ~0x05, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "andn",	F3(2, 0x05, 1), F3(~2, ~0x05, ~1),		"1,i,d", 0, v6 },

/* This is not a commutative instruction.  */
{ "andncc",	F3(2, 0x15, 0), F3(~2, ~0x15, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "andncc",	F3(2, 0x15, 1), F3(~2, ~0x15, ~1),		"1,i,d", 0, v6 },

{ "bclr",	F3(2, 0x05, 0), F3(~2, ~0x05, ~0)|ASI(~0),	"2,r", F_ALIAS, v6 },	/* andn rd,rs2,rd */
{ "bclr",	F3(2, 0x05, 1), F3(~2, ~0x05, ~1),		"i,r", F_ALIAS, v6 },	/* andn rd,i,rd */

{ "cmp",	F3(2, 0x14, 0), F3(~2, ~0x14, ~0)|RD_G0|ASI(~0),	"1,2", 0, v6 },	/* subcc rs1,rs2,%g0 */
{ "cmp",	F3(2, 0x14, 1), F3(~2, ~0x14, ~1)|RD_G0,		"1,i", 0, v6 },	/* subcc rs1,i,%g0 */

{ "sub",	F3(2, 0x04, 0), F3(~2, ~0x04, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "sub",	F3(2, 0x04, 1), F3(~2, ~0x04, ~1),		"1,i,d", 0, v6 },

{ "subcc",	F3(2, 0x14, 0), F3(~2, ~0x14, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "subcc",	F3(2, 0x14, 1), F3(~2, ~0x14, ~1),		"1,i,d", 0, v6 },

/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "subx",	F3(2, 0x0c, 0), F3(~2, ~0x0c, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v6 },
{ "subx",	F3(2, 0x0c, 1), F3(~2, ~0x0c, ~1),		"1,i,d", F_ALIAS, v6 },
{ "subc",	F3(2, 0x0c, 0), F3(~2, ~0x0c, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "subc",	F3(2, 0x0c, 1), F3(~2, ~0x0c, ~1),		"1,i,d", 0, v9 },

/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "subxcc",	F3(2, 0x1c, 0), F3(~2, ~0x1c, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v6 },
{ "subxcc",	F3(2, 0x1c, 1), F3(~2, ~0x1c, ~1),		"1,i,d", F_ALIAS, v6 },
{ "subccc",	F3(2, 0x1c, 0), F3(~2, ~0x1c, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "subccc",	F3(2, 0x1c, 1), F3(~2, ~0x1c, ~1),		"1,i,d", 0, v9 },

{ "and",	F3(2, 0x01, 0), F3(~2, ~0x01, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "and",	F3(2, 0x01, 1), F3(~2, ~0x01, ~1),		"1,i,d", 0, v6 },
{ "and",	F3(2, 0x01, 1), F3(~2, ~0x01, ~1),		"i,1,d", 0, v6 },

{ "andcc",	F3(2, 0x11, 0), F3(~2, ~0x11, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "andcc",	F3(2, 0x11, 1), F3(~2, ~0x11, ~1),		"1,i,d", 0, v6 },
{ "andcc",	F3(2, 0x11, 1), F3(~2, ~0x11, ~1),		"i,1,d", 0, v6 },

{ "dec",	F3(2, 0x04, 1)|SIMM13(0x1), F3(~2, ~0x04, ~1)|SIMM13(~0x0001), "r", F_ALIAS, v6 },	/* sub rd,1,rd */
{ "dec",	F3(2, 0x04, 1),		    F3(~2, ~0x04, ~1),		       "i,r", F_ALIAS, v8 },	/* sub rd,imm,rd */
{ "deccc",	F3(2, 0x14, 1)|SIMM13(0x1), F3(~2, ~0x14, ~1)|SIMM13(~0x0001), "r", F_ALIAS, v6 },	/* subcc rd,1,rd */
{ "deccc",	F3(2, 0x14, 1),		    F3(~2, ~0x14, ~1),		       "i,r", F_ALIAS, v8 },	/* subcc rd,imm,rd */
{ "inc",	F3(2, 0x00, 1)|SIMM13(0x1), F3(~2, ~0x00, ~1)|SIMM13(~0x0001), "r", F_ALIAS, v6 },	/* add rd,1,rd */
{ "inc",	F3(2, 0x00, 1),		    F3(~2, ~0x00, ~1),		       "i,r", F_ALIAS, v8 },	/* add rd,imm,rd */
{ "inccc",	F3(2, 0x10, 1)|SIMM13(0x1), F3(~2, ~0x10, ~1)|SIMM13(~0x0001), "r", F_ALIAS, v6 },	/* addcc rd,1,rd */
{ "inccc",	F3(2, 0x10, 1),		    F3(~2, ~0x10, ~1),		       "i,r", F_ALIAS, v8 },	/* addcc rd,imm,rd */

{ "btst",	F3(2, 0x11, 0), F3(~2, ~0x11, ~0)|RD_G0|ASI(~0), "1,2", F_ALIAS, v6 },	/* andcc rs1,rs2,%g0 */
{ "btst",	F3(2, 0x11, 1), F3(~2, ~0x11, ~1)|RD_G0, "i,1", F_ALIAS, v6 },	/* andcc rs1,i,%g0 */

{ "neg",	F3(2, 0x04, 0), F3(~2, ~0x04, ~0)|RS1_G0|ASI(~0), "2,d", F_ALIAS, v6 }, /* sub %g0,rs2,rd */
{ "neg",	F3(2, 0x04, 0), F3(~2, ~0x04, ~0)|RS1_G0|ASI(~0), "r", F_ALIAS, v6 }, /* sub %g0,rd,rd */

{ "add",	F3(2, 0x00, 0), F3(~2, ~0x00, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "add",	F3(2, 0x00, 1), F3(~2, ~0x00, ~1),		"1,i,d", 0, v6 },
{ "add",	F3(2, 0x00, 1), F3(~2, ~0x00, ~1),		"i,1,d", 0, v6 },
{ "addcc",	F3(2, 0x10, 0), F3(~2, ~0x10, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "addcc",	F3(2, 0x10, 1), F3(~2, ~0x10, ~1),		"1,i,d", 0, v6 },
{ "addcc",	F3(2, 0x10, 1), F3(~2, ~0x10, ~1),		"i,1,d", 0, v6 },
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "addx",	F3(2, 0x08, 0), F3(~2, ~0x08, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v6 },
{ "addx",	F3(2, 0x08, 1), F3(~2, ~0x08, ~1),		"1,i,d", F_ALIAS, v6 },
{ "addx",	F3(2, 0x08, 1), F3(~2, ~0x08, ~1),		"i,1,d", F_ALIAS, v6 },
{ "addc",	F3(2, 0x08, 0), F3(~2, ~0x08, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "addc",	F3(2, 0x08, 1), F3(~2, ~0x08, ~1),		"1,i,d", 0, v9 },
{ "addc",	F3(2, 0x08, 1), F3(~2, ~0x08, ~1),		"i,1,d", 0, v9 },
/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "addxcc",	F3(2, 0x18, 0), F3(~2, ~0x18, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v6 },
{ "addxcc",	F3(2, 0x18, 1), F3(~2, ~0x18, ~1),		"1,i,d", F_ALIAS, v6 },
{ "addxcc",	F3(2, 0x18, 1), F3(~2, ~0x18, ~1),		"i,1,d", F_ALIAS, v6 },
{ "addccc",	F3(2, 0x18, 0), F3(~2, ~0x18, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "addccc",	F3(2, 0x18, 1), F3(~2, ~0x18, ~1),		"1,i,d", 0, v9 },
{ "addccc",	F3(2, 0x18, 1), F3(~2, ~0x18, ~1),		"i,1,d", 0, v9 },

{ "smul",	F3(2, 0x0b, 0), F3(~2, ~0x0b, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "smul",	F3(2, 0x0b, 1), F3(~2, ~0x0b, ~1),		"1,i,d", 0, v8 },
{ "smul",	F3(2, 0x0b, 1), F3(~2, ~0x0b, ~1),		"i,1,d", 0, v8 },
{ "smulcc",	F3(2, 0x1b, 0), F3(~2, ~0x1b, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "smulcc",	F3(2, 0x1b, 1), F3(~2, ~0x1b, ~1),		"1,i,d", 0, v8 },
{ "smulcc",	F3(2, 0x1b, 1), F3(~2, ~0x1b, ~1),		"i,1,d", 0, v8 },
{ "umul",	F3(2, 0x0a, 0), F3(~2, ~0x0a, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "umul",	F3(2, 0x0a, 1), F3(~2, ~0x0a, ~1),		"1,i,d", 0, v8 },
{ "umul",	F3(2, 0x0a, 1), F3(~2, ~0x0a, ~1),		"i,1,d", 0, v8 },
{ "umulcc",	F3(2, 0x1a, 0), F3(~2, ~0x1a, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "umulcc",	F3(2, 0x1a, 1), F3(~2, ~0x1a, ~1),		"1,i,d", 0, v8 },
{ "umulcc",	F3(2, 0x1a, 1), F3(~2, ~0x1a, ~1),		"i,1,d", 0, v8 },
{ "sdiv",	F3(2, 0x0f, 0), F3(~2, ~0x0f, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "sdiv",	F3(2, 0x0f, 1), F3(~2, ~0x0f, ~1),		"1,i,d", 0, v8 },
{ "sdiv",	F3(2, 0x0f, 1), F3(~2, ~0x0f, ~1),		"i,1,d", 0, v8 },
{ "sdivcc",	F3(2, 0x1f, 0), F3(~2, ~0x1f, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "sdivcc",	F3(2, 0x1f, 1), F3(~2, ~0x1f, ~1),		"1,i,d", 0, v8 },
{ "sdivcc",	F3(2, 0x1f, 1), F3(~2, ~0x1f, ~1),		"i,1,d", 0, v8 },
{ "udiv",	F3(2, 0x0e, 0), F3(~2, ~0x0e, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "udiv",	F3(2, 0x0e, 1), F3(~2, ~0x0e, ~1),		"1,i,d", 0, v8 },
{ "udiv",	F3(2, 0x0e, 1), F3(~2, ~0x0e, ~1),		"i,1,d", 0, v8 },
{ "udivcc",	F3(2, 0x1e, 0), F3(~2, ~0x1e, ~0)|ASI(~0),	"1,2,d", 0, v8 },
{ "udivcc",	F3(2, 0x1e, 1), F3(~2, ~0x1e, ~1),		"1,i,d", 0, v8 },
{ "udivcc",	F3(2, 0x1e, 1), F3(~2, ~0x1e, ~1),		"i,1,d", 0, v8 },

{ "mulx",	F3(2, 0x09, 0), F3(~2, ~0x09, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "mulx",	F3(2, 0x09, 1), F3(~2, ~0x09, ~1),		"1,i,d", 0, v9 },
{ "sdivx",	F3(2, 0x2d, 0), F3(~2, ~0x2d, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "sdivx",	F3(2, 0x2d, 1), F3(~2, ~0x2d, ~1),		"1,i,d", 0, v9 },
{ "udivx",	F3(2, 0x0d, 0), F3(~2, ~0x0d, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "udivx",	F3(2, 0x0d, 1), F3(~2, ~0x0d, ~1),		"1,i,d", 0, v9 },

{ "call",	F1(0x1), F1(~0x1), "L", F_DELAYED, v6 },
{ "call",	F1(0x1), F1(~0x1), "L,#", F_DELAYED, v6 },
{ "call",	F3(2, 0x38, 0)|RD(0xf), F3(~2, ~0x38, ~0)|RD(~0xf)|ASI_RS2(~0), "1", F_DELAYED, v6 }, /* jmpl rs1+%g0, %o7 */
{ "call",	F3(2, 0x38, 0)|RD(0xf), F3(~2, ~0x38, ~0)|RD(~0xf)|ASI_RS2(~0), "1,#", F_DELAYED, v6 },

/* Conditional instructions.

   Because this part of the table was such a mess earlier, I have
   macrofied it so that all the branches and traps are generated from
   a single-line description of each condition value.  John Gilmore. */

/* Define branches -- one annulled, one without, etc. */
#define br(opcode, mask, lose, flags) \
 { opcode, (mask)|ANNUL, (lose),       ",a l",   (flags), v6 }, \
 { opcode, (mask)      , (lose)|ANNUL, "l",     (flags), v6 }

#define brx(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose)|ANNUL|BPRED, "Z,G",   (flags), v9 }, \
 { opcode, (mask), (lose)|ANNUL|BPRED, ",N Z,G",   (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a Z,G", (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a,N Z,G", (flags), v9 }, \
 { opcode, (mask)|BPRED, (lose)|ANNUL, ",T Z,G",   (flags), v9 }, \
 { opcode, (mask)|ANNUL|BPRED, (lose), ",a,T Z,G", (flags), v9 }, \
 { opcode, (mask), (lose)|ANNUL|BPRED, "z,G",   (flags), v9 }, \
 { opcode, (mask), (lose)|ANNUL|BPRED, ",N z,G",   (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a z,G", (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a,N z,G", (flags), v9 }, \
 { opcode, (mask)|BPRED, (lose)|ANNUL, ",T z,G",   (flags), v9 }, \
 { opcode, (mask)|ANNUL|BPRED, (lose), ",a,T z,G", (flags), v9 }

/* Define four traps: reg+reg, reg + immediate, immediate alone, reg alone. */
#define tr(opcode, mask, lose, flags) \
 { opcode, (mask)|(2<<11)|IMMED, (lose)|RS1_G0,	"Z,i",   (flags), v9 }, /* %g0 + imm */ \
 { opcode, (mask)|(2<<11)|IMMED, (lose),	"Z,1+i", (flags), v9 }, /* rs1 + imm */ \
 { opcode, (mask)|(2<<11), IMMED|(lose),	"Z,1+2", (flags), v9 }, /* rs1 + rs2 */ \
 { opcode, (mask)|(2<<11), IMMED|(lose)|RS2_G0,	"Z,1",   (flags), v9 }, /* rs1 + %g0 */ \
 { opcode, (mask)|IMMED, (lose)|RS1_G0,	"z,i",   (flags)|F_ALIAS, v9 }, /* %g0 + imm */ \
 { opcode, (mask)|IMMED, (lose),	"z,1+i", (flags)|F_ALIAS, v9 }, /* rs1 + imm */ \
 { opcode, (mask), IMMED|(lose),	"z,1+2", (flags)|F_ALIAS, v9 }, /* rs1 + rs2 */ \
 { opcode, (mask), IMMED|(lose)|RS2_G0,	"z,1",   (flags)|F_ALIAS, v9 }, /* rs1 + %g0 */ \
 { opcode, (mask)|IMMED, (lose)|RS1_G0,		"i",     (flags), v6 }, /* %g0 + imm */ \
 { opcode, (mask)|IMMED, (lose),		"1+i",   (flags), v6 }, /* rs1 + imm */ \
 { opcode, (mask), IMMED|(lose),		"1+2",   (flags), v6 }, /* rs1 + rs2 */ \
 { opcode, (mask), IMMED|(lose)|RS2_G0,		"1",     (flags), v6 } /* rs1 + %g0 */

/* Define both branches and traps based on condition mask */
#define cond(bop, top, mask, flags) \
  br(bop,  F2(0, 2)|(mask), F2(~0, ~2)|((~mask)&COND(~0)), F_DELAYED|(flags)), \
  brx(bop, F2(0, 1)|(mask), F2(~0, ~1)|((~mask)&COND(~0)), F_DELAYED|(flags)), /* v9 */ \
  tr(top,  F3(2, 0x3a, 0)|(mask), F3(~2, ~0x3a, 0)|((~mask)&COND(~0)), (flags))

/* Define all the conditions, all the branches, all the traps.  */

cond ("b",	"t",    CONDA, 0),
cond ("ba",	"ta",   CONDA, F_ALIAS), /* for nothing */
cond ("bcc",	"tcc",  CONDCC, 0),
cond ("bcs",	"tcs",  CONDCS, 0),
cond ("be",	"te",   CONDE, 0),
cond ("bg",	"tg",   CONDG, 0),
cond ("bgt",	"tgt",   CONDG, F_ALIAS),
cond ("bge",	"tge",  CONDGE, 0),
cond ("bgeu",	"tgeu", CONDGEU, F_ALIAS), /* for cc */
cond ("bgu",	"tgu",  CONDGU, 0),
cond ("bl",	"tl",   CONDL, 0),
cond ("blt",	"tlt",   CONDL, F_ALIAS),
cond ("ble",	"tle",  CONDLE, 0),
cond ("bleu",	"tleu", CONDLEU, 0),
cond ("blu",	"tlu",  CONDLU, F_ALIAS), /* for cs */
cond ("bn",	"tn",   CONDN, 0),
cond ("bne",	"tne",  CONDNE, 0),
cond ("bneg",	"tneg", CONDNEG, 0),
cond ("bnz",	"tnz",  CONDNZ, F_ALIAS), /* for ne */
cond ("bpos",	"tpos", CONDPOS, 0),
cond ("bvc",	"tvc",  CONDVC, 0),
cond ("bvs",	"tvs",  CONDVS, 0),
cond ("bz",	"tz",   CONDZ, F_ALIAS), /* for e */

#undef cond
#undef br
#undef brr /* v9 */
#undef tr

#define brr(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose)|ANNUL|BPRED, "1,k",   F_DELAYED|(flags), v9 }, \
 { opcode, (mask), (lose)|ANNUL|BPRED, ",N 1,k", F_DELAYED|(flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a 1,k", F_DELAYED|(flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a,N 1,k", F_DELAYED|(flags), v9 }, \
 { opcode, (mask)|BPRED, (lose)|ANNUL, ",T 1,k",   F_DELAYED|(flags), v9 }, \
 { opcode, (mask)|ANNUL|BPRED, (lose), ",a,T 1,k", F_DELAYED|(flags), v9 }

#define condr(bop, mask, flags) /* v9 */ \
  brr(bop, F2(0, 3)|COND(mask), F2(~0, ~3)|COND(~(mask)), (flags)) /* v9 */ \

/* v9 */ condr("brnz", 0x5, 0),
/* v9 */ condr("brz", 0x1, 0),
/* v9 */ condr("brgez", 0x7, 0),
/* v9 */ condr("brlz", 0x3, 0),
/* v9 */ condr("brlez", 0x2, 0),
/* v9 */ condr("brgz", 0x6, 0),

#undef condr /* v9 */
#undef brr /* v9 */

#define movr(opcode, mask, flags) /* v9 */ \
 { opcode, F3(2, 0x2f, 0)|COND(mask), F3(~2, ~0x2f, ~0)|COND(~(mask)), "1,2,d", (flags), v9 }, \
 { opcode, F3(2, 0x2f, 1)|COND(mask), F3(~2, ~0x2f, ~1)|COND(~(mask)), "1,j,d", (flags), v9 }

#define fmrrs(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose), "1,f,g", (flags), v9 }
#define fmrrd(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose), "1,B,H", (flags), v9 }
#define fmrrq(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose), "1,R,J", (flags), v9 }

#define fmovrs(mop, mask, flags) /* v9 */ \
  fmrrs(mop, F3(2, 0x35, 0)|OPF_LOW5(5)|COND(mask), F3(~2, ~0x35, 0)|OPF_LOW5(~5)|COND(~(mask)), (flags)) /* v9 */
#define fmovrd(mop, mask, flags) /* v9 */ \
  fmrrd(mop, F3(2, 0x35, 0)|OPF_LOW5(6)|COND(mask), F3(~2, ~0x35, 0)|OPF_LOW5(~6)|COND(~(mask)), (flags)) /* v9 */
#define fmovrq(mop, mask, flags) /* v9 */ \
  fmrrq(mop, F3(2, 0x35, 0)|OPF_LOW5(7)|COND(mask), F3(~2, ~0x35, 0)|OPF_LOW5(~7)|COND(~(mask)), (flags)) /* v9 */

/* v9 */ movr("movrne", 0x5, 0),
/* v9 */ movr("movre", 0x1, 0),
/* v9 */ movr("movrgez", 0x7, 0),
/* v9 */ movr("movrlz", 0x3, 0),
/* v9 */ movr("movrlez", 0x2, 0),
/* v9 */ movr("movrgz", 0x6, 0),
/* v9 */ movr("movrnz", 0x5, F_ALIAS),
/* v9 */ movr("movrz", 0x1, F_ALIAS),

/* v9 */ fmovrs("fmovrsne", 0x5, 0),
/* v9 */ fmovrs("fmovrse", 0x1, 0),
/* v9 */ fmovrs("fmovrsgez", 0x7, 0),
/* v9 */ fmovrs("fmovrslz", 0x3, 0),
/* v9 */ fmovrs("fmovrslez", 0x2, 0),
/* v9 */ fmovrs("fmovrsgz", 0x6, 0),
/* v9 */ fmovrs("fmovrsnz", 0x5, F_ALIAS),
/* v9 */ fmovrs("fmovrsz", 0x1, F_ALIAS),

/* v9 */ fmovrd("fmovrdne", 0x5, 0),
/* v9 */ fmovrd("fmovrde", 0x1, 0),
/* v9 */ fmovrd("fmovrdgez", 0x7, 0),
/* v9 */ fmovrd("fmovrdlz", 0x3, 0),
/* v9 */ fmovrd("fmovrdlez", 0x2, 0),
/* v9 */ fmovrd("fmovrdgz", 0x6, 0),
/* v9 */ fmovrd("fmovrdnz", 0x5, F_ALIAS),
/* v9 */ fmovrd("fmovrdz", 0x1, F_ALIAS),

/* v9 */ fmovrq("fmovrqne", 0x5, 0),
/* v9 */ fmovrq("fmovrqe", 0x1, 0),
/* v9 */ fmovrq("fmovrqgez", 0x7, 0),
/* v9 */ fmovrq("fmovrqlz", 0x3, 0),
/* v9 */ fmovrq("fmovrqlez", 0x2, 0),
/* v9 */ fmovrq("fmovrqgz", 0x6, 0),
/* v9 */ fmovrq("fmovrqnz", 0x5, F_ALIAS),
/* v9 */ fmovrq("fmovrqz", 0x1, F_ALIAS),

#undef movr /* v9 */
#undef fmovr /* v9 */
#undef fmrr /* v9 */

{ "mova",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~0)|F3(~2, ~0x2c,~1), "6,I,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDA,0), MCOND(~FCONDA,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 0)|MCOND(CONDA,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDA,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|MCOND(CONDA,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDA,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 0)|MCOND(CONDA,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDA,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "mova",	F3(2, 0x2c, 1)|MCOND(CONDA,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDA,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movcc",	F3(2, 0x2c, 0)|MCOND(CONDCC,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDCC,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movcc",	F3(2, 0x2c, 1)|MCOND(CONDCC,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDCC,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movcc",	F3(2, 0x2c, 0)|MCOND(CONDCC,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDCC,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movcc",	F3(2, 0x2c, 1)|MCOND(CONDCC,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDCC,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movgeu",	F3(2, 0x2c, 0)|MCOND(CONDGEU,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGEU,~1)|XCC|(1<<12), "z,2,d", F_ALIAS, v9 },
{ "movgeu",	F3(2, 0x2c, 1)|MCOND(CONDGEU,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGEU,~1)|XCC|(1<<12), "z,I,d", F_ALIAS, v9 },
{ "movgeu",	F3(2, 0x2c, 0)|MCOND(CONDGEU,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGEU,~1)|(1<<12), "Z,2,d", F_ALIAS, v9 },
{ "movgeu",	F3(2, 0x2c, 1)|MCOND(CONDGEU,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGEU,~1)|(1<<12), "Z,I,d", F_ALIAS, v9 },
{ "movcs",	F3(2, 0x2c, 0)|MCOND(CONDCS,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDCS,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movcs",	F3(2, 0x2c, 1)|MCOND(CONDCS,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDCS,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movcs",	F3(2, 0x2c, 0)|MCOND(CONDCS,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDCS,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movcs",	F3(2, 0x2c, 1)|MCOND(CONDCS,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDCS,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movlu",	F3(2, 0x2c, 0)|MCOND(CONDLU,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLU,~1)|XCC|(1<<12), "z,2,d", F_ALIAS, v9 },
{ "movlu",	F3(2, 0x2c, 1)|MCOND(CONDLU,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLU,~1)|XCC|(1<<12), "z,I,d", F_ALIAS, v9 },
{ "movlu",	F3(2, 0x2c, 0)|MCOND(CONDLU,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLU,~1)|(1<<12), "Z,2,d", F_ALIAS, v9 },
{ "movlu",	F3(2, 0x2c, 1)|MCOND(CONDLU,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLU,~1)|(1<<12), "Z,I,d", F_ALIAS, v9 },
{ "move",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "move",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "move",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "move",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDE,0), MCOND(~FCONDE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "move",	F3(2, 0x2c, 0)|MCOND(CONDE,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDE,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|MCOND(CONDE,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDE,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "move",	F3(2, 0x2c, 0)|MCOND(CONDE,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDE,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "move",	F3(2, 0x2c, 1)|MCOND(CONDE,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDE,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDG,0), MCOND(~FCONDG,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|MCOND(CONDG,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDG,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|MCOND(CONDG,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDG,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 0)|MCOND(CONDG,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDG,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movg",	F3(2, 0x2c, 1)|MCOND(CONDG,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDG,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDGE,0), MCOND(~FCONDGE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|MCOND(CONDGE,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGE,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|MCOND(CONDGE,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGE,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 0)|MCOND(CONDGE,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGE,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movge",	F3(2, 0x2c, 1)|MCOND(CONDGE,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGE,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movgu",	F3(2, 0x2c, 0)|MCOND(CONDGU,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGU,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movgu",	F3(2, 0x2c, 1)|MCOND(CONDGU,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGU,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movgu",	F3(2, 0x2c, 0)|MCOND(CONDGU,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDGU,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movgu",	F3(2, 0x2c, 1)|MCOND(CONDGU,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDGU,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDL,0), MCOND(~FCONDL,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|MCOND(CONDL,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDL,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|MCOND(CONDL,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDL,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 0)|MCOND(CONDL,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDL,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movl",	F3(2, 0x2c, 1)|MCOND(CONDL,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDL,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDLE,0), MCOND(~FCONDLE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|MCOND(CONDLE,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLE,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|MCOND(CONDLE,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLE,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 0)|MCOND(CONDLE,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLE,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movle",	F3(2, 0x2c, 1)|MCOND(CONDLE,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLE,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movleu",	F3(2, 0x2c, 0)|MCOND(CONDLEU,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLEU,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movleu",	F3(2, 0x2c, 1)|MCOND(CONDLEU,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLEU,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movleu",	F3(2, 0x2c, 0)|MCOND(CONDLEU,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDLEU,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movleu",	F3(2, 0x2c, 1)|MCOND(CONDLEU,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDLEU,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movlg",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDLG,0), MCOND(~FCONDLG,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDN,0), MCOND(~FCONDN,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|MCOND(CONDN,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDN,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|MCOND(CONDN,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDN,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 0)|MCOND(CONDN,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDN,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movn",	F3(2, 0x2c, 1)|MCOND(CONDN,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDN,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDNE,0), MCOND(~FCONDNE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|MCOND(CONDNE,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNE,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|MCOND(CONDNE,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNE,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 0)|MCOND(CONDNE,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNE,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movne",	F3(2, 0x2c, 1)|MCOND(CONDNE,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNE,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movneg",	F3(2, 0x2c, 0)|MCOND(CONDNEG,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNEG,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movneg",	F3(2, 0x2c, 1)|MCOND(CONDNEG,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNEG,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movneg",	F3(2, 0x2c, 0)|MCOND(CONDNEG,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNEG,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movneg",	F3(2, 0x2c, 1)|MCOND(CONDNEG,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNEG,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movnz",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDNZ,0), MCOND(~FCONDNZ,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 0)|MCOND(CONDNZ,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNZ,~1)|XCC|(1<<12), "z,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|MCOND(CONDNZ,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNZ,~1)|XCC|(1<<12), "z,I,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 0)|MCOND(CONDNZ,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDNZ,~1)|(1<<12), "Z,2,d", F_ALIAS, v9 },
{ "movnz",	F3(2, 0x2c, 1)|MCOND(CONDNZ,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDNZ,~1)|(1<<12), "Z,I,d", F_ALIAS, v9 },
{ "movo",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movo",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDO,0), MCOND(~FCONDO,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movpos",	F3(2, 0x2c, 0)|MCOND(CONDPOS,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDPOS,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movpos",	F3(2, 0x2c, 1)|MCOND(CONDPOS,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDPOS,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movpos",	F3(2, 0x2c, 0)|MCOND(CONDPOS,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDPOS,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movpos",	F3(2, 0x2c, 1)|MCOND(CONDPOS,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDPOS,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movu",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDU,0), MCOND(~FCONDU,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movue",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDUE,0), MCOND(~FCONDUE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movug",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDUG,0), MCOND(~FCONDUG,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movuge",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDUGE,0), MCOND(~FCONDUGE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movul",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDUL,0), MCOND(~FCONDUL,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", 0, v9 },
{ "movule",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDULE,0), MCOND(~FCONDULE,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", 0, v9 },
{ "movvc",	F3(2, 0x2c, 0)|MCOND(CONDVC,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDVC,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movvc",	F3(2, 0x2c, 1)|MCOND(CONDVC,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDVC,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movvc",	F3(2, 0x2c, 0)|MCOND(CONDVC,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDVC,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movvc",	F3(2, 0x2c, 1)|MCOND(CONDVC,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDVC,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movvs",	F3(2, 0x2c, 0)|MCOND(CONDVS,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDVS,~1)|XCC|(1<<12), "z,2,d", 0, v9 },
{ "movvs",	F3(2, 0x2c, 1)|MCOND(CONDVS,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDVS,~1)|XCC|(1<<12), "z,I,d", 0, v9 },
{ "movvs",	F3(2, 0x2c, 0)|MCOND(CONDVS,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDVS,~1)|(1<<12), "Z,2,d", 0, v9 },
{ "movvs",	F3(2, 0x2c, 1)|MCOND(CONDVS,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDVS,~1)|(1<<12), "Z,I,d", 0, v9 },
{ "movz",	F3(2, 0x2c, 0)|FCC(0)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~0)|F3(~2, ~0x2c, ~0), "6,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|FCC(0)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~0)|F3(~2, ~0x2c, ~1), "6,I,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 0)|FCC(1)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~1)|F3(~2, ~0x2c, ~0), "7,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|FCC(1)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~1)|F3(~2, ~0x2c, ~1), "7,I,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 0)|FCC(2)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~2)|F3(~2, ~0x2c, ~0), "8,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|FCC(2)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~2)|F3(~2, ~0x2c, ~1), "8,I,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 0)|FCC(3)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~3)|F3(~2, ~0x2c, ~0), "9,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|FCC(3)|MCOND(FCONDZ,0), MCOND(~FCONDZ,~0)|FCC(~3)|F3(~2, ~0x2c, ~1), "9,I,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 0)|MCOND(CONDZ,1)|ICC, F3(~2, ~0x2c, ~0)|MCOND(~CONDZ,~1)|XCC|(1<<12), "z,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|MCOND(CONDZ,1)|ICC, F3(~2, ~0x2c, ~1)|MCOND(~CONDZ,~1)|XCC|(1<<12), "z,I,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 0)|MCOND(CONDZ,1)|XCC, F3(~2, ~0x2c, ~0)|MCOND(~CONDZ,~1)|(1<<12), "Z,2,d", F_ALIAS, v9 },
{ "movz",	F3(2, 0x2c, 1)|MCOND(CONDZ,1)|XCC, F3(~2, ~0x2c, ~1)|MCOND(~CONDZ,~1)|(1<<12), "Z,I,d", F_ALIAS, v9 },

{ "fmovda",	F3F(2, 0x35, 0x102)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x102)|MCOND(~CONDA,~0),	"z,f,g", 0, v9 },
{ "fmovda",	F3F(2, 0x35, 0x082)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDA,~0),	"6,f,g", 0, v9 },
{ "fmovda",	F3F(2, 0x35, 0x182)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x182)|MCOND(~CONDA,~0),	"Z,f,g", 0, v9 },
{ "fmovda",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDA,~0),	"7,f,g", 0, v9 },
{ "fmovda",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDA,~0),	"8,f,g", 0, v9 },
{ "fmovda",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDA,~0),	"9,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x103)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDA,~0),	"z,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x083)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDA,~0),	"6,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x183)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDA,~0),	"Z,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDA,~0),	"7,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDA,~0),	"8,f,g", 0, v9 },
{ "fmovqa",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDA,~0),	"9,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x101)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDA,~0),	"z,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x081)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDA,~0),	"6,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x181)|MCOND(CONDA,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDA,~0),	"Z,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDA,~0),	"7,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDA,~0),	"8,f,g", 0, v9 },
{ "fmovsa",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDA,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDA,~0),	"9,f,g", 0, v9 },
{ "fmovdcc",	F3F(2, 0x35, 0x102)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDCC,~0),	"z,f,g", 0, v9 },
{ "fmovdcc",	F3F(2, 0x35, 0x182)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDCC,~0),	"Z,f,g", 0, v9 },
{ "fmovqcc",	F3F(2, 0x35, 0x103)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDCC,~0),	"z,f,g", 0, v9 },
{ "fmovqcc",	F3F(2, 0x35, 0x183)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDCC,~0),	"Z,f,g", 0, v9 },
{ "fmovscc",	F3F(2, 0x35, 0x101)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDCC,~0),	"z,f,g", 0, v9 },
{ "fmovscc",	F3F(2, 0x35, 0x181)|MCOND(CONDCC,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDCC,~0),	"Z,f,g", 0, v9 },
{ "fmovdcs",	F3F(2, 0x35, 0x102)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDCS,~0),	"z,f,g", 0, v9 },
{ "fmovdcs",	F3F(2, 0x35, 0x182)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDCS,~0),	"Z,f,g", 0, v9 },
{ "fmovqcs",	F3F(2, 0x35, 0x103)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDCS,~0),	"z,f,g", 0, v9 },
{ "fmovqcs",	F3F(2, 0x35, 0x183)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDCS,~0),	"Z,f,g", 0, v9 },
{ "fmovscs",	F3F(2, 0x35, 0x101)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDCS,~0),	"z,f,g", 0, v9 },
{ "fmovscs",	F3F(2, 0x35, 0x181)|MCOND(CONDCS,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDCS,~0),	"Z,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x102)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x102)|MCOND(~CONDE,~0),	"z,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x082)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDE,~0),	"6,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x182)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x182)|MCOND(~CONDE,~0),	"Z,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDE,~0),	"7,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDE,~0),	"8,f,g", 0, v9 },
{ "fmovde",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDE,~0),	"9,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x103)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDE,~0),	"z,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x083)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDE,~0),	"6,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x183)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDE,~0),	"Z,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDE,~0),	"7,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDE,~0),	"8,f,g", 0, v9 },
{ "fmovqe",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDE,~0),	"9,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x101)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDE,~0),	"z,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x081)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDE,~0),	"6,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x181)|MCOND(CONDE,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDE,~0),	"Z,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDE,~0),	"7,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDE,~0),	"8,f,g", 0, v9 },
{ "fmovse",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDE,~0),	"9,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x102)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x102)|MCOND(~CONDG,~0),	"z,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x082)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDG,~0),	"6,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x182)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x182)|MCOND(~CONDG,~0),	"Z,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDG,~0),	"7,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDG,~0),	"8,f,g", 0, v9 },
{ "fmovdg",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDG,~0),	"9,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x103)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDG,~0),	"z,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x083)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDG,~0),	"6,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x183)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDG,~0),	"Z,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDG,~0),	"7,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDG,~0),	"8,f,g", 0, v9 },
{ "fmovqg",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDG,~0),	"9,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x101)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDG,~0),	"z,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x081)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDG,~0),	"6,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x181)|MCOND(CONDG,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDG,~0),	"Z,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDG,~0),	"7,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDG,~0),	"8,f,g", 0, v9 },
{ "fmovsg",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDG,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDG,~0),	"9,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x102)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDGE,~0),	"z,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x082)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDGE,~0),	"6,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x182)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDGE,~0),	"Z,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDGE,~0),	"7,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDGE,~0),	"8,f,g", 0, v9 },
{ "fmovdge",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDGE,~0),	"9,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x103)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDGE,~0),	"z,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x083)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDGE,~0),	"6,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x183)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDGE,~0),	"Z,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDGE,~0),	"7,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDGE,~0),	"8,f,g", 0, v9 },
{ "fmovqge",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDGE,~0),	"9,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x101)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDGE,~0),	"z,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x081)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDGE,~0),	"6,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x181)|MCOND(CONDGE,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDGE,~0),	"Z,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDGE,~0),	"7,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDGE,~0),	"8,f,g", 0, v9 },
{ "fmovsge",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDGE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDGE,~0),	"9,f,g", 0, v9 },
{ "fmovdgeu",	F3F(2, 0x35, 0x102)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDGEU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovdgeu",	F3F(2, 0x35, 0x182)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDGEU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovqgeu",	F3F(2, 0x35, 0x103)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDGEU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovqgeu",	F3F(2, 0x35, 0x183)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDGEU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovsgeu",	F3F(2, 0x35, 0x101)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDGEU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovsgeu",	F3F(2, 0x35, 0x181)|MCOND(CONDGEU,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDGEU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovdgu",	F3F(2, 0x35, 0x102)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDGU,~0),	"z,f,g", 0, v9 },
{ "fmovdgu",	F3F(2, 0x35, 0x182)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDGU,~0),	"Z,f,g", 0, v9 },
{ "fmovqgu",	F3F(2, 0x35, 0x103)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDGU,~0),	"z,f,g", 0, v9 },
{ "fmovqgu",	F3F(2, 0x35, 0x183)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDGU,~0),	"Z,f,g", 0, v9 },
{ "fmovsgu",	F3F(2, 0x35, 0x101)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDGU,~0),	"z,f,g", 0, v9 },
{ "fmovsgu",	F3F(2, 0x35, 0x181)|MCOND(CONDGU,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDGU,~0),	"Z,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x102)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x102)|MCOND(~CONDL,~0),	"z,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x082)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDL,~0),	"6,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x182)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x182)|MCOND(~CONDL,~0),	"Z,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDL,~0),	"7,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDL,~0),	"8,f,g", 0, v9 },
{ "fmovdl",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDL,~0),	"9,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x103)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDL,~0),	"z,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x083)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDL,~0),	"6,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x183)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDL,~0),	"Z,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDL,~0),	"7,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDL,~0),	"8,f,g", 0, v9 },
{ "fmovql",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDL,~0),	"9,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x101)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDL,~0),	"z,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x081)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDL,~0),	"6,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x181)|MCOND(CONDL,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDL,~0),	"Z,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDL,~0),	"7,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDL,~0),	"8,f,g", 0, v9 },
{ "fmovsl",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDL,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDL,~0),	"9,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x102)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDLE,~0),	"z,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x082)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDLE,~0),	"6,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x182)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDLE,~0),	"Z,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDLE,~0),	"7,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDLE,~0),	"8,f,g", 0, v9 },
{ "fmovdle",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDLE,~0),	"9,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x103)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDLE,~0),	"z,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x083)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDLE,~0),	"6,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x183)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDLE,~0),	"Z,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDLE,~0),	"7,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDLE,~0),	"8,f,g", 0, v9 },
{ "fmovqle",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDLE,~0),	"9,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x101)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDLE,~0),	"z,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x081)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDLE,~0),	"6,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x181)|MCOND(CONDLE,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDLE,~0),	"Z,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDLE,~0),	"7,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDLE,~0),	"8,f,g", 0, v9 },
{ "fmovsle",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDLE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDLE,~0),	"9,f,g", 0, v9 },
{ "fmovdleu",	F3F(2, 0x35, 0x102)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDLEU,~0),	"z,f,g", 0, v9 },
{ "fmovdleu",	F3F(2, 0x35, 0x182)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDLEU,~0),	"Z,f,g", 0, v9 },
{ "fmovqleu",	F3F(2, 0x35, 0x103)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDLEU,~0),	"z,f,g", 0, v9 },
{ "fmovqleu",	F3F(2, 0x35, 0x183)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDLEU,~0),	"Z,f,g", 0, v9 },
{ "fmovsleu",	F3F(2, 0x35, 0x101)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDLEU,~0),	"z,f,g", 0, v9 },
{ "fmovsleu",	F3F(2, 0x35, 0x181)|MCOND(CONDLEU,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDLEU,~0),	"Z,f,g", 0, v9 },
{ "fmovdlg",	F3F(2, 0x35, 0x082)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDLG,~0),	"6,f,g", 0, v9 },
{ "fmovdlg",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDLG,~0),	"7,f,g", 0, v9 },
{ "fmovdlg",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDLG,~0),	"8,f,g", 0, v9 },
{ "fmovdlg",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDLG,~0),	"9,f,g", 0, v9 },
{ "fmovqlg",	F3F(2, 0x35, 0x083)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDLG,~0),	"6,f,g", 0, v9 },
{ "fmovqlg",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDLG,~0),	"7,f,g", 0, v9 },
{ "fmovqlg",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDLG,~0),	"8,f,g", 0, v9 },
{ "fmovqlg",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDLG,~0),	"9,f,g", 0, v9 },
{ "fmovslg",	F3F(2, 0x35, 0x081)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDLG,~0),	"6,f,g", 0, v9 },
{ "fmovslg",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDLG,~0),	"7,f,g", 0, v9 },
{ "fmovslg",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDLG,~0),	"8,f,g", 0, v9 },
{ "fmovslg",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDLG,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDLG,~0),	"9,f,g", 0, v9 },
{ "fmovdlu",	F3F(2, 0x35, 0x102)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDLU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovdlu",	F3F(2, 0x35, 0x182)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDLU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovqlu",	F3F(2, 0x35, 0x103)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDLU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovqlu",	F3F(2, 0x35, 0x183)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDLU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovslu",	F3F(2, 0x35, 0x101)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDLU,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovslu",	F3F(2, 0x35, 0x181)|MCOND(CONDLU,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDLU,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x102)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x102)|MCOND(~CONDN,~0),	"z,f,g", 0, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x082)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDN,~0),	"6,f,g", 0, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x182)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x182)|MCOND(~CONDN,~0),	"Z,f,g", 0, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDN,~0),	"7,f,g", 0, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDN,~0),	"8,f,g", 0, v9 },
{ "fmovdn",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDN,~0),	"9,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x103)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDN,~0),	"z,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x083)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDN,~0),	"6,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x183)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDN,~0),	"Z,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDN,~0),	"7,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDN,~0),	"8,f,g", 0, v9 },
{ "fmovqn",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDN,~0),	"9,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x101)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDN,~0),	"z,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x081)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDN,~0),	"6,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x181)|MCOND(CONDN,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDN,~0),	"Z,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDN,~0),	"7,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDN,~0),	"8,f,g", 0, v9 },
{ "fmovsn",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDN,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDN,~0),	"9,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x102)|MCOND(CONDNE,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDNE,~0),	"z,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x082)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDNE,~0),	"6,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x182)|MCOND(CONDNE,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDNE,~0),	"Z,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDNE,~0),	"7,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDNE,~0),	"8,f,g", 0, v9 },
{ "fmovdne",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDNE,~0),	"9,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x103)|MCOND(CONDNE,0),  F3F(~2, ~0x35, ~0x103)|MCOND(~CONDNE,~0),	"z,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x083)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDNE,~0),	"6,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x183)|MCOND(CONDNE,0),  F3F(~2, ~0x35, ~0x183)|MCOND(~CONDNE,~0),	"Z,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDNE,~0),	"7,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDNE,~0),	"8,f,g", 0, v9 },
{ "fmovqne",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDNE,~0),	"9,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x101)|MCOND(CONDNE,0),  F3F(~2, ~0x35, ~0x101)|MCOND(~CONDNE,~0),	"z,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x081)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDNE,~0),	"6,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x181)|MCOND(CONDNE,0),  F3F(~2, ~0x35, ~0x181)|MCOND(~CONDNE,~0),	"Z,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDNE,~0),	"7,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDNE,~0),	"8,f,g", 0, v9 },
{ "fmovsne",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDNE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDNE,~0),	"9,f,g", 0, v9 },
{ "fmovdneg",	F3F(2, 0x35, 0x102)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDNEG,~0),	"z,f,g", 0, v9 },
{ "fmovdneg",	F3F(2, 0x35, 0x182)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDNEG,~0),	"Z,f,g", 0, v9 },
{ "fmovqneg",	F3F(2, 0x35, 0x103)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDNEG,~0),	"z,f,g", 0, v9 },
{ "fmovqneg",	F3F(2, 0x35, 0x183)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDNEG,~0),	"Z,f,g", 0, v9 },
{ "fmovsneg",	F3F(2, 0x35, 0x101)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDNEG,~0),	"z,f,g", 0, v9 },
{ "fmovsneg",	F3F(2, 0x35, 0x181)|MCOND(CONDNEG,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDNEG,~0),	"Z,f,g", 0, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x102)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDNZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x082)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDNZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x182)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDNZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDNZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDNZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovdnz",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDNZ,~0),	"9,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x103)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDNZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x083)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDNZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x183)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDNZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDNZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDNZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovqnz",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDNZ,~0),	"9,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x101)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDNZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x081)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDNZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x181)|MCOND(CONDNZ,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDNZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDNZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDNZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovsnz",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDNZ,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDNZ,~0),	"9,f,g", F_ALIAS, v9 },
{ "fmovdo",	F3F(2, 0x35, 0x082)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDO,~0),	"6,f,g", 0, v9 },
{ "fmovdo",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDO,~0),	"7,f,g", 0, v9 },
{ "fmovdo",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDO,~0),	"8,f,g", 0, v9 },
{ "fmovdo",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDO,~0),	"9,f,g", 0, v9 },
{ "fmovqo",	F3F(2, 0x35, 0x083)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDO,~0),	"6,f,g", 0, v9 },
{ "fmovqo",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDO,~0),	"7,f,g", 0, v9 },
{ "fmovqo",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDO,~0),	"8,f,g", 0, v9 },
{ "fmovqo",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDO,~0),	"9,f,g", 0, v9 },
{ "fmovso",	F3F(2, 0x35, 0x081)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDO,~0),	"6,f,g", 0, v9 },
{ "fmovso",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDO,~0),	"7,f,g", 0, v9 },
{ "fmovso",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDO,~0),	"8,f,g", 0, v9 },
{ "fmovso",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDO,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDO,~0),	"9,f,g", 0, v9 },
{ "fmovdpos",	F3F(2, 0x35, 0x102)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDPOS,~0),	"z,f,g", 0, v9 },
{ "fmovdpos",	F3F(2, 0x35, 0x182)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDPOS,~0),	"Z,f,g", 0, v9 },
{ "fmovqpos",	F3F(2, 0x35, 0x103)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDPOS,~0),	"z,f,g", 0, v9 },
{ "fmovqpos",	F3F(2, 0x35, 0x183)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDPOS,~0),	"Z,f,g", 0, v9 },
{ "fmovspos",	F3F(2, 0x35, 0x101)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDPOS,~0),	"z,f,g", 0, v9 },
{ "fmovspos",	F3F(2, 0x35, 0x181)|MCOND(CONDPOS,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDPOS,~0),	"Z,f,g", 0, v9 },
{ "fmovdu",	F3F(2, 0x35, 0x082)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDU,~0),	"6,f,g", 0, v9 },
{ "fmovdu",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDU,~0),	"7,f,g", 0, v9 },
{ "fmovdu",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDU,~0),	"8,f,g", 0, v9 },
{ "fmovdu",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDU,~0),	"9,f,g", 0, v9 },
{ "fmovqu",	F3F(2, 0x35, 0x083)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDU,~0),	"6,f,g", 0, v9 },
{ "fmovqu",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDU,~0),	"7,f,g", 0, v9 },
{ "fmovqu",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDU,~0),	"8,f,g", 0, v9 },
{ "fmovqu",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDU,~0),	"9,f,g", 0, v9 },
{ "fmovsu",	F3F(2, 0x35, 0x081)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDU,~0),	"6,f,g", 0, v9 },
{ "fmovsu",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDU,~0),	"7,f,g", 0, v9 },
{ "fmovsu",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDU,~0),	"8,f,g", 0, v9 },
{ "fmovsu",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDU,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDU,~0),	"9,f,g", 0, v9 },
{ "fmovdue",	F3F(2, 0x35, 0x082)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDUE,~0),	"6,f,g", 0, v9 },
{ "fmovdue",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDUE,~0),	"7,f,g", 0, v9 },
{ "fmovdue",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDUE,~0),	"8,f,g", 0, v9 },
{ "fmovdue",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDUE,~0),	"9,f,g", 0, v9 },
{ "fmovque",	F3F(2, 0x35, 0x083)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDUE,~0),	"6,f,g", 0, v9 },
{ "fmovque",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDUE,~0),	"7,f,g", 0, v9 },
{ "fmovque",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDUE,~0),	"8,f,g", 0, v9 },
{ "fmovque",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDUE,~0),	"9,f,g", 0, v9 },
{ "fmovsue",	F3F(2, 0x35, 0x081)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDUE,~0),	"6,f,g", 0, v9 },
{ "fmovsue",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDUE,~0),	"7,f,g", 0, v9 },
{ "fmovsue",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDUE,~0),	"8,f,g", 0, v9 },
{ "fmovsue",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDUE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDUE,~0),	"9,f,g", 0, v9 },
{ "fmovdug",	F3F(2, 0x35, 0x082)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDUG,~0),	"6,f,g", 0, v9 },
{ "fmovdug",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDUG,~0),	"7,f,g", 0, v9 },
{ "fmovdug",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDUG,~0),	"8,f,g", 0, v9 },
{ "fmovdug",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDUG,~0),	"9,f,g", 0, v9 },
{ "fmovqug",	F3F(2, 0x35, 0x083)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDUG,~0),	"6,f,g", 0, v9 },
{ "fmovqug",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDUG,~0),	"7,f,g", 0, v9 },
{ "fmovqug",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDUG,~0),	"8,f,g", 0, v9 },
{ "fmovqug",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDUG,~0),	"9,f,g", 0, v9 },
{ "fmovsug",	F3F(2, 0x35, 0x081)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDUG,~0),	"6,f,g", 0, v9 },
{ "fmovsug",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDUG,~0),	"7,f,g", 0, v9 },
{ "fmovsug",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDUG,~0),	"8,f,g", 0, v9 },
{ "fmovsug",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDUG,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDUG,~0),	"9,f,g", 0, v9 },
{ "fmovduge",	F3F(2, 0x35, 0x082)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDUGE,~0),	"6,f,g", 0, v9 },
{ "fmovduge",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDUGE,~0),	"7,f,g", 0, v9 },
{ "fmovduge",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDUGE,~0),	"8,f,g", 0, v9 },
{ "fmovduge",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDUGE,~0),	"9,f,g", 0, v9 },
{ "fmovquge",	F3F(2, 0x35, 0x083)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDUGE,~0),	"6,f,g", 0, v9 },
{ "fmovquge",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDUGE,~0),	"7,f,g", 0, v9 },
{ "fmovquge",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDUGE,~0),	"8,f,g", 0, v9 },
{ "fmovquge",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDUGE,~0),	"9,f,g", 0, v9 },
{ "fmovsuge",	F3F(2, 0x35, 0x081)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDUGE,~0),	"6,f,g", 0, v9 },
{ "fmovsuge",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDUGE,~0),	"7,f,g", 0, v9 },
{ "fmovsuge",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDUGE,~0),	"8,f,g", 0, v9 },
{ "fmovsuge",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDUGE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDUGE,~0),	"9,f,g", 0, v9 },
{ "fmovdul",	F3F(2, 0x35, 0x082)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDUL,~0),	"6,f,g", 0, v9 },
{ "fmovdul",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDUL,~0),	"7,f,g", 0, v9 },
{ "fmovdul",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDUL,~0),	"8,f,g", 0, v9 },
{ "fmovdul",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDUL,~0),	"9,f,g", 0, v9 },
{ "fmovqul",	F3F(2, 0x35, 0x083)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDUL,~0),	"6,f,g", 0, v9 },
{ "fmovqul",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDUL,~0),	"7,f,g", 0, v9 },
{ "fmovqul",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDUL,~0),	"8,f,g", 0, v9 },
{ "fmovqul",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDUL,~0),	"9,f,g", 0, v9 },
{ "fmovsul",	F3F(2, 0x35, 0x081)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDUL,~0),	"6,f,g", 0, v9 },
{ "fmovsul",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDUL,~0),	"7,f,g", 0, v9 },
{ "fmovsul",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDUL,~0),	"8,f,g", 0, v9 },
{ "fmovsul",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDUL,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDUL,~0),	"9,f,g", 0, v9 },
{ "fmovdule",	F3F(2, 0x35, 0x082)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDULE,~0),	"6,f,g", 0, v9 },
{ "fmovdule",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDULE,~0),	"7,f,g", 0, v9 },
{ "fmovdule",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDULE,~0),	"8,f,g", 0, v9 },
{ "fmovdule",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDULE,~0),	"9,f,g", 0, v9 },
{ "fmovqule",	F3F(2, 0x35, 0x083)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDULE,~0),	"6,f,g", 0, v9 },
{ "fmovqule",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDULE,~0),	"7,f,g", 0, v9 },
{ "fmovqule",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDULE,~0),	"8,f,g", 0, v9 },
{ "fmovqule",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDULE,~0),	"9,f,g", 0, v9 },
{ "fmovsule",	F3F(2, 0x35, 0x081)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDULE,~0),	"6,f,g", 0, v9 },
{ "fmovsule",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDULE,~0),	"7,f,g", 0, v9 },
{ "fmovsule",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDULE,~0),	"8,f,g", 0, v9 },
{ "fmovsule",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDULE,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDULE,~0),	"9,f,g", 0, v9 },
{ "fmovdvc",	F3F(2, 0x35, 0x102)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDVC,~0),	"z,f,g", 0, v9 },
{ "fmovdvc",	F3F(2, 0x35, 0x182)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDVC,~0),	"Z,f,g", 0, v9 },
{ "fmovqvc",	F3F(2, 0x35, 0x103)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDVC,~0),	"z,f,g", 0, v9 },
{ "fmovqvc",	F3F(2, 0x35, 0x183)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDVC,~0),	"Z,f,g", 0, v9 },
{ "fmovsvc",	F3F(2, 0x35, 0x101)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDVC,~0),	"z,f,g", 0, v9 },
{ "fmovsvc",	F3F(2, 0x35, 0x181)|MCOND(CONDVC,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDVC,~0),	"Z,f,g", 0, v9 },
{ "fmovdvs",	F3F(2, 0x35, 0x102)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x102)|MCOND(~CONDVS,~0),	"z,f,g", 0, v9 },
{ "fmovdvs",	F3F(2, 0x35, 0x182)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x182)|MCOND(~CONDVS,~0),	"Z,f,g", 0, v9 },
{ "fmovqvs",	F3F(2, 0x35, 0x103)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x103)|MCOND(~CONDVS,~0),	"z,f,g", 0, v9 },
{ "fmovqvs",	F3F(2, 0x35, 0x183)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x183)|MCOND(~CONDVS,~0),	"Z,f,g", 0, v9 },
{ "fmovsvs",	F3F(2, 0x35, 0x101)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x101)|MCOND(~CONDVS,~0),	"z,f,g", 0, v9 },
{ "fmovsvs",	F3F(2, 0x35, 0x181)|MCOND(CONDVS,0), F3F(~2, ~0x35, ~0x181)|MCOND(~CONDVS,~0),	"Z,f,g", 0, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x102)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x102)|MCOND(~CONDZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x082)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x082)|MCOND(~FCONDZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x182)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x182)|MCOND(~CONDZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x0a2)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0a2)|MCOND(~FCONDZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x0c2)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0c2)|MCOND(~FCONDZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovdz",	F3F(2, 0x35, 0x0e2)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0e2)|MCOND(~FCONDZ,~0),	"9,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x103)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x103)|MCOND(~CONDZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x083)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x083)|MCOND(~FCONDZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x183)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x183)|MCOND(~CONDZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x0a3)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0a3)|MCOND(~FCONDZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x0c3)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0c3)|MCOND(~FCONDZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovqz",	F3F(2, 0x35, 0x0e3)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0e3)|MCOND(~FCONDZ,~0),	"9,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x101)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x101)|MCOND(~CONDZ,~0),	"z,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x081)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x081)|MCOND(~FCONDZ,~0),	"6,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x181)|MCOND(CONDZ,0),   F3F(~2, ~0x35, ~0x181)|MCOND(~CONDZ,~0),	"Z,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x0a1)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0a1)|MCOND(~FCONDZ,~0),	"7,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x0c1)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0c1)|MCOND(~FCONDZ,~0),	"8,f,g", F_ALIAS, v9 },
{ "fmovsz",	F3F(2, 0x35, 0x0e1)|MCOND(FCONDZ,0), F3F(~2, ~0x35, ~0x0e1)|MCOND(~FCONDZ,~0),	"9,f,g", F_ALIAS, v9 },

#define brfc(opcode, mask, lose, flags) \
 { opcode, (mask), ANNUL|(lose), "l",    flags|F_DELAYED, v6 }, \
 { opcode, (mask)|ANNUL, (lose), ",a l", flags|F_DELAYED, v6 }

#define brfcx(opcode, mask, lose, flags) /* v9 */ \
 { opcode, FBFCC(0)|(mask), ANNUL|BPRED|FBFCC(~0)|(lose), "6,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(0)|(mask)|ANNUL, BPRED|FBFCC(~0)|(lose), ",a 6,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(0)|(mask), ANNUL|BPRED|FBFCC(~0)|(lose), ",N 6,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(0)|(mask)|ANNUL, BPRED|FBFCC(~0)|(lose), ",a,N 6,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(0)|(mask)|BPRED, ANNUL|FBFCC(~0)|(lose), ",T 6,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(0)|(mask)|BPRED|ANNUL, FBFCC(~0)|(lose), ",a,T 6,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask), ANNUL|BPRED|FBFCC(~1)|(lose), "7,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask)|ANNUL, BPRED|FBFCC(~1)|(lose), ",a 7,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask), ANNUL|BPRED|FBFCC(~1)|(lose), ",N 7,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask)|ANNUL, BPRED|FBFCC(~1)|(lose), ",a,N 7,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask)|BPRED, ANNUL|FBFCC(~1)|(lose), ",T 7,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(1)|(mask)|BPRED|ANNUL, FBFCC(~1)|(lose), ",a,T 7,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask), ANNUL|BPRED|FBFCC(~2)|(lose), "8,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask)|ANNUL, BPRED|FBFCC(~2)|(lose), ",a 8,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask), ANNUL|BPRED|FBFCC(~2)|(lose), ",N 8,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask)|ANNUL, BPRED|FBFCC(~2)|(lose), ",a,N 8,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask)|BPRED, ANNUL|FBFCC(~2)|(lose), ",T 8,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(2)|(mask)|BPRED|ANNUL, FBFCC(~2)|(lose), ",a,T 8,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask), ANNUL|BPRED|FBFCC(~3)|(lose), "9,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask)|ANNUL, BPRED|FBFCC(~3)|(lose), ",a 9,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask), ANNUL|BPRED|FBFCC(~3)|(lose), ",N 9,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask)|ANNUL, BPRED|FBFCC(~3)|(lose), ",a,N  9,G", flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask)|BPRED, ANNUL|FBFCC(~3)|(lose), ",T 9,G",   flags|F_DELAYED, v9 }, \
 { opcode, FBFCC(3)|(mask)|BPRED|ANNUL, FBFCC(~3)|(lose), ",a,T 9,G", flags|F_DELAYED, v9 }

#define condfc(fop, cop, mask, flags) \
  brfc(fop, F2(0, 6)|COND(mask), F2(~0, ~6)|COND(~(mask)), flags), \
  brfcx(fop, F2(0, 5)|COND(mask), F2(~0, ~5)|COND(~(mask)), flags), /* v9 */ \
  brfc(cop, F2(0, 7)|COND(mask), F2(~0, ~7)|COND(~(mask)), flags) \

#define condf(fop, mask, flags) \
  brfcx(fop, F2(0, 5)|COND(mask), F2(~0, ~5)|COND(~(mask)), flags), /* v9 */ \
  brfc(fop, F2(0, 6)|COND(mask), F2(~0, ~6)|COND(~(mask)), flags)

condfc("fb",	"cb",	 0x8, 0),
condfc("fba",	"cba",	 0x8, F_ALIAS),
condfc("fbe",	"cb0",	 0x9, 0),
condf("fbz",		 0x9, F_ALIAS),
condfc("fbg",	"cb2",	 0x6, 0),
condfc("fbge",	"cb02",	 0xb, 0),
condfc("fbl",	"cb1",	 0x4, 0),
condfc("fble",	"cb01",	 0xd, 0),
condfc("fblg",	"cb12",	 0x2, 0),
condfc("fbn",	"cbn",	 0x0, 0),
condfc("fbne",	"cb123", 0x1, 0),
condf("fbnz",		 0x1, F_ALIAS),
condfc("fbo",	"cb012", 0xf, 0),
condfc("fbu",	"cb3",	 0x7, 0),
condfc("fbue",	"cb03",	 0xa, 0),
condfc("fbug",	"cb23",	 0x5, 0),
condfc("fbuge",	"cb023", 0xc, 0),
condfc("fbul",	"cb13",	 0x3, 0),
condfc("fbule",	"cb013", 0xe, 0),

#undef condfc
#undef brfc
#undef brfcx	/* v9 */

{ "jmp",	F3(2, 0x38, 0), F3(~2, ~0x38, ~0)|RD_G0|ASI(~0),	"1+2", F_DELAYED, v6 }, /* jmpl rs1+rs2,%g0 */
{ "jmp",	F3(2, 0x38, 0), F3(~2, ~0x38, ~0)|RD_G0|ASI_RS2(~0),	"1", F_DELAYED, v6 }, /* jmpl rs1+%g0,%g0 */
{ "jmp",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1)|RD_G0,		"1+i", F_DELAYED, v6 }, /* jmpl rs1+i,%g0 */
{ "jmp",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1)|RD_G0,		"i+1", F_DELAYED, v6 }, /* jmpl i+rs1,%g0 */
{ "jmp",	F3(2, 0x38, 1), F3(~2, ~0x38, ~1)|RD_G0|RS1_G0,	"i", F_DELAYED, v6 }, /* jmpl %g0+i,%g0 */

{ "nop",	F2(0, 4), 0xfeffffff, "", 0, v6 }, /* sethi 0, %g0 */

{ "set",	F2(0x0, 0x4), F2(~0x0, ~0x4), "Sh,d", F_ALIAS, v6 },

{ "sethi",	F2(0x0, 0x4), F2(~0x0, ~0x4), "h,d", 0, v6 },

{ "taddcc",	F3(2, 0x20, 0), F3(~2, ~0x20, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "taddcc",	F3(2, 0x20, 1), F3(~2, ~0x20, ~1),		"1,i,d", 0, v6 },
{ "taddcc",	F3(2, 0x20, 1), F3(~2, ~0x20, ~1),		"i,1,d", 0, v6 },
{ "taddcctv",	F3(2, 0x22, 0), F3(~2, ~0x22, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "taddcctv",	F3(2, 0x22, 1), F3(~2, ~0x22, ~1),		"1,i,d", 0, v6 },
{ "taddcctv",	F3(2, 0x22, 1), F3(~2, ~0x22, ~1),		"i,1,d", 0, v6 },

{ "tsubcc",	F3(2, 0x21, 0), F3(~2, ~0x21, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "tsubcc",	F3(2, 0x21, 1), F3(~2, ~0x21, ~1),		"1,i,d", 0, v6 },
{ "tsubcctv",	F3(2, 0x23, 0), F3(~2, ~0x23, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "tsubcctv",	F3(2, 0x23, 1), F3(~2, ~0x23, ~1),		"1,i,d", 0, v6 },

/* FIXME Thise is marked F_ALIAS, so that it won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "unimp",	F2(0x0, 0x0), 0xffc00000, "n", F_ALIAS, v6 },
{ "illtrap",	F2(0, 0), F2(~0, ~0)|RD_G0, "n", 0, v9 },

/* This *is* a commutative instruction.  */
{ "xnor",	F3(2, 0x07, 0), F3(~2, ~0x07, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "xnor",	F3(2, 0x07, 1), F3(~2, ~0x07, ~1),		"1,i,d", 0, v6 },
{ "xnor",	F3(2, 0x07, 1), F3(~2, ~0x07, ~1),		"i,1,d", 0, v6 },
/* This *is* a commutative instruction.  */
{ "xnorcc",	F3(2, 0x17, 0), F3(~2, ~0x17, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "xnorcc",	F3(2, 0x17, 1), F3(~2, ~0x17, ~1),		"1,i,d", 0, v6 },
{ "xnorcc",	F3(2, 0x17, 1), F3(~2, ~0x17, ~1),		"i,1,d", 0, v6 },
{ "xor",	F3(2, 0x03, 0), F3(~2, ~0x03, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "xor",	F3(2, 0x03, 1), F3(~2, ~0x03, ~1),		"1,i,d", 0, v6 },
{ "xor",	F3(2, 0x03, 1), F3(~2, ~0x03, ~1),		"i,1,d", 0, v6 },
{ "xorcc",	F3(2, 0x13, 0), F3(~2, ~0x13, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "xorcc",	F3(2, 0x13, 1), F3(~2, ~0x13, ~1),		"1,i,d", 0, v6 },
{ "xorcc",	F3(2, 0x13, 1), F3(~2, ~0x13, ~1),		"i,1,d", 0, v6 },

{ "not",	F3(2, 0x07, 0), F3(~2, ~0x07, ~0)|ASI(~0), "1,d", F_ALIAS, v6 }, /* xnor rs1,%0,rd */
{ "not",	F3(2, 0x07, 0), F3(~2, ~0x07, ~0)|ASI(~0), "r", F_ALIAS, v6 }, /* xnor rd,%0,rd */

{ "btog",	F3(2, 0x03, 0), F3(~2, ~0x03, ~0)|ASI(~0),	"2,r", F_ALIAS, v6 }, /* xor rd,rs2,rd */
{ "btog",	F3(2, 0x03, 1), F3(~2, ~0x03, ~1),		"i,r", F_ALIAS, v6 }, /* xor rd,i,rd */

/* FPop1 and FPop2 are not instructions.  Don't accept them.  */

{ "fdtoi",	F3F(2, 0x34, 0x0d2), F3F(~2, ~0x34, ~0x0d2)|RS1_G0, "B,g", 0, v6 },
{ "fstoi",	F3F(2, 0x34, 0x0d1), F3F(~2, ~0x34, ~0x0d1)|RS1_G0, "f,g", 0, v6 },
{ "fqtoi",	F3F(2, 0x34, 0x0d3), F3F(~2, ~0x34, ~0x0d3)|RS1_G0, "R,g", 0, v8 },

{ "fdtox",	F3F(2, 0x34, 0x082), F3F(~2, ~0x34, ~0x082)|RS1_G0, "B,g", 0, v9 },
{ "fstox",	F3F(2, 0x34, 0x081), F3F(~2, ~0x34, ~0x081)|RS1_G0, "f,g", 0, v9 },
{ "fqtox",	F3F(2, 0x34, 0x083), F3F(~2, ~0x34, ~0x083)|RS1_G0, "R,g", 0, v9 },

{ "fitod",	F3F(2, 0x34, 0x0c8), F3F(~2, ~0x34, ~0x0c8)|RS1_G0, "f,H", 0, v6 },
{ "fitos",	F3F(2, 0x34, 0x0c4), F3F(~2, ~0x34, ~0x0c4)|RS1_G0, "f,g", 0, v6 },
{ "fitoq",	F3F(2, 0x34, 0x0cc), F3F(~2, ~0x34, ~0x0cc)|RS1_G0, "f,J", 0, v8 },

{ "fxtod",	F3F(2, 0x34, 0x088), F3F(~2, ~0x34, ~0x088)|RS1_G0, "f,H", 0, v9 },
{ "fxtos",	F3F(2, 0x34, 0x084), F3F(~2, ~0x34, ~0x084)|RS1_G0, "f,g", 0, v9 },
{ "fxtoq",	F3F(2, 0x34, 0x08c), F3F(~2, ~0x34, ~0x08c)|RS1_G0, "f,J", 0, v9 },

{ "fdtoq",	F3F(2, 0x34, 0x0ce), F3F(~2, ~0x34, ~0x0ce)|RS1_G0, "B,J", 0, v8 },
{ "fdtos",	F3F(2, 0x34, 0x0c6), F3F(~2, ~0x34, ~0x0c6)|RS1_G0, "B,g", 0, v6 },
{ "fqtod",	F3F(2, 0x34, 0x0cb), F3F(~2, ~0x34, ~0x0cb)|RS1_G0, "R,H", 0, v8 },
{ "fqtos",	F3F(2, 0x34, 0x0c7), F3F(~2, ~0x34, ~0x0c7)|RS1_G0, "R,g", 0, v8 },
{ "fstod",	F3F(2, 0x34, 0x0c9), F3F(~2, ~0x34, ~0x0c9)|RS1_G0, "f,H", 0, v6 },
{ "fstoq",	F3F(2, 0x34, 0x0cd), F3F(~2, ~0x34, ~0x0cd)|RS1_G0, "f,J", 0, v8 },

{ "fdivd",	F3F(2, 0x34, 0x04e), F3F(~2, ~0x34, ~0x04e), "v,B,H", 0, v6 },
{ "fdivq",	F3F(2, 0x34, 0x04f), F3F(~2, ~0x34, ~0x04f), "V,R,J", 0, v8 },
{ "fdivs",	F3F(2, 0x34, 0x04d), F3F(~2, ~0x34, ~0x04d), "e,f,g", 0, v6 },
{ "fmuld",	F3F(2, 0x34, 0x04a), F3F(~2, ~0x34, ~0x04a), "v,B,H", 0, v6 },
{ "fmulq",	F3F(2, 0x34, 0x04b), F3F(~2, ~0x34, ~0x04b), "V,R,J", 0, v8 },
{ "fmuls",	F3F(2, 0x34, 0x049), F3F(~2, ~0x34, ~0x049), "e,f,g", 0, v6 },

{ "fdmulq",	F3F(2, 0x34, 0x06e), F3F(~2, ~0x34, ~0x06e), "v,B,J", 0, v8 },
{ "fsmuld",	F3F(2, 0x34, 0x069), F3F(~2, ~0x34, ~0x069), "e,f,H", 0, v8 },

{ "fsqrtd",	F3F(2, 0x34, 0x02a), F3F(~2, ~0x34, ~0x02a)|RS1_G0, "B,H", 0, v7 },
{ "fsqrtq",	F3F(2, 0x34, 0x02b), F3F(~2, ~0x34, ~0x02b)|RS1_G0, "R,J", 0, v8 },
{ "fsqrts",	F3F(2, 0x34, 0x029), F3F(~2, ~0x34, ~0x029)|RS1_G0, "f,g", 0, v7 },

{ "fabsd",	F3F(2, 0x34, 0x00a), F3F(~2, ~0x34, ~0x00a)|RS1_G0, "B,H", 0, v9 },
{ "fabsq",	F3F(2, 0x34, 0x00b), F3F(~2, ~0x34, ~0x00b)|RS1_G0, "R,J", 0, v9 },
{ "fabss",	F3F(2, 0x34, 0x009), F3F(~2, ~0x34, ~0x009)|RS1_G0, "f,g", 0, v6 },
{ "fmovd",	F3F(2, 0x34, 0x002), F3F(~2, ~0x34, ~0x002)|RS1_G0, "B,H", 0, v9 },
{ "fmovq",	F3F(2, 0x34, 0x003), F3F(~2, ~0x34, ~0x003)|RS1_G0, "R,J", 0, v9 },
{ "fmovs",	F3F(2, 0x34, 0x001), F3F(~2, ~0x34, ~0x001)|RS1_G0, "f,g", 0, v6 },
{ "fnegd",	F3F(2, 0x34, 0x006), F3F(~2, ~0x34, ~0x006)|RS1_G0, "B,H", 0, v9 },
{ "fnegq",	F3F(2, 0x34, 0x007), F3F(~2, ~0x34, ~0x007)|RS1_G0, "R,J", 0, v9 },
{ "fnegs",	F3F(2, 0x34, 0x005), F3F(~2, ~0x34, ~0x005)|RS1_G0, "f,g", 0, v6 },

{ "faddd",	F3F(2, 0x34, 0x042), F3F(~2, ~0x34, ~0x042), "v,B,H", 0, v6 },
{ "faddq",	F3F(2, 0x34, 0x043), F3F(~2, ~0x34, ~0x043), "V,R,J", 0, v8 },
{ "fadds",	F3F(2, 0x34, 0x041), F3F(~2, ~0x34, ~0x041), "e,f,g", 0, v6 },
{ "fsubd",	F3F(2, 0x34, 0x046), F3F(~2, ~0x34, ~0x046), "v,B,H", 0, v6 },
{ "fsubq",	F3F(2, 0x34, 0x047), F3F(~2, ~0x34, ~0x047), "V,R,J", 0, v8 },
{ "fsubs",	F3F(2, 0x34, 0x045), F3F(~2, ~0x34, ~0x045), "e,f,g", 0, v6 },

#define CMPFCC(x)	(((x)&0x3)<<25)

{ "fcmpd",	          F3F(2, 0x35, 0x052),            F3F(~2, ~0x35, ~0x052)|RD_G0,  "v,B",   0, v6 },
{ "fcmpd",	CMPFCC(0)|F3F(2, 0x35, 0x052), CMPFCC(~0)|F3F(~2, ~0x35, ~0x052),	 "6,v,B", 0, v9 },
{ "fcmpd",	CMPFCC(1)|F3F(2, 0x35, 0x052), CMPFCC(~1)|F3F(~2, ~0x35, ~0x052),	 "7,v,B", 0, v9 },
{ "fcmpd",	CMPFCC(2)|F3F(2, 0x35, 0x052), CMPFCC(~2)|F3F(~2, ~0x35, ~0x052),	 "8,v,B", 0, v9 },
{ "fcmpd",	CMPFCC(3)|F3F(2, 0x35, 0x052), CMPFCC(~3)|F3F(~2, ~0x35, ~0x052),	 "9,v,B", 0, v9 },
{ "fcmped",	          F3F(2, 0x35, 0x056),            F3F(~2, ~0x35, ~0x056)|RD_G0,  "v,B",   0, v6 },
{ "fcmped",	CMPFCC(0)|F3F(2, 0x35, 0x056), CMPFCC(~0)|F3F(~2, ~0x35, ~0x056),	 "6,v,B", 0, v9 },
{ "fcmped",	CMPFCC(1)|F3F(2, 0x35, 0x056), CMPFCC(~1)|F3F(~2, ~0x35, ~0x056),	 "7,v,B", 0, v9 },
{ "fcmped",	CMPFCC(2)|F3F(2, 0x35, 0x056), CMPFCC(~2)|F3F(~2, ~0x35, ~0x056),	 "8,v,B", 0, v9 },
{ "fcmped",	CMPFCC(3)|F3F(2, 0x35, 0x056), CMPFCC(~3)|F3F(~2, ~0x35, ~0x056),	 "9,v,B", 0, v9 },
{ "fcmpq",	          F3F(2, 0x34, 0x053),            F3F(~2, ~0x34, ~0x053)|RD_G0,	 "V,R", 0, v8 },
{ "fcmpq",	CMPFCC(0)|F3F(2, 0x35, 0x053), CMPFCC(~0)|F3F(~2, ~0x35, ~0x053),	 "6,V,R", 0, v9 },
{ "fcmpq",	CMPFCC(1)|F3F(2, 0x35, 0x053), CMPFCC(~1)|F3F(~2, ~0x35, ~0x053),	 "7,V,R", 0, v9 },
{ "fcmpq",	CMPFCC(2)|F3F(2, 0x35, 0x053), CMPFCC(~2)|F3F(~2, ~0x35, ~0x053),	 "8,V,R", 0, v9 },
{ "fcmpq",	CMPFCC(3)|F3F(2, 0x35, 0x053), CMPFCC(~3)|F3F(~2, ~0x35, ~0x053),	 "9,V,R", 0, v9 },
{ "fcmpeq",	          F3F(2, 0x34, 0x057),            F3F(~2, ~0x34, ~0x057)|RD_G0,	 "V,R", 0, v8 },
{ "fcmpeq",	CMPFCC(0)|F3F(2, 0x35, 0x057), CMPFCC(~0)|F3F(~2, ~0x35, ~0x057),	 "6,V,R", 0, v9 },
{ "fcmpeq",	CMPFCC(1)|F3F(2, 0x35, 0x057), CMPFCC(~1)|F3F(~2, ~0x35, ~0x057),	 "7,V,R", 0, v9 },
{ "fcmpeq",	CMPFCC(2)|F3F(2, 0x35, 0x057), CMPFCC(~2)|F3F(~2, ~0x35, ~0x057),	 "8,V,R", 0, v9 },
{ "fcmpeq",	CMPFCC(3)|F3F(2, 0x35, 0x057), CMPFCC(~3)|F3F(~2, ~0x35, ~0x057),	 "9,V,R", 0, v9 },
{ "fcmps",	          F3F(2, 0x35, 0x051),            F3F(~2, ~0x35, ~0x051)|RD_G0, "e,f",   0, v6 },
{ "fcmps",	CMPFCC(0)|F3F(2, 0x35, 0x051), CMPFCC(~0)|F3F(~2, ~0x35, ~0x051),	 "6,e,f", 0, v9 },
{ "fcmps",	CMPFCC(1)|F3F(2, 0x35, 0x051), CMPFCC(~1)|F3F(~2, ~0x35, ~0x051),	 "7,e,f", 0, v9 },
{ "fcmps",	CMPFCC(2)|F3F(2, 0x35, 0x051), CMPFCC(~2)|F3F(~2, ~0x35, ~0x051),	 "8,e,f", 0, v9 },
{ "fcmps",	CMPFCC(3)|F3F(2, 0x35, 0x051), CMPFCC(~3)|F3F(~2, ~0x35, ~0x051),	 "9,e,f", 0, v9 },
{ "fcmpes",	          F3F(2, 0x35, 0x055),            F3F(~2, ~0x35, ~0x055)|RD_G0, "e,f",   0, v6 },
{ "fcmpes",	CMPFCC(0)|F3F(2, 0x35, 0x055), CMPFCC(~0)|F3F(~2, ~0x35, ~0x055),	 "6,e,f", 0, v9 },
{ "fcmpes",	CMPFCC(1)|F3F(2, 0x35, 0x055), CMPFCC(~1)|F3F(~2, ~0x35, ~0x055),	 "7,e,f", 0, v9 },
{ "fcmpes",	CMPFCC(2)|F3F(2, 0x35, 0x055), CMPFCC(~2)|F3F(~2, ~0x35, ~0x055),	 "8,e,f", 0, v9 },
{ "fcmpes",	CMPFCC(3)|F3F(2, 0x35, 0x055), CMPFCC(~3)|F3F(~2, ~0x35, ~0x055),	 "9,e,f", 0, v9 },

/* FIXME These are marked F_ALIAS, so that they won't conflict with new v9
   insns when v9 is present.  Otherwise, the F_ALIAS flag is ignored.  */
{ "cpop1",	F3(2, 0x36, 0), F3(~2, ~0x36, ~1), "[1+2],d", F_ALIAS, v6 },
{ "cpop2",	F3(2, 0x37, 0), F3(~2, ~0x37, ~1), "[1+2],d", F_ALIAS, v6 },
{ "impdep1",	F3(2, 0x36, 0), F3(~2, ~0x36, ~1), "[1+2],d", 0, v9 },
{ "impdep2",	F3(2, 0x37, 0), F3(~2, ~0x37, ~1), "[1+2],d", 0, v9 },
    
{ "casa",	F3(3, 0x3c, 0), F3(~3, ~0x3c, ~0), "[1]A,2,d", 0, v9 },
{ "casa",	F3(3, 0x3c, 1), F3(~3, ~0x3c, ~1), "[1]o,2,d", 0, v9 },
{ "casxa",	F3(3, 0x3e, 0), F3(~3, ~0x3e, ~0), "[1]A,2,d", 0, v9 },
{ "casxa",	F3(3, 0x3e, 1), F3(~3, ~0x3e, ~1), "[1]o,2,d", 0, v9 },

};

const int bfd_sparc_num_opcodes = ((sizeof sparc_opcodes)/(sizeof sparc_opcodes[0]));
