// symtab.h -- the gold symbol table   -*- C++ -*-

// Symbol_table
//   The symbol table.

#include "gold.h"

#include <string>
#include <utility>

#include "elfcpp.h"
#include "targetsize.h"
#include "object.h"
#include "workqueue.h"

#ifndef GOLD_SYMTAB_H
#define GOLD_SYMTAB_H

namespace gold
{

// An entry in the symbol table.  The symbol table can have a lot of
// entries, so we don't want this class to get too big.

template<int size>
class Symbol
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Value;
  typedef typename Size_types<size>::Unsigned_type Size;

 private:
  // Every symbol has a unique name, more or less, so we use
  // std::string for the name.  There are only a few versions in a
  // given link, so for them we point into a pool.
  std::string name_;
  const char* version_;
  Object* object_;
  unsigned int shnum_;
  Value value_;
  Size size_;
  elfcpp::STT type_ : 4;
  elfcpp::STB binding_ : 4;
  elfcpp:STV visibility_ : 2;
  unsigned int other_ : 6;
};

// The main linker symbol table.

template<int size>
class Symbol_table
{
 public:
  Symbol_table();

  // Return a pointer to a symbol specified by name.
  Symbol*
  lookup(const std::string& name) const;

  // Return a pointer to a symbol specified by name plus version.
  Symbol*
  lookup(const std::string& name, const char* version) const;

  Task_token&
  token() const
  { return this->token_; }

 private:
  Symbol_table(const Symbol_table&);
  Symbol_table& operator=(const Symbol_table&);

  typedef std::pair<std::string, std::string> Symbol_table_key;

  Unordered_map<Symbol_table_key, Symbol<size>*> table_;
  Task_token token_;
};

} // End namespace gold.

#endif // !defined(GOLD_SYMTAB_H)
