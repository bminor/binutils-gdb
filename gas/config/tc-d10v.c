/* tc-d10v.c -- Assembler code for the Mitsubishi D10V

   Copyright (C) 1996 Free Software Foundation.

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
#include "opcode/d10v.h"
#include "elf/ppc.h"

const char comment_chars[] = "#;";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = "";
const char *md_shortopts = "";
const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";


/* fixups */
#define MAX_INSN_FIXUPS (5)
struct d10v_fixup
{
  expressionS exp;
  bfd_reloc_code_real_type reloc;
};

typedef struct _fixups
{
  int fc;
  struct d10v_fixup fix[MAX_INSN_FIXUPS];
  struct _fixups *next;
} Fixups;

static Fixups FixUps[2];
static Fixups *fixups;

/* local functions */
static int reg_name_search PARAMS ((char *name));
static int register_name PARAMS ((expressionS *expressionP));
static int check_range PARAMS ((unsigned long num, int bits, int flags));
static int postfix PARAMS ((char *p));
static bfd_reloc_code_real_type get_reloc PARAMS ((struct d10v_operand *op));
static int get_operands PARAMS ((expressionS exp[]));
static unsigned long build_insn PARAMS ((struct d10v_opcode *opcode, expressionS *opers, unsigned long insn));
static void write_long PARAMS ((struct d10v_opcode *opcode, unsigned long insn, Fixups *fx));
static void write_1_short PARAMS ((struct d10v_opcode *opcode, unsigned long insn, Fixups *fx));
static int write_2_short PARAMS ((struct d10v_opcode *opcode1, unsigned long insn1, 
				  struct d10v_opcode *opcode2, unsigned long insn2, int exec_type, Fixups *fx));
static unsigned long do_assemble PARAMS ((char *str, struct d10v_opcode **opcode));
static unsigned long d10v_insert_operand PARAMS (( unsigned long insn, int op_type,
						   offsetT value, int left));


struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);       

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { NULL,       NULL,           0 }
};

/* Opcode hash table.  */
static struct hash_control *d10v_hash;

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
  long min, max, bit1;
  int retval=0;

  if (flags & OPERAND_SHIFT)
    {
      /* all special shift operands are unsigned */
      /* and <= 16.  We allow 0 for now. */
      if (num>16)
	return 1;
      else
	return 0;
    }

  if (flags & OPERAND_SIGNED)
    {
      bit1 = (1 << (bits - 1)); 
      max = bit1 -1;
      if (num & max)
	{
	  /* sign-extend */
	  num = ((num & (bit1 | max))^(~max))+bit1;
	}

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
  fprintf(stream, "D10V options:\n\
none yet\n");
} 

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  return 0;
}

symbolS *
md_undefined_symbol (name)
  char *name;
{
  return 0;
}

char *
md_atof (type, litp, sizep)
  int type;
char *litp;
int *sizep;
{
  return "";
}

void
md_convert_frag (abfd, sec, fragP)
  bfd *abfd;
  asection *sec;
  fragS *fragP;
{
  printf ("call to md_convert_frag \n");
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
  char *prev_name = "";
  struct d10v_opcode *opcode;
  d10v_hash = hash_new();

  /* Insert unique names into hash table.  The D10v instruction set
     has many identical opcode names that have different opcodes based
     on the operands.  This hash table then provides a quick index to
     the first opcode with a particular name in the opcode table.  */

  for (opcode = (struct d10v_opcode *)d10v_opcodes; opcode->name; opcode++)
    {
      if (strcmp (prev_name, opcode->name))
	{
	  prev_name = (char *)opcode->name;
	  hash_insert (d10v_hash, opcode->name, (char *) opcode);
	}
    }

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
      if (*p==0 || *p=='\n' || *p=='\r') 
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
get_reloc (op) 
     struct d10v_operand *op;
{
  int bits = op->bits;

  /*  printf("get_reloc:  bits=%d  address=%d\n",bits,op->flags & OPERAND_ADDR);   */
  if (bits <= 4) 
    return (0);
      
  if (op->flags & OPERAND_ADDR) 
    {
      if (bits == 8)
	return (BFD_RELOC_D10V_10_PCREL_R);
      else
	  return (BFD_RELOC_D10V_18_PCREL);
    }

  return (BFD_RELOC_16);
}

/* get_operands parses a string of operands and returns
   an array of expressions */

static int
get_operands (exp) 
     expressionS exp[];
{
  char *p = input_line_pointer;
  int numops = 0;
  int post = 0;

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
	as_bad ("illegal operand");
      else if (exp[numops].X_op == O_absent) 
	as_bad ("missing operand");

      numops++;
      p = input_line_pointer;
    }

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

  exp[numops].X_op = 0;
  return (numops);
}

