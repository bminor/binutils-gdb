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

/* local functions */
static int reg_name_search PARAMS ((char *name));
static void register_name PARAMS ((expressionS *expressionP));
static int postfix PARAMS ((char *p));
static bfd_reloc_code_real_type get_reloc PARAMS ((struct d10v_operand *op));
static int get_operands PARAMS ((expressionS exp[]));
static unsigned long build_insn PARAMS ((struct d10v_opcode *opcode, expressionS *opers));
static void write_long PARAMS ((struct d10v_opcode *opcode, unsigned long insn));
static void write_1_short PARAMS ((struct d10v_opcode *opcode, unsigned long insn));
static int write_2_short PARAMS ((struct d10v_opcode *opcode1, unsigned long insn1, 
				  struct d10v_opcode *opcode2, unsigned long insn2, int exec_type));
static unsigned long do_assemble PARAMS ((char *str, struct d10v_opcode **opcode));

/* fixups */
#define MAX_INSN_FIXUPS (5)
struct d10v_fixup
{
  expressionS exp;
  int opindex;
  bfd_reloc_code_real_type reloc;
};
struct d10v_fixup fixups[MAX_INSN_FIXUPS];
static int fc;

struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);       

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  /*
  { "byte",     ppc_byte,       0 },
  { "long",     ppc_elf_cons,   4 },
  { "word",     ppc_elf_cons,   2 },
  { "short",    ppc_elf_cons,   2 },
  { "rdata",    ppc_elf_rdata,  0 },
  { "rodata",   ppc_elf_rdata,  0 },
  { "lcomm",    ppc_elf_lcomm,  0 },
  */
  { NULL,       NULL,           0 }
};

/* Opcode hash table.  */
static struct hash_control *d10v_hash;

/* Structure to hold information about predefined registers.  */
struct pd_reg
{
  char *name;
  int value;
};


/* an expressionS only has one register type, so we fake it */
/* by setting high bits to indicate type */
#define REGISTER_MASK		0xFF

/*   The table is sorted. Suitable for searching by a binary search. */
static const struct pd_reg pre_defined_registers[] =
{
  { "a0", OPERAND_ACC+0 },
  { "a1", OPERAND_ACC+1 },
  { "bpc", OPERAND_CONTROL+3 },
  { "bpsw", OPERAND_CONTROL+1 },
  { "c", OPERAND_FLAG+3 },
  { "f0", OPERAND_FLAG+0 },
  { "f1", OPERAND_FLAG+1 },
  { "iba", OPERAND_CONTROL+14 },
  { "mod_e", OPERAND_CONTROL+11 },
  { "mod_s", OPERAND_CONTROL+10 },
  { "pc", OPERAND_CONTROL+2 },
  { "psw", OPERAND_CONTROL+0 },
  { "r0", 0 },
  { "r1", 1 },
  { "r10", 10 },
  { "r11", 11 },
  { "r12", 12 },
  { "r13", 13 },
  { "r14", 14 },
  { "r15", 15 },
  { "r2", 2 },
  { "r3", 3 },
  { "r4", 4 },
  { "r5", 5 },
  { "r6", 6 },
  { "r7", 7 },
  { "r8", 8 },
  { "r9", 9 },
  { "rpt_c", OPERAND_CONTROL+7 },
  { "rpt_e", OPERAND_CONTROL+9 },
  { "rpt_s", OPERAND_CONTROL+8 },
  { "sp", 15 },
};
#define REG_NAME_CNT	(sizeof(pre_defined_registers) / sizeof(struct pd_reg))

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
  high = REG_NAME_CNT - 1;

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


/* register_name check to see if an expression is a valid
   register name.  If it is, it marks the expression type 
   as O_register. */

