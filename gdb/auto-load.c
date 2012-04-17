/* GDB routines for supporting auto-loaded scripts.

   Copyright (C) 2012 Free Software Foundation, Inc.

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
#include "auto-load.h"
#include "progspace.h"
#include "python/python.h"
#include "gdb_regex.h"
#include "ui-out.h"
#include "filenames.h"
#include "command.h"
#include "observer.h"
#include "objfiles.h"
#include "exceptions.h"
#include "cli/cli-script.h"
#include "gdbcmd.h"
#include "cli/cli-decode.h"
#include "cli/cli-setshow.h"

/* The suffix of per-objfile scripts to auto-load as non-Python command files.
   E.g. When the program loads libfoo.so, look for libfoo-gdb.gdb.  */
#define GDB_AUTO_FILE_NAME "-gdb.gdb"

static void source_gdb_script_for_objfile (struct objfile *objfile, FILE *file,
					   const char *filename);

/* User-settable option to enable/disable auto-loading of GDB_AUTO_FILE_NAME
   scripts:
   set auto-load gdb-scripts on|off
   This is true if we should auto-load associated scripts when an objfile
   is opened, false otherwise.  */
static int auto_load_gdb_scripts = 1;

/* "show" command for the auto_load_gdb_scripts configuration variable.  */

static void
show_auto_load_gdb_scripts (struct ui_file *file, int from_tty,
			    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Auto-loading of canned sequences of commands "
			    "scripts is %s.\n"),
		    value);
}

/* Internal-use flag to enable/disable auto-loading.
   This is true if we should auto-load python code when an objfile is opened,
   false otherwise.

   Both auto_load_scripts && global_auto_load must be true to enable
   auto-loading.

   This flag exists to facilitate deferring auto-loading during start-up
   until after ./.gdbinit has been read; it may augment the search directories
   used to find the scripts.  */
int global_auto_load = 1;

/* Auto-load .gdbinit file from the current directory?  */
int auto_load_local_gdbinit = 1;

/* Absolute pathname to the current directory .gdbinit, if it exists.  */
char *auto_load_local_gdbinit_pathname = NULL;

/* Boolean value if AUTO_LOAD_LOCAL_GDBINIT_PATHNAME has been loaded.  */
int auto_load_local_gdbinit_loaded = 0;

/* "show" command for the auto_load_local_gdbinit configuration variable.  */

static void
show_auto_load_local_gdbinit (struct ui_file *file, int from_tty,
			      struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Auto-loading of .gdbinit script from current "
			    "directory is %s.\n"),
		    value);
}

/* Definition of script language for GDB canned sequences of commands.  */

static const struct script_language script_language_gdb
  = { GDB_AUTO_FILE_NAME, source_gdb_script_for_objfile };

static void
source_gdb_script_for_objfile (struct objfile *objfile, FILE *file,
			       const char *filename)
{
  struct auto_load_pspace_info *pspace_info;
  volatile struct gdb_exception e;

  /* Add this script to the hash table too so "info auto-load gdb-scripts"
     can print it.  */
  pspace_info = get_auto_load_pspace_data_for_loading (current_program_space);
  maybe_add_script (pspace_info, filename, filename, &script_language_gdb);

  TRY_CATCH (e, RETURN_MASK_ALL)
    {
      script_from_file (file, filename);
    }
  exception_print (gdb_stderr, e);
}

/* For scripts specified in .debug_gdb_scripts, multiple objfiles may load
   the same script.  There's no point in loading the script multiple times,
   and there can be a lot of objfiles and scripts, so we keep track of scripts
   loaded this way.  */

struct auto_load_pspace_info
{
  /* For each program space we keep track of loaded scripts.  */
  struct htab *loaded_scripts;

  /* Non-zero if we've issued the warning about an auto-load script not being
     found.  We only want to issue this warning once.  */
  int script_not_found_warning_printed;
};

/* Objects of this type are stored in the loaded script hash table.  */

