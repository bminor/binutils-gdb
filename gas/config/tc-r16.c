/* tc-r16.c -- Assemble code for the Experimental R16

   Copyright (C) 1993 Free Software Foundation.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
  Adapted from the SH assember
  Relocation doesnt work yet.
  */

#include <stdio.h>
#include "as.h"
#include "bfd.h"
#include "subsegs.h"
#define DEFINE_TABLE
#include "../opcodes/r16-opc.h"
#include <ctype.h>

#if 1	/**** TEMP ****/
#define R_PCRELIMM8BY4  23	/* 8 bit pc relative to long boundary shifted 4 */
#define R_PCRELIMM11BY2	24	/* 11 bit pc relative to long boundary shifted 2 */
#endif	/**** TEMP ****/

const char comment_chars[] = "!";
const char line_separator_chars[] = ";";
const char line_comment_chars[] = "!";

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
   */

void cons ();
void s_align_bytes ();

const pseudo_typeS md_pseudo_table[] =
{
  {"page", listing_eject, 0},
  {0, 0, 0}
};

const int md_reloc_size = 8;

static int relax;		/* set if -relax seen */

const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

#define JREG 0			/* Register used as a temp when relaxing */
#define C(what,length) (((what) << 2) + (length))
#define GET_WHAT(x) ((x>>2))

/* These are the two types of relaxable instruction */
#define COND_JUMP 1
#define UNCD_JUMP  2

#define UNDEF_DISP 0
#define COND12 1
#define COND32 2
#define UNCD12 1
#define UNCD32 2
#define UNDEF_WORD_DISP 4
#define END 5

#define C12_LEN	2
#define C32_LEN	10	/* allow for align */
#define U12_LEN	2
#define U32_LEN	8	/* allow for align */


/* Initialize the relax table */
const relax_typeS md_relax_table[] =
{
{    1,     1,	     0, 0 },			/* 0: unused */  
{    1,     1,	     0, 0 },			/* 1: unused */  
{    1,     1,	     0, 0 },			/* 2: unused */  
{    1,     1,	     0, 0 },			/* 3: unused */  
{    1,     1,	     0, 0 },			/* 4: unused */  
{ 2048, -2046, C12_LEN, C(COND_JUMP, COND32) },	/* 5: C(COND_JUMP, COND12) */
{    0,     0, C32_LEN, 0 },			/* 6: C(COND_JUMP, COND32) */
{    1,     1,	     0, 0 },			/* 7: unused */  
{    1,     1,	     0, 0 },			/* 8: unused */  
{ 2048, -2046, U12_LEN, C(UNCD_JUMP, UNCD32) },	/* 9: C(UNCD_JUMP, UNCD12) */
{    0,     0, U32_LEN, 0 },			/*10: C(UNCD_JUMP, UNCD32) */
{    1,     1,	     0, 0 },			/*11: unused */  
};

static struct hash_control *opcode_hash_control;	/* Opcode mnemonics */

/*
  This function is called once, at assembler startup time.  This should
  set up all the tables, etc that the MD part of the assembler needs
  */

void
md_begin ()
{
  r16_opcode_info *opcode;
  char *prev_name = "";

  opcode_hash_control = hash_new ();

  /* Insert unique names into hash table */
  for (opcode = r16_table; opcode->name; opcode++)
    {
      if (strcmp (prev_name, opcode->name))
	{
	  prev_name = opcode->name;
	  hash_insert (opcode_hash_control, opcode->name, (char *) opcode);
	}
      else
	{
	  /* Make all the opcodes with the same name point to the same
	   string */
	  opcode->name = prev_name;
	}
    }
}

static int reg_m;
static int reg_n;
static expressionS immediate;	/* absolute expression */

/* try and parse a reg name */
static char *
parse_reg (s, reg)
char *s;
unsigned *reg;
{
    if (s[0] == 'r')
    {
	if (s[1] == '1' && s[2] >= '0' && s[2] <= '5')
	{
	    *reg = 10 + s[2] - '0';
	    return s+3;
	}
	if (s[1] >= '0' && s[1] <= '9')
	{
	    *reg = (s[1] - '0');
	    return s+2;
	}
    }
    as_bad("register expected");
    return s;
}

static char *
parse_creg (s, reg)
char *s;
unsigned *reg;
{
    if (s[0] == 'c' && s[1] == 'r')
    {
	if (s[2] == '1' && s[3] >= '0' && s[3] <= '5')
	{
	    *reg = 10 + s[3] - '0';
	    return s+4;
	}
	if (s[2] >= '0' && s[2] <= '9')
	{
	    *reg = (s[2] - '0');
	    return s+3;
	}
    }
    as_bad("register expected");
    return s;
}

