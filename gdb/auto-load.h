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

#ifndef AUTO_LOAD_H
#define AUTO_LOAD_H 1

struct program_space;

extern int gdbpy_global_auto_load;

extern struct auto_load_pspace_info *
  get_auto_load_pspace_data_for_loading (struct program_space *pspace);
extern int maybe_add_script (struct auto_load_pspace_info *pspace_info,
			     const char *name, const char *full_path);
extern void auto_load_objfile_script (struct objfile *objfile,
				      const char *suffix);
extern int
  script_not_found_warning_print (struct auto_load_pspace_info *pspace_info);

#endif /* AUTO_LOAD_H */
