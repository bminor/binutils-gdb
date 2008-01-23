// script-sections.cc -- linker script SECTIONS for gold

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

#include "gold.h"

#include <string>
#include <vector>

#include "script-c.h"
#include "script.h"
#include "script-sections.h"

// Support for the SECTIONS clause in linker scripts.

namespace gold
{

// An element in a SECTIONS clause.

class Sections_element
{
 public:
  Sections_element()
  { }

  virtual ~Sections_element()
  { }

  virtual void
  print(FILE* f) const = 0;
};

// An assignment in a SECTIONS clause outside of an output section.

class Sections_element_assignment : public Sections_element
{
 public:
  Sections_element_assignment(const char* name, size_t namelen,
			      Expression* val, bool provide, bool hidden)
    : assignment_(name, namelen, val, provide, hidden)
  { }

  void
  print(FILE* f) const
  {
    fprintf(f, "  ");
    this->assignment_.print(f);
  }

 private:
  Symbol_assignment assignment_;
};

// An assertion in a SECTIONS clause outside of an output section.

class Sections_element_assertion : public Sections_element
{
 public:
  Sections_element_assertion(Expression* check, const char* message,
			     size_t messagelen)
    : assertion_(check, message, messagelen)
  { }

  void
  print(FILE* f) const
  {
    fprintf(f, "  ");
    this->assertion_.print(f);
  }

 private:
  Script_assertion assertion_;
};

// An element in an output section in a SECTIONS clause.

class Output_section_element
{
 public:
  Output_section_element()
  { }

  virtual ~Output_section_element()
  { }

  virtual void
  print(FILE* f) const = 0;
};

// A symbol assignment in an output section.

class Output_section_element_assignment : public Output_section_element
{
 public:
  Output_section_element_assignment(const char* name, size_t namelen,
				    Expression* val, bool provide,
				    bool hidden)
    : assignment_(name, namelen, val, provide, hidden)
  { }

  void
  print(FILE* f) const
  {
    fprintf(f, "    ");
    this->assignment_.print(f);
  }

 private:
  Symbol_assignment assignment_;
};

// An assertion in an output section.

class Output_section_element_assertion : public Output_section_element
{
 public:
  Output_section_element_assertion(Expression* check, const char* message,
				   size_t messagelen)
    : assertion_(check, message, messagelen)
  { }

  void
  print(FILE* f) const
  {
    fprintf(f, "    ");
    this->assertion_.print(f);
  }

 private:
  Script_assertion assertion_;
};

// A data item in an output section.

class Output_section_element_data : public Output_section_element
{
 public:
  Output_section_element_data(int size, bool is_signed, Expression* val)
    : size_(size), is_signed_(is_signed), val_(val)
  { }

  void
  print(FILE*) const;

 private:
  // The size in bytes.
  int size_;
  // Whether the value is signed.
  bool is_signed_;
  // The value.
  Expression* val_;
};

// Print for debugging.

void
Output_section_element_data::print(FILE* f) const
{
  const char* s;
  switch (this->size_)
    {
    case 1:
      s = "BYTE";
      break;
    case 2:
      s = "SHORT";
      break;
    case 4:
      s = "LONG";
      break;
    case 8:
      if (this->is_signed_)
	s = "SQUAD";
      else
	s = "QUAD";
      break;
    default:
      gold_unreachable();
    }
  fprintf(f, "    %s(", s);
  this->val_->print(f);
  fprintf(f, ")\n");
}

// A fill value setting in an output section.

class Output_section_element_fill : public Output_section_element
{
 public:
  Output_section_element_fill(Expression* val)
    : val_(val)
  { }

  void
  print(FILE* f) const
  {
    fprintf(f, "    FILL(");
    this->val_->print(f);
    fprintf(f, ")\n");
  }

 private:
  // The new fill value.
  Expression* val_;
};

// An input section specification in an output section

class Output_section_element_input : public Output_section_element
{
 public:
  // Note that an Input_section_spec holds some pointers to vectors.
  // This constructor takes ownership of them.  The parser is
  // implemented such that this works.
  Output_section_element_input(const Input_section_spec* spec, bool keep);