static char *
parse_exp(s)
char *s;
{   char *save;
    char *new;

    save = input_line_pointer;
    input_line_pointer = s;
    expression(&immediate);
    if (immediate.X_op == O_absent)
	as_bad("missing operand");
    new = input_line_pointer;
    input_line_pointer = save;
    return new;
}

static char *
parse_imm(s, val, min, max)
char *s;
unsigned *val;
unsigned min, max;
{   char *new;

    new = parse_exp(s);
    if (immediate.X_op != O_constant
	|| immediate.X_add_number < min
	|| immediate.X_add_number > max)
    {
	as_bad ("operand must be absolute in range %d..%d", min, max);
    }
    *val = immediate.X_add_number;
    return new;
}


static char *
parse_mem(s, reg, off, siz)
char *s;
unsigned *reg;
unsigned *off;
unsigned siz;
{   char *new;

    if (*s == '(')
    {   s = parse_reg(s+1, reg);
	if (*s == ',')
	{   s = parse_imm(s+1, off, 0, 63);
	    if (siz > 1)
	    {   if (siz > 2)
		  {
		    if (*off & 0x3)
		      as_bad ("operand must be a multiple of 4");
		    *off >>= 2;
		  }
		else
		  {
		    if (*off & 0x1)
		      as_bad ("operand must be a multiple of 2");
		    *off >>= 1;
		  }
	    }
	}
	else
	    *off = 0;
	if (*s == ')')
	    s++;
    }
    else
	as_bad("base register expected");
    return s;
}



/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.
   */

