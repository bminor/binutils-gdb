/* Opcode table for the TXVU.
   Copyright 1998 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler, GDB, the GNU debugger, and
the GNU Binutils.

GAS/GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GAS/GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS or GDB; see the file COPYING.	If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* Type to denote a TXVU instruction (at least a 32 bit unsigned int).  */
typedef unsigned int TXVU_INSN;

/* Maximum number of operands and syntax chars an instruction can have.  */
#define TXVU_MAX_OPERANDS 16

typedef struct txvu_opcode {
  char *mnemonic;
  /* The value stored is 128 + operand number.
     This allows ASCII chars to go here as well.  */
  unsigned char syntax[TXVU_MAX_OPERANDS];
  TXVU_INSN mask, value;	/* recognize insn if (op&mask)==value */
  int flags;			/* various flag bits */

/* Values for `flags'.  */

/* This insn is a conditional branch.  */
#define TXVU_OPCODE_COND_BRANCH 1

  /* These values are used to optimize assembly and disassembly.  Each insn is
     on a list of related insns (same first letter for assembly, same insn code
     for disassembly).  */
  /* FIXME: May wish to move this to separate table.  */
  struct txvu_opcode *next_asm;	/* Next instruction to try during assembly.  */
  struct txvu_opcode *next_dis;	/* Next instruction to try during disassembly.  */

  /* Macros to create the hash values for the lists.  */
#define TXVU_HASH_UPPER_OPCODE(string) \
  (tolower ((string)[0]) >= 'a' && tolower ((string)[0]) <= 'z' \
   ? tolower ((string)[0]) - 'a' : 26)
#define TXVU_HASH_LOWER_OPCODE(string) \
  (tolower ((string)[0]) >= 'a' && tolower ((string)[0]) <= 'z' \
   ? tolower ((string)[0]) - 'a' : 26)
/* ??? The icode hashing is very simplistic.
   upper: bits 0x3c, can't use lower two bits because of bc field
   lower: upper 6 bits  */
#define TXVU_ICODE_HASH_SIZE 6 /* bits */
#define TXVU_HASH_UPPER_ICODE(insn) \
  ((insn) & 0x3c)
#define TXVU_HASH_LOWER_ICODE(insn) \
  ((((insn) & 0xfc) >> 26) & 0x3f)

  /* Macros to access `next_asm', `next_dis' so users needn't care about the
     underlying mechanism.  */
#define TXVU_OPCODE_NEXT_ASM(op) ((op)->next_asm)
#define TXVU_OPCODE_NEXT_DIS(op) ((op)->next_dis)
} txvu_opcode;

/* The operand table.  */

