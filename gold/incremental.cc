// inremental.cc -- incremental linking support for gold

// Copyright 2009 Free Software Foundation, Inc.
// Written by Mikolaj Zalewski <mikolajz@google.com>.

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

using elfcpp::Convert;

namespace gold {

// Version information. Will change frequently during the development, later
// we could think about backward (and forward?) compatibility.
const int INCREMENTAL_LINK_VERSION = 1;

namespace internal {

// Header of the .gnu_incremental_input section.
struct Incremental_input_header_data
{
  // Incremental linker version.
  elfcpp::Elf_Word version;

  // Numer of input files in the link.
  elfcpp::Elf_Word input_file_count;

  // Offset of command line options in .gnu_incremental_strtab.
  elfcpp::Elf_Word command_line_offset;

  // Padding.
  elfcpp::Elf_Word reserved;
};

// Data stored in .gnu_incremental_input after the header for each of the
// Incremental_input_header_data::input_file_count input entries.
struct Incremental_input_entry_data
{
  // Offset of file name in .gnu_incremental_strtab section.
  elfcpp::Elf_Word filename_offset;

  // Offset of data in .gnu_incremental_input.
  elfcpp::Elf_Word data_offset;

  // Timestamp (in seconds).
  elfcpp::Elf_Xword timestamp_sec;

  // Nano-second part of timestamp (if supported).
  elfcpp::Elf_Word timestamp_usec;

  // Type of the input entry.
  elfcpp::Elf_Half input_type;

  // Padding.
  elfcpp::Elf_Half reserved;
};

}

// Accessors.

// See internal::Incremental_input_header for fields descriptions.
template<int size, bool big_endian>
class Incremental_input_header_write
{
 public:
  Incremental_input_header_write(unsigned char *p)
    : p_(reinterpret_cast<internal::Incremental_input_header_data>(p))
  { }

  void
  put_version(elfcpp::Elf_Word v)
  { this->p_->version = Convert<32, big_endian>::convert_host(v); }

  void
  input_file_count(elfcpp::Elf_Word v)
  { this->p_->input_file_count = Convert<32, big_endian>::convert_host(v); }

  void
  command_line_offset(elfcpp::Elf_Word v)
  { this->p_->command_line_offset = Convert<32, big_endian>::convert_host(v); }

  void
  reserved(elfcpp::Elf_Word v)
  { this->p_->reserved = Convert<32, big_endian>::convert_host(v); }

 private:
  internal::Incremental_input_header_data* p_;
};

// See internal::Incremental_input_entry for fields descriptions.
template<int size, bool big_endian>
class Incremental_input_entry_write
{
 public:
  Incremental_input_entry_write(unsigned char *p)
    : p_(reinterpret_cast<internal::Incremental_input_entry_data>(p))
  { }

  void
  put_filename_offset(elfcpp::Elf_Word v)
  { this->p_->filename_offset = Convert<32, big_endian>::convert_host(v); }

  void
  put_data_offset(elfcpp::Elf_Word v)
  { this->p_->data_offset = Convert<32, big_endian>::convert_host(v); }

  void
  put_timestamp_sec(elfcpp::Elf_Word v)
  { this->p_->timestamp_sec = Convert<32, big_endian>::convert_host(v); }

  void
  put_timestamp_usec(elfcpp::Elf_Word v)
  { this->p_->timestamp_usec = Convert<32, big_endian>::convert_host(v); }

  void
  put_input_type(elfcpp::Elf_Word v)
  { this->p_->input_type = Convert<32, big_endian>::convert_host(v); }

  void
  put_reserved(elfcpp::Elf_Word v)
  { this->p_->reserved = Convert<32, big_endian>::convert_host(v); }

 private:
  internal::Incremental_input_entry_data* p_;
};

} // End namespace gold.
