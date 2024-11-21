/* Python interface to compunits.

   Copyright (C) 2008-2024 Free Software Foundation, Inc.

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

#include <algorithm>
#include "charset.h"
#include "symtab.h"
#include "source.h"
#include "python-internal.h"
#include "objfiles.h"
#include "block.h"

struct compunit_object {
  PyObject_HEAD

  /* The GDB compunit structure.  */
  struct compunit_symtab *compunit;

  /* A compunit object is associated with an objfile, so keep track with
     a doubly-linked list, rooted in the objfile.  This allows
     invalidation of the underlying struct compunit_symtab when the objfile is
     deleted.  */
  compunit_object *prev;
  compunit_object *next;
};

/* This function is called when an objfile is about to be freed.
   Invalidate the compunit as further actions on the compunit
   would result in bad data.  All access to obj->compunit should be
   gated by CUPY_REQUIRE_VALID which will raise an exception on
   compunits.  */
struct cupy_deleter
{
  void operator() (compunit_object *obj)
  {
    while (obj)
      {
	compunit_object *next = obj->next;

	obj->compunit = nullptr;
	obj->next = nullptr;
	obj->prev = nullptr;
	obj = next;
      }
  }
};

extern PyTypeObject compunit_object_type
    CPYCHECKER_TYPE_OBJECT_FOR_TYPEDEF ("compunit_object");
static const registry<objfile>::key<compunit_object, cupy_deleter>
     cupy_objfile_data_key;

/* Require a valid compunit.  All access to compunit_object->compunit
   should be gated by this call.  */

#define CUPY_REQUIRE_VALID(compunit_obj, compunit)		 \
  do {							 \
    compunit = compunit_object_to_compunit (compunit_obj);	 \
    if (compunit == nullptr)					 \
      {							 \
	PyErr_SetString (PyExc_RuntimeError,		 \
			 _("Compunit object is invalid.")); \
	return nullptr;					 \
      }							 \
  } while (0)


/* Getter function for gdb.Compunit.objfile.  */

static PyObject *
cupy_get_objfile (PyObject *self, void *closure)
{
  struct compunit_symtab *compunit = nullptr;

  CUPY_REQUIRE_VALID (self, compunit);

  return objfile_to_objfile_object (compunit->objfile ()).release ();
}

/* Getter function for gdb.Compunit.producer.  */

static PyObject *
cupy_get_producer (PyObject *self, void *closure)
{
  struct compunit_symtab *compunit = nullptr;

  CUPY_REQUIRE_VALID (self, compunit);
  if (compunit->producer () != nullptr)
    {
      const char *producer = compunit->producer ();

      return host_string_to_python_string (producer).release ();
    }

  Py_RETURN_NONE;
}

/* Implementation of gdb.Compunit.is_valid (self) -> Boolean.
   Returns True if this Symbol table still exists in GDB.  */

static PyObject *
cupy_is_valid (PyObject *self, PyObject *args)
{
  struct compunit_symtab *compunit = nullptr;

  compunit = compunit_object_to_compunit (self);
  if (compunit == nullptr)
    Py_RETURN_FALSE;

  Py_RETURN_TRUE;
}

/* Return the GLOBAL_BLOCK of the underlying compunit.  */

static PyObject *
cupy_global_block (PyObject *self, PyObject *args)
{
  struct compunit_symtab *compunit = nullptr;
  const struct blockvector *blockvector;

  CUPY_REQUIRE_VALID (self, compunit);

  blockvector = compunit->blockvector ();
  const struct block *block = blockvector->global_block ();

  return block_to_block_object (block, compunit->objfile ());
}

/* Return the STATIC_BLOCK of the underlying compunit.  */

static PyObject *
cupy_static_block (PyObject *self, PyObject *args)
{
  struct compunit_symtab *compunit = nullptr;
  const struct blockvector *blockvector;

  CUPY_REQUIRE_VALID (self, compunit);

  blockvector = compunit->blockvector ();
  const struct block *block = blockvector->static_block ();

  return block_to_block_object (block, compunit->objfile ());
}

