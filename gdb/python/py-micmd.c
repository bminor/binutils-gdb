/* MI Command Set for GDB, the GNU debugger.

   Copyright (C) 2019-2022 Free Software Foundation, Inc.

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

/* GDB/MI commands implemented in Python.  */

#include "defs.h"
#include "python-internal.h"
#include "arch-utils.h"
#include "charset.h"
#include "language.h"
#include "mi/mi-cmds.h"
#include "mi/mi-parse.h"
#include "cli/cli-cmds.h"
#include <string>

/* Debugging of Python MI commands.  */

static bool pymicmd_debug;

/* Implementation of "show debug py-micmd".  */

static void
show_pymicmd_debug (struct ui_file *file, int from_tty,
		    struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Python MI command debugging is %s.\n"), value);
}

/* Print a "py-micmd" debug statement.  */

#define pymicmd_debug_printf(fmt, ...) \
  debug_prefixed_printf_cond (pymicmd_debug, "py-micmd", fmt, ##__VA_ARGS__)

/* Print a "py-micmd" enter/exit debug statements.  */

#define PYMICMD_SCOPED_DEBUG_ENTER_EXIT \
  scoped_debug_enter_exit (pymicmd_debug, "py-micmd")

struct mi_command_py;

/* Representation of a Python gdb.MICommand object.  */

struct micmdpy_object
{
  PyObject_HEAD

  /* The object representing this command in the MI command table.  This
     pointer can be nullptr if the command is not currently installed into
     the MI command table (see gdb.MICommand.installed property).  */
  struct mi_command_py *mi_command;

  /* The string representing the name of this command, without the leading
     dash.  This string is never nullptr once the Python object has been
     initialised.

     The memory for this string was allocated with malloc, and needs to be
     deallocated with free when the Python object is deallocated.

     When the MI_COMMAND field is not nullptr, then the mi_command_py
     object's name will point back to this string.  */
  char *mi_command_name;
};

/* The MI command implemented in Python.  */

struct mi_command_py : public mi_command
{
  /* Constructs a new mi_command_py object.  NAME is command name without
     leading dash.  OBJECT is a reference to a Python object implementing
     the command.  This object must inherit from gdb.MICommand and must
     implement the invoke method.  */

  mi_command_py (const char *name, micmdpy_object *object)
    : mi_command (name, nullptr),
      m_pyobj (object)
  {
    pymicmd_debug_printf ("this = %p", this);
  }

  ~mi_command_py ()
  {
    /* The Python object representing a MI command contains a pointer back
       to this c++ object.  We can safely set this pointer back to nullptr
       now, to indicate the Python object no longer references a valid c++
       object.

       However, the Python object also holds the storage for our name
       string.  We can't clear that here as our parent's destructor might
       still want to reference that string.  Instead we rely on the Python
       object deallocator to free that memory, and reset the pointer.  */
    m_pyobj->mi_command = nullptr;

    pymicmd_debug_printf ("this = %p", this);
  };

  /* Validate that CMD_OBJ, a non-nullptr pointer, is installed into the MI
     command table correctly.  This function looks up the command in the MI
     command table and checks that the object we get back references
     CMD_OBJ.  This function is only intended for calling within a
     gdb_assert.  This function performs many assertions internally, and
     then always returns true.  */
  static void validate_installation (micmdpy_object *cmd_obj);

  /* Update m_pyobj to NEW_PYOBJ.  The pointer from M_PYOBJ that points
     back to this object is swapped with the pointer in NEW_PYOBJ, which
     must be nullptr, so that NEW_PYOBJ now points back to this object.
     Additionally our parent's name string is stored in m_pyobj, so we
     swap the name string with NEW_PYOBJ.

     Before this call m_pyobj is the Python object representing this MI
     command object.  After this call has completed, NEW_PYOBJ now
     represents this MI command object.  */
  void swap_python_object (micmdpy_object *new_pyobj)
  {
    gdb_assert (new_pyobj->mi_command == nullptr);
    std::swap (new_pyobj->mi_command, m_pyobj->mi_command);
    std::swap (new_pyobj->mi_command_name, m_pyobj->mi_command_name);
    m_pyobj = new_pyobj;
  }

  /* Called when the MI command is invoked.  */
  virtual void invoke(struct mi_parse *parse) const override;

private:
  /* The Python object representing this MI command.  */
  micmdpy_object *m_pyobj;
};

extern PyTypeObject micmdpy_object_type
	CPYCHECKER_TYPE_OBJECT_FOR_TYPEDEF ("micmdpy_object");

/* Holds a Python object containing the string 'invoke'.  */

static PyObject *invoke_cst;

/* Convert KEY_OBJ into a string that can be used as a field name in MI
   output.  KEY_OBJ must be a Python string object, and must only contain
   characters suitable for use as an MI field name.

   If KEY_OBJ is not a string, or if KEY_OBJ contains invalid characters,
   then an error is thrown.  Otherwise, KEY_OBJ is converted to a string
   and returned.  */

static gdb::unique_xmalloc_ptr<char>
py_object_to_mi_key (PyObject *key_obj)
{
  /* The key must be a string.  */
  if (!PyString_Check (key_obj))
    {
      gdbpy_ref<> key_repr (PyObject_Repr (key_obj));
      gdb::unique_xmalloc_ptr<char> key_repr_string;
      if (key_repr != nullptr)
	key_repr_string = python_string_to_target_string (key_repr.get ());
      if (key_repr_string == nullptr)
	gdbpy_handle_exception ();

      gdbpy_error (_("non-string object used as key: %s"),
		   key_repr_string.get ());
    }

  gdb::unique_xmalloc_ptr<char> key_string
    = python_string_to_target_string (key_obj);
  if (key_string == nullptr)
    gdbpy_handle_exception ();

  /* Predicate function, returns true if NAME is a valid field name for use
     in MI result output, otherwise, returns false.  */
  auto is_valid_key_name = [] (const char *name) -> bool
  {
    gdb_assert (name != nullptr);

    if (*name == '\0' || !isalpha (*name))
      return false;

    for (; *name != '\0'; ++name)
      if (!isalnum (*name) && *name != '_' && *name != '-')
	return false;

    return true;
  };

  if (!is_valid_key_name (key_string.get ()))
    {
      if (*key_string.get () == '\0')
	gdbpy_error (_("Invalid empty key in MI result"));
      else
	gdbpy_error (_("Invalid key in MI result: %s"), key_string.get ());
    }

  return key_string;
}

/* Serialize RESULT and print it in MI format to the current_uiout.
   FIELD_NAME is used as the name of this result field.

   RESULT can be a dictionary, a sequence, an iterator, or an object that
   can be converted to a string, these are converted to the matching MI
   output format (dictionaries as tuples, sequences and iterators as lists,
   and strings as named fields).

   If anything goes wrong while formatting the output then an error is
   thrown.

   This function is the recursive inner core of serialize_mi_result, and
   should only be called from that function.  */

static void
serialize_mi_result_1 (PyObject *result, const char *field_name)
{
  struct ui_out *uiout = current_uiout;

  if (PyDict_Check (result))
    {
      PyObject *key, *value;
      Py_ssize_t pos = 0;
      ui_out_emit_tuple tuple_emitter (uiout, field_name);
      while (PyDict_Next (result, &pos, &key, &value))
	{
	  gdb::unique_xmalloc_ptr<char> key_string
	    (py_object_to_mi_key (key));
	  serialize_mi_result_1 (value, key_string.get ());
	}
    }
  else if (PySequence_Check (result) && !PyString_Check (result))
    {
      ui_out_emit_list list_emitter (uiout, field_name);
      Py_ssize_t len = PySequence_Size (result);
      if (len == -1)
	gdbpy_handle_exception ();
      for (Py_ssize_t i = 0; i < len; ++i)
	{
          gdbpy_ref<> item (PySequence_ITEM (result, i));
	  if (item == nullptr)
	    gdbpy_handle_exception ();
	  serialize_mi_result_1 (item.get (), nullptr);
	}
    }
  else if (PyIter_Check (result))
    {
      gdbpy_ref<> item;
      ui_out_emit_list list_emitter (uiout, field_name);
      while (true)
	{
	  item.reset (PyIter_Next (result));
	  if (item == nullptr)
	    {
	      if (PyErr_Occurred () != nullptr)
		gdbpy_handle_exception ();
	      break;
	    }
	  serialize_mi_result_1 (item.get (), nullptr);
	}
    }
  else
    {
      gdb::unique_xmalloc_ptr<char> string (gdbpy_obj_to_string (result));
      if (string == nullptr)
	gdbpy_handle_exception ();
      uiout->field_string (field_name, string.get ());
    }
}

/* Serialize RESULT and print it in MI format to the current_uiout.

   This function handles the top-level result initially returned from the
   invoke method of the Python command implementation.  At the top-level
   the result must be a dictionary.  The values within this dictionary can
   be a wider range of types.  Handling the values of the top-level
   dictionary is done by serialize_mi_result_1, see that function for more
   details.

   If anything goes wrong while parsing and printing the MI output then an
   error is thrown.  */

static void
serialize_mi_result (PyObject *result)
{
  /* At the top-level, the result must be a dictionary.  */

  if (!PyDict_Check (result))
    gdbpy_error (_("Result from invoke must be a dictionary"));

  PyObject *key, *value;
  Py_ssize_t pos = 0;
  while (PyDict_Next (result, &pos, &key, &value))
    {
      gdb::unique_xmalloc_ptr<char> key_string
	(py_object_to_mi_key (key));
      serialize_mi_result_1 (value, key_string.get ());
    }
}

/* Called when the MI command is invoked.  PARSE contains the parsed
   command line arguments from the user.  */

void
mi_command_py::invoke (struct mi_parse *parse) const
{
  PYMICMD_SCOPED_DEBUG_ENTER_EXIT;

  pymicmd_debug_printf ("this = %p, name = %s", this, name ());

  mi_parse_argv (parse->args, parse);

  if (parse->argv == nullptr)
    error (_("Problem parsing arguments: %s %s"), parse->command, parse->args);

  PyObject *obj = (PyObject *) this->m_pyobj;
  gdb_assert (obj != nullptr);

  gdbpy_enter enter_py;

  /* Place all the arguments into a list which we pass as a single argument
     to the MI command's invoke method.  */
  gdbpy_ref<> argobj (PyList_New (parse->argc));
  if (argobj == nullptr)
    gdbpy_handle_exception ();

  for (int i = 0; i < parse->argc; ++i)
    {
      gdbpy_ref<> str (PyUnicode_Decode (parse->argv[i],
					 strlen (parse->argv[i]),
					 host_charset (), nullptr));
      if (PyList_SetItem (argobj.get (), i, str.release ()) < 0)
	gdbpy_handle_exception ();
    }

  gdb_assert (PyErr_Occurred () == nullptr);
  gdbpy_ref<> result (PyObject_CallMethodObjArgs (obj, invoke_cst,
						  argobj.get (), nullptr));
  if (result == nullptr)
    gdbpy_handle_exception ();

  if (result != Py_None)
    serialize_mi_result (result.get ());
}

/* See declaration above.  */

void
mi_command_py::validate_installation (micmdpy_object *cmd_obj)
{
  gdb_assert (cmd_obj != nullptr);
  mi_command_py *cmd = cmd_obj->mi_command;
  gdb_assert (cmd != nullptr);
  const char *name = cmd_obj->mi_command_name;
  gdb_assert (name != nullptr);
  gdb_assert (name == cmd->name ());
  mi_command *mi_cmd = mi_cmd_lookup (name);
  gdb_assert (mi_cmd == cmd);
  gdb_assert (cmd->m_pyobj == cmd_obj);
}

/* Return a reference to the gdb._mi_commands dictionary.  If the
   dictionary can't be found for any reason then nullptr is returned, and
   a Python exception will be set.  */

static gdbpy_ref<>
micmdpy_global_command_dictionary ()
{
  if (gdb_python_module == nullptr)
    {
      PyErr_SetString (PyExc_RuntimeError, _("unable to find gdb module"));
      return nullptr;
    }

  gdbpy_ref<> mi_cmd_dict (PyObject_GetAttrString (gdb_python_module,
						   "_mi_commands"));
  if (mi_cmd_dict == nullptr)
    return nullptr;

  if (!PyDict_Check (mi_cmd_dict.get ()))
    {
      PyErr_SetString (PyExc_RuntimeError,
		       _("gdb._mi_commands is not a dictionary as expected"));
      return nullptr;
    }

  return mi_cmd_dict;
}

/* Uninstall OBJ, making the MI command represented by OBJ unavailable for
   use by the user.  On success 0 is returned, otherwise -1 is returned
   and a Python exception will be set.  */

static int
micmdpy_uninstall_command (micmdpy_object *obj)
{
  PYMICMD_SCOPED_DEBUG_ENTER_EXIT;

  gdb_assert (obj->mi_command != nullptr);
  gdb_assert (obj->mi_command_name != nullptr);

  pymicmd_debug_printf ("name = %s", obj->mi_command_name);

  /* Remove the command from the internal MI table of commands, this will
     cause the c++ object to be deleted, which will clear the mi_command
     member variable within the Python object.  */
  remove_mi_cmd_entry (obj->mi_command->name ());
  gdb_assert (obj->mi_command == nullptr);

  gdbpy_ref<> mi_cmd_dict = micmdpy_global_command_dictionary ();
  if (mi_cmd_dict == nullptr)
    return -1;

  /* Grab the name for this command.  */
  gdbpy_ref<> name_obj
    = host_string_to_python_string (obj->mi_command_name);
  if (name_obj == nullptr)
    return -1;

  /* Lookup the gdb.MICommand object in the dictionary of all Python MI
     commands, this is gdb._mi_command, and remove it.  */
  PyObject *curr = PyDict_GetItemWithError (mi_cmd_dict.get (),
					    name_obj.get ());

  /* Did we encounter an error?  Failing to find the object in the
     dictionary isn't an error, that's fine.  */
  if (curr == nullptr && PyErr_Occurred ())
    return -1;

  /* Did we find this command in the gdb._mi_commands dictionary?  If so,
     then remove it.  */
  if (curr != nullptr)
    {
      /* Yes we did!  Remove it.  */
      if (PyDict_DelItem (mi_cmd_dict.get (), name_obj.get ()) < 0)
	return -1;
    }

  return 0;
}

/* Install OBJ as a usable MI command.  Return 0 on success, and -1 on
   error, in which case, a Python error will have been set.

   After successful completion the command name associated with OBJ will
   be installed in the MI command table (so it can be found if the user
   enters that command name), additionally, OBJ will have been added to
   the gdb._mi_commands dictionary (using the command name as its key),
   this will ensure that OBJ remains live even if the user gives up all
   references.  */

static int
micmdpy_install_command (micmdpy_object *obj)
{
  PYMICMD_SCOPED_DEBUG_ENTER_EXIT;

  gdb_assert (obj->mi_command == nullptr);
  gdb_assert (obj->mi_command_name != nullptr);

  pymicmd_debug_printf ("name = %s", obj->mi_command_name);

  gdbpy_ref<> mi_cmd_dict = micmdpy_global_command_dictionary ();
  if (mi_cmd_dict == nullptr)
    return -1;

  /* Look up this command name in the gdb._mi_commands dictionary, a
     command with this name may already exist.  */
  gdbpy_ref<> name_obj
    = host_string_to_python_string (obj->mi_command_name);

  PyObject *curr = PyDict_GetItemWithError (mi_cmd_dict.get (),
					    name_obj.get ());
  if (curr == nullptr && PyErr_Occurred ())
    return -1;
  if (curr != nullptr)
    {
      /* There is a command with this name already in the gdb._mi_commands
	 dictionary.  First, validate that the object in the dictionary is
	 of the expected type, just in case something weird has happened.  */
      if (!PyObject_IsInstance (curr, (PyObject *) &micmdpy_object_type))
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("unexpected object in gdb._mi_commands dictionary"));
	  return -1;
	}

      /* To get to this function OBJ should not be installed, which should
	 mean OBJ is not in the gdb._mi_commands dictionary.  If we find
	 that OBJ is the thing in the dictionary, then something weird is
	 going on, we should throw an error.  */
      micmdpy_object *other = (micmdpy_object *) curr;
      if (other == obj || other->mi_command == nullptr)
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("uninstalled command found in gdb._mi_commands dictionary"));
	  return -1;
	}

      /* All Python mi command object should always have a name set.  */
      gdb_assert (other->mi_command_name != nullptr);

      /* We always insert commands into the gdb._mi_commands dictionary
	 using their name as a key, if this check fails then the dictionary
	 is in some weird state.  */
      if (other->mi_command_name != other->mi_command->name ()
	  || strcmp (other->mi_command_name, obj->mi_command_name) != 0)
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("gdb._mi_commands dictionary is corrupted"));
	  return -1;
	}

      /* Switch the state of the c++ object held in the MI command table
	 so that it now references OBJ.  After this action the old Python
	 object that used to be referenced from the MI command table will
	 now show as uninstalled, while the new Python object will show as
	 installed.  */
      other->mi_command->swap_python_object (obj);

      gdb_assert (other->mi_command == nullptr);
      gdb_assert (obj->mi_command != nullptr);
      gdb_assert (obj->mi_command->name () == obj->mi_command_name);

      /* Remove the previous Python object from the gdb._mi_commands
	 dictionary, we'll install the new object below.  */
      if (PyDict_DelItem (mi_cmd_dict.get (), name_obj.get ()) < 0)
       return -1;
    }
  else
    {
      /* There's no Python object for this command name in the
	 gdb._mi_commands dictionary from which we can steal an existing
	 object already held in the MI commands table, and so, we now
	 create a new c++ object, and install it into the MI table.  */
      obj->mi_command = new mi_command_py (obj->mi_command_name, obj);
      mi_command_up micommand (obj->mi_command);

      /* Add the command to the gdb internal MI command table.  */
      bool result = insert_mi_cmd_entry (std::move (micommand));
      if (!result)
	{
	  PyErr_SetString (PyExc_RuntimeError,
			   _("unable to add command, name may already be in use"));
	  return -1;
	}
    }

  /* Finally, add the Python object to the gdb._mi_commands dictionary.  */
  if (PyDict_SetItem (mi_cmd_dict.get (), name_obj.get (), (PyObject *) obj) < 0)
    return -1;

  return 0;
}

