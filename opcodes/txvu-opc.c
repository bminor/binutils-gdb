/* Opcode table for the TXVU
   Copyright 1998 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "ansidecl.h"
#include "sysdep.h"
#include "dis-asm.h"
#include "opcode/txvu.h"

#ifndef NULL
#define NULL 0
#endif

#if defined (__STDC__) || defined (ALMOST_STDC)
#define XCONCAT2(a,b)	a##b
#else
#define XCONCAT2(a,b)	a/**/b
#endif
#define CONCAT2(a,b)	XCONCAT2(a,b)

typedef struct {
  int value;
  const char *name;
} keyword;

static int lookup_keyword_value PARAMS ((const keyword *, const char *, int));
static const char *lookup_keyword_name PARAMS ((const keyword *table, int));

static char *scan_symbol PARAMS ((char *));

/* Return non-zero if CH is a character that may appear in a symbol.  */
/* FIXME: This will need revisiting.  */
#define issymchar(ch) (isalnum (ch) || ch == '_')

#define SKIP_BLANKS(var) while (isspace (*(var))) ++(var)

/* ??? One can argue it's preferable to have the PARSE_FN support in tc-txvu.c
   and the PRINT_FN support in txvu-dis.c.  For this project I like having
   them all in one place.  */

#define PARSE_FN(fn) \
static long CONCAT2 (parse_,fn) \
     PARAMS ((const txvu_opcode *, const txvu_operand *, int, char **, \
	      const char **));
#define INSERT_FN(fn) \
static void CONCAT2 (insert_,fn) \
     PARAMS ((const txvu_opcode *, const txvu_operand *, int, TXVU_INSN *, \
	      long, const char **))
#define EXTRACT_FN(fn) \
static long CONCAT2 (extract_,fn) \
     PARAMS ((const txvu_opcode *, const txvu_operand *, int, TXVU_INSN *, \
	      int *));
#define PRINT_FN(fn) \
static void CONCAT2 (print_,fn) \
     PARAMS ((const txvu_opcode *, const txvu_operand *, int, TXVU_INSN *, \
	      disassemble_info *, long));

PARSE_FN (dotdest);
INSERT_FN (dotdest);
EXTRACT_FN (dotdest);
PRINT_FN (dotdest);

PARSE_FN (dotdest1);
PARSE_FN (dest1);
PRINT_FN (dest1);

PARSE_FN (bc);
EXTRACT_FN (bc);
PRINT_FN (sdest);

PARSE_FN (vfreg);
PRINT_FN (vfreg);

PARSE_FN (bcftreg);
PRINT_FN (bcftreg);

PARSE_FN (accdest);
PRINT_FN (accdest);

INSERT_FN (xyz);

PARSE_FN (ireg);
PRINT_FN (ireg);

PARSE_FN (freg);
PRINT_FN (freg);

PARSE_FN (ffstreg);
INSERT_FN (ffstreg);
EXTRACT_FN (ffstreg);
PRINT_FN (ffstreg);

PARSE_FN (vi01);
PRINT_FN (vi01);

INSERT_FN (luimm12);
EXTRACT_FN (luimm12);

INSERT_FN (luimm12up6);

INSERT_FN (luimm15);
EXTRACT_FN (luimm15);

/* Various types of TXVU operands, including insn suffixes.

   Fields are:

   BITS SHIFT FLAGS PARSE_FN INSERT_FN EXTRACT_FN PRINT_FN

   Operand values are 128 + table index.  This allows ASCII chars to be
   included in the syntax spec.  */

