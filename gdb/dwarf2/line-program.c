/* DWARF 2 debugging format support for GDB.

   Copyright (C) 1994-2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "dwarf2/line-program.h"
#include "dwarf2/cu.h"
#include "dwarf2/line-header.h"
#include "dwarf2/read.h"
#include "buildsym.h"
#include "complaints.h"
#include "filenames.h"
#include "gdbarch.h"

static void
dwarf2_debug_line_missing_file_complaint ()
{
  complaint (_(".debug_line section has line data without a file"));
}

static void
dwarf2_debug_line_missing_end_sequence_complaint ()
{
  complaint (_(".debug_line section has line "
	       "program sequence without an end"));
}

/* State machine to track the state of the line number program.  */

class lnp_state_machine
{
public:
  /* Initialize a machine state for the start of a line number
     program.  */
  lnp_state_machine (struct dwarf2_cu *cu, gdbarch *arch, line_header *lh);

  file_entry *current_file ()
  {
    /* lh->file_names is 0-based, but the file name numbers in the
       statement program are 1-based.  */
    return m_line_header->file_name_at (m_file);
  }

  /* Record the line in the state machine.  END_SEQUENCE is true if
     we're processing the end of a sequence.  */
  void record_line (bool end_sequence);

  /* Check ADDRESS is -1, -2, or zero and less than UNRELOCATED_LOWPC, and if
     true nop-out rest of the lines in this sequence.  */
  void check_line_address (struct dwarf2_cu *cu,
			   const gdb_byte *line_ptr,
			   unrelocated_addr unrelocated_lowpc,
			   unrelocated_addr address);

  void handle_set_discriminator (unsigned int discriminator)
  {
    m_discriminator = discriminator;
    m_line_has_non_zero_discriminator |= discriminator != 0;
  }

  /* Handle DW_LNE_set_address.  */
  void handle_set_address (unrelocated_addr address)
  {
    m_op_index = 0;
    m_address
      = (unrelocated_addr) gdbarch_adjust_dwarf2_line (m_gdbarch,
						       (CORE_ADDR) address,
						       false);
  }

  /* Handle DW_LNS_advance_pc.  */
  void handle_advance_pc (CORE_ADDR adjust);

  /* Handle a special opcode.  */
  void handle_special_opcode (unsigned char op_code);

  /* Handle DW_LNS_advance_line.  */
  void handle_advance_line (int line_delta)
  {
    advance_line (line_delta);
  }

  /* Handle DW_LNS_set_file.  */
  void handle_set_file (file_name_index file);

  /* Handle DW_LNS_negate_stmt.  */
  void handle_negate_stmt ()
  {
    m_flags ^= LEF_IS_STMT;
  }

  /* Handle DW_LNS_const_add_pc.  */
  void handle_const_add_pc ();

  /* Handle DW_LNS_fixed_advance_pc.  */
  void handle_fixed_advance_pc (CORE_ADDR addr_adj)
  {
    addr_adj = gdbarch_adjust_dwarf2_line (m_gdbarch, addr_adj, true);
    m_address = (unrelocated_addr) ((CORE_ADDR) m_address + addr_adj);
    m_op_index = 0;
  }

  /* Handle DW_LNS_copy.  */
  void handle_copy ()
  {
    record_line (false);
    m_discriminator = 0;
    m_flags &= ~LEF_PROLOGUE_END;
    m_flags &= ~LEF_EPILOGUE_BEGIN;
  }

  /* Handle DW_LNE_end_sequence.  */
  void handle_end_sequence ()
  {
    m_currently_recording_lines = true;
  }

  /* Handle DW_LNS_set_prologue_end.  */
  void handle_set_prologue_end ()
  {
    m_flags |= LEF_PROLOGUE_END;
  }

