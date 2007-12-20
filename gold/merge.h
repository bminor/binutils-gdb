// merge.h -- handle section merging for gold  -*- C++ -*-

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

#ifndef GOLD_MERGE_H
#define GOLD_MERGE_H

#include <climits>

#include "stringpool.h"
#include "output.h"

namespace gold
{

// This class manages mappings from input sections to offsets in an
// output section.  This is used where input sections are merged.  The
// actual data is stored in fields in Object.

class Merge_map
{
 public:
  Merge_map()
  { }

  // Add a mapping for the bytes from OFFSET to OFFSET + LENGTH in the
  // input section SHNDX in object OBJECT to OUTPUT_OFFSET in the
  // output section.  An OUTPUT_OFFSET of -1 means that the bytes are
  // discarded.  OUTPUT_OFFSET is not the offset from the start of the
  // output section, it is the offset from the start of the merged
  // data within the output section.
  void
  add_mapping(Relobj* object, unsigned int shndx,
	      section_offset_type offset, section_size_type length,
	      section_offset_type output_offset);

  // Return the output offset for an input address.  The input address
  // is at offset OFFSET in section SHNDX in OBJECT.  This sets
  // *OUTPUT_OFFSET to the offset in the output section; this will be
  // -1 if the bytes are not being copied to the output.  This returns
  // true if the mapping is known, false otherwise.  This returns the
  // value stored by add_mapping, namely the offset from the start of
  // the merged data within the output section.
  bool
  get_output_offset(const Relobj* object, unsigned int shndx,
		    section_offset_type offset,
		    section_offset_type *output_offset) const;
};

// A general class for SHF_MERGE data, to hold functions shared by
// fixed-size constant data and string data.

class Output_merge_base : public Output_section_data
{
 public:
  Output_merge_base(uint64_t entsize, uint64_t addralign)
    : Output_section_data(addralign), merge_map_(), entsize_(entsize)
  { }

  // Return the output offset for an input offset.
  bool
  do_output_offset(const Relobj* object, unsigned int shndx,
		   section_offset_type offset,
		   section_offset_type* poutput) const;

 protected:
  // Return the entry size.
  uint64_t
  entsize() const
  { return this->entsize_; }

  // Add a mapping from an OFFSET in input section SHNDX in object
  // OBJECT to an OUTPUT_OFFSET in the output section.  OUTPUT_OFFSET
  // is the offset from the start of the merged data in the output
  // section.
  void
  add_mapping(Relobj* object, unsigned int shndx, section_offset_type offset,
	      section_size_type length, section_offset_type output_offset)
  {
    this->merge_map_.add_mapping(object, shndx, offset, length, output_offset);
  }

 private:
  // A mapping from input object/section/offset to offset in output
  // section.
  Merge_map merge_map_;
  // The entry size.  For fixed-size constants, this is the size of
  // the constants.  For strings, this is the size of a character.
  uint64_t entsize_;
};

// Handle SHF_MERGE sections with fixed-size constant data.

class Output_merge_data : public Output_merge_base
{
 public:
  Output_merge_data(uint64_t entsize, uint64_t addralign)
    : Output_merge_base(entsize, addralign), p_(NULL), len_(0), alc_(0),
      input_count_(0),
      hashtable_(128, Merge_data_hash(this), Merge_data_eq(this))
  { }

 protected:
  // Add an input section.
  bool
  do_add_input_section(Relobj* object, unsigned int shndx);

  // Set the final data size.
  void
  set_final_data_size();

  // Write the data to the file.
  void
  do_write(Output_file*);

  // Write the data to a buffer.
  void
  do_write_to_buffer(unsigned char*);

  // Print merge stats to stderr.
  void
  do_print_merge_stats(const char* section_name);

 private:
  // We build a hash table of the fixed-size constants.  Each constant
  // is stored as a pointer into the section data we are accumulating.

  // A key in the hash table.  This is an offset in the section
  // contents we are building.
  typedef section_offset_type Merge_data_key;

  // Compute the hash code.  To do this we need a pointer back to the
  // object holding the data.
  class Merge_data_hash
  {
   public:
    Merge_data_hash(const Output_merge_data* pomd)
      : pomd_(pomd)
    { }

