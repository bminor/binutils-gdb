/* tc-sh.c -- Assemble code for the Hitachi Super-H

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
   Written By Steve Chamberlain
   sac@cygnus.com
 */

#include <stdio.h>
#include "as.h"
#include "bfd.h"
#include "subsegs.h"
#define DEFINE_TABLE
#include "opcodes/sh-opc.h"
#include <ctype.h>
const char comment_chars[] = "!";
const char line_separator_chars[] = ";";
const char line_comment_chars[] = "!#";

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
 */

void cons ();
void s_align_bytes ();

int shl = 0;

static void little()
{
  shl = 1;
}

const pseudo_typeS md_pseudo_table[] =
{
  {"int", cons, 4},
  {"word", cons, 2},
  {"form", listing_psize, 0},
  {"little", little,0},
  {"heading", listing_title, 0},
  {"import", s_ignore, 0},
  {"page", listing_eject, 0},
  {"program", s_ignore, 0},
  {0, 0, 0}
};

/*int md_reloc_size; */

static int relax;		/* set if -relax seen */

const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

#define C(a,b) ENCODE_RELAX(a,b)

#define JREG 14			/* Register used as a temp when relaxing */
#define ENCODE_RELAX(what,length) (((what) << 4) + (length))
#define GET_WHAT(x) ((x>>4))

/* These are the two types of relaxable instrction */
#define COND_JUMP 1
#define UNCOND_JUMP  2

#define UNDEF_DISP 0
#define COND8  1
#define COND12 2
#define COND32 3
#define UNCOND12 1
#define UNCOND32 2
#define UNDEF_WORD_DISP 4
#define END 5

#define UNCOND12 1
#define UNCOND32 2

#define COND8_F 254
#define COND8_M -256
#define COND8_LENGTH 2
#define COND12_F (4094 - 4)	/* -4 since there are two extra */
/* instructions needed */
#define COND12_M -4096
#define COND12_LENGTH 6
#define COND32_F (1<<30)
#define COND32_M -(1<<30)
#define COND32_LENGTH 14

#define COND8_RANGE(x) ((x) > COND8_M && (x) < COND8_F)
#define COND12_RANGE(x) ((x) > COND12_M && (x) < COND12_F)

#define UNCOND12_F 4094
#define UNCOND12_M -4096
#define UNCOND12_LENGTH 2

#define UNCOND32_F (1<<30)
#define UNCOND32_M -(1<<30)
#define UNCOND32_LENGTH 14


const relax_typeS md_relax_table[C (END, 0)] = {
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },

  { 0 },
  /* C (COND_JUMP, COND8) */
  { COND8_F, COND8_M, COND8_LENGTH, C (COND_JUMP, COND12) },
  /* C (COND_JUMP, COND12) */
  { COND12_F, COND12_M, COND12_LENGTH, C (COND_JUMP, COND32), },
  /* C (COND_JUMP, COND32) */
  { COND32_F, COND32_M, COND32_LENGTH, 0, },
  { 0 }, { 0 }, { 0 }, { 0 },
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },

  { 0 },
  /* C (UNCOND_JUMP, UNCOND12) */
  { UNCOND12_F, UNCOND12_M, UNCOND12_LENGTH, C (UNCOND_JUMP, UNCOND32), },
  /* C (UNCOND_JUMP, UNCOND32) */
  { UNCOND32_F, UNCOND32_M, UNCOND32_LENGTH, 0, },
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
};

static struct hash_control *opcode_hash_control;	/* Opcode mnemonics */

/*
   This function is called once, at assembler startup time.  This should
   set up all the tables, etc that the MD part of the assembler needs
 */

