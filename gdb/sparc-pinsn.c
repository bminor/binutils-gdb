/* Print sparc instructions for GDB, the GNU debugger.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@mcc.com)

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
#include "param.h"
#include "symtab.h"
#include "sparc-opcode.h"

/* sparc instructions are never longer than this many bytes.  */
#define MAXLEN 4

/* Print the sparc instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes, which
   is always 4.  */

static char *icc_name[] =
{ "~", "eq", "le", "lt", "leu", "ltu", "neg", "vs",
  "", "ne", "gt", "ge", "gtu", "geu", "pos", "vc"};

static char *fcc_name[] =
{ "~fb", "fbne", "fblg", "fbul", "fbl", "fbug", "fbg", "fbu",
  "fb", "fbe", "fbue", "fbge", "fbuge", "fble", "fbule", "fbo"};

static char *ccc_name[] =
{ "~cb", "cb123", "cb12", "cb13", "cb1", "cb23", "cb2", "cb3",
  "cb", "cb0", "cb03", "cb02", "cb023", "cb01", "cb013", "cb012"};

static char *arith_name[] =
{ "add", "and", "or", "xor", "sub", "andn", "orn", "xnor",
  "addx", 0, 0, 0, "subx", 0, 0, 0};

static char *xarith_name[] =
{ "taddcc", "tsubcc", "taddcctv", "tsubcctv", "mulscc", "sll", "srl", "sra"};

static char *state_reg_name[] =
{ "%y", "%psr", "%wim", "%tbr", 0, 0, 0, 0};

static char *ldst_i_name[] =
{ "ld", "ldub", "lduh", "ldd", "st", "stb", "sth", "std",
  0, "ldsb", "ldsh", 0, 0, "ldstub", 0, "swap",
  "lda", "lduba", "lduha", "ldda", "sta", "stba", "stha", "stda",
  0, "ldsba", "ldsha", 0, 0, "ldstuba", 0, "swapa"};

static char *ldst_f_name[] =
{ "ldf", "ldfsr", 0, "lddf", "stf", "stfsr", "stdfq", "stdf"};

static char *ldst_c_name[] =
{ "ldc", "ldcsr", 0, "lddc", "stc", "stcsr", "stdcq", "stdc"};

static int this_sethi_target = -1;
static int last_sethi_target = -1;
static int sethi_value = 0;