void
md_assemble (str)
char *str;
{
    char *op_start;
    char *op_end;
    r16_opcode_info *opcode;
    char *output;
    int nlen = 0;
    unsigned short inst;
    unsigned reg, off;
    char name[20];

    /* Drop leading whitespace */
    while (*str == ' ')
	str++;

    /* find the op code end */
    for (op_start = op_end = str;
	*op_end && nlen < 20 && !is_end_of_line[*op_end] && *op_end != ' ';
	op_end++)
    {
	name[nlen] = op_start[nlen];
	nlen++;
    }
    name[nlen] = 0;
    if (nlen == 0)
    {
	as_bad ("can't find opcode ");
	return;
    }

    opcode = (r16_opcode_info *) hash_find (opcode_hash_control, name);
    if (opcode == NULL)
    {
	as_bad ("unknown opcode \"%s\"", name);
	return;
    }
    inst = opcode->inst;
    switch (opcode->opclass)
    { case O0:
	output = frag_more (2);
	break;
      case OT:
	op_end = parse_imm(op_end + 1, &reg, 0, 15);
	inst |= reg;
	output = frag_more (2);
	break;
      case O1:
	op_end = parse_reg (op_end + 1, &reg);
	inst |= reg;
	output = frag_more (2);
	break;
      case OC:
	op_end = parse_reg (op_end + 1, &reg);
	inst |= reg;
	if (*op_end == ',')
	{   op_end = parse_creg(op_end + 1, &reg);
	    inst |= reg<<4;
	}
	output = frag_more (2);
	break;
      case O2:
	op_end = parse_reg (op_end + 1, &reg);
	inst |= reg;
	if (*op_end == ',')
	{   op_end = parse_reg(op_end + 1, &reg);
	    inst |= reg<<4;
	}
	else
	    as_bad("second operand missing");
	output = frag_more (2);
	break;
      case OI:
	op_end = parse_reg (op_end + 1, &reg);
	inst |= reg;
	if (*op_end == ',')
	{   op_end = parse_imm(op_end + 1, &reg, 1, 32);
	    inst |= (reg-1)<<4;
	}
	else
	    as_bad("second operand missing");
	output = frag_more (2);
	break;
      case OB:
	op_end = parse_reg (op_end + 1, &reg);
	inst |= reg;
	if (*op_end == ',')
	{   op_end = parse_imm(op_end + 1, &reg, 0, 31);
	    inst |= reg<<4;
	}
	else
	    as_bad("second operand missing");
	output = frag_more (2);
	break;
      case LS:
	op_end = parse_reg(op_end + 1, &reg);
	inst |= reg<<8;
	if (*op_end == ',')
	  {   
	    int size;
	    if ((inst & 0x6000) == 0)
	      size = 4;
	    else if ((inst & 0x6000) == 0x4000)
	      size = 2;
	    else if ((inst & 0x6000) == 0x2000)
	      size = 1;
	    op_end = parse_mem(op_end + 1, &reg, &off, size);
	    inst |= (reg) | (off<<4);
	  }
	else
	    as_bad("second operand missing");
	output = frag_more (2);
	break;
      case LI:
	op_end = parse_reg (op_end + 1, &reg);
	if (*op_end == ',')
	{   unsigned val;
	    op_end = parse_imm(op_end + 1, &val, 0, 0x7FF);
	    inst |= val&0x7FF;
	}
	else
	    as_bad("second operand missing");
	if (reg != 1)
	    as_bad("register must be r1");
	output = frag_more (2);
	break;
      case LR:
	op_end = parse_reg(op_end + 1, &reg);
	if (reg == 3 || reg == 15)
	  as_bad("register must not be r3 or r15");
	inst |= (reg<<8);
	output = frag_more (2);
	if (*op_end++ == ',')
	  {   
	    if (*op_end++ != '[')
	      as_bad ("second operand missing '['");
	    input_line_pointer = parse_exp(op_end);
	    if (*input_line_pointer++ != ']')
	      as_bad ("second operand missing ']'");
	    fix_new_exp(frag_now, output-frag_now->fr_literal, 2, &immediate,
			1, R_PCRELIMM8BY4);
	  }
	else
	    as_bad("second operand missing");
	break;
      case LJ:
	output = frag_more (2);
	if (*++op_end != '[')
	    as_bad ("operand missing '['");
	input_line_pointer = parse_exp(op_end+1);
	if (*input_line_pointer++ != ']')
	    as_bad ("operand missing ']'");
	fix_new_exp(frag_now, output-frag_now->fr_literal, 2, &immediate,
		1, R_PCRELIMM8BY4);
	break;
      case OM:
	op_end = parse_reg(op_end + 1, &reg);
	if (*op_end == '-')
	{   int endreg;
	    op_end = parse_reg(op_end + 1, &endreg);
	    if (*op_end == ',')
	    {   int basereg;
		op_end++;
		if (*op_end == '(')
		{   op_end = parse_reg(op_end + 1, &basereg);
		    if (*op_end == ')')
			op_end++;
		    if (endreg == 15 && basereg == 3)
		    {	inst |= 0x0080;		/* list form */
		    	inst |= reg;
		    }
		    else if (endreg - reg == 3)
		    {	inst |= basereg;	/* quadrant form */
			switch (reg)
			{ case 0:		   break;
			  case 4:  inst |= (1<<5); break;
			  case 8:  inst |= (2<<5); break;
			  case 12: inst |= (3<<5); break;
			  default:
			    as_bad("first register must be r0, r4, r8, or r12");
			}
		    }
		    else
			as_bad("bad register list or base register");
		}
		else
		    as_bad("base register expected");
	    }
	    else
		as_bad("second operand missing");
	}
	else
	    as_bad("reg-reg expected");
	output = frag_more (2);
	break;
      case BR:
	output = frag_more (2);
	input_line_pointer = parse_exp(op_end+1);
	fix_new_exp (frag_now, output-frag_now->fr_literal, 2, &immediate,
		     1, R_PCRELIMM11BY2);
	break;
      default:
	as_bad("cant deal with opcode \"%s\"", name);
    }
    output[0] = (inst>>8);
    output[1] = (inst);
}

#ifndef BFD_ASSEMBLER
void
DEFUN (tc_crawl_symbol_chain, (headers),
       object_headers * headers)
{
}

void
DEFUN (tc_headers_hook, (headers),
       object_headers * headers)
{
}
#endif

symbolS *
DEFUN (md_undefined_symbol, (name),
       char *name)
{
  return 0;
}

void
DEFUN_VOID (md_end)
{
}

/* Various routines to kill one day */
/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
   */
char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  char *atof_ieee ();

  switch (type)
    {
    case 'f':
    case 'F':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'd':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

    case 'x':
    case 'X':
      prec = 6;
      break;

    case 'p':
    case 'P':
      prec = 6;
      break;

    default:
      *sizeP = 0;
      return "Bad call to MD_NTOF()";
    }
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (long) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
  return 0;
}

int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;

{
  if (!strcmp (*argP, "relax"))
    {
      relax = 1;
      **argP = 0;
    }
  return 1;
}

int md_short_jump_size;

void
tc_Nout_fix_to_chars ()
{
  as_fatal ("call to tc_Nout_fix_to_chars");
}