void
md_begin ()
{
  sh_opcode_info *opcode;
  char *prev_name = "";

  opcode_hash_control = hash_new ();

  /* Insert unique names into hash table */
  for (opcode = sh_table; opcode->name; opcode++)
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

typedef struct
  {
    sh_arg_type type;
    int reg;
  }

sh_operand_info;

/* try and parse a reg name, returns number of chars consumed */
static int
parse_reg (src, mode, reg)
     char *src;
     int *mode;
     int *reg;
{
  if (src[0] == 'r')
    {
      if (src[1] == '1')
	{
	  if (src[2] >= '0' && src[2] <= '5')
	    {
	      *mode = A_REG_N;
	      *reg = 10 + src[2] - '0';
	      return 3;
	    }
	}
      if (src[1] >= '0' && src[1] <= '9')
	{
	  *mode = A_REG_N;
	  *reg = (src[1] - '0');
	  return 2;
	}
    }

  if (src[0] == 's' && src[1] == 'r')
    {
      *mode = A_SR;
      return 2;
    }

  if (src[0] == 's' && src[1] == 'p')
    {
      *mode = A_REG_N;
      *reg = 15;
      return 2;
    }

  if (src[0] == 'p' && src[1] == 'r')
    {
      *mode = A_PR;
      return 2;
    }
  if (src[0] == 'p' && src[1] == 'c')
    {
      *mode = A_DISP_PC;
      return 2;
    }
  if (src[0] == 'g' && src[1] == 'b' && src[2] == 'r')
    {
      *mode = A_GBR;
      return 3;
    }
  if (src[0] == 'v' && src[1] == 'b' && src[2] == 'r')
    {
      *mode = A_VBR;
      return 3;
    }

  if (src[0] == 'm' && src[1] == 'a' && src[2] == 'c')
    {
      if (src[3] == 'l')
	{
	  *mode = A_MACL;
	  return 4;
	}
      if (src[3] == 'h')
	{
	  *mode = A_MACH;
	  return 4;
	}
    }

  return 0;
}

static symbolS *dot()
{
  const char *fake;

  /* JF: '.' is pseudo symbol with value of current location
     in current segment.  */
  fake = FAKE_LABEL_NAME;
  return  symbol_new (fake,
		      now_seg,
		      (valueT) frag_now_fix (),
		      frag_now);

}


static
char *
parse_exp (s)
     char *s;
{
  char *save;
  char *new;

  save = input_line_pointer;
  input_line_pointer = s;
  expression (&immediate);
  if (immediate.X_op == O_absent)
    as_bad ("missing operand");
  new = input_line_pointer;
  input_line_pointer = save;
  return new;
}


/* The many forms of operand:

   Rn                   Register direct
   @Rn                  Register indirect
   @Rn+                 Autoincrement
   @-Rn                 Autodecrement
   @(disp:4,Rn)
   @(disp:8,GBR)
   @(disp:8,PC)

   @(R0,Rn)
   @(R0,GBR)

   disp:8
   disp:12
   #imm8
   pr, gbr, vbr, macl, mach

 */

static
char *
parse_at (src, op)
     char *src;
     sh_operand_info *op;
{
  int len;
  int mode;
  src++;
  if (src[0] == '-')
    {
      /* Must be predecrement */
      src++;

      len = parse_reg (src, &mode, &(op->reg));
      if (mode != A_REG_N)
	as_bad ("illegal register after @-");

      op->type = A_DEC_N;
      src += len;
    }
  else if (src[0] == '(')
    {
      /* Could be @(disp, rn), @(disp, gbr), @(disp, pc),  @(r0, gbr) or
         @(r0, rn) */
      src++;
      len = parse_reg (src, &mode, &(op->reg));
      if (len && mode == A_REG_N)
	{
	  src += len;
	  if (op->reg != 0)
	    {
	      as_bad ("must be @(r0,...)");
	    }
	  if (src[0] == ',')
	    src++;
	  /* Now can be rn or gbr */
	  len = parse_reg (src, &mode, &(op->reg));
	  if (mode == A_GBR)
	    {
	      op->type = A_R0_GBR;
	    }
	  else if (mode == A_REG_N)
	    {
	      op->type = A_IND_R0_REG_N;
	    }
	  else
	    {
	      as_bad ("syntax error in @(r0,...)");
	    }
	}
      else
	{
	  /* Must be an @(disp,.. thing) */
	  src = parse_exp (src);
	  if (src[0] == ',')
	    src++;
	  /* Now can be rn, gbr or pc */
	  len = parse_reg (src, &mode, &op->reg);
	  if (len)
	    {
	      if (mode == A_REG_N)
		{
		  op->type = A_DISP_REG_N;
		}
	      else if (mode == A_GBR)
		{
		  op->type = A_DISP_GBR;
		}
	      else if (mode == A_DISP_PC)
		{
		  /* Turn a plain @(4,pc) into @(.+4,pc) */
		  if (immediate.X_op == O_constant) { 
		    immediate.X_add_symbol = dot();
		    immediate.X_op = O_symbol;
		  }
		  op->type = A_DISP_PC;
		}
	      else
		{
		  as_bad ("syntax error in @(disp,[Rn, gbr, pc])");
		}
	    }
	  else
	    {
	      as_bad ("syntax error in @(disp,[Rn, gbr, pc])");
	    }
	}
      src += len;
      if (src[0] != ')')
	as_bad ("expecting )");
      else
	src++;
    }
  else
    {
      src += parse_reg (src, &mode, &(op->reg));
      if (mode != A_REG_N)
	{
	  as_bad ("illegal register after @");
	}
      if (src[0] == '+')
	{
	  op->type = A_INC_N;
	  src++;
	}
      else
	{
	  op->type = A_IND_N;
	}
    }
  return src;
}

static void
get_operand (ptr, op)
     char **ptr;
     sh_operand_info *op;
{
  char *src = *ptr;
  int mode = -1;
  unsigned int len;

  if (src[0] == '#')
    {
      src++;
      *ptr = parse_exp (src);
      op->type = A_IMM;
      return;
    }

  else if (src[0] == '@')
    {
      *ptr = parse_at (src, op);
      return;
    }
  len = parse_reg (src, &mode, &(op->reg));
  if (len)
    {
      *ptr = src + len;
      op->type = mode;
      return;
    }
  else
    {
      /* Not a reg, the only thing left is a displacement */
      *ptr = parse_exp (src);
      op->type = A_DISP_PC;
      return;
    }
}

static
char *
get_operands (info, args, operand)
     sh_opcode_info *info;
     char *args;
     sh_operand_info *operand;

{
  char *ptr = args;
  if (info->arg[0])
    {
      ptr++;

      get_operand (&ptr, operand + 0);
      if (info->arg[1])
	{
	  if (*ptr == ',')
	    {
	      ptr++;
	    }
	  get_operand (&ptr, operand + 1);
	}
      else
	{
	  operand[1].type = 0;
	}
    }
  else
    {
      operand[0].type = 0;
      operand[1].type = 0;
    }
  return ptr;
}

/* Passed a pointer to a list of opcodes which use different
   addressing modes, return the opcode which matches the opcodes
   provided
 */

static
sh_opcode_info *
get_specific (opcode, operands)
     sh_opcode_info *opcode;
     sh_operand_info *operands;
{
  sh_opcode_info *this_try = opcode;
  char *name = opcode->name;
  int arg_to_test = 0;
  int n = 0;
  while (opcode->name)
    {
      this_try = opcode++;
      if (this_try->name != name)
	{
	  /* We've looked so far down the table that we've run out of
	     opcodes with the same name */
	  return 0;
	}
      /* look at both operands needed by the opcodes and provided by
         the user - since an arg test will often fail on the same arg
         again and again, we'll try and test the last failing arg the
         first on each opcode try */

      for (n = 0; this_try->arg[n]; n++)
	{
	  sh_operand_info *user = operands + arg_to_test;
	  sh_arg_type arg = this_try->arg[arg_to_test];
	  switch (arg)
	    {
	    case A_IMM:
	    case A_BDISP12:
	    case A_BDISP8:
	    case A_DISP_GBR:
	    case A_DISP_PC:
	    case A_MACH:
	    case A_PR:
	    case A_MACL:
	      if (user->type != arg)
		goto fail;
	      break;
	    case A_R0:
	      /* opcode needs r0 */
	      if (user->type != A_REG_N || user->reg != 0)
		goto fail;
	      break;
	    case A_R0_GBR:
	      if (user->type != A_R0_GBR || user->reg != 0)
		goto fail;
	      break;

	    case A_REG_N:
	    case A_INC_N:
	    case A_DEC_N:
	    case A_IND_N:
	    case A_IND_R0_REG_N:
	    case A_DISP_REG_N:
	      /* Opcode needs rn */
	      if (user->type != arg)
		goto fail;
	      reg_n = user->reg;
	      break;
	    case A_GBR:
	    case A_SR:
	    case A_VBR:
	      if (user->type != arg)
		goto fail;
	      break;

	    case A_REG_M:
	    case A_INC_M:
	    case A_DEC_M:
	    case A_IND_M:
	    case A_IND_R0_REG_M:
	    case A_DISP_REG_M:
	      /* Opcode needs rn */
	      if (user->type != arg - A_REG_M + A_REG_N)
		goto fail;
	      reg_m = user->reg;
	      break;
	    default:
	      printf ("unhandled %d\n", arg);
	      goto fail;
	    }
	  /* If we did 0, test 1 next, else 0 */
	  arg_to_test = 1 - arg_to_test;
	}
      return this_try;
    fail:;
    }

  return 0;
}

int
check (operand, low, high)
     expressionS *operand;
     int low;
     int high;
{
  if (operand->X_op != O_constant
      || operand->X_add_number < low
      || operand->X_add_number > high)
    {
      as_bad ("operand must be absolute in range %d..%d", low, high);
    }
  return operand->X_add_number;
}


static void
insert (where, how, pcrel)
     char *where;
     int how;
     int pcrel;
{
  fix_new_exp (frag_now,
	       where - frag_now->fr_literal,
	       4,
	       &immediate,
	       pcrel,
	       how);

}

static void
build_relax (opcode)
     sh_opcode_info *opcode;
{
  int len;
  int high_byte = shl ? 1 : 0 ;
  char *p;

  if (opcode->arg[0] == A_BDISP8)
    {
      p = frag_var (rs_machine_dependent,
		    md_relax_table[C (COND_JUMP, COND32)].rlx_length,
		    len = md_relax_table[C (COND_JUMP, COND8)].rlx_length,
		    C (COND_JUMP, 0),
		    immediate.X_add_symbol,
		    immediate.X_add_number,
		    0);
      p[high_byte] = (opcode->nibbles[0] << 4) | (opcode->nibbles[1]);
    }
  else if (opcode->arg[0] == A_BDISP12)
    {
      p = frag_var (rs_machine_dependent,
		    md_relax_table[C (UNCOND_JUMP, UNCOND32)].rlx_length,
		    len = md_relax_table[C (UNCOND_JUMP, UNCOND12)].rlx_length,
		    C (UNCOND_JUMP, 0),
		    immediate.X_add_symbol,
		    immediate.X_add_number,
		    0);
      p[high_byte] = (opcode->nibbles[0] << 4);
    }

}

/* Now we know what sort of opcodes it is, lets build the bytes -
 */
static void
build_Mytes (opcode, operand)
     sh_opcode_info *opcode;
     sh_operand_info *operand;

{
  int index;
  char nbuf[4];
  char *output = frag_more (2);
  int low_byte = shl ? 0 : 1;
  nbuf[0] = 0;
  nbuf[1] = 0;
  nbuf[2] = 0;
  nbuf[3] = 0;

  for (index = 0; index < 4; index++)
    {
      sh_nibble_type i = opcode->nibbles[index];
      if (i < 16)
	{
	  nbuf[index] = i;
	}
      else
	{
	  switch (i)
	    {
	    case REG_N:
	      nbuf[index] = reg_n;
	      break;
	    case REG_M:
	      nbuf[index] = reg_m;
	      break;
	    case DISP_4:
	      insert (output + low_byte, R_SH_IMM4, 0);
	      break;
	    case IMM_4BY4:
	      insert (output + low_byte, R_SH_IMM4BY4, 0);
	      break;
	    case IMM_4BY2:
	      insert (output + low_byte, R_SH_IMM4BY2, 0);
	      break;
	    case IMM_4:
	      insert (output + low_byte, R_SH_IMM4, 0);
	      break;
	    case IMM_8BY4:
	      insert (output + low_byte, R_SH_IMM8BY4, 0);
	      break;
	    case IMM_8BY2:
	      insert (output + low_byte, R_SH_IMM8BY2, 0);
	      break;
	    case IMM_8:
	      insert (output + low_byte, R_SH_IMM8, 0);
	      break;
	    case PCRELIMM_8BY4:
	      insert (output, R_SH_PCRELIMM8BY4, 1);
	      break;
	    case PCRELIMM_8BY2:
	      insert (output, R_SH_PCRELIMM8BY2, 1);
	      break;
	    default:
	      printf ("failed for %d\n", i);
	    }
	}
    }
  if (shl) {
    output[1] = (nbuf[0] << 4) | (nbuf[1]);
    output[0] = (nbuf[2] << 4) | (nbuf[3]);
  }
  else {
    output[0] = (nbuf[0] << 4) | (nbuf[1]);
    output[1] = (nbuf[2] << 4) | (nbuf[3]);
  }
}

/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.
 */

void
md_assemble (str)
     char *str;
{
  unsigned char *op_start;
  unsigned char *op_end;
  sh_operand_info operand[2];
  sh_opcode_info *opcode;
  char name[20];
  int nlen = 0;
  /* Drop leading whitespace */
  while (*str == ' ')
    str++;

  /* find the op code end */
  for (op_start = op_end = (unsigned char *) (str);
       *op_end
       && nlen < 20
       && !is_end_of_line[*op_end] && *op_end != ' ';
       op_end++)
    {
      name[nlen] = op_start[nlen];
      nlen++;
    }
  name[nlen] = 0;

  if (nlen == 0)
    {
      as_bad ("can't find opcode ");
    }

  opcode = (sh_opcode_info *) hash_find (opcode_hash_control, name);

  if (opcode == NULL)
    {
      as_bad ("unknown opcode");
      return;
    }

  if (opcode->arg[0] == A_BDISP12
      || opcode->arg[0] == A_BDISP8)
    {
      parse_exp (op_end + 1);
      build_relax (opcode);
    }
  else
    {
      if (opcode->arg[0] != A_END)
	{
	  get_operands (opcode, op_end, operand);
	}
      opcode = get_specific (opcode, operand);

      if (opcode == 0)
	{
	  /* Couldn't find an opcode which matched the operands */
	  char *where = frag_more (2);

	  where[0] = 0x0;
	  where[1] = 0x0;
	  as_bad ("invalid operands for opcode");
	  return;
	}

      build_Mytes (opcode, operand);
    }

}

void
DEFUN (tc_crawl_symbol_chain, (headers),
       object_headers * headers)
{
  printf ("call to tc_crawl_symbol_chain \n");
}

symbolS *
DEFUN (md_undefined_symbol, (name),
       char *name)
{
  return 0;
}

void
DEFUN (tc_headers_hook, (headers),
       object_headers * headers)
{
  printf ("call to tc_headers_hook \n");
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
     int type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return "bad call to md_atof";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  if (shl)
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
  else
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
     
  return NULL;
}



CONST char *md_shortopts = "";
struct option md_longopts[] = {

#define OPTION_RELAX  (OPTION_MD_BASE)
#define OPTION_LITTLE (OPTION_MD_BASE+1)

  {"relax", no_argument, NULL, OPTION_RELAX},
  {"little", no_argument, NULL, OPTION_LITTLE},
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case OPTION_RELAX:
      relax = 1;
      break;
    case OPTION_LITTLE:
      shl = 1;
      break;

    default:
      return 0;
    }

  return 1;
}

