// script.cc -- handle linker scripts for gold.

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
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "filenames.h"

#include "elfcpp.h"
#include "dirsearch.h"
#include "options.h"
#include "fileread.h"
#include "workqueue.h"
#include "readsyms.h"
#include "parameters.h"
#include "layout.h"
#include "symtab.h"
#include "script.h"
#include "script-c.h"

namespace gold
{

// A token read from a script file.  We don't implement keywords here;
// all keywords are simply represented as a string.

class Token
{
 public:
  // Token classification.
  enum Classification
  {
    // Token is invalid.
    TOKEN_INVALID,
    // Token indicates end of input.
    TOKEN_EOF,
    // Token is a string of characters.
    TOKEN_STRING,
    // Token is a quoted string of characters.
    TOKEN_QUOTED_STRING,
    // Token is an operator.
    TOKEN_OPERATOR,
    // Token is a number (an integer).
    TOKEN_INTEGER
  };

  // We need an empty constructor so that we can put this STL objects.
  Token()
    : classification_(TOKEN_INVALID), value_(NULL), value_length_(0),
      opcode_(0), lineno_(0), charpos_(0)
  { }

  // A general token with no value.
  Token(Classification classification, int lineno, int charpos)
    : classification_(classification), value_(NULL), value_length_(0),
      opcode_(0), lineno_(lineno), charpos_(charpos)
  {
    gold_assert(classification == TOKEN_INVALID
		|| classification == TOKEN_EOF);
  }

  // A general token with a value.
  Token(Classification classification, const char* value, size_t length,
	int lineno, int charpos)
    : classification_(classification), value_(value), value_length_(length),
      opcode_(0), lineno_(lineno), charpos_(charpos)
  {
    gold_assert(classification != TOKEN_INVALID
		&& classification != TOKEN_EOF);
  }

  // A token representing an operator.
  Token(int opcode, int lineno, int charpos)
    : classification_(TOKEN_OPERATOR), value_(NULL), value_length_(0),
      opcode_(opcode), lineno_(lineno), charpos_(charpos)
  { }

  // Return whether the token is invalid.
  bool
  is_invalid() const
  { return this->classification_ == TOKEN_INVALID; }

  // Return whether this is an EOF token.
  bool
  is_eof() const
  { return this->classification_ == TOKEN_EOF; }

  // Return the token classification.
  Classification
  classification() const
  { return this->classification_; }

  // Return the line number at which the token starts.
  int
  lineno() const
  { return this->lineno_; }

  // Return the character position at this the token starts.
  int
  charpos() const
  { return this->charpos_; }

  // Get the value of a token.

  const char*
  string_value(size_t* length) const
  {
    gold_assert(this->classification_ == TOKEN_STRING
		|| this->classification_ == TOKEN_QUOTED_STRING);
    *length = this->value_length_;
    return this->value_;
  }

  int
  operator_value() const
  {
    gold_assert(this->classification_ == TOKEN_OPERATOR);
    return this->opcode_;
  }

  uint64_t
  integer_value() const
  {
    gold_assert(this->classification_ == TOKEN_INTEGER);
    // Null terminate.
    std::string s(this->value_, this->value_length_);
    return strtoull(s.c_str(), NULL, 0);
  }

 private:
  // The token classification.
  Classification classification_;
  // The token value, for TOKEN_STRING or TOKEN_QUOTED_STRING or
  // TOKEN_INTEGER.
  const char* value_;
  // The length of the token value.
  size_t value_length_;
  // The token value, for TOKEN_OPERATOR.
  int opcode_;
  // The line number where this token started (one based).
  int lineno_;
  // The character position within the line where this token started
  // (one based).
  int charpos_;
};

// This class handles lexing a file into a sequence of tokens.

class Lex
{
 public:
  // We unfortunately have to support different lexing modes, because
  // when reading different parts of a linker script we need to parse
  // things differently.
  enum Mode
  {
    // Reading an ordinary linker script.
    LINKER_SCRIPT,
    // Reading an expression in a linker script.
    EXPRESSION,
    // Reading a version script.
    VERSION_SCRIPT
  };

  Lex(const char* input_string, size_t input_length, int parsing_token)
    : input_string_(input_string), input_length_(input_length),
      current_(input_string), mode_(LINKER_SCRIPT),
      first_token_(parsing_token), token_(),
      lineno_(1), linestart_(input_string)
  { }

  // Read a file into a string.
  static void
  read_file(Input_file*, std::string*);

  // Return the next token.
  const Token*
  next_token();

  // Return the current lexing mode.
  Lex::Mode
  mode() const
  { return this->mode_; }

  // Set the lexing mode.
  void
  set_mode(Mode mode)
  { this->mode_ = mode; }

 private:
  Lex(const Lex&);
  Lex& operator=(const Lex&);

  // Make a general token with no value at the current location.
  Token
  make_token(Token::Classification c, const char* start) const
  { return Token(c, this->lineno_, start - this->linestart_ + 1); }

