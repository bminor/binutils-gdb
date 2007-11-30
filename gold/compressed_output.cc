// compressed_output.cc -- manage compressed output sections for gold

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

#include "gold.h"

#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include "compressed_output.h"
#include "parameters.h"

namespace gold
{

// Compress UNCOMPRESSED_DATA of size UNCOMPRESSED_SIZE.  Returns true
// if it successfully compressed, false if it failed for any reason
// (including not having zlib support in the library).  If it returns
// true, it allocates memory for the compressed data using new, and
// sets *COMPRESSED_DATA and *COMPRESSED_SIZE to appropriate values.

static bool
zlib_compress(const char* uncompressed_data, unsigned long uncompressed_size,
              char** compressed_data, unsigned long* compressed_size)
{
#ifndef HAVE_ZLIB_H
  return false;
#else
  *compressed_size = uncompressed_size + uncompressed_size / 1000 + 128;
  *compressed_data = new char[*compressed_size];

  int compress_level;
  if (parameters->optimization_level() >= 1)
    compress_level = 9;
  else
    compress_level = 1;

  int rc = compress2(reinterpret_cast<Bytef*>(*compressed_data),
                     compressed_size,
                     reinterpret_cast<const Bytef*>(uncompressed_data),
                     uncompressed_size,
                     compress_level);
  if (rc == Z_OK)
    return true;
  else
    {
      delete[] *compressed_data;
      *compressed_data = NULL;
      return false;
    }
#endif  // #ifdef HAVE_ZLIB_H
}

// After compressing an output section, we rename it from foo to
// foo.zlib.nnnn, where nnnn is the uncompressed size of the section.

static std::string
zlib_compressed_suffix(unsigned long uncompressed_size)
{
  char size_string[64];
  snprintf(size_string, sizeof(size_string), "%lu", uncompressed_size);
  return std::string(".zlib.") + size_string;
}

// Class Output_compressed_section_data.

// Add an input section.  In this case, we just keep track of the sections.

bool
Output_compressed_section_data::do_add_input_section(Relobj* obj,
                                                     unsigned int shndx)
{
  this->objects_.push_back(Object_entry(obj, shndx));
  return true;
}

// Set the final data size of a compressed section.  This is where
// we actually compress the section data.

void
Output_compressed_section_data::set_final_data_size()
{
  // FIXME: assert that relocations have already been applied.

  off_t uncompressed_size = 0;
  for (std::vector<Object_entry>::iterator it = this->objects_.begin();
       it != this->objects_.end();
       ++it)
    {
      it->contents
        = it->object->section_contents(it->shndx, &it->length, false);
      uncompressed_size += it->length;
    }

  // (Try to) compress the data.
  unsigned long compressed_size;
  char* uncompressed_data = new char[uncompressed_size];
  off_t pos = 0;
  for (std::vector<Object_entry>::const_iterator it = this->objects_.begin();
       it != this->objects_.end();
       ++it)
    {
      memcpy(uncompressed_data + pos,
             reinterpret_cast<const char*>(it->contents),
             it->length);
      pos += it->length;
    }

  bool success = false;
  if (options_.zlib_compress_debug_sections())
    success = zlib_compress(uncompressed_data, uncompressed_size,
                            &this->data_, &compressed_size);
  if (success)
    {
      delete[] uncompressed_data;
      this->set_data_size(compressed_size);
      this->new_section_name_ = zlib_compressed_suffix(uncompressed_size);
    }
  else
    {
      gold_warning(_("Not compressing section data: zlib error"));
      gold_assert(this->data_ == NULL);
      this->data_ = uncompressed_data;
      this->set_data_size(uncompressed_size);
    }
}

// Change the name of the output section to reflect it's compressed.
// The layout routines call into this right before finalizing the
// shstrtab.

const char*
Output_compressed_section_data::do_modified_output_section_name(
  const char* name)
{
  // This mean we never compressed the data.
  if (this->new_section_name_.empty())
    return NULL;
  this->new_section_name_ = std::string(name) + this->new_section_name_;
  return this->new_section_name_.c_str();
}

// Write out a compressed section.  If we couldn't compress, we just
// write it out as normal, uncompressed data.

void
Output_compressed_section_data::do_write(Output_file* of)
{
  unsigned char* uview = of->get_output_view(this->offset(),
                                             this->data_size());
  char* view = reinterpret_cast<char*>(uview);
  memcpy(view, this->data_, this->data_size());
  of->write_output_view(this->offset(), this->data_size(), uview);
}

// Class Output_compressed_string.

// Add an input section.  We don't do anything special here.

template<typename Char_type>
bool
Output_compressed_string<Char_type>::do_add_input_section(Relobj* object,
                                                          unsigned int shndx)
{
  return Output_merge_string<Char_type>::do_add_input_section(object, shndx);
}

// Set the final data size of a compressed section.  This is where
// we actually compress the section data.

template<typename Char_type>
void
Output_compressed_string<Char_type>::set_final_data_size()
{
  // First let the superclass finalize all its data, then write it to
  // a buffer.
  unsigned long uncompressed_size = this->finalize_merged_data();
  char* uncompressed_data = new char[uncompressed_size];
  this->stringpool_to_buffer(uncompressed_data, uncompressed_size);

  // (Try to) compress the data.
  unsigned long compressed_size;
  if (options_.zlib_compress_debug_sections()
      && zlib_compress(uncompressed_data, uncompressed_size,
                       &this->compressed_data_, &compressed_size))
    {
      this->set_data_size(compressed_size);
      // Save some memory.
      this->clear_stringpool();
      // We will be renaming the section to name.zlib.uncompressed_size.
      this->new_section_name_ = zlib_compressed_suffix(uncompressed_size);
    }
  else
    {
      this->compressed_data_ = NULL;
      this->set_data_size(uncompressed_size);
    }

  delete[] uncompressed_data;
}

// Change the name of the output section to reflect it's compressed.
// The layout routines call into this right before finalizing the
// shstrtab.

template<typename Char_type>
const char*
Output_compressed_string<Char_type>::do_modified_output_section_name(
  const char* name)
{
  // This mean we never compressed the data
  if (this->new_section_name_.empty())
    return NULL;
  this->new_section_name_ = std::string(name) + this->new_section_name_;
  return this->new_section_name_.c_str();
}

// Write out a compressed string section.  If we couldn't compress,
// we just write out the normal string section.

template<typename Char_type>
void
Output_compressed_string<Char_type>::do_write(Output_file* of)
{
  if (this->compressed_data_ == NULL)
    Output_merge_string<Char_type>::do_write(of);
  else
    {
      unsigned char* uview = of->get_output_view(this->offset(),
                                                 this->data_size());
      char* view = reinterpret_cast<char*>(uview);
      memcpy(view, this->compressed_data_, this->data_size());
      of->write_output_view(this->offset(), this->data_size(), uview);
    }
}

// Instantiate the templates we need.

template
class Output_compressed_string<char>;

template
class Output_compressed_string<uint16_t>;

template
class Output_compressed_string<uint32_t>;

} // End namespace gold.