  void handle_set_epilogue_begin ()
  {
    m_flags |= LEF_EPILOGUE_BEGIN;
  }

private:
  /* Advance the line by LINE_DELTA.  */
  void advance_line (int line_delta)
  {
    m_line += line_delta;

    if (line_delta != 0)
      m_line_has_non_zero_discriminator = m_discriminator != 0;
  }

  bool record_line_p ();
  void finish_line ();
  void record_line_1 (unsigned int line, linetable_entry_flags flags);

  struct dwarf2_cu *m_cu;

  /* The builder associated with the CU.  */
  buildsym_compunit *m_builder;

  gdbarch *m_gdbarch;

  /* The line number header.  */
  line_header *m_line_header;

  /* These are part of the standard DWARF line number state machine,
     and initialized according to the DWARF spec.  */

  unsigned char m_op_index = 0;
  /* The line table index of the current file.  */
  file_name_index m_file = 1;
  unsigned int m_line = 1;

  /* These are initialized in the constructor.  */

  unrelocated_addr m_address;
  linetable_entry_flags m_flags;
  unsigned int m_discriminator = 0;

  /* Additional bits of state we need to track.  */

  /* The last file a line number was recorded for.  */
  struct subfile *m_last_subfile = NULL;

  /* The address of the last line entry.  */
  unrelocated_addr m_last_address;

  /* Set to true when a previous line at the same address (using
     m_last_address) had LEF_IS_STMT set in m_flags.  This is reset to false
     when a line entry at a new address (m_address different to
     m_last_address) is processed.  */
  bool m_stmt_at_address = false;

  /* When true, record the lines we decode.  */
  bool m_currently_recording_lines = true;

  /* The last line number that was recorded, used to coalesce
     consecutive entries for the same line.  This can happen, for
     example, when discriminators are present.  PR 17276.  */
  unsigned int m_last_line = 0;
  bool m_line_has_non_zero_discriminator = false;
};

void
lnp_state_machine::handle_advance_pc (CORE_ADDR adjust)
{
  CORE_ADDR addr_adj = (((m_op_index + adjust)
			 / m_line_header->maximum_ops_per_instruction)
			* m_line_header->minimum_instruction_length);
  addr_adj = gdbarch_adjust_dwarf2_line (m_gdbarch, addr_adj, true);
  m_address = (unrelocated_addr) ((CORE_ADDR) m_address + addr_adj);
  m_op_index = ((m_op_index + adjust)
		% m_line_header->maximum_ops_per_instruction);
}

void
lnp_state_machine::handle_special_opcode (unsigned char op_code)
{
  unsigned char adj_opcode = op_code - m_line_header->opcode_base;
  unsigned char adj_opcode_d = adj_opcode / m_line_header->line_range;
  unsigned char adj_opcode_r = adj_opcode % m_line_header->line_range;
  CORE_ADDR addr_adj = (((m_op_index + adj_opcode_d)
			 / m_line_header->maximum_ops_per_instruction)
			* m_line_header->minimum_instruction_length);
  addr_adj = gdbarch_adjust_dwarf2_line (m_gdbarch, addr_adj, true);
  m_address = (unrelocated_addr) ((CORE_ADDR) m_address + addr_adj);
  m_op_index = ((m_op_index + adj_opcode_d)
		% m_line_header->maximum_ops_per_instruction);

  int line_delta = m_line_header->line_base + adj_opcode_r;
  advance_line (line_delta);
  record_line (false);
  m_discriminator = 0;
  m_flags &= ~LEF_PROLOGUE_END;
  m_flags &= ~LEF_EPILOGUE_BEGIN;
}

void
lnp_state_machine::handle_set_file (file_name_index file)
{
  m_file = file;

  const file_entry *fe = current_file ();
  if (fe == NULL)
    dwarf2_debug_line_missing_file_complaint ();
  else
    {
      m_line_has_non_zero_discriminator = m_discriminator != 0;
      dwarf2_start_subfile (m_cu, *fe, *m_line_header);
    }
}

