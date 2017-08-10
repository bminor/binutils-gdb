/* Skipping uninteresting files and functions while stepping.

   Copyright (C) 2011-2017 Free Software Foundation, Inc.

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
#include "skip.h"
#include "value.h"
#include "valprint.h"
#include "ui-out.h"
#include "symtab.h"
#include "gdbcmd.h"
#include "command.h"
#include "completer.h"
#include "stack.h"
#include "cli/cli-utils.h"
#include "arch-utils.h"
#include "linespec.h"
#include "objfiles.h"
#include "breakpoint.h" /* for get_sal_arch () */
#include "source.h"
#include "filenames.h"
#include "fnmatch.h"
#include "gdb_regex.h"
#include "common/gdb_optional.h"

struct skiplist_entry
{
  skiplist_entry (bool file_is_glob_, std::string &&file_,
		  bool function_is_regexp_, std::string &&function_)
    : number (-1),
      file_is_glob (file_is_glob_),
      file (std::move (file_)),
      function_is_regexp (function_is_regexp_),
      function (std::move (function_)),
      enabled (true),
      next (NULL)
  {
  }

  int number;

  /* True if FILE is a glob-style pattern.
     Otherwise it is the plain file name (possibly with directories).  */
  bool file_is_glob;

  /* The name of the file or empty if no name.  */
  std::string file;

  /* True if FUNCTION is a regexp.
     Otherwise it is a plain function name (possibly with arguments,
     for C++).  */
  bool function_is_regexp;

  /* The name of the function or empty if no name.  */
  std::string function;

  /* If this is a function regexp, the compiled form.  */
  gdb::optional<compiled_regex> compiled_function_regexp;

  bool enabled;

  struct skiplist_entry *next;
};

static void add_skiplist_entry (std::unique_ptr<skiplist_entry> &&e);

static struct skiplist_entry *skiplist_entry_chain;
static int skiplist_entry_count;

#define ALL_SKIPLIST_ENTRIES(E) \
  for (E = skiplist_entry_chain; E; E = E->next)

#define ALL_SKIPLIST_ENTRIES_SAFE(E,TMP) \
  for (E = skiplist_entry_chain;         \
       E ? (TMP = E->next, 1) : 0;       \
       E = TMP)

/* Create a skip object.  */

static std::unique_ptr<skiplist_entry>
make_skip_entry (bool file_is_glob, std::string &&file,
		 bool function_is_regexp, std::string &&function)
{
  gdb_assert (!file.empty () || !function.empty ());
  if (file_is_glob)
    gdb_assert (!file.empty ());
  if (function_is_regexp)
    gdb_assert (!function.empty ());

  return std::unique_ptr<skiplist_entry>
    (new skiplist_entry (file_is_glob, std::move (file),
			 function_is_regexp, std::move (function)));
}

static void
skip_file_command (char *arg, int from_tty)
{
  struct symtab *symtab;
  const char *filename = NULL;

  /* If no argument was given, try to default to the last
     displayed codepoint.  */
  if (arg == NULL)
    {
      symtab = get_last_displayed_symtab ();
      if (symtab == NULL)
	error (_("No default file now."));

      /* It is not a typo, symtab_to_filename_for_display woule be needlessly
	 ambiguous.  */
      filename = symtab_to_fullname (symtab);
    }
  else
    filename = arg;

  add_skiplist_entry (make_skip_entry (false, std::string (filename), false,
				       std::string ()));

  printf_filtered (_("File %s will be skipped when stepping.\n"), filename);
}

/* Create a skiplist entry for the given function NAME and add it to the
   list.  */

static void
skip_function (const char *name)
{
  add_skiplist_entry (make_skip_entry (false, std::string (), false,
				       std::string (name)));

  printf_filtered (_("Function %s will be skipped when stepping.\n"), name);
}

static void
skip_function_command (char *arg, int from_tty)
{
  /* Default to the current function if no argument is given.  */
  if (arg == NULL)
    {
      const char *name = NULL;
      CORE_ADDR pc;

      if (!last_displayed_sal_is_valid ())
	error (_("No default function now."));

      pc = get_last_displayed_addr ();
      if (!find_pc_partial_function (pc, &name, NULL, NULL))
	{
	  error (_("No function found containing current program point %s."),
		  paddress (get_current_arch (), pc));
	}
      skip_function (name);
      return;
    }

  skip_function (arg);
}

