// ehframe.cc -- handle exception frame sections for gold

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"

#include "elfcpp.h"
#include "dwarf.h"
#include "ehframe.h"

namespace gold
{

// This file handles generation of the exception frame header that
// gcc's runtime support libraries use to find unwind information at
// runtime.

// The exception frame header starts with four bytes:

// 0: The version number, currently 1.

// 1: The encoding of the pointer to the exception frames.  This can
//    be any DWARF unwind encoding (DW_EH_PE_*).  It is normally a 4
//    byte PC relative offset (DW_EH_PE_pcrel | DW_EH_PE_sdata4).

// 2: The encoding of the count of the number of FDE pointers in the
//    lookup table.  This can be any DWARF unwind encoding, and in
//    particular can be DW_EH_PE_omit if the count is omitted.  It is
//    normally a 4 byte unsigned count (DW_EH_PE_udata4).

// 3: The encoding of the lookup table entries.  Currently gcc's
//    libraries will only support DW_EH_PE_datarel | DW_EH_PE_sdata4,
//    which means that the values are 4 byte offsets from the start of
//    the table.

// The exception frame header is followed by a pointer to the contents
// of the exception frame section (.eh_frame).  This pointer is
// encoded as specified in the byte at offset 1 of the header (i.e.,
// it is normally a 4 byte PC relative offset).

// If there is a lookup table, this is followed by the count of the
// number of FDE pointers, encoded as specified in the byte at offset
// 2 of the header (i.e., normally a 4 byte unsigned integer).

// This is followed by the table, which should start at an 4-byte
// aligned address in memory.  Each entry in the table is 8 bytes.
// Each entry represents an FDE.  The first four bytes of each entry
// are an offset to the starting PC for the FDE.  The last four bytes
// of each entry are an offset to the FDE data.  The offsets are from
// the start of the exception frame header information.  The entries
// are in sorted order by starting PC.

// FIXME: We currently always generate an empty exception frame
// header.

const int eh_frame_hdr_size = 4;

// Construct the exception frame header.

Eh_frame_hdr::Eh_frame_hdr(const Target* target,
			   Output_section* eh_frame_section)
  : Output_section_data(4),
    target_(target), eh_frame_section_(eh_frame_section)
{
}

// Set the final address and size of the exception frame header.

void
Eh_frame_hdr::do_set_address(uint64_t, off_t)
{
  this->set_data_size(eh_frame_hdr_size + 4);
}

// Write the data to the flie.

void
Eh_frame_hdr::do_write(Output_file* of)
{
  const off_t off = this->offset();
  const off_t oview_size = this->data_size();
  unsigned char* const oview = of->get_output_view(off, oview_size);

  // Version number.
  oview[0] = 1;

  // Write out a 4 byte PC relative offset to the address of the
  // .eh_frame section.
  oview[1] = elfcpp::DW_EH_PE_pcrel | elfcpp::DW_EH_PE_sdata4;
  uint64_t eh_frame_address = this->eh_frame_section_->address();
  uint64_t eh_frame_hdr_address = this->address();
  uint64_t eh_frame_offset = (eh_frame_address -
			      (eh_frame_hdr_address + 4));
  if (this->target_->is_big_endian())
    elfcpp::Swap<32, true>::writeval(oview + 4, eh_frame_offset);
  else
    elfcpp::Swap<32, false>::writeval(oview + 4, eh_frame_offset);

  // We don't currently write out the sorted table.
  oview[2] = elfcpp::DW_EH_PE_omit;
  oview[3] = elfcpp::DW_EH_PE_omit;

  gold_assert(oview_size == 8);

  of->write_output_view(off, oview_size, oview);
}

} // End namespace gold.
