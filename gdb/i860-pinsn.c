/* Print i860 instructions for GDB, the GNU debugger.
   Copyright status of this module is unclear!!!
   Copyright (C) 1992 Free Software Foundation, Inc.
   SVR4 changes Contributed by Peggy Fieland (pfieland@stratus.com)

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

#include <stdio.h>

#include "defs.h"
#include "tm-i860.h"
#include "i860-opcode.h"

/* i860 instructions are never longer than this many bytes.  */
#define MAXLEN 4

static int fp_instr();
static void fld_offset();
static void gen_rrr();
static void gen_irr();
static void ctrl_a();

/* 
 * integer registers names
 */
static char *ireg[32] = 
  {
	"r0",	"r1",	"sp",	"fp",	"r4",	"r5",	"r6",	"r7",	
	"r8",	"r9",	"r10",	"r11",	"r12",	"r13",	"r14",	"r15",
	"r16",	"r17",	"r18",	"r19",	"r20",	"r21",	"r22",	"r23",
	"r24",	"r25",	"r26",	"r27",	"r28",	"r29",	"r30",	"r31",
  };

/* 
 * Control registers of the ld.c and st.c instructions 
 */
static char *ctlreg[32] = 
  {
	"fir", "psr", "dirbase", "db", "fsr", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?"
  };


