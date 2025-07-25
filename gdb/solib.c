/* Handle shared libraries for GDB, the GNU Debugger.

   Copyright (C) 1990-2025 Free Software Foundation, Inc.

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


#include <fcntl.h>
#include "exceptions.h"
#include "extract-store-integer.h"
#include "symtab.h"
#include "bfd.h"
#include "build-id.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbcore.h"
#include "command.h"
#include "target.h"
#include "frame.h"
#include "inferior.h"
#include "gdbsupport/environ.h"
#include "cli/cli-cmds.h"
#include "elf/external.h"
#include "elf/common.h"
#include "filenames.h"
#include "exec.h"
#include "observable.h"
#include "readline/tilde.h"
#include "solib.h"
#include "interps.h"
#include "filesystem.h"
#include "gdb_bfd.h"
#include "gdbsupport/filestuff.h"
#include "gdbsupport/scoped_fd.h"
#include "source.h"
#include "cli/cli-style.h"

/* See solib.h.  */

bool debug_solib;

/* If non-empty, this is a search path for loading non-absolute shared library
   symbol files.  This takes precedence over the environment variables PATH
   and LD_LIBRARY_PATH.  */
static std::string solib_search_path;

static void
show_solib_search_path (struct ui_file *file, int from_tty,
			struct cmd_list_element *c, const char *value)
{
  gdb_printf (file,
	      _("The search path for loading non-absolute "
		"shared library symbol files is %s.\n"),
	      value);
}

/* Same as HAVE_DOS_BASED_FILE_SYSTEM, but usable as an rvalue.  */
#if (HAVE_DOS_BASED_FILE_SYSTEM)
#define DOS_BASED_FILE_SYSTEM 1
#else
#define DOS_BASED_FILE_SYSTEM 0
#endif

/* Return the full pathname of a binary file (the main executable or a
   shared library file), or NULL if not found.  If FD is non-NULL, *FD
   is set to either -1 or an open file handle for the binary file.

   Global variable GDB_SYSROOT is used as a prefix directory
   to search for binary files if they have an absolute path.
   If GDB_SYSROOT starts with "target:" and target filesystem
   is the local filesystem then the "target:" prefix will be
   stripped before the search starts.  This ensures that the
   same search algorithm is used for local files regardless of
   whether a "target:" prefix was used.

   Global variable SOLIB_SEARCH_PATH is used as a prefix directory
   (or set of directories, as in LD_LIBRARY_PATH) to search for all
   shared libraries if not found in either the sysroot (if set) or
   the local filesystem.  SOLIB_SEARCH_PATH is not used when searching
   for the main executable.

   Search algorithm:
   * If a sysroot is set and path is absolute:
   *   Search for sysroot/path.
   * else
   *   Look for it literally (unmodified).
   * If IS_SOLIB is non-zero:
   *   Look in SOLIB_SEARCH_PATH.
   *   If available, use target defined search function.
   * If NO sysroot is set, perform the following two searches:
   *   Look in inferior's $PATH.
   *   If IS_SOLIB is non-zero:
   *     Look in inferior's $LD_LIBRARY_PATH.
   *
   * The last check avoids doing this search when targeting remote
   * machines since a sysroot will almost always be set.
*/

static gdb::unique_xmalloc_ptr<char>
solib_find_1 (const char *in_pathname, int *fd, bool is_solib)
{
  int found_file = -1;
  gdb::unique_xmalloc_ptr<char> temp_pathname;
  const char *fskind = effective_target_file_system_kind ();
  const char *sysroot = gdb_sysroot.c_str ();
  int prefix_len, orig_prefix_len;

  /* If the absolute prefix starts with "target:" but the filesystem
     accessed by the target_fileio_* methods is the local filesystem
     then we strip the "target:" prefix now and work with the local
     filesystem.  This ensures that the same search algorithm is used
     for all local files regardless of whether a "target:" prefix was
     used.  */
  if (is_target_filename (sysroot) && target_filesystem_is_local ())
    sysroot += strlen (TARGET_SYSROOT_PREFIX);

  /* Strip any trailing slashes from the absolute prefix.  */
  prefix_len = orig_prefix_len = strlen (sysroot);

  while (prefix_len > 0 && IS_DIR_SEPARATOR (sysroot[prefix_len - 1]))
    prefix_len--;

  std::string sysroot_holder;
  if (prefix_len == 0)
    sysroot = NULL;
  else if (prefix_len != orig_prefix_len)
    {
      sysroot_holder = std::string (sysroot, prefix_len);
      sysroot = sysroot_holder.c_str ();
    }

  /* If we're on a non-DOS-based system, backslashes won't be
     understood as directory separator, so, convert them to forward
     slashes, iff we're supposed to handle DOS-based file system
     semantics for target paths.  */
  if (!DOS_BASED_FILE_SYSTEM && fskind == file_system_kind_dos_based)
    {
      char *p;

      /* Avoid clobbering our input.  */
      p = (char *) alloca (strlen (in_pathname) + 1);
      strcpy (p, in_pathname);
      in_pathname = p;

      for (; *p; p++)
	{
	  if (*p == '\\')
	    *p = '/';
	}
    }

  /* Note, we're interested in IS_TARGET_ABSOLUTE_PATH, not
     IS_ABSOLUTE_PATH.  The latter is for host paths only, while
     IN_PATHNAME is a target path.  For example, if we're supposed to
     be handling DOS-like semantics we want to consider a
     'c:/foo/bar.dll' path as an absolute path, even on a Unix box.
     With such a path, before giving up on the sysroot, we'll try:

       1st attempt, c:/foo/bar.dll ==> /sysroot/c:/foo/bar.dll
       2nd attempt, c:/foo/bar.dll ==> /sysroot/c/foo/bar.dll
       3rd attempt, c:/foo/bar.dll ==> /sysroot/foo/bar.dll
  */

  if (!IS_TARGET_ABSOLUTE_PATH (fskind, in_pathname) || sysroot == NULL)
    temp_pathname.reset (xstrdup (in_pathname));
  else
    {
      bool need_dir_separator;

      /* Concatenate the sysroot and the target reported filename.  We
	 may need to glue them with a directory separator.  Cases to
	 consider:

	| sysroot         | separator | in_pathname    |
	|-----------------+-----------+----------------|
	| /some/dir       | /         | c:/foo/bar.dll |
	| /some/dir       |           | /foo/bar.dll   |
	| target:         |           | c:/foo/bar.dll |
	| target:         |           | /foo/bar.dll   |
	| target:some/dir | /         | c:/foo/bar.dll |
	| target:some/dir |           | /foo/bar.dll   |

	IOW, we don't need to add a separator if IN_PATHNAME already
	has one, or when the sysroot is exactly "target:".
	There's no need to check for drive spec explicitly, as we only
	get here if IN_PATHNAME is considered an absolute path.  */
      need_dir_separator = !(IS_DIR_SEPARATOR (in_pathname[0])
			     || strcmp (TARGET_SYSROOT_PREFIX, sysroot) == 0);

      /* Cat the prefixed pathname together.  */
      temp_pathname.reset (concat (sysroot,
				   need_dir_separator ? SLASH_STRING : "",
				   in_pathname, (char *) NULL));
    }

  /* Handle files to be accessed via the target.  */
  if (is_target_filename (temp_pathname.get ()))
    {
      if (fd != NULL)
	*fd = -1;
      return temp_pathname;
    }

  /* Now see if we can open it.  */
  found_file = gdb_open_cloexec (temp_pathname.get (), O_RDONLY | O_BINARY, 0)
		 .release ();

  /* If the search in gdb_sysroot failed, and the path name has a
     drive spec (e.g, c:/foo), try stripping ':' from the drive spec,
     and retrying in the sysroot:
       c:/foo/bar.dll ==> /sysroot/c/foo/bar.dll.  */

  if (found_file < 0 && sysroot != NULL
      && HAS_TARGET_DRIVE_SPEC (fskind, in_pathname))
    {
      bool need_dir_separator = !IS_DIR_SEPARATOR (in_pathname[2]);
      char drive[2] = { in_pathname[0], '\0' };

      temp_pathname.reset (concat (sysroot, SLASH_STRING, drive,
				   need_dir_separator ? SLASH_STRING : "",
				   in_pathname + 2, (char *) NULL));

      found_file
	= gdb_open_cloexec (temp_pathname.get (), O_RDONLY | O_BINARY, 0)
	    .release ();
      if (found_file < 0)
	{
	  /* If the search in gdb_sysroot still failed, try fully
	     stripping the drive spec, and trying once more in the
	     sysroot before giving up.

	     c:/foo/bar.dll ==> /sysroot/foo/bar.dll.  */

	  temp_pathname.reset (concat (sysroot,
				       need_dir_separator ? SLASH_STRING : "",
				       in_pathname + 2, (char *) NULL));

	  found_file
	    = gdb_open_cloexec (temp_pathname.get (), O_RDONLY | O_BINARY, 0)
		.release ();
	}
    }

  /* We try to find the library in various ways.  After each attempt,
     either found_file >= 0 and temp_pathname is a malloc'd string, or
     found_file < 0 and temp_pathname does not point to storage that
     needs to be freed.  */

  if (found_file < 0)
    temp_pathname.reset (NULL);

  /* If the search in gdb_sysroot failed, and the path name is
     absolute at this point, make it relative.  (openp will try and open the
     file according to its absolute path otherwise, which is not what we want.)
     Affects subsequent searches for this solib.  */
  if (found_file < 0 && IS_TARGET_ABSOLUTE_PATH (fskind, in_pathname))
    {
      /* First, get rid of any drive letters etc.  */
      while (!IS_TARGET_DIR_SEPARATOR (fskind, *in_pathname))
	in_pathname++;

      /* Next, get rid of all leading dir separators.  */
      while (IS_TARGET_DIR_SEPARATOR (fskind, *in_pathname))
	in_pathname++;
    }

  /* If not found, and we're looking for a solib, search the
     solib_search_path (if any).  */
  if (is_solib && found_file < 0 && !solib_search_path.empty ())
    found_file = openp (solib_search_path.c_str (),
			OPF_TRY_CWD_FIRST | OPF_RETURN_REALPATH, in_pathname,
			O_RDONLY | O_BINARY, &temp_pathname);

  /* If not found, and we're looking for a solib, next search the
     solib_search_path (if any) for the basename only (ignoring the
     path).  This is to allow reading solibs from a path that differs
     from the opened path.  */
  if (is_solib && found_file < 0 && !solib_search_path.empty ())
    found_file = openp (solib_search_path.c_str (),
			OPF_TRY_CWD_FIRST | OPF_RETURN_REALPATH,
			target_lbasename (fskind, in_pathname),
			O_RDONLY | O_BINARY, &temp_pathname);

  /* If not found, next search the inferior's $PATH environment variable.  */
  if (found_file < 0 && sysroot == NULL)
    found_file = openp (current_inferior ()->environment.get ("PATH"),
			OPF_TRY_CWD_FIRST | OPF_RETURN_REALPATH, in_pathname,
			O_RDONLY | O_BINARY, &temp_pathname);

  /* If not found, and we're looking for a solib, next search the
     inferior's $LD_LIBRARY_PATH environment variable.  */
  if (is_solib && found_file < 0 && sysroot == NULL)
    found_file
      = openp (current_inferior ()->environment.get ("LD_LIBRARY_PATH"),
	       OPF_TRY_CWD_FIRST | OPF_RETURN_REALPATH, in_pathname,
	       O_RDONLY | O_BINARY, &temp_pathname);

  if (fd == NULL)
    {
      if (found_file >= 0)
	close (found_file);
    }
  else
    *fd = found_file;

  return temp_pathname;
}

