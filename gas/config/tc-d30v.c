/* tc-d30v.c -- Assembler code for the Mitsubishi D30V

   Copyright (C) 1997, 1998 Free Software Foundation.

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
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"     
#include "opcode/d30v.h"

const char comment_chars[] = ";";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = "";
const char *md_shortopts = "OnN";
const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";

#define NOP_MULTIPLY 1
#define NOP_ALL 2
static int warn_nops = 0;
static int Optimizing = 0;

#define FORCE_SHORT	1
#define FORCE_LONG	2

/* EXEC types.  */
typedef enum _exec_type
{
  EXEC_UNKNOWN,			/* no order specified */
  EXEC_PARALLEL,		/* done in parallel (FM=00) */
  EXEC_SEQ,			/* sequential (FM=01) */
  EXEC_REVSEQ			/* reverse sequential (FM=10) */
} exec_type_enum;

/* fixups */
#define MAX_INSN_FIXUPS (5)
struct d30v_fixup
{
  expressionS exp;
  int operand;
  int pcrel;
  int size;
  bfd_reloc_code_real_type reloc;
};

typedef struct _fixups
{
  int fc;
  struct d30v_fixup fix[MAX_INSN_FIXUPS];
  struct _fixups *next;
} Fixups;

static Fixups FixUps[2];
static Fixups *fixups;

/* Whether current and previous instruction is a word multiply.  */
int cur_mul32_p = 0;
int prev_mul32_p = 0;

/*  The flag_explicitly_parallel is true iff the instruction being assembled
    has been explicitly written as a parallel short-instruction pair by the
    human programmer.  It is used in parallel_ok() to distinguish between
    those dangerous parallelizations attempted by the human, which are to be
    allowed, and those attempted by the assembler, which are not.  It is set
    from md_assemble().  */
static int flag_explicitly_parallel = 0; 
static int flag_xp_state = 0;

/* Two nops */
#define NOP_LEFT ((long long)NOP << 32)
#define NOP_RIGHT ((long long)NOP)
#define NOP2 (FM00 | NOP_LEFT | NOP_RIGHT)

/* local functions */
static int reg_name_search PARAMS ((char *name));
static int register_name PARAMS ((expressionS *expressionP));
static int check_range PARAMS ((unsigned long num, int bits, int flags));
static int postfix PARAMS ((char *p));
static bfd_reloc_code_real_type get_reloc PARAMS ((struct d30v_operand *op, int rel_flag));
static int get_operands PARAMS ((expressionS exp[], int cmp_hack));
static struct d30v_format *find_format PARAMS ((struct d30v_opcode *opcode, 
			expressionS ops[],int fsize, int cmp_hack));
static long long build_insn PARAMS ((struct d30v_insn *opcode, expressionS *opers));
static void write_long PARAMS ((struct d30v_insn *opcode, long long insn, Fixups *fx));
static void write_1_short PARAMS ((struct d30v_insn *opcode, long long insn, Fixups *fx));
static int write_2_short PARAMS ((struct d30v_insn *opcode1, long long insn1, 
		   struct d30v_insn *opcode2, long long insn2, exec_type_enum exec_type, Fixups *fx));
static long long do_assemble PARAMS ((char *str, struct d30v_insn *opcode));
static int parallel_ok PARAMS ((struct d30v_insn *opcode1, unsigned long insn1, 
				struct d30v_insn *opcode2, unsigned long insn2,
				exec_type_enum exec_type));
static void d30v_number_to_chars PARAMS ((char *buf, long long value, int nbytes));
static void check_size PARAMS ((long value, int bits, char *file, int line));

struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);       


/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { "word", cons, 4 },
  { "hword", cons, 2 },
  { NULL, NULL, 0 }
};

/* Opcode hash table.  */
static struct hash_control *d30v_hash;

/* reg_name_search does a binary search of the pre_defined_registers
   array to see if "name" is a valid regiter name.  Returns the register
   number from the array on success, or -1 on failure. */

static int
reg_name_search (name)
     char *name;
{
  int middle, low, high;
  int cmp;

  low = 0;
  high = reg_name_cnt() - 1;

  do
    {
      middle = (low + high) / 2;
      cmp = strcasecmp (name, pre_defined_registers[middle].name);
      if (cmp < 0)
	high = middle - 1;
      else if (cmp > 0)
	low = middle + 1;
      else 
	  return pre_defined_registers[middle].value;
    }
  while (low <= high);
  return -1;
}

/* register_name() checks the string at input_line_pointer
   to see if it is a valid register name */

static int
register_name (expressionP)
     expressionS *expressionP;
{
  int reg_number;
  char c, *p = input_line_pointer;
  
  while (*p && *p!='\n' && *p!='\r' && *p !=',' && *p!=' ' && *p!=')')
    p++;

  c = *p;
  if (c)
    *p++ = 0;

  /* look to see if it's in the register table */
  reg_number = reg_name_search (input_line_pointer);
  if (reg_number >= 0) 
    {
      expressionP->X_op = O_register;
      /* temporarily store a pointer to the string here */
      expressionP->X_op_symbol = (struct symbol *)input_line_pointer;
      expressionP->X_add_number = reg_number;
      input_line_pointer = p;
      return 1;
    }
  if (c)
    *(p-1) = c;
  return 0;
}


static int
check_range (num, bits, flags)
     unsigned long num;
     int bits;
     int flags;
{
  long min, max;
  int retval=0;

  /* don't bother checking 32-bit values */
  if (bits == 32)
    return 0;

  if (flags & OPERAND_SIGNED)
    {
      max = (1 << (bits - 1))-1; 
      min = - (1 << (bits - 1));
      if (((long)num > max) || ((long)num < min))
	retval = 1;
    }
  else
    {
      max = (1 << bits) - 1;
      min = 0;
      if ((num > max) || (num < min))
	retval = 1;
    }
  return retval;
}