/***********************************************************************
 * Print the i860 instruction at address MEMADDR in debugged memory,
 * on STREAM.  Returns length of the instruction, in bytes, which
 * is always 4.  
 */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;		/* address of the instruction */
     FILE *stream;		/* stream on which to write result */
{
  union insn_fmt insn;		/* the instruction we're decoding */
  long offset;			/* the (decoded) offset from the instruction */
  long split_offset;		/* the value of a ld/st-style split offset */
  int ai;			/* autoincrement flag */
  char suffix;			/* length suffix */

  adj_read_memory (memaddr, &insn, MAXLEN);

  /* These offsets used in ld, st, bte, etc. instructions and are formed by
   * combining 2 separate fields within the instruction and sign-extending 
   * the result
   */

  split_offset = (insn.gen.dest << 11) | insn.gen.offset;
  split_offset = SIGN_EXT(16, split_offset);


  switch (insn.gen.op1)
    {
    case 000:
      fprintf (stream, "ld.b %s(%s),%s", ireg[insn.gen.src1], 
		ireg[insn.gen.src2], ireg[insn.gen.dest]);
      break;
    case 001:
      offset = SIGN_EXT(16, insn.geni.offset);
      fprintf (stream, "ld.b 0x%x(%s),%s", offset, 
		ireg[insn.geni.src2], ireg[insn.geni.dest]);
      break;
    case 002:
      fprintf (stream, "ixfr %s,f%d", ireg[insn.gen.src1], insn.gen.dest);
      break;
    case 003:
      fprintf (stream, "st.b %s,0x%x(%s)", ireg[insn.gen.src1], split_offset,
		ireg[insn.geni.src2]);
      break;
    case 004:
      fprintf (stream, "ld.%c %s(%s),%s", (insn.gen.offset & 1) ? 'l' : 's',
		ireg[insn.gen.src1], ireg[insn.gen.src2], ireg[insn.gen.dest]);
      break;
    case 005:
      offset = SIGN_EXT(16, insn.geni.offset);
      fprintf (stream, "ld.%c 0x%x(%s),%s", (insn.geni.offset & 1) ? 'l' : 's',
		(offset & ~1), ireg[insn.geni.src2], ireg[insn.geni.dest]);
      break;

    case 007:
      fprintf (stream, "st.%c %s,0x%x(%s)", (insn.geni.offset & 1) ? 'l' : 's',
		ireg[insn.gen.src1], (split_offset & ~1), ireg[insn.geni.src2]);
      break;

    case 010:
      offset = insn.gen.offset;
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "fld.%c %s(%s)%s,f%d", suffix,
		ireg[insn.gen.src1], ireg[insn.gen.src2], ai ? "++" : "", 
		insn.gen.dest);
      break;
    case 011:
      offset = SIGN_EXT(16, insn.geni.offset);
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "fld.%c 0x%x(%s)%s,f%d", suffix,
		offset, ireg[insn.gen.src2], ai ? "++" : "", insn.gen.dest);
      break;
    case 012:
      offset = insn.gen.offset;
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "fst.%c f%d,%s(%s)%s", suffix,
		insn.gen.dest, ireg[insn.gen.src1], ireg[insn.gen.src2], 
		ai ? "++" : "");
      break;
    case 013:
      offset = SIGN_EXT(16, insn.geni.offset);
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "fst.%c f%d,0x%x(%s)%s", suffix,
		insn.gen.dest, offset, ireg[insn.gen.src2], ai ? "++" : "");
      break;
    case 014:
      fprintf (stream, "ld.c %s,%s", ctlreg[insn.gen.src2], 
		ireg[insn.gen.dest]);
      break;
    case 015:
      offset = SIGN_EXT(16, insn.geni.offset);
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "flush 0x%x(%s)%s", offset, ireg[insn.gen.src2], 
		ai ? "++" : "");
      break;
    case 016:
      fprintf (stream, "st.c %s,%s", ireg[insn.gen.src1], 
		ctlreg[insn.gen.src2]);
      break;
    case 017:
      offset = SIGN_EXT(16, insn.geni.offset);
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "pst.d f%d,0x%x(%s)%s", insn.gen.dest, 
		offset, ireg[insn.gen.src2], ai ? "++" : "");
      break;

    case 020:
      fprintf (stream, "bri %s", ireg[insn.gen.src1]);
      break;
    case 021:
      gen_rrr("trap", insn, stream);
      break;
    case 022:
      /*
       * Floating-point Opcodes
       */
      if (!fp_instr(insn.fp, stream))
	fprintf (stream, "0x%08x (invalid instruction)", insn.int_val);
      break;
    case 023:
      /*
       * Core Escape Opcodes
       */
      switch (insn.esc.op2)
	{
	case 1: 
	  fprintf (stream, "lock");
	  break;
	case 2:
	  fprintf (stream, "calli %s", ireg[insn.esc.src1]);
	  break;
	case 4: 
	  fprintf (stream, "intovr");
	  break;
	case 7: 
	  fprintf (stream, "unlock");
	  break;
	default:
	  fprintf (stream, "0x%08x (invalid instruction)", insn.int_val);
	  break;
	}
      break;

    case 024:
      fprintf (stream, "btne %s,%s,", ireg[insn.gen.src1], 
		ireg[insn.gen.src2]);
      offset = split_offset << 2;
      print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
      break;
    case 025:
      fprintf (stream, "btne 0x%x,%s,", insn.gen.src1, ireg[insn.gen.src2]);
      offset = split_offset << 2;
      print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
      break;
    case 026:
      fprintf (stream, "bte %s,%s,", ireg[insn.gen.src1], 
		ireg[insn.gen.src2]);
      offset = split_offset << 2;
      print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
      break;
    case 027:
      fprintf (stream, "bte 0x%x,%s,", insn.gen.src1, ireg[insn.gen.src2]);
      offset = split_offset << 2;
      print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
      break;

    case 030:
      offset = insn.gen.offset;
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "pfld.%c %s(%s)%s,f%d", suffix, 
      		ireg[insn.gen.src1], ireg[insn.gen.src2], ai ? "++" : "", 
		insn.gen.dest);
      break;
    case 031:
      offset = SIGN_EXT(16, insn.geni.offset);
      fld_offset(&offset, &suffix, &ai);

      fprintf (stream, "pfld.%c 0x%x(%s)%s,f%d", suffix, 
      		offset, ireg[insn.gen.src2], ai ? "++" : "", insn.gen.dest);
      break;
    case 032:
      ctrl_a("br", insn, memaddr, stream);
      break;
    case 033:
      ctrl_a("call", insn, memaddr, stream);
      break;
    case 034:
      ctrl_a("bc", insn, memaddr, stream);
      break;
    case 035:
      ctrl_a("bc.t", insn, memaddr, stream);
      break;
    case 036:
      ctrl_a("bnc", insn, memaddr, stream);
      break;
    case 037:
      ctrl_a("bnc.t", insn, memaddr, stream);
      break;

    case 040:
      gen_rrr("addu", insn, stream);
      break;
    case 041:
      gen_irr("addu", insn, SIGN_EXT(16, insn.geni.offset), stream);
      break;
    case 042:
      gen_rrr("subu", insn, stream);
      break;
    case 043:
      gen_irr("subu", insn, SIGN_EXT(16, insn.geni.offset), stream);
      break;
    case 044:
      gen_rrr("adds", insn, stream);
      break;
    case 045:
      gen_irr("adds", insn, SIGN_EXT(16, insn.geni.offset), stream);
      break;
    case 046:
      gen_rrr("subs", insn, stream);
      break;
    case 047:
      gen_irr("subs", insn, SIGN_EXT(16, insn.geni.offset), stream);
      break;

    case 050:
      if (insn.gen.src1 == 0)
	{
	  if (insn.gen.src2 == 0 && insn.gen.dest == 0)
	    fprintf (stream, "nop");
	  else
	    fprintf (stream, "mov %s,%s", ireg[insn.gen.src2], 
			ireg[insn.gen.dest]);
	}
      else
	gen_rrr("shl", insn, stream);
      break;
    case 051:
      gen_irr("shl", insn, insn.geni.offset, stream);
      break;
    case 052:
      gen_rrr("shr", insn, stream);
      break;
    case 053:
      gen_irr("shr", insn, insn.geni.offset, stream);
      break;
    case 054:
      if (insn.gen.src1 == 0 && insn.gen.src2 == 0 && insn.gen.dest == 0)
	{
	  if ((insn.int_val & (1 << 9)) != 0)
	    fprintf (stream, "d.");
	  fprintf (stream, "fnop");
	}
      else
	gen_rrr("shrd", insn, stream);
      break;
    case 055:
      fprintf (stream, "bla %s,%s,", ireg[insn.gen.src1], 
		ireg[insn.gen.src2]);
      offset = split_offset << 2;
      print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
      break;
    case 056:
      gen_rrr("shra", insn, stream);
      break;
    case 057:
      gen_irr("shra", insn, insn.geni.offset, stream);
      break;

    case 060:
      gen_rrr("and", insn, stream);
      break;
    case 061:
      gen_irr("and", insn, insn.geni.offset, stream);
      break;

    case 063:
      gen_irr("andh", insn, insn.geni.offset, stream);
      break;
    case 064:
      gen_rrr("andnot", insn, stream);
      break;
    case 065:
      gen_irr("andnot", insn, insn.geni.offset, stream);
      break;

    case 067:
      gen_irr("andnoth", insn, insn.geni.offset, stream);
      break;
      
    case 070:
      gen_rrr("or", insn, stream);
      break;
    case 071:
      gen_irr("or", insn, insn.geni.offset, stream);
      break;

    case 073:
      gen_irr("orh", insn, insn.geni.offset, stream);
      break;
    case 074:
      gen_rrr("xor", insn, stream);
      break;
    case 075:
      gen_irr("xor", insn, insn.geni.offset, stream);
      break;

    case 077:
      gen_irr("xorh", insn, insn.geni.offset, stream);
      break;
      
    default:
      fprintf (stream, "0x%08x (invalid instruction)", insn.int_val);
      break;
    }

  return(4);
}

