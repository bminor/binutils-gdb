/* Disassembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

This file is used to generate m32r-dis.c.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "dis-asm.h"
#include "bfd.h"
#include "m32r-opc.h"

/* ??? The layout of this stuff is still work in progress.
   For speed in assembly/disassembly, we use inline functions.  That of course
   will only work for GCC.  When this stuff is finished, we can decide whether
   to keep the inline functions (and only get the performance increase when
   compiled with GCC), or switch to macros, or use something else.
*/

/* Default text to print if an instruction isn't recognized.  */
#define UNKNOWN_INSN_MSG "*unknown*"

/* FIXME: Machine generate.  */
#ifndef CGEN_PCREL_OFFSET
#define CGEN_PCREL_OFFSET 0
#endif

static int print_insn PARAMS ((bfd_vma, disassemble_info *, char *, int));

static int extract_insn_normal
     PARAMS ((const CGEN_INSN *, void *, cgen_insn_t, CGEN_FIELDS *));
static void print_insn_normal
     PARAMS ((void *, const CGEN_INSN *, CGEN_FIELDS *, bfd_vma, int));

CGEN_INLINE void
m32r_cgen_print_operand
     PARAMS ((int opindex, disassemble_info * info, CGEN_FIELDS * fields, void const * attrs, bfd_vma pc, int length));


/* Default extraction routine.

   ATTRS is a mask of the boolean attributes.  We only need `unsigned',
   but for generality we take a bitmask of all of them.  */

static int
extract_normal (buf_ctrl, insn_value, attrs, start, length, shift, total_length, valuep)
     void *       buf_ctrl;
     cgen_insn_t  insn_value;
     unsigned int attrs;
     int          start;
     int          length;
     int          shift;
     int          total_length;
     long *       valuep;
{
  long value;

#ifdef CGEN_INT_INSN
#if 0
  value = ((insn_value >> (CGEN_BASE_INSN_BITSIZE - (start + length)))
	   & ((1 << length) - 1));
#else
  value = ((insn_value >> (total_length - (start + length)))
	   & ((1 << length) - 1));
#endif
  if (! (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_UNSIGNED))
      && (value & (1 << (length - 1))))
    value -= 1 << length;
#else
  /* FIXME: unfinished */
#endif

  /* This is backwards as we undo the effects of insert_normal.  */
  if (shift < 0)
    value >>= -shift;
  else
    value <<= shift;

  * valuep = value;
  return 1;
}

/* Default print handler.  */

static void
print_normal (dis_info, value, attrs, pc, length)
     void *        dis_info;
     long          value;
     unsigned int  attrs;
     unsigned long pc; /* FIXME: should be bfd_vma */
     int           length;
{
  disassemble_info * info = dis_info;

  /* Print the operand as directed by the attributes.  */
  if (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_FAKE))
    ; /* nothing to do (??? at least not yet) */
  else if (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_PCREL_ADDR))
    (*info->print_address_func) (pc + CGEN_PCREL_OFFSET + value, info);
  /* ??? Not all cases of this are currently caught.  */
  else if (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_ABS_ADDR))
    /* FIXME: Why & 0xffffffff?  */
    (*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
  else if (attrs & CGEN_ATTR_MASK (CGEN_OPERAND_UNSIGNED))
    (*info->fprintf_func) (info->stream, "0x%lx", value);
  else
    (*info->fprintf_func) (info->stream, "%ld", value);
}

/* Keyword print handler.  */

static void
print_keyword (dis_info, keyword_table, value, attrs)
     void *         dis_info;
     CGEN_KEYWORD * keyword_table;
     long           value;
     CGEN_ATTR *    attrs;
{
  disassemble_info * info = dis_info;
  const CGEN_KEYWORD_ENTRY * ke;

  ke = cgen_keyword_lookup_value (keyword_table, value);
  info->fprintf_func (info->stream, "%s", ke == NULL ? "???" : ke->name);
}

/* -- disassembler routines inserted here */

/* Default insn extractor.

   The extracted fields are stored in DIS_FLDS.
   BUF_CTRL is used to handle reading variable length insns (FIXME: not done).
   Return the length of the insn in bits, or 0 if no match.  */

