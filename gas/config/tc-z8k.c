/* tc-z8k.c -- Assemble code for the Zilog Z800N
   Copyright (C) 1992 Free Software Foundation.
   
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
#define DEFINE_TABLE
#include "../opcodes/z8k-opc.h"

#include "as.h"
#include "bfd.h"
#include <ctype.h>
#include "listing.h"

char  comment_chars[]  = { ';',0 };
char line_separator_chars[] = { '$' ,0};

extern int machine;
extern int coff_flags;
int segmented_mode;
int  md_reloc_size ;

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
   */

void cons();


void s_segm()
{
  segmented_mode = 1;
  machine = bfd_mach_z8001;
  coff_flags = F_Z8001;
}

void s_unseg()
{
  segmented_mode = 0;
  machine = bfd_mach_z8002;
  coff_flags = F_Z8002;
}
const pseudo_typeS md_pseudo_table[] = 
{
{ "int",	cons,		2	},
{ "data.b",     cons, 		1      },
{ "data.w",     cons,		 2      },
{ "data.l",     cons, 		4      },
{ "form",     	listing_psize,	0      },
{ "heading", 	listing_title,	0},
{ "import", 	s_ignore, 	0},
{ "page", 	listing_eject,	0},
{ "program", 	s_ignore,	0},
{ "SEGM",       s_segm,         0},
{ "UNSEG",       s_unseg,         0},
{ 0,0,0 }
};


const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
char FLT_CHARS[] = "rRsSfFdDxXpP";


const relax_typeS md_relax_table[1];


static struct hash_control *opcode_hash_control;	/* Opcode mnemonics */



void md_begin () 
{
  opcode_entry_type *opcode;
  char *prev_name= "";
  int idx = 0;
	
  opcode_hash_control = hash_new();

	
  for (opcode = z8k_table; opcode->name; opcode++) 
  {
    /* Only enter unique codes into the table */
    char *src= opcode->name;

    if (strcmp(opcode->name, prev_name)) 
    {
      hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
      idx++;
    }
    opcode->idx = idx;
    prev_name = opcode->name;
  }
	
}


struct z8k_exp {
	char *e_beg;
	char *e_end;
	expressionS e_exp;
};
typedef struct z8k_op 
{
  char regsize;   /* 'b','w','r','q' */
  unsigned int reg; /* 0..15 */

  int mode;

  unsigned int x_reg;/* any other register associated with the mode */
  expressionS exp; /* any expression */
} op_type;



static op_type *da_address;
static op_type *imm_operand;

int the_cc;

char * 
DEFUN(whatreg,(reg, src),
      int *reg AND
      char *src)
{
  if (isdigit(src[1])) 
  {
    *reg = (src[0] - '0') * 10 +src[1] - '0';
    return src+2;
  }
  else
  {
    *reg = (src[0] - '0');
    return src+1;
  }
return 0;
}

/*
  parse operands	

  rh0-rh7, rl0-rl7
  r0-r15
  rr0-rr14
  rq0--rq12
  WREG r0,r1,r2,r3,r4,r5,r6,r7,fp,sp
  r0l,r0h,..r7l,r7h
  @WREG
  @WREG+
  @-WREG
  #const
  
  */


/* try and parse a reg name, returns number of chars consumed */
char*
DEFUN(parse_reg,(src, mode, reg),
      char *src AND
      int * mode AND
      unsigned int *reg)
{
  char *res = 0;
  if (src[0] == 'R') 
  {
    if (src[1] == 'R')
    {
      *mode = CLASS_REG_LONG;
      res =   whatreg(reg, src+2);
    }
    else  if (src[1] == 'H')
    {
      *mode = CLASS_REG_BYTE;
      res = whatreg(reg, src+2);
    }
    else if (src[1] == 'L')
    {
      *mode = CLASS_REG_BYTE;
      res = whatreg(reg, src+2);
    }
    else if (src[1] == 'Q')
    {
    * mode = CLASS_REG_QUAD;
      res = whatreg(reg, src+2);
    }
    else 
    {
      *mode = CLASS_REG_WORD;
      res = whatreg(reg, src+1);
    }
  }
  return res;


}

