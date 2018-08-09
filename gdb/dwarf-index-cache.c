/* Caching of GDB/DWARF index files.

   Copyright (C) 1994-2018 Free Software Foundation, Inc.

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
#include "dwarf-index-cache.h"

#include "build-id.h"
#include "cli/cli-cmds.h"
#include "command.h"
#include "common/scoped_mmap.h"
#include "common/pathstuff.h"
#include "dwarf-index-write.h"
#include "dwarf2read.h"
#include "objfiles.h"
#include "selftest.h"
#include <string>
#include <stdlib.h>

/* When set to 1, show debug messages about the index cache.  */
static int debug_index_cache = 0;

/* The index cache directory, used for "set/show index-cache directory".  */
static char *index_cache_directory = NULL;

/* See dwarf-index.cache.h.  */
index_cache global_index_cache;

/* set/show index-cache commands.  */
static cmd_list_element *set_index_cache_prefix_list;
static cmd_list_element *show_index_cache_prefix_list;

/* A cheap (as in low-quality) recursive mkdir.  Try to create all the parents
   directories up to DIR and DIR itself.  Stop if we hit an error along the way.
   There is no attempt to remove created directories in case of failure.  */

static void
mkdir_recursive (const char *dir)
{
  gdb::unique_xmalloc_ptr<char> holder (xstrdup (dir));
  char * const start = holder.get ();
  char *component_start = start;
  char *component_end = start;

  while (1)
    {
      /* Find the beginning of the next component.  */
      while (*component_start == '/')
	component_start++;

      /* Are we done?  */
      if (*component_start == '\0')
	return;

      /* Find the slash or null-terminator after this component.  */
      component_end = component_start;
      while (*component_end != '/' && *component_end != '\0')
	component_end++;

      /* Temporarily replace the slash with a null terminator, so we can create
         the directory up to this component.  */
      char saved_char = *component_end;
      *component_end = '\0';

      /* If we get EEXIST and the existing path is a directory, then we're
         happy.  If it exists, but it's a regular file and this is not the last
         component, we'll fail at the next component.  If this is the last
         component, the caller will fail with ENOTDIR when trying to
         open/create a file under that path.  */
      if (mkdir (start, 0700) != 0)
	if (errno != EEXIST)
	  return;

      /* Restore the overwritten char.  */
      *component_end = saved_char;
      component_start = component_end;
    }
}

/* Default destructor of index_cache_resource.  */
index_cache_resource::~index_cache_resource () = default;

/* See dwarf-index-cache.h.  */

void
index_cache::set_directory (std::string dir)
{
  gdb_assert (!dir.empty ());

  m_dir = std::move (dir);

  if (debug_index_cache)
    printf_unfiltered ("index cache: now using directory %s\n", m_dir.c_str ());
}

/* See dwarf-index-cache.h.  */

void
index_cache::enable ()
{
  if (debug_index_cache)
    printf_unfiltered ("index cache: enabling (%s)\n", m_dir.c_str ());

  m_enabled = true;
}

/* See dwarf-index-cache.h.  */

void
index_cache::disable ()
{
  if (debug_index_cache)
    printf_unfiltered ("index cache: disabling\n");

  m_enabled = false;
}

/* See dwarf-index-cache.h.  */

void
index_cache::store (struct dwarf2_per_objfile *dwarf2_per_objfile)
{
  objfile *obj = dwarf2_per_objfile->objfile;

  if (!enabled ())
    return;

  const bfd_build_id *build_id = build_id_bfd_get (obj->obfd);
  if (build_id == nullptr)
    {
      if (debug_index_cache)
	printf_unfiltered ("index cache: objfile %s has no build id\n",
			   objfile_name (obj));
      return;
    }

  if (m_dir.empty ())
    {
      warning (_("The index cache directory name is empty, skipping store."));
      return;
    }

  std::string build_id_str = build_id_to_string (build_id);

  TRY
    {
      /* Try to create the containing directory.  */
      mkdir_recursive (m_dir.c_str ());

      if (debug_index_cache)
        printf_unfiltered ("index cache: writing index cache for objfile %s\n",
			 objfile_name (obj));

      /* Write the index itself to the directory, using the build id as the
         filename.  */
      write_psymtabs_to_index (dwarf2_per_objfile, m_dir.c_str (),
			       build_id_str.c_str (), dw_index_kind::GDB_INDEX);
    }
  CATCH (except, RETURN_MASK_ERROR)
    {
      if (debug_index_cache)
	printf_unfiltered ("index cache: couldn't store index cache for objfile "
			 "%s: %s", objfile_name (obj), except.message);
    }
  END_CATCH
}