static int
extract_insn_normal (insn, buf_ctrl, insn_value, fields)
     const CGEN_INSN *  insn;
     void *             buf_ctrl;
     cgen_insn_t        insn_value;
     CGEN_FIELDS *      fields;
{
  const CGEN_SYNTAX *   syntax = CGEN_INSN_SYNTAX (insn);
  const unsigned char * syn;

  CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

  CGEN_INIT_EXTRACT ();

  for (syn = CGEN_SYNTAX_STRING (syntax); * syn; ++ syn)
    {
      int length;

      if (CGEN_SYNTAX_CHAR_P (* syn))
	continue;

      length = m32r_cgen_extract_operand (CGEN_SYNTAX_FIELD (* syn),
					   buf_ctrl, insn_value, fields);
      if (length == 0)
	return 0;
    }

  /* We recognized and successfully extracted this insn.  */
  return CGEN_INSN_BITSIZE (insn);
}

/* Default insn printer.

   DIS_INFO is defined as `void *' so the disassembler needn't know anything
   about disassemble_info.
*/

static void
print_insn_normal (dis_info, insn, fields, pc, length)
     void *             dis_info;
     const CGEN_INSN *  insn;
     CGEN_FIELDS *      fields;
     bfd_vma            pc;
     int                length;
{
  const CGEN_SYNTAX *   syntax = CGEN_INSN_SYNTAX (insn);
  disassemble_info *    info = dis_info;
  const unsigned char * syn;

  CGEN_INIT_PRINT ();

  for (syn = CGEN_SYNTAX_STRING (syntax); * syn; ++ syn)
    {
      if (CGEN_SYNTAX_MNEMONIC_P (* syn))
	{
	  info->fprintf_func (info->stream, "%s", CGEN_INSN_MNEMONIC (insn));
	  continue;
	}
      if (CGEN_SYNTAX_CHAR_P (* syn))
	{
	  info->fprintf_func (info->stream, "%c", CGEN_SYNTAX_CHAR (* syn));
	  continue;
	}

      /* We have an operand.  */
      m32r_cgen_print_operand (CGEN_SYNTAX_FIELD (* syn), info,
				fields, CGEN_INSN_ATTRS (insn), pc, length);
    }
}

/* Default value for CGEN_PRINT_INSN.
   Given BUFLEN bytes (target byte order) read into BUF, look up the
   insn in the instruction table and disassemble it.

   The result is the size of the insn in bytes.  */

#ifndef CGEN_PRINT_INSN
#define CGEN_PRINT_INSN print_insn
#endif

