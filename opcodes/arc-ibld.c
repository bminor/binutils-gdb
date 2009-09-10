/* Instruction building/extraction support for arc. -*- C -*-

   THIS FILE IS MACHINE GENERATED WITH CGEN: Cpu tools GENerator.
   - the resultant file is machine generated, cgen-ibld.in isn't

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2005, 2006
   Free Software Foundation, Inc.

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
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

/* ??? Eventually more and more of this stuff can go to cpu-independent files.
   Keep that in mind.  */

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "dis-asm.h"
#include "bfd.h"
#include "symcat.h"
#include "arc-desc.h"
#include "arc-opc.h"
#include "opintl.h"
#include "safe-ctype.h"

#undef  min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef  max
#define max(a,b) ((a) > (b) ? (a) : (b))

/* Used by the ifield rtx function.  */
#define FLD(f) (fields->f)

static const char * insert_normal
  (CGEN_CPU_DESC, long, unsigned int, unsigned int, unsigned int,
   unsigned int, unsigned int, unsigned int, CGEN_INSN_BYTES_PTR);
static const char * insert_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *,
   CGEN_FIELDS *, CGEN_INSN_BYTES_PTR, bfd_vma);
static int extract_normal
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *, CGEN_INSN_INT,
   unsigned int, unsigned int, unsigned int, unsigned int,
   unsigned int, unsigned int, bfd_vma, long *);
static int extract_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *, CGEN_EXTRACT_INFO *,
   CGEN_INSN_INT, CGEN_FIELDS *, bfd_vma);
#if CGEN_INT_INSN_P
static void put_insn_int_value
  (CGEN_CPU_DESC, CGEN_INSN_BYTES_PTR, int, int, CGEN_INSN_INT);
#endif
#if ! CGEN_INT_INSN_P
static CGEN_INLINE void insert_1
  (CGEN_CPU_DESC, unsigned long, int, int, int, unsigned char *);
static CGEN_INLINE int fill_cache
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *,  int, int, bfd_vma);
static CGEN_INLINE long extract_1
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *, int, int, int, unsigned char *, bfd_vma);
#endif

/* Operand insertion.  */

#if ! CGEN_INT_INSN_P

/* Subroutine of insert_normal.  */

static CGEN_INLINE void
insert_1 (CGEN_CPU_DESC cd,
	  unsigned long value,
	  int start,
	  int length,
	  int word_length,
	  unsigned char *bufp)
{
  unsigned long x,mask;
  int shift;

  x = cgen_get_insn_value (cd, bufp, word_length);

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;
  if (CGEN_INSN_LSB0_P)
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  x = (x & ~(mask << shift)) | ((value & mask) << shift);

  cgen_put_insn_value (cd, bufp, word_length, (bfd_vma) x);
}

#endif /* ! CGEN_INT_INSN_P */

/* Default insertion routine.

   ATTRS is a mask of the boolean attributes.
   WORD_OFFSET is the offset in bits from the start of the insn of the value.
   WORD_LENGTH is the length of the word in bits in which the value resides.
   START is the starting bit number in the word, architecture origin.
   LENGTH is the length of VALUE in bits.
   TOTAL_LENGTH is the total length of the insn in bits.

   The result is an error message or NULL if success.  */

/* ??? This duplicates functionality with bfd's howto table and
   bfd_install_relocation.  */
/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static const char *
insert_normal (CGEN_CPU_DESC cd,
	       long value,
	       unsigned int attrs,
	       unsigned int word_offset,
	       unsigned int start,
	       unsigned int length,
	       unsigned int word_length,
	       unsigned int total_length,
	       CGEN_INSN_BYTES_PTR buffer)
{
  static char errbuf[100];
  /* Written this way to avoid undefined behaviour.  */
  unsigned long mask = (((1L << (length - 1)) - 1) << 1) | 1;

  /* If LENGTH is zero, this operand doesn't contribute to the value.  */
  if (length == 0)
    return NULL;

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the base-insn-bitsize,
     word_length may be too big.  */
  if (cd->min_insn_bitsize < cd->base_insn_bitsize)
    {
      if (word_offset == 0
	  && word_length > total_length)
	word_length = total_length;
    }

  /* Ensure VALUE will fit.  */
  if (CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGN_OPT))
    {
      long minval = - (1L << (length - 1));
      unsigned long maxval = mask;
      
      if ((value > 0 && (unsigned long) value > maxval)
	  || value < minval)
	{
	  /* xgettext:c-format */
	  sprintf (errbuf,
		   _("operand out of range (%ld not between %ld and %lu)"),
		   value, minval, maxval);
	  return errbuf;
	}
    }
  else if (! CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGNED))
    {
      unsigned long maxval = mask;
      unsigned long val = (unsigned long) value;

      /* For hosts with a word size > 32 check to see if value has been sign
	 extended beyond 32 bits.  If so then ignore these higher sign bits
	 as the user is attempting to store a 32-bit signed value into an
	 unsigned 32-bit field which is allowed.  */
      if (sizeof (unsigned long) > 4 && ((value >> 32) == -1))
	val &= 0xFFFFFFFF;

      if (val > maxval)
	{
	  /* xgettext:c-format */
	  sprintf (errbuf,
		   _("operand out of range (0x%lx not between 0 and 0x%lx)"),
		   val, maxval);
	  return errbuf;
	}
    }
  else
    {
      if (! cgen_signed_overflow_ok_p (cd))
	{
	  long minval = - (1L << (length - 1));
	  long maxval =   (1L << (length - 1)) - 1;
	  
	  if (value < minval || value > maxval)
	    {
	      sprintf
		/* xgettext:c-format */
		(errbuf, _("operand out of range (%ld not between %ld and %ld)"),
		 value, minval, maxval);
	      return errbuf;
	    }
	}
    }

#if CGEN_INT_INSN_P

  {
    int shift;

    if (CGEN_INSN_LSB0_P)
      shift = (word_offset + start + 1) - length;
    else
      shift = total_length - (word_offset + start + length);
    *buffer = (*buffer & ~(mask << shift)) | ((value & mask) << shift);
  }

#else /* ! CGEN_INT_INSN_P */

  {
    unsigned char *bufp = (unsigned char *) buffer + word_offset / 8;

    insert_1 (cd, value, start, length, word_length, bufp);
  }

#endif /* ! CGEN_INT_INSN_P */

  return NULL;
}

/* Default insn builder (insert handler).
   The instruction is recorded in CGEN_INT_INSN_P byte order (meaning
   that if CGEN_INSN_BYTES_PTR is an int * and thus, the value is
   recorded in host byte order, otherwise BUFFER is an array of bytes
   and the value is recorded in target byte order).
   The result is an error message or NULL if success.  */

static const char *
insert_insn_normal (CGEN_CPU_DESC cd,
		    const CGEN_INSN * insn,
		    CGEN_FIELDS * fields,
		    CGEN_INSN_BYTES_PTR buffer,
		    bfd_vma pc)
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  unsigned long value;
  const CGEN_SYNTAX_CHAR_TYPE * syn;

  CGEN_INIT_INSERT (cd);
  value = CGEN_INSN_BASE_VALUE (insn);

  /* If we're recording insns as numbers (rather than a string of bytes),
     target byte order handling is deferred until later.  */

#if CGEN_INT_INSN_P

  put_insn_int_value (cd, buffer, cd->base_insn_bitsize,
		      CGEN_FIELDS_BITSIZE (fields), value);

#else

  cgen_put_insn_value (cd, buffer, min ((unsigned) cd->base_insn_bitsize,
					(unsigned) CGEN_FIELDS_BITSIZE (fields)),
		       value);