const txvu_operand txvu_operands[] =
{
  /* place holder (??? not sure if needed) */
#define UNUSED 128
  { 0 },

  /* Operands that exist in the same place for essentially the same purpose
     in both upper and lower instructions.  These don't have a U or L prefix.
     Operands specific to the upper or lower instruction are so prefixed.  */

  /* Destination indicator attached to mnemonic, with leading '.' or '/'.
     After parsing this, the value is stored in global `dest' so that the
     register parser can verify the same choice of xyzw is used.  */
#define DOTDEST (UNUSED + 1)
  { 4, TXVU_SHIFT_DEST, TXVU_OPERAND_SUFFIX,
      parse_dotdest, insert_dotdest, extract_dotdest, print_dotdest },

  /* ft reg, with vector specification same as DOTDEST */
#define VFTREG (DOTDEST + 1)
  { 5, TXVU_SHIFT_TREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* fs reg, with vector specification same as DOTDEST */
#define VFSREG (VFTREG + 1)
  { 5, TXVU_SHIFT_SREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* fd reg, with vector specification same as DOTDEST */
#define VFDREG (VFSREG + 1)
  { 5, TXVU_SHIFT_DREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* Upper word operands.  */

  /* broadcast */
#define UBC (VFDREG + 1)
  { 2, 0, TXVU_OPERAND_SUFFIX, parse_bc, 0, extract_bc, print_sdest },

  /* ftreg in broadcast case */
#define UBCFTREG (UBC + 1)
  { 5, TXVU_SHIFT_TREG, 0, parse_bcftreg, 0, 0, print_bcftreg },

  /* accumulator dest */
#define UACCDEST (UBCFTREG + 1)
  { 0, 0, 0, parse_accdest, 0, 0, print_accdest },

  /* The XYZ operand is a fake one that is used to ensure only "xyz" is
     specified.  It simplifies the opmula and opmsub entries.  */
#define UXYZ (UACCDEST + 1)
  { 0, 0, TXVU_OPERAND_FAKE, 0, insert_xyz, 0, 0 },

  /* Lower word operands.  */

  /* 5 bit signed immediate.  */
#define LIMM5 (UXYZ + 1)
  { 5, 6, TXVU_OPERAND_SIGNED, 0, 0, 0, 0 },

  /* 11 bit signed immediate.  */
#define LIMM11 (LIMM5 + 1)
  { 11, 0, TXVU_OPERAND_SIGNED, 0, 0, 0, 0 },

  /* 15 bit unsigned immediate.  */
#define LUIMM15 (LIMM11 + 1)
  { 15, 0, 0, 0, insert_luimm15, extract_luimm15, 0 },

  /* ID register.  */
#define LIDREG (LUIMM15 + 1)
  { 5, 6, 0, parse_ireg, 0, 0, print_ireg },

  /* IS register.  */
#define LISREG (LIDREG + 1)
  { 5, 11, 0, parse_ireg, 0, 0, print_ireg },

  /* IT register.  */
#define LITREG (LISREG + 1)
  { 5, 16, 0, parse_ireg, 0, 0, print_ireg },

  /* FS reg, with FSF field selector.  */
#define LFSFFSREG (LITREG + 1)
  { 5, 11, 0, parse_ffstreg, insert_ffstreg, extract_ffstreg, print_ffstreg },

  /* FS reg, no selector (choice of x,y,z,w is provided by opcode).  */
#define LFSREG (LFSFFSREG + 1)
  { 5, 11, 0, parse_freg, 0, 0, print_freg },

  /* FT reg, with FTF field selector.  */
#define LFTFFTREG (LFSREG + 1)
  { 5, 16, 0, parse_ffstreg, insert_ffstreg, extract_ffstreg, print_ffstreg },

  /* VI01 register.  */
#define LVI01 (LFTFFTREG + 1)
  { 0, 0, 0, parse_vi01, 0, 0, print_vi01 },

  /* 24 bit unsigned immediate.  */
#define LUIMM24 (LVI01 + 1)
  { 24, 0, 0, 0, 0, 0, 0 },

  /* 12 bit unsigned immediate, split into 1 and 11 bit pieces.  */
#define LUIMM12 (LUIMM24 + 1)
  { 12, 0, 0, 0, insert_luimm12, extract_luimm12, 0 },

  /* upper 6 bits of 12 bit unsigned immediate */
#define LUIMM12UP6 (LUIMM12 + 1)
  { 12, 0, 0, 0, insert_luimm12up6, extract_luimm12, 0 },

  /* 11 bit pc-relative signed immediate.  */
#define LPCREL11 (LUIMM12UP6 + 1)
  { 11, 0, TXVU_OPERAND_SIGNED + TXVU_OPERAND_RELATIVE_BRANCH, 0, 0, 0, 0 },

  /* Destination indicator, single letter only, with leading '.' or '/'.  */
#define LDOTDEST1 (LPCREL11 + 1)
  { 4, TXVU_SHIFT_DEST, TXVU_OPERAND_SUFFIX,
      /* Note that we borrow the insert/extract/print functions from the
	 vector case.  */
      parse_dotdest1, insert_dotdest, extract_dotdest, print_dotdest },

  /* Destination indicator, single letter only, no leading '.'.  */
#define LDEST1 (LDOTDEST1 + 1)
  { 0, 0, 0, parse_dest1, 0, 0, print_dest1 },

/* end of list place holder */
  { 0 }
};

/* Macros to put a field's value into the right place.  */
/* ??? If assembler needs these, move to opcode/txvu.h.  */

/* value X, B bits, shift S */
#define V(x,b,s) (((x) & ((1 << (b)) - 1)) << (s))

/* Field value macros for both upper and lower instructions.
   These shift a value into the right place in the instruction.  */

/* [FI] T reg field (remember it's V for value, not vector, here).  */
#define VT(x) V ((x), 5, TXVU_SHIFT_TREG)
/* [FI] S reg field.  */
#define VS(x) V ((x), 5, TXVU_SHIFT_SREG)
/* [FI] D reg field.  */
#define VD(x) V ((x), 5, TXVU_SHIFT_DREG)
/* DEST field.  */
#define VDEST(x) V ((x), 4, 21)

/* Masks for fields in both upper and lower instructions.
   These mask out all bits but the ones for the field in the instruction.  */

#define MT VT (~0)
#define MS VS (~0)
#define MD VD (~0)
#define MDEST VDEST (~0)

/* Upper instruction Value macros.  */

/* Upper Flag bits.  */
#define VUF(x) V ((x), 5, 27)
/* Upper REServed two bits next to flag bits.  */
#define VURES(x) V ((x), 2, 25)
/* 4 bit opcode field.  */
#define VUOP4(x) V ((x), 4, 2)
/* 6 bit opcode field.  */
#define VUOP6(x) V ((x), 6, 0)
/* 9 bit opcode field.  */
#define VUOP9(x) V ((x), 9, 2)
/* 11 bit opcode field.  */
#define VUOP11(x) V ((x), 11, 0)
/* BroadCast field.  */
#define VUBC(x) V ((x), 2, 0)

/* Upper instruction field masks.  */
#define MUUBITS (VUF (~0) + VURES (~0))
#define MURES VURES (~0)
#define MUOP4 VUOP4 (~0)
#define MUOP6 VUOP6 (~0)
#define MUOP9 VUOP9 (~0)
#define MUOP11 VUOP11 (~0)

/* A space, separates instruction name (mnemonic + mnemonic operands) from
   operands.  */
#define SP ' '
/* Commas separate operands.  */
#define C ','
/* Special I,P,Q,R operands.  */
#define I 'i'
#define P 'p'
#define Q 'q'
#define R 'r'

/* TXVU instructions.
   [??? some of these comments are left over from the ARC port from which
   this code is borrowed, delete in time]

   Longer versions of insns must appear before shorter ones (if gas sees
   "lsr r2,r3,1" when it's parsing "lsr %a,%b" it will think the ",1" is
   junk).  This isn't necessary for `ld' because of the trailing ']'.

   Instructions that are really macros based on other insns must appear
   before the real insn so they're chosen when disassembling.  Eg: The `mov'
   insn is really the `and' insn.

   This table is best viewed on a wide screen (161 columns).  I'd prefer to
   keep it this way.  The rest of the file, however, should be viewable on an
   80 column terminal.  */

/* ??? This table also includes macros: asl, lsl, and mov.  The ppc port has
   a more general facility for dealing with macros which could be used if
   we need to.  */

/* These tables can't be `const' because members `next_asm' and `next_dis' are
   computed at run-time.  We could split this into two, as that would put the
   constant stuff into a readonly section.  */

struct txvu_opcode txvu_upper_opcodes[] =
{
  /* Macros appear first, so the disassembler will try them first.  */
  /* ??? Any aliases?  */
  /* ??? When close to being finished, clean up by aligning fields.  */

  /* The rest of these needn't be sorted, but it helps to find them if they are.  */
  { "abs", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x1fd) },
  { "add", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x28) },
  { "addi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x22) },
  { "addq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x20) },
  { "add", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (0) },
  { "adda", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2bc) },
  { "addai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x23e) },
  { "addaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x23c) },
  { "adda", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0xf) },
  { "clip", { DOTDEST, SP, VFSREG }, MURES + MDEST + MT + MUOP11, VDEST (0xf) + VUOP11 (0x1ff) },
  { "ftoi0", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17c) },
  { "ftoi4", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17d) },
  { "ftoi12", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17e) },
  { "ftoi15", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17f) },
  { "itof0", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13c) },
  { "itof4", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13d) },
  { "itof12", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13e) },
  { "itof15", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13f) },
  { "madd", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x29) },
  { "maddi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x23) },
  { "maddq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x21) },
  { "madd", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x2) },
  { "madda", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2bd) },
  { "maddai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x23f) },
  { "maddaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x23d) },
  { "madda", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x2f) },
  { "max", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2b) },
  { "maxi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x2d) },
  { "max", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x4) },
  /* ??? mini or min? */
  { "mini", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2f) },
  { "minii", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x1f) },
  { "mini", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x5) },
  { "msub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2d) },
  { "msubi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x27) },
  { "msubq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x25) },
  { "msub", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x3) },
  { "msuba", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2fd) },
  { "msubai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x27f) },
  { "msubaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x27d) },
  { "msuba", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x3f) },
  { "mul", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2a) },
  { "muli", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x1e) },
  { "mulq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x1c) },
  { "mul", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (6) },
  { "mula", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2be) },
  { "mulai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x1fe) },
  { "mulaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x1fc) },
  { "mula", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x6f) },
  { "nop", { 0 }, MURES + MDEST + MT + MS + MUOP11, VUOP11 (0x2ff) },
  { "opmula", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG, UXYZ }, MURES + MUOP11, VUOP11 (0x2fe) },
  { "opmsub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG, UXYZ }, MURES + MUOP6, VUOP6 (0x2e) },
  { "sub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2c) },
  { "subi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x26) },
  { "subq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x24) },
  { "sub", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (1) },
  { "suba", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2fc) },
  { "subai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x27e) },
  { "subaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x27c) },
  { "suba", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x1f) }
};
const int txvu_upper_opcodes_count = sizeof (txvu_upper_opcodes) / sizeof (txvu_upper_opcodes[0]);

/* Lower instruction Value macros.  */

/* 6 bit opcode.  */
#define VLOP6(x) V ((x), 6, 0)
/* 7 bit opcode.  */
#define VLOP7(x) V ((x), 7, 25)
/* 11 bit opcode.  */
#define VLOP11(x) V ((x), 11, 0)
/* 11 bit immediate.  */
#define VLIMM11(x) V ((x), 11, 0)
/* FTF field.  */
#define VLFTF(x) V ((x), 2, 23)
/* FSF field.  */
#define VLFSF(x) V ((x), 2, 21)
/* Upper bit of 12 bit unsigned immediate.  */
#define VLUIMM12TOP(x) V ((x), 1, 21)
/* Upper 4 bits of 15 bit unsigned immediate.  */
#define VLUIMM15TOP(x) VDEST (x)

/* Lower instruction field masks.  */
#define MLOP6 VLOP6 (~0)
#define MLOP7 VLOP7 (~0)
#define MLOP11 VLOP11 (~0)
#define MLIMM11 VLIMM11 (~0)
#define MLB24 V (1, 1, 24)
#define MLUIMM12TOP VLUIMM12TOP (~0)
/* 12 bit unsigned immediates are split into two parts, 1 bit and 11 bits.
   The upper 1 bit is part of the `dest' field.  This mask is for the
   other 3 bits of the dest field.  */
#define MLUIMM12UNUSED V (7, 3, 22)
#define MLUIMM15TOP MDEST

struct txvu_opcode txvu_lower_opcodes[] =
{
  /* Macros appear first, so the disassembler will try them first.  */
  /* ??? Any aliases?  */
  /* ??? There isn't an explicit nop.  Apparently it's "move vf0,vf0".  */
  { "nop", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x33c) },

  /* The rest of these needn't be sorted, but it helps to find them if they are.  */
  { "b", { SP, LPCREL11 }, MLOP7 + MDEST + MT + MS, VLOP7 (0x20) },
  { "bal", { SP, LITREG, C, LPCREL11 }, MLOP7 + MDEST + MS, VLOP7 (0x21) },
  { "div", { SP, Q, C, LFSFFSREG, C, LFTFFTREG }, MLOP7 + MLOP11, VLOP7 (0x40) + VLOP11 (0x3bc) },
  { "eatan", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fd) },
  { "eatanxy", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77c) },
  { "eatanxz", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77d) },
  { "eexp", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fe) },
  { "eleng", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x74e) },
  { "ercpr", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7be) },
  { "erleng", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73f) },
  { "ersadd", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73d) },
  { "ersqrt", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7bd) },
  { "esadd", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73c) },
  { "esin", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fc) },
  { "esqrt", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7bc) },
  { "esum", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77e) },
  { "fcand", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x12) },
  { "fceq", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x10) },
  { "fcget", { SP, LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x1c) },
  { "fcor", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x13) },
  { "fcset", { SP, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x11) },
  { "fmand", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x1a) },
  { "fmeq", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x18) },
  { "fmor", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x1b) },
  { "fsand", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x16) },
  { "fseq", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x14) },
  { "fsor", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x17) },
  { "fsset", { SP, LUIMM12UP6 }, MLOP7 + MLUIMM12UNUSED + V (~0, 6, 0) + MS + MT, VLOP7 (0x15) },
  { "iadd", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x30) },
  { "iaddi", { SP, LITREG, C, LISREG, C, LIMM5 }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x32) },
  { "iaddiu", { SP, LITREG, C, LISREG, C, LUIMM15 }, MLOP7, VLOP7 (0x08) },
  { "iand", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x34) },
  { "ibeq", { SP, LITREG, C, LISREG, C, LPCREL11 }, MLOP7 + MDEST, VLOP7 (0x28) },
  { "ibgez", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2f) },
  { "ibgtz", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2d) },
  { "iblez", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2e) },
  { "ibltz", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2c) },
  { "ibne", { SP, LITREG, C, LISREG, C, LPCREL11 }, MLOP7 + MDEST, VLOP7 (0x29) },
  { "ilw", { LDOTDEST1, SP, LITREG, C, LIMM11, '(', LISREG, ')', LDEST1 }, MLOP7, VLOP7 (0x04) },
  { "ilwr", { LDOTDEST1, SP, LITREG, C, '(', LISREG, ')', LDEST1 }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3fe) },
  { "ior", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x35) },
  { "isub", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x31) },
  { "isubiu", { SP, LITREG, C, LISREG, C, LUIMM15 }, MLOP7, VLOP7 (0x09) },
  { "isw", { LDOTDEST1, SP, LITREG, C, LIMM11, '(', LISREG, ')', LDEST1 }, MLOP7, VLOP7 (0x05) },
  { "iswr", { LDOTDEST1, SP, LITREG, C, '(', LISREG, ')', LDEST1 }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3ff) },
  { "jalr", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x25) },
  { "jr", { SP, LISREG }, MLOP7 + MDEST + MT + MLIMM11, VLOP7 (0x24) },
  { "lq", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x00) },
  { "lqd", { DOTDEST, SP, VFTREG, C, '(', '-', '-', LISREG, ')' }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x37e) },
  { "lqi", { DOTDEST, SP, VFTREG, C, '(', LISREG, '+', '+', ')' }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x37c) },
  /* Only a single VF reg is allowed here.  We can use VFTREG because LDOTDEST1
     handles verifying only a single choice of xyzw is present.  */
  { "mfir", { LDOTDEST1, SP, VFTREG, C, LISREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3fc) },
  { "mfp", { DOTDEST, SP, VFTREG, C, P }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x67c) },
  { "move", { DOTDEST, SP, VFTREG, C, VFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x33c) },
  { "mr32", { DOTDEST, SP, VFTREG, C, VFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x33d) },
  { "mtir", { LDOTDEST1, SP, LITREG, C, VFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3fd) },
  { "rget", { DOTDEST, SP, VFTREG, C, R }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43d) },
  { "rinit", { SP, R, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43e) },
  { "rnext", { DOTDEST, SP, VFTREG, C, R }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43c) },
  { "rsqrt", { SP, Q, C, LFSFFSREG, C, LFTFFTREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3be) },
  { "rxor", { SP, R, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43f) },
  { "sq", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x01) },
  { "sqd", { DOTDEST, SP, VFTREG, C, '(', '-', '-', LISREG, ')' }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x37f) },
  { "sqi", { DOTDEST, SP, VFTREG, C, '(', LISREG, '+', '+', ')' }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x37d) },
  { "sqrt", { SP, Q, C, LFTFFTREG }, MLOP7 + VLFSF (~0) + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3bd) },
  { "waitp", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x7bf) },
  { "waitq", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x3bf) },
  { "xgkick", { SP, LISREG }, MLOP7 + MDEST + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6fc) },
  { "xitop", { SP, LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6bd) },
  { "xtop", { SP, LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6bc) }
};
const int txvu_lower_opcodes_count = sizeof (txvu_lower_opcodes) / sizeof (txvu_lower_opcodes[0]);

