/* tc-crx.c -- Assembler code for the CRX CPU core.
   Copyright 2004 Free Software Foundation, Inc.

   Contributed by Tomer Levi, NSC, Israel.
   Originally written for GAS 2.12 by Tomer Levi, NSC, Israel.
   Updates, BFDizing, GNUifying and ELF support by Tomer Levi.

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
   along with GAS; see the file COPYING.  If not, write to the
   Free Software Foundation, 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "dwarf2dbg.h"
#include "opcode/crx.h"
#include "elf/crx.h"

#include <limits.h>

/* Word is considered here as a 16-bit unsigned short int.  */
#define WORD_SIZE   16
#define WORD_SHIFT  16

/* Register is 4-bit size.  */
#define REG_SIZE   4

/* Maximum size of a single instruction (in words).  */
#define INSN_MAX_SIZE   3

/* Maximum bits which may be set in a `mask16' operand.  */
#define MAX_REGS_IN_MASK16  8

/* Escape to 16-bit immediate.  */
#define ESC_16  0xE
/* Escape to 32-bit immediate.  */
#define ESC_32  0xF

/* Utility macros for string comparison.  */
#define streq(a, b)           (strcmp (a, b) == 0)
#define strneq(a, b, c)       (strncmp (a, b, c) == 0)

/* A mask to set n_bits starting from offset offs.  */
#define SET_BITS_MASK(offs,n_bits)    ((((1 << (n_bits)) - 1) << (offs)))
/* A mask to clear n_bits starting from offset offs.  */
#define CLEAR_BITS_MASK(offs,n_bits)  (~(((1 << (n_bits)) - 1) << (offs)))

/* Get the argument type for each operand of a given instruction.  */
#define GET_ACTUAL_TYPE						  \
  for (i = 0; i < insn->nargs; i++)				  \
    atyp_act[i] = getarg_type (instruction->operands[i].op_type)

/* Get the size (in bits) for each operand of a given instruction.  */
#define GET_ACTUAL_SIZE						  \
  for (i = 0; i < insn->nargs; i++)				  \
    bits_act[i] = getbits (instruction->operands[i].op_type)

/* Non-zero if OP is instruction with no operands.  */
#define NO_OPERANDS_INST(OP)			  \
  (streq (OP, "di") || streq (OP, "nop")	  \
   || streq (OP, "retx") || streq (OP, "ei")	  \
   || streq (OP, "wait") || streq (OP, "eiwait"))

/* Print a number NUM, shifted by SHIFT bytes, into a location
   pointed by index BYTE of array 'output_opcode'.  */
#define CRX_PRINT(BYTE, NUM, SHIFT)   output_opcode[BYTE] |= (NUM << SHIFT)

/* Opcode mnemonics hash table.  */
static struct hash_control *crx_inst_hash;
/* CRX registers hash table.  */
static struct hash_control *reg_hash;
/* CRX coprocessor registers hash table.  */
static struct hash_control *copreg_hash;
/* Current instruction we're assembling.  */
const inst *instruction;

/* Initialize global variables.  */
long output_opcode[2];
/* Nonzero means a relocatable symbol.  */
int relocatable;
/* Nonzero means a constant's bit-size was already set.  */
int size_was_set;
/* Nonzero means a negative constant.  */
int signflag;
/* Nonzero means a CST4 instruction.  */
int cst4flag;
/* A copy of the original instruction (used in error messages).  */
char ins_parse[MAX_INST_LEN];
/* Holds the current processed argument number.  */
int cur_arg_num;

/* Generic assembler global variables which must be defined by all targets.  */

/* Characters which always start a comment.  */
const char comment_chars[] = "#";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = "#";

/* This array holds machine specific line separator characters.  */
const char line_separator_chars[] = ";";

/* Chars that can be used to separate mant from exp in floating point nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant as in 0f12.456  */
const char FLT_CHARS[] = "f'";

/* Target-specific multicharacter options, not const-declared at usage.  */
const char *md_shortopts = "";
struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

/* This table describes all the machine specific pseudo-ops
   the assembler has to support.  The fields are:
   *** Pseudo-op name without dot.
   *** Function to call to execute this pseudo-op.
   *** Integer arg to pass to the function.  */

const pseudo_typeS md_pseudo_table[] =
{
  /* In CRX machine, align is in bytes (not a ptwo boundary).  */
  {"align", s_align_bytes, 0},
  {0, 0, 0}
};

const relax_typeS md_relax_table[] =
{
  /* bCC  */
  {0xfa, -0x100, 2, 1},			/*  8 */
  {0xfffe, -0x10000, 4, 2},		/* 16 */
  {0xfffffffe, -0xfffffffe, 6, 0},	/* 32 */

  /* bal  */
  {0xfffe, -0x10000, 4, 4},		/* 16 */
  {0xfffffffe, -0xfffffffe, 6, 0},	/* 32 */

  /* cmpbr  */
  {0xfe, -0x100, 4, 6},			/*  8 */
  {0xfffffe, -0x1000000, 6, 0}		/* 24 */
};

static void    reset_vars	        (char *, ins *);
static reg     get_register	        (char *);
static copreg  get_copregister	        (char *);
static void    set_operand_size		(ins *);
static argtype getarg_type	        (operand_type);
static int     getbits		        (operand_type);
static int     get_flags	        (operand_type);
static int     get_number_of_operands   (void);
static void    parse_operand	        (char *, ins *);
static int     gettrap		        (char *);
static void    handle_LoadStor	        (char *);
static int     get_cinv_parameters      (char *);
static unsigned long getconstant        (unsigned long, int);
static int     getreg_image	        (reg);
static void    parse_operands	        (ins *, char *);
static void    parse_insn	        (ins *, char *);
static void    print_operand	        (int, int, argument *);
static void    print_constant	        (int, int, argument *);
static int     exponent2scale	        (int);
static void    mask_const	        (unsigned long *, int);
static void    mask_reg		        (int, unsigned short *);
static int     process_label_constant   (char *, ins *);
static void    set_operand	        (char *, ins *);
static char *  preprocess_reglist       (char *, int *);
static int     assemble_insn	        (char *, ins *);
static void    print_insn	        (ins *);

/* Return the bit size for a given operand.  */

static int
getbits (operand_type op)
{
  if (op < MAX_OPRD)
    return crx_optab[op].bit_size;
  else
    return 0;
}

/* Return the argument type of a given operand.  */

static argtype
getarg_type (operand_type op)
{
  if (op < MAX_OPRD)
    return crx_optab[op].arg_type;
  else
    return nullargs;
}

/* Return the flags of a given operand.  */

static int
get_flags (operand_type op)
{
  if (op < MAX_OPRD)
    return crx_optab[op].flags;
  else
    return 0;
}

/* Get the core processor register 'reg_name'.  */

static reg
get_register (char *reg_name)
{
  const reg_entry *reg;

  reg = (const reg_entry *) hash_find (reg_hash, reg_name);

  if (reg != NULL)
    return reg->value.reg_val;
  else
    return nullregister;
}

/* Get the coprocessor register 'copreg_name'.  */

static copreg
get_copregister (char *copreg_name)
{
  const reg_entry *copreg;

  copreg = (const reg_entry *) hash_find (copreg_hash, copreg_name);

  if (copreg != NULL)
    return copreg->value.copreg_val;
  else
    return nullcopregister;
}

/* Mask a constant to the number of bits it is to be mapped to.  */

static void
mask_const (unsigned long int *t, int size)
{
  *t &= (((LONGLONG)1 << size) - 1);
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (segT seg, valueT val)
{
  /* Round .text section to a multiple of 2.  */
  if (seg == text_section)
    return (val + 1) & ~1;
  return val;
}

/* Parse an operand that is machine-specific (remove '*').  */

void
md_operand (expressionS * exp)
{
  char c = *input_line_pointer;

  switch (c)
    {
    case '*':
      input_line_pointer++;
      expression (exp);
      break;
    default:
      break;
    }
}

/* Reset global variables before parsing a new instruction.  */

static void
reset_vars (char *op, ins *crx_ins)
{
  unsigned int i;

  cur_arg_num = relocatable = size_was_set = signflag = cst4flag = 0;
  memset (& output_opcode, '\0', sizeof (output_opcode));

  /* Memset the 'signflag' field in every argument.  */
  for (i = 0; i < MAX_OPERANDS; i++)
    crx_ins->arg[i].signflag = 0;

  /* Save a copy of the original OP (used in error messages).  */
  strcpy (ins_parse, op);
}

/* This macro decides whether a particular reloc is an entry in a
   switch table.  It is used when relaxing, because the linker needs
   to know about all such entries so that it can adjust them if
   necessary.  */

#define SWITCH_TABLE(fix)				  \
  (   (fix)->fx_addsy != NULL				  \
   && (fix)->fx_subsy != NULL				  \
   && S_GET_SEGMENT ((fix)->fx_addsy) ==		  \
      S_GET_SEGMENT ((fix)->fx_subsy)			  \
   && S_GET_SEGMENT (fix->fx_addsy) != undefined_section  \
   && (   (fix)->fx_r_type == BFD_RELOC_CRX_NUM8	  \
       || (fix)->fx_r_type == BFD_RELOC_CRX_NUM16	  \
       || (fix)->fx_r_type == BFD_RELOC_CRX_NUM32))

/* See whether we need to force a relocation into the output file.
   This is used to force out switch and PC relative relocations when
   relaxing.  */

int
crx_force_relocation (fixS *fix)
{
  if (generic_force_reloc (fix) || SWITCH_TABLE (fix))
    return 1;

  return 0;
}

/* Generate a relocation entry for a fixup.  */

arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS * fixP)
{
  arelent * reloc;

  reloc = xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr  = xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  reloc->address = fixP->fx_frag->fr_address + fixP->fx_where;
  reloc->addend = fixP->fx_offset;

  if (fixP->fx_subsy != NULL)
    {
      if (SWITCH_TABLE (fixP))
	{
	  /* Keep the current difference in the addend.  */
	  reloc->addend = (S_GET_VALUE (fixP->fx_addsy)
			   - S_GET_VALUE (fixP->fx_subsy) + fixP->fx_offset);

	  switch (fixP->fx_r_type)
	    {
	    case BFD_RELOC_CRX_NUM8:
	      fixP->fx_r_type = BFD_RELOC_CRX_SWITCH8;
	      break;
	    case BFD_RELOC_CRX_NUM16:
	      fixP->fx_r_type = BFD_RELOC_CRX_SWITCH16;
	      break;
	    case BFD_RELOC_CRX_NUM32:
	      fixP->fx_r_type = BFD_RELOC_CRX_SWITCH32;
	      break;
	    default:
	      abort ();
	      break;
	    }
	}
      else
	{
	  /* We only resolve difference expressions in the same section.  */
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("can't resolve `%s' {%s section} - `%s' {%s section}"),
			fixP->fx_addsy ? S_GET_NAME (fixP->fx_addsy) : "0",
			segment_name (fixP->fx_addsy
				      ? S_GET_SEGMENT (fixP->fx_addsy)
				      : absolute_section),
			S_GET_NAME (fixP->fx_subsy),
			segment_name (S_GET_SEGMENT (fixP->fx_addsy)));
	}
    }

  assert ((int) fixP->fx_r_type > 0);
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixP->fx_r_type);

  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("internal error: reloc %d (`%s') not supported by object file format"),
		    fixP->fx_r_type,
		    bfd_get_reloc_code_name (fixP->fx_r_type));
      return NULL;
    }
  assert (!fixP->fx_pcrel == !reloc->howto->pc_relative);

  return reloc;
}

