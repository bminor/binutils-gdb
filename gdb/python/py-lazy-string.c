/* Python interface to lazy strings.

   Copyright (C) 2010 Free Software Foundation, Inc.

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
#include "python-internal.h"
#include "charset.h"
#include "value.h"
#include "exceptions.h"
#include "valprint.h"
#include "language.h"

typedef struct {
  PyObject_HEAD
  /*  Holds the address of the lazy string.  */
  CORE_ADDR address;

  /*  Holds the encoding that will be applied to the string
      when the string is printed by GDB.  If the encoding is set
      to None then GDB will select the most appropriate
      encoding when the sting is printed.  */
  char *encoding;

  /* Holds the length of the string in characters.  If the
     length is -1, then the string will be fetched and encoded up to
     the first null of appropriate width.  */
  long length;

  /*  This attribute holds the type that is represented by the lazy
      string's type.  */
  struct type *type;
} lazy_string_object;

static PyTypeObject lazy_string_object_type;

static PyObject *
stpy_get_address (PyObject *self, void *closure)
{
  lazy_string_object *self_string = (lazy_string_object *) self;
  return PyLong_FromUnsignedLongLong (self_string->address);
}

static PyObject *
stpy_get_encoding (PyObject *self, void *closure)
{
  lazy_string_object *self_string = (lazy_string_object *) self;
  PyObject *result;

  /* An encoding can be set to NULL by the user, so check before
     attempting a Python FromString call.  If NULL return Py_None.  */
  if (self_string->encoding)
    result = PyString_FromString (self_string->encoding);
  else
    {
      result = Py_None;
      Py_INCREF (result);
    }

  return result;
}

static PyObject *
stpy_get_length (PyObject *self, void *closure)
{
  lazy_string_object *self_string = (lazy_string_object *) self;
  return PyLong_FromLong (self_string->length);
}

PyObject *
stpy_get_type (PyObject *self, void *closure)
{
  lazy_string_object *str_obj = (lazy_string_object *) self;
  return type_to_type_object (str_obj->type);
}

static PyObject *
stpy_convert_to_value  (PyObject *self, PyObject *args)
{
  lazy_string_object *self_string = (lazy_string_object *) self;
  struct value *val;

  val = value_at_lazy (self_string->type, self_string->address);
  return value_to_value_object (val);
}

static void
stpy_dealloc (PyObject *self)
{
  lazy_string_object *self_string = (lazy_string_object *) self;
  xfree (self_string->encoding);
}

PyObject *
gdbpy_create_lazy_string_object (CORE_ADDR address, long length,
			   const char *encoding, struct type *type)
{
  lazy_string_object *str_obj = NULL;

  if (address == 0)
    {
      PyErr_SetString (PyExc_MemoryError,
		       "Cannot create a lazy string from a GDB-side string.");
      return NULL;
    }

  if (!type)
    {
      PyErr_SetString (PyExc_RuntimeError,
		       "A lazy string's type cannot be NULL.");
      return NULL;
    }

  str_obj = PyObject_New (lazy_string_object, &lazy_string_object_type);
  if (!str_obj)
    return NULL;

  str_obj->address = address;
  str_obj->length = length;
  if (encoding == NULL || !strcmp (encoding, ""))
    str_obj->encoding = NULL;
  else
    str_obj->encoding = xstrdup (encoding);
  str_obj->type = type;

  return (PyObject *) str_obj;
}

void
gdbpy_initialize_lazy_string (void)
{
  if (PyType_Ready (&lazy_string_object_type) < 0)
    return;

  Py_INCREF (&lazy_string_object_type);
}

/* Determine whether the printer object pointed to by OBJ is a
   Python lazy string.  */
int
gdbpy_is_lazy_string (PyObject *result)
{
  return PyObject_TypeCheck (result, &lazy_string_object_type);
}