#endif /* ! CGEN_INT_INSN_P */

  /* ??? It would be better to scan the format's fields.
     Still need to be able to insert a value based on the operand though;
     e.g. storing a branch displacement that got resolved later.
     Needs more thought first.  */

  for (syn = CGEN_SYNTAX_STRING (syntax); * syn; ++ syn)
    {
      const char *errmsg;

      if (CGEN_SYNTAX_CHAR_P (* syn))
	continue;

      errmsg = (* cd->insert_operand) (cd, CGEN_SYNTAX_FIELD (*syn),
				       fields, buffer, pc);
      if (errmsg)
	return errmsg;
    }

  return NULL;
}

#if CGEN_INT_INSN_P
/* Cover function to store an insn value into an integral insn.  Must go here
   because it needs <prefix>-desc.h for CGEN_INT_INSN_P.  */

static void
put_insn_int_value (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
		    CGEN_INSN_BYTES_PTR buf,
		    int length,
		    int insn_length,
		    CGEN_INSN_INT value)
{
  /* For architectures with insns smaller than the base-insn-bitsize,
     length may be too big.  */
  if (length > insn_length)
    *buf = value;
  else
    {
      int shift = insn_length - length;
      /* Written this way to avoid undefined behaviour.  */
      CGEN_INSN_INT mask = (((1L << (length - 1)) - 1) << 1) | 1;

      *buf = (*buf & ~(mask << shift)) | ((value & mask) << shift);
    }
}
#endif

/* Operand extraction.  */

#if ! CGEN_INT_INSN_P

/* Subroutine of extract_normal.
   Ensure sufficient bytes are cached in EX_INFO.
   OFFSET is the offset in bytes from the start of the insn of the value.
   BYTES is the length of the needed value.
   Returns 1 for success, 0 for failure.  */

static CGEN_INLINE int
fill_cache (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
	    CGEN_EXTRACT_INFO *ex_info,
	    int offset,
	    int bytes,
	    bfd_vma pc)
{
  /* It's doubtful that the middle part has already been fetched so
     we don't optimize that case.  kiss.  */
  unsigned int mask;
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

static CGEN_INLINE long
extract_1 (CGEN_CPU_DESC cd,
	   CGEN_EXTRACT_INFO *ex_info ATTRIBUTE_UNUSED,
	   int start,
	   int length,
	   int word_length,
	   unsigned char *bufp,
	   bfd_vma pc ATTRIBUTE_UNUSED)
{
  unsigned long x;
  int shift;

  x = cgen_get_insn_value (cd, bufp, word_length);

  if (CGEN_INSN_LSB0_P)
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  return x >> shift;
}

#endif /* ! CGEN_INT_INSN_P */

/* Default extraction routine.

   INSN_VALUE is the first base_insn_bitsize bits of the insn in host order,
   or sometimes less for cases like the m32r where the base insn size is 32
   but some insns are 16 bits.
   ATTRS is a mask of the boolean attributes.  We only need `SIGNED',
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
extract_normal (CGEN_CPU_DESC cd,
#if ! CGEN_INT_INSN_P
		CGEN_EXTRACT_INFO *ex_info,
#else
		CGEN_EXTRACT_INFO *ex_info ATTRIBUTE_UNUSED,
#endif
		CGEN_INSN_INT insn_value,
		unsigned int attrs,
		unsigned int word_offset,
		unsigned int start,
		unsigned int length,
		unsigned int word_length,
		unsigned int total_length,
#if ! CGEN_INT_INSN_P
		bfd_vma pc,
#else
		bfd_vma pc ATTRIBUTE_UNUSED,
#endif
		long *valuep)
{
  long value, mask;

  /* If LENGTH is zero, this operand doesn't contribute to the value
     so give it a standard value of zero.  */
  if (length == 0)
    {
      *valuep = 0;
      return 1;
    }

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the insn-base-bitsize,
     word_length may be too big.  */
  if (cd->min_insn_bitsize < cd->base_insn_bitsize)
    {
      if (word_offset + word_length > total_length)
	word_length = total_length - word_offset;
    }

  /* Does the value reside in INSN_VALUE, and at the right alignment?  */

  if (CGEN_INT_INSN_P || (word_offset == 0 && word_length == total_length))
    {
      if (CGEN_INSN_LSB0_P)
	value = insn_value >> ((word_offset + start + 1) - length);
      else
	value = insn_value >> (total_length - ( word_offset + start + length));
    }

#if ! CGEN_INT_INSN_P

  else
    {
      unsigned char *bufp = ex_info->insn_bytes + word_offset / 8;

      if (word_length > 32)
	abort ();

      if (fill_cache (cd, ex_info, word_offset / 8, word_length / 8, pc) == 0)
	return 0;

      value = extract_1 (cd, ex_info, start, length, word_length, bufp, pc);
    }

#endif /* ! CGEN_INT_INSN_P */

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;

  value &= mask;
  /* sign extend? */
  if (CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGNED)
      && (value & (1L << (length - 1))))
    value |= ~mask;

  *valuep = value;

  return 1;
}

/* Default insn extractor.

   INSN_VALUE is the first base_insn_bitsize bits, translated to host order.
   The extracted fields are stored in FIELDS.
   EX_INFO is used to handle reading variable length insns.
   Return the length of the insn in bits, or 0 if no match,
   or -1 if an error occurs fetching data (memory_error_func will have
   been called).  */

static int
extract_insn_normal (CGEN_CPU_DESC cd,
		     const CGEN_INSN *insn,
		     CGEN_EXTRACT_INFO *ex_info,
		     CGEN_INSN_INT insn_value,
		     CGEN_FIELDS *fields,
		     bfd_vma pc)
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  const CGEN_SYNTAX_CHAR_TYPE *syn;

  CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

  CGEN_INIT_EXTRACT (cd);

  for (syn = CGEN_SYNTAX_STRING (syntax); *syn; ++syn)
    {
      int length;

      if (CGEN_SYNTAX_CHAR_P (*syn))
	continue;

      length = (* cd->extract_operand) (cd, CGEN_SYNTAX_FIELD (*syn),
					ex_info, insn_value, fields, pc);
      if (length <= 0)
	return length;
    }

  /* We recognized and successfully extracted this insn.  */
  return CGEN_INSN_BITSIZE (insn);
}

/* Machine generated code added here.  */

static const char * arc_cgen_insert_operand
  (CGEN_CPU_DESC, int, CGEN_FIELDS *, CGEN_INSN_BYTES_PTR, bfd_vma);

/* Main entry point for operand insertion.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.  It's also needed by GAS to insert operands that couldn't be
   resolved during parsing.  */

