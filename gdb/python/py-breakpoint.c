/* Python interface to breakpoints

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
#include "breakpoint.h"
#include "gdbcmd.h"
#include "gdbthread.h"
#include "observer.h"
#include "cli/cli-script.h"
#include "ada-lang.h"

/* From breakpoint.c.  */
typedef struct breakpoint_object breakpoint_object;

static PyTypeObject breakpoint_object_type;

/* A dynamically allocated vector of breakpoint objects.  Each
   breakpoint has a number.  A breakpoint is valid if its slot in this
   vector is non-null.  When a breakpoint is deleted, we drop our
   reference to it and zero its slot; this is how we let the Python
   object have a lifetime which is independent from that of the gdb
   breakpoint.  */
static breakpoint_object **bppy_breakpoints;

/* Number of slots in bppy_breakpoints.  */
static int bppy_slots;

/* Number of live breakpoints.  */
static int bppy_live;

/* Variables used to pass information between the Breakpoint
   constructor and the breakpoint-created hook function.  */
static breakpoint_object *bppy_pending_object;

struct breakpoint_object
{
  PyObject_HEAD

  /* The breakpoint number according to gdb.  */
  int number;

  /* The gdb breakpoint object, or NULL if the breakpoint has been
     deleted.  */
  struct breakpoint *bp;
};

/* Require that BREAKPOINT be a valid breakpoint ID; throw a Python
   exception if it is invalid.  */
#define BPPY_REQUIRE_VALID(Breakpoint)					\
    do {								\
      if (! bpnum_is_valid ((Breakpoint)->number))			\
	return PyErr_Format (PyExc_RuntimeError, _("Breakpoint %d is invalid."), \
			     (Breakpoint)->number);			\
    } while (0)

/* Require that BREAKPOINT be a valid breakpoint ID; throw a Python
   exception if it is invalid.  This macro is for use in setter functions.  */
#define BPPY_SET_REQUIRE_VALID(Breakpoint)				\
    do {								\
      if (! bpnum_is_valid ((Breakpoint)->number))			\
        {								\
	  PyErr_Format (PyExc_RuntimeError, _("Breakpoint %d is invalid."), \
			(Breakpoint)->number);				\
	  return -1;							\
	}								\
    } while (0)

/* This is used to initialize various gdb.bp_* constants.  */
struct pybp_code
{
  /* The name.  */
  const char *name;
  /* The code.  */
  enum type_code code;
};

/* Entries related to the type of user set breakpoints.  */
static struct pybp_code pybp_codes[] =
{
  { "BP_NONE", bp_none},
  { "BP_BREAKPOINT", bp_breakpoint},
  { "BP_WATCHPOINT", bp_watchpoint},
  { "BP_HARDWARE_WATCHPOINT", bp_hardware_watchpoint},
  { "BP_READ_WATCHPOINT", bp_read_watchpoint},
  { "BP_ACCESS_WATCHPOINT", bp_access_watchpoint},
  {NULL} /* Sentinel.  */
};

/* Entries related to the type of watchpoint.  */
static struct pybp_code pybp_watch_types[] =
{
  { "WP_READ", hw_read},
  { "WP_WRITE", hw_write},
  { "WP_ACCESS", hw_access},
  {NULL} /* Sentinel.  */
};

/* Evaluate to true if the breakpoint NUM is valid, false otherwise.  */
static int 
bpnum_is_valid (int num)
{
  if (num >=0 
      && num < bppy_slots 
      && bppy_breakpoints[num] != NULL)
    return 1;
  
  return 0;
}

/* Python function which checks the validity of a breakpoint object.  */
static PyObject *
bppy_is_valid (PyObject *self, PyObject *args)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  if (self_bp->bp)
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

/* Python function to test whether or not the breakpoint is enabled.  */
static PyObject *
bppy_get_enabled (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);
  if (! self_bp->bp)
    Py_RETURN_FALSE;
  if (self_bp->bp->enable_state == bp_enabled)
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

/* Python function to test whether or not the breakpoint is silent.  */
static PyObject *
bppy_get_silent (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);
  if (self_bp->bp->silent)
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

