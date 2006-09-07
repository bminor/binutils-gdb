// symtab.h -- the gold symbol table   -*- C++ -*-

// Symbol_table
//   The symbol table.

#include <string>
#include <utility>

#include "elfcpp.h"
#include "targetsize.h"
#include "stringpool.h"

#ifndef GOLD_SYMTAB_H
#define GOLD_SYMTAB_H

namespace gold
{

class Object;

template<int size, bool big_endian>
class Sized_object;

template<int size, bool big_endian>
class Sized_target;

// The base class of an entry in the symbol table.  The symbol table
// can have a lot of entries, so we don't want this class to big.
// Size dependent fields can be found in the template class
// Sized_symbol.  Targets may support their own derived classes.

class Symbol
{
 public:
  // Return the symbol name.
  const char*
  name() const
  { return this->name_; }

  // Return the symbol version.  This will return NULL for an
  // unversioned symbol.
  const char*
  version() const
  { return this->version_; }

  // Return the object with which this symbol is associated.
  Object*
  object() const
  { return this->object_; }

  // Return the symbol binding.
  elfcpp::STB
  binding() const
  { return this->binding_; }

  // Return the symbol type.
  elfcpp::STT
  type() const
  { return this->type_; }

  // Return the symbol visibility.
  elfcpp::STV
  visibility() const
  { return this->visibility_; }

  // Return the non-visibility part of the st_other field.
  unsigned char
  other() const
  { return this->other_; }

  // Return the section index.
  unsigned int
  shnum() const
  { return this->shnum_; }

  // Return whether this symbol is a forwarder.  This will never be
  // true of a symbol found in the hash table, but may be true of
  // symbol pointers attached to object files.
  bool
  is_forwarder() const
  { return this->is_forwarder_; }

  // Mark this symbol as a forwarder.
  void
  set_forwarder()
  { this->is_forwarder_ = true; }

  // Return whether this symbol was seen in a dynamic object.
  bool
  in_dyn() const
  { return this->in_dyn_; }

  // Mark this symbol as seen in a dynamic object.
  void
  set_in_dyn()
  { this->in_dyn_ = true; }

 protected:
  // Instances of this class should always be created at a specific
  // size.
  Symbol()
  { }

  // Initialize fields from an ELF symbol in OBJECT.
  template<int size, bool big_endian>
  void
  init_base(const char *name, const char* version, Object* object,
	    const elfcpp::Sym<size, big_endian>&);

  // Override existing symbol.
  template<int size, bool big_endian>
  void
  override_base(const elfcpp::Sym<size, big_endian>&, Object* object);

 private:
  Symbol(const Symbol&);
  Symbol& operator=(const Symbol&);

  // Symbol name (expected to point into a Stringpool).
  const char* name_;
  // Symbol version (expected to point into a Stringpool).  This may
  // be NULL.
  const char* version_;
  // Object in which symbol is defined, or in which it was first seen.
  Object* object_;
  // Section number in object_ in which symbol is defined.
  unsigned int shnum_;
  // Symbol type.
  elfcpp::STT type_ : 4;
  // Symbol binding.
  elfcpp::STB binding_ : 4;
  // Symbol visibility.
  elfcpp::STV visibility_ : 2;
  // Rest of symbol st_other field.
  unsigned int other_ : 6;
  // True if this symbol always requires special target-specific
  // handling.
  bool is_special_ : 1;
  // True if this is the default version of the symbol.
  bool is_def_ : 1;
  // True if this symbol really forwards to another symbol.  This is
  // used when we discover after the fact that two different entries
  // in the hash table really refer to the same symbol.  This will
  // never be set for a symbol found in the hash table, but may be set
  // for a symbol found in the list of symbols attached to an Object.
  // It forwards to the symbol found in the forwarders_ map of
  // Symbol_table.
  bool is_forwarder_ : 1;
  // True if we've seen this symbol in a dynamic object.
  bool in_dyn_ : 1;
};

// The parts of a symbol which are size specific.  Using a template
// derived class like this helps us use less space on a 32-bit system.

template<int size>
class Sized_symbol : public Symbol
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Value_type;
  typedef typename elfcpp::Elf_types<size>::Elf_WXword Size_type;