char *
DEFUN(parse_exp,(s, op),
      char *s AND
      expressionS *op)
{
	char *save = input_line_pointer;
	char *new;
	segT seg;
	input_line_pointer = s;
	seg = expr(0,op);
	new = input_line_pointer;
	input_line_pointer = save;
	if (SEG_NORMAL(seg)) 
	    return new;
	switch (seg) {
	case SEG_ABSOLUTE:
	case SEG_UNKNOWN:
	case SEG_DIFFERENCE:
	case SEG_BIG:
	case SEG_REGISTER:
		return new;
	case SEG_ABSENT:
		as_bad("Missing operand");
		return new;
	default:
		as_bad("Don't understand operand of type %s", segment_name (seg));
		return new;
	}
}


/* The many forms of operand:
   
   <rb>
   <r>
   <rr>
   <rq>
   @r
   #exp	
   exp
   exp(r)
   r(#exp)
   r(r)


   
   */

static
char *
DEFUN(checkfor,(ptr, what),
      char *ptr AND
      char what)
{
if (*ptr == what) ptr++;
else {
as_bad("expected %c", what);
}
return ptr;
}

/* Make sure the mode supplied is the size of a word */
static void 
DEFUN(regword,(mode, string),
      int mode AND 
      char *string)
{
  int ok;
  ok = CLASS_REG_WORD;
  if (ok != mode) 
  {  
    as_bad("register is wrong size for a word %s", string);
  }
}

/* Make sure the mode supplied is the size of an address */
static void 
DEFUN(regaddr,(mode, string),
      int mode AND
      char *string)
{
  int ok;
  ok = segmented_mode ? CLASS_REG_LONG : CLASS_REG_WORD;
  if (ok != mode) 
  {  
    as_bad("register is wrong size for address %s", string);
  }
}

struct cc_names {
int value;
char *name;


};

struct cc_names table[] = 
{
  0x0,"F",
  0x6,"Z",
  0xe,"NZ",
  0x7,"C",
  0xf,"NC",
  0xd,"PL",
  0x5,"MI",
  0xe,"NE",
  0x6,"EQ",
  0x4,"OV",
  0xc,"NOV",
  0x4,"PE",
  0xC,"PO",
  0x9,"GE",
  0x1,"LT",
  0xa,"GT",
  0x2,"LE",
  0xf,"UGE",
  0x7,"ULT",
  0xb,"UGT",
  0x3,"ULE",
  0,0
 };

static void
DEFUN(get_cc_operand,(ptr, mode, dst),
      char **ptr AND 
      struct z8k_op *mode AND 
      unsigned int dst)
{
  char *src = *ptr;
  int r;
  int i;
  mode->mode = CLASS_CC;
  for (i = 0; table[i].name; i++)
  {
    int j;
    for (j = 0; table[i].name[j]; j++) 
    {
      if (table[i].name[j] != src[j])
       goto fail;
    }
   the_cc = table[i].value;
    *ptr = src + j;
    return;
   fail:;
  }
the_cc = 0x8;
  return ;
}

static void 
DEFUN(get_operand,(ptr, mode, dst),
      char **ptr AND 
      struct z8k_op *mode AND 
      unsigned int dst)
{
  char *src = *ptr;
  char *end;
  unsigned   int num;
  unsigned  int len;
  unsigned int size;
  mode->mode = 0;

  if (*src == '#') 
  {
    mode->mode = CLASS_IMM;
    imm_operand = mode;
    src = parse_exp(src+1, &(mode->exp));
  }	
  else if (*src == '@') {
    int d;
    mode->mode = CLASS_IR;
    src= parse_reg(src+1, &d, &mode->reg);
  }
  else 
  {
    int reg;
    end = parse_reg(src, &mode->mode, &reg);

    if (end)
    {
      int nw, nr;
      src = end;
      if (*src == '(') 
      {
	src++;
	end = parse_reg(src, &nw, &nr);
	if (end) 
	{
	  /* Got Ra(Rb) */
	  src = end;

	  if (*src != ')')
	  {
	    as_bad("Missing ) in ra(rb)");
	  }
	  else
	  {
	    src++;
	  }
	  
	  regaddr(mode->mode,"ra(rb) ra");
	  regword(mode->mode,"ra(rb) rb");
	  mode->mode = CLASS_BX;
	  mode->reg = reg;
	  mode->x_reg = nr;

	}
	else 
	{
	  /* Got Ra(disp) */
	  if (*src == '#')
	   src++;
	  src = parse_exp(src, &(mode->exp));
	  src = checkfor(src, ')');
	  mode->mode = CLASS_BA;
	  mode->reg = reg;
	  mode->x_reg = 0;
	}
      }
      else
      {
	mode->reg = reg;
	mode->x_reg = 0;
      }
    }
    else 
    {
      /* No initial reg */
      src = parse_exp(src, &(mode->exp));
      if (*src == '(') 
      {
	src++;
	end = parse_reg(src, &(mode->mode), &reg);
	regword(mode->mode,"addr(Ra) ra");
	mode->mode = CLASS_X;
	mode->reg = reg;
	mode->x_reg =0;
	da_address = mode;
	src = checkfor(end, ')');
      }
      else 
      {
	/* Just an address */
	mode->mode = CLASS_DA;
	mode->reg = 0;
	mode->x_reg = 0;
	da_address = mode;
      }
    }
  }
  *ptr = src;
}