/* Value of DEST in use.
   Each of the registers must specify the same value as the opcode.
   ??? Perhaps remove the duplication?  */
static int state_vu_mnemonic_dest;

/* Value of BC to use.
   The register specified for the ftreg must match the broadcast register
   specified in the opcode.  */
static int state_vu_mnemonic_bc;

/* Multiple destination choice support.
   The "dest" string selects any combination of x,y,z,w.
   [The letters are ordered that way to follow the manual's style.]  */

/* Utility to parse a `dest' spec.
   Return the found value.
   *PSTR is set to the character that terminated the parsing.
   It is up to the caller to do any error checking.  */

static long
u_parse_dest (pstr)
     char **pstr;
{
  long dest = 0;

  while (**pstr)
    {
      switch (**pstr)
	{
	case 'x' : case 'X' : dest |= TXVU_DEST_X; break;
	case 'y' : case 'Y' : dest |= TXVU_DEST_Y; break;
	case 'z' : case 'Z' : dest |= TXVU_DEST_Z; break;
	case 'w' : case 'W' : dest |= TXVU_DEST_W; break;
	default : return dest;
	}
      ++*pstr;
    }

  return dest;
}

static long
parse_dotdest (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  long dest;

  if (**pstr != '.' && **pstr != '/')
    {
      *errmsg = "missing `.'";
      return 0;
    }

  ++*pstr;
  dest = u_parse_dest (pstr);
  if (dest == 0 || isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  return dest;
}

/* Parse a `dest' spec where only a single letter is allowed,
   but the encoding handles all four.  */

static long
parse_dotdest1 (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char c;
  long dest;

  if (**pstr != '.' && **pstr != '/')
    {
      *errmsg = "missing `.'";
      return 0;
    }

  ++*pstr;
  switch (**pstr)
    {
    case 'x' : case 'X' : dest = TXVU_DEST_X; break;
    case 'y' : case 'Y' : dest = TXVU_DEST_Y; break;
    case 'z' : case 'Z' : dest = TXVU_DEST_Z; break;
    case 'w' : case 'W' : dest = TXVU_DEST_W; break;
    default : *errmsg = "invalid `dest'"; return 0;
    }
  ++*pstr;
  c = tolower (**pstr);
  if (c == 'x' || c == 'y' || c == 'z' || c == 'w')
    {
      *errmsg = "only one of x,y,z,w can be specified";
      return 0;
    }
  if (isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  return dest;
}

/* Parse a `dest' spec with no leading '.', where only a single letter is
   allowed, but the encoding handles all four.  The parsed value must match
   that recorded in `dest'.  */

static long
parse_dest1 (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char c;
  long dest;

  dest = u_parse_dest (pstr);
  if (dest != TXVU_DEST_X
      && dest != TXVU_DEST_Y
      && dest != TXVU_DEST_Z
      && dest != TXVU_DEST_W)
    {
      *errmsg = "expecting one of x,y,z,w";
      return 0;
    }

  if (dest != state_vu_mnemonic_dest)
    {
      *errmsg = "`dest' suffix does not match instruction `dest'";
      return 0;
    }

  return dest;
}

static void
insert_dotdest (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  /* Record the DEST value in use so the register parser can use it.  */
  state_vu_mnemonic_dest = value;
  *insn |= value << operand->shift;
}

static long
extract_dotdest (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  /* Record the DEST value in use so the register printer can use it.  */
  state_vu_mnemonic_dest = (*insn >> operand->shift) & ((1 << operand->bits) - 1);
  return state_vu_mnemonic_dest;
}

/* Utility to print a multiple dest spec.  */

static void
u_print_dest (info, insn, value)
     disassemble_info *info;
     TXVU_INSN *insn;
     long value;
{
  if (value & TXVU_DEST_X)
    (*info->fprintf_func) (info->stream, "x");
  if (value & TXVU_DEST_Y)
    (*info->fprintf_func) (info->stream, "y");
  if (value & TXVU_DEST_Z)
    (*info->fprintf_func) (info->stream, "z");
  if (value & TXVU_DEST_W)
    (*info->fprintf_func) (info->stream, "w");
}

static void
print_dotdest (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, ".");
  u_print_dest (info, insn, value);
}

static void
print_dest1 (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  u_print_dest (info, insn, state_vu_mnemonic_dest);
}

/* Utilities for single destination choice handling.  */

static long
u_parse_sdest (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char c;
  long dest = 0;

  switch (**pstr)
    {
    case 'x' : case 'X' : dest = TXVU_SDEST_X; break;
    case 'y' : case 'Y' : dest = TXVU_SDEST_Y; break;
    case 'z' : case 'Z' : dest = TXVU_SDEST_Z; break;
    case 'w' : case 'W' : dest = TXVU_SDEST_W; break;
    default : *errmsg = "only one of x,y,z,w can be specified"; return 0;
    }
  ++*pstr;
  c = tolower (**pstr);
  if (c == 'x' || c == 'y' || c == 'z' || c == 'w')
    {
      *errmsg = "only one of x,y,z,w can be specified";
      return 0;
    }
  if (isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  return dest;
}

static void
print_sdest (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  char c;

  switch (value)
    {
    case TXVU_SDEST_X : c = 'x'; break;
    case TXVU_SDEST_Y : c = 'y'; break;
    case TXVU_SDEST_Z : c = 'z'; break;
    case TXVU_SDEST_W : c = 'w'; break;
    }

  (*info->fprintf_func) (info->stream, "%c", c);
}

/* Broadcase field.  */

static long
parse_bc (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  long value = u_parse_sdest (pstr, errmsg);

  if (*errmsg)
    return 0;
  /* Save value for later verification in register parsing.  */
  state_vu_mnemonic_bc = value;
  return value;
}

/* During the extraction process, save the bc field for use in
   printing the bc register.  */

static long
extract_bc (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  state_vu_mnemonic_bc = *insn & 3;
  return state_vu_mnemonic_bc;
}

static long
parse_vfreg (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_dest;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  reg_dest = u_parse_dest (&str);
  if (reg_dest == 0 || isalnum (*str))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }
  if (reg_dest != state_vu_mnemonic_dest)
    {
      *errmsg = "register `dest' does not match instruction `dest'";
      return 0;
    }
  *pstr = str;
  return reg;
}

static void
print_vfreg (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     disassemble_info *info;
     TXVU_INSN *insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value);
  u_print_dest (info, insn, state_vu_mnemonic_dest);
}

