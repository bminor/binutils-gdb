/* Python interface to values.

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
#include "value.h"
#include "exceptions.h"
#include "language.h"
#include "dfp.h"

/* List of all values which are currently exposed to Python. It is
   maintained so that when an objfile is discarded, preserve_values
   can copy the values' types if needed.  This is declared
   unconditionally to reduce the number of uses of HAVE_PYTHON in the
   generic code.  */
struct value *values_in_python;

#ifdef HAVE_PYTHON

#include "python-internal.h"

/* Even though Python scalar types directly map to host types, we use
   target types here to remain consistent with the the values system in
   GDB (which uses target arithmetic).  */

/* Python's integer type corresponds to C's long type.  */
#define builtin_type_pyint builtin_type (current_gdbarch)->builtin_long

/* Python's float type corresponds to C's double type.  */
#define builtin_type_pyfloat builtin_type (current_gdbarch)->builtin_double

/* Python's long type corresponds to C's long long type.  */
#define builtin_type_pylong builtin_type (current_gdbarch)->builtin_long_long

#define builtin_type_pybool \
  language_bool_type (current_language, current_gdbarch)

typedef struct {
  PyObject_HEAD
  struct value *value;
  int owned_by_gdb;
} value_object;

/* Called by the Python interpreter when deallocating a value object.  */
static void
valpy_dealloc (PyObject *obj)
{
  value_object *self = (value_object *) obj;

  value_remove_from_list (&values_in_python, self->value);

  if (!self->owned_by_gdb)
    value_free (self->value);
  self->ob_type->tp_free (self);
}

/* Called when a new gdb.Value object needs to be allocated.  */
static PyObject *
valpy_new (PyTypeObject *subtype, PyObject *args, PyObject *keywords)
{
  struct value *value = NULL;   /* Initialize to appease gcc warning.  */
  value_object *value_obj;
  volatile struct gdb_exception except;

  if (PyTuple_Size (args) != 1)
    {
      PyErr_SetString (PyExc_TypeError, _("Value object creation takes only "
					  "1 argument"));
      return NULL;
    }

  value_obj = (value_object *) subtype->tp_alloc (subtype, 1);
  if (value_obj == NULL)
    {
      PyErr_SetString (PyExc_MemoryError, _("Could not allocate memory to "
					    "create Value object."));
      return NULL;
    }

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      value = convert_value_from_python (PyTuple_GetItem (args, 0));
    }
  if (except.reason < 0)
    {
      subtype->tp_free (value_obj);
      return PyErr_Format (except.reason == RETURN_QUIT
			     ? PyExc_KeyboardInterrupt : PyExc_TypeError,
			     "%s", except.message);
    }

  value_obj->value = value;
  release_value (value);
  value_prepend_to_list (&values_in_python, value);

  return (PyObject *) value_obj;
}

/* Given a value of a pointer type, apply the C unary * operator to it.  */
static PyObject *
valpy_dereference (PyObject *self, PyObject *args)
{
  struct value *res_val = NULL;	  /* Initialize to appease gcc warning.  */
  volatile struct gdb_exception except;

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      res_val = value_ind (((value_object *) self)->value);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  return value_to_value_object (res_val);
}

#ifdef HAVE_LIBPYTHON2_4
static int
#else
static Py_ssize_t
#endif
valpy_length (PyObject *self)
{
  /* We don't support getting the number of elements in a struct / class.  */
  PyErr_SetString (PyExc_NotImplementedError,
		   "Invalid operation on gdb.Value.");
  return -1;
}

/* Given string name of an element inside structure, return its value
   object.  */
static PyObject *
valpy_getitem (PyObject *self, PyObject *key)
{
  value_object *self_value = (value_object *) self;
  char *field;
  struct value *res_val = NULL;	  /* Initialize to appease gcc warning.  */
  struct cleanup *old;
  volatile struct gdb_exception except;

  field = python_string_to_target_string (key);
  if (field == NULL)
    return NULL;

  old = make_cleanup (xfree, field);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      res_val = value_struct_elt (&self_value->value, NULL, field, 0, NULL);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  do_cleanups (old);

  return value_to_value_object (res_val);
}

static int
valpy_setitem (PyObject *self, PyObject *key, PyObject *value)
{
  PyErr_Format (PyExc_NotImplementedError,
		_("Setting of struct elements is not currently supported."));
  return -1;
}

/* Called by the Python interpreter to obtain string representation
   of the object.  */