/* Implement gdb.MICommand.__init__.  The init method takes the name of
   the MI command as the first argument, which must be a string, starting
   with a single dash.  */

static int
micmdpy_init (PyObject *self, PyObject *args, PyObject *kwargs)
{
  PYMICMD_SCOPED_DEBUG_ENTER_EXIT;

  micmdpy_object *cmd = (micmdpy_object *) self;

  static const char *keywords[] = { "name", nullptr };
  const char *name;

  if (!gdb_PyArg_ParseTupleAndKeywords (args, kwargs, "s", keywords,
					&name))
    return -1;

  /* Validate command name */
  const int name_len = strlen (name);
  if (name_len == 0)
    {
      PyErr_SetString (PyExc_ValueError, _("MI command name is empty."));
      return -1;
    }
  else if ((name_len < 2) || (name[0] != '-') || !isalnum (name[1]))
    {
      PyErr_SetString (PyExc_ValueError,
		       _("MI command name does not start with '-'"
			 " followed by at least one letter or digit."));
      return -1;
    }
  else
    {
      for (int i = 2; i < name_len; i++)
	{
	  if (!isalnum (name[i]) && name[i] != '-')
	    {
	      PyErr_Format
		(PyExc_ValueError,
		 _("MI command name contains invalid character: %c."),
		 name[i]);
	      return -1;
	    }
	}

      /* Skip over the leading dash.  For the rest of this function the
	 dash is not important.  */
      ++name;
    }

  /* If this object already has a name set, then this object has been
     initialized before.  We handle this case a little differently.  */
  if (cmd->mi_command_name != nullptr)
    {
      /* First, we don't allow the user to change the MI command name.
	 Supporting this would be tricky as we would need to delete the
	 mi_command_py from the MI command table, however, the user might
	 be trying to perform this reinitialization from within the very
	 command we're about to delete... it all gets very messy.

	 So, for now at least, we don't allow this.  This doesn't seem like
	 an excessive restriction.  */
      if (strcmp (cmd->mi_command_name, name) != 0)
	{
	  PyErr_SetString
	    (PyExc_ValueError,
	     _("can't reinitialize object with a different command name"));
	  return -1;
	}

      /* If there's already an object registered with the MI command table,
	 then we're done.  That object must be a mi_command_py, which
	 should reference back to this micmdpy_object.  */
      if (cmd->mi_command != nullptr)
	{
	  mi_command_py::validate_installation (cmd);
	  return 0;
	}
    }
  else
    cmd->mi_command_name = xstrdup (name);

  /* Now we can install this mi_command_py in the MI command table.  */
  return micmdpy_install_command (cmd);
}

