/* Disassembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

THIS FILE IS USED TO GENERATE fr30-dis.c.

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
#include "fr30-opc.h"
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
fr30_cgen_extract_operand (od, opindex, ex_info, insn_value, fields, pc)
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
    case FR30_OPERAND_RI :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_Ri);
      break;
    case FR30_OPERAND_RJ :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_Rj);
      break;
    case FR30_OPERAND_RS1 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_Rs1);
      break;
    case FR30_OPERAND_RS2 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 12, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_Rs2);
      break;
    case FR30_OPERAND_U4 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 8, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_u4);
      break;
    case FR30_OPERAND_M4 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 8, 4, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) | ((! (15))));
        fields->f_m4 = value;
      }
      break;
    case FR30_OPERAND_I8 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 4, 8, CGEN_FIELDS_BITSIZE (fields), & fields->f_i8);
      break;
    case FR30_OPERAND_U8 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & fields->f_u8);
      break;
    case FR30_OPERAND_O8 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 4, 8, CGEN_FIELDS_BITSIZE (fields), & fields->f_o8);
      break;
    case FR30_OPERAND_S10 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (2));
        fields->f_s10 = value;
      }
      break;
    case FR30_OPERAND_U10 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (2));
        fields->f_u10 = value;
      }
      break;
    case FR30_OPERAND_DIR8 :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & fields->f_dir8);
      break;
    case FR30_OPERAND_DIR9 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (1));
        fields->f_dir9 = value;
      }
      break;
    case FR30_OPERAND_DIR10 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (2));
        fields->f_dir10 = value;
      }
      break;
    case FR30_OPERAND_LABEL9 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_SIGNED), 8, 8, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (1));
        fields->f_rel8 = value;
      }
      break;
    case FR30_OPERAND_LABEL12 :
      {
        long value;
        length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_SIGNED), 5, 11, CGEN_FIELDS_BITSIZE (fields), & value);
        value = ((value) << (1));
        fields->f_rel11 = value;
      }
      break;
    case FR30_OPERAND_CC :
      length = extract_normal (od, ex_info, insn_value, 0|(1<<CGEN_OPERAND_UNSIGNED), 4, 4, CGEN_FIELDS_BITSIZE (fields), & fields->f_cc);
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
fr30_cgen_print_operand (od, opindex, info, fields, attrs, pc, length)
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
    case FR30_OPERAND_RI :
      print_keyword (od, info, & fr30_cgen_opval_h_gr, fields->f_Ri, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case FR30_OPERAND_RJ :
      print_keyword (od, info, & fr30_cgen_opval_h_gr, fields->f_Rj, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case FR30_OPERAND_RS1 :
      print_keyword (od, info, & fr30_cgen_opval_h_dr, fields->f_Rs1, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case FR30_OPERAND_RS2 :
      print_keyword (od, info, & fr30_cgen_opval_h_dr, fields->f_Rs2, 0|(1<<CGEN_OPERAND_UNSIGNED));
      break;
    case FR30_OPERAND_U4 :
      print_normal (od, info, fields->f_u4, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_M4 :
      print_normal (od, info, fields->f_m4, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_I8 :
      print_normal (od, info, fields->f_i8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_U8 :
      print_normal (od, info, fields->f_u8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_O8 :
      print_normal (od, info, fields->f_o8, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_S10 :
      print_normal (od, info, fields->f_s10, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_SIGNED), pc, length);
      break;
    case FR30_OPERAND_U10 :
      print_normal (od, info, fields->f_u10, 0|(1<<CGEN_OPERAND_HASH_PREFIX)|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_DIR8 :
      print_normal (od, info, fields->f_dir8, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_DIR9 :
      print_normal (od, info, fields->f_dir9, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_DIR10 :
      print_normal (od, info, fields->f_dir10, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;
    case FR30_OPERAND_LABEL9 :
      print_normal (od, info, fields->f_rel8, 0|(1<<CGEN_OPERAND_SIGNED), pc, length);
      break;
    case FR30_OPERAND_LABEL12 :
      print_normal (od, info, fields->f_rel11, 0|(1<<CGEN_OPERAND_SIGNED), pc, length);
      break;
    case FR30_OPERAND_CC :
      print_normal (od, info, fields->f_cc, 0|(1<<CGEN_OPERAND_UNSIGNED), pc, length);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while printing insn.\n"),
	       opindex);
    abort ();
  }
}

cgen_extract_fn * const fr30_cgen_extract_handlers[] = 
{
  0, /* default */
  extract_insn_normal,
};

cgen_print_fn * const fr30_cgen_print_handlers[] = 
{
  0, /* default */
  print_insn_normal,
};


void
fr30_cgen_init_dis (od)
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

      length = fr30_cgen_extract_operand (od, CGEN_SYNTAX_FIELD (*syn),
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
      fr30_cgen_print_operand (od, CGEN_SYNTAX_FIELD (*syn), info,
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
      if (! fr30_cgen_insn_supported (od, insn))
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
print_insn_fr30 (pc, info)
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
      od = fr30_cgen_opcode_open (mach,
				    big_p ?
				    CGEN_ENDIAN_BIG
				    : CGEN_ENDIAN_LITTLE);
      fr30_cgen_init_dis (od);
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
