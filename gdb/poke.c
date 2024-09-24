/* GDB integration with GNU poke.

   Copyright (C) 2021-2024 Free Software Foundation, Inc.

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

#include "arch-utils.h"
#include "command.h"
#include "defs.h"
#include "cli/cli-cmds.h"
#include "gdbtypes.h"
#include "target.h"
#include "value.h"
#include "symtab.h"
#include "event-top.h"
#include "gdbsupport/gdb_unique_ptr.h"

extern "C" {
#include <libpoke.h>
}

#include <algorithm>
#include <ctype.h>
#include <vector>
#include <type_traits>

/* Global poke incremental compiler.  */

// TODO offset<...,gdbarch_addressable_memory_unit_size>
// TODO poke_add_var_command
// TODO Remove poke-* commands and introduce sub-commands.
// TODO Support more than one inferior.
// TODO Smarter holes.

static pk_compiler poke_compiler ();

struct pk_compiler_deleter;

using pk_compiler_up
    = std::unique_ptr<std::remove_pointer_t<pk_compiler>, pk_compiler_deleter>;

struct pk_compiler_deleter
{
  void
  operator() (pk_compiler pkc)
  {
    pk_val val, exit_exception;

    // FIXME Move this to gdb.pk and there go through `iolist' and close
    // them.
    if (pk_compile_statement (poke_compiler (),
			      "try close (get_ios); catch if E_no_ios {}",
			      nullptr, &val, &exit_exception)
	    != PK_OK
	|| exit_exception != PK_NULL)
      error (_("Error while closing an IOS on exit."));

    pk_compiler_free (pkc);
  }
};

/* Global vector of the Poke code used to define types.  This is
   filled in by poke_add_type and used by poke_dump_types.  */

static std::vector<std::string> type_poke_strings;

/* Terminal hook that flushes the terminal.  */

static void
poke_term_flush (pk_compiler pkc ATTRIBUTE_UNUSED)
{
  /* Do nothing here.  */
}

/* Terminal hook that prints a fixed string.  */

static void
poke_puts_1 (pk_compiler pkc ATTRIBUTE_UNUSED, const char *str)
{
  gdb_printf ("%s", str);
}
#define poke_puts(STR) poke_puts_1 (poke_compiler (), (STR))

/* Terminal hook that prints a formatted string.  */

ATTRIBUTE_PRINTF (2, 3)
static void
poke_printf (pk_compiler pkc ATTRIBUTE_UNUSED, const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  gdb_vprintf (format, ap);
  va_end (ap);
}

/* Terminal hook that indents to a given level.  */

static void
poke_term_indent (pk_compiler pkc ATTRIBUTE_UNUSED, unsigned int lvl,
		  unsigned int step)
{
  gdb_printf ("\n%*s", (step * lvl), "");
}

/* Terminal hook that starts a styling class.  */

static void
poke_term_class (pk_compiler pkc ATTRIBUTE_UNUSED, const char *class_name)
{
  /* Do nothing here.  */
}

/* Terminal hook that finishes a styling class.  */

static int
poke_term_end_class (pk_compiler pkc ATTRIBUTE_UNUSED, const char *class_name)
{
  /* Just report success.  */
  return 1;
}

/* Terminal hook that starts a terminal hyperlink.  */

static void
poke_term_hyperlink (pk_compiler pkc ATTRIBUTE_UNUSED, const char *url,
		     const char *id)
{
  /* Do nothing here.  */
}

/* Terminal hook that finishes a terminal hyperlink.  */

static int
poke_term_end_hyperlink (pk_compiler pkc ATTRIBUTE_UNUSED)
{
  /* Just report success.  */
  return 1;
}

/* Terminal hook that returns the current terminal foreground
   color.  */

static struct pk_color
poke_term_get_color (pk_compiler pkc ATTRIBUTE_UNUSED)
{
  /* Just return the default foreground color.	*/
  struct pk_color dfl = { -1, -1, -1 };
  return dfl;
}

/* Terminal hook that returns the current terminal background
   color.  */