void
lnp_state_machine::handle_const_add_pc ()
{
  CORE_ADDR adjust
    = (255 - m_line_header->opcode_base) / m_line_header->line_range;

  CORE_ADDR addr_adj
    = (((m_op_index + adjust)
	/ m_line_header->maximum_ops_per_instruction)
       * m_line_header->minimum_instruction_length);

  addr_adj = gdbarch_adjust_dwarf2_line (m_gdbarch, addr_adj, true);
  m_address = (unrelocated_addr) ((CORE_ADDR) m_address + addr_adj);
  m_op_index = ((m_op_index + adjust)
		% m_line_header->maximum_ops_per_instruction);
}

/* Return true if we should add LINE to the line number table.
   LINE is the line to add, LAST_LINE is the last line that was added,
   LAST_SUBFILE is the subfile for LAST_LINE.
   LINE_HAS_NON_ZERO_DISCRIMINATOR is non-zero if LINE has ever
   had a non-zero discriminator.

   We have to be careful in the presence of discriminators.
   E.g., for this line:

     for (i = 0; i < 100000; i++);

   clang can emit four line number entries for that one line,
   each with a different discriminator.
   See gdb.dwarf2/dw2-single-line-discriminators.exp for an example.

   However, we want gdb to coalesce all four entries into one.
   Otherwise the user could stepi into the middle of the line and
   gdb would get confused about whether the pc really was in the
   middle of the line.

   Things are further complicated by the fact that two consecutive
   line number entries for the same line is a heuristic used by gcc
   to denote the end of the prologue.  So we can't just discard duplicate
   entries, we have to be selective about it.  The heuristic we use is
   that we only collapse consecutive entries for the same line if at least
   one of those entries has a non-zero discriminator.  PR 17276.

   Note: Addresses in the line number state machine can never go backwards
   within one sequence, thus this coalescing is ok.  */

bool
lnp_state_machine::record_line_p ()
{
  if (m_builder->get_current_subfile () != m_last_subfile)
    return true;
  if (m_line != m_last_line)
    return true;
  /* Same line for the same file that we've seen already.
     As a last check, for pr 17276, only record the line if the line
     has never had a non-zero discriminator.  */
  if (!m_line_has_non_zero_discriminator)
    return true;
  return false;
}

/* Use the CU's builder to record line number LINE with the given
   flags.  */

void
lnp_state_machine::record_line_1 (unsigned int line,
				  linetable_entry_flags flags)
{
  if (m_currently_recording_lines)
    {
      unrelocated_addr addr
	= unrelocated_addr (gdbarch_addr_bits_remove (m_gdbarch,
						      (CORE_ADDR) m_address));

      if (dwarf_line_debug)
	gdb_printf (gdb_stdlog, "Recording line %u, file %s, address %s\n",
		    m_line, lbasename (m_last_subfile->name.c_str ()),
		    paddress (m_gdbarch, (CORE_ADDR) addr));

      m_builder->record_line (m_last_subfile, line, addr, flags);
    }
}

/* Subroutine of dwarf_decode_lines_1 to simplify it.
   Mark the end of a set of line number records.  */

void
lnp_state_machine::finish_line ()
{
  if (m_last_subfile == nullptr)
    return;

  if (dwarf_line_debug)
    {
      gdb_printf (gdb_stdlog,
		  "Finishing current line, file %s, address %s\n",
		  lbasename (m_last_subfile->name.c_str ()),
		  paddress (m_gdbarch, (CORE_ADDR) m_address));
    }

  record_line_1 (0, LEF_IS_STMT);
}

