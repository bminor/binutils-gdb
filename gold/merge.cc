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

// For each object with merge sections, we store an Object_merge_map.
// This is used to map locations in input sections to a merged output
// section.  The output section itself is not recorded here--it can be
// found in the map_to_output_ field of the Object.

class Object_merge_map
{
 public:
  Object_merge_map()
    : first_shnum_(-1U), first_map_(),
      second_shnum_(-1U), second_map_(),
      section_merge_maps_()
  { }

  ~Object_merge_map();

  // Add a mapping for MERGE_MAP, for the bytes from OFFSET to OFFSET
  // + LENGTH in the input section SHNDX to OUTPUT_OFFSET in the
  // output section.  An OUTPUT_OFFSET of -1 means that the bytes are
  // discarded.
  void
  add_mapping(const Merge_map*, unsigned int shndx, off_t offset, off_t length,
	      off_t output_offset);

  // Get the output offset for an input address in MERGE_MAP.  The
  // input address is at offset OFFSET in section SHNDX.  This sets
  // *OUTPUT_OFFSET to the offset in the output section; this will be
  // -1 if the bytes are not being copied to the output.  This returns
  // true if the mapping is known, false otherwise.
  bool
  get_output_offset(const Merge_map*, unsigned int shndx, off_t offset,
		    off_t *output_offset);

 private:
  // Map input section offsets to a length and an output section
  // offset.  An output section offset of -1 means that this part of
  // the input section is being discarded.
  struct Input_merge_entry
  {
    // The offset in the input section.
    off_t input_offset;
    // The length.
    off_t length;
    // The offset in the output section.
    off_t output_offset;
  };

  // A less-than comparison routine for Input_merge_entry.
  struct Input_merge_compare
  {
    bool
    operator()(const Input_merge_entry& i1, const Input_merge_entry& i2) const
    { return i1.input_offset < i2.input_offset; }
  };

  // A list of entries for a particular section.
  struct Input_merge_map
  {
    // The Merge_map for this section.
    const Merge_map* merge_map;
    // The list of mappings.
    std::vector<Input_merge_entry> entries;
    // Whether the ENTRIES field is sorted by input_offset.
    bool sorted;

    Input_merge_map()
      : merge_map(NULL), entries(), sorted(true)
    { }
  };

  // Map input section indices to merge maps.
  typedef std::map<unsigned int, Input_merge_map*> Section_merge_maps;

  // Return a pointer to the Input_merge_map to use for the input
  // section SHNDX, or NULL.
  Input_merge_map*
  get_input_merge_map(unsigned int shndx);

  // Get or make the the Input_merge_map to use for the section SHNDX
  // with MERGE_MAP.
  Input_merge_map*
  get_or_make_input_merge_map(const Merge_map* merge_map, unsigned int shndx);

  // Any given object file will normally only have a couple of input
  // sections with mergeable contents.  So we keep the first two input
  // section numbers inline, and push any further ones into a map.  A
  // value of -1U in first_shnum_ or second_shnum_ means that we don't
  // have a corresponding entry.
  unsigned int first_shnum_;
  Input_merge_map first_map_;
  unsigned int second_shnum_;
  Input_merge_map second_map_;
  Section_merge_maps section_merge_maps_;
};

// Destructor.

Object_merge_map::~Object_merge_map()
{
  for (Section_merge_maps::iterator p = this->section_merge_maps_.begin();
       p != this->section_merge_maps_.end();
       ++p)
    delete p->second;
}

// Get the Input_merge_map to use for an input section, or NULL.

Object_merge_map::Input_merge_map*
Object_merge_map::get_input_merge_map(unsigned int shndx)
{
  gold_assert(shndx != -1U);
  if (shndx == this->first_shnum_)
    return &this->first_map_;
  if (shndx == this->second_shnum_)
    return &this->second_map_;
  Section_merge_maps::const_iterator p = this->section_merge_maps_.find(shndx);
  if (p != this->section_merge_maps_.end())
    return p->second;
  return NULL;
}

// Get or create the Input_merge_map to use for an input section.

Object_merge_map::Input_merge_map*
Object_merge_map::get_or_make_input_merge_map(const Merge_map* merge_map,
					      unsigned int shndx)
{
  Input_merge_map* map = this->get_input_merge_map(shndx);
  if (map != NULL)
    {
      // For a given input section in a given object, every mapping
      // must be donw with the same Merge_map.
      gold_assert(map->merge_map == merge_map);
      return map;
    }

  // We need to create a new entry.
  if (this->first_shnum_ == -1U)
    {
      this->first_shnum_ = shndx;
      this->first_map_.merge_map = merge_map;
      return &this->first_map_;
    }
  if (this->second_shnum_ == -1U)
    {
      this->second_shnum_ = shndx;
      this->second_map_.merge_map = merge_map;
      return &this->second_map_;
    }

  Input_merge_map* new_map = new Input_merge_map;
  new_map->merge_map = merge_map;
  this->section_merge_maps_[shndx] = new_map;
  return new_map;
}

// Add a mapping.

