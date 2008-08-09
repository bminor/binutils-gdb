/* General python/gdb code

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
#include "command.h"
#include "ui-out.h"
#include "cli/cli-script.h"
#include "gdbcmd.h"

#include <ctype.h>

/* True if we should print the stack when catching a Python error,
   false otherwise.  */
static int gdbpy_should_print_stack = 1;

#ifdef HAVE_PYTHON

#include "python.h"
#include "libiberty.h"
#include "cli/cli-decode.h"
#include "charset.h"
#include "top.h"
#include "exceptions.h"
#include "python-internal.h"
#include "version.h"
#include "target.h"
#include "gdbthread.h"


PyObject *gdb_module;

static PyObject *get_parameter (PyObject *, PyObject *);
static PyObject *execute_gdb_command (PyObject *, PyObject *);
static PyObject *gdbpy_write (PyObject *, PyObject *);
static PyObject *gdbpy_flush (PyObject *, PyObject *);

static PyMethodDef GdbMethods[] =
{
  { "execute", execute_gdb_command, METH_VARARGS,
    "Execute a gdb command" },
  { "get_parameter", get_parameter, METH_VARARGS,
    "Return a gdb parameter's value" },

  { "write", gdbpy_write, METH_VARARGS,
    "Write a string using gdb's filtered stream." },
  { "flush", gdbpy_flush, METH_NOARGS,
    "Flush gdb's filtered stdout stream." },

  {NULL, NULL, 0, NULL}
};

/* Given a command_line, return a command string suitable for passing
   to Python.  Lines in the string are separated by newlines.  The
   return value is allocated using xmalloc and the caller is
   responsible for freeing it.  */

static char *
compute_python_string (struct command_line *l)
{
  struct command_line *iter;
  char *script = NULL;
  int size = 0;
  int here;

  for (iter = l; iter; iter = iter->next)
    size += strlen (iter->line) + 1;

  script = xmalloc (size + 1);
  here = 0;
  for (iter = l; iter; iter = iter->next)
    {
      int len = strlen (iter->line);
      strcpy (&script[here], iter->line);
      here += len;
      script[here++] = '\n';
    }
  script[here] = '\0';
  return script;
}

/* Take a command line structure representing a 'python' command, and
   evaluate its body using the Python interpreter.  */

void
eval_python_from_control_command (struct command_line *cmd)
{
  char *script;

  if (cmd->body_count != 1)
    error (_("Invalid \"python\" block structure."));

  script = compute_python_string (cmd->body_list[0]);
  PyRun_SimpleString (script);
  xfree (script);
  if (PyErr_Occurred ())
    {
      gdbpy_print_stack ();
      error (_("error while executing Python code"));
    }
}

/* Implementation of the gdb "python" command.  */

static void
python_command (char *arg, int from_tty)
{
  while (arg && *arg && isspace (*arg))
    ++arg;
  if (arg && *arg)
    {
      PyRun_SimpleString (arg);
      if (PyErr_Occurred ())
	{
	  gdbpy_print_stack ();
	  error (_("error while executing Python code"));
	}
    }
  else
    {
      struct command_line *l = get_command_line (python_control, "");
      struct cleanup *cleanups = make_cleanup_free_command_lines (&l);
      execute_control_command_untraced (l);
      do_cleanups (cleanups);
    }
}



/* Transform a gdb parameters's value into a Python value.  May return
   NULL (and set a Python exception) on error.  Helper function for
   get_parameter.  */