static void
register_name (expressionP)
     expressionS *expressionP;
{
  int reg_number;
  char *name;

  if (expressionP->X_op == O_symbol) 
    {
      name = (char *)S_GET_NAME (expressionP->X_add_symbol);
      reg_number = reg_name_search (name);

      /* look to see if it's in the register table */
      if (reg_number >= 0) 
	{
	  expressionP->X_op = O_register;
	  expressionP->X_add_number = reg_number;
	}
    }
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
  /* printf ("call to md_convert_frag \n"); */
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
      /* create an expression */
      expression (&exp[numops]);
      /* check to see if it might be a register name */
      register_name (&exp[numops]);

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
d10v_insert_operand (insn, op_type, value) 
     unsigned long insn;
     int op_type;
     offsetT value;
{
  int shift, bits;

  shift = d10v_operands[op_type].shift;
  bits = d10v_operands[op_type].bits;
  /* truncate to the proper number of bits */
  /* FIXME: overflow checking here? */
  value &= 0x7FFFFFFF >> (31 - bits);
  insn |= (value << shift);

  return insn;
}


/* build_insn takes a pointer to the opcode entry in the opcode table
   and the array of operand expressions and returns the instruction */

static unsigned long
build_insn (opcode, opers) 
     struct d10v_opcode *opcode;
     expressionS *opers;
{
  int i, bits, shift, flags;
  unsigned long insn;
  unsigned int number;
  insn = opcode->opcode;

  for (i=0;opcode->operands[i];i++) 
    {
      flags = d10v_operands[opcode->operands[i]].flags;
      bits = d10v_operands[opcode->operands[i]].bits;
      shift = d10v_operands[opcode->operands[i]].shift;
      number = opers[i].X_add_number;

      if (flags & OPERAND_REG) 
	{
	  number &= REGISTER_MASK;
	  if (opcode->format == LONG_L)
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

	  if (fc >= MAX_INSN_FIXUPS)
	    as_fatal ("too many fixups");
	  fixups[fc].exp = opers[i];

	  /* put the operand number here for now.  We can look up
	     the reloc type and/or fixup the instruction in md_apply_fix() */
	  fixups[fc].reloc = opcode->operands[i];
	  fc++;
	}

      /* truncate to the proper number of bits */
      /* FIXME: overflow checking here? */
      number &= 0x7FFFFFFF >> (31 - bits);
      insn = insn | (number << shift);
    }
  return insn;
}

/* write out a long form instruction */
static void
write_long (opcode, insn) 
     struct d10v_opcode *opcode;
     unsigned long insn;
{
  int i;
  char *f = frag_more(4);

  insn |= FM11;
  /*  printf("INSN: %08x\n",insn); */
  number_to_chars_bigendian (f, insn, 4);

  for (i=0; i<fc; i++) 
    {
      if (get_reloc((struct d10v_operand *)&d10v_operands[fixups[i].reloc])) 
	{ 
	  /*
	  printf("fix_new_exp: where:%x size:4\n    ",f - frag_now->fr_literal);
	  print_expr_1(stdout,&fixups[i].exp);
	  printf("\n");
	  */
	  
	  fix_new_exp (frag_now,
		       f - frag_now->fr_literal, 
		       4,
		       &fixups[i].exp,
		       1,
		       fixups[i].reloc);
	}
    }
}

/* write out a short form instruction by itself */
static void
write_1_short (opcode, insn) 
     struct d10v_opcode *opcode;
     unsigned long insn;
{
  char *f = frag_more(4);
  int i;

  insn |= FM00 | (NOP << 15);
  /*  printf("INSN: %08x\n",insn); */
  number_to_chars_bigendian (f, insn, 4);
  for (i=0; i<fc; i++) 
    {
      if (get_reloc((struct d10v_operand *)&d10v_operands[fixups[i].reloc])) 
	{ 
	  /*
	  printf("fix_new_exp: where:%x size:4\n    ",f - frag_now->fr_literal);
	  print_expr_1(stdout,&fixups[i].exp);
	  printf("\n");
	  */

	  fix_new_exp (frag_now,
		       f - frag_now->fr_literal, 
		       4,
		       &fixups[i].exp,
		       1,
		       fixups[i].reloc);
	}
    }
}

/* write out a short form instruction if possible */
/* return number of instructions not written out */
static int
write_2_short (opcode1, insn1, opcode2, insn2, exec_type) 
     struct d10v_opcode *opcode1, *opcode2;
     unsigned long insn1, insn2;
     int exec_type;
{
  unsigned long insn;

  if(opcode1->exec_type == BRANCH_LINK)
    {
      /* subroutines must be called from 32-bit boundaries */
      /* so the return address will be correct */
      write_1_short (opcode1, insn1);
      return (1);
    }

  switch (exec_type) 
    {
    case 0:
      if (opcode1->unit == IU) 
	{
	  insn = FM10 | (insn2 << 15) | insn1;
	}
      else
	{
	  insn = FM01 | (insn1 << 15) | insn2;  
	}
      break;
    case 1:	/* parallel */
	  insn = FM00 | (insn1 << 15) | insn2;  
      break;
    case 2:	/* sequential */
	  insn = FM01 | (insn1 << 15) | insn2;  
      break;
    case 3:	/* reverse sequential */
	  insn = FM10 | (insn1 << 15) | insn2;  
      break;
    default:
      as_fatal("unknown execution type passed to write_2_short()");
    }

  /*  printf("INSN: %08x\n",insn); */
  number_to_chars_bigendian (frag_more(4), insn, 4);
  return (0);
}


/* This is the main entry point for the machine-dependent assembler.  str points to a
   machine-dependent instruction.  This function is supposed to emit the frags/bytes 
   it assembles to.  For the D10V, it mostly handles the special VLIW parsing and packing
   and leaves the difficult stuff to do_assemble().
 */

static unsigned long prev_insn;
static struct d10v_opcode *prev_opcode = 0;

void
md_assemble (str)
     char *str;
{
  struct d10v_opcode *opcode;
  unsigned long insn;
  int t=0;
  char *str2;

  /*  printf("md_assemble: str=%s\n",str); */
  fc = 0;
  
  /* look for the special multiple instruction seperators */
  str2 = strstr (str, "||");
  if (str2) 
    t = 1;
  else
    {
      str2 = strstr (str, "->");
      if (str2) 
	t = 2;
      else
	{
	  str2 = strstr (str, "<-");
	  if (str2) 
	    t = 3;
	}
    }


  /* str2 points to the seperator, if one */
  if (str2) 
    {
      *str2 = 0;

      /* if two instructions are present and we already have one saved
	 then first write it out */
      if (prev_opcode) 
	write_1_short (prev_opcode, prev_insn);
      
      /* assemble first instruction and save it */
      prev_insn = do_assemble (str, &prev_opcode);
      str = str2 + 2;
    }

  insn = do_assemble (str, &opcode);

  /* if this is a long instruction, write it and any previous short instruction */
  if (opcode->format & LONG_OPCODE) 
    {
      if (t) 
	as_fatal("Unable to mix instructions as specified");
      if (prev_opcode) 
	{
	  write_1_short (prev_opcode, prev_insn);
	  prev_opcode = NULL;
	}
      write_long (opcode, insn);
      prev_opcode = NULL;
      return;
    }
  
  if (prev_opcode && (write_2_short (prev_opcode, prev_insn, opcode, insn, t) == 0)) 
    {
      /* no instructions saved */
      prev_opcode = NULL;
    }
  else
    {
      if (t) 
	as_fatal("Unable to mix instructions as specified");
      /* save off last instruction so it may be packed on next pass */
      prev_opcode = opcode;
      prev_insn = insn;
    }
}


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

  /*  printf("do_assemble: str=%s\n",str);*/

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
      as_bad ("can't find opcode ");
  
  /* find the first opcode with the proper name */
  *opcode = (struct d10v_opcode *)hash_find (d10v_hash, name);
  if (*opcode == NULL)
    {
      as_bad ("unknown opcode");
      return;
    }

  save = input_line_pointer;
  input_line_pointer = op_end;

  /* get all the operands and save them as expressions */
  numops = get_operands (myops);

  /* now search the opcode table table for one with operands */
  /* that match what we've got */
  do
    {
      match = 1;
      for (i = 0; (*opcode)->operands[i]; i++) 
	{
	  int flags = d10v_operands[(*opcode)->operands[i]].flags;

	  if (myops[i].X_op==0) 
	    {
	      match=0;
	      break;
	    }

	  if (flags & OPERAND_REG) 
	    {
	      if ((myops[i].X_op != O_register) ||
		  ((flags & OPERAND_ACC) != (myops[i].X_add_number & OPERAND_ACC)) ||
		  ((flags & OPERAND_FLAG) != (myops[i].X_add_number & OPERAND_FLAG)) ||
		  ((flags & OPERAND_CONTROL) != (myops[i].X_add_number & OPERAND_CONTROL)))
		{
		  match=0;
		  break;
		}	  
	    }

	  if (((flags & OPERAND_MINUS) && ((myops[i].X_op != O_absent) || (myops[i].X_add_number != OPERAND_MINUS))) ||
	      ((flags & OPERAND_PLUS) && ((myops[i].X_op != O_absent) || (myops[i].X_add_number != OPERAND_PLUS))) ||
	      ((flags & OPERAND_ATMINUS) && ((myops[i].X_op != O_absent) || (myops[i].X_add_number != OPERAND_ATMINUS))) ||
	      ((flags & OPERAND_ATPAR) && ((myops[i].X_op != O_absent) || (myops[i].X_add_number != OPERAND_ATPAR))) ||
	      ((flags & OPERAND_ATSIGN) && ((myops[i].X_op != O_absent) || (myops[i].X_add_number != OPERAND_ATSIGN)))) 
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
    } while (!match);

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
	      myops[i].X_add_number = 0;
	      /* FIXME create a fixup */
	    }
	}
    }
  
  input_line_pointer = save;

  /* at this point, we have "opcode" pointing to the opcode entry in the
     d10v opcode table, with myops filled out with the operands. */
  insn = build_insn ((*opcode), myops); 
  /*  printf("sub-insn = %lx\n",insn); */

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
  valueT value;
  char *where;
  unsigned long insn;
  int op_type;

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
  
  /*   printf("md_apply_fix: value=0x%x  type=%d\n",  value, fixp->fx_r_type); */

  op_type = fixp->fx_r_type;
  fixp->fx_r_type = get_reloc((struct d10v_operand *)&d10v_operands[op_type]); 

  /*  printf("reloc=%d\n",fixp->fx_r_type); */

  /* Fetch the instruction, insert the fully resolved operand
     value, and stuff the instruction back again.  */
  where = fixp->fx_frag->fr_literal + fixp->fx_where;
  insn = bfd_getb32 ((unsigned char *) where);
  /* printf("   insn=%x  value=%x\n",insn,value); */

  insn = d10v_insert_operand (insn, op_type, (offsetT) value);
  
  /* printf("   new insn=%x\n",insn); */
  
  bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
  
  if (fixp->fx_done)
    return 1;

  fixp->fx_addnumber = value;
  return 1;
}

/* This is called after the assembler has finished parsing the input 
   file.  Because the D10V assembler sometimes saves short instructions
   to see if it can package them with the next instruction, there may
   be a short instruction that still needs written.  */

int md_after_pass()
{
  if (prev_opcode) 
    {
      write_1_short (prev_opcode, prev_insn);
      prev_opcode = NULL;
    }
}


/* there is a label to be defined.  Any saved instruction must
   be written out. */
int
start_label()
{
  if (prev_opcode) 
    {
      write_1_short (prev_opcode, prev_insn);
      prev_opcode = NULL;
    }
  return 1;
}