static struct pk_color
poke_term_get_bgcolor (pk_compiler pkc ATTRIBUTE_UNUSED)
{
  /* Just return the default background color.	*/
  struct pk_color dfl = { -1, -1, -1 };
  return dfl;
}

/* Terminal hook that sets the terminal foreground color.  */

static void
poke_term_set_color (pk_compiler pkc ATTRIBUTE_UNUSED, struct pk_color color)
{
  /* Do nothing.  */
}

/* Terminal hook that sets the terminal background color.  */

static void
poke_term_set_bgcolor (pk_compiler pkc ATTRIBUTE_UNUSED, struct pk_color color)
{
  /* Do nothing.  */
}

/* Terminal interface for poke.  */

static struct pk_term_if poke_term_if;

/* Foreign IO device hook that returns an unique name identifying the
   kind of device.  */

static const char *
iod_get_if_name ()
{
  return "GDB";
}

/* Foreign IO device hook that recognizes whether a given IO space
   handler refer to this kind of device, and normalizes it for further
   use.  */

static char *
iod_handler_normalize (const char *handler, uint64_t flags, int *error)
{
  char *new_handler = nullptr;

  // FIXME Extend to more inferior.
  if (strcmp (handler, "gdb://inferior/mem") == 0)
    new_handler = xstrdup (handler);
  if (error)
    *error = PK_IOD_OK;

  return new_handler;
}

/* Foreign IO device hook that opens a new device.  */

static int iod_opened_p = 0;

static void *
iod_open (const char *handler, uint64_t flags, int *error, void *data)
{
  iod_opened_p = 1;
  return &iod_opened_p;
}

/* Foreign IO device hook that reads data from a device.  */

static int
iod_pread (void *dev, void *buf, size_t count, pk_iod_off offset)
{
  int ret = target_read_memory (offset, (gdb_byte *)buf, count);
  return ret != 0 ? PK_IOD_ERROR : PK_IOD_OK;
}

/* Foreign IO device hook that writes data to a device.  */

static int
iod_pwrite (void *dev, const void *buf, size_t count, pk_iod_off offset)
{
  int ret = target_write_memory (offset, (gdb_byte *)buf, count);
  return ret == -1 ? PK_IOD_ERROR : PK_IOD_OK;
}

/* Foreign IO device hook that returns the flags of an IO device. */

static uint64_t
iod_get_flags (void *dev)
{
  return PK_IOS_F_READ | PK_IOS_F_WRITE;
}

/* Foreign IO device hook that returns the size of an IO device, in
   bytes.  */

static pk_iod_off
iod_size (void *dev)
{
  return gdbarch_addr_bit (get_current_arch ()) == 32 ? 0xffffffff
						      : 0xffffffffffffffff;
}

/* Foreign IO device hook that flushes an IO device.  */

static int
iod_flush (void *dev, pk_iod_off offset)
{
  /* Do nothing here.  */
  return PK_OK;
}

/* Foreign IO device hook that closes a given device.  */

static int
iod_close (void *dev)
{
  iod_opened_p = 0;
  return PK_OK;
}

/* Implementation of the poke foreign IO device interface, that uses
   the hooks defined above.  */

static struct pk_iod_if iod_if
    = { iod_get_if_name, iod_handler_normalize, iod_open, iod_close, iod_pread,
	iod_pwrite,	 iod_get_flags,		iod_size, iod_flush };

/* Handler for alien tokens.  */

static struct pk_alien_token alien_token;

static struct pk_alien_token *poke_alien_token_handler (std::string_view expr,
							char **errmsg);

static struct pk_alien_token *
poke_alien_delimited_token_handler (char delimiter, const char *id,
				    char **errmsg);

static struct pk_alien_token *poke_alien_simple_token_handler (const char *id,
							       char **errmsg);

static struct pk_alien_token *
poke_alien_simple_token_handler (const char *id, char **errmsg)
{
  std::string expr{ id };

  if (startswith (id, "addr::"))
    expr = "&" + expr.substr (6);
  return poke_alien_token_handler (expr, errmsg);
}