  // Make a general token with a value at the current location.
  Token
  make_token(Token::Classification c, const char* v, size_t len,
	     const char* start)
    const
  { return Token(c, v, len, this->lineno_, start - this->linestart_ + 1); }

  // Make an operator token at the current location.
  Token
  make_token(int opcode, const char* start) const
  { return Token(opcode, this->lineno_, start - this->linestart_ + 1); }

  // Make an invalid token at the current location.
  Token
  make_invalid_token(const char* start)
  { return this->make_token(Token::TOKEN_INVALID, start); }

  // Make an EOF token at the current location.
  Token
  make_eof_token(const char* start)
  { return this->make_token(Token::TOKEN_EOF, start); }

  // Return whether C can be the first character in a name.  C2 is the
  // next character, since we sometimes need that.
  inline bool
  can_start_name(char c, char c2);

  // Return whether C can appear in a name which has already started.
  inline bool
  can_continue_name(char c);

  // Return whether C, C2, C3 can start a hex number.
  inline bool
  can_start_hex(char c, char c2, char c3);

  // Return whether C can appear in a hex number.
  inline bool
  can_continue_hex(char c);

  // Return whether C can start a non-hex number.
  static inline bool
  can_start_number(char c);

  // Return whether C can appear in a non-hex number.
  inline bool
  can_continue_number(char c)
  { return Lex::can_start_number(c); }

  // If C1 C2 C3 form a valid three character operator, return the
  // opcode.  Otherwise return 0.
  static inline int
  three_char_operator(char c1, char c2, char c3);

  // If C1 C2 form a valid two character operator, return the opcode.
  // Otherwise return 0.
  static inline int
  two_char_operator(char c1, char c2);

  // If C1 is a valid one character operator, return the opcode.
  // Otherwise return 0.
  static inline int
  one_char_operator(char c1);

  // Read the next token.
  Token
  get_token(const char**);

  // Skip a C style /* */ comment.  Return false if the comment did
  // not end.
  bool
  skip_c_comment(const char**);

  // Skip a line # comment.  Return false if there was no newline.
  bool
  skip_line_comment(const char**);

  // Build a token CLASSIFICATION from all characters that match
  // CAN_CONTINUE_FN.  The token starts at START.  Start matching from
  // MATCH.  Set *PP to the character following the token.
  inline Token
  gather_token(Token::Classification,
	       bool (Lex::*can_continue_fn)(char),
	       const char* start, const char* match, const char** pp);

  // Build a token from a quoted string.
  Token
  gather_quoted_string(const char** pp);