struct loaded_script
{
  /* Name as provided by the objfile.  */
  const char *name;

  /* Full path name or NULL if script wasn't found (or was otherwise
     inaccessible).  */
  const char *full_path;

  const struct script_language *language;
};

/* Per-program-space data key.  */
static const struct program_space_data *auto_load_pspace_data;

static void
auto_load_pspace_data_cleanup (struct program_space *pspace, void *arg)
{
  struct auto_load_pspace_info *info;

  info = program_space_data (pspace, auto_load_pspace_data);
  if (info != NULL)
    {
      if (info->loaded_scripts)
	htab_delete (info->loaded_scripts);
      xfree (info);
    }
}

/* Get the current autoload data.  If none is found yet, add it now.  This
   function always returns a valid object.  */

static struct auto_load_pspace_info *
get_auto_load_pspace_data (struct program_space *pspace)
{
  struct auto_load_pspace_info *info;

  info = program_space_data (pspace, auto_load_pspace_data);
  if (info == NULL)
    {
      info = XZALLOC (struct auto_load_pspace_info);
      set_program_space_data (pspace, auto_load_pspace_data, info);
    }

  return info;
}

/* Hash function for the loaded script hash.  */

static hashval_t
hash_loaded_script_entry (const void *data)
{
  const struct loaded_script *e = data;

  return htab_hash_string (e->name) ^ htab_hash_pointer (e->language);
}

/* Equality function for the loaded script hash.  */

static int
eq_loaded_script_entry (const void *a, const void *b)
{
  const struct loaded_script *ea = a;
  const struct loaded_script *eb = b;

  return strcmp (ea->name, eb->name) == 0 && ea->language == eb->language;
}

/* Initialize the table to track loaded scripts.
   Each entry is hashed by the full path name.  */

static void
init_loaded_scripts_info (struct auto_load_pspace_info *pspace_info)
{
  /* Choose 31 as the starting size of the hash table, somewhat arbitrarily.
     Space for each entry is obtained with one malloc so we can free them
     easily.  */

  pspace_info->loaded_scripts = htab_create (31,
					     hash_loaded_script_entry,
					     eq_loaded_script_entry,
					     xfree);

  pspace_info->script_not_found_warning_printed = FALSE;
}

/* Wrapper on get_auto_load_pspace_data to also allocate the hash table
   for loading scripts.  */

struct auto_load_pspace_info *
get_auto_load_pspace_data_for_loading (struct program_space *pspace)
{
  struct auto_load_pspace_info *info;

  info = get_auto_load_pspace_data (pspace);
  if (info->loaded_scripts == NULL)
    init_loaded_scripts_info (info);

  return info;
}

/* Add script NAME in LANGUAGE to hash table of PSPACE_INFO.
   FULL_PATH is NULL if the script wasn't found.  The result is
   true if the script was already in the hash table.  */

int
maybe_add_script (struct auto_load_pspace_info *pspace_info,
		  const char *name, const char *full_path,
		  const struct script_language *language)
{
  struct htab *htab = pspace_info->loaded_scripts;
  struct loaded_script **slot, entry;
  int in_hash_table;

  entry.name = name;
  entry.language = language;
  slot = (struct loaded_script **) htab_find_slot (htab, &entry, INSERT);
  in_hash_table = *slot != NULL;

  /* If this script is not in the hash table, add it.  */

  if (! in_hash_table)
    {
      char *p;

      /* Allocate all space in one chunk so it's easier to free.  */
      *slot = xmalloc (sizeof (**slot)
		       + strlen (name) + 1
		       + (full_path != NULL ? (strlen (full_path) + 1) : 0));
      p = ((char*) *slot) + sizeof (**slot);
      strcpy (p, name);
      (*slot)->name = p;
      if (full_path != NULL)
	{
	  p += strlen (p) + 1;
	  strcpy (p, full_path);
	  (*slot)->full_path = p;
	}
      else
	(*slot)->full_path = NULL;
      (*slot)->language = language;
    }

  return in_hash_table;
}