static const char *
arc_cgen_insert_operand (CGEN_CPU_DESC       cd,
			 int                 opindex,
			 CGEN_FIELDS *       fields,
			 CGEN_INSN_BYTES_PTR buffer,
			 bfd_vma pc          ATTRIBUTE_UNUSED)
{
  const char * errmsg = NULL;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      errmsg = insert_normal (cd, fields->f_F, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_F :
      errmsg = insert_normal (cd, fields->f_F, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_F0 :
      errmsg = insert_normal (cd, fields->f_F, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_F1 :
      errmsg = insert_normal (cd, fields->f_F, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_F1F :
      errmsg = insert_normal (cd, fields->f_F, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_GP :
      break;
    case ARC_OPERAND_LDODI :
      errmsg = insert_normal (cd, fields->f_LDODi, 0, 0, 20, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_LDRDI :
      errmsg = insert_normal (cd, fields->f_LDRDi, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_NE :
      break;
    case ARC_OPERAND_PCL :
      break;
    case ARC_OPERAND_QCONDB :
      errmsg = insert_normal (cd, fields->f_cond_Q, 0, 0, 27, 5, 32, total_length, buffer);
      break;
    case ARC_OPERAND_QCONDI :
      errmsg = insert_normal (cd, fields->f_cond_Q, 0, 0, 27, 5, 32, total_length, buffer);
      break;
    case ARC_OPERAND_QCONDJ :
      errmsg = insert_normal (cd, fields->f_cond_Q, 0, 0, 27, 5, 32, total_length, buffer);
      break;
    case ARC_OPERAND_R0 :
      break;
    case ARC_OPERAND_R31 :
      break;
    case ARC_OPERAND_RA :
      errmsg = insert_normal (cd, fields->f_op_A, 0, 0, 26, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RA_0 :
      errmsg = insert_normal (cd, fields->f_op_A, 0, 0, 26, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RB :
      {
{
  FLD (f_op__b) = ((FLD (f_op_B)) & (7));
  FLD (f_B_5_3) = ((unsigned int) (FLD (f_op_B)) >> (3));
}
        errmsg = insert_normal (cd, fields->f_op__b, 0, 0, 5, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_B_5_3, 0, 0, 17, 3, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_RB_0 :
      {
{
  FLD (f_op__b) = ((FLD (f_op_B)) & (7));
  FLD (f_B_5_3) = ((unsigned int) (FLD (f_op_B)) >> (3));
}
        errmsg = insert_normal (cd, fields->f_op__b, 0, 0, 5, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_B_5_3, 0, 0, 17, 3, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_RC :
      errmsg = insert_normal (cd, fields->f_op_C, 0, 0, 20, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RC_ILINK :
      errmsg = insert_normal (cd, fields->f_op_Cj, 0, 0, 20, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RC_NOILINK :
      errmsg = insert_normal (cd, fields->f_op_Cj, 0, 0, 20, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_R_A :
      errmsg = insert_normal (cd, fields->f_op__a, 0, 0, 13, 3, 32, total_length, buffer);
      break;
    case ARC_OPERAND_R_B :
      errmsg = insert_normal (cd, fields->f_op__b, 0, 0, 5, 3, 32, total_length, buffer);
      break;
    case ARC_OPERAND_R_C :
      errmsg = insert_normal (cd, fields->f_op__c, 0, 0, 8, 3, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RCC :
      errmsg = insert_normal (cd, fields->f_brcond, 0, 0, 28, 4, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RCCS :
      errmsg = insert_normal (cd, fields->f_brscond, 0, 0, 8, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_RH :
      {
{
  FLD (f_h_2_0) = ((FLD (f_op_h)) & (7));
  FLD (f_h_5_3) = ((unsigned int) (FLD (f_op_h)) >> (3));
}
        errmsg = insert_normal (cd, fields->f_h_2_0, 0, 0, 8, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_h_5_3, 0, 0, 13, 3, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_SP :
      break;
    case ARC_OPERAND_STODI :
      errmsg = insert_normal (cd, fields->f_STODi, 0, 0, 26, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U6 :
      errmsg = insert_normal (cd, fields->f_u6, 0, 0, 20, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U6X2 :
      {
        long value = fields->f_u6x2;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (cd, value, 0, 0, 20, 6, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND__AW :
      break;
    case ARC_OPERAND__L :
      break;
    case ARC_OPERAND__S :
      break;
    case ARC_OPERAND_CBIT :
      break;
    case ARC_OPERAND_DELAY_N :
      errmsg = insert_normal (cd, fields->f_delay_N, 0, 0, 26, 1, 32, total_length, buffer);
      break;
    case ARC_OPERAND_DUMMY_OP :
      errmsg = insert_normal (cd, fields->f_dummy, 0, 0, 16, 16, 32, total_length, buffer);
      break;
    case ARC_OPERAND_I2COND :
      errmsg = insert_normal (cd, fields->f_cond_i2, 0, 0, 5, 2, 32, total_length, buffer);
      break;
    case ARC_OPERAND_I3COND :
      errmsg = insert_normal (cd, fields->f_cond_i3, 0, 0, 7, 3, 32, total_length, buffer);
      break;
    case ARC_OPERAND_LABEL10 :
      {
        long value = fields->f_rel10;
        value = ((int) (((value) - (((pc) & (-4))))) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 7, 9, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_LABEL13A :
      {
        long value = fields->f_rel13bl;
        value = ((int) (((value) - (((pc) & (-4))))) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 5, 11, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_LABEL21 :
      {
{
  FLD (f_d21l) = ((((unsigned int) (((FLD (f_rel21)) - (((pc) & (-4))))) >> (1))) & (1023));
  FLD (f_d21h) = ((unsigned int) (((FLD (f_rel21)) - (((pc) & (-4))))) >> (11));
}
        errmsg = insert_normal (cd, fields->f_d21l, 0, 0, 5, 10, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d21h, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 10, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_LABEL21A :
      {
{
  FLD (f_d21bl) = ((((unsigned int) (((FLD (f_rel21bl)) - (((pc) & (-4))))) >> (2))) & (511));
  FLD (f_d21h) = ((unsigned int) (((FLD (f_rel21bl)) - (((pc) & (-4))))) >> (11));
}
        errmsg = insert_normal (cd, fields->f_d21bl, 0, 0, 5, 9, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d21h, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 10, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_LABEL25 :
      {
{
  FLD (f_d21l) = ((((unsigned int) (((FLD (f_rel25)) - (((pc) & (-4))))) >> (1))) & (1023));
  FLD (f_d25m) = ((unsigned int) (((FLD (f_rel25)) - (((pc) & (-4))))) >> (11));
  FLD (f_d25h) = ((unsigned int) (((FLD (f_rel25)) - (((pc) & (-4))))) >> (21));
}
        errmsg = insert_normal (cd, fields->f_d21l, 0, 0, 5, 10, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d25m, 0, 0, 16, 10, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d25h, 0|(1<<CGEN_IFLD_SIGNED), 0, 28, 4, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_LABEL25A :
      {
{
  FLD (f_d21bl) = ((((unsigned int) (((FLD (f_rel25bl)) - (((pc) & (-4))))) >> (2))) & (511));
  FLD (f_d25m) = ((unsigned int) (((FLD (f_rel25bl)) - (((pc) & (-4))))) >> (11));
  FLD (f_d25h) = ((unsigned int) (((FLD (f_rel25bl)) - (((pc) & (-4))))) >> (21));
}
        errmsg = insert_normal (cd, fields->f_d21bl, 0, 0, 5, 9, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d25m, 0, 0, 16, 10, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d25h, 0|(1<<CGEN_IFLD_SIGNED), 0, 28, 4, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_LABEL7 :
      {
        long value = fields->f_rel7;
        value = ((int) (((value) - (((pc) & (-4))))) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 10, 6, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_LABEL8 :
      {
        long value = fields->f_rel8;
        value = ((int) (((value) - (((pc) & (-4))))) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 9, 7, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_LABEL9 :
      {
{
  FLD (f_d9l) = ((((unsigned int) (((FLD (f_rel9)) - (((pc) & (-4))))) >> (1))) & (127));
  FLD (f_d9h) = ((unsigned int) (((FLD (f_rel9)) - (((pc) & (-4))))) >> (8));
}
        errmsg = insert_normal (cd, fields->f_d9l, 0, 0, 8, 7, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d9h, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 1, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_LBIT :
      break;
    case ARC_OPERAND_NBIT :
      break;
    case ARC_OPERAND_S12 :
      {
{
  FLD (f_u6) = ((FLD (f_s12)) & (63));
  FLD (f_s12h) = ((unsigned int) (FLD (f_s12)) >> (6));
}
        errmsg = insert_normal (cd, fields->f_u6, 0, 0, 20, 6, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s12h, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 6, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_S12X2 :
      {
{
  FLD (f_u6) = ((((unsigned int) (FLD (f_s12x2)) >> (1))) & (63));
  FLD (f_s12h) = ((unsigned int) (FLD (f_s12x2)) >> (7));
}
        errmsg = insert_normal (cd, fields->f_u6, 0, 0, 20, 6, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s12h, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 6, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_S1BIT :
      break;
    case ARC_OPERAND_S2BIT :
      break;
    case ARC_OPERAND_S9 :
      {
{
  FLD (f_u8) = ((FLD (f_s9)) & (255));
  FLD (f_d9h) = ((unsigned int) (FLD (f_s9)) >> (8));
}
        errmsg = insert_normal (cd, fields->f_u8, 0, 0, 8, 8, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d9h, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 1, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case ARC_OPERAND_S9X4 :
      {
        long value = fields->f_s9x4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_SC_S9_ :
      {
        long value = fields->f_s9x4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_SC_S9B :
      errmsg = insert_normal (cd, fields->f_s9x1, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, buffer);
      break;
    case ARC_OPERAND_SC_S9W :
      {
        long value = fields->f_s9x2;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_SC_U5_ :
      {
        long value = fields->f_u5x4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0, 0, 11, 5, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_SC_U5B :
      errmsg = insert_normal (cd, fields->f_u5, 0, 0, 11, 5, 32, total_length, buffer);
      break;
    case ARC_OPERAND_SC_U5W :
      {
        long value = fields->f_u5x2;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (cd, value, 0, 0, 11, 5, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_TRAPNUM :
      errmsg = insert_normal (cd, fields->f_trapnum, 0, 0, 5, 6, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U3 :
      errmsg = insert_normal (cd, fields->f_u3, 0, 0, 13, 3, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U5 :
      errmsg = insert_normal (cd, fields->f_u5, 0, 0, 11, 5, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U5X4 :
      {
        long value = fields->f_u5x4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0, 0, 11, 5, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_U7 :
      errmsg = insert_normal (cd, fields->f_u7, 0, 0, 9, 7, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U8 :
      errmsg = insert_normal (cd, fields->f_u8, 0, 0, 8, 8, 32, total_length, buffer);
      break;
    case ARC_OPERAND_U8X4 :
      {
        long value = fields->f_u8x4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0, 0, 8, 8, 32, total_length, buffer);
      }
      break;
    case ARC_OPERAND_UNCONDB :
      break;
    case ARC_OPERAND_UNCONDI :
      break;
    case ARC_OPERAND_UNCONDJ :
      break;
    case ARC_OPERAND_VBIT :
      break;
    case ARC_OPERAND_ZBIT :
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while building insn.\n"),
	       opindex);
      abort ();
  }

  return errmsg;
}

static int arc_cgen_extract_operand
  (CGEN_CPU_DESC, int, CGEN_EXTRACT_INFO *, CGEN_INSN_INT, CGEN_FIELDS *, bfd_vma);

/* Main entry point for operand extraction.
   The result is <= 0 for error, >0 for success.
   ??? Actual values aren't well defined right now.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `print_insn_normal', but keeping it
   separate makes clear the interface between `print_insn_normal' and each of
   the handlers.  */

static int
arc_cgen_extract_operand (CGEN_CPU_DESC cd,
			     int opindex,
			     CGEN_EXTRACT_INFO *ex_info,
			     CGEN_INSN_INT insn_value,
			     CGEN_FIELDS * fields,
			     bfd_vma pc)
{
  /* Assume success (for those operands that are nops).  */
  int length = 1;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_F);
      break;
    case ARC_OPERAND_F :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_F);
      break;
    case ARC_OPERAND_F0 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_F);
      break;
    case ARC_OPERAND_F1 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_F);
      break;
    case ARC_OPERAND_F1F :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_F);
      break;
    case ARC_OPERAND_GP :
      break;
    case ARC_OPERAND_LDODI :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 1, 32, total_length, pc, & fields->f_LDODi);
      break;
    case ARC_OPERAND_LDRDI :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_LDRDi);
      break;
    case ARC_OPERAND_NE :
      break;
    case ARC_OPERAND_PCL :
      break;
    case ARC_OPERAND_QCONDB :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 27, 5, 32, total_length, pc, & fields->f_cond_Q);
      break;
    case ARC_OPERAND_QCONDI :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 27, 5, 32, total_length, pc, & fields->f_cond_Q);
      break;
    case ARC_OPERAND_QCONDJ :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 27, 5, 32, total_length, pc, & fields->f_cond_Q);
      break;
    case ARC_OPERAND_R0 :
      break;
    case ARC_OPERAND_R31 :
      break;
    case ARC_OPERAND_RA :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 6, 32, total_length, pc, & fields->f_op_A);
      break;
    case ARC_OPERAND_RA_0 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 6, 32, total_length, pc, & fields->f_op_A);
      break;
    case ARC_OPERAND_RB :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 3, 32, total_length, pc, & fields->f_op__b);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 17, 3, 32, total_length, pc, & fields->f_B_5_3);
        if (length <= 0) break;
{
  FLD (f_op_B) = ((FLD (f_op__b)) | (((FLD (f_B_5_3)) << (3))));
}
      }
      break;
    case ARC_OPERAND_RB_0 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 3, 32, total_length, pc, & fields->f_op__b);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 17, 3, 32, total_length, pc, & fields->f_B_5_3);
        if (length <= 0) break;
{
  FLD (f_op_B) = ((FLD (f_op__b)) | (((FLD (f_B_5_3)) << (3))));
}
      }
      break;
    case ARC_OPERAND_RC :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_op_C);
      break;
    case ARC_OPERAND_RC_ILINK :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_op_Cj);
      break;
    case ARC_OPERAND_RC_NOILINK :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_op_Cj);
      break;
    case ARC_OPERAND_R_A :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 13, 3, 32, total_length, pc, & fields->f_op__a);
      break;
    case ARC_OPERAND_R_B :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 3, 32, total_length, pc, & fields->f_op__b);
      break;
    case ARC_OPERAND_R_C :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 3, 32, total_length, pc, & fields->f_op__c);
      break;
    case ARC_OPERAND_RCC :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 28, 4, 32, total_length, pc, & fields->f_brcond);
      break;
    case ARC_OPERAND_RCCS :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 1, 32, total_length, pc, & fields->f_brscond);
      break;
    case ARC_OPERAND_RH :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 3, 32, total_length, pc, & fields->f_h_2_0);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 13, 3, 32, total_length, pc, & fields->f_h_5_3);
        if (length <= 0) break;
{
  FLD (f_op_h) = ((FLD (f_h_2_0)) | (((FLD (f_h_5_3)) << (3))));
}
      }
      break;
    case ARC_OPERAND_SP :
      break;
    case ARC_OPERAND_STODI :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 1, 32, total_length, pc, & fields->f_STODi);
      break;
    case ARC_OPERAND_U6 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_u6);
      break;
    case ARC_OPERAND_U6X2 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & value);
        value = ((value) << (1));
        fields->f_u6x2 = value;
      }
      break;
    case ARC_OPERAND__AW :
      break;
    case ARC_OPERAND__L :
      break;
    case ARC_OPERAND__S :
      break;
    case ARC_OPERAND_CBIT :
      break;
    case ARC_OPERAND_DELAY_N :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 1, 32, total_length, pc, & fields->f_delay_N);
      break;
    case ARC_OPERAND_DUMMY_OP :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 16, 32, total_length, pc, & fields->f_dummy);
      break;
    case ARC_OPERAND_I2COND :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 2, 32, total_length, pc, & fields->f_cond_i2);
      break;
    case ARC_OPERAND_I3COND :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 7, 3, 32, total_length, pc, & fields->f_cond_i3);
      break;
    case ARC_OPERAND_LABEL10 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 7, 9, 32, total_length, pc, & value);
        value = ((((value) << (1))) + (((pc) & (-4))));
        fields->f_rel10 = value;
      }
      break;
    case ARC_OPERAND_LABEL13A :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 5, 11, 32, total_length, pc, & value);
        value = ((((value) << (2))) + (((pc) & (-4))));
        fields->f_rel13bl = value;
      }
      break;
    case ARC_OPERAND_LABEL21 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 10, 32, total_length, pc, & fields->f_d21l);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 10, 32, total_length, pc, & fields->f_d21h);
        if (length <= 0) break;
{
  FLD (f_rel21) = ((((((FLD (f_d21l)) << (1))) | (((FLD (f_d21h)) << (11))))) + (((pc) & (-4))));
}
      }
      break;
    case ARC_OPERAND_LABEL21A :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 9, 32, total_length, pc, & fields->f_d21bl);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 10, 32, total_length, pc, & fields->f_d21h);
        if (length <= 0) break;
{
  FLD (f_rel21bl) = ((((((FLD (f_d21bl)) << (2))) | (((FLD (f_d21h)) << (11))))) + (((pc) & (-4))));
}
      }
      break;
    case ARC_OPERAND_LABEL25 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 10, 32, total_length, pc, & fields->f_d21l);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 10, 32, total_length, pc, & fields->f_d25m);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 28, 4, 32, total_length, pc, & fields->f_d25h);
        if (length <= 0) break;
{
  FLD (f_rel25) = ((((((((FLD (f_d21l)) << (1))) | (((FLD (f_d25m)) << (11))))) | (((FLD (f_d25h)) << (21))))) + (((pc) & (-4))));
}
      }
      break;
    case ARC_OPERAND_LABEL25A :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 9, 32, total_length, pc, & fields->f_d21bl);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 10, 32, total_length, pc, & fields->f_d25m);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 28, 4, 32, total_length, pc, & fields->f_d25h);
        if (length <= 0) break;
{
  FLD (f_rel25bl) = ((((((((FLD (f_d21bl)) << (2))) | (((FLD (f_d25m)) << (11))))) | (((FLD (f_d25h)) << (21))))) + (((pc) & (-4))));
}
      }
      break;
    case ARC_OPERAND_LABEL7 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 10, 6, 32, total_length, pc, & value);
        value = ((((value) << (1))) + (((pc) & (-4))));
        fields->f_rel7 = value;
      }
      break;
    case ARC_OPERAND_LABEL8 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 9, 7, 32, total_length, pc, & value);
        value = ((((value) << (1))) + (((pc) & (-4))));
        fields->f_rel8 = value;
      }
      break;
    case ARC_OPERAND_LABEL9 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 7, 32, total_length, pc, & fields->f_d9l);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 1, 32, total_length, pc, & fields->f_d9h);
        if (length <= 0) break;
{
  FLD (f_rel9) = ((((((FLD (f_d9l)) << (1))) | (((FLD (f_d9h)) << (8))))) + (((pc) & (-4))));
}
      }
      break;
    case ARC_OPERAND_LBIT :
      break;
    case ARC_OPERAND_NBIT :
      break;
    case ARC_OPERAND_S12 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_u6);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 6, 32, total_length, pc, & fields->f_s12h);
        if (length <= 0) break;
{
  FLD (f_s12) = ((FLD (f_u6)) | (((FLD (f_s12h)) << (6))));
}
      }
      break;
    case ARC_OPERAND_S12X2 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 6, 32, total_length, pc, & fields->f_u6);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 6, 32, total_length, pc, & fields->f_s12h);
        if (length <= 0) break;
{
  FLD (f_s12x2) = ((((FLD (f_u6)) << (1))) | (((FLD (f_s12h)) << (7))));
}
      }
      break;
    case ARC_OPERAND_S1BIT :
      break;
    case ARC_OPERAND_S2BIT :
      break;
    case ARC_OPERAND_S9 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 8, 32, total_length, pc, & fields->f_u8);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 16, 1, 32, total_length, pc, & fields->f_d9h);
        if (length <= 0) break;
{
  FLD (f_s9) = ((FLD (f_u8)) | (((FLD (f_d9h)) << (8))));
}
      }
      break;
    case ARC_OPERAND_S9X4 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_s9x4 = value;
      }
      break;
    case ARC_OPERAND_SC_S9_ :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_s9x4 = value;
      }
      break;
    case ARC_OPERAND_SC_S9B :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, pc, & fields->f_s9x1);
      break;
    case ARC_OPERAND_SC_S9W :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 9, 32, total_length, pc, & value);
        value = ((value) << (1));
        fields->f_s9x2 = value;
      }
      break;
    case ARC_OPERAND_SC_U5_ :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 11, 5, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_u5x4 = value;
      }
      break;
    case ARC_OPERAND_SC_U5B :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 11, 5, 32, total_length, pc, & fields->f_u5);
      break;
    case ARC_OPERAND_SC_U5W :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 11, 5, 32, total_length, pc, & value);
        value = ((value) << (1));
        fields->f_u5x2 = value;
      }
      break;
    case ARC_OPERAND_TRAPNUM :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 6, 32, total_length, pc, & fields->f_trapnum);
      break;
    case ARC_OPERAND_U3 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 13, 3, 32, total_length, pc, & fields->f_u3);
      break;
    case ARC_OPERAND_U5 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 11, 5, 32, total_length, pc, & fields->f_u5);
      break;
    case ARC_OPERAND_U5X4 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 11, 5, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_u5x4 = value;
      }
      break;
    case ARC_OPERAND_U7 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 9, 7, 32, total_length, pc, & fields->f_u7);
      break;
    case ARC_OPERAND_U8 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 8, 32, total_length, pc, & fields->f_u8);
      break;
    case ARC_OPERAND_U8X4 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 8, 8, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_u8x4 = value;
      }
      break;
    case ARC_OPERAND_UNCONDB :
      break;
    case ARC_OPERAND_UNCONDI :
      break;
    case ARC_OPERAND_UNCONDJ :
      break;
    case ARC_OPERAND_VBIT :
      break;
    case ARC_OPERAND_ZBIT :
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while decoding insn.\n"),
	       opindex);
      abort ();
    }

  return length;
}

