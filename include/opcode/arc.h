/* Opcode table for the ARC.
   Copyright 1994, 1995 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* List of the various cpu types.
   The tables currently use bit masks to say whether the instruction or
   whatever is supported by a particular cpu.  This lets us have one entry
   apply to several cpus.
   There may be more ARCs in the future, beyond the current project.  */

/* The `base' cpu must be 0 (table entries are omitted for the base cpu).  */
#define ARC_MACH_BASE 0
#define ARC_MACH_HOST 1
#define ARC_MACH_GRAPHICS 2
#define ARC_MACH_AUDIO 4

/* Mask of number of bits necessary to record cpu type.  */
#define ARC_MACH_MASK 7

/* Type to denote an ARC instruction (at least a 32 bit unsigned long).  */
typedef unsigned long arc_insn;

struct arc_opcode {
  char *syntax;			/* syntax of insn */
  unsigned long mask, value;	/* recognize instruction if (op&mask)==value */
  int flags;			/* various flag bits */

/* Values for `flags'.  */

/* Return MACH number, given flag bits.  */
#define ARC_OPCODE_MACH(bits) ((bits) & ARC_MACH_MASK)
};

struct arc_operand_value {
  char *name;			/* eg: "eq" */
  short value;			/* eg: 1 */
  unsigned char type;		/* index into `arc_operands' */
  unsigned char flags;		/* various flag bits */

/* Values for `flags'.  */

/* Return MACH number, given flag bits.  */
#define ARC_OPVAL_MACH(bits) ((bits) & ARC_MACH_MASK)
};

struct arc_operand {
  /* One of the insn format chars.  */
  unsigned char fmt;

  /* The number of bits in the operand (may be unused for a modifier).  */
  unsigned char bits;

  /* How far the operand is left shifted in the instruction, or
     the modifier's flag bit (may be unused for a modifier.  */
  unsigned char shift;

  /* Various flag bits.  */
  int flags;

/* Values for `flags'.  */

/* This operand is a suffix to the opcode.  */
#define ARC_OPERAND_SUFFIX 1

/* This operand is a relative branch displacement.  The disassembler
   prints these symbolically if possible.  */
#define ARC_OPERAND_RELATIVE 2

/* This operand is an absolute branch address.  The disassembler
   prints these symbolically if possible.  */
#define ARC_OPERAND_ABSOLUTE 4

/* This operand takes signed values.  */
#define ARC_OPERAND_SIGNED 8

/* This operand takes signed values, but also accepts a full positive
   range of values.  That is, if bits is 16, it takes any value from
   -0x8000 to 0xffff.  */
#define ARC_OPERAND_SIGNOPT 0x10

/* This operand should be regarded as a negative number for the
   purposes of overflow checking (i.e., the normal most negative
   number is disallowed and one more than the normal most positive
   number is allowed).  This flag will only be set for a signed
   operand.  */
#define ARC_OPERAND_NEGATIVE 0x20

/* This operand doesn't really exist.  The program uses these operands
   in special ways.  */
#define ARC_OPERAND_FAKE 0x40

/* Modifier values.  */
/* A dot is required before a suffix.  Eg: .le  */
#define ARC_MOD_DOT 0x1000

/* A normal register is allowed (not used, but here for completeness).  */
#define ARC_MOD_REG 0x2000

/* An auxiliary register name is expected.  */
#define ARC_MOD_AUXREG 0x4000

/* Sum of all ARC_MOD_XXX bits.  */
#define ARC_MOD_BITS 0x7000

/* Non-zero if the operand type is really a modifier.  */
#define ARC_MOD_P(X) ((X) & ARC_MOD_BITS)

  /* Insertion function.  This is used by the assembler.  To insert an
     operand value into an instruction, check this field.

     If it is NULL, execute
         i |= (p & ((1 << o->bits) - 1)) << o->shift;
     (I is the instruction which we are filling in, O is a pointer to
     this structure, and OP is the opcode value; this assumes twos
     complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction and the operand value.  It will return the new value
     of the instruction.  If the ERRMSG argument is not NULL, then if
     the operand value is illegal, *ERRMSG will be set to a warning
     string (the operand will be inserted in any case).  If the
     operand value is legal, *ERRMSG will be unchanged.

     REG is non-NULL when inserting a register value.  */

  arc_insn (*insert) PARAMS ((arc_insn insn,
			      const struct arc_operand *operand, int mods,
			      const struct arc_operand_value *reg, long value,
			      const char **errmsg));

  /* Extraction function.  This is used by the disassembler.  To
     extract this operand type from an instruction, check this field.

     If it is NULL, compute
         op = ((i) >> o->shift) & ((1 << o->bits) - 1);
	 if ((o->flags & ARC_OPERAND_SIGNED) != 0
	     && (op & (1 << (o->bits - 1))) != 0)
	   op -= 1 << o->bits;
     (I is the instruction, O is a pointer to this structure, and OP
     is the result; this assumes twos complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction value.  It will return the value of the operand.  If
     the INVALID argument is not NULL, *INVALID will be set to
     non-zero if this operand type can not actually be extracted from
     this operand (i.e., the instruction does not match).  If the
     operand is valid, *INVALID will not be changed.

     INSN is a pointer to an array of two `arc_insn's.  The first element is
     the insn, the second is the limm if present.

     Operands that have a printable form like registers and suffixes have
     their struct arc_operand_value pointer stored in OPVAL.  */

  long (*extract) PARAMS ((arc_insn *insn,
			   const struct arc_operand *operand,
			   int mods, const struct arc_operand_value **opval,
			   int *invalid));
};

/* Bits that say what version of cpu we have.
   These should be passed to arc_init_opcode_tables.
   At present, all there is is the cpu type.  */

/* MACH number, given value passed to `arc_init_opcode_tables'.  */
#define ARC_HAVE_MACH(bits) ((bits) & ARC_MACH_MASK)

/* Special register values:  */
#define ARC_REG_SHIMM_UPDATE 61
#define ARC_REG_SHIMM 63
#define ARC_REG_LIMM 62

/* Non-zero if REG is a constant marker.  */
#define ARC_REG_CONSTANT_P(REG) ((REG) >= 61)

/* Positions and masks of various fields:  */
#define ARC_SHIFT_REGA 21
#define ARC_SHIFT_REGB 15
#define ARC_SHIFT_REGC 9
#define ARC_MASK_REG 63

/* Non-zero if X will fit in a signed 9 bit field.  */
#define ARC_SHIMM_CONST_P(x) ((long) (x) >= -256 && (long) (x) <= 255)

extern const struct arc_operand arc_operands[];
extern const int arc_operand_count;
extern const struct arc_opcode arc_opcodes[];
extern const int arc_opcodes_count;
extern const struct arc_operand_value arc_suffixes[];
extern const int arc_suffixes_count;
extern const struct arc_operand_value arc_reg_names[];
extern const int arc_reg_names_count;
extern unsigned char arc_operand_map[];

/* Utility fns in arc-opc.c.  */
/* `arc_opcode_init_tables' must be called before `arc_xxx_supported'.  */
void arc_opcode_init_tables PARAMS ((int));
void arc_opcode_init_insert PARAMS ((void));
void arc_opcode_init_extract PARAMS ((void));
int arc_opcode_limm_p PARAMS ((long *));
const struct arc_operand_value *arc_opcode_lookup_suffix PARAMS ((const struct arc_operand *type, int value));
int arc_opcode_supported PARAMS ((const struct arc_opcode *));
int arc_opval_supported PARAMS ((const struct arc_operand_value *));
