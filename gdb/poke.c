/* GDB integration with GNU poke.

   Copyright (C) 2021 Free Software Foundation, Inc.

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

#include "defs.h"
#include "command.h"
#include "arch-utils.h"
#include "target.h"
#include "gdbcmd.h"
extern "C" {
#include <libpoke.h>
}
#include <ctype.h>
#include <vector>
#include <algorithm>

/* Global poke incremental compiler.  */

static pk_compiler poke_compiler;
static bool poke_compiler_lives = false;

/* Global vector of the Poke code used to define types.  This is
   filled in by poke_add_type and used by poke_dump_types.  */

static std::vector<std::string> type_poke_strings;

/* Terminal hook that flushes the terminal.  */

static void
poke_term_flush (void)
{
  /* Do nothing here.  */
}

/* Terminal hook that prints a fixed string.  */

static void
poke_puts (const char *str)
{
  gdb_printf ("%s", str);
}

/* Terminal hook that prints a formatted string.  */

__attribute__ ((__format__ (__printf__, 1, 2)))
static void
poke_printf (const char *format, ...)
{
  va_list ap;
  char *str;
  int r;

  va_start (ap, format);
  r = vasprintf (&str, format, ap);
  if (r == -1)
    error (_("out of memory in vasprintf")); /* XXX fatal */
  va_end (ap);

  gdb_printf ("%s", str);
  free (str);
}

/* Terminal hook that indents to a given level.  */

static void
poke_term_indent (unsigned int lvl, unsigned int step)
{
  gdb_printf ("\n%*s", (step * lvl), "");
}

/* Terminal hook that starts a styling class.  */

static void
poke_term_class (const char *class_name)
{
  /* Do nothing here.  */
}

/* Terminal hook that finishes a styling class.  */

static int
poke_term_end_class (const char *class_name)
{
  /* Just report success.  */
  return 1;
}

/* Terminal hook that starts a terminal hyperlink.  */

static void
poke_term_hyperlink (const char *url, const char *id)
{
  /* Do nothing here.  */
}

/* Terminal hook that finishes a terminal hyperlink.  */

static int
poke_term_end_hyperlink (void)
{
  /* Just report success.  */
  return 1;
}

/* Terminal hook that returns the current terminal foreground
   color.  */

static struct pk_color
poke_term_get_color (void)
{
  /* Just return the default foreground color.  */
  struct pk_color dfl = {-1,-1,-1};
  return dfl;
}

/* Terminal hook that returns the current terminal background
   color.  */

static struct pk_color
poke_term_get_bgcolor (void)
{
  /* Just return the default background color.  */
  struct pk_color dfl = {-1,-1,-1};
  return dfl;
}

/* Terminal hook that sets the terminal foreground color.  */

static void
poke_term_set_color (struct pk_color color)
{
  /* Do nothing.  */
}

/* Terminal hook that sets the terminal background color.  */

static void
poke_term_set_bgcolor (struct pk_color color)
{
  /* Do nothing.  */
}

/* Implementation of the poke terminal interface, that uses the hooks
   defined above.  */

static struct pk_term_if poke_term_if =
  {
    .flush_fn = poke_term_flush,
    .puts_fn = poke_puts,
    .printf_fn = poke_printf,
    .indent_fn = poke_term_indent,
    .class_fn = poke_term_class,
    .end_class_fn = poke_term_end_class,
    .hyperlink_fn = poke_term_hyperlink,
    .end_hyperlink_fn = poke_term_end_hyperlink,
    .get_color_fn = poke_term_get_color,
    .get_bgcolor_fn = poke_term_get_bgcolor,
    .set_color_fn = poke_term_set_color,
    .set_bgcolor_fn = poke_term_set_bgcolor,
  };

/* Foreign IO device hook that returns an unique name identifying the
   kind of device.  */

static const char *
iod_get_if_name (void)
{
  return "GDB";
}

/* Foreign IO device hook that recognizes whether a given IO space
   handler refer to this kind of device, and normalizes it for further
   use.  */

