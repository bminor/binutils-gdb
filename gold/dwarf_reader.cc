// dwarf_reader.cc -- parse dwarf2/3 debug information

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

#include "elfcpp_swap.h"
#include "dwarf.h"
#include "object.h"
#include "reloc.h"
#include "dwarf_reader.h"

namespace {

// Read an unsigned LEB128 number.  Each byte contains 7 bits of
// information, plus one bit saying whether the number continues or
// not.

uint64_t
read_unsigned_LEB_128(const unsigned char* buffer, size_t* len)
{
  uint64_t result = 0;
  size_t num_read = 0;
  unsigned int shift = 0;
  unsigned char byte;

  do
    {
      byte = *buffer++;
      num_read++;
      result |= (static_cast<uint64_t>(byte & 0x7f)) << shift;
      shift += 7;
    }
  while (byte & 0x80);

  *len = num_read;

  return result;
}

// Read a signed LEB128 number.  These are like regular LEB128
// numbers, except the last byte may have a sign bit set.

int64_t
read_signed_LEB_128(const unsigned char* buffer, size_t* len)
{
  int64_t result = 0;
  int shift = 0;
  size_t num_read = 0;
  unsigned char byte;

  do
    {
      byte = *buffer++;
      num_read++;
      result |= (static_cast<uint64_t>(byte & 0x7f) << shift);
      shift += 7;
    }
  while (byte & 0x80);

  if ((shift < 8 * static_cast<int>(sizeof(result))) && (byte & 0x40))
    result |= -((static_cast<int64_t>(1)) << shift);
  *len = num_read;
  return result;
}

} // End anonymous namespace.