  // The string we are tokenizing.
  const char* input_string_;
  // The length of the string.
  size_t input_length_;
  // The current offset into the string.
  const char* current_;
  // The current lexing mode.
  Mode mode_;
  // The code to use for the first token.  This is set to 0 after it
  // is used.
  int first_token_;
  // The current token.
  Token token_;
  // The current line number.
  int lineno_;
  // The start of the current line in the string.
  const char* linestart_;
};

// Read the whole file into memory.  We don't expect linker scripts to
// be large, so we just use a std::string as a buffer.  We ignore the
// data we've already read, so that we read aligned buffers.

void
Lex::read_file(Input_file* input_file, std::string* contents)
{
  off_t filesize = input_file->file().filesize();
  contents->clear();
  contents->reserve(filesize);

  off_t off = 0;
  unsigned char buf[BUFSIZ];
  while (off < filesize)
    {
      off_t get = BUFSIZ;
      if (get > filesize - off)
	get = filesize - off;
      input_file->file().read(off, get, buf);
      contents->append(reinterpret_cast<char*>(&buf[0]), get);
      off += get;
    }
}

// Return whether C can be the start of a name, if the next character
// is C2.  A name can being with a letter, underscore, period, or
// dollar sign.  Because a name can be a file name, we also permit
// forward slash, backslash, and tilde.  Tilde is the tricky case
// here; GNU ld also uses it as a bitwise not operator.  It is only
// recognized as the operator if it is not immediately followed by
// some character which can appear in a symbol.  That is, when we
// don't know that we are looking at an expression, "~0" is a file
// name, and "~ 0" is an expression using bitwise not.  We are
// compatible.

inline bool
Lex::can_start_name(char c, char c2)
{
  switch (c)
    {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'Q': case 'P': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'q': case 'p': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case '_': case '.': case '$':
      return true;

    case '/': case '\\':
      return this->mode_ == LINKER_SCRIPT;

    case '~':
      return this->mode_ == LINKER_SCRIPT && can_continue_name(c2);

    default:
      return false;
    }
}

// Return whether C can continue a name which has already started.
// Subsequent characters in a name are the same as the leading
// characters, plus digits and "=+-:[],?*".  So in general the linker
// script language requires spaces around operators, unless we know
// that we are parsing an expression.

inline bool
Lex::can_continue_name(char c)
{
  switch (c)
    {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'Q': case 'P': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'q': case 'p': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case '_': case '.': case '$':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return true;

    case '/': case '\\': case '~':
    case '=': case '+': case '-':
    case ':': case '[': case ']':
    case ',': case '?': case '*':
      return this->mode_ == LINKER_SCRIPT;

    default:
      return false;
    }
}

// For a number we accept 0x followed by hex digits, or any sequence
// of digits.  The old linker accepts leading '$' for hex, and
// trailing HXBOD.  Those are for MRI compatibility and we don't
// accept them.  The old linker also accepts trailing MK for mega or
// kilo.  FIXME: Those are mentioned in the documentation, and we
// should accept them.

// Return whether C1 C2 C3 can start a hex number.

inline bool
Lex::can_start_hex(char c1, char c2, char c3)
{
  if (c1 == '0' && (c2 == 'x' || c2 == 'X'))
    return this->can_continue_hex(c3);
  return false;
}

// Return whether C can appear in a hex number.

inline bool
Lex::can_continue_hex(char c)
{
  switch (c)
    {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return true;

    default:
      return false;
    }
}

// Return whether C can start a non-hex number.

inline bool
Lex::can_start_number(char c)
{
  switch (c)
    {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return true;

    default:
      return false;
    }
}

// If C1 C2 C3 form a valid three character operator, return the
// opcode (defined in the yyscript.h file generated from yyscript.y).
// Otherwise return 0.

inline int
Lex::three_char_operator(char c1, char c2, char c3)
{
  switch (c1)
    {
    case '<':
      if (c2 == '<' && c3 == '=')
	return LSHIFTEQ;
      break;
    case '>':
      if (c2 == '>' && c3 == '=')
	return RSHIFTEQ;
      break;
    default:
      break;
    }
  return 0;
}

// If C1 C2 form a valid two character operator, return the opcode
// (defined in the yyscript.h file generated from yyscript.y).
// Otherwise return 0.

inline int
Lex::two_char_operator(char c1, char c2)
{
  switch (c1)
    {
    case '=':
      if (c2 == '=')
	return EQ;
      break;
    case '!':
      if (c2 == '=')
	return NE;
      break;
    case '+':
      if (c2 == '=')
	return PLUSEQ;
      break;
    case '-':
      if (c2 == '=')
	return MINUSEQ;
      break;
    case '*':
      if (c2 == '=')
	return MULTEQ;
      break;
    case '/':
      if (c2 == '=')
	return DIVEQ;
      break;
    case '|':
      if (c2 == '=')
	return OREQ;
      if (c2 == '|')
	return OROR;
      break;
    case '&':
      if (c2 == '=')
	return ANDEQ;
      if (c2 == '&')
	return ANDAND;
      break;
    case '>':
      if (c2 == '=')
	return GE;
      if (c2 == '>')
	return RSHIFT;
      break;
    case '<':
      if (c2 == '=')
	return LE;
      if (c2 == '<')
	return LSHIFT;
      break;
    default:
      break;
    }
  return 0;
}

// If C1 is a valid operator, return the opcode.  Otherwise return 0.

inline int
Lex::one_char_operator(char c1)
{
  switch (c1)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '!':
    case '&':
    case '|':
    case '^':
    case '~':
    case '<':
    case '>':
    case '=':
    case '?':
    case ',':
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case ';':
      return c1;
    default:
      return 0;
    }
}

// Skip a C style comment.  *PP points to just after the "/*".  Return
// false if the comment did not end.

bool
Lex::skip_c_comment(const char** pp)
{
  const char* p = *pp;
  while (p[0] != '*' || p[1] != '/')
    {
      if (*p == '\0')
	{
	  *pp = p;
	  return false;
	}

      if (*p == '\n')
	{
	  ++this->lineno_;
	  this->linestart_ = p + 1;
	}
      ++p;
    }

  *pp = p + 2;
  return true;
}

// Skip a line # comment.  Return false if there was no newline.

bool
Lex::skip_line_comment(const char** pp)
{
  const char* p = *pp;
  size_t skip = strcspn(p, "\n");
  if (p[skip] == '\0')
    {
      *pp = p + skip;
      return false;
    }

  p += skip + 1;
  ++this->lineno_;
  this->linestart_ = p;
  *pp = p;

  return true;
}

// Build a token CLASSIFICATION from all characters that match
// CAN_CONTINUE_FN.  Update *PP.

inline Token
Lex::gather_token(Token::Classification classification,
		  bool (Lex::*can_continue_fn)(char),
		  const char* start,
		  const char* match,
		  const char **pp)
{
  while ((this->*can_continue_fn)(*match))
    ++match;
  *pp = match;
  return this->make_token(classification, start, match - start, start);
}

// Build a token from a quoted string.

Token
Lex::gather_quoted_string(const char** pp)
{
  const char* start = *pp;
  const char* p = start;
  ++p;
  size_t skip = strcspn(p, "\"\n");
  if (p[skip] != '"')
    return this->make_invalid_token(start);
  *pp = p + skip + 1;
  return this->make_token(Token::TOKEN_QUOTED_STRING, p, skip, start);
}

// Return the next token at *PP.  Update *PP.  General guideline: we
// require linker scripts to be simple ASCII.  No unicode linker
// scripts.  In particular we can assume that any '\0' is the end of
// the input.

