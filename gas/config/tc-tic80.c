/* tc-tic80.c -- Assemble for the TI TMS320C80 (MV)
   Copyright (C) 1996 Free Software Foundation, Inc.

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
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "as.h"
#include "opcode/tic80.h"

#define internal_error(what) \
  as_fatal("internal error:%s:%d: %s\n",__FILE__,__LINE__,what)
#define internal_error_a(what,arg) \
  as_fatal("internal error:%s:%d: %s %d\n",__FILE__,__LINE__,what,arg)


/* Generic assembler global variables which must be defined by all targets. */

/* Characters which always start a comment. */
const char comment_chars[] = ";";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = ";*";

/* Characters which may be used to separate multiple commands on a 
   single line. */
const char line_separator_chars[] = "";

/* Characters which are used to indicate an exponent in a floating 
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant, 
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
   */

const pseudo_typeS md_pseudo_table[] =
{
  {"word", cons, 4},
  { NULL,       NULL,           0 }
};

/* Opcode hash table.  */
static struct hash_control *tic80_hash;

static struct tic80_opcode * find_opcode PARAMS ((struct tic80_opcode *, expressionS []));
static void build_insn PARAMS ((struct tic80_opcode *, expressionS *));
static int get_operands PARAMS ((expressionS exp[]));
static int register_name PARAMS ((expressionS *expressionP));
static int is_bitnum PARAMS ((expressionS *expressionP));
static int is_ccode PARAMS ((expressionS *expressionP));
static int const_overflow PARAMS ((unsigned long num, int bits, int flags));


int
md_estimate_size_before_relax (fragP, segment_type)
     fragS *fragP;
     segT segment_type;
{
  internal_error ("Relaxation is a luxury we can't afford");
  return (-1);
}

/* We have no need to default values of symbols.  */

/* ARGSUSED */
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

#define MAX_LITTLENUMS 4

char *
md_atof (type, litP, sizeP)
     int type;
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

    default:
      *sizeP = 0;
      return "bad call to md_atof ()";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    {
      input_line_pointer = t;
    }
  
  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (valueT) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
  return (NULL);
}

/* register_name() checks the string at input_line_pointer
   to see if it is a valid register name */

static int
register_name (expressionP)
     expressionS *expressionP;
{
  int reg_number, class;
  char c;
  char *p = input_line_pointer;
  
  while (*p != '\000' && *p != '\n' && *p != '\r' && *p != ',' && *p != ' ' && *p != '(' && *p != ')')
    p++;

  c = *p;
  if (c)
    {
      *p++ = '\000';
    }

  /* look to see if it's in the register table */

  class = TIC80_OPERAND_FPA | TIC80_OPERAND_CR | TIC80_OPERAND_GPR;
  reg_number = tic80_symbol_to_value (input_line_pointer, class);
  if (reg_number != -1) 
    {
      expressionP -> X_op = O_register;
      /* temporarily store a pointer to the string here */
      expressionP -> X_op_symbol = (struct symbol *) input_line_pointer;
      expressionP -> X_add_number = reg_number;
      input_line_pointer = p;
      return (1);
    }
  if (c)
    {
      *(p - 1) = c;
    }
  return (0);
}

/* is_bitnum() checks the string at input_line_pointer
   to see if it is a valid predefined symbol for the BITNUM field */

static int
is_bitnum (expressionP)
     expressionS *expressionP;
{
  int bitnum_val, class;
  char c;
  char *p = input_line_pointer;
  
  while (*p != '\000' && *p != '\n' && *p != '\r' && *p != ',' && *p != ' ' && *p != '(' && *p != ')')
    p++;

  c = *p;
  if (c)
    {
      *p++ = '\000';
    }

  /* look to see if it's in the register table */

  class = TIC80_OPERAND_BITNUM;
  bitnum_val = tic80_symbol_to_value (input_line_pointer, class);
  if (bitnum_val != -1) 
    {
      expressionP -> X_op = O_constant;
      /* temporarily store a pointer to the string here */
      expressionP -> X_op_symbol = (struct symbol *) input_line_pointer;
      /* Bitnums are stored as one's complement */
      expressionP -> X_add_number = ~bitnum_val & 0x1F;
      input_line_pointer = p;
      return (1);
    }
  if (c)
    {
      *(p - 1) = c;
    }
  return (0);
}

