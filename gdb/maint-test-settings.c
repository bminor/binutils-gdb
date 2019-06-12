/* Support for GDB maintenance commands.

   Copyright (C) 2019 Free Software Foundation, Inc.

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
#include "gdbcmd.h"

/* Command list for "maint test-settings".  */
static cmd_list_element *maintenance_test_settings_list;

/* Command list for "maint test-settings set/show".  */
static cmd_list_element *maintenance_test_settings_set_list;
static cmd_list_element *maintenance_test_settings_show_list;

/* The "maintenance test-options" prefix command.  */

static void
maintenance_test_settings_cmd (const char *arg, int from_tty)
{
  printf_unfiltered
    (_("\"maintenance test-settings\" must be followed "
       "by the name of a subcommand.\n"));
  help_list (maintenance_test_settings_list, "maintenance test-settings ",
	     all_commands, gdb_stdout);
}

/* The "maintenance test-options set" prefix command.  */

static void
maintenance_test_settings_set_cmd (const char *args, int from_tty)
{
  printf_unfiltered (_("\"maintenance test-settings set\" must be followed "
		       "by the name of a set command.\n"));
  help_list (maintenance_test_settings_set_list,
	     "maintenance test-settings set ",
	     all_commands, gdb_stdout);
}

/* The "maintenance test-options show" prefix command.  */

static void
maintenance_test_settings_show_cmd (const char *args, int from_tty)
{
  cmd_show_list (maintenance_test_settings_show_list, from_tty, "");
}

/* Control variables for all the "maintenance test-options set/show
   xxx" commands.  */

static int maintenance_test_settings_boolean;

static auto_boolean maintenance_test_settings_auto_boolean = AUTO_BOOLEAN_AUTO;

static unsigned int maintenance_test_settings_uinteger;

static int maintenance_test_settings_integer;

static int maintenance_test_settings_zinteger;

static unsigned int maintenance_test_settings_zuinteger;

static int maintenance_test_settings_zuinteger_unlimited;

static char *maintenance_test_settings_string;

static char *maintenance_test_settings_string_noescape;

static char *maintenance_test_settings_optional_filename;

static char *maintenance_test_settings_filename;

static const char *maintenance_test_settings_enum;

/* Enum values for the "maintenance test-settings set/show boolean"
   commands.  */
static const char *const maintenance_test_settings_enums[] = {
  "xxx", "yyy", "zzz", nullptr
};

/* The "maintenance test-options show xxx" commands.  */

static void
maintenance_test_settings_show_value_cmd
  (struct ui_file *file, int from_tty,
   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, (("%s\n")), value);
}


