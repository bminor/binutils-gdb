// script.h -- handle linker scripts for gold   -*- C++ -*-

// Copyright 2006, 2007, 2008 Free Software Foundation, Inc.
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

// We implement a subset of the original GNU ld linker script language
// for compatibility.  The goal is not to implement the entire
// language.  It is merely to implement enough to handle common uses.
// In particular we need to handle /usr/lib/libc.so on a typical
// GNU/Linux system, and we want to handle linker scripts used by the
// Linux kernel build.

#ifndef GOLD_SCRIPT_H
#define GOLD_SCRIPT_H

#include <vector>

namespace gold
{

class General_options;
class Command_line;
class Symbol_table;
class Layout;
class Input_argument;
class Input_objects;
class Input_group;
class Input_file;
class Target;
class Task_token;
class Workqueue;

// This class represents an expression in a linker script.

class Expression
{
 protected:
  // These should only be created by child classes.
  Expression()
  { }

 public:
  virtual ~Expression()
  { }

  // Return the value of the expression.
  uint64_t
  eval(const Symbol_table*, const Layout*);

 protected:
  struct Expression_eval_info;

 public:
  // Compute the value of the expression (implemented by child class).
  // This is public rather than protected because it is called
  // directly by children of Expression on other Expression objects.
  virtual uint64_t
  value(const Expression_eval_info*) = 0;

 private:
  // May not be copied.
  Expression(const Expression&);
  Expression& operator=(const Expression&);
};

// We can read a linker script in two different contexts: when
// initially parsing the command line, and when we find an input file
// which is actually a linker script.  Also some of the data which can
// be set by a linker script can also be set via command line options
// like -e and --defsym.  This means that we have a type of data which
// can be set both during command line option parsing and while
// reading input files.  We store that data in an instance of this
// object.  We will keep pointers to that instance in both the
// Command_line and Layout objects.

class Script_options
{
 public:
  Script_options();

  // The entry address.
  const char*
  entry() const
  { return this->entry_.empty() ? NULL : this->entry_.c_str(); }

  // Set the entry address.
  void
  set_entry(const char* entry, size_t length)
  { this->entry_.assign(entry, length); }

  // Add a symbol to be defined.  These are for symbol definitions
  // which appear outside of a SECTIONS clause.
  void
  add_symbol_assignment(const char* name, size_t length, Expression* value,
			bool provided, bool hidden)
  {
    this->symbol_assignments_.push_back(Symbol_assignment(name, length, value,
							  provided, hidden));
  }

  // Define a symbol from the command line.
  bool
  define_symbol(const char* definition);

  // Add all symbol definitions to the symbol table.
  void
  add_symbols_to_table(Symbol_table*, const Target*);

  // Finalize the symbol values.
  void
  finalize_symbols(Symbol_table*, const Layout*);

 private:
  // We keep a list of symbol assignments.
  struct Symbol_assignment
  {
    // Symbol name.
    std::string name;
    // Expression to assign to symbol.
    Expression* value;
    // Whether the assignment should be provided (only set if there is
    // an undefined reference to the symbol.
    bool provide;
    // Whether the assignment should be hidden.
    bool hidden;
    // The entry in the symbol table.
    Symbol* sym;

    Symbol_assignment(const char* namea, size_t lengtha, Expression* valuea,
		      bool providea, bool hiddena)
      : name(namea, lengtha), value(valuea), provide(providea),
	hidden(hiddena), sym(NULL)
    { }
  };

  typedef std::vector<Symbol_assignment> Symbol_assignments;

  template<int size>
  void
  sized_finalize_symbols(Symbol_table*, const Layout*);

  // The entry address.  This will be empty if not set.
  std::string entry_;
  // Symbols to set.
  Symbol_assignments symbol_assignments_;
};

// FILE was found as an argument on the command line, but was not
// recognized as an ELF file.  Try to read it as a script.  We've
// already read BYTES of data into P.  Return true if the file was
// handled.  This has to handle /usr/lib/libc.so on a GNU/Linux
// system.

bool
read_input_script(Workqueue*, const General_options&, Symbol_table*, Layout*,
		  Dirsearch*, Input_objects*, Input_group*,
		  const Input_argument*, Input_file*, const unsigned char* p,
		  off_t bytes, Task_token* this_blocker,
		  Task_token* next_blocker);

// FILE was found as an argument to --script (-T).
// Read it as a script, and execute its contents immediately.

bool
read_commandline_script(const char* filename, Command_line*);

} // End namespace gold.

#endif // !defined(GOLD_SCRIPT_H)
