/* DWARF 2 debugging format support for GDB.

   Copyright (C) 1994-2023 Free Software Foundation, Inc.

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

#include "defs.h"
#include "dwarf2/comp-unit-head.h"
#include "dwarf2/leb.h"
#include "dwarf2/line-header.h"
#include "dwarf2/read.h"
#include "complaints.h"
#include "filenames.h"
#include "gdbsupport/pathstuff.h"

void
line_header::add_include_dir (const char *include_dir)
{
  if (dwarf_line_debug >= 2)
    {
      size_t new_size;
      if (version >= 5)
	new_size = m_include_dirs.size ();
      else
	new_size = m_include_dirs.size () + 1;
      gdb_printf (gdb_stdlog, "Adding dir %zu: %s\n",
		  new_size, include_dir);
    }
  m_include_dirs.push_back (include_dir);
}

void
line_header::add_file_name (const char *name,
			    dir_index d_index,
			    unsigned int mod_time,
			    unsigned int length)
{
  file_name_index index
    = version >= 5 ? file_names_size (): file_names_size () + 1;

  if (dwarf_line_debug >= 2)
    gdb_printf (gdb_stdlog, "Adding file %d: %s\n", index, name);

  m_file_names.emplace_back (name, index, d_index, mod_time, length);
}

std::string
line_header::file_file_name (const file_entry &fe) const
{
  gdb_assert (is_valid_file_index (fe.index));

  std::string ret = fe.name;

  if (IS_ABSOLUTE_PATH (ret))
    return ret;

  const char *dir = fe.include_dir (this);
  if (dir != nullptr)
    ret = path_join (dir, ret.c_str ());

  if (IS_ABSOLUTE_PATH (ret))
    return ret;

  if (m_comp_dir != nullptr)
    ret = path_join (m_comp_dir, ret.c_str ());

  return ret;
}

static void
dwarf2_statement_list_fits_in_line_number_section_complaint (void)
{
  complaint (_("statement list doesn't fit in .debug_line section"));
}

/* Cover function for read_initial_length.
   Returns the length of the object at BUF, and stores the size of the
   initial length in *BYTES_READ and stores the size that offsets will be in
   *OFFSET_SIZE.
   If the initial length size is not equivalent to that specified in
   CU_HEADER then issue a complaint.
   This is useful when reading non-comp-unit headers.  */

static LONGEST
read_checked_initial_length_and_offset (bfd *abfd, const gdb_byte *buf,
					const struct comp_unit_head *cu_header,
					unsigned int *bytes_read,
					unsigned int *offset_size)
{
  LONGEST length = read_initial_length (abfd, buf, bytes_read);

  if (cu_header != nullptr)
    {
      gdb_assert (cu_header->initial_length_size == 4
		  || cu_header->initial_length_size == 8
		  || cu_header->initial_length_size == 12);

      if (cu_header->initial_length_size != *bytes_read)
	complaint (_("intermixed 32-bit and 64-bit DWARF sections"));
    }

  *offset_size = (*bytes_read == 4) ? 4 : 8;
  return length;
}


/* Like read_formatted_entries but the .debug_line and .debug_line_str
   are stored in LINE_BUFP and LINE_STR_DATA.  This is used for cases
   where these sections are read from separate files without necessarily
   having access to the entire debuginfo file they originate from.  */

