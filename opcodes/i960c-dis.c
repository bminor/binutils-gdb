/* Disassembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE i960c-dis.c.

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
#include "i960c-opc.h"
#include "opintl.h"

#undef INLINE
#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

/* Default text to print if an instruction isn't recognized.  */
#define UNKNOWN_INSN_MSG _("*unknown*")

/* Used by the ifield rtx function.  */
#define FLD(f) (fields->f)

static int extract_normal
     PARAMS ((CGEN_OPCODE_DESC, CGEN_EXTRACT_INFO *, CGEN_INSN_INT,
	      unsigned int, unsigned int, unsigned int, unsigned int,
	      unsigned int, unsigned int, bfd_vma, long *));
static void print_normal
     PARAMS ((CGEN_OPCODE_DESC, PTR, long, unsigned int, bfd_vma, int));
static void print_address
     PARAMS ((CGEN_OPCODE_DESC, PTR, bfd_vma, unsigned int, bfd_vma, int));
static void print_keyword
     PARAMS ((CGEN_OPCODE_DESC, PTR, CGEN_KEYWORD *, long, unsigned int));
static int extract_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *, CGEN_EXTRACT_INFO *,
	      CGEN_INSN_INT, CGEN_FIELDS *, bfd_vma));
static void print_insn_normal
     PARAMS ((CGEN_OPCODE_DESC, PTR, const CGEN_INSN *, CGEN_FIELDS *,
	      bfd_vma, int));
static int print_insn PARAMS ((CGEN_OPCODE_DESC, bfd_vma,
			       disassemble_info *, char *, int));
static int default_print_insn
     PARAMS ((CGEN_OPCODE_DESC, bfd_vma, disassemble_info *));