/* Prepare machine-dependent frags for relaxation.  */

int
md_estimate_size_before_relax (fragS *fragp, asection *seg)
{
  /* If symbol is undefined or located in a different section,
     select the largest supported relocation.  */
  relax_substateT subtype;
  relax_substateT rlx_state[] = {0, 2,
				 3, 4,
				 5, 6};

  for (subtype = 0; subtype < ARRAY_SIZE (rlx_state); subtype += 2)
    {
      if (fragp->fr_subtype == rlx_state[subtype]
	  && (!S_IS_DEFINED (fragp->fr_symbol)
	      || seg != S_GET_SEGMENT (fragp->fr_symbol)))
	{
	  fragp->fr_subtype = rlx_state[subtype + 1];
	  break;
	}
    }

  if (fragp->fr_subtype >= ARRAY_SIZE (md_relax_table))
    abort ();

  return md_relax_table[fragp->fr_subtype].rlx_length;
}

void
md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED, asection *sec, fragS *fragP)
{
  /* 'opcode' points to the start of the instruction, whether
     we need to change the instruction's fixed encoding.  */
  char *opcode = fragP->fr_literal + fragP->fr_fix;
  bfd_reloc_code_real_type reloc;

  subseg_change (sec, 0);

  switch (fragP->fr_subtype)
    {
    case 0:
      reloc = BFD_RELOC_CRX_REL8;
      break;
    case 1:
      *opcode = 0x7e;
      reloc = BFD_RELOC_CRX_REL16;
      break;
    case 2:
      *opcode = 0x7f;
      reloc = BFD_RELOC_CRX_REL32;
      break;
    case 3:
      reloc = BFD_RELOC_CRX_REL16;
      break;
    case 4:
      *++opcode = 0x31;
      reloc = BFD_RELOC_CRX_REL32;
      break;
    case 5:
      reloc = BFD_RELOC_CRX_REL8_CMP;
      break;
    case 6:
      *++opcode = 0x31;
      reloc = BFD_RELOC_CRX_REL24;
      break;
    default:
      abort ();
      break;
    }

    fix_new (fragP, fragP->fr_fix,
	     bfd_get_reloc_size (bfd_reloc_type_lookup (stdoutput, reloc)),
	     fragP->fr_symbol, fragP->fr_offset, 1, reloc);
    fragP->fr_var = 0;
    fragP->fr_fix += md_relax_table[fragP->fr_subtype].rlx_length;
}

/* Process machine-dependent command line options.  Called once for
   each option on the command line that the machine-independent part of
   GAS does not understand.  */

int
md_parse_option (int c ATTRIBUTE_UNUSED, char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Machine-dependent usage-output.  */

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
  return;
}

/* Turn a string in input_line_pointer into a floating point constant
   of type TYPE, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP.  An error message is
   returned, or NULL on OK.  */

char *
md_atof (int type, char *litP, int *sizeP)
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

  if (! target_big_endian)
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

/* Apply a fixS (fixup of an instruction or data that we didn't have
   enough info to complete immediately) to the data in a frag.
   Since linkrelax is nonzero and TC_LINKRELAX_FIXUP is defined to disable
   relaxation of debug sections, this function is called only when
   fixuping relocations of debug sections.  */

void
md_apply_fix3 (fixS *fixP, valueT *valP, segT seg)
{
  valueT val = * valP;
  char *buf = fixP->fx_frag->fr_literal + fixP->fx_where;
  fixP->fx_offset = 0;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_CRX_NUM8:
      bfd_put_8 (stdoutput, (unsigned char) val, buf);
      break;
    case BFD_RELOC_CRX_NUM16:
      bfd_put_16 (stdoutput, val, buf);
      break;
    case BFD_RELOC_CRX_NUM32:
      bfd_put_32 (stdoutput, val, buf);
      break;
    default:
      /* We shouldn't ever get here because linkrelax is nonzero.  */
      abort ();
      break;
    }

  fixP->fx_done = 0;

  if (fixP->fx_addsy == NULL
      && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;

  if (fixP->fx_pcrel == 1
      && fixP->fx_addsy != NULL
      && S_GET_SEGMENT (fixP->fx_addsy) == seg)
    fixP->fx_done = 1;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixS *fixp)
{
  return fixp->fx_frag->fr_address + fixp->fx_where;
}

/* This function is called once, at assembler startup time.  This should
   set up all the tables, etc that the MD part of the assembler needs.  */

void
md_begin (void)
{
  const char *hashret = NULL;
  int i = 0;

  /* Set up a hash table for the instructions.  */
  if ((crx_inst_hash = hash_new ()) == NULL)
    as_fatal (_("Virtual memory exhausted"));
  
  while (crx_instruction[i].mnemonic != NULL)
    {
      const char *mnemonic = crx_instruction[i].mnemonic;

      hashret = hash_insert (crx_inst_hash, mnemonic,
	(PTR) &crx_instruction[i]);

      if (hashret != NULL && *hashret != '\0')
	as_fatal (_("Can't hash `%s': %s\n"), crx_instruction[i].mnemonic,
		  *hashret == 0 ? _("(unknown reason)") : hashret);

      /* Insert unique names into hash table.  The CRX instruction set
	 has many identical opcode names that have different opcodes based
	 on the operands.  This hash table then provides a quick index to
	 the first opcode with a particular name in the opcode table.  */
      do
	{
	  ++i;
	}
      while (crx_instruction[i].mnemonic != NULL
	     && streq (crx_instruction[i].mnemonic, mnemonic));
    }

  /* Initialize reg_hash hash table.  */
  if ((reg_hash = hash_new ()) == NULL)
    as_fatal (_("Virtual memory exhausted"));

  {
    const reg_entry *regtab;

    for (regtab = crx_regtab;
	 regtab < (crx_regtab + NUMREGS); regtab++)
      {
	hashret = hash_insert (reg_hash, regtab->name, (PTR) regtab);
	if (hashret)
	  as_fatal (_("Internal Error:  Can't hash %s: %s"),
		    regtab->name,
		    hashret);
      }
  }

  /* Initialize copreg_hash hash table.  */
  if ((copreg_hash = hash_new ()) == NULL)
    as_fatal (_("Virtual memory exhausted"));

  {
    const reg_entry *copregtab;

    for (copregtab = crx_copregtab; copregtab < (crx_copregtab + NUMCOPREGS);
	 copregtab++)
      {
	hashret = hash_insert (copreg_hash, copregtab->name, (PTR) copregtab);
	if (hashret)
	  as_fatal (_("Internal Error:  Can't hash %s: %s"),
		    copregtab->name,
		    hashret);
      }
  }
  /*  Set linkrelax here to avoid fixups in most sections.  */
  linkrelax = 1;
}

/* Set the number of bits corresponding to a constant -
   here we check for possible overflow cases.  */