/* FT register in broadcast case.  */

static long
parse_bcftreg (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_bc;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  reg_bc = u_parse_sdest (&str, errmsg);
  if (*errmsg)
    return 0;
  if (reg_bc != state_vu_mnemonic_bc)
    {
      *errmsg = "register `bc' does not match instruction `bc'";
      return 0;
    }
  *pstr = str;
  return reg;
}

static void
print_bcftreg (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value);
  print_sdest (opcode, operand, mods, insn, info, state_vu_mnemonic_bc);
}

/* ACC handling.  */

static long
parse_accdest (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  long acc_dest = 0;

  if (strncasecmp (str, "acc", 3) != 0)
    {
      *errmsg = "expecting `acc'";
      return 0;
    }
  str += 3;
  acc_dest = u_parse_dest (&str);
  if (acc_dest == 0 || isalnum (*str))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }
  if (acc_dest != state_vu_mnemonic_dest)
    {
      *errmsg = "acc `dest' does not match instruction `dest'";
      return 0;
    }
  *pstr = str;
  /* Value isn't used, but we must return something.  */
  return 0;
}

static void
print_accdest (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     disassemble_info *info;
     TXVU_INSN *insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "acc");
  u_print_dest (info, insn, state_vu_mnemonic_dest);
}

/* XYZ operand handling.
   This simplifies the opmula,opmsub entries by keeping them equivalent to
   the others.  */

static void
insert_xyz (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  if (state_vu_mnemonic_dest != (TXVU_DEST_X | TXVU_DEST_Y | TXVU_DEST_Z))
    *errmsg = "expecting `xyz' for `dest' value";
}

/* F[ST] register using selector in F[ST]F field.
   Internally, the value is encoded in 7 bits: the 2 bit xyzw indicator
   followed by the 5 bit register number.  */

static long
parse_ffstreg (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  int reg, xyzw;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  xyzw = u_parse_sdest (&str, errmsg);
  if (*errmsg)
    return 0;
  *pstr = str;
  return reg | (xyzw << 5);
}

static void
print_ffstreg (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value & TXVU_MASK_REG);
  print_sdest (opcode, operand, mods, insn, info, (value >> 5) & 3);
}

static void
insert_ffstreg (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  if (operand->shift == TXVU_SHIFT_SREG)
    *insn |= VLFSF (value >> 5) | VS (value);
  else
    *insn |= VLFTF (value >> 5) | VT (value);
}

static long
extract_ffstreg (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  if (operand->shift == TXVU_SHIFT_SREG)
    return (((*insn & VLFSF (~0)) >> 21) << 5) | VS (*insn);
  else
    return (((*insn & VLFTF (~0)) >> 21) << 5) | VT (*insn);
}

/* F register.  */

static long
parse_freg (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  *pstr = str;
  return reg;
}

static void
print_freg (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value);
}

/* I register.  */

static long
parse_ireg (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'i')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  *pstr = str;
  return reg;
}

static void
print_ireg (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "vi%02ld", value);
}

/* VI01 register.  */

static long
parse_vi01 (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'i')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg != 1)
    {
      *errmsg = "vi01 required here";
      return 0;
    }
  *pstr = str;
  return reg;
}

static void
print_vi01 (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "vi01");
}

/* Lower instruction 12 bit unsigned immediate.  */

static void
insert_luimm12 (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  *insn |= VLUIMM12TOP ((value & (1 << 11)) != 0) | VLIMM11 (value);
}

static long
extract_luimm12 (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return (((*insn & MLUIMM12TOP) != 0) << 11) | VLIMM11 (*insn);
}

/* Lower instruction 12 bit unsigned immediate, upper 6 bits.  */

static void
insert_luimm12up6 (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  *insn |= VLUIMM12TOP ((value & (1 << 11)) != 0) | (value & 0x7c0);
}

/* Lower instruction 15 bit unsigned immediate.  */

static void
insert_luimm15 (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
  *insn |= VLUIMM15TOP (value >> 11) | VLIMM11 (value);
}

static long
extract_luimm15 (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return (((*insn & MLUIMM15TOP) >> 21) << 11) | VLIMM11 (*insn);
}

/* PKE support.  */

PARSE_FN (pke_ibit);
PRINT_FN (pke_ibit);

PARSE_FN (pke_mode);
PRINT_FN (pke_mode);

PARSE_FN (pke_ability);
PRINT_FN (pke_ability);

PARSE_FN (pke_mpgaddr);

PARSE_FN (pke_varlendata);

PARSE_FN (pke_imrbits);
PRINT_FN (pke_imrbits);

PARSE_FN (pke_unpacktype);
PRINT_FN (pke_unpacktype);

PARSE_FN (pke_unpackaddr);