static char *
iod_handler_normalize (const char *handler, uint64_t flags, int *error)
{
  char *new_handler = NULL;

  if (strcmp (handler, "<gdb>") == 0)
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
  int ret = target_read_memory (offset, (gdb_byte *) buf, count);
  return ret == -1 ? PK_IOD_ERROR : PK_IOD_OK;
}

/* Foreign IO device hook that writes data to a device.  */

static int
iod_pwrite (void *dev, const void *buf, size_t count, pk_iod_off offset)
{
  int ret = target_write_memory (offset, (gdb_byte *) buf, count);
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
  return (gdbarch_addr_bit (get_current_arch ()) == 32
	  ? 0xffffffff : 0xffffffffffffffff);
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

static struct pk_iod_if iod_if =
  {
    iod_get_if_name,
    iod_handler_normalize,
    iod_open,
    iod_close,
    iod_pread,
    iod_pwrite,
    iod_get_flags,
    iod_size,
    iod_flush
  };

/* Handler for alien tokens.  */

static struct pk_alien_token alien_token;

static struct pk_alien_token *
poke_alien_token_handler (const char *id, char **errmsg)
{
  /* In GDB alien poke tokens with the form $addr::FOO provide the
     address of the symbol `FOO' as an offset in bytes, i.e. it
     resolves to the GDB value &foo as a Poke offset with unit bytes.

     $FOO, on the other hand, provide the value of the symbol FOO
     incarnated in a proper Poke value.  */

  if (strncmp (id, "addr::", 6) == 0)
    {
      CORE_ADDR addr;

      std::string expr = "&";
      expr += id + 6;

      try
	{
	  addr = parse_and_eval_address (expr.c_str ());
	}
      catch (const gdb_exception_error &except)
	{
	  goto error;
	}

      alien_token.kind = PK_ALIEN_TOKEN_OFFSET;
      alien_token.value.offset.magnitude = addr;
      alien_token.value.offset.width = 64;
      alien_token.value.offset.signed_p = 0;
      alien_token.value.offset.unit = 8;
    }
  else
    {
      struct value *value;

      try
	{
	  value = parse_and_eval (id);
	}
      catch (const gdb_exception_error &except)
	{
	  goto error;
	}

      struct type *type = value_type (value);

      if (can_dereference (type))
	{
	  alien_token.kind = PK_ALIEN_TOKEN_OFFSET;
	  alien_token.value.offset.magnitude
	    = value_as_address (value);
	  alien_token.value.offset.width
	    = TYPE_LENGTH (type) * 8;
	  alien_token.value.offset.signed_p = 0;
	  alien_token.value.offset.unit = 8;
	}
      else if (is_integral_type (type))
	{
	  alien_token.kind = PK_ALIEN_TOKEN_INTEGER;
	  alien_token.value.integer.magnitude
	    = value_as_long (value);
	  alien_token.value.integer.width
	    = TYPE_LENGTH (type) * 8;
	  alien_token.value.integer.signed_p
	    = !type->is_unsigned ();
	}
      else
	goto error;
    }

  *errmsg = NULL;
  return &alien_token;

 error:
  std::string emsg = "can't access GDB variable '";
  emsg += id;
  emsg += "'";
  *errmsg = xstrdup (emsg.c_str ());
  return NULL;
}

/* Given a string, prefix it in order to avoid collision with Poke's
   keywords.  */

static std::vector<std::string> poke_keywords
  {
    "pinned", "struct", "union", "else", "while", "until",
    "for", "in", "where", "if", "sizeof", "fun", "method",
    "type", "var", "unit", "break", "continue", "return",
    "string", "as", "try", "catch", "raise", "void", "any",
    "print", "printf", "isa", "unmap", "big", "little",
    "load", "lambda", "assert",
  };

static std::string
normalize_poke_identifier (std::string prefix, std::string str)
{
  if (std::find (poke_keywords.begin (),
		 poke_keywords.end (),
		 str) != poke_keywords.end ())
    str = prefix + str;

  return str;
}

/* Given a GDB type name, mangle it to a valid Poke type name.  */

static std::string
gdb_type_name_to_poke (std::string str, struct type *type = NULL)
{
  for (int i = 0; i < str.length (); ++i)
    if (!(str.begin()[i] == '_'
	  || (str.begin()[i] >= 'a' && str.begin()[i] <= 'z')
	  || (str.begin()[i] >= '0' && str.begin()[i] <= '9')
	  || (str.begin()[i] >= 'A' && str.begin()[i] <= 'Z')))
      str.begin()[i] = '_';

  if (type)
    {
      /* Prepend struct and union tags with suitable prefixes.  This
	 is to avoid ending with recursive typedefs in C programs.  */
      if (type->code () == TYPE_CODE_STRUCT)
	str = "struct_" + str;
      else if (type->code () == TYPE_CODE_UNION)
	str = "union_" + str;
    }

  return str;
}

/* Command to feed the poke compiler with the definition of some given
   GDB type.  */

static void poke_command (const char *args, int from_tty);

static std::string
poke_add_type (struct type *type)
{
  std::string type_name;
  std::string str = "";

  if (type != nullptr)
    {
      if (type->name ())
	type_name = type->name ();

      /* Do not try to add a type that is already defined.  */
      if (type_name != ""
	  && pk_decl_p (poke_compiler,
			gdb_type_name_to_poke (type_name, type).c_str (),
			PK_DECL_KIND_TYPE))
	return type_name;

      switch (type->code ())
	{
	case TYPE_CODE_PTR:
	  {
	    str = ("offset<uint<"
		   + (std::to_string (TYPE_LENGTH (type) * 8))
		   + ">,B>");
	    break;
	  }
	case TYPE_CODE_TYPEDEF:
	  {
	    struct type *target_type = TYPE_TARGET_TYPE (type);
	    std::string target_type_code = poke_add_type (target_type);

	    if (target_type_code == "")
	      goto skip;

	    if (target_type->name ())
	      str += gdb_type_name_to_poke (target_type->name (), target_type);
	    else
	      str += target_type_code;
	    break;
	  }
	case TYPE_CODE_INT:
	  {
	    size_t type_length = TYPE_LENGTH (type) * 8;

	    if (type_length > 64)
	      goto skip;

	    if (type->is_unsigned ())
	      str += "u";
	    str += "int<";
	    str += std::to_string (type_length);
	    str += ">";
	    break;
	  }
	case TYPE_CODE_ARRAY:
	  {
	    struct type *target_type = TYPE_TARGET_TYPE (type);
	    size_t target_type_length = TYPE_LENGTH (target_type);
	    std::string target_type_code
	      = poke_add_type (target_type);

	    if (target_type_code == "")
	      goto skip;

	    if (target_type->name ())
	      str = gdb_type_name_to_poke (target_type->name (), target_type);
	    else
	      str = target_type_code;

	    str += "[";
	    str += std::to_string (TYPE_LENGTH (type) / target_type_length);
	    str += "]";
	    break;
	  }
	case TYPE_CODE_STRUCT:
	  {
	    size_t natural_bitpos = 0;
	    str += "struct {";

	    for (int idx = 0; idx < type->num_fields (); idx++)
	      {
		std::string field_name
		  = normalize_poke_identifier ("__f", type->field (idx).name ());
		struct type *field_type = type->field (idx).type ();
		size_t field_bitpos = type->field (idx).loc_bitpos ();

		if (idx > 0)
		  str += " ";
		if (field_type->name ())
		  {
		    if (poke_add_type (field_type) == "")
		      goto skip;
		    str += gdb_type_name_to_poke (field_type->name (), field_type);
		  }
		else
		  {
		    std::string pstr = poke_add_type (field_type);
		    if (pstr == "")
		      goto skip;
		    str += pstr;
		  }
		str += " ";
		if (field_name != "")
		  str += field_name;
		if (field_bitpos != natural_bitpos)
		  str += " @ " + (field_bitpos % 8 == 0
				  ? std::to_string (field_bitpos / 8) + "#B"
				  : std::to_string (field_bitpos) + "#b");
		str += ";";

		natural_bitpos = field_bitpos + TYPE_LENGTH (field_type) * 8;
	      }

	    str += "}";
	    break;
	  }
	default:
	  goto skip;
	  break;
	}

      if (type_name != "")
	{
	  std::string poke_type_name
	    = gdb_type_name_to_poke (type_name, type);

	  std::string deftype = "type ";
	  deftype += poke_type_name;
	  deftype += " = ";
	  deftype += str;

	  type_poke_strings.push_back (deftype);
	  poke_command (deftype.c_str(), 0 /* from_tty */);
	  gdb_printf ("added type %s\n", poke_type_name.c_str ());
	}
    }

  return str;

 skip:
  return "";
}

/* Definition of a handler for unhandled Poke exceptions.  */

static const char *poke_exception_handler="\
fun gdb_exception_handler = (Exception exception) void:\
{\
  if (exception.code != EC_exit && exception.code != EC_signal)\
  {\
    print (\"unhandled \"\
           + (exception.name == \"\" ? \"unknown\" : exception.name)\
           + \" exception\n\");\
\
    if (exception.location != \"\" || exception.msg != \"\")\
    {\
      if (exception.location != \"\")\
        print (exception.location + \" \");\
      print (exception.msg + \"\n\");\
    }\
  }\
}";

/* Call the default poke exception handler.  */

static void
poke_handle_exception (pk_val exception)
{
  pk_val handler = pk_decl_val (poke_compiler, "gdb_exception_handler");

  if (handler == PK_NULL)
    error (_("Couldn't get a handler for poke gdb_exception_handler"));
  if (pk_call (poke_compiler, handler, NULL, NULL, 1, exception)
      == PK_ERROR)
    error (_("Couldn't call gdb_exception_handler in poke"));
}

/* Start the poke incremental compiler.  */

static void
start_poke (void)
{
  pk_val exit_exception;

  /* Note how we are creating an incremental compiler without the
     standard Poke types (int, etc) because they collide with the C
     types.  */
  poke_compiler = pk_compiler_new_with_flags (&poke_term_if,
					      PK_F_NOSTDTYPES);
  if (poke_compiler == NULL)
    error (_("Couldn't start the poke incremental compiler."));
  poke_compiler_lives = true;

  /* Install the handler for alien tokens that recognizes GDB
     symbols.  */
  pk_set_alien_token_fn (poke_compiler, poke_alien_token_handler);

  /* Use hexadecimal output by default.  */
  pk_set_obase (poke_compiler, 16);

  /* Use `tree' printing mode by default.  */
  pk_set_omode (poke_compiler, PK_PRINT_TREE);

  /* Define a handler that we will use for processing unhandled Poke
     exceptions.  */
  if (pk_compile_buffer (poke_compiler, poke_exception_handler,
			 NULL, &exit_exception) != PK_OK
      || exit_exception != PK_NULL)
    error (_("Could not define the Poke default exception handler"));

  /* Install our foreign IO device interface to access the target's
     memory, and open it.  */
  if (pk_register_iod (poke_compiler, &iod_if) != PK_OK)
    error (_("Could not register the foreign IO device interface in poke."));

  if (pk_compile_buffer (poke_compiler, "open (\"<gdb>\");", NULL,
			 &exit_exception) != PK_OK
      || exit_exception != PK_NULL)
    {
      if (exit_exception != PK_NULL)
	poke_handle_exception (exit_exception);
      error (_("Could not open <gdb>"));
    }

  /* Provide access to pickles installed by poke applications.  */
  pk_val pk_load_path = pk_decl_val (poke_compiler, "load_path");
  std::string load_path = pk_string_str (pk_load_path);
  load_path += ":%DATADIR%/pickles";
  pk_decl_set_val (poke_compiler, "load_path",
		   pk_make_string (load_path.c_str ()));
}

/* Function to finalize the poke subsystem.  This is registered with
   make_final_cleanup in _initialize_poke.  */

static void
poke_finalize (void *arg)
{
  if (poke_compiler_lives)
    {
      pk_val val, exit_exception;
      if (pk_compile_statement (poke_compiler,
				"try close (get_ios); catch if E_no_ios {}",
				NULL, &val, &exit_exception) != PK_OK
	  || exit_exception != PK_NULL)
	error (_("Error while closing an IOS on exit."));

      pk_compiler_free (poke_compiler);
      poke_compiler_lives = false;
    }
}

/* Command to dump the Poke definition of known types.  */

static void
poke_dump_types (const char *args, int from_tty)
{
  if (!poke_compiler_lives)
    start_poke ();

  for (const std::string &s : type_poke_strings)
    printf ("%s;\n", s.c_str ());
}

/* Commands to add GDB types to the running poke compiler.  */

static void
poke_add_type_command (const char *args, int from_tty)
{
  if (!poke_compiler_lives)
    start_poke ();

  std::string type_name = skip_spaces (args);
  type_name = gdb_type_name_to_poke (type_name);

  expression_up expr = parse_expression (args);
  struct value *val = evaluate_type (expr.get ());
  struct type *type = value_type (val);

  poke_add_type (type);
}

static void
poke_add_types (const char *args, int from_tty)
{
  if (!poke_compiler_lives)
    start_poke ();

  std::string symbol_name_regexp = skip_spaces (args);
  global_symbol_searcher spec (TYPES_DOMAIN, symbol_name_regexp.c_str ());
  std::vector<symbol_search> symbols = spec.search ();
  for (const symbol_search &p : symbols)
    {
      QUIT;

      struct symbol *sym = p.symbol;
      struct type *type = sym->type ();

      if (type)
	poke_add_type (type);
    }
}

/* Command to execute a poke statement or declaration.  */

static void
poke_command (const char *args, int from_tty)
{
  if (!poke_compiler_lives)
    start_poke ();

  int what; /* 0 -> declaration, 1 -> statement */
  const char *end;
  std::string cmd;
  pk_val exit_exception = PK_NULL;

#define IS_COMMAND(input, cmd) \
  (strncmp ((input), (cmd), sizeof (cmd) - 1) == 0 \
   && ((input)[sizeof (cmd) - 1] == ' ' || (input)[sizeof (cmd) - 1] == '\t'))

  args = skip_spaces (args);
  if (args == NULL)
    return;

  if (IS_COMMAND (args, "fun"))
  {
    what = 0;
    cmd = args;
  }
  else
    {
      if (IS_COMMAND (args, "var")
	  || IS_COMMAND (args, "type")
	  || IS_COMMAND (args, "unit"))
	what = 0;
      else
	what = 1;

      cmd = args;
      cmd += ';';
    }

  pk_set_lexical_cuckolding_p (poke_compiler, 1);

  if (what == 0)
    {
      /* Declaration.  */
      if (pk_compile_buffer (poke_compiler, cmd.c_str (),
			     &end, &exit_exception) != PK_OK
	  || exit_exception != PK_NULL)
	goto error;
    }
  else
    {
      /* Statement.  */
      pk_val val;

      if (pk_compile_statement (poke_compiler, cmd.c_str (), &end,
				&val, &exit_exception) != PK_OK
	  || exit_exception != PK_NULL)
	goto error;

      if (val != PK_NULL)
	{
	  pk_print_val (poke_compiler, val, &exit_exception);
	  poke_puts ("\n");
	}
    }

  pk_set_lexical_cuckolding_p (poke_compiler, 0);
#undef IS_COMMAND
 error:
  if (exit_exception != PK_NULL)
    poke_handle_exception (exit_exception);
}

/* Initialize the poke GDB subsystem.  */

void _initialize_poke (void);
void
_initialize_poke ()
{
  add_com ("poke-add-type", class_vars, poke_add_type_command, _("\
Make Poke aware of a GDB type given an expression.\n\
Usage: poke-add-type EXPRESSION\n"));

  add_com ("poke-add-types", class_vars, poke_add_types, _("\
Make Poke aware of GDB types based on a regexp.\n\
Usage: poke-add-type REGEXP\n"));

  add_com ("poke-dump-types", class_vars, poke_dump_types, _("\
Dump the definition of all the GDB types known to poke.\n\
Usage: poke-dump-types\n"));

  add_com ("poke", class_vars, poke_command, _("\
Execute a Poke statement or declaration.\n\
Usage: poke [STMT]\n"));

  make_final_cleanup (poke_finalize, NULL);
}
