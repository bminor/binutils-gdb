/* Instruction printing code for the ARC.
   Copyright (C) 1994 Free Software Foundation, Inc. 
   Contributed by Doug Evans (dje@cygnus.com).

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "dis-asm.h"
#include "opcode/arc.h"

/* Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction (4 or 8 for the ARC). */

int
print_insn_arc (pc, info)
     bfd_vma pc;
     struct disassemble_info *info;
{
  const struct arc_opcode *opcode,*opcode_end;
  bfd_byte buffer[4];
  void *stream = info->stream;
  fprintf_ftype func = info->fprintf_func;
  int status;
  /* First element is insn, second element is limm (if present).  */
  arc_insn insn[2];
  int got_limm_p = 0;
  static int initialized = 0;

  if (!initialized)
    {
      /* ??? Hmmm... what do we pass here?  */
      arc_opcode_init_tables (ARC_HAVE_MULT_SHIFT);
      initialized = 1;
    }

  status = (*info->read_memory_func) (pc, buffer, 4, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }
  insn[0] = bfd_getb32 (buffer);

  func (stream, "%08lx\t", insn[0]);

  opcode_end = arc_opcodes + arc_opcodes_count;
  for (opcode = arc_opcodes; opcode < opcode_end; opcode++)
    {
      char *syn;
      int mods,invalid;
      long value;
      const struct arc_operand *operand;
      const struct arc_operand_value *opval;

      if ((insn[0] & opcode->mask) != opcode->value)
	continue;

      /* Make two passes over the operands.  First see if any of them
	 have extraction functions, and, if they do, make sure the
	 instruction is valid.  */

      arc_opcode_init_extract ();
      invalid = 0;

      /* ??? Granted, this is slower than the `ppc' way.  Maybe when this is
	 done it'll be clear what the right way to do this is.  */
      /* Instructions like "add.f r0,r1,1" are tricky because the ".f" gets
	 printed first, but we don't know how to print it until we've processed
	 the regs.  Since we're scanning all the args before printing the insn
	 anyways, it's quite easy.  */

      for (syn = opcode->syntax; *syn; ++syn)
	{
	  if (*syn != '%' || *++syn == '%')
	    continue;
	  mods = 0;
	  while (ARC_MOD_P (arc_operands[arc_operand_map[*syn]].flags))
	    {
	      mods |= arc_operands[arc_operand_map[*syn]].flags & ARC_MOD_BITS;
	      ++syn;
	    }
	  operand = arc_operands + arc_operand_map[*syn];
	  if (operand->extract)
	    (*operand->extract) (insn, operand, mods,
				 (const struct arc_operand_value **) NULL,
				 &invalid);
	}
      if (invalid)
	continue;

      /* The instruction is valid.  */

      /* If we have an insn with a limm, fetch it now.  Scanning the insns
	 twice lets us do this.  */
      if (arc_opcode_limm_p (NULL))
	{
	  status = (*info->read_memory_func) (pc + 4, buffer, 4, info);
	  if (status != 0)
	    {
	      (*info->memory_error_func) (status, pc, info);
	      return -1;
	    }
	  insn[1] = bfd_getb32 (buffer);
	  got_limm_p = 1;
	}

      for (syn = opcode->syntax; *syn; ++syn)
	{
	  if (*syn != '%' || *++syn == '%')
	    {
	      func (stream, "%c", *syn);
	      continue;
	    }

	  /* We have an operand.  Fetch any special modifiers.  */
	  mods = 0;
	  while (ARC_MOD_P (arc_operands[arc_operand_map[*syn]].flags))
	    {
	      mods |= arc_operands[arc_operand_map[*syn]].flags & ARC_MOD_BITS;
	      ++syn;
	    }
	  operand = arc_operands + arc_operand_map[*syn];

	  /* Extract the value from the instruction.  */
	  opval = NULL;
	  if (operand->extract)
	    {
	      value = (*operand->extract) (insn, operand, mods,
					   &opval, (int *) NULL);
	    }
	  else
	    {
	      value = (insn[0] >> operand->shift) & ((1 << operand->bits) - 1);
	      if ((operand->flags & ARC_OPERAND_SIGNED)
		  && (value & (1 << (operand->bits - 1))))
		value -= 1 << operand->bits;

	      /* If this is a suffix operand, set `opval'.  */
	      if (operand->flags & ARC_OPERAND_SUFFIX)
		opval = arc_opcode_lookup_suffix (operand, value);
	    }

	  /* Print the operand as directed by the flags.  */
	  if (operand->flags & ARC_OPERAND_FAKE)
	    ; /* nothing to do (??? at least not yet) */
	  else if (operand->flags & ARC_OPERAND_SUFFIX)
	    {
	      /* Default suffixes aren't printed.  Fortunately, they all have
		 zero values.  Also, zero values for boolean suffixes are
		 represented by the absence of text.  */

	      if (value != 0)
		{
		  /* ??? OPVAL should have a value.  If it doesn't just cope
		     as we want disassembly to be reasonably robust.
		     Also remember that several condition code values (16-31)
		     aren't defined yet.  For these cases just print the
		     number suitably decorated.  */
		  if (opval)
		    func (stream, "%s%s",
			  mods & ARC_MOD_DOT ? "." : "",
			  opval->name);
		  else
		    func (stream, "%s%c%d",
			  mods & ARC_MOD_DOT ? "." : "",
			  operand->fmt, value);
		}
	    }
	  else if (operand->flags & ARC_OPERAND_RELATIVE)
	    (*info->print_address_func) (pc + value, info);
	  /* ??? Not all cases of this are currently caught.  */
	  else if (operand->flags & ARC_OPERAND_ABSOLUTE)
	    (*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
	  else if (opval)
	    /* Note that this case catches both normal and auxiliary regs.  */
	    func (stream, "%s", opval->name);
	  else
	    func (stream, "%ld", value);
	}

      /* We have found and printed an instruction; return.  */
      return got_limm_p ? 8 : 4;
    }

  func (stream, "*unknown*");
  return 4;
}