void
md_create_short_jump (ptr, from_Nddr, to_Nddr, frag, to_symbol)
     char *ptr;
     addressT from_Nddr;
     addressT to_Nddr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check: short_jump");
}

void
md_create_long_jump (ptr, from_Nddr, to_Nddr, frag, to_symbol)
     char *ptr;
     addressT from_Nddr, to_Nddr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check: long_jump");
}

/*
called after relaxing, change the frags so they know how big they are
*/
#ifndef BFD_ASSEMBLER
void
md_convert_frag (headers, fragP)
     object_headers *headers;
     register fragS *fragP;
#else
void
md_convert_frag (abfd, sec, fragP)
     bfd *abfd;
     segT sec;
     register fragS *fragP;
#endif
{
  unsigned char *buffer = (unsigned char *) (fragP->fr_fix + fragP->fr_literal);
  int targ_addr = S_GET_VALUE (fragP->fr_symbol) + fragP->fr_offset;
#ifdef BFD_ASSEMBLER /* not needed otherwise? */
  targ_addr += fragP->fr_symbol->sy_frag->fr_address;
#endif
  switch (fragP->fr_subtype)
    {
    case C (COND_JUMP, COND12):
    case C (UNCD_JUMP, UNCD12):
      {
	/* Get the address of the end of the instruction */
	int next_inst = fragP->fr_fix + fragP->fr_address + 2;
	unsigned char t0;
	int disp = targ_addr - next_inst;
	if (disp&1)
	    as_bad("odd displacement at %x", next_inst-2);
	if (disp < 0)	/* move sign to low order bit */
	    disp |= 1;
	t0 = buffer[0] & 0xF8;
	md_number_to_chars (buffer, disp, 2);
	buffer[0] = (buffer[0] & 0x07) | t0;
	fragP->fr_fix += 2;
	fragP->fr_var = 0;
      }
      break;

    case C (COND_JUMP, COND32):
    case C (COND_JUMP, UNDEF_WORD_DISP):
      {
	/* A conditional branch wont fit into 12 bits so:
	 *	b!cond	1f
	 *	jmpi	0f
	 *	.align 2
	 * 0:	.long disp
	 * 1:
	 */
	int next_inst = fragP->fr_fix + fragP->fr_address + C32_LEN;
	int align = next_inst&02;
	buffer[0] ^= 0x08;	/* Toggle T/F bit */
	buffer[2] = 0x73;	/* Build jmpi */
	buffer[3] = 0x00;
	if (align)
	{
	    buffer[1] = 3;	/* branch over jmpi, and ptr */
	    buffer[4] = 0;	/* space for 32 bit address */
	    buffer[5] = 0;
	    buffer[6] = 0;
	    buffer[7] = 0;
	    /* Make reloc for the long disp */
	    fix_new(fragP, fragP->fr_fix + 4, 4,
		    fragP->fr_symbol, fragP->fr_offset, 0, BFD_RELOC_32);
#if 0
	    /**** frag has shrunk but gas can't deal with that */
	    fragP->fr_fix += C32_LEN - 2;
#else
	    fragP->fr_fix += C32_LEN;
#endif
	}
	else
	{
	    buffer[1] = 4;	/* branch over jmpi, and ptr */
	    buffer[4] = 0;	/* alignment */
	    buffer[5] = 0;
	    buffer[6] = 0;	/* space for 32 bit address */
	    buffer[7] = 0;
	    buffer[8] = 0;
	    buffer[9] = 0;
	    /* Make reloc for the long disp */
	    fix_new(fragP, fragP->fr_fix + 6, 4,
		    fragP->fr_symbol, fragP->fr_offset, 0, BFD_RELOC_32);
	    fragP->fr_fix += C32_LEN;
	}
	fragP->fr_var = 0;
      }
      break;

    case C (UNCD_JUMP, UNCD32):
    case C (UNCD_JUMP, UNDEF_WORD_DISP):
      {
	/* An unconditional branch wont fit in 12 bits, make code which looks like
	 *	jmpi	0f
	 *	.align 2
	 * 0:	.long disp
	 */
	int next_inst = fragP->fr_fix + fragP->fr_address + U32_LEN;
	int align = next_inst&02;
	buffer[0] = 0x73;	/* build jmpi */
	buffer[1] = 0x00;
	if (align)
	{
	    buffer[2] = 0;		/* space for 32 bit address */
	    buffer[3] = 0;
	    buffer[4] = 0;
	    buffer[5] = 0;
	    /* Make reloc for the long disp */
	    fix_new (fragP, fragP->fr_fix + 2, 4,
		     fragP->fr_symbol, fragP->fr_offset, 0, BFD_RELOC_32);
#if 0
	    /**** frag has shrunk but gas can't deal with that */
	    fragP->fr_fix += U32_LEN - 2;
#else
	    fragP->fr_fix += U32_LEN;
#endif
	}
	else
	{
	    buffer[2] = 0;		/* alignment */
	    buffer[3] = 0;
	    buffer[4] = 0;		/* space for 32 bit address */
	    buffer[5] = 0;
	    buffer[6] = 0;
	    buffer[7] = 0;
	    /* Make reloc for the long disp */
	    fix_new (fragP, fragP->fr_fix + 4, 4,
		     fragP->fr_symbol, fragP->fr_offset, 0, BFD_RELOC_32);
	    fragP->fr_fix += U32_LEN;
	}
	fragP->fr_var = 0;
      }
      break;

    default:
      abort ();
    }
}

