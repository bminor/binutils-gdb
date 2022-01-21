/* debuginfod utilities for GDB.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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
#include <errno.h>
#include "gdbsupport/scoped_fd.h"
#include "debuginfod-support.h"
#include "gdbsupport/gdb_optional.h"
#include "cli/cli-cmds.h"
#include "cli/cli-style.h"
#include "cli-out.h"
#include "target.h"

/* Set/show debuginfod commands.  */
static cmd_list_element *set_debuginfod_prefix_list;
static cmd_list_element *show_debuginfod_prefix_list;

static const char debuginfod_on[] = "on";
static const char debuginfod_off[] = "off";
static const char debuginfod_ask[] = "ask";

static const char *debuginfod_enabled_enum[] =
{
  debuginfod_on,
  debuginfod_off,
  debuginfod_ask,
  nullptr
};

static const char *debuginfod_enabled =
#if defined(HAVE_LIBDEBUGINFOD)
  debuginfod_ask;
#else
  debuginfod_off;
#endif

static unsigned int debuginfod_verbose = 1;

#ifndef HAVE_LIBDEBUGINFOD
scoped_fd
debuginfod_source_query (const unsigned char *build_id,
			 int build_id_len,
			 const char *srcpath,
			 gdb::unique_xmalloc_ptr<char> *destname)
{
  return scoped_fd (-ENOSYS);
}

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id,
			    int build_id_len,
			    const char *filename,
			    gdb::unique_xmalloc_ptr<char> *destname)
{
  return scoped_fd (-ENOSYS);
}

#define NO_IMPL _("Support for debuginfod is not compiled into GDB.")

#else
#include <elfutils/debuginfod.h>

struct user_data
{
  user_data (const char *desc, string_file &styled_fname)
    : desc (desc), styled_fname (styled_fname)
  { }

  const char * const desc;
  string_file & styled_fname;
  gdb::optional<ui_out::progress_update> progress;
};

/* Deleter for a debuginfod_client.  */

struct debuginfod_client_deleter
{
  void operator() (debuginfod_client *c)
  {
    debuginfod_end (c);
  }
};

using debuginfod_client_up
  = std::unique_ptr<debuginfod_client, debuginfod_client_deleter>;

/* Convert SIZE into a unit suitable for use with progress updates.
   SIZE should in given in bytes and will be converted into KB or MB.
   UNIT will be set to "KB" or "MB" accordingly.  */

static void
get_size_and_unit (double *size, const char **unit)
{
  *size /= 1024;

  /* If size is greater than 0.01 MB then set unit to MB.  */
  if (*size > 10.24)
    {
      *size /= 1024;
      *unit = "MB";
    }
  else
    *unit = "KB";
}

static int
progressfn (debuginfod_client *c, long cur, long total)
{
  user_data *data = static_cast<user_data *> (debuginfod_get_user_data (c));
  gdb_assert (data != nullptr);

  if (check_quit_flag ())
    {
      if (data->progress.has_value ())
	data->progress.reset ();

      printf_filtered ("Cancelled download of %s %s\n",
		       data->desc, data->styled_fname.c_str ());
      return 1;
    }

  if (debuginfod_verbose == 0)
    return 0;

  /* Print progress update.  Try to include the transfer if possible.  */
  if (total > 0)
    {
      /* Transfer size is known.  */
      if (!data->progress.has_value ()
	  || data->progress->get_state () != ui_out::progress_update::PERCENT)
	{
	  double size = (double)total;
	  const char *unit = "";

	  get_size_and_unit (&size, &unit);
	  std::string message = string_printf ("Downloading %.2f %s %s %s",
					       size, unit, data->desc,
					       data->styled_fname.c_str ());

	  if (!data->progress.has_value ())
	    data->progress.emplace (current_uiout, message, 1);
	  else
	    data->progress->update_name (message);
	}

	double percent = (double)cur / (double)total;
	if (percent >= 0.0 || percent <= 100.0)
	  {
	    current_uiout->update_progress_percent (percent);
	    return 0;
	  }
    }

  if (!data->progress.has_value ()
      || data->progress->get_state () != ui_out::progress_update::SPIN)
    {
      std::string message = string_printf ("Downloading %s %s", data->desc,
					   data->styled_fname.c_str ());
      data->progress.emplace (current_uiout, message, 1);
    }

  current_uiout->update_progress_spin ();
  return 0;
}

