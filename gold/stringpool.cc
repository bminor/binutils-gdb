// stringpool.cc -- a string pool for gold

#include "gold.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>

#include "output.h"
#include "stringpool.h"

namespace gold
{

Stringpool::Stringpool()
  : string_set_(), strings_(), strtab_size_(0), next_index_(1)
{
}

Stringpool::~Stringpool()
{
  for (std::list<Stringdata*>::iterator p = this->strings_.begin();
       p != this->strings_.end();
       ++p)
    delete[] reinterpret_cast<char*>(*p);
}

// Hash function.

size_t
Stringpool::Stringpool_hash::operator()(const char* s) const
{
  // Fowler/Noll/Vo (FNV) hash (type FNV-1a).
  if (sizeof(size_t) == 8)
    {
      size_t result = static_cast<size_t>(14695981039346656037ULL);
      while (*s != '\0')
	{
	  result &= (size_t) *s++;
	  result *= 1099511628211ULL;
	}
      return result;
    }
  else
    {
      size_t result = 2166136261UL;
      while (*s != '\0')
	{
	  result ^= (size_t) *s++;
	  result *= 16777619UL;
	}
      return result;
    }
}

// Add a string to the list of canonical strings.  Return a pointer to
// the canonical string.  If PKEY is not NULL, set *PKEY to the key.

const char*
Stringpool::add_string(const char* s, Key* pkey)
{
  // The size we allocate for a new Stringdata.
  const size_t buffer_size = 1000;
  // The amount we multiply the Stringdata index when calculating the
  // key.
  const size_t key_mult = 1024;
  assert(key_mult >= buffer_size);

  size_t len = strlen(s);

  size_t alc;
  bool front = true;
  if (len >= buffer_size)
    {
      alc = sizeof(Stringdata) + len;
      front = false;
    }
  else if (this->strings_.empty())
    alc = sizeof(Stringdata) + buffer_size;
  else
    {
      Stringdata *psd = this->strings_.front();
      if (len >= psd->alc - psd->len)
	alc = sizeof(Stringdata) + buffer_size;
      else
	{
	  char* ret = psd->data + psd->len;
	  memcpy(ret, s, len + 1);

	  if (pkey != NULL)
	    *pkey = psd->index * key_mult + psd->len;

	  psd->len += len + 1;

	  return ret;
	}
    }

  Stringdata *psd = reinterpret_cast<Stringdata*>(new char[alc]);
  psd->alc = alc - sizeof(Stringdata);
  memcpy(psd->data, s, len + 1);
  psd->len = len + 1;
  psd->index = this->next_index_;
  ++this->next_index_;

  if (pkey != NULL)
    *pkey = psd->index * key_mult;

  if (front)
    this->strings_.push_front(psd);
  else
    this->strings_.push_back(psd);

  return psd->data;
}

// Add a string to a string pool.

const char*
Stringpool::add(const char* s, Key* pkey)
{
  // FIXME: This will look up the entry twice in the hash table.  The
  // problem is that we can't insert S before we canonicalize it.  I
  // don't think there is a way to handle this correctly with
  // unordered_map, so this should be replaced with custom code to do
  // what we need, which is to return the empty slot.

  String_set_type::const_iterator p = this->string_set_.find(s);
  if (p != this->string_set_.end())
    {
      if (pkey != NULL)
	*pkey = p->second.first;
      return p->first;
    }

  Key k;
  const char* ret = this->add_string(s, &k);

  const off_t ozero = 0;
  std::pair<const char*, Val> element(ret, std::make_pair(k, ozero));
  std::pair<String_set_type::iterator, bool> ins =
    this->string_set_.insert(element);
  assert(ins.second);

  if (pkey != NULL)
    *pkey = k;

  return ret;
}

// Add a prefix of a string to a string pool.

const char*
Stringpool::add(const char* s, size_t len, Key* pkey)
{
  // FIXME: This implementation should be rewritten when we rewrite
  // the hash table to avoid copying.
  std::string st(s, len);
  return this->add(st, pkey);
}

const char*
Stringpool::find(const char* s, Key* pkey) const
{
  String_set_type::const_iterator p = this->string_set_.find(s);
  if (p == this->string_set_.end())
    return NULL;

  if (pkey != NULL)
    *pkey = p->second.first;

  return p->first;
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

bool
Stringpool::Stringpool_sort_comparison::operator()(
  String_set_type::iterator it1,
  String_set_type::iterator it2) const
{
  const char* s1 = it1->first;
  const char* s2 = it2->first;
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  int minlen = len1 < len2 ? len1 : len2;
  const char* p1 = s1 + len1 - 1;
  const char* p2 = s2 + len2 - 1;
  for (int i = minlen - 1; i >= 0; --i, --p1, --p2)
    {
      if (*p1 != *p2)
	return *p1 > *p2;
    }
  return len1 > len2;
}

// Return whether s1 is a suffix of s2.

bool
Stringpool::is_suffix(const char* s1, const char* s2)
{
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  if (len1 > len2)
    return false;
  return strcmp(s1, s2 + len2 - len1) == 0;
}

// Turn the stringpool into an ELF strtab: determine the offsets of
// each string in the table.

void
Stringpool::set_string_offsets()
{
  size_t count = this->string_set_.size();

  std::vector<String_set_type::iterator> v;
  v.reserve(count);

  for (String_set_type::iterator p = this->string_set_.begin();
       p != this->string_set_.end();
       ++p)
    v.push_back(p);

  std::sort(v.begin(), v.end(), Stringpool_sort_comparison());

  // Offset 0 is reserved for the empty string.
  off_t offset = 1;
  for (size_t i = 0; i < count; ++i)
    {
      if (v[i]->first[0] == '\0')
	v[i]->second.second = 0;
      else if (i > 0 && Stringpool::is_suffix(v[i]->first, v[i - 1]->first))
	v[i]->second.second = (v[i - 1]->second.second
			       + strlen(v[i - 1]->first)
			       - strlen(v[i]->first));
      else
	{
	  v[i]->second.second = offset;
	  offset += strlen(v[i]->first) + 1;
	}
    }

  this->strtab_size_ = offset;
}

// Get the offset of a string in the ELF strtab.  The string must
// exist.

off_t
Stringpool::get_offset(const char* s) const
{
  String_set_type::const_iterator p = this->string_set_.find(s);
  if (p != this->string_set_.end())
    return p->second.second;
  abort();
}

// Write the ELF strtab into the output file at the specified offset.

void
Stringpool::write(Output_file* of, off_t offset)
{
  unsigned char* viewu = of->get_output_view(offset, this->strtab_size_);
  char* view = reinterpret_cast<char*>(viewu);
  view[0] = '\0';
  for (String_set_type::const_iterator p = this->string_set_.begin();
       p != this->string_set_.end();
       ++p)
    strcpy(view + p->second.second, p->first);
  of->write_output_view(offset, this->strtab_size_, viewu);
}

} // End namespace gold.