/* Python function to set the enabled state of a breakpoint.  */
static int
bppy_set_enabled (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  int cmp;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `enabled' attribute."));

      return -1;
    }
  else if (! PyBool_Check (newvalue))
    {
      PyErr_SetString (PyExc_TypeError,
		       _("The value of `enabled' must be a boolean."));
      return -1;
    }

  cmp = PyObject_IsTrue (newvalue);
  if (cmp < 0)
    return -1;
  else if (cmp == 1)
    enable_breakpoint (self_bp->bp);
  else 
    disable_breakpoint (self_bp->bp);
  return 0;
}

/* Python function to set the 'silent' state of a breakpoint.  */
static int
bppy_set_silent (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  int cmp;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `silent' attribute."));
      return -1;
    }
  else if (! PyBool_Check (newvalue))
    {
      PyErr_SetString (PyExc_TypeError,
		       _("The value of `silent' must be a boolean."));
      return -1;
    }

  cmp = PyObject_IsTrue (newvalue);
  if (cmp < 0)
    return -1;
  else
    self_bp->bp->silent = cmp;

  return 0;
}

/* Python function to set the thread of a breakpoint.  */
static int
bppy_set_thread (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  int id;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `thread' attribute."));
      return -1;
    }
  else if (PyInt_Check (newvalue))
    {
      id = (int) PyInt_AsLong (newvalue);
      if (! valid_thread_id (id))
	{
	  PyErr_SetString (PyExc_RuntimeError, 
			   _("Invalid thread ID."));
	  return -1;
	}
    }
  else if (newvalue == Py_None)
    id = -1;
  else
    {
      PyErr_SetString (PyExc_TypeError,
		       _("The value of `thread' must be an integer or None."));
      return -1;
    }

  self_bp->bp->thread = id;

  return 0;
}

/* Python function to set the (Ada) task of a breakpoint.  */
static int
bppy_set_task (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  int id;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `task' attribute."));
      return -1;
    }
  else if (PyInt_Check (newvalue))
    {
      id = (int) PyInt_AsLong (newvalue);
      if (! valid_task_id (id))
	{
	  PyErr_SetString (PyExc_RuntimeError, 
			   _("Invalid task ID."));
	  return -1;
	}
    }
  else if (newvalue == Py_None)
    id = 0;
  else
    {
      PyErr_SetString (PyExc_TypeError,
		       _("The value of `task' must be an integer or None."));
      return -1;
    }

  self_bp->bp->task = id;

  return 0;
}


/* Python function to set the ignore count of a breakpoint.  */
static int
bppy_set_ignore_count (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  long value;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError,
		       _("Cannot delete `ignore_count' attribute."));
      return -1;
    }
  else if (! PyInt_Check (newvalue))
    {
      PyErr_SetString (PyExc_TypeError,
		       _("The value of `ignore_count' must be an integer."));
      return -1;
    }

  value = PyInt_AsLong (newvalue);
  if (value < 0)
    value = 0;
  set_ignore_count (self_bp->number, (int) value, 0);

  return 0;
}

/* Python function to set the hit count of a breakpoint.  */
static int
bppy_set_hit_count (PyObject *self, PyObject *newvalue, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `hit_count' attribute."));
      return -1;
    }
  else if (! PyInt_Check (newvalue) || PyInt_AsLong (newvalue) != 0)
    {
      PyErr_SetString (PyExc_AttributeError,
		       _("The value of `hit_count' must be zero."));
      return -1;
    }

  self_bp->bp->hit_count = 0;

  return 0;
}

/* Python function to get the location of a breakpoint.  */
static PyObject *
bppy_get_location (PyObject *self, void *closure)
{
  char *str;
  breakpoint_object *obj = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (obj);

  if (obj->bp->type != bp_breakpoint)
    Py_RETURN_NONE;

  str = obj->bp->addr_string;

  if (! str)
    str = "";
  return PyString_Decode (str, strlen (str), host_charset (), NULL);
}

