/* Generic opcode table support for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE m32r-opc.c.

Copyright (C) 1998 Free Software Foundation, Inc.

This file is part of the GNU Binutils and GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "libiberty.h"
#include "bfd.h"
#include "symcat.h"
#include "m32r-opc.h"
#include "opintl.h"

/* Look up instruction INSN_VALUE and extract its fields.
   INSN, if non-null, is the insn table entry.
   Otherwise INSN_VALUE is examined to compute it.
   LENGTH is the bit length of INSN_VALUE if known, otherwise 0.
   0 is only valid if `insn == NULL && ! defined (CGEN_INT_INSN)'.
   If INSN != NULL, LENGTH must be valid.
   ALIAS_P is non-zero if alias insns are to be included in the search.

   The result a pointer to the insn table entry, or NULL if the instruction
   wasn't recognized.  */

const CGEN_INSN *
m32r_cgen_lookup_insn (insn, insn_value, length, fields, alias_p)
     const CGEN_INSN *insn;
     cgen_insn_t insn_value;
     int length;
     CGEN_FIELDS *fields;
     int alias_p;
{
  char buf[16];

  if (!insn)
    {
      const CGEN_INSN_LIST *insn_list;

#ifdef CGEN_INT_INSN
      switch (length)
	{
	case 8:
	  buf[0] = insn_value;
	  break;
	case 16:
	  if (cgen_current_endian == CGEN_ENDIAN_BIG)
	    bfd_putb16 (insn_value, buf);
	  else
	    bfd_putl16 (insn_value, buf);
	  break;
	case 32:
	  if (cgen_current_endian == CGEN_ENDIAN_BIG)
	    bfd_putb32 (insn_value, buf);
	  else
	    bfd_putl32 (insn_value, buf);
	  break;
	default:
	  abort ();
	}
#else
      abort (); /* FIXME: unfinished */
#endif

      /* The instructions are stored in hash lists.
	 Pick the first one and keep trying until we find the right one.  */

      insn_list = CGEN_DIS_LOOKUP_INSN (buf, insn_value);
      while (insn_list != NULL)
	{
	  insn = insn_list->insn;

	  if (alias_p
	      || ! CGEN_INSN_ATTR (insn, CGEN_INSN_ALIAS))
	    {
	      /* Basic bit mask must be correct.  */
	      /* ??? May wish to allow target to defer this check until the
		 extract handler.  */
	      if ((insn_value & CGEN_INSN_MASK (insn)) == CGEN_INSN_VALUE (insn))
		{
		  /* ??? 0 is passed for `pc' */
		  int elength = (*CGEN_EXTRACT_FN (insn)) (insn, NULL,
							   insn_value, fields,
							   (bfd_vma) 0);
		  if (elength > 0)
		    {
		      /* sanity check */
		      if (length != 0 && length != elength)
			abort ();
		      return insn;
		    }
		}
	    }

	  insn_list = CGEN_DIS_NEXT_INSN (insn_list);
	}
    }
  else
    {
      /* Sanity check: can't pass an alias insn if ! alias_p.  */
      if (! alias_p
	  && CGEN_INSN_ATTR (insn, CGEN_INSN_ALIAS))
	abort ();
      /* Sanity check: length must be correct.  */
      if (length != CGEN_INSN_BITSIZE (insn))
	abort ();

      /* ??? 0 is passed for `pc' */
      length = (*CGEN_EXTRACT_FN (insn)) (insn, NULL, insn_value, fields,
					  (bfd_vma) 0);
      /* Sanity check: must succeed.
	 Could relax this later if it ever proves useful.  */
      if (length == 0)
	abort ();
      return insn;
    }

  return NULL;
}

/* Fill in the operand instances used by INSN whose operands are FIELDS.
   INDICES is a pointer to a buffer of MAX_OPERAND_INSTANCES ints to be filled
   in.  */

void
m32r_cgen_get_insn_operands (insn, fields, indices)
     const CGEN_INSN * insn;
     const CGEN_FIELDS * fields;
     int *indices;
{
  const CGEN_OPERAND_INSTANCE *opinst;
  int i;

  for (i = 0, opinst = CGEN_INSN_OPERANDS (insn);
       opinst != NULL
	 && CGEN_OPERAND_INSTANCE_TYPE (opinst) != CGEN_OPERAND_INSTANCE_END;
       ++i, ++opinst)
    {
      const CGEN_OPERAND *op = CGEN_OPERAND_INSTANCE_OPERAND (opinst);
      if (op == NULL)
	indices[i] = CGEN_OPERAND_INSTANCE_INDEX (opinst);
      else
	indices[i] = m32r_cgen_get_int_operand (CGEN_OPERAND_INDEX (op), fields);
    }
}

/* Cover function to m32r_cgen_get_insn_operands when either INSN or FIELDS
   isn't known.
   The INSN, INSN_VALUE, and LENGTH arguments are passed to
   m32r_cgen_lookup_insn unchanged.

   The result is the insn table entry or NULL if the instruction wasn't
   recognized.  */

const CGEN_INSN *
m32r_cgen_lookup_get_insn_operands (insn, insn_value, length, indices)
     const CGEN_INSN *insn;
     cgen_insn_t insn_value;
     int length;
     int *indices;
{
  CGEN_FIELDS fields;

  /* Pass non-zero for ALIAS_P only if INSN != NULL.
     If INSN == NULL, we want a real insn.  */
  insn = m32r_cgen_lookup_insn (insn, insn_value, length, &fields,
				  insn != NULL);
  if (! insn)
    return NULL;

  m32r_cgen_get_insn_operands (insn, &fields, indices);
  return insn;
}
/* Attributes.  */

static const CGEN_ATTR_ENTRY MACH_attr[] =
{
  { "m32r", MACH_M32R },
/* start-sanitize-m32rx */
  { "m32rx", MACH_M32RX },
/* end-sanitize-m32rx */
  { "max", MACH_MAX },
  { 0, 0 }
};

/* start-sanitize-m32rx */
static const CGEN_ATTR_ENTRY PIPE_attr[] =
{
  { "NONE", PIPE_NONE },
  { "O", PIPE_O },
  { "S", PIPE_S },
  { "OS", PIPE_OS },
  { 0, 0 }
};