static unsigned long
d10v_insert_operand (insn, op_type, value, left) 
     unsigned long insn;
     int op_type;
     offsetT value;
     int left;
{
  int shift, bits;

  shift = d10v_operands[op_type].shift;
  if (left)
    shift += 15;

  bits = d10v_operands[op_type].bits;

  /* truncate to the proper number of bits */
  if (check_range (value, bits, d10v_operands[op_type].flags))
    as_bad("operand out of range: %d",value);

  value &= 0x7FFFFFFF >> (31 - bits);
  insn |= (value << shift);

  return insn;
}


/* build_insn takes a pointer to the opcode entry in the opcode table
   and the array of operand expressions and returns the instruction */

static unsigned long
build_insn (opcode, opers, insn) 
     struct d10v_opcode *opcode;
     expressionS *opers;
     unsigned long insn;
{
  int i, bits, shift, flags, format;
  unsigned int number;
  
  /* the insn argument is only used for the DIVS kludge */
  if (insn)
    format = LONG_R;
  else
    {
      insn = opcode->opcode;
      format = opcode->format;
    }
  
  for (i=0;opcode->operands[i];i++) 
    {
      flags = d10v_operands[opcode->operands[i]].flags;
      bits = d10v_operands[opcode->operands[i]].bits;
      shift = d10v_operands[opcode->operands[i]].shift;
      number = opers[i].X_add_number;

      if (flags & OPERAND_REG) 
	{
	  number &= REGISTER_MASK;
	  if (format == LONG_L)
	    shift += 15;
	}

      if (opers[i].X_op != O_register && opers[i].X_op != O_constant) 
	{
	  /* now create a fixup */

	  /*
	  printf("need a fixup: ");
	  print_expr_1(stdout,&opers[i]);
	  printf("\n");
	  */

	  if (fixups->fc >= MAX_INSN_FIXUPS)
	    as_fatal ("too many fixups");
	  fixups->fix[fixups->fc].exp = opers[i];

	  /* put the operand number here for now.  We can look up
	     the reloc type and/or fixup the instruction in md_apply_fix() */
	  fixups->fix[fixups->fc].reloc = opcode->operands[i];
	  (fixups->fc)++;
	}

      /* truncate to the proper number of bits */
      if ((opers[i].X_op == O_constant) && check_range (number, bits, flags))
	as_bad("operand out of range: %d",number);
      number &= 0x7FFFFFFF >> (31 - bits);
      insn = insn | (number << shift);
    }

  /* kludge: for DIVS, we need to put the operands in twice */
  /* on the second pass, format is changed to LONG_R to force */
  /* the second set of operands to not be shifted over 15 */
  if ((opcode->opcode == OPCODE_DIVS) && (format==LONG_L))
    insn = build_insn (opcode, opers, insn);
      
  return insn;
}