static struct pk_alien_token *
poke_alien_delimited_token_handler (char delimiter, const char *id,
				    char **errmsg)
{
  std::string expr{ id + 1, strlen (id) - 2 }; /* Remove '<' & '>'.  */

  return poke_alien_token_handler (expr, errmsg);
}

static struct pk_alien_token *
poke_alien_token_handler (std::string_view expr, char **errmsg)
{
  /* In GDB delimited alien poke tokens with the form $<&FOO> or $addr::FOO
     provide the address of the symbol `FOO' as an offset in bytes, i.e. it
     resolves to the GDB value &FOO as a Poke offset with unit bytes
     and a magnitude whose width is the number of bits conforming an
     address in the target architecture.

     $<FOO> or $FOO, on the other hand, provides the value of the symbol FOO
     incarnated in a proper Poke value, provided that FOO is of a type
     that this handler knows how to handle.  Otherwise the string is
     not recognized as a token.  */

  // gdb_printf ("DEBUG--- %s\n", expr.data ());

  if (startswith (expr, "&"))
    {
      CORE_ADDR addr;
      int addr_bit;
      int unit_size;
      gdbarch *arch;

      try
	{
	  struct value *value;

	  expr = expr.substr (1);
	  value = parse_and_eval (expr.data ());
	  addr = value->address ();
	}
      catch (const gdb_exception_error &except)
	{
	  goto error;
	}

      arch = get_current_arch ();
      addr_bit = gdbarch_addr_bit (arch);
      alien_token.kind = PK_ALIEN_TOKEN_OFFSET;
      alien_token.value.offset.magnitude = addr;
      gdb_assert (addr_bit <= 64);
      alien_token.value.offset.width = addr_bit;
      alien_token.value.offset.signed_p = 0;
      unit_size = gdbarch_addressable_memory_unit_size (arch);
      alien_token.value.offset.unit = unit_size * 8;
    }
  else if (startswith (expr, "param:"))
    {
      /* Read the param.  */
      goto error;
    }
  else
    {
      struct value *value;

      // gdb_printf ("DEBUG ---parse_and_eval %s\n", expr.data ());

      try
	{
	  value = parse_and_eval (expr.data ());
	  // gdb_printf ("DEBUG ---parse_and_eval %s worked\n", expr.data ());
	}
      catch (const gdb_exception_error &except)
	{
	  goto error;
	}

      struct type *type = value->type ();

      if (can_dereference (type))
	{
	  alien_token.kind = PK_ALIEN_TOKEN_OFFSET;
	  alien_token.value.offset.magnitude = value_as_address (value);
	  alien_token.value.offset.width = type->length () * 8;
	  alien_token.value.offset.signed_p = !type->is_unsigned ();
	  alien_token.value.offset.unit = 8;
	}
      else if (is_integral_type (type))
	{
	  alien_token.kind = PK_ALIEN_TOKEN_INTEGER;
	  alien_token.value.integer.magnitude = value_as_long (value);
	  alien_token.value.integer.width = type->length () * 8;
	  alien_token.value.integer.signed_p = !type->is_unsigned ();
	}
      else if (is_floating_type (type))
	{
	  alien_token.kind = PK_ALIEN_TOKEN_INTEGER;
	  alien_token.value.integer.magnitude = value_as_long (value);
	  alien_token.value.integer.width = type->length () * 8;
	  alien_token.value.integer.signed_p = 0;
	}
      else
	goto error;
    }

  *errmsg = nullptr;
  return &alien_token;

error:
  *errmsg = concat ("can't access GDB variable '", expr.data (), "'", nullptr);
  return nullptr;
}

/* Given a string, prefix it in order to avoid collision with Poke's
   keywords.  */

static std::string
normalize_poke_identifier (std::string prefix, std::string str)
{
  if (pk_keyword_p (poke_compiler (), str.c_str ()))
    str = prefix + str;

  return str;
}

/* Given a GDB type name, mangle it to a valid Poke type name.	*/