static PyObject *
parameter_to_python (struct cmd_list_element *cmd)
{
  switch (cmd->var_type)
    {
    case var_string:
    case var_string_noescape:
    case var_optional_filename:
    case var_filename:
    case var_enum:
      {
	char *str = * (char **) cmd->var;
	if (! str)
	  str = "";
	return PyString_Decode (str, strlen (str), host_charset (), NULL);
      }

    case var_boolean:
      {
	if (* (int *) cmd->var)
	  Py_RETURN_TRUE;
	else
	  Py_RETURN_FALSE;
      }

    case var_auto_boolean:
      {
	enum auto_boolean ab = * (enum auto_boolean *) cmd->var;
	if (ab == AUTO_BOOLEAN_TRUE)
	  Py_RETURN_TRUE;
	else if (ab == AUTO_BOOLEAN_FALSE)
	  Py_RETURN_FALSE;
	else
	  Py_RETURN_NONE;
      }

    case var_integer:
      if ((* (int *) cmd->var) == INT_MAX)
	Py_RETURN_NONE;
      /* Fall through.  */
    case var_zinteger:
      return PyLong_FromLong (* (int *) cmd->var);

    case var_uinteger:
      {
	unsigned int val = * (unsigned int *) cmd->var;
	if (val == UINT_MAX)
	  Py_RETURN_NONE;
	return PyLong_FromUnsignedLong (val);
      }
    }

  return PyErr_Format (PyExc_RuntimeError, "programmer error: unhandled type");
}

/* A Python function which returns a gdb parameter's value as a Python
   value.  */

static PyObject *
get_parameter (PyObject *self, PyObject *args)
{
  struct cmd_list_element *alias, *prefix, *cmd;
  char *arg, *newarg;
  volatile struct gdb_exception except;

  if (! PyArg_ParseTuple (args, "s", &arg))
    return NULL;

  newarg = concat ("show ", arg, (char *) NULL);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      if (! lookup_cmd_composition (newarg, &alias, &prefix, &cmd))
	{
	  xfree (newarg);
	  return PyErr_Format (PyExc_RuntimeError,
			       "could not find variable `%s'", arg);
	}
    }
  xfree (newarg);
  GDB_PY_HANDLE_EXCEPTION (except);

  if (! cmd->var)
    return PyErr_Format (PyExc_RuntimeError, "`%s' is not a variable", arg);
  return parameter_to_python (cmd);
}

/* A Python function which evaluates a string using the gdb CLI.  */

static PyObject *
execute_gdb_command (PyObject *self, PyObject *args)
{
  struct cmd_list_element *alias, *prefix, *cmd;
  char *arg, *newarg;
  volatile struct gdb_exception except;
  struct cleanup *old_chain;

  if (! PyArg_ParseTuple (args, "s", &arg))
    return NULL;

  old_chain = make_cleanup (null_cleanup, 0);

  TRY_CATCH (except, RETURN_MASK_ALL)
    {
      execute_command (arg, 0);
    }
  GDB_PY_HANDLE_EXCEPTION (except);

  /* Do any commands attached to breakpoint we stopped at. Only if we
     are always running synchronously. Or if we have just executed a
     command that doesn't start the target. */
  if (!target_can_async_p () || !is_running (inferior_ptid))
    {
      bpstat_do_actions (&stop_bpstat);
      do_cleanups (old_chain);
    }

  Py_RETURN_NONE;
}



/* Printing.  */

/* A python function to write a single string using gdb's filtered
   output stream.  */
static PyObject *
gdbpy_write (PyObject *self, PyObject *args)
{
  char *arg;
  if (! PyArg_ParseTuple (args, "s", &arg))
    return NULL;
  printf_filtered ("%s", arg);
  Py_RETURN_NONE;
}

/* A python function to flush gdb's filtered output stream.  */
static PyObject *
gdbpy_flush (PyObject *self, PyObject *args)
{
  gdb_flush (gdb_stdout);
  Py_RETURN_NONE;
}

/* Print a python exception trace, or print nothing and clear the
   python exception, depending on gdbpy_should_print_stack.  Only call
   this if a python exception is set.  */
void
gdbpy_print_stack (void)
{
  if (gdbpy_should_print_stack)
    PyErr_Print ();
  else
    PyErr_Clear ();
}

#else /* HAVE_PYTHON */

