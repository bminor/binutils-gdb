/* GDB parameters implemented in Python

   Copyright (C) 2008, 2009, 2010 Free Software Foundation, Inc.

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
#include "value.h"
#include "exceptions.h"
#include "python-internal.h"
#include "charset.h"
#include "gdbcmd.h"
#include "cli/cli-decode.h"
#include "completer.h"

/* Parameter constants and their values.  */
struct parm_constant
{
  char *name;
  int value;
};

struct parm_constant parm_constants[] =
{
  { "PARAM_BOOLEAN", var_boolean }, /* ARI: var_boolean */
  { "PARAM_AUTO_BOOLEAN", var_auto_boolean },
  { "PARAM_UINTEGER", var_uinteger },
  { "PARAM_INTEGER", var_integer },
  { "PARAM_STRING", var_string },
  { "PARAM_STRING_NOESCAPE", var_string_noescape },
  { "PARAM_OPTIONAL_FILENAME", var_optional_filename },
  { "PARAM_FILENAME", var_filename },
  { "PARAM_ZINTEGER", var_zinteger },
  { "PARAM_ENUM", var_enum },
  { NULL, 0 }
};

/* A union that can hold anything described by enum var_types.  */
union parmpy_variable
{
  /* Hold an integer value, for boolean and integer types.  */
  int intval;

  /* Hold an auto_boolean.  */
  enum auto_boolean autoboolval;

  /* Hold an unsigned integer value, for uinteger.  */
  unsigned int uintval;

  /* Hold a string, for the various string types.  */
  char *stringval;

  /* Hold a string, for enums.  */
  const char *cstringval;
};

/* A GDB parameter.  */
struct parmpy_object
{
  PyObject_HEAD

  /* The type of the parameter.  */
  enum var_types type;

  /* The value of the parameter.  */
  union parmpy_variable value;

  /* For an enum command, the possible values.  The vector is
     allocated with xmalloc, as is each element.  It is
     NULL-terminated.  */
  const char **enumeration;
};

typedef struct parmpy_object parmpy_object;

static PyTypeObject parmpy_object_type;

/* Some handy string constants.  */
static PyObject *set_doc_cst;
static PyObject *show_doc_cst;



/* Get an attribute.  */
static PyObject *
get_attr (PyObject *obj, PyObject *attr_name)
{
  if (PyString_Check (attr_name)
      && ! strcmp (PyString_AsString (attr_name), "value"))
    {
      parmpy_object *self = (parmpy_object *) obj;

      return gdbpy_parameter_value (self->type, &self->value);
    }

  return PyObject_GenericGetAttr (obj, attr_name);
}

/* Set a parameter value from a Python value.  Return 0 on success, -1
   on failure.  */
