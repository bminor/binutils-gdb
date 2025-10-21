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
#include "gdbcore.h"
#include "gdbsupport/rsp-low.h"

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

  /* A Tuple of gdb.CorefileMappedFile objects.  This tuple is only created
     the first time the user calls gdb.Corefile.mapped_files(), the result
     is cached here.  If this pointer is not NULL then this is an owning
     pointer (i.e. this owns a reference to the Tuple).  */
  PyObject *mapped_files;
};

extern PyTypeObject corefile_object_type;

/* A gdb.CorefileMapped object.  */

struct corefile_mapped_file_object
{
  PyObject_HEAD

  /* The name of a file that was mapped when the core file was created.
     This is a 'str' object.  */
  PyObject *filename;

  /* The build-id of a file that was mapped when the core file was
     created.  This is either a 'str' if the file had a build-id, or
     'None' if there was no build-id for this file.  */
  PyObject *build_id;

  /* A List of gdb.CorefileMappedFileRegion objects.  */
  PyObject *regions;

  /* True if this represents the main executable from which the core file
     was created.  */
  bool is_main_exec_p;
};

extern PyTypeObject corefile_mapped_file_object_type;

/* A gdb.CorefileMappedFileRegion object.  */

struct corefile_mapped_file_region_object
{
  PyObject_HEAD

  /* The start and end addresses for this mapping, these are addresses
     within the inferior's address space.  */
  CORE_ADDR start;
  CORE_ADDR end;

  /* The offset within the mapped file for this mapping.  */
  ULONGEST file_offset;
};

extern PyTypeObject corefile_mapped_file_region_object_type;

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

  if (get_inferior_core_bfd (inf) == nullptr)
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
      object->mapped_files = nullptr;
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

  return get_inferior_core_bfd (obj->inferior) != nullptr;
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
  bfd *abfd = get_inferior_core_bfd (obj->inferior);
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

/* Implement gdb.Corefile.mapped_files ().  Return a List of
   gdb.CorefileMappedFile objects.  The list is created the first time
   this method is called, and then cached within the gdb.Corefile object,
   future calls just return a reference to the same list.  */

static PyObject *
cfpy_mapped_files (PyObject *self, PyObject *args)
{
  corefile_object *obj = (corefile_object *) self;

  CFPY_REQUIRE_VALID (obj);

  /* If we have already created the List then just return another reference
     to the existing list.  */
  if (obj->mapped_files != nullptr)
    {
      Py_INCREF (obj->mapped_files);
      return obj->mapped_files;
    }

  /* Get all the mapping data from GDB.  */
  std::vector<core_mapped_file> mapped_files;
  try
    {
      mapped_files
	= gdb_read_core_file_mappings (obj->inferior->arch (),
				       get_inferior_core_bfd (obj->inferior));
    }
  catch (const gdb_exception &except)
    {
      return gdbpy_handle_gdb_exception (nullptr, except);
    }

  /* Create a new list to hold the results.  */
  gdbpy_ref<> tuple (PyTuple_New (mapped_files.size ()));
  if (tuple == nullptr)
    return nullptr;

  /* Create each gdb.CorefileMappedFile object.  */
  Py_ssize_t tuple_idx = 0;
  for (const core_mapped_file &file : mapped_files)
    {
      /* The filename 'str' object.  */
      gdbpy_ref<> filename
	= host_string_to_python_string (file.filename.c_str ());
      if (filename == nullptr)
	return nullptr;

      /* The build-id object.  Either a 'str' or 'None'.  */
      gdbpy_ref<> build_id;
      if (file.build_id != nullptr)
	{
	  std::string hex_form = bin2hex (file.build_id->data,
					  file.build_id->size);

	  build_id
	    = host_string_to_python_string (hex_form.c_str ());
	  if (build_id == nullptr)
	    return nullptr;
	}
      else
	build_id = gdbpy_ref<>::new_reference (Py_None);

      /* List to hold all the gdb.CorefileMappedFileRegion objects.  */
      gdbpy_ref<> regions (PyTuple_New (file.regions.size ()));
      if (regions == nullptr)
	return nullptr;

      /* Create all the gdb.CorefileMappedFileRegion objects.  */
      Py_ssize_t regions_idx = 0;
      for (const core_mapped_file::region &r : file.regions)
	{
	  /* Actually create the object.  */
	  gdbpy_ref<corefile_mapped_file_region_object> region_obj
	    (PyObject_New (corefile_mapped_file_region_object,
			   &corefile_mapped_file_region_object_type));
	  if (region_obj == nullptr)
	    return nullptr;

	  /* Initialise the object.  */
	  region_obj->start = r.start;
	  region_obj->end = r.end;
	  region_obj->file_offset = r.file_ofs;

	  /* Add to the gdb.CorefileMappedFileRegion list.  */
	  if (PyTuple_SetItem (regions.get (), regions_idx++,
			       (PyObject *) region_obj.release ()) < 0)
	    return nullptr;
	}

      /* Actually create the gdb.CorefileMappedFile object.  */
      gdbpy_ref<corefile_mapped_file_object> entry
	(PyObject_New (corefile_mapped_file_object,
		       &corefile_mapped_file_object_type));
      if (entry == nullptr)
	return nullptr;

      /* Initialise the object.  */
      entry->filename = filename.release ();
      entry->build_id = build_id.release ();
      entry->regions = regions.release ();
      entry->is_main_exec_p = file.is_main_exec;

      /* Add to the gdb.CorefileMappedFile list.  */
      if (PyTuple_SetItem (tuple.get (), tuple_idx++,
			   (PyObject *) entry.release ()) < 0)
	return nullptr;
    }

  /* No errors.  Move the reference currently in LIST into the Corefile
     object itself.  Then create a new reference and hand this back to the
     user.  */
  obj->mapped_files = tuple.release ();
  Py_INCREF (obj->mapped_files);
  return obj->mapped_files;
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
  Py_XDECREF (corefile->mapped_files);

  Py_TYPE (obj)->tp_free (obj);
}

