/* Definitions for expressions in GDB

   Copyright (C) 2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef EXPOP_H
#define EXPOP_H

#include "block.h"
#include "c-lang.h"
#include "cp-abi.h"
#include "expression.h"
#include "objfiles.h"
#include "gdbsupport/traits.h"
#include "gdbsupport/enum-flags.h"

struct agent_expr;
struct axs_value;

extern void gen_expr_binop (struct expression *exp,
			    enum exp_opcode op,
			    expr::operation *lhs, expr::operation *rhs,
			    struct agent_expr *ax, struct axs_value *value);
extern void gen_expr_structop (struct expression *exp,
			       enum exp_opcode op,
			       expr::operation *lhs,
			       const char *name,
			       struct agent_expr *ax, struct axs_value *value);

extern struct value *eval_op_scope (struct type *expect_type,
				    struct expression *exp,
				    enum noside noside,
				    struct type *type, const char *string);
extern struct value *eval_op_var_msym_value (struct type *expect_type,
					     struct expression *exp,
					     enum noside noside,
					     bool outermost_p,
					     minimal_symbol *msymbol,
					     struct objfile *objfile);
extern struct value *eval_op_var_entry_value (struct type *expect_type,
					      struct expression *exp,
					      enum noside noside, symbol *sym);
extern struct value *eval_op_func_static_var (struct type *expect_type,
					      struct expression *exp,
					      enum noside noside,
					      value *func, const char *var);
extern struct value *eval_op_register (struct type *expect_type,
				       struct expression *exp,
				       enum noside noside, const char *name);

namespace expr
{

/* The check_objfile overloads are used to check whether a particular
   component of some operation references an objfile.  The passed-in
   objfile will never be a debug objfile.  */

/* See if EXP_OBJFILE matches OBJFILE.  */
static inline bool
check_objfile (struct objfile *exp_objfile, struct objfile *objfile)
{
  if (exp_objfile->separate_debug_objfile_backlink)
    exp_objfile = exp_objfile->separate_debug_objfile_backlink;
  return exp_objfile == objfile;
}

static inline bool 
check_objfile (struct type *type, struct objfile *objfile)
{
  struct objfile *ty_objfile = type->objfile_owner ();
  if (ty_objfile != nullptr)
    return check_objfile (ty_objfile, objfile);
  return false;
}

static inline bool 
check_objfile (struct symbol *sym, struct objfile *objfile)
{
  return check_objfile (symbol_objfile (sym), objfile);
}

static inline bool 
check_objfile (const struct block *block, struct objfile *objfile)
{
  return check_objfile (block_objfile (block), objfile);
}

static inline bool
check_objfile (minimal_symbol *minsym, struct objfile *objfile)
{
  /* This may seem strange but minsyms are only used with an objfile
     as well.  */
  return false;
}

static inline bool
check_objfile (internalvar *ivar, struct objfile *objfile)
{
  return false;
}

static inline bool
check_objfile (const std::string &str, struct objfile *objfile)
{
  return false;
}

static inline bool 
check_objfile (const operation_up &op, struct objfile *objfile)
{
  return op->uses_objfile (objfile);
}

static inline bool
check_objfile (enum exp_opcode val, struct objfile *objfile)
{
  return false;
}

static inline bool
check_objfile (ULONGEST val, struct objfile *objfile)
{
  return false;
}

template<typename T>
static inline bool
check_objfile (enum_flags<T> val, struct objfile *objfile)
{
  return false;
}

template<typename T>
static inline bool 
check_objfile (const std::vector<T> &collection, struct objfile *objfile)
{
  for (const auto &item : collection)
    {
      if (check_objfile (item, objfile))
	return true;
    }
  return false;
}

template<typename S, typename T>
static inline bool 
check_objfile (const std::pair<S, T> &item, struct objfile *objfile)
{
  return (check_objfile (item.first, objfile)
	  || check_objfile (item.second, objfile));
}

static inline void
dump_for_expression (struct ui_file *stream, int depth,
		     const operation_up &op)
{
  op->dump (stream, depth);
}

extern void dump_for_expression (struct ui_file *stream, int depth,
				 enum exp_opcode op);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 const std::string &str);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 struct type *type);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 CORE_ADDR addr);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 internalvar *ivar);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 symbol *sym);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 minimal_symbol *msym);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 const block *bl);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 type_instance_flags flags);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 enum c_string_type_values flags);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 enum range_flag flags);
extern void dump_for_expression (struct ui_file *stream, int depth,
				 objfile *objf);