void
md_show_usage (stream)
  FILE *stream;
{
  fprintf(stream, (_"\nD30V options:\n\
-O                      Make adjacent short instructions parallel if possible.\n\
-n                      Warn about all NOPs inserted by the assembler.\n\
-N			Warn about NOPs inserted after word multiplies.\n"));
} 

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
      /* Optimize. Will attempt to parallelize operations */
    case 'O':
      Optimizing = 1;
      break;

      /* Warn about all NOPS that the assembler inserts.  */
    case 'n':
      warn_nops = NOP_ALL;
      break;

      /* Warn about the NOPS that the assembler inserts because of the
	 multiply hazard.  */
    case 'N':
      warn_nops = NOP_MULTIPLY;
      break;

    default:
      return 0;
    }
  return 1;
}

symbolS *
md_undefined_symbol (name)
  char *name;
{
  return 0;
}

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
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;
  
  *sizeP = prec * 2;
  
  for (i = 0; i < prec; i++)
    {
      md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
    }
  return NULL;
}

void
md_convert_frag (abfd, sec, fragP)
  bfd *abfd;
  asection *sec;
  fragS *fragP;
{
  abort ();
}

valueT
md_section_align (seg, addr)
     asection *seg;
     valueT addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}


void
md_begin ()
{
  struct d30v_opcode *opcode;
  d30v_hash = hash_new();

  /* Insert opcode names into a hash table. */
  for (opcode = (struct d30v_opcode *)d30v_opcode_table; opcode->name; opcode++)
      hash_insert (d30v_hash, opcode->name, (char *) opcode);

  fixups = &FixUps[0];
  FixUps[0].next = &FixUps[1];
  FixUps[1].next = &FixUps[0];
}


/* this function removes the postincrement or postdecrement
   operator ( '+' or '-' ) from an expression */

static int postfix (p) 
     char *p;
{
  while (*p != '-' && *p != '+') 
    {
      if (*p==0 || *p=='\n' || *p=='\r' || *p==' ' || *p==',') 
	break;
      p++;
    }

  if (*p == '-') 
    {
      *p = ' ';
      return (-1);
    }
  if (*p == '+') 
    {
      *p = ' ';
      return (1);
    }

  return (0);
}


static bfd_reloc_code_real_type 
get_reloc (op, rel_flag) 
     struct d30v_operand *op;
     int rel_flag;
{
  switch (op->bits)
    {
    case 6:
      if (op->flags & OPERAND_SHIFT)
	return BFD_RELOC_D30V_9_PCREL;
      else
	return BFD_RELOC_D30V_6;
      break;
    case 12:
      if (!(op->flags & OPERAND_SHIFT))
	as_warn(_("unexpected 12-bit reloc type"));
      if (rel_flag == RELOC_PCREL)
	return BFD_RELOC_D30V_15_PCREL;
      else
	return BFD_RELOC_D30V_15;
    case 18:
      if (!(op->flags & OPERAND_SHIFT))
	as_warn(_("unexpected 18-bit reloc type"));
      if (rel_flag == RELOC_PCREL)
	return BFD_RELOC_D30V_21_PCREL;
      else
	return BFD_RELOC_D30V_21;
    case 32:
      if (rel_flag == RELOC_PCREL)
	return BFD_RELOC_D30V_32_PCREL;
      else
	return BFD_RELOC_D30V_32;
    default:
      return 0;
    }
}

/* get_operands parses a string of operands and returns
   an array of expressions */

static int
get_operands (exp, cmp_hack) 
     expressionS exp[];
     int cmp_hack;
{
  char *p = input_line_pointer;
  int numops = 0;
  int post = 0;

  if (cmp_hack)
    {
      exp[numops].X_op = O_absent;
      exp[numops++].X_add_number = cmp_hack - 1;
    }

  while (*p)  
    {
      while (*p == ' ' || *p == '\t' || *p == ',') 
	p++;
      if (*p==0 || *p=='\n' || *p=='\r') 
	break;
      
      if (*p == '@') 
	{
	  p++;
	  exp[numops].X_op = O_absent;
	  if (*p == '(') 
	    {
	      p++;
	      exp[numops].X_add_number = OPERAND_ATPAR;
	      post = postfix (p);
	    }
	  else if (*p == '-') 
	    {
	      p++;
	      exp[numops].X_add_number = OPERAND_ATMINUS;
	    }
	  else
	    {
	      exp[numops].X_add_number = OPERAND_ATSIGN;
	      post = postfix (p);
	    }
	  numops++;
	  continue;
	}

      if (*p == ')') 
	{
	  /* just skip the trailing paren */
	  p++;
	  continue;
	}

      input_line_pointer = p;

      /* check to see if it might be a register name */
      if (!register_name (&exp[numops]))
	{
	  /* parse as an expression */
	  expression (&exp[numops]);
	}

      if (exp[numops].X_op == O_illegal) 
	as_bad (_("illegal operand"));
      else if (exp[numops].X_op == O_absent) 
	as_bad (_("missing operand"));

      numops++;
      p = input_line_pointer;

      switch (post) 
	{
	case -1:	/* postdecrement mode */
	  exp[numops].X_op = O_absent;
	  exp[numops++].X_add_number = OPERAND_MINUS;
	  break;
	case 1:	/* postincrement mode */
	  exp[numops].X_op = O_absent;
	  exp[numops++].X_add_number = OPERAND_PLUS;
	  break;
	}
      post = 0;
    }

  exp[numops].X_op = 0;
  return (numops);
}

/* build_insn generates the instruction.  It does everything */
/* but write the FM bits. */