static PyObject *
valpy_str (PyObject *self)
{
  char *s = NULL;
  long dummy;
  struct ui_file *stb;
  struct cleanup *old_chain;
  PyObject *result;
  volatile struct gdb_exception except;

  stb = mem_fileopen ();
  old_chain = make_cleanup_ui_file_delete (stb);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      common_val_print (((value_object *) self)->value, stb, 0, 0, 0,
			Val_pretty_default, current_language);
      s = ui_file_xstrdup (stb, &dummy);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  do_cleanups (old_chain);

  result = PyUnicode_Decode (s, strlen (s), host_charset (), NULL);
  xfree (s);

  return result;
}

enum valpy_opcode
{
  VALPY_ADD,
  VALPY_SUB,
  VALPY_MUL,
  VALPY_DIV,
  VALPY_REM,
  VALPY_POW
};

/* If TYPE is a reference, return the target; otherwise return TYPE.  */
#define STRIP_REFERENCE(TYPE) \
  ((TYPE_CODE (TYPE) == TYPE_CODE_REF) ? (TYPE_TARGET_TYPE (TYPE)) : (TYPE))

/* Returns a value object which is the result of applying the operation
   specified by OPCODE to the given arguments.  */
static PyObject *
valpy_binop (enum valpy_opcode opcode, PyObject *self, PyObject *other)
{
  struct value *res_val = NULL;	  /* Initialize to appease gcc warning.  */
  volatile struct gdb_exception except;

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      struct value *arg1, *arg2;

      /* If the gdb.Value object is the second operand, then it will be passed
	 to us as the OTHER argument, and SELF will be an entirely different
	 kind of object, altogether.  Because of this, we can't assume self is
	 a gdb.Value object and need to convert it from python as well.  */
      arg1 = convert_value_from_python (self);
      arg2 = convert_value_from_python (other);

      switch (opcode)
	{
	case VALPY_ADD:
	  {
	    struct type *ltype = value_type (arg1);
	    struct type *rtype = value_type (arg2);

	    CHECK_TYPEDEF (ltype);
	    ltype = STRIP_REFERENCE (ltype);
	    CHECK_TYPEDEF (rtype);
	    rtype = STRIP_REFERENCE (rtype);

	    if (TYPE_CODE (ltype) == TYPE_CODE_PTR)
	      res_val = value_ptradd (arg1, arg2);
	    else if (TYPE_CODE (rtype) == TYPE_CODE_PTR)
	      res_val = value_ptradd (arg2, arg1);
	    else
	      res_val = value_binop (arg1, arg2, BINOP_ADD);
	  }
	  break;
	case VALPY_SUB:
	  {
	    struct type *ltype = value_type (arg1);
	    struct type *rtype = value_type (arg2);

	    CHECK_TYPEDEF (ltype);
	    ltype = STRIP_REFERENCE (ltype);
	    CHECK_TYPEDEF (rtype);
	    rtype = STRIP_REFERENCE (rtype);

	    if (TYPE_CODE (ltype) == TYPE_CODE_PTR)
	      {
		if (TYPE_CODE (rtype) == TYPE_CODE_PTR)
		    /* A ptrdiff_t for the target would be preferable
		       here.  */
		    res_val = value_from_longest (builtin_type_pyint,
						  value_ptrdiff (arg1, arg2));
		else
		  res_val = value_ptrsub (arg1, arg2);
	      }
	    else
	      res_val = value_binop (arg1, arg2, BINOP_SUB);
	  }
	  break;
	case VALPY_MUL:
	  res_val = value_binop (arg1, arg2, BINOP_MUL);
	  break;
	case VALPY_DIV:
	  res_val = value_binop (arg1, arg2, BINOP_DIV);
	  break;
	case VALPY_REM:
	  res_val = value_binop (arg1, arg2, BINOP_REM);
	  break;
	case VALPY_POW:
	  res_val = value_binop (arg1, arg2, BINOP_EXP);
	  break;
	}
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  return value_to_value_object (res_val);
}

static PyObject *
valpy_add (PyObject *self, PyObject *other)
{
  return valpy_binop (VALPY_ADD, self, other);
}

static PyObject *
valpy_subtract (PyObject *self, PyObject *other)
{
  return valpy_binop (VALPY_SUB, self, other);
}

static PyObject *
valpy_multiply (PyObject *self, PyObject *other)
{
  return valpy_binop (VALPY_MUL, self, other);
}

static PyObject *
valpy_divide (PyObject *self, PyObject *other)
{
  return valpy_binop (VALPY_DIV, self, other);
}

static PyObject *
valpy_remainder (PyObject *self, PyObject *other)
{
  return valpy_binop (VALPY_REM, self, other);
}

