/* Disassembler for the PA-RISC. Somewhat derived from sparc-pinsn.c.
   Copyright 1989, 1990, 1992 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
#include "symtab.h"
#include "opcode/hppa.h"

static char *control_reg[] =
{  "rctr", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
   "pidr1", "pidr2", "ccr", "sar", "pidr3", "pidr4",
   "iva", "eiem", "itmr", "pcsq", "pcoq", "iir", "isr",
   "ior", "ipsw", "eirr", "tr0", "tr1", "tr2", "tr3",
   "tr4", "tr5", "tr6", "tr7"
   };

static char *compare_cond_names[] =
{  "", ",=", ",<", ",<=", ",<<", ",<<=", ",sv",
   ",od", ",tr", ",<>", ",>=", ",>", ",>>=",
   ",>>", ",nsv", ",ev"
   };

static char *add_cond_names[] =
{  "", ",=", ",<", ",<=", ",nuv", ",znv", ",sv",
   ",od", ",tr", ",<>", ",>=", ",>", ",uv",
   ",vnz", ",nsv", ",ev"
   };

static char *logical_cond_names[] =
{  "", ",=", ",<", ",<=", 0, 0, 0, ",od",
   ",tr", ",<>", ",>=", ",>", 0, 0, 0, ",ev"
   };

static char *unit_cond_names[] =
{  "", 0, ",sbz", ",shz", ",sdc", 0, ",sbc", ",shc",
   ",tr", 0, ",nbz", ",nhz", ",ndc", 0, ",nbc", ",nhc"
   };

static char *shift_cond_names[] =
{"", ",=", ",<", ",od", ",tr", ",<>", ",>=", ",ev"};

static char *index_compl_names[] = {"", ",m", ",s", ",sm"};
static char *short_ldst_compl_names[] = {"", ",ma", "", ",mb"};
static char *short_bytes_compl_names[] = {"", ",b,m", ",e", ",e,m"};
static char *float_format_names[] = {",sgl", ",dbl", ",quad"};
static char *float_comp_names[] =
{",false?", ",false", ",?", ",!<=>", ",=", ",=t", ",?=", ",!<>",
 ",!?>=", ",<", ",?<", ",!>=", ",!?>", ",<=", ",?<=", ",!>",
 ",!?<=", ",>", ",?>", ",!<=", ",!?<", ",>=", ",?>=", ",!<",
 ",!?=", ",<>", ",!=", ",!=t", ",!?", ",<=>", ",true?", ",true"
 };

/* For a bunch of different instructions form an index into a 
   completer name table. */
#define GET_COMPL(insn) (GET_FIELD (insn, 26, 26) | \
			 GET_FIELD (insn, 18, 18) << 1)

#define GET_COND(insn) (GET_FIELD ((insn), 16, 18) + \
			(GET_FIELD ((insn), 19, 19) ? 8 : 0))

static void fput_reg PARAMS ((unsigned reg, FILE *stream));
static void fput_const PARAMS ((unsigned num, FILE *stream));
static void fput_reg_r PARAMS ((unsigned reg, FILE *stream));
static void fput_creg PARAMS ((unsigned reg, FILE *stream));