static long long
build_insn (opcode, opers) 
     struct d30v_insn *opcode;
     expressionS *opers;
{
  int i, length, bits, shift, flags;
  unsigned int number, id=0;
  long long insn;
  struct d30v_opcode *op = opcode->op;
  struct d30v_format *form = opcode->form;

  insn = opcode->ecc << 28 | op->op1 << 25 | op->op2 << 20 | form->modifier << 18;

  for (i=0; form->operands[i]; i++) 
    { 
      flags = d30v_operand_table[form->operands[i]].flags;

      /* must be a register or number */
      if (!(flags & OPERAND_REG) && !(flags & OPERAND_NUM) && 
	  !(flags & OPERAND_NAME) && !(flags & OPERAND_SPECIAL))
	continue;

      bits = d30v_operand_table[form->operands[i]].bits;
      if (flags & OPERAND_SHIFT)
	bits += 3;

      length = d30v_operand_table[form->operands[i]].length;
      shift = 12 - d30v_operand_table[form->operands[i]].position;
      if (opers[i].X_op != O_symbol)
	number = opers[i].X_add_number;
      else
	number = 0;
      if (flags & OPERAND_REG)
	{
	  /* check for mvfsys or mvtsys control registers */
	  if (flags & OPERAND_CONTROL && (number & 0x7f) > MAX_CONTROL_REG)
	    {
	      /* PSWL or PSWH */
	      id = (number & 0x7f) - MAX_CONTROL_REG;
	      number = 0;
	    }
	  else if (number & OPERAND_FLAG)
	    {
	      id = 3;  /* number is a flag register */
	    }
	  number &= 0x7F;
	}
      else if (flags & OPERAND_SPECIAL)
	{
	  number = id;
	}

      if (opers[i].X_op != O_register && opers[i].X_op != O_constant && !(flags & OPERAND_NAME))
	{
	  /* now create a fixup */

	  if (fixups->fc >= MAX_INSN_FIXUPS)
	    as_fatal (_("too many fixups"));

	  fixups->fix[fixups->fc].reloc = 
	    get_reloc((struct d30v_operand *)&d30v_operand_table[form->operands[i]], op->reloc_flag);
	  fixups->fix[fixups->fc].size = 4;
	  fixups->fix[fixups->fc].exp = opers[i];
	  fixups->fix[fixups->fc].operand = form->operands[i];
	  if (fixups->fix[fixups->fc].reloc == BFD_RELOC_D30V_9_PCREL)
	    fixups->fix[fixups->fc].pcrel = RELOC_PCREL;
	  else
	    fixups->fix[fixups->fc].pcrel = op->reloc_flag;
	  (fixups->fc)++;
	}

      /* truncate to the proper number of bits */
      if ((opers[i].X_op == O_constant) && check_range (number, bits, flags))
	as_bad(_("operand out of range: %d"),number);
      if (bits < 31)
	number &= 0x7FFFFFFF >> (31 - bits);
      if (flags & OPERAND_SHIFT)
	number >>= 3;
      if (bits == 32)
	{
	  /* it's a LONG instruction */
	  insn |= (number >> 26);	/* top 6 bits */
	  insn <<= 32;			/* shift the first word over */
	  insn |= ((number & 0x03FC0000) << 2);  /* next 8 bits */ 
	  insn |= number & 0x0003FFFF;		/* bottom 18 bits */
	}
      else
	insn |= number << shift;
    }
  return insn;
}


/* write out a long form instruction */
static void
write_long (opcode, insn, fx) 
     struct d30v_insn *opcode;
     long long insn;
     Fixups *fx;
{
  int i, where;
  char *f = frag_more(8);

  insn |= FM11;
  d30v_number_to_chars (f, insn, 8);

  for (i=0; i < fx->fc; i++) 
    {
      if (fx->fix[i].reloc)
	{ 
	  where = f - frag_now->fr_literal; 
	  fix_new_exp (frag_now,
		       where,
		       fx->fix[i].size,
		       &(fx->fix[i].exp),
		       fx->fix[i].pcrel,
		       fx->fix[i].reloc);
	}
    }
  fx->fc = 0;
}


/* write out a short form instruction by itself */
static void
write_1_short (opcode, insn, fx) 
     struct d30v_insn *opcode;
     long long insn;
     Fixups *fx;
{
  char *f = frag_more(8);
  int i, where;

  if (warn_nops == NOP_ALL)
    as_warn (_("NOP inserted"));

  /* the other container needs to be NOP */
  /* according to 4.3.1: for FM=00, sub-instructions performed only
     by IU cannot be encoded in L-container. */
  if (opcode->op->unit == IU)
    insn |= FM00 | NOP_LEFT;			/* right container */
  else
    insn = FM00 | (insn << 32) | NOP_RIGHT;	/* left container */

  d30v_number_to_chars (f, insn, 8);

  for (i=0; i < fx->fc; i++) 
    {
      if (fx->fix[i].reloc)
	{ 
	  where = f - frag_now->fr_literal; 
	  fix_new_exp (frag_now,
		       where, 
		       fx->fix[i].size,
		       &(fx->fix[i].exp),
		       fx->fix[i].pcrel,
		       fx->fix[i].reloc);
	}
    }
  fx->fc = 0;
}

