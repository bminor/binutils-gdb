/* Python interface to core files.

   Copyright (C) 2025 Free Software Foundation, Inc.

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

#include "python-internal.h"
#include "progspace.h"
#include "observable.h"
#include "inferior.h"

/* A gdb.Corefile object.  */

struct corefile_object
{
  PyObject_HEAD

  /* The inferior this core file is attached to.  This will be set to NULL
     when the inferior is deleted, or if a different core file is loaded
     for the inferior.  When this is NULL the gdb.Corefile object is
     considered invalid.*/
  struct inferior *inferior;

  /* Dictionary holding user-added attributes.  This is the __dict__
     attribute of the object.  This is an owning reference.  */
  PyObject *dict;
};

extern PyTypeObject corefile_object_type
    CPYCHECKER_TYPE_OBJECT_FOR_TYPEDEF ("corefile_object");

/* Clear the inferior pointer in a Corefile object OBJ when an inferior is
   deleted.  */

struct inferior_corefile_deleter
{
  void operator() (corefile_object *obj)
  {
    if (!gdb_python_initialized)
      return;

    gdbpy_enter enter_py;

    /* When OBJECT goes out of scope this will Py_DECREF on OBJ.  */
    gdbpy_ref<corefile_object> object (obj);

    /* Clearing the inferior pointer marks the gdb.Corefile as invalid.  */
    object->inferior = nullptr;
  }
};

/* Store a gdb.Corefile object in an inferior's registry.  */

static const registry<inferior>::key<corefile_object,
				     inferior_corefile_deleter>
     cfpy_inferior_corefile_data_key;

/* See python-internal.h.  */

gdbpy_ref<>
gdbpy_core_file_from_inferior (inferior *inf)
{
  gdb_assert (inf != nullptr);
  gdb_assert (inf->pspace != nullptr);

  program_space *pspace = inf->pspace;

  if (pspace->core_bfd () == nullptr)
    return gdbpy_ref<>::new_reference (Py_None);

  PyObject *result = (PyObject *) cfpy_inferior_corefile_data_key.get (inf);
  if (result == nullptr)
    {
      gdbpy_ref<corefile_object> object
	(PyObject_New (corefile_object, &corefile_object_type));
      if (object == nullptr)
	return nullptr;

      /* Ensure the 'inferior' field is set to NULL.  If the PyDict_New
	 call fails then the gdb.Corefile will be discarded and
	 cfpy_dealloc will be called, which requires that the 'inferior' be
	 set to NULL.  */
      object->inferior = nullptr;
      object->dict = PyDict_New ();
      if (object->dict == nullptr)
	return nullptr;

      /* Now that the gdb.Corefile has been successfully initialised and we
	 know that it is going to be passed back to the user, move it out
	 of the invalid state by setting the 'inferior' field to a non NULL
	 value.  */
      object->inferior = inf;
      cfpy_inferior_corefile_data_key.set (inf, object.get ());
      result = (PyObject *) object.release ();
    }

  return gdbpy_ref<>::new_reference (result);
}

/* Return true if OBJ is valid.  */

static bool
cfpy_corefile_object_is_valid (const corefile_object *obj)
{
  if (obj->inferior == nullptr)
    return false;

  gdb_assert (obj->inferior->pspace != nullptr);

  return obj->inferior->pspace->core_bfd () != nullptr;
}

/* Require that COREFILE_OBJ be a valid core file.  A valid core file
   object has a valid program space, and the program space has a core file
   loaded into it.  */
#define CFPY_REQUIRE_VALID(corefile_obj)				\
  do {									\
    if (!cfpy_corefile_object_is_valid (corefile_obj))			\
      {									\
	PyErr_SetString (PyExc_RuntimeError,				\
			 _("Corefile no longer exists."));		\
	return nullptr;							\
      }									\
  } while (0)

/* Read the gdb.Corefile.filename attribute.  */

static PyObject *
cfpy_get_filename (PyObject *self, void *closure)
{
  corefile_object *obj = (corefile_object *) self;

  CFPY_REQUIRE_VALID (obj);

  /* If the program space's core file had been cleared, then this Corefile
     object would have been invalidated.  */
  bfd *abfd = obj->inferior->pspace->core_bfd ();
  gdb_assert (abfd != nullptr);

  return host_string_to_python_string (bfd_get_filename (abfd)).release ();
}