static void fprint_addr1 ();
static void fprint_ldst ();
static void fprint_f_ldst ();
static void fprint_c_ldst ();
static void fprint_fpop ();

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     FILE *stream;
{
  union insn_fmt insn;
  int disp22;

  read_memory (memaddr, &insn, MAXLEN);

  this_sethi_target = -1;
  switch (insn.op1.op1)
    {
    case 1:
      /* CALL format.  */
      fprintf (stream, "call ");
      print_address (memaddr + (insn.call.disp << 2), stream);
      break;
    case 0:
      /* Bicc, FBfcc, CBccc, SETHI format.  */
      switch (insn.op2.op2)
	{
	case 0:
	  fprintf (stream, "unimp");
	  break;
	case 2:
	  /* Bicc.  */
	  fprintf (stream, "b%s", icc_name[insn.branch.cond]);
	  if (insn.branch.a) fprintf (stream, ",a ");
	  else fprintf (stream, " ");
	  disp22 = insn.branch.disp;
	  disp22 = ((disp22 << 10) >> 10);
	  print_address (memaddr + (disp22 << 2), stream);
	  break;
	case 4:
	  /* SETHI.  */
	  fprintf (stream, "sethi %%hi(0x%x),%s",
		   insn.sethi.imm << 10, reg_names[insn.sethi.rd]);
	  this_sethi_target = insn.sethi.rd;
	  sethi_value = insn.sethi.imm << 12;
	  break;
	case 6:
	  /* FBdfcc.  */
	  fprintf (stream, "fb%s", fcc_name[insn.branch.cond]);
	  if (insn.branch.a) fprintf (stream, ",a ");
	  else fprintf (stream, " ");
	  disp22 = insn.branch.disp;
	  disp22 = ((disp22 << 10) >> 10);
	  print_address (memaddr + (disp22 << 2), stream);
	  break;
	case 7:
	  /* CBccc.  */
	  fprintf (stream, "cb%s", ccc_name[insn.branch.cond]);
	  if (insn.branch.a) fprintf (stream, ",a ");
	  else fprintf (stream, " ");
	  disp22 = insn.branch.disp;
	  disp22 = ((disp22 << 10) >> 10);
	  print_address (memaddr + (disp22 << 2), stream);
	  break;
	default:
	  fprintf (stream, "0x%x (illegal op2 format)", insn.intval);
	  break;
	}
      break;
    case 2:
      {
	/* vaguely arithmetic insns.  */
	char *rd = reg_names[insn.arith.rd];
	char *rs1 = reg_names[insn.arith.rs1];

	if (insn.op3.op3 <= 28)
	  {
	    /* Arithmetic insns, with a few unimplemented.  */
	    register int affect_cc = insn.op3.op3 & 16;
	    char *name = arith_name[insn.op3.op3 ^ affect_cc];
	    char *tmp = affect_cc ? "cc" : "";

	    if (name == 0)
	      {
		fprintf (stream, "0x%08x (unimplemented arithmetic insn)",
			 insn.intval);
	      }
	    else if (insn.arith.i)
	      {
		/* With explicit sign extension.  */
		fprintf (stream, "%s%s %s,0x%x,%s",
			 name, tmp, rs1,
			 (int) (insn.arith_imm.simm << 19) >> 19,
			 rd);
		if (last_sethi_target == insn.arith.rd)
		  {
		    fprintf (stream, "\t! ");
		    print_address (sethi_value +
				   (int) (insn.arith_imm.simm << 19) >> 19);
		  }
	      }
	    else
	      {
		fprintf (stream, "%s%s %s,%s,%s",
			 name, tmp, rs1, reg_names[insn.arith.rs2], rd);
	      }
	    break;
	  }
	if (insn.op3.op3 < 32)
	  {
	    fprintf (stream, "0x%08x (unimplemented arithmetic insn)",
		     insn.intval);
	    break;
	  }
	else
	  {
	    int op = insn.op3.op3 ^ 32;

	    if (op < 8)
	      {
		char *name = xarith_name[op];
		/* tagged add/sub insns and shift insns.  */
		if (insn.arith.i)
		  {
		    int i = (int) (insn.arith_imm.simm << 19) >> 19;
		    if (op > 4)
		      /* Its a shift insn.  */
		      i &= 31;

		    fprintf (stream, "%s %s,0x%x,%s",
			     name, rs1, i, rd);
		  }
		else
		  {
		    fprintf (stream, "%s %s,%s,%s",
			     name, rs1, reg_names[insn.arith.rs2], rd);
		  }
		break;
	      }
	    if (op < 20)
	      {
		/* read/write state registers.  */
		char *sr = state_reg_name[op & 7];
		if (sr == 0)
		  fprintf (stream, "0x%08x (unimplemented state register insn",
			   insn.intval);
		else
		  fprintf (stream, "%s %s,%s", op & 16 ? "wr" : "rd", sr, rd);
		break;
	      }
	    if (op < 22)
	      {
		/* floating point insns.  */
		int opcode = insn.arith.opf;

		fprint_fpop (stream, insn, op & 3, opcode);
		break;
	      }
	    if (op < 24)
	      {
		/* coprocessor insns.  */
		char *rs2 = reg_names[insn.arith.rs2];
		int opcode = insn.arith.opf;

		fprintf (stream, "cpop%d rs1=%s,rs2=%s,op=0x%x,rd=%s",
			 op & 1, rs1, rs2, opcode, rd);
		break;
	      }

	    switch (op)
	      {
		char *rndop_ptr;

	      case 24:
		fprint_addr1 (stream, "jumpl", insn);
		break;
	      case 25:
		fprint_addr1 (stream, "rett", insn);
		break;
	      case 26:
		{
		  char rndop_buf[32];
		  sprintf (rndop_buf, "t%s", icc_name[insn.branch.cond]);
		  fprint_addr1 (stream, rndop_buf, insn);
		}
		break;
	      case 27:
		fprint_addr1 (stream, "iflush", insn);
		break;

	      case 28:
		rndop_ptr = "save";
	      case 29:
		if (op == 29)
		  rndop_ptr = "restore";

		if (insn.arith.i)
		  {
		    fprintf (stream, "%s %s,0x%x,%s",
			     rndop_ptr, rs1,
			     ((int) (insn.arith_imm.simm << 19) >> 19), rd);
		  }
		else
		  {
		    fprintf (stream, "%s %s,%s,%s",
			     rndop_ptr, rs1, reg_names[insn.arith.rs2], rd);
		  }
		break;
	      case 30:
	      case 31:
		fprintf (stream, "0x%08x (unimplemented op3 insn)",
			 insn.intval);
		break;
	      }
	    break;
	  }
      }
    case 3:
      /* load and store insns.  */
      {
	char *rd = reg_names[insn.arith.rd];
	char *rs1 = reg_names[insn.arith.rs1];
	int op = insn.arith.op3;

	if ((op & 32) == 0)
	  {
	    /* Integer ops.  */
	    fprint_ldst (stream, insn, op);
	    break;
	  }
	if ((op & 16) == 0)
	  {
	    /* Float ops.  */
	    op ^= 32;
	    if (op <= 7)
	      {
		fprint_f_ldst (stream, insn, op);
	      }
	    else
	      fprintf (stream, "0x%08x (unimplemented float load/store insn)",
		       insn.intval);
	  }
	else
	  {
	    /* Coprocessor ops.  */
	    op ^= (32+16);
	    if (op <= 7)
	      {
		fprint_c_ldst (stream, insn, op);
	      }
	    else
	      fprintf (stream, "0x%08x (unimplemented coprocessor load/store insn)",
		       insn.intval);
	  }
	break;
      }
    }
  return 4;
}

