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

// Record that the input argument INPUT is an achive ARCHIVE.  This is
// called by Read_symbols after finding out the type of the file.

void
Incremental_inputs::report_archive(const Input_argument* input,
                                   Archive* archive)
{
  Hold_lock hl(*this->lock_);

  Input_info info;
  info.type = INCREMENTAL_INPUT_ARCHIVE;
  info.archive = archive;
  inputs_map_.insert(std::make_pair(input, info));
}

// Record that the input argument INPUT is an object OBJ.  This is
// called by Read_symbols after finding out the type of the file.

void
Incremental_inputs::report_object(const Input_argument* input,
                                  Object* obj)
{
  Hold_lock hl(*this->lock_);

  Input_info info;
  info.type = (obj->is_dynamic()
	       ? INCREMENTAL_INPUT_SHARED_LIBRARY
	       : INCREMENTAL_INPUT_OBJECT);
  info.object = obj;
  inputs_map_.insert(std::make_pair(input, info));
}

// Record that the input argument INPUT is an script SCRIPT.  This is
// called by read_script after parsing the script and reading the list
// of inputs added by this script.

void
Incremental_inputs::report_script(const Input_argument* input,
                                  Script_info* script)
{
  Hold_lock hl(*this->lock_);

  Input_info info;
  info.type = INCREMENTAL_INPUT_SCRIPT;
  info.script = script;
  inputs_map_.insert(std::make_pair(input, info));
}

// Compute indexes in the order in which the inputs should appear in
// .gnu_incremental_inputs.  This needs to be done after all the
// scripts are parsed.  The function is first called for the command
// line inputs arguments and may call itself recursively for e.g. a
// list of elements of a group or a list of inputs added by a script.
// The [BEGIN; END) interval to analyze and *INDEX is the current
// value of the index (that will be updated).

void
Incremental_inputs::finalize_inputs(
    Input_argument_list::const_iterator begin,
    Input_argument_list::const_iterator end,
    unsigned int* index)
{
  for (Input_argument_list::const_iterator p = begin; p != end; ++p)
    {
      if (p->is_group())
        {
          finalize_inputs(p->group()->begin(), p->group()->end(), index);
          continue;
        }

      Inputs_info_map::iterator it = inputs_map_.find(&(*p));
      // TODO: turn it into an assert when the code will be more stable.
      if (it == inputs_map_.end())
        {
          gold_error("internal error: %s: incremental build info not provided",
		     (p->is_file() ? p->file().name() : "[group]"));
          continue;
        }
      Input_info* info = &it->second;
      info->index = *index;
      (*index)++;
      this->strtab_->add(p->file().name(), false, &info->filename_key);
      if (info->type == INCREMENTAL_INPUT_SCRIPT)
        {
          finalize_inputs(info->script->inputs()->begin(),
                          info->script->inputs()->end(),
                          index);
        }
    }
}

// Finalize the incremental link information.  Called from
// Layout::finalize.

void
Incremental_inputs::finalize()
{
  unsigned int index = 0;
  finalize_inputs(this->inputs_->begin(), this->inputs_->end(), &index);

  // Sanity check.
  for (Inputs_info_map::const_iterator p = inputs_map_.begin();
       p != inputs_map_.end();
       ++p)
    {
      gold_assert(p->second.filename_key != 0);
    }

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
  const int entry_size =
      Incremental_inputs_entry_write<size, big_endian>::data_size;
  const int header_size =
      Incremental_inputs_header_write<size, big_endian>::data_size;

  unsigned int sz = header_size + entry_size * this->inputs_map_.size();
  unsigned char* buffer = new unsigned char[sz];
  unsigned char* inputs_base = buffer + header_size;

  Incremental_inputs_header_write<size, big_endian> header_writer(buffer);
  gold_assert(this->command_line_key_ > 0);
  int cmd_offset = this->strtab_->get_offset_from_key(this->command_line_key_);

  header_writer.put_version(INCREMENTAL_LINK_VERSION);
  header_writer.put_input_file_count(this->inputs_map_.size());
  header_writer.put_command_line_offset(cmd_offset);
  header_writer.put_reserved(0);

  for (Inputs_info_map::const_iterator it = this->inputs_map_.begin();
       it != this->inputs_map_.end();
       ++it)
    {
      gold_assert(it->second.index < this->inputs_map_.size());

      unsigned char* entry_buffer =
          inputs_base + it->second.index * entry_size;
      Incremental_inputs_entry_write<size, big_endian> entry(entry_buffer);
      int filename_offset =
          this->strtab_->get_offset_from_key(it->second.filename_key);
      entry.put_filename_offset(filename_offset);
      // TODO: add per input data and timestamp.  Currently we store
      // an out-of-bounds offset for future version of gold to reject
      // such an incremental_inputs section.
      entry.put_data_offset(0xffffffff);
      entry.put_timestamp_sec(0);
      entry.put_timestamp_usec(0);
      entry.put_input_type(it->second.type);
      entry.put_reserved(0);
    }

  return new Output_data_const_buffer(buffer, sz, 8,
				      "** incremental link inputs list");
}

} // End namespace gold.