static std::string
gdb_type_name_to_poke (std::string str, struct type *type = nullptr)
{
  for (auto &ch : str)
    if (!isalnum (ch))
      ch = '_';

  if (type != nullptr)
    {
      /* Prepend struct and union tags with suitable prefixes.	This
	 is to avoid ending with recursive typedefs in C programs.  */
      if (type->code () == TYPE_CODE_STRUCT)
	str = "struct_" + str;
      else if (type->code () == TYPE_CODE_UNION)
	str = "union_" + str;
    }

  return str;
}

/* Given a GDB symbol name, mangle it to a valid Poke variable name.  */

static std::string
gdb_sym_name_to_poke (std::string str)
{
  for (auto &ch : str)
    if (!isalnum (ch))
      ch = '_';
  return str;
}

static void poke_command (const char *args, int from_tty);

/* Command to feed the poke compiler with the definition of some given
   GDB type.  */

static std::string
poke_add_type (struct type *type, std::string *poke_type_name = nullptr)
{
  std::string type_name;
  std::string str;
  auto add_hole = [&] (size_t nbits) {
    auto nbytes{ nbits / 8 };
    auto nbits_rem{ nbits & 7 };

    str += "uint<8>[" + std::to_string (nbytes) + "]; ";
    if (nbits_rem != 0)
      str += " uint<" + std::to_string (nbits_rem) + ">; ";
  };

  if (type != nullptr)
    {
      if (type->name () != nullptr)
	type_name = type->name ();

      /* Do not try to add a type that is already defined.  */
      if (!type_name.empty ()
	  && pk_decl_p (poke_compiler (),
			gdb_type_name_to_poke (type_name, type).c_str (),
			PK_DECL_KIND_TYPE))
	return type_name;

      switch (type->code ())
	{
	case TYPE_CODE_PTR:
	  {
	    str = ("offset<uint<" + (std::to_string (type->length () * 8))
		   + ">,B>");
	    break;
	  }
	case TYPE_CODE_TYPEDEF:
	  {
	    struct type *target_type = type->target_type ();
	    std::string target_type_code = poke_add_type (target_type);

	    if (target_type_code.empty ())
	      goto skip;

	    if (target_type->name () != nullptr)
	      str += gdb_type_name_to_poke (target_type->name (), target_type);
	    else
	      str += target_type_code;
	    break;
	  }
	/* Poke doesn't have first-class floating-point numbers, so encode
	   them as INT.  */
	case TYPE_CODE_FLT:
	case TYPE_CODE_INT:
	case TYPE_CODE_ENUM:
	  {
	    size_t type_length = type->length ();

	    if (type_length <= 8)
	      {
		if (type->code () == TYPE_CODE_FLT || type->is_unsigned ())
		  str += "u";
		str += "int<";
		str += std::to_string (type_length * 8);
		str += ">";
	      }
	    else
	      /* Add as an opaque array of bytes.  */
	      str += "uint<8>[" + std::to_string (type_length) + "]";
	    break;
	  }
	case TYPE_CODE_ARRAY:
	  {
	    struct type *target_type = type->target_type ();
	    size_t target_type_length = target_type->length ();
	    std::string target_type_code = poke_add_type (target_type);

	    if (target_type_code.empty ())
	      goto skip;

	    // FIXME
	    /* Poke doesn't have multi-dimensional arrays.  */
	    if (type->is_multi_dimensional ())
	      add_hole (type->length () * 8);

	    if (target_type->name () != nullptr)
	      str = gdb_type_name_to_poke (target_type->name (), target_type);
	    else
	      str = target_type_code;

	    str += "[";
	    str += std::to_string (type->length () / target_type_length);
	    str += "]";
	    break;
	  }
	case TYPE_CODE_STRUCT:
	case TYPE_CODE_UNION:
	  {
	    size_t current_bitpos = 0;

	    if (type->code () == TYPE_CODE_UNION)
	      str += "pinned ";
	    str += "struct {";

	    // gdb_printf ("DEBUG type_name:%s target_type:%p\n", type->name
	    // (), type->target_type ());

	    for (int idx = 0; idx < type->num_fields (); idx++)
	      {
		// TODO Skip static fields.
		// TODO C++ types?

		std::string field_name = normalize_poke_identifier (
		    "__f", type->field (idx).name ());
		struct type *field_type = type->field (idx).type ();
		size_t field_bitpos = type->field (idx).loc_bitpos ();
		size_t field_bitsize = type->field (idx).bitsize ();

		// gdb_printf ("DEBUG bitpos:%d bitsize:%d\n", field_bitpos,
		// field_bitsize);

		if (idx > 0)
		  str += " ";

		if (current_bitpos != field_bitpos)
		  {
		    add_hole (field_bitpos - current_bitpos);
		    current_bitpos = field_bitpos;
		  }

		if (field_bitsize != 0)
		  {
		    /* This field is a bit-field.  */

		    str += "uint<" + std::to_string (field_bitsize) + "> ";
		    current_bitpos += field_bitsize;
		  }
		else
		  {
		    /* This is a normal field.	*/

		    if (field_type->name () != nullptr)
		      {
			if (poke_add_type (field_type).empty ())
			  add_hole (field_type->length () * 8);
			else
			  str += gdb_type_name_to_poke (field_type->name (),
							field_type);
		      }
		    else
		      {
			std::string pstr = poke_add_type (field_type);
			if (pstr.empty ())
			  add_hole (field_type->length () * 8);
			str += pstr;
		      }
		    str += " ";

		    current_bitpos += field_type->length () * 8;
		  }
		if (!field_name.empty ())
		  str += field_name;
		str += ";";
	      }

	    str += "}";
	    break;
	  }
	default:
	  add_hole (type->length () * 8);
	  break;
	}

      if (!type_name.empty ())
	{
	  std::string ptype_name = gdb_type_name_to_poke (type_name, type);

	  std::string deftype = "type ";
	  deftype += ptype_name;
	  deftype += " = ";
	  deftype += str;

	  type_poke_strings.push_back (deftype);
	  poke_command (deftype.c_str (), 0 /* from_tty */);
	  if (poke_type_name != nullptr)
	    *poke_type_name = ptype_name;
	  gdb_printf ("added type %s = %s\n", ptype_name.c_str (),
		      str.c_str ());
	}
      else
	{
	  gdb_printf ("skipped type %s\n", str.c_str ());
	}
    }

  return str;

skip:
  if (!type_name.empty ())
    gdb_printf ("skipped type %s\n", type_name.c_str ());
  return {};
}