/* -- disassembler routines inserted here */

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
i960_cgen_extract_operand (od, opindex, ex_info, insn_value, fields, pc)
     CGEN_OPCODE_DESC od;
     int opindex;
     CGEN_EXTRACT_INFO *ex_info;
     CGEN_INSN_INT insn_value;
     CGEN_FIELDS * fields;
     bfd_vma pc;
{
  int length;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case I960_OPERAND_SRC1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 27, 5, 32, total_length, pc, & fields->f_src1);
      break;
    case I960_OPERAND_SRC2 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 13, 5, 32, total_length, pc, & fields->f_src2);
      break;
    case I960_OPERAND_DST :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 5, 32, total_length, pc, & fields->f_srcdst);
      break;
    case I960_OPERAND_LIT1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 27, 5, 32, total_length, pc, & fields->f_src1);
      break;
    case I960_OPERAND_LIT2 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 13, 5, 32, total_length, pc, & fields->f_src2);
      break;
    case I960_OPERAND_ST_SRC :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 5, 32, total_length, pc, & fields->f_srcdst);
      break;
    case I960_OPERAND_ABASE :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 13, 5, 32, total_length, pc, & fields->f_abase);
      break;
    case I960_OPERAND_OFFSET :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 20, 12, 32, total_length, pc, & fields->f_offset);
      break;
    case I960_OPERAND_SCALE :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 22, 3, 32, total_length, pc, & fields->f_scale);
      break;
    case I960_OPERAND_INDEX :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 27, 5, 32, total_length, pc, & fields->f_index);
      break;
    case I960_OPERAND_OPTDISP :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 32, 0, 32, 32, total_length, pc, & fields->f_optdisp);
      break;
    case I960_OPERAND_BR_SRC1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 5, 32, total_length, pc, & fields->f_br_src1);
      break;
    case I960_OPERAND_BR_SRC2 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 13, 5, 32, total_length, pc, & fields->f_br_src2);
      break;
    case I960_OPERAND_BR_DISP :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), 0, 19, 11, 32, total_length, pc, & value);
        value = ((((value) << (2))) + (pc));
        fields->f_br_disp = value;
      }
      break;
    case I960_OPERAND_BR_LIT1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 0, 8, 5, 32, total_length, pc, & fields->f_br_src1);
      break;
    case I960_OPERAND_CTRL_DISP :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), 0, 8, 22, 32, total_length, pc, & value);
        value = ((((value) << (2))) + (pc));
        fields->f_ctrl_disp = value;
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
i960_cgen_print_operand (od, opindex, info, fields, attrs, pc, length)
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
    case I960_OPERAND_SRC1 :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_src1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_SRC2 :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_src2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_DST :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_srcdst, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_LIT1 :
      print_normal (od, info, fields->f_src1, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_LIT2 :
      print_normal (od, info, fields->f_src2, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_ST_SRC :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_srcdst, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_ABASE :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_abase, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_OFFSET :
      print_normal (od, info, fields->f_offset, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_SCALE :
      print_normal (od, info, fields->f_scale, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_INDEX :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_index, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_OPTDISP :
      print_normal (od, info, fields->f_optdisp, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_BR_SRC1 :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_br_src1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_BR_SRC2 :
      print_keyword (od, info, & i960_cgen_opval_h_gr, fields->f_br_src2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case I960_OPERAND_BR_DISP :
      print_address (od, info, fields->f_br_disp, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), pc, length);
      break;
    case I960_OPERAND_BR_LIT1 :
      print_normal (od, info, fields->f_br_src1, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case I960_OPERAND_CTRL_DISP :
      print_address (od, info, fields->f_ctrl_disp, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), pc, length);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while printing insn.\n"),
	       opindex);
    abort ();
  }
}

cgen_extract_fn * const i960_cgen_extract_handlers[] = 
{
  0, /* default */
  extract_insn_normal,
};

cgen_print_fn * const i960_cgen_print_handlers[] = 
{
  0, /* default */
  print_insn_normal,
};


void
i960_cgen_init_dis (od)
     CGEN_OPCODE_DESC od;
{
}


#if ! CGEN_INT_INSN_P

/* Subroutine of extract_normal.
   Ensure sufficient bytes are cached in EX_INFO.
   OFFSET is the offset in bytes from the start of the insn of the value.
   BYTES is the length of the needed value.
   Returns 1 for success, 0 for failure.  */

static INLINE int
fill_cache (od, ex_info, offset, bytes, pc)
     CGEN_OPCODE_DESC od;
     CGEN_EXTRACT_INFO *ex_info;
     int offset, bytes;
     bfd_vma pc;
{
  /* It's doubtful that the middle part has already been fetched so
     we don't optimize that case.  kiss.  */
  int mask;
  disassemble_info *info = (disassemble_info *) ex_info->dis_info;

  /* First do a quick check.  */
  mask = (1 << bytes) - 1;
  if (((ex_info->valid >> offset) & mask) == mask)
    return 1;

  /* Search for the first byte we need to read.  */
  for (mask = 1 << offset; bytes > 0; --bytes, ++offset, mask <<= 1)
    if (! (mask & ex_info->valid))
      break;

  if (bytes)
    {
      int status;

      pc += offset;
      status = (*info->read_memory_func)
	(pc, ex_info->insn_bytes + offset, bytes, info);

      if (status != 0)
	{
	  (*info->memory_error_func) (status, pc, info);
	  return 0;
	}

      ex_info->valid |= ((1 << bytes) - 1) << offset;
    }

  return 1;
}

/* Subroutine of extract_normal.  */

static INLINE long
extract_1 (od, ex_info, start, length, word_length, bufp, pc)
     CGEN_OPCODE_DESC od;
     CGEN_EXTRACT_INFO *ex_info;
     int start,length,word_length;
     unsigned char *bufp;
     bfd_vma pc;
{
  unsigned long x,mask;
  int shift;
  int big_p = CGEN_OPCODE_INSN_ENDIAN (od) == CGEN_ENDIAN_BIG;

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
	x = (bufp[0] << 16) | bfd_getb16 (bufp + 1);
      else
	x = bfd_getl16 (bufp) | (bufp[2] << 16);
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
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  return (x >> shift) & mask;
}

#endif /* ! CGEN_INT_INSN_P */

/* Default extraction routine.

   INSN_VALUE is the first CGEN_BASE_INSN_SIZE bits of the insn in host order,
   or sometimes less for cases like the m32r where the base insn size is 32
   but some insns are 16 bits.
   ATTRS is a mask of the boolean attributes.  We only need `UNSIGNED',
   but for generality we take a bitmask of all of them.
   WORD_OFFSET is the offset in bits from the start of the insn of the value.
   WORD_LENGTH is the length of the word in bits in which the value resides.
   START is the starting bit number in the word, architecture origin.
   LENGTH is the length of VALUE in bits.
   TOTAL_LENGTH is the total length of the insn in bits.

   Returns 1 for success, 0 for failure.  */

/* ??? The return code isn't properly used.  wip.  */

/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static int
extract_normal (od, ex_info, insn_value, attrs, word_offset, start, length,
		word_length, total_length, pc, valuep)
     CGEN_OPCODE_DESC od;
     CGEN_EXTRACT_INFO *ex_info;
     CGEN_INSN_INT insn_value;
     unsigned int attrs;
     unsigned int word_offset, start, length, word_length, total_length;
     bfd_vma pc;
     long *valuep;
{
  CGEN_INSN_INT value;

  /* If LENGTH is zero, this operand doesn't contribute to the value
     so give it a standard value of zero.  */
  if (length == 0)
    {
      *valuep = 0;
      return 1;
    }

  if (CGEN_INT_INSN_P
      && word_offset != 0)
    abort ();

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the insn-base-bitsize,
     word_length may be too big.  */
#if CGEN_MIN_INSN_BITSIZE < CGEN_BASE_INSN_BITSIZE
  if (word_offset == 0
      && word_length > total_length)
    word_length = total_length;
#endif

  /* Does the value reside in INSN_VALUE?  */

  if (word_offset == 0)
    {
      /* Written this way to avoid undefined behaviour.  */
      CGEN_INSN_INT mask = (((1L << (length - 1)) - 1) << 1) | 1;

      if (CGEN_INSN_LSB0_P)
	value = insn_value >> ((start + 1) - length);
      else
	value = insn_value >> (word_length - (start + length));
      value &= mask;
      /* sign extend? */
      if (! (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_UNSIGNED))
	  && (value & (1L << (length - 1))))
	value |= ~mask;
    }

#if ! CGEN_INT_INSN_P

  else
    {
      unsigned char *bufp = ex_info->insn_bytes + word_offset / 8;

      if (word_length > 32)
	abort ();

      if (fill_cache (od, ex_info, word_offset / 8, word_length / 8, pc) == 0)
	return 0;

      value = extract_1 (od, ex_info, start, length, word_length, bufp, pc);
    }

#endif /* ! CGEN_INT_INSN_P */

  *valuep = value;

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
     CGEN_INSN_INT insn_value;
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

      length = i960_cgen_extract_operand (od, CGEN_SYNTAX_FIELD (*syn),
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
      i960_cgen_print_operand (od, CGEN_SYNTAX_FIELD (*syn), info,
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
  ex_info.insn_bytes = buf;

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
      if (! i960_cgen_insn_supported (od, insn))
	continue;
#endif

      /* Basic bit mask must be correct.  */
      /* ??? May wish to allow target to defer this check until the extract
	 handler.  */
      if ((insn_value & CGEN_INSN_BASE_MASK (insn))
	  == CGEN_INSN_BASE_VALUE (insn))
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
print_insn_i960 (pc, info)
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
      od = i960_cgen_opcode_open (mach,
				    big_p ?
				    CGEN_ENDIAN_BIG
				    : CGEN_ENDIAN_LITTLE);
      i960_cgen_init_dis (od);
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