Token
Lex::get_token(const char** pp)
{
  const char* p = *pp;

  while (true)
    {
      if (*p == '\0')
	{
	  *pp = p;
	  return this->make_eof_token(p);
	}

      // Skip whitespace quickly.
      while (*p == ' ' || *p == '\t')
	++p;

      if (*p == '\n')
	{
	  ++p;
	  ++this->lineno_;
	  this->linestart_ = p;
	  continue;
	}

      // Skip C style comments.
      if (p[0] == '/' && p[1] == '*')
	{
	  int lineno = this->lineno_;
	  int charpos = p - this->linestart_ + 1;

	  *pp = p + 2;
	  if (!this->skip_c_comment(pp))
	    return Token(Token::TOKEN_INVALID, lineno, charpos);
	  p = *pp;

	  continue;
	}

      // Skip line comments.
      if (*p == '#')
	{
	  *pp = p + 1;
	  if (!this->skip_line_comment(pp))
	    return this->make_eof_token(p);
	  p = *pp;
	  continue;
	}

      // Check for a name.
      if (this->can_start_name(p[0], p[1]))
	return this->gather_token(Token::TOKEN_STRING,
				  &Lex::can_continue_name,
				  p, p + 1, pp);

      // We accept any arbitrary name in double quotes, as long as it
      // does not cross a line boundary.
      if (*p == '"')
	{
	  *pp = p;
	  return this->gather_quoted_string(pp);
	}

      // Check for a number.

      if (this->can_start_hex(p[0], p[1], p[2]))
	return this->gather_token(Token::TOKEN_INTEGER,
				  &Lex::can_continue_hex,
				  p, p + 3, pp);

      if (Lex::can_start_number(p[0]))
	return this->gather_token(Token::TOKEN_INTEGER,
				  &Lex::can_continue_number,
				  p, p + 1, pp);

      // Check for operators.

      int opcode = Lex::three_char_operator(p[0], p[1], p[2]);
      if (opcode != 0)
	{
	  *pp = p + 3;
	  return this->make_token(opcode, p);
	}

      opcode = Lex::two_char_operator(p[0], p[1]);
      if (opcode != 0)
	{
	  *pp = p + 2;
	  return this->make_token(opcode, p);
	}

      opcode = Lex::one_char_operator(p[0]);
      if (opcode != 0)
	{
	  *pp = p + 1;
	  return this->make_token(opcode, p);
	}

      return this->make_token(Token::TOKEN_INVALID, p);
    }
}

// Return the next token.

const Token*
Lex::next_token()
{
  // The first token is special.
  if (this->first_token_ != 0)
    {
      this->token_ = Token(this->first_token_, 0, 0);
      this->first_token_ = 0;
      return &this->token_;
    }

  this->token_ = this->get_token(&this->current_);

  // Don't let an early null byte fool us into thinking that we've
  // reached the end of the file.
  if (this->token_.is_eof()
      && (static_cast<size_t>(this->current_ - this->input_string_)
	  < this->input_length_))
    this->token_ = this->make_invalid_token(this->current_);

  return &this->token_;
}

// A trivial task which waits for THIS_BLOCKER to be clear and then
// clears NEXT_BLOCKER.  THIS_BLOCKER may be NULL.

class Script_unblock : public Task
{
 public:
  Script_unblock(Task_token* this_blocker, Task_token* next_blocker)
    : this_blocker_(this_blocker), next_blocker_(next_blocker)
  { }

  ~Script_unblock()
  {
    if (this->this_blocker_ != NULL)
      delete this->this_blocker_;
  }

  Task_token*
  is_runnable()
  {
    if (this->this_blocker_ != NULL && this->this_blocker_->is_blocked())
      return this->this_blocker_;
    return NULL;
  }

  void
  locks(Task_locker* tl)
  { tl->add(this, this->next_blocker_); }

  void
  run(Workqueue*)
  { }

  std::string
  get_name() const
  { return "Script_unblock"; }

 private:
  Task_token* this_blocker_;
  Task_token* next_blocker_;
};

// Class Script_options.

Script_options::Script_options()
  : entry_(), symbol_assignments_()
{
}

// Add any symbols we are defining to the symbol table.

void
Script_options::add_symbols_to_table(Symbol_table* symtab,
				     const Target* target)
{
  for (Symbol_assignments::iterator p = this->symbol_assignments_.begin();
       p != this->symbol_assignments_.end();
       ++p)
    {
      elfcpp::STV vis = p->hidden ? elfcpp::STV_HIDDEN : elfcpp::STV_DEFAULT;
      p->sym = symtab->define_as_constant(target,
					  p->name.c_str(),
					  NULL, // version
					  0, // value
					  0, // size
					  elfcpp::STT_NOTYPE,
					  elfcpp::STB_GLOBAL,
					  vis,
					  0, // nonvis
					  p->provide);
    }
}

// Finalize symbol values.

