// stringpool.cc -- a string pool for gold

#include "gold.h"

#include <cassert>
#include <cstring>

#include "stringpool.h"

namespace gold
{

Stringpool::Stringpool()
  : string_set_(), strings_()
{
}

Stringpool::~Stringpool()
{
  for (std::list<stringdata*>::iterator p = this->strings_.begin();
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
      size_t result = 14695981039346656037ULL;
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
// the canonical string.

const char*
Stringpool::add_string(const char* s)
{
  const size_t buffer_size = 1000;
  size_t len = strlen(s);

  size_t alc;
  bool front = true;
  if (len >= buffer_size)
    {
      alc = sizeof(stringdata) + len;
      front = false;
    }
  else if (this->strings_.empty())
    alc = sizeof(stringdata) + buffer_size;
  else
    {
      stringdata *psd = this->strings_.front();
      if (len >= psd->alc - psd->len)
	alc = sizeof(stringdata) + buffer_size;
      else
	{
	  char* ret = psd->data + psd->len;
	  memcpy(ret, s, len + 1);
	  psd->len += len + 1;
	  return ret;
	}
    }

  stringdata *psd = reinterpret_cast<stringdata*>(new char[alc]);
  psd->alc = alc;
  memcpy(psd->data, s, len + 1);
  psd->len = len + 1;
  if (front)
    this->strings_.push_front(psd);
  else
    this->strings_.push_back(psd);
  return psd->data;
}

// Add a string to a string pool.

const char*
Stringpool::add(const char* s)
{
  // FIXME: This will look up the entry twice in the hash table.  The
  // problem is that we can't insert S before we canonicalize it.  I
  // don't think there is a way to handle this correct with
  // unordered_set, so this should be replaced with custom code to do
  // what we need, which is to return the empty slot.

  String_set_type::const_iterator p = this->string_set_.find(s);
  if (p != this->string_set_.end())
    return *p;

  const char* ret = this->add_string(s);
  std::pair<String_set_type::iterator, bool> ins =
    this->string_set_.insert(ret);
  assert(ins.second);
  return ret;
}

// Add a prefix of a string to a string pool.

const char*
Stringpool::add(const char* s, size_t len)
{
  // FIXME: This implementation should be rewritten when we rewrite
  // the hash table to avoid copying.
  std::string st(s, len);
  return this->add(st);
}

} // End namespace gold.