/* write out a short form instruction if possible */
/* return number of instructions not written out */
static int
write_2_short (opcode1, insn1, opcode2, insn2, exec_type, fx) 
     struct d30v_insn *opcode1, *opcode2;
     long long insn1, insn2;
     exec_type_enum exec_type;
     Fixups *fx;
{
  long long insn = NOP2;
  char *f;
  int i,j, where;

  if(exec_type != EXEC_PARALLEL &&
     ((opcode1->op->flags_used & (FLAG_JSR | FLAG_DELAY)) == FLAG_JSR))
    {
      /* subroutines must be called from 32-bit boundaries */
      /* so the return address will be correct */
      write_1_short (opcode1, insn1, fx->next);
      return (1);
    }

  switch (exec_type) 
    {
    case EXEC_UNKNOWN:	/* order not specified */
      if (Optimizing && parallel_ok (opcode1, insn1, opcode2, insn2, exec_type))
	{
	  /* parallel */
	  exec_type = EXEC_PARALLEL;
	  if (opcode1->op->unit == IU)
	    insn = FM00 | (insn2 << 32) | insn1;
	  else if (opcode2->op->unit == MU)
	    insn = FM00 | (insn2 << 32) | insn1;
	  else
	    {
	      insn = FM00 | (insn1 << 32) | insn2;  
	      fx = fx->next;
	    }
	}
      else if (opcode1->op->unit == IU)
	{
	  /* reverse sequential */
	  insn = FM10 | (insn2 << 32) | insn1;
	  exec_type = EXEC_REVSEQ;
	}
      else
	{
	  /* sequential */
	  insn = FM01 | (insn1 << 32) | insn2;
	  fx = fx->next;
	  exec_type = EXEC_SEQ;
	}
      break;

    case EXEC_PARALLEL:	/* parallel */
      flag_explicitly_parallel = flag_xp_state;
      if (! parallel_ok (opcode1, insn1, opcode2, insn2, exec_type))
	as_fatal (_("Instructions may not be executed in parallel"));
      else if (opcode1->op->unit == IU)
	{
	  if (opcode2->op->unit == IU)
	    as_fatal (_("Two IU instructions may not be executed in parallel"));
	  as_warn (_("Swapping instruction order"));
 	  insn = FM00 | (insn2 << 32) | insn1;
	}
      else if (opcode2->op->unit == MU)
	{
	  if (opcode1->op->unit == MU)
	    as_fatal (_("Two MU instructions may not be executed in parallel"));
	  as_warn (_("Swapping instruction order"));
	  insn = FM00 | (insn2 << 32) | insn1;
	}
      else
	{
	  insn = FM00 | (insn1 << 32) | insn2;  
	  fx = fx->next;
	}
      flag_explicitly_parallel = 0;
      break;

    case EXEC_SEQ:	/* sequential */
      if (opcode1->op->unit == IU)
	as_fatal (_("IU instruction may not be in the left container"));
      insn = FM01 | (insn1 << 32) | insn2;  
      fx = fx->next;
      break;

    case EXEC_REVSEQ:	/* reverse sequential */
      if (opcode2->op->unit == MU)
	as_fatal (_("MU instruction may not be in the right container"));
      insn = FM10 | (insn1 << 32) | insn2;  
      fx = fx->next;
      break;

    default:
      as_fatal(_("unknown execution type passed to write_2_short()"));
    }

  /*  printf("writing out %llx\n",insn); */
  f = frag_more(8);
  d30v_number_to_chars (f, insn, 8);

  /* If the previous instruction was a 32-bit multiply but it is put into a
     parallel container, mark the current instruction as being a 32-bit
     multiply.  */
  if (prev_mul32_p && exec_type == EXEC_PARALLEL)
    cur_mul32_p = 1;

  for (j=0; j<2; j++) 
    {
      for (i=0; i < fx->fc; i++) 
	{
	  if (fx->fix[i].reloc)
	    {
	      where = (f - frag_now->fr_literal) + 4*j;

	      fix_new_exp (frag_now,
			   where, 
			   fx->fix[i].size,
			   &(fx->fix[i].exp),
			   fx->fix[i].pcrel,
			   fx->fix[i].reloc);
	    }
	}
      fx->fc = 0;
      fx = fx->next;
    }
  return (0);
}


