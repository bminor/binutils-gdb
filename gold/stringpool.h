// stringpool.h -- a string pool for gold    -*- C++ -*-

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

#include <string>
#include <list>

#ifndef GOLD_STRINGPOOL_H
#define GOLD_STRINGPOOL_H

namespace gold
{

class Output_file;

// A Stringpool is a pool of unique strings.  It provides the
// following features:

// Every string in the pool is unique.  Thus, if you have two strings
// in the Stringpool, you can compare them for equality by using
// pointer comparison rather than string comparison.

// There is a key associated with every string in the pool.  If you
// add strings to the Stringpool in the same order, then the key for
// each string will always be the same for any run of the linker.
// This is not true of the string pointers themselves, as they may
// change due to address space randomization.  Some parts of the
// linker (e.g., the symbol table) use the key value instead of the
// string pointer so that repeated runs of the linker will generate
// precisely the same output.

// When you add a string to a Stringpool, Stringpool will optionally
// make a copy of it.  Thus there is no requirement to keep a copy
// elsewhere.

// A Stringpool can be turned into a string table, a sequential series
// of null terminated strings.  The first string may optionally be a
// single zero byte, as required for SHT_STRTAB sections.  This
// conversion is only permitted after all strings have been added to
// the Stringpool.  After doing this conversion, you can ask for the
// offset of any string in the stringpool in the string table, and you
// can write the resulting string table to an output file.

// When a Stringpool is turned into a string table, then as an
// optimization it will reuse string suffixes to avoid duplicating
// strings.  That is, given the strings "abc" and "bc", only the
// string "abc" will be stored, and "bc" will be represented by an
// offset into the middle of the string "abc".

// Stringpools are implemented in terms of Stringpool_template, which
// is generalized on the type of character used for the strings.  Most
// uses will want the Stringpool type which uses char.  Other cases
// are used for merging wide string constants.

template<typename Stringpool_char>
class Stringpool_template
{
 public:
  // The type of a key into the stringpool.  As described above, a key
  // value will always be the same during any run of the linker.  Zero
  // is never a valid key value.
  typedef size_t Key;

  // Create a Stringpool.
  Stringpool_template();

  ~Stringpool_template();

  // Clear all the data from the stringpool.
  void
  clear();

  // Indicate that we should not reserve offset 0 to hold the empty
  // string when converting the stringpool to a string table.  This
  // should not be called for a proper ELF SHT_STRTAB section.
  void
  set_no_zero_null()
  { this->zero_null_ = false; }

  // Add the string S to the pool.  This returns a canonical permanent
  // pointer to the string in the pool.  If COPY is true, the string
  // is copied into permanent storage.  If PKEY is not NULL, this sets
  // *PKEY to the key for the string.
  const Stringpool_char*
  add(const Stringpool_char* s, bool copy, Key* pkey);

  // Add the prefix of length LEN of string S to the pool.
  const Stringpool_char*
  add_prefix(const Stringpool_char* s, size_t len, Key* pkey);

  // If the string S is present in the pool, return the canonical
  // string pointer.  Otherwise, return NULL.  If PKEY is not NULL,
  // set *PKEY to the key.
  const Stringpool_char*
  find(const Stringpool_char* s, Key* pkey) const;

  // Turn the stringpool into a string table: determine the offsets of
  // all the strings.  After this is called, no more strings may be
  // added to the stringpool.
  void
  set_string_offsets();

  // Get the offset of the string S in the string table.  This returns
  // the offset in bytes, not in units of Stringpool_char.  This may
  // only be called after set_string_offsets has been called.
  off_t
  get_offset(const Stringpool_char* s) const;

  // Get the offset of the string S in the string table.
  off_t
  get_offset(const std::basic_string<Stringpool_char>& s) const
  { return this->get_offset(s.c_str()); }

  // Get the size of the string table.  This returns the number of
  // bytes, not in units of Stringpool_char.
  off_t
  get_strtab_size() const
  {
    gold_assert(this->strtab_size_ != 0);
    return this->strtab_size_;
  }

  // Write the string table into the output file at the specified
  // offset.
  void
  write(Output_file*, off_t offset);

  // Write the string table into the specified buffer, of the
  // specified size.  buffer_size should be at least
  // get_strtab_size().
  void
  write_to_buffer(char* buffer, size_t buffer_size);

 private:
  Stringpool_template(const Stringpool_template&);
  Stringpool_template& operator=(const Stringpool_template&);

  // Return the length of a string in units of Stringpool_char.
  static size_t
  string_length(const Stringpool_char*);

  // We store the actual data in a list of these buffers.
  struct Stringdata
  {
    // Length of data in buffer.
    size_t len;
    // Allocated size of buffer.
    size_t alc;
    // Buffer index.
    unsigned int index;
    // Buffer.
    char data[1];
  };

  // Copy a string into the buffers, returning a canonical string.
  const Stringpool_char*
  add_string(const Stringpool_char*, Key*);

  // Hash function.
  struct Stringpool_hash
  {
    size_t
    operator()(const Stringpool_char*) const;
  };

  // Equality comparison function for hash table.
  struct Stringpool_eq
  {
    bool
    operator()(const Stringpool_char* p1, const Stringpool_char* p2) const;
  };

  // Return whether s1 is a suffix of s2.
  static bool
  is_suffix(const Stringpool_char* s1, size_t len1,
            const Stringpool_char* s2, size_t len2);

  // The hash table is a map from string names to a pair of Key and
  // string table offsets.  We only use the offsets if we turn this
  // into an string table section.

  typedef std::pair<Key, off_t> Val;

#ifdef HAVE_TR1_UNORDERED_SET
  typedef Unordered_map<const Stringpool_char*, Val, Stringpool_hash,
			Stringpool_eq,
			std::allocator<std::pair<const Stringpool_char* const,
						 Val> >,
			true> String_set_type;
#else
  typedef Unordered_map<const Stringpool_char*, Val, Stringpool_hash,
			Stringpool_eq> String_set_type;
#endif

  // Comparison routine used when sorting into a string table.  We
  // store string-sizes in the sort-vector so we don't have to
  // recompute them log(n) times as we sort.
  struct Stringpool_sort_info
  {
    typename String_set_type::iterator it;
    size_t string_length;
    Stringpool_sort_info(typename String_set_type::iterator i, size_t s)
      : it(i), string_length(s)
    { }
  };

  struct Stringpool_sort_comparison
  {
    bool
    operator()(const Stringpool_sort_info&, const Stringpool_sort_info&) const;
  };

  // List of Stringdata structures.
  typedef std::list<Stringdata*> Stringdata_list;

  // Mapping from const char* to namepool entry.
  String_set_type string_set_;
  // List of buffers.
  Stringdata_list strings_;
  // Size of string table.
  off_t strtab_size_;
  // Next Stringdata index.
  unsigned int next_index_;
  // Next key value for a string we don't copy.
  int next_uncopied_key_;
  // Whether to reserve offset 0 to hold the null string.
  bool zero_null_;
};

// The most common type of Stringpool.
typedef Stringpool_template<char> Stringpool;

} // End namespace gold.

#endif // !defined(GOLD_STRINGPOOL_H)
