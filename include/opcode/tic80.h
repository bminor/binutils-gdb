/* tic80.h -- Header file for TI TMS320C80 (MV) opcode table
   Copyright 1996 Free Software Foundation, Inc.
   Written by Fred Fish (fnf@cygnus.com), Cygnus Support
   using other opcode/*.h files as templates.

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

/* The opcode table is an array of struct TIc80_opcode.  */

struct TIc80_opcode
{
  /* The opcode name.  */
  const char *name;

  /* The opcode itself.  Those bits which will be filled in with
     operands are zeroes.  */
  unsigned long opcode;

  /* The opcode mask.  This is used by the disassembler.  This is a
     mask containing ones indicating those bits which must match the
     opcode field, and zeroes indicating those bits which need not
     match (and are presumably filled in by operands).  */
  unsigned long mask;

  /* The format of this opcode.  */
  unsigned char format;

  /* An array of operand codes.  Each code is an index into the
     operand table.  They appear in the order which the operands must
     appear in assembly code, and are terminated by a zero.  */
  unsigned char operands[8];
};

/* The table itself is sorted by major opcode number, and is otherwise
   in the order in which the disassembler should consider
   instructions.  */
extern const struct tic80_opcode tic80_opcodes[];
extern const int tic80_num_opcodes;


/* The operands table is an array of struct tic80_operand.  */

struct tic80_operand
{
  /* The number of bits in the operand.  */
  int bits;

  /* How far the operand is left shifted in the instruction.  */
  int shift;

  /* One bit syntax flags.  */
  int flags;
};

/* Elements in the table are retrieved by indexing with values from
   the operands field of the tic80_opcodes table.  */

extern const struct tic80_operand tic80_operands[];

/* Values defined for the flags field of a struct tic80_operand.  */
#define TIC80_OPERAND_DREG 0x1

#define TIC80_OPERAND_AREG 0x2

#define TIC80_OPERAND_PSW 0x4

#define TIC80_OPERAND_MDR 0x8

#define TIC80_OPERAND_SIGNED 0x10

#define TIC80_OPERAND_PROMOTE 0x20

#define TIC80_OPERAND_PAREN 0x40

#define TIC80_OPERAND_REPEATED 0x80

#define TIC80_OPERAND_EXTENDED 0x100

#define TIC80_OPERAND_PCREL 0x400

#define TIC80_OPERAND_MEMADDR 0x800

#define FMT_1 1
#define FMT_2 2
#define FMT_3 3
#define FMT_4 4
#define FMT_5 5
#define FMT_6 6
#define FMT_7 7

#endif /* TIC80_H */