const txvu_operand pke_operands[] =
{
  /* place holder (??? not sure if needed) */
#define PKE_UNUSED 128
  { 0 },

  /* The I bit.  */
#define PKE_IBIT (PKE_UNUSED + 1)
  { 1, 31, TXVU_OPERAND_SUFFIX, parse_pke_ibit, 0, 0, print_pke_ibit },

  /* An 8 bit unsigned immediate, stored in upper 8 bits of immed field.  */
#define PKE_UIMM8UP (PKE_IBIT + 1)
  { 8, 8, 0, 0, 0, 0, 0 },

  /* An 8 bit unsigned immediate, stored in lower 8 bits of immed field.  */
#define PKE_UIMM8LO (PKE_UIMM8UP + 1)
  { 8, 0, 0, 0, 0, 0, 0 },

  /* An 16 bit unsigned immediate, stored in lower 8 bits of immed field.  */
#define PKE_UIMM16 (PKE_UIMM8LO + 1)
  { 16, 0, 0, 0, 0, 0, 0 },

  /* The mode operand of `stmod'.  */
#define PKE_MODE (PKE_UIMM16 + 1)
  { 2, 0, 0, parse_pke_mode, 0, 0, print_pke_mode },

  /* The ability operand of `mskpath3'.  */
#define PKE_ABILITY (PKE_MODE + 1)
  { 1, 15, 0, parse_pke_ability, 0, 0, print_pke_ability },

  /* A VU address.  */
#define PKE_VUADDR (PKE_ABILITY + 1)
  { 16, 0, 0, 0, 0, 0, 0 },

  /* A 32 bit immediate, appearing in 2nd,3rd,4th,5th words.  */
#define PKE_UIMM32 (PKE_VUADDR + 1)
  { 32, 0, 0, 0, 0, 0, 0 },

  /* VU address used by mpg insn.  */
#define PKE_MPGADDR (PKE_UIMM32 + 1)
  { 16, 0, TXVU_OPERAND_ADDRESS, parse_pke_mpgaddr, 0, 0, 0 },

  /* A variable length data specifier.
     Any of: file name, number, or '*'.  */
#define PKE_VARLENDATA (PKE_MPGADDR + 1)
  { 0, 0, 0, parse_pke_varlendata, 0, 0, 0 },

  /* The IMR bits of the unpack insn.  */
#define PKE_IMRBITS (PKE_VARLENDATA + 1)
  { 0, 0, TXVU_OPERAND_SUFFIX, parse_pke_imrbits, 0, 0, print_pke_imrbits },

  /* The type of the unpack insn.  */
#define PKE_UNPACKTYPE (PKE_IMRBITS + 1)
  { 4, 24, 0, parse_pke_unpacktype, 0, 0, print_pke_unpacktype },

  /* VU address used by unpack insn.  */
#define PKE_UNPACKADDR (PKE_UIMM32 + 1)
  { 16, 0, TXVU_OPERAND_ADDRESS, parse_pke_unpackaddr, 0, 0, 0 },

  /* Fake operand processed at end of parsing an insn to set the length.  */

/* end of list place holder */
  { 0 }
};

/* Field mask values.  */
#define MPKECMD 0x7f000000
#define MPKEUNPACK 0x60000000

/* Field values.  */
#define VPKECMD(x) V ((x), 7, 24)
#define VPKEUNPACK V (0x60, 8, 24)

struct txvu_opcode pke_opcodes[] =
{
  { "pkenop", { PKE_IBIT }, 0x7fffffff, 0 },
  { "stcycle", { PKE_IBIT, SP, PKE_UIMM8UP, C, PKE_UIMM8LO }, MPKECMD, VPKECMD (1) },
  { "offset", { PKE_IBIT, SP, PKE_UIMM16 }, MPKECMD, VPKECMD (2) },
  { "base", { PKE_IBIT, SP, PKE_UIMM16 }, MPKECMD, VPKECMD (3) },
  { "itop", { PKE_IBIT, SP, PKE_UIMM16 }, MPKECMD, VPKECMD (4) },
  { "stmod", { PKE_IBIT, SP, PKE_MODE }, MPKECMD + V (~0, 14, 2), VPKECMD (5) },
  { "mskpath3", { PKE_IBIT, SP, PKE_ABILITY }, MPKECMD + V (~0, 15, 0), VPKECMD (6) },
  { "pkemark", { PKE_IBIT, SP, PKE_UIMM16 }, MPKECMD, VPKECMD (7) },
  { "flushe", { PKE_IBIT }, MPKECMD, VPKECMD (16) },
  { "flush", { PKE_IBIT }, MPKECMD, VPKECMD (17) },
  { "flusha", { PKE_IBIT }, MPKECMD, VPKECMD (19) },
  { "pkemscal", { PKE_IBIT, SP, PKE_VUADDR }, MPKECMD, VPKECMD (20) },
  { "pkemscnt", { PKE_IBIT }, MPKECMD, VPKECMD (23) },
  { "pkemscalf", { PKE_IBIT, SP, PKE_VUADDR }, MPKECMD, VPKECMD (21) },

  /* 2 word instructions */
  { "stmask", { PKE_IBIT, SP, PKE_UIMM32 }, MPKECMD, VPKECMD (32), PKE_OPCODE_LEN2 },

  /* 5 word instructions */
  { "strow", { PKE_IBIT, SP, PKE_UIMM32, C, PKE_UIMM32, C, PKE_UIMM32, C, PKE_UIMM32 }, MPKECMD, VPKECMD (48), PKE_OPCODE_LEN5 },
  { "stcol", { PKE_IBIT, SP, PKE_UIMM32, C, PKE_UIMM32, C, PKE_UIMM32, C, PKE_UIMM32 }, MPKECMD, VPKECMD (49), PKE_OPCODE_LEN5 },

  /* variable length instructions */
  { "mpg", { PKE_IBIT, SP, PKE_MPGADDR, PKE_VARLENDATA }, MPKECMD, VPKECMD (0x4a), PKE_OPCODE_LENVAR + PKE_OPCODE_MPG },
  { "direct", { PKE_IBIT, SP, PKE_VARLENDATA }, MPKECMD, VPKECMD (0x50), PKE_OPCODE_LENVAR + PKE_OPCODE_DIRECT },
  { "directhl", { PKE_IBIT, SP, PKE_VARLENDATA }, MPKECMD, VPKECMD (0x51), PKE_OPCODE_LENVAR + PKE_OPCODE_DIRECT },
  { "unpack", { PKE_IMRBITS, SP, PKE_UNPACKTYPE, C, PKE_UNPACKADDR, C, PKE_VARLENDATA }, MPKEUNPACK, VPKEUNPACK, PKE_OPCODE_LENVAR + PKE_OPCODE_UNPACK },
};
const int pke_opcodes_count = sizeof (pke_opcodes) / sizeof (pke_opcodes[0]);

/* Non-zero if a variable length instruction was parsed.  */
static int state_pke_varlen_p;

/* Length of parsed insn, in 32 bit words, or 0 if unknown.  */
static int state_pke_len;

/* PKE parse,insert,extract,print helper fns.  */

static long
parse_pke_ibit (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  int flags = 0;

  if (*str != '[')
    return 0;

  for (str = str + 1; *str != ']'; ++str)
    {
      switch (tolower (*str))
	{
	case 'i' : flags = 1; break;
	default : *errmsg = "unknown flag"; return 0;
	}
    }

  *pstr = str + 1;
  return flags;
}

static void
print_pke_ibit (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  if (value)
    (*info->fprintf_func) (info->stream, "[i]");
}

static const keyword stmod_modes[] = {
  { PKE_MODE_DIRECT, "direct" },
  { PKE_MODE_ADD,    "add" },
  { PKE_MODE_ADDROW, "addrow" },
  { 0, 0 }
};

static long
parse_pke_mode (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  int mode;
  char *str = *pstr;
  char *start;
  char c;

  start = str;
  str = scan_symbol (str);
  c = *str;
  *str = 0;
  mode = lookup_keyword_value (stmod_modes, start, 0);
  *str = c;
  if (mode != -1)
    {
      *pstr = str;
      return mode;
    }
  *errmsg = "invalid mode";
  return 0;
}