/* Check 2 instructions and determine if they can be safely */
/* executed in parallel.  Returns 1 if they can be.         */
static int
parallel_ok (op1, insn1, op2, insn2, exec_type)
     struct d30v_insn *op1, *op2;
     unsigned long insn1, insn2;
     exec_type_enum exec_type;
{
  int i, j, shift, regno, bits, ecc;
  unsigned long flags, mask, flags_set1, flags_set2, flags_used1, flags_used2;
  unsigned long ins, mod_reg[2][3], used_reg[2][3], flag_reg[2];
  struct d30v_format *f;
  struct d30v_opcode *op;

  /* section 4.3: both instructions must not be IU or MU only */
  if ((op1->op->unit == IU && op2->op->unit == IU)
      || (op1->op->unit == MU && op2->op->unit == MU))
    return 0;

  /* first instruction must not be a jump to safely optimize, unless this
     is an explicit parallel operation.  */
  if (exec_type != EXEC_PARALLEL
      && (op1->op->flags_used & (FLAG_JMP | FLAG_JSR)))
    return 0;

  /* If one instruction is /TX or /XT and the other is /FX or /XF respectively,
     then it is safe to allow the two to be done as parallel ops, since only
     one will ever be executed at a time.  */
  if ((op1->ecc == ECC_TX && op2->ecc == ECC_FX)
      || (op1->ecc == ECC_FX && op2->ecc == ECC_TX)
      || (op1->ecc == ECC_XT && op2->ecc == ECC_XF)
      || (op1->ecc == ECC_XF && op2->ecc == ECC_XT))
     return 1;

  /* [0] r0-r31
     [1] r32-r63
     [2] a0, a1, flag registers */

  for (j = 0; j < 2; j++)
    {
      if (j == 0)
	{
	  f = op1->form;
	  op = op1->op;
	  ecc = op1->ecc;
	  ins = insn1;
	}
      else
	{
	  f = op2->form;
	  op = op2->op;
	  ecc = op2->ecc;
	  ins = insn2;
	}
      flag_reg[j] = 0;
      mod_reg[j][0] = mod_reg[j][1] = 0;
      mod_reg[j][2] = (op->flags_set & FLAG_ALL);
      used_reg[j][0] = used_reg[j][1] = 0;
      used_reg[j][2] = (op->flags_used & FLAG_ALL);

      /* BSR/JSR always sets R62 */
      if (op->flags_used & FLAG_JSR)
	mod_reg[j][1] = (1L << (62-32));

      /* conditional execution affects the flags_used */
      switch (ecc)
	{
	case ECC_TX:
	case ECC_FX:
	  used_reg[j][2] |= flag_reg[j] = FLAG_0;
	  break;

	case ECC_XT:
	case ECC_XF:
	  used_reg[j][2] |= flag_reg[j] = FLAG_1;
	  break;

	case ECC_TT:
	case ECC_TF:
	  used_reg[j][2] |= flag_reg[j] = (FLAG_0 | FLAG_1);
	  break;
	}

      for (i = 0; f->operands[i]; i++)
	{
	  flags = d30v_operand_table[f->operands[i]].flags;
	  shift = 12 - d30v_operand_table[f->operands[i]].position;
	  bits = d30v_operand_table[f->operands[i]].bits;
	  if (bits == 32)
	    mask = 0xffffffff;
	  else
	    mask = 0x7FFFFFFF >> (31 - bits);

	  if ((flags & OPERAND_PLUS) || (flags & OPERAND_MINUS))
	    {
	      /* this is a post-increment or post-decrement */
	      /* the previous register needs to be marked as modified */

	      shift = 12 - d30v_operand_table[f->operands[i-1]].position;
	      regno = (ins >> shift) & 0x3f;
	      if (regno >= 32)
		mod_reg[j][1] |= 1L << (regno - 32);
	      else
		mod_reg[j][0] |= 1L << regno;
	    }
	  else if (flags & OPERAND_REG)
	    {
	      regno = (ins >> shift) & mask;
	      /* the memory write functions don't have a destination register */
	      if ((flags & OPERAND_DEST) && !(op->flags_set & FLAG_MEM))
		{
		  /* MODIFIED registers and flags */
		  if (flags & OPERAND_ACC)
		    {
		      if (regno == 0)
			mod_reg[j][2] |= FLAG_A0;
		      else if (regno == 1)
			mod_reg[j][2] |= FLAG_A1;
		      else
			abort ();
		    }
		  else if (flags & OPERAND_FLAG)
		    mod_reg[j][2] |= 1L << regno;
		  else if (!(flags & OPERAND_CONTROL))
		    {
		      int r, z;

		      /* need to check if there are two destination */
		      /* registers, for example ld2w */
		      if (flags & OPERAND_2REG)
			z = 1;
		      else
			z = 0;

		      for (r = regno; r <= regno + z; r++)
			{ 
			  if (r >= 32)
			    mod_reg[j][1] |= 1L << (r - 32);
			  else
			    mod_reg[j][0] |= 1L << r;
			}
		    }
		}
	      else
		{
		  /* USED, but not modified registers and flags */
		  if (flags & OPERAND_ACC)
		    {
		      if (regno == 0)
			used_reg[j][2] |= FLAG_A0;
		      else if (regno == 1)
			used_reg[j][2] |= FLAG_A1;
		      else
			abort ();
		    }
		  else if (flags & OPERAND_FLAG)
		    used_reg[j][2] |= 1L << regno;
		  else if (!(flags & OPERAND_CONTROL))
		    {
		      int r, z;

		      /* need to check if there are two source */
		      /* registers, for example st2w */
		      if (flags & OPERAND_2REG)
			z = 1;
		      else
			z = 0;

		      for (r = regno; r <= regno + z; r++)
			{ 
			  if (r >= 32)
			    used_reg[j][1] |= 1L << (r - 32);
			  else
			    used_reg[j][0] |= 1L << r;
			}
		    }
		}
	    }
	}
    }

  flags_set1 = op1->op->flags_set;
  flags_set2 = op2->op->flags_set;
  flags_used1 = op1->op->flags_used;
  flags_used2 = op2->op->flags_used;

  /* ST2W/ST4HB combined with ADDppp/SUBppp is illegal.  */
  if (((flags_set1 & (FLAG_MEM | FLAG_2WORD)) == (FLAG_MEM | FLAG_2WORD)
       && (flags_used2 & FLAG_ADDSUBppp) != 0)
      || ((flags_set2 & (FLAG_MEM | FLAG_2WORD)) == (FLAG_MEM | FLAG_2WORD)
	  && (flags_used1 & FLAG_ADDSUBppp) != 0))
    return 0;

  /* Load instruction combined with half-word multiply is illegal.  */
  if (((flags_used1 & FLAG_MEM) != 0 && (flags_used2 & FLAG_MUL16))
      || ((flags_used2 & FLAG_MEM) != 0 && (flags_used1 & FLAG_MUL16)))
    return 0;

  /* Specifically allow add || add by removing carry, overflow bits dependency.
     This is safe, even if an addc follows since the IU takes the argument in
     the right container, and it writes its results last.
     However, don't paralellize add followed by addc or sub followed by
     subb.  */

  if (mod_reg[0][2] == FLAG_CVVA && mod_reg[1][2] == FLAG_CVVA
      && (used_reg[0][2] & ~flag_reg[0]) == 0
      && (used_reg[1][2] & ~flag_reg[1]) == 0
      && op1->op->unit == EITHER && op2->op->unit == EITHER)
    {
      mod_reg[0][2] = mod_reg[1][2] = 0;
    }

  for(j = 0; j < 3; j++)
    {
      /* If the second instruction depends on the first, we obviously
	 cannot parallelize.  Note, the mod flag implies use, so
	 check that as well.  */
      /* If flag_explicitly_parallel is set, then the case of the 
	 second instruction using a register the first instruction
	 modifies is assumed to be okay; we trust the human.  We
	 don't trust the human if both instructions modify the same
	 register but we do trust the human if they modify the same
	 flags. */
      if (flag_explicitly_parallel)
	{
	  if ((j < 2) && (mod_reg[0][j] & mod_reg[1][j]) != 0)
	    return 0;
	}
      else
        if ((mod_reg[0][j] & (mod_reg[1][j] | used_reg[1][j])) != 0)
	  return 0;
    }

  return 1;
}



/* This is the main entry point for the machine-dependent assembler.  str points to a
   machine-dependent instruction.  This function is supposed to emit the frags/bytes 
   it assembles to.  For the D30V, it mostly handles the special VLIW parsing and packing
   and leaves the difficult stuff to do_assemble().  */

static long long prev_insn = -1;
static struct d30v_insn prev_opcode;
static subsegT prev_subseg;
static segT prev_seg = 0;