static debuginfod_client *
get_debuginfod_client ()
{
  static debuginfod_client_up global_client;

  if (global_client == nullptr)
    {
      global_client.reset (debuginfod_begin ());

      if (global_client != nullptr)
	debuginfod_set_progressfn (global_client.get (), progressfn);
    }

  return global_client.get ();
}

/* Check if debuginfod is enabled.  If configured to do so, ask the user
   whether to enable debuginfod.  */

static bool
debuginfod_is_enabled ()
{
  const char *urls = getenv (DEBUGINFOD_URLS_ENV_VAR);

  if (urls == nullptr || urls[0] == '\0'
      || debuginfod_enabled == debuginfod_off)
    return false;

  if (debuginfod_enabled == debuginfod_ask)
    {
      int resp = nquery (_("\nThis GDB supports auto-downloading debuginfo " \
			   "from the following URLs:\n%s\nEnable debuginfod " \
			   "for this session? "),
			 urls);
      if (!resp)
	{
	  printf_filtered (_("Debuginfod has been disabled.\nTo make this " \
			     "setting permanent, add \'set debuginfod " \
			     "enabled off\' to .gdbinit.\n"));
	  debuginfod_enabled = debuginfod_off;
	  return false;
	}

      printf_filtered (_("Debuginfod has been enabled.\nTo make this " \
			 "setting permanent, add \'set debuginfod enabled " \
			 "on\' to .gdbinit.\n"));
      debuginfod_enabled = debuginfod_on;
    }

  return true;
}