/* Called when a gdb.MICommand object is deallocated.  */

static void
micmdpy_dealloc (PyObject *obj)
{
  PYMICMD_SCOPED_DEBUG_ENTER_EXIT;

  micmdpy_object *cmd = (micmdpy_object *) obj;

  /* If the Python object failed to initialize, then the name field might
     be nullptr.  */
  pymicmd_debug_printf ("obj = %p, name = %s", cmd,
			(cmd->mi_command_name == nullptr
			 ? "(null)" : cmd->mi_command_name));

  /* Remove the command from the MI command table if needed.  This will
     cause the mi_command_py object to be deleted, which, in turn, will
     clear the cmd->mi_command member variable, hence the assert.  */
  if (cmd->mi_command != nullptr)
    remove_mi_cmd_entry (cmd->mi_command->name ());
  gdb_assert (cmd->mi_command == nullptr);

  /* Free the memory that holds the command name.  */
  xfree (cmd->mi_command_name);
  cmd->mi_command_name = nullptr;

  /* Finally, free the memory for this Python object.  */
  Py_TYPE (obj)->tp_free (obj);
}

/* Python initialization for the MI commands components.  */

int
gdbpy_initialize_micommands ()
{
  micmdpy_object_type.tp_new = PyType_GenericNew;
  if (PyType_Ready (&micmdpy_object_type) < 0)
    return -1;

  if (gdb_pymodule_addobject (gdb_module, "MICommand",
			      (PyObject *) &micmdpy_object_type)
      < 0)
    return -1;

  invoke_cst = PyString_FromString ("invoke");
  if (invoke_cst == nullptr)
    return -1;

  return 0;
}

