// expression.cc -- expressions in linker scripts for gold

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

#include "gold.h"

#include <string>

#include "parameters.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "script.h"
#include "script-c.h"

namespace gold
{

// This file holds the code which handles linker expressions.

// The dot symbol, which linker scripts refer to simply as ".",
// requires special treatment.  The dot symbol is set several times,
// section addresses will refer to it, output sections will change it,
// and it can be set based on the value of other symbols.  We simplify
// the handling by prohibiting setting the dot symbol to the value of
// a non-absolute symbol.

// When evaluating the value of an expression, we pass in a pointer to
// this struct, so that the expression evaluation can find the
// information it needs.

struct Expression::Expression_eval_info
{
  // The symbol table.
  const Symbol_table* symtab;
  // The layout--we use this to get section information.
  const Layout* layout;
  // Whether expressions can refer to the dot symbol.  The dot symbol
  // is only available within a SECTIONS clause.
  bool is_dot_available;
  // Whether the dot symbol currently has a value.
  bool dot_has_value;
  // The current value of the dot symbol.
  uint64_t dot_value;
  // Points to the IS_ABSOLUTE variable, which is set to false if the
  // expression uses a value which is not absolute.
  bool* is_absolute;
};

// Evaluate an expression.

uint64_t
Expression::eval(const Symbol_table* symtab, const Layout* layout)
{
  bool dummy;
  return this->eval_maybe_dot(symtab, layout, false, false, 0, &dummy);
}

// Evaluate an expression which may refer to the dot symbol.

uint64_t
Expression::eval_with_dot(const Symbol_table* symtab, const Layout* layout,
			  bool dot_has_value, uint64_t dot_value,
			  bool* is_absolute)
{
  return this->eval_maybe_dot(symtab, layout, true, dot_has_value, dot_value,
			      is_absolute);
}

// Evaluate an expression which may or may not refer to the dot
// symbol.

uint64_t
Expression::eval_maybe_dot(const Symbol_table* symtab, const Layout* layout,
			   bool is_dot_available, bool dot_has_value,
			   uint64_t dot_value, bool* is_absolute)
{
  Expression_eval_info eei;
  eei.symtab = symtab;
  eei.layout = layout;
  eei.is_dot_available = is_dot_available;
  eei.dot_has_value = dot_has_value;
  eei.dot_value = dot_value;

  // We assume the value is absolute, and only set this to false if we
  // find a section relative reference.
  *is_absolute = true;
  eei.is_absolute = is_absolute;

  return this->value(&eei);
}

// A number.

class Integer_expression : public Expression
{
 public:
  Integer_expression(uint64_t val)
    : val_(val)
  { }

  uint64_t
  value(const Expression_eval_info*)
  { return this->val_; }

  void
  print(FILE* f) const
  { fprintf(f, "0x%llx", static_cast<unsigned long long>(this->val_)); }

 private:
  uint64_t val_;
};

extern "C" Expression*
script_exp_integer(uint64_t val)
{
  return new Integer_expression(val);
}

// An expression whose value is the value of a symbol.

class Symbol_expression : public Expression
{
 public:
  Symbol_expression(const char* name, size_t length)
    : name_(name, length)
  { }

  uint64_t
  value(const Expression_eval_info*);

  void
  print(FILE* f) const
  { fprintf(f, "%s", this->name_.c_str()); }

 private:
  std::string name_;
};

uint64_t
Symbol_expression::value(const Expression_eval_info* eei)
{
  Symbol* sym = eei->symtab->lookup(this->name_.c_str());
  if (sym == NULL || !sym->is_defined())
    {
      gold_error(_("undefined symbol '%s' referenced in expression"),
		 this->name_.c_str());
      return 0;
    }

  // If this symbol does not have an absolute value, then the whole
  // expression does not have an absolute value.  This is not strictly
  // accurate: the subtraction of two symbols in the same section is
  // absolute.  This is unlikely to matter in practice, as this value
  // is only used for error checking.
  if (!sym->value_is_absolute())
    *eei->is_absolute = false;

  if (parameters->get_size() == 32)
    return eei->symtab->get_sized_symbol<32>(sym)->value();
  else if (parameters->get_size() == 64)
    return eei->symtab->get_sized_symbol<64>(sym)->value();
  else
    gold_unreachable();
}

// An expression whose value is the value of the special symbol ".".
// This is only valid within a SECTIONS clause.

class Dot_expression : public Expression
{
 public:
  Dot_expression()
  { }