/* Implementation of gdb.Corefile.is_valid (self) -> Boolean.
   Returns True if this core file object is associated with a program space
   that still exists, an the program space still has a core file loaded.  */

static PyObject *
cfpy_is_valid (PyObject *self, PyObject *args)
{
  corefile_object *obj = (corefile_object *) self;

  if (!cfpy_corefile_object_is_valid (obj))
    Py_RETURN_FALSE;

  Py_RETURN_TRUE;
}

/* Callback from gdb::observers::core_file_changed.  The core file in
   PSPACE has been changed.  */

static void
cfpy_corefile_changed (inferior *inf)
{
  cfpy_inferior_corefile_data_key.clear (inf);
}

/* Called when a gdb.Corefile is destroyed.  */

static void
cfpy_dealloc (PyObject *obj)
{
  corefile_object *corefile = (corefile_object *) obj;

  /* Every gdb.Corefile is cached in an inferior's registry.  The only way
     for a gdb.Corefile to be deallocated is to remove the object reference
     from the registry (and dec its ref count), but before we do that, we
     set the object's inferior pointer to NULL.  */
  gdb_assert (corefile->inferior == nullptr);

  Py_XDECREF (corefile->dict);

  Py_TYPE (obj)->tp_free (obj);
}

/* __repr__ implementation for gdb.Corefile.  */

static PyObject *
cfpy_repr (PyObject *self)
{
  corefile_object *obj = (corefile_object *) self;

  if (!cfpy_corefile_object_is_valid (obj))
    return gdb_py_invalid_object_repr (self);

  program_space *pspace = obj->inferior->pspace;
  gdb_assert (pspace != nullptr);
  return PyUnicode_FromFormat ("<%s inferior=%d filename='%s'>",
			       Py_TYPE (self)->tp_name,
			       obj->inferior->num,
			       bfd_get_filename (pspace->core_bfd ()));
}



static int
gdbpy_initialize_corefile ()
{
  gdb::observers::core_file_changed.attach (cfpy_corefile_changed,
					    "py-corefile");

  if (gdbpy_type_ready (&corefile_object_type) < 0)
    return -1;

  return 0;
}

GDBPY_INITIALIZE_FILE (gdbpy_initialize_corefile);



static gdb_PyGetSetDef corefile_getset[] =
{
  { "__dict__", gdb_py_generic_dict, nullptr,
    "The __dict__ for the gdb.Corefile.", &corefile_object_type },
  { "filename", cfpy_get_filename, nullptr,
    "The filename of a valid Corefile object.", nullptr },
  { nullptr }
};

static PyMethodDef corefile_object_methods[] =
{
  { "is_valid", cfpy_is_valid, METH_NOARGS,
    "is_valid () -> Boolean.\n\
Return true if this Corefile is valid, false if not." },
  { nullptr }
};

PyTypeObject corefile_object_type =
{
  PyVarObject_HEAD_INIT (nullptr, 0)
  "gdb.Corefile",		  /*tp_name*/
  sizeof (corefile_object),	  /*tp_basicsize*/
  0,				  /*tp_itemsize*/
  cfpy_dealloc,			  /*tp_dealloc*/
  0,				  /*tp_print*/
  0,				  /*tp_getattr*/
  0,				  /*tp_setattr*/
  0,				  /*tp_compare*/
  cfpy_repr,			  /*tp_repr*/
  0,				  /*tp_as_number*/
  0,				  /*tp_as_sequence*/
  0,				  /*tp_as_mapping*/
  0,				  /*tp_hash */
  0,				  /*tp_call*/
  0,				  /*tp_str*/
  0,				  /*tp_getattro*/
  0,				  /*tp_setattro*/
  0,				  /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,		  /*tp_flags*/
  "GDB corefile object",	  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  corefile_object_methods,	  /* tp_methods */
  0,				  /* tp_members */
  corefile_getset,		  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  offsetof (corefile_object, dict), /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new */
};