/* Call the default poke exception handler.  */

static void
poke_handle_exception (pk_val exception)
{
  pk_val handler = pk_decl_val (poke_compiler (), "gdb_exception_handler");

  if (handler == PK_NULL)
    error (_("Couldn't get a handler for poke gdb_exception_handler"));
  if (pk_call (poke_compiler (), handler, nullptr, nullptr, 1, exception)
      == PK_ERROR)
    error (_("Couldn't call gdb_exception_handler in poke"));
}

/* Start the poke incremental compiler.  */

static pk_compiler
poke_compiler ()
{
  static pk_compiler_up pkc;

  if (pkc == nullptr)
    {
      pk_val poke_exception;

      /* Implementation of the poke terminal interface, that uses the hooks
	 defined above.  */
      poke_term_if.flush_fn = poke_term_flush;
      poke_term_if.puts_fn = poke_puts_1;
      poke_term_if.printf_fn = poke_printf;
      poke_term_if.indent_fn = poke_term_indent;
      poke_term_if.class_fn = poke_term_class;
      poke_term_if.end_class_fn = poke_term_end_class;
      poke_term_if.hyperlink_fn = poke_term_hyperlink;
      poke_term_if.end_hyperlink_fn = poke_term_end_hyperlink;
      poke_term_if.get_color_fn = poke_term_get_color;
      poke_term_if.get_bgcolor_fn = poke_term_get_bgcolor;
      poke_term_if.set_color_fn = poke_term_set_color;
      poke_term_if.set_bgcolor_fn = poke_term_set_bgcolor;

      /* Note how we are creating an incremental compiler without the
	 standard Poke types (int, etc) because they collide with the C
	 types.  */
      pkc.reset (pk_compiler_new_with_flags (&poke_term_if, PK_F_NOSTDTYPES));
      if (pkc == nullptr)
	error (_("Couldn't start the poke incremental compiler."));

      /* Install the handler for alien tokens that recognizes GDB
	 symbols.  */
      pk_set_alien_token_fn (pkc.get (), poke_alien_simple_token_handler);
      pk_set_alien_dtoken_fn (pkc.get (), poke_alien_delimited_token_handler);

      /* Use hexadecimal output by default.  */
      pk_set_obase (pkc.get (), 16);

      /* Use `tree' printing mode by default.  */
      pk_set_omode (pkc.get (), PK_PRINT_TREE);

      /* Set the default endianness.  */
      switch (gdbarch_byte_order (get_current_arch ()))
	{
	case BFD_ENDIAN_BIG:
	  pk_set_endian (pkc.get (), PK_ENDIAN_MSB);
	  break;
	case BFD_ENDIAN_LITTLE:
	  pk_set_endian (pkc.get (), PK_ENDIAN_LSB);
	  break;
	default:
	  break;
	}

      /* Install our foreign IO device interface to access the target's
	 memory.  */
      if (pk_register_iod (pkc.get (), &iod_if) != PK_OK)
	error (
	    _("Could not register the foreign IO device interface in poke."));

      /* Provide access to pickles installed by poke applications, also to
	 the pickles installed by GDB.	*/
      pk_val pk_load_path = pk_decl_val (poke_compiler (), "load_path");
      std::string load_path = pk_string_str (pk_load_path);
      load_path += ":" + gdb_datadir + "/poke:%DATADIR%/pickles";
      pk_decl_set_val (pkc.get (), "load_path",
		       pk_make_string (poke_compiler (), load_path.c_str ()));

      /* Load the Poke components.  */
      if (pk_load (pkc.get (), "gdb", &poke_exception) != PK_OK)
	error (_("Could not load gdb.pk"));
      if (poke_exception != PK_NULL)
	{
	  pk_val exc;
	  pk_print_val (pkc.get (), poke_exception, &exc);
	  error (_("Poke exception happened during loading gdb.pk"));
	}
    }

  return pkc.get ();
}

