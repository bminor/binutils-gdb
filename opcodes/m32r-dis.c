/* Disassembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE m32r-dis.c.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

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
#include "dis-asm.h"
#include "bfd.h"
#include "symcat.h"
#include "m32r-opc.h"
#include "opintl.h"

#undef INLINE
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

/* Default text to print if an instruction isn't recognized.  */
#define UNKNOWN_INSN_MSG _("*unknown*")

static int extract_normal
     PARAMS ((CGEN_OPCODE_DESC, CGEN_EXTRACT_INFO *, CGEN_INSN_BYTES,
	      unsigned int, int, int, int, long *));
static void print_normal
     PARAMS ((CGEN_OPCODE_DESC, PTR, long, unsigned int, bfd_vma, int));
static void print_address
     PARAMS ((CGEN_OPCODE_DESC, PTR, bfd_vma, unsigned int, bfd_vma, int));
static void print_keyword
     PARAMS ((CGEN_OPCODE_DESC, PTR, CGEN_KEYWORD *, long, unsigned int));
static int extract_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *, CGEN_EXTRACT_INFO *,
	      unsigned long, CGEN_FIELDS *, bfd_vma));
static void print_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, PTR, const CGEN_INSN *, CGEN_FIELDS *,
	      bfd_vma, int));
static int print_insn PARAMS ((CGEN_OPCODE_DESC, bfd_vma,
			       disassemble_info *, char *, int));
static int default_print_insn
     PARAMS ((CGEN_OPCODE_DESC, bfd_vma, disassemble_info *));

/* -- disassembler routines inserted here */
/* -- dis.c */

/* Immediate values are prefixed with '#'.  */

#define CGEN_PRINT_NORMAL(od, info, value, attrs, pc, length) \
do { \
  if ((attrs) & (1 << CGEN_OPERAND_HASH_PREFIX)) \
    (*info->fprintf_func) (info->stream, "#"); \
} while (0)

/* Handle '#' prefixes as operands.  */

static void
print_hash (od, dis_info, value, attrs, pc, length)
     CGEN_OPCODE_DESC od;
     PTR dis_info;
     long value;
     unsigned int attrs;
     bfd_vma pc;
     int length;
{
  disassemble_info *info = dis_info;
  (*info->fprintf_func) (info->stream, "#");
}

#undef CGEN_PRINT_INSN
#define CGEN_PRINT_INSN my_print_insn

static int
my_print_insn (od, pc, info)
     CGEN_OPCODE_DESC od;
     bfd_vma pc;
     disassemble_info *info;
{
  char buffer[CGEN_MAX_INSN_SIZE];
  char *buf = buffer;
  int status;
  int buflen = (pc & 3) == 0 ? 4 : 2;

  /* Read the base part of the insn.  */

  status = (*info->read_memory_func) (pc, buf, buflen, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }

  /* 32 bit insn?  */
  if ((pc & 3) == 0 && (buf[0] & 0x80) != 0)
    return print_insn (od, pc, info, buf, buflen);

  /* Print the first insn.  */
  if ((pc & 3) == 0)
    {
      if (print_insn (od, pc, info, buf, 2) == 0)
	(*info->fprintf_func) (info->stream, UNKNOWN_INSN_MSG);
      buf += 2;
    }

  if (buf[0] & 0x80)
    {
      /* Parallel.  */
      (*info->fprintf_func) (info->stream, " || ");
      buf[0] &= 0x7f;
    }
  else
    (*info->fprintf_func) (info->stream, " -> ");

  /* The "& 3" is to pass a consistent address.
     Parallel insns arguably both begin on the word boundary.
     Also, branch insns are calculated relative to the word boundary.  */
  if (print_insn (od, pc & ~ (bfd_vma) 3, info, buf, 2) == 0)
    (*info->fprintf_func) (info->stream, UNKNOWN_INSN_MSG);

  return (pc & 3) ? 2 : 4;
}

/* -- */

/* Main entry point for operand extraction.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `print_insn_normal', but keeping it
   separate makes clear the interface between `print_insn_normal' and each of
   the handlers.
*/

