/* Print SPARC instructions.
   Copyright 1989, 1991, 1992, 1993 Free Software Foundation, Inc.

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

#include "opcode/sparc.h"
#include "dis-asm.h"
#include <string.h>

static  char *reg_names[] =
 { "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",	
  "o0", "o1", "o2", "o3", "o4", "o5", "sp", "o7",	
  "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",	
  "i0", "i1", "i2", "i3", "i4", "i5", "fp", "i7",	
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	
  "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",	
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",
  "y", "psr", "wim", "tbr", "pc", "npc", "fpsr", "cpsr" };

#define	freg_names	(&reg_names[4 * 8])

/* FIXME--need to deal with byte order (probably using masking and
   shifting rather than bitfields is easiest).  */

union sparc_insn
  {
    unsigned long int code;
    struct
      {
	unsigned int anop:2;
#define	op	ldst.anop
	unsigned int anrd:5;
#define	rd	ldst.anrd
	unsigned int op3:6;
	unsigned int anrs1:5;
#define	rs1	ldst.anrs1
	unsigned int i:1;
	unsigned int anasi:8;
#define	asi	ldst.anasi
	unsigned int anrs2:5;
#define	rs2	ldst.anrs2
#define	shcnt	rs2
      } ldst;
    struct
      {
	unsigned int anop:2, anrd:5, op3:6, anrs1:5, i:1;
	unsigned int IMM13:13;
#define	imm13	IMM13.IMM13
      } IMM13;
    struct
      {
	unsigned int anop:2;
	unsigned int a:1;
	unsigned int cond:4;
	unsigned int op2:3;
	unsigned int DISP22:22;
#define	disp22	branch.DISP22
      } branch;
#ifndef NO_V9
    struct
      {
	unsigned int _OP:2, _RD:5, op3:6, _RS1:5;
	unsigned int DISP14:14;
#define	disp14	DISP14.DISP14
      } DISP14;
    struct
      {
	unsigned int _OP:2;
	unsigned int a:1;
	unsigned int cond:4;
	unsigned int op2:3;
	unsigned int p:1;
	unsigned int DISP21:21;
#define	disp21	branch2.DISP21
      } branch2;
#endif /* NO_V9 */

#define	imm22	disp22
    struct
      {
	unsigned int anop:2;
	unsigned int adisp30:30;
#define	disp30	call.adisp30
      } call;
  };

/* Nonzero if INSN is the opcode for a delayed branch.  */
static int
is_delayed_branch (insn)
     union sparc_insn insn;
{
  unsigned int i;

  for (i = 0; i < NUMOPCODES; ++i)
    {
      const struct sparc_opcode *opcode = &sparc_opcodes[i];
      if ((opcode->match & insn.code) == opcode->match
	  && (opcode->lose & insn.code) == 0)
	return (opcode->flags & F_DELAYED);
    }
  return 0;
}

static int opcodes_sorted = 0;
extern void qsort ();

/* Print one instruction from MEMADDR on STREAM.

   We suffix the instruction with a comment that gives the absolute
   address involved, as well as its symbolic form, if the instruction
   is preceded by a findable `sethi' and it either adds an immediate
   displacement to that register, or it is an `add' or `or' instruction
   on that register.  */