template<typename T>
void
dump_for_expression (struct ui_file *stream, int depth,
		     const std::vector<T> &vals)
{
  fprintf_filtered (stream, _("%*sVector:\n"), depth, "");
  for (auto &item : vals)
    dump_for_expression (stream, depth + 1, item);
}

template<typename X, typename Y>
void
dump_for_expression (struct ui_file *stream, int depth,
		     const std::pair<X, Y> &vals)
{
  dump_for_expression (stream, depth, vals.first);
  dump_for_expression (stream, depth, vals.second);
}

/* Base class for most concrete operations.  This class holds data,
   specified via template parameters, and supplies generic
   implementations of the 'dump' and 'uses_objfile' methods.  */
template<typename... Arg>
class tuple_holding_operation : public operation
{
public:

  explicit tuple_holding_operation (Arg... args)
    : m_storage (std::forward<Arg> (args)...)
  {
  }

  DISABLE_COPY_AND_ASSIGN (tuple_holding_operation);

  bool uses_objfile (struct objfile *objfile) const override
  {
    return do_check_objfile<0, Arg...> (objfile, m_storage);
  }

  void dump (struct ui_file *stream, int depth) const override
  {
    dump_for_expression (stream, depth, opcode ());
    do_dump<0, Arg...> (stream, depth + 1, m_storage);
  }

protected:

  /* Storage for the data.  */
  std::tuple<Arg...> m_storage;

private:

  /* do_dump does the work of dumping the data.  */
  template<int I, typename... T>
  typename std::enable_if<I == sizeof... (T), void>::type
  do_dump (struct ui_file *stream, int depth, const std::tuple<T...> &value)
    const
  {
  }

  template<int I, typename... T>
  typename std::enable_if<I < sizeof... (T), void>::type
  do_dump (struct ui_file *stream, int depth, const std::tuple<T...> &value)
    const
  {
    dump_for_expression (stream, depth, std::get<I> (value));
    do_dump<I + 1, T...> (stream, depth, value);
  }

  /* do_check_objfile does the work of checking whether this object
     refers to OBJFILE.  */
  template<int I, typename... T>
  typename std::enable_if<I == sizeof... (T), bool>::type
  do_check_objfile (struct objfile *objfile, const std::tuple<T...> &value)
    const
  {
    return false;
  }

  template<int I, typename... T>
  typename std::enable_if<I < sizeof... (T), bool>::type
  do_check_objfile (struct objfile *objfile, const std::tuple<T...> &value)
    const
  {
    if (check_objfile (std::get<I> (value), objfile))
      return true;
    return do_check_objfile<I + 1, T...> (objfile, value);
  }
};

/* The check_constant overloads are used to decide whether a given
   concrete operation is a constant.  This is done by checking the
   operands.  */

static inline bool
check_constant (const operation_up &item)
{
  return item->constant_p ();
}

static inline bool
check_constant (struct minimal_symbol *msym)
{
  return false;
}

static inline bool
check_constant (struct type *type)
{
  return true;
}

static inline bool
check_constant (const struct block *block)
{
  return true;
}

static inline bool
check_constant (const std::string &str)
{
  return true;
}

static inline bool
check_constant (struct objfile *objfile)
{
  return true;
}

static inline bool
check_constant (ULONGEST cst)
{
  return true;
}

static inline bool
check_constant (struct symbol *sym)
{
  enum address_class sc = SYMBOL_CLASS (sym);
  return (sc == LOC_BLOCK
	  || sc == LOC_CONST
	  || sc == LOC_CONST_BYTES
	  || sc == LOC_LABEL);
}

template<typename T>
static inline bool
check_constant (const std::vector<T> &collection)
{
  for (const auto &item : collection)
    if (!check_constant (item))
      return false;
  return true;
}

template<typename S, typename T>
static inline bool
check_constant (const std::pair<S, T> &item)
{
  return check_constant (item.first) && check_constant (item.second);
}

/* Base class for concrete operations.  This class supplies an
   implementation of 'constant_p' that works by checking the
   operands.  */
template<typename... Arg>
class maybe_constant_operation
  : public tuple_holding_operation<Arg...>
{
public:

  using tuple_holding_operation<Arg...>::tuple_holding_operation;

  bool constant_p () const override
  {
    return do_check_constant<0, Arg...> (this->m_storage);
  }

private:

  template<int I, typename... T>
  typename std::enable_if<I == sizeof... (T), bool>::type
  do_check_constant (const std::tuple<T...> &value) const
  {
    return true;
  }

  template<int I, typename... T>
  typename std::enable_if<I < sizeof... (T), bool>::type
  do_check_constant (const std::tuple<T...> &value) const
  {
    if (!check_constant (std::get<I> (value)))
      return false;
    return do_check_constant<I + 1, T...> (value);
  }
};

