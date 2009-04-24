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
#include "output.h"
#include "incremental.h"

using elfcpp::Convert;

namespace gold {

// Version information. Will change frequently during the development, later
// we could think about backward (and forward?) compatibility.
const int INCREMENTAL_LINK_VERSION = 1;

namespace internal {

// Header of the .gnu_incremental_input section.
struct Incremental_inputs_header_data
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
struct Incremental_inputs_entry_data
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
class Incremental_inputs_header_write
{
 public:
  Incremental_inputs_header_write(unsigned char *p)
    : p_(reinterpret_cast<internal::Incremental_inputs_header_data*>(p))
  { }
  
  static const int data_size = sizeof(internal::Incremental_inputs_header_data);

  void
  put_version(elfcpp::Elf_Word v)
  { this->p_->version = Convert<32, big_endian>::convert_host(v); }

  void
  put_input_file_count(elfcpp::Elf_Word v)
  { this->p_->input_file_count = Convert<32, big_endian>::convert_host(v); }

  void
  put_command_line_offset(elfcpp::Elf_Word v)
  { this->p_->command_line_offset = Convert<32, big_endian>::convert_host(v); }

  void
  put_reserved(elfcpp::Elf_Word v)
  { this->p_->reserved = Convert<32, big_endian>::convert_host(v); }

 private:
  internal::Incremental_inputs_header_data* p_;
};

// See internal::Incremental_input_entry for fields descriptions.
template<int size, bool big_endian>
class Incremental_inputs_entry_write
{
 public:
  Incremental_inputs_entry_write(unsigned char *p)
    : p_(reinterpret_cast<internal::Incremental_inputs_entry_data*>(p))
  { }

  static const int data_size = sizeof(internal::Incremental_inputs_entry_data);

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
  internal::Incremental_inputs_entry_data* p_;
};

// Add the command line to the string table, setting
// command_line_key_.  In incremental builds, the command line is
// stored in .gnu_incremental_inputs so that the next linker run can
// check if the command line options didn't change.

void
Incremental_inputs::report_command_line(int argc, const char* const* argv)
{
  // Always store 'gold' as argv[0] to avoid a full relink if the user used a
  // different path to the linker.
  std::string args("gold");
  // Copied from collect_argv in main.cc.
  for (int i = 1; i < argc; ++i)
    {
      args.append(" '");
      // Now append argv[i], but with all single-quotes escaped
      const char* argpos = argv[i];
      while (1)
        {
          const int len = strcspn(argpos, "'");
          args.append(argpos, len);
          if (argpos[len] == '\0')
            break;
          args.append("'\"'\"'");
          argpos += len + 1;
        }
      args.append("'");
    }
  this->strtab_->add(args.c_str(), true, &this->command_line_key_);
}

// Finalize the incremental link information.  Called from
// Layout::finalize.

void
Incremental_inputs::finalize()
{
  this->strtab_->set_string_offsets();
}

// Create the content of the .gnu_incremental_inputs section.

Output_section_data*
Incremental_inputs::create_incremental_inputs_section_data()
{
  switch (parameters->size_and_endianness())
    {
#ifdef HAVE_TARGET_32_LITTLE
    case Parameters::TARGET_32_LITTLE:
      return this->sized_create_inputs_section_data<32, false>();
#endif
#ifdef HAVE_TARGET_32_BIG
    case Parameters::TARGET_32_BIG:
      return this->sized_create_inputs_section_data<32, true>();
#endif
#ifdef HAVE_TARGET_64_LITTLE
    case Parameters::TARGET_64_LITTLE:
      return this->sized_create_inputs_section_data<64, false>();
#endif
#ifdef HAVE_TARGET_64_BIG
    case Parameters::TARGET_64_BIG:
      return this->sized_create_inputs_section_data<64, true>();
#endif
    default:
      gold_unreachable();
    }  
}

// Sized creation of .gnu_incremental_inputs section.

template<int size, bool big_endian>
Output_section_data*
Incremental_inputs::sized_create_inputs_section_data()
{  
  unsigned int sz =
      Incremental_inputs_header_write<size, big_endian>::data_size;
  unsigned char* buffer = new unsigned char[sz];
  Incremental_inputs_header_write<size, big_endian> header_writer(buffer);
  
  gold_assert(this->command_line_key_ > 0);
  int cmd_offset = this->strtab_->get_offset_from_key(this->command_line_key_);
  
  header_writer.put_version(INCREMENTAL_LINK_VERSION);
  header_writer.put_input_file_count(0);   // TODO: store input files data.
  header_writer.put_command_line_offset(cmd_offset);
  header_writer.put_reserved(0);
  
  return new Output_data_const_buffer(buffer, sz, 8,
      "** incremental link inputs list");
}

} // End namespace gold.