static void
read_formatted_entries
  (bfd *parent_bfd, const gdb_byte **line_bufp,
   const gdb::array_view<const gdb_byte> line_str_data,
   struct line_header *lh,
   unsigned int offset_size,
   void (*callback) (struct line_header *lh,
		     const char *name,
		     dir_index d_index,
		     unsigned int mod_time,
		     unsigned int length))
{
  gdb_byte format_count, formati;
  ULONGEST data_count, datai;
  const gdb_byte *buf = *line_bufp;
  const gdb_byte *str_buf = line_str_data.data ();
  const gdb_byte *format_header_data;
  unsigned int bytes_read;

  format_count = read_1_byte (parent_bfd, buf);
  buf += 1;
  format_header_data = buf;
  for (formati = 0; formati < format_count; formati++)
    {
      read_unsigned_leb128 (parent_bfd, buf, &bytes_read);
      buf += bytes_read;
      read_unsigned_leb128 (parent_bfd, buf, &bytes_read);
      buf += bytes_read;
    }

  data_count = read_unsigned_leb128 (parent_bfd, buf, &bytes_read);
  buf += bytes_read;
  for (datai = 0; datai < data_count; datai++)
    {
      const gdb_byte *format = format_header_data;
      struct file_entry fe;

      for (formati = 0; formati < format_count; formati++)
	{
	  ULONGEST content_type = read_unsigned_leb128 (parent_bfd, format, &bytes_read);
	  format += bytes_read;

	  ULONGEST form  = read_unsigned_leb128 (parent_bfd, format, &bytes_read);
	  format += bytes_read;

	  gdb::optional<const char *> string;
	  gdb::optional<unsigned int> uint;

	  switch (form)
	    {
	    case DW_FORM_string:
	      string.emplace (read_direct_string (parent_bfd, buf, &bytes_read));
	      buf += bytes_read;
	      break;

	    case DW_FORM_line_strp:
	      {
		if (line_str_data.empty ())
		  error (_("Dwarf Error: DW_FORM_line_strp used without " \
			   "required section"));
		if (line_str_data.size () <= offset_size)
		  error (_("Dwarf Error: DW_FORM_line_strp pointing outside " \
			   "of section .debug_line"));

		ULONGEST str_offset = read_offset (parent_bfd, buf, offset_size);

		const char *str;
		if (str_buf[str_offset] == '\0')
		  str = nullptr;
		else
		  str = (const char *) (str_buf + str_offset);
		string.emplace (str);
		buf += offset_size;
		break;
	      }

	    case DW_FORM_data1:
	      uint.emplace (read_1_byte (parent_bfd, buf));
	      buf += 1;
	      break;

	    case DW_FORM_data2:
	      uint.emplace (read_2_bytes (parent_bfd, buf));
	      buf += 2;
	      break;

	    case DW_FORM_data4:
	      uint.emplace (read_4_bytes (parent_bfd, buf));
	      buf += 4;
	      break;

	    case DW_FORM_data8:
	      uint.emplace (read_8_bytes (parent_bfd, buf));
	      buf += 8;
	      break;

	    case DW_FORM_data16:
	      /*  This is used for MD5, but file_entry does not record MD5s. */
	      buf += 16;
	      break;

	    case DW_FORM_udata:
	      uint.emplace (read_unsigned_leb128 (parent_bfd, buf, &bytes_read));
	      buf += bytes_read;
	      break;

	    case DW_FORM_block:
	      /* It is valid only for DW_LNCT_timestamp which is ignored by
		 current GDB.  */
	      break;
	    }

	  /* Normalize nullptr string.  */
	  if (string.has_value () && *string == nullptr)
	    string.emplace ("");

	  switch (content_type)
	    {
	    case DW_LNCT_path:
	      if (string.has_value ())
		fe.name = *string;
	      break;
	    case DW_LNCT_directory_index:
	      if (uint.has_value ())
		fe.d_index = (dir_index) *uint;
	      break;
	    case DW_LNCT_timestamp:
	      if (uint.has_value ())
		fe.mod_time = *uint;
	      break;
	    case DW_LNCT_size:
	      if (uint.has_value ())
		fe.length = *uint;
	      break;
	    case DW_LNCT_MD5:
	      break;
	    default:
	      complaint (_("Unknown format content type %s"),
			 pulongest (content_type));
	    }
	}

      callback (lh, fe.name, fe.d_index, fe.mod_time, fe.length);
    }

  *line_bufp = buf;
}

/* See line-header.h.  */

