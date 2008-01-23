// script-sections.h -- linker script SECTIONS for gold   -*- C++ -*-

// Copyright 2008 Free Software Foundation, Inc.
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

// This is for the support of the SECTIONS clause in linker scripts.

#ifndef GOLD_SCRIPT_SECTIONS_H
#define GOLD_SCRIPT_SECTIONS_H

#include <cstdio>
#include <vector>

namespace gold
{

struct Parser_output_section_header;
struct Parser_output_section_trailer;
struct Input_section_spec;
class Expression;
class Sections_element;
class Output_section_definition;

class Script_sections
{
 public:
  Script_sections();

  // Start a SECTIONS clause.
  void
  start_sections();

  // Finish a SECTIONS clause.
  void
  finish_sections();

  // Return whether we ever saw a SECTIONS clause.  If we did, then
  // all section layout needs to go through this class.
  bool
  saw_sections_clause() const
  { return this->saw_sections_clause_; }

  // Return whether we are currently processing a SECTIONS clause.
  bool
  in_sections_clause() const
  { return this->in_sections_clause_; }

  // Start processing entries for an output section.
  void
  start_output_section(const char* name, size_t namelen,
		       const Parser_output_section_header*);

  // Finish processing entries for an output section.
  void
  finish_output_section(const Parser_output_section_trailer*);

  // Add a data item to the current output section.
  void
  add_data(int size, bool is_signed, Expression* val);

  // Add a symbol to be defined.
  void
  add_symbol_assignment(const char* name, size_t length, Expression* value,
			bool provide, bool hidden);
  // Add an assertion.
  void
  add_assertion(Expression* check, const char* message, size_t messagelen);

  // Add a setting for the fill value.
  void
  add_fill(Expression* val);

  // Add an input section specification.
  void
  add_input_section(const Input_section_spec* spec, bool keep);

  // Print the contents to the FILE.  This is for debugging.
  void
  print(FILE*) const;

 private:
  typedef std::vector<Sections_element*> Sections_elements;

  // True if we ever saw a SECTIONS clause.
  bool saw_sections_clause_;
  // True if we are currently processing a SECTIONS clause.
  bool in_sections_clause_;
  // The list of elements in the SECTIONS clause.
  Sections_elements* sections_elements_;
  // The current output section, if there is one.
  Output_section_definition* output_section_;
};

} // End namespace gold.

#endif // !defined(GOLD_SCRIPT_SECTIONS_H