/* is_ccode() checks the string at input_line_pointer
   to see if it is a valid predefined symbol for the a condition code */

static int
is_ccode (expressionP)
     expressionS *expressionP;
{
  int ccode_val, class;
  char c;
  char *p = input_line_pointer;
  
  while (*p != '\000' && *p != '\n' && *p != '\r' && *p != ',' && *p != ' ' && *p != '(' && *p != ')')
    p++;

  c = *p;
  if (c)
    {
      *p++ = '\000';
    }

  /* look to see if it's in the register table */

  class = TIC80_OPERAND_CC;
  ccode_val = tic80_symbol_to_value (input_line_pointer, class);
  if (ccode_val != -1) 
    {
      expressionP -> X_op = O_constant;
      /* temporarily store a pointer to the string here */
      expressionP -> X_op_symbol = (struct symbol *) input_line_pointer;
      expressionP -> X_add_number = ccode_val & 0x1F;
      input_line_pointer = p;
      return (1);
    }
  if (c)
    {
      *(p - 1) = c;
    }
  return (0);
}

/* Check to see if the constant value in NUM will fit in a field of
   width BITS if it has flags FLAGS. */

static int
const_overflow (num, bits, flags)
     unsigned long num;
     int bits;
     int flags;
{
  long min, max;
  int retval = 0;

  /* Only need to check fields less than 32 bits wide */
  if (bits < 32)
    if (flags & TIC80_OPERAND_SIGNED)
      {
	max = (1 << (bits - 1)) - 1; 
	min = - (1 << (bits - 1));  
	retval = ((long) num > max) || ((long) num < min);
      }
    else
      {
	max = (1 << bits) - 1;
	min = 0;
	retval = (num > max) || (num < min);
      }
  return (retval);
}

/* get_operands() parses a string of operands and fills in a passed array of
   expressions in EXP.

   Note that we use O_absent expressions to record additional information
   about the previous non-O_absent expression, such as ":m" or ":s"
   modifiers or register numbers enclosed in parens like "(r10)".

   Returns the number of expressions that were placed in EXP.

   */

static int
get_operands (exp) 
     expressionS exp[];
{
  char *p = input_line_pointer;
  int numexp = 0;
  int mflag = 0;
  int sflag = 0;
  int parens = 0;

  while (*p)  
    {
      /* Skip leading whitespace */
      while (*p == ' ' || *p == '\t' || *p == ',') 
	{
	  p++;
	}

      /* Check to see if we have any operands left to parse */
      if (*p == 0 || *p == '\n' || *p == '\r') 
	{
	  break;
	}
      
      /* Notice scaling or direct memory operand modifiers and save them in
	 an O_absent expression after the expression that they modify. */

      if (*p == ':') 
	{
	  p++;
	  exp[numexp].X_op = O_absent;
	  if (*p == 'm') 
	    {
	      p++;
	      exp[numexp].X_add_number = TIC80_OPERAND_M_SI | TIC80_OPERAND_M_LI;
	    }
	  else if (*p == 's')
	    {
	      p++;
	      exp[numexp].X_add_number = TIC80_OPERAND_SCALED;
	    }
	  else
	    {
	      as_bad ("':' not followed by 'm' or 's'");
	    }
	  numexp++;
	  continue;
	}

      /* Handle leading '(' on operands that use them, by recording that we
	 have entered a paren nesting level and then continuing.  We complain
	 about multiple nesting. */

      if (*p == '(')
	{
	  if (++parens != 1)
	    {
	      as_bad ("paren nesting");
	    }
	  p++;
	  continue;
	}

      /* Handle trailing ')' on operands that use them, by reducing the
	 nesting level and then continuing.  We complain if there were too
	 many closures. */

      if (*p == ')') 
	{
	  /* Record that we have left a paren group and continue */
	  if (--parens < 0)
	    {
	      as_bad ("mismatched parenthesis");
	    }
	  p++;
	  continue;
	}

      /* Begin operand parsing at the current scan point. */

      input_line_pointer = p;

      /* Check to see if it might be a register name or some other
	 predefined symbol name that translates into a constant value. */

      if (!register_name (&exp[numexp]) &&
	  !is_bitnum (&exp[numexp]) &&
	  !is_ccode (&exp[numexp]))
	{
	  /* parse as an expression */
	  expression (&exp[numexp]);
	}

      if (exp[numexp].X_op == O_illegal)
	{
	  as_bad ("illegal operand");
	}
      else if (exp[numexp].X_op == O_absent)
	{
	  as_bad ("missing operand");
	}

      numexp++;
      p = input_line_pointer;
    }

  if (parens)
    {
      exp[numexp].X_op = O_absent;
      exp[numexp++].X_add_number = TIC80_OPERAND_PARENS;
    }

  /* Mark the end of the valid operands with an illegal expression. */
  exp[numexp].X_op = O_illegal;

  return (numexp);
}