static PyObject *
valpy_power (PyObject *self, PyObject *other, PyObject *unused)
{
  /* We don't support the ternary form of pow.  I don't know how to express
     that, so let's just throw NotImplementedError to at least do something
     about it.  */
  if (unused != Py_None)
    {
      PyErr_SetString (PyExc_NotImplementedError,
		       "Invalid operation on gdb.Value.");
      return NULL;
    }

  return valpy_binop (VALPY_POW, self, other);
}

static PyObject *
valpy_negative (PyObject *self)
{
  struct value *val = NULL;
  volatile struct gdb_exception except;

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      val = value_neg (((value_object *) self)->value);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  return value_to_value_object (val);
}

static PyObject *
valpy_positive (PyObject *self)
{
  struct value *copy = value_copy (((value_object *) self)->value);

  return value_to_value_object (copy);
}

static PyObject *
valpy_absolute (PyObject *self)
{
  if (value_less (((value_object *) self)->value,
		  value_from_longest (builtin_type_int8, 0)))
    return valpy_negative (self);
  else
    return valpy_positive (self);
}

/* Implements boolean evaluation of gdb.Value.  */
static int
valpy_nonzero (PyObject *self)
{
  value_object *self_value = (value_object *) self;
  struct type *type;

  type = check_typedef (value_type (self_value->value));

  if (is_integral_type (type) || TYPE_CODE (type) == TYPE_CODE_PTR)
    return !!value_as_long (self_value->value);
  else if (TYPE_CODE (type) == TYPE_CODE_FLT)
    return value_as_double (self_value->value) != 0;
  else if (TYPE_CODE (type) == TYPE_CODE_DECFLOAT)
    return !decimal_is_zero (value_contents (self_value->value),
			     TYPE_LENGTH (type));
  else
    {
      PyErr_SetString (PyExc_TypeError, _("Attempted truth testing on invalid "
					  "gdb.Value type."));
      return 0;
    }
}

/* Implements comparison operations for value objects.  */
static PyObject *
valpy_richcompare (PyObject *self, PyObject *other, int op)
{
  int result = 0;
  struct value *value_self, *value_other;
  volatile struct gdb_exception except;

  if (PyObject_TypeCheck (other, &value_object_type))
    value_other = ((value_object *) other)->value;
  else if (PyInt_Check (other))
    {
      LONGEST l;

      l = PyInt_AsLong (other);
      if (PyErr_Occurred ())
	return NULL;

      value_other = value_from_longest (builtin_type_pyint, l);
    }
  else if (PyFloat_Check (other))
    {
      DOUBLEST d;

      d = PyFloat_AsDouble (other);
      if (PyErr_Occurred ())
	return NULL;

      value_other = value_from_double (builtin_type_pyfloat, d);
    }
  else if (PyString_Check (other) || PyUnicode_Check (other))
    {
      char *str;

      str = python_string_to_target_string (other);
      value_other = value_from_string (str);
      xfree (str);
    }
  else if (other == Py_None)
    /* Comparing with None is special.  From what I can tell, in Python
       None is smaller than anything else.  */
    switch (op) {
      case Py_LT:
      case Py_LE:
      case Py_EQ:
	Py_RETURN_FALSE;
      case Py_NE:
      case Py_GT:
      case Py_GE:
	Py_RETURN_TRUE;
      default:
	/* Can't happen.  */
	PyErr_SetString (PyExc_NotImplementedError,
			 "Invalid operation on gdb.Value.");
	return NULL;
    }
  else
    {
      PyErr_SetString (PyExc_NotImplementedError,
		       "Operation not supported on gdb.Value of this type.");
      return NULL;
    }

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      switch (op) {
        case Py_LT:
	  result = value_less (((value_object *) self)->value, value_other);
	  break;
	case Py_LE:
	  result = value_less (((value_object *) self)->value, value_other)
	    || value_equal (((value_object *) self)->value, value_other);
	  break;
	case Py_EQ:
	  result = value_equal (((value_object *) self)->value, value_other);
	  break;
	case Py_NE:
	  result = !value_equal (((value_object *) self)->value, value_other);
	  break;
        case Py_GT:
	  result = value_less (value_other, ((value_object *) self)->value);
	  break;
	case Py_GE:
	  result = value_less (value_other, ((value_object *) self)->value)
	    || value_equal (((value_object *) self)->value, value_other);
	  break;
	default:
	  /* Can't happen.  */
	  PyErr_SetString (PyExc_NotImplementedError,
			   "Invalid operation on gdb.Value.");
	  return NULL;
      }
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  if (result == 1)
    Py_RETURN_TRUE;

  Py_RETURN_FALSE;
}

/* Returns an object for a value which is released from the all_values chain,
   so its lifetime is not bound to the execution of a command.  */