/* Return the full pathname of the main executable, or NULL if not
   found.  If FD is non-NULL, *FD is set to either -1 or an open file
   handle for the main executable.  */

gdb::unique_xmalloc_ptr<char>
exec_file_find (const char *in_pathname, int *fd)
{
  gdb::unique_xmalloc_ptr<char> result;
  const char *fskind = effective_target_file_system_kind ();

  if (in_pathname == NULL)
    return NULL;

  if (!gdb_sysroot.empty () && IS_TARGET_ABSOLUTE_PATH (fskind, in_pathname))
    {
      result = solib_find_1 (in_pathname, fd, false);

      if (result == NULL && fskind == file_system_kind_dos_based)
	{
	  char *new_pathname;

	  new_pathname = (char *) alloca (strlen (in_pathname) + 5);
	  strcpy (new_pathname, in_pathname);
	  strcat (new_pathname, ".exe");

	  result = solib_find_1 (new_pathname, fd, false);
	}
    }
  else
    {
      /* It's possible we don't have a full path, but rather just a
	 filename.  Some targets, such as HP-UX, don't provide the
	 full path, sigh.

	 Attempt to qualify the filename against the source path.
	 (If that fails, we'll just fall back on the original
	 filename.  Not much more we can do...)  */

      if (!source_full_path_of (in_pathname, &result))
	result.reset (xstrdup (in_pathname));
      if (fd != NULL)
	*fd = -1;
    }

  return result;
}

/* Return the full pathname of a shared library file, or NULL if not
   found.  If FD is non-NULL, *FD is set to either -1 or an open file
   handle for the shared library.

   The search algorithm used is described in solib_find_1's comment
   above.  */

gdb::unique_xmalloc_ptr<char>
solib_find (const char *in_pathname, int *fd)
{
  const char *solib_symbols_extension
    = gdbarch_solib_symbols_extension (current_inferior ()->arch ());

  /* If solib_symbols_extension is set, replace the file's
     extension.  */
  if (solib_symbols_extension != NULL)
    {
      const char *p = in_pathname + strlen (in_pathname);

      while (p > in_pathname && *p != '.')
	p--;

      if (*p == '.')
	{
	  char *new_pathname;

	  new_pathname
	    = (char *) alloca (p - in_pathname + 1
			       + strlen (solib_symbols_extension) + 1);
	  memcpy (new_pathname, in_pathname, p - in_pathname + 1);
	  strcpy (new_pathname + (p - in_pathname) + 1,
		  solib_symbols_extension);

	  in_pathname = new_pathname;
	}
    }

  return solib_find_1 (in_pathname, fd, true);
}

/* Open and return a BFD for the shared library PATHNAME.  If FD is not -1,
   it is used as file handle to open the file.  Throws an error if the file
   could not be opened.  Handles both local and remote file access.

   If unsuccessful, the FD will be closed (unless FD was -1).  */

gdb_bfd_ref_ptr
solib_bfd_fopen (const char *pathname, int fd)
{
  gdb_bfd_ref_ptr abfd (gdb_bfd_open (pathname, gnutarget, fd));

  if (abfd == NULL)
    {
      /* Arrange to free PATHNAME when the error is thrown.  */
      error (_("Could not open `%s' as an executable file: %s"), pathname,
	     bfd_errmsg (bfd_get_error ()));
    }

  return abfd;
}

/* Find shared library PATHNAME and open a BFD for it.  */

gdb_bfd_ref_ptr
solib_bfd_open (const char *pathname)
{
  int found_file;
  const struct bfd_arch_info *b;

  /* Search for shared library file.  */
  gdb::unique_xmalloc_ptr<char> found_pathname
    = solib_find (pathname, &found_file);
  if (found_pathname == NULL)
    {
      /* Return failure if the file could not be found, so that we can
	 accumulate messages about missing libraries.  */
      if (errno == ENOENT)
	return NULL;

      perror_with_name (pathname);
    }

  /* Open bfd for shared library.  */
  gdb_bfd_ref_ptr abfd (solib_bfd_fopen (found_pathname.get (), found_file));

  /* Check bfd format.  */
  if (!bfd_check_format (abfd.get (), bfd_object))
    error (_("`%s': not in executable format: %s"),
	   bfd_get_filename (abfd.get ()), bfd_errmsg (bfd_get_error ()));

  /* Check bfd arch.  */
  b = gdbarch_bfd_arch_info (current_inferior ()->arch ());
  if (!b->compatible (b, bfd_get_arch_info (abfd.get ())))
    error (_("`%s': Shared library architecture %s is not compatible "
	     "with target architecture %s."),
	   bfd_get_filename (abfd.get ()),
	   bfd_get_arch_info (abfd.get ())->printable_name, b->printable_name);

  return abfd;
}

