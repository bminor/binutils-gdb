/* Generic opcode table support for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE i960c-opc.c.

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
#include "i960c-opc.h"
#include "opintl.h"

/* Used by the ifield rtx function.  */
#define FLD(f) (fields->f)

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
i960_cgen_lookup_insn (od, insn, insn_value, length, fields, alias_p)
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
	      if ((base_insn & CGEN_INSN_BASE_MASK (insn))
		  == CGEN_INSN_BASE_VALUE (insn))
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
i960_cgen_get_insn_operands (od, insn, fields, indices)
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
	indices[i] = i960_cgen_get_int_operand (CGEN_OPERAND_INDEX (op),
						  fields);
    }
}

/* Cover function to i960_cgen_get_insn_operands when either INSN or FIELDS
   isn't known.
   The INSN, INSN_VALUE, and LENGTH arguments are passed to
   i960_cgen_lookup_insn unchanged.

   The result is the insn table entry or NULL if the instruction wasn't
   recognized.  */

const CGEN_INSN *
i960_cgen_lookup_get_insn_operands (od, insn, insn_value, length, indices)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN *insn;
     CGEN_INSN_BYTES insn_value;
     int length;
     int *indices;
{
  CGEN_FIELDS fields;

  /* Pass non-zero for ALIAS_P only if INSN != NULL.
     If INSN == NULL, we want a real insn.  */
  insn = i960_cgen_lookup_insn (od, insn, insn_value, length, &fields,
				  insn != NULL);
  if (! insn)
    return NULL;

  i960_cgen_get_insn_operands (od, insn, &fields, indices);
  return insn;
}
/* Attributes.  */

static const CGEN_ATTR_ENTRY bool_attr[] =
{
  { "#f", 0 },
  { "#t", 1 },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY MACH_attr[] =
{
  { "base", MACH_BASE },
  { "i960_ka_sa", MACH_I960_KA_SA },
  { "i960_ca", MACH_I960_CA },
  { "max", MACH_MAX },
  { 0, 0 }
};

const CGEN_ATTR_TABLE i960_cgen_ifield_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNSIGNED", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "RESERVED", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE i960_cgen_hardware_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNSIGNED", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "CACHE-ADDR", &bool_attr[0], &bool_attr[0] },
  { "FUN-ACCESS", &bool_attr[0], &bool_attr[0] },
  { "PC", &bool_attr[0], &bool_attr[0] },
  { "PROFILE", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE i960_cgen_operand_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNSIGNED", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "NEGATIVE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "SEM-ONLY", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE i960_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNCOND-CTI", &bool_attr[0], &bool_attr[0] },
  { "COND-CTI", &bool_attr[0], &bool_attr[0] },
  { "SKIP-CTI", &bool_attr[0], &bool_attr[0] },
  { "DELAY-SLOT", &bool_attr[0], &bool_attr[0] },
  { "RELAXABLE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "ALIAS", &bool_attr[0], &bool_attr[0] },
  { "NO-DIS", &bool_attr[0], &bool_attr[0] },
  { "PBB", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

CGEN_KEYWORD_ENTRY i960_cgen_opval_h_gr_entries[] = 
{
  { "fp", 31 },
  { "sp", 1 },
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
  { "r15", 15 },
  { "g0", 16 },
  { "g1", 17 },
  { "g2", 18 },
  { "g3", 19 },
  { "g4", 20 },
  { "g5", 21 },
  { "g6", 22 },
  { "g7", 23 },
  { "g8", 24 },
  { "g9", 25 },
  { "g10", 26 },
  { "g11", 27 },
  { "g12", 28 },
  { "g13", 29 },
  { "g14", 30 },
  { "g15", 31 }
};

CGEN_KEYWORD i960_cgen_opval_h_gr = 
{
  & i960_cgen_opval_h_gr_entries[0],
  34
};

CGEN_KEYWORD_ENTRY i960_cgen_opval_h_cc_entries[] = 
{
  { "cc", 0 }
};

CGEN_KEYWORD i960_cgen_opval_h_cc = 
{
  & i960_cgen_opval_h_cc_entries[0],
  1
};


/* The hardware table.  */

#define HW_ENT(n) i960_cgen_hw_entries[n]
static const CGEN_HW_ENTRY i960_cgen_hw_entries[] =
{
  { HW_H_PC, & HW_ENT (HW_H_PC + 1), "h-pc", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0|(1<<CGEN_HW_PROFILE)|(1<<CGEN_HW_PC), { (1<<MACH_BASE) } } },
  { HW_H_MEMORY, & HW_ENT (HW_H_MEMORY + 1), "h-memory", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } } },
  { HW_H_SINT, & HW_ENT (HW_H_SINT + 1), "h-sint", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } } },
  { HW_H_UINT, & HW_ENT (HW_H_UINT + 1), "h-uint", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } } },
  { HW_H_ADDR, & HW_ENT (HW_H_ADDR + 1), "h-addr", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } } },
  { HW_H_IADDR, & HW_ENT (HW_H_IADDR + 1), "h-iaddr", CGEN_ASM_KEYWORD, (PTR) 0, { CGEN_HW_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } } },
  { HW_H_GR, & HW_ENT (HW_H_GR + 1), "h-gr", CGEN_ASM_KEYWORD, (PTR) & i960_cgen_opval_h_gr, { CGEN_HW_NBOOL_ATTRS, 0|(1<<CGEN_HW_CACHE_ADDR)|(1<<CGEN_HW_PROFILE), { (1<<MACH_BASE) } } },
  { HW_H_CC, & HW_ENT (HW_H_CC + 1), "h-cc", CGEN_ASM_KEYWORD, (PTR) & i960_cgen_opval_h_cc, { CGEN_HW_NBOOL_ATTRS, 0|(1<<CGEN_HW_CACHE_ADDR)|(1<<CGEN_HW_PROFILE), { (1<<MACH_BASE) } } },
  { 0 }
};

/* The instruction field table.  */