/* write out a long form instruction */
static void
write_long (opcode, insn, fx) 
     struct d10v_opcode *opcode;
     unsigned long insn;
     Fixups *fx;
{
  int i;
  char *f = frag_more(4);

  insn |= FM11;
  /* printf("INSN: %08x\n",insn); */
  number_to_chars_bigendian (f, insn, 4);

  for (i=0; i < fx->fc; i++) 
    {
      if (get_reloc((struct d10v_operand *)&d10v_operands[fx->fix[i].reloc]))
	{ 
	  /*
	  printf("fix_new_exp: where:%x size:4\n    ",f - frag_now->fr_literal);
	  print_expr_1(stdout,&(fx->fix[i].exp));
	  printf("\n");
	  */

	  fix_new_exp (frag_now,
		       f - frag_now->fr_literal, 
		       4,
		       &(fx->fix[i].exp),
		       1,
		       fx->fix[i].reloc);
	}
    }
  fx->fc = 0;
}


/* write out a short form instruction by itself */
static void
write_1_short (opcode, insn, fx) 
     struct d10v_opcode *opcode;
     unsigned long insn;
     Fixups *fx;
{
  char *f = frag_more(4);
  int i;

  if (opcode->exec_type == PARONLY)
    as_fatal ("Instruction must be executed in parallel with another instruction.");

  /* the other container needs to be NOP */
  /* according to 4.3.1: for FM=00, sub-instructions performed only
     by IU cannot be encoded in L-container. */
  if (opcode->unit == IU)
    insn |= FM00 | (NOP << 15);		/* right container */
  else
    insn = FM00 | (insn << 15) | NOP;	/* left container */

  /*  printf("INSN: %08x\n",insn);  */
  number_to_chars_bigendian (f, insn, 4);
  for (i=0; i < fx->fc; i++) 
    {
      bfd_reloc_code_real_type reloc;
      reloc = get_reloc((struct d10v_operand *)&d10v_operands[fx->fix[i].reloc]);
      if (reloc)
	{ 
	  /*
	  printf("fix_new_exp: where:%x size:4\n    ",f - frag_now->fr_literal);
	  print_expr_1(stdout,&(fx->fix[i].exp));
	  printf("\n");
	  */

	  /* if it's an R reloc, we may have to switch it to L */
	  if ( (reloc == BFD_RELOC_D10V_10_PCREL_R) && (opcode->unit != IU) )
	    fx->fix[i].reloc |= 1024;

	  fix_new_exp (frag_now,
		       f - frag_now->fr_literal, 
		       4,
		       &(fx->fix[i].exp),
		       1,
		       fx->fix[i].reloc);
	}
    }
  fx->fc = 0;
}

