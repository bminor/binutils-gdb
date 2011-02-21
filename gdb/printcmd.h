/* Print values for GNU debugger GDB.

   Copyright (C) 2011 Free Software Foundation, Inc.

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

#ifndef _PRINTCMD_H_
#define _PRINTCMD_H_

extern void printf_command (char *arg, int from_tty);
typedef void (printf_callback) (char *fbuf, char **expp, void *loc_v,
				void *aexpr_v);
extern void string_printf (char *arg, struct ui_file *stream,
			   printf_callback callback, void *loc_v,
			   void *aexpr_v);

#endif /* _PRINTCMD_H_ */