/* It would be nice if this routine could print out a symbolic address
   when appropriate.  */
static void
fprint_addr1 (stream, name, insn)
     FILE *stream;
     char *name;
     union insn_fmt insn;
{
  char *rs1 = reg_names[insn.arith.rs1];
  char *rd = reg_names[insn.arith.rd];

  if (insn.arith.i)
    {
      fprintf (stream, "%s %s,0x%x,%s",
	       name, rs1,
	       (int) (insn.arith_imm.simm << 19) >> 19,
	       rd);
    }
  else
    {
      fprintf (stream, "%s %s,%s,%s",
	       name, rs1, reg_names[insn.arith.rs2], rd);
    }
}

static void
fprint_mem (stream, insn)
     FILE *stream;
     union insn_fmt insn;
{
  char *reg_name = reg_names[insn.arith.rs1];
  if (insn.arith.i)
    {
      if (insn.arith_imm.simm == 0)
	fprintf (stream, "[%s]", reg_name);
      else if (insn.arith_imm.simm & 0x1000)
	fprintf (stream, "[%s-0x%x]", reg_name,
		 - (insn.arith_imm.simm | 0xffffe000));
      else
	fprintf (stream, "[%s+0x%x]", reg_name, insn.arith_imm.simm);
    }
  else
    {
      if (insn.arith.rs2 == 0)
	fprintf (stream, "[%s]", reg_name);
      else
	fprintf (stream, "[%s,%s]", reg_names[insn.arith.rs2], reg_name);
    }
}

static void
fprint_ldst (stream, insn, op)
     FILE *stream;
     union insn_fmt insn;
     int op;
{
  char *name = ldst_i_name[op];
  char *rd = reg_names[insn.arith.rd];

  if (name)
    {
      if (name[0] == 's')
	{
	  fprintf (stream, "%s %s,", name, rd);
	  fprint_mem (stream, insn);
	}
      else
	{
	  fprintf (stream, "%s ", name);
	  fprint_mem (stream, insn);
	  fprintf (stream, ",%s", rd);
	}
    }
  else
    fprintf (stream, "0x%08x (unimplemented load/store insn)", insn.intval);
}

static void
fprint_f_ldst (stream, insn, op)
     FILE *stream;
     union insn_fmt insn;
     int op;
{
  char *name = ldst_f_name[op];
  if (name)
    {
      char *rd = reg_names[insn.arith.rd + 32];

      if (name[0] == 's')
	{
	  fprintf (stream, "%s %s,", name, rd);
	  fprint_mem (stream, insn);
	}
      else
	{
	  fprintf (stream, "%s ", name);
	  fprint_mem (stream, insn);
	  fprintf (stream, ",%s", rd);
	}
    }
  else
    fprintf (stream, "0x%08x (unimplemented float load/store insn)", insn.intval);
}

static void
fprint_c_ldst (stream, insn, op)
     FILE *stream;
     union insn_fmt insn;
     int op;
{
  char *name = ldst_c_name[op];
  if (name)
    {
      if (name[0] == 's')
	{
	  fprintf (stream, "%s %%cpreg(%d),", name, insn.arith.rs1);
	  fprint_mem (stream, insn);
	}
      else
	{
	  fprintf (stream, "%s ");
	  fprint_mem (stream, insn);
	  fprintf (stream, ",%%cpreg(%d)", insn.arith.rd);
	}
    }
  else
    fprintf (stream, "0x%08x (unimplemented coprocessor load/store insn)",
	     insn.intval);
}