/* Clear the table of loaded section scripts.  */

static void
clear_section_scripts (void)
{
  struct program_space *pspace = current_program_space;
  struct auto_load_pspace_info *info;

  info = program_space_data (pspace, auto_load_pspace_data);
  if (info != NULL && info->loaded_scripts != NULL)
    {
      htab_delete (info->loaded_scripts);
      info->loaded_scripts = NULL;
      info->script_not_found_warning_printed = FALSE;
    }
}

/* Look for the auto-load script in LANGUAGE associated with OBJFILE and load
   it.  */

void
auto_load_objfile_script (struct objfile *objfile,
			  const struct script_language *language)
{
  char *realname;
  char *filename, *debugfile;
  int len;
  FILE *input;
  struct cleanup *cleanups;

  realname = gdb_realpath (objfile->name);
  len = strlen (realname);
  filename = xmalloc (len + strlen (language->suffix) + 1);
  memcpy (filename, realname, len);
  strcpy (filename + len, language->suffix);

  cleanups = make_cleanup (xfree, filename);
  make_cleanup (xfree, realname);

  input = fopen (filename, "r");
  debugfile = filename;

  if (!input && debug_file_directory)
    {
      /* Also try the same file in the separate debug info directory.  */
      debugfile = xmalloc (strlen (filename)
			   + strlen (debug_file_directory) + 1);
      strcpy (debugfile, debug_file_directory);
      /* FILENAME is absolute, so we don't need a "/" here.  */
      strcat (debugfile, filename);

      make_cleanup (xfree, debugfile);
      input = fopen (debugfile, "r");
    }

  if (!input && gdb_datadir)
    {
      /* Also try the same file in a subdirectory of gdb's data
	 directory.  */
      debugfile = xmalloc (strlen (gdb_datadir) + strlen (filename)
			   + strlen ("/auto-load") + 1);
      strcpy (debugfile, gdb_datadir);
      strcat (debugfile, "/auto-load");
      /* FILENAME is absolute, so we don't need a "/" here.  */
      strcat (debugfile, filename);

      make_cleanup (xfree, debugfile);
      input = fopen (debugfile, "r");
    }

  if (input)
    {
      make_cleanup_fclose (input);

      /* To preserve existing behaviour we don't check for whether the
	 script was already in the table, and always load it.
	 It's highly unlikely that we'd ever load it twice,
	 and these scripts are required to be idempotent under multiple
	 loads anyway.  */
      language->source_script_for_objfile (objfile, input, debugfile);
    }

  do_cleanups (cleanups);
}

/* Load any auto-loaded scripts for OBJFILE.  */

void
load_auto_scripts_for_objfile (struct objfile *objfile)
{
  if (!global_auto_load)
    return;

  if (auto_load_gdb_scripts)
    auto_load_objfile_script (objfile, &script_language_gdb);

  gdbpy_load_auto_scripts_for_objfile (objfile);
}

/* This is a new_objfile observer callback to auto-load scripts.

   Two flavors of auto-loaded scripts are supported.
   1) based on the path to the objfile
   2) from .debug_gdb_scripts section  */

static void
auto_load_new_objfile (struct objfile *objfile)
{
  if (!objfile)
    {
      /* OBJFILE is NULL when loading a new "main" symbol-file.  */
      clear_section_scripts ();
      return;
    }

  load_auto_scripts_for_objfile (objfile);
}

/* Collect scripts to be printed in a vec.  */

typedef struct loaded_script *loaded_script_ptr;
DEF_VEC_P (loaded_script_ptr);

struct collect_matching_scripts_data
{
  VEC (loaded_script_ptr) **scripts_p;

  const struct script_language *language;
};

/* Traversal function for htab_traverse.
   Collect the entry if it matches the regexp.  */