/* Get the gdb.MICommand.name attribute, returns a string, the name of this
   MI command.  */

static PyObject *
micmdpy_get_name (PyObject *self, void *closure)
{
  struct micmdpy_object *micmd_obj = (struct micmdpy_object *) self;

  gdb_assert (micmd_obj->mi_command_name != nullptr);
  std::string name_str = string_printf ("-%s", micmd_obj->mi_command_name);
  return PyString_FromString (name_str.c_str ());
}

/* Get the gdb.MICommand.installed property.  Returns true if this MI
   command is installed into the MI command table, otherwise returns
   false.  */

static PyObject *
micmdpy_get_installed (PyObject *self, void *closure)
{
  struct micmdpy_object *micmd_obj = (struct micmdpy_object *) self;

  if (micmd_obj->mi_command == nullptr)
    Py_RETURN_FALSE;
  Py_RETURN_TRUE;
}

/* Set the gdb.MICommand.installed property.  The property can be set to
   either true or false.  Setting the property to true will cause the
   command to be installed into the MI command table (if it isn't
   already), while setting this property to false will cause the command
   to be removed from the MI command table (if it is present).  */

static int
micmdpy_set_installed (PyObject *self, PyObject *newvalue, void *closure)
{
  struct micmdpy_object *micmd_obj = (struct micmdpy_object *) self;

  bool installed_p = PyObject_IsTrue (newvalue);
  if (installed_p == (micmd_obj->mi_command != nullptr))
    return 0;

  if (installed_p)
    return micmdpy_install_command (micmd_obj);
  else
    return micmdpy_uninstall_command (micmd_obj);
}