/* Python function to get the breakpoint expression.  */
static PyObject *
bppy_get_expression (PyObject *self, void *closure)
{
  char *str;
  breakpoint_object *obj = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (obj);

  if (obj->bp->type != bp_watchpoint
      && obj->bp->type != bp_hardware_watchpoint  
      && obj->bp->type != bp_read_watchpoint
      && obj->bp->type != bp_access_watchpoint)
    Py_RETURN_NONE;

  str = obj->bp->exp_string;
  if (! str)
    str = "";

  return PyString_Decode (str, strlen (str), host_charset (), NULL);
}

/* Python function to get the condition expression of a breakpoint.  */
static PyObject *
bppy_get_condition (PyObject *self, void *closure)
{
  char *str;
  breakpoint_object *obj = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (obj);

  str = obj->bp->cond_string;
  if (! str)
    Py_RETURN_NONE;

  return PyString_Decode (str, strlen (str), host_charset (), NULL);
}

static int
bppy_set_condition (PyObject *self, PyObject *newvalue, void *closure)
{
  char *exp;
  breakpoint_object *self_bp = (breakpoint_object *) self;
  volatile struct gdb_exception except;

  BPPY_SET_REQUIRE_VALID (self_bp);

  if (newvalue == NULL)
    {
      PyErr_SetString (PyExc_TypeError, 
		       _("Cannot delete `condition' attribute."));
      return -1;
    }
  else if (newvalue == Py_None)
    exp = "";
  else
    {
      exp = python_string_to_host_string (newvalue);
      if (exp == NULL)
	return -1;
    }

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      set_breakpoint_condition (self_bp->bp, exp, 0);
    }
  GDB_PY_SET_HANDLE_EXCEPTION (except);

  return 0;
}

/* Python function to get the commands attached to a breakpoint.  */
static PyObject *
bppy_get_commands (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;
  struct breakpoint *bp = self_bp->bp;
  long length;
  volatile struct gdb_exception except;
  struct ui_file *string_file;
  struct cleanup *chain;
  PyObject *result;
  char *cmdstr;

  BPPY_REQUIRE_VALID (self_bp);

  if (! self_bp->bp->commands)
    Py_RETURN_NONE;

  string_file = mem_fileopen ();
  chain = make_cleanup_ui_file_delete (string_file);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      ui_out_redirect (uiout, string_file);
      print_command_lines (uiout, breakpoint_commands (bp), 0);
      ui_out_redirect (uiout, NULL);
    }
  cmdstr = ui_file_xstrdup (string_file, &length);
  GDB_PY_HANDLE_EXCEPTION (except);

  result = PyString_Decode (cmdstr, strlen (cmdstr), host_charset (), NULL);
  do_cleanups (chain);
  xfree (cmdstr);
  return result;
}

/* Python function to get the breakpoint type.  */
static PyObject *
bppy_get_type (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  return PyInt_FromLong (self_bp->bp->type);
}

/* Python function to get the breakpoint's number.  */
static PyObject *
bppy_get_number (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  return PyInt_FromLong (self_bp->number);
}

/* Python function to get the breakpoint's thread ID.  */
static PyObject *
bppy_get_thread (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  if (self_bp->bp->thread == -1)
    Py_RETURN_NONE;

  return PyInt_FromLong (self_bp->bp->thread);
}

/* Python function to get the breakpoint's task ID (in Ada).  */
static PyObject *
bppy_get_task (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  if (self_bp->bp->task == 0)
    Py_RETURN_NONE;

  return PyInt_FromLong (self_bp->bp->task);
}

/* Python function to get the breakpoint's hit count.  */
static PyObject *
bppy_get_hit_count (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  return PyInt_FromLong (self_bp->bp->hit_count);
}

/* Python function to get the breakpoint's ignore count.  */
static PyObject *
bppy_get_ignore_count (PyObject *self, void *closure)
{
  breakpoint_object *self_bp = (breakpoint_object *) self;

  BPPY_REQUIRE_VALID (self_bp);

  return PyInt_FromLong (self_bp->bp->ignore_count);
}

