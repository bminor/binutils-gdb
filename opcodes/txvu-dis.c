/* Instruction printing code for the TXVU
   Copyright (C) 1998 Free Software Foundation, Inc. 

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "dis-asm.h"
#include "opcode/txvu.h"
#include "elf-bfd.h"
#include "elf/txvu.h"

static void print_insn PARAMS ((bfd_vma, disassemble_info *, TXVU_INSN, int));

/* Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction.  */

int
print_insn_txvu (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  bfd_byte buffer[8];
  void *stream = info->stream;
  fprintf_ftype func = info->fprintf_func;
  int status;
  /* First element is upper, second is lower.  */
  TXVU_INSN upper,lower;
  static int initialized = 0;

  if (!initialized)
    {
      initialized = 1;
      txvu_opcode_init_tables (0);
    }

  status = (*info->read_memory_func) (pc, buffer, 8, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }
  upper = bfd_getl32 (buffer);
  lower = bfd_getl32 (buffer + 4);

  /* FIXME: This will need revisiting.  */
  print_insn (pc, info, upper, 0);
  (*func) (stream, " || ");
  print_insn (pc, info, lower, 1);

  return 8;
}

/* Print one instruction.
   LOWER_P is non-zero if disassembling a lower slot insn.  */

static void
print_insn (pc, info, insn, lower_p)
     bfd_vma pc;
     disassemble_info *info;
     TXVU_INSN insn;
     int lower_p;
{
  const struct txvu_opcode *opcode;
  void *stream = info->stream;
  fprintf_ftype func = info->fprintf_func;

  /* The instructions are stored in lists hashed by the insn code
     (though we needn't care how they're hashed).  */

  if (lower_p)
    opcode = txvu_lower_opcode_lookup_dis (insn);
  else
    opcode = txvu_upper_opcode_lookup_dis (insn);
  for ( ; opcode != NULL; opcode = TXVU_OPCODE_NEXT_DIS (opcode))
    {
      const unsigned char *syn;
      int mods,invalid,num_operands;
      long value;
      const struct txvu_operand *operand;

      /* Basic bit mask must be correct.  */
      if ((insn & opcode->mask) != opcode->value)
	continue;

      /* Make two passes over the operands.  First see if any of them
	 have extraction functions, and, if they do, make sure the
	 instruction is valid.  */

      txvu_opcode_init_print ();
      invalid = 0;

      for (syn = opcode->syntax; *syn; ++syn)
	{
	  int index;

	  if (*syn < 128)
	    continue;

	  mods = 0;
	  index = TXVU_OPERAND_INDEX (*syn);
	  while (TXVU_MOD_P (txvu_operands[index].flags))
	    {
	      mods |= txvu_operands[index].flags & TXVU_MOD_BITS;
	      ++syn;
	      index = TXVU_OPERAND_INDEX (*syn);
	    }
	  operand = txvu_operands + index;
	  if (operand->extract)
	    (*operand->extract) (insn, operand, mods, &invalid);
	}
      if (invalid)
	continue;

      /* The instruction is valid.  */

      (*func) (stream, "%s", opcode->mnemonic);
      num_operands = 0;
      for (syn = opcode->syntax; *syn; ++syn)
	{
	  int index;

	  if (*syn < 128)
	    {
	      (*func) (stream, "%c", *syn);
	      continue;
	    }

	  /* We have an operand.  Fetch any special modifiers.  */
	  mods = 0;
	  index = TXVU_OPERAND_INDEX (*syn);
	  while (TXVU_MOD_P (txvu_operands[index].flags))
	    {
	      mods |= txvu_operands[index].flags & TXVU_MOD_BITS;
	      ++syn;
	      index = TXVU_OPERAND_INDEX (*syn);
	    }
	  operand = txvu_operands + index;

	  /* Extract the value from the instruction.  */
	  if (operand->extract)
	    {
	      value = (*operand->extract) (insn, operand, mods, (int *) NULL);
	    }
	  else
	    {
	      value = (insn >> operand->shift) & ((1 << operand->bits) - 1);
	      if ((operand->flags & TXVU_OPERAND_SIGNED)
		  && (value & (1 << (operand->bits - 1))))
		value -= 1 << operand->bits;
	    }

	  /* If second or later operand, print a comma.  */
	  if (num_operands > 0)
	    (*func) (stream, ",");

	  /* Print the operand as directed by the flags.  */
	  if (operand->print)
	    (*operand->print) (info, insn, value);
	  else if (operand->flags & TXVU_OPERAND_FAKE)
	    ; /* nothing to do (??? at least not yet) */
	  else if (operand->flags & TXVU_OPERAND_RELATIVE_BRANCH)
	    (*info->print_address_func) (pc + value, info);
	  /* ??? Not all cases of this are currently caught.  */
	  else if (operand->flags & TXVU_OPERAND_ABSOLUTE_BRANCH)
	    (*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
	  else if (operand->flags & TXVU_OPERAND_ADDRESS)
	    (*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
	  else
	    (*func) (stream, "%ld", value);

	  if (! (operand->flags & TXVU_OPERAND_SUFFIX))
	    ++num_operands;
	}

      /* We have found and printed an instruction; return.  */
      return;
    }

  (*func) (stream, "*unknown*");
}