static cgen_insert_fn * const arc_cgen_insert_handlers[] = 
{
  insert_insn_normal,
};

static cgen_extract_fn * const arc_cgen_extract_handlers[] = 
{
  extract_insn_normal,
};

static int arc_cgen_get_int_operand     (CGEN_CPU_DESC, int, const CGEN_FIELDS *);
static bfd_vma arc_cgen_get_vma_operand (CGEN_CPU_DESC, int, const CGEN_FIELDS *);

/* Getting values from cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they return.
   TODO: floating point, inlining support, remove cases where result type
   not appropriate.  */

static int
arc_cgen_get_int_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     const CGEN_FIELDS * fields)
{
  int value;

  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F0 :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F1 :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F1F :
      value = fields->f_F;
      break;
    case ARC_OPERAND_GP :
      value = 0;
      break;
    case ARC_OPERAND_LDODI :
      value = fields->f_LDODi;
      break;
    case ARC_OPERAND_LDRDI :
      value = fields->f_LDRDi;
      break;
    case ARC_OPERAND_NE :
      value = 0;
      break;
    case ARC_OPERAND_PCL :
      value = 0;
      break;
    case ARC_OPERAND_QCONDB :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_QCONDI :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_QCONDJ :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_R0 :
      value = 0;
      break;
    case ARC_OPERAND_R31 :
      value = 0;
      break;
    case ARC_OPERAND_RA :
      value = fields->f_op_A;
      break;
    case ARC_OPERAND_RA_0 :
      value = fields->f_op_A;
      break;
    case ARC_OPERAND_RB :
      value = fields->f_op_B;
      break;
    case ARC_OPERAND_RB_0 :
      value = fields->f_op_B;
      break;
    case ARC_OPERAND_RC :
      value = fields->f_op_C;
      break;
    case ARC_OPERAND_RC_ILINK :
      value = fields->f_op_Cj;
      break;
    case ARC_OPERAND_RC_NOILINK :
      value = fields->f_op_Cj;
      break;
    case ARC_OPERAND_R_A :
      value = fields->f_op__a;
      break;
    case ARC_OPERAND_R_B :
      value = fields->f_op__b;
      break;
    case ARC_OPERAND_R_C :
      value = fields->f_op__c;
      break;
    case ARC_OPERAND_RCC :
      value = fields->f_brcond;
      break;
    case ARC_OPERAND_RCCS :
      value = fields->f_brscond;
      break;
    case ARC_OPERAND_RH :
      value = fields->f_op_h;
      break;
    case ARC_OPERAND_SP :
      value = 0;
      break;
    case ARC_OPERAND_STODI :
      value = fields->f_STODi;
      break;
    case ARC_OPERAND_U6 :
      value = fields->f_u6;
      break;
    case ARC_OPERAND_U6X2 :
      value = fields->f_u6x2;
      break;
    case ARC_OPERAND__AW :
      value = 0;
      break;
    case ARC_OPERAND__L :
      value = 0;
      break;
    case ARC_OPERAND__S :
      value = 0;
      break;
    case ARC_OPERAND_CBIT :
      value = 0;
      break;
    case ARC_OPERAND_DELAY_N :
      value = fields->f_delay_N;
      break;
    case ARC_OPERAND_DUMMY_OP :
      value = fields->f_dummy;
      break;
    case ARC_OPERAND_I2COND :
      value = fields->f_cond_i2;
      break;
    case ARC_OPERAND_I3COND :
      value = fields->f_cond_i3;
      break;
    case ARC_OPERAND_LABEL10 :
      value = fields->f_rel10;
      break;
    case ARC_OPERAND_LABEL13A :
      value = fields->f_rel13bl;
      break;
    case ARC_OPERAND_LABEL21 :
      value = fields->f_rel21;
      break;
    case ARC_OPERAND_LABEL21A :
      value = fields->f_rel21bl;
      break;
    case ARC_OPERAND_LABEL25 :
      value = fields->f_rel25;
      break;
    case ARC_OPERAND_LABEL25A :
      value = fields->f_rel25bl;
      break;
    case ARC_OPERAND_LABEL7 :
      value = fields->f_rel7;
      break;
    case ARC_OPERAND_LABEL8 :
      value = fields->f_rel8;
      break;
    case ARC_OPERAND_LABEL9 :
      value = fields->f_rel9;
      break;
    case ARC_OPERAND_LBIT :
      value = 0;
      break;
    case ARC_OPERAND_NBIT :
      value = 0;
      break;
    case ARC_OPERAND_S12 :
      value = fields->f_s12;
      break;
    case ARC_OPERAND_S12X2 :
      value = fields->f_s12x2;
      break;
    case ARC_OPERAND_S1BIT :
      value = 0;
      break;
    case ARC_OPERAND_S2BIT :
      value = 0;
      break;
    case ARC_OPERAND_S9 :
      value = fields->f_s9;
      break;
    case ARC_OPERAND_S9X4 :
      value = fields->f_s9x4;
      break;
    case ARC_OPERAND_SC_S9_ :
      value = fields->f_s9x4;
      break;
    case ARC_OPERAND_SC_S9B :
      value = fields->f_s9x1;
      break;
    case ARC_OPERAND_SC_S9W :
      value = fields->f_s9x2;
      break;
    case ARC_OPERAND_SC_U5_ :
      value = fields->f_u5x4;
      break;
    case ARC_OPERAND_SC_U5B :
      value = fields->f_u5;
      break;
    case ARC_OPERAND_SC_U5W :
      value = fields->f_u5x2;
      break;
    case ARC_OPERAND_TRAPNUM :
      value = fields->f_trapnum;
      break;
    case ARC_OPERAND_U3 :
      value = fields->f_u3;
      break;
    case ARC_OPERAND_U5 :
      value = fields->f_u5;
      break;
    case ARC_OPERAND_U5X4 :
      value = fields->f_u5x4;
      break;
    case ARC_OPERAND_U7 :
      value = fields->f_u7;
      break;
    case ARC_OPERAND_U8 :
      value = fields->f_u8;
      break;
    case ARC_OPERAND_U8X4 :
      value = fields->f_u8x4;
      break;
    case ARC_OPERAND_UNCONDB :
      value = 0;
      break;
    case ARC_OPERAND_UNCONDI :
      value = 0;
      break;
    case ARC_OPERAND_UNCONDJ :
      value = 0;
      break;
    case ARC_OPERAND_VBIT :
      value = 0;
      break;
    case ARC_OPERAND_ZBIT :
      value = 0;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting int operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

static bfd_vma
arc_cgen_get_vma_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     const CGEN_FIELDS * fields)
{
  bfd_vma value;

  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F0 :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F1 :
      value = fields->f_F;
      break;
    case ARC_OPERAND_F1F :
      value = fields->f_F;
      break;
    case ARC_OPERAND_GP :
      value = 0;
      break;
    case ARC_OPERAND_LDODI :
      value = fields->f_LDODi;
      break;
    case ARC_OPERAND_LDRDI :
      value = fields->f_LDRDi;
      break;
    case ARC_OPERAND_NE :
      value = 0;
      break;
    case ARC_OPERAND_PCL :
      value = 0;
      break;
    case ARC_OPERAND_QCONDB :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_QCONDI :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_QCONDJ :
      value = fields->f_cond_Q;
      break;
    case ARC_OPERAND_R0 :
      value = 0;
      break;
    case ARC_OPERAND_R31 :
      value = 0;
      break;
    case ARC_OPERAND_RA :
      value = fields->f_op_A;
      break;
    case ARC_OPERAND_RA_0 :
      value = fields->f_op_A;
      break;
    case ARC_OPERAND_RB :
      value = fields->f_op_B;
      break;
    case ARC_OPERAND_RB_0 :
      value = fields->f_op_B;
      break;
    case ARC_OPERAND_RC :
      value = fields->f_op_C;
      break;
    case ARC_OPERAND_RC_ILINK :
      value = fields->f_op_Cj;
      break;
    case ARC_OPERAND_RC_NOILINK :
      value = fields->f_op_Cj;
      break;
    case ARC_OPERAND_R_A :
      value = fields->f_op__a;
      break;
    case ARC_OPERAND_R_B :
      value = fields->f_op__b;
      break;
    case ARC_OPERAND_R_C :
      value = fields->f_op__c;
      break;
    case ARC_OPERAND_RCC :
      value = fields->f_brcond;
      break;
    case ARC_OPERAND_RCCS :
      value = fields->f_brscond;
      break;
    case ARC_OPERAND_RH :
      value = fields->f_op_h;
      break;
    case ARC_OPERAND_SP :
      value = 0;
      break;
    case ARC_OPERAND_STODI :
      value = fields->f_STODi;
      break;
    case ARC_OPERAND_U6 :
      value = fields->f_u6;
      break;
    case ARC_OPERAND_U6X2 :
      value = fields->f_u6x2;
      break;
    case ARC_OPERAND__AW :
      value = 0;
      break;
    case ARC_OPERAND__L :
      value = 0;
      break;
    case ARC_OPERAND__S :
      value = 0;
      break;
    case ARC_OPERAND_CBIT :
      value = 0;
      break;
    case ARC_OPERAND_DELAY_N :
      value = fields->f_delay_N;
      break;
    case ARC_OPERAND_DUMMY_OP :
      value = fields->f_dummy;
      break;
    case ARC_OPERAND_I2COND :
      value = fields->f_cond_i2;
      break;
    case ARC_OPERAND_I3COND :
      value = fields->f_cond_i3;
      break;
    case ARC_OPERAND_LABEL10 :
      value = fields->f_rel10;
      break;
    case ARC_OPERAND_LABEL13A :
      value = fields->f_rel13bl;
      break;
    case ARC_OPERAND_LABEL21 :
      value = fields->f_rel21;
      break;
    case ARC_OPERAND_LABEL21A :
      value = fields->f_rel21bl;
      break;
    case ARC_OPERAND_LABEL25 :
      value = fields->f_rel25;
      break;
    case ARC_OPERAND_LABEL25A :
      value = fields->f_rel25bl;
      break;
    case ARC_OPERAND_LABEL7 :
      value = fields->f_rel7;
      break;
    case ARC_OPERAND_LABEL8 :
      value = fields->f_rel8;
      break;
    case ARC_OPERAND_LABEL9 :
      value = fields->f_rel9;
      break;
    case ARC_OPERAND_LBIT :
      value = 0;
      break;
    case ARC_OPERAND_NBIT :
      value = 0;
      break;
    case ARC_OPERAND_S12 :
      value = fields->f_s12;
      break;
    case ARC_OPERAND_S12X2 :
      value = fields->f_s12x2;
      break;
    case ARC_OPERAND_S1BIT :
      value = 0;
      break;
    case ARC_OPERAND_S2BIT :
      value = 0;
      break;
    case ARC_OPERAND_S9 :
      value = fields->f_s9;
      break;
    case ARC_OPERAND_S9X4 :
      value = fields->f_s9x4;
      break;
    case ARC_OPERAND_SC_S9_ :
      value = fields->f_s9x4;
      break;
    case ARC_OPERAND_SC_S9B :
      value = fields->f_s9x1;
      break;
    case ARC_OPERAND_SC_S9W :
      value = fields->f_s9x2;
      break;
    case ARC_OPERAND_SC_U5_ :
      value = fields->f_u5x4;
      break;
    case ARC_OPERAND_SC_U5B :
      value = fields->f_u5;
      break;
    case ARC_OPERAND_SC_U5W :
      value = fields->f_u5x2;
      break;
    case ARC_OPERAND_TRAPNUM :
      value = fields->f_trapnum;
      break;
    case ARC_OPERAND_U3 :
      value = fields->f_u3;
      break;
    case ARC_OPERAND_U5 :
      value = fields->f_u5;
      break;
    case ARC_OPERAND_U5X4 :
      value = fields->f_u5x4;
      break;
    case ARC_OPERAND_U7 :
      value = fields->f_u7;
      break;
    case ARC_OPERAND_U8 :
      value = fields->f_u8;
      break;
    case ARC_OPERAND_U8X4 :
      value = fields->f_u8x4;
      break;
    case ARC_OPERAND_UNCONDB :
      value = 0;
      break;
    case ARC_OPERAND_UNCONDI :
      value = 0;
      break;
    case ARC_OPERAND_UNCONDJ :
      value = 0;
      break;
    case ARC_OPERAND_VBIT :
      value = 0;
      break;
    case ARC_OPERAND_ZBIT :
      value = 0;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting vma operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

static void arc_cgen_set_int_operand  (CGEN_CPU_DESC, int, CGEN_FIELDS *, int);
static void arc_cgen_set_vma_operand  (CGEN_CPU_DESC, int, CGEN_FIELDS *, bfd_vma);

/* Stuffing values in cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they accept.
   TODO: floating point, inlining support, remove cases where argument type
   not appropriate.  */

static void
arc_cgen_set_int_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     CGEN_FIELDS * fields,
			     int value)
{
  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F0 :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F1 :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F1F :
      fields->f_F = value;
      break;
    case ARC_OPERAND_GP :
      break;
    case ARC_OPERAND_LDODI :
      fields->f_LDODi = value;
      break;
    case ARC_OPERAND_LDRDI :
      fields->f_LDRDi = value;
      break;
    case ARC_OPERAND_NE :
      break;
    case ARC_OPERAND_PCL :
      break;
    case ARC_OPERAND_QCONDB :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_QCONDI :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_QCONDJ :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_R0 :
      break;
    case ARC_OPERAND_R31 :
      break;
    case ARC_OPERAND_RA :
      fields->f_op_A = value;
      break;
    case ARC_OPERAND_RA_0 :
      fields->f_op_A = value;
      break;
    case ARC_OPERAND_RB :
      fields->f_op_B = value;
      break;
    case ARC_OPERAND_RB_0 :
      fields->f_op_B = value;
      break;
    case ARC_OPERAND_RC :
      fields->f_op_C = value;
      break;
    case ARC_OPERAND_RC_ILINK :
      fields->f_op_Cj = value;
      break;
    case ARC_OPERAND_RC_NOILINK :
      fields->f_op_Cj = value;
      break;
    case ARC_OPERAND_R_A :
      fields->f_op__a = value;
      break;
    case ARC_OPERAND_R_B :
      fields->f_op__b = value;
      break;
    case ARC_OPERAND_R_C :
      fields->f_op__c = value;
      break;
    case ARC_OPERAND_RCC :
      fields->f_brcond = value;
      break;
    case ARC_OPERAND_RCCS :
      fields->f_brscond = value;
      break;
    case ARC_OPERAND_RH :
      fields->f_op_h = value;
      break;
    case ARC_OPERAND_SP :
      break;
    case ARC_OPERAND_STODI :
      fields->f_STODi = value;
      break;
    case ARC_OPERAND_U6 :
      fields->f_u6 = value;
      break;
    case ARC_OPERAND_U6X2 :
      fields->f_u6x2 = value;
      break;
    case ARC_OPERAND__AW :
      break;
    case ARC_OPERAND__L :
      break;
    case ARC_OPERAND__S :
      break;
    case ARC_OPERAND_CBIT :
      break;
    case ARC_OPERAND_DELAY_N :
      fields->f_delay_N = value;
      break;
    case ARC_OPERAND_DUMMY_OP :
      fields->f_dummy = value;
      break;
    case ARC_OPERAND_I2COND :
      fields->f_cond_i2 = value;
      break;
    case ARC_OPERAND_I3COND :
      fields->f_cond_i3 = value;
      break;
    case ARC_OPERAND_LABEL10 :
      fields->f_rel10 = value;
      break;
    case ARC_OPERAND_LABEL13A :
      fields->f_rel13bl = value;
      break;
    case ARC_OPERAND_LABEL21 :
      fields->f_rel21 = value;
      break;
    case ARC_OPERAND_LABEL21A :
      fields->f_rel21bl = value;
      break;
    case ARC_OPERAND_LABEL25 :
      fields->f_rel25 = value;
      break;
    case ARC_OPERAND_LABEL25A :
      fields->f_rel25bl = value;
      break;
    case ARC_OPERAND_LABEL7 :
      fields->f_rel7 = value;
      break;
    case ARC_OPERAND_LABEL8 :
      fields->f_rel8 = value;
      break;
    case ARC_OPERAND_LABEL9 :
      fields->f_rel9 = value;
      break;
    case ARC_OPERAND_LBIT :
      break;
    case ARC_OPERAND_NBIT :
      break;
    case ARC_OPERAND_S12 :
      fields->f_s12 = value;
      break;
    case ARC_OPERAND_S12X2 :
      fields->f_s12x2 = value;
      break;
    case ARC_OPERAND_S1BIT :
      break;
    case ARC_OPERAND_S2BIT :
      break;
    case ARC_OPERAND_S9 :
      fields->f_s9 = value;
      break;
    case ARC_OPERAND_S9X4 :
      fields->f_s9x4 = value;
      break;
    case ARC_OPERAND_SC_S9_ :
      fields->f_s9x4 = value;
      break;
    case ARC_OPERAND_SC_S9B :
      fields->f_s9x1 = value;
      break;
    case ARC_OPERAND_SC_S9W :
      fields->f_s9x2 = value;
      break;
    case ARC_OPERAND_SC_U5_ :
      fields->f_u5x4 = value;
      break;
    case ARC_OPERAND_SC_U5B :
      fields->f_u5 = value;
      break;
    case ARC_OPERAND_SC_U5W :
      fields->f_u5x2 = value;
      break;
    case ARC_OPERAND_TRAPNUM :
      fields->f_trapnum = value;
      break;
    case ARC_OPERAND_U3 :
      fields->f_u3 = value;
      break;
    case ARC_OPERAND_U5 :
      fields->f_u5 = value;
      break;
    case ARC_OPERAND_U5X4 :
      fields->f_u5x4 = value;
      break;
    case ARC_OPERAND_U7 :
      fields->f_u7 = value;
      break;
    case ARC_OPERAND_U8 :
      fields->f_u8 = value;
      break;
    case ARC_OPERAND_U8X4 :
      fields->f_u8x4 = value;
      break;
    case ARC_OPERAND_UNCONDB :
      break;
    case ARC_OPERAND_UNCONDI :
      break;
    case ARC_OPERAND_UNCONDJ :
      break;
    case ARC_OPERAND_VBIT :
      break;
    case ARC_OPERAND_ZBIT :
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting int operand.\n"),
		       opindex);
      abort ();
  }
}