  uint64_t
  value(const Expression_eval_info*);

  void
  print(FILE* f) const
  { fprintf(f, "."); }
};

uint64_t
Dot_expression::value(const Expression_eval_info* eei)
{
  if (!eei->is_dot_available)
    {
      gold_error(_("invalid reference to dot symbol outside of "
		   "SECTIONS clause"));
      return 0;
    }
  else if (!eei->dot_has_value)
    {
      gold_error(_("invalid reference to dot symbol before "
		   "it has been given a value"));
      return 0;
    }
  return eei->dot_value;
}

// A string.  This is either the name of a symbol, or ".".

extern "C" Expression*
script_exp_string(const char* name, size_t length)
{
  if (length == 1 && name[0] == '.')
    return new Dot_expression();
  else
    return new Symbol_expression(name, length);
}

// A unary expression.

class Unary_expression : public Expression
{
 public:
  Unary_expression(Expression* arg)
    : arg_(arg)
  { }

  ~Unary_expression()
  { delete this->arg_; }

 protected:
  uint64_t
  arg_value(const Expression_eval_info* eei) const
  { return this->arg_->value(eei); }

  void
  arg_print(FILE* f) const
  { this->arg_->print(f); }

 private:
  Expression* arg_;
};

// Handle unary operators.  We use a preprocessor macro as a hack to
// capture the C operator.

#define UNARY_EXPRESSION(NAME, OPERATOR)			\
  class Unary_ ## NAME : public Unary_expression		\
  {								\
   public:							\
    Unary_ ## NAME(Expression* arg)				\
      : Unary_expression(arg)					\
    { }								\
    								\
    uint64_t							\
    value(const Expression_eval_info* eei)			\
    { return OPERATOR this->arg_value(eei); }			\
								\
    void							\
    print(FILE* f) const					\
    {								\
      fprintf(f, "(%s ", #OPERATOR);				\
      this->arg_print(f);					\
      fprintf(f, ")");						\
    }								\
  };								\
  								\
  extern "C" Expression*					\
  script_exp_unary_ ## NAME(Expression* arg)			\
  {								\
    return new Unary_ ## NAME(arg);				\
  }

UNARY_EXPRESSION(minus, -)
UNARY_EXPRESSION(logical_not, !)
UNARY_EXPRESSION(bitwise_not, ~)

// A binary expression.

class Binary_expression : public Expression
{
 public:
  Binary_expression(Expression* left, Expression* right)
    : left_(left), right_(right)
  { }

  ~Binary_expression()
  {
    delete this->left_;
    delete this->right_;
  }

 protected:
  uint64_t
  left_value(const Expression_eval_info* eei) const
  { return this->left_->value(eei); }

  uint64_t
  right_value(const Expression_eval_info* eei) const
  { return this->right_->value(eei); }

  void
  left_print(FILE* f) const
  { this->left_->print(f); }

  void
  right_print(FILE* f) const
  { this->right_->print(f); }

  // This is a call to function FUNCTION_NAME.  Print it.  This is for
  // debugging.
  void
  print_function(FILE* f, const char *function_name) const
  {
    fprintf(f, "%s(", function_name);
    this->left_print(f);
    fprintf(f, ", ");
    this->right_print(f);
    fprintf(f, ")");
  }