void
lnp_state_machine::record_line (bool end_sequence)
{
  if (dwarf_line_debug)
    {
      gdb_printf (gdb_stdlog,
		  "Processing actual line %u: file %u,"
		  " address %s, is_stmt %u, prologue_end %u,"
		  " epilogue_begin %u, discrim %u%s\n",
		  m_line, m_file,
		  paddress (m_gdbarch, (CORE_ADDR) m_address),
		  (m_flags & LEF_IS_STMT) != 0,
		  (m_flags & LEF_PROLOGUE_END) != 0,
		  (m_flags & LEF_EPILOGUE_BEGIN) != 0,
		  m_discriminator,
		  (end_sequence ? "\t(end sequence)" : ""));
    }

  file_entry *fe = current_file ();

  if (fe == NULL)
    dwarf2_debug_line_missing_file_complaint ();
  /* For now we ignore lines not starting on an instruction boundary.
     But not when processing end_sequence for compatibility with the
     previous version of the code.  */
  else if (m_op_index == 0 || end_sequence)
    {
      /* When we switch files we insert an end maker in the first file,
	 switch to the second file and add a new line entry.  The
	 problem is that the end marker inserted in the first file will
	 discard any previous line entries at the same address.  If the
	 line entries in the first file are marked as is-stmt, while
	 the new line in the second file is non-stmt, then this means
	 the end marker will discard is-stmt lines so we can have a
	 non-stmt line.  This means that there are less addresses at
	 which the user can insert a breakpoint.

	 To improve this we track the last address in m_last_address,
	 and whether we have seen an is-stmt at this address.  Then
	 when switching files, if we have seen a stmt at the current
	 address, and we are switching to create a non-stmt line, then
	 discard the new line.  */
      bool file_changed = m_last_subfile != m_builder->get_current_subfile ();
      bool ignore_this_line
	= ((file_changed && !end_sequence && m_last_address == m_address
	    && ((m_flags & LEF_IS_STMT) == 0)
	    && m_stmt_at_address)
	   || (!end_sequence && m_line == 0));

      if ((file_changed && !ignore_this_line) || end_sequence)
	finish_line ();

      if (!end_sequence && !ignore_this_line)
	{
	  linetable_entry_flags lte_flags = m_flags;
	  if (m_cu->producer_is_codewarrior ())
	    lte_flags |= LEF_IS_STMT;

	  if (record_line_p ())
	    {
	      m_last_subfile = m_builder->get_current_subfile ();
	      record_line_1 (m_line, lte_flags);
	      m_last_line = m_line;
	    }
	}
    }

  /* Track whether we have seen any IS_STMT true at m_address in case we
     have multiple line table entries all at m_address.  */
  if (m_last_address != m_address)
    {
      m_stmt_at_address = false;
      m_last_address = m_address;
    }
  m_stmt_at_address |= (m_flags & LEF_IS_STMT) != 0;
}

lnp_state_machine::lnp_state_machine (struct dwarf2_cu *cu, gdbarch *arch,
				      line_header *lh)
  : m_cu (cu),
    m_builder (cu->get_builder ()),
    m_gdbarch (arch),
    m_line_header (lh),
    /* Call `gdbarch_adjust_dwarf2_line' on the initial 0 address as
       if there was a line entry for it so that the backend has a
       chance to adjust it and also record it in case it needs it.
       This is currently used by MIPS code,
       cf. `mips_adjust_dwarf2_line'.  */
    m_address ((unrelocated_addr) gdbarch_adjust_dwarf2_line (arch, 0, 0)),
    m_flags (lh->default_is_stmt ? LEF_IS_STMT : (linetable_entry_flags) 0),
    m_last_address (m_address)
{
}