/* write out a short form instruction if possible */
/* return number of instructions not written out */
static int
write_2_short (opcode1, insn1, opcode2, insn2, exec_type, fx) 
     struct d10v_opcode *opcode1, *opcode2;
     unsigned long insn1, insn2;
     int exec_type;
     Fixups *fx;
{
  unsigned long insn;
  char *f;
  int i,j;

  if ( (exec_type != 1) && ((opcode1->exec_type == PARONLY)
	                || (opcode2->exec_type == PARONLY)))
    as_fatal("Instruction must be executed in parallel");
  
  if ( (opcode1->format & LONG_OPCODE) || (opcode2->format & LONG_OPCODE))
    as_fatal ("Long instructions may not be combined.");

  if(opcode1->exec_type == BRANCH_LINK)
    {
      /* subroutines must be called from 32-bit boundaries */
      /* so the return address will be correct */
      write_1_short (opcode1, insn1, fx->next);
      return (1);
    }

  switch (exec_type) 
    {
    case 0:
      if (opcode1->unit == IU) 
	{
	  /* reverse sequential */
	  insn = FM10 | (insn2 << 15) | insn1;
	}
      else
	{
	  insn = FM01 | (insn1 << 15) | insn2;
	  fx = fx->next;  
	}
      break;
    case 1:	/* parallel */
      if (opcode1->exec_type == SEQ || opcode2->exec_type == SEQ)
	as_fatal ("One of these instructions may not be executed in parallel.");

      if (opcode1->unit == IU)
	{
	  if (opcode2->unit == IU)
	    as_fatal ("Two IU instructions may not be executed in parallel");
	  as_warn ("Swapping instruction order");
 	  insn = FM00 | (insn2 << 15) | insn1;
	  fx = fx->next;
	}
      else if (opcode2->unit == MU)
	{
	  if (opcode1->unit == MU)
	    as_fatal ("Two MU instructions may not be executed in parallel");
	  as_warn ("Swapping instruction order");
	  insn = FM00 | (insn2 << 15) | insn1;
	  fx = fx->next;
	}
      else
	insn = FM00 | (insn1 << 15) | insn2;  
      fx = fx->next;
      break;
    case 2:	/* sequential */
      if (opcode1->unit == IU)
	as_fatal ("IU instruction may not be in the left container");
      insn = FM01 | (insn1 << 15) | insn2;  
      fx = fx->next;
      break;
    case 3:	/* reverse sequential */
      if (opcode2->unit == MU)
	as_fatal ("MU instruction may not be in the right container");
      insn = FM10 | (insn1 << 15) | insn2;  
      break;
    default:
      as_fatal("unknown execution type passed to write_2_short()");
    }

  /*  printf("INSN: %08x\n",insn); */
  f = frag_more(4);
  number_to_chars_bigendian (f, insn, 4);

for (j=0; j<2; j++) 
  {
    bfd_reloc_code_real_type reloc;
    for (i=0; i < fx->fc; i++) 
      {
	reloc = get_reloc((struct d10v_operand *)&d10v_operands[fx->fix[i].reloc]);
	if (reloc)
	  {
	    if ( (reloc == BFD_RELOC_D10V_10_PCREL_R) && (j == 0) )
	      fx->fix[i].reloc |= 1024;
	    
	    /*
	    printf("fix_new_exp: where:%x reloc:%d\n    ",f - frag_now->fr_literal,fx->fix[i].reloc);
	    print_expr_1(stdout,&(fx->fix[i].exp));
	    printf("\n");
	    */
	    fix_new_exp (frag_now,
			 f - frag_now->fr_literal, 
			 4,
			 &(fx->fix[i].exp),
			 1,
			 fx->fix[i].reloc);
	  }
      }
    fx->fc = 0;
    fx = fx->next;
  }

  return (0);
}


/* This is the main entry point for the machine-dependent assembler.  str points to a
   machine-dependent instruction.  This function is supposed to emit the frags/bytes 
   it assembles to.  For the D10V, it mostly handles the special VLIW parsing and packing
   and leaves the difficult stuff to do_assemble().
 */

static unsigned long prev_insn;
static struct d10v_opcode *prev_opcode = 0;
static subsegT prev_subseg;
static segT prev_seg;