static int
set_parameter_value (parmpy_object *self, PyObject *value)
{
  int cmp;

  switch (self->type)
    {
    case var_string:
    case var_string_noescape:
    case var_optional_filename:
    case var_filename:
      if (! gdbpy_is_string (value)
	  && (self->type == var_filename
	      || value != Py_None))
	{
	  PyErr_SetString (PyExc_RuntimeError, 
			   _("String required for filename."));

	  return -1;
	}
      if (self->value.stringval)
	xfree (self->value.stringval);
      if (value == Py_None)
	{
	  if (self->type == var_optional_filename)
	    self->value.stringval = xstrdup ("");
	  else
	    self->value.stringval = NULL;
	}
      else
	self->value.stringval = python_string_to_host_string (value);
      break;

    case var_enum:
      {
	int i;
	char *str;

	if (! gdbpy_is_string (value))
	  {
	    PyErr_SetString (PyExc_RuntimeError, 
			     _("ENUM arguments must be a string."));
	    return -1;
	  }

	str = python_string_to_host_string (value);
	for (i = 0; self->enumeration[i]; ++i)
	  if (! strcmp (self->enumeration[i], str))
	    break;
	xfree (str);
	if (! self->enumeration[i])
	  {
	    PyErr_SetString (PyExc_RuntimeError,
			     _("The value must be member of an enumeration."));
	    return -1;
	  }
	self->value.cstringval = self->enumeration[i];
	break;
      }

    case var_boolean:
      if (! PyBool_Check (value))
	{
	  PyErr_SetString (PyExc_RuntimeError, 
			   _("A boolean argument is required."));
	  return -1;
	}
      cmp = PyObject_IsTrue (value);
      if (cmp < 0) 
	  return -1;
      self->value.intval = cmp;
      break;

    case var_auto_boolean:
      if (! PyBool_Check (value) && value != Py_None)
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("A boolean or None is required"));
	  return -1;
	}

      if (value == Py_None)
	self->value.autoboolval = AUTO_BOOLEAN_AUTO;
      else
	{
	  cmp = PyObject_IsTrue (value);
	  if (cmp < 0 )
	    return -1;	  
	  if (cmp == 1)
	    self->value.autoboolval = AUTO_BOOLEAN_TRUE;
	  else 
	    self->value.autoboolval = AUTO_BOOLEAN_FALSE;

	  break;
	}

    case var_integer:
    case var_zinteger:
    case var_uinteger:
      {
	long l;
	int ok;

	if (! PyInt_Check (value))
	  {
	    PyErr_SetString (PyExc_RuntimeError, 
			     _("The value must be integer."));
	    return -1;
	  }

	l = PyInt_AsLong (value);
	if (self->type == var_uinteger)
	  {
	    ok = (l >= 0 && l <= UINT_MAX);
	    if (l == 0)
	      l = UINT_MAX;
	  }
	else if (self->type == var_integer)
	  {
	    ok = (l >= INT_MIN && l <= INT_MAX);
	    if (l == 0)
	      l = INT_MAX;
	  }
	else
	  ok = (l >= INT_MIN && l <= INT_MAX);

	if (! ok)
	  {
	    PyErr_SetString (PyExc_RuntimeError, 
			     _("Range exceeded."));
	    return -1;
	  }

	self->value.intval = (int) l;
	break;
      }

    default:
      PyErr_SetString (PyExc_RuntimeError, 
		       _("Unhandled type in parameter value."));
      return -1;
    }

  return 0;
}

/* Set an attribute.  */
static int
set_attr (PyObject *obj, PyObject *attr_name, PyObject *val)
{
  if (PyString_Check (attr_name)
      && ! strcmp (PyString_AsString (attr_name), "value"))
    {
      if (!val)
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("Cannot delete a parameter's value."));
	  return -1;
	}
      return set_parameter_value ((parmpy_object *) obj, val);
    }

  return PyObject_GenericSetAttr (obj, attr_name, val);
}



/* A helper function that dispatches to the appropriate add_setshow
   function.  */
static void
add_setshow_generic (int parmclass, enum command_class cmdclass,
		     char *cmd_name, parmpy_object *self,
		     char *set_doc, char *show_doc, char *help_doc,
		     struct cmd_list_element **set_list,
		     struct cmd_list_element **show_list)
{
  switch (parmclass)
    {
    case var_boolean:
      add_setshow_boolean_cmd (cmd_name, cmdclass, &self->value.intval,
			       set_doc, show_doc, help_doc,
			       NULL, NULL, set_list, show_list);
      break;

    case var_auto_boolean:
      add_setshow_auto_boolean_cmd (cmd_name, cmdclass,
				    &self->value.autoboolval,
				    set_doc, show_doc, help_doc,
				    NULL, NULL, set_list, show_list);
      break;

    case var_uinteger:
      add_setshow_uinteger_cmd (cmd_name, cmdclass, &self->value.uintval,
				set_doc, show_doc, help_doc,
				NULL, NULL, set_list, show_list);
      break;

    case var_integer:
      add_setshow_integer_cmd (cmd_name, cmdclass, &self->value.intval,
			       set_doc, show_doc, help_doc,
			       NULL, NULL, set_list, show_list);
      break;

    case var_string:
      add_setshow_string_cmd (cmd_name, cmdclass, &self->value.stringval,
			      set_doc, show_doc, help_doc,
			      NULL, NULL, set_list, show_list);
      break;

    case var_string_noescape:
      add_setshow_string_noescape_cmd (cmd_name, cmdclass,
				       &self->value.stringval,
				       set_doc, show_doc, help_doc,
				       NULL, NULL, set_list, show_list);
      break;

    case var_optional_filename:
      add_setshow_optional_filename_cmd (cmd_name, cmdclass,
					 &self->value.stringval,
					 set_doc, show_doc, help_doc,
					 NULL, NULL, set_list, show_list);
      break;

    case var_filename:
      add_setshow_filename_cmd (cmd_name, cmdclass, &self->value.stringval,
				set_doc, show_doc, help_doc,
				NULL, NULL, set_list, show_list);
      break;

    case var_zinteger:
      add_setshow_zinteger_cmd (cmd_name, cmdclass, &self->value.intval,
				set_doc, show_doc, help_doc,
				NULL, NULL, set_list, show_list);
      break;

    case var_enum:
      add_setshow_enum_cmd (cmd_name, cmdclass, self->enumeration,
			    &self->value.cstringval,
			    set_doc, show_doc, help_doc,
			    NULL, NULL, set_list, show_list);
      /* Initialize the value, just in case.  */
      self->value.cstringval = self->enumeration[0];
      break;
    }
}

