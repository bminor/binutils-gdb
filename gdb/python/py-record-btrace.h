/* Python interface to btrace record targets.

   Copyright 2016-2017 Free Software Foundation, Inc.

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

#ifndef GDB_PY_RECORD_BTRACE_H
#define GDB_PY_RECORD_BTRACE_H

#include "python-internal.h"

/* Implementation of record.method [str].  */
extern PyObject *recpy_bt_method (PyObject *self, void *closure);

/* Implementation of record.format [str].  */
extern PyObject *recpy_bt_format (PyObject *self, void *closure);

/* Implementation of record.goto (instruction) -> None.  */
extern PyObject *recpy_bt_goto (PyObject *self, PyObject *value);

/* Implementation of record.instruction_history [list].  */
extern PyObject *recpy_bt_instruction_history (PyObject *self, void *closure);

/* Implementation of record.function_call_history [list].  */
extern PyObject *recpy_bt_function_call_history (PyObject *self, void *closure);

/* Implementation of record.replay_position [instruction].  */
extern PyObject *recpy_bt_replay_position (PyObject *self, void *closure);

/* Implementation of record.begin [instruction].  */
extern PyObject *recpy_bt_begin (PyObject *self, void *closure);

/* Implementation of record.end [instruction].  */
extern PyObject *recpy_bt_end (PyObject *self, void *closure);

#endif /* GDB_PY_RECORD_BTRACE_H */
