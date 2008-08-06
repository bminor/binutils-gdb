/* Gdb/Python header for private use by Python module.

   Copyright (C) 2008 Free Software Foundation, Inc.

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

#ifndef GDB_PYTHON_INTERNAL_H
#define GDB_PYTHON_INTERNAL_H

/* Python 2.4 doesn't include stdint.h soon enough to get {u,}intptr_t
   needed by pyport.h.  */
#include <stdint.h>

/* /usr/include/features.h on linux systems will define _POSIX_C_SOURCE
   if it sees _GNU_SOURCE (which config.h will define).
   pyconfig.h defines _POSIX_C_SOURCE to a different value than
   /usr/include/features.h does causing compilation to fail.
   To work around this, undef _POSIX_C_SOURCE before we include Python.h.  */
#undef _POSIX_C_SOURCE

#if HAVE_LIBPYTHON2_4
#include "python2.4/Python.h"
/* Py_ssize_t is not defined until 2.5.  */
typedef Py_intptr_t Py_ssize_t;
#elif HAVE_LIBPYTHON2_5
#include "python2.5/Python.h"
#elif HAVE_LIBPYTHON2_6
#include "python2.6/Python.h"
#else
#error "Unable to find usable Python.h"
#endif

struct block;
struct symbol;
struct symtab_and_line;

extern PyObject *gdb_module;

struct cleanup *make_cleanup_py_decref (PyObject *py);

/* Use this after a TRY_EXCEPT to throw the appropriate Python
   exception.  */
#define GDB_PY_HANDLE_EXCEPTION(Exception)				\
    do {								\
      if (Exception.reason < 0)						\
	return PyErr_Format (Exception.reason == RETURN_QUIT		\
			     ? PyExc_KeyboardInterrupt : PyExc_RuntimeError, \
			     "%s", Exception.message);			\
    } while (0)


void gdbpy_print_stack (void);

PyObject *python_string_to_unicode (PyObject *obj);
char *unicode_to_target_string (PyObject *unicode_str);
char *python_string_to_target_string (PyObject *obj);

#endif /* GDB_PYTHON_INTERNAL_H */