static PyObject *
cupy_get_symtabs (PyObject *self, void *closure)
{
  struct compunit_symtab *compunit = nullptr;

  CUPY_REQUIRE_VALID (self, compunit);

  gdbpy_ref<> list (PyList_New (0));
  if (list == nullptr)
    return nullptr;

  for (struct symtab *each : compunit->filetabs ())
    {
      {
	gdbpy_ref<> item (symtab_to_symtab_object (each));
	if (item.get () == nullptr
	      || PyList_Append (list.get (), item.get ()) == -1)
	    return nullptr;
	}
    }

  return list.release ();
}

static void
cupy_dealloc (PyObject *obj)
{
  compunit_object *compunit = (compunit_object *) obj;

  if (compunit->prev)
    compunit->prev->next = compunit->next;
  else if (compunit->compunit)
    cupy_objfile_data_key.set (compunit->compunit->objfile (),
			       compunit->next);
  if (compunit->next)
    compunit->next->prev = compunit->prev;
  compunit->compunit = nullptr;
  Py_TYPE (obj)->tp_free (obj);
}

/* Given a compunit, and a compunit_object that has previously been
   allocated and initialized, populate the compunit_object with the
   struct compunit_symtab data.  Also, register the compunit_object life-cycle
   with the life-cycle of the object file associated with this
   compunit, if needed.  */
static void
set_compunit (compunit_object *obj, struct compunit_symtab *compunit)
{
  obj->compunit = compunit;
  obj->prev = nullptr;
  if (compunit)
    {
      obj->next = cupy_objfile_data_key.get (compunit->objfile ());
      if (obj->next)
	obj->next->prev = obj;
      cupy_objfile_data_key.set (compunit->objfile (), obj);
    }
  else
    obj->next = nullptr;
}

/* Object initializer; creates a new compunit.

   Use: __init__(FILENAME, OBJFILE, START, END [, CAPACITY]).  */

static int
cupy_init (PyObject *zelf, PyObject *args, PyObject *kw)
{
  struct compunit_object *self = (struct compunit_object*) zelf;

  if (self->compunit)
    {
      PyErr_Format (PyExc_RuntimeError,
		    _("Compunit object already initialized."));
      return -1;
    }

  static const char *keywords[] = { "filename", "objfile", "start", "end",
				    "capacity", nullptr };
  const char *filename;
  PyObject *objf_obj = nullptr;
  uint64_t start = 0;
  uint64_t end = 0;
  uint64_t capacity = 8;


  if (!gdb_PyArg_ParseTupleAndKeywords (args, kw, "sOKK|K", keywords,
					&filename, &objf_obj, &start, &end,
					&capacity))
    return -1;

  auto objf = objfile_object_to_objfile (objf_obj);
  if (! objf)
    {
      PyErr_Format (PyExc_TypeError,
		    _("The objfile argument is not valid gdb.Objfile object"));
      return -1;
    }

  /* Check that start-end range is valid.  */
  if (! (start <= end))
    {
      PyErr_Format (PyExc_ValueError,
		    _("The start argument must be less or equal to the end "
		      "argument"));
      return -1;

    }

  /* Check that to-be created compunits' global block does not overlap
     with existing compunit.  */

  for (struct objfile *of : objf->pspace ()->objfiles_safe ())
    {
      for (compunit_symtab *cu : of->compunits ())
	{
	  global_block *gb = cu->blockvector ()->global_block ();
	  if (std::max (start, gb->start ()) < std::min(end, gb->end ()))
	    {
	      PyErr_Format (PyExc_ValueError,
		    _("The start-end range overlaps with existing compunit"));
	      return -1;
	    }
	}
    }

  blockvector *bv = allocate_blockvector (&objf->objfile_obstack,
					  FIRST_LOCAL_BLOCK, capacity);
  compunit_symtab *cu = allocate_compunit_symtab (objf, filename);
  cu->set_dirname (nullptr);
  cu->set_blockvector (bv);

  /* Allocate global block. */
  global_block *gb = new (&objf->objfile_obstack) global_block ();
  gb->set_multidict (mdict_create_linear_expandable (language_minimal));
  gb->set_start ((CORE_ADDR) start);
  gb->set_end ((CORE_ADDR) end);
  gb->set_compunit (cu);
  bv->set_block (GLOBAL_BLOCK, gb);

  /* Allocate static block.  */
  struct block *sb = new (&objf->objfile_obstack) block ();
  sb->set_multidict (mdict_create_linear_expandable (language_minimal));
  sb->set_start ((CORE_ADDR) start);
  sb->set_end ((CORE_ADDR) end);
  sb->set_superblock (gb);
  bv->set_block (STATIC_BLOCK, sb);

  add_compunit_symtab_to_objfile (cu);

  set_compunit(self, cu);

  return 0;
}

