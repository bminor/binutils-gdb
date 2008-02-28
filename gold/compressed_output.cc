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

#include "parameters.h"
#include "options.h"
#include "compressed_output.h"

namespace gold
{

// Compress UNCOMPRESSED_DATA of size UNCOMPRESSED_SIZE.  Returns true
// if it successfully compressed, false if it failed for any reason
// (including not having zlib support in the library).  If it returns
// true, it allocates memory for the compressed data using new, and
// sets *COMPRESSED_DATA and *COMPRESSED_SIZE to appropriate values.

#ifdef HAVE_ZLIB_H

static bool
zlib_compress(const char* uncompressed_data, unsigned long uncompressed_size,
              char** compressed_data, unsigned long* compressed_size)
{
  *compressed_size = uncompressed_size + uncompressed_size / 1000 + 128;
  *compressed_data = new char[*compressed_size];

  int compress_level;
  if (parameters->options().optimize() >= 1)
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
}

#else // !defined(HAVE_ZLIB_H)

static bool
zlib_compress(const char*, unsigned long, char**, unsigned long*)
{
  return false;
}

#endif // !defined(HAVE_ZLIB_H)

// After compressing an output section, we rename it from foo to
// foo.zlib.nnnn, where nnnn is the uncompressed size of the section.

static std::string
zlib_compressed_suffix(unsigned long uncompressed_size)
{
  char size_string[64];
  snprintf(size_string, sizeof(size_string), "%lu", uncompressed_size);
  return std::string(".zlib.") + size_string;
}

// Class Output_compressed_section.

// Set the final data size of a compressed section.  This is where
// we actually compress the section data.

void
Output_compressed_section::set_final_data_size()
{
  off_t uncompressed_size = this->postprocessing_buffer_size();

  // (Try to) compress the data.
  unsigned long compressed_size;
  unsigned char* u_uncompressed_data = this->postprocessing_buffer();
  char* uncompressed_data = reinterpret_cast<char*>(u_uncompressed_data);

  // At this point the contents of all regular input sections will
  // have been copied into the postprocessing buffer, and relocations
  // will have been applied.  Now we need to copy in the contents of
  // anything other than a regular input section.
  this->write_to_postprocessing_buffer();

  bool success = false;
  if (this->options_->zlib_compress_debug_sections())
    success = zlib_compress(uncompressed_data, uncompressed_size,
                            &this->data_, &compressed_size);
  if (success)
    {
      std::string suffix(zlib_compressed_suffix(uncompressed_size));
      this->new_section_name_ = std::string(this->name()) + suffix;
      this->set_name(this->new_section_name_.c_str());
      this->set_data_size(compressed_size);
    }
  else
    {
      gold_warning(_("not compressing section data: zlib error"));
      gold_assert(this->data_ == NULL);
      this->set_data_size(uncompressed_size);
    }
}

// Write out a compressed section.  If we couldn't compress, we just
// write it out as normal, uncompressed data.

void
Output_compressed_section::do_write(Output_file* of)
{
  off_t offset = this->offset();
  off_t data_size = this->data_size();
  unsigned char* view = of->get_output_view(offset, data_size);
  if (this->data_ == NULL)
    memcpy(view, this->postprocessing_buffer(), data_size);
  else
    memcpy(view, this->data_, data_size);
  of->write_output_view(offset, data_size, view);
}

} // End namespace gold.