/* Extract and return the actual string from the lazy string object
   STRING.  Addtionally, the string type is written to *STR_TYPE, the
   string length is written to *LENGTH, and the string encoding is
   written to *ENCODING.  On error, NULL is returned.  The caller is
   responsible for freeing the returned buffer.  */
gdb_byte *
gdbpy_extract_lazy_string (PyObject *string, struct type **str_type,
		     long *length, char **encoding)
{
  int width;
  int bytes_read;
  gdb_byte *buffer = NULL;
  int errcode = 0;
  CORE_ADDR addr;
  struct gdbarch *gdbarch;
  enum bfd_endian byte_order;
  PyObject *py_len = NULL, *py_encoding = NULL; 
  PyObject *py_addr = NULL, *py_type = NULL;
  volatile struct gdb_exception except;

  py_len = PyObject_GetAttrString (string, "length");
  py_encoding = PyObject_GetAttrString (string, "encoding");
  py_addr = PyObject_GetAttrString (string, "address");
  py_type = PyObject_GetAttrString (string, "type");

  /* A NULL encoding, length, address or type is not ok.  */
  if (!py_len || !py_encoding || !py_addr || !py_type)
    goto error;

  *length = PyLong_AsLong (py_len);
  addr = PyLong_AsLongLong (py_addr);

  /* If the user supplies Py_None an encoding, set encoding to NULL.
     This will trigger the resulting LA_PRINT_CALL to automatically
     select an encoding.  */
  if (py_encoding == Py_None)
    *encoding = NULL;
  else
    *encoding = xstrdup (PyString_AsString (py_encoding));

  *str_type = type_object_to_type (py_type);
  gdbarch = get_type_arch (*str_type);
  byte_order = gdbarch_byte_order (gdbarch);
  width = TYPE_LENGTH (*str_type);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      errcode = read_string (addr, *length, width,
			     *length, byte_order, &buffer,
			     &bytes_read);
    }
  if (except.reason < 0)
    {
      PyErr_Format (except.reason == RETURN_QUIT			\
		    ? PyExc_KeyboardInterrupt : PyExc_RuntimeError,	\
		    "%s", except.message);				\
      goto error;

    }

  if (errcode)
    goto error;

  *length = bytes_read / width;

  Py_DECREF (py_encoding);
  Py_DECREF (py_len);
  Py_DECREF (py_addr);
  Py_DECREF (py_type);
  return buffer;

 error:
  Py_XDECREF (py_encoding);
  Py_XDECREF (py_len);
  Py_XDECREF (py_addr);
  Py_XDECREF (py_type);
  xfree (buffer);
  *length = 0;
  *str_type = NULL;
  return NULL;
}



static PyMethodDef lazy_string_object_methods[] = {
  { "value", stpy_convert_to_value, METH_NOARGS,
    "Create a (lazy) value that contains a pointer to the string." },
  {NULL}  /* Sentinel */
};


static PyGetSetDef lazy_string_object_getset[] = {
  { "address", stpy_get_address, NULL, "Address of the string.", NULL },
  { "encoding", stpy_get_encoding, NULL, "Encoding of the string.", NULL },
  { "length", stpy_get_length, NULL, "Length of the string.", NULL },
  { "type", stpy_get_type, NULL, "Type associated with the string.", NULL },
  { NULL }  /* Sentinel */
};

static PyTypeObject lazy_string_object_type = {
  PyObject_HEAD_INIT (NULL)
  0,				  /*ob_size*/
  "gdb.LazyString",	          /*tp_name*/
  sizeof (lazy_string_object),	  /*tp_basicsize*/
  0,				  /*tp_itemsize*/
  stpy_dealloc,                   /*tp_dealloc*/
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
  Py_TPFLAGS_DEFAULT,             /*tp_flags*/
  "GDB lazy string object",	  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,			          /* tp_iter */
  0,				  /* tp_iternext */
  lazy_string_object_methods,	  /* tp_methods */
  0,				  /* tp_members */
  lazy_string_object_getset	  /* tp_getset */
};