void
Script_options::finalize_symbols(Symbol_table* symtab, const Layout* layout)
{
  if (parameters->get_size() == 32)
    {
#if defined(HAVE_TARGET_32_LITTLE) || defined(HAVE_TARGET_32_BIG)
      this->sized_finalize_symbols<32>(symtab, layout);
#else
      gold_unreachable();
#endif
    }
  else if (parameters->get_size() == 64)
    {
#if defined(HAVE_TARGET_64_LITTLE) || defined(HAVE_TARGET_64_BIG)
      this->sized_finalize_symbols<64>(symtab, layout);
#else
      gold_unreachable();
#endif
    }
  else
    gold_unreachable();
}

template<int size>
void
Script_options::sized_finalize_symbols(Symbol_table* symtab,
				       const Layout* layout)
{
  for (Symbol_assignments::iterator p = this->symbol_assignments_.begin();
       p != this->symbol_assignments_.end();
       ++p)
    {
      if (p->sym != NULL)
	{
	  Sized_symbol<size>* ssym = symtab->get_sized_symbol<size>(p->sym);
	  ssym->set_value(p->value->eval(symtab, layout));
	}
    }
}

// This class holds data passed through the parser to the lexer and to
// the parser support functions.  This avoids global variables.  We
// can't use global variables because we need not be called by a
// singleton thread.

class Parser_closure
{
 public:
  Parser_closure(const char* filename,
		 const Position_dependent_options& posdep_options,
		 bool in_group, bool is_in_sysroot,
                 Command_line* command_line,
		 Script_options* script_options,
		 Lex* lex)
    : filename_(filename), posdep_options_(posdep_options),
      in_group_(in_group), is_in_sysroot_(is_in_sysroot),
      command_line_(command_line), script_options_(script_options),
      lex_(lex), lineno_(0), charpos_(0), lex_mode_stack_(), inputs_(NULL)
  { }

  // Return the file name.
  const char*
  filename() const
  { return this->filename_; }

  // Return the position dependent options.  The caller may modify
  // this.
  Position_dependent_options&
  position_dependent_options()
  { return this->posdep_options_; }

  // Return whether this script is being run in a group.
  bool
  in_group() const
  { return this->in_group_; }

  // Return whether this script was found using a directory in the
  // sysroot.
  bool
  is_in_sysroot() const
  { return this->is_in_sysroot_; }

  // Returns the Command_line structure passed in at constructor time.
  // This value may be NULL.  The caller may modify this, which modifies
  // the passed-in Command_line object (not a copy).
  Command_line*
  command_line()
  { return this->command_line_; }

  // Return the options which may be set by a script.
  Script_options*
  script_options()
  { return this->script_options_; }

  // Return the next token, and advance.
  const Token*
  next_token()
  {
    const Token* token = this->lex_->next_token();
    this->lineno_ = token->lineno();
    this->charpos_ = token->charpos();
    return token;
  }

  // Set a new lexer mode, pushing the current one.
  void
  push_lex_mode(Lex::Mode mode)
  {
    this->lex_mode_stack_.push_back(this->lex_->mode());
    this->lex_->set_mode(mode);
  }

  // Pop the lexer mode.
  void
  pop_lex_mode()
  {
    gold_assert(!this->lex_mode_stack_.empty());
    this->lex_->set_mode(this->lex_mode_stack_.back());
    this->lex_mode_stack_.pop_back();
  }

  // Return the line number of the last token.
  int
  lineno() const
  { return this->lineno_; }

  // Return the character position in the line of the last token.
  int
  charpos() const
  { return this->charpos_; }

  // Return the list of input files, creating it if necessary.  This
  // is a space leak--we never free the INPUTS_ pointer.
  Input_arguments*
  inputs()
  {
    if (this->inputs_ == NULL)
      this->inputs_ = new Input_arguments();
    return this->inputs_;
  }

  // Return whether we saw any input files.
  bool
  saw_inputs() const
  { return this->inputs_ != NULL && !this->inputs_->empty(); }

 private:
  // The name of the file we are reading.
  const char* filename_;
  // The position dependent options.
  Position_dependent_options posdep_options_;
  // Whether we are currently in a --start-group/--end-group.
  bool in_group_;
  // Whether the script was found in a sysrooted directory.
  bool is_in_sysroot_;
  // May be NULL if the user chooses not to pass one in.
  Command_line* command_line_;
  // Options which may be set from any linker script.
  Script_options* script_options_;
  // The lexer.
  Lex* lex_;
  // The line number of the last token returned by next_token.
  int lineno_;
  // The column number of the last token returned by next_token.
  int charpos_;
  // A stack of lexer modes.
  std::vector<Lex::Mode> lex_mode_stack_;
  // New input files found to add to the link.
  Input_arguments* inputs_;
};

// FILE was found as an argument on the command line.  Try to read it
// as a script.  We've already read BYTES of data into P, but we
// ignore that.  Return true if the file was handled.