/* The gdb.MICommand properties.   */

static gdb_PyGetSetDef micmdpy_object_getset[] = {
  { "name", micmdpy_get_name, nullptr, "The command's name.", nullptr },
  { "installed", micmdpy_get_installed, micmdpy_set_installed,
    "Is this command installed for use.", nullptr },
  { nullptr }	/* Sentinel.  */
};

/* The gdb.MICommand descriptor.  */

PyTypeObject micmdpy_object_type = {
  PyVarObject_HEAD_INIT (nullptr, 0) "gdb.MICommand", /*tp_name */
  sizeof (micmdpy_object),			   /*tp_basicsize */
  0,						   /*tp_itemsize */
  micmdpy_dealloc,				   /*tp_dealloc */
  0,						   /*tp_print */
  0,						   /*tp_getattr */
  0,						   /*tp_setattr */
  0,						   /*tp_compare */
  0,						   /*tp_repr */
  0,						   /*tp_as_number */
  0,						   /*tp_as_sequence */
  0,						   /*tp_as_mapping */
  0,						   /*tp_hash */
  0,						   /*tp_call */
  0,						   /*tp_str */
  0,						   /*tp_getattro */
  0,						   /*tp_setattro */
  0,						   /*tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/*tp_flags */
  "GDB mi-command object",			   /* tp_doc */
  0,						   /* tp_traverse */
  0,						   /* tp_clear */
  0,						   /* tp_richcompare */
  0,						   /* tp_weaklistoffset */
  0,						   /* tp_iter */
  0,						   /* tp_iternext */
  0,						   /* tp_methods */
  0,						   /* tp_members */
  micmdpy_object_getset,			   /* tp_getset */
  0,						   /* tp_base */
  0,						   /* tp_dict */
  0,						   /* tp_descr_get */
  0,						   /* tp_descr_set */
  0,						   /* tp_dictoffset */
  micmdpy_init,					   /* tp_init */
  0,						   /* tp_alloc */
};

void _initialize_py_micmd ();
void
_initialize_py_micmd ()
{
  add_setshow_boolean_cmd
    ("py-micmd", class_maintenance, &pymicmd_debug,
     _("Set Python micmd debugging."),
     _("Show Python micmd debugging."),
     _("When on, Python micmd debugging is enabled."),
     nullptr,
     show_pymicmd_debug,
     &setdebuglist, &showdebuglist);
}