int
m32r_cgen_extract_operand (od, opindex, ex_info, insn_value, fields, pc)
     CGEN_OPCODE_DESC od;
     int opindex;
     CGEN_EXTRACT_INFO *ex_info;
     CGEN_INSN_BYTES insn_value;
     CGEN_FIELDS * fields;
     bfd_vma pc;
{
  int length;

  switch (opindex)
    {
    case M32R_OPERAND_SR :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r2);
      break;
    case M32R_OPERAND_DR :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 4, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r1);
      break;
    case M32R_OPERAND_SRC1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 4, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r1);
      break;
    case M32R_OPERAND_SRC2 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r2);
      break;
    case M32R_OPERAND_SCR :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r2);
      break;
    case M32R_OPERAND_DCR :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 4, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_r1);
      break;
    case M32R_OPERAND_SIMM8 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX), 8, 8, CGEN_FIELDS_BITSIZE (fields), & fields->f_simm8);
      break;
    case M32R_OPERAND_SIMM16 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX), 16, 16, CGEN_FIELDS_BITSIZE (fields), & fields->f_simm16);
      break;
    case M32R_OPERAND_UIMM4 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_uimm4);
      break;
    case M32R_OPERAND_UIMM5 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 11, 5, CGEN_FIELDS_BITSIZE (fields), & fields->f_uimm5);
      break;
    case M32R_OPERAND_UIMM16 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 16, 16, CGEN_FIELDS_BITSIZE (fields), & fields->f_uimm16);
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 15, 1, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) + (1));
        fields->f_imm1 = value;
      }
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 4, 2, CGEN_FIELDS_BITSIZE (fields), & fields->f_accd);
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 2, CGEN_FIELDS_BITSIZE (fields), & fields->f_accs);
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 1, CGEN_FIELDS_BITSIZE (fields), & fields->f_acc);
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      length = extract_normal (od, ex_info, insn_value, 0, 0, 0, CGEN_FIELDS_BITSIZE (fields), & fields->f_nil);
      break;
    case M32R_OPERAND_HI16 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), 16, 16, CGEN_FIELDS_BITSIZE (fields), & fields->f_hi16);
      break;
    case M32R_OPERAND_SLO16 :
      length = extract_normal (od, ex_info, insn_value, 0, 16, 16, CGEN_FIELDS_BITSIZE (fields), & fields->f_simm16);
      break;
    case M32R_OPERAND_ULO16 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 16, 16, CGEN_FIELDS_BITSIZE (fields), & fields->f_uimm16);
      break;
    case M32R_OPERAND_UIMM24 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_ABS_ADDR)|(1<<CGEN_OPERAND_UNSIGNED), 8, 24, CGEN_FIELDS_BITSIZE (fields), & fields->f_uimm24);
      break;
    case M32R_OPERAND_DISP8 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((((value) << (2))) + (((pc) & (-4))));
        fields->f_disp8 = value;
      }
      break;
    case M32R_OPERAND_DISP16 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), 16, 16, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((((value) << (2))) + (pc));
        fields->f_disp16 = value;
      }
      break;
    case M32R_OPERAND_DISP24 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), 8, 24, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((((value) << (2))) + (pc));
        fields->f_disp24 = value;
      }
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while decoding insn.\n"),
	       opindex);
      abort ();
    }

  return length;
}

/* Main entry point for printing operands.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `print_insn_normal', but keeping it
   separate makes clear the interface between `print_insn_normal' and each of
   the handlers.
*/