/* A full list of floating point opcodes - if the entry is NULL, there is
 * no corresponding instruction
 */
 
static char *fp_ops[] = 
  {
	"r2p1",		"r2pt",		"r2ap1",	"r2apt",
	"i2p1",		"i2pt",		"i2ap1",	"i2apt",
	"rat1p2",	"m12apm",	"ra1p2",	"m12ttpa",
	"iat1p2",	"m12tpm",	"ia1p2",	"m12tpa",

	"r2s1",		"r2st",		"r2as1",	"r2ast",
	"i2s1",		"i2st",		"i2as1",	"i2ast",
	"rat1s2",	"m12asm",	"ra1s2",	"m12ttsa",
	"iat1s2",	"m12tsm",	"ia1s2",	"m12tsa",

	"fmul",		"fmlow",	"frcp",		"frsqr",	
	"fmul3",	NULL,		NULL,		NULL,
	NULL,		NULL,		NULL,		NULL,
	NULL,		NULL,		NULL,		NULL,
	
	"fadd",		"fsub",		"fix",		"famov",
	"fgt",		"feq",		NULL,		NULL,
	NULL,		NULL,		"ftrunc",	NULL,
	NULL,		NULL,		NULL,		NULL,
	
	"fxfr",		NULL,		NULL,		NULL,
	NULL,		NULL,		NULL,		NULL,
	NULL,		"fiadd",	NULL,		NULL,
	NULL,		"fisub",	NULL,		NULL,
	
	"faddp",	"faddz",	NULL,		NULL,
	NULL,		NULL,		NULL,		"fzchkl",	
	NULL,		NULL,		"form",		NULL,
	NULL,		NULL,		NULL,		"fzchks",	
  };