void
md_assemble (str)
     char *str;
{
  struct d30v_insn opcode;
  long long insn;
  exec_type_enum extype = EXEC_UNKNOWN;		/* execution type; parallel, etc */
  static exec_type_enum etype = EXEC_UNKNOWN;	/* saved extype.  used for multiline instructions */
  char *str2;

  if ( (prev_insn != -1) && prev_seg && ((prev_seg != now_seg) || (prev_subseg != now_subseg)))
    d30v_cleanup();

  flag_explicitly_parallel = 0;
  flag_xp_state = 0;
  if (etype == EXEC_UNKNOWN)
    {
      /* look for the special multiple instruction separators */
      str2 = strstr (str, "||");
      if (str2) 
	{
	  extype = EXEC_PARALLEL;
	  flag_xp_state = 1;
	}
      else
	{
	  str2 = strstr (str, "->");
	  if (str2) 
	    extype = EXEC_SEQ;
	  else
	    {
	      str2 = strstr (str, "<-");
	      if (str2) 
		extype = EXEC_REVSEQ;
	    }
	}
      /* str2 points to the separator, if one */
      if (str2) 
	{
	  *str2 = 0;
	  
	  /* if two instructions are present and we already have one saved
	     then first write it out */
	  d30v_cleanup();
	  
	  /* assemble first instruction and save it */
	  prev_insn = do_assemble (str, &prev_opcode);
	  if (prev_insn == -1)
	    as_fatal (_("cannot assemble instruction "));
	  if (prev_opcode.form->form >= LONG)
	    as_fatal (_("First opcode is long.  Unable to mix instructions as specified.")); 
	  fixups = fixups->next;
	  str = str2 + 2;
	}
    }

  insn = do_assemble (str, &opcode);
  if (insn == -1)
    {
      if (extype)
	{
	  etype = extype;
	  return;
	}
      as_fatal (_("cannot assemble instruction "));
    }

  if (etype)
    {
      extype = etype;
      etype = 0;
    }

  /* Word multiply instructions must not be followed by either a load or a
     16-bit multiply instruction in the next cycle.  */
  if (prev_mul32_p && (opcode.op->flags_used & (FLAG_MEM | FLAG_MUL16)))
    {
      /* However, load and multiply should able to be combined in a parallel
	 operation, so check for that first.  */

      if (prev_insn != -1
	  && (opcode.op->flags_used & FLAG_MEM)
	  && opcode.form->form < LONG
	  && (extype == EXEC_PARALLEL || (Optimizing && extype == EXEC_UNKNOWN))
	  && parallel_ok (&prev_opcode, (long)prev_insn,
			  &opcode, (long)insn, extype)
	  && write_2_short (&prev_opcode, (long)prev_insn,
			    &opcode, (long)insn, extype, fixups) == 0)
	{
	  /* no instructions saved */
	  prev_insn = -1;
	  return;
	}

      /* Can't parallelize, flush current instruction and emit a word of NOPS */
      else
	{
	  char *f;
	  d30v_cleanup();

	  f = frag_more(8);
	  d30v_number_to_chars (f, NOP2, 8);
	  if (warn_nops == NOP_ALL || warn_nops == NOP_MULTIPLY)
	    {
	      if ((opcode.op->flags_used & FLAG_MEM))
		as_warn (_("word of NOPs added between word multiply and load"));
	      else
		as_warn (_("word of NOPs added between word multiply and 16-bit multiply"));
	    }
	}
    }

  /* if this is a long instruction, write it and any previous short instruction */
  if (opcode.form->form >= LONG) 
    {
      if (extype) 
	as_fatal(_("Unable to mix instructions as specified"));
      d30v_cleanup();
      write_long (&opcode, insn, fixups);
      prev_insn = -1;
      return;
    }

  if ((prev_insn != -1) && 
      (write_2_short (&prev_opcode, (long)prev_insn, &opcode, (long)insn, extype, fixups) == 0)) 
    {
      /* no instructions saved */
      prev_insn = -1;
    }

  else
    {
      if (extype) 
	as_fatal(_("Unable to mix instructions as specified"));
      /* save off last instruction so it may be packed on next pass */
      memcpy(&prev_opcode, &opcode, sizeof(prev_opcode));
      prev_insn = insn;
      prev_seg = now_seg;
      prev_subseg = now_subseg;
      fixups = fixups->next;
    }
}


/* do_assemble assembles a single instruction and returns an opcode */
/* it returns -1 (an invalid opcode) on error */

static long long
do_assemble (str, opcode) 
     char *str;
     struct d30v_insn *opcode;
{
  unsigned char *op_start, *save;
  unsigned char *op_end;
  char name[20];
  int cmp_hack, nlen = 0, fsize = 0;
  expressionS myops[6];
  long long insn;

  /* Drop leading whitespace */
  while (*str == ' ')
    str++;

  /* find the opcode end */
  for (op_start = op_end = (unsigned char *) (str);
       *op_end
       && nlen < 20
       && *op_end != '/'
       && !is_end_of_line[*op_end] && *op_end != ' ';
       op_end++)
    {
      name[nlen] = tolower(op_start[nlen]);
      nlen++;
    }

  if (nlen == 0)
    return (-1);

  name[nlen] = 0;

  /* if there is an execution condition code, handle it */
  if (*op_end == '/')
    {
      int i = 0;
      while ( (i < ECC_MAX) && strncasecmp(d30v_ecc_names[i],op_end+1,2))
	i++;
      
      if (i == ECC_MAX)
	{
	  char tmp[4];
	  strncpy(tmp,op_end+1,2);
	  tmp[2] = 0;
	  as_fatal (_("unknown condition code: %s"),tmp);
	  return -1;
	}
      /*      printf("condition code=%d\n",i); */
      opcode->ecc = i;
      op_end += 3;
    }
  else
    opcode->ecc = ECC_AL;
  

  /* CMP and CMPU change their name based on condition codes */
  if (!strncmp(name,"cmp",3))
    {
      int p,i;
      char **str = (char **)d30v_cc_names;
      if (name[3] == 'u')
	p = 4;
      else
	p = 3;

      for(i=1; *str && strncmp(*str,&name[p],2); i++, str++)
	;

      /* cmpu only supports some condition codes */
      if (p == 4)
	{
	  if (i < 3 || i > 6)
	    {
	      name[p+2]=0;
	      as_fatal (_("cmpu doesn't support condition code %s"),&name[p]);      
	    }
	}

      if (!*str)
	{
	  name[p+2]=0;
	  as_fatal (_("unknown condition code: %s"),&name[p]);      
	}
      
      cmp_hack = i;
      name[p] = 0;
    }
  else
    cmp_hack = 0;
  
  /*  printf("cmp_hack=%d\n",cmp_hack); */