/* Return a new reference to gdb.Compunit Python object representing
   COMPUNIT.  Return NULL and set the Python error on failure.  */
gdbpy_ref<>
compunit_to_compunit_object (struct compunit_symtab *compunit)
{
  compunit_object *compunit_obj;

  compunit_obj = PyObject_NEW(compunit_object, &compunit_object_type);
  if (compunit_obj)
    set_compunit(compunit_obj, compunit);

  return gdbpy_ref<>::new_reference ((PyObject * )compunit_obj);
}

/* Return struct compunit_symtab reference that is wrapped by this object.  */
struct compunit_symtab *
compunit_object_to_compunit (PyObject *obj)
{
  if (! PyObject_TypeCheck (obj, &compunit_object_type))
    return nullptr;
  return ((compunit_object *) obj)->compunit;
}

static int CPYCHECKER_NEGATIVE_RESULT_SETS_EXCEPTION
gdbpy_initialize_compunits (void)
{
  if (gdbpy_type_ready (&compunit_object_type) < 0)
    return -1;

  return 0;
}

GDBPY_INITIALIZE_FILE (gdbpy_initialize_compunits);



static gdb_PyGetSetDef compunit_object_getset[] = {
  { "objfile", cupy_get_objfile, nullptr, "The compunit's objfile.",
    nullptr },
  { "producer", cupy_get_producer, nullptr,
    "The name/version of the program that compiled this compunit.", nullptr },
  { "symtabs", cupy_get_symtabs, nullptr,
    "List of symbol tables associated with this compunit", nullptr },
  {nullptr}  /* Sentinel */
};

static PyMethodDef compunit_object_methods[] = {
  { "is_valid", cupy_is_valid, METH_NOARGS,
    "is_valid () -> Boolean.\n\
Return true if this compunit is valid, false if not." },
  { "global_block", cupy_global_block, METH_NOARGS,
    "global_block () -> gdb.Block.\n\
Return the global block of the compunit." },
  { "static_block", cupy_static_block, METH_NOARGS,
    "static_block () -> gdb.Block.\n\
Return the static block of the compunit." },
  {nullptr}  /* Sentinel */
};

PyTypeObject compunit_object_type = {
  PyVarObject_HEAD_INIT (nullptr, 0)
  "gdb.Compunit",		  /*tp_name*/
  sizeof (compunit_object),	  /*tp_basicsize*/
  0,				  /*tp_itemsize*/
  cupy_dealloc,			  /*tp_dealloc*/
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
  0,			          /*tp_str*/
  0,				  /*tp_getattro*/
  0,				  /*tp_setattro*/
  0,				  /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,		  /*tp_flags*/
  "GDB compunit object",	  /*tp_doc */
  0,				  /*tp_traverse */
  0,				  /*tp_clear */
  gdbpy_richcompare<compunit_object, compunit_symtab,
      &compunit_object::compunit>,/*tp_richcompare */
  0,				  /*tp_weaklistoffset */
  0,				  /*tp_iter */
  0,				  /*tp_iternext */
  compunit_object_methods,	  /*tp_methods */
  0,				  /*tp_members */
  compunit_object_getset,	  /*tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  cupy_init,                      /* tp_init */
  0,				  /* tp_alloc */
  PyType_GenericNew		  /* tp_new */
};