static int
collect_matching_scripts (void **slot, void *info)
{
  struct loaded_script *script = *slot;
  struct collect_matching_scripts_data *data = info;

  if (script->language == data->language && re_exec (script->name))
    VEC_safe_push (loaded_script_ptr, *data->scripts_p, script);

  return 1;
}

/* Print SCRIPT.  */

static void
print_script (struct loaded_script *script)
{
  struct ui_out *uiout = current_uiout;
  struct cleanup *chain;

  chain = make_cleanup_ui_out_tuple_begin_end (uiout, NULL);

  ui_out_field_string (uiout, "loaded", script->full_path ? "Yes" : "Missing");
  ui_out_field_string (uiout, "script", script->name);
  ui_out_text (uiout, "\n");

  /* If the name isn't the full path, print it too.  */
  if (script->full_path != NULL
      && strcmp (script->name, script->full_path) != 0)
    {
      ui_out_text (uiout, "\tfull name: ");
      ui_out_field_string (uiout, "full_path", script->full_path);
      ui_out_text (uiout, "\n");
    }

  do_cleanups (chain);
}

/* Helper for info_auto_load_scripts to sort the scripts by name.  */

static int
sort_scripts_by_name (const void *ap, const void *bp)
{
  const struct loaded_script *a = *(const struct loaded_script **) ap;
  const struct loaded_script *b = *(const struct loaded_script **) bp;

  return FILENAME_CMP (a->name, b->name);
}

/* Special internal GDB value of auto_load_info_scripts's PATTERN identify
   the "info auto-load XXX" command has been executed through the general
   "info auto-load" invocation.  Extra newline will be printed if needed.  */
char auto_load_info_scripts_pattern_nl[] = "";

/* Implementation for "info auto-load gdb-scripts"
   (and "info auto-load python-scripts").  List scripts in LANGUAGE matching
   PATTERN.  FROM_TTY is the usual GDB boolean for user interactivity.  */

void
auto_load_info_scripts (char *pattern, int from_tty,
			const struct script_language *language)
{
  struct ui_out *uiout = current_uiout;
  struct auto_load_pspace_info *pspace_info;
  struct cleanup *script_chain;
  VEC (loaded_script_ptr) *scripts;
  int nr_scripts;

  dont_repeat ();

  pspace_info = get_auto_load_pspace_data (current_program_space);

  if (pattern && *pattern)
    {
      char *re_err = re_comp (pattern);

      if (re_err)
	error (_("Invalid regexp: %s"), re_err);
    }
  else
    {
      re_comp ("");
    }

  /* We need to know the number of rows before we build the table.
     Plus we want to sort the scripts by name.
     So first traverse the hash table collecting the matching scripts.  */

  scripts = VEC_alloc (loaded_script_ptr, 10);
  script_chain = make_cleanup (VEC_cleanup (loaded_script_ptr), &scripts);

  if (pspace_info != NULL && pspace_info->loaded_scripts != NULL)
    {
      struct collect_matching_scripts_data data = { &scripts, language };

      immediate_quit++;
      /* Pass a pointer to scripts as VEC_safe_push can realloc space.  */
      htab_traverse_noresize (pspace_info->loaded_scripts,
			      collect_matching_scripts, &data);
      immediate_quit--;
    }

  nr_scripts = VEC_length (loaded_script_ptr, scripts);

  /* Table header shifted right by preceding "gdb-scripts:  " would not match
     its columns.  */
  if (nr_scripts > 0 && pattern == auto_load_info_scripts_pattern_nl)
    ui_out_text (uiout, "\n");

  make_cleanup_ui_out_table_begin_end (uiout, 2, nr_scripts,
				       "AutoLoadedScriptsTable");

  ui_out_table_header (uiout, 7, ui_left, "loaded", "Loaded");
  ui_out_table_header (uiout, 70, ui_left, "script", "Script");
  ui_out_table_body (uiout);

  if (nr_scripts > 0)
    {
      int i;
      loaded_script_ptr script;

      qsort (VEC_address (loaded_script_ptr, scripts),
	     VEC_length (loaded_script_ptr, scripts),
	     sizeof (loaded_script_ptr), sort_scripts_by_name);
      for (i = 0; VEC_iterate (loaded_script_ptr, scripts, i, script); ++i)
	print_script (script);
    }

  do_cleanups (script_chain);

  if (nr_scripts == 0)
    {
      if (pattern && *pattern)
	ui_out_message (uiout, 0, "No auto-load scripts matching %s.\n",
			pattern);
      else
	ui_out_message (uiout, 0, "No auto-load scripts.\n");
    }
}