/* __repr__ implementation for gdb.Corefile.  */

static PyObject *
cfpy_repr (PyObject *self)
{
  corefile_object *obj = (corefile_object *) self;

  if (!cfpy_corefile_object_is_valid (obj))
    return gdb_py_invalid_object_repr (self);

  bfd *core_bfd = get_inferior_core_bfd (obj->inferior);
  gdb_assert (core_bfd != nullptr);
  return PyUnicode_FromFormat ("<%s inferior=%d filename='%s'>",
			       Py_TYPE (self)->tp_name,
			       obj->inferior->num,
			       bfd_get_filename (core_bfd));
}



/* Called when a gdb.CorefileMappedFile is destroyed.  */

static void
cfmfpy_dealloc (PyObject *obj)
{
  corefile_mapped_file_object *mapped_file
    = (corefile_mapped_file_object *) obj;

  Py_XDECREF (mapped_file->filename);
  Py_XDECREF (mapped_file->build_id);
  Py_XDECREF (mapped_file->regions);

  Py_TYPE (obj)->tp_free (obj);
}

/* Read the gdb.CorefileMappedFile.filename attribute.  */

static PyObject *
cfmfpy_get_filename (PyObject *self, void *closure)
{
  corefile_mapped_file_object *obj
    = (corefile_mapped_file_object *) self;

  gdb_assert (obj->filename != nullptr);

  Py_INCREF (obj->filename);
  return obj->filename;
}

/* Read the gdb.CorefileMappedFile.build_id attribute.  */

static PyObject *
cfmfpy_get_build_id (PyObject *self, void *closure)
{
  corefile_mapped_file_object *obj
    = (corefile_mapped_file_object *) self;

  gdb_assert (obj->build_id != nullptr);

  Py_INCREF (obj->build_id);
  return obj->build_id;
}

/* Read the gdb.CorefileMappedFile.regions attribute.  */

static PyObject *
cfmfpy_get_regions (PyObject *self, void *closure)
{
  corefile_mapped_file_object *obj
    = (corefile_mapped_file_object *) self;

  gdb_assert (obj->regions != nullptr);

  Py_INCREF (obj->regions);
  return obj->regions;
}

/* Read the gdb.CorefileMappedFile.is_main_executable attribute.  */

static PyObject *
cfmf_is_main_exec (PyObject *self, void *closure)
{
  corefile_mapped_file_object *obj
    = (corefile_mapped_file_object *) self;

  if (obj->is_main_exec_p)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}



/* Read the gdb.CorefileMappedFileRegion.start attribute.  */