/* Compile the regexp in E.
   An error is thrown if there's an error.
   MESSAGE is used as a prefix of the error message.  */

static void
compile_skip_regexp (struct skiplist_entry *e, const char *message)
{
  int flags = REG_NOSUB;

#ifdef REG_EXTENDED
  flags |= REG_EXTENDED;
#endif

  gdb_assert (e->function_is_regexp && !e->function.empty ());
  e->compiled_function_regexp.emplace (e->function.c_str (), flags, message);
}

/* Process "skip ..." that does not match "skip file" or "skip function".  */

static void
skip_command (char *arg, int from_tty)
{
  const char *file = NULL;
  const char *gfile = NULL;
  const char *function = NULL;
  const char *rfunction = NULL;
  int i;

  if (arg == NULL)
    {
      skip_function_command (arg, from_tty);
      return;
    }

  gdb_argv argv (arg);

  for (i = 0; argv[i] != NULL; ++i)
    {
      const char *p = argv[i];
      const char *value = argv[i + 1];

      if (strcmp (p, "-fi") == 0
	  || strcmp (p, "-file") == 0)
	{
	  if (value == NULL)
	    error (_("Missing value for %s option."), p);
	  file = value;
	  ++i;
	}
      else if (strcmp (p, "-gfi") == 0
	       || strcmp (p, "-gfile") == 0)
	{
	  if (value == NULL)
	    error (_("Missing value for %s option."), p);
	  gfile = value;
	  ++i;
	}
      else if (strcmp (p, "-fu") == 0
	       || strcmp (p, "-function") == 0)
	{
	  if (value == NULL)
	    error (_("Missing value for %s option."), p);
	  function = value;
	  ++i;
	}
      else if (strcmp (p, "-rfu") == 0
	       || strcmp (p, "-rfunction") == 0)
	{
	  if (value == NULL)
	    error (_("Missing value for %s option."), p);
	  rfunction = value;
	  ++i;
	}
      else if (*p == '-')
	error (_("Invalid skip option: %s"), p);
      else if (i == 0)
	{
	  /* Assume the user entered "skip FUNCTION-NAME".
	     FUNCTION-NAME may be `foo (int)', and therefore we pass the
	     complete original arg to skip_function command as if the user
	     typed "skip function arg".  */
	  skip_function_command (arg, from_tty);
	  return;
	}
      else
	error (_("Invalid argument: %s"), p);
    }

  if (file != NULL && gfile != NULL)
    error (_("Cannot specify both -file and -gfile."));

  if (function != NULL && rfunction != NULL)
    error (_("Cannot specify both -function and -rfunction."));

  /* This shouldn't happen as "skip" by itself gets punted to
     skip_function_command.  */
  gdb_assert (file != NULL || gfile != NULL
	      || function != NULL || rfunction != NULL);

  std::string entry_file;
  if (file != NULL)
    entry_file = file;
  else if (gfile != NULL)
    entry_file = gfile;
  std::string entry_function;
  if (function != NULL)
    entry_function = function;
  else if (rfunction != NULL)
    entry_function = rfunction;
  std::unique_ptr<skiplist_entry> e
    = make_skip_entry (gfile != NULL, std::move (entry_file),
		       rfunction != NULL, std::move (entry_function));
  if (rfunction != NULL)
    compile_skip_regexp (e.get (), _("regexp"));

  add_skiplist_entry (std::move (e));

  /* I18N concerns drive some of the choices here (we can't piece together
     the output too much).  OTOH we want to keep this simple.  Therefore the
     only polish we add to the output is to append "(s)" to "File" or
     "Function" if they're a glob/regexp.  */
  {
    const char *file_to_print = file != NULL ? file : gfile;
    const char *function_to_print = function != NULL ? function : rfunction;
    const char *file_text = gfile != NULL ? _("File(s)") : _("File");
    const char *lower_file_text = gfile != NULL ? _("file(s)") : _("file");
    const char *function_text
      = rfunction != NULL ? _("Function(s)") : _("Function");

    if (function_to_print == NULL)
      {
	printf_filtered (_("%s %s will be skipped when stepping.\n"),
			 file_text, file_to_print);
      }
    else if (file_to_print == NULL)
      {
	printf_filtered (_("%s %s will be skipped when stepping.\n"),
			 function_text, function_to_print);
      }
    else
      {
	printf_filtered (_("%s %s in %s %s will be skipped"
			   " when stepping.\n"),
			 function_text, function_to_print,
			 lower_file_text, file_to_print);
      }
  }
}

