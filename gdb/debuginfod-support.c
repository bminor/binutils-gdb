/* debuginfod utilities for GDB.
   Copyright (C) 2020-2021 Free Software Foundation, Inc.

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

/* Set/show debuginfod commands.  */
static cmd_list_element *set_debuginfod_prefix_list;
static cmd_list_element *show_debuginfod_prefix_list;

static const char debuginfod_on[] = "on";
static const char debuginfod_off[] = "off";
static const char debuginfod_ask[] = "ask";

static const char *debuginfod_enable = debuginfod_ask;
static unsigned debuginfod_verbose = 1;

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

/* Stub set/show commands that indicate debuginfod is not supported.  */

static void
set_debuginfod_on_command (const char *args, int from_tty)
{
  error (NO_IMPL);
  debuginfod_enable = debuginfod_off;
}

static void
set_debuginfod_off_command (const char *args, int from_tty)
{
  error (NO_IMPL);
  debuginfod_enable = debuginfod_off;
}

static void
set_debuginfod_ask_command (const char *args, int from_tty)
{
  error (NO_IMPL);
  debuginfod_enable = debuginfod_off;
}

static void
show_debuginfod_status_command (const char *args, int from_tty)
{
  error (NO_IMPL);
}

static void
set_debuginfod_urls_command (const std::string& urls)
{
  error (NO_IMPL);
}

static const std::string&
get_debuginfod_urls_command ()
{
  static std::string empty;
  return empty;
}

static void
show_debuginfod_urls_command (struct ui_file *file, int from_tty,
			      struct cmd_list_element *cmd, const char *value)
{
  error (NO_IMPL);
}

static void
set_debuginfod_verbose_command (const char *args, int from_tty,
				struct cmd_list_element *c)
{
  error (NO_IMPL);
  debuginfod_verbose = 0;
}

static void
show_debuginfod_verbose_command (struct ui_file *file, int from_tty,
				 struct cmd_list_element *cmd,
				 const char *value)
{
  error (NO_IMPL);
}
#else
#include <elfutils/debuginfod.h>

struct user_data
{
  user_data (const char *desc, const char *fname)
    : desc (desc), fname (fname)
  { }