void
Object_merge_map::add_mapping(const Merge_map* merge_map, unsigned int shndx,
			      off_t input_offset, off_t length,
			      off_t output_offset)
{
  Input_merge_map* map = this->get_or_make_input_merge_map(merge_map, shndx);

  // Try to merge the new entry in the last one we saw.
  if (!map->entries.empty())
    {
      Input_merge_entry& entry(map->entries.back());

      // If this entry is not in order, we need to sort the vector
      // before looking anything up.
      if (input_offset < entry.input_offset + entry.length)
	{
	  gold_assert(input_offset < entry.input_offset
		      && input_offset + length <= entry.input_offset);
	  map->sorted = false;
	}
      else if (entry.input_offset + entry.length == input_offset
	       && (output_offset == -1
		   ? entry.output_offset == -1
		   : entry.output_offset + entry.length == output_offset))
	{
	  entry.length += length;
	  return;
	}
    }

  Input_merge_entry entry;
  entry.input_offset = input_offset;
  entry.length = length;
  entry.output_offset = output_offset;
  map->entries.push_back(entry);
}

// Get the output offset for an input address.

bool
Object_merge_map::get_output_offset(const Merge_map* merge_map,
				    unsigned int shndx, off_t input_offset,
				    off_t *output_offset)
{
  Input_merge_map* map = this->get_input_merge_map(shndx);
  if (map == NULL || map->merge_map != merge_map)
    return false;

  if (!map->sorted)
    {
      std::sort(map->entries.begin(), map->entries.end(),
		Input_merge_compare());
      map->sorted = true;
    }

  Input_merge_entry entry;
  entry.input_offset = input_offset;
  std::vector<Input_merge_entry>::const_iterator p =
    std::lower_bound(map->entries.begin(), map->entries.end(),
		     entry, Input_merge_compare());
  if (p == map->entries.end() || p->input_offset > input_offset)
    {
      if (p == map->entries.begin())
	return false;
      --p;
      gold_assert(p->input_offset <= input_offset);
    }

  if (input_offset - p->input_offset >= p->length)
    return false;

  *output_offset = p->output_offset;
  if (*output_offset != -1)
    *output_offset += (input_offset - p->input_offset);
  return true;
}

// Class Merge_map.

// Add a mapping for the bytes from OFFSET to OFFSET + LENGTH in input
// section SHNDX in object OBJECT to an OUTPUT_OFFSET in a merged
// output section.

void
Merge_map::add_mapping(Relobj* object, unsigned int shndx,
		       off_t offset, off_t length, off_t output_offset)
{
  Object_merge_map* object_merge_map = object->merge_map();
  if (object_merge_map == NULL)
    {
      object_merge_map = new Object_merge_map();
      object->set_merge_map(object_merge_map);
    }

  object_merge_map->add_mapping(this, shndx, offset, length, output_offset);
}

// Return the output offset for an input address.  The input address
// is at offset OFFSET in section SHNDX in OBJECT.  This sets
// *OUTPUT_OFFSET to the offset in the output section.  This returns
// true if the mapping is known, false otherwise.

bool
Merge_map::get_output_offset(const Relobj* object, unsigned int shndx,
			     off_t offset, off_t* output_offset) const
{
  Object_merge_map* object_merge_map = object->merge_map();
  if (object_merge_map == NULL)
    return false;
  return object_merge_map->get_output_offset(this, shndx, offset,
					     output_offset);
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
Output_merge_data::set_final_data_size()
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

// Write the data to a buffer.

void
Output_merge_data::do_write_to_buffer(unsigned char* buffer)
{
  memcpy(buffer, this->p_, this->len_);
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
  const Char_type* pend = p + len;

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
      const Char_type* pl;
      for (pl = p; *pl != 0; ++pl)
	{
	  if (pl >= pend)
	    {
	      object->error(_("entry in mergeable string section "
			      "not null terminated"));
	      break;
	    }
	}

      const Char_type* str = this->stringpool_.add_prefix(p, pl - p, NULL);

      size_t bytelen_with_null = ((pl - p) + 1) * sizeof(Char_type);
      this->merged_strings_.push_back(Merged_string(object, shndx, i, str,
						    bytelen_with_null));

      p = pl + 1;
      i += bytelen_with_null;
    }

  return true;
}

// Finalize the mappings from the input sections to the output
// section, and return the final data size.

template<typename Char_type>
off_t
Output_merge_string<Char_type>::finalize_merged_data()
{
  this->stringpool_.set_string_offsets();

  for (typename Merged_strings::const_iterator p =
	 this->merged_strings_.begin();
       p != this->merged_strings_.end();
       ++p)
    this->add_mapping(p->object, p->shndx, p->offset, p->length,
		      this->stringpool_.get_offset(p->string));

  // Save some memory.
  this->merged_strings_.clear();

  return this->stringpool_.get_strtab_size();
}

template<typename Char_type>
void
Output_merge_string<Char_type>::set_final_data_size()
{
  const off_t final_data_size = this->finalize_merged_data();
  this->set_data_size(final_data_size);
}

// Write out a merged string section.

template<typename Char_type>
void
Output_merge_string<Char_type>::do_write(Output_file* of)
{
  this->stringpool_.write(of, this->offset());
}

// Write a merged string section to a buffer.

template<typename Char_type>
void
Output_merge_string<Char_type>::do_write_to_buffer(unsigned char* buffer)
{
  this->stringpool_.write_to_buffer(buffer, this->data_size());
}

// Instantiate the templates we need.

template
class Output_merge_string<char>;

template
class Output_merge_string<uint16_t>;

template
class Output_merge_string<uint32_t>;

} // End namespace gold.
