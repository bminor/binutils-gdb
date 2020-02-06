/* debuginfod utilities for GDB.
   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef DEBUGINFOD_SUPPORT_H
#define DEBUGINFOD_SUPPORT_H

/* Query debuginfod servers for a source file associated with an
   an executable with build_id. src_path should be the source
   file's absolute path that includes the compilation directory of
   the CU associated with the source file. If the file is
   successfully retrieved, its path on the local machine is stored
   at filename. The caller should free() this value. If GDB is not
   built with debuginfod, this function returns -ENOSYS.  */

extern int debuginfod_source_query (const unsigned char *build_id,
                                    int build_id_len,
                                    const char *src_path,
                                    char **filename);

/* Query debuginfod servers for a debuginfo file with build_id. If the
   file is successfully retrieved, its path on the local machine is
   stored at filename. The caller should free() this value. If GDB
   is not built with debuginfod, this function returns -ENOSYS.  */

extern int debuginfod_debuginfo_query (const unsigned char *build_id,
                                       int build_id_len,
                                       char **filename);
#endif /* DEBUGINFOD_SUPPORT_H */