  const char * const desc;
  const char * const fname;
  gdb::optional<ui_out::progress_meter> meter;
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

/* Enable debuginfod.  */

static void
set_debuginfod_on_command (const char *args, int from_tty)
{
  debuginfod_enable = debuginfod_on;
}

/* Disable debuginfod.  */

static void
set_debuginfod_off_command (const char *args, int from_tty)
{
  debuginfod_enable = debuginfod_off;
}

/* Before next query, ask user whether to enable debuginfod.  */

static void
set_debuginfod_ask_command (const char *args, int from_tty)
{
  debuginfod_enable = debuginfod_ask;
}

/* Show whether debuginfod is enabled.  */

static void
show_debuginfod_status_command (const char *args, int from_tty)
{
  printf_unfiltered (_("Debuginfod functionality is currently set to " \
		     "\"%s\".\n"), debuginfod_enable);
}

/* Set the URLs that debuginfod will query.  */

static void
set_debuginfod_urls_command (const std::string& urls)
{
  if (setenv ("DEBUGINFOD_URLS", urls.c_str (), 1) != 0)
    warning (_("Unable to set debuginfod URLs: %s"), safe_strerror (errno));
}

/* Get current debuginfod URLs.  */

static const std::string&
get_debuginfod_urls_command ()
{
  static std::string urls;
  const char *envvar = getenv (DEBUGINFOD_URLS_ENV_VAR);

  if (envvar != nullptr)
    urls = envvar;
  else
    urls.clear ();

  return urls;
}

/* Show the URLs that debuginfod will query.  */

static void
show_debuginfod_urls_command (struct ui_file *file, int from_tty,
			      struct cmd_list_element *cmd, const char *value)
{
  if (value == nullptr || value[0] == '\0')
    fprintf_unfiltered (file, _("Debuginfod URLs have not been set.\n"));
  else
    fprintf_filtered (file, _("Debuginfod URLs are currently set to:\n%s\n"),
		      value);
}

/* No-op setter used for compatibility when gdb is built without debuginfod.  */

static void
set_debuginfod_verbose_command (const char *args, int from_tty,
				struct cmd_list_element *c)
{
  return;
}

/* Show verbosity.  */

static void
show_debuginfod_verbose_command (struct ui_file *file, int from_tty,
				 struct cmd_list_element *cmd, const char *value)
{
  fprintf_filtered (file, _("Debuginfod verbose output is set to %s.\n"),
		    value);
}

static int
progressfn (debuginfod_client *c, long cur, long total)
{
  user_data *data = static_cast<user_data *> (debuginfod_get_user_data (c));
  gdb_assert (data != nullptr);

  if (check_quit_flag ())
    {
      printf_filtered ("Cancelling download of %s %ps...\n",
		       data->desc,
		       styled_string (file_name_style.style (), data->fname));
      return 1;
    }

  if (total == 0)
    return 0;

  if (!data->meter.has_value ())
    {
      float size_in_mb = 1.0f * total / (1024 * 1024);
      string_file styled_filename (current_uiout->can_emit_style_escape ());
      fprintf_styled (&styled_filename,
		      file_name_style.style (),
		      "%s",
		      data->fname);
      std::string message
	= string_printf ("Downloading %.2f MB %s %s", size_in_mb, data->desc,
			 styled_filename.c_str());
      data->meter.emplace (current_uiout, message, 1);
    }

  current_uiout->progress ((double)cur / (double)total);

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
debuginfod_enabled ()
{
  const char *urls = getenv (DEBUGINFOD_URLS_ENV_VAR);

  if (urls == nullptr || urls[0] == '\0'
      || debuginfod_enable == debuginfod_off)
    return false;

  if (debuginfod_enable == debuginfod_ask)
    {
      int resp = nquery (_("\nThis GDB supports auto-downloading debuginfo " \
			   "from the following URLs:\n%s\nEnable debuginfod " \
			   "for this session? "),
			 urls);
      if (!resp)
	{
	  printf_filtered (_("Debuginfod has been disabled.\nTo make this " \
			     "setting permanent, add \'set debuginfod off\' " \
			     "to .gdbinit.\n"));
	  debuginfod_enable = debuginfod_off;
	  return false;
	}

      printf_filtered (_("Debuginfod has been enabled.\nTo make this " \
			 "setting permanent, add \'set debuginfod on\' " \
			 "to .gdbinit.\n"));
      debuginfod_enable = debuginfod_on;
    }

  return true;
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_source_query (const unsigned char *build_id,
			 int build_id_len,
			 const char *srcpath,
			 gdb::unique_xmalloc_ptr<char> *destname)
{
  if (!debuginfod_enabled ())
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = get_debuginfod_client ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  user_data data ("source file", srcpath);

  debuginfod_set_user_data (c, &data);
  scoped_fd fd (debuginfod_find_source (c,
					build_id,
					build_id_len,
					srcpath,
					nullptr));
  debuginfod_set_user_data (c, nullptr);

  if (debuginfod_verbose > 0 && fd.get () < 0 && fd.get () != -ENOENT)
    printf_filtered (_("Download failed: %s.  Continuing without source file %ps.\n"),
		     safe_strerror (-fd.get ()),
		     styled_string (file_name_style.style (),  srcpath));

  if (fd.get () >= 0)
    *destname = make_unique_xstrdup (srcpath);

  return fd;
}

/* See debuginfod-support.h  */

scoped_fd
debuginfod_debuginfo_query (const unsigned char *build_id,
			    int build_id_len,
			    const char *filename,
			    gdb::unique_xmalloc_ptr<char> *destname)
{
  if (!debuginfod_enabled ())
    return scoped_fd (-ENOSYS);

  debuginfod_client *c = get_debuginfod_client ();

  if (c == nullptr)
    return scoped_fd (-ENOMEM);

  char *dname = nullptr;
  user_data data ("separate debug info for", filename);

  debuginfod_set_user_data (c, &data);
  scoped_fd fd (debuginfod_find_debuginfo (c, build_id, build_id_len,
					   &dname));
  debuginfod_set_user_data (c, nullptr);

  if (debuginfod_verbose > 0 && fd.get () < 0 && fd.get () != -ENOENT)
    printf_filtered (_("Download failed: %s.  Continuing without debug info for %ps.\n"),
		     safe_strerror (-fd.get ()),
		     styled_string (file_name_style.style (),  filename));

  if (fd.get () >= 0)
    destname->reset (dname);

  return fd;
}
#endif

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

  /* set debuginfod on */
  add_cmd ("on", class_run, set_debuginfod_on_command,
	   _("Enable debuginfod."), &set_debuginfod_prefix_list);

  /* set debuginfod off */
  add_cmd ("off", class_run, set_debuginfod_off_command,
	   _("Disable debuginfod."), &set_debuginfod_prefix_list);

  /* set debuginfod ask */
  add_cmd ("ask", class_run, set_debuginfod_ask_command, _("\
Ask the user whether to enable debuginfod before performing the next query."),
	   &set_debuginfod_prefix_list);

  /* show debuginfod status */
  add_cmd ("status", class_run, show_debuginfod_status_command,
	   _("Show whether debuginfod is set to \"on\", \"off\" or \"ask\"."),
	   &show_debuginfod_prefix_list);

  /* set/show debuginfod urls */
  add_setshow_string_noescape_cmd ("urls", class_run, _("\
Set the list of debuginfod server URLs."), _("\
Show the list of debuginfod server URLs."), _("\
Manage the space-separated list of debuginfod server URLs that GDB will query \
when missing debuginfo, executables or source files.\nThe default value is \
copied from the DEBUGINFOD_URLS environment variable."),
				   set_debuginfod_urls_command,
				   get_debuginfod_urls_command,
				   show_debuginfod_urls_command,
				   &set_debuginfod_prefix_list,
				   &show_debuginfod_prefix_list);

  /* set/show debuginfod verbose */
  add_setshow_zuinteger_cmd ("verbose", class_support,
			     &debuginfod_verbose, _("\
Set verbosity of debuginfod output."), _("\
Show debuginfod debugging."), _("\
When set to a non-zero value, display verbose output for each debuginfod \
query.\nTo disable, set to zero.  Verbose output is displayed by default."),
			     set_debuginfod_verbose_command,
			     show_debuginfod_verbose_command,
			     &set_debuginfod_prefix_list,
			     &show_debuginfod_prefix_list);
}