static void
arc_cgen_set_vma_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     CGEN_FIELDS * fields,
			     bfd_vma value)
{
  switch (opindex)
    {
    case ARC_OPERAND_EXDI :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F0 :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F1 :
      fields->f_F = value;
      break;
    case ARC_OPERAND_F1F :
      fields->f_F = value;
      break;
    case ARC_OPERAND_GP :
      break;
    case ARC_OPERAND_LDODI :
      fields->f_LDODi = value;
      break;
    case ARC_OPERAND_LDRDI :
      fields->f_LDRDi = value;
      break;
    case ARC_OPERAND_NE :
      break;
    case ARC_OPERAND_PCL :
      break;
    case ARC_OPERAND_QCONDB :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_QCONDI :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_QCONDJ :
      fields->f_cond_Q = value;
      break;
    case ARC_OPERAND_R0 :
      break;
    case ARC_OPERAND_R31 :
      break;
    case ARC_OPERAND_RA :
      fields->f_op_A = value;
      break;
    case ARC_OPERAND_RA_0 :
      fields->f_op_A = value;
      break;
    case ARC_OPERAND_RB :
      fields->f_op_B = value;
      break;
    case ARC_OPERAND_RB_0 :
      fields->f_op_B = value;
      break;
    case ARC_OPERAND_RC :
      fields->f_op_C = value;
      break;
    case ARC_OPERAND_RC_ILINK :
      fields->f_op_Cj = value;
      break;
    case ARC_OPERAND_RC_NOILINK :
      fields->f_op_Cj = value;
      break;
    case ARC_OPERAND_R_A :
      fields->f_op__a = value;
      break;
    case ARC_OPERAND_R_B :
      fields->f_op__b = value;
      break;
    case ARC_OPERAND_R_C :
      fields->f_op__c = value;
      break;
    case ARC_OPERAND_RCC :
      fields->f_brcond = value;
      break;
    case ARC_OPERAND_RCCS :
      fields->f_brscond = value;
      break;
    case ARC_OPERAND_RH :
      fields->f_op_h = value;
      break;
    case ARC_OPERAND_SP :
      break;
    case ARC_OPERAND_STODI :
      fields->f_STODi = value;
      break;
    case ARC_OPERAND_U6 :
      fields->f_u6 = value;
      break;
    case ARC_OPERAND_U6X2 :
      fields->f_u6x2 = value;
      break;
    case ARC_OPERAND__AW :
      break;
    case ARC_OPERAND__L :
      break;
    case ARC_OPERAND__S :
      break;
    case ARC_OPERAND_CBIT :
      break;
    case ARC_OPERAND_DELAY_N :
      fields->f_delay_N = value;
      break;
    case ARC_OPERAND_DUMMY_OP :
      fields->f_dummy = value;
      break;
    case ARC_OPERAND_I2COND :
      fields->f_cond_i2 = value;
      break;
    case ARC_OPERAND_I3COND :
      fields->f_cond_i3 = value;
      break;
    case ARC_OPERAND_LABEL10 :
      fields->f_rel10 = value;
      break;
    case ARC_OPERAND_LABEL13A :
      fields->f_rel13bl = value;
      break;
    case ARC_OPERAND_LABEL21 :
      fields->f_rel21 = value;
      break;
    case ARC_OPERAND_LABEL21A :
      fields->f_rel21bl = value;
      break;
    case ARC_OPERAND_LABEL25 :
      fields->f_rel25 = value;
      break;
    case ARC_OPERAND_LABEL25A :
      fields->f_rel25bl = value;
      break;
    case ARC_OPERAND_LABEL7 :
      fields->f_rel7 = value;
      break;
    case ARC_OPERAND_LABEL8 :
      fields->f_rel8 = value;
      break;
    case ARC_OPERAND_LABEL9 :
      fields->f_rel9 = value;
      break;
    case ARC_OPERAND_LBIT :
      break;
    case ARC_OPERAND_NBIT :
      break;
    case ARC_OPERAND_S12 :
      fields->f_s12 = value;
      break;
    case ARC_OPERAND_S12X2 :
      fields->f_s12x2 = value;
      break;
    case ARC_OPERAND_S1BIT :
      break;
    case ARC_OPERAND_S2BIT :
      break;
    case ARC_OPERAND_S9 :
      fields->f_s9 = value;
      break;
    case ARC_OPERAND_S9X4 :
      fields->f_s9x4 = value;
      break;
    case ARC_OPERAND_SC_S9_ :
      fields->f_s9x4 = value;
      break;
    case ARC_OPERAND_SC_S9B :
      fields->f_s9x1 = value;
      break;
    case ARC_OPERAND_SC_S9W :
      fields->f_s9x2 = value;
      break;
    case ARC_OPERAND_SC_U5_ :
      fields->f_u5x4 = value;
      break;
    case ARC_OPERAND_SC_U5B :
      fields->f_u5 = value;
      break;
    case ARC_OPERAND_SC_U5W :
      fields->f_u5x2 = value;
      break;
    case ARC_OPERAND_TRAPNUM :
      fields->f_trapnum = value;
      break;
    case ARC_OPERAND_U3 :
      fields->f_u3 = value;
      break;
    case ARC_OPERAND_U5 :
      fields->f_u5 = value;
      break;
    case ARC_OPERAND_U5X4 :
      fields->f_u5x4 = value;
      break;
    case ARC_OPERAND_U7 :
      fields->f_u7 = value;
      break;
    case ARC_OPERAND_U8 :
      fields->f_u8 = value;
      break;
    case ARC_OPERAND_U8X4 :
      fields->f_u8x4 = value;
      break;
    case ARC_OPERAND_UNCONDB :
      break;
    case ARC_OPERAND_UNCONDI :
      break;
    case ARC_OPERAND_UNCONDJ :
      break;
    case ARC_OPERAND_VBIT :
      break;
    case ARC_OPERAND_ZBIT :
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

/* Function to call before using the instruction builder tables.  */

void
arc_cgen_init_ibld_table (CGEN_CPU_DESC cd)
{
  cd->insert_handlers  = &arc_cgen_insert_handlers [0];
  cd->extract_handlers = &arc_cgen_extract_handlers[0];

  cd->insert_operand  = arc_cgen_insert_operand;
  cd->extract_operand = arc_cgen_extract_operand;

  cd->get_int_operand = arc_cgen_get_int_operand;
  cd->set_int_operand = arc_cgen_set_int_operand;
  cd->get_vma_operand = arc_cgen_get_vma_operand;
  cd->set_vma_operand = arc_cgen_set_vma_operand;
}