/* find_opcode() gets a pointer to the entry in the opcode table that
   matches the instruction being assembled, or returns NULL if no such match
   is found.

   First it parses all the operands and save them as expressions.  Note that
   we use O_absent expressions to record additional information about the
   previous non-O_absent expression, such as ":m" or ":s" modifiers or
   register numbers enclosed in parens like "(r10)".

   It then looks at all opcodes with the same name and uses the operands to
   choose the correct opcode.  */

static struct tic80_opcode *
find_opcode (opcode, myops)
     struct tic80_opcode *opcode;
     expressionS myops[];
{
  int numexp;				/* Number of expressions from parsing operands */
  int expi;				/* Index of current expression to match */
  int opi;				/* Index of current operand to match */
  int match = 0;			/* Set to 1 when an operand match is found */
  struct tic80_opcode *opc = opcode;	/* Pointer to current opcode table entry */
  const struct tic80_opcode *end;	/* Pointer to end of opcode table */

  /* First parse all the operands so we only have to do it once.  There may
     be more expressions generated than there are operands. */

  numexp = get_operands (myops);

  /* For each opcode with the same name, try to match it against the parsed
     operands. */

  end = tic80_opcodes + tic80_num_opcodes;
  while (!match && (opc < end) && (strcmp (opc -> name, opcode -> name) == 0))
    {
      /* Start off assuming a match.  If we find a mismatch, then this is
	 reset and the operand/expr matching loop terminates with match
	 equal to zero, which allows us to try the next opcode. */

      match = 1;

      /* For each expression, try to match it against the current operand
	 for the current opcode.  Upon any mismatch, we abandon further
	 matching for the current opcode table entry.  */

      for (expi = 0, opi = -1; (expi < numexp) && match; expi++)
	{
	  int bits, flags, X_op, num;

	  X_op = myops[expi].X_op;
	  num = myops[expi].X_add_number;
	  if (X_op != O_absent)
	    {
	      /* The O_absent expressions apply to the previously seen
		 operand, so only increment the operand index when the
		 current expression needs to be matched against the next
		 operand. */
	      opi++;
	    }
	  flags = tic80_operands[opc -> operands[opi]].flags;
	  bits = tic80_operands[opc -> operands[opi]].bits;

	  switch (X_op)
	    {
	    case O_register:
	      /* Also check that registers that are supposed to be even actually
		 are even. */
	      if (((flags & TIC80_OPERAND_GPR) != (num & TIC80_OPERAND_GPR)) ||
		  ((flags & TIC80_OPERAND_FPA) != (num & TIC80_OPERAND_FPA)) ||
		  ((flags & TIC80_OPERAND_CR) != (num & TIC80_OPERAND_CR)) ||
		  ((flags & TIC80_OPERAND_EVEN) && (num & 1)) ||
		  const_overflow (num & ~TIC80_OPERAND_MASK, bits, flags))
		{
		  match = 0;
		}
	      break;
	    case O_constant:
	      if ((flags & TIC80_OPERAND_ENDMASK) && (num == 32))
		{
		  /* Endmask values of 0 and 32 give identical results */
		  num = 0;
		}
	      if ((flags & (TIC80_OPERAND_FPA | TIC80_OPERAND_GPR)) ||
		  const_overflow (num, bits, flags))
		{
		  match = 0;
		}
	      break;
	    case O_illegal:
	    case O_absent:
	    case O_symbol:
	    case O_symbol_rva:
	    case O_big:
	    case O_uminus:
	    case O_bit_not:
	    case O_logical_not:
	    case O_multiply:
	    case O_divide:
	    case O_modulus:
	    case O_left_shift:
	    case O_right_shift:
	    case O_bit_inclusive_or:
	    case O_bit_or_not:
	    case O_bit_exclusive_or:
	    case O_bit_and:
	    case O_add:
	    case O_subtract:
	    case O_eq:
	    case O_ne:
	    case O_lt:
	    case O_le:
	    case O_ge:
	    case O_gt:
	    case O_logical_and:
	    case O_logical_or:
	    case O_max:
	    default:
	      internal_error_a ("unhandled expression type", X_op);
	    }
	}
      if (!match)
	{
	  opc++;
	}
    }  

  return (match ? opc : NULL);

#if 0

  /* Now search the opcode table table for one with operands that
     matches what we've got. */

  while (!match)
    {
      match = 1;
      for (i = 0; opcode -> operands[i]; i++) 
	{
	  int flags = tic80_operands[opcode->operands[i]].flags;
	  int X_op = myops[i].X_op;
	  int num = myops[i].X_add_number;

	  if (X_op == 0) 
	    {
	      match = 0;
	      break;
	    }
	      
	  if (flags & (TIC80_OPERAND_GPR | TIC80_OPERAND_FPA | TIC80_OPERAND_CR)) 
	    {
	      if ((X_op != O_register) ||
		  ((flags & TIC80_OPERAND_GPR) != (num & TIC80_OPERAND_GPR)) ||
		  ((flags & TIC80_OPERAND_FPA) != (num & TIC80_OPERAND_FPA)) ||
		  ((flags & TIC80_OPERAND_CR) != (num & TIC80_OPERAND_CR)))
		{
		  match=0;
		  break;
		}	  
	    }
	      
	  if (((flags & TIC80_OPERAND_MINUS) && ((X_op != O_absent) || (num != TIC80_OPERAND_MINUS))) ||
	      ((flags & TIC80_OPERAND_PLUS) && ((X_op != O_absent) || (num != TIC80_OPERAND_PLUS))) ||
	      ((flags & TIC80_OPERAND_ATMINUS) && ((X_op != O_absent) || (num != TIC80_OPERAND_ATMINUS))) ||
	      ((flags & TIC80_OPERAND_ATPAR) && ((X_op != O_absent) || (num != TIC80_OPERAND_ATPAR))) ||
	      ((flags & TIC80_OPERAND_ATSIGN) && ((X_op != O_absent) || (num != TIC80_OPERAND_ATSIGN)))) 
	    {
	      match=0;
	      break;
	    }	      
	}
      /* we're only done if the operands matched so far AND there
	 are no more to check */
      if (match && myops[i].X_op==0) 
	break;
      else
	match = 0;

      next_opcode = opcode+1;
      if (next_opcode->opcode == 0) 
	break;
      if (strcmp(next_opcode->name, opcode->name))
	break;
      opcode = next_opcode;
    }

  if (!match)  
    {
      as_bad ("bad opcode or operands");
      return (0);
    }

  /* Check that all registers that are required to be even are. */
  /* Also, if any operands were marked as registers, but were really symbols */
  /* fix that here. */
  for (i=0; opcode->operands[i]; i++) 
    {
      if ((tic80_operands[opcode->operands[i]].flags & TIC80_OPERAND_EVEN) &&
	  (myops[i].X_add_number & 1)) 
	as_fatal ("Register number must be EVEN");
      if (myops[i].X_op == O_register)
	{
	  if (!(tic80_operands[opcode->operands[i]].flags & TIC80_OPERAND_REG)) 
	    {
	      myops[i].X_op = O_symbol;
	      myops[i].X_add_symbol = symbol_find_or_make ((char *)myops[i].X_op_symbol);
	      myops[i].X_add_number = 0;
	      myops[i].X_op_symbol = NULL;
	    }
	}
    }

#endif
}