/* Command to dump the Poke definition of known types.	*/

static void
poke_dump_types (const char *args, int from_tty)
{
  for (const std::string &s : type_poke_strings)
    gdb_printf ("%s;\n", s.c_str ());
}

/* Commands to add GDB types to the running poke compiler.  */

static void
poke_add_type_command (const char *args, int from_tty)
{
  const char *sans_space_args{ skip_spaces (args) };

  if (sans_space_args == nullptr)
    return;

  std::string type_name{ sans_space_args };

  type_name = gdb_type_name_to_poke (type_name);

  expression_up expr = parse_expression (args);
  struct value *val = expr->evaluate_type ();
  struct type *type = val->type ();

  poke_add_type (type);
}

static void
poke_add_types (const char *args, int from_tty)
{
  const char *symbol_name_regexp = skip_spaces (args);

  if (symbol_name_regexp == nullptr)
    return;

  global_symbol_searcher spec (SEARCH_TYPE_DOMAIN, symbol_name_regexp);
  std::vector<symbol_search> symbols{ spec.search () };

  for (const symbol_search &p : symbols)
    {
      QUIT;

      struct symbol *sym = p.symbol;
      struct type *type = sym->type ();

      if (type != nullptr)
	poke_add_type (type);
    }
}

static void
poke_add_var_command (const char *args, int from_tty)
{
  const auto sans_space_args{ skip_spaces (args) };

  if (sans_space_args == nullptr)
    return;

  std::string sym_name{ sans_space_args };
}

static std::string
addr_to_offset (CORE_ADDR addr)
{
  int unit_size{ gdbarch_addressable_memory_unit_size (get_current_arch ()) };

  return std::to_string (addr) + "#" + std::to_string (unit_size * 8);
}