bool
read_input_script(Workqueue* workqueue, const General_options& options,
		  Symbol_table* symtab, Layout* layout,
		  Dirsearch* dirsearch, Input_objects* input_objects,
		  Input_group* input_group,
		  const Input_argument* input_argument,
		  Input_file* input_file, const unsigned char*, off_t,
		  Task_token* this_blocker, Task_token* next_blocker)
{
  std::string input_string;
  Lex::read_file(input_file, &input_string);

  Lex lex(input_string.c_str(), input_string.length(), PARSING_LINKER_SCRIPT);

  Parser_closure closure(input_file->filename().c_str(),
			 input_argument->file().options(),
			 input_group != NULL,
			 input_file->is_in_sysroot(),
                         NULL,
			 layout->script_options(),
			 &lex);

  if (yyparse(&closure) != 0)
    return false;

  // THIS_BLOCKER must be clear before we may add anything to the
  // symbol table.  We are responsible for unblocking NEXT_BLOCKER
  // when we are done.  We are responsible for deleting THIS_BLOCKER
  // when it is unblocked.

  if (!closure.saw_inputs())
    {
      // The script did not add any files to read.  Note that we are
      // not permitted to call NEXT_BLOCKER->unblock() here even if
      // THIS_BLOCKER is NULL, as we do not hold the workqueue lock.
      workqueue->queue(new Script_unblock(this_blocker, next_blocker));
      return true;
    }

  for (Input_arguments::const_iterator p = closure.inputs()->begin();
       p != closure.inputs()->end();
       ++p)
    {
      Task_token* nb;
      if (p + 1 == closure.inputs()->end())
	nb = next_blocker;
      else
	{
	  nb = new Task_token(true);
	  nb->add_blocker();
	}
      workqueue->queue(new Read_symbols(options, input_objects, symtab,
					layout, dirsearch, &*p,
					input_group, this_blocker, nb));
      this_blocker = nb;
    }

  return true;
}

// FILENAME was found as an argument to --script (-T).
// Read it as a script, and execute its contents immediately.

bool
read_commandline_script(const char* filename, Command_line* cmdline)
{
  // TODO: if filename is a relative filename, search for it manually
  // using "." + cmdline->options()->search_path() -- not dirsearch.
  Dirsearch dirsearch;

  // The file locking code wants to record a Task, but we haven't
  // started the workqueue yet.  This is only for debugging purposes,
  // so we invent a fake value.
  const Task* task = reinterpret_cast<const Task*>(-1);

  Input_file_argument input_argument(filename, false, "",
				     cmdline->position_dependent_options());
  Input_file input_file(&input_argument);
  if (!input_file.open(cmdline->options(), dirsearch, task))
    return false;

  std::string input_string;
  Lex::read_file(&input_file, &input_string);

  Lex lex(input_string.c_str(), input_string.length(), PARSING_LINKER_SCRIPT);

  Parser_closure closure(filename,
			 cmdline->position_dependent_options(),
			 false,
			 input_file.is_in_sysroot(),
                         cmdline,
			 cmdline->script_options(),
			 &lex);
  if (yyparse(&closure) != 0)
    {
      input_file.file().unlock(task);
      return false;
    }

  input_file.file().unlock(task);

  gold_assert(!closure.saw_inputs());

  return true;
}

// Implement the --defsym option on the command line.  Return true if
// all is well.

bool
Script_options::define_symbol(const char* definition)
{
  Lex lex(definition, strlen(definition), PARSING_DEFSYM);
  lex.set_mode(Lex::EXPRESSION);

  // Dummy value.
  Position_dependent_options posdep_options;

  Parser_closure closure("command line", posdep_options, false, false, NULL,
			 this, &lex);

  if (yyparse(&closure) != 0)
    return false;

  gold_assert(!closure.saw_inputs());

  return true;
}

// Manage mapping from keywords to the codes expected by the bison
// parser.

class Keyword_to_parsecode
{
 public:
  // The structure which maps keywords to parsecodes.
  struct Keyword_parsecode
  {
    // Keyword.
    const char* keyword;
    // Corresponding parsecode.
    int parsecode;
  };

  // Return the parsecode corresponding KEYWORD, or 0 if it is not a
  // keyword.
  static int
  keyword_to_parsecode(const char* keyword, size_t len);

 private:
  // The array of all keywords.
  static const Keyword_parsecode keyword_parsecodes_[];

  // The number of keywords.
  static const int keyword_count;
};

// Mapping from keyword string to keyword parsecode.  This array must
// be kept in sorted order.  Parsecodes are looked up using bsearch.
// This array must correspond to the list of parsecodes in yyscript.y.