/* build_insn takes a pointer to the opcode entry in the opcode table
   and the array of operand expressions and writes out the instruction. */

static void
build_insn (opcode, opers) 
     struct tic80_opcode *opcode;
     expressionS *opers;
{
  int expi;				/* Index of current expression to match */
  int opi;				/* Index of current operand to match */
  unsigned long insn[2];		/* Instruction and long immediate (if any) */
  int extended = 0;			/* Nonzero if instruction is 8 bytes */
  char *f;				/* Temporary pointer to output location */

  /* Start with the raw opcode bits from the opcode table. */
  insn[0] = opcode -> opcode;

  /* For each operand expression, insert the appropriate bits into the
     instruction . */
  for (expi = 0, opi = -1; opers[expi].X_op != O_illegal; expi++)
    {
      int bits, shift, flags, X_op, num;

      X_op = opers[expi].X_op;
      num = opers[expi].X_add_number;
      if (X_op != O_absent)
	{
	  /* The O_absent expressions apply to the previously seen
	     operand, so only increment the operand index when the
	     current expression needs to be matched against the next
	     operand. */
	  opi++;
	}
      else
	{
	  /* Found a modifier that applies to the previously
	     seen operand, so handle it. */
	  switch (opers[expi].X_add_number)
	    {
	    case TIC80_OPERAND_M_SI | TIC80_OPERAND_M_LI:
	      internal_error_a ("unhandled operand modifier", opers[expi].X_add_number);
	      break;
	    case TIC80_OPERAND_SCALED:
	      internal_error_a ("unhandled operand modifier", opers[expi].X_add_number);
	      break;
	    case TIC80_OPERAND_PARENS:
	      internal_error_a ("unhandled operand modifier", opers[expi].X_add_number);
	      break;
	    default:
	      internal_error_a ("unhandled operand modifier", opers[expi].X_add_number);
	      break;
	    }
	}
      flags = tic80_operands[opcode -> operands[opi]].flags;
      bits = tic80_operands[opcode -> operands[opi]].bits;
      shift = tic80_operands[opcode -> operands[opi]].shift;

      switch (X_op)
	{
	case O_register:
	  num &= ~TIC80_OPERAND_MASK;
	  insn[0] = insn[0] | (num << shift);
	  break;
	case O_constant:
	  if ((flags & TIC80_OPERAND_ENDMASK) && (num == 32))
	    {
	      /* Endmask values of 0 and 32 give identical results */
	      num = 0;
	    }
	  /* Mask off upper bits, just it case it is signed and is negative */
	  if (bits < 32)
	    {
	      num &= (1 << bits) - 1;
	      insn[0] = insn[0] | (num << shift);
	    }
	  else
	    {
	      extended++;
	      insn[1] = num;
	    }
	  break;
	case O_illegal:
	case O_absent:
	case O_symbol:
	case O_symbol_rva:
	case O_big:
	case O_uminus:
	case O_bit_not:
	case O_logical_not:
	case O_multiply:
	case O_divide:
	case O_modulus:
	case O_left_shift:
	case O_right_shift:
	case O_bit_inclusive_or:
	case O_bit_or_not:
	case O_bit_exclusive_or:
	case O_bit_and:
	case O_add:
	case O_subtract:
	case O_eq:
	case O_ne:
	case O_lt:
	case O_le:
	case O_ge:
	case O_gt:
	case O_logical_and:
	case O_logical_or:
	case O_max:
	default:
	  internal_error_a ("unhandled expression", X_op);
	  break;
	}
    }

  /* Write out the instruction, either 4 or 8 bytes.  */

  f = frag_more (4);
  md_number_to_chars (f, insn[0], 4);
  if (extended)
    {
      f = frag_more (4);
      md_number_to_chars (f, insn[1], 4);
    }
}