void
m32r_cgen_print_operand (od, opindex, info, fields, attrs, pc, length)
     CGEN_OPCODE_DESC od;
     int opindex;
     disassemble_info * info;
     CGEN_FIELDS * fields;
     void const * attrs;
     bfd_vma pc;
     int length;
{
  switch (opindex)
    {
    case M32R_OPERAND_SR :
      print_keyword (od, info, & m32r_cgen_opval_h_gr, fields->f_r2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_DR :
      print_keyword (od, info, & m32r_cgen_opval_h_gr, fields->f_r1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_SRC1 :
      print_keyword (od, info, & m32r_cgen_opval_h_gr, fields->f_r1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_SRC2 :
      print_keyword (od, info, & m32r_cgen_opval_h_gr, fields->f_r2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_SCR :
      print_keyword (od, info, & m32r_cgen_opval_h_cr, fields->f_r2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_DCR :
      print_keyword (od, info, & m32r_cgen_opval_h_cr, fields->f_r1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case M32R_OPERAND_SIMM8 :
      print_normal (od, info, fields->f_simm8, 0|(1<<CGEN_OPERAND_HASH_PREFIX), pc, length);
      break;
    case M32R_OPERAND_SIMM16 :
      print_normal (od, info, fields->f_simm16, 0|(1<<CGEN_OPERAND_HASH_PREFIX), pc, length);
      break;
    case M32R_OPERAND_UIMM4 :
      print_normal (od, info, fields->f_uimm4, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case M32R_OPERAND_UIMM5 :
      print_normal (od, info, fields->f_uimm5, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case M32R_OPERAND_UIMM16 :
      print_normal (od, info, fields->f_uimm16, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
/* start-sanitize-m32rx */
    case M32R_OPERAND_IMM1 :
      print_normal (od, info, fields->f_imm1, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCD :
      print_keyword (od, info, & m32r_cgen_opval_h_accums, fields->f_accd, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACCS :
      print_keyword (od, info, & m32r_cgen_opval_h_accums, fields->f_accs, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
    case M32R_OPERAND_ACC :
      print_keyword (od, info, & m32r_cgen_opval_h_accums, fields->f_acc, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
/* end-sanitize-m32rx */
    case M32R_OPERAND_HASH :
      print_hash (od, info, fields->f_nil, 0, pc, length);
      break;
    case M32R_OPERAND_HI16 :
      print_normal (od, info, fields->f_hi16, 0|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case M32R_OPERAND_SLO16 :
      print_normal (od, info, fields->f_simm16, 0, pc, length);
      break;
    case M32R_OPERAND_ULO16 :
      print_normal (od, info, fields->f_uimm16, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case M32R_OPERAND_UIMM24 :
      print_address (od, info, fields->f_uimm24, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_ABS_ADDR)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case M32R_OPERAND_DISP8 :
      print_address (od, info, fields->f_disp8, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), pc, length);
      break;
    case M32R_OPERAND_DISP16 :
      print_address (od, info, fields->f_disp16, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), pc, length);
      break;
    case M32R_OPERAND_DISP24 :
      print_address (od, info, fields->f_disp24, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), pc, length);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while printing insn.\n"),
	       opindex);
    abort ();
  }
}

cgen_extract_fn * const m32r_cgen_extract_handlers[] = 
{
  0, /* default */
  extract_insn_normal,
};

cgen_print_fn * const m32r_cgen_print_handlers[] = 
{
  0, /* default */
  print_insn_normal,
};


void
m32r_cgen_init_dis (od)
     CGEN_OPCODE_DESC od;
{
}


#if ! CGEN_INT_INSN_P

/* Subroutine of extract_normal.  */

static INLINE long
extract_1 (od, ex_info, start, length, word_length, bufp)
     CGEN_OPCODE_DESC od;
     CGEN_EXTRACT_INFO *info;
     int start,length,word_length;
     unsigned char *bufp;
{
  unsigned long x,mask;
  int shift;
  int big_p = CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG;

  /* FIXME: Need to use ex_info to ensure bytes have been fetched.  */

  switch (word_length)
    {
    case 8:
      x = *bufp;
      break;
    case 16:
      if (big_p)
	x = bfd_getb16 (bufp);
      else
	x = bfd_getl16 (bufp);
      break;
    case 24:
      /* ??? This may need reworking as these cases don't necessarily
	 want the first byte and the last two bytes handled like this.  */
      if (big_p)
	x = (bfd_getb8 (bufp) << 16) | bfd_getb16 (bufp + 1);
      else
	x = bfd_getl16 (bufp) | (bfd_getb8 (bufp + 2) << 16);
      break;
    case 32:
      if (big_p)
	x = bfd_getb32 (bufp);
      else
	x = bfd_getl32 (bufp);
      break;
    default :
      abort ();
    }

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;
  if (CGEN_INSN_LSB0_P)
    shift = start;
  else
    shift = (word_length - (start + length));
  return (x >> shift) & mask;
}

#endif /* ! CGEN_INT_INSN_P */

/* Default extraction routine.

   ATTRS is a mask of the boolean attributes.  We only need `unsigned',
   but for generality we take a bitmask of all of them.  */

/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static int
extract_normal (od, ex_info, insn_value, attrs, start, length, total_length, valuep)
     CGEN_OPCODE_DESC od;
     CGEN_EXTRACT_INFO *ex_info;
     CGEN_INSN_BYTES insn_value;
     unsigned int attrs;
     int start, length, total_length;
     long *valuep;
{
  unsigned long value;

  /* If LENGTH is zero, this operand doesn't contribute to the value
     so give it a standard value of zero.  */
  if (length == 0)
    {
      *valuep = 0;
      return 1;
    }

#if CGEN_INT_INSN_P

  {
    /* Written this way to avoid undefined behaviour.  */
    unsigned long mask = (((1L << (length - 1)) - 1) << 1) | 1;

    if (CGEN_INSN_LSB0_P)
      value = insn_value >> start;
    else
      value = insn_value >> (total_length - (start + length));
    value &= mask;
    /* sign extend? */
    if (! (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_UNSIGNED))
	&& (value & (1L << (length - 1))))
      value |= ~mask;
  }

#else

  /* The hard case is probably too slow for the normal cases.
     It's certainly more difficult to understand than the normal case.
     Thus this is split into two.  Keep it that way.  The hard case is defined
     to be when a field straddles a (loosely defined) word boundary
     (??? which may require target specific help to determine).  */

#if 0 /*wip*/

#define HARD_CASE_P 0 /* FIXME:wip */

  if (HARD_CASE_P)
    {
    }
#endif
  else
    {
      unsigned char *bufp = (unsigned char *) insn_value;

      if (length > 32)
	abort ();

      /* Adjust start,total_length,bufp to point to the pseudo-word that holds
	 the value.  For example in a 48 bit insn where the value to insert
	 (say an immediate value) is the last 16 bits then word_length here
	 would be 16.  To handle a 24 bit insn with an 18 bit immediate,
	 extract_1 handles 24 bits (using a combination of bfd_get8,16).  */

      if (total_length > 32)
	{
	  int needed_width = start % 8 + length;
	  int fetch_length = (needed_width <= 8 ? 8
			      : needed_width <= 16 ? 16
			      : 32);

	  if (CGEN_INSN_LSB0_P)
	    {
	      if (CGEN_INSN_WORD_ENDIAN (od) == CGEN_ENDIAN_BIG)
		{
		  abort (); /* wip */
		}
	      else
		{
		  int offset = start & ~7;

		  bufp += offset / 8;
		  start -= offset;
		  total_length -= offset;
		}
	    }
	  else
	    {
	      if (CGEN_INSN_WORD_ENDIAN (od) == CGEN_ENDIAN_BIG)
		{
		  int offset = start & ~7;

		  bufp += offset / 8;
		  start -= offset;
		  total_length -= offset;
		}
	      else
		{
		  abort (); /* wip */
		}
	    }
	}

      /* FIXME: which bytes are being extracted have been lost.  */
      value = extract_1 (od, ex_info, start, length, total_length, bufp);
    }

#endif /* ! CGEN_INT_INSN_P */

  *valuep = value;

  /* FIXME: for now */
  return 1;
}

/* Default print handler.  */

static void
print_normal (od, dis_info, value, attrs, pc, length)
     CGEN_OPCODE_DESC od;
     PTR dis_info;
     long value;
     unsigned int attrs;
     bfd_vma pc;
     int length;
{
  disassemble_info *info = (disassemble_info *) dis_info;

#ifdef CGEN_PRINT_NORMAL
  CGEN_PRINT_NORMAL (od, info, value, attrs, pc, length);
#endif

  /* Print the operand as directed by the attributes.  */
  if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_SEM_ONLY))
    ; /* nothing to do */
  else if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_UNSIGNED))
    (*info->fprintf_func) (info->stream, "0x%lx", value);
  else
    (*info->fprintf_func) (info->stream, "%ld", value);
}

/* Default address handler.  */

static void
print_address (od, dis_info, value, attrs, pc, length)
     CGEN_OPCODE_DESC od;
     PTR dis_info;
     bfd_vma value;
     unsigned int attrs;
     bfd_vma pc;
     int length;
{
  disassemble_info *info = (disassemble_info *) dis_info;

#ifdef CGEN_PRINT_ADDRESS
  CGEN_PRINT_ADDRESS (od, info, value, attrs, pc, length);
#endif

  /* Print the operand as directed by the attributes.  */
  if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_SEM_ONLY))
    ; /* nothing to do */
  else if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_PCREL_ADDR))
    (*info->print_address_func) (value, info);
  else if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_ABS_ADDR))
    (*info->print_address_func) (value, info);
  else if (CGEN_BOOL_ATTR (attrs, CGEN_OPERAND_UNSIGNED))
    (*info->fprintf_func) (info->stream, "0x%lx", (long) value);
  else
    (*info->fprintf_func) (info->stream, "%ld", (long) value);
}