gdb_bfd_ref_ptr
solib_ops::bfd_open (const char *pathname) const
{
  return solib_bfd_open (pathname);
}

/* Given a pointer to one of the shared objects in our list of mapped
   objects, use the recorded name to open a bfd descriptor for the
   object, build a section table, relocate all the section addresses
   by the base address at which the shared object was mapped, and then
   add the sections to the target's section table.

   FIXME: In most (all?) cases the shared object file name recorded in
   the dynamic linkage tables will be a fully qualified pathname.  For
   cases where it isn't, do we really mimic the systems search
   mechanism correctly in the below code (particularly the tilde
   expansion stuff?).  */

static int
solib_map_sections (solib &so)
{
  gdb::unique_xmalloc_ptr<char> filename (tilde_expand (so.name.c_str ()));
  gdb_bfd_ref_ptr abfd (so.ops ().bfd_open (filename.get ()));

  /* If we have a core target then the core target might have some helpful
     information (i.e. build-ids) about the shared libraries we are trying
     to load.  Grab those hints now and use the below to validate or find
     the shared libraries.

     If we don't have a core target then this will return an empty struct
     with no hint information, we then lookup the shared library based on
     its filename.  */
  std::optional<CORE_ADDR> solib_addr = so.ops ().find_solib_addr (so);
  std::optional <const core_target_mapped_file_info> mapped_file_info
    = core_target_find_mapped_file (so.name.c_str (), solib_addr);

  /* If we already know the build-id of this solib from a core file, verify
     it matches ABFD's build-id.  If there is a mismatch or the solib wasn't
     found, attempt to query debuginfod for the correct solib.  */
  if (mapped_file_info.has_value ())
    {
      bool mismatch = (abfd != nullptr
		       && build_id_bfd_get (abfd.get ()) != nullptr
		       && !build_id_equal (mapped_file_info->build_id (),
					   build_id_bfd_get (abfd.get ())));

      if (abfd == nullptr || mismatch)
	{
	  /* If GDB found a suitable file during the file mapping
	     processing stage then lets use that.  We don't check the
	     build-id after opening this file, either this file was found
	     by build-id, in which case it's going to match, or this file
	     doesn't have a build-id, so checking tells us nothing.
	     However, if it was good enough during the mapped file
	     processing, we assume it's good enough now.  */
	  if (!mapped_file_info->filename ().empty ())
	    abfd = so.ops ().bfd_open (mapped_file_info->filename ().c_str ());
	  else
	    abfd = nullptr;

	  if (abfd == nullptr)
	    abfd = find_objfile_by_build_id (current_program_space,
					     mapped_file_info->build_id (),
					     so.name.c_str ());

	  if (abfd == nullptr && mismatch)
	    {
	      warning (_("Build-id of %ps does not match core file."),
		       styled_string (file_name_style.style (),
				      filename.get ()));
	      abfd = nullptr;
	    }
	}
    }

  if (abfd == NULL)
    return 0;

  /* Leave bfd open, core_xfer_memory and "info files" need it.  */
  so.abfd = std::move (abfd);

  /* Copy the full path name into `so.name`, allowing symbol_file_add
     to find it later.  This also affects the =library-loaded GDB/MI
     event, and in particular the part of that notification providing
     the library's host-side path.  If we let the target dictate
     that objfile's path, and the target is different from the host,
     GDB/MI will not provide the correct host-side path.  */

  so.name = bfd_get_filename (so.abfd.get ());
  so.sections = build_section_table (so.abfd.get ());

  for (target_section &p : so.sections)
    {
      /* Relocate the section binding addresses as recorded in the shared
	 object's file by the base address to which the object was actually
	 mapped.  */
      so.ops ().relocate_section_addresses (so, &p);

      /* If the target didn't provide information about the address
	 range of the shared object, assume we want the location of
	 the .text section.  */
      if (so.addr_low == 0 && so.addr_high == 0
	  && strcmp (p.the_bfd_section->name, ".text") == 0)
	{
	  so.addr_low = p.addr;
	  so.addr_high = p.endaddr;
	}
    }

  /* Add the shared object's sections to the current set of file
     section tables.  Do this immediately after mapping the object so
     that later nodes in the list can query this object, as is needed
     in solib-osf.c.  */
  current_program_space->add_target_sections (&so, so.sections);

  return 1;
}

/* See solib.h.  */

void
solib::clear ()
{
  this->sections.clear ();
  this->abfd = nullptr;

  /* Our caller closed the objfile, possibly via objfile_purge_solibs.  */
  this->symbols_loaded = false;
  this->objfile = nullptr;

  this->addr_low = this->addr_high = 0;

  /* Restore the target-supplied file name.  SO_NAME may be the path
     of the symbol file.  */
  this->name = this->original_name;

  /* Do the same for target-specific data.  */
  this->ops ().clear_so (*this);
}

lm_info::~lm_info () = default;

/* Read in symbols for shared object SO.  If SYMFILE_VERBOSE is set in FLAGS,
   be chatty about it.  Return true if any symbols were actually loaded.  */

bool
solib_read_symbols (solib &so, symfile_add_flags flags)
{
  if (so.symbols_loaded)
    {
      /* If needed, we've already warned in our caller.  */
    }
  else if (so.abfd == NULL)
    {
      /* We've already warned about this library, when trying to open
	 it.  */
    }
  else
    {
      flags |= current_inferior ()->symfile_flags;

      try
	{
	  /* Have we already loaded this shared object?  */
	  so.objfile = nullptr;
	  for (objfile *objfile : current_program_space->objfiles ())
	    {
	      if (filename_cmp (objfile_name (objfile), so.name.c_str ())
		    == 0
		  && objfile->addr_low == so.addr_low)
		{
		  so.objfile = objfile;
		  break;
		}
	    }
	  if (so.objfile == NULL)
	    {
	      section_addr_info sap
		= build_section_addr_info_from_section_table (so.sections);
	      gdb_bfd_ref_ptr tmp_bfd = so.abfd;
	      so.objfile
		= symbol_file_add_from_bfd (tmp_bfd, so.name.c_str (),
					    flags, &sap, OBJF_SHARED, nullptr);
	      so.objfile->addr_low = so.addr_low;
	    }

	  so.symbols_loaded = true;
	}
      catch (const gdb_exception_error &e)
	{
	  exception_fprintf (gdb_stderr, e,
			     _("Error while reading shared"
			       " library symbols for %s:\n"),
			     so.name.c_str ());
	}

      return true;
    }

  return false;
}

/* Return true if KNOWN->objfile is used by any other solib object
   in PSPACE's list of shared libraries.  Return false otherwise.  */

static bool
solib_used (program_space *pspace, const solib &known)
{
  for (const solib &pivot : pspace->solibs ())
    if (&pivot != &known && pivot.objfile == known.objfile)
      return true;

  return false;
}

/* Notify interpreters and observers that solib SO has been loaded.  */

static void
notify_solib_loaded (solib &so)
{
  interps_notify_solib_loaded (so);
  gdb::observers::solib_loaded.notify (so);
}

/* Notify interpreters and observers that solib SO has been unloaded.
   When STILL_IN_USE is true, the objfile backing SO is still in use,
   this indicates that SO was loaded multiple times, but only mapped
   in once (the mapping was reused).

   When SILENT is true, don't announce to the user if any breakpoints are
   disabled as a result of unloading SO.  */