/* This is the main entry point for the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This function is supposed to emit the frags/bytes
   it assembles to.
 */

void
md_assemble (str)
     char *str;
{
  char *scan;
  unsigned char *input_line_save;
  struct tic80_opcode *opcode;
  expressionS myops[16];
  unsigned long insn;

  /* Ensure there is something there to assemble. */
  assert (str);

  /* Drop any leading whitespace. */
  while (isspace (*str))
    {
      str++;
    }

  /* Isolate the mnemonic from the rest of the string by finding the first
     whitespace character and zapping it to a null byte. */
  for (scan = str; *scan != '\000' && !isspace (*scan); scan++) {;}
  if (*scan != '\000')
    {
      *scan++ = '\000';
    }

  /* Try to find this mnemonic in the hash table */
  if ((opcode = (struct tic80_opcode *) hash_find (tic80_hash, str)) == NULL)
    {
      as_bad ("Invalid mnemonic: '%s'", str);
      return;
    }

  str = scan;
  while (isspace (*scan))
    {
      scan++;
    }

  input_line_save = input_line_pointer;
  input_line_pointer = str;

  opcode = find_opcode (opcode, myops);
  if (opcode == NULL)
    {
      as_bad ("Invalid operands: '%s'", input_line_save);
    }

  input_line_pointer = input_line_save;
  build_insn (opcode, myops);
}