PyObject *
value_to_value_object (struct value *val)
{
  value_object *val_obj;

  val_obj = PyObject_New (value_object, &value_object_type);
  if (val_obj != NULL)
    {
      val_obj->value = val;
      release_value (val);
      value_prepend_to_list (&values_in_python, val);
    }

  return (PyObject *) val_obj;
}

/* Try to convert a Python value to a gdb value.  If the value cannot
   be converted, throw a gdb exception.  */

struct value *
convert_value_from_python (PyObject *obj)
{
  struct value *value = NULL; /* -Wall */
  PyObject *target_str, *unicode_str;
  struct cleanup *old;

  if (! obj)
    error (_("Internal error while converting Python value."));

  if (PyBool_Check (obj))
    value = value_from_longest (builtin_type_pybool, obj == Py_True);
  else if (PyInt_Check (obj))
    value = value_from_longest (builtin_type_pyint, PyInt_AsLong (obj));
  else if (PyLong_Check (obj))
    {
      LONGEST l = PyLong_AsLongLong (obj);
      if (! PyErr_Occurred ())
	value = value_from_longest (builtin_type_pylong, l);
    }
  else if (PyFloat_Check (obj))
    {
      double d = PyFloat_AsDouble (obj);
      if (! PyErr_Occurred ())
	value = value_from_double (builtin_type_pyfloat, d);
    }
  else if (PyString_Check (obj) || PyUnicode_Check (obj))
    {
      char *s;

      s = python_string_to_target_string (obj);
      if (s == NULL)
	return NULL;

      old = make_cleanup (xfree, s);
      value = value_from_string (s);
      do_cleanups (old);
    }
  else if (PyObject_TypeCheck (obj, &value_object_type))
    value = ((value_object *) obj)->value;
  else
    error (_("Could not convert Python object: %s"),
	   PyString_AsString (PyObject_Str (obj)));

  if (PyErr_Occurred ())
    error (_("Error converting Python value."));

  return value;
}

/* Returns value object in the ARGth position in GDB's history.  */
PyObject *
gdbpy_get_value_from_history (PyObject *self, PyObject *args)
{
  int i;
  struct value *res_val = NULL;	  /* Initialize to appease gcc warning.  */
  volatile struct gdb_exception except;

  if (!PyArg_ParseTuple (args, "i", &i))
    return NULL;

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      res_val = access_value_history (i);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  return value_to_value_object (res_val);
}

void
gdbpy_initialize_values (void)
{
  value_object_type.tp_new = valpy_new;
  if (PyType_Ready (&value_object_type) < 0)
    return;

  Py_INCREF (&value_object_type);
  PyModule_AddObject (gdb_module, "Value", (PyObject *) &value_object_type);

  values_in_python = NULL;
}

static PyMethodDef value_object_methods[] = {
  { "dereference", valpy_dereference, METH_NOARGS, "Dereferences the value." },
  {NULL}  /* Sentinel */
};

static PyNumberMethods value_object_as_number = {
  valpy_add,
  valpy_subtract,
  valpy_multiply,
  valpy_divide,
  valpy_remainder,
  NULL,			      /* nb_divmod */
  valpy_power,		      /* nb_power */
  valpy_negative,	      /* nb_negative */
  valpy_positive,	      /* nb_positive */
  valpy_absolute,	      /* nb_absolute */
  valpy_nonzero		      /* nb_nonzero */
};

static PyMappingMethods value_object_as_mapping = {
  valpy_length,
  valpy_getitem,
  valpy_setitem
};

PyTypeObject value_object_type = {
  PyObject_HEAD_INIT (NULL)
  0,				  /*ob_size*/
  "gdb.Value",			  /*tp_name*/
  sizeof (value_object),	  /*tp_basicsize*/
  0,				  /*tp_itemsize*/
  valpy_dealloc,		  /*tp_dealloc*/
  0,				  /*tp_print*/
  0,				  /*tp_getattr*/
  0,				  /*tp_setattr*/
  0,				  /*tp_compare*/
  0,				  /*tp_repr*/
  &value_object_as_number,	  /*tp_as_number*/
  0,				  /*tp_as_sequence*/
  &value_object_as_mapping,	  /*tp_as_mapping*/
  0,				  /*tp_hash */
  0,				  /*tp_call*/
  valpy_str,			  /*tp_str*/
  0,				  /*tp_getattro*/
  0,				  /*tp_setattro*/
  0,				  /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,	/*tp_flags*/
  "GDB value object",		  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  valpy_richcompare,		  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  value_object_methods		  /* tp_methods */
};

#endif /* HAVE_PYTHON */