/* Print one instruction from MEMADDR on STREAM.  */
int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     FILE *stream;
{
  long insn;
  unsigned int i, op;

  insn = read_memory_integer (memaddr, sizeof (insn));

  for (i = 0; i < NUMOPCODES; ++i)
    {
      const struct pa_opcode *opcode = &pa_opcodes[i];
      if ((insn & opcode->mask) == opcode->match)
	{
	  register const char *s;
	  
	  fputs_filtered (opcode->name, stream);
	
	  if (!index ("cCY<?!@-+&U>~nZFM", opcode->args[0]))
	    fputs_filtered (" ", stream);
	  for (s = opcode->args; *s != '\0'; ++s)
	    {
	      switch (*s)
		{
		case 'x':
		  fput_reg (GET_FIELD (insn, 11, 15), stream);
		  break;
		case 'X':
                  if (GET_FIELD (insn, 25, 25))
		      fput_reg_r (GET_FIELD (insn, 11, 15), stream);
		  else
		      fput_reg (GET_FIELD (insn, 11, 15), stream);
		  break;
		case 'b':
		  fput_reg (GET_FIELD (insn, 6, 10), stream);
		  break;
		case '^':
		  fput_creg (GET_FIELD (insn, 6, 10), stream);
		  break;
		case 'E':
                  if (GET_FIELD (insn, 25, 25))
		      fput_reg_r (GET_FIELD (insn, 6, 10), stream);
		  else
		      fput_reg (GET_FIELD (insn, 6, 10), stream);
		  break;
		case 't':
		  fput_reg (GET_FIELD (insn, 27, 31), stream);
		  break;
		case 'v':
                  if (GET_FIELD (insn, 25, 25))
		      fput_reg_r (GET_FIELD (insn, 27, 31), stream);
		  else
		      fput_reg (GET_FIELD (insn, 27, 31), stream);
		  break;
		case '4':
		  fput_creg (GET_FIELD (insn, 6, 10), stream);
		  break;
		case '6':
		  fput_reg (GET_FIELD (insn, 11, 15), stream);
		  break;
		case '7':
		  fput_reg (GET_FIELD (insn, 27, 31), stream);
		  break;
		case '8':
		  fput_reg (GET_FIELD (insn, 16, 20), stream);
		  break;
		case '9':
		  fput_reg (GET_FIELD (insn, 21, 25), stream);
		  break;
		case '5':
		  fput_const (extract_5_load (insn), stream);
		  break;
		  /* case 's': */
		case 'S':
		  fprintf_filtered (stream, "sr%d", extract_3 (insn));
		  break;
		case 'c':
		  fprintf_filtered (stream, "%s ",
				    index_compl_names[GET_COMPL (insn)]);
		  break;
		case 'C':
		  fprintf_filtered (stream, "%s ",
				    short_ldst_compl_names[GET_COMPL (insn)]);
		  break;
		case 'Y':
		  fprintf_filtered (stream, "%s ",
				    short_bytes_compl_names[GET_COMPL (insn)]);
		  break;
		/* these four conditions are for the set of instructions
		   which distinguish true/false conditions by opcode rather
		   than by the 'f' bit (sigh): comb, comib, addb, addib */
		case '<':
		  fputs_filtered (compare_cond_names[GET_FIELD (insn, 16, 18)],
				  stream);
		  break;
		case '?':
		  fputs_filtered (compare_cond_names[GET_FIELD (insn, 16, 18) + 8],
				  stream);
		  break;
		case '!':
		  fputs_filtered (add_cond_names[GET_FIELD (insn, 16, 18)],
				  stream);
		  break;
		case '@':
		  fputs_filtered (add_cond_names[GET_FIELD (insn, 16, 18) + 8],
				  stream);
		  break;
		case '-':
		  fprintf_filtered (stream, "%s ",
				    compare_cond_names[GET_COND (insn)]);
		  break;
		case '+':
		  fprintf_filtered (stream, "%s ",
				    add_cond_names[GET_FIELD (insn, 16, 18)]);
		  break;

		case '&':
		  fprintf_filtered (stream, "%s ",
				    logical_cond_names[GET_COND (insn)]);
		  break;
		case 'U':
		  fprintf_filtered (stream, "%s ",
				    unit_cond_names[GET_COND (insn)]);
		  break;
		case '>':
		case '~':
		  fprintf_filtered (stream, "%s ",
				    shift_cond_names[GET_FIELD (insn, 16, 18)]);
		  break;
		case 'V':
		  fput_const (extract_5_store (insn), stream);
		  break;
		case 'i':
		  fput_const (extract_11 (insn), stream);
		  break;
		case 'j':
		  fput_const (extract_14 (insn), stream);
		  break;
		case 'k':
		  fput_const (extract_21 (insn), stream);
		  break;
		case 'n':
		  if (insn & 0x2)
		    fprintf_filtered (stream, ",n ");
		  else
		    fprintf_filtered (stream, " ");
		  break;
		case 'w':
		  print_address (memaddr + 8 + extract_12 (insn), stream);
		  break;
		case 'W':
		  op = GET_FIELD (insn, 0, 5);

		  if (op == 0x38 /* be */ || op == 0x39 /* ble */)
		    fput_const (extract_17 (insn), stream);
		  else
		    print_address (memaddr + 8 + extract_17 (insn), stream);

		  break;
		case 'B':
		  {
		    int space;
		    if (space = GET_FIELD (insn, 16, 17))
		      fprintf_filtered (stream, "sr%d,", space);
		    fput_reg (GET_FIELD (insn, 6, 10), stream);
		    break;
		  }
		case 'p':
		  fprintf_filtered (stream, "%d",
				    31 - GET_FIELD (insn, 22, 26));
		  break;
		case 'P':
		  fprintf_filtered (stream, "%d",
				    GET_FIELD (insn, 22, 26));
		  break;
		case 'Q':
		  fprintf_filtered (stream, "%d",
				    GET_FIELD (insn, 11, 15));
		  break;
		case 'T':
		  fprintf_filtered (stream, "%d",
				    32 - GET_FIELD (insn, 27, 31));
		  break;
		case 'A':
		  fput_const (GET_FIELD (insn, 6, 18), stream);
		  break;
		case 'Z':
		  if (GET_FIELD (insn, 26, 26))
		    fprintf_filtered (stream, ",m ");
		  else
		    fprintf_filtered (stream, " ");
		  break;
		case 'D':
		  fput_const (GET_FIELD (insn, 6, 31), stream);
		  break;
		case 'f':
		  fprintf_filtered (stream, ",%d", GET_FIELD (insn, 23, 25));
		  break;
		case 'O':
		  fput_const ((GET_FIELD (insn, 6,20) << 5 |
			       GET_FIELD (insn, 27, 31)), stream);
		  break;
		case 'o':
		  fput_const (GET_FIELD (insn, 6, 20), stream);
		  break;
		case '2':
		  fput_const ((GET_FIELD (insn, 6, 22) << 5 |
			       GET_FIELD (insn, 27, 31)), stream);
		  break;
		case '1':
		  fput_const ((GET_FIELD (insn, 11, 20) << 5 |
			       GET_FIELD (insn, 27, 31)), stream);
		  break;
		case '0':
		  fput_const ((GET_FIELD (insn, 16, 20) << 5 |
			       GET_FIELD (insn, 27, 31)), stream);
		  break;
		case 'u':
		  fprintf_filtered (stream, "%d", GET_FIELD (insn, 23, 25));
		  break;
		case 'F':
		  /* if no destination completer, need a space here */
		  if (GET_FIELD (insn, 21, 22) == 1)
		    fputs_filtered (float_format_names[GET_FIELD (insn, 19, 20)],
				    stream);
		  else
		    fprintf_filtered (stream, "%s ",
				      float_format_names[GET_FIELD
							 (insn, 19, 20)]);
		  break;
		case 'G':
		  fprintf_filtered (stream, "%s ",
				    float_format_names[GET_FIELD (insn,
								  17, 18)]);
		  break;
		case 'H':
		    fputs_filtered (float_format_names[GET_FIELD 
                                                      (insn, 26, 26)], stream);
		  break;
		case 'M':
		  fputs_filtered (float_comp_names[GET_FIELD (insn, 27, 31)],
				  stream);
		  break;
		case '}':
		  fprintf_filtered (stream, "fp%d", GET_FIELD (insn, 6, 10));
		  break;
		case '|':
		  fprintf_filtered (stream, "fp%d", GET_FIELD (insn, 11, 15));
		  break;
		case '{':
		  if (GET_FIELD (insn, 23, 25) == 0)
		    fprintf_filtered (stream, "fp%d",
				      GET_FIELD (insn, 27, 31));
		  else
		    fprintf_filtered (stream, "cp%d",
				      GET_FIELD (insn, 27, 31));
		  break;
		default:
		  fprintf_filtered (stream, "%c", *s);
		  break;
		}
	    }

/* If this is an external branch, examine the previous instruction and see if
   it was an ldil that loaded something into the same base reg.  If so, then
   calculate the branch target from the constants in both instructions, and
   print it out. */

	  op = GET_FIELD (insn, 0, 5);
	  if (op == 0x38 /* be */ || op == 0x39 /* ble */)
	    {
	      CORE_ADDR target_address;
	      long prev_insn;
	      int basereg, basereg_prev;

	      target_address = extract_17 (insn);
	      basereg = GET_FIELD (insn, 6, 10);
	      if (basereg != 0)
		{
		  prev_insn = read_memory_integer (memaddr - 4,
						   sizeof(prev_insn));
		  basereg_prev = GET_FIELD (prev_insn, 6, 10);

		  if ((prev_insn & 0xfc000000) == 0x20000000 /* ldil */
		      && basereg == basereg_prev)
		    target_address += extract_21 (prev_insn);
		}
	      fprintf_filtered (stream, "\t! ");
	      print_address (target_address, stream);
	    }

	  return sizeof(insn);
	}
    }
  fprintf_filtered (stream, "%#8x", insn);
  return sizeof(insn);
}
  
/* Utility function to print registers */

static void
fput_reg (reg, stream)
     unsigned reg;
     FILE *stream;
{
  if (reg)
    fputs_filtered (reg_names[reg], stream);
  else
    fputs_filtered ("r0", stream);
}

void
fput_reg_r (reg, stream)
     unsigned reg;
     FILE *stream;
{
  if (reg)
    fputs_filtered (reg_names[reg], stream);
  else
    fputs_filtered ("r0", stream);
  fputs_filtered ("R", stream);
}

void
fput_creg (reg, stream)
     unsigned reg;
     FILE *stream;
{
  fputs_filtered (control_reg[reg], stream);
}

/* print constants with sign */

void
fput_const (num, stream)
     unsigned num;
     FILE *stream;
{
  if ((int)num < 0)
    fprintf_filtered (stream, "-%x", -(int)num);
  else
    fprintf_filtered (stream, "%x", num);
}