static const CGEN_IFLD i960_cgen_ifld_table[] =
{
  { I960_F_NIL, "f-nil", 0, 0, 0, 0, { CGEN_IFLD_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }  },
  { I960_F_OPCODE, "f-opcode", 0, 32, 0, 8, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_SRCDST, "f-srcdst", 0, 32, 8, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_SRC2, "f-src2", 0, 32, 13, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_M3, "f-m3", 0, 32, 18, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_M2, "f-m2", 0, 32, 19, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_M1, "f-m1", 0, 32, 20, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_OPCODE2, "f-opcode2", 0, 32, 21, 4, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_ZERO, "f-zero", 0, 32, 25, 2, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_SRC1, "f-src1", 0, 32, 27, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_ABASE, "f-abase", 0, 32, 13, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_MODEA, "f-modea", 0, 32, 18, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_ZEROA, "f-zeroa", 0, 32, 19, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_OFFSET, "f-offset", 0, 32, 20, 12, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_MODEB, "f-modeb", 0, 32, 18, 4, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_SCALE, "f-scale", 0, 32, 22, 3, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_ZEROB, "f-zerob", 0, 32, 25, 2, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_INDEX, "f-index", 0, 32, 27, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_OPTDISP, "f-optdisp", 32, 32, 0, 32, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_BR_SRC1, "f-br-src1", 0, 32, 8, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_BR_SRC2, "f-br-src2", 0, 32, 13, 5, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_BR_M1, "f-br-m1", 0, 32, 18, 1, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_BR_DISP, "f-br-disp", 0, 32, 19, 11, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_PCREL_ADDR), { (1<<MACH_BASE) } }  },
  { I960_F_BR_ZERO, "f-br-zero", 0, 32, 30, 2, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { I960_F_CTRL_DISP, "f-ctrl-disp", 0, 32, 8, 22, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_PCREL_ADDR), { (1<<MACH_BASE) } }  },
  { I960_F_CTRL_ZERO, "f-ctrl-zero", 0, 32, 30, 2, { CGEN_IFLD_NBOOL_ATTRS, 0|(1<<CGEN_IFLD_UNSIGNED), { (1<<MACH_BASE) } }  },
  { 0 }
};

/* The operand table.  */

#define OPERAND(op) CONCAT2 (I960_OPERAND_,op)
#define OP_ENT(op) i960_cgen_operand_table[OPERAND (op)]

const CGEN_OPERAND i960_cgen_operand_table[MAX_OPERANDS] =
{
/* pc: program counter */
  { "pc", & HW_ENT (HW_H_PC), 0, 0,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_SEM_ONLY), { (1<<MACH_BASE) } }  },
