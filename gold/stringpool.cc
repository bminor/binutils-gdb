// stringpool.cc -- a string pool for gold

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

#include <cstring>
#include <algorithm>
#include <vector>

#include "output.h"
#include "parameters.h"
#include "stringpool.h"

namespace gold
{

template<typename Stringpool_char>
Stringpool_template<Stringpool_char>::Stringpool_template()
  : string_set_(), strings_(), strtab_size_(0), next_index_(1),
    next_uncopied_key_(-1), zero_null_(true)
{
}

template<typename Stringpool_char>
void
Stringpool_template<Stringpool_char>::clear()
{
  for (typename std::list<Stringdata*>::iterator p = this->strings_.begin();
       p != this->strings_.end();
       ++p)
    delete[] reinterpret_cast<char*>(*p);
  this->strings_.clear();
  this->string_set_.clear();
}

template<typename Stringpool_char>
Stringpool_template<Stringpool_char>::~Stringpool_template()
{
  this->clear();
}

// Return the length of a string of arbitrary character type.

template<typename Stringpool_char>
size_t
Stringpool_template<Stringpool_char>::string_length(const Stringpool_char* p)
{
  size_t len = 0;
  for (; *p != 0; ++p)
    ++len;
  return len;
}

// Specialize string_length for char.  Maybe we could just use
// std::char_traits<>::length?

template<>
inline size_t
Stringpool_template<char>::string_length(const char* p)
{
  return strlen(p);
}

// Compare two strings of arbitrary character type for equality.

template<typename Stringpool_char>
bool
Stringpool_template<Stringpool_char>::string_equal(const Stringpool_char* s1,
						   const Stringpool_char* s2)
{
  while (*s1 != 0)
    if (*s1++ != *s2++)
      return false;
  return *s2 == 0;
}

// Specialize string_equal for char.

template<>
inline bool
Stringpool_template<char>::string_equal(const char* s1, const char* s2)
{
  return strcmp(s1, s2) == 0;
}

// Equality comparison function for the hash table.

template<typename Stringpool_char>
inline bool
Stringpool_template<Stringpool_char>::Stringpool_eq::operator()(
    const Hashkey& h1,
    const Hashkey& h2) const
{
  return (h1.hash_code == h2.hash_code
	  && h1.length == h2.length
	  && memcmp(h1.string, h2.string,
		    h1.length * sizeof(Stringpool_char)) == 0);
}

// Hash function.  The length is in characters, not bytes.

template<typename Stringpool_char>
size_t
Stringpool_template<Stringpool_char>::string_hash(const Stringpool_char* s,
						  size_t length)
{
  // Fowler/Noll/Vo (FNV) hash (type FNV-1a).
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
  if (sizeof(size_t) > 4)
    {
      size_t result = static_cast<size_t>(14695981039346656037ULL);
      for (size_t i = 0; i < length * sizeof(Stringpool_char); ++i)
	{
	  result ^= static_cast<size_t>(*p++);
	  result *= 1099511628211ULL;
	}
      return result;
    }
  else
    {
      size_t result = 2166136261UL;
      for (size_t i = 0; i < length * sizeof(Stringpool_char); ++i)
	{
	  result ^= static_cast<size_t>(*p++);
	  result *= 16777619UL;
	}
      return result;
    }
}

// Add the string S to the list of canonical strings.  Return a
// pointer to the canonical string.  If PKEY is not NULL, set *PKEY to
// the key.  LENGTH is the length of S in characters.  Note that S may
// not be NUL terminated.

template<typename Stringpool_char>
const Stringpool_char*
Stringpool_template<Stringpool_char>::add_string(const Stringpool_char* s,
						 size_t len,
						 Key* pkey)
{
  // We are in trouble if we've already computed the string offsets.
  gold_assert(this->strtab_size_ == 0);

  // The size we allocate for a new Stringdata.
  const size_t buffer_size = 1000;
  // The amount we multiply the Stringdata index when calculating the
  // key.
  const size_t key_mult = 1024;
  gold_assert(key_mult >= buffer_size);

  // Convert len to the number of bytes we need to allocate, including
  // the null character.
  len = (len + 1) * sizeof(Stringpool_char);

  size_t alc;
  bool front = true;
  if (len > buffer_size)
    {
      alc = sizeof(Stringdata) + len;
      front = false;
    }
  else if (this->strings_.empty())
    alc = sizeof(Stringdata) + buffer_size;
  else
    {
      Stringdata *psd = this->strings_.front();
      if (len > psd->alc - psd->len)
	alc = sizeof(Stringdata) + buffer_size;
      else
	{
	  char* ret = psd->data + psd->len;
	  memcpy(ret, s, len - sizeof(Stringpool_char));
	  memset(ret + len - sizeof(Stringpool_char), 0,
		 sizeof(Stringpool_char));

	  if (pkey != NULL)
	    *pkey = psd->index * key_mult + psd->len;

	  psd->len += len;

	  return reinterpret_cast<const Stringpool_char*>(ret);
	}
    }

  Stringdata *psd = reinterpret_cast<Stringdata*>(new char[alc]);
  psd->alc = alc - sizeof(Stringdata);
  memcpy(psd->data, s, len - sizeof(Stringpool_char));
  memset(psd->data + len - sizeof(Stringpool_char), 0,
	 sizeof(Stringpool_char));
  psd->len = len;
  psd->index = this->next_index_;
  ++this->next_index_;

  if (pkey != NULL)
    *pkey = psd->index * key_mult;

  if (front)
    this->strings_.push_front(psd);
  else
    this->strings_.push_back(psd);

  return reinterpret_cast<const Stringpool_char*>(psd->data);
}

// Add a string to a string pool.

template<typename Stringpool_char>
const Stringpool_char*
Stringpool_template<Stringpool_char>::add(const Stringpool_char* s, bool copy,
                                          Key* pkey)
{
  typedef std::pair<typename String_set_type::iterator, bool> Insert_type;

  if (!copy)
    {
      // When we don't need to copy the string, we can call insert
      // directly.

      const Key k = this->next_uncopied_key_;
      const off_t ozero = 0;
      std::pair<Hashkey, Hashval> element(Hashkey(s),
					  std::make_pair(k, ozero));

      Insert_type ins = this->string_set_.insert(element);

      typename String_set_type::const_iterator p = ins.first;

      if (ins.second)
	{
	  // We just added the string.  The key value has now been
	  // used.
	  --this->next_uncopied_key_;
	}
      else
	{
	  gold_assert(k != p->second.first);
	}

      if (pkey != NULL)
	*pkey = p->second.first;
      return p->first.string;
    }

  return this->add_prefix(s, string_length(s), pkey);
}

// Add a prefix of a string to a string pool.

template<typename Stringpool_char>
const Stringpool_char*
Stringpool_template<Stringpool_char>::add_prefix(const Stringpool_char* s,
                                                 size_t len,
                                                 Key* pkey)
{
  // When adding an entry, this will look it up twice in the hash
  // table.  The problem is that we can't insert S before we
  // canonicalize it by copying it into the canonical list. The hash
  // code will only be computed once, so this isn't all that
  // expensive.

  Hashkey hk(s, len);
  typename String_set_type::const_iterator p = this->string_set_.find(hk);
  if (p != this->string_set_.end())
    {
      if (pkey != NULL)
	*pkey = p->second.first;
      return p->first.string;
    }

  Key k;
  hk.string = this->add_string(s, len, &k);
  // The contents of the string stay the same, so we don't need to
  // adjust hk.hash_code or hk.length.

  const off_t ozero = 0;
  std::pair<Hashkey, Hashval> element(hk, std::make_pair(k, ozero));

  typedef std::pair<typename String_set_type::iterator, bool> Insert_type;
  Insert_type ins = this->string_set_.insert(element);
  gold_assert(ins.second);

  if (pkey != NULL)
    *pkey = k;
  return hk.string;
}

template<typename Stringpool_char>
const Stringpool_char*
Stringpool_template<Stringpool_char>::find(const Stringpool_char* s,
					   Key* pkey) const
{
  Hashkey hk(s);
  typename String_set_type::const_iterator p = this->string_set_.find(hk);
  if (p == this->string_set_.end())
    return NULL;

  if (pkey != NULL)
    *pkey = p->second.first;

  return p->first.string;
}

// Comparison routine used when sorting into an ELF strtab.  We want
// to sort this so that when one string is a suffix of another, we
// always see the shorter string immediately after the longer string.
// For example, we want to see these strings in this order:
//   abcd
//   cd
//   d
// When strings are not suffixes, we don't care what order they are
// in, but we need to ensure that suffixes wind up next to each other.
// So we do a reversed lexicographic sort on the reversed string.

template<typename Stringpool_char>
bool
Stringpool_template<Stringpool_char>::Stringpool_sort_comparison::operator()(
  const Stringpool_sort_info& sort_info1,
  const Stringpool_sort_info& sort_info2) const
{
  const Hashkey& h1(sort_info1->first);
  const Hashkey& h2(sort_info2->first);
  const Stringpool_char* s1 = h1.string;
  const Stringpool_char* s2 = h2.string;
  const size_t len1 = h1.length;
  const size_t len2 = h2.length;
  const size_t minlen = len1 < len2 ? len1 : len2;
  const Stringpool_char* p1 = s1 + len1 - 1;
  const Stringpool_char* p2 = s2 + len2 - 1;
  for (size_t i = minlen; i > 0; --i, --p1, --p2)
    {
      if (*p1 != *p2)
	return *p1 > *p2;
    }
  return len1 > len2;
}

// Return whether s1 is a suffix of s2.

template<typename Stringpool_char>
bool
Stringpool_template<Stringpool_char>::is_suffix(const Stringpool_char* s1,
                                                size_t len1,
						const Stringpool_char* s2,
                                                size_t len2)
{
  if (len1 > len2)
    return false;
  return memcmp(s1, s2 + len2 - len1, len1 * sizeof(Stringpool_char)) == 0;
}

// Turn the stringpool into an ELF strtab: determine the offsets of
// each string in the table.

template<typename Stringpool_char>
void
Stringpool_template<Stringpool_char>::set_string_offsets()
{
  if (this->strtab_size_ != 0)
    {
      // We've already computed the offsets.
      return;
    }

  const size_t charsize = sizeof(Stringpool_char);

  // Offset 0 may be reserved for the empty string.
  off_t offset = this->zero_null_ ? charsize : 0;

  // Sorting to find suffixes can take over 25% of the total CPU time
  // used by the linker.  Since it's merely an optimization to reduce
  // the strtab size, and gives a relatively small benefit (it's
  // typically rare for a symbol to be a suffix of another), we only
  // take the time to sort when the user asks for heavy optimization.
  if (parameters->optimization_level() < 2)
    {
      for (typename String_set_type::iterator curr = this->string_set_.begin();
           curr != this->string_set_.end();
           curr++)
        {
          if (this->zero_null_ && curr->first.string[0] == 0)
            curr->second.second = 0;
          else
            {
              curr->second.second = offset;
              offset += (curr->first.length + 1) * charsize;
            }
        }
    }
  else
    {
      size_t count = this->string_set_.size();

      std::vector<Stringpool_sort_info> v;
      v.reserve(count);

      for (typename String_set_type::iterator p = this->string_set_.begin();
           p != this->string_set_.end();
           ++p)
        v.push_back(Stringpool_sort_info(p));

      std::sort(v.begin(), v.end(), Stringpool_sort_comparison());

      for (typename std::vector<Stringpool_sort_info>::iterator last = v.end(),
             curr = v.begin();
           curr != v.end();
           last = curr++)
        {
          if (this->zero_null_ && (*curr)->first.string[0] == 0)
            (*curr)->second.second = 0;
          else if (last != v.end()
                   && is_suffix((*curr)->first.string,
				(*curr)->first.length,
                                (*last)->first.string,
				(*last)->first.length))
            (*curr)->second.second = ((*last)->second.second
				      + (((*last)->first.length
					  - (*curr)->first.length)
					 * charsize));
          else
            {
              (*curr)->second.second = offset;
              offset += ((*curr)->first.length + 1) * charsize;
            }
        }
    }

  this->strtab_size_ = offset;
}

// Get the offset of a string in the ELF strtab.  The string must
// exist.

template<typename Stringpool_char>
off_t
Stringpool_template<Stringpool_char>::get_offset(const Stringpool_char* s)
  const
{
  gold_assert(this->strtab_size_ != 0);
  typename String_set_type::const_iterator p = this->string_set_.find(s);
  if (p != this->string_set_.end())
    return p->second.second;
  gold_unreachable();
}

// Write the ELF strtab into the buffer.

template<typename Stringpool_char>
void
Stringpool_template<Stringpool_char>::write_to_buffer(unsigned char* buffer,
                                                      size_t bufsize)
{
  gold_assert(this->strtab_size_ != 0);
   // Quiet the compiler in opt mode.
  if (bufsize < static_cast<size_t>(this->strtab_size_))
    gold_assert(bufsize >= static_cast<size_t>(this->strtab_size_));
  if (this->zero_null_)
    buffer[0] = '\0';
  for (typename String_set_type::const_iterator p = this->string_set_.begin();
       p != this->string_set_.end();
       ++p)
    {
      const int len = (p->first.length + 1) * sizeof(Stringpool_char);
      gold_assert(p->second.second + len <= this->strtab_size_);
      memcpy(buffer + p->second.second, p->first.string, len);
    }
}

// Write the ELF strtab into the output file at the specified offset.

template<typename Stringpool_char>
void
Stringpool_template<Stringpool_char>::write(Output_file* of, off_t offset)
{
  gold_assert(this->strtab_size_ != 0);
  unsigned char* view = of->get_output_view(offset, this->strtab_size_);
  this->write_to_buffer(view, this->strtab_size_);
  of->write_output_view(offset, this->strtab_size_, view);
}

// Print statistical information to stderr.  This is used for --stats.

template<typename Stringpool_char>
void
Stringpool_template<Stringpool_char>::print_stats(const char* name) const
{
#if defined(HAVE_TR1_UNORDERED_MAP) || defined(HAVE_EXT_HASH_MAP)
  fprintf(stderr, _("%s: %s entries: %zu; buckets: %zu\n"),
	  program_name, name, this->string_set_.size(),
	  this->string_set_.bucket_count());
#else
  fprintf(stderr, _("%s: %s entries: %zu\n"),
	  program_name, name, this->table_.size());
#endif
  fprintf(stderr, _("%s: %s Stringdata structures: %zu\n"),
	  program_name, name, this->strings_.size());
}

// Instantiate the templates we need.

template
class Stringpool_template<char>;

template
class Stringpool_template<uint16_t>;

template
class Stringpool_template<uint32_t>;

} // End namespace gold.