#if HAVE_SYS_MMAN_H

/* Hold the resources for an mmapped index file.  */

struct index_cache_resource_mmap final : public index_cache_resource
{
  /* Try to mmap FILENAME.  Throw an exception on failure, including if the
     file doesn't exist. */
  index_cache_resource_mmap (const char *filename)
    : mapping (mmap_file (filename))
  {}

  scoped_mmap mapping;
};

/* See dwarf-index-cache.h.  */

gdb::array_view<const gdb_byte>
index_cache::lookup_gdb_index (const bfd_build_id *build_id,
			       std::unique_ptr<index_cache_resource> *resource)
{
  if (!enabled ())
    return {};

  if (m_dir.empty ())
    {
      warning (_("The index cache directory name is empty, skipping cache "
		 "lookup."));
      return {};
    }

  /* Compute where we would expect a gdb index file for this build id to be.  */
  std::string filename = make_index_filename (build_id, INDEX4_SUFFIX);

  TRY
    {
      if (debug_index_cache)
        printf_unfiltered ("index cache: trying to read %s\n",
			   filename.c_str ());

      /* Try to map that file.  */
      index_cache_resource_mmap *mmap_resource
	= new index_cache_resource_mmap (filename.c_str ());

      /* Yay, it worked!  Hand the resource to the caller.  */
      resource->reset (mmap_resource);

      return gdb::array_view<const gdb_byte>
	  ((const gdb_byte *) mmap_resource->mapping.get (),
	   mmap_resource->mapping.size ());
    }
  CATCH (except, RETURN_MASK_ERROR)
    {
      if (debug_index_cache)
	printf_unfiltered ("index cache: couldn't read %s: %s\n",
			 filename.c_str (), except.message);
    }
  END_CATCH

  return {};
}

#else /* !HAVE_SYS_MMAN_H */

/* See dwarf-index-cache.h.  This is a no-op on unsupported systems.  */

gdb::array_view<const gdb_byte>
index_cache::lookup_gdb_index (const bfd_build_id *build_id,
			       std::unique_ptr<index_cache_resource> *resource)
{
  return {};
}

#endif

/* See dwarf-index-cache.h.  */

std::string
index_cache::make_index_filename (const bfd_build_id *build_id,
				  const char *suffix) const
{
  std::string build_id_str = build_id_to_string (build_id);

  return m_dir + SLASH_STRING + build_id_str + suffix;
}

/* "set index-cache" handler.  */

static void
set_index_cache_command (const char *arg, int from_tty)
{
  printf_unfiltered (_("\
Missing arguments.  See \"help set index-cache\" for help.\n"));
}

/* True when we are executing "show index-cache".  This is used to improve the
   printout a little bit.  */
static bool in_show_index_cache_command = false;

/* "show index-cache" handler.  */

static void
show_index_cache_command (const char *arg, int from_tty)
{
  /* Note that we are executing "show index-cache".  */
  auto restore_flag = make_scoped_restore (&in_show_index_cache_command, true);

  /* Call all "show index-cache" subcommands.  */
  cmd_show_list (show_index_cache_prefix_list, from_tty, "");

  printf_unfiltered ("\n");
  printf_unfiltered
    (_("The index cache is currently %s.\n"),
     global_index_cache.enabled () ? _("enabled") : _("disabled"));
}

/* "set index-cache on" handler.  */

static void
set_index_cache_on_command (const char *arg, int from_tty)
{
  global_index_cache.enable ();
}

/* "set index-cache off" handler.  */

static void
set_index_cache_off_command (const char *arg, int from_tty)
{
  global_index_cache.disable ();
}

/* "set index-cache directory" handler.  */

static void
set_index_cache_directory_command (const char *arg, int from_tty,
				   cmd_list_element *element)
{
  /* Make sure the index cache directory is absolute and tilde-expanded.  */
  gdb::unique_xmalloc_ptr<char> abs (gdb_abspath (index_cache_directory));
  xfree (index_cache_directory);
  index_cache_directory = abs.release ();
  global_index_cache.set_directory (index_cache_directory);
}

/* "show index-cache stats" handler.  */

