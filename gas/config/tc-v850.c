/* tc-v850.c -- Assembler code for the NEC V850
   Copyright (C) 1996, 1997 Free Software Foundation.

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
#include "opcode/v850.h"

/* sign-extend a 16-bit number */
#define SEXT16(x)	((((x) & 0xffff) ^ (~ 0x7fff)) + 0x8000)

/* Temporarily holds the reloc in a cons expression.  */
static bfd_reloc_code_real_type hold_cons_reloc;

/* Set to TRUE if we want to be pedantic about signed overflows.  */
static boolean warn_signed_overflows   = FALSE;
static boolean warn_unsigned_overflows = FALSE;

/* Indicates the target BFD machine number.  */
static int     machine                 = TARGET_MACHINE;

/* Indicates the target processor(s) for the assemble.  */
static unsigned int	processor_mask = TARGET_PROCESSOR;


/* Structure to hold information about predefined registers.  */
struct reg_name
{
  const char * name;
  int          value;
};

/* Generic assembler global variables which must be defined by all targets. */

/* Characters which always start a comment. */
const char comment_chars[] = "#";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = ";#";

/* Characters which may be used to separate multiple commands on a 
   single line.  */
const char line_separator_chars[] = ";";

/* Characters which are used to indicate an exponent in a floating 
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant, 
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";


const relax_typeS md_relax_table[] = {
  {0xff, -0x100, 2, 1},
  {0x1fffff, -0x200000, 6, 0},
};


static segT sdata_section = NULL;
static segT tdata_section = NULL;
static segT zdata_section = NULL;
static segT sbss_section = NULL;
static segT tbss_section = NULL;
static segT zbss_section = NULL;
static segT rosdata_section = NULL;
static segT rozdata_section = NULL;
/* start-sanitize-v850e */
static segT call_table_data_section = NULL;
static segT call_table_text_section = NULL;
/* end-sanitize-v850e */


/* local functions */
static unsigned long v850_insert_operand
  PARAMS ((unsigned long insn, const struct v850_operand *operand,
	   offsetT val, char *file, unsigned int line));


/* fixups */
#define MAX_INSN_FIXUPS (5)
struct v850_fixup
{
  expressionS              exp;
  int                      opindex;
  bfd_reloc_code_real_type reloc;
};
struct v850_fixup fixups[MAX_INSN_FIXUPS];
static int fc;

