// stringpool.h -- a string pool for gold    -*- C++ -*-

#include <string>
#include <list>

// Stringpool
//   Manage a pool of unique strings.

#ifndef GOLD_STRINGPOOL_H
#define GOLD_STRINGPOOL_H

namespace gold
{

class Stringpool
{
 public:
  Stringpool();

  ~Stringpool();

  // Add a string to the pool.  This returns a canonical permanent
  // pointer to the string.
  const char* add(const char*);

  const char* add(const std::string& s)
  { return this->add(s.c_str()); }

  // Add the prefix of a string to the pool.
  const char* add(const char *, size_t);

 private:
  Stringpool(const Stringpool&);
  Stringpool& operator=(const Stringpool&);

  struct stringdata
  {
    // Length of data in buffer.
    size_t len;
    // Allocated size of buffer.
    size_t alc;
    // Buffer.
    char data[1];
  };

  const char* add_string(const char*);

  struct Stringpool_hash
  {
    size_t
    operator()(const char*) const;
  };

  struct Stringpool_eq
  {
    bool
    operator()(const char* p1, const char* p2) const
    { return strcmp(p1, p2) == 0; }
  };

#ifdef HAVE_TR1_UNORDERED_SET
  typedef Unordered_set<const char*, Stringpool_hash, Stringpool_eq,
			std::allocator<const char*>,
			true> String_set_type;
#else
  typedef Unordered_set<const char*, Stringpool_hash, Stringpool_eq,
			std::allocator<const char*> > String_set_type;
#endif

  String_set_type string_set_;
  std::list<stringdata*> strings_;
};

} // End namespace gold.

#endif // !defined(GOLD_STRINGPOOL_H)