const Keyword_to_parsecode::Keyword_parsecode
Keyword_to_parsecode::keyword_parsecodes_[] =
{
  { "ABSOLUTE", ABSOLUTE },
  { "ADDR", ADDR },
  { "ALIGN", ALIGN_K },
  { "ALIGNOF", ALIGNOF },
  { "ASSERT", ASSERT_K },
  { "AS_NEEDED", AS_NEEDED },
  { "AT", AT },
  { "BIND", BIND },
  { "BLOCK", BLOCK },
  { "BYTE", BYTE },
  { "CONSTANT", CONSTANT },
  { "CONSTRUCTORS", CONSTRUCTORS },
  { "COPY", COPY },
  { "CREATE_OBJECT_SYMBOLS", CREATE_OBJECT_SYMBOLS },
  { "DATA_SEGMENT_ALIGN", DATA_SEGMENT_ALIGN },
  { "DATA_SEGMENT_END", DATA_SEGMENT_END },
  { "DATA_SEGMENT_RELRO_END", DATA_SEGMENT_RELRO_END },
  { "DEFINED", DEFINED },
  { "DSECT", DSECT },
  { "ENTRY", ENTRY },
  { "EXCLUDE_FILE", EXCLUDE_FILE },
  { "EXTERN", EXTERN },
  { "FILL", FILL },
  { "FLOAT", FLOAT },
  { "FORCE_COMMON_ALLOCATION", FORCE_COMMON_ALLOCATION },
  { "GROUP", GROUP },
  { "HLL", HLL },
  { "INCLUDE", INCLUDE },
  { "INFO", INFO },
  { "INHIBIT_COMMON_ALLOCATION", INHIBIT_COMMON_ALLOCATION },
  { "INPUT", INPUT },
  { "KEEP", KEEP },
  { "LENGTH", LENGTH },
  { "LOADADDR", LOADADDR },
  { "LONG", LONG },
  { "MAP", MAP },
  { "MAX", MAX_K },
  { "MEMORY", MEMORY },
  { "MIN", MIN_K },
  { "NEXT", NEXT },
  { "NOCROSSREFS", NOCROSSREFS },
  { "NOFLOAT", NOFLOAT },
  { "NOLOAD", NOLOAD },
  { "ONLY_IF_RO", ONLY_IF_RO },
  { "ONLY_IF_RW", ONLY_IF_RW },
  { "OPTION", OPTION },
  { "ORIGIN", ORIGIN },
  { "OUTPUT", OUTPUT },
  { "OUTPUT_ARCH", OUTPUT_ARCH },
  { "OUTPUT_FORMAT", OUTPUT_FORMAT },
  { "OVERLAY", OVERLAY },
  { "PHDRS", PHDRS },
  { "PROVIDE", PROVIDE },
  { "PROVIDE_HIDDEN", PROVIDE_HIDDEN },
  { "QUAD", QUAD },
  { "SEARCH_DIR", SEARCH_DIR },
  { "SECTIONS", SECTIONS },
  { "SEGMENT_START", SEGMENT_START },
  { "SHORT", SHORT },
  { "SIZEOF", SIZEOF },
  { "SIZEOF_HEADERS", SIZEOF_HEADERS },
  { "SORT_BY_ALIGNMENT", SORT_BY_ALIGNMENT },
  { "SORT_BY_NAME", SORT_BY_NAME },
  { "SPECIAL", SPECIAL },
  { "SQUAD", SQUAD },
  { "STARTUP", STARTUP },
  { "SUBALIGN", SUBALIGN },
  { "SYSLIB", SYSLIB },
  { "TARGET", TARGET_K },
  { "TRUNCATE", TRUNCATE },
  { "VERSION", VERSIONK },
  { "global", GLOBAL },
  { "l", LENGTH },
  { "len", LENGTH },
  { "local", LOCAL },
  { "o", ORIGIN },
  { "org", ORIGIN },
  { "sizeof_headers", SIZEOF_HEADERS },
};

const int Keyword_to_parsecode::keyword_count =
  (sizeof(Keyword_to_parsecode::keyword_parsecodes_)
   / sizeof(Keyword_to_parsecode::keyword_parsecodes_[0]));

// Comparison function passed to bsearch.

extern "C"
{

struct Ktt_key
{
  const char* str;
  size_t len;
};

static int
ktt_compare(const void* keyv, const void* kttv)
{
  const Ktt_key* key = static_cast<const Ktt_key*>(keyv);
  const Keyword_to_parsecode::Keyword_parsecode* ktt =
    static_cast<const Keyword_to_parsecode::Keyword_parsecode*>(kttv);
  int i = strncmp(key->str, ktt->keyword, key->len);
  if (i != 0)
    return i;
  if (ktt->keyword[key->len] != '\0')
    return -1;
  return 0;
}

} // End extern "C".

int
Keyword_to_parsecode::keyword_to_parsecode(const char* keyword, size_t len)
{
  Ktt_key key;
  key.str = keyword;
  key.len = len;
  void* kttv = bsearch(&key,
		       Keyword_to_parsecode::keyword_parsecodes_,
		       Keyword_to_parsecode::keyword_count,
		       sizeof(Keyword_to_parsecode::keyword_parsecodes_[0]),
		       ktt_compare);
  if (kttv == NULL)
    return 0;
  Keyword_parsecode* ktt = static_cast<Keyword_parsecode*>(kttv);
  return ktt->parsecode;
}

} // End namespace gold.

// The remaining functions are extern "C", so it's clearer to not put
// them in namespace gold.

using namespace gold;

// This function is called by the bison parser to return the next
// token.