/* Alternate list of floating point opcodes for PFMAM/PFMSM instructions
 */
 
static char *alt_fp_ops[] = 
  {
	"mr2p1",	"mr2pt",	"mr2mp1",	"mr2mpt",
	"mi2p1",	"mi2pt",	"mi2mp1",	"mi2mpt",
	"mrmt1p2",	"mm12mpm",	"mrm1p2",	"mm12ttpm",
	"mimt1p2",	"mm12tpm",	"mim1p2",	"mm12tpm",

	"mr2s1",	"mr2st",	"mr2ms1",	"mr2mst",
	"mi2s1",	"mi2st",	"mi2ms1",	"mi2mst",
	"mrmt1s2",	"mm12msm",	"mrm1s2",	"mm12ttsm",
	"mimt1s2",	"mm12tsm",	"mim1s2",	"mm12tsm",
  };


/* Floating point precision suffix values - indexed by s and r bits of 
 * instructions.
 */

static char precision[2] =
  {
	's',	'd',
  };

/***********************************************************************
 * Print floating-point instruction 'insn' on the indicated stream
 * Returns 1 if successful, 0 on failure (invalid instruction)
 */

static int
fp_instr(insn, stream)
    struct fp_fmt insn;			/* instruction to decode */
    FILE *stream;			/* stream to print on */
{
  char *name;				/* the opcode name */

  name = fp_ops[insn.op2];
  if (name && insn.d)
    fprintf(stream, "d.");


  if (insn.op2 < 0x20) 
    {
      /* 
       * DPC Ops
       */
      if (insn.p == 0)			/* use PFMAM/PFMSM ops if p=0 */
	name = alt_fp_ops[insn.op2];

      fprintf (stream, "%s.%c%c f%d,f%d,f%d", name, 
		precision[insn.s], precision[insn.r],
		insn.src1, insn.src2, insn.dest);
    }
  else
    {
    switch (insn.op2)
      {
      case 0x21:	/* fmlow (no pipeline allowed) */
	fprintf (stream, "%s.%c%c f%d,f%d,f%d", name,
		  precision[insn.s], precision[insn.r],
		  insn.src1, insn.src2, insn.dest);
	break;

      case 0x22:	/* frcp */
      case 0x23:	/* fsqrt */
	fprintf (stream, "%s.%c%c f%d,f%d", name,
		  precision[insn.s], precision[insn.r],
		  insn.src2, insn.dest);
	break;

      case 0x24:	/* pfmul3 */
	fprintf (stream, "pfmul3.dd f%d,f%d,f%d", 
		  insn.src1, insn.src2, insn.dest);
	break;

      case 0x30:	/* fadd */
      case 0x49:	/* fiadd */
	if (insn.src2 == 0)
	  {
	    /* 
	     * Really fmov
	     */
	    fprintf (stream, "%sfmov.%c%c f%d,f%d", insn.p ? "p" : "", 
		      precision[insn.s], precision[insn.r],
		      insn.src1, insn.dest);
	  }
	else
	  {
	    fprintf (stream, "%s%s.%c%c f%d,f%d,f%d", insn.p ? "p" : "", name,
		      precision[insn.s], precision[insn.r],
		      insn.src1, insn.src2, insn.dest);
	  }
	break;

      case 0x32:	/* fix */
      case 0x3A:	/* ftrunc */
	fprintf (stream, "%s%s.%c%c f%d,f%d", insn.p ? "p" : "", name,
		  precision[insn.s], precision[insn.r],
		  insn.src1, insn.dest);
	break;

      case 0x34:	/* pfgt/pfle */
	if (insn.r)
	  name = "fle";
	fprintf (stream, "p%s.%c%c f%d,f%d,f%d", name,
		  precision[insn.s], precision[insn.s],
		  insn.src1, insn.src2, insn.dest);
	break;

      case 0x35:	/* pfeq */
	fprintf (stream, "pfeq.%c%c f%d,f%d,f%d", 
		  precision[insn.s], precision[insn.r],
		  insn.src1, insn.src2, insn.dest);
	break;

      case 0x40:	/* fxfr */
	fprintf (stream, "fxfr f%d,%s", insn.src1, ireg[insn.dest]);
	break;

      case 0x50:	/* faddp */
      case 0x51:	/* faddz */
      case 0x57:	/* fzchkl */
      case 0x5F:	/* fzchks */
	/*
	 * Graphics ops with no precision
	 */
	fprintf (stream, "%s%s f%d,f%d,f%d", insn.p ? "p" : "",  name,
		  insn.src1, insn.src2, insn.dest);
	break;

      case 0x5A:	/* form */
	fprintf (stream, "%sform f%d,f%d", insn.p ? "p" : "", 
		  insn.src1, insn.dest);
	break;

      default:
	/*
	 * All the rest are uniform 3-address, optionally pipelined, etc
	 */
	if (name)
	  fprintf (stream, "%s%s.%c%c f%d,f%d,f%d", insn.p ? "p" : "",  name,
		    precision[insn.s], precision[insn.r],
		    insn.src1, insn.src2, insn.dest);
	else
	  return (0);
	break;
      }
    }
  return (1);
}