static
char *
DEFUN(get_operands,(opcode, op_end, operand),
      opcode_entry_type *opcode AND
      char *op_end AND
      op_type *operand) 
{
  char *ptr = op_end;
  switch (opcode->noperands) 
  {
   case 0:
    operand[0].mode = 0;
    operand[1].mode = 0;
    break;
		    
   case 1:    
    ptr++;
    get_operand(& ptr, operand +0,0);
    operand[1].mode =0;
    break;
		    
   case 2:
    ptr++;
    if (opcode->arg_info[0] == CLASS_CC) 
    {
      get_cc_operand(&ptr, operand+0,0);
    }
    else
    {
      get_operand(& ptr, operand +0,0);
    }
    if (*ptr == ',') ptr++;
    get_operand(& ptr, operand +1, 1);
    break;
		    
   default:
    abort();    
  }
	
	
  return ptr;    
}

/* Passed a pointer to a list of opcodes which use different
   addressing modes, return the opcode which matches the opcodes
   provided
   */

int reg[16];
expressionS disp;

static
opcode_entry_type *
DEFUN(get_specific,(opcode,  operands),
      opcode_entry_type *opcode AND
      op_type *operands)

{
  opcode_entry_type *this_try = opcode ;
  int found = 0;
  unsigned int noperands = opcode->noperands;
	
  unsigned int dispreg;
  unsigned int this_index = opcode->idx;

  while (this_index == opcode->idx && !found) 
  {
    unsigned int i;
		    
    this_try  = opcode ++;
    for (i = 0; i < noperands; i++) 
    {
      int mode = operands[i].mode;

      if ((mode&CLASS_MASK) != (this_try->arg_info[i] & CLASS_MASK)) goto fail;

      reg[this_try->arg_info[i] & ARG_MASK] = operands[i].reg;
    }

    found =1;
   fail: ;
  }
  if (found) 
   return this_try;
  else 
   return 0;
}

static void
DEFUN(check_operand,(operand, width, string),
	  struct z8k_op *operand AND
	  unsigned int width AND
	  char *string)
{
  if (operand->exp.X_add_symbol == 0 
      && operand->exp.X_subtract_symbol == 0)
  {
		    
    /* No symbol involved, let's look at offset, it's dangerous if any of
       the high bits are not 0 or ff's, find out by oring or anding with
       the width and seeing if the answer is 0 or all fs*/
    if ((operand->exp.X_add_number & ~width) != 0 &&
	(operand->exp.X_add_number | width)!= (~0))
    {
      as_warn("operand %s0x%x out of range.", string, operand->exp.X_add_number);
    }
  }
	
}

static void 
DEFUN(newfix,(ptr, type, operand),
      int ptr AND
      int type AND
      op_type *operand)
{

  fix_new(frag_now,
	  ptr,
	  1,
	  operand->exp.X_add_symbol,
	  operand->exp.X_subtract_symbol,
	  operand->exp.X_add_number,
	  0,
	  type);
}


/* Now we know what sort of opcodes it is, lets build the bytes -
 */
static void 
    DEFUN (build_bytes,(this_try, operand),
	   opcode_entry_type *this_try AND
	   struct z8k_op *operand)
{
  unsigned int i;
  char buffer[20];
  int length;
  char *output;
  char *output_ptr = buffer;
  char part;
  int c;
  char high;
  int nib;
  int nibble;
  unsigned short *class_ptr;
  length = this_try->length;
  if (segmented_mode && da_address) 
  {
    /* two more bytes when in segmented mode and using da address
       mode */
    length += 2;	
  }
  output  = frag_more(length);
memset(buffer, 20, 0);  
  class_ptr = this_try->byte_info;
 top: ;