/* Wrapper for "info auto-load gdb-scripts".  */

static void
info_auto_load_gdb_scripts (char *pattern, int from_tty)
{
  auto_load_info_scripts (pattern, from_tty, &script_language_gdb);
}

/* Implement 'info auto-load local-gdbinit'.  */

static void
info_auto_load_local_gdbinit (char *args, int from_tty)
{
  if (auto_load_local_gdbinit_pathname == NULL)
    printf_filtered (_("Local .gdbinit file was not found.\n"));
  else if (auto_load_local_gdbinit_loaded)
    printf_filtered (_("Local .gdbinit file \"%s\" has been loaded.\n"),
		     auto_load_local_gdbinit_pathname);
  else
    printf_filtered (_("Local .gdbinit file \"%s\" has not been loaded.\n"),
		     auto_load_local_gdbinit_pathname);
}

/* Return non-zero if SCRIPT_NOT_FOUND_WARNING_PRINTED of PSPACE_INFO was unset
   before calling this function.  Always set SCRIPT_NOT_FOUND_WARNING_PRINTED
   of PSPACE_INFO.  */

int
script_not_found_warning_print (struct auto_load_pspace_info *pspace_info)
{
  int retval = !pspace_info->script_not_found_warning_printed;

  pspace_info->script_not_found_warning_printed = 1;

  return retval;
}

/* The only valid "set auto-load" argument is off|0|no|disable.  */

static void
set_auto_load_cmd (char *args, int from_tty)
{
  struct cmd_list_element *list;
  size_t length;

  /* See parse_binary_operation in use by the sub-commands.  */

  length = args ? strlen (args) : 0;

  while (length > 0 && (args[length - 1] == ' ' || args[length - 1] == '\t'))
    length--;

  if (length == 0 || (strncmp (args, "off", length) != 0
		      && strncmp (args, "0", length) != 0
		      && strncmp (args, "no", length) != 0
		      && strncmp (args, "disable", length) != 0))
    error (_("Valid is only global 'set auto-load no'; "
	     "otherwise check the auto-load sub-commands."));

  for (list = *auto_load_set_cmdlist_get (); list != NULL; list = list->next)
    if (list->var_type == var_boolean)
      {
	gdb_assert (list->type == set_cmd);
	do_setshow_command (args, from_tty, list);
      }
}

/* Initialize "set auto-load " commands prefix and return it.  */

struct cmd_list_element **
auto_load_set_cmdlist_get (void)
{
  static struct cmd_list_element *retval;

  if (retval == NULL)
    add_prefix_cmd ("auto-load", class_maintenance, set_auto_load_cmd, _("\
Auto-loading specific settings.\n\
Configure various auto-load-specific variables such as\n\
automatic loading of Python scripts."),
		    &retval, "set auto-load ",
		    1/*allow-unknown*/, &setlist);

  return &retval;
}

/* Command "show auto-load" displays summary of all the current
   "show auto-load " settings.  */

static void
show_auto_load_cmd (char *args, int from_tty)
{
  cmd_show_list (*auto_load_show_cmdlist_get (), from_tty, "");
}

/* Initialize "show auto-load " commands prefix and return it.  */