void
v850_sdata (int ignore)
{
  subseg_set (sdata_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_tdata (int ignore)
{
  subseg_set (tdata_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_zdata (int ignore)
{
  subseg_set (zdata_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_sbss (int ignore)
{
  subseg_set (sbss_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_tbss (int ignore)
{
  subseg_set (tbss_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_zbss (int ignore)
{
  subseg_set (zbss_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_rosdata (int ignore)
{
  subseg_set (rosdata_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_rozdata (int ignore)
{
  subseg_set (rozdata_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

/* start-sanitize-v850e */
void
v850_call_table_data (int ignore)
{
  subseg_set (call_table_data_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}

void
v850_call_table_text (int ignore)
{
  subseg_set (call_table_text_section, (subsegT) get_absolute_expression ());
  
  demand_empty_rest_of_line ();
}
/* end-sanitize-v850e */

static void
v850_section (int arg)
{
  char   saved_c;
  char * ptr;
  
  for (ptr = input_line_pointer; * ptr != '\n' && * ptr != 0; ptr ++)
    if (* ptr == ',' && ptr[1] == '.')
      break;

  saved_c = * ptr;
  * ptr = ';';
  
  obj_elf_section (arg);

  * ptr = saved_c;
}

void
v850_bss (int ignore)
{
  register int temp = get_absolute_expression ();

  obj_elf_section_change_hook();
  
  subseg_set (bss_section, (subsegT) temp);
  
  demand_empty_rest_of_line ();
}

void
v850_offset (int ignore)
{
  int temp = get_absolute_expression ();
  
  temp -= frag_now_fix();
  
  if (temp > 0)
    (void) frag_more (temp);
  
  demand_empty_rest_of_line ();
}

void
set_machine (int number)
{
  machine = number;
  bfd_set_arch_mach (stdoutput, TARGET_ARCH, machine);

  switch (machine)
    {
    case 0: processor_mask = PROCESSOR_V850; break;
/* start-sanitize-v850e */
    case bfd_mach_v850e: processor_mask = PROCESSOR_V850E; break;
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
    case bfd_mach_v850eq: processor_mask = PROCESSOR_V850EQ; break;
/* end-sanitize-v850eq */
    }
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  {"sdata",   v850_sdata,   0},
  {"tdata",   v850_tdata,   0},
  {"zdata",   v850_zdata,   0},
  {"sbss",    v850_sbss,    0},
  {"tbss",    v850_tbss,    0},
  {"zbss",    v850_zbss,    0},
  {"rosdata", v850_rosdata, 0},
  {"rozdata", v850_rozdata, 0},
  {"bss",     v850_bss,     0},
  {"offset",  v850_offset,  0},
  {"section", v850_section, 0},
  {"word",    cons,         4},
  {"v850",    set_machine,  0},
/* start-sanitize-v850e */
  {"call_table_data", v850_call_table_data, 0},
  {"call_table_text", v850_call_table_text, 0},
  {"v850e",           set_machine,          bfd_mach_v850e},
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
  {"v850eq",          set_machine,          bfd_mach_v850eq},
/* end-sanitize-v850eq */
  { NULL,     NULL,         0}
};

/* Opcode hash table.  */
static struct hash_control *v850_hash;

/* This table is sorted. Suitable for searching by a binary search. */
static const struct reg_name pre_defined_registers[] =
{
  { "ep",  30 },		/* ep - element ptr */
  { "gp",   4 },		/* gp - global ptr */
  { "hp",   2 },		/* hp - handler stack ptr */
  { "lp",  31 },		/* lp - link ptr */
  { "r0",   0 },
  { "r1",   1 },
  { "r10", 10 },
  { "r11", 11 },
  { "r12", 12 },
  { "r13", 13 },
  { "r14", 14 },
  { "r15", 15 },
  { "r16", 16 },
  { "r17", 17 },
  { "r18", 18 },
  { "r19", 19 },
  { "r2",   2 },
  { "r20", 20 },
  { "r21", 21 },
  { "r22", 22 },
  { "r23", 23 },
  { "r24", 24 },
  { "r25", 25 },
  { "r26", 26 },
  { "r27", 27 },
  { "r28", 28 },
  { "r29", 29 },
  { "r3",   3 },
  { "r30", 30 },
  { "r31", 31 },
  { "r4",   4 },
  { "r5",   5 },
  { "r6",   6 },
  { "r7",   7 },
  { "r8",   8 },
  { "r9",   9 },
  { "sp",   3 },		/* sp - stack ptr */
  { "tp",   5 },		/* tp - text ptr */
  { "zero", 0 },
};
#define REG_NAME_CNT	(sizeof (pre_defined_registers) / sizeof (struct reg_name))


static const struct reg_name system_registers[] = 
{
/* start-sanitize-v850e */
  { "ctbp",  20 },
  { "ctpc",  16 },
  { "ctpsw", 17 },
  { "dbpc",  18 },
  { "dbpsw", 19 },
/* end-sanitize-v850e */
  { "ecr",    4 },
  { "eipc",   0 },
  { "eipsw",  1 },
  { "fepc",   2 },
  { "fepsw",  3 },
  { "psw",    5 },
};
#define SYSREG_NAME_CNT	(sizeof (system_registers) / sizeof (struct reg_name))

static const struct reg_name cc_names[] =
{
  { "c",  0x1 },
  { "e",  0x2 },
  { "ge", 0xe },
  { "gt", 0xf },
  { "h",  0xb },
  { "l",  0x1 },
  { "le", 0x7 },
  { "lt", 0x6 },
  { "n",  0x4 },
  { "nc", 0x9 },
  { "ne", 0xa },
  { "nh", 0x3 },
  { "nl", 0x9 },
  { "ns", 0xc },
  { "nv", 0x8 },
  { "nz", 0xa },
  { "p",  0xc },
  { "s",  0x4 },
  { "sa", 0xd },
  { "t",  0x5 },
  { "v",  0x0 },
  { "z",  0x2 },
};
#define CC_NAME_CNT	(sizeof (cc_names) / sizeof (struct reg_name))

/* reg_name_search does a binary search of the given register table
   to see if "name" is a valid regiter name.  Returns the register
   number from the array on success, or -1 on failure. */

static int
reg_name_search (regs, regcount, name)
     const struct reg_name * regs;
     int                     regcount;
     const char *            name;
{
  int middle, low, high;
  int cmp;

  low = 0;
  high = regcount - 1;

  do
    {
      middle = (low + high) / 2;
      cmp = strcasecmp (name, regs[middle].name);
      if (cmp < 0)
	high = middle - 1;
      else if (cmp > 0)
	low = middle + 1;
      else
	return regs[middle].value;
    }
  while (low <= high);
  return -1;
}


/* Summary of register_name().
 *
 * in: Input_line_pointer points to 1st char of operand.
 *
 * out: A expressionS.
 *	The operand may have been a register: in this case, X_op == O_register,
 *	X_add_number is set to the register number, and truth is returned.
 *	Input_line_pointer->(next non-blank) char after operand, or is in
 *	its original state.
 */
static boolean
register_name (expressionP)
     expressionS * expressionP;
{
  int    reg_number;
  char * name;
  char * start;
  char   c;

  /* Find the spelling of the operand */
  start = name = input_line_pointer;

  c = get_symbol_end ();

  reg_number = reg_name_search (pre_defined_registers, REG_NAME_CNT, name);

  * input_line_pointer = c;	/* put back the delimiting char */
  
  /* look to see if it's in the register table */
  if (reg_number >= 0) 
    {
      expressionP->X_op         = O_register;
      expressionP->X_add_number = reg_number;

      /* make the rest nice */
      expressionP->X_add_symbol = NULL;
      expressionP->X_op_symbol  = NULL;
      
      return true;
    }
  else
    {
      /* reset the line as if we had not done anything */
      input_line_pointer = start;
      
      return false;
    }
}

/* Summary of system_register_name().
 *
 * in: Input_line_pointer points to 1st char of operand.
 *
 * out: A expressionS.
 *	The operand may have been a register: in this case, X_op == O_register,
 *	X_add_number is set to the register number, and truth is returned.
 *	Input_line_pointer->(next non-blank) char after operand, or is in
 *	its original state.
 */
static boolean
system_register_name (expressionP, accept_numbers)
     expressionS * expressionP;
     boolean       accept_numbers;
{
  int    reg_number;
  char * name;
  char * start;
  char   c;

  /* Find the spelling of the operand */
  start = name = input_line_pointer;

  c = get_symbol_end ();
  reg_number = reg_name_search (system_registers, SYSREG_NAME_CNT, name);

  * input_line_pointer = c;   /* put back the delimiting char */
  
  if (reg_number < 0
      && accept_numbers)
    {
      input_line_pointer   = start; /* reset input_line pointer */

      if (isdigit (* input_line_pointer))
	reg_number = strtol (input_line_pointer, & input_line_pointer, 10);

      /* Make sure that the register number is allowable. */
      if (   reg_number < 0
	  || reg_number > 5
/* start-sanitize-v850e */
	  && reg_number < 16
	  || reg_number > 20
/* end-sanitize-v850e */
	     )
	{
	  reg_number = -1;
	}
    }
      
  /* look to see if it's in the register table */
  if (reg_number >= 0) 
    {
      expressionP->X_op         = O_register;
      expressionP->X_add_number = reg_number;

      /* make the rest nice */
      expressionP->X_add_symbol = NULL;
      expressionP->X_op_symbol  = NULL;

      return true;
    }
  else
    {
      /* reset the line as if we had not done anything */
      input_line_pointer = start;
      
      return false;
    }
}

/* Summary of cc_name().
 *
 * in: Input_line_pointer points to 1st char of operand.
 *
 * out: A expressionS.
 *	The operand may have been a register: in this case, X_op == O_register,
 *	X_add_number is set to the register number, and truth is returned.
 *	Input_line_pointer->(next non-blank) char after operand, or is in
 *	its original state.
 */
static boolean
cc_name (expressionP)
     expressionS * expressionP;
{
  int    reg_number;
  char * name;
  char * start;
  char   c;

  /* Find the spelling of the operand */
  start = name = input_line_pointer;

  c = get_symbol_end ();
  reg_number = reg_name_search (cc_names, CC_NAME_CNT, name);

  * input_line_pointer = c;   /* put back the delimiting char */
  
  /* look to see if it's in the register table */
  if (reg_number >= 0) 
    {
      expressionP->X_op         = O_constant;
      expressionP->X_add_number = reg_number;

      /* make the rest nice */
      expressionP->X_add_symbol = NULL;
      expressionP->X_op_symbol  = NULL;

      return true;
    }
  else
    {
      /* reset the line as if we had not done anything */
      input_line_pointer = start;
      
      return false;
    }
}

static void
skip_white_space (void)
{
  while (   * input_line_pointer == ' '
	 || * input_line_pointer == '\t')
    ++ input_line_pointer;
}

/* start-sanitize-v850e */
/* Summary of parse_register_list ().
 *
 * in: Input_line_pointer  points to 1st char of a list of registers.
 *     insn                is the partially constructed instruction.
 *     operand             is the operand being inserted.
 *
 * out: True if the parse completed successfully, False otherwise.
 *      If the parse completes the correct bit fields in the
 *      instruction will be filled in.
 *
 * Parses register lists with the syntax:
 *
 *   { rX }
 *   { rX, rY }
 *   { rX - rY }
 *   { rX - rY, rZ }
 *   etc
 *
 * and also parses constant epxressions whoes bits indicate the
 * registers in the lists.  The LSB in the expression refers to
 * the lowest numbered permissable register in the register list,
 * and so on upwards.  System registers are considered to be very
 * high numbers.
 * 
 */
static char *
parse_register_list
(
  unsigned long *             insn,
  const struct v850_operand * operand
)
{
  static int  type1_regs[ 32 ] = { 30,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 31, 29, 28, 23, 22, 21, 20, 27, 26, 25, 24 };
/* start-sanitize-v850eq */
  static int  type2_regs[ 32 ] = { 19, 18, 17, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31, 29, 28, 23, 22, 21, 20, 27, 26, 25, 24 };
  static int  type3_regs[ 32 ] = {  3,  2,  1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 13, 12,  7,  6,  5,  4, 11, 10,  9,  8 };
/* end-sanitize-v850eq */
  int *       regs;
  expressionS exp;


  /* Select a register array to parse. */
  switch (operand->shift)
    {
    case 0xffe00001: regs = type1_regs; break;
/* start-sanitize-v850eq */
    case 0xfff8000f: regs = type2_regs; break;
    case 0xfff8001f: regs = type3_regs; break;
/* end-sanitize-v850eq */
    default:
      as_bad ("unknown operand shift: %x\n", operand->shift );		    
      return "internal failure in parse_register_list";
    }

  skip_white_space();

  /* If the expression starts with a curly brace it is a register list.
     Otherwise it is a constant expression ,whoes bits indicate which
     registers are to be included in the list.  */
  
  if (* input_line_pointer != '{')
    {
      int bits;
      int reg;
      int i;
		
      expression (& exp);
      
      if (exp.X_op != O_constant)
	return "constant expression or register list expected";

/* start-sanitize-v850eq */
      if (regs == type1_regs)
/* end-sanitize-v850eq */
	{
	  if (exp.X_add_number & 0xFFFFF000)
	    return "high bits set in register list expression";
	  
	  for (reg = 20; reg < 32; reg ++)
	    if (exp.X_add_number & (1 << (reg - 20)))
	      {
		for (i = 0; i < 32; i++)
		  if (regs[i] == reg)
		    * insn |= (1 << i);
	      }
	}
/* start-sanitize-v850eq */
      else if (regs == type2_regs)
	{
	  if (exp.X_add_number & 0xFFFE0000)
	    return "high bits set in register list expression";
	  
	  for (reg = 1; reg < 16; reg ++)
	    if (exp.X_add_number & (1 << (reg - 1)))
	      {
		for (i = 0; i < 32; i++)
		  if (regs[i] == reg)
		    * insn |= (1 << i);
	      }

	  if (exp.X_add_number & (1 << 15))
	    * insn |= (1 << 3);
	  
	  if (exp.X_add_number & (1 << 16))
	    * insn |= (1 << 19);
	}
      else /* regs == type3_regs */
	{
	  if (exp.X_add_number & 0xFFFE0000)
	    return "high bits set in register list expression";
	  
	  for (reg = 16; reg < 32; reg ++)
	    if (exp.X_add_number & (1 << (reg - 16)))
	      {
		for (i = 0; i < 32; i++)
		  if (regs[i] == reg)
		    * insn |= (1 << i);
	      }

	  if (exp.X_add_number & (1 << 16))
	    * insn |= (1 << 19);
	}
/* end-sanitize-v850eq */

      return NULL;
    }

  input_line_pointer ++;

  /* Parse the register list until a terminator (closing curly brace or new-line) is found.  */
  for (;;)
    {
      if (register_name (& exp))
	{
	  int  i;
	  
	  /* Locate the given register in the list, and if it is there, insert the corresponding bit into the instruction.  */
	  for (i = 0; i < 32; i++)
	    {
	      if (regs[ i ] == exp.X_add_number)
		{
		  * insn |= (1 << i);
		  break;
		}
	    }

	  if (i == 32)
	    {
	      return "illegal register included in list";
	    }
	}
      else if (system_register_name (& exp, true))
	{
	  if (regs == type1_regs)
	    {
	      return "system registers cannot be included in list";
	    }
	  else if (exp.X_add_number == 5)
	    {
	      if (regs == type2_regs)
		return "PSW cannot be included in list";
	      else
		* insn |= 0x8;
	    }
	  else
	    * insn |= 0x80000;
	}
      else if (* input_line_pointer == '}')
	{
	  input_line_pointer ++;
	  break;
	}
      else if (* input_line_pointer == ',')
	{
	  input_line_pointer ++;
	  continue;
	}
      else if (* input_line_pointer == '-')
	{
	  /* We have encountered a range of registers: rX - rY */
	  int         j;
	  expressionS exp2;

	  /* Skip the dash.  */
	  ++ input_line_pointer;

	  /* Get the second register in the range.  */
	  if (! register_name (& exp2))
	    {
	      return "second register should follow dash in register list";
	      exp2.X_add_number = exp.X_add_number;
	    }

	  /* Add the rest of the registers in the range.  */
	  for (j = exp.X_add_number + 1; j <= exp2.X_add_number; j++)
	    {
	      int  i;
	  
	      /* Locate the given register in the list, and if it is there, insert the corresponding bit into the instruction.  */
	      for (i = 0; i < 32; i++)
		{
		  if (regs[ i ] == j)
		    {
		      * insn |= (1 << i);
		      break;
		    }
		}

	      if (i == 32)
		{
		  return "illegal register included in list";
		}
	    }
	}
      else
	{
	  break;
	}

      skip_white_space();
    }

  return NULL;
}
/* end-sanitize-v850e */

CONST char * md_shortopts = "m:";

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof md_longopts; 


void
md_show_usage (stream)
  FILE * stream;
{
  fprintf (stream, "V850 options:\n");
  fprintf (stream, "\t-wsigned_overflow    Warn if signed immediate values overflow\n");
  fprintf (stream, "\t-wunsigned_overflow  Warn if unsigned immediate values overflow\n");
  fprintf (stream, "\t-mv850               The code is targeted at the v850\n");
/* start-sanitize-v850e */
  fprintf (stream, "\t-mv850e              The code is targeted at the v850e\n");
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
  fprintf (stream, "\t-mv850eq             The code is targeted at the v850eq\n");
/* end-sanitize-v850eq */
} 

int
md_parse_option (c, arg)
     int    c;
     char * arg;
{
  switch (c)
    {
    case 'w':
      if (strcmp (arg, "signed_overflow") == 0)
	{
	  warn_signed_overflows = TRUE;
	  return 1;
	}
      else if (strcmp (arg, "unsigned_overflow") == 0)
	{
	  warn_unsigned_overflows = TRUE;
	  return 1;
	}
      break;

    case 'm':
      if (strcmp (arg, "v850") == 0)
	{
	  machine = 0;
	  return 1;
	}
/* start-sanitize-v850e */
      else if (strcmp (arg, "v850e") == 0)
	{
	  machine = bfd_mach_v850e;
	  processor_mask = PROCESSOR_V850 | PROCESSOR_V850E;
	  
	  return 1;
	}
/* end-sanitize-v850e */
/* start-sanitize-v850eq */
      else if (strcmp (arg, "v850eq") == 0)
	{
	  machine = bfd_mach_v850eq;
	  processor_mask = PROCESSOR_V850EQ;
	  return 1;
	}
/* end-sanitize-v850eq */
      break;
    }
  
  return 0;
}

symbolS *
md_undefined_symbol (name)
  char * name;
{
  return 0;
}

char *
md_atof (type, litp, sizep)
  int    type;
  char * litp;
  int *  sizep;
{
  int            prec;
  LITTLENUM_TYPE words[4];
  char *         t;
  int            i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizep = 0;
      return "bad call to md_atof";
    }
  
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizep = prec * 2;

  for (i = prec - 1; i >= 0; i--)
    {
      md_number_to_chars (litp, (valueT) words[i], 2);
      litp += 2;
    }

  return NULL;
}


/* Very gross.  */
void
md_convert_frag (abfd, sec, fragP)
  bfd *      abfd;
  asection * sec;
  fragS *    fragP;
{
  subseg_change (sec, 0);
  if (fragP->fr_subtype == 0)
    {
      fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
	       fragP->fr_offset, 1, BFD_RELOC_UNUSED + (int)fragP->fr_opcode);
      fragP->fr_var = 0;
      fragP->fr_fix += 2;
    }
  else if (fragP->fr_subtype == 1)
    {
      /* Reverse the condition of the first branch.  */
      fragP->fr_literal[0] &= 0xf7;
      /* Mask off all the displacement bits.  */
      fragP->fr_literal[0] &= 0x8f;
      fragP->fr_literal[1] &= 0x07;
      /* Now set the displacement bits so that we branch
	 around the unconditional branch.  */
      fragP->fr_literal[0] |= 0x30;

      /* Now create the unconditional branch + fixup to the final
	 target.  */
      md_number_to_chars (&fragP->fr_literal[2], 0x00000780, 4);
      fix_new (fragP, fragP->fr_fix + 2, 4, fragP->fr_symbol,
	       fragP->fr_offset, 1, BFD_RELOC_UNUSED + (int)fragP->fr_opcode + 1);
      fragP->fr_var = 0;
      fragP->fr_fix += 6;
    }
  else
    abort ();
}

valueT
md_section_align (seg, addr)
     asection * seg;
     valueT     addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}

void
md_begin ()
{
  char *                              prev_name = "";
  register const struct v850_opcode * op;
  flagword                            applicable;

  
  v850_hash = hash_new();

  /* Insert unique names into hash table.  The V850 instruction set
     has many identical opcode names that have different opcodes based
     on the operands.  This hash table then provides a quick index to
     the first opcode with a particular name in the opcode table.  */

  op = v850_opcodes;
  while (op->name)
    {
      if (strcmp (prev_name, op->name)) 
	{
	  prev_name = (char *) op->name;
	  hash_insert (v850_hash, op->name, (char *) op);
	}
      op++;
    }

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, machine);

  applicable = bfd_applicable_section_flags (stdoutput);

  sdata_section = subseg_new (".sdata", 0);
  bfd_set_section_flags (stdoutput, sdata_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA | SEC_HAS_CONTENTS));
  
  tdata_section = subseg_new (".tdata", 0);
  bfd_set_section_flags (stdoutput, tdata_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA | SEC_HAS_CONTENTS));
  
  zdata_section = subseg_new (".zdata", 0);
  bfd_set_section_flags (stdoutput, zdata_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA | SEC_HAS_CONTENTS));
  
  sbss_section = subseg_new (".sbss", 0);
  bfd_set_section_flags (stdoutput, sbss_section, applicable & SEC_ALLOC);
  
  tbss_section = subseg_new (".tbss", 0);
  bfd_set_section_flags (stdoutput, tbss_section, applicable & SEC_ALLOC);
  
  zbss_section = subseg_new (".zbss", 0);
  bfd_set_section_flags (stdoutput, zbss_section, applicable & SEC_ALLOC);
  
  rosdata_section = subseg_new (".rosdata", 0);
  bfd_set_section_flags (stdoutput, rosdata_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY));
			 
  rozdata_section = subseg_new (".rozdata", 0);
  bfd_set_section_flags (stdoutput, rozdata_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY));

/* start-sanitize-v850e */
  call_table_data_section = subseg_new (".call_table_data", 0);
  bfd_set_section_flags (stdoutput, call_table_data_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA | SEC_HAS_CONTENTS));
  
  call_table_text_section = subseg_new (".call_table_text", 0);
  bfd_set_section_flags (stdoutput, call_table_text_section, applicable & (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE));
/* end-sanitize-v850e */
}


/* start-sanitize-v850e */
static bfd_reloc_code_real_type
handle_ctoff (const struct v850_operand * operand)
{
  if (operand == NULL)
    return BFD_RELOC_V850_CALLT_16_16_OFFSET;

  assert (operand->bits == 6);
  assert (operand->shift == 0);
      
  return BFD_RELOC_V850_CALLT_6_7_OFFSET;
}
/* end-sanitize-v850e */

static bfd_reloc_code_real_type
handle_sdaoff (const struct v850_operand * operand)
{
  if (operand == NULL)                             return BFD_RELOC_V850_SDA_16_16_OFFSET;
  if (operand->bits == 15 && operand->shift == 17) return BFD_RELOC_V850_SDA_15_16_OFFSET;
  /* start-sanitize-v850e */
  if (operand->bits == -1)                         return BFD_RELOC_V850_SDA_16_16_SPLIT_OFFSET;
  /* end-sanitize-v850e */
  
  assert (operand->bits == 16);
  assert (operand->shift == 16);
  
  return BFD_RELOC_V850_SDA_16_16_OFFSET;
}

static bfd_reloc_code_real_type
handle_zdaoff (const struct v850_operand * operand)
{
  if (operand == NULL)                             return BFD_RELOC_V850_ZDA_16_16_OFFSET;
  if (operand->bits == 15 && operand->shift == 17) return BFD_RELOC_V850_ZDA_15_16_OFFSET;
  /* start-sanitize-v850e */
  if (operand->bits == -1)                         return BFD_RELOC_V850_ZDA_16_16_SPLIT_OFFSET;
  /* end-sanitize-v850e */
  
  assert (operand->bits == 16);
  assert (operand->shift == 16);
  
  return BFD_RELOC_V850_ZDA_16_16_OFFSET;
}

static bfd_reloc_code_real_type
handle_tdaoff (const struct v850_operand * operand)
{
  if (operand == NULL)                               return BFD_RELOC_V850_TDA_7_7_OFFSET;  /* data item, not an instruction.  */
  if (operand->bits == 6 && operand->shift == 1)     return BFD_RELOC_V850_TDA_6_8_OFFSET;  /* sld.w/sst.w, operand: D8_6  */
  /* start-sanitize-v850e */
  if (operand->bits == 4 && operand->insert != NULL) return BFD_RELOC_V850_TDA_4_5_OFFSET;  /* sld.hu, operand: D5-4 */
  if (operand->bits == 4 && operand->insert == NULL) return BFD_RELOC_V850_TDA_4_4_OFFSET;  /* sld.bu, operand: D4   */
  /* end-sanitize-v850e */
  if (operand->bits == 16 && operand->shift == 16)   return BFD_RELOC_V850_TDA_16_16_OFFSET; /* set1 & chums, operands: D16 */
  
  assert (operand->bits == 7);
  
  return  operand->insert != NULL
    ? BFD_RELOC_V850_TDA_7_8_OFFSET     /* sld.h/sst.h, operand: D8_7 */
    : BFD_RELOC_V850_TDA_7_7_OFFSET;    /* sld.b/sst.b, opreand: D7   */
}

/* Warning: The code in this function relies upon the definitions
   in the v850_operands[] array (defined in opcodes/v850-opc.c)
   matching the hard coded values contained herein.  */

static bfd_reloc_code_real_type
v850_reloc_prefix (const struct v850_operand * operand)
{
  boolean paren_skipped = false;


  /* Skip leading opening parenthesis.  */
  if (* input_line_pointer == '(')
    {
      ++ input_line_pointer;
      paren_skipped = true;
    }

#define CHECK_(name, reloc) 						\
  if (strncmp (input_line_pointer, name##"(", strlen (name) + 1) == 0)	\
    {									\
      input_line_pointer += strlen (name);				\
      return reloc;							\
    }
  
  CHECK_ ("hi0",    BFD_RELOC_HI16);
  CHECK_ ("hi",     BFD_RELOC_HI16_S);
  CHECK_ ("lo",     BFD_RELOC_LO16);
  CHECK_ ("sdaoff", handle_sdaoff (operand));
  CHECK_ ("zdaoff", handle_zdaoff (operand));
  CHECK_ ("tdaoff", handle_tdaoff (operand));

/* start-sanitize-v850e */
  CHECK_ ("hilo", BFD_RELOC_32);
  CHECK_ ("ctoff",  handle_ctoff (operand));
/* end-sanitize-v850e */
  
  /* Restore skipped parenthesis.  */
  if (paren_skipped)
    -- input_line_pointer;
  
  return BFD_RELOC_UNUSED;
}

void
md_assemble (str) 
     char * str;
{
  char *                    s;
  char *                    start_of_operands;
  struct v850_opcode *      opcode;
  struct v850_opcode *      next_opcode;
  const unsigned char *     opindex_ptr;
  int                       next_opindex;
  int                       relaxable;
  unsigned long             insn;
  unsigned long             insn_size;
  char *                    f;
  int                       i;
  int                       match;
  boolean                   extra_data_after_insn = false;
  unsigned                  extra_data_len;
  unsigned long             extra_data;
  char *		    saved_input_line_pointer;
  
  /* Get the opcode.  */
  for (s = str; *s != '\0' && ! isspace (*s); s++)
    continue;
  
  if (*s != '\0')
    *s++ = '\0';

  /* find the first opcode with the proper name */
  opcode = (struct v850_opcode *)hash_find (v850_hash, str);
  if (opcode == NULL)
    {
      as_bad ("Unrecognized opcode: `%s'", str);
      ignore_rest_of_line ();
      return;
    }

  str = s;
  while (isspace (* str))
    ++ str;

  start_of_operands = str;

  saved_input_line_pointer = input_line_pointer;
  
  for (;;)
    {
      const char * errmsg = NULL;

      match = 0;
      
      if ((opcode->processors & processor_mask) == 0)
	{
	  errmsg = "Target processor does not support this instruction.";
	  goto error;
	}
      
      relaxable = 0;
      fc = 0;
      next_opindex = 0;
      insn = opcode->opcode;
      extra_data_after_insn = false;

      input_line_pointer = str = start_of_operands;

      for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr++)
	{
	  const struct v850_operand * operand;
	  char *                      hold;
	  expressionS                 ex;
	  bfd_reloc_code_real_type    reloc;

	  if (next_opindex == 0)
	    {
	      operand = & v850_operands[ * opindex_ptr ];
	    }
	  else
	    {
	      operand      = & v850_operands[ next_opindex ];
	      next_opindex = 0;
	    }

	  errmsg = NULL;

	  while (*str == ' ' || *str == ',' || *str == '[' || *str == ']')
	    ++str;

	  if (operand->flags & V850_OPERAND_RELAX)
	    relaxable = 1;

	  /* Gather the operand. */
	  hold = input_line_pointer;
	  input_line_pointer = str;
	  
/* fprintf (stderr, "operand: %s   index = %d, opcode = %s\n", input_line_pointer, opindex_ptr - opcode->operands, opcode->name ); */

	  /* lo(), hi(), hi0(), etc... */
	  if ((reloc = v850_reloc_prefix (operand)) != BFD_RELOC_UNUSED)
	    {
	      expression (& ex);

	      if (ex.X_op == O_constant)
		{
		  switch (reloc)
		    {
		    case BFD_RELOC_LO16:
		      {
			/* Truncate, then sign extend the value.  */
			ex.X_add_number = SEXT16 (ex.X_add_number);
			break;
		      }

		    case BFD_RELOC_HI16:
		      {
			/* Truncate, then sign extend the value.  */
			ex.X_add_number = SEXT16 (ex.X_add_number >> 16);
			break;
		      }

		    case BFD_RELOC_HI16_S:
		      {
			/* Truncate, then sign extend the value.  */
		        int temp = (ex.X_add_number >> 16) & 0xffff;

			temp += (ex.X_add_number >> 15) & 1;

			ex.X_add_number = SEXT16 (temp);
			break;
		      }
		    
/* start-sanitize-v850e */
		    case BFD_RELOC_32:
		      if ((operand->flags & V850E_IMMEDIATE32) == 0)
			{
			  errmsg = "use bigger instruction";
			  goto error;
			}
		      
		      extra_data_after_insn = true;
		      extra_data_len        = 4;
		      extra_data            = ex.X_add_number;
		      ex.X_add_number       = 0;
		      break;
/* end-sanitize-v850e */
			 
		    default:
		      as_bad ( "AAARG -> unhandled constant reloc");
		      break;
		    }

		  insn = v850_insert_operand (insn, operand, ex.X_add_number,
					      (char *) NULL, 0);
		}
	      else
		{
		  if (reloc == BFD_RELOC_32)
		    {
		      if ((operand->flags & V850E_IMMEDIATE32) == 0)
			{
			  errmsg = "use bigger instruction";
			  goto error;
			}
		      
		      extra_data_after_insn = true;
		      extra_data_len        = 4;
		      extra_data            = ex.X_add_number;
		      ex.X_add_number       = 0;
		    }
		      
		  if (fc > MAX_INSN_FIXUPS)
		    as_fatal ("too many fixups");

		  fixups[ fc ].exp     = ex;
		  fixups[ fc ].opindex = * opindex_ptr;
		  fixups[ fc ].reloc   = reloc;
		  fc++;
		}
	    }
	  else
	    {
	      errmsg = NULL;
	      
	      if ((operand->flags & V850_OPERAND_REG) != 0) 
		{
		  if (!register_name (& ex))
		    {
		      errmsg = "invalid register name";
		    }

		  if ((operand->flags & V850_NOT_R0)
		      && ex.X_add_number == 0)
		    {
		      errmsg = "register r0 cannot be used here";
		    }
		}
	      else if ((operand->flags & V850_OPERAND_SRG) != 0) 
		{
		  if (!system_register_name (& ex, true))
		    {
		      errmsg = "invalid system register name";
		    }
		}
	      else if ((operand->flags & V850_OPERAND_EP) != 0)
		{
		  char * start = input_line_pointer;
		  char   c     = get_symbol_end ();
		  
		  if (strcmp (start, "ep") != 0 && strcmp (start, "r30") != 0)
		    {
		      /* Put things back the way we found them.  */
		      *input_line_pointer = c;
		      input_line_pointer = start;
		      errmsg = "expected EP register";
		      goto error;
		    }
		  
		  *input_line_pointer = c;
		  str = input_line_pointer;
		  input_line_pointer = hold;
	      
		  while (*str == ' ' || *str == ',' || *str == '[' || *str == ']')
		    ++str;
		  continue;
		}
	      else if ((operand->flags & V850_OPERAND_CC) != 0) 
		{
		  if (!cc_name (& ex))
		    {
		      errmsg = "invalid condition code name";
		    }
		}
/* start-sanitize-v850e */
	      else if (operand->flags & V850E_PUSH_POP) 
		{
		  errmsg = parse_register_list (& insn, operand);
		  
		  /* The parse_register_list() function has already done everything, so fake a dummy expression.  */
		  ex.X_op         = O_constant;
		  ex.X_add_number = 0;
		}
	      else if (operand->flags & V850E_IMMEDIATE16) 
		{
		  expression (& ex);

		  if (ex.X_op != O_constant)
		    errmsg = "constant expression expected";
		  else if (ex.X_add_number & 0xffff0000)
		    {
		      if (ex.X_add_number & 0xffff)
			errmsg = "constant too big to fit into instruction";
		      else if ((insn & 0x001fffc0) == 0x00130780)
			ex.X_add_number >>= 16;
		      else
			errmsg = "constant too big to fit into instruction";
		    }
		  
		  extra_data_after_insn = true;
		  extra_data_len        = 2;
		  extra_data            = ex.X_add_number;
		  ex.X_add_number       = 0;
		}
	      else if (operand->flags & V850E_IMMEDIATE32) 
		{
		  expression (& ex);
		  
		  if (ex.X_op != O_constant)
		    errmsg = "constant expression expected";
		  
		  extra_data_after_insn = true;
		  extra_data_len        = 4;
		  extra_data            = ex.X_add_number;
		  ex.X_add_number       = 0;
		}
/* end-sanitize-v850e */
	      else if (register_name (&ex)
		       && (operand->flags & V850_OPERAND_REG) == 0)
		{
		  errmsg = "syntax error: register not expected";
		}
	      else if (system_register_name (& ex, false)
		       && (operand->flags & V850_OPERAND_SRG) == 0)
		{
		  errmsg = "syntax error: system register not expected";
		}
	      else if (cc_name (&ex)
		       && (operand->flags & V850_OPERAND_CC) == 0)
		{
		  errmsg = "syntax error: condition code not expected";
		}
	      else
		{
		  expression (& ex);
/* start-sanitize-v850e */
		  /* Special case:
		     If we are assembling a MOV instruction (or a CALLT.... :-)
		     and the immediate value does not fit into the bits available
		     then create a fake error so that the next MOV instruction
		      will be selected.  This one has a 32 bit immediate field.  */

		  if (((insn & 0x07e0) == 0x0200)
		      && ex.X_op == O_constant
		      && (ex.X_add_number < (- (1 << (operand->bits - 1))) || ex.X_add_number > ((1 << operand->bits) - 1)))
		    errmsg = "use bigger instruction";
/* end-sanitize-v850e */
		}

	      if (errmsg)
		goto error;
	      
/* fprintf (stderr, "insn: %x, operand %d, op: %d, add_number: %d\n", insn, opindex_ptr - opcode->operands, ex.X_op, ex.X_add_number ); */

	      switch (ex.X_op) 
		{
		case O_illegal:
		  errmsg = "illegal operand";
		  goto error;
		case O_absent:
		  errmsg = "missing operand";
		  goto error;
		case O_register:
		  if ((operand->flags & (V850_OPERAND_REG | V850_OPERAND_SRG)) == 0)
		    {
		      errmsg = "invalid operand";
		      goto error;
		    }
		  insn = v850_insert_operand (insn, operand, ex.X_add_number,
					      (char *) NULL, 0);
		  break;

		case O_constant:
		  insn = v850_insert_operand (insn, operand, ex.X_add_number,
					      (char *) NULL, 0);
		  break;

		default:
		  /* We need to generate a fixup for this expression.  */
		  if (fc >= MAX_INSN_FIXUPS)
		    as_fatal ("too many fixups");

		  fixups[ fc ].exp     = ex;
		  fixups[ fc ].opindex = * opindex_ptr;
		  fixups[ fc ].reloc   = BFD_RELOC_UNUSED;
		  ++fc;
		  break;
		}
	    }

	  str = input_line_pointer;
	  input_line_pointer = hold;

	  while (*str == ' ' || *str == ',' || *str == '[' || *str == ']'
		 || *str == ')')
	    ++str;
	}
      match = 1;

    error:
      if (match == 0)
        {
	  next_opcode = opcode + 1;
	  if (next_opcode->name != NULL && strcmp (next_opcode->name, opcode->name) == 0)
	    {
	      opcode = next_opcode;
	      continue;
	    }
	  
	  as_bad (errmsg);
	  ignore_rest_of_line ();
	  input_line_pointer = saved_input_line_pointer;
	  return;
        }
      break;
    }
      
  while (isspace (*str))
    ++str;

  if (*str != '\0')
    as_bad ("junk at end of line: `%s'", str);

  input_line_pointer = str;

  /* Write out the instruction.

     Four byte insns have an opcode with the two high bits on.  */ 
  if (relaxable && fc > 0)
    {
      f = frag_var (rs_machine_dependent, 6, 4, 0,
		    fixups[0].exp.X_add_symbol,
		    fixups[0].exp.X_add_number,
		    (char *)fixups[0].opindex);
      insn_size = 2;
      md_number_to_chars (f, insn, insn_size);
      md_number_to_chars (f + 2, 0, 4);
      fc = 0;
    }
  else 
    {
      if ((insn & 0x0600) == 0x0600)
	insn_size = 4;
      else
	insn_size = 2;

/* start-sanitize-v850e */
      /* Special case: 32 bit MOV */
      if ((insn & 0xffe0) == 0x0620)
	insn_size = 2;
/* end-sanitize-v850e */
      
      f = frag_more (insn_size);
      
      md_number_to_chars (f, insn, insn_size);

      if (extra_data_after_insn)
	{
	  f = frag_more (extra_data_len);
	  
	  md_number_to_chars (f, extra_data, extra_data_len);

	  extra_data_after_insn = false;
	}
    }

  /* Create any fixups.  At this point we do not use a
     bfd_reloc_code_real_type, but instead just use the
     BFD_RELOC_UNUSED plus the operand index.  This lets us easily
     handle fixups for any operand type, although that is admittedly
     not a very exciting feature.  We pick a BFD reloc type in
     md_apply_fix.  */  
  for (i = 0; i < fc; i++)
    {
      const struct v850_operand * operand;
      bfd_reloc_code_real_type    reloc;
      
      operand = & v850_operands[ fixups[i].opindex ];

      reloc = fixups[i].reloc;
      
      if (reloc != BFD_RELOC_UNUSED)
	{
	  reloc_howto_type * reloc_howto = bfd_reloc_type_lookup (stdoutput, reloc);
	  int                size;
	  int                address;
	  fixS *             fixP;

	  if (!reloc_howto)
	    abort();
	  
	  size = bfd_get_reloc_size (reloc_howto);

	  if (size != 2 && size != 4) /* XXX this will abort on an R_V850_8 reloc - is this reloc actually used ? */
	    abort();

	  address = (f - frag_now->fr_literal) + insn_size - size;

	  if (reloc == BFD_RELOC_32)
	    {
	      address += 2;
	    }
	  
	  fixP = fix_new_exp (frag_now, address, size,
			      & fixups[i].exp, 
			      reloc_howto->pc_relative,
			      reloc);

	  switch (reloc)
	    {
	    case BFD_RELOC_LO16:
	    case BFD_RELOC_HI16:
	    case BFD_RELOC_HI16_S:
	      fixP->fx_no_overflow = 1;
	      break;
	    }
	}
      else
	{
	  fix_new_exp (
		       frag_now,
		       f - frag_now->fr_literal, 4,
		       & fixups[i].exp,
		       1 /* FIXME: V850_OPERAND_RELATIVE ??? */,
		       (bfd_reloc_code_real_type) (fixups[i].opindex + (int) BFD_RELOC_UNUSED)
		       );
	}
    }

  input_line_pointer = saved_input_line_pointer;
}


/* If while processing a fixup, a reloc really needs to be created */
/* then it is done here.  */
                 
arelent *
tc_gen_reloc (seg, fixp)
     asection * seg;
     fixS *     fixp;
{
  arelent * reloc;
  
  reloc              = (arelent *) xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = & fixp->fx_addsy->bsym;
  reloc->address     = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto       = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);

  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
                    "reloc %d not supported by object file format", (int)fixp->fx_r_type);

      xfree (reloc);
      
      return NULL;
    }
  
  reloc->addend = fixp->fx_addnumber;
  
  return reloc;
}

/* Assume everything will fit in two bytes, then expand as necessary.  */
int
md_estimate_size_before_relax (fragp, seg)
     fragS * fragp;
     asection * seg;
{
  fragp->fr_var = 4;
  return 2;
} 

long
md_pcrel_from (fixp)
     fixS * fixp;
{
  /* If the symbol is undefined, or in a section other than our own,
     then let the linker figure it out.  */
  if (fixp->fx_addsy != (symbolS *) NULL && ! S_IS_DEFINED (fixp->fx_addsy))
    {
      /* The symbol is undefined.  Let the linker figure it out.  */
      return 0;
    }
  return fixp->fx_frag->fr_address + fixp->fx_where;
}

int
md_apply_fix3 (fixp, valuep, seg)
     fixS *   fixp;
     valueT * valuep;
     segT     seg;
{
  valueT value;
  char * where;

  if (fixp->fx_addsy == (symbolS *) NULL)
    {
      value = * valuep;
      fixp->fx_done = 1;
    }
  else if (fixp->fx_pcrel)
    value = * valuep;
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
			    "expression too complex");
	    }
	}
    }

  if ((int) fixp->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      int                         opindex;
      const struct v850_operand * operand;
      unsigned long               insn;

      opindex = (int) fixp->fx_r_type - (int) BFD_RELOC_UNUSED;
      operand = & v850_operands[ opindex ];

      /* Fetch the instruction, insert the fully resolved operand
         value, and stuff the instruction back again.

	 Note the instruction has been stored in little endian
	 format!  */
      where = fixp->fx_frag->fr_literal + fixp->fx_where;

      insn = bfd_getl32 ((unsigned char *) where);
      insn = v850_insert_operand (insn, operand, (offsetT) value,
				  fixp->fx_file, fixp->fx_line);
      bfd_putl32 ((bfd_vma) insn, (unsigned char *) where);

      if (fixp->fx_done)
	{
	  /* Nothing else to do here. */
	  return 1;
	}

      /* Determine a BFD reloc value based on the operand information.  
	 We are only prepared to turn a few of the operands into relocs. */

      if (operand->bits == 22)
	fixp->fx_r_type = BFD_RELOC_V850_22_PCREL;
      else if (operand->bits == 9)
	fixp->fx_r_type = BFD_RELOC_V850_9_PCREL;
      else
	{
	  /* fprintf (stderr, "bits: %d, insn: %x\n", operand->bits, insn); */
	  
	  as_bad_where(fixp->fx_file, fixp->fx_line,
		       "unresolved expression that must be resolved");
	  fixp->fx_done = 1;
	  return 1;
	}
    }
  else if (fixp->fx_done)
    {
      /* We still have to insert the value into memory!  */
      where = fixp->fx_frag->fr_literal + fixp->fx_where;
      if (fixp->fx_size == 1)
	*where = value & 0xff;
      else if (fixp->fx_size == 2)
	bfd_putl16 (value & 0xffff, (unsigned char *) where);
      else if (fixp->fx_size == 4)
	bfd_putl32 (value, (unsigned char *) where);
    }
  
  fixp->fx_addnumber = value;
  return 1;
}


/* Insert an operand value into an instruction.  */

static unsigned long
v850_insert_operand (insn, operand, val, file, line)
     unsigned long insn;
     const struct v850_operand * operand;
     offsetT val;
     char *file;
     unsigned int line;
{
  if (operand->insert)
    {
      const char * message = NULL;
      
      insn = (*operand->insert) (insn, val, & message);
      if (message != NULL)
	{
	  if (file == (char *) NULL)
	    as_warn (message);
	  else
	    as_warn_where (file, line, message);
	}
    }
  else
    {
      if (operand->bits != 32)
	{
	  long    min, max;
	  offsetT test;

	  if ((operand->flags & V850_OPERAND_SIGNED) != 0)
	    {
	      if (! warn_signed_overflows)
		max = (1 << operand->bits) - 1;
	      else
		max = (1 << (operand->bits - 1)) - 1;
	      
	      min = - (1 << (operand->bits - 1));
	    }
	  else
	    {
	      max = (1 << operand->bits) - 1;
	      
	      if (! warn_unsigned_overflows)
		min = - (1 << (operand->bits - 1));
	      else
		min = 0;
	    }
	  
	  test = val;
	  
	  if (test < (offsetT) min || test > (offsetT) max)
	    {
	      const char * err = "operand out of range (%s not between %ld and %ld)";
	      char         buf[100];
	      
	      sprint_value (buf, test);
	      if (file == (char *) NULL)
		as_warn (err, buf, min, max);
	      else
		as_warn_where (file, line, err, buf, min, max);
	    }
	}

      insn |= (((long) val & ((1 << operand->bits) - 1)) << operand->shift);
    }
  
  return insn;
}

/* Parse a cons expression.  We have to handle hi(), lo(), etc
   on the v850.  */
void
parse_cons_expression_v850 (exp)
  expressionS *exp;
{
  /* See if there's a reloc prefix like hi() we have to handle.  */
  hold_cons_reloc = v850_reloc_prefix (NULL);

  /* Do normal expression parsing.  */
  expression (exp);
}

/* Create a fixup for a cons expression.  If parse_cons_expression_v850
   found a reloc prefix, then we use that reloc, else we choose an
   appropriate one based on the size of the expression.  */
void
cons_fix_new_v850 (frag, where, size, exp)
     fragS *frag;
     int where;
     int size;
     expressionS *exp;
{
  if (hold_cons_reloc == BFD_RELOC_UNUSED)
    {
      if (size == 4)
	hold_cons_reloc = BFD_RELOC_32;
      if (size == 2)
	hold_cons_reloc = BFD_RELOC_16;
      if (size == 1)
	hold_cons_reloc = BFD_RELOC_8;
    }

  if (exp != NULL)
    fix_new_exp (frag, where, size, exp, 0, hold_cons_reloc);
  else
    fix_new (frag, where, size, NULL, 0, 0, hold_cons_reloc);
}