static void
skip_info (char *arg, int from_tty)
{
  struct skiplist_entry *e;
  int num_printable_entries = 0;
  struct value_print_options opts;

  get_user_print_options (&opts);

  /* Count the number of rows in the table and see if we need space for a
     64-bit address anywhere.  */
  ALL_SKIPLIST_ENTRIES (e)
    if (arg == NULL || number_is_in_list (arg, e->number))
      num_printable_entries++;

  if (num_printable_entries == 0)
    {
      if (arg == NULL)
	current_uiout->message (_("Not skipping any files or functions.\n"));
      else
	current_uiout->message (
	  _("No skiplist entries found with number %s.\n"), arg);

      return;
    }

  ui_out_emit_table table_emitter (current_uiout, 6, num_printable_entries,
				   "SkiplistTable");

  current_uiout->table_header (5, ui_left, "number", "Num");   /* 1 */
  current_uiout->table_header (3, ui_left, "enabled", "Enb");  /* 2 */
  current_uiout->table_header (4, ui_right, "regexp", "Glob"); /* 3 */
  current_uiout->table_header (20, ui_left, "file", "File");   /* 4 */
  current_uiout->table_header (2, ui_right, "regexp", "RE");   /* 5 */
  current_uiout->table_header (40, ui_noalign, "function", "Function"); /* 6 */
  current_uiout->table_body ();

  ALL_SKIPLIST_ENTRIES (e)
    {

      QUIT;
      if (arg != NULL && !number_is_in_list (arg, e->number))
	continue;

      ui_out_emit_tuple tuple_emitter (current_uiout, "blklst-entry");
      current_uiout->field_int ("number", e->number); /* 1 */

      if (e->enabled)
	current_uiout->field_string ("enabled", "y"); /* 2 */
      else
	current_uiout->field_string ("enabled", "n"); /* 2 */

      if (e->file_is_glob)
	current_uiout->field_string ("regexp", "y"); /* 3 */
      else
	current_uiout->field_string ("regexp", "n"); /* 3 */

      current_uiout->field_string ("file",
				   e->file.empty () ? "<none>"
				   : e->file.c_str ()); /* 4 */
      if (e->function_is_regexp)
	current_uiout->field_string ("regexp", "y"); /* 5 */
      else
	current_uiout->field_string ("regexp", "n"); /* 5 */

      current_uiout->field_string ("function",
				   e->function.empty () ? "<none>"
				   : e->function.c_str ()); /* 6 */

      current_uiout->text ("\n");
    }
}

static void
skip_enable_command (char *arg, int from_tty)
{
  struct skiplist_entry *e;
  int found = 0;

  ALL_SKIPLIST_ENTRIES (e)
    if (arg == NULL || number_is_in_list (arg, e->number))
      {
        e->enabled = true;
        found = 1;
      }

  if (!found)
    error (_("No skiplist entries found with number %s."), arg);
}

static void
skip_disable_command (char *arg, int from_tty)
{
  struct skiplist_entry *e;
  int found = 0;

  ALL_SKIPLIST_ENTRIES (e)
    if (arg == NULL || number_is_in_list (arg, e->number))
      {
	e->enabled = false;
        found = 1;
      }

  if (!found)
    error (_("No skiplist entries found with number %s."), arg);
}

static void
skip_delete_command (char *arg, int from_tty)
{
  struct skiplist_entry *e, *temp, *b_prev;
  int found = 0;

  b_prev = 0;
  ALL_SKIPLIST_ENTRIES_SAFE (e, temp)
    if (arg == NULL || number_is_in_list (arg, e->number))
      {
	if (b_prev != NULL)
	  b_prev->next = e->next;
	else
	  skiplist_entry_chain = e->next;

	delete e;
        found = 1;
      }
    else
      {
	b_prev = e;
      }

  if (!found)
    error (_("No skiplist entries found with number %s."), arg);
}