void
md_show_usage (stream)
     FILE *stream;
{
  fprintf(stream, "\
SH options:\n\
-relax			alter jump instructions for long displacements\n");
}

int md_short_jump_size;

void
tc_Nout_fix_to_chars ()
{
  printf ("call to tc_Nout_fix_to_chars \n");
  abort ();
}

void
md_create_short_jump (ptr, from_Nddr, to_Nddr, frag, to_symbol)
     char *ptr;
     addressT from_Nddr;
     addressT to_Nddr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check.");
}

void
md_create_long_jump (ptr, from_Nddr, to_Nddr, frag, to_symbol)
     char *ptr;
     addressT from_Nddr, to_Nddr;
     fragS *frag;
     symbolS *to_symbol;
{
  as_fatal ("failed sanity check.");
}

/*
   called after relaxing, change the frags so they know how big they are
 */
void
md_convert_frag (headers, fragP)
     object_headers *headers;
     fragS *fragP;

{
  unsigned char *buffer = (unsigned char *) (fragP->fr_fix + fragP->fr_literal);
  int donerelax = 0;
  int highbyte = shl ? 1 : 0;
  int lowbyte = shl ? 0 : 1;
  int targ_addr = ((fragP->fr_symbol ? S_GET_VALUE (fragP->fr_symbol) : 0) + fragP->fr_offset);
  switch (fragP->fr_subtype)
    {
    case C (COND_JUMP, COND8):
      {
	/* Get the address of the end of the instruction */
	int next_inst = fragP->fr_fix + fragP->fr_address + 2;

	int disp = targ_addr - next_inst - 2;
	disp /= 2;
	
	md_number_to_chars (buffer + lowbyte, disp & 0xff, 1);
	fragP->fr_fix += 2;
	fragP->fr_var = 0;
      }
      break;

    case C (UNCOND_JUMP, UNCOND12):
      {
	/* Get the address of the end of the instruction */
	int next_inst = fragP->fr_fix + fragP->fr_address + 2;

	int t;
	int disp = targ_addr - next_inst - 2;

	disp /= 2;
	t = buffer[highbyte] & 0xf0;
	md_number_to_chars (buffer, disp & 0xfff, 2);
	buffer[highbyte] = (buffer[highbyte] & 0xf) | t;
	fragP->fr_fix += 2;
	fragP->fr_var = 0;
      }
      break;

    case C (UNCOND_JUMP, UNCOND32):
    case C (UNCOND_JUMP, UNDEF_WORD_DISP):
      {
	/* A jump wont fit in 12 bits, make code which looks like
	   bra foo
	   mov.w @(0, PC), r14
	   .long disp
	   foo: bra @r14
	 */

	int next_inst =
	fragP->fr_fix + fragP->fr_address + UNCOND32_LENGTH;

	int disp = targ_addr - next_inst;
	int t = buffer[0] & 0x10;

	disp /= 2;
abort();
	buffer[highbyte] = 0xa0;	/* branch over move and disp */
	buffer[lowbyte] = 3;
	buffer[highbyte+2] = 0xd0 | JREG;	/* Build mov insn */
	buffer[lowbyte+2] = 0x00;

	buffer[highbyte+4] = 0;		/* space for 32 bit jump disp */
	buffer[lowbyte+4] = 0;
	buffer[highbyte+6] = 0;
	buffer[lowbyte+6] = 0;

	buffer[highbyte+8] = 0x40 | JREG;	/* Build jmp @JREG */
	buffer[lowbyte+8] = t ? 0xb : 0x2b;

	buffer[highbyte+10] = 0x20;	/* build nop */
	buffer[lowbyte+10] = 0x0b;

	/* Make reloc for the long disp */
	fix_new (fragP,
		 fragP->fr_fix + 4,
		 4,
		 fragP->fr_symbol,
		 fragP->fr_offset,
		 0,
		 R_SH_IMM32);
	fragP->fr_fix += UNCOND32_LENGTH;
	fragP->fr_var = 0;
	donerelax = 1;

      }
      break;

    case C (COND_JUMP, COND12):
      {
	/* A bcond won't fit, so turn it into a b!cond; bra disp; nop */
	int next_inst =
	fragP->fr_fix + fragP->fr_address + 6;

	int disp = targ_addr - next_inst;
	disp /= 2;
	md_number_to_chars (buffer + 2, disp & 0xfff, 2);
	buffer[highbyte] ^= 0x2;	/* Toggle T/F bit */
	buffer[lowbyte] = 1;		/* branch over jump and nop */
	buffer[highbyte+2] = (buffer[highbyte+2] & 0xf) | 0xa0;	/* Build jump insn */
	buffer[lowbyte+5] = 0x20;	/* Build nop */
	buffer[lowbyte+5] = 0x0b;
	fragP->fr_fix += 6;
	fragP->fr_var = 0;
	donerelax = 1;
      }
      break;

    case C (COND_JUMP, COND32):
    case C (COND_JUMP, UNDEF_WORD_DISP):
      {
	/* A bcond won't fit and it won't go into a 12 bit
	   displacement either, the code sequence looks like:
	   b!cond foop
	   mov.w @(n, PC), r14
	   jmp  @r14
	   nop
	   .long where
	   foop:
	 */

	int next_inst =
	fragP->fr_fix + fragP->fr_address + COND32_LENGTH;

	int disp = targ_addr - next_inst;
	disp /= 2;
abort();
	buffer[0] ^= 0x2;	/* Toggle T/F bit */
#define JREG 14
	buffer[1] = 5;		/* branch over mov, jump, nop and ptr */
	buffer[2] = 0xd0 | JREG;	/* Build mov insn */
	buffer[3] = 0x2;
	buffer[4] = 0x40 | JREG;	/* Build jmp @JREG */
	buffer[5] = 0x0b;
	buffer[6] = 0x20;	/* build nop */
	buffer[7] = 0x0b;
	buffer[8] = 0;		/* space for 32 bit jump disp */
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 0;
	buffer[12] = 0;
	buffer[13] = 0;
	/* Make reloc for the long disp */
	fix_new (fragP,
		 fragP->fr_fix + 8,
		 4,
		 fragP->fr_symbol,
		 fragP->fr_offset,
		 0,
		 R_SH_IMM32);
	fragP->fr_fix += COND32_LENGTH;
	fragP->fr_var = 0;
	donerelax = 1;
      }
      break;

    default:
      abort ();
    }

  if (donerelax && !relax)
    {
      as_warn ("Offset doesn't fit at 0x%lx, trying to get to %s+0x%x",
	       (unsigned long) fragP->fr_address,
	       fragP->fr_symbol  ?    S_GET_NAME(fragP->fr_symbol): "",
	       targ_addr);
    }

}