static void
notify_solib_unloaded (program_space *pspace, const solib &so,
		       bool still_in_use, bool silent)
{
  interps_notify_solib_unloaded (so, still_in_use);
  gdb::observers::solib_unloaded.notify (pspace, so, still_in_use, silent);
}

/* See solib.h.  */

void
update_solib_list (int from_tty)
{
  const solib_ops *ops = current_program_space->solib_ops ();

  if (ops == nullptr)
    return;

  /* We can reach here due to changing solib-search-path or the
     sysroot, before having any inferior.  */
  if (target_has_execution () && inferior_ptid != null_ptid)
    {
      struct inferior *inf = current_inferior ();

      /* If we are attaching to a running process for which we
	 have not opened a symbol file, we may be able to get its
	 symbols now!  */
      if (inf->attach_flag
	  && current_program_space->symfile_object_file == nullptr)
	{
	  try
	    {
	      ops->open_symbol_file_object (from_tty);
	    }
	  catch (const gdb_exception_error &ex)
	    {
	      exception_fprintf (gdb_stderr, ex,
				 "Error reading attached "
				 "process's symbol file.\n");
	    }
	}
    }

  /* GDB and the inferior's dynamic linker each maintain their own
     list of currently loaded shared objects; we want to bring the
     former in sync with the latter.  Scan both lists, seeing which
     shared objects appear where.  There are three cases:

     - A shared object appears on both lists.  This means that GDB
     knows about it already, and it's still loaded in the inferior.
     Nothing needs to happen.

     - A shared object appears only on GDB's list.  This means that
     the inferior has unloaded it.  We should remove the shared
     object from GDB's tables.

     - A shared object appears only on the inferior's list.  This
     means that it's just been loaded.  We should add it to GDB's
     tables.

     So we walk GDB's list, checking each entry to see if it appears
     in the inferior's list too.  If it does, no action is needed, and
     we remove it from the inferior's list.  If it doesn't, the
     inferior has unloaded it, and we remove it from GDB's list.  By
     the time we're done walking GDB's list, the inferior's list
     contains only the new shared objects, which we then add.  */

  owning_intrusive_list<solib> inferior = ops->current_sos ();
  owning_intrusive_list<solib>::iterator gdb_iter
    = current_program_space->solibs ().begin ();
  while (gdb_iter != current_program_space->solibs ().end ())
    {
      intrusive_list<solib>::iterator inferior_iter = inferior.begin ();

      /* Check to see whether the shared object *gdb also appears in
	 the inferior's current list.  */
      for (; inferior_iter != inferior.end (); ++inferior_iter)
	if (ops->same (*gdb_iter, *inferior_iter))
	  break;

      /* If the shared object appears on the inferior's list too, then
	 it's still loaded, so we don't need to do anything.  Delete
	 it from the inferior's list, and leave it on GDB's list.  */
      if (inferior_iter != inferior.end ())
	{
	  inferior.erase (inferior_iter);
	  ++gdb_iter;
	}

      /* If it's not on the inferior's list, remove it from GDB's tables.  */
      else
	{
	  bool still_in_use
	    = (gdb_iter->objfile != nullptr
	       && solib_used (current_program_space, *gdb_iter));

	  /* Notify any observer that the shared object has been
	     unloaded before we remove it from GDB's tables.  */
	  notify_solib_unloaded (current_program_space, *gdb_iter,
				 still_in_use, false);

	  /* Unless the user loaded it explicitly, free SO's objfile.  */
	  if (gdb_iter->objfile != nullptr
	      && !(gdb_iter->objfile->flags & OBJF_USERLOADED)
	      && !still_in_use)
	    gdb_iter->objfile->unlink ();

	  current_program_space->deleted_solibs.push_back (gdb_iter->name);

	  /* Some targets' section tables might be referring to
	     sections from so.abfd; remove them.  */
	  current_program_space->remove_target_sections (&*gdb_iter);

	  gdb_iter = current_program_space->solibs ().erase (gdb_iter);
	}
    }

  /* Now the inferior's list contains only shared objects that don't
     appear in GDB's list --- those that are newly loaded.  Add them
     to GDB's shared object list.  */
  if (!inferior.empty ())
    {
      int not_found = 0;
      const char *not_found_filename = NULL;

      /* Fill in the rest of each of the `so' nodes.  */
      for (solib &new_so : inferior)
	{
	  current_program_space->added_solibs.push_back (&new_so);

	  try
	    {
	      /* Fill in the rest of the `struct solib' node.  */
	      if (!solib_map_sections (new_so))
		{
		  not_found++;
		  if (not_found_filename == NULL)
		    not_found_filename = new_so.original_name.c_str ();
		}
	    }

	  catch (const gdb_exception_error &e)
	    {
	      exception_fprintf (gdb_stderr, e,
				 _("Error while mapping shared "
				   "library sections:\n"));
	    }

	  /* Notify any observer that the shared object has been
	     loaded now that we've added it to GDB's tables.  */
	  notify_solib_loaded (new_so);
	}

      /* Add the new shared objects to GDB's list.  */
      current_program_space->solibs ().splice (std::move (inferior));

      /* If a library was not found, issue an appropriate warning
	 message.  We have to use a single call to warning in case the
	 front end does something special with warnings, e.g., pop up
	 a dialog box.  It Would Be Nice if we could get a "warning: "
	 prefix on each line in the CLI front end, though - it doesn't
	 stand out well.  */

      if (not_found == 1)
	warning (_("Could not load shared library symbols for %ps.\n"
		   "Do you need \"%ps\" or \"%ps\"?"),
		 styled_string (file_name_style.style (),
				not_found_filename),
		 styled_string (command_style.style (),
				"set solib-search-path"),
		 styled_string (command_style.style (), "set sysroot"));
      else if (not_found > 1)
	warning (_("\
Could not load shared library symbols for %d libraries, e.g. %ps.\n\
Use the \"%ps\" command to see the complete listing.\n\
Do you need \"%ps\" or \"%ps\"?"),
		 not_found,
		 styled_string (file_name_style.style (),
				not_found_filename),
		 styled_string (command_style.style (),
				"info sharedlibrary"),
		 styled_string (command_style.style (),
				"set solib-search-path"),
		 styled_string (command_style.style (),
				"set sysroot"));

    }
}

/* Return non-zero if NAME is the libpthread shared library.

   Uses a fairly simplistic heuristic approach where we check
   the file name against "/libpthread".  This can lead to false
   positives, but this should be good enough in practice.

   As of glibc-2.34, functions formerly residing in libpthread have
   been moved to libc, so "/libc." needs to be checked too.  (Matching
   the "." will avoid matching libraries such as libcrypt.) */

bool
libpthread_name_p (const char *name)
{
  return (strstr (name, "/libpthread") != NULL
	  || strstr (name, "/libc.") != NULL);
}

/* Return non-zero if SO is the libpthread shared library.  */

static bool
libpthread_solib_p (const solib &so)
{
  return libpthread_name_p (so.name.c_str ());
}

/* Read in symbolic information for any shared objects whose names
   match PATTERN.  (If we've already read a shared object's symbol
   info, leave it alone.)  If PATTERN is zero, read them all.

   If READSYMS is 0, defer reading symbolic information until later
   but still do any needed low level processing.

   FROM_TTY is described for update_solib_list, above.  */

