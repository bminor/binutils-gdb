/* tic80.h -- Header file for TI TMS320C80 (MV) opcode table
   Copyright 1996, 1997 Free Software Foundation, Inc.
   Written by Fred Fish (fnf@cygnus.com), Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef TIC80_H
#define TIC80_H

/* The opcode table is an array of struct tic80_opcode.  */

struct tic80_opcode
{
  /* The opcode name.  */

  const char *name;

  /* The opcode itself.  Those bits which will be filled in with operands
     are zeroes.  */

  unsigned long opcode;

  /* The opcode mask.  This is used by the disassembler.  This is a mask
     containing ones indicating those bits which must match the opcode
     field, and zeroes indicating those bits which need not match (and are
     presumably filled in by operands).  */

  unsigned long mask;

  /* The format of this opcode.  I.E. short-immediate, register, long
     immediate, etc.  FIXME: Will this ever be used?  */

  unsigned char format;

  /* An array of operand codes.  Each code is an index into the operand
     table.  They appear in the order which the operands must appear in
     assembly code, and are terminated by a zero.  FIXME: Adjust size to
     match actual requirements when TIc80 support is complete */

  unsigned char operands[8];
};

/* The table itself is sorted by major opcode number, and is otherwise in
   the order in which the disassembler should consider instructions.
   FIXME: This isn't currently true. */

extern const struct tic80_opcode tic80_opcodes[];
extern const int tic80_num_opcodes;


/* The operands table is an array of struct tic80_operand.  */

struct tic80_operand
{
  /* The number of bits in the operand.  */

  int bits;

  /* How far the operand is left shifted in the instruction.  */

  int shift;

  /* Insertion function.  This is used by the assembler.  To insert an
     operand value into an instruction, check this field.

     If it is NULL, execute
         i |= (op & ((1 << o->bits) - 1)) << o->shift;
     (i is the instruction which we are filling in, o is a pointer to
     this structure, and op is the opcode value; this assumes twos
     complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction and the operand value.  It will return the new value
     of the instruction.  If the ERRMSG argument is not NULL, then if
     the operand value is illegal, *ERRMSG will be set to a warning
     string (the operand will be inserted in any case).  If the
     operand value is legal, *ERRMSG will be unchanged (most operands
     can accept any value).  */

  unsigned long (*insert) PARAMS ((unsigned long instruction, long op,
				   const char **errmsg));

  /* Extraction function.  This is used by the disassembler.  To
     extract this operand type from an instruction, check this field.

     If it is NULL, compute
         op = ((i) >> o->shift) & ((1 << o->bits) - 1);
	 if ((o->flags & PPC_OPERAND_SIGNED) != 0
	     && (op & (1 << (o->bits - 1))) != 0)
	   op -= 1 << o->bits;
     (i is the instruction, o is a pointer to this structure, and op
     is the result; this assumes twos complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction value.  It will return the value of the operand.  If
     the INVALID argument is not NULL, *INVALID will be set to
     non-zero if this operand type can not actually be extracted from
     this operand (i.e., the instruction does not match).  If the
     operand is valid, *INVALID will not be changed.  */

  long (*extract) PARAMS ((unsigned long instruction, int *invalid));

  /* One bit syntax flags.  */

  unsigned long flags;
};

/* Elements in the table are retrieved by indexing with values from
   the operands field of the tic80_opcodes table.  */

extern const struct tic80_operand tic80_operands[];

/* Values defined for the flags field of a struct tic80_operand.  */

/* This operand takes signed values.  */
#define TIC80_OPERAND_SIGNED (01)

/* The next operand should be wrapped in parentheses rather than separated
   from this one by a comma.  This is used for various instructions, like
   the load and store instructions, which want their operands to look like
   "displacement(reg)" */
#define TIC80_OPERAND_PARENS (02)

/* This operand may use the symbolic names for the condition and size
   codes that the branch instructions use, such as "eq.b", "or.f",
   etc. */
#define TIC80_OPERAND_CC_SZ (04)

/* This operand names a register.  The disassembler uses this to print
   register names with a leading 'r'.  */
#define TIC80_OPERAND_GPR (010)

/* This operand names a floating point accumulator register.  The disassembler
   prints these with a leading 'a'.  */
#define TIC80_OPERAND_FPA (020)

/* This operand is a relative branch displacement.  The disassembler
   prints these symbolically if possible.  */
#define TIC80_OPERAND_RELATIVE (040)

/* This flag is a hint to the disassembler for using hex as the prefered
   printing format, even for small positive or negative immediate values.
   Normally values in the range -999 to 999 are printed as signed decimal
   values and other values are printed in hex. */
#define TIC80_OPERAND_BITFIELD (0100)

/* Values which go in the struct tic80_opcode format field to distinguish
   between various types of instructions with the same mnemonic.  FIXME: Not
   currently used? */

#define FMT_UNUSED	0		/* Unused */
#define FMT_SI		1		/* Short immediate format */
#define FMT_LI		2		/* Long immediate format */
#define FMT_REG		3		/* Register format */

#endif /* TIC80_H */
