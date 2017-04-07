/* Python interface to record targets.

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

#include "defs.h"
#include "inferior.h"
#include "record.h"
#include "python-internal.h"
#include "py-record-btrace.h"
#include "py-record-full.h"
#include "target.h"

/* Python Record object.  */

typedef struct
{
  PyObject_HEAD

  /* The ptid this object refers to.  */
  ptid_t ptid;

  /* The current recording method.  */
  enum record_method method;
} recpy_record_object;

/* Python Record type.  */

static PyTypeObject recpy_record_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Implementation of record.ptid.  */

static PyObject *
recpy_ptid (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  return gdbpy_create_ptid_object (obj->ptid);
}

/* Implementation of record.method.  */

static PyObject *
recpy_method (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_FULL)
    return recpy_full_method (self, closure);

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_method (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.format.  */

static PyObject *
recpy_format (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_FULL)
    return recpy_full_format (self, closure);

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_format (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.goto (instruction) -> None.  */

static PyObject *
recpy_goto (PyObject *self, PyObject *value)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_goto (self, value);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.replay_position [instruction]  */

static PyObject *
recpy_replay_position (PyObject *self, void *closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_replay_position (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.instruction_history [list].  */

static PyObject *
recpy_instruction_history (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_instruction_history (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.function_call_history [list].  */

static PyObject *
recpy_function_call_history (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_function_call_history (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.begin [instruction].  */

static PyObject *
recpy_begin (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_begin (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.end [instruction].  */

static PyObject *
recpy_end (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_end (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Record method list.  */

static PyMethodDef recpy_record_methods[] = {
  { "goto", recpy_goto, METH_VARARGS,
    "goto (instruction|function_call) -> None.\n\
Rewind to given location."},
  { NULL }
};

/* Record member list.  */

static gdb_PyGetSetDef recpy_record_getset[] = {
  { "ptid", recpy_ptid, NULL, "Current thread.", NULL },
  { "method", recpy_method, NULL, "Current recording method.", NULL },
  { "format", recpy_format, NULL, "Current recording format.", NULL },
  { "replay_position", recpy_replay_position, NULL, "Current replay position.",
    NULL },
  { "instruction_history", recpy_instruction_history, NULL,
    "List of instructions in current recording.", NULL },
  { "function_call_history", recpy_function_call_history, NULL,
    "List of function calls in current recording.", NULL },
  { "begin", recpy_begin, NULL,
    "First instruction in current recording.", NULL },
  { "end", recpy_end, NULL,
    "One past the last instruction in current recording.  This is typically \
the current instruction and is used for e.g. record.goto (record.end).", NULL },
  { NULL }
};

/* Sets up the record API in the gdb module.  */

int
gdbpy_initialize_record (void)
{
  recpy_record_type.tp_new = PyType_GenericNew;
  recpy_record_type.tp_flags = Py_TPFLAGS_DEFAULT;
  recpy_record_type.tp_basicsize = sizeof (recpy_record_object);
  recpy_record_type.tp_name = "gdb.Record";
  recpy_record_type.tp_doc = "GDB record object";
  recpy_record_type.tp_methods = recpy_record_methods;
  recpy_record_type.tp_getset = recpy_record_getset;

  return PyType_Ready (&recpy_record_type);
}

/* Implementation of gdb.start_recording (method) -> gdb.Record.  */

PyObject *
gdbpy_start_recording (PyObject *self, PyObject *args)
{
  const char *method = NULL;
  const char *format = NULL;
  PyObject *ret = NULL;

  if (!PyArg_ParseTuple (args, "|ss", &method, &format))
    return NULL;

  TRY
    {
      record_start (method, format, 0);
      ret = gdbpy_current_recording (self, args);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      gdbpy_convert_exception (except);
    }
  END_CATCH

  return ret;
}

/* Implementation of gdb.current_recording (self) -> gdb.Record.  */

PyObject *
gdbpy_current_recording (PyObject *self, PyObject *args)
{
  recpy_record_object *ret = NULL;

  if (find_record_target () == NULL)
    Py_RETURN_NONE;

  ret = PyObject_New (recpy_record_object, &recpy_record_type);
  ret->ptid = inferior_ptid;
  ret->method = target_record_method (inferior_ptid);

  return (PyObject *) ret;
}

/* Implementation of gdb.stop_recording (self) -> None.  */

PyObject *
gdbpy_stop_recording (PyObject *self, PyObject *args)
{
  PyObject *ret = NULL;

  TRY
    {
      record_stop (0);
      ret = Py_None;
      Py_INCREF (Py_None);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      gdbpy_convert_exception (except);
    }
  END_CATCH

  return ret;
}