void
solib_add (const char *pattern, int from_tty, int readsyms)
{
  if (print_symbol_loading_p (from_tty, 0, 0))
    {
      if (pattern != NULL)
	{
	  gdb_printf (_("Loading symbols for shared libraries: %s\n"),
		      pattern);
	}
      else
	gdb_printf (_("Loading symbols for shared libraries.\n"));
    }

  current_program_space->solib_add_generation++;

  if (pattern)
    {
      char *re_err = re_comp (pattern);

      if (re_err)
	error (_("Invalid regexp: %s"), re_err);
    }

  update_solib_list (from_tty);

  /* Walk the list of currently loaded shared libraries, and read
     symbols for any that match the pattern --- or any whose symbols
     aren't already loaded, if no pattern was given.  */
  {
    bool any_matches = false;
    bool loaded_any_symbols = false;
    symfile_add_flags add_flags = SYMFILE_DEFER_BP_RESET;

    if (from_tty)
      add_flags |= SYMFILE_VERBOSE;

    for (solib &gdb : current_program_space->solibs ())
      if (!pattern || re_exec (gdb.name.c_str ()))
	{
	  /* Normally, we would read the symbols from that library
	     only if READSYMS is set.  However, we're making a small
	     exception for the pthread library, because we sometimes
	     need the library symbols to be loaded in order to provide
	     thread support (x86-linux for instance).  */
	  const int add_this_solib = (readsyms || libpthread_solib_p (gdb));

	  any_matches = true;
	  if (add_this_solib)
	    {
	      if (gdb.symbols_loaded)
		{
		  /* If no pattern was given, be quiet for shared
		     libraries we have already loaded.  */
		  if (pattern && (from_tty || info_verbose))
		    gdb_printf (_("Symbols already loaded for %ps\n"),
				styled_string (file_name_style.style (),
					       gdb.name.c_str ()));
		}
	      else if (solib_read_symbols (gdb, add_flags))
		loaded_any_symbols = true;
	    }
	}

    if (loaded_any_symbols || !current_program_space->deleted_solibs.empty ())
      breakpoint_re_set ();

    if (from_tty && pattern && !any_matches)
      gdb_printf ("No loaded shared libraries match the pattern `%s'.\n",
		  pattern);

    if (loaded_any_symbols)
      {
	/* Getting new symbols may change our opinion about what is
	   frameless.  */
	reinit_frame_cache ();
      }
  }
}

/* Helper function for "info sharedlibrary" and "info namespace".
   This receives a list of solibs to be printed, and prints a table
   with all the relevant data.  If PRINT_NAMESPACE is true, figure out
   the solib_ops of the current gdbarch, to calculate the namespace
   that contains an solib.
   Returns true if one or more solibs are missing debug information,
   false otherwise.  */

static void
print_solib_list_table (std::vector<const solib *> solib_list,
			bool print_namespace)
{
  gdbarch *gdbarch = current_inferior ()->arch ();
  /* "0x", a little whitespace, and two hex digits per byte of pointers.  */
  int addr_width = 4 + (gdbarch_ptr_bit (gdbarch) / 4);
  const solib_ops *ops = current_program_space->solib_ops ();
  struct ui_out *uiout = current_uiout;
  bool so_missing_debug_info = false;

  if (ops == nullptr)
    return;

  /* There are 3 conditions for this command to print solib namespaces,
     first PRINT_NAMESPACE has to be true, second the solib_ops has to
     support multiple namespaces, and third there must be more than one
     active namespace.  Fold all these into the PRINT_NAMESPACE condition.  */
  print_namespace = (print_namespace
		     && ops != nullptr
		     && ops->supports_namespaces ()
		     && ops->num_active_namespaces () > 1);

  int num_cols = 4;
  if (print_namespace)
    num_cols++;

  {
    ui_out_emit_table table_emitter (uiout, num_cols, solib_list.size (),
				     "SharedLibraryTable");

    /* The "- 1" is because ui_out adds one space between columns.  */
    uiout->table_header (addr_width - 1, ui_left, "from", "From");
    uiout->table_header (addr_width - 1, ui_left, "to", "To");
    if (print_namespace)
      uiout->table_header (5, ui_left, "namespace", "NS");
    uiout->table_header (12 - 1, ui_left, "syms-read", "Syms Read");
    uiout->table_header (0, ui_noalign, "name", "Shared Object Library");

    uiout->table_body ();

    for (const solib *so : solib_list)
      {
	if (so->name.empty ())
	  continue;

	ui_out_emit_tuple tuple_emitter (uiout, "lib");

	if (so->addr_high != 0)
	  {
	    uiout->field_core_addr ("from", gdbarch, so->addr_low);
	    uiout->field_core_addr ("to", gdbarch, so->addr_high);
	  }
	else
	  {
	    uiout->field_skip ("from");
	    uiout->field_skip ("to");
	  }

	if (print_namespace)
	  {
	    try
	      {
		uiout->field_fmt ("namespace", "[[%d]]", ops->find_solib_ns (*so));
	      }
	    catch (const gdb_exception_error &er)
	      {
		uiout->field_skip ("namespace");
	      }
	  }

	if (!top_level_interpreter ()->interp_ui_out ()->is_mi_like_p ()
	    && so->symbols_loaded && !so->objfile->has_symbols ())
	  {
	    so_missing_debug_info = true;
	    uiout->field_string ("syms-read", "Yes (*)");
	  }
	else
	  uiout->field_string ("syms-read", so->symbols_loaded ? "Yes" : "No");

	uiout->field_string ("name", so->name, file_name_style.style ());

	uiout->text ("\n");
      }
  }

  if (so_missing_debug_info)
    uiout->message (_("(*): Shared library is missing "
		      "debugging information.\n"));
}

/* Implement the "info sharedlibrary" command.  Walk through the
   shared library list and print information about each attached
   library matching PATTERN.  If PATTERN is elided, print them
   all.  */

static void
info_sharedlibrary_command (const char *pattern, int from_tty)
{
  struct ui_out *uiout = current_uiout;

  if (pattern)
    {
      char *re_err = re_comp (pattern);

      if (re_err)
	error (_("Invalid regexp: %s"), re_err);
    }

  update_solib_list (from_tty);

  /* ui_out_emit_table table_emitter needs to know the number of rows,
     so we need to make two passes over the libs.  */

  std::vector<const solib *> print_libs;
  for (const solib &so : current_program_space->solibs ())
    {
      if (!so.name.empty ())
	{
	  if (pattern && !re_exec (so.name.c_str ()))
	    continue;
	  print_libs.push_back (&so);
	}
    }

  print_solib_list_table (print_libs, true);

  if (print_libs.size () == 0)
    {
      if (pattern)
	uiout->message (_("No shared libraries matched.\n"));
      else
	uiout->message (_("No shared libraries loaded at this time.\n"));
    }
}

/* Implement the "info linker-namespaces" command.  If the current
   gdbarch's solib_ops object does not support multiple namespaces,
   this command would just look like "info sharedlibrary", so point
   the user to that command instead.
   If solib_ops does support multiple namespaces, this command
   will group the libraries by linker namespace, or only print the
   libraries in the supplied namespace.  */
static void
info_linker_namespace_command (const char *pattern, int from_tty)
{
  const solib_ops *ops = current_program_space->solib_ops ();

  /* This command only really makes sense for inferiors that support
     linker namespaces, so we can leave early.  */
  if (ops == nullptr || !ops->supports_namespaces ())
    error (_("Current inferior does not support linker namespaces.  "
	     "Use \"info sharedlibrary\" instead."));

  struct ui_out *uiout = current_uiout;
  std::vector<std::pair<int, std::vector<const solib *>>> all_solibs_to_print;

  pattern = skip_spaces (pattern);

  if (pattern == nullptr || pattern[0] == '\0')
    {
      uiout->message (_("There are %d linker namespaces loaded\n"),
		      ops->num_active_namespaces ());

      int printed = 0;
      for (int i = 0; printed < ops->num_active_namespaces (); i++)
	{
	  std::vector<const solib *> solibs_to_print
	    = ops->get_solibs_in_ns (i);
	  if (solibs_to_print.size () > 0)
	    {
	      all_solibs_to_print.push_back (std::make_pair
					      (i, solibs_to_print));
	      printed++;
	    }
	}
    }
  else
    {
      int ns;
      /* Check if the pattern includes the optional [[ and ]] decorators.
	 To match multiple occurrences, '+' needs to be escaped, and every
	 escape sequence must be doubled to survive the compiler pass.  */
      re_comp ("^\\[\\[[0-9]\\+\\]\\]$");
      if (re_exec (pattern))
	ns = strtol (pattern + 2, nullptr, 10);
      else
	{
	  char *end = nullptr;
	  ns = strtol (pattern, &end, 10);
	  if (end[0] != '\0')
	    error (_("Invalid linker namespace identifier: %s"), pattern);
	}

      all_solibs_to_print.push_back
	(std::make_pair (ns, ops->get_solibs_in_ns (ns)));
    }

  bool ns_separator = false;

  for (const auto &[ns, solibs_to_print] : all_solibs_to_print)
    {
      if (ns_separator)
	uiout->message ("\n\n");
      else
	ns_separator = true;

      if (solibs_to_print.size () == 0)
	{
	  uiout->message (_("Linker namespace [[%d]] is not active.\n"), ns);
	  /* If we got here, a specific namespace was requested, so there
	     will only be one vector.  We can leave early.  */
	  break;
	}
      uiout->message
	(_("There are %zu libraries loaded in linker namespace [[%d]]\n"),
	 solibs_to_print.size (), ns);
      uiout->message
	(_("Displaying libraries for linker namespace [[%d]]:\n"), ns);

      print_solib_list_table (solibs_to_print, false);
    }
}