typedef struct txvu_operand {
  /* The number of bits in the operand (may be unused for a modifier).  */
  unsigned char bits;

  /* How far the operand is left shifted in the instruction, or
     the modifier's flag bit (may be unused for a modifier).  */
  unsigned char shift;

  /* Various flag bits.  */
  int flags;

/* Values for `flags'.  */

/* This operand is a suffix to the opcode.  */
#define TXVU_OPERAND_SUFFIX 1

/* This operand is a relative branch displacement.  The disassembler
   prints these symbolically if possible.  */
#define TXVU_OPERAND_RELATIVE_BRANCH 2

/* This operand is an absolute branch address.  The disassembler
   prints these symbolically if possible.  */
#define TXVU_OPERAND_ABSOLUTE_BRANCH 4

/* This operand is an address.  The disassembler
   prints these symbolically if possible.  */
#define TXVU_OPERAND_ADDRESS 8

/* This operand takes signed values (default is unsigned).
   The default was chosen to be unsigned as most fields are unsigned
   (e.g. registers).  */
#define TXVU_OPERAND_SIGNED 0x10

/* This operand takes signed values, but also accepts a full positive
   range of values.  That is, if bits is 16, it takes any value from
   -0x8000 to 0xffff.  */
#define TXVU_OPERAND_SIGNOPT 0x20

/* This operand should be regarded as a negative number for the
   purposes of overflow checking (i.e., the normal most negative
   number is disallowed and one more than the normal most positive
   number is allowed).  This flag will only be set for a signed
   operand.  */
#define TXVU_OPERAND_NEGATIVE 0x40

/* This operand doesn't really exist.  The program uses these operands
   in special ways by creating insertion or extraction functions to have
   arbitrary processing performed during assembly/disassemble.
   Parse and print routines are ignored for FAKE operands.  */
#define TXVU_OPERAND_FAKE 0x80

/* Modifier values.  */

/* A dot is required before a suffix.  e.g. .le  */
#define TXVU_MOD_DOT 0x1000

/* Sum of all TXVU_MOD_XXX bits.  */
#define TXVU_MOD_BITS 0x7000

/* Non-zero if the operand type is really a modifier.  */
#define TXVU_MOD_P(X) ((X) & TXVU_MOD_BITS)

  /* Parse function.  This is used by the assembler.
     MODS is a list of modifiers preceding the operand in the syntax string.
     If the operand cannot be parsed an error message is stored in ERRMSG,
     otherwise ERRMSG is unchanged.  */
  long (*parse) PARAMS ((const struct txvu_opcode *opcode,
			 const struct txvu_operand *operand,
			 int mods, char **str, const char **errmsg));

  /* Insertion function.  This is used by the assembler.  To insert an
     operand value into an instruction, check this field.

     If it is NULL, execute
         i |= (p & ((1 << o->bits) - 1)) << o->shift;
     (I is the instruction which we are filling in, O is a pointer to
     this structure, and OP is the opcode value; this assumes twos
     complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction and the operand value.  It will overwrite the appropriate
     bits of the instruction with the operand's value.
     MODS is a list of modifiers preceding the operand in the syntax string.
     If the ERRMSG argument is not NULL, then if the operand value is illegal,
     *ERRMSG will be set to a warning string (the operand will be inserted in
     any case).  If the operand value is legal, *ERRMSG will be unchanged.
     OPCODE may be NULL, in which case the value isn't known.  This happens
     when applying fixups.  */

  void (*insert) PARAMS ((const struct txvu_opcode *opcode,
			  const struct txvu_operand *operand,
			  int mods, TXVU_INSN *insn,
			  long value, const char **errmsg));

  /* Extraction function.  This is used by the disassembler.  To
     extract this operand type from an instruction, check this field.

     If it is NULL, compute
         op = ((i) >> o->shift) & ((1 << o->bits) - 1);
	 if ((o->flags & TXVU_OPERAND_SIGNED) != 0
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
     MODS is a list of modifiers preceding the operand in the syntax string.

     INSN is a pointer to one or two `TXVU_INSN's.  The first element is
     the insn, the second is an immediate constant if present.
     FIXME: just thrown in here for now.
     */

  long (*extract) PARAMS ((const struct txvu_opcode *opcode,
			   const struct txvu_operand *operand,
			   int mods, TXVU_INSN *insn, int *pinvalid));

  /* Print function.  This is used by the disassembler.  */
  void (*print) PARAMS ((const struct txvu_opcode *opcode,
			 const struct txvu_operand *operand,
			 int mods, TXVU_INSN *insn,
			 disassemble_info *info, long value));
} txvu_operand;

/* Given an operand entry, return the table index.  */
#define TXVU_OPERAND_INDEX(op) ((op) - 128)

/* VU support.  */

/* Positions, masks, and values of various fields used in multiple places
   (the opcode table, the disassembler, GAS).  */
#define TXVU_SHIFT_DEST 21
#define TXVU_SHIFT_TREG 16
#define TXVU_SHIFT_SREG 11
#define TXVU_SHIFT_DREG 6
#define TXVU_MASK_REG 31
/* Bits for multiple dest choices.  */
#define TXVU_DEST_X 8
#define TXVU_DEST_Y 4
#define TXVU_DEST_Z 2
#define TXVU_DEST_W 1
/* Values for a single dest choice.  */
#define TXVU_SDEST_X 0
#define TXVU_SDEST_Y 1
#define TXVU_SDEST_Z 2
#define TXVU_SDEST_W 3

extern const txvu_operand txvu_operands[];
extern /*const*/ txvu_opcode txvu_upper_opcodes[];
extern /*const*/ txvu_opcode txvu_lower_opcodes[];
extern const int txvu_upper_opcodes_count;
extern const int txvu_lower_opcodes_count;

const txvu_opcode *txvu_upper_opcode_lookup_asm PARAMS ((const char *));
const txvu_opcode *txvu_lower_opcode_lookup_asm PARAMS ((const char *));
const txvu_opcode *txvu_upper_opcode_lookup_dis PARAMS ((unsigned int));
const txvu_opcode *txvu_lower_opcode_lookup_dis PARAMS ((unsigned int));

