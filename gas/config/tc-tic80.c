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


/* Generic assembler global variables which must be defined by all targets. */

/* Characters which always start a comment. */
const char comment_chars[] = ";";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = ";*";

/* Characters which may be used to separate multiple commands on a 
   single line.  */
const char line_separator_chars[] = ";";

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
static unsigned long build_insn PARAMS ((struct tic80_opcode *, expressionS *, unsigned long insn));
static int get_operands PARAMS ((expressionS exp[]));
static int reg_name_search PARAMS ((char *name));
static int register_name PARAMS ((expressionS *expressionP));


int
md_estimate_size_before_relax (fragP, segment_type)
     fragS *fragP;
     segT segment_type;
{
  as_fatal ("Relaxation is a luxury we can't afford\n");
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

/* reg_name_search does a binary search of the tic80_pre_defined_registers
   array to see if "name" is a valid regiter name.  Returns the register
   number from the array on success, or -1 on failure. */

static int
reg_name_search (name)
     char *name;
{
  int middle, low, high;
  int cmp;

  low = 0;
  high = tic80_num_regs - 1;

  do
    {
      middle = (low + high) / 2;
      cmp = strcasecmp (name, tic80_pre_defined_registers[middle].name);
      if (cmp < 0)
	{
	  high = middle - 1;
	}
      else if (cmp > 0)
	{
	  low = middle + 1;
	}
      else 
	{
	  return (tic80_pre_defined_registers[middle].value);
	}
    }
  while (low <= high);
  return (-1);
}

/* register_name() checks the string at input_line_pointer
   to see if it is a valid register name */

static int
register_name (expressionP)
     expressionS *expressionP;
{
  int reg_number;
  char c;
  char *p = input_line_pointer;
  
  while (*p != '\000' && *p != '\n' && *p != '\r' && *p != ',' && *p != ' ' && *p != ')')
    p++;

  c = *p;
  if (c)
    {
      *p++ = '\000';
    }

  /* look to see if it's in the register table */

  reg_number = reg_name_search (input_line_pointer);
  if (reg_number >= 0) 
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

      /* Check to see if it might be a register name */

      if (!register_name (&exp[numexp]))
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

  exp[numexp].X_op = 0;
  return (numexp);
}

/* find_opcode() gets a pointer to the entry in the opcode table that
   matches the instruction being assembled, or returns NULL if no such match
   is found.

   First it parses all the operands and save them as expressions.  Note that
   we use O_absent expressions to record additional information about the
   previous non-O_absent expression, such as ":m" or ":s" modifiers or
   register numbers enclosed in parens like "(r10)".

   It then looks at all opcodes with the same name and use the operands to
   choose the correct opcode.  */


static struct tic80_opcode *
find_opcode (opcode, myops)
     struct tic80_opcode *opcode;
     expressionS myops[];
{
  int i, match, done, numops;
  struct tic80_opcode *next_opcode;

  match = 0;

  /* First parse all the operands so we only have to do it once. */

  get_operands (myops);

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
	      
	  if (flags & TIC80_OPERAND_GPR) 
	    {
	      if ((X_op != O_register) ||
		  ((flags & OPERAND_ACC) != (num & OPERAND_ACC)) ||
		  ((flags & OPERAND_FLAG) != (num & OPERAND_FLAG)) ||
		  ((flags & OPERAND_CONTROL) != (num & OPERAND_CONTROL)))
		{
		  match=0;
		  break;
		}	  
	    }
	      
	  if (((flags & OPERAND_MINUS) && ((X_op != O_absent) || (num != OPERAND_MINUS))) ||
	      ((flags & OPERAND_PLUS) && ((X_op != O_absent) || (num != OPERAND_PLUS))) ||
	      ((flags & OPERAND_ATMINUS) && ((X_op != O_absent) || (num != OPERAND_ATMINUS))) ||
	      ((flags & OPERAND_ATPAR) && ((X_op != O_absent) || (num != OPERAND_ATPAR))) ||
	      ((flags & OPERAND_ATSIGN) && ((X_op != O_absent) || (num != OPERAND_ATSIGN)))) 
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
      if ((tic80_operands[opcode->operands[i]].flags & OPERAND_EVEN) &&
	  (myops[i].X_add_number & 1)) 
	as_fatal("Register number must be EVEN");
      if (myops[i].X_op == O_register)
	{
	  if (!(tic80_operands[opcode->operands[i]].flags & OPERAND_REG)) 
	    {
	      myops[i].X_op = O_symbol;
	      myops[i].X_add_symbol = symbol_find_or_make ((char *)myops[i].X_op_symbol);
	      myops[i].X_add_number = 0;
	      myops[i].X_op_symbol = NULL;
	    }
	}
    }
  return opcode;
}

/* build_insn takes a pointer to the opcode entry in the opcode table
   and the array of operand expressions and returns the instruction */

static unsigned long
build_insn (opcode, opers, insn) 
     struct tic80_opcode *opcode;
     expressionS *opers;
     unsigned long insn;
{
  return (0);
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
  expressionS myops[6];
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
      return;
    }

  input_line_pointer = input_line_save;
  insn = build_insn (opcode, myops, 0);

  /* FIXME - finish this */
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
  as_fatal ("md_apply_fix() not implemented yet\n");
  abort ();
}


/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  as_fatal ("md_pcrel_from() not implemented yet\n");
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
  as_fatal ("md_convert_frag() not implemented yet\n");
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
  as_fatal ("tc_coff_fix2rtype() not implemented yet\n");
  abort ();
}

#endif	/* OBJ_COFF */

/* end of tc-tic80.c */
