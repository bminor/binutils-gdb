/* General utility routines for GDB/Python.

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

#include "defs.h"
#include "charset.h"
#include "python-internal.h"


/* This is a cleanup function which decrements the refcount on a
   Python object.  */

static void
py_decref (void *p)
{
  PyObject *py = p;
  /* Note that we need the extra braces in this 'if' to avoid a
     warning from gcc.  */
  if (py)
    {
      Py_DECREF (py);
    }
}

/* Return a new cleanup which will decrement the Python object's
   refcount when run.  */

struct cleanup *
make_cleanup_py_decref (PyObject *py)
{
  return make_cleanup (py_decref, (void *) py);
}

/* Converts a Python 8-bit string to a unicode string object.  Assumes the
   8-bit string is in the host charset.  If an error occurs during conversion,
   returns NULL with a python exception set.

   As an added bonus, the functions accepts a unicode string and returns it
   right away, so callers don't need to check which kind of string they've
   got.

   If the given object is not one of the mentioned string types, NULL is
   returned, with the TypeError python exception set.  */
PyObject *
python_string_to_unicode (PyObject *obj)
{
  PyObject *unicode_str;

  /* If obj is already a unicode string, just return it.
     I wish life was always that simple...  */
  if (PyUnicode_Check (obj))
    unicode_str = obj;
  else if (PyString_Check (obj))
    unicode_str = PyUnicode_FromEncodedObject (obj, host_charset (), NULL);
  else
    {
      PyErr_SetString (PyExc_TypeError,
		       _("Expected a string or unicode object."));
      unicode_str = NULL;
    }

  return unicode_str;
}

/* Returns a newly allocated string with the contents of the given unicode
   string object converted to the target's charset.  If an error occurs during
   the conversion, NULL will be returned and a python exception will be set.

   The caller is responsible for xfree'ing the string.  */
char *
unicode_to_target_string (PyObject *unicode_str)
{
  char *target_string;
  PyObject *string;

  /* Translate string to target's charset.  */
  string = PyUnicode_AsEncodedString (unicode_str, target_charset (), NULL);
  if (string == NULL)
    return NULL;

  target_string = xstrdup (PyString_AsString (string));

  Py_DECREF (string);

  return target_string;
}

/* Converts a python string (8-bit or unicode) to a target string in
   the target's charset.  Returns NULL on error, with a python exception set.

   The caller is responsible for xfree'ing the string.  */
char *
python_string_to_target_string (PyObject *obj)
{
  PyObject *str;

  str = python_string_to_unicode (obj);
  if (str == NULL)
    return NULL;

  return unicode_to_target_string (str);
}