/* A helper which computes enum values.  Returns 1 on success, 0 on
   error.  */
static int
compute_enum_values (parmpy_object *self, PyObject *enum_values)
{
  Py_ssize_t size, i;

  if (! enum_values)
    {
      PyErr_SetString (PyExc_RuntimeError,
		       _("An enumeration is required for PARAM_ENUM."));
      return 0;
    }

  if (! PySequence_Check (enum_values))
    {
      PyErr_SetString (PyExc_RuntimeError, 
		       _("The enumeration is not a sequence."));
      return 0;
    }

  size = PySequence_Size (enum_values);
  if (size < 0)
    return 0;
  if (size == 0)
    {
      PyErr_SetString (PyExc_RuntimeError, 
		       _("The enumeration is empty."));
      return 0;
    }

  self->enumeration = xmalloc ((size + 1) * sizeof (char *));
  memset (self->enumeration, 0, (size + 1) * sizeof (char *));

  for (i = 0; i < size; ++i)
    {
      PyObject *item = PySequence_GetItem (enum_values, i);

      if (! item)
	return 0;
      if (! gdbpy_is_string (item))
	{
	  PyErr_SetString (PyExc_RuntimeError, 
			   _("The enumeration item not a string."));
	  return 0;
	}
      self->enumeration[i] = python_string_to_host_string (item);
    }

  return 1;
}

/* A helper function which returns a documentation string for an
   object.  */
static char *
get_doc_string (PyObject *object, PyObject *attr)
{
  char *result = NULL;

  if (PyObject_HasAttr (object, attr))
    {
      PyObject *ds_obj = PyObject_GetAttr (object, attr);

      if (ds_obj && gdbpy_is_string (ds_obj))
	result = python_string_to_host_string (ds_obj);
    }
  if (! result)
    result = xstrdup (_("This command is not documented."));
  return result;
}