int
print_insn_sparc (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  FILE *stream = info->stream;
  union sparc_insn insn;

  register unsigned int i;

  if (!opcodes_sorted)
    {
      static int compare_opcodes ();
      qsort ((char *) sparc_opcodes, NUMOPCODES,
	     sizeof (sparc_opcodes[0]), compare_opcodes);
      opcodes_sorted = 1;
    }

  {
    int status =
      (*info->read_memory_func) (memaddr, (char *) &insn, sizeof (insn), info);
    if (status != 0)
      {
	(*info->memory_error_func) (status, memaddr, info);
	return -1;
      }
  }

  for (i = 0; i < NUMOPCODES; ++i)
    {
      const struct sparc_opcode *opcode = &sparc_opcodes[i];
      if ((opcode->match & insn.code) == opcode->match
	  && (opcode->lose & insn.code) == 0)
	{
	  /* Nonzero means that we have found an instruction which has
	     the effect of adding or or'ing the imm13 field to rs1.  */
	  int imm_added_to_rs1 = 0;

	  /* Nonzero means that we have found a plus sign in the args
	     field of the opcode table.  */
	  int found_plus = 0;
	  
	  /* Do we have an `add' or `or' instruction where rs1 is the same
	     as rsd, and which has the i bit set?  */
	  if ((opcode->match == 0x80102000 || opcode->match == 0x80002000)
	  /*			  (or)				 (add)  */
	      && insn.rs1 == insn.rd)
	    imm_added_to_rs1 = 1;

	  if (insn.rs1 != insn.rd
	      && strchr (opcode->args, 'r') != 0)
	      /* Can't do simple format if source and dest are different.  */
	      continue;

	  (*info->fprintf_func) (stream, opcode->name);

	  {
	    register const char *s;

	    if (opcode->args[0] != ',')
	      (*info->fprintf_func) (stream, " ");
	    for (s = opcode->args; *s != '\0'; ++s)
	      {
		while (*s == ',')
		  {
		    (*info->fprintf_func) (stream, ",");
		    ++s;
		    switch (*s) {
		    case 'a':
		      (*info->fprintf_func) (stream, "a");
		      ++s;
		      continue;
#ifndef NO_V9
		    case 'N':
		      (*info->fprintf_func) (stream, "pn");
		      ++s;
		      continue;

		    case 'T':
		      (*info->fprintf_func) (stream, "pt");
		      ++s;
		      continue;
#endif				/* NO_V9 */

		    default:
		      break;
		    }		/* switch on arg */
		  }		/* while there are comma started args */

		(*info->fprintf_func) (stream, " ");
			
		switch (*s)
		  {
		  case '+':
		    found_plus = 1;

		    /* note fall-through */
		  default:
		    (*info->fprintf_func) (stream, "%c", *s);
		    break;

		  case '#':
		    (*info->fprintf_func) (stream, "0");
		    break;

#define	reg(n)	(*info->fprintf_func) (stream, "%%%s", reg_names[n])
		  case '1':
		  case 'r':
		    reg (insn.rs1);
		    break;

		  case '2':
		    reg (insn.rs2);
		    break;

		  case 'd':
		    reg (insn.rd);
		    break;
#undef	reg

#define	freg(n)	(*info->fprintf_func) (stream, "%%%s", freg_names[n])
		  case 'e':
		  case 'v':	/* double/even */
		  case 'V':	/* quad/multiple of 4 */
		    freg (insn.rs1);
		    break;

		  case 'f':
		  case 'B':	/* double/even */
		  case 'R':	/* quad/multiple of 4 */
		    freg (insn.rs2);
		    break;

		  case 'g':
		  case 'H':	/* double/even */
		  case 'J':	/* quad/multiple of 4 */
		    freg (insn.rd);
		    break;
#undef	freg

#define	creg(n)	(*info->fprintf_func) (stream, "%%c%u", (unsigned int) (n))
		  case 'b':
		    creg (insn.rs1);
		    break;

		  case 'c':
		    creg (insn.rs2);
		    break;

		  case 'D':
		    creg (insn.rd);
		    break;
#undef	creg

		  case 'h':
		    (*info->fprintf_func) (stream, "%%hi(%#x)",
					   (int) insn.imm22 << 10);
		    break;

		  case 'i':
		    {
		      /* We cannot trust the compiler to sign-extend
			 when extracting the bitfield, hence the shifts.  */
		      int imm = ((int) insn.imm13 << 19) >> 19;

		      /* Check to see whether we have a 1+i, and take
			 note of that fact.

			 Note: because of the way we sort the table,
			 we will be matching 1+i rather than i+1,
			 so it is OK to assume that i is after +,
			 not before it.  */
		      if (found_plus)
			imm_added_to_rs1 = 1;
		      
		      if (imm <= 9)
			(*info->fprintf_func) (stream, "%d", imm);
		      else
			(*info->fprintf_func) (stream, "%#x", imm);
		    }
		    break;

#ifndef NO_V9
		  case 'I':	/* 11 bit immediate.  */
		  case 'j':	/* 10 bit immediate.  */
		    {
		      /* We cannot trust the compiler to sign-extend
			 when extracting the bitfield, hence the shifts.  */
		      int imm;

		      if (*s == 'I')
			imm = ((int) insn.imm13 << 21) >> 21;
		      else
			imm = ((int) insn.imm13 << 22) >> 22;

		      /* Check to see whether we have a 1+i, and take
			 note of that fact.
			 
			 Note: because of the way we sort the table,
			 we will be matching 1+i rather than i+1,
			 so it is OK to assume that i is after +,
			 not before it.  */
		      if (found_plus)
			imm_added_to_rs1 = 1;
		      
		      if (imm <= 9)
			(info->fprintf_func) (stream, "%d", imm);
		      else
			(info->fprintf_func) (stream, "%#x", (unsigned) imm);
		    }
		    break;




		  case 'k':
		    (*info->print_address_func)
		      ((bfd_vma) (memaddr
				  + (((int) insn.disp14 << 18) >> 18) * 4),
		       info);
		    break;

		  case 'G':
		    (*info->print_address_func)
		      ((bfd_vma) (memaddr
				  /* We use only 19 of the 21 bits.  */
				  + (((int) insn.disp21 << 13) >> 13) * 4),
		       info);
		    break;

		  case '6':
		  case '7':
		  case '8':
		  case '9':
		    (*info->fprintf_func) (stream, "fcc%c", *s - '6' + '0');
		    break;

		  case 'z':
		    (*info->fprintf_func) (stream, "icc");
		    break;

		  case 'Z':
		    (*info->fprintf_func) (stream, "xcc");
		    break;

		  case 'E':
		    (*info->fprintf_func) (stream, "%%ccr");
		    break;

		  case 's':
		    (*info->fprintf_func) (stream, "%%fprs");
		    break;
#endif				/* NO_V9 */

		  case 'M':
		    (*info->fprintf_func) (stream, "%%asr%d", insn.rs1);
		    break;
		    
		  case 'm':
		    (*info->fprintf_func) (stream, "%%asr%d", insn.rd);
		    break;
		    
		  case 'L':
		    (*info->print_address_func)
		     ((bfd_vma) memaddr + insn.disp30 * 4,
		      info);
		    break;

		  case 'l':
		    if ((insn.code >> 22) == 0)
		      /* Special case for `unimp'.  Don't try to turn
			 it's operand into a function offset.  */
		      (*info->fprintf_func)
			(stream, "%#x",
			 (int) (((int) insn.disp22 << 10) >> 10));
		    else
		      /* We cannot trust the compiler to sign-extend
			 when extracting the bitfield, hence the shifts.  */
		      (*info->print_address_func)
			((bfd_vma) (memaddr
				    + (((int) insn.disp22 << 10) >> 10) * 4),
			 info);
		    break;

		  case 'A':
		    (*info->fprintf_func) (stream, "(%d)", (int) insn.asi);
		    break;

		  case 'C':
		    (*info->fprintf_func) (stream, "%%csr");
		    break;

		  case 'F':
		    (*info->fprintf_func) (stream, "%%fsr");
		    break;

		  case 'p':
		    (*info->fprintf_func) (stream, "%%psr");
		    break;

		  case 'q':
		    (*info->fprintf_func) (stream, "%%fq");
		    break;

		  case 'Q':
		    (*info->fprintf_func) (stream, "%%cq");
		    break;

		  case 't':
		    (*info->fprintf_func) (stream, "%%tbr");
		    break;

		  case 'w':
		    (*info->fprintf_func) (stream, "%%wim");
		    break;

		  case 'y':
		    (*info->fprintf_func) (stream, "%%y");
		    break;
		  }
	      }
	  }

	  /* If we are adding or or'ing something to rs1, then
	     check to see whether the previous instruction was
	     a sethi to the same register as in the sethi.
	     If so, attempt to print the result of the add or
	     or (in this context add and or do the same thing)
	     and its symbolic value.  */
	  if (imm_added_to_rs1)
	    {
	      union sparc_insn prev_insn;
	      int errcode;

	      errcode =
		(*info->read_memory_func)
		  (memaddr - 4,
		   (char *)&prev_insn, sizeof (prev_insn), info);

	      if (errcode == 0)
		{
		  /* If it is a delayed branch, we need to look at the
		     instruction before the delayed branch.  This handles
		     sequences such as

		     sethi %o1, %hi(_foo), %o1
		     call _printf
		     or %o1, %lo(_foo), %o1
		     */

		  if (is_delayed_branch (prev_insn))
		    errcode = (*info->read_memory_func)
		      (memaddr - 8, (char *)&prev_insn, sizeof (prev_insn),
		       info);
		}

	      /* If there was a problem reading memory, then assume
		 the previous instruction was not sethi.  */
	      if (errcode == 0)
		{
		  /* Is it sethi to the same register?  */
		  if ((prev_insn.code & 0xc1c00000) == 0x01000000
		      && prev_insn.rd == insn.rs1)
		    {
		      (*info->fprintf_func) (stream, "\t! ");
		      /* We cannot trust the compiler to sign-extend
			 when extracting the bitfield, hence the shifts.  */
		      (*info->print_address_func)
			(((int) prev_insn.imm22 << 10)
			 | (insn.imm13 << 19) >> 19,
			 info);
		    }
		}
	    }

	  return sizeof (insn);
	}
    }

  (*info->fprintf_func) (stream, "%#8x", insn.code);
  return sizeof (insn);
}