line_header_up
dwarf_decode_line_header (bfd *parent_bfd,
			  gdb::array_view<const gdb_byte> line_data,
			  gdb::array_view<const gdb_byte> line_str_data,
			  const gdb_byte **debug_line_ptr,
			  bool is_dwz,
			  const struct comp_unit_head *cu_header,
			  const char *comp_dir)
{
  const gdb_byte *line_ptr, *buf;
  unsigned int bytes_read, offset_size;
  int i;
  const char *cur_dir, *cur_file;

  buf = *debug_line_ptr;

  /* Make sure that at least there's room for the total_length field.
     That could be 12 bytes long, but we're just going to fudge that.  */
  if (buf + 4 >= line_data.data () + line_data.size ())
    {
      dwarf2_statement_list_fits_in_line_number_section_complaint ();
      return 0;
    }

  line_header_up lh (new line_header (comp_dir));

  lh->sect_off = (sect_offset) (buf - line_data.data ());
  lh->offset_in_dwz = is_dwz;

  line_ptr = buf;

  /* Read in the header.  */
  LONGEST unit_length
    = read_checked_initial_length_and_offset (parent_bfd, buf, cu_header,
					      &bytes_read, &offset_size);

  line_ptr += bytes_read;

  if (line_ptr + unit_length > buf + line_data.size ())
    {
      dwarf2_statement_list_fits_in_line_number_section_complaint ();
      return 0;
    }

  const gdb_byte *start_here = line_ptr;

  lh->statement_program_end = start_here + unit_length;
  lh->version = read_2_bytes (parent_bfd, line_ptr);
  line_ptr += 2;
  if (lh->version > 5)
    {
      /* This is a version we don't understand.  The format could have
	 changed in ways we don't handle properly so just punt.  */
      complaint (_("unsupported version in .debug_line section"));
      return nullptr;
    }
  if (lh->version >= 5)
    {
      gdb_byte segment_selector_size;

      /* Skip address size.  */
      read_1_byte (parent_bfd, line_ptr);
      line_ptr += 1;

      segment_selector_size = read_1_byte (parent_bfd, line_ptr);
      line_ptr += 1;
      if (segment_selector_size != 0)
	{
	  complaint (_("unsupported segment selector size %u "
		       "in .debug_line section"),
		     segment_selector_size);
	  return nullptr;
	}
    }

  LONGEST header_length = read_offset (parent_bfd, line_ptr, offset_size);
  line_ptr += offset_size;
  lh->statement_program_start = line_ptr + header_length;

  lh->minimum_instruction_length = read_1_byte (parent_bfd, line_ptr);
  line_ptr += 1;

  if (lh->version >= 4)
    {
      lh->maximum_ops_per_instruction = read_1_byte (parent_bfd, line_ptr);
      line_ptr += 1;
    }
  else
    lh->maximum_ops_per_instruction = 1;

  if (lh->maximum_ops_per_instruction == 0)
    {
      lh->maximum_ops_per_instruction = 1;
      complaint (_("invalid maximum_ops_per_instruction "
		   "in `.debug_line' section"));
    }

  lh->default_is_stmt = read_1_byte (parent_bfd, line_ptr);
  line_ptr += 1;

  lh->line_base = read_1_signed_byte (parent_bfd, line_ptr);
  line_ptr += 1;

  lh->line_range = read_1_byte (parent_bfd, line_ptr);
  line_ptr += 1;

  lh->opcode_base = read_1_byte (parent_bfd, line_ptr);
  line_ptr += 1;

  lh->standard_opcode_lengths.reset (new unsigned char[lh->opcode_base]);

  lh->standard_opcode_lengths[0] = 1;  /* This should never be used anyway.  */
  for (i = 1; i < lh->opcode_base; ++i)
    {
      lh->standard_opcode_lengths[i] = read_1_byte (parent_bfd, line_ptr);
      line_ptr += 1;
    }

  if (lh->version >= 5)
    {
      /* Read directory table.  */
      read_formatted_entries
	(parent_bfd, &line_ptr, line_str_data,
	 lh.get (), offset_size,
	 [] (struct line_header *header, const char *name,
	     dir_index d_index, unsigned int mod_time,
	     unsigned int length)
	{
	  header->add_include_dir (name);
	});

      /* Read file name table.  */
      read_formatted_entries
	(parent_bfd, &line_ptr, line_str_data,
	 lh.get (), offset_size,
	 [] (struct line_header *header, const char *name,
	     dir_index d_index, unsigned int mod_time,
	     unsigned int length)
	{
	  header->add_file_name (name, d_index, mod_time, length);
	});
    }
  else
    {
      /* Read directory table.  */
      while ((cur_dir = read_direct_string (parent_bfd, line_ptr, &bytes_read)) != nullptr)
	{
	  line_ptr += bytes_read;
	  lh->add_include_dir (cur_dir);
	}
      line_ptr += bytes_read;

      /* Read file name table.  */
      while ((cur_file = read_direct_string (parent_bfd, line_ptr, &bytes_read)) != nullptr)
	{
	  unsigned int mod_time, length;
	  dir_index d_index;

	  line_ptr += bytes_read;
	  d_index = (dir_index) read_unsigned_leb128 (parent_bfd, line_ptr, &bytes_read);
	  line_ptr += bytes_read;
	  mod_time = read_unsigned_leb128 (parent_bfd, line_ptr, &bytes_read);
	  line_ptr += bytes_read;
	  length = read_unsigned_leb128 (parent_bfd, line_ptr, &bytes_read);
	  line_ptr += bytes_read;

	  lh->add_file_name (cur_file, d_index, mod_time, length);
	}
      line_ptr += bytes_read;
    }

  if (line_ptr > (buf + line_data.size ()))
    complaint (_("line number info header doesn't "
		 "fit in `.debug_line' section"));

  *debug_line_ptr += unit_length + offset_size;
  return lh;
}

line_header_up
dwarf_decode_line_header  (sect_offset sect_off, bool is_dwz,
			   dwarf2_per_objfile *per_objfile,
			   struct dwarf2_section_info *section,
			   const struct comp_unit_head *cu_header,
			   const char *comp_dir)
{
  struct objfile *objfile = per_objfile->objfile;
  struct dwarf2_per_bfd *per_bfd = per_objfile->per_bfd;

  /* Read .debug_line.  */
  dwarf2_section_info *line_sec = &per_bfd->line;
  bfd_size_type line_size = line_sec->get_size (objfile);

  gdb::array_view<const gdb_byte> line (line_sec->buffer, line_size);

  /* Read .debug_line_str.  */
  dwarf2_section_info *line_str_sec = &per_bfd->line_str;
  bfd_size_type line_str_size = line_str_sec->get_size (objfile);

  gdb::array_view<const gdb_byte> line_str (line_str_sec->buffer,
					    line_str_size);

  const gdb_byte *line_ptr = line.data () + to_underlying (sect_off);

  return dwarf_decode_line_header
    (per_bfd->obfd, line, line_str, &line_ptr,
     is_dwz, cu_header, comp_dir);
}
