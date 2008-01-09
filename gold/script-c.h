/* script-c.h -- C interface for linker scripts in gold.  */

/* Copyright 2006, 2007, 2008 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <iant@google.com>.

   This file is part of gold.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* This file exists so that both the bison parser and script.cc can
   include it, so that they can communicate back and forth.  */

#ifndef GOLD_SCRIPT_C_H
#define GOLD_SCRIPT_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* A string value for the bison parser.  */

struct Parser_string
{
  const char* value;
  size_t length;
};

/* The expression functions deal with pointers to Expression objects.
   Since the bison parser generates C code, this is a hack to keep the
   C++ code type safe.  This hacks assumes that all pointers look
   alike.  */

#ifdef __cplusplus
namespace gold
{
class Expression;
}
typedef gold::Expression* Expression_ptr;
#else
typedef void* Expression_ptr;
#endif

/* The bison parser definitions.  */

#include "yyscript.h"

/* The bison parser function.  */

extern int
yyparse(void* closure);

/* Called by the bison parser skeleton to return the next token.  */

extern int
yylex(YYSTYPE*, void* closure);

/* Called by the bison parser skeleton to report an error.  */

extern void
yyerror(void* closure, const char*);

/* Called by the bison parser to add a file to the link.  */

extern void
script_add_file(void* closure, const char*, size_t);

/* Called by the bison parser to start and stop a group.  */

extern void
script_start_group(void* closure);
extern void
script_end_group(void* closure);

/* Called by the bison parser to start and end an AS_NEEDED list.  */

extern void
script_start_as_needed(void* closure);
extern void
script_end_as_needed(void* closure);

/* Called by the bison parser to set the entry symbol.  */

extern void
script_set_entry(void* closure, const char*, size_t);

/* Called by the bison parser to parse an OPTION.  */

extern void
script_parse_option(void* closure, const char*, size_t);

/* Called by the bison parser to push the lexer into expression
   mode.  */

extern void
script_push_lex_into_expression_mode(void* closure);

/* Called by the bison parser to pop the lexer mode.  */

extern void
script_pop_lex_mode(void* closure);

/* Called by the bison parser to set a symbol to a value.  PROVIDE is
   non-zero if the symbol should be provided--only defined if there is
   an undefined reference.  HIDDEN is non-zero if the symbol should be
   hidden.  */

extern void
script_set_symbol(void* closure, const char*, size_t, Expression_ptr,
		  int provide, int hidden);

/* Called by the bison parser for expressions.  */

extern Expression_ptr
script_exp_unary_minus(Expression_ptr);
extern Expression_ptr
script_exp_unary_logical_not(Expression_ptr);
extern Expression_ptr
script_exp_unary_bitwise_not(Expression_ptr);
extern Expression_ptr
script_exp_binary_mult(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_div(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_mod(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_add(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_sub(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_lshift(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_rshift(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_eq(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_ne(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_le(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_ge(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_lt(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_gt(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_bitwise_and(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_bitwise_xor(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_bitwise_or(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_logical_and(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_binary_logical_or(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_trinary_cond(Expression_ptr, Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_integer(uint64_t);
extern Expression_ptr
script_exp_string(const char*, size_t);
extern Expression_ptr
script_exp_function_max(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_function_min(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_function_defined(const char*, size_t);
extern Expression_ptr
script_exp_function_sizeof_headers();
extern Expression_ptr
script_exp_function_alignof(const char*, size_t);
extern Expression_ptr
script_exp_function_sizeof(const char*, size_t);
extern Expression_ptr
script_exp_function_addr(const char*, size_t);
extern Expression_ptr
script_exp_function_loadaddr(const char*, size_t);
extern Expression_ptr
script_exp_function_origin(const char*, size_t);
extern Expression_ptr
script_exp_function_length(const char*, size_t);
extern Expression_ptr
script_exp_function_constant(const char*, size_t);
extern Expression_ptr
script_exp_function_absolute(Expression_ptr);
extern Expression_ptr
script_exp_function_align(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_function_data_segment_align(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_function_data_segment_relro_end(Expression_ptr, Expression_ptr);
extern Expression_ptr
script_exp_function_data_segment_end(Expression_ptr);
extern Expression_ptr
script_exp_function_segment_start(const char*, size_t, Expression_ptr);
extern Expression_ptr
script_exp_function_assert(Expression_ptr, const char*, size_t);

#ifdef __cplusplus
}
#endif

#endif /* !defined(GOLD_SCRIPT_C_H) */
