/* Python interface to architecture

   Copyright (C) 2013 Free Software Foundation, Inc.

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
#include "gdbarch.h"
#include "arch-utils.h"
#include "python-internal.h"

typedef struct arch_object_type_object {
  PyObject_HEAD
  struct gdbarch *gdbarch;
} arch_object;

static struct gdbarch_data *arch_object_data = NULL;
static PyTypeObject arch_object_type;

/* Associates an arch_object with GDBARCH as gdbarch_data via the gdbarch
   post init registration mechanism (gdbarch_data_register_post_init).  */

static void *
arch_object_data_init (struct gdbarch *gdbarch)
{
  arch_object *arch_obj = PyObject_New (arch_object, &arch_object_type);

  if (arch_obj == NULL)
    return NULL;

  arch_obj->gdbarch = gdbarch;

  return (void *) arch_obj;
}

/* Returns the struct gdbarch value corresponding to the given Python
   architecture object OBJ.  */

struct gdbarch *
arch_object_to_gdbarch (PyObject *obj)
{
  arch_object *py_arch = (arch_object *) obj;

  return py_arch->gdbarch;
}

/* Returns the Python architecture object corresponding to GDBARCH.
   Returns a new reference to the arch_object associated as data with
   GDBARCH.  */

PyObject *
gdbarch_to_arch_object (struct gdbarch *gdbarch)
{
  PyObject *new_ref = (PyObject *) gdbarch_data (gdbarch, arch_object_data);

  /* new_ref could be NULL if registration of arch_object with GDBARCH failed
     in arch_object_data_init.  */
  Py_XINCREF (new_ref);

  return new_ref;
}

/* Implementation of gdb.Architecture.name (self) -> String.
   Returns the name of the architecture as a string value.  */

static PyObject *
archpy_name (PyObject *self, PyObject *args)
{
  struct gdbarch *gdbarch = arch_object_to_gdbarch (self);
  const char *name = (gdbarch_bfd_arch_info (gdbarch))->printable_name;
  PyObject *py_name = PyString_FromString (name);

  return py_name;
}

/* Initializes the Architecture class in the gdb module.  */

void
gdbpy_initialize_arch (void)
{
  arch_object_data = gdbarch_data_register_post_init (arch_object_data_init);
  arch_object_type.tp_new = PyType_GenericNew;
  if (PyType_Ready (&arch_object_type) < 0)
    return;

  Py_INCREF (&arch_object_type);
  PyModule_AddObject (gdb_module, "Architecture",
                      (PyObject *) &arch_object_type);
}

static PyMethodDef arch_object_methods [] = {
  { "name", archpy_name, METH_NOARGS,
    "name () -> String.\n\
Return the name of the architecture as a string value." },
  {NULL}  /* Sentinel */
};

static PyTypeObject arch_object_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
  "gdb.Architecture",                 /* tp_name */
  sizeof (arch_object),               /* tp_basicsize */
  0,                                  /* tp_itemsize */
  0,                                  /* tp_dealloc */
  0,                                  /* tp_print */
  0,                                  /* tp_getattr */
  0,                                  /* tp_setattr */
  0,                                  /* tp_compare */
  0,                                  /* tp_repr */
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_sequence */
  0,                                  /* tp_as_mapping */
  0,                                  /* tp_hash  */
  0,                                  /* tp_call */
  0,                                  /* tp_str */
  0,                                  /* tp_getattro */
  0,                                  /* tp_setattro */
  0,                                  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,                 /* tp_flags */
  "GDB architecture object",          /* tp_doc */
  0,                                  /* tp_traverse */
  0,                                  /* tp_clear */
  0,                                  /* tp_richcompare */
  0,                                  /* tp_weaklistoffset */
  0,                                  /* tp_iter */
  0,                                  /* tp_iternext */
  arch_object_methods,                /* tp_methods */
  0,                                  /* tp_members */
  0,                                  /* tp_getset */
  0,                                  /* tp_base */
  0,                                  /* tp_dict */
  0,                                  /* tp_descr_get */
  0,                                  /* tp_descr_set */
  0,                                  /* tp_dictoffset */
  0,                                  /* tp_init */
  0,                                  /* tp_alloc */
};