/* Compare opcodes A and B.  */

static int
compare_opcodes (a, b)
     char *a, *b;
{
  struct sparc_opcode *op0 = (struct sparc_opcode *) a;
  struct sparc_opcode *op1 = (struct sparc_opcode *) b;
  unsigned long int match0 = op0->match, match1 = op1->match;
  unsigned long int lose0 = op0->lose, lose1 = op1->lose;
  register unsigned int i;

  /* If a bit is set in both match and lose, there is something
     wrong with the opcode table.  */
  if (match0 & lose0)
    {
      fprintf (stderr, "Internal error:  bad sparc-opcode.h: \"%s\", %#.8lx, %#.8lx\n",
	       op0->name, match0, lose0);
      op0->lose &= ~op0->match;
      lose0 = op0->lose;
    }

  if (match1 & lose1)
    {
      fprintf (stderr, "Internal error: bad sparc-opcode.h: \"%s\", %#.8lx, %#.8lx\n",
	       op1->name, match1, lose1);
      op1->lose &= ~op1->match;
      lose1 = op1->lose;
    }

  /* Because the bits that are variable in one opcode are constant in
     another, it is important to order the opcodes in the right order.  */
  for (i = 0; i < 32; ++i)
    {
      unsigned long int x = 1 << i;
      int x0 = (match0 & x) != 0;
      int x1 = (match1 & x) != 0;

      if (x0 != x1)
	return x1 - x0;
    }

  for (i = 0; i < 32; ++i)
    {
      unsigned long int x = 1 << i;
      int x0 = (lose0 & x) != 0;
      int x1 = (lose1 & x) != 0;

      if (x0 != x1)
	return x1 - x0;
    }

  /* They are functionally equal.  So as long as the opcode table is
     valid, we can put whichever one first we want, on aesthetic grounds.  */

  /* Our first aesthetic ground is that aliases defer to real insns.  */
  {
    int alias_diff = (op0->flags & F_ALIAS) - (op1->flags & F_ALIAS);
    if (alias_diff != 0)
      /* Put the one that isn't an alias first.  */
      return alias_diff;
  }

  /* Except for aliases, two "identical" instructions had
     better have the same opcode.  This is a sanity check on the table.  */
  i = strcmp (op0->name, op1->name);
  if (i)
      if (op0->flags & F_ALIAS) /* If they're both aliases, be arbitrary. */
	  return i;
      else
	  fprintf (stderr,
		   "Internal error: bad sparc-opcode.h: \"%s\" == \"%s\"\n",
		   op0->name, op1->name);

  /* Fewer arguments are preferred.  */
  {
    int length_diff = strlen (op0->args) - strlen (op1->args);
    if (length_diff != 0)
      /* Put the one with fewer arguments first.  */
      return length_diff;
  }

  /* Put 1+i before i+1.  */
  {
    char *p0 = (char *) strchr(op0->args, '+');
    char *p1 = (char *) strchr(op1->args, '+');

    if (p0 && p1)
      {
	/* There is a plus in both operands.  Note that a plus
	   sign cannot be the first character in args,
	   so the following [-1]'s are valid.  */
	if (p0[-1] == 'i' && p1[1] == 'i')
	  /* op0 is i+1 and op1 is 1+i, so op1 goes first.  */
	  return 1;
	if (p0[1] == 'i' && p1[-1] == 'i')
	  /* op0 is 1+i and op1 is i+1, so op0 goes first.  */
	  return -1;
      }
  }

  /* They are, as far as we can tell, identical.
     Since qsort may have rearranged the table partially, there is
     no way to tell which one was first in the opcode table as
     written, so just say there are equal.  */
  return 0;
}