/* Keyword print handler.  */

static void
print_keyword (od, dis_info, keyword_table, value, attrs)
     CGEN_OPCODE_DESC od;
     PTR dis_info;
     CGEN_KEYWORD *keyword_table;
     long value;
     unsigned int attrs;
{
  disassemble_info *info = (disassemble_info *) dis_info;
  const CGEN_KEYWORD_ENTRY *ke;

  ke = cgen_keyword_lookup_value (keyword_table, value);
  if (ke != NULL)
    (*info->fprintf_func) (info->stream, "%s", ke->name);
  else
    (*info->fprintf_func) (info->stream, "???");
}

/* Default insn extractor.

   INSN_VALUE is the first CGEN_BASE_INSN_SIZE bytes, translated to host order.
   The extracted fields are stored in FIELDS.
   EX_INFO is used to handle reading variable length insns.
   Return the length of the insn in bits, or 0 if no match,
   or -1 if an error occurs fetching data (memory_error_func will have
   been called).  */

static int
extract_insn_normal (od, insn, ex_info, insn_value, fields, pc)
     CGEN_OPCODE_DESC od;
     const CGEN_INSN *insn;
     CGEN_EXTRACT_INFO *ex_info;
     unsigned long insn_value;
     CGEN_FIELDS *fields;
     bfd_vma pc;
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  const unsigned char *syn;

  CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

  CGEN_INIT_EXTRACT (od);

  for (syn = CGEN_SYNTAX_STRING (syntax); *syn; ++syn)
    {
      int length;

      if (CGEN_SYNTAX_CHAR_P (*syn))
	continue;

      length = m32r_cgen_extract_operand (od, CGEN_SYNTAX_FIELD (*syn),
					    ex_info, insn_value, fields, pc);
      if (length <= 0)
	return length;
    }

  /* We recognized and successfully extracted this insn.  */
  return CGEN_INSN_BITSIZE (insn);
}

