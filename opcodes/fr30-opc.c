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
static unsigned int dis_hash_insn PARAMS ((const char *, unsigned long));

/* Cover function to read and properly byteswap an insn value.  */

CGEN_INSN_INT
cgen_get_insn_value (od, buf, length)
     CGEN_OPCODE_DESC od;
     unsigned char *buf;
     int length;
{
  CGEN_INSN_INT value;

  switch (length)
    {
    case 8:
      value = *buf;
      break;
    case 16:
      if (CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG)
	value = bfd_getb16 (buf);
      else
	value = bfd_getl16 (buf);
      break;
    case 32:
      if (CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG)
	value = bfd_getb32 (buf);
      else
	value = bfd_getl32 (buf);
      break;
    default:
      abort ();
    }

  return value;
}

/* Cover function to store an insn value properly byteswapped.  */

void
cgen_put_insn_value (od, buf, length, value)
     CGEN_OPCODE_DESC od;
     unsigned char *buf;
     int length;
     CGEN_INSN_INT value;
{
  switch (length)
    {
    case 8:
      buf[0] = value;
      break;
    case 16:
      if (CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG)
	bfd_putb16 (value, buf);
      else
	bfd_putl16 (value, buf);
      break;
    case 32:
      if (CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG)
	bfd_putb32 (value, buf);
      else
	bfd_putl32 (value, buf);
      break;
    default:
      abort ();
    }
}

/* Look up instruction INSN_VALUE and extract its fields.
   INSN, if non-null, is the insn table entry.
   Otherwise INSN_VALUE is examined to compute it.
   LENGTH is the bit length of INSN_VALUE if known, otherwise 0.
   0 is only valid if `insn == NULL && ! CGEN_INT_INSN_P'.
   If INSN != NULL, LENGTH must be valid.
   ALIAS_P is non-zero if alias insns are to be included in the search.

   The result a pointer to the insn table entry, or NULL if the instruction
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
  unsigned char buf[16];
  unsigned char *bufp;
  unsigned int base_insn;
#if CGEN_INT_INSN_P
  CGEN_EXTRACT_INFO *info = NULL;
#else
  CGEN_EXTRACT_INFO ex_info;
  CGEN_EXTRACT_INFO *info = &ex_info;
#endif

#if ! CGEN_INT_INSN_P
  ex_info.dis_info = NULL;
  ex_info.bytes = insn_value;
  ex_info.valid = -1;
#endif

  if (!insn)
    {
      const CGEN_INSN_LIST *insn_list;

#if CGEN_INT_INSN_P
      cgen_put_insn_value (od, buf, length, insn_value);
      bufp = buf;
      base_insn = insn_value; /*???*/
#else
      base_insn = cgen_get_insn_value (od, buf, length);
      bufp = insn_value;
#endif

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
	      if ((insn_value & CGEN_INSN_MASK (insn)) == CGEN_INSN_VALUE (insn))
		{
		  /* ??? 0 is passed for `pc' */
		  int elength = (*CGEN_EXTRACT_FN (insn)) (od, insn, info,
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
      length = (*CGEN_EXTRACT_FN (insn)) (od, insn, info, insn_value, fields,
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
  { "FUN-ACCESS", NULL },
  { "PC", NULL },
  { "PROFILE", NULL },
  { 0, 0 }
};

const CGEN_ATTR_TABLE fr30_cgen_operand_attr_table[] =
{
  { "ABS-ADDR", NULL },
  { "NEGATIVE", NULL },
  { "PCREL-ADDR", NULL },
  { "RELAX", NULL },
  { "SEM-ONLY", NULL },
  { "SIGN-OPT", NULL },
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
  { "usp", 3 }
};

CGEN_KEYWORD fr30_cgen_opval_h_dr = 
{
  & fr30_cgen_opval_h_dr_entries[0],
  4
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_mdr_entries[] = 
{
  { "mdh", 4 },
  { "mdl", 5 }
};

CGEN_KEYWORD fr30_cgen_opval_h_mdr = 
{
  & fr30_cgen_opval_h_mdr_entries[0],
  2
};

CGEN_KEYWORD_ENTRY fr30_cgen_opval_h_cr_entries[] = 
{
  { "pc", 0 },
  { "ps", 1 }
};

CGEN_KEYWORD fr30_cgen_opval_h_cr = 
{
  & fr30_cgen_opval_h_cr_entries[0],
  2
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
  { HW_H_MDR, & HW_ENT (HW_H_MDR + 1), "h-mdr", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_mdr, { 0, 0, { 0 } } },
  { HW_H_CR, & HW_ENT (HW_H_CR + 1), "h-cr", CGEN_ASM_KEYWORD, (PTR) & fr30_cgen_opval_h_cr, { 0, 0, { 0 } } },
  { HW_H_NBIT, & HW_ENT (HW_H_NBIT + 1), "h-nbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0|(1<<CGEN_HW_FUN_ACCESS), { 0 } } },
  { HW_H_ZBIT, & HW_ENT (HW_H_ZBIT + 1), "h-zbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0|(1<<CGEN_HW_FUN_ACCESS), { 0 } } },
  { HW_H_VBIT, & HW_ENT (HW_H_VBIT + 1), "h-vbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0|(1<<CGEN_HW_FUN_ACCESS), { 0 } } },
  { HW_H_CBIT, & HW_ENT (HW_H_CBIT + 1), "h-cbit", CGEN_ASM_KEYWORD, (PTR) 0, { 0, 0|(1<<CGEN_HW_FUN_ACCESS), { 0 } } },
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

static const CGEN_OPERAND_INSTANCE fmt_add_ops[] = {
  { INPUT, "Rj", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RJ), 0 },
  { INPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0 },
  { OUTPUT, "Ri", & HW_ENT (HW_H_GR), CGEN_MODE_SI, & OP_ENT (RI), 0 },
  { OUTPUT, "vbit", & HW_ENT (HW_H_VBIT), CGEN_MODE_BI, 0, 0 },
  { OUTPUT, "cbit", & HW_ENT (HW_H_CBIT), CGEN_MODE_BI, 0, 0 },
  { OUTPUT, "zbit", & HW_ENT (HW_H_ZBIT), CGEN_MODE_BI, 0, 0 },
  { OUTPUT, "nbit", & HW_ENT (HW_H_NBIT), CGEN_MODE_BI, 0, 0 },
  { 0 }
};

#undef INPUT
#undef OUTPUT

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
     unsigned long value;
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

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