/* A floating-point constant.  The constant is encoded in the target
   format.  */

typedef std::array<gdb_byte, 16> float_data;

/* An operation that holds a floating-point constant of a given
   type.

   This does not need the facilities provided by
   tuple_holding_operation, so it does not use it.  */
class float_const_operation
  : public operation
{
public:

  float_const_operation (struct type *type, float_data data)
    : m_type (type),
      m_data (data)
  {
  }

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return value_from_contents (m_type, m_data.data ());
  }

  enum exp_opcode opcode () const override
  { return OP_FLOAT; }

  bool constant_p () const override
  { return true; }

  void dump (struct ui_file *stream, int depth) const override;

private:

  struct type *m_type;
  float_data m_data;
};

class scope_operation
  : public maybe_constant_operation<struct type *, std::string>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return eval_op_scope (expect_type, exp, noside,
			  std::get<0> (m_storage),
			  std::get<1> (m_storage).c_str ());
  }

  value *evaluate_for_address (struct expression *exp,
			       enum noside noside) override;

  enum exp_opcode opcode () const override
  { return OP_SCOPE; }

protected:

  void do_generate_ax (struct expression *exp,
		       struct agent_expr *ax,
		       struct axs_value *value,
		       struct type *cast_type)
    override;
};

class long_const_operation
  : public tuple_holding_operation<struct type *, LONGEST>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return value_from_longest (std::get<0> (m_storage),
			       std::get<1> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return OP_LONG; }

  bool constant_p () const override
  { return true; }

protected:

  void do_generate_ax (struct expression *exp,
		       struct agent_expr *ax,
		       struct axs_value *value,
		       struct type *cast_type)
    override;
};

class var_msym_value_operation
  : public maybe_constant_operation<minimal_symbol *, struct objfile *>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return eval_op_var_msym_value (expect_type, exp, noside, m_outermost,
				   std::get<0> (m_storage),
				   std::get<1> (m_storage));
  }

  value *evaluate_for_sizeof (struct expression *exp, enum noside noside)
    override;

  value *evaluate_for_address (struct expression *exp, enum noside noside)
    override;

  value *evaluate_for_cast (struct type *expect_type,
			    struct expression *exp,
			    enum noside noside) override;

  enum exp_opcode opcode () const override
  { return OP_VAR_MSYM_VALUE; }

  void set_outermost () override
  {
    m_outermost = true;
  }

protected:

  /* True if this is the outermost operation in the expression.  */
  bool m_outermost = false;

  void do_generate_ax (struct expression *exp,
		       struct agent_expr *ax,
		       struct axs_value *value,
		       struct type *cast_type)
    override;
};

class var_entry_value_operation
  : public tuple_holding_operation<symbol *>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return eval_op_var_entry_value (expect_type, exp, noside,
				    std::get<0> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return OP_VAR_ENTRY_VALUE; }
};

class func_static_var_operation
  : public maybe_constant_operation<operation_up, std::string>
{
public:

  using maybe_constant_operation::maybe_constant_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    value *func = std::get<0> (m_storage)->evaluate (nullptr, exp, noside);
    return eval_op_func_static_var (expect_type, exp, noside, func,
				    std::get<1> (m_storage).c_str ());
  }

  enum exp_opcode opcode () const override
  { return OP_FUNC_STATIC_VAR; }
};

class last_operation
  : public tuple_holding_operation<int>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return access_value_history (std::get<0> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return OP_LAST; }
};

class register_operation
  : public tuple_holding_operation<std::string>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    return eval_op_register (expect_type, exp, noside,
			     std::get<0> (m_storage).c_str ());
  }

  enum exp_opcode opcode () const override
  { return OP_REGISTER; }

protected:

  void do_generate_ax (struct expression *exp,
		       struct agent_expr *ax,
		       struct axs_value *value,
		       struct type *cast_type)
    override;
};

class bool_operation
  : public tuple_holding_operation<bool>
{
public:

  using tuple_holding_operation::tuple_holding_operation;

  value *evaluate (struct type *expect_type,
		   struct expression *exp,
		   enum noside noside) override
  {
    struct type *type = language_bool_type (exp->language_defn, exp->gdbarch);
    return value_from_longest (type, std::get<0> (m_storage));
  }

  enum exp_opcode opcode () const override
  { return OP_BOOL; }

  bool constant_p () const override
  { return true; }
};

} /* namespace expr */

#endif /* EXPOP_H */
