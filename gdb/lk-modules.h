/* Handle kernel modules as shared libraries.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef __LK_MODULES_H__
#define __LK_MODULES_H__

extern struct target_so_ops *lk_modules_so_ops;

/* Check if debug info for module NAME are loaded.  Needed by lsmod command.  */

extern bool lk_modules_debug_info_loaded (const std::string &name);

#endif /*  __LK_MODULES_H__  */