void
md_assemble (str)
     char *str;
{
  struct d10v_opcode *opcode;
  unsigned long insn;
  int extype=0;			/* execution type; parallel, etc */
  static int etype=0;		/* saved extype.  used for multiline instructions */
  char *str2;

  /*  printf("md_assemble: str=%s\n",str); */

  if (etype == 0)
    {
      /* look for the special multiple instruction separators */
      str2 = strstr (str, "||");
      if (str2) 
	extype = 1;
      else
	{
	  str2 = strstr (str, "->");
	  if (str2) 
	    extype = 2;
	  else
	    {
	      str2 = strstr (str, "<-");
	      if (str2) 
		extype = 3;
	    }
	}
      /* str2 points to the separator, if one */
      if (str2) 
	{
	  *str2 = 0;
	  
	  /* if two instructions are present and we already have one saved
	     then first write it out */
	  if (prev_opcode) 
	    write_1_short (prev_opcode, prev_insn, fixups->next);
	  
	  /* assemble first instruction and save it */
	  prev_insn = do_assemble (str, &prev_opcode);
	  if (prev_insn == -1)
	    as_fatal ("can't find opcode ");
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
      as_fatal ("can't find opcode ");
    }

  if (etype)
    {
      extype = etype;
      etype = 0;
    }

  /* if this is a long instruction, write it and any previous short instruction */
  if (opcode->format & LONG_OPCODE) 
    {
      if (extype) 
	as_fatal("Unable to mix instructions as specified");
      if (prev_opcode) 
	{
	  write_1_short (prev_opcode, prev_insn, fixups->next);
	  prev_opcode = NULL;
	}
      write_long (opcode, insn, fixups);
      prev_opcode = NULL;
      return;
    }
  
  if (prev_opcode && (write_2_short (prev_opcode, prev_insn, opcode, insn, extype, fixups) == 0)) 
    {
      /* no instructions saved */
      prev_opcode = NULL;
    }
  else
    {
      if (extype) 
	as_fatal("Unable to mix instructions as specified");
      /* save off last instruction so it may be packed on next pass */
      prev_opcode = opcode;
      prev_insn = insn;
      prev_seg = now_seg;
      prev_subseg = now_subseg;
      fixups = fixups->next;
    }
}


/* do_assemble assembles a single instruction and returns an opcode */
/* it returns -1 (an invalid opcode) on error */

static unsigned long
do_assemble (str, opcode) 
     char *str;
     struct d10v_opcode **opcode;
{
  struct d10v_opcode *next_opcode;
  unsigned char *op_start, *save;
  unsigned char *op_end;
  char name[20];
  int nlen = 0, i, match, numops;
  expressionS myops[6];
  unsigned long insn;

  /* printf("do_assemble: str=%s\n",str); */

  /* Drop leading whitespace */
  while (*str == ' ')
    str++;

  /* find the opcode end */
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
    return (-1);
  
  /* find the first opcode with the proper name */
  *opcode = (struct d10v_opcode *)hash_find (d10v_hash, name);
  if (*opcode == NULL)
      as_fatal ("unknown opcode: %s",name);

  save = input_line_pointer;
  input_line_pointer = op_end;

  /* get all the operands and save them as expressions */
  numops = get_operands (myops);

  /* now see if the operand is a fake.  If so, find the correct size */
  /* instruction, if possible */
  match = 0;
  if ((*opcode)->format == OPCODE_FAKE)
    {
      int opnum = (*opcode)->operands[0];
      if (myops[opnum].X_op == O_constant)
	{
	  next_opcode=(*opcode)+1;
	  for (i=0; (*opcode)->operands[i+1]; i++)
	    {
	      int bits = d10v_operands[next_opcode->operands[opnum]].bits;
	      int flags = d10v_operands[next_opcode->operands[opnum]].flags;
	      if (!check_range (myops[opnum].X_add_number, bits, flags))
		{
		  match = 1;
		  break;
		}
	      next_opcode++;
	    }
	}
      else
	{
	  /* not a constant, so use a long instruction */
	  next_opcode = (*opcode)+2;
	  match = 1;
	}
      if (match)
	*opcode = next_opcode;
      else
	as_fatal ("value out of range");
    }
  else
    {
      /* now search the opcode table table for one with operands */
      /* that matches what we've got */
      while (!match)
	{
	  match = 1;
	  for (i = 0; (*opcode)->operands[i]; i++) 
	    {
	      int flags = d10v_operands[(*opcode)->operands[i]].flags;
	      int X_op = myops[i].X_op;
	      int num = myops[i].X_add_number;
	      
	      if (X_op==0) 
		{
		  match=0;
		  break;
		}
	      
	      if (flags & OPERAND_REG) 
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
	  
	  /* we're only done if the operands matched AND there
	     are no more to check */
	  if (match && myops[i].X_op==0) 
	    break;
	  
	  next_opcode = (*opcode)+1;
	  if (next_opcode->opcode == 0) 
	    break;
	  if (strcmp(next_opcode->name, (*opcode)->name))
	    break;
	  (*opcode) = next_opcode;
	}
    }

  if (!match)  
    {
      as_bad ("bad opcode or operands");
      return (0);
    }

  /* Check that all registers that are required to be even are. */
  /* Also, if any operands were marked as registers, but were really symbols */
  /* fix that here. */
  for (i=0; (*opcode)->operands[i]; i++) 
    {
      if ((d10v_operands[(*opcode)->operands[i]].flags & OPERAND_EVEN) &&
	  (myops[i].X_add_number & 1)) 
	as_fatal("Register number must be EVEN");
      if (myops[i].X_op == O_register)
	{
	  if (!(d10v_operands[(*opcode)->operands[i]].flags & OPERAND_REG)) 
	    {
	      myops[i].X_op = O_symbol;
	      myops[i].X_add_symbol = symbol_find_or_make ((char *)myops[i].X_op_symbol);
	      myops[i].X_add_number = 0;
	      myops[i].X_op_symbol = NULL;
	    }
	}
    }
  
  input_line_pointer = save;

  /* at this point, we have "opcode" pointing to the opcode entry in the
     d10v opcode table, with myops filled out with the operands. */
  insn = build_insn ((*opcode), myops, 0); 
  /* printf("sub-insn = %lx\n",insn); */

  return (insn);
}


/* if while processing a fixup, a reloc really needs to be created */
/* then it is done here */
                 
arelent *
tc_gen_reloc (seg, fixp)
     asection *seg;
     fixS *fixp;
{
  arelent *reloc;
  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
                    "reloc %d not supported by object file format", (int)fixp->fx_r_type);
      return NULL;
    }
  reloc->addend = fixp->fx_addnumber;
  /*  printf("tc_gen_reloc: addr=%x  addend=%x\n", reloc->address, reloc->addend); */
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
    return 0;
    /*  return fixp->fx_frag->fr_address + fixp->fx_where; */
}

