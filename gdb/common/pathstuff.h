/* Path manipulation routines for GDB and gdbserver.

   Copyright (C) 1986-2018 Free Software Foundation, Inc.

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

#ifndef PATHSTUFF_H
#define PATHSTUFF_H

/* Path utilities.  */

/* Return the real path of FILENAME, expanding all the symbolic links.

   Contrary to "gdb_abspath", this function does not use
   CURRENT_DIRECTORY for path expansion.  Instead, it relies on the
   current working directory (CWD) of GDB or gdbserver.  */

extern gdb::unique_xmalloc_ptr<char> gdb_realpath (const char *filename);

/* Return a copy of FILENAME, with its directory prefix canonicalized
   by gdb_realpath.  */

extern gdb::unique_xmalloc_ptr<char>
  gdb_realpath_keepfile (const char *filename);

/* Return PATH in absolute form, performing tilde-expansion if necessary.
   PATH cannot be NULL or the empty string.
   This does not resolve symlinks however, use gdb_realpath for that.

   Contrary to "gdb_realpath", this function uses CURRENT_DIRECTORY
   for the path expansion.  This may lead to scenarios the current
   working directory (CWD) is different than CURRENT_DIRECTORY.  */

extern gdb::unique_xmalloc_ptr<char> gdb_abspath (const char *path);

/* Return whether PATH contains a directory separator character.  */

extern bool contains_dir_separator (const char *path);

#endif /* PATHSTUFF_H */