 private:
  Expression* left_;
  Expression* right_;
};

// Handle binary operators.  We use a preprocessor macro as a hack to
// capture the C operator.

#define BINARY_EXPRESSION(NAME, OPERATOR)				\
  class Binary_ ## NAME : public Binary_expression			\
  {									\
  public:								\
    Binary_ ## NAME(Expression* left, Expression* right)		\
      : Binary_expression(left, right)					\
    { }									\
									\
    uint64_t								\
    value(const Expression_eval_info* eei)				\
    {									\
      return (this->left_value(eei)					\
	      OPERATOR this->right_value(eei));				\
    }									\
									\
    void								\
    print(FILE* f) const						\
    {									\
      fprintf(f, "(");							\
      this->left_print(f);						\
      fprintf(f, " %s ", #OPERATOR);					\
      this->right_print(f);						\
      fprintf(f, ")");							\
    }									\
  };									\
									\
  extern "C" Expression*						\
  script_exp_binary_ ## NAME(Expression* left, Expression* right)	\
  {									\
    return new Binary_ ## NAME(left, right);				\
  }

BINARY_EXPRESSION(mult, *)
BINARY_EXPRESSION(div, /)
BINARY_EXPRESSION(mod, %)
BINARY_EXPRESSION(add, +)
BINARY_EXPRESSION(sub, -)
BINARY_EXPRESSION(lshift, <<)
BINARY_EXPRESSION(rshift, >>)
BINARY_EXPRESSION(eq, ==)
BINARY_EXPRESSION(ne, !=)
BINARY_EXPRESSION(le, <=)
BINARY_EXPRESSION(ge, >=)
BINARY_EXPRESSION(lt, <)
BINARY_EXPRESSION(gt, >)
BINARY_EXPRESSION(bitwise_and, &)
BINARY_EXPRESSION(bitwise_xor, ^)
BINARY_EXPRESSION(bitwise_or, |)
BINARY_EXPRESSION(logical_and, &&)
BINARY_EXPRESSION(logical_or, ||)

// A trinary expression.

class Trinary_expression : public Expression
{
 public:
  Trinary_expression(Expression* arg1, Expression* arg2, Expression* arg3)
    : arg1_(arg1), arg2_(arg2), arg3_(arg3)
  { }

  ~Trinary_expression()
  {
    delete this->arg1_;
    delete this->arg2_;
    delete this->arg3_;
  }

 protected:
  uint64_t
  arg1_value(const Expression_eval_info* eei) const
  { return this->arg1_->value(eei); }

  uint64_t
  arg2_value(const Expression_eval_info* eei) const
  { return this->arg2_->value(eei); }

  uint64_t
  arg3_value(const Expression_eval_info* eei) const
  { return this->arg3_->value(eei); }

  void
  arg1_print(FILE* f) const
  { this->arg1_->print(f); }

  void
  arg2_print(FILE* f) const
  { this->arg2_->print(f); }

  void
  arg3_print(FILE* f) const
  { this->arg3_->print(f); }

 private:
  Expression* arg1_;
  Expression* arg2_;
  Expression* arg3_;
};

// The conditional operator.

class Trinary_cond : public Trinary_expression
{
 public:
  Trinary_cond(Expression* arg1, Expression* arg2, Expression* arg3)
    : Trinary_expression(arg1, arg2, arg3)
  { }

  uint64_t
  value(const Expression_eval_info* eei)
  {
    return (this->arg1_value(eei)
	    ? this->arg2_value(eei)
	    : this->arg3_value(eei));
  }

  void
  print(FILE* f) const
  {
    fprintf(f, "(");
    this->arg1_print(f);
    fprintf(f, " ? ");
    this->arg2_print(f);
    fprintf(f, " : ");
    this->arg3_print(f);
    fprintf(f, ")");
  }
};

extern "C" Expression*
script_exp_trinary_cond(Expression* arg1, Expression* arg2, Expression* arg3)
{
  return new Trinary_cond(arg1, arg2, arg3);
}

// Max function.

class Max_expression : public Binary_expression
{
 public:
  Max_expression(Expression* left, Expression* right)
    : Binary_expression(left, right)
  { }

  uint64_t
  value(const Expression_eval_info* eei)
  { return std::max(this->left_value(eei), this->right_value(eei)); }

  void
  print(FILE* f) const
  { this->print_function(f, "MAX"); }
};

extern "C" Expression*
script_exp_function_max(Expression* left, Expression* right)
{
  return new Max_expression(left, right);
}

// Min function.

class Min_expression : public Binary_expression
{
 public:
  Min_expression(Expression* left, Expression* right)
    : Binary_expression(left, right)
  { }

  uint64_t
  value(const Expression_eval_info* eei)
  { return std::min(this->left_value(eei), this->right_value(eei)); }

  void
  print(FILE* f) const
  { this->print_function(f, "MIN"); }
};

extern "C" Expression*
script_exp_function_min(Expression* left, Expression* right)
{
  return new Min_expression(left, right);
}

// Align function.

class Align_expression : public Binary_expression
{
 public:
  Align_expression(Expression* left, Expression* right)
    : Binary_expression(left, right)
  { }