static void
print_outcome (user_data &data, int fd)
{
  /* Clears the current line of progress output.  */
  if (data.progress.has_value ())
    data.progress.reset ();

  if (debuginfod_verbose > 1 && fd >= 0)
    {
      struct stat s;

      if (fstat (fd, &s) == 0)
	{
	  double size = (double)s.st_size;
	  const char *unit = "";

	  get_size_and_unit (&size, &unit);
	  printf_filtered (_("Retrieved %.02f %s %s %s\n"), size, unit,
			   data.desc, data.styled_fname.c_str ());
	}
      else
	warning (_("Retrieved %s %s but size cannot be read: %s\n"),
		 data.desc, data.styled_fname.c_str (),
		 safe_strerror (errno));
    }
  else if (debuginfod_verbose > 0 && fd < 0 && fd != -ENOENT)
    printf_filtered (_("Download failed: %s. " \
		       "Continuing without %s %s.\n"),
		     safe_strerror (-fd), data.desc,
		     data.styled_fname.c_str ());
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_source_query (const unsigned char *build_id,
			 int build_id_len,
			 const char *srcpath,
			 gdb::unique_xmalloc_ptr<char> *destname)
{
  if (!debuginfod_is_enabled ())
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = get_debuginfod_client ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  char *dname = nullptr;
  string_file styled_srcpath (current_uiout->can_emit_style_escape ());
  fprintf_styled (&styled_srcpath, file_name_style.style (), "%s", srcpath);
  user_data data ("source file", styled_srcpath);

  debuginfod_set_user_data (c, &data);
  gdb::optional<target_terminal::scoped_restore_terminal_state> term_state;
  if (target_supports_terminal_ours ())
    {
      term_state.emplace ();
      target_terminal::ours ();
    }

  scoped_fd fd (debuginfod_find_source (c,
					build_id,
					build_id_len,
					srcpath,
					&dname));
  debuginfod_set_user_data (c, nullptr);
  print_outcome (data, fd.get ());

  if (fd.get () >= 0)
    destname->reset (dname);

  return fd;
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id,
			    int build_id_len,
			    const char *filename,
			    gdb::unique_xmalloc_ptr<char> *destname)
{
  if (!debuginfod_is_enabled ())
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = get_debuginfod_client ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  char *dname = nullptr;
  string_file styled_filename (current_uiout->can_emit_style_escape ());
  fprintf_styled (&styled_filename, file_name_style.style (), "%s", filename);
  user_data data ("separate debug info for", styled_filename);

  debuginfod_set_user_data (c, &data);
  gdb::optional<target_terminal::scoped_restore_terminal_state> term_state;
  if (target_supports_terminal_ours ())
    {
      term_state.emplace ();
      target_terminal::ours ();
    }

  scoped_fd fd (debuginfod_find_debuginfo (c, build_id, build_id_len,
					   &dname));
  debuginfod_set_user_data (c, nullptr);
  print_outcome (data, fd.get ());

  if (fd.get () >= 0)
    destname->reset (dname);

  return fd;
}
#endif

/* Set callback for "set debuginfod enabled".  */

static void
set_debuginfod_enabled (const char *value)
{
#if defined(HAVE_LIBDEBUGINFOD)
  debuginfod_enabled = value;
#else
  error (NO_IMPL);
#endif
}

/* Get callback for "set debuginfod enabled".  */

static const char *
get_debuginfod_enabled ()
{
  return debuginfod_enabled;
}

/* Show callback for "set debuginfod enabled".  */

static void
show_debuginfod_enabled (ui_file *file, int from_tty, cmd_list_element *cmd,
			 const char *value)
{
  fprintf_filtered (file,
		    _("Debuginfod functionality is currently set to "
		      "\"%s\".\n"), debuginfod_enabled);
}

/* Set callback for "set debuginfod urls".  */

static void
set_debuginfod_urls (const std::string &urls)
{
#if defined(HAVE_LIBDEBUGINFOD)
  if (setenv (DEBUGINFOD_URLS_ENV_VAR, urls.c_str (), 1) != 0)
    warning (_("Unable to set debuginfod URLs: %s"), safe_strerror (errno));
#else
  error (NO_IMPL);
#endif
}

/* Get callback for "set debuginfod urls".  */

static const std::string&
get_debuginfod_urls ()
{
  static std::string urls;
#if defined(HAVE_LIBDEBUGINFOD)
  const char *envvar = getenv (DEBUGINFOD_URLS_ENV_VAR);

  if (envvar != nullptr)
    urls = envvar;
  else
    urls.clear ();
#endif

  return urls;
}

/* Show callback for "set debuginfod urls".  */

static void
show_debuginfod_urls (ui_file *file, int from_tty, cmd_list_element *cmd,
		      const char *value)
{
  if (value[0] == '\0')
    fprintf_filtered (file, _("Debuginfod URLs have not been set.\n"));
  else
    fprintf_filtered (file, _("Debuginfod URLs are currently set to:\n%s\n"),
		      value);
}

/* Show callback for "set debuginfod verbose".  */

static void
show_debuginfod_verbose_command (ui_file *file, int from_tty,
				 cmd_list_element *cmd, const char *value)
{
  fprintf_filtered (file, _("Debuginfod verbose output is set to %s.\n"),
		    value);
}

/* Register debuginfod commands.  */

void _initialize_debuginfod ();
void
_initialize_debuginfod ()
{
  /* set/show debuginfod */
  add_setshow_prefix_cmd ("debuginfod", class_run,
			  _("Set debuginfod options."),
			  _("Show debuginfod options."),
			  &set_debuginfod_prefix_list,
			  &show_debuginfod_prefix_list,
			  &setlist, &showlist);

  add_setshow_enum_cmd ("enabled", class_run, debuginfod_enabled_enum,
			_("Set whether to use debuginfod."),
			_("Show whether to use debuginfod."),
			_("\
When on, enable the use of debuginfod to download missing debug info and\n\
source files."),
			set_debuginfod_enabled,
			get_debuginfod_enabled,
			show_debuginfod_enabled,
			&set_debuginfod_prefix_list,
			&show_debuginfod_prefix_list);

  /* set/show debuginfod urls */
  add_setshow_string_noescape_cmd ("urls", class_run, _("\
Set the list of debuginfod server URLs."), _("\
Show the list of debuginfod server URLs."), _("\
Manage the space-separated list of debuginfod server URLs that GDB will query \
when missing debuginfo, executables or source files.\nThe default value is \
copied from the DEBUGINFOD_URLS environment variable."),
				   set_debuginfod_urls,
				   get_debuginfod_urls,
				   show_debuginfod_urls,
				   &set_debuginfod_prefix_list,
				   &show_debuginfod_prefix_list);

  /* set/show debuginfod verbose */
  add_setshow_zuinteger_cmd ("verbose", class_support,
			     &debuginfod_verbose, _("\
Set verbosity of debuginfod output."), _("\
Show debuginfod debugging."), _("\
When set to a non-zero value, display verbose output for each debuginfod \
query.\nTo disable, set to zero.  Verbose output is displayed by default."),
			     nullptr,
			     show_debuginfod_verbose_command,
			     &set_debuginfod_prefix_list,
			     &show_debuginfod_prefix_list);
}