  /* need to look for .s or .l */
  if (name[nlen-2] == '.')
    {
      switch (name[nlen-1])
	{
	case 's':
	  fsize = FORCE_SHORT;
	  break;
	case 'l':
	  fsize = FORCE_LONG;
	  break;
	}
      name[nlen-2] = 0;
    }

  /* find the first opcode with the proper name */  
  opcode->op = (struct d30v_opcode *)hash_find (d30v_hash, name);
  if (opcode->op == NULL)
      as_fatal (_("unknown opcode: %s"),name);

  save = input_line_pointer;
  input_line_pointer = op_end;
  while (!(opcode->form = find_format (opcode->op, myops, fsize, cmp_hack)))
    {
      opcode->op++;
      if (strcmp(opcode->op->name,name))
	return -1;
    }
  input_line_pointer = save;

  insn = build_insn (opcode, myops); 

  /* Propigate multiply status */
  if (insn != -1)
    {
      prev_mul32_p = cur_mul32_p;
      cur_mul32_p = (opcode->op->flags_used & FLAG_MUL32) != 0;
    }

  return (insn);
}


/* find_format() gets a pointer to an entry in the format table.       */
/* It must look at all formats for an opcode and use the operands */
/* to choose the correct one.  Returns NULL on error. */

static struct d30v_format *
find_format (opcode, myops, fsize, cmp_hack)
     struct d30v_opcode *opcode;
     expressionS myops[];
     int fsize;
     int cmp_hack;
{
  int numops, match, index, i=0, j, k;
  struct d30v_format *fm;

  /* get all the operands and save them as expressions */
  numops = get_operands (myops, cmp_hack);

  while ((index = opcode->format[i++]) != 0)
    {
      if ((fsize == FORCE_SHORT) && (index >= LONG))
	continue;

      if ((fsize == FORCE_LONG) && (index < LONG))
	continue;

      fm = (struct d30v_format *)&d30v_format_table[index];
      k = index;
      while (fm->form == index)
	{
	  match = 1;
	  /* now check the operands for compatibility */
	  for (j = 0; match && fm->operands[j]; j++)
	    {
	      int flags = d30v_operand_table[fm->operands[j]].flags;
	      int X_op = myops[j].X_op;
	      int num = myops[j].X_add_number;
	      
	      if ( flags & OPERAND_SPECIAL )
		break;
	      else if (X_op == 0)
		match = 0;
	      else if (flags & OPERAND_REG)
		{
		  if ((X_op != O_register)
		      || ((flags & OPERAND_ACC) && !(num & OPERAND_ACC))
		      || ((flags & OPERAND_FLAG) && !(num & OPERAND_FLAG))
		      || ((flags & OPERAND_CONTROL)
			  && !(num & (OPERAND_CONTROL | OPERAND_FLAG))))
		    {
		      match = 0;
		    }
		}
	      else
		if (((flags & OPERAND_MINUS) && ((X_op != O_absent) || (num != OPERAND_MINUS)))
		    || ((flags & OPERAND_PLUS) && ((X_op != O_absent) || (num != OPERAND_PLUS)))
		    || ((flags & OPERAND_ATMINUS) && ((X_op != O_absent) || (num != OPERAND_ATMINUS)))
		    || ((flags & OPERAND_ATPAR) && ((X_op != O_absent) || (num != OPERAND_ATPAR)))
		    || ((flags & OPERAND_ATSIGN) && ((X_op != O_absent) || (num != OPERAND_ATSIGN))))
		  {
		    match=0;
		  }
		else if (flags & OPERAND_NUM)
		  {
		    /* a number can be a constant or symbol expression */
		    if (fm->form >= LONG)
		      {
			/* If we're testing for a LONG format, either fits */
			if (X_op != O_constant && X_op != O_symbol)
			  match = 0;
		      }
		    else if ((fm->form < LONG) && (((fsize == FORCE_SHORT) && (X_op == O_symbol)) ||
						   (fm->form == SHORT_D2 && j == 0)))
		      match = 1;
		    /* This is the tricky part.  Will the constant or symbol */
		    /* fit into the space in the current format? */
		    else if (X_op == O_constant)
		      {
			if (check_range (num, d30v_operand_table[fm->operands[j]].bits, flags))
			  match = 0;
		      }
		    else if (X_op == O_symbol && S_IS_DEFINED(myops[j].X_add_symbol) &&
			     (S_GET_SEGMENT(myops[j].X_add_symbol) == now_seg) &&
			     opcode->reloc_flag == RELOC_PCREL)
		      {
			/* if the symbol is defined, see if the value will fit */
			/* into the form we're considering */
			fragS *f;
			long value;
			/* calculate the current address by running through the previous frags */
			/* and adding our current offset */
			for (value = 0, f = frchain_now->frch_root; f; f = f->fr_next)
			  value += f->fr_fix + f->fr_offset;
			value = S_GET_VALUE(myops[j].X_add_symbol) - value -
			  (obstack_next_free(&frchain_now->frch_obstack) - frag_now->fr_literal);
			if (check_range (value, d30v_operand_table[fm->operands[j]].bits, flags)) 
			  match = 0;
		      }
		    else
		      match = 0;
		  }
	    }
	  /* printf("through the loop: match=%d\n",match);  */
	  /* we're only done if the operands matched so far AND there
	     are no more to check */
	  if (match && myops[j].X_op==0) 
	    return fm;
	  match = 0;
	  fm = (struct d30v_format *)&d30v_format_table[++k];
	}
      /* printf("trying another format: i=%d\n",i); */
    }
  return NULL;
}

/* if while processing a fixup, a reloc really needs to be created */
/* then it is done here */
                 
arelent *
tc_gen_reloc (seg, fixp)
     asection *seg;
     fixS *fixp;
{
  arelent *reloc;
  reloc = (arelent *) xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
                    _("reloc %d not supported by object file format"), (int)fixp->fx_r_type);
      return NULL;
    }
  reloc->addend = fixp->fx_addnumber;
  return reloc;
}

int
md_estimate_size_before_relax (fragp, seg)
     fragS *fragp;
     asection *seg;
{
  abort ();
  return 0;
} 

