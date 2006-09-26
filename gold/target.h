// target.h -- target support for gold   -*- C++ -*-

// The abstract class Target is the interface for target specific
// support.  It defines abstract methods which each target must
// implement.  Typically there will be one target per processor, but
// in some cases it may be necessary to have subclasses.

// For speed and consistency we want to use inline functions to handle
// relocation processing.  So besides implementations of the abstract
// methods, each target is expected to define a template
// specialization of the relocation functions.

#ifndef GOLD_TARGET_H
#define GOLD_TARGET_H

#include "symtab.h"
#include "elfcpp.h"

namespace gold
{

class Object;

// The abstract class for target specific handling.

class Target
{
 public:
  virtual ~Target()
  { }

  // Return the bit size that this target implements.  This should
  // return 32 or 64.
  int
  get_size() const
  { return this->size_; }

  // Return whether this target is big-endian.
  bool
  is_big_endian() const
  { return this->is_big_endian_; }

  // Whether this target has a specific make_symbol function.
  bool
  has_make_symbol() const
  { return this->has_make_symbol_; }

  // Whether this target has a specific resolve function.
  bool
  has_resolve() const
  { return this->has_resolve_; }

 protected:
  Target(int size, bool is_big_endian, bool has_make_symbol, bool has_resolve)
    : size_(size),
      is_big_endian_(is_big_endian),
      has_make_symbol_(has_make_symbol),
      has_resolve_(has_resolve)
  { }

 private:
  Target(const Target&);
  Target& operator=(const Target&);

  // The target size.
  int size_;
  // Whether this target is big endian.
  bool is_big_endian_;
  // Whether this target has a special make_symbol function.
  bool has_make_symbol_;
  // Whether this target has a special resolve function.
  bool has_resolve_;
};

// The abstract class for a specific size and endianness of target.
// Each actual target implementation class should derive from an
// instantiation of Sized_target.

template<int size, bool big_endian>
class Sized_target : public Target
{
 public:
  // Make a new symbol table entry for the target.  This should be
  // overridden by a target which needs additional information in the
  // symbol table.  This will only be called if has_make_symbol()
  // returns true.
  virtual Sized_symbol<size>*
  make_symbol()
  { abort(); }

  // Resolve a symbol for the target.  This should be overridden by a
  // target which needs to take special action.  TO is the
  // pre-existing symbol.  SYM is the new symbol, seen in OBJECT.
  virtual void
  resolve(Symbol*, const elfcpp::Sym<size, big_endian>&, Object*)
  { abort(); }

 protected:
  Sized_target(bool has_make_symbol, bool has_resolve)
    : Target(size, big_endian, has_make_symbol, has_resolve)
  { }
};

} // End namespace gold.

#endif // !defined(GOLD_TARGET_H)