static void
poke_add_vars (const char *args, int from_tty)
{
  const char *symbol_name_regexp = skip_spaces (args);

  if (symbol_name_regexp == nullptr)
    return;

  global_symbol_searcher spec (SEARCH_VAR_DOMAIN, symbol_name_regexp);
  std::vector<symbol_search> symbols{ spec.search () };

  for (const symbol_search &p : symbols)
    {
      QUIT;

      struct symbol *sym = p.symbol;
      struct type *type = sym->type ();
      CORE_ADDR value_addr = sym->value_address ();
      const char *natural_name = sym->natural_name ();

      if (type != nullptr)
	{
	  std::string defvar{ "var " };
	  std::string poke_type_name;

	  poke_add_type (type, &poke_type_name);

	  defvar += gdb_sym_name_to_poke (natural_name) + " = "
		    + poke_type_name + " @ " + addr_to_offset (value_addr);

	  poke_command (defvar.c_str (), 0 /* from_tty */);
	  gdb_printf ("added variable %s\n", defvar.c_str ());
	}
    }
}

/* Command to execute a poke statement or declaration.	*/

static void
poke_command (const char *args, int from_tty)
{
  args = skip_spaces (args);
  if (args == nullptr)
    return;

  enum
  {
    POKE_DECLARATION,
    POKE_STATEMENT,
  } what;
  const char *end;
  std::string cmd = args;
  pk_val exit_exception = PK_NULL;
  auto is_declaration = [&] (std::string_view prefix) {
    return startswith (cmd, prefix) && isspace (cmd[prefix.length ()]);
  };

  if (is_declaration ("fun"))
    what = POKE_DECLARATION;
  else
    {
      if (is_declaration ("var") || is_declaration ("type")
	  || is_declaration ("unit"))
	what = POKE_DECLARATION;
      else
	what = POKE_STATEMENT;

      cmd += ';';
    }

  pk_set_lexical_cuckolding_p (poke_compiler (), 1);

  if (what == POKE_DECLARATION)
    {
      /* Declaration.  */
      if (pk_compile_buffer (poke_compiler (), cmd.c_str (), &end,
			     &exit_exception)
	      != PK_OK
	  || exit_exception != PK_NULL)
	/* Poke compiler will right error message to the output.  */
	goto error;
    }
  else
    {
      /* Statement.  */
      pk_val val;

      if (pk_compile_statement (poke_compiler (), cmd.c_str (), &end, &val,
				&exit_exception)
	      != PK_OK
	  || exit_exception != PK_NULL)
	goto error;

      if (val != PK_NULL)
	{
	  pk_print_val (poke_compiler (), val, &exit_exception);
	  poke_puts ("\n");
	}
    }

  pk_set_lexical_cuckolding_p (poke_compiler (), 0);

error:
  if (exit_exception != PK_NULL)
    poke_handle_exception (exit_exception);
}

/* Initialize the poke GDB subsystem.  */

void _initialize_poke ();

void
_initialize_poke ()
{
  add_com ("poke-add-type", class_vars, poke_add_type_command, _("\
Make Poke aware of a GDB type given an expression.\n\
Usage: poke-add-type EXPRESSION\n"));

  add_com ("poke-add-types", class_vars, poke_add_types, _("\
Make Poke aware of GDB types based on a regexp.\n\
Usage: poke-add-types REGEXP\n"));

  add_com ("poke-dump-types", class_vars, poke_dump_types, _("\
Dump the definition of all the GDB types known to poke.\n\
Usage: poke-dump-types\n"));

  add_com ("poke-add-var", class_vars, poke_add_var_command, _("\
Add Poke variable for the given symbol.\n\
Usage: poke-add-var EXPRESSION\n"));

  add_com ("poke-add-vars", class_vars, poke_add_vars, _("\
Add Poke variables for the given regexp.\n\
Usage: poke-add-vars REGEXP\n"));

  add_com ("poke", class_vars, poke_command, _("\
Execute a Poke statement or declaration.\n\
Usage: poke [STMT]\n"));
}