/* Dummy implementation of the gdb "python" command.  */

static void
python_command (char *arg, int from_tty)
{
  while (arg && *arg && isspace (*arg))
    ++arg;
  if (arg && *arg)
    error (_("Python scripting is not supported in this copy of GDB."));
  else
    {
      struct command_line *l = get_command_line (python_control, "");
      struct cleanup *cleanups = make_cleanup_free_command_lines (&l);
      execute_control_command_untraced (l);
      do_cleanups (cleanups);
    }
}

void
eval_python_from_control_command (struct command_line *cmd)
{
  error (_("Python scripting is not supported in this copy of GDB."));
}

#endif /* HAVE_PYTHON */



/* Lists for 'maint set python' commands.  */

static struct cmd_list_element *set_python_list;
static struct cmd_list_element *show_python_list;

/* Function for use by 'maint set python' prefix command.  */

static void
set_python (char *args, int from_tty)
{
  help_list (set_python_list, "maintenance set python ", -1, gdb_stdout);
}

/* Function for use by 'maint show python' prefix command.  */

static void
show_python (char *args, int from_tty)
{
  cmd_show_list (show_python_list, from_tty, "");
}

/* Initialize the Python code.  */

void
_initialize_python (void)
{
  add_com ("python", class_obscure, python_command,
#ifdef HAVE_PYTHON
	   _("\
Evaluate a Python command.\n\
\n\
The command can be given as an argument, for instance:\n\
\n\
    python print 23\n\
\n\
If no argument is given, the following lines are read and used\n\
as the Python commands.  Type a line containing \"end\" to indicate\n\
the end of the command.")
#else /* HAVE_PYTHON */
	   _("\
Evaluate a Python command.\n\
\n\
Python scripting is not supported in this copy of GDB.\n\
This command is only a placeholder.")
#endif /* HAVE_PYTHON */
	   );

  add_prefix_cmd ("python", no_class, show_python,
		  _("Prefix command for python maintenance settings."),
		  &show_python_list, "maintenance show python ", 0,
		  &maintenance_show_cmdlist);
  add_prefix_cmd ("python", no_class, set_python,
		  _("Prefix command for python maintenance settings."),
		  &set_python_list, "maintenance set python ", 0,
		  &maintenance_set_cmdlist);

  add_setshow_boolean_cmd ("print-stack", class_maintenance,
			   &gdbpy_should_print_stack, _("\
Enable or disable printing of Python stack dump on error."), _("\
Show whether Python stack will be printed on error."), _("\
Enables or disables printing of Python stack traces."),
			   NULL, NULL,
			   &set_python_list,
			   &show_python_list);

#ifdef HAVE_PYTHON
  Py_Initialize ();

  gdb_module = Py_InitModule ("gdb", GdbMethods);

  /* The casts to (char*) are for python 2.4.  */
  PyModule_AddStringConstant (gdb_module, "VERSION", (char*) version);
  PyModule_AddStringConstant (gdb_module, "HOST_CONFIG", (char*) host_name);
  PyModule_AddStringConstant (gdb_module, "TARGET_CONFIG", (char*) target_name);

  PyRun_SimpleString ("import gdb");

  /* Create a couple objects which are used for Python's stdout and
     stderr.  */
  PyRun_SimpleString ("\
import sys\n\
class GdbOutputFile:\n\
  def close(self):\n\
    # Do nothing.\n\
    return None\n\
\n\
  def isatty(self):\n\
    return False\n\
\n\
  def write(self, s):\n\
    gdb.write(s)\n\
\n\
  def writelines(self, iterable):\n\
    for line in iterable:\n\
      self.write(line)\n\
\n\
  def flush(self):\n\
    gdb.flush()\n\
\n\
sys.stderr = GdbOutputFile()\n\
sys.stdout = GdbOutputFile()\n\
");
#endif /* HAVE_PYTHON */
}
