// compressed_output.h -- compressed output sections for gold  -*- C++ -*-

// Copyright 2007 Free Software Foundation, Inc.
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

// We support compressing .debug_* sections on output.  (And,
// potentially one day, other sections.)  This is a form of
// relaxation.  This file adds support for merging and emitting the
// compressed sections.

#ifndef GOLD_COMPRESSED_OUTPUT_H
#define GOLD_COMPRESSED_OUTPUT_H

#include <string>
#include <vector>

#include "output.h"
#include "merge.h"

namespace gold
{

class General_options;

// This is used for compressing a section before emitting it in the
// output file.  This only works for unloaded sections, since it
// assumes the final section contents are available at
// set_final_data_size() time.  For loaded sections (those that end up
// in segments), this is not true; relocations are applied after
// set_final_data_size() is called.  However, for unloaded sections,
// we can -- and do -- postpone calling finalize_data_size() until
// after relocations are applies.

class Output_compressed_section_data : public Output_section_data
{
 public:
  Output_compressed_section_data(uint64_t addralign,
                                 const General_options& options)
    : Output_section_data(addralign), options_(options), data_(NULL)
  { }

 protected:
  // Add an input section.
  bool
  do_add_input_section(Relobj* object, unsigned int shndx);

  // Set the final data size.
  void
  set_final_data_size();

  // Change the name of the output section to reflect it's compressed.
  const char*
  do_modified_output_section_name(const char* name);

  // Write the data to the file.
  void
  do_write(Output_file*);

 private:
  struct Object_entry
  {
    Relobj* object;
    unsigned int shndx;
    const unsigned char* contents;
    off_t length;

    Object_entry(Relobj* o, unsigned int s)
      : object(o), shndx(s), contents(NULL), length(0)
    { }
  };

  const General_options& options_;
  std::vector<Object_entry> objects_;
  char* data_;
  std::string new_section_name_;
};

// This is a special case for when the output section is a string
// section and does not have any relocations to apply to it.

template<typename Char_type>
class Output_compressed_string : public Output_merge_string<Char_type>
{
 public:
  Output_compressed_string(uint64_t addralign,
                           const General_options& options)
    : Output_merge_string<Char_type>(addralign),
      options_(options), compressed_data_(NULL)
  { }

  ~Output_compressed_string()
  { delete[] compressed_data_; }

 protected:
  // Add an input section.
  bool
  do_add_input_section(Relobj* object, unsigned int shndx);

  // Set the final data size.  Also compresses the buffer.
  void
  set_final_data_size();

  // Change the name of the output section to reflect it's compressed.
  const char*
  do_modified_output_section_name(const char* name);

  // Write the data to the file.
  void
  do_write(Output_file*);

 private:
  const General_options& options_;
  char* compressed_data_;
  // This is just a buffer to store the section name in "permanent" storage.
  std::string new_section_name_;
};

} // End namespace gold.

#endif // !defined(GOLD_COMPRESSED_OUTPUT_H)