struct cmd_list_element **
auto_load_show_cmdlist_get (void)
{
  static struct cmd_list_element *retval;

  if (retval == NULL)
    add_prefix_cmd ("auto-load", class_maintenance, show_auto_load_cmd, _("\
Show auto-loading specific settings.\n\
Show configuration of various auto-load-specific variables such as\n\
automatic loading of Python scripts."),
		    &retval, "show auto-load ",
		    0/*allow-unknown*/, &showlist);

  return &retval;
}

/* Command "info auto-load" displays whether the various auto-load files have
   been loaded.  This is reimplementation of cmd_show_list which inserts
   newlines at proper places.  */

static void
info_auto_load_cmd (char *args, int from_tty)
{
  struct cmd_list_element *list;
  struct cleanup *infolist_chain;
  struct ui_out *uiout = current_uiout;

  infolist_chain = make_cleanup_ui_out_tuple_begin_end (uiout, "infolist");

  for (list = *auto_load_info_cmdlist_get (); list != NULL; list = list->next)
    {
      struct cleanup *option_chain
	= make_cleanup_ui_out_tuple_begin_end (uiout, "option");

      gdb_assert (!list->prefixlist);
      gdb_assert (list->type == not_set_cmd);

      ui_out_field_string (uiout, "name", list->name);
      ui_out_text (uiout, ":  ");
      cmd_func (list, auto_load_info_scripts_pattern_nl, from_tty);

      /* Close the tuple.  */
      do_cleanups (option_chain);
    }

  /* Close the tuple.  */
  do_cleanups (infolist_chain);
}

/* Initialize "info auto-load " commands prefix and return it.  */

struct cmd_list_element **
auto_load_info_cmdlist_get (void)
{
  static struct cmd_list_element *retval;

  if (retval == NULL)
    add_prefix_cmd ("auto-load", class_info, info_auto_load_cmd, _("\
Print current status of auto-loaded files.\n\
Print whether various files like Python scripts or .gdbinit files have been\n\
found and/or loaded."),
		    &retval, "info auto-load ",
		    0/*allow-unknown*/, &infolist);

  return &retval;
}

void _initialize_auto_load (void);

void
_initialize_auto_load (void)
{
  auto_load_pspace_data
    = register_program_space_data_with_cleanup (auto_load_pspace_data_cleanup);

  observer_attach_new_objfile (auto_load_new_objfile);

  add_setshow_boolean_cmd ("gdb-scripts", class_support,
			   &auto_load_gdb_scripts, _("\
Enable or disable auto-loading of canned sequences of commands scripts."), _("\
Show whether auto-loading of canned sequences of commands scripts is enabled."),
			   _("\
If enabled, canned sequences of commands are loaded when the debugger reads\n\
an executable or shared library.\n\
This options has security implications for untrusted inferiors."),
			   NULL, show_auto_load_gdb_scripts,
			   auto_load_set_cmdlist_get (),
			   auto_load_show_cmdlist_get ());

  add_cmd ("gdb-scripts", class_info, info_auto_load_gdb_scripts,
	   _("Print the list of automatically loaded sequences of commands.\n\
Usage: info auto-load gdb-scripts [REGEXP]"),
	   auto_load_info_cmdlist_get ());

  add_setshow_boolean_cmd ("local-gdbinit", class_support,
			   &auto_load_local_gdbinit, _("\
Enable or disable auto-loading of .gdbinit script in current directory."), _("\
Show whether auto-loading .gdbinit script in current directory is enabled."),
			   _("\
If enabled, canned sequences of commands are loaded when debugger starts\n\
from .gdbinit file in current directory.  Such files are deprecated,\n\
use a script associated with inferior executable file instead.\n\
This options has security implications for untrusted inferiors."),
			   NULL, show_auto_load_local_gdbinit,
			   auto_load_set_cmdlist_get (),
			   auto_load_show_cmdlist_get ());

  add_cmd ("local-gdbinit", class_info, info_auto_load_local_gdbinit,
	   _("Print whether current directory .gdbinit file has been loaded.\n\
Usage: info auto-load local-gdbinit"),
	   auto_load_info_cmdlist_get ());
}