static void
set_operand_size (ins * crx_ins)
{
  int cnt_bits = 0;
  const cst4_entry *cst4_op;
  argument *cur_arg = &crx_ins->arg[cur_arg_num];  /* Current argument.  */
  unsigned long int temp = cur_arg->constant;

  /* If the constant's size was already set - nothing to do.  */
  if (size_was_set)
    return;

  /* Already dealt with negative numbers in process_label_constants.  */
  while (temp > 0)
    {
      temp >>= 1;
      cnt_bits++;
    }

  /* Arithmetic instructions :
     16-bit positive signed immediate -->> represent as 32-bit.  */
  if (IS_INSN_TYPE (ARITH_INS) && !relocatable && !signflag)
    {
      if (cnt_bits == 16)
        {
          cur_arg->size = 32;
          return;
        }
    }
  /* Index addressing mode :
     6-bit positive signed immediate -->> represent as 22-bit.  */
  if (IS_INSN_TYPE (LD_STOR_INS)
      || IS_INSN_TYPE (STOR_IMM_INS)
      || IS_INSN_TYPE (CSTBIT_INS))
    {
      if (!signflag && cur_arg->type == arg_idxr)
        {
          if (cnt_bits == 6)
            {
              cur_arg->size = 22;
              return;
            }
          if (cnt_bits == 22)
	    as_bad (_("Offset out of range in Instruction `%s'"), ins_parse);
        }
    }
  /* load/stor instructions :
     16-bit positive signed immediate -->> represent as 32-bit.  */
  if (IS_INSN_TYPE (LD_STOR_INS))
    {
      if (!signflag && cur_arg->type == arg_cr)
        {
          if (cnt_bits == 16)
            {
              cur_arg->size = 32;
              return;
            }
          if (cnt_bits == 32)
	    as_bad (_("Offset out of range in Instruction `%s'"), ins_parse);
        }
    }
  /* Post-increment mode :
     12-bit positive signed immediate -->> represent as 28-bit.  */
  if (IS_INSN_TYPE (CSTBIT_INS)
      || IS_INSN_TYPE (LD_STOR_INS_INC)
      || IS_INSN_TYPE (STOR_IMM_INS))
    {
      if (!signflag && cur_arg->type == arg_cr)
        {
          if (cnt_bits == 12)
            {
              cur_arg->size = 28;
              if (IS_INSN_TYPE (LD_STOR_INS_INC))
		as_bad (_("Offset out of range in Instruction `%s'"), ins_parse);
              return;
            }
          if (IS_INSN_TYPE (CSTBIT_INS) || IS_INSN_TYPE (STOR_IMM_INS))
            {
              if (cnt_bits == 28)
		as_bad (_("Offset out of range in Instruction `%s'"), ins_parse);
            }

        }
    }

  /* Handle negative cst4 mapping for arithmetic/cmp&br operations.  */
  if (signflag && !relocatable
      && ((IS_INSN_TYPE (ARITH_INS) || IS_INSN_TYPE (ARITH_BYTE_INS))
      || ((IS_INSN_TYPE (CMPBR_INS) && cur_arg_num == 0))))
    {
      for (cst4_op = cst4_map; cst4_op < (cst4_map + cst4_maps); cst4_op++)
	{
	  if (cur_arg->constant == (unsigned int)(-cst4_op->value))
	    {
	      cur_arg->size = 4;
	      cur_arg->constant = cst4_op->binary;
	      cur_arg->signflag = 0;
	      return;
	    }
	}
    }
  /* Because of the cst4 mapping -- -1 and -4 already handled above
     as well as for relocatable cases.  */
  if (signflag && IS_INSN_TYPE (ARITH_BYTE_INS))
    {
      if (!relocatable)
        {
          if (cur_arg->constant <= 0xffff)
            cur_arg->size = 16;
          else
	    /* Setting to 18 so that there is no match.  */
            cur_arg->size = 18;
        }
      else
        cur_arg->size = 16;
      return;
    }

  if (signflag && IS_INSN_TYPE (ARITH_INS))
    {
      /* For all immediates which can be expressed in less than 16 bits.  */
      if (cur_arg->constant <= 0xffff && !relocatable)
        {
          cur_arg->size = 16;
          return;
        }
      /* Either it is relocatable or not representable in 16 bits.  */
      if (cur_arg->constant < 0xffffffff || relocatable)
        {
          cur_arg->size = 32;
          return;
        }
      cur_arg->size = 33;
      return;
    }

  if (signflag && !relocatable)
    return;

  if (!relocatable)
    cur_arg->size = cnt_bits;

  /* Checking for Error Conditions.  */
  if (IS_INSN_TYPE (ARITH_INS) && !signflag)
    {
      if (cnt_bits > 32)
	as_bad (_("Cannot represent Immediate in %d bits in Instruction `%s'"),
		cnt_bits, ins_parse);
    }
  else if (IS_INSN_TYPE (ARITH_BYTE_INS) && !signflag)
    {
      if (cnt_bits > 16)
	as_bad (_("Cannot represent Immediate in %d bits in Instruction `%s'"),
		cnt_bits, ins_parse);
    }
}

/* Handle the constants immediate/absolute values and
   Labels (jump targets/Memory locations).  */