static void
fprint_fpop (stream, insn, op, opcode)
     FILE *stream;
     union insn_fmt insn;
     int op, opcode;
{
  char *name;
  char *rs1, *rs2, *rd;

  switch (op)
    {
    case 0:
      rs2 = reg_names[insn.arith.rs2 + 32];
      rd = reg_names[insn.arith.rd + 32];
      if ((opcode ^ 0x2f) <= 0x2f)
	{
	  switch (opcode)
	    {
	    case 0x1:
	      name = "fmovs";
	      break;
	    case 0x5:
	      name = "fnegs";
	      break;
	    case 0x9:
	      name = "fabss";
	      break;
	    case 0x29:
	      name = "fsqrts";
	      break;
	    case 0x2a:
	      name = "fsqrtd";
	      break;
	    case 0x2b:
	      name = "fsqrtx";
	      break;
	    }
	  fprintf (stream, "%s %s,%s", name, rs2, rd);
	  return;
	}
      if ((opcode ^ 0x5f) <= 0x5f)
	{
	  rs1 = reg_names[insn.arith.rs1 + 32];
	  switch (opcode)
	    {
	    case 0x41:
	      name = "fadds";
	      break;
	    case 0x42:
	      name = "faddd";
	      break;
	    case 0x43:
	      name = "faddx";
	      break;
	    case 0x45:
	      name = "fsubs";
	      break;
	    case 0x46:
	      name = "fsubd";
	      break;
	    case 0x47:
	      name = "fsubx";
	      break;
	    case 0x49:
	      name = "fmuls";
	      break;
	    case 0x4a:
	      name = "fmuld";
	      break;
	    case 0x4b:
	      name = "fmulx";
	      break;
	    case 0x4d:
	      name = "fdivs";
	      break;
	    case 0x4e:
	      name = "fdivd";
	      break;
	    case 0x4f:
	      name = "fdivx";
	      break;
	    default:
	      goto unimplemented;
	    }
	  if ((opcode & 0x10) == 0)
	    fprintf (stream, "%s %s,%s,%s", name, rs1, rs2, rd);
	  else
	    fprintf (stream, "%s %s,%s", name, rs1, rs2);
	  return;
	}
      if ((opcode ^ 0xdf) <= 0xdf)
	{
	  switch (opcode)
	    {
	    case 0xc4:
	      name = "fitos";
	      break;
	    case 0xc8:
	      name = "fitod";
	      break;
	    case 0xcc:
	      name = "fitox";
	      break;
	    case 0xd1:
	      name = "fstoi";
	      break;
	    case 0xd2:
	      name = "fdtoi";
	      break;
	    case 0xd3:
	      name = "fxtoi";
	      break;
	    case 0xc9:
	      name = "fstod";
	      break;
	    case 0xcd:
	      name = "fstox";
	      break;
	    case 0xc6:
	      name = "fdtos";
	      break;
	    case 0xce:
	      name = "fdtox";
	      break;
	    case 0xc7:
	      name = "fxtos";
	      break;
	    case 0xcb:
	      name = "fxtod";
	      break;
	    default:
	      goto unimplemented;
	    }
	  fprintf (stream, "%s %s,%s", name, rs2, rd);
	  return;
	}
      goto unimplemented;

    case 1:
      rs1 = reg_names[insn.arith.rs1 + 32];
      rs2 = reg_names[insn.arith.rs2 + 32];
      if ((opcode ^ 0x57) <= 0x57)
	{
	  switch (opcode)
	    {
	    case 0x51:
	      name = "fcmps";
	      break;
	    case 0x52:
	      name = "fcmpd";
	      break;
	    case 0x53:
	      name = "fcmpx";
	      break;
	    case 0x55:
	      name = "fcmpes";
	      break;
	    case 0x56:
	      name = "fcmped";
	      break;
	    case 0x57:
	      name = "fcmpex";
	      break;
	    default:
	      goto unimplemented;
	    }
	  fprintf (stream, "%s %s,%s", name, rs1, rs2);
	  return;
	}
      else goto unimplemented;

    case 2:
    case 3:
      goto unimplemented;
    }
 unimplemented:
  fprintf (stream, "0x%08x (unimplemented fpop insn)", insn.intval);
}