static void
show_index_cache_stats_command (const char *arg, int from_tty)
{
  const char *indent = "";

  /* If this command is invoked through "show index-cache", make the display a
     bit nicer.  */
  if (in_show_index_cache_command)
    {
      indent = "  ";
      printf_unfiltered ("\n");
    }

  printf_unfiltered (_("%s  Cache hits (this session): %u\n"),
		     indent, global_index_cache.n_hits ());
  printf_unfiltered (_("%sCache misses (this session): %u\n"),
		     indent, global_index_cache.n_misses ());
}

#if GDB_SELF_TEST && defined (HAVE_MKDTEMP)
namespace selftests
{

/* Try to create DIR using mkdir_recursive and make sure it exists.  */

static bool
create_dir_and_check (const char *dir)
{
  mkdir_recursive (dir);

  struct stat st;
  if (stat (dir, &st) != 0)
    perror_with_name (("stat"));

  return (st.st_mode & S_IFDIR) != 0;
}

/* Test mkdir_recursive.  */

static void
test_mkdir_recursive ()
{
  char base[] = "/tmp/gdb-selftests-XXXXXX";

  if (mkdtemp (base) == NULL)
    perror_with_name (("mkdtemp"));

  /* Try not to leave leftover directories.  */
  struct cleanup_dirs {
    cleanup_dirs (const char *base)
      : m_base (base)
    {}

    ~cleanup_dirs () {
      rmdir (string_printf ("%s/a/b/c/d/e", m_base).c_str ());
      rmdir (string_printf ("%s/a/b/c/d", m_base).c_str ());
      rmdir (string_printf ("%s/a/b/c", m_base).c_str ());
      rmdir (string_printf ("%s/a/b", m_base).c_str ());
      rmdir (string_printf ("%s/a", m_base).c_str ());
      rmdir (m_base);
    }

  private:
    const char *m_base;
  } cleanup_dirs (base);

  std::string dir = string_printf ("%s/a/b", base);
  SELF_CHECK (create_dir_and_check (dir.c_str ()));

  dir = string_printf ("%s/a/b/c//d/e/", base);
  SELF_CHECK (create_dir_and_check (dir.c_str ()));
}
}
#endif /*  GDB_SELF_TEST && defined (HAVE_MKDTEMP) */

void
_initialize_index_cache ()
{
  /* Set the default index cache directory.  */
  std::string cache_dir = get_standard_cache_dir ();
  if (!cache_dir.empty ())
    {
      index_cache_directory = xstrdup (cache_dir.c_str ());
      global_index_cache.set_directory (std::move (cache_dir));
    }
  else
    warning (_("Couldn't determine a path for the index cache directory."));

  /* set index-cache */
  add_prefix_cmd ("index-cache", class_files, set_index_cache_command,
		  _("Set index-cache options"), &set_index_cache_prefix_list,
		  "set index-cache ", false, &setlist);

  /* show index-cache */
  add_prefix_cmd ("index-cache", class_files, show_index_cache_command,
		  _("Show index-cache options"), &show_index_cache_prefix_list,
		  "show index-cache ", false, &showlist);

  /* set index-cache on */
  add_cmd ("on", class_files, set_index_cache_on_command,
	   _("Enable the index cache."), &set_index_cache_prefix_list);

  /* set index-cache off */
  add_cmd ("off", class_files, set_index_cache_off_command,
	   _("Disable the index cache."), &set_index_cache_prefix_list);

  /* set index-cache directory */
  add_setshow_filename_cmd ("directory", class_files, &index_cache_directory,
			    _("Set the directory of the index cache."),
			    _("Show the directory of the index cache."),
			    NULL,
			    set_index_cache_directory_command, NULL,
			    &set_index_cache_prefix_list,
			    &show_index_cache_prefix_list);

  /* show index-cache stats */
  add_cmd ("stats", class_files, show_index_cache_stats_command,
	   _("Show some stats about the index cache."),
	   &show_index_cache_prefix_list);

  /* set debug index-cache */
  add_setshow_boolean_cmd ("index-cache", class_maintenance,
			   &debug_index_cache,
			   _("Set display of index-cache debug messages."),
			   _("Show display of index-cache debug messages."),
			   _("\
When non-zero, debugging output for the index cache is displayed."),
			    NULL, NULL,
			    &setdebuglist, &showdebuglist);

#if GDB_SELF_TEST && defined (HAVE_MKDTEMP)
  selftests::register_test ("mkdir_recursive", selftests::test_mkdir_recursive);
#endif
}