static PyObject *
cfmfrpy_get_start (PyObject *self, void *closure)
{
  corefile_mapped_file_region_object *obj
    = (corefile_mapped_file_region_object *) self;

  return gdb_py_object_from_ulongest (obj->start).release ();
}

/* Read the gdb.CorefileMappedFileRegion.end attribute.  */

static PyObject *
cfmfrpy_get_end (PyObject *self, void *closure)
{
  corefile_mapped_file_region_object *obj
    = (corefile_mapped_file_region_object *) self;

  return gdb_py_object_from_ulongest (obj->end).release ();
}

/* Read the gdb.CorefileMappedFileRegion.file_offset attribute.  */

static PyObject *
cfmfrpy_get_file_offset (PyObject *self, void *closure)
{
  corefile_mapped_file_region_object *obj
    = (corefile_mapped_file_region_object *) self;

  return gdb_py_object_from_ulongest (obj->file_offset).release ();
}



static int
gdbpy_initialize_corefile ()
{
  gdb::observers::core_file_changed.attach (cfpy_corefile_changed,
					    "py-corefile");

  if (gdbpy_type_ready (&corefile_object_type) < 0)
    return -1;

  if (gdbpy_type_ready (&corefile_mapped_file_object_type) < 0)
    return -1;

  if (gdbpy_type_ready (&corefile_mapped_file_region_object_type) < 0)
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
  { "mapped_files", cfpy_mapped_files, METH_NOARGS,
    "mapped_files () -> List of mapping tuples.\n\
Return a list of tuples.  Each tuple represents a mapping from the\
core file." },
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

static gdb_PyGetSetDef corefile_mapped_file_object_getset[] =
{
  { "filename", cfmfpy_get_filename, nullptr,
    "The filename of a CorefileMappedFile object.", nullptr },
  { "build_id", cfmfpy_get_build_id, nullptr,
    "The build-id of a CorefileMappedFile object or None.", nullptr },
  { "regions", cfmfpy_get_regions, nullptr,
    "The list of regions from a CorefileMappedFile object.", nullptr },
  { "is_main_executable", cfmf_is_main_exec, nullptr,
    "True for the main executable mapping, otherwise False.", nullptr },
  { nullptr }
};

PyTypeObject corefile_mapped_file_object_type =
{
  PyVarObject_HEAD_INIT (NULL, 0)
  "gdb.CorefileMappedFile",	  /*tp_name*/
  sizeof (corefile_mapped_file_object),	/*tp_basicsize*/
  0,				  /*tp_itemsize*/
  cfmfpy_dealloc,		  /*tp_dealloc*/
  0,				  /*tp_print*/
  0,				  /*tp_getattr*/
  0,				  /*tp_setattr*/
  0,				  /*tp_compare*/
  0,				  /*tp_repr*/
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
  "GDB corefile mapped file object",	/* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  0,				  /* tp_methods */
  0,				  /* tp_members */
  corefile_mapped_file_object_getset,	/* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new */
};

static gdb_PyGetSetDef corefile_mapped_file_region_object_getset[] =
{
  { "start", cfmfrpy_get_start, nullptr,
    "The start address of a CorefileMappedFileRegion object.", nullptr },
  { "end", cfmfrpy_get_end, nullptr,
    "The end address of a CorefileMappedFileRegion object.", nullptr },
  { "file_offset", cfmfrpy_get_file_offset, nullptr,
    "The file offset of a CorefileMappedFileRegion object.", nullptr },
  { nullptr }
};

PyTypeObject corefile_mapped_file_region_object_type =
{
  PyVarObject_HEAD_INIT (NULL, 0)
  "gdb.CorefileMappedFileRegion",  /*tp_name*/
  sizeof (corefile_mapped_file_region_object),	/*tp_basicsize*/
  0,				  /*tp_itemsize*/
  0,				  /*tp_dealloc*/
  0,				  /*tp_print*/
  0,				  /*tp_getattr*/
  0,				  /*tp_setattr*/
  0,				  /*tp_compare*/
  0,				  /*tp_repr*/
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
  "GDB corefile mapped file region object",	/* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  0,				  /* tp_methods */
  0,				  /* tp_members */
  corefile_mapped_file_region_object_getset,	/* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  0,				  /* tp_init */
  0,				  /* tp_alloc */
  0,				  /* tp_new */
};