void
_initialize_maint_test_settings (void)
{
  add_prefix_cmd ("test-settings", no_class,
		  maintenance_test_settings_cmd,
		  _("\
Generic command for testing the settings infrastructure."),
		  &maintenance_test_settings_list,
		  "maintenance test-settings ", 0,
		  &maintenancelist);

  add_prefix_cmd ("set", class_maintenance,
		  maintenance_test_settings_set_cmd, _("\
Set GDB internal variables used for set/show command infrastructure testing."),
		  &maintenance_test_settings_set_list,
		  "maintenance test-settings set ",
		  0/*allow-unknown*/,
		  &maintenance_test_settings_list);

  add_prefix_cmd ("show", class_maintenance,
		  maintenance_test_settings_show_cmd, _("\
Show GDB internal variables used for set/show command infrastructure testing."),
		  &maintenance_test_settings_show_list,
		  "maintenance test-settings show ",
		  0/*allow-unknown*/,
		  &maintenance_test_settings_list);

  add_setshow_boolean_cmd ("boolean", class_maintenance,
			   &maintenance_test_settings_boolean, _("\
command used for internal testing"), _("\
command used for internal testing"),
			   nullptr, /* help_doc */
			   nullptr, /* set_cmd */
			   maintenance_test_settings_show_value_cmd,
			   &maintenance_test_settings_set_list,
			   &maintenance_test_settings_show_list);

  add_setshow_auto_boolean_cmd ("auto-boolean", class_maintenance,
				&maintenance_test_settings_auto_boolean, _("\
command used for internal testing"), _("\
command used for internal testing"),
				nullptr, /* help_doc */
				nullptr, /* set_cmd */
				maintenance_test_settings_show_value_cmd,
				&maintenance_test_settings_set_list,
				&maintenance_test_settings_show_list);

  add_setshow_uinteger_cmd ("uinteger", class_maintenance,
			   &maintenance_test_settings_uinteger, _("\
command used for internal testing"), _("\
command used for internal testing"),
			   nullptr, /* help_doc */
			   nullptr, /* set_cmd */
			   maintenance_test_settings_show_value_cmd,
			   &maintenance_test_settings_set_list,
			   &maintenance_test_settings_show_list);

  add_setshow_integer_cmd ("integer", class_maintenance,
			   &maintenance_test_settings_integer, _("\
command used for internal testing"), _("\
command used for internal testing"),
			   nullptr, /* help_doc */
			   nullptr, /* set_cmd */
			   maintenance_test_settings_show_value_cmd,
			   &maintenance_test_settings_set_list,
			   &maintenance_test_settings_show_list);

  add_setshow_string_cmd ("string", class_maintenance,
     &maintenance_test_settings_string, _("\
command used for internal testing"), _("\
command used for internal testing"),
     nullptr, /* help_doc */
     nullptr, /* set_cmd */
     maintenance_test_settings_show_value_cmd,
     &maintenance_test_settings_set_list,
     &maintenance_test_settings_show_list);

  add_setshow_string_noescape_cmd
    ("string-noescape", class_maintenance,
     &maintenance_test_settings_string_noescape, _("\
command used for internal testing"), _("\
command used for internal testing"),
     nullptr, /* help_doc */
     nullptr, /* set_cmd */
     maintenance_test_settings_show_value_cmd,
     &maintenance_test_settings_set_list,
     &maintenance_test_settings_show_list);

  add_setshow_optional_filename_cmd
    ("optional-filename", class_maintenance,
     &maintenance_test_settings_optional_filename, _("\
command used for internal testing"), _("\
command used for internal testing"),
     nullptr, /* help_doc */
     nullptr, /* set_cmd */
     maintenance_test_settings_show_value_cmd,
     &maintenance_test_settings_set_list,
     &maintenance_test_settings_show_list);

  add_setshow_filename_cmd ("filename", class_maintenance,
			    &maintenance_test_settings_filename, _("\
command used for internal testing"), _("\
command used for internal testing"),
			    nullptr, /* help_doc */
			    nullptr, /* set_cmd */
			    maintenance_test_settings_show_value_cmd,
			    &maintenance_test_settings_set_list,
			    &maintenance_test_settings_show_list);

  add_setshow_zinteger_cmd ("zinteger", class_maintenance,
			    &maintenance_test_settings_zinteger, _("\
command used for internal testing"), _("\
command used for internal testing"),
			    nullptr, /* help_doc */
			    nullptr, /* set_cmd */
			    maintenance_test_settings_show_value_cmd,
			    &maintenance_test_settings_set_list,
			    &maintenance_test_settings_show_list);

  add_setshow_zuinteger_cmd ("zuinteger", class_maintenance,
			     &maintenance_test_settings_zuinteger, _("\
command used for internal testing"), _("\
command used for internal testing"),
			     nullptr, /* help_doc */
			     nullptr, /* set_cmd */
			     maintenance_test_settings_show_value_cmd,
			     &maintenance_test_settings_set_list,
			     &maintenance_test_settings_show_list);

  add_setshow_zuinteger_unlimited_cmd
    ("zuinteger-unlimited", class_maintenance,
     &maintenance_test_settings_zuinteger_unlimited, _("\
command used for internal testing"), _("\
command used for internal testing"),
     nullptr, /* help_doc */
     nullptr, /* set_cmd */
     maintenance_test_settings_show_value_cmd,
     &maintenance_test_settings_set_list,
     &maintenance_test_settings_show_list);

  add_setshow_enum_cmd ("enum", class_maintenance,
			maintenance_test_settings_enums,
			&maintenance_test_settings_enum, _("\
command used for internal testing"), _("\
command used for internal testing"),
			nullptr, /* help_doc */
			nullptr, /* set_cmd */
			maintenance_test_settings_show_value_cmd,
			&maintenance_test_settings_set_list,
			&maintenance_test_settings_show_list);
}