void
md_apply_fix1 (fixP, val)
     fixS *fixP;
     long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  int addr = fixP->fx_frag->fr_address + fixP->fx_where;

  switch (fixP->fx_r_type)
  {
    case R_PCRELIMM11BY2:		/* second byte of 2 byte opcode */
      val /= 2;
      if (((val & ~0x3ff) != 0) && ((val | 0x3ff) != -1))
	as_warn ("pcrel for branch too far (0x%x) at 0x%x", val, addr);
      buf[0] |= ((val >> 7) & 0x7);
      buf[1] |= ((val & 0x7f) << 1);
      buf[1] |= ((val >> 10) & 0x1);
      break;
    case R_PCRELIMM8BY4:	/* lower 8 bits of 2 byte opcode */
      val += 3;
      val /= 4;
      if (val & ~0xff)
	as_warn ("pcrel for lrw too far (0x%x) at 0x%x", val, addr);
      buf[1] |= (val & 0xff);
      break;
    default:
      if (fixP->fx_size != 4)
	abort ();
      *buf++ = val >> 24;
      *buf++ = val >> 16;
      *buf++ = val >> 8;
      *buf = val >> 0;
      break;
    }
}

#ifdef BFD_ASSEMBLER
int
md_apply_fix (fixP, valp)
     fixS *fixP;
     valueT *valp;
{
  md_apply_fix1 (fixP, *valp);
  return 1;
}
#else
void
md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  md_apply_fix1 (fixP, val);
}
#endif

void
DEFUN (md_operand, (expressionP), expressionS * expressionP)
{
}

int md_long_jump_size;

