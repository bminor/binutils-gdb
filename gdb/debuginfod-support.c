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
#include <sstream>

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
  user_data (const char *desc, std::string &fname)
    : desc (desc), fname (fname)
  { }

  const char * const desc;
  std::string & fname;
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

/* Ensure the progress message can fit on a single line. Otherwise
   garbled output is possible with \r.

   An example of possible truncations, starting with the original message:
   "Downloading XX MB separate debug info for /aa/bb/cc/dd/ee"
   "Downloading XX MB separate debug info for /aa/bb/.../ee"
   "Downloading XX MB separate debug info for ee"
   "Downloading XX MB separate debug info"
   "Downloading XX MB"
   "Downloading"
  */

static std::string
build_message (std::string size, std::string unit, std::string desc, std::string fname)
{
  int width = get_chars_per_line ();
  std::stringstream message;

  message << "Downloading";
  /* Leave room for spinner and percent indicator.  */
  int message_size = message.str ().length () + 6;

  if (!size.empty () && !unit.empty ())
    {
      message_size += size.size () + unit.size () + 2;
      if (message_size > width)
	return message.str ();

      /* "Downloading XX MB" */
      message << " " << size << " " << unit;
    }

  /* If FNAME does not fit then message will end with DESC_END.
     In case DESC_END is "separate debug info for", remove " for".  */
  std::string desc_end = desc;
  if (desc.substr (desc.size () - 4) == " for")
    desc_end = desc.substr (0, desc.size () - 4);

  if (message_size + desc_end.size () + 1 > width)
    return message.str ();

  string_file styled_fname (current_uiout->can_emit_style_escape ());
  if (message_size + desc.size () + fname.size () + 2 <= width)
    {
      /* Truncation is not necessary. Return untruncated message.
	 "Downloading XX MB separate debug info for /usr/libxyz.so" */
      fprintf_styled (&styled_fname, file_name_style.style (), "%s",
		      fname.c_str ());

      message << " " << desc << " " << styled_fname.c_str ();
      return message.str ();
    }

   while (fname.back () == '/')
    fname.pop_back ();

  /* Find path separators for the first, second and final components.
     If FNAME does not have path separators and it does not fit in the
     available space, do not include it in message.  */
  size_t sep1 = fname.find ('/');
  if (sep1 == std::string::npos)
    {
      message << " " << desc_end;
      return message.str ();
    }

  size_t sep2 = fname.find ('/', sep1 + 1);
  size_t sep3;
  if (sep2 == std::string::npos)
    sep3 = std::string::npos;
  else
    sep3 = fname.find ('/', sep2 + 1);
  size_t seplast = fname.find_last_of ('/');

  /* If the first, second, and final path components are distinct, try to
     truncate FNAME so that it fits in the available space.  Preserve the
     first, second and final path components.  For example,
     "/aa/bb/cc/dd/ee" becomes "/aa/bb/.../ee" and
     "../aa/bb/cc/dd/" becomes "../aa/.../ee"  */
  std::stringstream trunc;
  if (sep2 != sep3 && sep2 != seplast && sep2 != std::string::npos)
    {
      std::stringstream fnametmp;

      if (sep1 == 0 && sep3 != seplast && sep3 != std::string::npos)
	fnametmp << fname.substr (0, sep3 + 1)
		 << "..." << fname.substr (seplast);
      else if (sep1 != 0)
	fnametmp << fname.substr (0, sep2 + 1)
		 << "..." << fname.substr (seplast);

      if (!fnametmp.str ().empty ())
	{
	  trunc << " " << desc << " ";
	  if (message_size + trunc.str ().size () + fnametmp.str ().size () <= width)
	    {
	      fprintf_styled (&styled_fname, file_name_style.style (), "%s",
			      fnametmp.str ().c_str ());
	      message << trunc.str () << styled_fname.c_str ();
	      return message.str ();
	    }
	}
    }

  /* The first, second and final components are not distinct or
     "/aa/bb/.../ee" does not fit. Try "ee" instead.  */
  trunc.str ("");
  trunc << " " << desc << " ";
  fname = fname.substr (seplast + 1);
  if (message_size + trunc.str ().size () + fname.size () <= width)
    {
      fprintf_styled (&styled_fname, file_name_style.style (), "%s",
		      fname.c_str ());
      message << trunc.str () << styled_fname.c_str ();
      return message.str ();
    }

  /* We aren't able to fit anything from FNAME. End message with DESC_END
     since we already confirmed it will fit.  */
  message << " " << desc_end;
  return message.str ();
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

      string_file styled_fname (current_uiout->can_emit_style_escape ());
      fprintf_styled (&styled_fname, file_name_style.style (), "%s",
		      data->fname.c_str ());

      printf_filtered ("Cancelled download of %s %s\n",
		       data->desc, styled_fname.c_str ());
      return 1;
    }

  if (debuginfod_verbose == 0
      || (data->progress.has_value ()
	  && data->progress->get_state () == ui_out::progress_update::WORKING))
    return 0;

  /* Print progress update.  Include the transfer size if available.  */
  if (total > 0)
    {
      /* Transfer size is known.  */
      if (!data->progress.has_value ()
	  || data->progress->get_state () != ui_out::progress_update::PERCENT)
	{
	  double size = (double)total;
	  const char *unit = "";

	  get_size_and_unit (&size, &unit);
          std::string fsize = string_printf ("%.2f", size);
	  std::string message = build_message (fsize, unit, data->desc,
					       data->fname);
	  if (!data->progress.has_value ())
	    data->progress.emplace (current_uiout, message, 1);
	  else
	    data->progress->update_name (message);
	}

	double percent = (double)cur / (double)total;
	if (percent >= 0.0 && percent <= 1.0)
	  {
	    /* Ensure PERCENT doesn't require three digits to display.  */
	    if (percent > 0.99 && percent <= 1.0)
	      percent = .99;
	    current_uiout->update_progress_percent (percent);
	    return 0;
	  }
    }

  if (!data->progress.has_value ()
      || data->progress->get_state () != ui_out::progress_update::SPIN)
    {
      std::string message = build_message ("", "", data->desc, data->fname);

      if (!data->progress.has_value ())
	data->progress.emplace (current_uiout, message, 1);
      else
	data->progress->update_name (message);
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

/* Print the result of the most recent attempted download.  */

static void
print_outcome (user_data &data, int fd)
{
  /* Clears the current line of progress output.  */
  if (data.progress.has_value ())
    data.progress.reset ();

  string_file styled_fname (current_uiout->can_emit_style_escape ());
  fprintf_styled (&styled_fname, file_name_style.style (), "%s",
		  data.fname.c_str ());

  if (debuginfod_verbose > 1 && fd >= 0)
    {
      struct stat s;

      if (fstat (fd, &s) == 0)
	{
	  double size = (double)s.st_size;
	  const char *unit = "";

	  get_size_and_unit (&size, &unit);
	  printf_filtered (_("Retrieved %.02f %s %s %s\n"), size, unit,
			   data.desc, styled_fname.c_str ());
	}
      else
	warning (_("Retrieved %s %s but size cannot be read: %s\n"),
		 data.desc, styled_fname.c_str (),
		 safe_strerror (errno));
    }
  else if (debuginfod_verbose > 0 && fd < 0 && fd != -ENOENT)
    printf_filtered (_("Download failed: %s. " \
		       "Continuing without %s %s.\n"),
		     safe_strerror (-fd), data.desc,
		     styled_fname.c_str ());
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
  std::string fname = srcpath;
  user_data data ("source file", fname);

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
  std::string fname = filename;
  user_data data ("separate debug info for", fname);

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