static int
process_label_constant (char *str, ins * crx_ins)
{
  char *save;
  unsigned long int temp, cnt;
  const cst4_entry *cst4_op;
  int is_cst4 = 0;
  int constant_val = 0;
  argument *cur_arg = &crx_ins->arg[cur_arg_num];  /* Current argument.  */

  save = input_line_pointer;
  signflag = 0;

  if (str[0] == '-')
    {
      signflag = 1;
      str++;
    }
  else if (str[0] == '+')
    str++;

  input_line_pointer = str;

  expression (&crx_ins->exp);

  switch (crx_ins->exp.X_op)
    {
    case O_big:
    case O_absent:
      /* Missing or bad expr becomes absolute 0.  */
      as_bad (_("missing or invalid displacement expression `%s' taken as 0"),
	      str);
      crx_ins->exp.X_op = O_constant;
      crx_ins->exp.X_add_number = 0;
      crx_ins->exp.X_add_symbol = (symbolS *) 0;
      crx_ins->exp.X_op_symbol = (symbolS *) 0;
      break;

    case O_constant:
      cur_arg->constant = constant_val = crx_ins->exp.X_add_number;
      if ((IS_INSN_TYPE (CMPBR_INS) || IS_INSN_TYPE (COP_BRANCH_INS))
	   && cur_arg_num == 2)
        {
          LONGLONG temp64 = 0;
          char ptr;
          char temp_str[30];
          unsigned int jump_value = 0;
          int BR_MASK = 0, BR_SIZE = 0;
          temp_str[0] = '\0';
          if (signflag)
            {
              temp_str[0] = '-';
              temp_str[1] = '\0';
            }
          strncat (temp_str, str, strlen (str));
	  temp64 = strtoll (temp_str, (char **) &ptr,0);

          if (temp64 % 2 != 0)
	    as_bad (_("Odd Offset in displacement in Instruction `%s'"),
		    ins_parse);

	  /* Determine the branch size.  */
          jump_value = (unsigned int)temp64 & 0xFFFFFFFF;
          if (((jump_value & 0xFFFFFF00) == 0xFFFFFF00)
	      || ((jump_value & 0xFFFFFF00) == 0x0))
            {
              BR_MASK = 0xFF;
              BR_SIZE = 8;
            }
          else
            if (((jump_value & 0xFF000000) == 0xFF000000)
		|| ((jump_value & 0xFF000000) == 0x0))
            {
              BR_MASK = 0xFFFFFF;
              BR_SIZE = 24;
            }
	  jump_value = jump_value >> 1;
          cur_arg->constant = jump_value & BR_MASK;
          cur_arg->size = BR_SIZE;
	  size_was_set = 1;
          cur_arg->signflag = signflag;
          input_line_pointer = save;
          return crx_ins->exp.X_op;
        }

      if (IS_INSN_TYPE (BRANCH_INS)
	  || IS_INSN_MNEMONIC ("bal")
	  || IS_INSN_TYPE (DCR_BRANCH_INS))
        {
          LONGLONG temp64 = 0;
          char ptr;
          char temp_str[30];
          unsigned int jump_value = 0;
          int BR_MASK = 0, BR_SIZE = 0;

          temp_str[0] = '\0';
          if (signflag)
            {
              temp_str[0] = '-';
              temp_str[1] = '\0';
            }
          strncat (temp_str, str, strlen (str));
	  temp64 = strtoll (temp_str, (char **) &ptr,0);

	  if (temp64 % 2 != 0)
	    as_bad (_("Odd Offset in displacement in Instruction `%s'"),
	    ins_parse);

	  /* Determine the branch size.  */
          jump_value = (unsigned int)temp64 & 0xFFFFFFFF;
          if (!IS_INSN_MNEMONIC ("bal") && !IS_INSN_TYPE (DCR_BRANCH_INS)
	      && (((jump_value & 0xFFFFFF00) == 0xFFFFFF00)
		  || ((jump_value & 0xFFFFFF00) == 0x0)))
            {
              BR_MASK = 0xFF;
              BR_SIZE = 8;
            }
          else if (((jump_value & 0xFFFF0000) == 0xFFFF0000)
		   || ((jump_value & 0xFFFF0000) == 0x0))
            {
              BR_MASK = 0xFFFF;
              BR_SIZE = 16;
            }
          else
            {
              BR_MASK = 0xFFFFFFFF;
              BR_SIZE = 32;
            }
	  jump_value = jump_value >> 1;
          cur_arg->constant = jump_value & BR_MASK;
          cur_arg->size = BR_SIZE;
	  size_was_set = 1;
          cur_arg->signflag = signflag;
          input_line_pointer = save;
          return crx_ins->exp.X_op;
        }
      /* Fix for movd $0xF12344, r0 -- signflag has to be set.  */
      if (constant_val < 0 && signflag != 1
          && !IS_INSN_TYPE (LD_STOR_INS) && !IS_INSN_TYPE (LD_STOR_INS_INC)
          && !IS_INSN_TYPE (CSTBIT_INS) && !IS_INSN_TYPE (STOR_IMM_INS)
          && !IS_INSN_TYPE (BRANCH_INS) && !IS_INSN_MNEMONIC ("bal"))
        {
          cur_arg->constant = ~(cur_arg->constant) + 1;
          signflag = 1;
        }
      /* For load/store instruction when the value is in the offset part.  */
      if (constant_val < 0 && signflag != 1
          && (IS_INSN_TYPE (LD_STOR_INS) || IS_INSN_TYPE (LD_STOR_INS_INC)
	      || IS_INSN_TYPE (CSTBIT_INS) || IS_INSN_TYPE (STOR_IMM_INS)))
        {
          if (cur_arg->type == arg_cr || cur_arg->type == arg_idxr)
            {
              cur_arg->constant = ~(cur_arg->constant) + 1;
              signflag = 1;
            }
        }
      if (signflag)
        {
          /* Signflag in never set in case of load store instructions
	     Mapping in case of only the arithinsn case.  */
          if ((cur_arg->constant != 1 && cur_arg->constant != 4)
	     || (!IS_INSN_TYPE (ARITH_INS)
		 && !IS_INSN_TYPE (ARITH_BYTE_INS)
		 && !IS_INSN_TYPE (CMPBR_INS)))
            {
              /* Counting the number of bits required to represent
	         the constant.  */
              cnt = 0;
              temp = cur_arg->constant - 1;
              while (temp > 0)
                {
                  temp >>= 1;
                  cnt++;
                }
              cur_arg->size = cnt + 1;
              cur_arg->constant = ~(cur_arg->constant) + 1;
              if (IS_INSN_TYPE (ARITH_INS) || IS_INSN_TYPE (ARITH_BYTE_INS))
                {
                  char ptr;
                  LONGLONG temp64;

		  temp64 = strtoull (str, (char **) &ptr, 0);
                  if (cnt < 4)
		    cur_arg->size = 5;

                  if (IS_INSN_TYPE (ARITH_INS))
                    {
                      if (cur_arg->size > 32
			  || (temp64 > ULONG_MAX))
			{
                          if (cur_arg->size > 32)
			    as_bad (_("In Instruction `%s': Immediate size is \
				    %lu bits cannot be accomodated"),
				    ins_parse, cnt + 1);

			  if (temp64 > ULONG_MAX)
			    as_bad (_("Value given more than 32 bits in \
				    Instruction `%s'"), ins_parse);
                        }
                    }
                  if (IS_INSN_TYPE (ARITH_BYTE_INS))
                    {
                      if (cur_arg->size > 16
			  || !((temp64 & 0xFFFF0000) == 0xFFFF0000
			       || (temp64 & 0xFFFF0000) == 0x0))
                        {
                          if (cur_arg->size > 16)
			    as_bad (_("In Instruction `%s': Immediate size is \
				    %lu bits cannot be accomodated"),
				    ins_parse, cnt + 1);

			  if (!((temp64 & 0xFFFF0000) == 0xFFFF0000
				|| (temp64 & 0xFFFF0000) == 0x0))
			    as_bad (_("Value given more than 16 bits in \
				    Instruction `%s'"), ins_parse);
                        }
                    }
                }
              if (IS_INSN_TYPE (LD_STOR_INS) && cur_arg->type == arg_cr)
                {
                  /* Cases handled ---
		     dispub4/dispuw4/dispud4 and for load store dispubwd4
		     is applicable only.  */
                  if (cur_arg->size <= 4)
                    cur_arg->size = 5;
                }
	      /* Argument number is checked to distinguish between
		 immediate and displacement in cmpbranch and bcopcond.  */
              if ((IS_INSN_TYPE (CMPBR_INS) || IS_INSN_TYPE (COP_BRANCH_INS))
		   && cur_arg_num == 2)
                {
                  if (cur_arg->size != 32)
                    cur_arg->constant >>= 1;
                }

	      mask_const (&cur_arg->constant, (int) cur_arg->size);
            }
        }
      else
        {
	  /* Argument number is checked to distinguish between
	     immediate and displacement in cmpbranch and bcopcond.  */
          if (((IS_INSN_TYPE (CMPBR_INS) || IS_INSN_TYPE (COP_BRANCH_INS))
		  && cur_arg_num == 2)
	        || IS_INSN_TYPE (BRANCH_NEQ_INS))
            {
              if (IS_INSN_TYPE (BRANCH_NEQ_INS))
                {
                  if (cur_arg->constant == 0)
		    as_bad (_("Instruction `%s' has Zero offset"), ins_parse);
                }

              if (cur_arg->constant % 2 != 0)
		as_bad (_("Instruction `%s' has odd offset"), ins_parse);

              if (IS_INSN_TYPE (BRANCH_NEQ_INS))
                {
                  if (cur_arg->constant > 32 || cur_arg->constant < 2)
		      as_bad (_("Instruction `%s' has illegal offset (%ld)"),
			      ins_parse, cur_arg->constant);

		  cur_arg->constant -= 2;
                }

              cur_arg->constant >>= 1;
              set_operand_size (crx_ins);
            }

	  /* Compare branch argument number zero to be compared -
	     mapped to cst4.  */
          if (IS_INSN_TYPE (CMPBR_INS) && cur_arg_num == 0)
            {
	      for (cst4_op = cst4_map; cst4_op < (cst4_map + cst4_maps); cst4_op++)
		{
		  if (cur_arg->constant == (unsigned int)cst4_op->value)
		    {
		      cur_arg->constant = cst4_op->binary;
		      is_cst4 = 1;
		      break;
		    }
		}
	      if (!is_cst4)
		as_bad (_("Instruction `%s' has invalid imm value as an \
			  operand"), ins_parse);
            }
        }
      break;

    case O_symbol:
    case O_subtract:
      crx_ins->rtype = BFD_RELOC_NONE;
      relocatable = 1;

      switch (cur_arg->type)
	{
	case arg_cr:
          /* Have to consider various cases here.  */
          if (IS_INSN_TYPE (LD_STOR_INS_INC))
	    /* 'load/stor <num>(reg)+'.  */
	    crx_ins->rtype = BFD_RELOC_CRX_REGREL12;
          else if (IS_INSN_TYPE (CSTBIT_INS)
		   || IS_INSN_TYPE (STOR_IMM_INS))
	    /* 'stor imm' and '[stc]bit'.  */
	    crx_ins->rtype = BFD_RELOC_CRX_REGREL28;
          else
	    /* General load/stor instruction.  */
	    crx_ins->rtype = BFD_RELOC_CRX_REGREL32;
	    break;
	case arg_idxr:
	  /* Index Mode 22 bits relocation.  */
	    crx_ins->rtype = BFD_RELOC_CRX_REGREL22;
	  break;
	case arg_c:
	  /* Absolute types.  */
          /* Case for jumps...dx  types.  */
          /* For bal.  */
          if (IS_INSN_MNEMONIC ("bal") || IS_INSN_TYPE (DCR_BRANCH_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_REL16;
	  else if (IS_INSN_TYPE (BRANCH_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_REL8;
          else if (IS_INSN_TYPE (LD_STOR_INS) || IS_INSN_TYPE (STOR_IMM_INS)
		   || IS_INSN_TYPE (CSTBIT_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_ABS32;
	  else if (IS_INSN_TYPE (BRANCH_NEQ_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_REL4;
          else if (IS_INSN_TYPE (CMPBR_INS) || IS_INSN_TYPE (COP_BRANCH_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_REL8_CMP;
	  break;
	case arg_ic:
          if (IS_INSN_TYPE (ARITH_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_IMM32;
	  else if (IS_INSN_TYPE (ARITH_BYTE_INS))
	    crx_ins->rtype = BFD_RELOC_CRX_IMM16;
	  break;
	default:
	  break;
      }
      cur_arg->size = (bfd_reloc_type_lookup (stdoutput, crx_ins->rtype))->bitsize;
      break;

    default:
      break;
    }

  input_line_pointer = save;
  cur_arg->signflag = signflag;
  return crx_ins->exp.X_op;
}

/* Get the values of the scale to be encoded -
   used for the scaled index mode of addressing.  */

static int
exponent2scale (int val)
{
  int exponent;

  /* If 'val' is 0, the following 'for' will be an endless loop.  */
  if (val == 0)
    return 0;

  for (exponent = 0; (val != 1); val >>= 1, exponent++)
    ;

  return exponent;
}

/* Parsing different types of operands
   -> constants		    Immediate/Absolute/Relative numbers
   -> Labels		    Relocatable symbols
   -> (rbase)		    Register base
   -> disp(rbase)	    Register relative
   -> disp(rbase)+	    Post-increment mode
   -> disp(rbase,ridx,scl)  Register index mode  */

static void
set_operand (char *operand, ins * crx_ins)
{
  char *operandS; /* Pointer to start of sub-opearand.  */
  char *operandE; /* Pointer to end of sub-opearand.  */
  expressionS scale;
  int scale_val;
  char *input_save, c;
  argument *cur_arg = &crx_ins->arg[cur_arg_num]; /* Current argument.  */

  /* Initialize pointers.  */
  operandS = operandE = operand;

  switch (cur_arg->type)
    {
    case arg_sc:    /* Case *+0x18.  */
    case arg_ic:    /* Case $0x18.  */
      operandS++;
    case arg_c:	    /* Case 0x18.  */
      /* Set constant.  */
      process_label_constant (operandS, crx_ins);
      
      if (cur_arg->type != arg_ic)
	cur_arg->type = arg_c;
      break;

    case arg_icr:   /* Case $0x18(r1).  */
      operandS++;
    case arg_cr:    /* Case 0x18(r1).   */
      /* Set displacement constant.  */
      while (*operandE != '(')
	operandE++;
      *operandE = '\0';
      process_label_constant (operandS, crx_ins);
      operandS = operandE;    
    case arg_rbase: /* Case (r1).  */
      operandS++;
      /* Set register base.  */
      while (*operandE != ')')
	operandE++;
      *operandE = '\0';
      if ((cur_arg->r = get_register (operandS)) == nullregister)
	as_bad (_("Illegal register `%s' in Instruction `%s'"),
		operandS, ins_parse);

      if (cur_arg->type != arg_rbase)
	cur_arg->type = arg_cr;
      break;

    case arg_idxr:
      /* Set displacement constant.  */
      while (*operandE != '(')
	operandE++;
      *operandE = '\0';
      process_label_constant (operandS, crx_ins);
      operandS = ++operandE;
      
      /* Set register base.  */
      while ((*operandE != ',') && (! ISSPACE (*operandE)))
	operandE++;
      *operandE++ = '\0';
      if ((cur_arg->r = get_register (operandS)) == nullregister)
	as_bad (_("Illegal register `%s' in Instruction `%s'"),
		operandS, ins_parse);

      /* Skip leading white space.  */
      while (ISSPACE (*operandE))
	operandE++;
      operandS = operandE;

      /* Set register index.  */
      while ((*operandE != ')') && (*operandE != ','))
	operandE++;
      c = *operandE;
      *operandE++ = '\0';

      if ((cur_arg->i_r = get_register (operandS)) == nullregister)
	as_bad (_("Illegal register `%s' in Instruction `%s'"),
		operandS, ins_parse);

      /* Skip leading white space.  */
      while (ISSPACE (*operandE))
	operandE++;
      operandS = operandE;

      /* Set the scale.  */
      if (c == ')')
	cur_arg->scale = 0;
      else
        {
	  while (*operandE != ')')
	    operandE++;
	  *operandE = '\0';

	  /* Preprocess the scale string.  */
	  input_save = input_line_pointer;
	  input_line_pointer = operandS;
	  expression (&scale);
	  input_line_pointer = input_save;

	  scale_val = scale.X_add_number;

	  /* Check if the scale value is legal.  */
          if (scale_val != 1 && scale_val != 2
              && scale_val != 4 && scale_val != 8)
	    as_bad (_("Illegal Scale - `%d'"), scale_val);

	  cur_arg->scale = exponent2scale (scale_val);
        }
      break;

    default:
      break;
    }
}

/* Parse a single operand.
   operand - Current operand to parse.
   crx_ins - Current assembled instruction.  */

static void
parse_operand (char *operand, ins * crx_ins)
{
  int ret_val;
  argument *cur_arg = &crx_ins->arg[cur_arg_num]; /* Current argument.  */

  /* Initialize the type to NULL before parsing.  */
  cur_arg->type = nullargs;

  /* Check whether this is a general processor register.  */
  if ((ret_val = get_register (operand)) != nullregister)
    {
      cur_arg->type = arg_r;
      cur_arg->r = ret_val;
      goto set_size;
    }

  /* Check whether this is a core [special] coprocessor register.  */
  if ((ret_val = get_copregister (operand)) != nullcopregister)
    {
      cur_arg->type = arg_copr;
      if (ret_val >= cs0)
	cur_arg->type = arg_copsr;
      cur_arg->cr = ret_val;
      goto set_size;
    }

  /* Deal with special characters.  */
  switch (operand[0])
    {
    case '$':
      if (strchr (operand, '(') != NULL)
	cur_arg->type = arg_icr;
      else
        cur_arg->type = arg_ic;
      goto set_params;
      break;

    case '*':
      cur_arg->type = arg_sc;
      goto set_params;
      break;

    case '(':
      cur_arg->type = arg_rbase;
      goto set_params;
      break;

    default:
	break;
    }
      
  if (strchr (operand, '(') != NULL)
    {
      if (strchr (operand, ',') != NULL
          && (strchr (operand, ',') > strchr (operand, '(')))
	    cur_arg->type = arg_idxr;
      else
	cur_arg->type = arg_cr;
    }
  else
    cur_arg->type = arg_c;
  goto set_params;

/* Parse an operand according to its type.  */
set_params:
  cur_arg->constant = 0;
  set_operand (operand, crx_ins);

/* Determine argument size.  */
set_size:
  switch (cur_arg->type)
    {
    /* The following are all registers, so set their size to REG_SIZE.  */
    case arg_r:
    case arg_copr:
    case arg_copsr:
    case arg_rbase:
      cur_arg->size = REG_SIZE;
      break;

    case arg_c:
    case arg_ic:
    case arg_sc:
    case arg_cr:
    case arg_icr:
    case arg_idxr:
      set_operand_size (crx_ins);
      break;

    default:
      as_bad (_("Illegal argument type in instruction `%s'"), ins_parse);
      break;
    }
}

/* Parse the various operands. Each operand is then analyzed to fillup 
   the fields in the crx_ins data structure.  */

static void
parse_operands (ins * crx_ins, char *operands)
{
  char *operandS;	       /* Operands string.  */
  char *operandH, *operandT;   /* Single operand head/tail pointers.  */
  int allocated = 0;	       /* Indicates a new operands string was allocated.  */
  char *operand[MAX_OPERANDS]; /* Separating the operands.  */
  int op_num = 0;	       /* Current operand number we are parsing.  */
  int bracket_flag = 0;	       /* Indicates a bracket '(' was found.  */
  int sq_bracket_flag = 0;     /* Indicates a square bracket '[' was found.  */

  /* Preprocess the list of registers, if necessary.  */
  operandS = operandH = operandT = (INST_HAS_REG_LIST) ?
    preprocess_reglist (operands, &allocated) : operands;

  while (*operandT != '\0')
    {
      if (*operandT == ',' && bracket_flag != 1 && sq_bracket_flag != 1)
        {
	  *operandT++ = '\0';
	  operand[op_num++] = strdup (operandH);
          operandH = operandT;
          continue;
        }

      if (*operandT == ' ')
	as_bad (_("Illegal operands (whitespace): `%s'"), ins_parse);

      if (*operandT == '(')
	bracket_flag = 1;
      else if (*operandT == '[')
	sq_bracket_flag = 1;

      if (*operandT == ')')
	{
	  if (bracket_flag)
	    bracket_flag = 0;
	  else
	    as_fatal (_("Missing matching brackets : `%s'"), ins_parse);
	}
      else if (*operandT == ']')
	{
	  if (sq_bracket_flag)
	    sq_bracket_flag = 0;
	  else
	    as_fatal (_("Missing matching brackets : `%s'"), ins_parse);
	}

      if (bracket_flag == 1 && *operandT == ')')
	bracket_flag = 0;
      else if (sq_bracket_flag == 1 && *operandT == ']')
	sq_bracket_flag = 0;

      operandT++;
    }

  /* Adding the last operand.  */
  operand[op_num++] = strdup (operandH);
  crx_ins->nargs = op_num;

  /* Verifying correct syntax of operands (all brackets should be closed).  */
  if (bracket_flag || sq_bracket_flag)
    as_fatal (_("Missing matching brackets : `%s'"), ins_parse);

  /* Now we parse each operand separately.  */
  for (op_num = 0; op_num < crx_ins->nargs; op_num++)
    {
      cur_arg_num = op_num;
      parse_operand (operand[op_num], crx_ins);
      free (operand[op_num]);
    }

  if (allocated)
    free (operandS);
}

/* Get the trap index in dispatch table, given its name.
   This routine is used by assembling the 'excp' instruction.  */

static int
gettrap (char *s)
{
  const trap_entry *trap;

  for (trap = crx_traps; trap < (crx_traps + NUMTRAPS); trap++)
    if (strcasecmp (trap->name, s) == 0)
      return trap->entry;

  as_bad (_("Unknown exception: `%s'"), s);
  return 0;
}

/* Post-Increment instructions, as well as Store-Immediate instructions, are a 
   sub-group within load/stor instruction groups. 
   Therefore, when parsing a Post-Increment/Store-Immediate insn, we have to 
   advance the instruction pointer to the start of that sub-group (that is, up 
   to the first instruction of that type).
   Otherwise, the insn will be mistakenly identified as of type LD_STOR_INS.  */

static void
handle_LoadStor (char *operands)
{
  /* Post-Increment instructions precede Store-Immediate instructions in 
     CRX instruction table, hence they are handled before. 
     This synchronization should be kept.  */

  /* Assuming Post-Increment insn has the following format :
     'MNEMONIC DISP(REG)+, REG' (e.g. 'loadw 12(r5)+, r6').
     LD_STOR_INS_INC are the only store insns containing a plus sign (+).  */
  if (strstr (operands, ")+") != NULL)
    {
      while (! IS_INSN_TYPE (LD_STOR_INS_INC))
	instruction++;
      return;
    }

  /* Assuming Store-Immediate insn has the following format :
     'MNEMONIC $DISP, ...' (e.g. 'storb $1, 12(r5)').
     STOR_IMM_INS are the only store insns containing a dollar sign ($).  */
  if (strstr (operands, "$") != NULL)
    while (! IS_INSN_TYPE (STOR_IMM_INS))
      instruction++;
}

/* Top level module where instruction parsing starts.
   crx_ins - data structure holds some information.
   operands - holds the operands part of the whole instruction.  */

static void
parse_insn (ins *insn, char *operands)
{
  /* Handle 'excp'/'cinv' */
  if (IS_INSN_MNEMONIC ("excp") || IS_INSN_MNEMONIC ("cinv"))
    {
      insn->nargs = 1;
      insn->arg[0].type = arg_ic;
      insn->arg[0].size = 4;
      insn->arg[0].constant = IS_INSN_MNEMONIC ("excp") ?
	gettrap (operands) : get_cinv_parameters (operands);
      return;
    }

  /* Handle load/stor unique instructions before parsing.  */
  if (IS_INSN_TYPE (LD_STOR_INS))
    handle_LoadStor (operands);

  if (operands != NULL)
    parse_operands (insn, operands);
}

/* Cinv instruction requires special handling.  */

static int
get_cinv_parameters (char * operand)
{
  char *p = operand;
  int d_used = 0, i_used = 0, u_used = 0, b_used = 0;

  while (*++p != ']')
    {
      if (*p == ',' || *p == ' ')
	continue;

      if (*p == 'd')
	d_used = 1;
      else if (*p == 'i')
	i_used = 1;
      else if (*p == 'u')
	u_used = 1;
      else if (*p == 'b')
	b_used = 1;
      else
	as_bad (_("Illegal `cinv' parameter: `%c'"), *p);
    }

  return ((b_used ? 8 : 0)
	+ (d_used ? 4 : 0)
	+ (i_used ? 2 : 0)
	+ (u_used ? 1 : 0));
}

/* Retrieve the opcode image of a given register.
   If the register is illegal for the current instruction,
   issue an error.  */

static int
getreg_image (reg r)
{
  const reg_entry *reg;
  char *reg_name;
  int is_procreg = 0; /* Nonzero means argument should be processor reg.  */

  if (((IS_INSN_MNEMONIC ("mtpr")) && (cur_arg_num == 1))
      || ((IS_INSN_MNEMONIC ("mfpr")) && (cur_arg_num == 0)) )
    is_procreg = 1;

  /* Check whether the register is in registers table.  */
  if (r < MAX_REG)
    reg = &crx_regtab[r];
  /* Check whether the register is in coprocessor registers table.  */
  else if (r < MAX_COPREG)
    reg = &crx_copregtab[r-MAX_REG];
  /* Register not found.  */
  else
    {
      as_bad (_("Unknown register: `%d'"), r);
      return 0;
    }

  reg_name = reg->name;

/* Issue a error message when register is illegal.  */
#define IMAGE_ERR \
  as_bad (_("Illegal register (`%s') in Instruction: `%s'"), \
	    reg_name, ins_parse);			     \
  break;

  switch (reg->type)
  {
    case CRX_U_REGTYPE:
      if (is_procreg || (instruction->flags & USER_REG))
	return reg->image;
      else
	IMAGE_ERR;

    case CRX_CFG_REGTYPE:
      if (is_procreg)
	return reg->image;
      else
	IMAGE_ERR;

    case CRX_R_REGTYPE:
      if (! is_procreg)
	return reg->image;
      else
	IMAGE_ERR;

    case CRX_C_REGTYPE:
    case CRX_CS_REGTYPE:
      return reg->image;
      break;

    default:
      IMAGE_ERR;
  }

  return 0;
}

/* Routine used to get the binary-string equivalent of a integer constant
   which currently require currbits to represent itself to be extended to
   nbits.  */

static unsigned long int
getconstant (unsigned long int x, int nbits)
{
  int cnt = 0;
  unsigned long int temp = x;

  while (temp > 0)
    {
      temp >>= 1;
      cnt++;
    }

  /* Escape sequence to next 16bit immediate.  */
  if (cnt > nbits)
    as_bad (_("Value `%ld' truncated to fit `%d' bits in instruction `%s'"),
	    x, cnt, ins_parse);
  else
    {
      if (signflag)
	x |= SET_BITS_MASK (cnt, nbits - cnt);
      else
	x &= CLEAR_BITS_MASK (cnt, nbits - cnt);
    }

  /* The following expression avoids overflow if
     'nbits' is the number of bits in 'bfd_vma'.  */
  return (x & ((((1 << (nbits - 1)) - 1) << 1) | 1));
}

/* Print a constant value to 'output_opcode':
   ARG holds the operand's type and value.
   SHIFT represents the location of the operand to be print into.
   NBITS determines the size (in bits) of the constant.  */

static void
print_constant (int nbits, int shift, argument *arg)
{
  unsigned long mask = 0;

  long constant = getconstant (arg->constant, nbits);

  switch (nbits)
  {
    case 32:
    case 28:
    case 24:
    case 22:
      /* mask the upper part of the constant, that is, the bits
	 going to the lowest byte of output_opcode[0].
	 The upper part of output_opcode[1] is always filled,
	 therefore it is always masked with 0xFFFF.  */
      mask = (1 << (nbits - 16)) - 1;
      /* Divide the constant between two consecutive words :
		 0	   1	     2	       3
	    +---------+---------+---------+---------+
	    |	      | X X X X | X X X X |	    |
	    +---------+---------+---------+---------+
	      output_opcode[0]    output_opcode[1]     */

      CRX_PRINT (0, (constant >> WORD_SHIFT) & mask, 0);
      CRX_PRINT (1, (constant & 0xFFFF), WORD_SHIFT);
      break;

    case 16:
    case 12:
      /* Special case - in arg_cr, the SHIFT represents the location
	 of the REGISTER, not the constant, which is itself not shifted.  */
      if (arg->type == arg_cr)
	{
	  CRX_PRINT (0, constant,  0);
	  break;
	}

      /* When instruction size is 3 and 'shift' is 16, a 16-bit constant is 
	 always filling the upper part of output_opcode[1]. If we mistakenly 
	 write it to output_opcode[0], the constant prefix (that is, 'match')
	 will be overriden.
		 0	   1	     2	       3
	    +---------+---------+---------+---------+
	    | 'match' |         | X X X X |	    |
	    +---------+---------+---------+---------+
	      output_opcode[0]    output_opcode[1]     */

      if ((instruction->size > 2) && (shift == WORD_SHIFT))
	CRX_PRINT (1, constant, WORD_SHIFT);
      else
	CRX_PRINT (0, constant, shift);
      break;

    default:
      CRX_PRINT (0, constant,  shift);
      break;
  }
}

/* Print an operand to 'output_opcode', which later on will be
   printed to the object file:
   ARG holds the operand's type, size and value.
   SHIFT represents the printing location of operand.
   NBITS determines the size (in bits) of a constant operand.  */

static void
print_operand (int nbits, int shift, argument *arg)
{
  switch (arg->type)
    {
    case arg_r:
      CRX_PRINT (0, getreg_image (arg->r), shift);
      break;

    case arg_copr:
      if (arg->cr < c0 || arg->cr > c15)
	as_bad (_("Illegal Co-processor register in Instruction `%s' "),
		ins_parse);
      CRX_PRINT (0, getreg_image (arg->cr), shift);
      break;

    case arg_copsr:
      if (arg->cr < cs0 || arg->cr > cs15)
	as_bad (_("Illegal Co-processor special register in Instruction `%s' "),
		ins_parse);
      CRX_PRINT (0, getreg_image (arg->cr), shift);
      break;

    case arg_idxr:
      /*    16      12	      8    6         0
	    +--------------------------------+
	    | r_base | r_idx  | scl|  disp   |
	    +--------------------------------+	  */
      CRX_PRINT (0, getreg_image (arg->r), 12);
      CRX_PRINT (0, getreg_image (arg->i_r), 8);
      CRX_PRINT (0, arg->scale, 6);
    case arg_ic:
    case arg_c:
      print_constant (nbits, shift, arg);
      break;

    case arg_rbase:
      CRX_PRINT (0, getreg_image (arg->r), shift);
      break;

    case arg_cr:
      /* case base_cst4.  */
      if ((instruction->flags & DISPU4MAP) && cst4flag)
	output_opcode[0] |= (getconstant (arg->constant, nbits)
			     << (shift + REG_SIZE));
      else
	/* rbase_disps<NN> and other such cases.  */
	print_constant (nbits, shift, arg);
      /* Add the register argument to the output_opcode.  */
      CRX_PRINT (0, getreg_image (arg->r), shift);
      break;

    default:
      break;
    }
}

/* Retrieve the number of operands for the current assembled instruction.  */

static int
get_number_of_operands (void)
{
  int i;

  for (i = 0; instruction->operands[i].op_type && i < MAX_OPERANDS; i++)
    ;
  return i;
}

/* Assemble a single instruction :
   Instruction has been parsed and all operand values set appropriately.
   Algorithm for assembling -
   For instruction to be assembled:
    Step 1: Find instruction in the array crx_instruction with same mnemonic.
    Step 2: Find instruction with same operand types.
    Step 3: If (size_of_operands) match then done, else increment the
	    array_index and goto Step3.
    Step 4: Cannot assemble
   Returns 1 upon success, 0 upon failure.  */

static int
assemble_insn (char *mnemonic, ins *insn)
{
  /* Argument type of each operand in the instruction we are looking for.  */
  argtype atyp[MAX_OPERANDS];
  /* Argument type of each operand in the current instruction.  */
  argtype atyp_act[MAX_OPERANDS];
  /* Size (in bits) of each operand in the instruction we are looking for.  */
  int bits[MAX_OPERANDS];
  /* Size (in bits) of each operand in the current instruction.  */
  int bits_act[MAX_OPERANDS];
  /* Location (in bits) of each operand in the current instruction.  */
  int shift_act[MAX_OPERANDS];
  /* Instruction type to match.  */
  unsigned int ins_type;
  int match = 0;
  int done_flag = 0;
  int dispu4map_type = 0;
  int changed_already = 0;
  unsigned int temp_value = 0;
  int instrtype, i;
  /* A pointer to the argument's constant value.  */
  unsigned long int *cons;
  /* Pointer to loop over all cst4_map entries.  */
  const cst4_entry *cst4_op;

  /* Instruction has no operands -> copy only the constant opcode.   */
  if (insn->nargs == 0)
    {
      output_opcode[0] = BIN (instruction->match, instruction->match_bits);
      return 1;
    }

  /* Find instruction with same number of operands.  */
  while (get_number_of_operands () != insn->nargs
         && IS_INSN_MNEMONIC (mnemonic))
    instruction++;

  if (!IS_INSN_MNEMONIC (mnemonic))
    return 0;

  /* Initialize argument type and size of each given operand.  */
  for (i = 0; i < insn->nargs; i++)
    {
      atyp[i] = insn->arg[i].type;
      bits[i] = insn->arg[i].size;
    }

  /* Initialize argument type and size of each operand in current inst.  */
  GET_ACTUAL_TYPE;
  GET_ACTUAL_SIZE;

  /* In some case, same mnemonic can appear with different instruction types.
     For example, 'storb' is supported with 3 different types :
     LD_STOR_INS, LD_STOR_INS_INC, STOR_IMM_INS.
     We assume that when reaching this point, the instruction type was 
     pre-determined. We need to make sure that the type stays the same
     during a search for matching instruction.  */
  ins_type = CRX_INS_TYPE(instruction->flags);

  while (match != 1
	 /* Check we didn't get to end of table.  */
	 && instruction->mnemonic != NULL
	 /* Check that the actual mnemonic is still available.  */
	 && IS_INSN_MNEMONIC (mnemonic)
	 /* Check that the instruction type wasn't changed.  */
	 && IS_INSN_TYPE(ins_type))
    {
      /* Check for argement type compatibility.  */
      for (i = 0; i < insn->nargs; i++)
        {
          if (atyp_act[i] == atyp[i])
	    done_flag = 1;
          else
            {
              done_flag = 0;
              break;
            }
        }

      if (done_flag)
	{
	  for (i = 0; i < insn->nargs; i++)
	    {
	      if ((get_flags (instruction->operands[i].op_type) & OPERAND_UNSIGNED)
		  && (insn->arg[i].signflag))
		    {
		      done_flag = 0;
		      break;
		    }
	    }
	}

      if (done_flag == 0)
        {
	  /* Try again with next instruction.  */
          instruction++;
	  GET_ACTUAL_TYPE;
	  GET_ACTUAL_SIZE;
          continue;
        }
      else
        {
          /* Check for size compatibility.  */
          for (i = 0; i < insn->nargs; i++)
            {
              if (bits[i] > bits_act[i])
                {
		  /* Actual size is too small - try again.  */
                  done_flag = 0;
                  instruction++;
	  	  GET_ACTUAL_TYPE;
		  GET_ACTUAL_SIZE;
                  break;
                }
            }

        }

      if (done_flag == 1)
        {
	  /* Full match is found.  */
          match = 1;
          break;
        }
    }

  if (match == 0)
    /* We haven't found a match - instruction can't be assembled.  */
    return 0;
  else
    /* Full match - print the final image.  */
    {
      /* If the post-increment address mode is used and the load/store 
	 source register is the same as rbase, the result of the 
	 instruction is undefined.  */
      if (IS_INSN_TYPE (LD_STOR_INS_INC))
	{
	  /* Enough to verify that one of the arguments is a simple reg.  */
	  if ((insn->arg[0].type == arg_r) || (insn->arg[1].type == arg_r))
	    if (insn->arg[0].r == insn->arg[1].r)
	      as_bad (_("Same src/dest register is used (`r%d'), result is undefined"), 
		       insn->arg[0].r);
	}

      /* Optimization: Omit a zero displacement in bit operations, 
	 saving 2-byte encoding space (e.g., 'cbitw $8, 0(r1)').  */
      if (IS_INSN_TYPE (CSTBIT_INS) && !relocatable)
        {
          if ((instruction->operands[1].op_type == rbase_disps12)
	       && (insn->arg[1].constant == 0))
                {
                  instruction--;
		  GET_ACTUAL_SIZE;
                }
        }

      /* Some instruction assume the stack pointer as rptr operand.
	 Issue an error when the register to be loaded is also SP.  */
      if (instruction->flags & NO_SP)
        {
	  if (getreg_image (insn->arg[0].r) == getreg_image (sp))
	    as_bad (_("`%s' has undefined result"), ins_parse);
	}

      /* If the rptr register is specified as one of the registers to be loaded, 
	 the final contents of rptr are undefined. Thus, we issue an error.  */
      if (instruction->flags & NO_RPTR)
        {
	  if ((1 << getreg_image (insn->arg[0].r)) & insn->arg[1].constant)
	    as_bad (_("Same src/dest register is used (`r%d'), result is undefined"), 
	     getreg_image (insn->arg[0].r));
	}

      /* Handle positive constants.  */
      if (!signflag)
        {
          if ((instruction->flags & DISPU4MAP) && !relocatable)
            {
              /* Get the map type of the instruction.  */
              instrtype = instruction->flags & REVERSE_MATCH ? 0 : 1;
	      cons = &insn->arg[instrtype].constant;
              dispu4map_type = instruction->flags & DISPU4MAP;

	      switch (dispu4map_type)
		{
		case DISPUB4:
		  /* 14 and 15 are reserved escape sequences of dispub4.  */
                  if (*cons == 14 || *cons == 15)
                    {
                      instruction++;
		      GET_ACTUAL_SIZE;
                    }
		  break;

		case DISPUW4:
		  /* Mapping has to be done.  */
		  if (*cons <= 15 && *cons % 2 != 0)
                    {
                      instruction++;
		      GET_ACTUAL_SIZE;
                    }
                  else if (*cons > 15 && *cons < 27 && *cons % 2 == 0)
                    {
                      instruction--;
		      GET_ACTUAL_SIZE;
                    }
		  if (*cons < 27 && *cons % 2 == 0)
		    *cons /= 2;
		  break;

		case DISPUD4:
                  /* Mapping has to be done.  */
                  if (*cons <= 15 && *cons % 4 != 0)
                    {
                      instruction++;
		      GET_ACTUAL_SIZE;
                    }
                  else if (*cons > 15 && *cons < 53 && *cons % 4 == 0)
                    {
                      instruction--;
		      GET_ACTUAL_SIZE;
                    }
		  if (*cons < 53 && *cons % 4 == 0)
		    *cons /= 4;
		  break;
		default:
		  as_bad (_("Invalid DISPU4 type"));
		  break;
	      }
            }

	  /* Check whether a cst4 mapping has to be done.  */
	  if ((instruction->flags & CST4MAP) && !relocatable)
	    {
	      /* 'const' equals reserved escape sequences -->>
		 represent as i16.  */
	      if (insn->arg[0].constant == ESC_16
		  || insn->arg[0].constant == ESC_32)
		{
		  instruction++;
		  GET_ACTUAL_SIZE;
		}
	      else
		{
		  /* Loop over cst4_map entries.  */
		  for (cst4_op = cst4_map; cst4_op < (cst4_map + cst4_maps);
		       cst4_op++)
		    {
		      /* 'const' equals a binary, which is already mapped
			 by a different value -->> represent as i16.  */
		      if (insn->arg[0].constant == (unsigned int)cst4_op->binary
			  && cst4_op->binary != cst4_op->value)
			{
			  instruction++;
			  GET_ACTUAL_SIZE;
			}
		      /* 'const' equals a value bigger than 16 -->> map to
			 its binary and represent as cst4.  */
		      else if (insn->arg[0].constant == (unsigned int)cst4_op->value
			       && insn->arg[0].constant >= 16)
			{
			  instruction--;
			  insn->arg[0].constant = cst4_op->binary;
			  GET_ACTUAL_SIZE;
			}
		    }
		}
	    }

	  /* Special check for 'addub 0, r0' instruction -
	     The opcode '0000 0000 0000 0000' is not allowed.  */
          if (IS_INSN_MNEMONIC ("addub"))
            {
              if ((instruction->operands[0].op_type == cst4)
		  && instruction->operands[1].op_type == regr)
                {
                  if (insn->arg[0].constant == 0 && insn->arg[1].r == r0)
		    instruction++;
                }
            }
          if ((IS_INSN_TYPE (LD_STOR_INS) || IS_INSN_TYPE (CSTBIT_INS)
	       || IS_INSN_TYPE (STOR_IMM_INS)) & !relocatable)
            {
	      instrtype = instruction->flags & REVERSE_MATCH ? 0 : 1;
              changed_already = 0;
              /* Convert 32 bits accesses to 16 bits accesses.  */
              if (instruction->operands[instrtype].op_type == abs32)
                {
                  if ((insn->arg[instrtype].constant & 0xFFFF0000) == 0xFFFF0000)
                    {
                      instruction--;
                      insn->arg[instrtype].constant =
                        insn->arg[instrtype].constant & 0xFFFF;
                      insn->arg[instrtype].size = 16;
                      changed_already = 1;
		      GET_ACTUAL_SIZE;
                    }
                }
              /* Convert 16 bits accesses to 32 bits accesses.  */
              if (instruction->operands[instrtype].op_type == abs16
                  && changed_already != 1)
                {
                  instruction++;
                  insn->arg[instrtype].constant =
                    insn->arg[instrtype].constant & 0xFFFF;
                  insn->arg[instrtype].size = 32;
		  GET_ACTUAL_SIZE;
                }
              changed_already = 0;
            }
        }

      for (i = 0; i < insn->nargs; i++)
        {
	  /* Mark a CST4 argument, if exists.  */
	  if (get_flags (instruction->operands[i].op_type) & OPERAND_CST4)
	    cst4flag = 1;

	  /* Handle reserved escape sequences.  */
	  if ((get_flags (instruction->operands[i].op_type) & OPERAND_ESC)
	      && !relocatable)
            {
	      /* 0x7e and 0x7f are reserved escape sequences of dispe9.  */
	      if (insn->arg[i].constant == 0x7e || insn->arg[i].constant == 0x7f)
                {
		  /* Use a disps17 for these values.  */
                  instruction++;
		  GET_ACTUAL_SIZE;
                }
            }
	}

      /* First, copy the instruction's opcode.  */
      output_opcode[0] = BIN (instruction->match, instruction->match_bits);

      /* Swap the argument values in case bcop instructions.  */
      if (IS_INSN_TYPE (COP_BRANCH_INS))
        {
          temp_value = insn->arg[0].constant;
          insn->arg[0].constant = insn->arg[1].constant;
          insn->arg[1].constant = temp_value;
        }

      for (i = 0; i < insn->nargs; i++)
        {
	  shift_act[i] = instruction->operands[i].shift;
          signflag = insn->arg[i].signflag;
          cur_arg_num = i;
          print_operand (bits_act[i], shift_act[i], &insn->arg[i]);
        }
    }

  return 1;
}

/* Set the appropriate bit for register 'r' in 'mask'.
   This indicates that this register is loaded or stored by
   the instruction.  */

static void
mask_reg (int r, unsigned short int *mask)
{
  if ((reg)r > (reg)sp)
    {
      as_bad (_("Invalid Register in Register List"));
      return;
    }

  *mask |= (1 << r);
}

/* Preprocess register list - create a 16-bit mask with one bit for each
   of the 16 general purpose registers. If a bit is set, it indicates
   that this register is loaded or stored by the instruction.  */

static char *
preprocess_reglist (char *param, int *allocated)
{
  char reg_name[MAX_REGNAME_LEN]; /* Current parsed register name.  */
  char *regP;			  /* Pointer to 'reg_name' string.  */
  int reg_counter = 0;		  /* Count number of parsed registers.  */
  unsigned short int mask = 0;	  /* Mask for 16 general purpose registers.  */
  char *new_param;		  /* New created operands string.  */
  char *paramP = param;		  /* Pointer to original opearands string.  */
  char maskstring[10];		  /* Array to print the mask as a string.  */
  int hi_found = 0, lo_found = 0; /* Boolean flags for hi/lo registers.  */
  reg r;
  copreg cr;

  /* If 'param' is already in form of a number, no need to preprocess.  */
  if (strchr (paramP, '{') == NULL)
    return param;

  /* Verifying correct syntax of operand.  */
  if (strchr (paramP, '}') == NULL)
    as_fatal (_("Missing matching brackets : `%s'"), ins_parse);

  while (*paramP++ != '{');

  new_param = (char *)xcalloc (MAX_INST_LEN, sizeof (char));
  *allocated = 1;
  strncpy (new_param, param, paramP - param - 1);

  while (*paramP != '}')
    {
      regP = paramP;
      memset (&reg_name, '\0', sizeof (reg_name));

      while (ISALNUM (*paramP))
	paramP++;

      strncpy (reg_name, regP, paramP - regP);

      /* Coprocessor register c<N>.  */
      if (IS_INSN_TYPE (COP_REG_INS))
        {
          if (((cr = get_copregister (reg_name)) == nullcopregister)
	      || (crx_copregtab[cr-MAX_REG].type != CRX_C_REGTYPE))
	    as_fatal (_("Illegal register `%s' in cop-register list"), reg_name);
	  mask_reg (getreg_image (cr - c0), &mask);
        }
      /* Coprocessor Special register cs<N>.  */
      else if (IS_INSN_TYPE (COPS_REG_INS))
        {
          if (((cr = get_copregister (reg_name)) == nullcopregister)
	      || (crx_copregtab[cr-MAX_REG].type != CRX_CS_REGTYPE))
	    as_fatal (_("Illegal register `%s' in cop-special-register list"), 
		      reg_name);
	  mask_reg (getreg_image (cr - cs0), &mask);
        }
      /* User register u<N>.  */
      else if (instruction->flags & USER_REG)
	{
	  if (streq(reg_name, "uhi"))
	    {
	      hi_found = 1;
	      goto next_inst;
	    }
	  else if (streq(reg_name, "ulo"))
	    {
	      lo_found = 1;
	      goto next_inst;
	    }
          else if (((r = get_register (reg_name)) == nullregister)
	      || (crx_regtab[r].type != CRX_U_REGTYPE))
	    as_fatal (_("Illegal register `%s' in user register list"), reg_name);
	  
	  mask_reg (getreg_image (r - u0), &mask);	  
	}
      /* General purpose register r<N>.  */
      else
        {
	  if (streq(reg_name, "hi"))
	    {
	      hi_found = 1;
	      goto next_inst;
	    }
	  else if (streq(reg_name, "lo"))
	    {
	      lo_found = 1;
	      goto next_inst;
	    }
          else if (((r = get_register (reg_name)) == nullregister)
	      || (crx_regtab[r].type != CRX_R_REGTYPE))
	    as_fatal (_("Illegal register `%s' in register list"), reg_name);

	  mask_reg (getreg_image (r - r0), &mask);
        }

      if (++reg_counter > MAX_REGS_IN_MASK16)
	as_bad (_("Maximum %d bits may be set in `mask16' operand"),
		MAX_REGS_IN_MASK16);

next_inst:
      while (!ISALNUM (*paramP) && *paramP != '}')
	  paramP++;
    }

  if (*++paramP != '\0')
    as_warn (_("rest of line ignored; first ignored character is `%c'"),
	     *paramP);

  switch (hi_found + lo_found)
    {
    case 0:
      /* At least one register should be specified.  */
      if (mask == 0)
	as_bad (_("Illegal `mask16' operand, operation is undefined - `%s'"),
		ins_parse);
      break;

    case 1:
      /* HI can't be specified without LO (and vise-versa).  */
      as_bad (_("HI/LO registers should be specified together"));
      break;

    case 2:
      /* HI/LO registers mustn't be masked with additional registers.  */
      if (mask != 0)
	as_bad (_("HI/LO registers should be specified without additional registers"));

    default:
      break;
    }

  sprintf (maskstring, "$0x%x", mask);
  strcat (new_param, maskstring);
  return new_param;
}

/* Print the instruction.
   Handle also cases where the instruction is relaxable/relocatable.  */

void
print_insn (ins *insn)
{
  unsigned int i, j, insn_size;
  char *this_frag;
  unsigned short words[4];
  int addr_mod;

  /* Arrange the insn encodings in a WORD size array.  */
  for (i = 0, j = 0; i < 2; i++)
    {
      words[j++] = (output_opcode[i] >> 16) & 0xFFFF;
      words[j++] = output_opcode[i] & 0xFFFF;
    }

  /* Handle relaxtion.  */
  if ((instruction->flags & RELAXABLE) && relocatable)
    {
      int relax_subtype;

      /* Write the maximal instruction size supported.  */
      insn_size = INSN_MAX_SIZE;

      /* bCC  */
      if (IS_INSN_TYPE (BRANCH_INS))
	relax_subtype = 0;
      /* bal  */
      else if (IS_INSN_TYPE (DCR_BRANCH_INS) || IS_INSN_MNEMONIC ("bal"))
	relax_subtype = 3;
      /* cmpbr  */
      else if (IS_INSN_TYPE (CMPBR_INS))
	relax_subtype = 5;
      else
	abort ();

      this_frag = frag_var (rs_machine_dependent, insn_size * 2,
			    4, relax_subtype,
			    insn->exp.X_add_symbol,
			    insn->exp.X_add_number,
			    0);
    }
  else
    {
      insn_size = instruction->size;
      this_frag = frag_more (insn_size * 2);

      /* Handle relocation.  */
      if ((relocatable) && (insn->rtype != BFD_RELOC_NONE))
	{
	  reloc_howto_type *reloc_howto;
	  int size;

	  reloc_howto = bfd_reloc_type_lookup (stdoutput, insn->rtype);

	  if (!reloc_howto)
	    abort ();

	  size = bfd_get_reloc_size (reloc_howto);

	  if (size < 1 || size > 4)
	    abort ();

	  fix_new_exp (frag_now, this_frag - frag_now->fr_literal,
		       size, &insn->exp, reloc_howto->pc_relative,
		       insn->rtype);
	}
    }

  /* Verify a 2-byte code alignment.  */
  addr_mod = frag_now_fix () & 1;
  if (frag_now->has_code && frag_now->insn_addr != addr_mod)
    as_bad (_("instruction address is not a multiple of 2"));
  frag_now->insn_addr = addr_mod;
  frag_now->has_code = 1;

  /* Write the instruction encoding to frag.  */
  for (i = 0; i < insn_size; i++)
    {
      md_number_to_chars (this_frag, (valueT) words[i], 2);
      this_frag += 2;
    }
}

/* This is the guts of the machine-dependent assembler.  OP points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *op)
{
  ins crx_ins;
  char *param;
  char c;

  /* Reset global variables for a new instruction.  */
  reset_vars (op, &crx_ins);

  /* Strip the mnemonic.  */
  for (param = op; *param != 0 && !ISSPACE (*param); param++)
    ;
  c = *param;
  *param++ = '\0';

  /* Find the instruction.  */
  instruction = (const inst *) hash_find (crx_inst_hash, op);
  if (instruction == NULL)
    {
      as_bad (_("Unknown opcode: `%s'"), op);
      return;
    }

  /* Tie dwarf2 debug info to the address at the start of the insn.  */
  dwarf2_emit_insn (0);

  if (NO_OPERANDS_INST (op))
    /* Handle instructions with no operands.  */
    crx_ins.nargs = 0;
  else
    /* Parse the instruction's operands.  */
    parse_insn (&crx_ins, param);

  /* Assemble the instruction.  */
  if (assemble_insn (op, &crx_ins) == 0)
    {
      as_bad (_("Illegal operands in instruction : `%s'"), ins_parse);
      return;
    }

  /* Print the instruction.  */
  print_insn (&crx_ins);
}