/* Add the given skiplist entry to our list, and set the entry's number.  */

static void
add_skiplist_entry (std::unique_ptr<skiplist_entry> &&e)
{
  struct skiplist_entry *e1;

  e->number = ++skiplist_entry_count;

  /* Add to the end of the chain so that the list of
     skiplist entries will be in numerical order.  */

  e1 = skiplist_entry_chain;
  if (e1 == NULL)
    skiplist_entry_chain = e.release ();
  else
    {
      while (e1->next)
	e1 = e1->next;
      e1->next = e.release ();
    }
}

/* Return non-zero if we're stopped at a file to be skipped.  */

static int
skip_file_p (struct skiplist_entry *e,
	     const struct symtab_and_line *function_sal)
{
  gdb_assert (!e->file.empty () && !e->file_is_glob);

  if (function_sal->symtab == NULL)
    return 0;

  /* Check first sole SYMTAB->FILENAME.  It may not be a substring of
     symtab_to_fullname as it may contain "./" etc.  */
  if (compare_filenames_for_search (function_sal->symtab->filename,
				    e->file.c_str ()))
    return 1;

  /* Before we invoke realpath, which can get expensive when many
     files are involved, do a quick comparison of the basenames.  */
  if (!basenames_may_differ
      && filename_cmp (lbasename (function_sal->symtab->filename),
		       lbasename (e->file.c_str ())) != 0)
    return 0;

  /* Note: symtab_to_fullname caches its result, thus we don't have to.  */
  {
    const char *fullname = symtab_to_fullname (function_sal->symtab);

    if (compare_filenames_for_search (fullname, e->file.c_str ()))
      return 1;
  }

  return 0;
}

/* Return non-zero if we're stopped at a globbed file to be skipped.  */

static int
skip_gfile_p (struct skiplist_entry *e,
	      const struct symtab_and_line *function_sal)
{
  gdb_assert (!e->file.empty () && e->file_is_glob);

  if (function_sal->symtab == NULL)
    return 0;

  /* Check first sole SYMTAB->FILENAME.  It may not be a substring of
     symtab_to_fullname as it may contain "./" etc.  */
  if (gdb_filename_fnmatch (e->file.c_str (), function_sal->symtab->filename,
			    FNM_FILE_NAME | FNM_NOESCAPE) == 0)
    return 1;

  /* Before we invoke symtab_to_fullname, which is expensive, do a quick
     comparison of the basenames.
     Note that we assume that lbasename works with glob-style patterns.
     If the basename of the glob pattern is something like "*.c" then this
     isn't much of a win.  Oh well.  */
  if (!basenames_may_differ
      && gdb_filename_fnmatch (lbasename (e->file.c_str ()),
			       lbasename (function_sal->symtab->filename),
			       FNM_FILE_NAME | FNM_NOESCAPE) != 0)
    return 0;

  /* Note: symtab_to_fullname caches its result, thus we don't have to.  */
  {
    const char *fullname = symtab_to_fullname (function_sal->symtab);

    if (compare_glob_filenames_for_search (fullname, e->file.c_str ()))
      return 1;
  }

  return 0;
}

/* Return non-zero if we're stopped at a function to be skipped.  */

static int
skip_function_p (struct skiplist_entry *e, const char *function_name)
{
  gdb_assert (!e->function.empty () && !e->function_is_regexp);
  return strcmp_iw (function_name, e->function.c_str ()) == 0;
}

/* Return non-zero if we're stopped at a function regexp to be skipped.  */

static int
skip_rfunction_p (struct skiplist_entry *e, const char *function_name)
{
  gdb_assert (!e->function.empty () && e->function_is_regexp
	      && e->compiled_function_regexp);
  return (e->compiled_function_regexp->exec (function_name, 0, NULL, 0)
	  == 0);
}

/* See skip.h.  */

