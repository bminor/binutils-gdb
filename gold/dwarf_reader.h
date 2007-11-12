// dwarf_reader.h -- parse dwarf2/3 debug information for gold  -*- C++ -*-

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

#ifndef GOLD_DWARF_READER_H
#define GOLD_DWARF_READER_H

#include <vector>
#include <map>

#include "elfcpp.h"
#include "elfcpp_swap.h"
#include "dwarf.h"
#include "reloc.h"

namespace gold
{

template<int size, bool big_endian>
class Track_relocs;
struct LineStateMachine;

// This class is used to read the line information from the debugging
// section of an object file.

template<int size, bool big_endian>
class Dwarf_line_info
{
 public:
  // Initializes a .debug_line reader for a given object file.
  Dwarf_line_info(Sized_relobj<size, big_endian>* object);

  // Given a section number and an offset, returns the associated
  // file and line-number, as a string: "file:lineno".  If unable
  // to do the mapping, returns the empty string.  You must call
  // read_line_mappings() before calling this function.
  std::string
  addr2line(unsigned int shndx, off_t offset);

 private:
  // Start processing line info, and populates the offset_map_.
  void
  read_line_mappings();

  // Reads the relocation section associated with .debug_line and
  // stores relocation information in reloc_map_.
  void
  read_relocs();

  // Looks in the symtab to see what section a symbol is in.
  unsigned int
  symbol_section(unsigned int sym,
                 typename elfcpp::Elf_types<size>::Elf_Addr* value);

  // Reads the DWARF2/3 header for this line info.  Each takes as input
  // a starting buffer position, and returns the ending position.
  const unsigned char*
  read_header_prolog(const unsigned char* lineptr);

  const unsigned char*
  read_header_tables(const unsigned char* lineptr);

  // Reads the DWARF2/3 line information.
  const unsigned char*
  read_lines(const unsigned char* lineptr);

  // Process a single line info opcode at START using the state
  // machine at LSM.  Return true if we should define a line using the
  // current state of the line state machine.  Place the length of the
  // opcode in LEN.
  bool
  process_one_opcode(const unsigned char* start,
                     struct LineStateMachine* lsm, size_t* len);

  // If we saw anything amiss while parsing, we set this to false.
  // Then addr2line will always fail (rather than return possibly-
  // corrupt data).
  bool data_valid_;

  // A DWARF2/3 line info header.  This is not the same size as in the
  // actual file, as the one in the file may have a 32 bit or 64 bit
  // lengths.

  struct Dwarf_line_infoHeader
  {
    off_t total_length;
    int version;
    off_t prologue_length;
    int min_insn_length; // insn stands for instructin
    bool default_is_stmt; // stmt stands for statement
    signed char line_base;
    int line_range;
    unsigned char opcode_base;
    std::vector<unsigned char> std_opcode_lengths;
    int offset_size;
  } header_;

  // buffer is the buffer for our line info, starting at exactly where
  // the line info to read is.
  const unsigned char* buffer_;
  const unsigned char* buffer_end_;

  // This has relocations that point into buffer.
  Track_relocs<size, big_endian> track_relocs_;

  // This is used to figure out what section to apply a relocation to.
  const unsigned char* symtab_buffer_;
  const unsigned char* symtab_buffer_end_;

  // Holds the directories and files as we see them.
  std::vector<std::string> directories_;
  // The first part is an index into directories_, the second the filename.
  std::vector< std::pair<int, std::string> > files_;

  // A map from offset of the relocation target to the shndx and
  // addend for the relocation.
  typedef std::map<typename elfcpp::Elf_types<size>::Elf_Addr,
                   std::pair<unsigned int,
                             typename elfcpp::Elf_types<size>::Elf_Swxword> >
  Reloc_map;
  Reloc_map reloc_map_;

  // We can't do better than to keep the offsets in a sorted vector.
  // Here, offset is the key, and file_num/line_num is the value.
  struct Offset_to_lineno_entry
  {
    off_t offset;
    int file_num;    // a pointer into files_
    int line_num;
    // Offsets are unique within a section, so that's a sufficient sort key.
    bool operator<(const Offset_to_lineno_entry& that) const
    { return this->offset < that.offset; }
  };
  // We have a vector of offset->lineno entries for every input section.
  typedef Unordered_map<unsigned int, std::vector<Offset_to_lineno_entry> >
  Lineno_map;

  Lineno_map line_number_map_;
};

} // End namespace gold.

#endif // !defined(GOLD_DWARF_READER_H)