/* See solib.h.  */

bool
solib_contains_address_p (const solib &solib, CORE_ADDR address)
{
  for (const target_section &p : solib.sections)
    if (p.addr <= address && address < p.endaddr)
      return true;

  return false;
}

/* If ADDRESS is in a shared lib in program space PSPACE, return its
   name.

   Provides a hook for other gdb routines to discover whether or not a
   particular address is within the mapped address space of a shared
   library.

   For example, this routine is called at one point to disable
   breakpoints which are in shared libraries that are not currently
   mapped in.  */

const char *
solib_name_from_address (struct program_space *pspace, CORE_ADDR address)
{
  for (const solib &so : pspace->solibs ())
    if (solib_contains_address_p (so, address))
      return so.name.c_str ();

  return nullptr;
}

bool
solib_ops::same (const solib &a, const solib &b) const
{
  return (filename_cmp (a.original_name.c_str (), b.original_name.c_str ())
	  == 0);
}

/* See solib.h.  */

bool
solib_keep_data_in_core (CORE_ADDR vaddr, unsigned long size)
{
  const solib_ops *ops = current_program_space->solib_ops ();

  return ops != nullptr && ops->keep_data_in_core (vaddr, size);
}

/* See solib.h.  */

void
clear_solib (program_space *pspace)
{
  for (solib &so : pspace->solibs ())
    {
      bool still_in_use
	= (so.objfile != nullptr && solib_used (pspace, so));

      notify_solib_unloaded (pspace, so, still_in_use, true);
      pspace->remove_target_sections (&so);
    };

  pspace->solibs ().clear ();

  if (const solib_ops *ops = pspace->solib_ops ();
      ops != nullptr)
    ops->clear_solib (pspace);
}

/* Shared library startup support.  When GDB starts up the inferior,
   it nurses it along (through the shell) until it is ready to execute
   its first instruction.  At this point, this function gets
   called.  */

void
solib_create_inferior_hook (int from_tty)
{
  if (const solib_ops *ops = current_program_space->solib_ops ();
      ops != nullptr)
    ops->create_inferior_hook (from_tty);
}

/* See solib.h.  */

bool
in_solib_dynsym_resolve_code (CORE_ADDR pc)
{
  const solib_ops *ops = current_program_space->solib_ops ();

  return ops != nullptr && ops->in_dynsym_resolve_code (pc);
}

/* Implements the "sharedlibrary" command.  */

static void
sharedlibrary_command (const char *args, int from_tty)
{
  dont_repeat ();
  solib_add (args, from_tty, 1);
}

/* See solib.h.  */

void
no_shared_libraries (program_space *pspace)
{
  /* The order of the two routines below is important: clear_solib notifies
     the solib_unloaded observers, and some of these observers might need
     access to their associated objfiles.  Therefore, we can not purge the
     solibs' objfiles before clear_solib has been called.  */

  clear_solib (pspace);
  objfile_purge_solibs (pspace);
}

/* Implements the command "nosharedlibrary", which discards symbols
   that have been auto-loaded from shared libraries.  Symbols from
   shared libraries that were added by explicit request of the user
   are not discarded.  */

static void
no_shared_libraries_command (const char *ignored, int from_tty)
{
  no_shared_libraries (current_program_space);
}

/* See solib.h.  */

void
update_solib_breakpoints (void)
{
  const solib_ops *ops = current_program_space->solib_ops ();

  if (ops != nullptr)
    ops->update_breakpoints ();
}

/* See solib.h.  */

void
handle_solib_event (void)
{
  if (const solib_ops *ops = current_program_space->solib_ops ();
      ops != nullptr)
    ops->handle_event ();

  current_inferior ()->pspace->clear_solib_cache ();

  /* Check for any newly added shared libraries if we're supposed to
     be adding them automatically.  Switch terminal for any messages
     produced by breakpoint_re_set.  */
  target_terminal::ours_for_output ();
  solib_add (NULL, 0, auto_solib_add);
  target_terminal::inferior ();
}

/* Reload shared libraries, but avoid reloading the same symbol file
   we already have loaded.  */

static void
reload_shared_libraries_1 (int from_tty)
{
  if (print_symbol_loading_p (from_tty, 0, 0))
    gdb_printf (_("Loading symbols for shared libraries.\n"));

  for (solib &so : current_program_space->solibs ())
    {
      const char *found_pathname = NULL;
      bool was_loaded = so.symbols_loaded;
      symfile_add_flags add_flags = SYMFILE_DEFER_BP_RESET;

      if (from_tty)
	add_flags |= SYMFILE_VERBOSE;

      gdb::unique_xmalloc_ptr<char> filename (
	tilde_expand (so.original_name.c_str ()));

      gdb_bfd_ref_ptr abfd = so.ops ().bfd_open (filename.get ());
      if (abfd != NULL)
	found_pathname = bfd_get_filename (abfd.get ());

      /* If this shared library is no longer associated with its previous
	 symbol file, close that.  */
      if ((found_pathname == NULL && was_loaded)
	  || (found_pathname != NULL
	      && filename_cmp (found_pathname, so.name.c_str ()) != 0))
	{
	  if (so.objfile && !(so.objfile->flags & OBJF_USERLOADED)
	      && !solib_used (current_program_space, so))
	    so.objfile->unlink ();
	  current_program_space->remove_target_sections (&so);
	  so.clear ();
	}

      /* If this shared library is now associated with a new symbol
	 file, open it.  */
      if (found_pathname != NULL
	  && (!was_loaded
	      || filename_cmp (found_pathname, so.name.c_str ()) != 0))
	{
	  bool got_error = false;

	  try
	    {
	      solib_map_sections (so);
	    }

	  catch (const gdb_exception_error &e)
	    {
	      exception_fprintf (gdb_stderr, e,
				 _("Error while mapping "
				   "shared library sections:\n"));
	      got_error = true;
	    }

	  if (!got_error
	      && (auto_solib_add || was_loaded || libpthread_solib_p (so)))
	    solib_read_symbols (so, add_flags);
	}
    }
}

static void
reload_shared_libraries (const char *ignored, int from_tty,
			 struct cmd_list_element *e)
{
  reload_shared_libraries_1 (from_tty);

  /* Creating inferior hooks here has two purposes.  First, if we reload 
     shared libraries then the address of solib breakpoint we've computed
     previously might be no longer valid.  For example, if we forgot to set
     solib-absolute-prefix and are setting it right now, then the previous
     breakpoint address is plain wrong.  Second, installing solib hooks
     also implicitly figures were ld.so is and loads symbols for it.
     Absent this call, if we've just connected to a target and set 
     solib-absolute-prefix or solib-search-path, we'll lose all information
     about ld.so.  */
  if (target_has_execution ())
    {
      /* Reset or free private data structures not associated with
	 solib entries.  */
      if (const solib_ops *ops = current_program_space->solib_ops ();
	  ops != nullptr)
	ops->clear_solib (current_program_space);

      /* Remove any previous solib event breakpoint.  This is usually
	 done in common code, at breakpoint_init_inferior time, but
	 we're not really starting up the inferior here.  */
      remove_solib_event_breakpoints ();

      solib_create_inferior_hook (from_tty);
    }

