/* Support for GDB-managed terminals.

   Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef MANAGED_TTY_H
#define MANAGED_TTY_H

/* Using host-dependent code here is fine, because the support for
   creating terminals for inferiors is meant to be used with
   child/native targets.  Note we disable the feature on no-MMU
   architectures for now because those can't use fork, see
   fork-inferior.c.  */
#if (defined (__linux__)				\
     && !(defined (__UCLIBC__) && defined (HAS_NOMMU)))
# define GDB_MANAGED_TERMINALS 1
#else
# define GDB_MANAGED_TERMINALS 0
#endif

extern bool debug_managed_tty;

/* Print a "managed-tty" debug statement.  */

#define managed_tty_debug_printf(fmt, ...) \
  debug_prefixed_printf_cond (debug_managed_tty, "managed-tty",fmt, ##__VA_ARGS__)

#endif /* MANAGED_TTY_H */