/* Python function to create a new breakpoint.  */
static PyObject *
bppy_new (PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
  PyObject *result;
  static char *keywords[] = { "spec", "type", "wp_class", NULL };
  char *spec;
  int type = bp_breakpoint;
  int access_type = hw_write;
  volatile struct gdb_exception except;

  if (! PyArg_ParseTupleAndKeywords (args, kwargs, "s|ii", keywords,
				     &spec, &type, &access_type))
    return NULL;

  result = subtype->tp_alloc (subtype, 0);
  if (! result)
    return NULL;
  bppy_pending_object = (breakpoint_object *) result;
  bppy_pending_object->number = -1;
  bppy_pending_object->bp = NULL;
  
  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      switch (type)
	{
	case bp_breakpoint:
	  {
	    create_breakpoint (python_gdbarch,
			       spec, NULL, -1,
			       0,
			       0, bp_breakpoint,
			       0,
			       AUTO_BOOLEAN_TRUE,
			       NULL, 0, 1);
	    break;
	  }
        case bp_watchpoint:
	  {
	    if (access_type == hw_write)
	      watch_command_wrapper (spec, 0);
	    else if (access_type == hw_access)
	      awatch_command_wrapper (spec, 0);
	    else if (access_type == hw_read)
	      rwatch_command_wrapper (spec, 0);
	    else
	      error(_("Cannot understand watchpoint access type."));
	    break;
	  }
	default:
	  error(_("Do not understand breakpoint type to set."));
	}
    }
  if (except.reason < 0)
    {
      subtype->tp_free (result);
      return PyErr_Format (except.reason == RETURN_QUIT
			   ? PyExc_KeyboardInterrupt : PyExc_RuntimeError,
			   "%s", except.message);
    }

  BPPY_REQUIRE_VALID ((breakpoint_object *) result);
  return result;
}



/* Static function to return a tuple holding all breakpoints.  */

PyObject *
gdbpy_breakpoints (PyObject *self, PyObject *args)
{
  PyObject *result;

  if (bppy_live == 0)
    Py_RETURN_NONE;

  result = PyTuple_New (bppy_live);
  if (result)
    {
      int i, out = 0;

      for (i = 0; out < bppy_live; ++i)
	{
	  if (! bppy_breakpoints[i])
	    continue;
	  Py_INCREF (bppy_breakpoints[i]);
	  PyTuple_SetItem (result, out, (PyObject *) bppy_breakpoints[i]);
	  ++out;
	}
    }
  return result;
}



/* Event callback functions.  */

/* Callback that is used when a breakpoint is created.  This function
   will create a new Python breakpoint object.  */
static void
gdbpy_breakpoint_created (int num)
{
  breakpoint_object *newbp;
  struct breakpoint *bp = NULL;
  PyGILState_STATE state;

  if (num < 0)
    return;

  bp = get_breakpoint (num);
  if (! bp)
    return;

  if (bp->type != bp_breakpoint 
      && bp->type != bp_watchpoint
      && bp->type != bp_hardware_watchpoint  
      && bp->type != bp_read_watchpoint
      && bp->type != bp_access_watchpoint)
    return;

  if (num >= bppy_slots)
    {
      int old = bppy_slots;

      bppy_slots = bppy_slots * 2 + 10;
      bppy_breakpoints
	= (breakpoint_object **) xrealloc (bppy_breakpoints,
					   (bppy_slots
					    * sizeof (breakpoint_object *)));
      memset (&bppy_breakpoints[old], 0,
	      (bppy_slots - old) * sizeof (PyObject *));
    }

  ++bppy_live;

  state = PyGILState_Ensure ();

  if (bppy_pending_object)
    {
      newbp = bppy_pending_object;
      bppy_pending_object = NULL;
    }
  else
    newbp = PyObject_New (breakpoint_object, &breakpoint_object_type);
  if (newbp)
    {
      newbp->number = num;
      newbp->bp = bp;
      bppy_breakpoints[num] = newbp;
      Py_INCREF (newbp);
    }

  /* Just ignore errors here.  */
  PyErr_Clear ();

  PyGILState_Release (state);
}

/* Callback that is used when a breakpoint is deleted.  This will
   invalidate the corresponding Python object.  */
static void
gdbpy_breakpoint_deleted (int num)
{
  PyGILState_STATE state;

  state = PyGILState_Ensure ();
  if (bpnum_is_valid (num))
    {
      bppy_breakpoints[num]->bp = NULL;
      Py_DECREF (bppy_breakpoints[num]);
      bppy_breakpoints[num] = NULL;
      --bppy_live;
    }
  PyGILState_Release (state);
}