int
function_name_is_marked_for_skip (const char *function_name,
				  const struct symtab_and_line *function_sal)
{
  struct skiplist_entry *e;

  if (function_name == NULL)
    return 0;

  ALL_SKIPLIST_ENTRIES (e)
    {
      int skip_by_file = 0;
      int skip_by_function = 0;

      if (!e->enabled)
	continue;

      if (!e->file.empty ())
	{
	  if (e->file_is_glob)
	    {
	      if (skip_gfile_p (e, function_sal))
		skip_by_file = 1;
	    }
	  else
	    {
	      if (skip_file_p (e, function_sal))
		skip_by_file = 1;
	    }
	}
      if (!e->function.empty ())
	{
	  if (e->function_is_regexp)
	    {
	      if (skip_rfunction_p (e, function_name))
		skip_by_function = 1;
	    }
	  else
	    {
	      if (skip_function_p (e, function_name))
		skip_by_function = 1;
	    }
	}

      /* If both file and function must match, make sure we don't errantly
	 exit if only one of them match.  */
      if (!e->file.empty () && !e->function.empty ())
	{
	  if (skip_by_file && skip_by_function)
	    return 1;
	}
      /* Only one of file/function is specified.  */
      else if (skip_by_file || skip_by_function)
	return 1;
    }

  return 0;
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_step_skip;

void
_initialize_step_skip (void)
{
  static struct cmd_list_element *skiplist = NULL;
  struct cmd_list_element *c;

  skiplist_entry_chain = 0;
  skiplist_entry_count = 0;

  add_prefix_cmd ("skip", class_breakpoint, skip_command, _("\
Ignore a function while stepping.\n\
\n\
Usage: skip [FUNCTION-NAME]\n\
       skip [<file-spec>] [<function-spec>]\n\
If no arguments are given, ignore the current function.\n\
\n\
<file-spec> is one of:\n\
       -fi|-file FILE-NAME\n\
       -gfi|-gfile GLOB-FILE-PATTERN\n\
<function-spec> is one of:\n\
       -fu|-function FUNCTION-NAME\n\
       -rfu|-rfunction FUNCTION-NAME-REGULAR-EXPRESSION"),
                  &skiplist, "skip ", 1, &cmdlist);

  c = add_cmd ("file", class_breakpoint, skip_file_command, _("\
Ignore a file while stepping.\n\
Usage: skip file [FILE-NAME]\n\
If no filename is given, ignore the current file."),
	       &skiplist);
  set_cmd_completer (c, filename_completer);

  c = add_cmd ("function", class_breakpoint, skip_function_command, _("\
Ignore a function while stepping.\n\
Usage: skip function [FUNCTION-NAME]\n\
If no function name is given, skip the current function."),
	       &skiplist);
  set_cmd_completer (c, location_completer);

  add_cmd ("enable", class_breakpoint, skip_enable_command, _("\
Enable skip entries.  You can specify numbers (e.g. \"skip enable 1 3\"), \
ranges (e.g. \"skip enable 4-8\"), or both (e.g. \"skip enable 1 3 4-8\").\n\n\
If you don't specify any numbers or ranges, we'll enable all skip entries.\n\n\
Usage: skip enable [NUMBERS AND/OR RANGES]"),
	   &skiplist);

  add_cmd ("disable", class_breakpoint, skip_disable_command, _("\
Disable skip entries.  You can specify numbers (e.g. \"skip disable 1 3\"), \
ranges (e.g. \"skip disable 4-8\"), or both (e.g. \"skip disable 1 3 4-8\").\n\n\
If you don't specify any numbers or ranges, we'll disable all skip entries.\n\n\
Usage: skip disable [NUMBERS AND/OR RANGES]"),
	   &skiplist);

  add_cmd ("delete", class_breakpoint, skip_delete_command, _("\
Delete skip entries.  You can specify numbers (e.g. \"skip delete 1 3\"), \
ranges (e.g. \"skip delete 4-8\"), or both (e.g. \"skip delete 1 3 4-8\").\n\n\
If you don't specify any numbers or ranges, we'll delete all skip entries.\n\n\
Usage: skip delete [NUMBERS AND/OR RANGES]"),
           &skiplist);

  add_info ("skip", skip_info, _("\
Display the status of skips.  You can specify numbers (e.g. \"skip info 1 3\"), \
ranges (e.g. \"skip info 4-8\"), or both (e.g. \"skip info 1 3 4-8\").\n\n\
If you don't specify any numbers or ranges, we'll show all skips.\n\n\
Usage: skip info [NUMBERS AND/OR RANGES]\n\
The \"Type\" column indicates one of:\n\
\tfile        - ignored file\n\
\tfunction    - ignored function"));
}