/***********************************************************************
 * Decode fld/fst-style offset encodings into actual offset, precision suffix, 
 * and autoincrement flag
 */

static void
fld_offset(offset, suffix, autoincrement)
    long *offset;			/* original and returned offset */
    char *suffix;			/* returned suffix character */
    int *autoincrement;			/* autoincrement flag (1 if ai) */
{
  long off = *offset;			/* working copy of *offset */

  *autoincrement = ((off & 1) != 0);

  if (off & 2)
    {
      *suffix = 'l';
      *offset = (off & ~3);
    }
  else if (off & 4)
    {
      *suffix = 'q';
      *offset = (off & ~7);
    }
  else
    {
      *suffix = 'd';
      *offset = (off & ~7);
    }
}

/***********************************************************************
 * Print a general format instruction of the three register form:
 *	op rx,ry,rz
 */

static void
gen_rrr(name, insn, stream)
    char *name;
    union insn_fmt insn;
    FILE *stream;
{
  fprintf (stream, "%s %s,%s,%s", name, ireg[insn.gen.src1], 
	    ireg[insn.gen.src2], ireg[insn.gen.dest]);
}

/***********************************************************************
 * Print a general format instruction of the immed + two register form:
 *	op i,ry,rz
 */

static void
gen_irr(name, insn, immed, stream)
    char *name;
    union insn_fmt insn;
    long immed;
    FILE *stream;
{
  fprintf (stream, "%s 0x%x,%s,%s", name, immed,
	    ireg[insn.gen.src2], ireg[insn.gen.dest]);
}

/***********************************************************************
 * Print a ctrl format instruction with a 26-bit displacement:
 *	op addr
 */

static void
ctrl_a(name, insn, memaddr, stream)
    char *name;
    union insn_fmt insn;
    CORE_ADDR memaddr;
    FILE *stream;
{
  long offset;

  fprintf (stream, "%s ", name);
  offset = SIGN_EXT(28, insn.ctrl.offset << 2);

  print_address ((CORE_ADDR) (memaddr + 4 + offset), stream);
}