/* Object initializer; sets up gdb-side structures for command.

   Use: __init__(NAME, CMDCLASS, PARMCLASS, [ENUM])

   NAME is the name of the parameter.  It may consist of multiple
   words, in which case the final word is the name of the new command,
   and earlier words must be prefix commands.

   CMDCLASS is the kind of command.  It should be one of the COMMAND_*
   constants defined in the gdb module.

   PARMCLASS is the type of the parameter.  It should be one of the
   PARAM_* constants defined in the gdb module.

   If PARMCLASS is PARAM_ENUM, then the final argument should be a
   collection of strings.  These strings are the valid values for this
   parameter.

   The documentation for the parameter is taken from the doc string
   for the python class.
   
*/
static int
parmpy_init (PyObject *self, PyObject *args, PyObject *kwds)
{
  parmpy_object *obj = (parmpy_object *) self;
  char *name;
  char *set_doc, *show_doc, *doc;
  char *cmd_name;
  int parmclass, cmdtype;
  PyObject *enum_values = NULL;
  struct cmd_list_element **set_list, **show_list;
  volatile struct gdb_exception except;

  if (! PyArg_ParseTuple (args, "sii|O", &name, &cmdtype, &parmclass,
			  &enum_values))
    return -1;

  if (cmdtype != no_class && cmdtype != class_run
      && cmdtype != class_vars && cmdtype != class_stack
      && cmdtype != class_files && cmdtype != class_support
      && cmdtype != class_info && cmdtype != class_breakpoint
      && cmdtype != class_trace && cmdtype != class_obscure
      && cmdtype != class_maintenance)
    {
      PyErr_Format (PyExc_RuntimeError, _("Invalid command class argument."));
      return -1;
    }

  if (parmclass != var_boolean /* ARI: var_boolean */
      && parmclass != var_auto_boolean
      && parmclass != var_uinteger && parmclass != var_integer
      && parmclass != var_string && parmclass != var_string_noescape
      && parmclass != var_optional_filename && parmclass != var_filename
      && parmclass != var_zinteger && parmclass != var_enum)
    {
      PyErr_SetString (PyExc_RuntimeError, _("Invalid parameter class argument."));
      return -1;
    }

  if (enum_values && parmclass != var_enum)
    {
      PyErr_SetString (PyExc_RuntimeError,
		       _("Only PARAM_ENUM accepts a fourth argument."));
      return -1;
    }
  if (parmclass == var_enum)
    {
      if (! compute_enum_values (obj, enum_values))
	return -1;
    }
  else
    obj->enumeration = NULL;
  obj->type = (enum var_types) parmclass;
  memset (&obj->value, 0, sizeof (obj->value));

  cmd_name = gdbpy_parse_command_name (name, &set_list,
				       &setlist);

  if (! cmd_name)
    return -1;
  xfree (cmd_name);
  cmd_name = gdbpy_parse_command_name (name, &show_list,
				       &showlist);
  if (! cmd_name)
    return -1;

  set_doc = get_doc_string (self, set_doc_cst);
  show_doc = get_doc_string (self, show_doc_cst);
  doc = get_doc_string (self, gdbpy_doc_cst);

  Py_INCREF (self);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      add_setshow_generic (parmclass, (enum command_class) cmdtype,
			   cmd_name, obj,
			   set_doc, show_doc,
			   doc, set_list, show_list);
    }
  if (except.reason < 0)
    {
      xfree (cmd_name);
      xfree (set_doc);
      xfree (show_doc);
      xfree (doc);
      Py_DECREF (self);
      PyErr_Format (except.reason == RETURN_QUIT
		    ? PyExc_KeyboardInterrupt : PyExc_RuntimeError,
		    "%s", except.message);
      return -1;
    }
  return 0;
}



/* Initialize the 'parameters' module.  */
void
gdbpy_initialize_parameters (void)
{
  int i;

  if (PyType_Ready (&parmpy_object_type) < 0)
    return;

  set_doc_cst = PyString_FromString ("set_doc");
  if (! set_doc_cst)
    return;
  show_doc_cst = PyString_FromString ("show_doc");
  if (! show_doc_cst)
    return;

  for (i = 0; parm_constants[i].name; ++i)
    {
      if (PyModule_AddIntConstant (gdb_module,
				   parm_constants[i].name,
				   parm_constants[i].value) < 0)
	return;
    }

  Py_INCREF (&parmpy_object_type);
  PyModule_AddObject (gdb_module, "Parameter",
		      (PyObject *) &parmpy_object_type);
}



static PyTypeObject parmpy_object_type =
{
  PyObject_HEAD_INIT (NULL)
  0,				  /*ob_size*/
  "gdb.Parameter",		  /*tp_name*/
  sizeof (parmpy_object),	  /*tp_basicsize*/
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
  get_attr,			  /*tp_getattro*/
  set_attr,			  /*tp_setattro*/
  0,				  /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "GDB parameter object",	  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  0,				  /* tp_methods */
  0,				  /* tp_members */
  0,				  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  parmpy_init,			  /* tp_init */
  0,				  /* tp_alloc */
  PyType_GenericNew		  /* tp_new */
};