  for (nibble = 0; c = *class_ptr++; nibble++)
  {

    switch (c & CLASS_MASK) 
    {
     default:
      abort();
     case CLASS_ADDRESS:
      /* Direct address, we don't cope with the SS mode right now */
      if (segmented_mode) {
	newfix((output_ptr-buffer)/2, R_DA | R_SEG, da_address);
	output_ptr += 8;
      }
      else {
	newfix((output_ptr-buffer)/2, R_DA, da_address);
	output_ptr += 4;
      }
      da_address = 0;
      break;
     case CLASS_CC:
      *output_ptr++ = the_cc;
      break;
     case CLASS_BIT:
      *output_ptr++ = c & 0xf;
      break;
     case CLASS_REGN0:
      if (reg[c&0xf] == 0) 
      {
	as_bad("can't use R0 here");
      }
     case CLASS_REG:
     case CLASS_REG_BYTE:
     case CLASS_REG_WORD:
     case CLASS_REG_LONG:
     case CLASS_REG_QUAD:
      /* Insert bit mattern of
	 right reg */
      *output_ptr++ =  reg[c & 0xf];
      break;
     case CLASS_IMM:
     {
       nib = 0;
       switch (c & ARG_MASK)
       {
	case ARG_IMM4:
	 newfix((output_ptr-buffer)/2, R_IMM4L, imm_operand);
	 *output_ptr++ = 0;
	 break;

	case   ARG_IMM8:  
	 newfix((output_ptr-buffer)/2, R_IMM8, imm_operand);
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;

	case   ARG_IMM16:  
	 newfix((output_ptr-buffer)/2, R_IMM16, imm_operand);
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 break;

	case   ARG_IMM32:  
	 newfix((output_ptr-buffer)/2, R_IMM32, imm_operand);
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;

	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;
	 *output_ptr++ = 0;

	 break;

	default:
	 abort();


       }
     }
    }
  }

  /* Copy from the nibble buffer into the frag */

 {
   char *src = buffer;
   char *fragp = output;
   while (src < output_ptr) 
   {
     *fragp = (src[0] << 4) | src[1];
     src+=2;
     fragp++;
   }

	
 }

}

/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This funciton is supposed to emit
   the frags/bytes it assembles to.
   */



void 
    DEFUN(md_assemble,(str),
	  char *str)
{
  char *op_start;
  char *op_end;
  unsigned int i;
  struct       z8k_op operand[2];  
  opcode_entry_type *opcode;
  opcode_entry_type * prev_opcode;
	
  char *dot = 0;
  char c;    
  /* Drop leading whitespace */
  while (*str == ' ')
   str++;
	
  /* find the op code end */
  for (op_start = op_end = str;
       *op_end != 0 && *op_end != ' ';
       op_end ++) 
  {
  }
	
  ;
	
  if (op_end == op_start) 
  {
    as_bad("can't find opcode ");
  }
  c = *op_end;
	
  *op_end = 0;
	
  opcode = (opcode_entry_type *) hash_find(opcode_hash_control,
					   op_start);
	
  if (opcode == NULL) 
  {
    as_bad("unknown opcode");
    return;
  }
	

  input_line_pointer =   get_operands(opcode, op_end,
				      operand);
  *op_end = c;
  prev_opcode = opcode;
	
  opcode = get_specific(opcode,  operand);
	
  if (opcode == 0)  
  {
    /* Couldn't find an opcode which matched the operands */
    char *where =frag_more(2);
    where[0] = 0x0;
    where[1] = 0x0;

    as_bad("Can't find opcode to match operands");
    return;
  }
	
  build_bytes(opcode, operand);
}	


void 
    DEFUN(tc_crawl_symbol_chain, (headers),
	  object_headers *headers)
{
	printf("call to tc_crawl_symbol_chain \n");
}

symbolS *DEFUN(md_undefined_symbol,(name),
	       char *name)
{
	return 0;
}

void 
    DEFUN(tc_headers_hook,(headers),
	  object_headers *headers)
{
	printf("call to tc_headers_hook \n"); 
}
void
    DEFUN_VOID(md_end) 
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
    md_atof(type,litP,sizeP)