/* src1: source register 1 */
  { "src1", & HW_ENT (HW_H_GR), 27, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* src2: source register 2 */
  { "src2", & HW_ENT (HW_H_GR), 13, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* dst: source/dest register */
  { "dst", & HW_ENT (HW_H_GR), 8, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* lit1: literal 1 */
  { "lit1", & HW_ENT (HW_H_UINT), 27, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* lit2: literal 2 */
  { "lit2", & HW_ENT (HW_H_UINT), 13, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* st_src: store src */
  { "st_src", & HW_ENT (HW_H_GR), 8, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* abase: abase */
  { "abase", & HW_ENT (HW_H_GR), 13, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* offset: offset */
  { "offset", & HW_ENT (HW_H_UINT), 20, 12,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* scale: scale */
  { "scale", & HW_ENT (HW_H_UINT), 22, 3,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* index: index */
  { "index", & HW_ENT (HW_H_GR), 27, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* optdisp: optional displacement */
  { "optdisp", & HW_ENT (HW_H_UINT), 0, 32,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* br_src1: branch src1 */
  { "br_src1", & HW_ENT (HW_H_GR), 8, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* br_src2: branch src2 */
  { "br_src2", & HW_ENT (HW_H_GR), 13, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* br_disp: branch displacement */
  { "br_disp", & HW_ENT (HW_H_IADDR), 19, 11,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_PCREL_ADDR), { (1<<MACH_BASE) } }  },
/* br_lit1: branch literal 1 */
  { "br_lit1", & HW_ENT (HW_H_UINT), 8, 5,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_UNSIGNED), { (1<<MACH_BASE) } }  },
/* ctrl_disp: ctrl branch disp */
  { "ctrl_disp", & HW_ENT (HW_H_IADDR), 8, 22,
    { CGEN_OPERAND_NBOOL_ATTRS, 0|(1<<CGEN_OPERAND_PCREL_ADDR), { (1<<MACH_BASE) } }  },
};

/* Instruction formats.  */

#define F(f) & i960_cgen_ifld_table[CONCAT2 (I960_,f)]

static const CGEN_IFMT fmt_empty = {
  0, 0, 0x0, { 0 }
};

static const CGEN_IFMT fmt_mulo = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_mulo1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_mulo2 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_mulo3 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_remo = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_remo1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_remo2 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_remo3 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_not = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_not1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_not2 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_not3 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_emul = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_emul1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_emul2 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_emul3 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movl = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movl1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movt = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movt1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movq = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_movq1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_modpc = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_lda_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_lda_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_lda_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_lda_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_lda_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_lda_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_lda_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_lda_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ld_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ld_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ld_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldob_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldob_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldob_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldos_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldos_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldos_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldib_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldib_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldib_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldis_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldis_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldis_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldl_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldl_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldl_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldt_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldt_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldt_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldq_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ldq_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_ldq_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_st_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_st_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_st_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stob_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stob_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stob_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stos_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stos_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stos_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stl_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stl_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stl_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stt_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stt_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stt_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stq_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_stq_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_stq_indirect_index_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_cmpobe_reg = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_cmpobe_lit = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_cmpobl_reg = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_cmpobl_lit = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_bbc_lit = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_cmpi = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_cmpi1 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_cmpi2 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_cmpi3 = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_testno_reg = {
  32, 32, 0xff002003, { F (F_OPCODE), F (F_BR_SRC1), F (F_BR_SRC2), F (F_BR_M1), F (F_BR_DISP), F (F_BR_ZERO), 0 }
};

static const CGEN_IFMT fmt_bno = {
  32, 32, 0xff000003, { F (F_OPCODE), F (F_CTRL_DISP), F (F_CTRL_ZERO), 0 }
};

static const CGEN_IFMT fmt_b = {
  32, 32, 0xff000003, { F (F_OPCODE), F (F_CTRL_DISP), F (F_CTRL_ZERO), 0 }
};

static const CGEN_IFMT fmt_bx_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_bx_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_bx_indirect_index = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_bx_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_bx_indirect_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_callx_disp = {
  32, 64, 0xff003c60, { F (F_OPCODE), F (F_OPTDISP), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_callx_indirect = {
  32, 32, 0xff003c60, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEB), F (F_SCALE), F (F_ZEROB), F (F_INDEX), 0 }
};

static const CGEN_IFMT fmt_callx_indirect_offset = {
  32, 32, 0xff003000, { F (F_OPCODE), F (F_SRCDST), F (F_ABASE), F (F_MODEA), F (F_ZEROA), F (F_OFFSET), 0 }
};

static const CGEN_IFMT fmt_ret = {
  32, 32, 0xff000003, { F (F_OPCODE), F (F_CTRL_DISP), F (F_CTRL_ZERO), 0 }
};

static const CGEN_IFMT fmt_calls = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_fmark = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

static const CGEN_IFMT fmt_flushreg = {
  32, 32, 0xff003fe0, { F (F_OPCODE), F (F_SRCDST), F (F_SRC2), F (F_M3), F (F_M2), F (F_M1), F (F_OPCODE2), F (F_ZERO), F (F_SRC1), 0 }
};

#undef F

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The instruction table.
   This is currently non-static because the simulator accesses it
   directly.  */

const CGEN_INSN i960_cgen_insn_table_entries[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { { 0 }, 0 },
/* mulo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MULO, "mulo", "mulo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x70000080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* mulo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MULO1, "mulo1", "mulo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x70000880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* mulo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MULO2, "mulo2", "mulo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x70001080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* mulo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MULO3, "mulo3", "mulo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x70001880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMO, "remo", "remo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x70000400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMO1, "remo1", "remo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x70000c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMO2, "remo2", "remo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x70001400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMO3, "remo3", "remo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x70001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVO, "divo", "divo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x70000580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVO1, "divo1", "divo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x70000d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVO2, "divo2", "divo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x70001580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVO3, "divo3", "divo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x70001d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remi $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMI, "remi", "remi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x74000400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remi $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMI1, "remi1", "remi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x74000c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remi $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMI2, "remi2", "remi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x74001400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* remi $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_REMI3, "remi3", "remi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x74001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divi $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVI, "divi", "divi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x74000580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divi $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVI1, "divi1", "divi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x74000d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divi $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVI2, "divi2", "divi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x74001580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* divi $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_DIVI3, "divi3", "divi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x74001d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* addo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ADDO, "addo", "addo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x59000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* addo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ADDO1, "addo1", "addo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x59000800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* addo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ADDO2, "addo2", "addo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x59001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* addo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ADDO3, "addo3", "addo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x59001800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* subo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SUBO, "subo", "subo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x59000100 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* subo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SUBO1, "subo1", "subo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x59000900 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* subo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SUBO2, "subo2", "subo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x59001100 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* subo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SUBO3, "subo3", "subo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x59001900 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notbit $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTBIT, "notbit", "notbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notbit $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTBIT1, "notbit1", "notbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notbit $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTBIT2, "notbit2", "notbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notbit $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTBIT3, "notbit3", "notbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* and $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_AND, "and", "and",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* and $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_AND1, "and1", "and",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* and $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_AND2, "and2", "and",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* and $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_AND3, "and3", "and",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* andnot $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ANDNOT, "andnot", "andnot",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x58000100 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* andnot $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ANDNOT1, "andnot1", "andnot",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x58000900 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* andnot $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ANDNOT2, "andnot2", "andnot",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x58001100 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* andnot $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ANDNOT3, "andnot3", "andnot",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x58001900 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* setbit $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SETBIT, "setbit", "setbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000180 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* setbit $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SETBIT1, "setbit1", "setbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000980 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* setbit $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SETBIT2, "setbit2", "setbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001180 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* setbit $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SETBIT3, "setbit3", "setbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001980 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notand $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTAND, "notand", "notand",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x58000200 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notand $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTAND1, "notand1", "notand",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x58000a00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notand $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTAND2, "notand2", "notand",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x58001200 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* notand $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOTAND3, "notand3", "notand",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x58001a00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* xor $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_XOR, "xor", "xor",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000300 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* xor $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_XOR1, "xor1", "xor",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000b00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* xor $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_XOR2, "xor2", "xor",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001300 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* xor $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_XOR3, "xor3", "xor",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001b00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* or $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_OR, "or", "or",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000380 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* or $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_OR1, "or1", "or",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000b80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* or $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_OR2, "or2", "or",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001380 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* or $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_OR3, "or3", "or",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001b80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* nor $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOR, "nor", "nor",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x58000400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* nor $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOR1, "nor1", "nor",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x58000c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* nor $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOR2, "nor2", "nor",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x58001400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* nor $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOR3, "nor3", "nor",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x58001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* not $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOT, "not", "not",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_not, { 0x58000500 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* not $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOT1, "not1", "not",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_not1, { 0x58000d00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* not $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOT2, "not2", "not",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_not2, { 0x58001500 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* not $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_NOT3, "not3", "not",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_not3, { 0x58001d00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* clrbit $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CLRBIT, "clrbit", "clrbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo, { 0x58000600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* clrbit $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CLRBIT1, "clrbit1", "clrbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo1, { 0x58000e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* clrbit $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CLRBIT2, "clrbit2", "clrbit",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo2, { 0x58001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* clrbit $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CLRBIT3, "clrbit3", "clrbit",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_mulo3, { 0x58001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shlo $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLO, "shlo", "shlo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x59000600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shlo $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLO1, "shlo1", "shlo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x59000e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shlo $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLO2, "shlo2", "shlo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x59001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shlo $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLO3, "shlo3", "shlo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x59001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shro $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRO, "shro", "shro",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x59000400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shro $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRO1, "shro1", "shro",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x59000c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shro $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRO2, "shro2", "shro",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x59001400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shro $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRO3, "shro3", "shro",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x59001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shli $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLI, "shli", "shli",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x59000700 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shli $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLI1, "shli1", "shli",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x59000f00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shli $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLI2, "shli2", "shli",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x59001700 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shli $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHLI3, "shli3", "shli",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x59001f00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shri $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRI, "shri", "shri",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo, { 0x59000580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shri $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRI1, "shri1", "shri",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_remo1, { 0x59000d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shri $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRI2, "shri2", "shri",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo2, { 0x59001580 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* shri $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_SHRI3, "shri3", "shri",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_remo3, { 0x59001d80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* emul $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_EMUL, "emul", "emul",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_emul, { 0x67000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* emul $lit1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_EMUL1, "emul1", "emul",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_emul1, { 0x67000800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* emul $src1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_EMUL2, "emul2", "emul",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_emul2, { 0x67001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* emul $lit1, $lit2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_EMUL3, "emul3", "emul",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), ',', ' ', OP (DST), 0 } },
    & fmt_emul3, { 0x67001800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* mov $src1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOV, "mov", "mov",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (DST), 0 } },
    & fmt_not2, { 0x5c001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* mov $lit1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOV1, "mov1", "mov",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (DST), 0 } },
    & fmt_not3, { 0x5c001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movl $src1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVL, "movl", "movl",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (DST), 0 } },
    & fmt_movl, { 0x5d001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movl $lit1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVL1, "movl1", "movl",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (DST), 0 } },
    & fmt_movl1, { 0x5d001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movt $src1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVT, "movt", "movt",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (DST), 0 } },
    & fmt_movt, { 0x5e001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movt $lit1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVT1, "movt1", "movt",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (DST), 0 } },
    & fmt_movt1, { 0x5e001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movq $src1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVQ, "movq", "movq",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (DST), 0 } },
    & fmt_movq, { 0x5f001600 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* movq $lit1, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MOVQ1, "movq1", "movq",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (DST), 0 } },
    & fmt_movq1, { 0x5f001e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* modpc $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MODPC, "modpc", "modpc",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_modpc, { 0x65000280 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* modac $src1, $src2, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_MODAC, "modac", "modac",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), ',', ' ', OP (DST), 0 } },
    & fmt_modpc, { 0x64000280 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_OFFSET, "lda-offset", "lda",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_lda_offset, { 0x8c000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDIRECT_OFFSET, "lda-indirect-offset", "lda",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_lda_indirect_offset, { 0x8c002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDIRECT, "lda-indirect", "lda",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_lda_indirect, { 0x8c001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDIRECT_INDEX, "lda-indirect-index", "lda",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_lda_indirect_index, { 0x8c001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_DISP, "lda-disp", "lda",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_lda_disp, { 0x8c003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDIRECT_DISP, "lda-indirect-disp", "lda",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_lda_indirect_disp, { 0x8c003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDEX_DISP, "lda-index-disp", "lda",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_lda_index_disp, { 0x8c003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* lda $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDA_INDIRECT_INDEX_DISP, "lda-indirect-index-disp", "lda",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_lda_indirect_index_disp, { 0x8c003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_OFFSET, "ld-offset", "ld",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ld_offset, { 0x90000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDIRECT_OFFSET, "ld-indirect-offset", "ld",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ld_indirect_offset, { 0x90002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDIRECT, "ld-indirect", "ld",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ld_indirect, { 0x90001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDIRECT_INDEX, "ld-indirect-index", "ld",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ld_indirect_index, { 0x90001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_DISP, "ld-disp", "ld",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ld_disp, { 0x90003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDIRECT_DISP, "ld-indirect-disp", "ld",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ld_indirect_disp, { 0x90003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDEX_DISP, "ld-index-disp", "ld",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ld_index_disp, { 0x90003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ld $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LD_INDIRECT_INDEX_DISP, "ld-indirect-index-disp", "ld",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ld_indirect_index_disp, { 0x90003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_OFFSET, "ldob-offset", "ldob",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldob_offset, { 0x80000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDIRECT_OFFSET, "ldob-indirect-offset", "ldob",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_indirect_offset, { 0x80002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDIRECT, "ldob-indirect", "ldob",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_indirect, { 0x80001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDIRECT_INDEX, "ldob-indirect-index", "ldob",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_indirect_index, { 0x80001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_DISP, "ldob-disp", "ldob",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldob_disp, { 0x80003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDIRECT_DISP, "ldob-indirect-disp", "ldob",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_indirect_disp, { 0x80003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDEX_DISP, "ldob-index-disp", "ldob",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_index_disp, { 0x80003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldob $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOB_INDIRECT_INDEX_DISP, "ldob-indirect-index-disp", "ldob",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldob_indirect_index_disp, { 0x80003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_OFFSET, "ldos-offset", "ldos",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldos_offset, { 0x88000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDIRECT_OFFSET, "ldos-indirect-offset", "ldos",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_indirect_offset, { 0x88002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDIRECT, "ldos-indirect", "ldos",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_indirect, { 0x88001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDIRECT_INDEX, "ldos-indirect-index", "ldos",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_indirect_index, { 0x88001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_DISP, "ldos-disp", "ldos",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldos_disp, { 0x88003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDIRECT_DISP, "ldos-indirect-disp", "ldos",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_indirect_disp, { 0x88003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDEX_DISP, "ldos-index-disp", "ldos",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_index_disp, { 0x88003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldos $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDOS_INDIRECT_INDEX_DISP, "ldos-indirect-index-disp", "ldos",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldos_indirect_index_disp, { 0x88003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_OFFSET, "ldib-offset", "ldib",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldib_offset, { 0xc0000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDIRECT_OFFSET, "ldib-indirect-offset", "ldib",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_indirect_offset, { 0xc0002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDIRECT, "ldib-indirect", "ldib",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_indirect, { 0xc0001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDIRECT_INDEX, "ldib-indirect-index", "ldib",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_indirect_index, { 0xc0001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_DISP, "ldib-disp", "ldib",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldib_disp, { 0xc0003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDIRECT_DISP, "ldib-indirect-disp", "ldib",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_indirect_disp, { 0xc0003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDEX_DISP, "ldib-index-disp", "ldib",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_index_disp, { 0xc0003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldib $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIB_INDIRECT_INDEX_DISP, "ldib-indirect-index-disp", "ldib",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldib_indirect_index_disp, { 0xc0003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_OFFSET, "ldis-offset", "ldis",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldis_offset, { 0xc8000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDIRECT_OFFSET, "ldis-indirect-offset", "ldis",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_indirect_offset, { 0xc8002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDIRECT, "ldis-indirect", "ldis",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_indirect, { 0xc8001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDIRECT_INDEX, "ldis-indirect-index", "ldis",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_indirect_index, { 0xc8001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_DISP, "ldis-disp", "ldis",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldis_disp, { 0xc8003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDIRECT_DISP, "ldis-indirect-disp", "ldis",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_indirect_disp, { 0xc8003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDEX_DISP, "ldis-index-disp", "ldis",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_index_disp, { 0xc8003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldis $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDIS_INDIRECT_INDEX_DISP, "ldis-indirect-index-disp", "ldis",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldis_indirect_index_disp, { 0xc8003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_OFFSET, "ldl-offset", "ldl",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldl_offset, { 0x98000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDIRECT_OFFSET, "ldl-indirect-offset", "ldl",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_indirect_offset, { 0x98002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDIRECT, "ldl-indirect", "ldl",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_indirect, { 0x98001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDIRECT_INDEX, "ldl-indirect-index", "ldl",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_indirect_index, { 0x98001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_DISP, "ldl-disp", "ldl",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldl_disp, { 0x98003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDIRECT_DISP, "ldl-indirect-disp", "ldl",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_indirect_disp, { 0x98003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDEX_DISP, "ldl-index-disp", "ldl",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_index_disp, { 0x98003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldl $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDL_INDIRECT_INDEX_DISP, "ldl-indirect-index-disp", "ldl",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldl_indirect_index_disp, { 0x98003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_OFFSET, "ldt-offset", "ldt",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldt_offset, { 0xa0000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDIRECT_OFFSET, "ldt-indirect-offset", "ldt",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_indirect_offset, { 0xa0002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDIRECT, "ldt-indirect", "ldt",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_indirect, { 0xa0001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDIRECT_INDEX, "ldt-indirect-index", "ldt",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_indirect_index, { 0xa0001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_DISP, "ldt-disp", "ldt",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldt_disp, { 0xa0003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDIRECT_DISP, "ldt-indirect-disp", "ldt",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_indirect_disp, { 0xa0003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDEX_DISP, "ldt-index-disp", "ldt",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_index_disp, { 0xa0003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldt $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDT_INDIRECT_INDEX_DISP, "ldt-indirect-index-disp", "ldt",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldt_indirect_index_disp, { 0xa0003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $offset, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_OFFSET, "ldq-offset", "ldq",
    { { MNEM, ' ', OP (OFFSET), ',', ' ', OP (DST), 0 } },
    & fmt_ldq_offset, { 0xb0000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $offset($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDIRECT_OFFSET, "ldq-indirect-offset", "ldq",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_indirect_offset, { 0xb0002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq ($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDIRECT, "ldq-indirect", "ldq",
    { { MNEM, ' ', '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_indirect, { 0xb0001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq ($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDIRECT_INDEX, "ldq-indirect-index", "ldq",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_indirect_index, { 0xb0001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $optdisp, $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_DISP, "ldq-disp", "ldq",
    { { MNEM, ' ', OP (OPTDISP), ',', ' ', OP (DST), 0 } },
    & fmt_ldq_disp, { 0xb0003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $optdisp($abase), $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDIRECT_DISP, "ldq-indirect-disp", "ldq",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_indirect_disp, { 0xb0003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $optdisp[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDEX_DISP, "ldq-index-disp", "ldq",
    { { MNEM, ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_index_disp, { 0xb0003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* ldq $optdisp($abase)[$index*S$scale], $dst */
  {
    { 1, 1, 1, 1 },
    I960_INSN_LDQ_INDIRECT_INDEX_DISP, "ldq-indirect-index-disp", "ldq",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', ',', ' ', OP (DST), 0 } },
    & fmt_ldq_indirect_index_disp, { 0xb0003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_OFFSET, "st-offset", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_st_offset, { 0x92000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDIRECT_OFFSET, "st-indirect-offset", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_st_indirect_offset, { 0x92002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDIRECT, "st-indirect", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_st_indirect, { 0x92001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDIRECT_INDEX, "st-indirect-index", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_st_indirect_index, { 0x92001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_DISP, "st-disp", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_st_disp, { 0x92003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDIRECT_DISP, "st-indirect-disp", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_st_indirect_disp, { 0x92003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDEX_DISP, "st-index-disp", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_st_index_disp, { 0x92003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* st $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_ST_INDIRECT_INDEX_DISP, "st-indirect-index-disp", "st",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_st_indirect_index_disp, { 0x92003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_OFFSET, "stob-offset", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_stob_offset, { 0x82000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDIRECT_OFFSET, "stob-indirect-offset", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_stob_indirect_offset, { 0x82002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDIRECT, "stob-indirect", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_stob_indirect, { 0x82001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDIRECT_INDEX, "stob-indirect-index", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stob_indirect_index, { 0x82001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_DISP, "stob-disp", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_stob_disp, { 0x82003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDIRECT_DISP, "stob-indirect-disp", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_stob_indirect_disp, { 0x82003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDEX_DISP, "stob-index-disp", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_stob_index_disp, { 0x82003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stob $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOB_INDIRECT_INDEX_DISP, "stob-indirect-index-disp", "stob",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stob_indirect_index_disp, { 0x82003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_OFFSET, "stos-offset", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_stos_offset, { 0x8a000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDIRECT_OFFSET, "stos-indirect-offset", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_stos_indirect_offset, { 0x8a002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDIRECT, "stos-indirect", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_stos_indirect, { 0x8a001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDIRECT_INDEX, "stos-indirect-index", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stos_indirect_index, { 0x8a001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_DISP, "stos-disp", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_stos_disp, { 0x8a003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDIRECT_DISP, "stos-indirect-disp", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_stos_indirect_disp, { 0x8a003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDEX_DISP, "stos-index-disp", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_stos_index_disp, { 0x8a003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stos $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STOS_INDIRECT_INDEX_DISP, "stos-indirect-index-disp", "stos",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stos_indirect_index_disp, { 0x8a003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_OFFSET, "stl-offset", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_stl_offset, { 0x9a000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDIRECT_OFFSET, "stl-indirect-offset", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_stl_indirect_offset, { 0x9a002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDIRECT, "stl-indirect", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_stl_indirect, { 0x9a001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDIRECT_INDEX, "stl-indirect-index", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stl_indirect_index, { 0x9a001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_DISP, "stl-disp", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_stl_disp, { 0x9a003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDIRECT_DISP, "stl-indirect-disp", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_stl_indirect_disp, { 0x9a003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDEX_DISP, "stl-index-disp", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_stl_index_disp, { 0x9a003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stl $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STL_INDIRECT_INDEX_DISP, "stl-indirect-index-disp", "stl",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stl_indirect_index_disp, { 0x9a003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_OFFSET, "stt-offset", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_stt_offset, { 0xa2000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDIRECT_OFFSET, "stt-indirect-offset", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_stt_indirect_offset, { 0xa2002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDIRECT, "stt-indirect", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_stt_indirect, { 0xa2001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDIRECT_INDEX, "stt-indirect-index", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stt_indirect_index, { 0xa2001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_DISP, "stt-disp", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_stt_disp, { 0xa2003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDIRECT_DISP, "stt-indirect-disp", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_stt_indirect_disp, { 0xa2003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDEX_DISP, "stt-index-disp", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_stt_index_disp, { 0xa2003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stt $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STT_INDIRECT_INDEX_DISP, "stt-indirect-index-disp", "stt",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stt_indirect_index_disp, { 0xa2003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $offset */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_OFFSET, "stq-offset", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), 0 } },
    & fmt_stq_offset, { 0xb2000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDIRECT_OFFSET, "stq-indirect-offset", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_stq_indirect_offset, { 0xb2002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDIRECT, "stq-indirect", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_stq_indirect, { 0xb2001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDIRECT_INDEX, "stq-indirect-index", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stq_indirect_index, { 0xb2001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_DISP, "stq-disp", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), 0 } },
    & fmt_stq_disp, { 0xb2003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDIRECT_DISP, "stq-indirect-disp", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_stq_indirect_disp, { 0xb2003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $optdisp[$index*S$scale */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDEX_DISP, "stq-index-disp", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '[', OP (INDEX), '*', 'S', OP (SCALE), 0 } },
    & fmt_stq_index_disp, { 0xb2003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* stq $st_src, $optdisp($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_STQ_INDIRECT_INDEX_DISP, "stq-indirect-index-disp", "stq",
    { { MNEM, ' ', OP (ST_SRC), ',', ' ', OP (OPTDISP), '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_stq_indirect_index_disp, { 0xb2003c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpobe $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBE_REG, "cmpobe-reg", "cmpobe",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x32000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobe $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBE_LIT, "cmpobe-lit", "cmpobe",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x32002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobne $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBNE_REG, "cmpobne-reg", "cmpobne",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x35000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobne $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBNE_LIT, "cmpobne-lit", "cmpobne",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x35002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobl $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBL_REG, "cmpobl-reg", "cmpobl",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_reg, { 0x34000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobl $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBL_LIT, "cmpobl-lit", "cmpobl",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_lit, { 0x34002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpoble $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBLE_REG, "cmpoble-reg", "cmpoble",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_reg, { 0x36000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpoble $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBLE_LIT, "cmpoble-lit", "cmpoble",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_lit, { 0x36002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobg $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBG_REG, "cmpobg-reg", "cmpobg",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_reg, { 0x31000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobg $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBG_LIT, "cmpobg-lit", "cmpobg",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_lit, { 0x31002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobge $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBGE_REG, "cmpobge-reg", "cmpobge",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_reg, { 0x33000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpobge $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPOBGE_LIT, "cmpobge-lit", "cmpobge",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobl_lit, { 0x33002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibe $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBE_REG, "cmpibe-reg", "cmpibe",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x3a000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibe $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBE_LIT, "cmpibe-lit", "cmpibe",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x3a002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibne $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBNE_REG, "cmpibne-reg", "cmpibne",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x3d000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibne $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBNE_LIT, "cmpibne-lit", "cmpibne",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x3d002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibl $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBL_REG, "cmpibl-reg", "cmpibl",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x3c000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibl $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBL_LIT, "cmpibl-lit", "cmpibl",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x3c002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpible $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBLE_REG, "cmpible-reg", "cmpible",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x3e000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpible $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBLE_LIT, "cmpible-lit", "cmpible",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x3e002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibg $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBG_REG, "cmpibg-reg", "cmpibg",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x39000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibg $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBG_LIT, "cmpibg-lit", "cmpibg",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x39002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibge $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBGE_REG, "cmpibge-reg", "cmpibge",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x3b000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpibge $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPIBGE_LIT, "cmpibge-lit", "cmpibge",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_lit, { 0x3b002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bbc $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BBC_REG, "bbc-reg", "bbc",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x30000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bbc $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BBC_LIT, "bbc-lit", "bbc",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_bbc_lit, { 0x30002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bbs $br_src1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BBS_REG, "bbs-reg", "bbs",
    { { MNEM, ' ', OP (BR_SRC1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_cmpobe_reg, { 0x37000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bbs $br_lit1, $br_src2, $br_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BBS_LIT, "bbs-lit", "bbs",
    { { MNEM, ' ', OP (BR_LIT1), ',', ' ', OP (BR_SRC2), ',', ' ', OP (BR_DISP), 0 } },
    & fmt_bbc_lit, { 0x37002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* cmpi $src1, $src2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPI, "cmpi", "cmpi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), 0 } },
    & fmt_cmpi, { 0x5a002080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpi $lit1, $src2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPI1, "cmpi1", "cmpi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), 0 } },
    & fmt_cmpi1, { 0x5a002880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpi $src1, $lit2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPI2, "cmpi2", "cmpi",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), 0 } },
    & fmt_cmpi2, { 0x5a003080 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpi $lit1, $lit2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPI3, "cmpi3", "cmpi",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), 0 } },
    & fmt_cmpi3, { 0x5a003880 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpo $src1, $src2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPO, "cmpo", "cmpo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (SRC2), 0 } },
    & fmt_cmpi, { 0x5a002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpo $lit1, $src2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPO1, "cmpo1", "cmpo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (SRC2), 0 } },
    & fmt_cmpi1, { 0x5a002800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpo $src1, $lit2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPO2, "cmpo2", "cmpo",
    { { MNEM, ' ', OP (SRC1), ',', ' ', OP (LIT2), 0 } },
    & fmt_cmpi2, { 0x5a003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* cmpo $lit1, $lit2 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CMPO3, "cmpo3", "cmpo",
    { { MNEM, ' ', OP (LIT1), ',', ' ', OP (LIT2), 0 } },
    & fmt_cmpi3, { 0x5a003800 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testno $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTNO_REG, "testno-reg", "testno",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x20000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testg $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTG_REG, "testg-reg", "testg",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x21000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* teste $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTE_REG, "teste-reg", "teste",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x22000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testge $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTGE_REG, "testge-reg", "testge",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x23000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testl $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTL_REG, "testl-reg", "testl",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x24000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testne $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTNE_REG, "testne-reg", "testne",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x25000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testle $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTLE_REG, "testle-reg", "testle",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x26000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* testo $br_src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_TESTO_REG, "testo-reg", "testo",
    { { MNEM, ' ', OP (BR_SRC1), 0 } },
    & fmt_testno_reg, { 0x27000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
/* bno $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BNO, "bno", "bno",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x10000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bg $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BG, "bg", "bg",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x11000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* be $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BE, "be", "be",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x12000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bge $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BGE, "bge", "bge",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x13000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bl $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BL, "bl", "bl",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x14000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bne $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BNE, "bne", "bne",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x15000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* ble $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BLE, "ble", "ble",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x16000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bo $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BO, "bo", "bo",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_bno, { 0x17000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* b $ctrl_disp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_B, "b", "b",
    { { MNEM, ' ', OP (CTRL_DISP), 0 } },
    & fmt_b, { 0x8000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bx $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BX_INDIRECT_OFFSET, "bx-indirect-offset", "bx",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_bx_indirect_offset, { 0x84002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bx ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BX_INDIRECT, "bx-indirect", "bx",
    { { MNEM, ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_bx_indirect, { 0x84001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bx ($abase)[$index*S$scale] */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BX_INDIRECT_INDEX, "bx-indirect-index", "bx",
    { { MNEM, ' ', '(', OP (ABASE), ')', '[', OP (INDEX), '*', 'S', OP (SCALE), ']', 0 } },
    & fmt_bx_indirect_index, { 0x84001c00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bx $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BX_DISP, "bx-disp", "bx",
    { { MNEM, ' ', OP (OPTDISP), 0 } },
    & fmt_bx_disp, { 0x84003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bx $optdisp($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_BX_INDIRECT_DISP, "bx-indirect-disp", "bx",
    { { MNEM, ' ', OP (OPTDISP), '(', OP (ABASE), ')', 0 } },
    & fmt_bx_indirect_disp, { 0x84003400 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* callx $optdisp */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CALLX_DISP, "callx-disp", "callx",
    { { MNEM, ' ', OP (OPTDISP), 0 } },
    & fmt_callx_disp, { 0x86003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* callx ($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CALLX_INDIRECT, "callx-indirect", "callx",
    { { MNEM, ' ', '(', OP (ABASE), ')', 0 } },
    & fmt_callx_indirect, { 0x86001000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* callx $offset($abase) */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CALLX_INDIRECT_OFFSET, "callx-indirect-offset", "callx",
    { { MNEM, ' ', OP (OFFSET), '(', OP (ABASE), ')', 0 } },
    & fmt_callx_indirect_offset, { 0x86002000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* ret */
  {
    { 1, 1, 1, 1 },
    I960_INSN_RET, "ret", "ret",
    { { MNEM, 0 } },
    & fmt_ret, { 0xa000000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* calls $src1 */
  {
    { 1, 1, 1, 1 },
    I960_INSN_CALLS, "calls", "calls",
    { { MNEM, ' ', OP (SRC1), 0 } },
    & fmt_calls, { 0x66003000 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* fmark */
  {
    { 1, 1, 1, 1 },
    I960_INSN_FMARK, "fmark", "fmark",
    { { MNEM, 0 } },
    & fmt_fmark, { 0x66003e00 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* flushreg */
  {
    { 1, 1, 1, 1 },
    I960_INSN_FLUSHREG, "flushreg", "flushreg",
    { { MNEM, 0 } },
    & fmt_flushreg, { 0x66003e80 },
    (PTR) 0,
    { CGEN_INSN_NBOOL_ATTRS, 0, { (1<<MACH_BASE) } }
  },
};

#undef A
#undef MNEM
#undef OP

static const CGEN_INSN_TABLE insn_table =
{
  & i960_cgen_insn_table_entries[0],
  sizeof (CGEN_INSN),
  MAX_INSNS,
  NULL
};

/* Formats for ALIAS macro-insns.  */

#define F(f) & i960_cgen_ifld_table[CONCAT2 (I960_,f)]

#undef F

/* Each non-simple macro entry points to an array of expansion possibilities.  */

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define MNEM CGEN_SYNTAX_MNEMONIC /* syntax value for mnemonic */
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The macro instruction table.  */

static const CGEN_INSN macro_insn_table_entries[] =
{
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
i960_cgen_opcode_open (mach, endian)
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

  CGEN_OPCODE_HW_LIST (table) = & i960_cgen_hw_entries[0];

  CGEN_OPCODE_IFLD_TABLE (table) = & i960_cgen_ifld_table[0];

  CGEN_OPCODE_OPERAND_TABLE (table) = & i960_cgen_operand_table[0];

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
i960_cgen_opcode_close (desc)
     CGEN_OPCODE_DESC desc;
{
  free (desc);
}

/* Getting values from cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they return.
   TODO: floating point, inlining support, remove cases where result type
   not appropriate.  */

int
i960_cgen_get_int_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  int value;

  switch (opindex)
    {
    case I960_OPERAND_SRC1 :
      value = fields->f_src1;
      break;
    case I960_OPERAND_SRC2 :
      value = fields->f_src2;
      break;
    case I960_OPERAND_DST :
      value = fields->f_srcdst;
      break;
    case I960_OPERAND_LIT1 :
      value = fields->f_src1;
      break;
    case I960_OPERAND_LIT2 :
      value = fields->f_src2;
      break;
    case I960_OPERAND_ST_SRC :
      value = fields->f_srcdst;
      break;
    case I960_OPERAND_ABASE :
      value = fields->f_abase;
      break;
    case I960_OPERAND_OFFSET :
      value = fields->f_offset;
      break;
    case I960_OPERAND_SCALE :
      value = fields->f_scale;
      break;
    case I960_OPERAND_INDEX :
      value = fields->f_index;
      break;
    case I960_OPERAND_OPTDISP :
      value = fields->f_optdisp;
      break;
    case I960_OPERAND_BR_SRC1 :
      value = fields->f_br_src1;
      break;
    case I960_OPERAND_BR_SRC2 :
      value = fields->f_br_src2;
      break;
    case I960_OPERAND_BR_DISP :
      value = fields->f_br_disp;
      break;
    case I960_OPERAND_BR_LIT1 :
      value = fields->f_br_src1;
      break;
    case I960_OPERAND_CTRL_DISP :
      value = fields->f_ctrl_disp;
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
i960_cgen_get_vma_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  bfd_vma value;

  switch (opindex)
    {
    case I960_OPERAND_SRC1 :
      value = fields->f_src1;
      break;
    case I960_OPERAND_SRC2 :
      value = fields->f_src2;
      break;
    case I960_OPERAND_DST :
      value = fields->f_srcdst;
      break;
    case I960_OPERAND_LIT1 :
      value = fields->f_src1;
      break;
    case I960_OPERAND_LIT2 :
      value = fields->f_src2;
      break;
    case I960_OPERAND_ST_SRC :
      value = fields->f_srcdst;
      break;
    case I960_OPERAND_ABASE :
      value = fields->f_abase;
      break;
    case I960_OPERAND_OFFSET :
      value = fields->f_offset;
      break;
    case I960_OPERAND_SCALE :
      value = fields->f_scale;
      break;
    case I960_OPERAND_INDEX :
      value = fields->f_index;
      break;
    case I960_OPERAND_OPTDISP :
      value = fields->f_optdisp;
      break;
    case I960_OPERAND_BR_SRC1 :
      value = fields->f_br_src1;
      break;
    case I960_OPERAND_BR_SRC2 :
      value = fields->f_br_src2;
      break;
    case I960_OPERAND_BR_DISP :
      value = fields->f_br_disp;
      break;
    case I960_OPERAND_BR_LIT1 :
      value = fields->f_br_src1;
      break;
    case I960_OPERAND_CTRL_DISP :
      value = fields->f_ctrl_disp;
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
i960_cgen_set_int_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     int value;
{
  switch (opindex)
    {
    case I960_OPERAND_SRC1 :
      fields->f_src1 = value;
      break;
    case I960_OPERAND_SRC2 :
      fields->f_src2 = value;
      break;
    case I960_OPERAND_DST :
      fields->f_srcdst = value;
      break;
    case I960_OPERAND_LIT1 :
      fields->f_src1 = value;
      break;
    case I960_OPERAND_LIT2 :
      fields->f_src2 = value;
      break;
    case I960_OPERAND_ST_SRC :
      fields->f_srcdst = value;
      break;
    case I960_OPERAND_ABASE :
      fields->f_abase = value;
      break;
    case I960_OPERAND_OFFSET :
      fields->f_offset = value;
      break;
    case I960_OPERAND_SCALE :
      fields->f_scale = value;
      break;
    case I960_OPERAND_INDEX :
      fields->f_index = value;
      break;
    case I960_OPERAND_OPTDISP :
      fields->f_optdisp = value;
      break;
    case I960_OPERAND_BR_SRC1 :
      fields->f_br_src1 = value;
      break;
    case I960_OPERAND_BR_SRC2 :
      fields->f_br_src2 = value;
      break;
    case I960_OPERAND_BR_DISP :
      fields->f_br_disp = value;
      break;
    case I960_OPERAND_BR_LIT1 :
      fields->f_br_src1 = value;
      break;
    case I960_OPERAND_CTRL_DISP :
      fields->f_ctrl_disp = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting int operand.\n"),
		       opindex);
      abort ();
  }
}

void
i960_cgen_set_vma_operand (opindex, fields, value)
     int opindex;
     CGEN_FIELDS * fields;
     bfd_vma value;
{
  switch (opindex)
    {
    case I960_OPERAND_SRC1 :
      fields->f_src1 = value;
      break;
    case I960_OPERAND_SRC2 :
      fields->f_src2 = value;
      break;
    case I960_OPERAND_DST :
      fields->f_srcdst = value;
      break;
    case I960_OPERAND_LIT1 :
      fields->f_src1 = value;
      break;
    case I960_OPERAND_LIT2 :
      fields->f_src2 = value;
      break;
    case I960_OPERAND_ST_SRC :
      fields->f_srcdst = value;
      break;
    case I960_OPERAND_ABASE :
      fields->f_abase = value;
      break;
    case I960_OPERAND_OFFSET :
      fields->f_offset = value;
      break;
    case I960_OPERAND_SCALE :
      fields->f_scale = value;
      break;
    case I960_OPERAND_INDEX :
      fields->f_index = value;
      break;
    case I960_OPERAND_OPTDISP :
      fields->f_optdisp = value;
      break;
    case I960_OPERAND_BR_SRC1 :
      fields->f_br_src1 = value;
      break;
    case I960_OPERAND_BR_SRC2 :
      fields->f_br_src2 = value;
      break;
    case I960_OPERAND_BR_DISP :
      fields->f_br_disp = value;
      break;
    case I960_OPERAND_BR_LIT1 :
      fields->f_br_src1 = value;
      break;
    case I960_OPERAND_CTRL_DISP :
      fields->f_ctrl_disp = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