/* end-sanitize-m32rx */
const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[] =
{
  { "ABS-ADDR", NULL },
  { "FAKE", NULL },
  { "HASH-PREFIX", NULL },
  { "NEGATIVE", NULL },
  { "PCREL-ADDR", NULL },
  { "RELAX", NULL },
  { "RELOC", NULL },
  { "SIGN-OPT", NULL },
  { "UNSIGNED", NULL },
  { 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
/* start-sanitize-m32rx */
  { "PIPE", & PIPE_attr[0] },
/* end-sanitize-m32rx */
  { "ALIAS", NULL },
  { "COND-CTI", NULL },
  { "FILL-SLOT", NULL },
  { "NO-DIS", NULL },
  { "PARALLEL", NULL },
  { "RELAX", NULL },
  { "RELAXABLE", NULL },
  { "SPECIAL", NULL },
  { "UNCOND-CTI", NULL },
  { 0, 0 }
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_gr_entries[] = 
{
  { "fp", 13 },
  { "lr", 14 },
  { "sp", 15 },
  { "r0", 0 },
  { "r1", 1 },
  { "r2", 2 },
  { "r3", 3 },
  { "r4", 4 },
  { "r5", 5 },
  { "r6", 6 },
  { "r7", 7 },
  { "r8", 8 },
  { "r9", 9 },
  { "r10", 10 },
  { "r11", 11 },
  { "r12", 12 },
  { "r13", 13 },
  { "r14", 14 },
  { "r15", 15 }
};

CGEN_KEYWORD m32r_cgen_opval_h_gr = 
{
  & m32r_cgen_opval_h_gr_entries[0],
  19
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_cr_entries[] = 
{
  { "psw", 0 },
  { "cbr", 1 },
  { "spi", 2 },
  { "spu", 3 },
  { "bpc", 6 },
  { "cr0", 0 },
  { "cr1", 1 },
  { "cr2", 2 },
  { "cr3", 3 },
  { "cr4", 4 },
  { "cr5", 5 },
  { "cr6", 6 },
  { "cr7", 7 },
  { "cr8", 8 },
  { "cr9", 9 },
  { "cr10", 10 },
  { "cr11", 11 },
  { "cr12", 12 },
  { "cr13", 13 },
  { "cr14", 14 },
  { "cr15", 15 }
};

CGEN_KEYWORD m32r_cgen_opval_h_cr = 
{
  & m32r_cgen_opval_h_cr_entries[0],
  21
};

/* start-sanitize-m32rx */
CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_accums_entries[] = 
{
  { "a0", 0 },
  { "a1", 1 }
};

CGEN_KEYWORD m32r_cgen_opval_h_accums = 
{
  & m32r_cgen_opval_h_accums_entries[0],
  2
};

/* end-sanitize-m32rx */

/* The hardware table.  */

#define HW_ENT(n) m32r_cgen_hw_entries[n]
static const CGEN_HW_ENTRY m32r_cgen_hw_entries[] =
{
  { HW_H_PC, & HW_ENT (HW_H_PC + 1), "h-pc", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_MEMORY, & HW_ENT (HW_H_MEMORY + 1), "h-memory", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_SINT, & HW_ENT (HW_H_SINT + 1), "h-sint", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_UINT, & HW_ENT (HW_H_UINT + 1), "h-uint", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_ADDR, & HW_ENT (HW_H_ADDR + 1), "h-addr", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_IADDR, & HW_ENT (HW_H_IADDR + 1), "h-iaddr", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_HI16, & HW_ENT (HW_H_HI16 + 1), "h-hi16", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_SLO16, & HW_ENT (HW_H_SLO16 + 1), "h-slo16", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_ULO16, & HW_ENT (HW_H_ULO16 + 1), "h-ulo16", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_GR, & HW_ENT (HW_H_GR + 1), "h-gr", CGEN_ASM_KEYWORD, (PTR) & m32r_cgen_opval_h_gr },
  { HW_H_CR, & HW_ENT (HW_H_CR + 1), "h-cr", CGEN_ASM_KEYWORD, (PTR) & m32r_cgen_opval_h_cr },
  { HW_H_ACCUM, & HW_ENT (HW_H_ACCUM + 1), "h-accum", CGEN_ASM_KEYWORD, (PTR) 0 },
/* start-sanitize-m32rx */
  { HW_H_ACCUMS, & HW_ENT (HW_H_ACCUMS + 1), "h-accums", CGEN_ASM_KEYWORD, (PTR) & m32r_cgen_opval_h_accums },
/* end-sanitize-m32rx */
  { HW_H_COND, & HW_ENT (HW_H_COND + 1), "h-cond", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_SM, & HW_ENT (HW_H_SM + 1), "h-sm", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_BSM, & HW_ENT (HW_H_BSM + 1), "h-bsm", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_IE, & HW_ENT (HW_H_IE + 1), "h-ie", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_BIE, & HW_ENT (HW_H_BIE + 1), "h-bie", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_BCOND, & HW_ENT (HW_H_BCOND + 1), "h-bcond", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_BPC, & HW_ENT (HW_H_BPC + 1), "h-bpc", CGEN_ASM_KEYWORD, (PTR) 0 },
  { HW_H_LOCK, & HW_ENT (HW_H_LOCK + 1), "h-lock", CGEN_ASM_KEYWORD, (PTR) 0 },
  { 0 }
};

/* The operand table.  */

#define OPERAND(op) CONCAT2 (M32R_OPERAND_,op)
#define OP_ENT(op) m32r_cgen_operand_table[OPERAND (op)]

const CGEN_OPERAND m32r_cgen_operand_table[MAX_OPERANDS] =
{
/* pc: program counter */
  { "pc", & HW_ENT (HW_H_PC), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
/* sr: source register */
  { "sr", & HW_ENT (HW_H_GR), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dr: destination register */
  { "dr", & HW_ENT (HW_H_GR), 4, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* src1: source register 1 */
  { "src1", & HW_ENT (HW_H_GR), 4, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* src2: source register 2 */
  { "src2", & HW_ENT (HW_H_GR), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* scr: source control register */
  { "scr", & HW_ENT (HW_H_CR), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dcr: destination control register */
  { "dcr", & HW_ENT (HW_H_CR), 4, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* simm8: 8 bit signed immediate */
  { "simm8", & HW_ENT (HW_H_SINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX), { 0 } }  },
/* simm16: 16 bit signed immediate */
  { "simm16", & HW_ENT (HW_H_SINT), 16, 16,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX), { 0 } }  },
/* uimm4: 4 bit trap number */
  { "uimm4", & HW_ENT (HW_H_UINT), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm5: 5 bit shift count */
  { "uimm5", & HW_ENT (HW_H_UINT), 11, 5,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm16: 16 bit unsigned immediate */
  { "uimm16", & HW_ENT (HW_H_UINT), 16, 16,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* start-sanitize-m32rx */
/* imm1: 1 bit immediate */
  { "imm1", & HW_ENT (HW_H_UINT), 15, 1,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* accd: accumulator destination register */
  { "accd", & HW_ENT (HW_H_ACCUMS), 4, 2,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* accs: accumulator source register */
  { "accs", & HW_ENT (HW_H_ACCUMS), 12, 2,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* acc: accumulator reg (d) */
  { "acc", & HW_ENT (HW_H_ACCUMS), 8, 1,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* end-sanitize-m32rx */
/* hash: # prefix */
  { "hash", & HW_ENT (HW_H_SINT), 0, 0,
    { 0, 0, { 0 } }  },
/* hi16: high 16 bit immediate, sign optional */
  { "hi16", & HW_ENT (HW_H_HI16), 16, 16,
    { 0, 0|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* slo16: 16 bit signed immediate, for low() */
  { "slo16", & HW_ENT (HW_H_SLO16), 16, 16,
    { 0, 0, { 0 } }  },
/* ulo16: 16 bit unsigned immediate, for low() */
  { "ulo16", & HW_ENT (HW_H_ULO16), 16, 16,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm24: 24 bit address */
  { "uimm24", & HW_ENT (HW_H_ADDR), 8, 24,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_ABS_ADDR)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* disp8: 8 bit displacement */
  { "disp8", & HW_ENT (HW_H_IADDR), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* disp16: 16 bit displacement */
  { "disp16", & HW_ENT (HW_H_IADDR), 16, 16,
    { 0, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* disp24: 24 bit displacement */
  { "disp24", & HW_ENT (HW_H_IADDR), 8, 24,
    { 0, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* condbit: condition bit */
  { "condbit", & HW_ENT (HW_H_COND), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
/* accum: accumulator */
  { "accum", & HW_ENT (HW_H_ACCUM), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
};

/* Operand references.  */

#define INPUT CGEN_OPERAND_INSTANCE_INPUT
#define OUTPUT CGEN_OPERAND_INSTANCE_OUTPUT

static const CGEN_OPERAND_INSTANCE fmt_add_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_add3_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_and3_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (UIMM16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_or3_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_ULO16), CGEN_MODE_UHI, & OP_ENT (ULO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addi_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_SINT), CGEN_MODE_SI, & OP_ENT (SIMM8), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addv_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addv3_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SINT), CGEN_MODE_SI, & OP_ENT (SIMM16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addx_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_bc8_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP8), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_bc24_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP24), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_beq_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP16), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_beqz_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP16), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_bl8_ops[] = {
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP8), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, 0, 14 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_bl24_ops[] = {
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP24), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, 0, 14 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_bcl8_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP8), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, 0, 14 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_bcl24_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP24), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, 0, 14 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_bra8_ops[] = {
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP8), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_bra24_ops[] = {
  { INPUT, & HW_ENT (HW_H_IADDR), CGEN_MODE_USI, & OP_ENT (DISP24), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_cmp_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_cmpi_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_SINT), CGEN_MODE_SI, & OP_ENT (SIMM16), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_cmpz_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_div_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_jc_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_jl_ops[] = {
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, 0, 14 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_jmp_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ld_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ld_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldb_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldb_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldh_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldh_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ld_plus_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ld24_ops[] = {
  { INPUT, & HW_ENT (HW_H_ADDR), CGEN_MODE_USI, & OP_ENT (UIMM24), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldi8_ops[] = {
  { INPUT, & HW_ENT (HW_H_SINT), CGEN_MODE_SI, & OP_ENT (SIMM8), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldi16_ops[] = {
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_lock_ops[] = {
  { INPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_LOCK), CGEN_MODE_UBI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_machi_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_machi_a_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACC), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACC), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mulhi_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mulhi_a_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACC), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mv_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_mvfachi_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mvfachi_a_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACCS), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mvfc_ops[] = {
  { INPUT, & HW_ENT (HW_H_CR), CGEN_MODE_USI, & OP_ENT (SCR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_mvtachi_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mvtachi_a_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACCS), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACCS), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mvtc_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_CR), CGEN_MODE_USI, & OP_ENT (DCR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_rac_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUM), CGEN_MODE_DI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_rac_dsi_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACCS), 0 },
  { INPUT, & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (IMM1), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, & OP_ENT (ACCD), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_rte_ops[] = {
  { INPUT, & HW_ENT (HW_H_BSM), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_BIE), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_BCOND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_BPC), CGEN_MODE_SI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_SM), CGEN_MODE_UBI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_IE), CGEN_MODE_UBI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_seth_ops[] = {
  { INPUT, & HW_ENT (HW_H_HI16), CGEN_MODE_SI, & OP_ENT (HI16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_sll3_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { INPUT, & HW_ENT (HW_H_SINT), CGEN_MODE_SI, & OP_ENT (SIMM16), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_slli_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { INPUT, & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (UIMM5), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_st_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_st_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_stb_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_QI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_stb_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_QI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_sth_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_HI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_sth_d_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_SLO16), CGEN_MODE_HI, & OP_ENT (SLO16), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_HI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_st_plus_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_trap_ops[] = {
  { INPUT, & HW_ENT (HW_H_PC), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_CR), CGEN_MODE_USI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_UINT), CGEN_MODE_SI, & OP_ENT (UIMM4), 0 },
  { OUTPUT, & HW_ENT (HW_H_CR), CGEN_MODE_USI, 0, 6 },
  { OUTPUT, & HW_ENT (HW_H_CR), CGEN_MODE_USI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_PC), CGEN_MODE_SI, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_unlock_ops[] = {
  { INPUT, & HW_ENT (HW_H_LOCK), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (SRC2), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { OUTPUT, & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_LOCK), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_satb_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_sat_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SR), 0 },
  { OUTPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (DR), 0 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_sadd_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 1 },
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_macwu1_ops[] = {
  { INPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 1 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 1 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_mulwu1_ops[] = {
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC1), 0 },
  { INPUT, & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (SRC2), 0 },
  { OUTPUT, & HW_ENT (HW_H_ACCUMS), CGEN_MODE_DI, 0, 1 },
  { 0 }
};

/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
static const CGEN_OPERAND_INSTANCE fmt_sc_ops[] = {
  { INPUT, & HW_ENT (HW_H_COND), CGEN_MODE_UBI, 0, 0 },
  { 0 }
};

/* end-sanitize-m32rx */
#undef INPUT
#undef OUTPUT

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The instruction table.
   This is currently non-static because the simulator accesses it
   directly.  */

const CGEN_INSN m32r_cgen_insn_table_entries[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus illegal.
     Also, the special `illegal' insn resides here.  */
  { { 0 }, 0 },
/* add $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADD, "add", "add",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0xa0,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(PARALLEL), { (1<<MACH_M32R), PIPE_OS } }
  },
/* add3 $dr,$sr,$hash$slo16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADD3, "add3", "add3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (HASH), OP (SLO16), 0 },
    { 32, 32, 0xf0f00000 }, 0x80a00000,
    (PTR) & fmt_add3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* and $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_AND, "and", "and",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0xc0,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(PARALLEL), { (1<<MACH_M32R), PIPE_OS } }
  },
/* and3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_AND3, "and3", "and3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (UIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x80c00000,
    (PTR) & fmt_and3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* or $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_OR, "or", "or",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0xe0,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(PARALLEL), { (1<<MACH_M32R), PIPE_OS } }
  },
/* or3 $dr,$sr,$hash$ulo16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_OR3, "or3", "or3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (HASH), OP (ULO16), 0 },
    { 32, 32, 0xf0f00000 }, 0x80e00000,
    (PTR) & fmt_or3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* xor $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_XOR, "xor", "xor",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0xd0,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(PARALLEL), { (1<<MACH_M32R), PIPE_OS } }
  },
/* xor3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_XOR3, "xor3", "xor3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (UIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x80d00000,
    (PTR) & fmt_and3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* addi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADDI, "addi", "addi",
    { MNEM, ' ', OP (DR), ',', OP (SIMM8), 0 },
    { 16, 16, 0xf000 }, 0x4000,
    (PTR) & fmt_addi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* addv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADDV, "addv", "addv",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x80,
    (PTR) & fmt_addv_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* addv3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADDV3, "addv3", "addv3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (SIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x80800000,
    (PTR) & fmt_addv3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* addx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ADDX, "addx", "addx",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x90,
    (PTR) & fmt_addx_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* bc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BC8, "bc8", "bc.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7c00,
    (PTR) & fmt_bc8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* bc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BC24, "bc24", "bc.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfc000000,
    (PTR) & fmt_bc24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* beq $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BEQ, "beq", "beq",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xf0f00000 }, 0xb0000000,
    (PTR) & fmt_beq_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* beqz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BEQZ, "beqz", "beqz",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0800000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bgez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BGEZ, "bgez", "bgez",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0b00000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bgtz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BGTZ, "bgtz", "bgtz",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0d00000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* blez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BLEZ, "blez", "blez",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0c00000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bltz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BLTZ, "bltz", "bltz",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0a00000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bnez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNEZ, "bnez", "bnez",
    { MNEM, ' ', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xfff00000 }, 0xb0900000,
    (PTR) & fmt_beqz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BL8, "bl8", "bl.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7e00,
    (PTR) & fmt_bl8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* bl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BL24, "bl24", "bl.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfe000000,
    (PTR) & fmt_bl24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* start-sanitize-m32rx */
/* bcl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BCL8, "bcl8", "bcl.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7800,
    (PTR) & fmt_bcl8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* bcl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BCL24, "bcl24", "bcl.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xf8000000,
    (PTR) & fmt_bcl24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* bnc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNC8, "bnc8", "bnc.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7d00,
    (PTR) & fmt_bc8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* bnc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNC24, "bnc24", "bnc.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfd000000,
    (PTR) & fmt_bc24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bne $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNE, "bne", "bne",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (DISP16), 0 },
    { 32, 32, 0xf0f00000 }, 0xb0100000,
    (PTR) & fmt_beq_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bra.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BRA8, "bra8", "bra.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7f00,
    (PTR) & fmt_bra8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* bra.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BRA24, "bra24", "bra.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xff000000,
    (PTR) & fmt_bra24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* start-sanitize-m32rx */
/* bncl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNCL8, "bncl8", "bncl.s",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7900,
    (PTR) & fmt_bcl8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* bncl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_BNCL24, "bncl24", "bncl.l",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xf9000000,
    (PTR) & fmt_bcl24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* cmp $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMP, "cmp", "cmp",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x40,
    (PTR) & fmt_cmp_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* cmpi $src2,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMPI, "cmpi", "cmpi",
    { MNEM, ' ', OP (SRC2), ',', OP (SIMM16), 0 },
    { 32, 32, 0xfff00000 }, 0x80400000,
    (PTR) & fmt_cmpi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* cmpu $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMPU, "cmpu", "cmpu",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x50,
    (PTR) & fmt_cmp_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* cmpui $src2,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMPUI, "cmpui", "cmpui",
    { MNEM, ' ', OP (SRC2), ',', OP (SIMM16), 0 },
    { 32, 32, 0xfff00000 }, 0x80500000,
    (PTR) & fmt_cmpi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* start-sanitize-m32rx */
/* cmpeq $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMPEQ, "cmpeq", "cmpeq",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x60,
    (PTR) & fmt_cmp_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_OS } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* cmpz $src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_CMPZ, "cmpz", "cmpz",
    { MNEM, ' ', OP (SRC2), 0 },
    { 16, 16, 0xfff0 }, 0x70,
    (PTR) & fmt_cmpz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_OS } }
  },
/* end-sanitize-m32rx */
/* div $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_DIV, "div", "div",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x90000000,
    (PTR) & fmt_div_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* divu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_DIVU, "divu", "divu",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x90100000,
    (PTR) & fmt_div_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* rem $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_REM, "rem", "rem",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x90200000,
    (PTR) & fmt_div_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* remu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_REMU, "remu", "remu",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x90300000,
    (PTR) & fmt_div_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* start-sanitize-m32rx */
/* divh $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_DIVH, "divh", "divh",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x90000010,
    (PTR) & fmt_div_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* jc $sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_JC, "jc", "jc",
    { MNEM, ' ', OP (SR), 0 },
    { 16, 16, 0xfff0 }, 0x1cc0,
    (PTR) & fmt_jc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL)|A(COND_CTI), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* jnc $sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_JNC, "jnc", "jnc",
    { MNEM, ' ', OP (SR), 0 },
    { 16, 16, 0xfff0 }, 0x1dc0,
    (PTR) & fmt_jc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL)|A(COND_CTI), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
/* jl $sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_JL, "jl", "jl",
    { MNEM, ' ', OP (SR), 0 },
    { 16, 16, 0xfff0 }, 0x1ec0,
    (PTR) & fmt_jl_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* jmp $sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_JMP, "jmp", "jmp",
    { MNEM, ' ', OP (SR), 0 },
    { 16, 16, 0xfff0 }, 0x1fc0,
    (PTR) & fmt_jmp_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* ld $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LD, "ld", "ld",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x20c0,
    (PTR) & fmt_ld_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* ld $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LD_D, "ld-d", "ld",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SLO16), ',', OP (SR), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0c00000,
    (PTR) & fmt_ld_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldb $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDB, "ldb", "ldb",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x2080,
    (PTR) & fmt_ldb_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* ldb $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDB_D, "ldb-d", "ldb",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SLO16), ',', OP (SR), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0800000,
    (PTR) & fmt_ldb_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDH, "ldh", "ldh",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x20a0,
    (PTR) & fmt_ldh_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* ldh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDH_D, "ldh-d", "ldh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SLO16), ',', OP (SR), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0a00000,
    (PTR) & fmt_ldh_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldub $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDUB, "ldub", "ldub",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x2090,
    (PTR) & fmt_ldb_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* ldub $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDUB_D, "ldub-d", "ldub",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SLO16), ',', OP (SR), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0900000,
    (PTR) & fmt_ldb_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* lduh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDUH, "lduh", "lduh",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x20b0,
    (PTR) & fmt_ldh_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* lduh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDUH_D, "lduh-d", "lduh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SLO16), ',', OP (SR), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0b00000,
    (PTR) & fmt_ldh_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ld $dr,@$sr+ */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LD_PLUS, "ld-plus", "ld",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), '+', 0 },
    { 16, 16, 0xf0f0 }, 0x20e0,
    (PTR) & fmt_ld_plus_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* ld24 $dr,$uimm24 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LD24, "ld24", "ld24",
    { MNEM, ' ', OP (DR), ',', OP (UIMM24), 0 },
    { 32, 32, 0xf0000000 }, 0xe0000000,
    (PTR) & fmt_ld24_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldi8 $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDI8, "ldi8", "ldi8",
    { MNEM, ' ', OP (DR), ',', OP (SIMM8), 0 },
    { 16, 16, 0xf000 }, 0x6000,
    (PTR) & fmt_ldi8_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* ldi16 $dr,$hash$slo16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LDI16, "ldi16", "ldi16",
    { MNEM, ' ', OP (DR), ',', OP (HASH), OP (SLO16), 0 },
    { 32, 32, 0xf0ff0000 }, 0x90f00000,
    (PTR) & fmt_ldi16_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* lock $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_LOCK, "lock", "lock",
    { MNEM, ' ', OP (DR), ',', '@', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x20d0,
    (PTR) & fmt_lock_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* machi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACHI, "machi", "machi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3040,
    (PTR) & fmt_machi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* machi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACHI_A, "machi-a", "machi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (ACC), 0 },
    { 16, 16, 0xf070 }, 0x3040,
    (PTR) & fmt_machi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* maclo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACLO, "maclo", "maclo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3050,
    (PTR) & fmt_machi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* maclo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACLO_A, "maclo-a", "maclo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (ACC), 0 },
    { 16, 16, 0xf070 }, 0x3050,
    (PTR) & fmt_machi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* macwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACWHI, "macwhi", "macwhi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3060,
    (PTR) & fmt_machi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* macwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACWLO, "macwlo", "macwlo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3070,
    (PTR) & fmt_machi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* mul $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MUL, "mul", "mul",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x1060,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* mulhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULHI, "mulhi", "mulhi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3000,
    (PTR) & fmt_mulhi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mulhi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULHI_A, "mulhi-a", "mulhi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (ACC), 0 },
    { 16, 16, 0xf070 }, 0x3000,
    (PTR) & fmt_mulhi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mullo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULLO, "mullo", "mullo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3010,
    (PTR) & fmt_mulhi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mullo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULLO_A, "mullo-a", "mullo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), ',', OP (ACC), 0 },
    { 16, 16, 0xf070 }, 0x3010,
    (PTR) & fmt_mulhi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mulwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULWHI, "mulwhi", "mulwhi",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3020,
    (PTR) & fmt_mulhi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* mulwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULWLO, "mulwlo", "mulwlo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x3030,
    (PTR) & fmt_mulhi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* mv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MV, "mv", "mv",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x1080,
    (PTR) & fmt_mv_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* mvfachi $dr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACHI, "mvfachi", "mvfachi",
    { MNEM, ' ', OP (DR), 0 },
    { 16, 16, 0xf0ff }, 0x50f0,
    (PTR) & fmt_mvfachi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mvfachi $dr,$accs */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACHI_A, "mvfachi-a", "mvfachi",
    { MNEM, ' ', OP (DR), ',', OP (ACCS), 0 },
    { 16, 16, 0xf0f3 }, 0x50f0,
    (PTR) & fmt_mvfachi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mvfaclo $dr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACLO, "mvfaclo", "mvfaclo",
    { MNEM, ' ', OP (DR), 0 },
    { 16, 16, 0xf0ff }, 0x50f1,
    (PTR) & fmt_mvfachi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mvfaclo $dr,$accs */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACLO_A, "mvfaclo-a", "mvfaclo",
    { MNEM, ' ', OP (DR), ',', OP (ACCS), 0 },
    { 16, 16, 0xf0f3 }, 0x50f1,
    (PTR) & fmt_mvfachi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mvfacmi $dr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACMI, "mvfacmi", "mvfacmi",
    { MNEM, ' ', OP (DR), 0 },
    { 16, 16, 0xf0ff }, 0x50f2,
    (PTR) & fmt_mvfachi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mvfacmi $dr,$accs */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFACMI_A, "mvfacmi-a", "mvfacmi",
    { MNEM, ' ', OP (DR), ',', OP (ACCS), 0 },
    { 16, 16, 0xf0f3 }, 0x50f2,
    (PTR) & fmt_mvfachi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mvfc $dr,$scr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVFC, "mvfc", "mvfc",
    { MNEM, ' ', OP (DR), ',', OP (SCR), 0 },
    { 16, 16, 0xf0f0 }, 0x1090,
    (PTR) & fmt_mvfc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* mvtachi $src1 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVTACHI, "mvtachi", "mvtachi",
    { MNEM, ' ', OP (SRC1), 0 },
    { 16, 16, 0xf0ff }, 0x5070,
    (PTR) & fmt_mvtachi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mvtachi $src1,$accs */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVTACHI_A, "mvtachi-a", "mvtachi",
    { MNEM, ' ', OP (SRC1), ',', OP (ACCS), 0 },
    { 16, 16, 0xf0f3 }, 0x5070,
    (PTR) & fmt_mvtachi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mvtaclo $src1 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVTACLO, "mvtaclo", "mvtaclo",
    { MNEM, ' ', OP (SRC1), 0 },
    { 16, 16, 0xf0ff }, 0x5071,
    (PTR) & fmt_mvtachi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* mvtaclo $src1,$accs */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVTACLO_A, "mvtaclo-a", "mvtaclo",
    { MNEM, ' ', OP (SRC1), ',', OP (ACCS), 0 },
    { 16, 16, 0xf0f3 }, 0x5071,
    (PTR) & fmt_mvtachi_a_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* mvtc $sr,$dcr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MVTC, "mvtc", "mvtc",
    { MNEM, ' ', OP (SR), ',', OP (DCR), 0 },
    { 16, 16, 0xf0f0 }, 0x10a0,
    (PTR) & fmt_mvtc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* neg $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_NEG, "neg", "neg",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x30,
    (PTR) & fmt_mv_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* nop */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_NOP, "nop", "nop",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x7000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* not $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_NOT, "not", "not",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0xb0,
    (PTR) & fmt_mv_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* rac */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_RAC, "rac", "rac",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x5090,
    (PTR) & fmt_rac_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* rac $accd,$accs,$imm1 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_RAC_DSI, "rac-dsi", "rac",
    { MNEM, ' ', OP (ACCD), ',', OP (ACCS), ',', OP (IMM1), 0 },
    { 16, 16, 0xf3f2 }, 0x5090,
    (PTR) & fmt_rac_dsi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* rach */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_RACH, "rach", "rach",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x5080,
    (PTR) & fmt_rac_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_S } }
  },
/* start-sanitize-m32rx */
/* rach $accd,$accs,$imm1 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_RACH_DSI, "rach-dsi", "rach",
    { MNEM, ' ', OP (ACCD), ',', OP (ACCS), ',', OP (IMM1), 0 },
    { 16, 16, 0xf3f2 }, 0x5080,
    (PTR) & fmt_rac_dsi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* rte */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_RTE, "rte", "rte",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x10d6,
    (PTR) & fmt_rte_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* seth $dr,$hash$hi16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SETH, "seth", "seth",
    { MNEM, ' ', OP (DR), ',', OP (HASH), OP (HI16), 0 },
    { 32, 32, 0xf0ff0000 }, 0xd0c00000,
    (PTR) & fmt_seth_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* sll $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SLL, "sll", "sll",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x1040,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* sll3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SLL3, "sll3", "sll3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (SIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x90c00000,
    (PTR) & fmt_sll3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* slli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SLLI, "slli", "slli",
    { MNEM, ' ', OP (DR), ',', OP (UIMM5), 0 },
    { 16, 16, 0xf0e0 }, 0x5040,
    (PTR) & fmt_slli_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* sra $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRA, "sra", "sra",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x1020,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* sra3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRA3, "sra3", "sra3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (SIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x90a00000,
    (PTR) & fmt_sll3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* srai $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRAI, "srai", "srai",
    { MNEM, ' ', OP (DR), ',', OP (UIMM5), 0 },
    { 16, 16, 0xf0e0 }, 0x5020,
    (PTR) & fmt_slli_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* srl $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRL, "srl", "srl",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x1000,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* srl3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRL3, "srl3", "srl3",
    { MNEM, ' ', OP (DR), ',', OP (SR), ',', OP (SIMM16), 0 },
    { 32, 32, 0xf0f00000 }, 0x90800000,
    (PTR) & fmt_sll3_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* srli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SRLI, "srli", "srli",
    { MNEM, ' ', OP (DR), ',', OP (UIMM5), 0 },
    { 16, 16, 0xf0e0 }, 0x5000,
    (PTR) & fmt_slli_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* st $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ST, "st", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2040,
    (PTR) & fmt_st_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* st $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ST_D, "st-d", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SLO16), ',', OP (SRC2), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0400000,
    (PTR) & fmt_st_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* stb $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_STB, "stb", "stb",
    { MNEM, ' ', OP (SRC1), ',', '@', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2000,
    (PTR) & fmt_stb_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* stb $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_STB_D, "stb-d", "stb",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SLO16), ',', OP (SRC2), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0000000,
    (PTR) & fmt_stb_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* sth $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_STH, "sth", "sth",
    { MNEM, ' ', OP (SRC1), ',', '@', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2020,
    (PTR) & fmt_sth_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* sth $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_STH_D, "sth-d", "sth",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SLO16), ',', OP (SRC2), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0200000,
    (PTR) & fmt_sth_d_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_NONE } }
  },
/* st $src1,@+$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ST_PLUS, "st-plus", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', '+', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2060,
    (PTR) & fmt_st_plus_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* st $src1,@-$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_ST_MINUS, "st-minus", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', '-', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2070,
    (PTR) & fmt_st_plus_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* sub $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SUB, "sub", "sub",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x20,
    (PTR) & fmt_add_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* subv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SUBV, "subv", "subv",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x0,
    (PTR) & fmt_addv_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* subx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SUBX, "subx", "subx",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 16, 16, 0xf0f0 }, 0x10,
    (PTR) & fmt_addx_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_OS } }
  },
/* trap $uimm4 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_TRAP, "trap", "trap",
    { MNEM, ' ', OP (UIMM4), 0 },
    { 16, 16, 0xfff0 }, 0x10f0,
    (PTR) & fmt_trap_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_M32R), PIPE_O } }
  },
/* unlock $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_UNLOCK, "unlock", "unlock",
    { MNEM, ' ', OP (SRC1), ',', '@', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x2050,
    (PTR) & fmt_unlock_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32R), PIPE_O } }
  },
/* start-sanitize-m32rx */
/* satb $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SATB, "satb", "satb",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x80600300,
    (PTR) & fmt_satb_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* sath $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SATH, "sath", "sath",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x80600200,
    (PTR) & fmt_satb_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* sat $dr,$sr */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SAT, "sat", "sat",
    { MNEM, ' ', OP (DR), ',', OP (SR), 0 },
    { 32, 32, 0xf0f0ffff }, 0x80600000,
    (PTR) & fmt_sat_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL), { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* pcmpbz $src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_PCMPBZ, "pcmpbz", "pcmpbz",
    { MNEM, ' ', OP (SRC2), 0 },
    { 16, 16, 0xfff0 }, 0x370,
    (PTR) & fmt_cmpz_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL), { (1<<MACH_M32RX), PIPE_OS } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* sadd */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SADD, "sadd", "sadd",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x50e4,
    (PTR) & fmt_sadd_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* macwu1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACWU1, "macwu1", "macwu1",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x50b0,
    (PTR) & fmt_macwu1_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* msblo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MSBLO, "msblo", "msblo",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x50d0,
    (PTR) & fmt_machi_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* mulwu1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MULWU1, "mulwu1", "mulwu1",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x50a0,
    (PTR) & fmt_mulwu1_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* maclh1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_MACLH1, "maclh1", "maclh1",
    { MNEM, ' ', OP (SRC1), ',', OP (SRC2), 0 },
    { 16, 16, 0xf0f0 }, 0x50c0,
    (PTR) & fmt_macwu1_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_M32RX), PIPE_S } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* sc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SC, "sc", "sc",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x7401,
    (PTR) & fmt_sc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
/* snc */
  {
    { 1, 1, 1, 1 },
    M32R_INSN_SNC, "snc", "snc",
    { MNEM, 0 },
    { 16, 16, 0xffff }, 0x7501,
    (PTR) & fmt_sc_ops[0],
    { CGEN_INSN_NBOOL_ATTRS, 0|A(SPECIAL), { (1<<MACH_M32RX), PIPE_O } }
  },
/* end-sanitize-m32rx */
};

#undef A
#undef MNEM
#undef OP

static CGEN_INSN_TABLE insn_table =
{
  & m32r_cgen_insn_table_entries[0],
  sizeof (CGEN_INSN),
  MAX_INSNS,
  NULL
};

/* Each non-simple macro entry points to an array of expansion possibilities.  */

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The macro instruction table.  */

static const CGEN_INSN macro_insn_table_entries[] =
{
/* bc $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bc8r", "bc",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7c00,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* bc $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bc24r", "bc",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfc000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bl $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bl8r", "bl",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7e00,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(FILL_SLOT)|A(UNCOND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* bl $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bl24r", "bl",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfe000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(UNCOND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bcl $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bcl8r", "bcl",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7800,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32RX), PIPE_O } }
  },
/* bcl $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bcl24r", "bcl",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xf8000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* bnc $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bnc8r", "bnc",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7d00,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* bnc $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bnc24r", "bnc",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xfd000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bra $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bra8r", "bra",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7f00,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(FILL_SLOT)|A(UNCOND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* bra $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bra24r", "bra",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xff000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(UNCOND_CTI)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* bncl $disp8 */
  {
    { 1, 1, 1, 1 },
    -1, "bncl8r", "bncl",
    { MNEM, ' ', OP (DISP8), 0 },
    { 16, 16, 0xff00 }, 0x7900,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAXABLE)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32RX), PIPE_O } }
  },
/* bncl $disp24 */
  {
    { 1, 1, 1, 1 },
    -1, "bncl24r", "bncl",
    { MNEM, ' ', OP (DISP24), 0 },
    { 32, 32, 0xff000000 }, 0xf9000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(RELAX)|A(COND_CTI)|A(ALIAS), { (1<<MACH_M32RX), PIPE_NONE } }
  },
/* ld $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    -1, "ld-2", "ld",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x20c0,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* ld $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "ld-d2", "ld",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0c00000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldb $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    -1, "ldb-2", "ldb",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x2080,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* ldb $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "ldb-d2", "ldb",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0800000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    -1, "ldh-2", "ldh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x20a0,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* ldh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "ldh-d2", "ldh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0a00000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldub $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    -1, "ldub-2", "ldub",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x2090,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* ldub $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "ldub-d2", "ldub",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0900000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* lduh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    -1, "lduh-2", "lduh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x20b0,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* lduh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "lduh-d2", "lduh",
    { MNEM, ' ', OP (DR), ',', '@', '(', OP (SR), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0b00000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* pop $dr */
  {
    { 1, 1, 1, 1 },
    -1, "pop", "pop",
    { MNEM, ' ', OP (DR), 0 },
    { 16, 16, 0xf0ff }, 0x20ef,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* ldi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    -1, "ldi8a", "ldi",
    { MNEM, ' ', OP (DR), ',', OP (SIMM8), 0 },
    { 16, 16, 0xf000 }, 0x6000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32R), PIPE_OS } }
  },
/* ldi $dr,$hash$slo16 */
  {
    { 1, 1, 1, 1 },
    -1, "ldi16a", "ldi",
    { MNEM, ' ', OP (DR), ',', OP (HASH), OP (SLO16), 0 },
    { 32, 32, 0xf0ff0000 }, 0x90f00000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* rac $accd */
  {
    { 1, 1, 1, 1 },
    -1, "rac-d", "rac",
    { MNEM, ' ', OP (ACCD), 0 },
    { 16, 16, 0xf3ff }, 0x5090,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32RX), PIPE_S } }
  },
/* rac $accd,$accs */
  {
    { 1, 1, 1, 1 },
    -1, "rac-ds", "rac",
    { MNEM, ' ', OP (ACCD), ',', OP (ACCS), 0 },
    { 16, 16, 0xf3f3 }, 0x5090,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32RX), PIPE_S } }
  },
/* rach $accd */
  {
    { 1, 1, 1, 1 },
    -1, "rach-d", "rach",
    { MNEM, ' ', OP (ACCD), 0 },
    { 16, 16, 0xf3ff }, 0x5080,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32RX), PIPE_S } }
  },
/* rach $accd,$accs */
  {
    { 1, 1, 1, 1 },
    -1, "rach-ds", "rach",
    { MNEM, ' ', OP (ACCD), ',', OP (ACCS), 0 },
    { 16, 16, 0xf3f3 }, 0x5080,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32RX), PIPE_S } }
  },
/* st $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    -1, "st-2", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x2040,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* st $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "st-d2", "st",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0400000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* stb $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    -1, "stb-2", "stb",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x2000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* stb $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "stb-d2", "stb",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0000000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* sth $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    -1, "sth-2", "sth",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ')', 0 },
    { 16, 16, 0xf0f0 }, 0x2020,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_O } }
  },
/* sth $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    -1, "sth-d2", "sth",
    { MNEM, ' ', OP (SRC1), ',', '@', '(', OP (SRC2), ',', OP (SLO16), ')', 0 },
    { 32, 32, 0xf0f00000 }, 0xa0200000,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(NO_DIS)|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
/* push $src1 */
  {
    { 1, 1, 1, 1 },
    -1, "push", "push",
    { MNEM, ' ', OP (SRC1), 0 },
    { 16, 16, 0xf0ff }, 0x207f,
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(ALIAS), { (1<<MACH_M32R), PIPE_NONE } }
  },
};

#undef A
#undef MNEM
#undef OP

static CGEN_INSN_TABLE macro_insn_table =
{
  & macro_insn_table_entries[0],
  sizeof (CGEN_INSN),
  (sizeof (macro_insn_table_entries) /
   sizeof (macro_insn_table_entries[0])),
  NULL
};

/* The hash functions are recorded here to help keep assembler code out of
   the disassembler and vice versa.

static int asm_hash_insn_p PARAMS ((const CGEN_INSN *));
static unsigned int asm_hash_insn PARAMS ((const char *));
static int dis_hash_insn_p PARAMS ((const CGEN_INSN *));
static unsigned int dis_hash_insn PARAMS ((const char *, unsigned long));

/* Return non-zero if INSN is to be added to the hash table.
   Targets are free to override CGEN_{ASM,DIS}_HASH_P in the .opc file.  */

static int
asm_hash_insn_p (insn)
     const CGEN_INSN * insn;
{
  return CGEN_ASM_HASH_P (insn);
}

static int
dis_hash_insn_p (insn)
     const CGEN_INSN * insn;
{
  /* If building the hash table and the NO-DIS attribute is present,
     ignore.  */
  if (CGEN_INSN_ATTR (insn, CGEN_INSN_NO_DIS))
    return 0;
  return CGEN_DIS_HASH_P (insn);
}

/* The result is the hash value of the insn.
   Targets are free to override CGEN_{ASM,DIS}_HASH in the .opc file.  */

static unsigned int
asm_hash_insn (mnem)
     const char * mnem;
{
  return CGEN_ASM_HASH (mnem);
}

static unsigned int
dis_hash_insn (buf, value)
     const char * buf;
     unsigned long value;
{
  return CGEN_DIS_HASH (buf, value);
}

const CGEN_OPCODE_TABLE m32r_cgen_opcode_table =
{
  & m32r_cgen_hw_entries[0],
  /*& m32r_cgen_operand_table[0], - FIXME:wip */
  & insn_table,
  & macro_insn_table,
  asm_hash_insn_p, asm_hash_insn, CGEN_ASM_HASH_SIZE,
  dis_hash_insn_p, dis_hash_insn, CGEN_DIS_HASH_SIZE
};

void
m32r_cgen_init_tables (mach)
    int mach;
{
}

/* Getting values from cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they return.
   TODO: floating point, inlining support, remove cases where result type
   not appropriate.  */

int
m32r_cgen_get_int_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  int value;

  switch (opindex)
    {
    case M32R_OPERAND_SR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC1 :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC2 :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_SCR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DCR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SIMM8 :
      value = fields->f_simm8;
      break;
    case M32R_OPERAND_SIMM16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_UIMM4 :
      value = fields->f_uimm4;
      break;
    case M32R_OPERAND_UIMM5 :
      value = fields->f_uimm5;
      break;
    case M32R_OPERAND_UIMM16 :
      value = fields->f_uimm16;
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      value = fields->f_imm1;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      value = fields->f_accd;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      value = fields->f_accs;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      value = fields->f_acc;
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      value = fields->f_nil;
      break;
    case M32R_OPERAND_HI16 :
      value = fields->f_hi16;
      break;
    case M32R_OPERAND_SLO16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_ULO16 :
      value = fields->f_uimm16;
      break;
    case M32R_OPERAND_UIMM24 :
      value = fields->f_uimm24;
      break;
    case M32R_OPERAND_DISP8 :
      value = fields->f_disp8;
      break;
    case M32R_OPERAND_DISP16 :
      value = fields->f_disp16;
      break;
    case M32R_OPERAND_DISP24 :
      value = fields->f_disp24;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting int operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

bfd_vma
m32r_cgen_get_vma_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  bfd_vma value;

  switch (opindex)
    {
    case M32R_OPERAND_SR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC1 :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC2 :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_SCR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DCR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SIMM8 :
      value = fields->f_simm8;
      break;
    case M32R_OPERAND_SIMM16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_UIMM4 :
      value = fields->f_uimm4;
      break;
    case M32R_OPERAND_UIMM5 :
      value = fields->f_uimm5;
      break;
    case M32R_OPERAND_UIMM16 :
      value = fields->f_uimm16;
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      value = fields->f_imm1;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      value = fields->f_accd;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      value = fields->f_accs;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      value = fields->f_acc;
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      value = fields->f_nil;
      break;
    case M32R_OPERAND_HI16 :
      value = fields->f_hi16;
      break;
    case M32R_OPERAND_SLO16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_ULO16 :
      value = fields->f_uimm16;
      break;
    case M32R_OPERAND_UIMM24 :
      value = fields->f_uimm24;
      break;
    case M32R_OPERAND_DISP8 :
      value = fields->f_disp8;
      break;
    case M32R_OPERAND_DISP16 :
      value = fields->f_disp16;
      break;
    case M32R_OPERAND_DISP24 :
      value = fields->f_disp24;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting vma operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

/* Stuffing values in cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they accept.
   TODO: floating point, inlining support, remove cases where argument type
   not appropriate.  */

void
m32r_cgen_set_int_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     int value;
{
  switch (opindex)
    {
    case M32R_OPERAND_SR :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_DR :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SRC1 :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SRC2 :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_SCR :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_DCR :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SIMM8 :
      fields->f_simm8 = value;
      break;
    case M32R_OPERAND_SIMM16 :
      fields->f_simm16 = value;
      break;
    case M32R_OPERAND_UIMM4 :
      fields->f_uimm4 = value;
      break;
    case M32R_OPERAND_UIMM5 :
      fields->f_uimm5 = value;
      break;
    case M32R_OPERAND_UIMM16 :
      fields->f_uimm16 = value;
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      fields->f_imm1 = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      fields->f_accd = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      fields->f_accs = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      fields->f_acc = value;
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      fields->f_nil = value;
      break;
    case M32R_OPERAND_HI16 :
      fields->f_hi16 = value;
      break;
    case M32R_OPERAND_SLO16 :
      fields->f_simm16 = value;
      break;
    case M32R_OPERAND_ULO16 :
      fields->f_uimm16 = value;
      break;
    case M32R_OPERAND_UIMM24 :
      fields->f_uimm24 = value;
      break;
    case M32R_OPERAND_DISP8 :
      fields->f_disp8 = value;
      break;
    case M32R_OPERAND_DISP16 :
      fields->f_disp16 = value;
      break;
    case M32R_OPERAND_DISP24 :
      fields->f_disp24 = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting int operand.\n"),
		       opindex);
      abort ();
  }
}

void
m32r_cgen_set_vma_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     bfd_vma value;
{
  switch (opindex)
    {
    case M32R_OPERAND_SR :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_DR :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SRC1 :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SRC2 :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_SCR :
      fields->f_r2 = value;
      break;
    case M32R_OPERAND_DCR :
      fields->f_r1 = value;
      break;
    case M32R_OPERAND_SIMM8 :
      fields->f_simm8 = value;
      break;
    case M32R_OPERAND_SIMM16 :
      fields->f_simm16 = value;
      break;
    case M32R_OPERAND_UIMM4 :
      fields->f_uimm4 = value;
      break;
    case M32R_OPERAND_UIMM5 :
      fields->f_uimm5 = value;
      break;
    case M32R_OPERAND_UIMM16 :
      fields->f_uimm16 = value;
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      fields->f_imm1 = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      fields->f_accd = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      fields->f_accs = value;
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      fields->f_acc = value;
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      fields->f_nil = value;
      break;
    case M32R_OPERAND_HI16 :
      fields->f_hi16 = value;
      break;
    case M32R_OPERAND_SLO16 :
      fields->f_simm16 = value;
      break;
    case M32R_OPERAND_ULO16 :
      fields->f_uimm16 = value;
      break;
    case M32R_OPERAND_UIMM24 :
      fields->f_uimm24 = value;
      break;
    case M32R_OPERAND_DISP8 :
      fields->f_disp8 = value;
      break;
    case M32R_OPERAND_DISP16 :
      fields->f_disp16 = value;
      break;
    case M32R_OPERAND_DISP24 :
      fields->f_disp24 = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