char type;
char *litP;
int *sizeP;
{
	int	prec;
	LITTLENUM_TYPE words[MAX_LITTLENUMS];
	LITTLENUM_TYPE *wordP;
	char	*t;
	char	*atof_ieee();
	
	switch(type) {
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
		*sizeP=0;
		return "Bad call to MD_ATOF()";
	}
	t=atof_ieee(input_line_pointer,type,words);
	if(t)
	    input_line_pointer=t;
	
	*sizeP=prec * sizeof(LITTLENUM_TYPE);
	for(wordP=words;prec--;) {
		md_number_to_chars(litP,(long)(*wordP++),sizeof(LITTLENUM_TYPE));
		litP+=sizeof(LITTLENUM_TYPE);
	}
	return "";	/* Someone should teach Dean about null pointers */
}

int
    md_parse_option(argP, cntP, vecP)
char **argP;
int *cntP;
char ***vecP;

{
	return 0;
	
}

int md_short_jump_size;

void tc_aout_fix_to_chars () { printf("call to tc_aout_fix_to_chars \n");
			       abort(); }
void md_create_short_jump(ptr, from_addr, to_addr, frag, to_symbol)
char *ptr;
long from_addr;
long to_addr;
fragS *frag;
symbolS *to_symbol;
{
	as_fatal("failed sanity check.");
}

void
    md_create_long_jump(ptr,from_addr,to_addr,frag,to_symbol)
char *ptr;
long from_addr, to_addr;
fragS *frag;
symbolS *to_symbol;
{
	as_fatal("failed sanity check.");
}

void
    md_convert_frag(headers, fragP)
object_headers *headers;
fragS * fragP;

{ printf("call to md_convert_frag \n"); abort(); }

long
    DEFUN(md_section_align,(seg, size),
	  segT seg AND
	  long size)
{
	return((size + (1 << section_alignment[(int) seg]) - 1) & (-1 << section_alignment[(int) seg]));
	
}

void
    md_apply_fix(fixP, val)
fixS *fixP;
long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
	
  switch(fixP->fx_r_type) {
   case R_IMM4L:
    buf[0] = (buf[0] & 0xf0) | ((buf[0] + val) & 0xf);
    break;

   case R_IMM8:
    buf[0] += val;
    break;

   case R_DA:
   case R_IMM16:
    *buf++=(val>>8);
    *buf++=val;
    break;
   case R_IMM32:
    *buf++=(val>>24);
    *buf++=(val>>16);
    *buf++=(val>>8);
    *buf++=val;
    break;
   case R_DA | R_SEG:
    *buf++ = (val>>16);
    *buf++ = 0x00;
    *buf++ = (val>>8);
    *buf++ = val;
    break;
   default:
    abort();
		
  }
}

void DEFUN(md_operand, (expressionP),expressionS *expressionP) 
{ }

int  md_long_jump_size;
int
    md_estimate_size_before_relax(fragP, segment_type)
register fragS *fragP;
register segT segment_type;
{ 
	printf("call tomd_estimate_size_before_relax \n"); abort(); }
/* Put number into target byte order */

void DEFUN(md_number_to_chars,(ptr, use, nbytes),
	   char *ptr AND
	   long use AND
	   int nbytes)
{
	switch (nbytes) {
	case 4: *ptr++ = (use >> 24) & 0xff;
	case 3: *ptr++ = (use >> 16) & 0xff;
	case 2: *ptr++ = (use >> 8) & 0xff;
	case 1: *ptr++ = (use >> 0) & 0xff;
		break;
	default:
		abort();
	}
}
long md_pcrel_from(fixP) 
fixS *fixP; { abort(); }

void tc_coff_symbol_emit_hook() { }


void tc_reloc_mangle(fix_ptr, intr, base)
fixS *fix_ptr;
struct internal_reloc *intr;
bfd_vma base;

{
  symbolS *symbol_ptr;
	
  symbol_ptr = fix_ptr->fx_addsy;
	
  /* If this relocation is attached to a symbol then it's ok
     to output it */
  if (fix_ptr->fx_r_type == RELOC_32) {
    /* cons likes to create reloc32's whatever the size of the reloc..
     */
    switch (fix_ptr->fx_size) 
    {
			    
     case 2:
      intr->r_type = R_IMM16;
      break;
     case 1:
      intr->r_type = R_IMM8;
      break;
     default:
      abort();
			    
    }
		
  }
  else {  
    intr->r_type = fix_ptr->fx_r_type;
  }
	
  intr->r_vaddr = fix_ptr->fx_frag->fr_address +  fix_ptr->fx_where  +base;
  intr->r_offset = fix_ptr->fx_offset;
	
  if (symbol_ptr)
   intr->r_symndx = symbol_ptr->sy_number;
  else
   intr->r_symndx = -1;
	
	
}