long
md_pcrel_from_section (fixp, sec)
     fixS *fixp;
     segT sec;
{
  if (fixp->fx_addsy != (symbolS *)NULL && (!S_IS_DEFINED (fixp->fx_addsy) ||
     (S_GET_SEGMENT (fixp->fx_addsy) != sec)))
    return 0;
  return fixp->fx_frag->fr_address + fixp->fx_where;
}

int
md_apply_fix3 (fixp, valuep, seg)
     fixS *fixp;
     valueT *valuep;
     segT seg;
{
  char *where;
  unsigned long insn, insn2;
  long value;

  if (fixp->fx_addsy == (symbolS *) NULL)
    {
      value = *valuep;
      fixp->fx_done = 1;
    }
  else if (fixp->fx_pcrel)
    {
      value = *valuep;
    }
  else
    {
      value = fixp->fx_offset;
      if (fixp->fx_subsy != (symbolS *) NULL)
	{
	  if (S_GET_SEGMENT (fixp->fx_subsy) == absolute_section) 
	    value -= S_GET_VALUE (fixp->fx_subsy);
	  else
	    {
	      /* We don't actually support subtracting a symbol.  */
	      as_bad_where (fixp->fx_file, fixp->fx_line,
			    _("expression too complex"));
	    }
	}
    }
  
  /* Fetch the instruction, insert the fully resolved operand
     value, and stuff the instruction back again.  */
  where = fixp->fx_frag->fr_literal + fixp->fx_where;
  insn = bfd_getb32 ((unsigned char *) where);
  
  switch (fixp->fx_r_type)
    {
    case BFD_RELOC_D30V_6:
      check_size (value, 6, fixp->fx_file, fixp->fx_line);
      insn |= value & 0x3F;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_9_PCREL:
      if (fixp->fx_where & 0x7)
	{
	  if (fixp->fx_done)
	    value += 4;
	  else
	    fixp->fx_r_type = BFD_RELOC_D30V_9_PCREL_R;
	}
      check_size (value, 9, fixp->fx_file, fixp->fx_line);
      insn |= ((value >> 3) & 0x3F) << 12;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_15:
      check_size (value, 15, fixp->fx_file, fixp->fx_line);
      insn |= (value >> 3) & 0xFFF;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_15_PCREL:
      if (fixp->fx_where & 0x7)
	{
	  if (fixp->fx_done)
	    value += 4;
	  else
	    fixp->fx_r_type = BFD_RELOC_D30V_15_PCREL_R;
	}
      check_size (value, 15, fixp->fx_file, fixp->fx_line);
      insn |= (value >> 3) & 0xFFF;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_21:
      check_size (value, 21, fixp->fx_file, fixp->fx_line);
      insn |= (value >> 3) & 0x3FFFF;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_21_PCREL:
      if (fixp->fx_where & 0x7)
	{
	  if (fixp->fx_done)
	    value += 4;
	  else
	    fixp->fx_r_type = BFD_RELOC_D30V_21_PCREL_R;
	}
      check_size (value, 21, fixp->fx_file, fixp->fx_line);
      insn |= (value >> 3) & 0x3FFFF;
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      break;

    case BFD_RELOC_D30V_32:
      insn2 = bfd_getb32 ((unsigned char *) where + 4);
      insn |= (value >> 26) & 0x3F;		/* top 6 bits */
      insn2 |= ((value & 0x03FC0000) << 2);	/* next 8 bits */ 
      insn2 |= value & 0x0003FFFF;		/* bottom 18 bits */
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      bfd_putb32 ((bfd_vma) insn2, (unsigned char *) where + 4);
      break;

    case BFD_RELOC_D30V_32_PCREL:
      insn2 = bfd_getb32 ((unsigned char *) where + 4);
      insn |= (value >> 26) & 0x3F;		/* top 6 bits */
      insn2 |= ((value & 0x03FC0000) << 2);	/* next 8 bits */ 
      insn2 |= value & 0x0003FFFF;		/* bottom 18 bits */
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      bfd_putb32 ((bfd_vma) insn2, (unsigned char *) where + 4);
      break;

    case BFD_RELOC_32:
      bfd_putb32 ((bfd_vma) value, (unsigned char *) where);
      break;

    default:
      as_fatal (_("line %d: unknown relocation type: 0x%x"),fixp->fx_line,fixp->fx_r_type);
    }
  return 0;
}


/* d30v_cleanup() is called after the assembler has finished parsing the input 
   file or after a label is defined.  Because the D30V assembler sometimes saves short 
   instructions to see if it can package them with the next instruction, there may
   be a short instruction that still needs written.  */
int
d30v_cleanup ()
{
  segT seg;
  subsegT subseg;

  if (prev_insn != -1)
    {
      seg = now_seg;
      subseg = now_subseg;
      subseg_set (prev_seg, prev_subseg);
      write_1_short (&prev_opcode, (long)prev_insn, fixups->next);
      subseg_set (seg, subseg);
      prev_insn = -1;
    }
  return 1;
}


static void                      
d30v_number_to_chars (buf, value, n)
     char *buf;			/* Return 'nbytes' of chars here. */
     long long value;		/* The value of the bits. */
     int n;			/* Number of bytes in the output. */
{
  while (n--)
    {
      buf[n] = value & 0xff;
      value >>= 8;
    }
}


/* This function is called at the start of every line. */
/* it checks to see if the first character is a '.' */
/* which indicates the start of a pseudo-op.  If it is, */
/* then write out any unwritten instructions */

void 
d30v_start_line()
{
  char *c = input_line_pointer;

  while(isspace(*c))
    c++;

  if (*c == '.') 
    d30v_cleanup();
}

static void 
check_size (value, bits, file, line)
     long value;
     int bits;
     char *file;
     int line;
{
  int tmp, max;

  if (value < 0)
    tmp = ~value;
  else
    tmp = value;
    
  max = (1 << (bits - 1)) - 1;

  if (tmp > max)
    as_bad_where (file, line,"value too large to fit in %d bits",bits);

  return;
}