/* Default insn printer.

   DIS_INFO is defined as `PTR' so the disassembler needn't know anything
   about disassemble_info.  */

static void
print_insn_normal (od, dis_info, insn, fields, pc, length)
     CGEN_OPCODE_DESC od;
     PTR dis_info;
     const CGEN_INSN *insn;
     CGEN_FIELDS *fields;
     bfd_vma pc;
     int length;
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  disassemble_info *info = (disassemble_info *) dis_info;
  const unsigned char *syn;

  CGEN_INIT_PRINT (od);

  for (syn = CGEN_SYNTAX_STRING (syntax); *syn; ++syn)
    {
      if (CGEN_SYNTAX_MNEMONIC_P (*syn))
	{
	  (*info->fprintf_func) (info->stream, "%s", CGEN_INSN_MNEMONIC (insn));
	  continue;
	}
      if (CGEN_SYNTAX_CHAR_P (*syn))
	{
	  (*info->fprintf_func) (info->stream, "%c", CGEN_SYNTAX_CHAR (*syn));
	  continue;
	}

      /* We have an operand.  */
      m32r_cgen_print_operand (od, CGEN_SYNTAX_FIELD (*syn), info,
				 fields, CGEN_INSN_ATTRS (insn), pc, length);
    }
}

/* Utility to print an insn.
   BUF is the base part of the insn, target byte order, BUFLEN bytes long.
   The result is the size of the insn in bytes or zero for an unknown insn
   or -1 if an error occurs fetching data (memory_error_func will have
   been called).  */