void
lnp_state_machine::check_line_address (struct dwarf2_cu *cu,
				       const gdb_byte *line_ptr,
				       unrelocated_addr unrelocated_lowpc,
				       unrelocated_addr address)
{
  /* Linkers resolve a symbolic relocation referencing a GC'd function to 0,
     -1 or -2 (-2 is used by certain lld versions, see
     https://github.com/llvm/llvm-project/commit/e618ccbf431f6730edb6d1467a127c3a52fd57f7).
     If ADDRESS is 0, ignoring the opcode will err if the text section is
     located at 0x0.  In this case, additionally check that if
     ADDRESS < UNRELOCATED_LOWPC.  */

  if ((address == (unrelocated_addr) 0 && address < unrelocated_lowpc)
      || address == (unrelocated_addr) -1
      || address == (unrelocated_addr) -2)
    {
      /* This line table is for a function which has been
	 GCd by the linker.  Ignore it.  PR gdb/12528 */

      struct objfile *objfile = cu->per_objfile->objfile;
      long line_offset = line_ptr - get_debug_line_section (cu)->buffer;

      complaint (_(".debug_line address at offset 0x%lx is 0 [in module %s]"),
		 line_offset, objfile_name (objfile));
      m_currently_recording_lines = false;
      /* Note: m_currently_recording_lines is left as false until we see
	 DW_LNE_end_sequence.  */
    }
}

/* Subroutine of dwarf_decode_lines to simplify it.
   Process the line number information in LH.  */