  /* Sometimes the platform-specific hook loads initial shared
     libraries, and sometimes it doesn't.  If it doesn't FROM_TTY will be
     incorrectly 0 but such solib targets should be fixed anyway.  If we
     made all the inferior hook methods consistent, this call could be
     removed.  Call it only after the solib target has been initialized by
     solib_create_inferior_hook.  */

  solib_add (NULL, 0, auto_solib_add);

  breakpoint_re_set ();

  /* We may have loaded or unloaded debug info for some (or all)
     shared libraries.  However, frames may still reference them.  For
     example, a frame's unwinder might still point at DWARF FDE
     structures that are now freed.  Also, getting new symbols may
     change our opinion about what is frameless.  */
  reinit_frame_cache ();
}

/* Wrapper for reload_shared_libraries that replaces "remote:"
   at the start of gdb_sysroot with "target:".  */

static void
gdb_sysroot_changed (const char *ignored, int from_tty,
		     struct cmd_list_element *e)
{
  const char *old_prefix = "remote:";
  const char *new_prefix = TARGET_SYSROOT_PREFIX;

  if (startswith (gdb_sysroot.c_str (), old_prefix))
    {
      static bool warning_issued = false;

      gdb_assert (strlen (old_prefix) == strlen (new_prefix));
      gdb_sysroot = new_prefix + gdb_sysroot.substr (strlen (old_prefix));

      if (!warning_issued)
	{
	  warning (_("\"%s\" is deprecated, use \"%s\" instead."), old_prefix,
		   new_prefix);
	  warning (_("sysroot set to \"%s\"."), gdb_sysroot.c_str ());

	  warning_issued = true;
	}
    }

  reload_shared_libraries (ignored, from_tty, e);
}

static void
show_auto_solib_add (struct ui_file *file, int from_tty,
		     struct cmd_list_element *c, const char *value)
{
  gdb_printf (file, _("Autoloading of shared library symbols is %s.\n"),
	      value);
}

/* Lookup the value for a specific symbol from dynamic symbol table.  Look
   up symbol from ABFD.  MATCH_SYM is a callback function to determine
   whether to pick up a symbol.  DATA is the input of this callback
   function.  Return 0 if symbol is not found.  */

CORE_ADDR
gdb_bfd_lookup_symbol_from_symtab (
  bfd *abfd, gdb::function_view<bool (const asymbol *)> match_sym)
{
  CORE_ADDR symaddr = 0;
  gdb::array_view<asymbol *> symbol_table
    = gdb_bfd_canonicalize_symtab (abfd, false);

  for (asymbol *sym : symbol_table)
    {
      if (match_sym (sym))
	{
	  gdbarch *gdbarch = current_inferior ()->arch ();
	  symaddr = sym->value;

	  /* Some ELF targets fiddle with addresses of symbols they
	     consider special.  They use minimal symbols to do that
	     and this is needed for correct breakpoint placement,
	     but we do not have full data here to build a complete
	     minimal symbol, so just set the address and let the
	     targets cope with that.  */
	  if (bfd_get_flavour (abfd) == bfd_target_elf_flavour
	      && gdbarch_elf_make_msymbol_special_p (gdbarch))
	    {
	      struct minimal_symbol msym
		{
		};

	      msym.set_value_address (symaddr);
	      gdbarch_elf_make_msymbol_special (gdbarch, sym, &msym);
	      symaddr = CORE_ADDR (msym.unrelocated_address ());
	    }

	  /* BFD symbols are section relative.  */
	  symaddr += sym->section->vma;
	  break;
	}
    }

  return symaddr;
}

/* See solib.h.  */

int
gdb_bfd_scan_elf_dyntag (const int desired_dyntag, bfd *abfd, CORE_ADDR *ptr,
			 CORE_ADDR *ptr_addr)
{
  int arch_size, step;
  bfd_size_type sect_size;
  long current_dyntag;
  CORE_ADDR dyn_ptr, dyn_addr;
  gdb_byte *bufend, *bufstart, *buf;
  Elf32_External_Dyn *x_dynp_32;
  Elf64_External_Dyn *x_dynp_64;
  struct bfd_section *sect;

  if (abfd == NULL)
    return 0;

  if (bfd_get_flavour (abfd) != bfd_target_elf_flavour)
    return 0;

  arch_size = bfd_get_arch_size (abfd);
  if (arch_size == -1)
    return 0;

  /* Find the start address of the .dynamic section.  */
  sect = bfd_get_section_by_name (abfd, ".dynamic");
  if (sect == NULL)
    return 0;

  bool found = false;
  for (const target_section &target_section :
       current_program_space->target_sections ())
    if (sect == target_section.the_bfd_section)
      {
	dyn_addr = target_section.addr;
	found = true;
	break;
      }
  if (!found)
    {
      /* ABFD may come from OBJFILE acting only as a symbol file without being
	 loaded into the target (see add_symbol_file_command).  This case is
	 such fallback to the file VMA address without the possibility of
	 having the section relocated to its actual in-memory address.  */

      dyn_addr = bfd_section_vma (sect);
    }

  /* Read in .dynamic from the BFD.  We will get the actual value
     from memory later.  */
  sect_size = bfd_section_size (sect);
  gdb::byte_vector buffer (sect_size);
  buf = bufstart = buffer.data ();
  if (!bfd_get_section_contents (abfd, sect,
				 buf, 0, sect_size))
    return 0;

  /* Iterate over BUF and scan for DYNTAG.  If found, set PTR and return.  */
  step = (arch_size == 32) ? sizeof (Elf32_External_Dyn)
			   : sizeof (Elf64_External_Dyn);
  for (bufend = buf + sect_size; buf < bufend; buf += step)
    {
      if (arch_size == 32)
	{
	  x_dynp_32 = (Elf32_External_Dyn *) buf;
	  current_dyntag = bfd_h_get_32 (abfd, (bfd_byte *) x_dynp_32->d_tag);
	  dyn_ptr = bfd_h_get_32 (abfd, (bfd_byte *) x_dynp_32->d_un.d_ptr);
	}
      else
	{
	  x_dynp_64 = (Elf64_External_Dyn *) buf;
	  current_dyntag = bfd_h_get_64 (abfd, (bfd_byte *) x_dynp_64->d_tag);
	  dyn_ptr = bfd_h_get_64 (abfd, (bfd_byte *) x_dynp_64->d_un.d_ptr);
	}
      if (current_dyntag == DT_NULL)
	return 0;
      if (current_dyntag == desired_dyntag)
	{
	  /* If requested, try to read the runtime value of this .dynamic
	   entry.  */
	  if (ptr)
	    {
	      struct type *ptr_type;
	      gdb_byte ptr_buf[8];
	      CORE_ADDR ptr_addr_1;

	      ptr_type = builtin_type (current_inferior ()->arch ())
			   ->builtin_data_ptr;
	      ptr_addr_1 = dyn_addr + (buf - bufstart) + arch_size / 8;
	      if (target_read_memory (ptr_addr_1, ptr_buf, arch_size / 8) == 0)
		dyn_ptr = extract_typed_address (ptr_buf, ptr_type);
	      *ptr = dyn_ptr;
	      if (ptr_addr)
		*ptr_addr = dyn_addr + (buf - bufstart);
	    }
	  return 1;
	}
    }

  return 0;
}

/* See solib.h.  */