namespace gold {

// This is the format of a DWARF2/3 line state machine that we process
// opcodes using.  There is no need for anything outside the lineinfo
// processor to know how this works.

struct LineStateMachine
{
  int file_num;
  uint64_t address;
  int line_num;
  int column_num;
  unsigned int shndx;    // the section address refers to
  bool is_stmt;          // stmt means statement.
  bool basic_block;
  bool end_sequence;
};

static void
ResetLineStateMachine(struct LineStateMachine* lsm, bool default_is_stmt)
{
  lsm->file_num = 1;
  lsm->address = 0;
  lsm->line_num = 1;
  lsm->column_num = 0;
  lsm->shndx = -1U;
  lsm->is_stmt = default_is_stmt;
  lsm->basic_block = false;
  lsm->end_sequence = false;
}

template<int size, bool big_endian>
Dwarf_line_info<size, big_endian>::Dwarf_line_info(Object* object)
  : data_valid_(false), buffer_(NULL), symtab_buffer_(NULL),
    directories_(1), files_(1)
{
  unsigned int debug_shndx;
  for (debug_shndx = 0; debug_shndx < object->shnum(); ++debug_shndx)
    if (object->section_name(debug_shndx) == ".debug_line")
      {
        off_t buffer_size;
        this->buffer_ = object->section_contents(
            debug_shndx, &buffer_size, false);
        this->buffer_end_ = this->buffer_ + buffer_size;
        break;
      }
  if (this->buffer_ == NULL)
    return;

  // Find the relocation section for ".debug_line".
  bool got_relocs = false;
  for (unsigned int reloc_shndx = 0;
       reloc_shndx < object->shnum();
       ++reloc_shndx)
    {
      unsigned int reloc_sh_type = object->section_type(reloc_shndx);
      if ((reloc_sh_type == elfcpp::SHT_REL
	   || reloc_sh_type == elfcpp::SHT_RELA)
	  && object->section_info(reloc_shndx) == debug_shndx)
	{
	  got_relocs = this->track_relocs_.initialize(object, reloc_shndx,
                                                      reloc_sh_type);
	  break;
	}
    }
  if (!got_relocs)
    return;

  // Finally, we need the symtab section to interpret the relocs.
  unsigned int symtab_shndx;
  for (symtab_shndx = 0; symtab_shndx < object->shnum(); ++symtab_shndx)
    if (object->section_type(symtab_shndx) == elfcpp::SHT_SYMTAB)
      {
        off_t symtab_size;
        this->symtab_buffer_ = object->section_contents(
            symtab_shndx, &symtab_size, false);
        this->symtab_buffer_end_ = this->symtab_buffer_ + symtab_size;
        break;
      }
  if (this->symtab_buffer_ == NULL)
    return;

  // Now that we have successfully read all the data, parse the debug
  // info.
  this->data_valid_ = true;
  this->read_line_mappings();
}

// Read the DWARF header.

template<int size, bool big_endian>
const unsigned char*
Dwarf_line_info<size, big_endian>::read_header_prolog(
    const unsigned char* lineptr)
{
  uint32_t initial_length = elfcpp::Swap<32, big_endian>::readval(lineptr);
  lineptr += 4;

  // In DWARF2/3, if the initial length is all 1 bits, then the offset
  // size is 8 and we need to read the next 8 bytes for the real length.
  if (initial_length == 0xffffffff)
    {
      header_.offset_size = 8;
      initial_length = elfcpp::Swap<64, big_endian>::readval(lineptr);
      lineptr += 8;
    }
  else
    header_.offset_size = 4;

  header_.total_length = initial_length;

  gold_assert(lineptr + header_.total_length <= buffer_end_);

  header_.version = elfcpp::Swap<16, big_endian>::readval(lineptr);
  lineptr += 2;

  if (header_.offset_size == 4)
    header_.prologue_length = elfcpp::Swap<32, big_endian>::readval(lineptr);
  else
    header_.prologue_length = elfcpp::Swap<64, big_endian>::readval(lineptr);
  lineptr += header_.offset_size;

  header_.min_insn_length = *lineptr;
  lineptr += 1;

  header_.default_is_stmt = *lineptr;
  lineptr += 1;

  header_.line_base = *reinterpret_cast<const signed char*>(lineptr);
  lineptr += 1;

  header_.line_range = *lineptr;
  lineptr += 1;

  header_.opcode_base = *lineptr;
  lineptr += 1;

  header_.std_opcode_lengths.reserve(header_.opcode_base + 1);
  header_.std_opcode_lengths[0] = 0;
  for (int i = 1; i < header_.opcode_base; i++)
    {
      header_.std_opcode_lengths[i] = *lineptr;
      lineptr += 1;
    }

  return lineptr;
}

// The header for a debug_line section is mildly complicated, because
// the line info is very tightly encoded.

template<int size, bool big_endian>
const unsigned char*
Dwarf_line_info<size, big_endian>::read_header_tables(
    const unsigned char* lineptr)
{
  // It is legal for the directory entry table to be empty.
  if (*lineptr)
    {
      int dirindex = 1;
      while (*lineptr)
        {
          const unsigned char* dirname = lineptr;
          gold_assert(dirindex == static_cast<int>(directories_.size()));
          directories_.push_back(reinterpret_cast<const char*>(dirname));
          lineptr += directories_.back().size() + 1;
          dirindex++;
        }
    }
  lineptr++;

  // It is also legal for the file entry table to be empty.
  if (*lineptr)
    {
      int fileindex = 1;
      size_t len;
      while (*lineptr)
        {
          const char* filename = reinterpret_cast<const char*>(lineptr);
          lineptr += strlen(filename) + 1;

          uint64_t dirindex = read_unsigned_LEB_128(lineptr, &len);
          if (dirindex >= directories_.size())
            dirindex = 0;
          lineptr += len;

          read_unsigned_LEB_128(lineptr, &len);   // mod_time
          lineptr += len;

          read_unsigned_LEB_128(lineptr, &len);   // filelength
          lineptr += len;

          gold_assert(fileindex == static_cast<int>(files_.size()));
          files_.push_back(std::pair<int, std::string>(dirindex, filename));
          fileindex++;
        }
    }
  lineptr++;

  return lineptr;
}

// Process a single opcode in the .debug.line structure.

// Templating on size and big_endian would yield more efficient (and
// simpler) code, but would bloat the binary.  Speed isn't important
// here.

template<int size, bool big_endian>
bool
Dwarf_line_info<size, big_endian>::process_one_opcode(
    const unsigned char* start, struct LineStateMachine* lsm, size_t* len)
{
  size_t oplen = 0;
  size_t templen;
  unsigned char opcode = *start;
  oplen++;
  start++;

  // If the opcode is great than the opcode_base, it is a special
  // opcode. Most line programs consist mainly of special opcodes.
  if (opcode >= header_.opcode_base)
    {
      opcode -= header_.opcode_base;
      const int advance_address = ((opcode / header_.line_range)
                                   * header_.min_insn_length);
      lsm->address += advance_address;

      const int advance_line = ((opcode % header_.line_range)
                                + header_.line_base);
      lsm->line_num += advance_line;
      lsm->basic_block = true;
      *len = oplen;
      return true;
    }

  // Otherwise, we have the regular opcodes
  switch (opcode)
    {
    case elfcpp::DW_LNS_copy:
      lsm->basic_block = false;
      *len = oplen;
      return true;

    case elfcpp::DW_LNS_advance_pc:
      {
        const uint64_t advance_address
            = read_unsigned_LEB_128(start, &templen);
        oplen += templen;
        lsm->address += header_.min_insn_length * advance_address;
      }
      break;

    case elfcpp::DW_LNS_advance_line:
      {
        const uint64_t advance_line = read_signed_LEB_128(start, &templen);
        oplen += templen;
        lsm->line_num += advance_line;
      }
      break;

    case elfcpp::DW_LNS_set_file:
      {
        const uint64_t fileno = read_unsigned_LEB_128(start, &templen);
        oplen += templen;
        lsm->file_num = fileno;
      }
      break;

    case elfcpp::DW_LNS_set_column:
      {
        const uint64_t colno = read_unsigned_LEB_128(start, &templen);
        oplen += templen;
        lsm->column_num = colno;
      }
      break;

    case elfcpp::DW_LNS_negate_stmt:
      lsm->is_stmt = !lsm->is_stmt;
      break;

    case elfcpp::DW_LNS_set_basic_block:
      lsm->basic_block = true;
      break;

    case elfcpp::DW_LNS_fixed_advance_pc:
      {
        int advance_address;
        advance_address = elfcpp::Swap<16, big_endian>::readval(start);
        oplen += 2;
        lsm->address += advance_address;
      }
      break;

    case elfcpp::DW_LNS_const_add_pc:
      {
        const int advance_address = (header_.min_insn_length
                                     * ((255 - header_.opcode_base)
                                        / header_.line_range));
        lsm->address += advance_address;
      }
      break;

    case elfcpp::DW_LNS_extended_op:
      {
        const uint64_t extended_op_len
            = read_unsigned_LEB_128(start, &templen);
        start += templen;
        oplen += templen + extended_op_len;

        const unsigned char extended_op = *start;
        start++;

        switch (extended_op)
          {
          case elfcpp::DW_LNE_end_sequence:
            lsm->end_sequence = true;
            *len = oplen;
            return true;

          case elfcpp::DW_LNE_set_address:
            {
              typename Reloc_map::const_iterator it
                  = reloc_map_.find(start - this->buffer_);
              if (it != reloc_map_.end())
                {
                  // value + addend.
                  lsm->address =
		    (elfcpp::Swap<size, big_endian>::readval(start)
		     + it->second.second);
                  lsm->shndx = it->second.first;
                }
              else
                {
                  // Every set_address should have an associated
                  // relocation.
                  this->data_valid_ = false;
                }
              break;
            }
          case elfcpp::DW_LNE_define_file:
            {
              const char* filename  = reinterpret_cast<const char*>(start);
              templen = strlen(filename) + 1;
              start += templen;

              uint64_t dirindex = read_unsigned_LEB_128(start, &templen);
              if (dirindex >= directories_.size())
                dirindex = 0;
              oplen += templen;

              read_unsigned_LEB_128(start, &templen);   // mod_time
              oplen += templen;

              read_unsigned_LEB_128(start, &templen);   // filelength
              oplen += templen;

              files_.push_back(std::pair<int, std::string>(dirindex,
							   filename));
            }
            break;
          }
      }
      break;

    default:
      {
        // Ignore unknown opcode  silently
        for (int i = 0; i < header_.std_opcode_lengths[opcode]; i++)
          {
            size_t templen;
            read_unsigned_LEB_128(start, &templen);
            start += templen;
            oplen += templen;
          }
      }
      break;
  }
  *len = oplen;
  return false;
}

// Read the debug information at LINEPTR and store it in the line
// number map.

template<int size, bool big_endian>
unsigned const char*
Dwarf_line_info<size, big_endian>::read_lines(unsigned const char* lineptr)
{
  struct LineStateMachine lsm;

  // LENGTHSTART is the place the length field is based on.  It is the
  // point in the header after the initial length field.
  const unsigned char* lengthstart = buffer_;

  // In 64 bit dwarf, the initial length is 12 bytes, because of the
  // 0xffffffff at the start.
  if (header_.offset_size == 8)
    lengthstart += 12;
  else
    lengthstart += 4;

  while (lineptr < lengthstart + header_.total_length)
    {
      ResetLineStateMachine(&lsm, header_.default_is_stmt);
      while (!lsm.end_sequence)
        {
          size_t oplength;
          bool add_line = this->process_one_opcode(lineptr, &lsm, &oplength);
          if (add_line)
            {
              Offset_to_lineno_entry entry
                  = { lsm.address, lsm.file_num, lsm.line_num };
              line_number_map_[lsm.shndx].push_back(entry);
            }
          lineptr += oplength;
        }
    }

  return lengthstart + header_.total_length;
}

// Looks in the symtab to see what section a symbol is in.

template<int size, bool big_endian>
unsigned int
Dwarf_line_info<size, big_endian>::symbol_section(
    unsigned int sym,
    typename elfcpp::Elf_types<size>::Elf_Addr* value)
{
  const int symsize = elfcpp::Elf_sizes<size>::sym_size;
  gold_assert(this->symtab_buffer_ + sym * symsize < this->symtab_buffer_end_);
  elfcpp::Sym<size, big_endian> elfsym(this->symtab_buffer_ + sym * symsize);
  *value = elfsym.get_st_value();
  return elfsym.get_st_shndx();
}

// Read the relocations into a Reloc_map.

template<int size, bool big_endian>
void
Dwarf_line_info<size, big_endian>::read_relocs()
{
  if (this->symtab_buffer_ == NULL)
    return;

  typename elfcpp::Elf_types<size>::Elf_Addr value;
  off_t reloc_offset;
  while ((reloc_offset = this->track_relocs_.next_offset()) != -1)
    {
      const unsigned int sym = this->track_relocs_.next_symndx();
      const unsigned int shndx = this->symbol_section(sym, &value);
      this->reloc_map_[reloc_offset] = std::make_pair(shndx, value);
      this->track_relocs_.advance(reloc_offset + 1);
    }
}

// Read the line number info.

template<int size, bool big_endian>
void
Dwarf_line_info<size, big_endian>::read_line_mappings()
{
  gold_assert(this->data_valid_ == true);

  read_relocs();
  while (this->buffer_ < this->buffer_end_)
    {
      const unsigned char* lineptr = this->buffer_;
      lineptr = this->read_header_prolog(lineptr);
      lineptr = this->read_header_tables(lineptr);
      lineptr = this->read_lines(lineptr);
      this->buffer_ = lineptr;
    }

  // Sort the lines numbers, so addr2line can use binary search.
  for (typename Lineno_map::iterator it = line_number_map_.begin();
       it != line_number_map_.end();
       ++it)
    // Each vector needs to be sorted by offset.
    std::sort(it->second.begin(), it->second.end());
}

// Return a string for a file name and line number.

template<int size, bool big_endian>
std::string
Dwarf_line_info<size, big_endian>::addr2line(unsigned int shndx, off_t offset)
{
  if (this->data_valid_ == false)
    return "";

  const Offset_to_lineno_entry lookup_key = { offset, 0, 0 };
  std::vector<Offset_to_lineno_entry>& offsets = this->line_number_map_[shndx];
  if (offsets.empty())
    return "";

  typename std::vector<Offset_to_lineno_entry>::const_iterator it
      = std::lower_bound(offsets.begin(), offsets.end(), lookup_key);

  // If we found an exact match, great, otherwise find the last entry
  // before the passed-in offset.
  if (it->offset > offset)
    {
      if (it == offsets.begin())
        return "";
      --it;
      gold_assert(it->offset < offset);
    }

  // Convert the file_num + line_num into a string.
  std::string ret;
  gold_assert(it->file_num < static_cast<int>(files_.size()));
  const std::pair<int, std::string>& filename_pair = files_[it->file_num];
  gold_assert(filename_pair.first < static_cast<int>(directories_.size()));
  const std::string& dirname = directories_[filename_pair.first];
  const std::string& filename = filename_pair.second;
  if (!dirname.empty())
    {
      ret += dirname;
      ret += "/";
    }
  ret += filename;
  if (ret.empty())
    ret = "(unknown)";

  char buffer[64];   // enough to hold a line number
  snprintf(buffer, sizeof(buffer), "%d", it->line_num);
  ret += ":";
  ret += buffer;

  return ret;
}

#ifdef HAVE_TARGET_32_LITTLE
template
class Dwarf_line_info<32, false>;
#endif

#ifdef HAVE_TARGET_32_BIG
template
class Dwarf_line_info<32, true>;
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
class Dwarf_line_info<64, false>;
#endif

#ifdef HAVE_TARGET_64_BIG
template
class Dwarf_line_info<64, true>;
#endif

} // End namespace gold.