/* Initialize the Python breakpoint code.  */
void
gdbpy_initialize_breakpoints (void)
{
  int i;

  breakpoint_object_type.tp_new = bppy_new;
  if (PyType_Ready (&breakpoint_object_type) < 0)
    return;

  Py_INCREF (&breakpoint_object_type);
  PyModule_AddObject (gdb_module, "Breakpoint",
		      (PyObject *) &breakpoint_object_type);

  observer_attach_breakpoint_created (gdbpy_breakpoint_created);
  observer_attach_breakpoint_deleted (gdbpy_breakpoint_deleted);

  /* Add breakpoint types constants.  */
  for (i = 0; pybp_codes[i].name; ++i)
    {
      if (PyModule_AddIntConstant (gdb_module,
				   /* Cast needed for Python 2.4.  */
				   (char *) pybp_codes[i].name,
				   pybp_codes[i].code) < 0)
	return;
    }

  /* Add watchpoint types constants.  */
  for (i = 0; pybp_watch_types[i].name; ++i)
    {
      if (PyModule_AddIntConstant (gdb_module,
				   /* Cast needed for Python 2.4.  */
				   (char *) pybp_watch_types[i].name,
				   pybp_watch_types[i].code) < 0)
	return;
    }

}



static PyGetSetDef breakpoint_object_getset[] = {
  { "enabled", bppy_get_enabled, bppy_set_enabled,
    "Boolean telling whether the breakpoint is enabled.", NULL },
  { "silent", bppy_get_silent, bppy_set_silent,
    "Boolean telling whether the breakpoint is silent.", NULL },
  { "thread", bppy_get_thread, bppy_set_thread,
    "Thread ID for the breakpoint.\n\
If the value is a thread ID (integer), then this is a thread-specific breakpoint.\n\
If the value is None, then this breakpoint is not thread-specific.\n\
No other type of value can be used.", NULL },
  { "task", bppy_get_task, bppy_set_task,
    "Thread ID for the breakpoint.\n\
If the value is a task ID (integer), then this is an Ada task-specific breakpoint.\n\
If the value is None, then this breakpoint is not task-specific.\n\
No other type of value can be used.", NULL },
  { "ignore_count", bppy_get_ignore_count, bppy_set_ignore_count,
    "Number of times this breakpoint should be automatically continued.",
    NULL },
  { "number", bppy_get_number, NULL,
    "Breakpoint's number assigned by GDB.", NULL },
  { "hit_count", bppy_get_hit_count, bppy_set_hit_count,
    "Number of times the breakpoint has been hit.\n\
Can be set to zero to clear the count. No other value is valid\n\
when setting this property.", NULL },
  { "location", bppy_get_location, NULL,
    "Location of the breakpoint, as specified by the user.", NULL},
  { "expression", bppy_get_expression, NULL,
    "Expression of the breakpoint, as specified by the user.", NULL},
  { "condition", bppy_get_condition, bppy_set_condition,
    "Condition of the breakpoint, as specified by the user,\
or None if no condition set."},
  { "commands", bppy_get_commands, NULL,
    "Commands of the breakpoint, as specified by the user."},
  { "type", bppy_get_type, NULL,
    "Type of breakpoint."},
  { NULL }  /* Sentinel.  */
};

static PyMethodDef breakpoint_object_methods[] =
{
  { "is_valid", bppy_is_valid, METH_NOARGS,
    "Return true if this breakpoint is valid, false if not." },
  { NULL } /* Sentinel.  */
};

static PyTypeObject breakpoint_object_type =
{
  PyObject_HEAD_INIT (NULL)
  0,				  /*ob_size*/
  "gdb.Breakpoint",		  /*tp_name*/
  sizeof (breakpoint_object),	  /*tp_basicsize*/
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
  "GDB breakpoint object",	  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  breakpoint_object_methods,	  /* tp_methods */
  0,				  /* tp_members */
  breakpoint_object_getset	  /* tp_getset */
};