int
md_apply_fix3 (fixp, valuep, seg)
     fixS *fixp;
     valueT *valuep;
     segT seg;
{
  char *where;
  unsigned long insn;
  long value;
  int op_type;
  int left=0;

  if (fixp->fx_addsy == (symbolS *) NULL)
    {
      value = *valuep;
      fixp->fx_done = 1;
    }
  else if (fixp->fx_pcrel)
    value = *valuep;
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
  
  /* printf("md_apply_fix: value=0x%x  type=%d\n",  value, fixp->fx_r_type);  */

  op_type = fixp->fx_r_type;
  if (op_type & 1024)
    {
      op_type -= 1024;
      fixp->fx_r_type = BFD_RELOC_D10V_10_PCREL_L;
      left = 1;
    }
  else
    fixp->fx_r_type = get_reloc((struct d10v_operand *)&d10v_operands[op_type]); 

  /* Fetch the instruction, insert the fully resolved operand
     value, and stuff the instruction back again.  */
  where = fixp->fx_frag->fr_literal + fixp->fx_where;
  insn = bfd_getb32 ((unsigned char *) where);

  switch (fixp->fx_r_type)
    {
    case BFD_RELOC_D10V_10_PCREL_L:
    case BFD_RELOC_D10V_10_PCREL_R:
    case BFD_RELOC_D10V_18_PCREL:
      /* instruction addresses are always right-shifted by 2 
         and pc-relative */
      if (!fixp->fx_pcrel)
	value -= fixp->fx_where;
      value >>= 2;
    default:
      break;
    }
  /* printf("   insn=%x  value=%x where=%x  pcrel=%x\n",insn,value,fixp->fx_where,fixp->fx_pcrel); */ 
 
  insn = d10v_insert_operand (insn, op_type, (offsetT)value, left);
  
  /* printf("   new insn=%x\n",insn); */
  
  bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
  
  if (fixp->fx_done)
    return 1;

  fixp->fx_addnumber = value;
  return 1;
}


/* d10v_cleanup() is called after the assembler has finished parsing the input 
   file or after a label is defined.  Because the D10V assembler sometimes saves short 
   instructions to see if it can package them with the next instruction, there may
   be a short instruction that still needs written.  */
int
d10v_cleanup (done)
     int done;
{
  segT seg;
  subsegT subseg;

  if ( prev_opcode && (done  || (now_seg == prev_seg) && (now_subseg == prev_subseg)))
    {
      seg = now_seg;
      subseg = now_subseg;
      subseg_set (prev_seg, prev_subseg);
      write_1_short (prev_opcode, prev_insn, fixups);
      subseg_set (seg, subseg);
      prev_opcode = NULL;
    }
  return 1;
}