gdb::unique_xmalloc_ptr<char>
gdb_bfd_read_elf_soname (const char *filename)
{
  gdb_bfd_ref_ptr abfd = gdb_bfd_open (filename, gnutarget);

  if (abfd == nullptr)
    return {};

  /* Check that ABFD is an ET_DYN ELF file.  */
  if (!bfd_check_format (abfd.get (), bfd_object)
      || !(bfd_get_file_flags (abfd.get ()) & DYNAMIC))
    return {};

  CORE_ADDR idx;
  if (!gdb_bfd_scan_elf_dyntag (DT_SONAME, abfd.get (), &idx, nullptr))
    return {};

  struct bfd_section *dynstr
    = bfd_get_section_by_name (abfd.get (), ".dynstr");
  int sect_size = bfd_section_size (dynstr);
  if (dynstr == nullptr || sect_size <= idx)
    return {};

  /* Read soname from the string table.  */
  gdb::byte_vector dynstr_buf;
  if (!gdb_bfd_get_full_section_contents (abfd.get (), dynstr, &dynstr_buf))
    return {};

  /* Ensure soname is null-terminated before returning a copy.  */
  char *soname = (char *) dynstr_buf.data () + idx;
  if (strnlen (soname, sect_size - idx) == sect_size - idx)
    return {};

  return make_unique_xstrdup (soname);
}

/* Lookup the value for a specific symbol from symbol table.  Look up symbol
   from ABFD.  MATCH_SYM is a callback function to determine whether to pick
   up a symbol.  DATA is the input of this callback function.  Return 0
   if symbol is not found.  */

static CORE_ADDR
bfd_lookup_symbol_from_dyn_symtab
  (bfd *abfd, gdb::function_view<bool (const asymbol *)> match_sym)
{
  long storage_needed = bfd_get_dynamic_symtab_upper_bound (abfd);
  CORE_ADDR symaddr = 0;

  if (storage_needed > 0)
    {
      unsigned int i;
      gdb::def_vector<asymbol *> storage (storage_needed / sizeof (asymbol *));
      asymbol **symbol_table = storage.data ();
      unsigned int number_of_symbols
	= bfd_canonicalize_dynamic_symtab (abfd, symbol_table);

      for (i = 0; i < number_of_symbols; i++)
	{
	  asymbol *sym = *symbol_table++;

	  if (match_sym (sym))
	    {
	      /* BFD symbols are section relative.  */
	      symaddr = sym->value + sym->section->vma;
	      break;
	    }
	}
    }
  return symaddr;
}

/* Lookup the value for a specific symbol from symbol table and dynamic
   symbol table.  Look up symbol from ABFD.  MATCH_SYM is a callback
   function to determine whether to pick up a symbol.  DATA is the
   input of this callback function.  Return 0 if symbol is not
   found.  */

CORE_ADDR
gdb_bfd_lookup_symbol (bfd *abfd,
		       gdb::function_view<bool (const asymbol *)> match_sym)
{
  CORE_ADDR symaddr = gdb_bfd_lookup_symbol_from_symtab (abfd, match_sym);

  /* On FreeBSD, the dynamic linker is stripped by default.  So we'll
     have to check the dynamic string table too.  */
  if (symaddr == 0)
    symaddr = bfd_lookup_symbol_from_dyn_symtab (abfd, match_sym);

  return symaddr;
}

/* The shared library list may contain user-loaded object files that
   can be removed out-of-band by the user.  So upon notification of
   free_objfile remove all references to any user-loaded file that is
   about to be freed.  */

static void
remove_user_added_objfile (struct objfile *objfile)
{
  if (objfile->flags & OBJF_USERLOADED)
    {
      for (solib &so : objfile->pspace ()->solibs ())
	if (so.objfile == objfile)
	  so.objfile = nullptr;
    }
}

/* Implementation of the linker_namespace convenience variable.

   This returns the GDB internal identifier of the linker namespace,
   for the selected frame, as an integer.  If the inferior doesn't support
   linker namespaces, this always returns 0.  */

static value *
linker_namespace_make_value (gdbarch *gdbarch, internalvar *var,
				     void *ignore)
{
  int nsid = 0;
  CORE_ADDR curr_pc = get_frame_pc (get_selected_frame ());

  for (const solib &so : current_program_space->solibs ())
    if (solib_contains_address_p (so, curr_pc))
      {
	if (so.ops ().supports_namespaces ())
	  nsid = so.ops ().find_solib_ns (so);

	break;
      }

  /* If the PC is not in an SO, or the solib_ops doesn't support
     linker namespaces, the inferior is in the default namespace.  */
  return value_from_longest (builtin_type (gdbarch)->builtin_int, nsid);
}

/* Implementation of `$_linker_namespace' variable.  */

static const struct internalvar_funcs linker_namespace_funcs =
{
  linker_namespace_make_value,
  nullptr,
};

INIT_GDB_FILE (solib)
{
  gdb::observers::free_objfile.attach (remove_user_added_objfile, "solib");
  gdb::observers::inferior_execd.attach (
    [] (inferior *exec_inf, inferior *follow_inf) {
    solib_create_inferior_hook (0);
  },
    "solib");

  /* Convenience variables for debugging linker namespaces.  These are
     set here, even if the solib_ops doesn't support them,
     for consistency.  */
  create_internalvar_type_lazy ("_linker_namespace",
				&linker_namespace_funcs, nullptr);
  set_internalvar_integer (lookup_internalvar ("_active_linker_namespaces"), 1);

  add_com (
    "sharedlibrary", class_files, sharedlibrary_command,
    _("Load shared object library symbols for files matching REGEXP."));
  cmd_list_element *info_sharedlibrary_cmd
    = add_info ("sharedlibrary", info_sharedlibrary_command,
		_("Status of loaded shared object libraries."));
  add_info_alias ("dll", info_sharedlibrary_cmd, 1);
  add_com ("nosharedlibrary", class_files, no_shared_libraries_command,
	   _("Unload all shared object library symbols."));

  add_info ("linker-namespaces", info_linker_namespace_command,
      _("Get information about linker namespaces in the inferior."));

  add_setshow_boolean_cmd ("auto-solib-add", class_support, &auto_solib_add,
			   _("\
Set autoloading of shared library symbols."),
			   _("\
Show autoloading of shared library symbols."),
			   _("\
If \"on\", symbols from all shared object libraries will be loaded\n\
automatically when the inferior begins execution, when the dynamic linker\n\
informs gdb that a new library has been loaded, or when attaching to the\n\
inferior.  Otherwise, symbols must be loaded manually, using \
`sharedlibrary'."),
			   NULL, show_auto_solib_add, &setlist, &showlist);

  set_show_commands sysroot_cmds
    = add_setshow_optional_filename_cmd ("sysroot", class_support,
					 &gdb_sysroot, _("\
Set an alternate system root."),
					 _("\
Show the current system root."),
					 _("\
The system root is used to load absolute shared library symbol files.\n\
For other (relative) files, you can add directories using\n\
`set solib-search-path'."),
					 gdb_sysroot_changed, NULL, &setlist,
					 &showlist);

  add_alias_cmd ("solib-absolute-prefix", sysroot_cmds.set, class_support, 0,
		 &setlist);
  add_alias_cmd ("solib-absolute-prefix", sysroot_cmds.show, class_support, 0,
		 &showlist);

  add_setshow_optional_filename_cmd ("solib-search-path", class_support,
				     &solib_search_path, _("\
Set the search path for loading non-absolute shared library symbol files."),
				     _("\
Show the search path for loading non-absolute shared library symbol files."),
				     _("\
This takes precedence over the environment variables \
PATH and LD_LIBRARY_PATH."),
				     reload_shared_libraries,
				     show_solib_search_path, &setlist,
				     &showlist);

  add_setshow_boolean_cmd ("solib", class_maintenance, &debug_solib, _("\
Set solib debugging."),
			   _("\
Show solib debugging."),
			   _("\
When true, solib-related debugging output is enabled."),
			   nullptr, nullptr, &setdebuglist, &showdebuglist);
}