/*
called just before address relaxation, return the length
by which a fragment must grow to reach it's destination
*/
int
md_estimate_size_before_relax (fragP, segment_type)
     register fragS *fragP;
     register segT segment_type;
{
  switch (fragP->fr_subtype)
    {
    case C (UNCD_JUMP, UNDEF_DISP):
      /* used to be a branch to somewhere which was unknown */
      if (!fragP->fr_symbol)
	{
	  fragP->fr_subtype = C (UNCD_JUMP, UNCD12);
	  fragP->fr_var = md_relax_table[C (UNCD_JUMP, UNCD12)].rlx_length;
	}
      else if (S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	{
	  fragP->fr_subtype = C (UNCD_JUMP, UNCD12);
	  fragP->fr_var = md_relax_table[C (UNCD_JUMP, UNCD12)].rlx_length;
	}
      else
	{
	  fragP->fr_subtype = C (UNCD_JUMP, UNDEF_WORD_DISP);
	  fragP->fr_var = md_relax_table[C (UNCD_JUMP, UNCD32)].rlx_length;
	  return md_relax_table[C (UNCD_JUMP, UNCD32)].rlx_length;
	}
      break;

    default:
      abort ();
    case C (COND_JUMP, UNDEF_DISP):
      /* used to be a branch to somewhere which was unknown */
      if (fragP->fr_symbol
	  && S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	{
	  /* Got a symbol and it's defined in this segment, become byte
	 sized - maybe it will fix up */
	  fragP->fr_subtype = C (COND_JUMP, COND12);
	  fragP->fr_var = md_relax_table[C (COND_JUMP, COND12)].rlx_length;
	}
      else if (fragP->fr_symbol)
	{
	  /* Its got a segment, but its not ours, so it will always be long */
	  fragP->fr_subtype = C (COND_JUMP, UNDEF_WORD_DISP);
	  fragP->fr_var = md_relax_table[C (COND_JUMP, COND32)].rlx_length;
	  return md_relax_table[C (COND_JUMP, COND32)].rlx_length;
	}
      else
	{
	  /* We know the abs value */
	  fragP->fr_subtype = C (COND_JUMP, COND12);
	  fragP->fr_var = md_relax_table[C (COND_JUMP, COND12)].rlx_length;
	}

      break;
    }
  return fragP->fr_var;
}

/* Put number into target byte order */

void
md_number_to_chars (ptr, use, nbytes)
     char *ptr;
     valueT use;
     int nbytes;
{
  switch (nbytes)
    {
    case 4:
      *ptr++ = (use >> 24) & 0xff;
    case 3:
      *ptr++ = (use >> 16) & 0xff;
    case 2:
      *ptr++ = (use >> 8) & 0xff;
    case 1:
      *ptr++ = (use >> 0) & 0xff;
      break;
    default:
      abort ();
    }
}


/* Round up a section size to the appropriate boundary.  */
valueT
md_section_align (segment, size)
     segT segment;
     valueT size;
{
  return size;			/* Byte alignment is fine */
}


long
md_pcrel_from (fixP)
     fixS *fixP;

{
  int gap = fixP->fx_size + fixP->fx_where +
  fixP->fx_frag->fr_address;
  return gap;
}



#ifdef BFD_ASSEMBLER

arelent *
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  arelent *rel;
  bfd_reloc_code_real_type code;

#define F(SZ,PCREL)		(((SZ) << 1) + (PCREL))
  switch (F (fixp->fx_size, fixp->fx_pcrel))
    {
#define MAP(SZ,PCREL,TYPE)	case F(SZ,PCREL): code = (TYPE); break
      MAP (1, 0, BFD_RELOC_8);
      MAP (2, 0, BFD_RELOC_16);
      MAP (4, 0, BFD_RELOC_32);
      MAP (1, 1, BFD_RELOC_8_PCREL);
      MAP (2, 1, BFD_RELOC_16_PCREL);
      MAP (4, 1, BFD_RELOC_32_PCREL);
    default:
      as_bad ("Can not do %d byte %srelocation", fixp->fx_size,
	      fixp->fx_pcrel ? "pc-relative" : "");
    }

  rel = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
  assert (rel != 0);
  rel->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  rel->address = fixp->fx_frag->fr_address + fixp->fx_where;
  if (fixp->fx_pcrel)
    rel->addend = fixp->fx_addnumber;
  else
    rel->addend = 0;

  rel->howto = bfd_reloc_type_lookup (stdoutput, code);
  if (!rel->howto)
    {
      const char *name;

      name = S_GET_NAME (fixp->fx_addsy);
      if (name == NULL)
	name = "<unknown>";
      as_fatal ("Cannot find relocation type for symbol %s, code %d",
		name, (int) code);
    }

  return rel;
}

#else	/* !BFD_ASSEMBLER */

#if (defined(OBJ_AOUT) | defined(OBJ_BOUT))
void
tc_aout_fix_to_chars(where, fixP, segment_address_in_file)
char *where;
fixS *fixP;
relax_addressT segment_address_in_file;
{
    /*
     * In: length of relocation (or of address) in chars: 1, 2 or 4.
     * Out: GNU LD relocation length code: 0, 1, or 2.
     */

    static CONST unsigned char nbytes_r_length[] = {42, 0, 1, 42, 2};
    long r_symbolnum;

    know (fixP->fx_addsy != NULL);

    md_number_to_chars (where,
	    fixP->fx_frag->fr_address + fixP->fx_where - segment_address_in_file,
	    4);

    r_symbolnum = (S_IS_DEFINED (fixP->fx_addsy)
	    ? S_GET_TYPE (fixP->fx_addsy)
	    : fixP->fx_addsy->sy_number);

    where[4] = (r_symbolnum >> 16) & 0x0ff;
    where[5] = (r_symbolnum >> 8) & 0x0ff;
    where[6] = r_symbolnum & 0x0ff;
    where[7] = (((fixP->fx_pcrel << 7) & 0x80)
	    | ((nbytes_r_length[fixP->fx_size] << 5) & 0x60)
	    | (((!S_IS_DEFINED (fixP->fx_addsy)) << 4) & 0x10));

}

void
tc_aout_pre_write_hook (headers)
     object_headers *headers;
{
}
#endif
#endif	/* !BFD_ASSEMBLER */

