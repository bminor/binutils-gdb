/* Generic opcode table support for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE fr30-opc.c.

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
#include "fr30-opc.h"
#include "opintl.h"

/* The hash functions are recorded here to help keep assembler code out of
   the disassembler and vice versa.  */

static int asm_hash_insn_p PARAMS ((const CGEN_INSN *));
static unsigned int asm_hash_insn PARAMS ((const char *));
static int dis_hash_insn_p PARAMS ((const CGEN_INSN *));
static unsigned int dis_hash_insn PARAMS ((const char *, CGEN_INSN_INT));

/* Look up instruction INSN_VALUE and extract its fields.
   INSN, if non-null, is the insn table entry.
   Otherwise INSN_VALUE is examined to compute it.
   LENGTH is the bit length of INSN_VALUE if known, otherwise 0.
   0 is only valid if `insn == NULL && ! CGEN_INT_INSN_P'.
   If INSN != NULL, LENGTH must be valid.
   ALIAS_P is non-zero if alias insns are to be included in the search.

   The result is a pointer to the insn table entry, or NULL if the instruction
   wasn't recognized.  */

const CGEN_INSN *
fr30_cgen_lookup_insn (od, insn, insn_value, length, fields, alias_p)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN *insn;
     CGEN_INSN_BYTES insn_value;
     int length;
     CGEN_FIELDS *fields;
     int alias_p;
{
  unsigned char buf[CGEN_MAX_INSN_SIZE];
  unsigned char *bufp;
  CGEN_INSN_INT base_insn;
#if CGEN_INT_INSN_P
  CGEN_EXTRACT_INFO *info = NULL;
#else
  CGEN_EXTRACT_INFO ex_info;
  CGEN_EXTRACT_INFO *info = &ex_info;
#endif

#if CGEN_INT_INSN_P
  cgen_put_insn_value (od, buf, length, insn_value);
  bufp = buf;
  base_insn = insn_value; /*???*/
#else
  ex_info.dis_info = NULL;
  ex_info.insn_bytes = insn_value;
  ex_info.valid = -1;
  base_insn = cgen_get_insn_value (od, buf, length);
  bufp = insn_value;
#endif

  if (!insn)
    {
      const CGEN_INSN_LIST *insn_list;

      /* The instructions are stored in hash lists.
	 Pick the first one and keep trying until we find the right one.  */

      insn_list = CGEN_DIS_LOOKUP_INSN (od, bufp, base_insn);
      while (insn_list != NULL)
	{
	  insn = insn_list->insn;

	  if (alias_p
	      || ! CGEN_INSN_ATTR (insn, CGEN_INSN_ALIAS))
	    {
	      /* Basic bit mask must be correct.  */
	      /* ??? May wish to allow target to defer this check until the
		 extract handler.  */
	      if ((base_insn & CGEN_INSN_MASK (insn)) == CGEN_INSN_VALUE (insn))
		{
		  /* ??? 0 is passed for `pc' */
		  int elength = (*CGEN_EXTRACT_FN (insn)) (od, insn, info,
							   base_insn, fields,
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
      length = (*CGEN_EXTRACT_FN (insn)) (od, insn, info, base_insn, fields,
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
fr30_cgen_get_insn_operands (od, insn, fields, indices)
     CGEN_OPCODE_DESC od;
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
	indices[i] = fr30_cgen_get_int_operand (CGEN_OPERAND_INDEX (op),
						  fields);
    }
}

/* Cover function to fr30_cgen_get_insn_operands when either INSN or FIELDS
   isn't known.
   The INSN, INSN_VALUE, and LENGTH arguments are passed to
   fr30_cgen_lookup_insn unchanged.

   The result is the insn table entry or NULL if the instruction wasn't
   recognized.  */

const CGEN_INSN *
fr30_cgen_lookup_get_insn_operands (od, insn, insn_value, length, indices)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN *insn;
     CGEN_INSN_BYTES insn_value;
     int length;
     int *indices;
{
  CGEN_FIELDS fields;

  /* Pass non-zero for ALIAS_P only if INSN != NULL.
     If INSN == NULL, we want a real insn.  */
  insn = fr30_cgen_lookup_insn (od, insn, insn_value, length, &fields,
				  insn != NULL);
  if (! insn)
    return NULL;

  fr30_cgen_get_insn_operands (od, insn, &fields, indices);
  return insn;
}
/* Attributes.  */

static const CGEN_ATTR_ENTRY MACH_attr[] =
{
  { "base", MACH_BASE },
  { "fr30", MACH_FR30 },
  { "max", MACH_MAX },
  { 0, 0 }
};

const CGEN_ATTR_TABLE fr30_cgen_hardware_attr_table[] =
{
  { "CACHE-ADDR", NULL },
  { "PC", NULL },
  { "PROFILE", NULL },
  { 0, 0 }
};

const CGEN_ATTR_TABLE fr30_cgen_operand_attr_table[] =
{
  { "ABS-ADDR", NULL },
  { "HASH-PREFIX", NULL },
  { "NEGATIVE", NULL },
  { "PCREL-ADDR", NULL },
  { "RELAX", NULL },
  { "SEM-ONLY", NULL },
  { "SIGN-OPT", NULL },
  { "SIGNED", NULL },
  { "UNSIGNED", NULL },
  { 0, 0 }
};

const CGEN_ATTR_TABLE fr30_cgen_insn_attr_table[] =
{
  { "ALIAS", NULL },
  { "COND-CTI", NULL },
  { "NO-DIS", NULL },
  { "RELAX", NULL },
  { "RELAXABLE", NULL },
  { "SKIP-CTI", NULL },
  { "UNCOND-CTI", NULL },
  { "VIRTUAL", NULL },
  { 0, 0 }
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_gr_entries[] = 
{
  { "ac", 13 },
  { "fp", 14 },
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

CGEN_KEYWORD fr30_cgen_opval_h_gr = 
{
  & fr30_cgen_opval_h_gr_entries[0],
  19
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_dr_entries[] = 
{
  { "tbr", 0 },
  { "rp", 1 },
  { "ssp", 2 },
  { "usp", 3 },
  { "mdh", 4 },
  { "mdl", 5 }
};

CGEN_KEYWORD fr30_cgen_opval_h_dr = 
{
  & fr30_cgen_opval_h_dr_entries[0],
  6
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_ps_entries[] = 
{
  { "ps", 1 }
};

CGEN_KEYWORD fr30_cgen_opval_h_ps = 
{
  & fr30_cgen_opval_h_ps_entries[0],
  1
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_r13_entries[] = 
{
  { "r13", 13 }
};

CGEN_KEYWORD fr30_cgen_opval_h_r13 = 
{
  & fr30_cgen_opval_h_r13_entries[0],
  1
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_r14_entries[] = 
{
  { "r14", 14 }
};

CGEN_KEYWORD fr30_cgen_opval_h_r14 = 
{
  & fr30_cgen_opval_h_r14_entries[0],
  1
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_r15_entries[] = 
{
  { "r15", 15 }
};

CGEN_KEYWORD fr30_cgen_opval_h_r15 = 
{
  & fr30_cgen_opval_h_r15_entries[0],
  1
};


/* The hardware table.  */

#define HW_ENT(n) fr30_cgen_hw_entries[n]
static const CGEN_HW_ENTRY fr30_cgen_hw_entries[] =
{
  { HW_H_PC, & HW_ENT (HW_H_PC + 1), "h-pc", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0|(1<<CGEN_HW_PROFILE)|(1<<CGEN_HW_PC), { 0 } } },
  { HW_H_MEMORY, & HW_ENT (HW_H_MEMORY + 1), "h-memory", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_SINT, & HW_ENT (HW_H_SINT + 1), "h-sint", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_UINT, & HW_ENT (HW_H_UINT + 1), "h-uint", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_ADDR, & HW_ENT (HW_H_ADDR + 1), "h-addr", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_IADDR, & HW_ENT (HW_H_IADDR + 1), "h-iaddr", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_GR, & HW_ENT (HW_H_GR + 1), "h-gr", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_gr, { 0, 0|(1<<CGEN_HW_CACHE_ADDR)|(1<<CGEN_HW_PROFILE), { 0 } } },
  { HW_H_DR, & HW_ENT (HW_H_DR + 1), "h-dr", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_dr, { 0, 0, { 0 } } },
  { HW_H_PS, & HW_ENT (HW_H_PS + 1), "h-ps", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_ps, { 0, 0, { 0 } } },
  { HW_H_R13, & HW_ENT (HW_H_R13 + 1), "h-r13", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_r13, { 0, 0, { 0 } } },
  { HW_H_R14, & HW_ENT (HW_H_R14 + 1), "h-r14", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_r14, { 0, 0, { 0 } } },
  { HW_H_R15, & HW_ENT (HW_H_R15 + 1), "h-r15", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_r15, { 0, 0, { 0 } } },
  { HW_H_NBIT, & HW_ENT (HW_H_NBIT + 1), "h-nbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_ZBIT, & HW_ENT (HW_H_ZBIT + 1), "h-zbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_VBIT, & HW_ENT (HW_H_VBIT + 1), "h-vbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { HW_H_CBIT, & HW_ENT (HW_H_CBIT + 1), "h-cbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0, { 0 } } },
  { 0 }
};

/* The operand table.  */

#define OPERAND(op) CONCAT2 (FR30_OPERAND_,op)
#define OP_ENT(op) fr30_cgen_operand_table[OPERAND (op)]

const CGEN_OPERAND fr30_cgen_operand_table[MAX_OPERANDS] =
{
/* pc: program counter */
  { "pc", & HW_ENT (HW_H_PC), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_SEM_ONLY), { 0 } }  },
/* Ri: destination register */
  { "Ri", & HW_ENT (HW_H_GR), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* Rj: source register */
  { "Rj", & HW_ENT (HW_H_GR), 8, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* Rs1: dedicated register */
  { "Rs1", & HW_ENT (HW_H_DR), 8, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* Rs2: dedicated register */
  { "Rs2", & HW_ENT (HW_H_DR), 12, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* R13: General Register 13 */
  { "R13", & HW_ENT (HW_H_R13), 0, 0,
    { 0, 0, { 0 } }  },
/* R14: General Register 14 */
  { "R14", & HW_ENT (HW_H_R14), 0, 0,
    { 0, 0, { 0 } }  },
/* R15: General Register 15 */
  { "R15", & HW_ENT (HW_H_R15), 0, 0,
    { 0, 0, { 0 } }  },
/* ps: Program Status register */
  { "ps", & HW_ENT (HW_H_PS), 0, 0,
    { 0, 0, { 0 } }  },
/* u4: 4  bit unsigned immediate */
  { "u4", & HW_ENT (HW_H_UINT), 8, 4,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* m4: 4  bit negative immediate */
  { "m4", & HW_ENT (HW_H_UINT), 8, 4,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* u8: 8  bit unsigned immediate */
  { "u8", & HW_ENT (HW_H_UINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* i8: 8  bit unsigned immediate */
  { "i8", & HW_ENT (HW_H_UINT), 4, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* udisp6: 6  bit unsigned immediate */
  { "udisp6", & HW_ENT (HW_H_UINT), 8, 4,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* disp8: 8  bit signed   immediate */
  { "disp8", & HW_ENT (HW_H_SINT), 4, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* disp9: 9  bit signed   immediate */
  { "disp9", & HW_ENT (HW_H_SINT), 4, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* disp10: 10 bit signed   immediate */
  { "disp10", & HW_ENT (HW_H_SINT), 4, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* s10: 10 bit signed   immediate */
  { "s10", & HW_ENT (HW_H_SINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* u10: 10 bit unsigned immediate */
  { "u10", & HW_ENT (HW_H_UINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* i32: 32 bit immediate */
  { "i32", & HW_ENT (HW_H_UINT), 16, 32,
    { 0, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dir8: 8  bit direct address */
  { "dir8", & HW_ENT (HW_H_UINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dir9: 9  bit direct address */
  { "dir9", & HW_ENT (HW_H_UINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dir10: 10 bit direct address */
  { "dir10", & HW_ENT (HW_H_UINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* label9: 9  bit pc relative address */
  { "label9", & HW_ENT (HW_H_SINT), 8, 8,
    { 0, 0|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* label12: 12 bit pc relative address */
  { "label12", & HW_ENT (HW_H_SINT), 5, 11,
    { 0, 0|(1<<CGEN_OPERAND_SIGNED), { 0 } }  },
/* cc: condition codes */
  { "cc", & HW_ENT (HW_H_UINT), 4, 4,
    { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* nbit: negative bit */
  { "nbit", & HW_ENT (HW_H_NBIT), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_SEM_ONLY), { 0 } }  },
/* vbit: overflow bit */
  { "vbit", & HW_ENT (HW_H_VBIT), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_SEM_ONLY), { 0 } }  },
/* zbit: zero     bit */
  { "zbit", & HW_ENT (HW_H_ZBIT), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_SEM_ONLY), { 0 } }  },
/* cbit: carry    bit */
  { "cbit", & HW_ENT (HW_H_CBIT), 0, 0,
    { 0, 0|(1<<CGEN_OPERAND_SEM_ONLY), { 0 } }  },
};

/* Operand references.  */

#define INPUT CGEN_OPERAND_INSTANCE_INPUT
#define OUTPUT CGEN_OPERAND_INSTANCE_OUTPUT
#define COND_REF CGEN_OPERAND_INSTANCE_COND_REF

static const CGEN_OPERAND_INSTANCE fmt_add_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addi_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "u4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (U4), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_add2_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "m4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (M4), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addc_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { INPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addn_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addni_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "u4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (U4), 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_addn2_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "m4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (M4), 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_cmp_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_cmpi_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "u4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (U4), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_cmp2_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "m4", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (M4), 0, 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_and_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_andm_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (RI), 0, 0 },
  { INPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_SI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_andh_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (RI), 0, 0 },
  { INPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_HI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_HI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_andb_ops[] = {
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_USI, & OP_ENT (RI), 0, 0 },
  { INPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0, 0 },
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_QI, & OP_ENT (RJ), 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0, 0 },
  { OUTPUT, "h_memory_Ri", & HW_ENT (HW_H_MEMORY), CGEN_MODE_QI, 0, 0, 0 },
  { 0 }
};

static const CGEN_OPERAND_INSTANCE fmt_ldi32_ops[] = {
  { INPUT, "i32", & HW_ENT (HW_H_UINT), CGEN_MODE_USI, & OP_ENT (I32), 0, 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0, 0 },
  { 0 }
};

#undef INPUT
#undef OUTPUT
#undef COND_REF

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The instruction table.
   This is currently non-static because the simulator accesses it
   directly.  */

const CGEN_INSN fr30_cgen_insn_table_entries[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { { 0 }, 0 },
/* add $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADD, "add", "add",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa600,
    (PTR) & fmt_add_ops[0],
    { 0, 0, { 0 } }
  },
/* add $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDI, "addi", "add",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa400,
    (PTR) & fmt_addi_ops[0],
    { 0, 0, { 0 } }
  },
/* add2 $m4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADD2, "add2", "add2",
    { { MNEM, ' ', OP (M4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa500,
    (PTR) & fmt_add2_ops[0],
    { 0, 0, { 0 } }
  },
/* addc $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDC, "addc", "addc",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa700,
    (PTR) & fmt_addc_ops[0],
    { 0, 0, { 0 } }
  },
/* addn $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDN, "addn", "addn",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa200,
    (PTR) & fmt_addn_ops[0],
    { 0, 0, { 0 } }
  },
/* addn $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDNI, "addni", "addn",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa000,
    (PTR) & fmt_addni_ops[0],
    { 0, 0, { 0 } }
  },
/* addn2 $m4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDN2, "addn2", "addn2",
    { { MNEM, ' ', OP (M4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa100,
    (PTR) & fmt_addn2_ops[0],
    { 0, 0, { 0 } }
  },
/* sub $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_SUB, "sub", "sub",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xac00,
    (PTR) & fmt_add_ops[0],
    { 0, 0, { 0 } }
  },
/* subc $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_SUBC, "subc", "subc",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xad00,
    (PTR) & fmt_addc_ops[0],
    { 0, 0, { 0 } }
  },
/* subn $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_SUBN, "subn", "subn",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xae00,
    (PTR) & fmt_addn_ops[0],
    { 0, 0, { 0 } }
  },
/* cmp $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CMP, "cmp", "cmp",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xaa00,
    (PTR) & fmt_cmp_ops[0],
    { 0, 0, { 0 } }
  },
/* cmp $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CMPI, "cmpi", "cmp",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa800,
    (PTR) & fmt_cmpi_ops[0],
    { 0, 0, { 0 } }
  },
/* cmp2 $m4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CMP2, "cmp2", "cmp2",
    { { MNEM, ' ', OP (M4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xa900,
    (PTR) & fmt_cmp2_ops[0],
    { 0, 0, { 0 } }
  },
/* and $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_AND, "and", "and",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8200,
    (PTR) & fmt_and_ops[0],
    { 0, 0, { 0 } }
  },
/* or $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_OR, "or", "or",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9200,
    (PTR) & fmt_and_ops[0],
    { 0, 0, { 0 } }
  },
/* eor $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EOR, "eor", "eor",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9a00,
    (PTR) & fmt_and_ops[0],
    { 0, 0, { 0 } }
  },
/* and $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ANDM, "andm", "and",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8400,
    (PTR) & fmt_andm_ops[0],
    { 0, 0, { 0 } }
  },
/* andh $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ANDH, "andh", "andh",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8500,
    (PTR) & fmt_andh_ops[0],
    { 0, 0, { 0 } }
  },
/* andb $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ANDB, "andb", "andb",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8600,
    (PTR) & fmt_andb_ops[0],
    { 0, 0, { 0 } }
  },
/* or $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ORM, "orm", "or",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9400,
    (PTR) & fmt_andm_ops[0],
    { 0, 0, { 0 } }
  },
/* orh $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ORH, "orh", "orh",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9500,
    (PTR) & fmt_andh_ops[0],
    { 0, 0, { 0 } }
  },
/* orb $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ORB, "orb", "orb",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9600,
    (PTR) & fmt_andb_ops[0],
    { 0, 0, { 0 } }
  },
/* eor $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EORM, "eorm", "eor",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9c00,
    (PTR) & fmt_andm_ops[0],
    { 0, 0, { 0 } }
  },
/* eorh $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EORH, "eorh", "eorh",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9d00,
    (PTR) & fmt_andh_ops[0],
    { 0, 0, { 0 } }
  },
/* eorb $Rj,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EORB, "eorb", "eorb",
    { { MNEM, ' ', OP (RJ), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9e00,
    (PTR) & fmt_andb_ops[0],
    { 0, 0, { 0 } }
  },
/* bandl $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BANDL, "bandl", "bandl",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* borl $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BORL, "borl", "borl",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* beorl $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BEORL, "beorl", "beorl",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bandh $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BANDH, "bandh", "bandh",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* borh $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BORH, "borh", "borh",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* beorh $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BEORH, "beorh", "beorh",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x9900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* btstl $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BTSTL, "btstl", "btstl",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* btsth $u4,@$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BTSTH, "btsth", "btsth",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mul $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MUL, "mul", "mul",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xaf00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mulu $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MULU, "mulu", "mulu",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xab00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mulh $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MULH, "mulh", "mulh",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xbf00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* muluh $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MULUH, "muluh", "muluh",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xbb00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div0s $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV0S, "div0s", "div0s",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9740,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div0u $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV0U, "div0u", "div0u",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9750,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div1 $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV1, "div1", "div1",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9760,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div2 $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV2, "div2", "div2",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9770,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div3 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV3, "div3", "div3",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9f60,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* div4s */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DIV4S, "div4s", "div4s",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9f70,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsl $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSL, "lsl", "lsl",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb600,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsl $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSLI, "lsli", "lsl",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsl2 $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSL2, "lsl2", "lsl2",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb500,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsr $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSR, "lsr", "lsr",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb200,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsr $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSRI, "lsri", "lsr",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lsr2 $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LSR2, "lsr2", "lsr2",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* asr $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ASR, "asr", "asr",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xba00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* asr $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ASRI, "asri", "asr",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* asr2 $u4,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ASR2, "asr2", "asr2",
    { { MNEM, ' ', OP (U4), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldi:8 $i8,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDI_8, "ldi:8", "ldi:8",
    { { MNEM, ' ', OP (I8), ',', OP (RI), 0 } },
    { 16, 16, 0xf000 }, 0xc000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldi:32 $i32,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDI32, "ldi32", "ldi:32",
    { { MNEM, ' ', OP (I32), ',', OP (RI), 0 } },
    { 16, 48, 0xfff0 }, 0x9f80,
    (PTR) & fmt_ldi32_ops[0],
    { 0, 0, { 0 } }
  },
/* ld @$Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LD, "ld", "ld",
    { { MNEM, ' ', '@', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lduh @$Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDUH, "lduh", "lduh",
    { { MNEM, ' ', '@', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x500,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldub @$Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDUB, "ldub", "ldub",
    { { MNEM, ' ', '@', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x600,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @($r13,$Rj),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR13, "ldr13", "ld",
    { { MNEM, ' ', '@', '(', OP (R13), ',', OP (RJ), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x0,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lduh @($r13,$Rj),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR13UH, "ldr13uh", "lduh",
    { { MNEM, ' ', '@', '(', OP (R13), ',', OP (RJ), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldub @($r13,$Rj),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR13UB, "ldr13ub", "ldub",
    { { MNEM, ' ', '@', '(', OP (R13), ',', OP (RJ), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x200,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @($r14,$disp10),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR14, "ldr14", "ld",
    { { MNEM, ' ', '@', '(', OP (R14), ',', OP (DISP10), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xf000 }, 0x2000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* lduh @($r14,$disp9),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR14UH, "ldr14uh", "lduh",
    { { MNEM, ' ', '@', '(', OP (R14), ',', OP (DISP9), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xf000 }, 0x4000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldub @($r14,$disp8),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR14UB, "ldr14ub", "ldub",
    { { MNEM, ' ', '@', '(', OP (R14), ',', OP (DISP8), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xf000 }, 0x6000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @($r15,$udisp6),$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR15, "ldr15", "ld",
    { { MNEM, ' ', '@', '(', OP (R15), ',', OP (UDISP6), ')', ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @$r15+,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR15GR, "ldr15gr", "ld",
    { { MNEM, ' ', '@', OP (R15), '+', ',', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @$r15+,$Rs2 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR15DR, "ldr15dr", "ld",
    { { MNEM, ' ', '@', OP (R15), '+', ',', OP (RS2), 0 } },
    { 16, 16, 0xfff0 }, 0x780,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ld @$r15+,$ps */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDR15PS, "ldr15ps", "ld",
    { { MNEM, ' ', '@', OP (R15), '+', ',', OP (PS), 0 } },
    { 16, 16, 0xffff }, 0x790,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Ri,@$Rj */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ST, "st", "st",
    { { MNEM, ' ', OP (RI), ',', '@', OP (RJ), 0 } },
    { 16, 16, 0xff00 }, 0x1400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* sth $Ri,@$Rj */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STH, "sth", "sth",
    { { MNEM, ' ', OP (RI), ',', '@', OP (RJ), 0 } },
    { 16, 16, 0xff00 }, 0x1500,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* stb $Ri,@$Rj */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STB, "stb", "stb",
    { { MNEM, ' ', OP (RI), ',', '@', OP (RJ), 0 } },
    { 16, 16, 0xff00 }, 0x1600,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Ri,@($r13,$Rj) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR13, "str13", "st",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R13), ',', OP (RJ), ')', 0 } },
    { 16, 16, 0xff00 }, 0x1000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* sth $Ri,@($r13,$Rj) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR13H, "str13h", "sth",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R13), ',', OP (RJ), ')', 0 } },
    { 16, 16, 0xff00 }, 0x1100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* stb $Ri,@($r13,$Rj) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR13B, "stR13b", "stb",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R13), ',', OP (RJ), ')', 0 } },
    { 16, 16, 0xff00 }, 0x1200,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Ri,@($r14,$disp10) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR14, "str14", "st",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R14), ',', OP (DISP10), ')', 0 } },
    { 16, 16, 0xf000 }, 0x3000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* sth $Ri,@($r14,$disp9) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR14H, "str14h", "sth",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R14), ',', OP (DISP9), ')', 0 } },
    { 16, 16, 0xf000 }, 0x5000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* stb $Ri,@($r14,$disp8) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR14B, "str14b", "stb",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R14), ',', OP (DISP8), ')', 0 } },
    { 16, 16, 0xf000 }, 0x7000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Ri,@($r15,$udisp6) */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR15, "str15", "st",
    { { MNEM, ' ', OP (RI), ',', '@', '(', OP (R15), ',', OP (UDISP6), ')', 0 } },
    { 16, 16, 0xff00 }, 0x1300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Ri,@-$r15 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR15GR, "str15gr", "st",
    { { MNEM, ' ', OP (RI), ',', '@', '-', OP (R15), 0 } },
    { 16, 16, 0xfff0 }, 0x1700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $Rs2,@-$r15 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR15DR, "str15dr", "st",
    { { MNEM, ' ', OP (RS2), ',', '@', '-', OP (R15), 0 } },
    { 16, 16, 0xfff0 }, 0x1780,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* st $ps,@-$r15 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STR15PS, "str15ps", "st",
    { { MNEM, ' ', OP (PS), ',', '@', '-', OP (R15), 0 } },
    { 16, 16, 0xffff }, 0x1790,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mov $Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MOV, "mov", "mov",
    { { MNEM, ' ', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8b00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mov $Rs1,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MOVDR, "movdr", "mov",
    { { MNEM, ' ', OP (RS1), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0xb700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mov $ps,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MOVPS, "movps", "mov",
    { { MNEM, ' ', OP (PS), ',', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x1710,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mov $Ri,$Rs1 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MOV2DR, "mov2dr", "mov",
    { { MNEM, ' ', OP (RI), ',', OP (RS1), 0 } },
    { 16, 16, 0xff00 }, 0xb300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* mov $Ri,$ps */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_MOV2PS, "mov2ps", "mov",
    { { MNEM, ' ', OP (RI), ',', OP (PS), 0 } },
    { 16, 16, 0xfff0 }, 0x710,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* jmp @$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_JMP, "jmp", "jmp",
    { { MNEM, ' ', '@', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* jmp:D @$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_JMPD, "jmpd", "jmp:D",
    { { MNEM, ' ', '@', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9f00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* call $label12 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CALL, "call", "call",
    { { MNEM, ' ', OP (LABEL12), 0 } },
    { 16, 16, 0xf400 }, 0xd000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* call:D $label12 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CALLD, "calld", "call:D",
    { { MNEM, ' ', OP (LABEL12), 0 } },
    { 16, 16, 0xf400 }, 0xd400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* call @$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CALLR, "callr", "call",
    { { MNEM, ' ', '@', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9710,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* call:D @$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_CALLRD, "callrd", "call:D",
    { { MNEM, ' ', '@', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9f10,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ret */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_RET, "ret", "ret",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9720,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ret:D */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_RETD, "retd", "ret:D",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9f20,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* int $u8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_INT, "int", "int",
    { { MNEM, ' ', OP (U8), 0 } },
    { 16, 16, 0xff00 }, 0x1f00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* inte */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_INTE, "inte", "inte",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9f30,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* reti */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_RETI, "reti", "reti",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9730,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bra $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BRA, "bra", "bra",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bno $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNO, "bno", "bno",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* beq $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BEQ, "beq", "beq",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe200,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bne $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNE, "bne", "bne",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bc $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BC, "bc", "bc",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bnc $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNC, "bnc", "bnc",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe500,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bn $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BN, "bn", "bn",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe600,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bp $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BP, "bp", "bp",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bv $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BV, "bv", "bv",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bnv $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNV, "bnv", "bnv",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xe900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* blt $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLT, "blt", "blt",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xea00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bge $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BGE, "bge", "bge",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xeb00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ble $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLE, "ble", "ble",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xec00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bgt $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BGT, "bgt", "bgt",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xed00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bls $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLS, "bls", "bls",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xee00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bhi $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BHI, "bhi", "bhi",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xef00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bra:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BRAD, "brad", "bra:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf000,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bno:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNOD, "bnod", "bno:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf100,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* beq:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BEQD, "beqd", "beq:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf200,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bne:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNED, "bned", "bne:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bc:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BCD, "bcd", "bc:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf400,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bnc:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNCD, "bncd", "bnc:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf500,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bn:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BND, "bnd", "bn:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf600,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bp:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BPD, "bpd", "bp:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bv:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BVD, "bvd", "bv:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bnv:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BNVD, "bnvd", "bnv:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xf900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* blt:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLTD, "bltd", "blt:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xfa00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bge:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BGED, "bged", "bge:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xfb00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ble:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLED, "bled", "ble:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xfc00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bgt:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BGTD, "bgtd", "bgt:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xfd00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bls:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BLSD, "blsd", "bls:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xfe00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* bhi:D $label9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_BHID, "bhid", "bhi:D",
    { { MNEM, ' ', OP (LABEL9), 0 } },
    { 16, 16, 0xff00 }, 0xff00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov @$dir10,$R13 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13, "dmov2r13", "dmov",
    { { MNEM, ' ', '@', OP (DIR10), ',', OP (R13), 0 } },
    { 16, 16, 0xff00 }, 0x800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovh @$dir9,$R13 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13H, "dmov2r13h", "dmovh",
    { { MNEM, ' ', '@', OP (DIR9), ',', OP (R13), 0 } },
    { 16, 16, 0xff00 }, 0x900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovb @$dir8,$R13 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13B, "dmov2r13b", "dmovb",
    { { MNEM, ' ', '@', OP (DIR8), ',', OP (R13), 0 } },
    { 16, 16, 0xff00 }, 0xa00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov $R13,@$dir10 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13, "dmovr13", "dmov",
    { { MNEM, ' ', OP (R13), ',', '@', OP (DIR10), 0 } },
    { 16, 16, 0xff00 }, 0x1800,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovh $R13,@$dir9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13H, "dmovr13h", "dmovh",
    { { MNEM, ' ', OP (R13), ',', '@', OP (DIR9), 0 } },
    { 16, 16, 0xff00 }, 0x1900,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovb $R13,@$dir8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13B, "dmovr13b", "dmovb",
    { { MNEM, ' ', OP (R13), ',', '@', OP (DIR8), 0 } },
    { 16, 16, 0xff00 }, 0x1a00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov @$dir10,@$R13+ */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13PI, "dmov2r13pi", "dmov",
    { { MNEM, ' ', '@', OP (DIR10), ',', '@', OP (R13), '+', 0 } },
    { 16, 16, 0xff00 }, 0xc00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovh @$dir9,@$R13+ */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13PIH, "dmov2r13pih", "dmovh",
    { { MNEM, ' ', '@', OP (DIR9), ',', '@', OP (R13), '+', 0 } },
    { 16, 16, 0xff00 }, 0xd00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovb @$dir8,@$R13+ */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R13PIB, "dmov2r13pib", "dmovb",
    { { MNEM, ' ', '@', OP (DIR8), ',', '@', OP (R13), '+', 0 } },
    { 16, 16, 0xff00 }, 0xe00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov @$R13+,@$dir10 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13PI, "dmovr13pi", "dmov",
    { { MNEM, ' ', '@', OP (R13), '+', ',', '@', OP (DIR10), 0 } },
    { 16, 16, 0xff00 }, 0x1c00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovh @$R13+,@$dir9 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13PIH, "dmovr13pih", "dmovh",
    { { MNEM, ' ', '@', OP (R13), '+', ',', '@', OP (DIR9), 0 } },
    { 16, 16, 0xff00 }, 0x1d00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmovb @$R13+,@$dir8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR13PIB, "dmovr13pib", "dmovb",
    { { MNEM, ' ', '@', OP (R13), '+', ',', '@', OP (DIR8), 0 } },
    { 16, 16, 0xff00 }, 0x1e00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov @$dir10,@-$R15 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOV2R15PD, "dmov2r15pd", "dmov",
    { { MNEM, ' ', '@', OP (DIR10), ',', '@', '-', OP (R15), 0 } },
    { 16, 16, 0xff00 }, 0xb00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* dmov @$R15+,@$dir10 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_DMOVR15PI, "dmovr15pi", "dmov",
    { { MNEM, ' ', '@', OP (R15), '+', ',', '@', OP (DIR10), 0 } },
    { 16, 16, 0xff00 }, 0x1b00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* ldres @$Ri+,$u4 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LDRES, "ldres", "ldres",
    { { MNEM, ' ', '@', OP (RI), '+', ',', OP (U4), 0 } },
    { 16, 16, 0xff00 }, 0xbc00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* stres $u4,@$Ri+ */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STRES, "stres", "stres",
    { { MNEM, ' ', OP (U4), ',', '@', OP (RI), '+', 0 } },
    { 16, 16, 0xff00 }, 0xbd00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* nop */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_NOP, "nop", "nop",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9fa0,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* andccr $u8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ANDCCR, "andccr", "andccr",
    { { MNEM, ' ', OP (U8), 0 } },
    { 16, 16, 0xff00 }, 0x8300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* orccr $u8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ORCCR, "orccr", "orccr",
    { { MNEM, ' ', OP (U8), 0 } },
    { 16, 16, 0xff00 }, 0x9300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* stilm $u8 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_STILM, "stilm", "stilm",
    { { MNEM, ' ', OP (U8), 0 } },
    { 16, 16, 0xff00 }, 0x8700,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* addsp $s10 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ADDSP, "addsp", "addsp",
    { { MNEM, ' ', OP (S10), 0 } },
    { 16, 16, 0xff00 }, 0xa300,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* extsb $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EXTSB, "extsb", "extsb",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9780,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* extub $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EXTUB, "extub", "extub",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x9790,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* extsh $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EXTSH, "extsh", "extsh",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x97a0,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* extuh $Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_EXTUH, "extuh", "extuh",
    { { MNEM, ' ', OP (RI), 0 } },
    { 16, 16, 0xfff0 }, 0x97b0,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* enter $u10 */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_ENTER, "enter", "enter",
    { { MNEM, ' ', OP (U10), 0 } },
    { 16, 16, 0xff00 }, 0xf00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* leave */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_LEAVE, "leave", "leave",
    { { MNEM, 0 } },
    { 16, 16, 0xffff }, 0x9f90,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
/* xchb @$Rj,$Ri */
  {
    { 1, 1, 1, 1 },
    FR30_INSN_XCHB, "xchb", "xchb",
    { { MNEM, ' ', '@', OP (RJ), ',', OP (RI), 0 } },
    { 16, 16, 0xff00 }, 0x8a00,
    (PTR) 0,
    { 0, 0, { 0 } }
  },
};

#undef A
#undef MNEM
#undef OP

static const CGEN_INSN_TABLE insn_table =
{
  & fr30_cgen_insn_table_entries[0],
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
/* ldi32 $i32,$Ri */
  {
    { 1, 1, 1, 1 },
    -1, "ldi32m", "ldi32",
    { { MNEM, ' ', OP (I32), ',', OP (RI), 0 } },
    { 16, 48, 0xfff0 }, 0x9f80,
    (PTR) 0,
    { 0, 0|A(ALIAS), { 0 } }
  },
};

#undef A
#undef MNEM
#undef OP

static const CGEN_INSN_TABLE macro_insn_table =
{
  & macro_insn_table_entries[0],
  sizeof (CGEN_INSN),
  (sizeof (macro_insn_table_entries) /
   sizeof (macro_insn_table_entries[0])),
  NULL
};

static void
init_tables ()
{
}

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

/* BUF is a pointer to the insn's bytes in target order.
   VALUE is an integer of the first CGEN_BASE_INSN_BITSIZE bits,
   host order.  */

static unsigned int
dis_hash_insn (buf, value)
     const char * buf;
     CGEN_INSN_INT value;
{
  return CGEN_DIS_HASH (buf, value);
}

/* Initialize an opcode table and return a descriptor.
   It's much like opening a file, and must be the first function called.  */

CGEN_OPCODE_DESC
fr30_cgen_opcode_open (mach, endian)
     int mach;
     enum cgen_endian endian;
{
  CGEN_OPCODE_TABLE * table = (CGEN_OPCODE_TABLE *) xmalloc (sizeof (CGEN_OPCODE_TABLE));
  static int init_p;

  if (! init_p)
    {
      init_tables ();
      init_p = 1;
    }

  memset (table, 0, sizeof (*table));

  CGEN_OPCODE_MACH (table) = mach;
  CGEN_OPCODE_ENDIAN (table) = endian;
  /* FIXME: for the sparc case we can determine insn-endianness statically.
     The worry here is where both data and insn endian can be independently
     chosen, in which case this function will need another argument.
     Actually, will want to allow for more arguments in the future anyway.  */
  CGEN_OPCODE_INSN_ENDIAN (table) = endian;

  CGEN_OPCODE_HW_LIST (table) = & fr30_cgen_hw_entries[0];

  CGEN_OPCODE_OPERAND_TABLE (table) = & fr30_cgen_operand_table[0];

  * CGEN_OPCODE_INSN_TABLE (table) = insn_table;

  * CGEN_OPCODE_MACRO_INSN_TABLE (table) = macro_insn_table;

  CGEN_OPCODE_ASM_HASH_P (table) = asm_hash_insn_p;
  CGEN_OPCODE_ASM_HASH (table) = asm_hash_insn;
  CGEN_OPCODE_ASM_HASH_SIZE (table) = CGEN_ASM_HASH_SIZE;

  CGEN_OPCODE_DIS_HASH_P (table) = dis_hash_insn_p;
  CGEN_OPCODE_DIS_HASH (table) = dis_hash_insn;
  CGEN_OPCODE_DIS_HASH_SIZE (table) = CGEN_DIS_HASH_SIZE;

  return (CGEN_OPCODE_DESC) table;
}

/* Close an opcode table.  */

void
fr30_cgen_opcode_close (desc)
     CGEN_OPCODE_DESC desc;
{
  free (desc);
}

/* Getting values from cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they return.
   TODO: floating point, inlining support, remove cases where result type
   not appropriate.  */

int
fr30_cgen_get_int_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  int value;

  switch (opindex)
    {
    case FR30_OPERAND_RI :
      value = fields->f_Ri;
      break;
    case FR30_OPERAND_RJ :
      value = fields->f_Rj;
      break;
    case FR30_OPERAND_RS1 :
      value = fields->f_Rs1;
      break;
    case FR30_OPERAND_RS2 :
      value = fields->f_Rs2;
      break;
    case FR30_OPERAND_R13 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_R14 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_R15 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_PS :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_U4 :
      value = fields->f_u4;
      break;
    case FR30_OPERAND_M4 :
      value = fields->f_m4;
      break;
    case FR30_OPERAND_U8 :
      value = fields->f_u8;
      break;
    case FR30_OPERAND_I8 :
      value = fields->f_i8;
      break;
    case FR30_OPERAND_UDISP6 :
      value = fields->f_udisp6;
      break;
    case FR30_OPERAND_DISP8 :
      value = fields->f_disp8;
      break;
    case FR30_OPERAND_DISP9 :
      value = fields->f_disp9;
      break;
    case FR30_OPERAND_DISP10 :
      value = fields->f_disp10;
      break;
    case FR30_OPERAND_S10 :
      value = fields->f_s10;
      break;
    case FR30_OPERAND_U10 :
      value = fields->f_u10;
      break;
    case FR30_OPERAND_I32 :
      value = fields->f_i32;
      break;
    case FR30_OPERAND_DIR8 :
      value = fields->f_dir8;
      break;
    case FR30_OPERAND_DIR9 :
      value = fields->f_dir9;
      break;
    case FR30_OPERAND_DIR10 :
      value = fields->f_dir10;
      break;
    case FR30_OPERAND_LABEL9 :
      value = fields->f_rel9;
      break;
    case FR30_OPERAND_LABEL12 :
      value = fields->f_rel12;
      break;
    case FR30_OPERAND_CC :
      value = fields->f_cc;
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
fr30_cgen_get_vma_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  bfd_vma value;

  switch (opindex)
    {
    case FR30_OPERAND_RI :
      value = fields->f_Ri;
      break;
    case FR30_OPERAND_RJ :
      value = fields->f_Rj;
      break;
    case FR30_OPERAND_RS1 :
      value = fields->f_Rs1;
      break;
    case FR30_OPERAND_RS2 :
      value = fields->f_Rs2;
      break;
    case FR30_OPERAND_R13 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_R14 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_R15 :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_PS :
      value = fields->f_nil;
      break;
    case FR30_OPERAND_U4 :
      value = fields->f_u4;
      break;
    case FR30_OPERAND_M4 :
      value = fields->f_m4;
      break;
    case FR30_OPERAND_U8 :
      value = fields->f_u8;
      break;
    case FR30_OPERAND_I8 :
      value = fields->f_i8;
      break;
    case FR30_OPERAND_UDISP6 :
      value = fields->f_udisp6;
      break;
    case FR30_OPERAND_DISP8 :
      value = fields->f_disp8;
      break;
    case FR30_OPERAND_DISP9 :
      value = fields->f_disp9;
      break;
    case FR30_OPERAND_DISP10 :
      value = fields->f_disp10;
      break;
    case FR30_OPERAND_S10 :
      value = fields->f_s10;
      break;
    case FR30_OPERAND_U10 :
      value = fields->f_u10;
      break;
    case FR30_OPERAND_I32 :
      value = fields->f_i32;
      break;
    case FR30_OPERAND_DIR8 :
      value = fields->f_dir8;
      break;
    case FR30_OPERAND_DIR9 :
      value = fields->f_dir9;
      break;
    case FR30_OPERAND_DIR10 :
      value = fields->f_dir10;
      break;
    case FR30_OPERAND_LABEL9 :
      value = fields->f_rel9;
      break;
    case FR30_OPERAND_LABEL12 :
      value = fields->f_rel12;
      break;
    case FR30_OPERAND_CC :
      value = fields->f_cc;
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
fr30_cgen_set_int_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     int value;
{
  switch (opindex)
    {
    case FR30_OPERAND_RI :
      fields->f_Ri = value;
      break;
    case FR30_OPERAND_RJ :
      fields->f_Rj = value;
      break;
    case FR30_OPERAND_RS1 :
      fields->f_Rs1 = value;
      break;
    case FR30_OPERAND_RS2 :
      fields->f_Rs2 = value;
      break;
    case FR30_OPERAND_R13 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_R14 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_R15 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_PS :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_U4 :
      fields->f_u4 = value;
      break;
    case FR30_OPERAND_M4 :
      fields->f_m4 = value;
      break;
    case FR30_OPERAND_U8 :
      fields->f_u8 = value;
      break;
    case FR30_OPERAND_I8 :
      fields->f_i8 = value;
      break;
    case FR30_OPERAND_UDISP6 :
      fields->f_udisp6 = value;
      break;
    case FR30_OPERAND_DISP8 :
      fields->f_disp8 = value;
      break;
    case FR30_OPERAND_DISP9 :
      fields->f_disp9 = value;
      break;
    case FR30_OPERAND_DISP10 :
      fields->f_disp10 = value;
      break;
    case FR30_OPERAND_S10 :
      fields->f_s10 = value;
      break;
    case FR30_OPERAND_U10 :
      fields->f_u10 = value;
      break;
    case FR30_OPERAND_I32 :
      fields->f_i32 = value;
      break;
    case FR30_OPERAND_DIR8 :
      fields->f_dir8 = value;
      break;
    case FR30_OPERAND_DIR9 :
      fields->f_dir9 = value;
      break;
    case FR30_OPERAND_DIR10 :
      fields->f_dir10 = value;
      break;
    case FR30_OPERAND_LABEL9 :
      fields->f_rel9 = value;
      break;
    case FR30_OPERAND_LABEL12 :
      fields->f_rel12 = value;
      break;
    case FR30_OPERAND_CC :
      fields->f_cc = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting int operand.\n"),
		       opindex);
      abort ();
  }
}

void
fr30_cgen_set_vma_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     bfd_vma value;
{
  switch (opindex)
    {
    case FR30_OPERAND_RI :
      fields->f_Ri = value;
      break;
    case FR30_OPERAND_RJ :
      fields->f_Rj = value;
      break;
    case FR30_OPERAND_RS1 :
      fields->f_Rs1 = value;
      break;
    case FR30_OPERAND_RS2 :
      fields->f_Rs2 = value;
      break;
    case FR30_OPERAND_R13 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_R14 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_R15 :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_PS :
      fields->f_nil = value;
      break;
    case FR30_OPERAND_U4 :
      fields->f_u4 = value;
      break;
    case FR30_OPERAND_M4 :
      fields->f_m4 = value;
      break;
    case FR30_OPERAND_U8 :
      fields->f_u8 = value;
      break;
    case FR30_OPERAND_I8 :
      fields->f_i8 = value;
      break;
    case FR30_OPERAND_UDISP6 :
      fields->f_udisp6 = value;
      break;
    case FR30_OPERAND_DISP8 :
      fields->f_disp8 = value;
      break;
    case FR30_OPERAND_DISP9 :
      fields->f_disp9 = value;
      break;
    case FR30_OPERAND_DISP10 :
      fields->f_disp10 = value;
      break;
    case FR30_OPERAND_S10 :
      fields->f_s10 = value;
      break;
    case FR30_OPERAND_U10 :
      fields->f_u10 = value;
      break;
    case FR30_OPERAND_I32 :
      fields->f_i32 = value;
      break;
    case FR30_OPERAND_DIR8 :
      fields->f_dir8 = value;
      break;
    case FR30_OPERAND_DIR9 :
      fields->f_dir9 = value;
      break;
    case FR30_OPERAND_DIR10 :
      fields->f_dir10 = value;
      break;
    case FR30_OPERAND_LABEL9 :
      fields->f_rel9 = value;
      break;
    case FR30_OPERAND_LABEL12 :
      fields->f_rel12 = value;
      break;
    case FR30_OPERAND_CC :
      fields->f_cc = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