  Sized_symbol()
  { }

  // Initialize fields from an ELF symbol in OBJECT.
  template<bool big_endian>
  void
  init(const char *name, const char* version, Object* object,
       const elfcpp::Sym<size, big_endian>&);

  // Override existing symbol.
  template<bool big_endian>
  void
  override(const elfcpp::Sym<size, big_endian>&, Object* object);

  // Return the symbol's value.
  Value_type
  value() const
  { return this->value_; }

  // Return the symbol's size (we can't call this 'size' because that
  // is a template parameter).
  Size_type
  symsize() const
  { return this->size_; }

 private:
  Sized_symbol(const Sized_symbol&);
  Sized_symbol& operator=(const Sized_symbol&);

  // Symbol value.
  Value_type value_;
  // Symbol size.
  Size_type size_;
};

// The main linker symbol table.

class Symbol_table
{
 public:
  Symbol_table();

  ~Symbol_table();

  // Add COUNT external symbols from OBJECT to the symbol table.  SYMS
  // is the symbols, SYM_NAMES is their names, SYM_NAME_SIZE is the
  // size of SYM_NAMES.  This sets SYMPOINTERS to point to the symbols
  // in the symbol table.
  template<int size, bool big_endian>
  void
  add_from_object(Sized_object<size, big_endian>* object,
		  const elfcpp::Sym<size, big_endian>* syms,
		  size_t count, const char* sym_names, size_t sym_name_size,
		  Symbol** sympointers);

  // Return the real symbol associated with the forwarder symbol FROM.
  Symbol*
  resolve_forwards(Symbol* from) const;

  // Return the size of the symbols in the table.
  int
  get_size() const
  { return this->size_; }

  // Return the sized version of a symbol in this table.
  template<int size>
  Sized_symbol<size>*
  get_sized_symbol(Symbol*);

  template<int size>
  const Sized_symbol<size>*
  get_sized_symbol(const Symbol*);

 private:
  Symbol_table(const Symbol_table&);
  Symbol_table& operator=(const Symbol_table&);

  // Set the size of the symbols in the table.
  void
  set_size(int size)
  { this->size_ = size; }

  // Make FROM a forwarder symbol to TO.
  void
  make_forwarder(Symbol* from, Symbol* to);

  // Add a symbol.
  template<int size, bool big_endian>
  Symbol*
  add_from_object(Sized_object<size, big_endian>*, const char *name,
		  const char *version, bool def,
		  const elfcpp::Sym<size, big_endian>& sym);

  // Resolve symbols.
  template<int size, bool big_endian>
  static void
  resolve(Sized_symbol<size>* to,
	  const elfcpp::Sym<size, big_endian>& sym,
	  Object*);

  template<int size, bool big_endian>
  static void
  resolve(Sized_symbol<size>* to, const Sized_symbol<size>* from);

  typedef std::pair<const char*, const char*> Symbol_table_key;

  struct Symbol_table_hash
  {
    size_t
    operator()(const Symbol_table_key&) const;
  };

  struct Symbol_table_eq
  {
    bool
    operator()(const Symbol_table_key&, const Symbol_table_key&) const;
  };

  typedef Unordered_map<Symbol_table_key, Symbol*, Symbol_table_hash,
			Symbol_table_eq> Symbol_table_type;

  // The size of the symbols in the symbol table (32 or 64).
  int size_;

  // The symbol table itself.
  Symbol_table_type table_;

  // A pool of symbol names.
  Stringpool namepool_;

  // Forwarding symbols.
  Unordered_map<Symbol*, Symbol*> forwarders_;
};

// We inline get_sized_symbol for efficiency.

template<int size>
Sized_symbol<size>*
Symbol_table::get_sized_symbol(Symbol* sym)
{
  assert(size == this->get_size());
  return static_cast<Sized_symbol<size>*>(sym);
}

template<int size>
const Sized_symbol<size>*
Symbol_table::get_sized_symbol(const Symbol* sym)
{
  assert(size == this->get_size());
  return static_cast<const Sized_symbol<size>*>(sym);
}

} // End namespace gold.

#endif // !defined(GOLD_SYMTAB_H)