/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc., that the MD part of the assembler will
   need.  */

void
md_begin ()
{
  char *prev_name = "";
  register const struct tic80_opcode *op;
  register const struct tic80_opcode *op_end;

  tic80_hash = hash_new();

  /* Insert unique names into hash table.  The TIc80 instruction set
     has many identical opcode names that have different opcodes based
     on the operands.  This hash table then provides a quick index to
     the first opcode with a particular name in the opcode table.  */

  op_end = tic80_opcodes + tic80_num_opcodes;
  for (op = tic80_opcodes; op < op_end; op++)
    {
      if (strcmp (prev_name, op -> name) != 0) 
	{
	  prev_name = (char *) op -> name;
	  hash_insert (tic80_hash, op -> name, (char *) op);
	}
    }
}



CONST char *md_shortopts = "";

struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof(md_longopts);

/* Take care of the target-specific command-line options.  */

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  return (0);
}

/* Print a description of the command-line options that we accept.  */

void
md_show_usage (stream)
     FILE *stream;
{
}


/* Attempt to simplify or even eliminate a fixup.  The return value is
   ignored; perhaps it was once meaningful, but now it is historical.
   To indicate that a fixup has been eliminated, set fixP->fx_done.
   */

void
md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  internal_error ("md_apply_fix() not implemented yet");
  abort ();
}


/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  internal_error ("md_pcrel_from() not implemented yet");
  abort ();
}

/*
 * Called after relax() is finished.
 * In:	Address of frag.
 *	fr_type == rs_machine_dependent.
 *	fr_subtype is what the address relaxed to.
 *
 * Out:	Any fixSs and constants are set up.
 *	Caller will turn frag into a ".space 0".
 */

void
md_convert_frag (headers, seg, fragP)
     object_headers *headers;
     segT seg;
     fragS *fragP;
{
  internal_error ("md_convert_frag() not implemented yet");
  abort ();
}


/*ARGSUSED*/
void
tc_coff_symbol_emit_hook (ignore)
     symbolS *ignore;
{
}

#if defined OBJ_COFF

short
tc_coff_fix2rtype (fixP)
     fixS *fixP;
{
  internal_error ("tc_coff_fix2rtype() not implemented yet");
  abort ();
}

#endif	/* OBJ_COFF */

/* end of tc-tic80.c */