  void
  print(FILE* f) const;

 private:
  // An input section pattern.
  struct Input_section_pattern
  {
    std::string pattern;
    Sort_wildcard sort;

    Input_section_pattern(const char* patterna, size_t patternlena,
			  Sort_wildcard sorta)
      : pattern(patterna, patternlena), sort(sorta)
    { }
  };

  typedef std::vector<Input_section_pattern> Input_section_patterns;

  typedef std::vector<std::string> Filename_exclusions;

  // The file name pattern.
  std::string filename_pattern_;
  // How the file names should be sorted.  This may only be
  // SORT_WILDCARD_NONE or SORT_WILDCARD_BY_NAME.
  Sort_wildcard filename_sort_;
  // The list of file names to exclude.
  Filename_exclusions filename_exclusions_;
  // The list of input section patterns.
  Input_section_patterns input_section_patterns_;
  // Whether to keep this section when garbage collecting.
  bool keep_;
};

// Construct Output_section_element_input.  The parser records strings
// as pointers into a copy of the script file, which will go away when
// parsing is complete.  We make sure they are in std::string objects.

Output_section_element_input::Output_section_element_input(
    const Input_section_spec* spec,
    bool keep)
  : filename_pattern_(spec->file.name.value, spec->file.name.length),
    filename_sort_(spec->file.sort),
    filename_exclusions_(),
    input_section_patterns_(),
    keep_(keep)
{
  if (spec->input_sections.exclude != NULL)
    {
      for (String_list::const_iterator p =
	     spec->input_sections.exclude->begin();
	   p != spec->input_sections.exclude->end();
	   ++p)
	this->filename_exclusions_.push_back(*p);
    }

  if (spec->input_sections.sections != NULL)
    {
      Input_section_patterns& isp(this->input_section_patterns_);
      for (String_sort_list::const_iterator p =
	     spec->input_sections.sections->begin();
	   p != spec->input_sections.sections->end();
	   ++p)
	isp.push_back(Input_section_pattern(p->name.value, p->name.length,
					    p->sort));
    }
}

// Print for debugging.

void
Output_section_element_input::print(FILE* f) const
{
  fprintf(f, "    ");

  if (this->keep_)
    fprintf(f, "KEEP(");

  if (!this->filename_pattern_.empty())
    {
      bool need_close_paren = false;
      switch (this->filename_sort_)
	{
	case SORT_WILDCARD_NONE:
	  break;
	case SORT_WILDCARD_BY_NAME:
	  fprintf(f, "SORT_BY_NAME(");
	  need_close_paren = true;
	  break;
	default:
	  gold_unreachable();
	}

      fprintf(f, "%s", this->filename_pattern_.c_str());

      if (need_close_paren)
	fprintf(f, ")");
    }

  if (!this->input_section_patterns_.empty()
      || !this->filename_exclusions_.empty())
    {
      fprintf(f, "(");

      bool need_space = false;
      if (!this->filename_exclusions_.empty())
	{
	  fprintf(f, "EXCLUDE_FILE(");
	  bool need_comma = false;
	  for (Filename_exclusions::const_iterator p =
		 this->filename_exclusions_.begin();
	       p != this->filename_exclusions_.end();
	       ++p)
	    {
	      if (need_comma)
		fprintf(f, ", ");
	      fprintf(f, "%s", p->c_str());
	      need_comma = true;
	    }
	  fprintf(f, ")");
	  need_space = true;
	}

      for (Input_section_patterns::const_iterator p =
	     this->input_section_patterns_.begin();
	   p != this->input_section_patterns_.end();
	   ++p)
	{
	  if (need_space)
	    fprintf(f, " ");

	  int close_parens = 0;
	  switch (p->sort)
	    {
	    case SORT_WILDCARD_NONE:
	      break;
	    case SORT_WILDCARD_BY_NAME:
	      fprintf(f, "SORT_BY_NAME(");
	      close_parens = 1;
	      break;
	    case SORT_WILDCARD_BY_ALIGNMENT:
	      fprintf(f, "SORT_BY_ALIGNMENT(");
	      close_parens = 1;
	      break;
	    case SORT_WILDCARD_BY_NAME_BY_ALIGNMENT:
	      fprintf(f, "SORT_BY_NAME(SORT_BY_ALIGNMENT(");
	      close_parens = 2;
	      break;
	    case SORT_WILDCARD_BY_ALIGNMENT_BY_NAME:
	      fprintf(f, "SORT_BY_ALIGNMENT(SORT_BY_NAME(");
	      close_parens = 2;
	      break;
	    default:
	      gold_unreachable();
	    }

	  fprintf(f, "%s", p->pattern.c_str());

	  for (int i = 0; i < close_parens; ++i)
	    fprintf(f, ")");

	  need_space = true;
	}

      fprintf(f, ")");
    }

  if (this->keep_)
    fprintf(f, ")");

  fprintf(f, "\n");
}

// An output section.

class Output_section_definition : public Sections_element
{
 public:
  Output_section_definition(const char* name, size_t namelen,
			    const Parser_output_section_header* header);

