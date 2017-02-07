/* Commands for Linux kernel target.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#include "cli/cli-decode.h"
#include "gdbcore.h"
#include "inferior.h"
#include "lk-lists.h"
#include "lk-low.h"
#include "lk-modules.h"
#include "typeprint.h"
#include "valprint.h"


/* Print line for module MOD to UIOUT for lsmod command.  */

static bool
lk_lsmod_print_single_module (struct ui_out *uiout, CORE_ADDR mod)
{
  char *src_list, name[LK_MODULE_NAME_LEN + 2];
  CORE_ADDR next, src_list_addr;
  size_t src_num, src_size, list_len;
  bool loaded;
  struct cleanup *ui_chain;


  /* Get name.  */
  read_memory_string (mod + LK_OFFSET (module, name), name + 1,
		      LK_MODULE_NAME_LEN);
  loaded = lk_modules_debug_info_loaded (name + 1);
  name[0] = loaded ? ' ' : '*' ;
  name[LK_MODULE_NAME_LEN + 1] = '\0';

  /* Get size.  */
  if (LK_FIELD (module, module_core))
    {
      src_size = lk_read_uint (mod + LK_OFFSET (module, init_size));
      src_size += lk_read_uint (mod + LK_OFFSET (module, core_size));
    }
  else
    {
      src_size = lk_read_uint (mod + LK_OFFSET (module, init_layout)
			       + LK_OFFSET (module_layout, size));
      src_size += lk_read_uint (mod + LK_OFFSET (module, core_layout)
				+ LK_OFFSET (module_layout, size));
    }

  /* Get number of sources and list of their names.  */
  src_num = 0;
  src_list_addr = mod + LK_OFFSET (module, source_list);
  src_list = xstrdup ("");
  list_len = 0;

  lk_list_for_each (next, src_list_addr, module, source_list)
    {
      char src_name[LK_MODULE_NAME_LEN + 1];
      CORE_ADDR src_mod, src_addr;

      src_addr = (LK_CONTAINER_OF (next, module_use, source_list)
		  + LK_OFFSET (module_use, source));
      src_mod = lk_read_addr (src_addr);
      read_memory_string (src_mod + LK_OFFSET (module, name), src_name,
			  LK_MODULE_NAME_LEN);

      /* 2 = strlen (", ").  */
      list_len += strlen (src_name) + 2;
      src_list = reconcat (src_list, src_list, src_name, ", ", NULL);

      src_num++;
    }

  make_cleanup (xfree, src_list);
  /* Remove trailing comma.  */
  if (strlen (src_list) >= 2)
    src_list [list_len - 2] = '\0';

  ui_chain = make_cleanup_ui_out_tuple_begin_end (uiout, NULL);

  uiout->field_fmt ("addr", "0x%s",
		    phex(mod, lk_builtin_type_size (unsigned_long)));
  uiout->field_string ("module", name);
  uiout->field_int ("size", src_size);
  uiout->field_int ("src_num", src_num);
  uiout->field_string ("src_list", src_list);
  uiout->text ("\n");

  do_cleanups (ui_chain);
  return loaded;
}

/* Print information about loaded kernel modules.  Output equivalent to
   lsmod, but also prints the address of the corrensponfing struct module.
   Marks modules with missing debug info with an asterix '*'.  */

void
lk_lsmod (char *args, int from_tty)
{
  struct ui_out *uiout;
  struct cleanup *ui_chain;
  CORE_ADDR modules, next;
  bool all_loaded = true;

  uiout = current_uiout;
  ui_chain = make_cleanup_ui_out_table_begin_end (uiout, 5, -1,
						  "ModuleTable");
  uiout->table_header (14, ui_left, "addr", "ADDR");
  uiout->table_header (20, ui_left, "module", "Module");
  uiout->table_header (7, ui_right, "size", "Size");
  uiout->table_header (4, ui_right, "src_num", "");
  uiout->table_header (40, ui_left, "src_list", "Used by");

  uiout->table_body ();

  modules = LK_ADDR (modules);
  lk_list_for_each (next, modules, module, list)
    {
      CORE_ADDR mod;
      mod = LK_CONTAINER_OF (next, module, list);
      all_loaded &= lk_lsmod_print_single_module (uiout, mod);
    }
  if (!all_loaded)
    uiout->text ("(*) Missing debug info for module.\n");

  do_cleanups (ui_chain);
}

static void
lk_print_struct (char *args_, int from_tty)
{
  struct format_data fmt;
  size_t pos;
  print_command_parse_format ((const char **) &args_, "print", &fmt);

  if (!args_)
    return;

  std::string args (args_);
  /* No address given default to behave like ptype.  */
  if ((pos = args.find (" ")) == std::string::npos)
    {
      args = "struct " + args;
      char *tmp = xstrdup (args.c_str ());
      whatis_exp (tmp, 1);
      xfree (tmp);
      return;
    }


  std::string type = args.substr (0, pos);
  std::string addr = args.substr (args.find_first_not_of (" ", pos));

  if ((pos = type.find ("."))!= std::string::npos)
    {
      std::string field = type.substr (pos + 1);
      type = type.substr (0, pos);
      args = "((struct " + type + " *) " + addr + ")->" + field;
    }
  else if ((pos = type.find ("->"))!= std::string::npos)
    {
      std::string field = type.substr (pos + 2);
      type = type.substr (0, pos);
      args = "((struct " + type + " *) " + addr + ")->" + field;
    }
  else
    args = "*(struct " + type + " *) " + addr;

  expression_up expr = parse_expression (args.c_str ());
  struct value *val = evaluate_expression (expr.get ());

  print_value (val, &fmt);
}

#include "gdbtypes.h"
void
lk_print_offset (char *args_, int from_tty)
{
  std::string args (args_);
  std::string type, field;
  size_t pos;

  if ((pos = args.find ('.')) != std::string::npos)
    {
      type = "struct " + args.substr (0, pos);
      field = args.substr (pos + 1);
    }
  else if ((pos = args.find ("->")) != std::string::npos)
    {
      type = "struct " + args.substr (0, pos);
      field = args.substr (pos + 2);
    }
  else
    return;

  expression_up expr = parse_expression (type.c_str ());
  struct type *tp = value_type (evaluate_type (expr.get ()));

  struct field *first = TYPE_FIELDS (tp);
  struct field *last = first + TYPE_NFIELDS (tp);

  for (; first != last; first++)
    if (field.compare (first->name) == 0)
      break;

  if (first == last)
    return;

  size_t offset = FIELD_BITPOS (*first);

  if (offset % TARGET_CHAR_BIT)
    printf_unfiltered ("offset = %lu + %lu\n", offset / 8, offset % TARGET_CHAR_BIT);
  else
    printf_unfiltered ("offset = %lu\n", offset / 8);
}

void
lk_init_cmds ()
{
  add_com ("lsmod", class_vars, lk_lsmod, "\n\
	lsmod\n\n\
List kernel modules as known by the kernel.  The address belongs to the \
corresponding struct module.  \
");

  add_com ("struct", class_vars, lk_print_struct, "\n\
	struct <struct>.<field> <address>\n\n\
Print content of field <field> in structure <struct> for structure located\n\
at address <address>.  If no field is given prints entire content of\n\
<struct>.  If neither <field> nor <address> is given prints type definition\n\
of <struct> (equivalent to ptype).\
");

  add_com ("offset", class_vars, lk_print_offset, "\n\
	offset <struct>.<field>\n\n\
Print offset of field <field> in structure <struct> in byte (+ bit for bit fields).\n\
");
}
