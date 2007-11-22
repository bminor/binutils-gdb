// merge.cc -- handle section merging for gold

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

#include "gold.h"

#include <cstdlib>
#include <algorithm>

#include "merge.h"

namespace gold
{

// Class Merge_map::Merge_key_less.

// Sort the entries in a merge mapping.  The key is an input object, a
// section index in that object, and an offset in that section.

inline bool
Merge_map::Merge_key_less::operator()(const Merge_key& mk1,
				      const Merge_key& mk2) const
{
  // The order of different objects and different sections doesn't
  // matter.  We want to get consistent results across links so we
  // don't use pointer comparison.
  if (mk1.object != mk2.object)
    {
      // Two different object files can have the same name: if foo.a
      // includes both bar/qux.o and baz/qux.o, then both end up with
      // the name foo.a(qux.o).  But it's impossible for two different
      // object files to have both the same name and the same offset.
      if (mk1.object->offset() != mk2.object->offset())
        return mk1.object->offset() < mk2.object->offset();
      return mk1.object->name() < mk2.object->name();
    }
  if (mk1.shndx != mk2.shndx)
    return mk1.shndx < mk2.shndx;
  return mk1.offset < mk2.offset;
}

// Class Merge_map.

// Add a mapping for the bytes from OFFSET to OFFSET + LENGTH in input
// section SHNDX in object OBJECT to an OUTPUT_OFFSET in a merged
// output section.

void
Merge_map::add_mapping(Relobj* object, unsigned int shndx,
		       off_t offset, off_t length, off_t output_offset)
{
  Merge_key mk;
  mk.object = object;
  mk.shndx = shndx;
  mk.offset = offset;

  Merge_value mv;
  mv.length = length;
  mv.output_offset = output_offset;

  std::pair<Merge_mapping::iterator, bool> ins =
    this->merge_map_.insert(std::make_pair(mk, mv));
  gold_assert(ins.second);
}

// Return the output offset for an input address.  The input address
// is at offset OFFSET in section SHNDX in OBJECT.  This sets
// *OUTPUT_OFFSET to the offset in the output section.  This returns
// true if the mapping is known, false otherwise.

bool
Merge_map::get_output_offset(const Relobj* object, unsigned int shndx,
			     off_t offset, off_t* output_offset) const
{
  Merge_key mk;
  mk.object = object;
  mk.shndx = shndx;
  mk.offset = offset;
  Merge_mapping::const_iterator p = this->merge_map_.lower_bound(mk);

  // If MK is not in the map, lower_bound returns the next iterator
  // larger than it.
  if (p == this->merge_map_.end()
      || p->first.object != object
      || p->first.shndx != shndx
      || p->first.offset != offset)
    {
      if (p == this->merge_map_.begin())
	return false;
      --p;
    }

  if (p->first.object != object || p->first.shndx != shndx)
    return false;

  if (offset - p->first.offset >= p->second.length)
    return false;

  *output_offset = p->second.output_offset;
  if (*output_offset != -1)
    *output_offset += (offset - p->first.offset);
  return true;
}

// Class Output_merge_base.

// Return the output offset for an input offset.  The input address is
// at offset OFFSET in section SHNDX in OBJECT.  If we know the
// offset, set *POUTPUT and return true.  Otherwise return false.

bool
Output_merge_base::do_output_offset(const Relobj* object,
				    unsigned int shndx,
				    off_t offset,
				    off_t* poutput) const
{
  return this->merge_map_.get_output_offset(object, shndx, offset, poutput);
}

// Class Output_merge_data.

// Compute the hash code for a fixed-size constant.

size_t
Output_merge_data::Merge_data_hash::operator()(Merge_data_key k) const
{
  const unsigned char* p = this->pomd_->constant(k);
  uint64_t entsize = this->pomd_->entsize();

  // Fowler/Noll/Vo (FNV) hash (type FNV-1a).
  if (sizeof(size_t) == 8)
    {
      size_t result = static_cast<size_t>(14695981039346656037ULL);
      for (uint64_t i = 0; i < entsize; ++i)
	{
	  result &= (size_t) *p++;
	  result *= 1099511628211ULL;
	}
      return result;
    }
  else
    {
      size_t result = 2166136261UL;
      for (uint64_t i = 0; i < entsize; ++i)
	{
	  result ^= (size_t) *p++;
	  result *= 16777619UL;
	}
      return result;
    }
}

// Return whether one hash table key equals another.

bool
Output_merge_data::Merge_data_eq::operator()(Merge_data_key k1,
					     Merge_data_key k2) const
{
  const unsigned char* p1 = this->pomd_->constant(k1);
  const unsigned char* p2 = this->pomd_->constant(k2);
  return memcmp(p1, p2, this->pomd_->entsize()) == 0;
}

// Add a constant to the end of the section contents.

void
Output_merge_data::add_constant(const unsigned char* p)
{
  uint64_t entsize = this->entsize();
  uint64_t addsize = std::max(entsize, this->addralign());
  if (this->len_ + addsize > this->alc_)
    {
      if (this->alc_ == 0)
	this->alc_ = 128 * addsize;
      else
	this->alc_ *= 2;
      this->p_ = static_cast<unsigned char*>(realloc(this->p_, this->alc_));
      if (this->p_ == NULL)
	gold_nomem();
    }

  memcpy(this->p_ + this->len_, p, entsize);
  if (addsize > entsize)
    memset(this->p_ + this->len_ + entsize, 0, addsize - entsize);
  this->len_ += addsize;
}

// Add the input section SHNDX in OBJECT to a merged output section
// which holds fixed length constants.  Return whether we were able to
// handle the section; if not, it will be linked as usual without
// constant merging.

bool
Output_merge_data::do_add_input_section(Relobj* object, unsigned int shndx)
{
  off_t len;
  const unsigned char* p = object->section_contents(shndx, &len, false);

  uint64_t entsize = this->entsize();

  if (len % entsize != 0)
    return false;

  for (off_t i = 0; i < len; i += entsize, p += entsize)
    {
      // Add the constant to the section contents.  If we find that it
      // is already in the hash table, we will remove it again.
      Merge_data_key k = this->len_;
      this->add_constant(p);

      std::pair<Merge_data_hashtable::iterator, bool> ins =
	this->hashtable_.insert(k);

      if (!ins.second)
	{
	  // Key was already present.  Remove the copy we just added.
	  this->len_ -= entsize;
	  k = *ins.first;
	}

      // Record the offset of this constant in the output section.
      this->add_mapping(object, shndx, i, entsize, k);
    }

  return true;
}

// Set the final data size in a merged output section with fixed size
// constants.

void
Output_merge_data::do_set_address(uint64_t, off_t)
{
  // Release the memory we don't need.
  this->p_ = static_cast<unsigned char*>(realloc(this->p_, this->len_));
  gold_assert(this->p_ != NULL);
  this->set_data_size(this->len_);
}

// Write the data of a merged output section with fixed size constants
// to the file.

void
Output_merge_data::do_write(Output_file* of)
{
  of->write(this->offset(), this->p_, this->len_);
}

// Class Output_merge_string.

// Add an input section to a merged string section.

template<typename Char_type>
bool
Output_merge_string<Char_type>::do_add_input_section(Relobj* object,
						     unsigned int shndx)
{
  off_t len;
  const unsigned char* pdata = object->section_contents(shndx, &len, false);

  const Char_type* p = reinterpret_cast<const Char_type*>(pdata);

  if (len % sizeof(Char_type) != 0)
    {
      object->error(_("mergeable string section length not multiple of "
		      "character size"));
      return false;
    }

  // The index I is in bytes, not characters.
  off_t i = 0;
  while (i < len)
    {
      off_t plen = 0;
      for (const Char_type* pl = p; *pl != 0; ++pl)
	{
          // The length PLEN is in characters, not bytes.
	  ++plen;
	  if (i + plen * static_cast<off_t>(sizeof(Char_type)) >= len)
	    {
	      object->error(_("entry in mergeable string section "
			      "not null terminated"));
	      break;
	    }
	}

      const Char_type* str = this->stringpool_.add(p, true, NULL);

      off_t bytelen_with_null = (plen + 1) * sizeof(Char_type);
      this->merged_strings_.push_back(Merged_string(object, shndx, i, str,
						    bytelen_with_null));

      p += plen + 1;
      i += bytelen_with_null;
    }

  return true;
}

// Set the final data size of a merged string section.  This is where
// we finalize the mappings from the input sections to the output
// section.

template<typename Char_type>
void
Output_merge_string<Char_type>::do_set_address(uint64_t, off_t)
{
  this->stringpool_.set_string_offsets();

  for (typename Merged_strings::const_iterator p =
	 this->merged_strings_.begin();
       p != this->merged_strings_.end();
       ++p)
    this->add_mapping(p->object, p->shndx, p->offset, p->length,
		      this->stringpool_.get_offset(p->string));

  this->set_data_size(this->stringpool_.get_strtab_size());

  // Save some memory.
  this->merged_strings_.clear();
}

// Write out a merged string section.

template<typename Char_type>
void
Output_merge_string<Char_type>::do_write(Output_file* of)
{
  this->stringpool_.write(of, this->offset());
}

// Instantiate the templates we need.

template
class Output_merge_string<char>;

template
class Output_merge_string<uint16_t>;

template
class Output_merge_string<uint32_t>;

} // End namespace gold.