  uint64_t
  value(const Expression_eval_info* eei)
  {
    uint64_t align = this->right_value(eei);
    uint64_t value = this->left_value(eei);
    if (align <= 1)
      return value;
    return ((value + align - 1) / align) * align;
  }

  void
  print(FILE* f) const
  { this->print_function(f, "ALIGN"); }
};

extern "C" Expression*
script_exp_function_align(Expression* left, Expression* right)
{
  return new Align_expression(left, right);
}

// Assert function.

class Assert_expression : public Unary_expression
{
 public:
  Assert_expression(Expression* arg, const char* message, size_t length)
    : Unary_expression(arg), message_(message, length)
  { }

  uint64_t
  value(const Expression_eval_info* eei)
  {
    uint64_t value = this->arg_value(eei);
    if (!value)
      gold_error("%s", this->message_.c_str());
    return value;
  }

  void
  print(FILE* f) const
  {
    fprintf(f, "ASSERT(");
    this->arg_print(f);
    fprintf(f, ", %s)", this->message_.c_str());
  }

 private:
  std::string message_;
};

extern "C" Expression*
script_exp_function_assert(Expression* expr, const char* message,
			   size_t length)
{
  return new Assert_expression(expr, message, length);
}

// Addr function.

class Addr_expression : public Expression
{
 public:
  Addr_expression(const char* section_name, size_t section_name_len)
    : section_name_(section_name, section_name_len)
  { }

  uint64_t
  value(const Expression_eval_info*);

  void
  print(FILE* f) const
  { fprintf(f, "ADDR(%s)", this->section_name_.c_str()); }

 private:
  std::string section_name_;
};

uint64_t
Addr_expression::value(const Expression_eval_info* eei)
{
  const char* section_name = this->section_name_.c_str();
  Output_section* os = eei->layout->find_output_section(section_name);
  if (os == NULL)
    {
      gold_error("ADDR called on nonexistent output section '%s'",
		 section_name);
      return 0;
    }

  // Note that the address of a section is an absolute address, and we
  // should not clear *EEI->IS_ABSOLUTE here.

  return os->address();
}

extern "C" Expression*
script_exp_function_addr(const char* section_name, size_t section_name_len)
{
  return new Addr_expression(section_name, section_name_len);
}

// Functions.

extern "C" Expression*
script_exp_function_defined(const char*, size_t)
{
  gold_fatal(_("DEFINED not implemented"));
}

extern "C" Expression*
script_exp_function_sizeof_headers()
{
  gold_fatal(_("SIZEOF_HEADERS not implemented"));
}

extern "C" Expression*
script_exp_function_alignof(const char*, size_t)
{
  gold_fatal(_("ALIGNOF not implemented"));
}

extern "C" Expression*
script_exp_function_sizeof(const char*, size_t)
{
  gold_fatal(_("SIZEOF not implemented"));
}

extern "C" Expression*
script_exp_function_loadaddr(const char*, size_t)
{
  gold_fatal(_("LOADADDR not implemented"));
}

extern "C" Expression*
script_exp_function_origin(const char*, size_t)
{
  gold_fatal(_("ORIGIN not implemented"));
}

extern "C" Expression*
script_exp_function_length(const char*, size_t)
{
  gold_fatal(_("LENGTH not implemented"));
}

extern "C" Expression*
script_exp_function_constant(const char*, size_t)
{
  gold_fatal(_("CONSTANT not implemented"));
}

extern "C" Expression*
script_exp_function_absolute(Expression*)
{
  gold_fatal(_("ABSOLUTE not implemented"));
}

extern "C" Expression*
script_exp_function_data_segment_align(Expression*, Expression*)
{
  gold_fatal(_("DATA_SEGMENT_ALIGN not implemented"));
}

extern "C" Expression*
script_exp_function_data_segment_relro_end(Expression*, Expression*)
{
  gold_fatal(_("DATA_SEGMENT_RELRO_END not implemented"));
}

extern "C" Expression*
script_exp_function_data_segment_end(Expression*)
{
  gold_fatal(_("DATA_SEGMENT_END not implemented"));
}

extern "C" Expression*
script_exp_function_segment_start(const char*, size_t, Expression*)
{
  gold_fatal(_("SEGMENT_START not implemented"));
}

} // End namespace gold.