extern "C" int
yylex(YYSTYPE* lvalp, void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  const Token* token = closure->next_token();
  switch (token->classification())
    {
    default:
      gold_unreachable();

    case Token::TOKEN_INVALID:
      yyerror(closurev, "invalid character");
      return 0;

    case Token::TOKEN_EOF:
      return 0;

    case Token::TOKEN_STRING:
      {
	// This is either a keyword or a STRING.
	size_t len;
	const char* str = token->string_value(&len);
	int parsecode = Keyword_to_parsecode::keyword_to_parsecode(str, len);
	if (parsecode != 0)
	  return parsecode;
	lvalp->string.value = str;
	lvalp->string.length = len;
	return STRING;
      }

    case Token::TOKEN_QUOTED_STRING:
      lvalp->string.value = token->string_value(&lvalp->string.length);
      return STRING;

    case Token::TOKEN_OPERATOR:
      return token->operator_value();

    case Token::TOKEN_INTEGER:
      lvalp->integer = token->integer_value();
      return INTEGER;
    }
}

// This function is called by the bison parser to report an error.

extern "C" void
yyerror(void* closurev, const char* message)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  gold_error(_("%s:%d:%d: %s"), closure->filename(), closure->lineno(),
	     closure->charpos(), message);
}

// Called by the bison parser to add a file to the link.

extern "C" void
script_add_file(void* closurev, const char* name, size_t length)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);

  // If this is an absolute path, and we found the script in the
  // sysroot, then we want to prepend the sysroot to the file name.
  // For example, this is how we handle a cross link to the x86_64
  // libc.so, which refers to /lib/libc.so.6.
  std::string name_string(name, length);
  const char* extra_search_path = ".";
  std::string script_directory;
  if (IS_ABSOLUTE_PATH(name_string.c_str()))
    {
      if (closure->is_in_sysroot())
	{
	  const std::string& sysroot(parameters->sysroot());
	  gold_assert(!sysroot.empty());
	  name_string = sysroot + name_string;
	}
    }
  else
    {
      // In addition to checking the normal library search path, we
      // also want to check in the script-directory.
      const char *slash = strrchr(closure->filename(), '/');
      if (slash != NULL)
	{
	  script_directory.assign(closure->filename(),
				  slash - closure->filename() + 1);
	  extra_search_path = script_directory.c_str();
	}
    }

  Input_file_argument file(name_string.c_str(), false, extra_search_path,
			   closure->position_dependent_options());
  closure->inputs()->add_file(file);
}

// Called by the bison parser to start a group.  If we are already in
// a group, that means that this script was invoked within a
// --start-group --end-group sequence on the command line, or that
// this script was found in a GROUP of another script.  In that case,
// we simply continue the existing group, rather than starting a new
// one.  It is possible to construct a case in which this will do
// something other than what would happen if we did a recursive group,
// but it's hard to imagine why the different behaviour would be
// useful for a real program.  Avoiding recursive groups is simpler
// and more efficient.

extern "C" void
script_start_group(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  if (!closure->in_group())
    closure->inputs()->start_group();
}

// Called by the bison parser at the end of a group.

extern "C" void
script_end_group(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  if (!closure->in_group())
    closure->inputs()->end_group();
}

// Called by the bison parser to start an AS_NEEDED list.

extern "C" void
script_start_as_needed(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  closure->position_dependent_options().set_as_needed();
}

// Called by the bison parser at the end of an AS_NEEDED list.

extern "C" void
script_end_as_needed(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  closure->position_dependent_options().clear_as_needed();
}

// Called by the bison parser to set the entry symbol.

extern "C" void
script_set_entry(void* closurev, const char* entry, size_t length)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  closure->script_options()->set_entry(entry, length);
}

// Called by the bison parser to define a symbol.

extern "C" void
script_set_symbol(void* closurev, const char* name, size_t length,
		  Expression* value, int providei, int hiddeni)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  const bool provide = providei != 0;
  const bool hidden = hiddeni != 0;
  closure->script_options()->add_symbol_assignment(name, length, value,
						   provide, hidden);
}

// Called by the bison parser to parse an OPTION.

extern "C" void
script_parse_option(void* closurev, const char* option, size_t length)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  // We treat the option as a single command-line option, even if
  // it has internal whitespace.
  if (closure->command_line() == NULL)
    {
      // There are some options that we could handle here--e.g.,
      // -lLIBRARY.  Should we bother?
      gold_warning(_("%s:%d:%d: ignoring command OPTION; OPTION is only valid"
		     " for scripts specified via -T/--script"),
		   closure->filename(), closure->lineno(), closure->charpos());
    }
  else
    {
      bool past_a_double_dash_option = false;
      char* mutable_option = strndup(option, length);
      gold_assert(mutable_option != NULL);
      closure->command_line()->process_one_option(1, &mutable_option, 0,
                                                  &past_a_double_dash_option);
      free(mutable_option);
    }
}

/* Called by the bison parser to push the lexer into expression
   mode.  */

extern void
script_push_lex_into_expression_mode(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  closure->push_lex_mode(Lex::EXPRESSION);
}

/* Called by the bison parser to pop the lexer mode.  */

extern void
script_pop_lex_mode(void* closurev)
{
  Parser_closure* closure = static_cast<Parser_closure*>(closurev);
  closure->pop_lex_mode();
}