/* PKE support.  */

/* PKE opcode flags.
   The usage here is a bit wasteful of bits, but there's enough bits
   and we can always make better usage later.  */

/* 2 word instruction */
#define PKE_OPCODE_LEN2 1
/* 5 word instruction */
#define PKE_OPCODE_LEN5 2
/* variable length instruction */
#define PKE_OPCODE_LENVAR 4
/* the mpg instruction */
#define PKE_OPCODE_MPG 8
/* the direct instruction */
#define PKE_OPCODE_DIRECT 16
/* the directhl instruction */
#define PKE_OPCODE_DIRECTHL 32
/* the unpack instruction */
#define PKE_OPCODE_UNPACK 64

/* Instruction flag bits.  M,R are only applicable to `unpack'.  */
#define PKE_FLAG_I 1
#define PKE_FLAG_M 2
#define PKE_FLAG_R 4

/* The "mode" operand of the "stmod" insn.  */
#define PKE_MODE_DIRECT 0
#define PKE_MODE_ADD 1
#define PKE_MODE_ADDROW 2

/* Unpack types.  */
#define PKE_UNPACK_S_32  0
#define PKE_UNPACK_S_16  1
#define PKE_UNPACK_S_8   2
#define PKE_UNPACK_V2_32 4
#define PKE_UNPACK_V2_16 5
#define PKE_UNPACK_V2_8  6
#define PKE_UNPACK_V3_32 8
#define PKE_UNPACK_V3_16 9
#define PKE_UNPACK_V3_8  10
#define PKE_UNPACK_V4_32 12
#define PKE_UNPACK_V4_16 13
#define PKE_UNPACK_V4_8  14
#define PKE_UNPACK_V4_5  15

extern const txvu_operand pke_operands[];
extern /*const*/ txvu_opcode pke_opcodes[];
extern const int pke_opcodes_count;
const txvu_opcode *pke_opcode_lookup_asm PARAMS ((const char *));
const txvu_opcode *pke_opcode_lookup_dis PARAMS ((unsigned int));

/* Return non-zero if the just parsed pke insn has variable length.  */
int pke_varlen_p PARAMS ((void));
/* Return length, in 32 bit words, of just parsed pke insn,
   or 0 if unknown.  */
int pke_len PARAMS ((void));

/* DMA support.  */

/* DMA instruction flags.  */
#define DMA_FLAG_PCE0 1
#define DMA_FLAG_PCE1 2
#define DMA_FLAG_INT 4
#define DMA_FLAG_SPR 8

extern const txvu_operand dma_operands[];
extern /*const*/ txvu_opcode dma_opcodes[];
extern const int dma_opcodes_count;
const txvu_opcode *dma_opcode_lookup_asm PARAMS ((const char *));
const txvu_opcode *dma_opcode_lookup_dis PARAMS ((unsigned int));

/* GPUIF support.  */

typedef enum {
  GPUIF_REG_PRIM = 0,
  GPUIF_REG_RGBAQ = 1,
  GPUIF_REG_ST = 2,
  GPUIF_REG_UV = 3,
  GPUIF_REG_XYZF2 = 4,
  GPUIF_REG_TEXCLUT_1 = 6,
  GPUIF_REG_TEXCLUT_2 = 7,
  GPUIF_REG_TEX0_1 = 8,
  GPUIF_REG_TEX0_2 = 9,
  GPUIF_REG_TEX1_1 = 10,
  GPUIF_REG_TEX1_2 = 11,
  GPUIF_REG_XYZF3 = 12,
  GPUIF_REG_PRMODE = 13,
  GPUIF_REG_A_D = 14,
  GPUIF_REG_NOP = 15
} gpuif_reg;

extern const txvu_operand gpuif_operands[];
extern /*const*/ txvu_opcode gpuif_opcodes[];
extern const int gpuif_opcodes_count;
const txvu_opcode *gpuif_opcode_lookup_asm PARAMS ((const char *));
const txvu_opcode *gpuif_opcode_lookup_dis PARAMS ((unsigned int));

/* Utility fns in txvu-opc.c.  */
void txvu_opcode_init_tables PARAMS ((int));
void txvu_opcode_init_parse PARAMS ((void));
void txvu_opcode_init_print PARAMS ((void));