    size_t
    operator()(Merge_data_key) const;

   private:
    const Output_merge_data* pomd_;
  };

  friend class Merge_data_hash;

  // Compare two entries in the hash table for equality.  To do this
  // we need a pointer back to the object holding the data.  Note that
  // we now have a pointer to the object stored in two places in the
  // hash table.  Fixing this would require specializing the hash
  // table, which would be hard to do portably.
  class Merge_data_eq
  {
   public:
    Merge_data_eq(const Output_merge_data* pomd)
      : pomd_(pomd)
    { }

    bool
    operator()(Merge_data_key k1, Merge_data_key k2) const;

   private:
    const Output_merge_data* pomd_;
  };

  friend class Merge_data_eq;

  // The type of the hash table.
  typedef Unordered_set<Merge_data_key, Merge_data_hash, Merge_data_eq>
    Merge_data_hashtable;

  // Given a hash table key, which is just an offset into the section
  // data, return a pointer to the corresponding constant.
  const unsigned char*
  constant(Merge_data_key k) const
  {
    gold_assert(k >= 0 && k < static_cast<section_offset_type>(this->len_));
    return this->p_ + k;
  }

  // Add a constant to the output.
  void
  add_constant(const unsigned char*);

  // The accumulated data.
  unsigned char* p_;
  // The length of the accumulated data.
  section_size_type len_;
  // The size of the allocated buffer.
  section_size_type alc_;
  // The number of entries seen in input files.
  size_t input_count_;
  // The hash table.
  Merge_data_hashtable hashtable_;
};

// Handle SHF_MERGE sections with string data.  This is a template
// based on the type of the characters in the string.

template<typename Char_type>
class Output_merge_string : public Output_merge_base
{
 public:
  Output_merge_string(uint64_t addralign)
    : Output_merge_base(sizeof(Char_type), addralign), stringpool_(),
      merged_strings_(), input_count_(0)
  {
    gold_assert(addralign <= sizeof(Char_type));
    this->stringpool_.set_no_zero_null();
  }

 protected:
  // Add an input section.
  bool
  do_add_input_section(Relobj* object, unsigned int shndx);

  // Do all the final processing after the input sections are read in.
  // Returns the final data size.
  section_size_type
  finalize_merged_data();

  // Set the final data size.
  void
  set_final_data_size();

  // Write the data to the file.
  void
  do_write(Output_file*);

  // Write the data to a buffer.
  void
  do_write_to_buffer(unsigned char*);

  // Print merge stats to stderr.
  void
  do_print_merge_stats(const char* section_name);

  // Writes the stringpool to a buffer.
  void
  stringpool_to_buffer(unsigned char* buffer, section_size_type buffer_size)
  { this->stringpool_.write_to_buffer(buffer, buffer_size); }

  // Clears all the data in the stringpool, to save on memory.
  void
  clear_stringpool()
  { this->stringpool_.clear(); }

 private:
  // The name of the string type, for stats.
  const char*
  string_name();

  // As we see input sections, we build a mapping from object, section
  // index and offset to strings.
  struct Merged_string
  {
    // The input object where the string was found.
    Relobj* object;
    // The input section in the input object.
    unsigned int shndx;
    // The offset in the input section.
    section_offset_type offset;
    // The string itself, a pointer into a Stringpool.
    const Char_type* string;
    // The length of the string in bytes, including the null terminator.
    size_t length;
    // The key in the Stringpool.
    Stringpool::Key stringpool_key;

    Merged_string(Relobj *objecta, unsigned int shndxa,
		  section_offset_type offseta, const Char_type* stringa,
		  size_t lengtha, Stringpool::Key stringpool_keya)
      : object(objecta), shndx(shndxa), offset(offseta), string(stringa),
	length(lengtha), stringpool_key(stringpool_keya)
    { }
  };

  typedef std::vector<Merged_string> Merged_strings;

  // As we see the strings, we add them to a Stringpool.
  Stringpool_template<Char_type> stringpool_;
  // Map from a location in an input object to an entry in the
  // Stringpool.
  Merged_strings merged_strings_;
  // The number of entries seen in input files.
  size_t input_count_;
};

} // End namespace gold.

#endif // !defined(GOLD_MERGE_H)