  // Finish the output section with the information in the trailer.
  void
  finish(const Parser_output_section_trailer* trailer);

  // Add a symbol to be defined.
  void
  add_symbol_assignment(const char* name, size_t length, Expression* value,
			bool provide, bool hidden);
  // Add an assertion.
  void
  add_assertion(Expression* check, const char* message, size_t messagelen);

  // Add a data item to the current output section.
  void
  add_data(int size, bool is_signed, Expression* val);

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
  typedef std::vector<Output_section_element*> Output_section_elements;

  // The output section name.
  std::string name_;
  // The address.  This may be NULL.
  Expression* address_;
  // The load address.  This may be NULL.
  Expression* load_address_;
  // The alignment.  This may be NULL.
  Expression* align_;
  // The input section alignment.  This may be NULL.
  Expression* subalign_;
  // The fill value.  This may be NULL.
  Expression* fill_;
  // The list of elements defining the section.
  Output_section_elements elements_;
};

// Constructor.

Output_section_definition::Output_section_definition(
    const char* name,
    size_t namelen,
    const Parser_output_section_header* header)
  : name_(name, namelen),
    address_(header->address),
    load_address_(header->load_address),
    align_(header->align),
    subalign_(header->subalign),
    fill_(NULL),
    elements_()
{
}

// Finish an output section.

void
Output_section_definition::finish(const Parser_output_section_trailer* trailer)
{
  this->fill_ = trailer->fill;
}

// Add a symbol to be defined.

void
Output_section_definition::add_symbol_assignment(const char* name,
						 size_t length,
						 Expression* value,
						 bool provide,
						 bool hidden)
{
  Output_section_element* p = new Output_section_element_assignment(name,
								    length,
								    value,
								    provide,
								    hidden);
  this->elements_.push_back(p);
}

// Add an assertion.

void
Output_section_definition::add_assertion(Expression* check,
					 const char* message,
					 size_t messagelen)
{
  Output_section_element* p = new Output_section_element_assertion(check,
								   message,
								   messagelen);
  this->elements_.push_back(p);
}

// Add a data item to the current output section.

void
Output_section_definition::add_data(int size, bool is_signed, Expression* val)
{
  Output_section_element* p = new Output_section_element_data(size, is_signed,
							      val);
  this->elements_.push_back(p);
}

// Add a setting for the fill value.

void
Output_section_definition::add_fill(Expression* val)
{
  Output_section_element* p = new Output_section_element_fill(val);
  this->elements_.push_back(p);
}

// Add an input section specification.

void
Output_section_definition::add_input_section(const Input_section_spec* spec,
					     bool keep)
{
  Output_section_element* p = new Output_section_element_input(spec, keep);
  this->elements_.push_back(p);
}

// Print for debugging.

void
Output_section_definition::print(FILE* f) const
{
  fprintf(f, "  %s ", this->name_.c_str());

  if (this->address_ != NULL)
    {
      this->address_->print(f);
      fprintf(f, " ");
    }

  fprintf(f, ": ");

  if (this->load_address_ != NULL)
    {
      fprintf(f, "AT(");
      this->load_address_->print(f);
      fprintf(f, ") ");
    }

  if (this->align_ != NULL)
    {
      fprintf(f, "ALIGN(");
      this->align_->print(f);
      fprintf(f, ") ");
    }

  if (this->subalign_ != NULL)
    {
      fprintf(f, "SUBALIGN(");
      this->subalign_->print(f);
      fprintf(f, ") ");
    }

  fprintf(f, "{\n");

  for (Output_section_elements::const_iterator p = this->elements_.begin();
       p != this->elements_.end();
       ++p)
    (*p)->print(f);

  fprintf(f, "  }");

  if (this->fill_ != NULL)
    {
      fprintf(f, " = ");
      this->fill_->print(f);
    }

  fprintf(f, "\n");
}

// Class Script_sections.

Script_sections::Script_sections()
  : saw_sections_clause_(false),
    in_sections_clause_(false),
    sections_elements_(NULL),
    output_section_(NULL)
{
}

// Start a SECTIONS clause.

void
Script_sections::start_sections()
{
  gold_assert(!this->in_sections_clause_ && this->output_section_ == NULL);
  this->saw_sections_clause_ = true;
  this->in_sections_clause_ = true;
  if (this->sections_elements_ == NULL)
    this->sections_elements_ = new Sections_elements;
}

// Finish a SECTIONS clause.

void
Script_sections::finish_sections()
{
  gold_assert(this->in_sections_clause_ && this->output_section_ == NULL);
  this->in_sections_clause_ = false;
}

// Add a symbol to be defined.

void
Script_sections::add_symbol_assignment(const char* name, size_t length,
				       Expression* val, bool provide,
				       bool hidden)
{
  if (this->output_section_ != NULL)
    this->output_section_->add_symbol_assignment(name, length, val,
						 provide, hidden);
  else
    {
      Sections_element* p = new Sections_element_assignment(name, length,
							    val, provide,
							    hidden);
      this->sections_elements_->push_back(p);
    }
}

// Add an assertion.

void
Script_sections::add_assertion(Expression* check, const char* message,
			       size_t messagelen)
{
  if (this->output_section_ != NULL)
    this->output_section_->add_assertion(check, message, messagelen);
  else
    {
      Sections_element* p = new Sections_element_assertion(check, message,
							   messagelen);
      this->sections_elements_->push_back(p);
    }
}

// Start processing entries for an output section.

void
Script_sections::start_output_section(
    const char* name,
    size_t namelen,
    const Parser_output_section_header *header)
{
  Output_section_definition* posd = new Output_section_definition(name,
								  namelen,
								  header);
  this->sections_elements_->push_back(posd);
  gold_assert(this->output_section_ == NULL);
  this->output_section_ = posd;
}

// Stop processing entries for an output section.

void
Script_sections::finish_output_section(
    const Parser_output_section_trailer* trailer)
{
  gold_assert(this->output_section_ != NULL);
  this->output_section_->finish(trailer);
  this->output_section_ = NULL;
}

// Add a data item to the current output section.

void
Script_sections::add_data(int size, bool is_signed, Expression* val)
{
  gold_assert(this->output_section_ != NULL);
  this->output_section_->add_data(size, is_signed, val);
}

// Add a fill value setting to the current output section.

void
Script_sections::add_fill(Expression* val)
{
  gold_assert(this->output_section_ != NULL);
  this->output_section_->add_fill(val);
}

// Add an input section specification to the current output section.

void
Script_sections::add_input_section(const Input_section_spec* spec, bool keep)
{
  gold_assert(this->output_section_ != NULL);
  this->output_section_->add_input_section(spec, keep);
}

// Print the SECTIONS clause to F for debugging.

void
Script_sections::print(FILE* f) const
{
  if (!this->saw_sections_clause_)
    return;

  fprintf(f, "SECTIONS {\n");

  for (Sections_elements::const_iterator p = this->sections_elements_->begin();
       p != this->sections_elements_->end();
       ++p)
    (*p)->print(f);

  fprintf(f, "}\n");
}

} // End namespace gold.