static int
print_insn (od, pc, info, buf, buflen)
     CGEN_OPCODE_DESC od;
     bfd_vma pc;
     disassemble_info *info;
     char *buf;
     int buflen;
{
  unsigned long insn_value;
  const CGEN_INSN_LIST *insn_list;
  CGEN_EXTRACT_INFO ex_info;

  ex_info.dis_info = info;
  ex_info.valid = (1 << CGEN_BASE_INSN_SIZE) - 1;
  ex_info.bytes = buf;

  switch (buflen)
    {
    case 1:
      insn_value = buf[0];
      break;
    case 2:
      insn_value = info->endian == BFD_ENDIAN_BIG ? bfd_getb16 (buf) : bfd_getl16 (buf);
      break;
    case 4:
      insn_value = info->endian == BFD_ENDIAN_BIG ? bfd_getb32 (buf) : bfd_getl32 (buf);
      break;
    default:
      abort ();
    }

  /* The instructions are stored in hash lists.
     Pick the first one and keep trying until we find the right one.  */

  insn_list = CGEN_DIS_LOOKUP_INSN (od, buf, insn_value);
  while (insn_list != NULL)
    {
      const CGEN_INSN *insn = insn_list->insn;
      CGEN_FIELDS fields;
      int length;

#if 0 /* not needed as insn shouldn't be in hash lists if not supported */
      /* Supported by this cpu?  */
      if (! m32r_cgen_insn_supported (od, insn))
	continue;
#endif

      /* Basic bit mask must be correct.  */
      /* ??? May wish to allow target to defer this check until the extract
	 handler.  */
      if ((insn_value & CGEN_INSN_MASK (insn)) == CGEN_INSN_VALUE (insn))
	{
	  /* Printing is handled in two passes.  The first pass parses the
	     machine insn and extracts the fields.  The second pass prints
	     them.  */

	  length = (*CGEN_EXTRACT_FN (insn)) (od, insn, &ex_info, insn_value,
					      &fields, pc);
	  /* length < 0 -> error */
	  if (length < 0)
	    return length;
	  if (length > 0)
	    {
	      (*CGEN_PRINT_FN (insn)) (od, info, insn, &fields, pc, length);
	      /* length is in bits, result is in bytes */
	      return length / 8;
	    }
	}

      insn_list = CGEN_DIS_NEXT_INSN (insn_list);
    }

  return 0;
}

/* Default value for CGEN_PRINT_INSN.
   The result is the size of the insn in bytes or zero for an unknown insn
   or -1 if an error occured fetching bytes.  */

#ifndef CGEN_PRINT_INSN
#define CGEN_PRINT_INSN default_print_insn
#endif

static int
default_print_insn (od, pc, info)
     CGEN_OPCODE_DESC od;
     bfd_vma pc;
     disassemble_info *info;
{
  char buf[CGEN_MAX_INSN_SIZE];
  int status;

  /* Read the base part of the insn.  */

  status = (*info->read_memory_func) (pc, buf, CGEN_BASE_INSN_SIZE, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }

  return print_insn (od, pc, info, buf, CGEN_BASE_INSN_SIZE);
}

/* Main entry point.
   Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction (in bytes).  */

int
print_insn_m32r (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  int length;
  static CGEN_OPCODE_DESC od = 0;
  int mach = info->mach;
  int big_p = info->endian == BFD_ENDIAN_BIG;

  /* If we haven't initialized yet, initialize the opcode table.  */
  if (! od)
    {
      od = m32r_cgen_opcode_open (mach,
				    big_p ?
				    CGEN_ENDIAN_BIG
				    : CGEN_ENDIAN_LITTLE);
      m32r_cgen_init_dis (od);
    }
  /* If we've switched cpu's, re-initialize.  */
  /* ??? Perhaps we should use BFD_ENDIAN.  */
  else if (mach != CGEN_OPCODE_MACH (od)
	   || (CGEN_OPCODE_ENDIAN (od)
	       != (big_p ? CGEN_ENDIAN_BIG : CGEN_ENDIAN_LITTLE)))
    {
      cgen_set_cpu (od, mach, big_p ? CGEN_ENDIAN_BIG : CGEN_ENDIAN_LITTLE);
    }

  /* We try to have as much common code as possible.
     But at this point some targets need to take over.  */
  /* ??? Some targets may need a hook elsewhere.  Try to avoid this,
     but if not possible try to move this hook elsewhere rather than
     have two hooks.  */
  length = CGEN_PRINT_INSN (od, pc, info);
  if (length > 0)
    return length;
  if (length < 0)
    return -1;

  (*info->fprintf_func) (info->stream, UNKNOWN_INSN_MSG);
  return CGEN_DEFAULT_INSN_SIZE;
}