static int
print_insn (pc, info, buf, buflen)
     bfd_vma            pc;
     disassemble_info * info;
     char *             buf;
     int                buflen;
{
  int                    i;
  unsigned long          insn_value;
  const CGEN_INSN_LIST * insn_list;
  int                    extra_bytes;
  
  switch (buflen)
    {
    case 8:
      insn_value = buf[0];
      break;
    case 16:
      insn_value = info->endian == BFD_ENDIAN_BIG ? bfd_getb16 (buf) : bfd_getl16 (buf);
      break;
    case 32:
      insn_value = info->endian == BFD_ENDIAN_BIG ? bfd_getb32 (buf) : bfd_getl32 (buf);
      break;
    default:
      abort ();
    }

  /* Special case - a 32 bit instruction which is actually two 16 bit instructions
     being executed in parallel.  */
  if (buflen == 32
      && ((insn_value & 0x80008000) == 0x00008000))
    {
      if (info->endian == BFD_ENDIAN_BIG)
	{
	  static char buf2 [4];
	  
	  print_insn (pc, info, buf, 16);

	  info->fprintf_func (info->stream, " || ");

	  buf2 [0] = buf [2] & ~ 0x80;
	  buf2 [1] = buf [3];
	  buf2 [2] = 0;
	  buf2 [3] = 0;
	  buf = buf2;
	  
	  insn_value <<= 17;
	  insn_value >>= 1;
	}
      else
	{
	  print_insn (pc, info, buf + 2, 16);

	  info->fprintf_func (info->stream, " || ");

	  insn_value &= 0x7fff;
	}

      pc += 2;
      extra_bytes = 2;
    }
  else
    extra_bytes = 0;
  
  /* The instructions are stored in hash lists.
     Pick the first one and keep trying until we find the right one.  */

  insn_list = CGEN_DIS_LOOKUP_INSN (buf, insn_value);

  while (insn_list != NULL)
    {
      const CGEN_INSN *   insn = insn_list->insn;
      unsigned long       value;

#if 0 /* not needed as insn shouldn't be in hash lists if not supported */
      /* Supported by this cpu?  */
      if (! m32r_cgen_insn_supported (insn))
	continue;
#endif

      /* If we are looking at a 16 bit insn we may have to adjust the value being examined.  */
      value = insn_value;
      if (CGEN_INSN_BITSIZE (insn) == 16)
	{
	/* If this is a big endian target,
	   and we have read 32 bits for the instruction value,
	   then we must examine the top 16 bits, not the bottom.  */
	  if (buflen == 32 && info->endian == BFD_ENDIAN_BIG)
	    value >>= 16;
	}
      
      /* Basic bit mask must be correct.  */
      /* ??? May wish to allow target to defer this check until the extract
	 handler.  */
      if ((value & CGEN_INSN_MASK (insn)) == CGEN_INSN_VALUE (insn))
	{
	  CGEN_FIELDS fields;
	  int         length;
	  
	  /* Printing is handled in two passes.  The first pass parses the
	     machine insn and extracts the fields.  The second pass prints
	     them.  */

	  length = CGEN_EXTRACT_FN (insn) (insn, NULL, value, & fields);
	  if (length > 0)
	    {
	      CGEN_PRINT_FN (insn) (info, insn, & fields, pc, length);
	      
	      /* length is in bits, result is in bytes */
	      return (length / 8) + extra_bytes;
	    }
	}
      
      insn_list = CGEN_DIS_NEXT_INSN (insn_list);
    }

  return extra_bytes;
}

/* Main entry point.
   Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction (in bytes).  */

int
print_insn_m32r (pc, info)
     bfd_vma            pc;
     disassemble_info * info;
{
  char       buffer [CGEN_MAX_INSN_SIZE];
  int        status;
  int        length;
  static int initialized    = 0;
  static int current_mach   = 0;
  static int current_bigend = 0;
  int        mach           = info->mach;
  int        bigend         = info->endian == BFD_ENDIAN_BIG;

  /* If we haven't initialized yet, or if we've switched cpu's, initialize.  */
  if (!initialized || mach != current_mach || bigend != current_bigend)
    {
      initialized    = 1;
      current_mach   = mach;
      current_bigend = bigend;
      
      m32r_cgen_init_dis (mach, bigend ? CGEN_ENDIAN_BIG : CGEN_ENDIAN_LITTLE);
    }

  /* Read enough of the insn so we can look it up in the hash lists.  */

  status = info->read_memory_func (pc, buffer, CGEN_BASE_INSN_SIZE, info);
  if (status != 0)
    {
      /* Try reading a 16 bit instruction.  */
      info->bytes_per_chunk = 2;
      status = info->read_memory_func (pc, buffer, CGEN_BASE_INSN_SIZE / 2, info);
      buffer [2] = buffer [3] = 0;
    }
  if (status != 0)
    {
      info->memory_error_func (status, pc, info);
      return -1;
    }

  /* We try to have as much common code as possible.
     But at this point some targets need to take over.  */
  /* ??? Some targets may need a hook elsewhere.  Try to avoid this,
     but if not possible try to move this hook elsewhere rather than
     have two hooks.  */
  length = CGEN_PRINT_INSN (pc, info, buffer, CGEN_BASE_INSN_BITSIZE);
  
  if (length)
    return length;

  info->fprintf_func (info->stream, UNKNOWN_INSN_MSG);
  
  return CGEN_DEFAULT_INSN_SIZE;
}

/* Get the generate machine specific code.  */
#include "m32r-dis.in"