valueT
DEFUN (md_section_align, (seg, size),
       segT seg AND
       valueT size)
{
  return ((size + (1 << section_alignment[(int) seg]) - 1)
	  & (-1 << section_alignment[(int) seg]));

}

void
md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  int addr = fixP->fx_frag->fr_address + fixP->fx_where;
  int lowbyte = shl ? 0 : 1;
  if (fixP->fx_r_type == 0)
    {
      if (fixP->fx_size == 2)
	fixP->fx_r_type = R_SH_IMM16;
      else
	fixP->fx_r_type = R_SH_IMM32;
    }

  switch (fixP->fx_r_type)
    {

    case R_SH_IMM4:
      *buf = (*buf & 0xf0) | (val & 0xf);
      break;

    case R_SH_IMM4BY2:
      *buf = (*buf & 0xf0) | ((val >> 1) & 0xf);
      break;

    case R_SH_IMM4BY4:
      *buf = (*buf & 0xf0) | ((val >> 2) & 0xf);
      break;

    case R_SH_IMM8BY2:
      *buf = val >> 1;
      break;

    case R_SH_IMM8BY4:
      *buf = val >> 2;
      break;

    case R_SH_IMM8:
      *buf++ = val;
      break;

    case R_SH_PCRELIMM8BY4:
      addr &= ~1;
#if 0
      if (val & 0x3)
	as_warn ("non aligned displacement at %x\n", addr);
#endif
      /*      val -= (addr + 4); */
      if (shl||1)
	val += 1;
      else
	val += 3;
      val /= 4;
      if (val & ~0xff)
	as_warn_where (fixP->fx_file, fixP->fx_line, "pcrel too far");
      buf[lowbyte] = val;
      break;

    case R_SH_PCRELIMM8BY2:
      addr &= ~1;
      /*      if ((val & 0x1) != shl)
	      as_bad ("odd displacement at %x\n", addr);*/
      /*      val -= (addr + 4); */
/*      if (!shl)
	val++;*/
      val /= 2;
      if (val & ~0xff)
	as_warn_where (fixP->fx_file, fixP->fx_line, "pcrel too far");
      buf[lowbyte] = val;
      break;

    case R_SH_IMM32:
      if (shl) 
	{
	  *buf++ = val >> 0;
	  *buf++ = val >> 8;
	  *buf++ = val >> 16;
	  *buf++ = val >> 24;
	}
      else 
	{
	  *buf++ = val >> 24;
	  *buf++ = val >> 16;
	  *buf++ = val >> 8;
	  *buf++ = val >> 0;
	}
      break;

    case R_SH_IMM16:
      if (shl) 
	{
	  *buf++ = val >> 0;
	  *buf++ = val >> 8;
	} 
      else 
	{
	  *buf++ = val >> 8;
	  *buf++ = val >> 0;
	}
      break;

    default:
      abort ();
    }
}

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
    case C (UNCOND_JUMP, UNDEF_DISP):
      /* used to be a branch to somewhere which was unknown */
      if (!fragP->fr_symbol)
	{
	  fragP->fr_subtype = C (UNCOND_JUMP, UNCOND12);
	  fragP->fr_var = md_relax_table[C (UNCOND_JUMP, UNCOND12)].rlx_length;
	}
      else if (S_GET_SEGMENT (fragP->fr_symbol) == segment_type)
	{
	  fragP->fr_subtype = C (UNCOND_JUMP, UNCOND12);
	  fragP->fr_var = md_relax_table[C (UNCOND_JUMP, UNCOND12)].rlx_length;
	}
      else
	{
	  fragP->fr_subtype = C (UNCOND_JUMP, UNDEF_WORD_DISP);
	  fragP->fr_var = md_relax_table[C (UNCOND_JUMP, UNCOND32)].rlx_length;
	  return md_relax_table[C (UNCOND_JUMP, UNCOND32)].rlx_length;
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
	  fragP->fr_subtype = C (COND_JUMP, COND8);
	  fragP->fr_var = md_relax_table[C (COND_JUMP, COND8)].rlx_length;
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
	  fragP->fr_subtype = C (COND_JUMP, COND8);
	  fragP->fr_var = md_relax_table[C (COND_JUMP, COND8)].rlx_length;
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
  if (shl)
    number_to_chars_littleendian (ptr, use, nbytes);
  else
    number_to_chars_bigendian (ptr, use, nbytes);
}

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  int gap = fixP->fx_size + fixP->fx_where +  fixP->fx_frag->fr_address - 1 ;
  return gap;
}

short
tc_coff_fix2rtype (fix_ptr)
     fixS *fix_ptr;
{
  if (fix_ptr->fx_r_type == RELOC_32)
    {
      /* cons likes to create reloc32's whatever the size of the reloc..
       */
      switch (fix_ptr->fx_size)
	{
	case 2:
	  return R_SH_IMM16;
	  break;
	case 1:
	  return R_SH_IMM8;
	  break;
	default:
	  abort ();
	}
    }
  return R_SH_IMM32;
}

int
tc_coff_sizemachdep (frag)
     fragS *frag;
{
  return md_relax_table[frag->fr_subtype].rlx_length;
}