static void
print_pke_mode (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     disassemble_info *info;
     TXVU_INSN *insn;
     long value;
{
  const char *name = lookup_keyword_name (stmod_modes, value);

  if (name)
    (*info->fprintf_func) (info->stream, name);
  else
    (*info->fprintf_func) (info->stream, "???");
}

static long
parse_pke_ability (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;

  if (strncasecmp (str, "disable", 7) == 0)
    {
      *pstr += 7;
      return 0;
    }
  else if (strncasecmp (str, "enable", 6) == 0)
    {
      *pstr += 6;
      return 1;
    }
  *errmsg = "invalid ability";
  return 0;
}

static void
print_pke_ability (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  if (value)
    (*info->fprintf_func) (info->stream, "enable");
  else
    (*info->fprintf_func) (info->stream, "disable");
}

static long
parse_pke_mpgaddr (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;

  if (*str == '*')
    {
      ++*pstr;
      return 0; /* FIXME:indicate * somehow */
    }

  start = str;
  str = strchr (str, ',');
  if (! str)
    {
      *errmsg = "invalid mpg address";
      return 0;
    }

  /* FIXME: call back to expression() to parse address.  */

  *pstr = str;
  return 0;
}

/* The result here is either the length specified,
   or PKE_VARLENDATA_FILE or PKE_VARLENDATA_UNKNOWN.  */

static long
parse_pke_varlendata (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;

  if (*str == '*')
    {
      ++*pstr;
      return 0; /* FIXME:indicate * somehow */
    }

  start = str;

  /* FIXME: call back to expression() to parse address,
     and pick out filename if such.  */

  *pstr = str + strlen (str);
  return 0;
}

static long
parse_pke_imrbits (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  int flags = 0;

  if (*str != '[')
    return 0;

  for (str = str + 1; *str != ']'; ++str)
    {
      switch (tolower (*str))
	{
	case 'i' : flags |= PKE_FLAG_I; break;
	case 'm' : flags |= PKE_FLAG_M; break;
	case 'r' : flags |= PKE_FLAG_R; break;
	default : *errmsg = "unknown pke flag"; return 0;
	}
    }

  *pstr = str + 1;
  return flags;
}

static void
print_pke_imrbits (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  if (value)
    {
      (*info->fprintf_func) (info->stream, "[");
      if (value & PKE_FLAG_I)
	(*info->fprintf_func) (info->stream, "i");
      if (value & PKE_FLAG_M)
	(*info->fprintf_func) (info->stream, "m");
      if (value & PKE_FLAG_R)
	(*info->fprintf_func) (info->stream, "r");
      (*info->fprintf_func) (info->stream, "]");
    }
}

static const keyword unpack_types[] = {
  { PKE_UNPACK_S_32, "s_32" },
  { PKE_UNPACK_S_16, "s_16" },
  { PKE_UNPACK_S_8, "s_8" },
  { PKE_UNPACK_V2_32, "v2_32" },
  { PKE_UNPACK_V2_16, "v2_16" },
  { PKE_UNPACK_V2_8, "v2_8" },
  { PKE_UNPACK_V3_32, "v3_32" },
  { PKE_UNPACK_V3_16, "v3_16" },
  { PKE_UNPACK_V3_8, "v3_8" },
  { PKE_UNPACK_V4_32, "v4_32" },
  { PKE_UNPACK_V4_16, "v4_16" },
  { PKE_UNPACK_V4_8, "v4_8" },
  { PKE_UNPACK_V4_5, "v4_5" },
  { 0, 0 }
};

static long
parse_pke_unpacktype (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  int type;
  char *str = *pstr;
  char *start;
  char c;

  start = str;
  str = scan_symbol (str);
  c = *str;
  *str = 0;
  type = lookup_keyword_value (unpack_types, start, 0);
  *str = c;
  if (type != -1)
    {
      *pstr = str;
      return type;
    }
  *errmsg = "invalid unpack type";
  return 0;
}

static void
print_pke_unpacktype (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  const char *name = lookup_keyword_name (unpack_types, value);

  if (name)
    (*info->fprintf_func) (info->stream, name);
  else
    (*info->fprintf_func) (info->stream, "???");
}

static long
parse_pke_unpackaddr (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
}

/* External PKE supporting routines.  */

/* Return non-zero if the just parsed pke insn has variable length.  */

int
pke_varlen_p ()
{
  /* This shouldn't be called unless a pke insn was parsed.
     Also, we want to catch errors in parsing that don't set this.  */
  if (state_pke_varlen_p == -1)
    abort ();

  return state_pke_varlen_p;
}

/* Return length, in 32 bit words, of just parsed pke insn,
   or 0 if unknown.  */

int
pke_len ()
{
  /* This shouldn't be called unless a pke insn was parsed.
     Also, we want to catch errors in parsing that don't set this.  */
  if (state_pke_len == -1)
    abort ();

  return state_pke_len;
}

/* DMA support.  */

PARSE_FN (dma_flags);
INSERT_FN (dma_flags);
EXTRACT_FN (dma_flags);
PRINT_FN (dma_flags);

PARSE_FN (dma_data);
INSERT_FN (dma_data);
EXTRACT_FN (dma_data);
PRINT_FN (dma_data);

PARSE_FN (dma_next);
INSERT_FN (dma_next);
EXTRACT_FN (dma_next);
PRINT_FN (dma_next);

const txvu_operand dma_operands[] =
{
    /* place holder (??? not sure if needed) */
#define DMA_UNUSED 128
    { 0 },

    /* dma tag flag bits */
#define DMA_FLAGS (DMA_UNUSED + 1)
    { 0, 0, TXVU_OPERAND_SUFFIX,
      parse_dma_flags, insert_dma_flags, extract_dma_flags, print_dma_flags },

    /* dma data spec */
#define DMA_DATA (DMA_FLAGS + 1)
    { 0, 0, TXVU_OPERAND_DMA_COUNT,
      0, insert_dma_data, extract_dma_data, print_dma_data },

    /* dma data finalization spec */
#define DMA_DATA2 (DMA_DATA + 1)
    { 0, 0, TXVU_OPERAND_FAKE,
      parse_dma_data2, 0, 0, 0},

    /* dma next tag spec */
#define DMA_ADDR (DMA_DATA2 + 1)
    { 0, 0, 0,
      parse_dma_addr, insert_dma_addr, extract_dma_addr, print_dma_addr},

/* end of list place holder */
  { 0 }
};

struct txvu_opcode dma_opcodes[] =
{
    /* ??? Some of these may take optional arguments.
    The way to handle that is to have multiple table entries, those with and
    those without the optional arguments.  */
/*TODO*/    { "dmacall", { DMA_FLAGS, SP, DMA_DATA, C, DMA_ADDR}, 0, 1},
/*TODO*/    { "dmacnt", { DMA_FLAGS, SP, DMA_DATA, C, DMA_ADDR}, 0, 2},
    { "dmaend", { DMA_FLAGS, SP, DMA_DATA, DMA_DATA2}, 0, 3},
    { "dmaend", { DMA_FLAGS, SP, DMA_DATA, C, DMA_ADDR}, 0, 4},
/*TODO*/    { "dmanext", { DMA_FLAGS, SP, DMA_DATA, C, DMA_ADDR}, 0, 5},
/*TODO      { "dmaref", { DMA_FLAGS, SP, DMA_DATA, C, DMA_PTR_ADDR}, 0, 6},	*/
/*TODO      { "dmarefs", { DMA_FLAGS, SP, DMA_DATA, C, DMA_PTR_ADDR}, 0, 7},	*/
/*TODO*/    { "dmaret", { DMA_FLAGS, SP, DMA_DATA, DMA_DATA2}, 0, 8}
/*TODO*/    { "dmaret", { DMA_FLAGS, SP, DMA_DATA, C, DMA_ADDR}, 0, 9}
};
const int dma_opcodes_count = sizeof (dma_opcodes) / sizeof (dma_opcodes[0]);

/* Did the current DMA instruction has specify "*" as its length operand?
-1=uninitialized, 0=no, 1=yes. */
static int state_dma_operand_count_p;

/* DMA parse,insert,extract,print helper fns.  */

static long
parse_dma_flags (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  int flags = 0;

  if (*str != '[')
    return 0;

  for (str = str + 1; *str != ']'; ++str)
    {
      switch (tolower (*str))
	{
	case '0' : flags |= DMA_FLAG_PCE0; break;
	case '1' : flags |= DMA_FLAG_PCE1; break;
	case 'i' : flags |= DMA_FLAG_INT; break;
	case 's' : flags |= DMA_FLAG_SPR; break;
	default : *errmsg = "unknown dma flag"; return 0;
	}
    }

  *pstr = str + 1;
  return flags;
}

static void
insert_dma_flags (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_dma_flags (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_dma_flags (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  if (value)
    {
      (*info->fprintf_func) (info->stream, "[");
      if (value & DMA_FLAG_PCE0)
	(*info->fprintf_func) (info->stream, "0");
      if (value & DMA_FLAG_PCE1)
	(*info->fprintf_func) (info->stream, "1");
      if (value & DMA_FLAG_INT)
	(*info->fprintf_func) (info->stream, "i");
      if (value & DMA_FLAG_SPR)
	(*info->fprintf_func) (info->stream, "s");
      (*info->fprintf_func) (info->stream, "]");
    }
}

static long
parse_dma_data2( opcode, operand, mods, pstr, errmsg)
    const txvu_opcode *opcode;
    const txvu_operand *operand;
    int mods;
    char **pstr;
    const char **errmsg;
{
    /*
    If txvu_dma_operand_count() < 0 error
    if txvu_dma_operand_count() > 0
	if dmaref || dmarefs
	    compute from two related symbols
	else
	    compute from current addr and end symbol
	store value to count field?
    */
}

static void
insert_dma_data (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_dma_data (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_dma_data (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

static long
parse_dma_addr (opcode, operand, mods, pstr, errmsg)
    const txvu_opcode *opcode;
    const txvu_operand *operand;
    int mods;
    char **pstr;
    const char **errmsg;
{
    char *start = *pstr;
    char *end = scan_symbol( start);

    if( end == start )
    {
	*errmsg = "invalid dma next tag";
	return 0;
    }

    /* FIXME: unfinished
    if txvu_dma_operand_count() > 0
	if dmaref || dmarefs
	    this operand must be a symbol (vs an expression)
	    lookup the symbol
	    store the symbol's value in the addr field (relocs?)
	    compute the end_symbol's name
	    lookup the end_symbol
	    if not found: error
	    compute the length as _$<name>-<name>
	else
	    evaluate the operand as an expression
	    store the value to the count field
	    compute the length as _$EndDma-.
	store the count field
    else
	evaluate the operand as an expression
	store the value to the count field
    */

    parse_dma_data2( opcode, operand, mods, pstr, errmsg);

    *pstr = end;
    return 0;
}

static void
insert_dma_addr (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_dma_addr (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_dma_addr (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

/* GPUIF support.  */

PARSE_FN (gpuif_prim);
INSERT_FN (gpuif_prim);
EXTRACT_FN (gpuif_prim);
PRINT_FN (gpuif_prim);

PARSE_FN (gpuif_regs);
INSERT_FN (gpuif_regs);
EXTRACT_FN (gpuif_regs);
PRINT_FN (gpuif_regs);

PARSE_FN (gpuif_nloop);
INSERT_FN (gpuif_nloop);
EXTRACT_FN (gpuif_nloop);
PRINT_FN (gpuif_nloop);

PARSE_FN (gpuif_eop);
INSERT_FN (gpuif_eop);
EXTRACT_FN (gpuif_eop);
PRINT_FN (gpuif_eop);

const txvu_operand gpuif_operands[] =
{
  /* place holder (??? not sure if needed) */
#define GPUIF_UNUSED 128
  { 0 },

  /* PRIM=foo operand */
#define GPUIF_PRIM (GPUIF_UNUSED + 1)
  { 0, 0, 0, parse_gpuif_prim, insert_gpuif_prim, extract_gpuif_prim, print_gpuif_prim },

  /* REGS=foo operand */
#define GPUIF_REGS (GPUIF_PRIM + 1)
  { 0, 0, 0, parse_gpuif_regs, insert_gpuif_regs, extract_gpuif_regs, print_gpuif_regs },

  /* NLOOP=foo operand */
#define GPUIF_NLOOP (GPUIF_REGS + 1)
  { 0, 0, 0, parse_gpuif_nloop, insert_gpuif_nloop, extract_gpuif_nloop, print_gpuif_nloop },

  /* EOP operand */
#define GPUIF_EOP (GPUIF_NLOOP + 1)
  { 0, 0, 0, parse_gpuif_eop, insert_gpuif_eop, extract_gpuif_eop, print_gpuif_eop },

/* end of list place holder */
  { 0 }
};

struct txvu_opcode gpuif_opcodes[] =
{
  /* Some of these may take optional arguments.
     The way this is handled is to have multiple table entries, those with and
     those without the optional arguments.
     !!! The order here is important.  The code that scans this table assumes
     that if it reaches the end of a syntax string there is nothing more to
     parse.  This means that longer versions of instructions must appear before
     shorter ones.  Otherwise the text at the "end" of a longer one may be
     interpreted as junk when the parser is using a shorter version of the
     syntax string.  */

  { "gpuifpacked", { SP, GPUIF_PRIM, C, GPUIF_REGS, C, GPUIF_NLOOP, C, GPUIF_EOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_REGS, C, GPUIF_NLOOP, C, GPUIF_EOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_PRIM, C, GPUIF_REGS, C, GPUIF_EOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_PRIM, C, GPUIF_REGS, C, GPUIF_NLOOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_REGS, C, GPUIF_EOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_REGS, C, GPUIF_NLOOP }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_PRIM, C, GPUIF_REGS }, 0, 1 },
  { "gpuifpacked", { SP, GPUIF_REGS }, 0, 1 },

  { "gpuifreglist", { SP, GPUIF_REGS, C, GPUIF_NLOOP, C, GPUIF_EOP }, 0, 2 },
  { "gpuifreglist", { SP, GPUIF_REGS, C, GPUIF_EOP }, 0, 2 },
  { "gpuifreglist", { SP, GPUIF_REGS, C, GPUIF_NLOOP }, 0, 2 },
  { "gpuifreglist", { SP, GPUIF_REGS }, 0, 2 },

  { "gpuifimage", { SP, GPUIF_NLOOP }, 0, 3 },
  { "gpuifimage", { 0 }, 0, 3 },
};
const int gpuif_opcodes_count = sizeof (gpuif_opcodes) / sizeof (gpuif_opcodes[0]);

/* GPUIF parse,insert,extract,print helper fns.  */

static long
parse_gpuif_prim (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long prim;

  if (strncasecmp (str, "prim=", 5) != 0)
    {
      *errmsg = "missing PRIM spec";
      return 0;
    }
  str += 5;
  for (start = str; isalnum (*str); ++str)
    continue;
  if (str == start)
    {
      *errmsg = "missing PRIM spec";
      return 0;
    }
  /* FIXME: Yes, atoi doesn't do error checking.  Later.  */
  prim = atoi (start);
  *pstr = str;
  return prim;
}

static void
insert_gpuif_prim (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_gpuif_prim (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_gpuif_prim (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

static const keyword gpuif_regs[] = {
  { GPUIF_REG_PRIM,      "prim" },
  { GPUIF_REG_RGBAQ,     "rgbaq" },
  { GPUIF_REG_ST,        "st" },
  { GPUIF_REG_UV,        "uv" },
  { GPUIF_REG_XYZF2,     "xyzf2" },
  { GPUIF_REG_TEXCLUT_1, "texclut_1" },
  { GPUIF_REG_TEXCLUT_2, "texclut_2" },
  { GPUIF_REG_TEX0_1,    "tex0_1" },
  { GPUIF_REG_TEX0_2,    "tex0_2" },
  { GPUIF_REG_TEX1_1,    "tex1_1" },
  { GPUIF_REG_TEX1_2,    "tex1_2" },
  { GPUIF_REG_XYZF3,     "xyzf3" },
  { GPUIF_REG_PRMODE,    "prmode" },
  { GPUIF_REG_A_D,       "a_d" },
  { GPUIF_REG_NOP,       "nop" },
  { 0, 0 }
};

/* Parse a REGS= spec.
   The result is ???.  */

static long
parse_gpuif_regs (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  char c;
  int reg;

  if (strncasecmp (str, "regs=", 5) != 0)
    {
      *errmsg = "missing REGS spec";
      return 0;
    }
  str += 5;
  SKIP_BLANKS (str);
  if (*str != '{')
    {
      *errmsg = "missing '{' in REGS spec";
      return 0;
    }
  ++str;

  while (*str && *str != '}')
    {
      /* Pick out the register name.  */

      SKIP_BLANKS (str);
      start = str;
      str = scan_symbol (str);
      if (str == start)
	{
	  *errmsg = "invalid REG";
	  return 0;
	}

      /* Look it up in the table.  */

      c = *str;
      *str = 0;
      reg = lookup_keyword_value (gpuif_regs, start, 0);
      *str = c;
      if (reg == -1)
	{
	  *errmsg = "invalid REG";
	  return 0;
	}

      /* FIXME: save `reg' away somewhere */

      /* Prepare for the next one.  */

      SKIP_BLANKS (str);
      if (*str == ',')
	++str;
      else if (*str != '}')
	break;
    }
  if (*str != '}')
    {
      *errmsg = "missing '{' in REGS spec";
      return 0;
    }

  *pstr = str + 1;
  return 0; /* FIXME */
}

static void
insert_gpuif_regs (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_gpuif_regs (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_gpuif_regs (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

static long
parse_gpuif_nloop (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  char c;
  int nloop;

  if (strncasecmp (str, "nloop=", 6) != 0)
    {
      *errmsg = "missing NLOOP spec";
      return 0;
    }
  str += 6;
  SKIP_BLANKS (str);
  start = str;
  str = scan_symbol (str);
  if (str == start)
    {
      *errmsg = "invalid NOOP spec";
      return 0;
    }
  /* FIXME: error checking */
  nloop = atoi (start);
  *pstr = str;
  return nloop;
}

static void
insert_gpuif_nloop (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_gpuif_nloop (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_gpuif_nloop (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

static long
parse_gpuif_eop (opcode, operand, mods, pstr, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     char **pstr;
     const char **errmsg;
{
  if (strncasecmp (*pstr, "eop", 3) == 0)
    {
      *pstr += 3;
      return 1;
    }
  *errmsg = "missing `EOP'";
  return 0;
}

static void
insert_gpuif_eop (opcode, operand, mods, insn, value, errmsg)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     long value;
     const char **errmsg;
{
}

static long
extract_gpuif_eop (opcode, operand, mods, insn, pinvalid)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     int *pinvalid;
{
  return 0;
}

static void
print_gpuif_eop (opcode, operand, mods, insn, info, value)
     const txvu_opcode *opcode;
     const txvu_operand *operand;
     int mods;
     TXVU_INSN *insn;
     disassemble_info *info;
     long value;
{
  (*info->fprintf_func) (info->stream, "???");
}

/* Init fns.
   These are called before doing each of the respective activities.  */

/* Called by the assembler before parsing an instruction.  */

void
txvu_opcode_init_parse ()
{
  state_vu_mnemonic_dest = -1;
  state_vu_mnemonic_bc = -1;
  state_pke_varlen_p = -1;
  state_pke_len = -1;
  state_dma_operand_count_p = -1;
}

/*
Query or set the current type of a DMA length operand, explicit or computed by "as".
The return value is the setting before "action" is applied:
    -1=uninitialized, 0=explicit, +1=computed
The value of "action" is interpreted as:
    -1=no change, 0=set explicit, +1=set computed
*/
int
txvu_dma_operand_count( action)
{
    int result = state_dma_operand_count;

    if( action == 0)
	state_dma_operand_count = 0;
    else if( action > 0)
        state_dma_operand_count_p = 1;

    return result;
}

/* Called by the disassembler before printing an instruction.  */

void
txvu_opcode_init_print ()
{
  state_vu_mnemonic_dest = -1;
  state_vu_mnemonic_bc = -1;
  state_pke_varlen_p = -1;
  state_pke_len = -1;
}

/* Indexed by first letter of opcode.  Points to chain of opcodes with same
   first letter.  */
/* ??? One can certainly use a better hash.  Later.  */
static txvu_opcode *upper_opcode_map[26 + 1];
static txvu_opcode *lower_opcode_map[26 + 1];

/* Indexed by insn code.  Points to chain of opcodes with same insn code.  */
static txvu_opcode *upper_icode_map[(1 << TXVU_ICODE_HASH_SIZE) - 1];
static txvu_opcode *lower_icode_map[(1 << TXVU_ICODE_HASH_SIZE) - 1];

/* Initialize any tables that need it.
   Must be called once at start up (or when first needed).

   FLAGS is currently unused but is intended to control initialization.  */

void
txvu_opcode_init_tables (flags)
     int flags;
{
  static int init_p = 0;

  /* We may be intentionally called more than once (for example gdb will call
     us each time the user switches cpu).  These tables only need to be init'd
     once though.  */
  /* ??? We can remove the need for txvu_opcode_supported by taking it into
     account here, but I'm not sure I want to do that yet (if ever).  */
  if (!init_p)
    {
      int i,n;

      /* Upper VU table.  */

      memset (upper_opcode_map, 0, sizeof (upper_opcode_map));
      memset (upper_icode_map, 0, sizeof (upper_icode_map));
      /* Scan the table backwards so macros appear at the front.  */
      for (i = txvu_upper_opcodes_count - 1; i >= 0; --i)
	{
	  int opcode_hash = TXVU_HASH_UPPER_OPCODE (txvu_upper_opcodes[i].mnemonic);
	  int icode_hash = TXVU_HASH_UPPER_ICODE (txvu_upper_opcodes[i].value);

	  txvu_upper_opcodes[i].next_asm = upper_opcode_map[opcode_hash];
	  upper_opcode_map[opcode_hash] = &txvu_upper_opcodes[i];

	  txvu_upper_opcodes[i].next_dis = upper_icode_map[icode_hash];
	  upper_icode_map[icode_hash] = &txvu_upper_opcodes[i];
	}

      /* Lower VU table.  */

      memset (lower_opcode_map, 0, sizeof (lower_opcode_map));
      memset (lower_icode_map, 0, sizeof (lower_icode_map));
      /* Scan the table backwards so macros appear at the front.  */
      for (i = txvu_lower_opcodes_count - 1; i >= 0; --i)
	{
	  int opcode_hash = TXVU_HASH_LOWER_OPCODE (txvu_lower_opcodes[i].mnemonic);
	  int icode_hash = TXVU_HASH_LOWER_ICODE (txvu_lower_opcodes[i].value);

	  txvu_lower_opcodes[i].next_asm = lower_opcode_map[opcode_hash];
	  lower_opcode_map[opcode_hash] = &txvu_lower_opcodes[i];

	  txvu_lower_opcodes[i].next_dis = lower_icode_map[icode_hash];
	  lower_icode_map[icode_hash] = &txvu_lower_opcodes[i];
	}

      /* FIXME: We just hash everything to the same value for the rest.
	 Quick hack while other things are worked on.  */

      /* PKE table.  */

      for (i = pke_opcodes_count - 2; i >= 0; --i)
	{
	  pke_opcodes[i].next_asm = & pke_opcodes[i+1];
	  pke_opcodes[i].next_dis = & pke_opcodes[i+1];
	}

      /* DMA table.  */

      for (i = dma_opcodes_count - 2; i >= 0; --i)
	{
	  dma_opcodes[i].next_asm = & dma_opcodes[i+1];
	  dma_opcodes[i].next_dis = & dma_opcodes[i+1];
	}

      /* GPUIF table.  */

      for (i = gpuif_opcodes_count - 2; i >= 0; --i)
	{
	  gpuif_opcodes[i].next_asm = & gpuif_opcodes[i+1];
	  gpuif_opcodes[i].next_dis = & gpuif_opcodes[i+1];
	}

      init_p = 1;
    }
}

/* Return the first insn in the chain for assembling upper INSN.  */

const txvu_opcode *
txvu_upper_opcode_lookup_asm (insn)
     const char *insn;
{
  return upper_opcode_map[TXVU_HASH_UPPER_OPCODE (insn)];
}

/* Return the first insn in the chain for disassembling upper INSN.  */

const txvu_opcode *
txvu_upper_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return upper_icode_map[TXVU_HASH_UPPER_ICODE (insn)];
}

/* Return the first insn in the chain for assembling lower INSN.  */

const txvu_opcode *
txvu_lower_opcode_lookup_asm (insn)
     const char *insn;
{
  return lower_opcode_map[TXVU_HASH_LOWER_OPCODE (insn)];
}

/* Return the first insn in the chain for disassembling lower INSN.  */

const txvu_opcode *
txvu_lower_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return lower_icode_map[TXVU_HASH_LOWER_ICODE (insn)];
}

/* Return the first insn in the chain for assembling lower INSN.  */

const txvu_opcode *
pke_opcode_lookup_asm (insn)
     const char *insn;
{
  return &pke_opcodes[0];
}

/* Return the first insn in the chain for disassembling lower INSN.  */

const txvu_opcode *
pke_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return &pke_opcodes[0];
}

/* Return the first insn in the chain for assembling lower INSN.  */

const txvu_opcode *
dma_opcode_lookup_asm (insn)
     const char *insn;
{
  return &dma_opcodes[0];
}

/* Return the first insn in the chain for disassembling lower INSN.  */

const txvu_opcode *
dma_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return &dma_opcodes[0];
}

/* Return the first insn in the chain for assembling lower INSN.  */

const txvu_opcode *
gpuif_opcode_lookup_asm (insn)
     const char *insn;
{
  return &gpuif_opcodes[0];
}

/* Return the first insn in the chain for disassembling lower INSN.  */

const txvu_opcode *
gpuif_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return &gpuif_opcodes[0];
}

/* Misc. utilities.  */

/* Scan a symbol and return a pointer to one past the end.  */

static char *
scan_symbol (sym)
     char *sym;
{
  while (*sym && issymchar (*sym))
    ++sym;
  return sym;
}

/* Given a keyword, look up its value, or -1 if not found.  */

static int
lookup_keyword_value (table, name, case_sensitive_p)
     const keyword *table;
     const char *name;
     int case_sensitive_p;
{
  const keyword *p;

  if (case_sensitive_p)
    {
      for (p = table; p->name; ++p)
	if (strcmp (name, p->name) == 0)
	  return p->value;
    }
  else
    {
      for (p = table; p->name; ++p)
	if (strcasecmp (name, p->name) == 0)
	  return p->value;
    }

  return -1;
}

/* Given a keyword's value, look up its name, or NULL if not found.  */

static const char *
lookup_keyword_name (table, value)
     const keyword *table;
     int value;
{
  const keyword *p;

  for (p = table; p->name; ++p)
    if (value == p->value)
      return p->name;

  return NULL;
}