static void
dwarf_decode_lines_1 (struct line_header *lh, struct dwarf2_cu *cu,
		      unrelocated_addr lowpc)
{
  const gdb_byte *line_ptr, *extended_end;
  const gdb_byte *line_end;
  unsigned int bytes_read, extended_len;
  unsigned char op_code, extended_op;
  struct objfile *objfile = cu->per_objfile->objfile;
  bfd *abfd = objfile->obfd.get ();
  struct gdbarch *gdbarch = objfile->arch ();

  line_ptr = lh->statement_program_start;
  line_end = lh->statement_program_end;

  /* Read the statement sequences until there's nothing left.  */
  while (line_ptr < line_end)
    {
      /* The DWARF line number program state machine.  Reset the state
	 machine at the start of each sequence.  */
      lnp_state_machine state_machine (cu, gdbarch, lh);
      bool end_sequence = false;

      /* Start a subfile for the current file of the state
	 machine.  */
      const file_entry *fe = state_machine.current_file ();

      if (fe != NULL)
	dwarf2_start_subfile (cu, *fe, *lh);

      /* Decode the table.  */
      while (line_ptr < line_end && !end_sequence)
	{
	  op_code = read_1_byte (abfd, line_ptr);
	  line_ptr += 1;

	  if (op_code >= lh->opcode_base)
	    {
	      /* Special opcode.  */
	      state_machine.handle_special_opcode (op_code);
	    }
	  else switch (op_code)
	    {
	    case DW_LNS_extended_op:
	      extended_len = read_unsigned_leb128 (abfd, line_ptr,
						   &bytes_read);
	      line_ptr += bytes_read;
	      extended_end = line_ptr + extended_len;
	      extended_op = read_1_byte (abfd, line_ptr);
	      line_ptr += 1;
	      if (DW_LNE_lo_user <= extended_op
		  && extended_op <= DW_LNE_hi_user)
		{
		  /* Vendor extension, ignore.  */
		  line_ptr = extended_end;
		  break;
		}
	      switch (extended_op)
		{
		case DW_LNE_end_sequence:
		  state_machine.handle_end_sequence ();
		  end_sequence = true;
		  break;
		case DW_LNE_set_address:
		  {
		    unrelocated_addr address
		      = cu->header.read_address (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;

		    state_machine.check_line_address (cu, line_ptr, lowpc,
						      address);
		    state_machine.handle_set_address (address);
		  }
		  break;
		case DW_LNE_define_file:
		  {
		    const char *cur_file;
		    unsigned int mod_time, length;
		    dir_index dindex;

		    cur_file = read_direct_string (abfd, line_ptr,
						   &bytes_read);
		    line_ptr += bytes_read;
		    dindex = (dir_index)
		      read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;
		    mod_time =
		      read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;
		    length =
		      read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;
		    lh->add_file_name (cur_file, dindex, mod_time, length);
		  }
		  break;
		case DW_LNE_set_discriminator:
		  {
		    /* The discriminator is not interesting to the
		       debugger; just ignore it.  We still need to
		       check its value though:
		       if there are consecutive entries for the same
		       (non-prologue) line we want to coalesce them.
		       PR 17276.  */
		    unsigned int discr
		      = read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;

		    state_machine.handle_set_discriminator (discr);
		  }
		  break;
		default:
		  complaint (_("mangled .debug_line section"));
		  return;
		}
	      /* Make sure that we parsed the extended op correctly.  If e.g.
		 we expected a different address size than the producer used,
		 we may have read the wrong number of bytes.  */
	      if (line_ptr != extended_end)
		{
		  complaint (_("mangled .debug_line section"));
		  return;
		}
	      break;
	    case DW_LNS_copy:
	      state_machine.handle_copy ();
	      break;
	    case DW_LNS_advance_pc:
	      {
		CORE_ADDR adjust
		  = read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		line_ptr += bytes_read;

		state_machine.handle_advance_pc (adjust);
	      }
	      break;
	    case DW_LNS_advance_line:
	      {
		int line_delta
		  = read_signed_leb128 (abfd, line_ptr, &bytes_read);
		line_ptr += bytes_read;

		state_machine.handle_advance_line (line_delta);
	      }
	      break;
	    case DW_LNS_set_file:
	      {
		file_name_index file
		  = (file_name_index) read_unsigned_leb128 (abfd, line_ptr,
							    &bytes_read);
		line_ptr += bytes_read;

		state_machine.handle_set_file (file);
	      }
	      break;
	    case DW_LNS_set_column:
	      (void) read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
	      line_ptr += bytes_read;
	      break;
	    case DW_LNS_negate_stmt:
	      state_machine.handle_negate_stmt ();
	      break;
	    case DW_LNS_set_basic_block:
	      break;
	    /* Add to the address register of the state machine the
	       address increment value corresponding to special opcode
	       255.  I.e., this value is scaled by the minimum
	       instruction length since special opcode 255 would have
	       scaled the increment.  */
	    case DW_LNS_const_add_pc:
	      state_machine.handle_const_add_pc ();
	      break;
	    case DW_LNS_fixed_advance_pc:
	      {
		CORE_ADDR addr_adj = read_2_bytes (abfd, line_ptr);
		line_ptr += 2;

		state_machine.handle_fixed_advance_pc (addr_adj);
	      }
	      break;
	    case DW_LNS_set_prologue_end:
	      state_machine.handle_set_prologue_end ();
	      break;
	    case DW_LNS_set_epilogue_begin:
	      state_machine.handle_set_epilogue_begin ();
	      break;
	    default:
	      {
		/* Unknown standard opcode, ignore it.  */
		int i;

		for (i = 0; i < lh->standard_opcode_lengths[op_code]; i++)
		  {
		    (void) read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		    line_ptr += bytes_read;
		  }
	      }
	    }
	}

      if (!end_sequence)
	dwarf2_debug_line_missing_end_sequence_complaint ();

      /* We got a DW_LNE_end_sequence (or we ran off the end of the buffer,
	 in which case we still finish recording the last line).  */
      state_machine.record_line (true);
    }
}

/* See dwarf2/line-program.h.  */

void
dwarf_decode_lines (struct line_header *lh, struct dwarf2_cu *cu,
		    unrelocated_addr lowpc, bool decode_mapping)
{
  if (decode_mapping)
    dwarf_decode_lines_1 (lh, cu, lowpc);

  /* Make sure a symtab is created for every file, even files
     which contain only variables (i.e. no code with associated
     line numbers).  */
  buildsym_compunit *builder = cu->get_builder ();
  struct compunit_symtab *cust = builder->get_compunit_symtab ();

  for (auto &fe : lh->file_names ())
    {
      dwarf2_start_subfile (cu, fe, *lh);
      subfile *sf = builder->get_current_subfile ();

      if (sf->symtab == nullptr)
	sf->symtab = allocate_symtab (cust, sf->name.c_str (),
				      sf->name_for_id.c_str ());

      fe.symtab = sf->symtab;
    }
}
