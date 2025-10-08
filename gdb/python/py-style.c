/* Python interface to ui_file_style objects.

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
#include "ui-style.h"
#include "py-color.h"
#include "cli/cli-decode.h"
#include "cli/cli-style.h"
#include "top.h"

/* Intensity constants and their values.  */
static struct {
  const char *name;
  ui_file_style::intensity value;
} intensity_constants[] =
{
  { "INTENSITY_NORMAL", ui_file_style::NORMAL },
  { "INTENSITY_DIM", ui_file_style::DIM },
  { "INTENSITY_BOLD", ui_file_style::BOLD }
};

/* A style.  */
struct style_object
{
  PyObject_HEAD

  /* Underlying style, only valid when STYLE_NAME is NULL.  */
  ui_file_style style;

  /* The name of the style.  Can be NULL, in which case STYLE holds the
     style value.  */
  char *style_name;
};

extern PyTypeObject style_object_type;

/* Initialize the 'style' module.  */

static int
gdbpy_initialize_style ()
{
  for (auto &pair : intensity_constants)
    if (PyModule_AddIntConstant (gdb_module, pair.name,
				 static_cast<long> (pair.value)) < 0)
      return -1;

  return gdbpy_type_ready (&style_object_type, gdb_module);
}

/* Free any resources help by SELF, and reset points to NULL.  */

static void
stylepy_free_resources (PyObject *self)
{
  style_object *style = (style_object *) self;

  xfree (style->style_name);
  style->style_name = nullptr;
}

/* gdb.Style deallocation method.  */

static void
stylepy_dealloc (PyObject *self)
{
  stylepy_free_resources (self);
  Py_TYPE (self)->tp_free (self);
}

/* Find style NAME and return it.  If NAME cannot be found then an empty
   optional is returned, and a Python error will be set.

   If HAS_INTENSITY_PTR is not NULL, then, if NAME is found,
   *HAS_INTENSITY_PTR will be set true if NAME has an "intensity"
   sub-command, and set false otherwise.  If NAME is not found then
   *HAS_INTENSITY_PTR is left unchanged.

   If FOUND_CMD_PTR is not NULL, then, if NAME is found, *FOUND_CMD_PTR is
   set to point to the prefix command matching NAME.  If NAME is a
   multi-word style name (e.g. 'disassembler comment') then *FOUND_CMD_PTR
   will point to the prefix command for the last word (e.g. 'comment').  */

static std::optional<ui_file_style>
stylepy_style_from_name (const char *name, bool *has_intensity_ptr = nullptr,
			 const cmd_list_element **found_cmd_ptr = nullptr)
{
  std::string cmd_str = std::string ("show style ") + name;

  struct cmd_list_element *cmd = nullptr;
  struct cmd_list_element *alias = nullptr;
  int found;
  try
    {
      struct cmd_list_element *prefix;
      found = lookup_cmd_composition (cmd_str.c_str (), &alias, &prefix, &cmd);
    }
  catch (const gdb_exception &ex)
    {
      PyErr_Format (PyExc_RuntimeError,
		    _("style '%s' cannot be found."), name);
      return {};
    }

  gdb_assert (!found || cmd != nullptr);

  if (!found || cmd == CMD_LIST_AMBIGUOUS || !cmd->is_prefix ())
    {
      PyErr_Format (PyExc_RuntimeError,
		    _("style '%s' cannot be found."), name);
      return {};
    }

  ui_file_style style;
  bool has_fg = false;
  bool has_bg = false;
  bool has_intensity = false;
  for (cmd_list_element *sub = *cmd->subcommands;
       sub != nullptr;
       sub = sub->next)
    {
      if (!sub->var.has_value ())
	continue;

      if (strcmp (sub->name, "foreground") == 0)
	{
	  const ui_file_style::color &color
	    = sub->var->get<ui_file_style::color> ();
	  style.set_fg (color);
	  has_fg = true;
	}
      else if (strcmp (sub->name, "background") == 0)
	{
	  const ui_file_style::color &color
	    = sub->var->get<ui_file_style::color> ();
	  style.set_bg (color);
	  has_bg = true;
	}
      else if (strcmp (sub->name, "intensity") == 0
	       && sub->var->type () == var_enum)
	{
	  const char *intensity_str = sub->var->get<const char *> ();
	  ui_file_style::intensity intensity = ui_file_style::NORMAL;
	  if (strcmp (intensity_str, "bold") == 0)
	    intensity = ui_file_style::BOLD;
	  else if (strcmp (intensity_str, "dim") == 0)
	    intensity = ui_file_style::DIM;
	  style.set_intensity (intensity);
	  has_intensity = true;
	}
    }

  /* All styles should have a foreground and background, but the intensity
     is optional.  */
  if (!has_fg || !has_bg)
    {
      PyErr_Format (PyExc_RuntimeError,
		    _("style '%s' missing '%s' component."), name,
		    (has_fg ? "background" : "foreground"));
      return {};
    }

  if (has_intensity_ptr != nullptr)
    *has_intensity_ptr = has_intensity;

  /* If NAME identified an alias then use that instead of CMD, this means
     the style's cached name will better match the string the user used to
     initialise the style.  */
  if (found_cmd_ptr != nullptr)
    *found_cmd_ptr = alias != nullptr ? alias : cmd;

  return style;
}

/* Initialise a gdb.Style object from a named style.  We only store the
   style name within SELF, each time the style is used, we look up its
   current value, this allows the style to change if the user adjusts the
   settings.  */

static int
stylepy_init_from_style_name (PyObject *self, const char *style_name)
{
  style_object *style = (style_object *) self;
  gdb_assert (style->style_name == nullptr);

  gdb_assert (style_name != nullptr);

  const cmd_list_element *cmd = nullptr;
  std::optional<ui_file_style> maybe_style
    = stylepy_style_from_name (style_name, nullptr, &cmd);
  if (!maybe_style.has_value ())
    return -1;

  /* If we found a style then we must have found a prefix command.  */
  gdb_assert (cmd != nullptr);
  gdb_assert (cmd->is_prefix ());

  /* Get the components of this command.  */
  std::vector<std::string> components = cmd->command_components ();
  gdb_assert (components.size () > 2);
  gdb_assert (components[0] == "show");
  gdb_assert (components[1] == "style");

  /* And build the components into a string, but without the 'show style'
     part at the start.  */
  std::string expanded_style_name (components[2]);
  for (int i = 3; i < components.size (); ++i)
    expanded_style_name += " " + components[i];

  style->style_name = xstrdup (expanded_style_name.c_str ());

  return 0;
}

/* Convert INTENSITY_VALUE to a valid intensity enum value, if possible,
   and return the enum value.  If INTENSITY_VALUE is not a valid intensity
   value then set a Python error, and return an empty optional.  */

static std::optional<ui_file_style::intensity>
stylepy_long_to_intensity (long intensity_value)
{
  switch (intensity_value)
    {
    case ui_file_style::NORMAL:
    case ui_file_style::DIM:
    case ui_file_style::BOLD:
      return static_cast<ui_file_style::intensity> (intensity_value);

    default:
      PyErr_Format
	(PyExc_ValueError, _("invalid 'intensity' value %d."),
	 intensity_value);
      return {};
    }
}

/* Initialise a gdb.Style object from a foreground and background
   gdb.Color object, and an intensity.  */

static int
stylepy_init_from_parts (PyObject *self, PyObject *fg, PyObject *bg,
			 int intensity_value)
{
  style_object *style = (style_object *) self;
  gdb_assert (style->style_name == nullptr);

  if (fg == Py_None)
    fg = nullptr;

  if (bg == Py_None)
    bg = nullptr;

  if (fg != nullptr && !gdbpy_is_color (fg))
    {
      PyErr_Format
	(PyExc_TypeError,
	 _("'foreground' argument must be gdb.Color or None, not %s."),
	 Py_TYPE (fg)->tp_name);
      return -1;
    }

  if (bg != nullptr && !gdbpy_is_color (bg))
    {
      PyErr_Format
	(PyExc_TypeError,
	 _("'background' argument must be gdb.Color or None, not %s."),
	 Py_TYPE (bg)->tp_name);
      return -1;
    }

  if (fg != nullptr)
    style->style.set_fg (gdbpy_get_color (fg));
  else
    style->style.set_fg (ui_file_style::color (ui_file_style::NONE));

  if (bg != nullptr)
    style->style.set_bg (gdbpy_get_color (bg));
  else
    style->style.set_bg (ui_file_style::color (ui_file_style::NONE));

  /* Convert INTENSITY_VALUE into the enum.  */
  std::optional<ui_file_style::intensity> intensity
    = stylepy_long_to_intensity (intensity_value);
  if (!intensity.has_value ())
    return -1;
  style->style.set_intensity (intensity.value ());

  return 0;
}

/* gdb.Style object initializer.  Either:
   gdb.Style.__init__("style name")
   gdb.Style.__init__(fg, bg, intensity)

   This init function supports two possible sets of arguments.  Both
   options are different enough that we can distinguish the two by the
   argument types.  Dispatch to one of the two stylepy_init_from_*
   functions above.  */

static int
stylepy_init (PyObject *self, PyObject *args, PyObject *kwargs)
{
  /* If this object was previously initialised, then clear it out now.  */
  stylepy_free_resources (self);

  /* Try to parse the incoming arguments as a string, this is a style
     name.  */
  const char *style_name = nullptr;
  static const char *keywords_style[] = { "style", nullptr };
  if (gdb_PyArg_ParseTupleAndKeywords (args, kwargs, "s", keywords_style,
				       &style_name))
    return stylepy_init_from_style_name (self, style_name);

  /* That didn't work, discard any errors.  */
  PyErr_Clear ();

  /* Try to parse the incoming arguments as a list of parts, this is an
     unnamed style.  */
  PyObject *foreground_color = nullptr;
  PyObject *background_color = nullptr;
  int intensity_value = static_cast<int> (ui_file_style::NORMAL);
  static const char *keywords_parts[]
    = { "foreground", "background", "intensity", nullptr };
  if (gdb_PyArg_ParseTupleAndKeywords (args, kwargs, "|OOi", keywords_parts,
				       &foreground_color,
				       &background_color,
				       &intensity_value))
    return stylepy_init_from_parts (self, foreground_color,
				    background_color, intensity_value);

  /* Return the error gdb_PyArg_ParseTupleAndKeywords set.  */
  return -1;
}



/* See python-internal.h.   */

bool
gdbpy_is_style (PyObject *obj)
{
  gdb_assert (obj != nullptr);
  return PyObject_TypeCheck (obj, &style_object_type);
}

/* Return the ui_file_style for STYLEPY.  If the style cannot be found,
   then return an empty optional, and set a Python error.  */

static std::optional<ui_file_style>
stylepy_to_style (style_object *stylepy)
{
  std::optional<ui_file_style> style;

  if (stylepy->style_name != nullptr)
    style = stylepy_style_from_name (stylepy->style_name);
  else
    style.emplace (stylepy->style);

  return style;
}

/* See python-internal.h.   */

std::optional<ui_file_style>
gdbpy_style_object_to_ui_file_style (PyObject *obj)
{
  gdb_assert (obj != nullptr);
  gdb_assert (gdbpy_is_style (obj));

  style_object *style_obj = (style_object *) obj;
  return stylepy_to_style (style_obj);
}

/* Implementation of gdb.Style.escape_sequence().  Return the escape
   sequence to apply Style.  If styling is turned off, then this returns
   the empty string.  Can raise an exception if a named style can no longer
   be read.  */

static PyObject *
stylepy_escape_sequence (PyObject *self, PyObject *args)
{
  style_object *style_obj = (style_object *) self;

  std::optional<ui_file_style> style = stylepy_to_style (style_obj);
  if (!style.has_value ())
    return nullptr;

  std::string style_str;
  if (term_cli_styling ())
    style_str = style->to_ansi ();

  return host_string_to_python_string (style_str.c_str ()).release ();
}

/* Implement gdb.Style.apply(STR).  Return a new string which is STR with
   escape sequences added so that STR is formatted in this style.  A
   trailing escape sequence is added to restore the default style.

   If styling is currently disabled ('set style enabled off'), then no
   escape sequences are added, but all the checks for the validity of the
   current style are still performed, and a new string, a copy of the
   input string is returned.

   Can raise a Python exception and return NULL if the argument types are
   wrong, or if a named style can no longer be read.  */

static PyObject *
stylepy_apply (PyObject *self, PyObject *args, PyObject *kw)
{
  style_object *style_obj = (style_object *) self;

  static const char *keywords[] = { "string", nullptr };
  PyObject *input_obj;

  /* Grab the incoming string as a Python object.  In the case where
     styling is not being applied we can just return this object with the
     reference count incremented.  */
  if (!gdb_PyArg_ParseTupleAndKeywords (args, kw, "O!", keywords,
					&PyUnicode_Type, &input_obj))
    return nullptr;

  std::optional<ui_file_style> style = stylepy_to_style (style_obj);
  if (!style.has_value ())
    return nullptr;

  if (gdb_stdout->can_emit_style_escape ())
    {
      gdb_assert (gdbpy_is_string (input_obj));
      gdb::unique_xmalloc_ptr<char>
	input (python_string_to_host_string (input_obj));

      std::string output
	= style->to_ansi () + input.get () + ui_file_style ().to_ansi ();

      return host_string_to_python_string (output.c_str ()).release ();
    }
  else
    {
      /* Return an unmodified "copy" of INPUT_OBJ by just incrementing the
	 reference count.  */
      Py_INCREF (input_obj);
      return input_obj;
    }
}



/* Implement reading the gdb.Style.foreground attribute.  */

static PyObject *
stylepy_get_foreground (PyObject *self, void *closure)
{
  style_object *style_obj = (style_object *) self;

  std::optional<ui_file_style> style = stylepy_to_style (style_obj);
  if (!style.has_value ())
    return nullptr;

  gdbpy_ref<> color = create_color_object (style->get_foreground ());
  if (color == nullptr)
    return nullptr;

  return color.release ();
}

/* Implement writing the gdb.Style.foreground attribute.  */

static int
stylepy_set_foreground (PyObject *self, PyObject *newvalue, void *closure)
{
  if (!gdbpy_is_color (newvalue))
    {
      PyErr_Format (PyExc_TypeError, _("value must be gdb.Color, not %s"),
		    Py_TYPE (newvalue)->tp_name);
      return -1;
    }

  style_object *style_obj = (style_object *) self;

  /* Handle unnamed styles.  This is easy, just update the embedded style
     object.  */
  if (style_obj->style_name == nullptr)
    {
      style_obj->style.set_fg (gdbpy_get_color (newvalue));
      return 0;
    }

  /* Handle named styles.  This is harder, we need to write to the actual
     parameter.  */
  std::string cmd_string
    = string_printf ("set style %s foreground %s",
		     style_obj->style_name,
		     gdbpy_get_color (newvalue).to_string ().c_str ());
  try
    {
      execute_command (cmd_string.c_str (), 0);
    }
  catch (const gdb_exception &except)
    {
      return gdbpy_handle_gdb_exception (-1, except);
    }

  return 0;
}

/* Implement reading the gdb.Style.background attribute.  */

static PyObject *
stylepy_get_background (PyObject *self, void *closure)
{
  style_object *style_obj = (style_object *) self;

  std::optional<ui_file_style> style = stylepy_to_style (style_obj);
  if (!style.has_value ())
    return nullptr;

  gdbpy_ref<> color = create_color_object (style->get_background ());
  if (color == nullptr)
    return nullptr;

  return color.release ();
}

/* Implement writing the gdb.Style.background attribute.  */

static int
stylepy_set_background (PyObject *self, PyObject *newvalue, void *closure)
{
  if (!gdbpy_is_color (newvalue))
    {
      PyErr_Format (PyExc_TypeError, _("value must be gdb.Color, not %s"),
		    Py_TYPE (newvalue)->tp_name);
      return -1;
    }

  style_object *style_obj = (style_object *) self;

  /* Handle unnamed styles.  This is easy, just update the embedded style
     object.  */
  if (style_obj->style_name == nullptr)
    {
      style_obj->style.set_bg (gdbpy_get_color (newvalue));
      return 0;
    }

  /* Handle named styles.  This is harder, we need to write to the actual
     parameter.  */
  std::string cmd_string
    = string_printf ("set style %s background %s",
		     style_obj->style_name,
		     gdbpy_get_color (newvalue).to_string ().c_str ());
  try
    {
      execute_command (cmd_string.c_str (), 0);
    }
  catch (const gdb_exception &except)
    {
      return gdbpy_handle_gdb_exception (-1, except);
    }

  return 0;
}

/* Implement reading the gdb.Style.intensity attribute.  */

static PyObject *
stylepy_get_intensity (PyObject *self, void *closure)
{
  style_object *style_obj = (style_object *) self;

  std::optional<ui_file_style> style = stylepy_to_style (style_obj);
  if (!style.has_value ())
    return nullptr;

  ui_file_style::intensity intensity = style->get_intensity ();
  return PyLong_FromLong (static_cast<long> (intensity));
}

/* Return a string representing INTENSITY.  */

static const char *
stylepy_intensity_to_string (ui_file_style::intensity intensity)
{
  const char *intensity_str = nullptr;
  switch (intensity)
    {
    case ui_file_style::NORMAL:
      intensity_str = "normal";
      break;
    case ui_file_style::BOLD:
      intensity_str = "bold";
      break;
    case ui_file_style::DIM:
      intensity_str = "dim";
      break;
    }

  gdb_assert (intensity_str != nullptr);
  return intensity_str;
}

/* Implement writing the gdb.Style.intensity attribute.  */

static int
stylepy_set_intensity (PyObject *self, PyObject *newvalue, void *closure)
{
  style_object *style_obj = (style_object *) self;

  if (!PyLong_Check (newvalue))
    {
      PyErr_Format
	(PyExc_TypeError,
	 _("value must be a Long (a gdb.INTENSITY constant), not %s"),
	 Py_TYPE (newvalue)->tp_name);
      return -1;
    }

  /* Convert the Python object to a value we can use.  */
  long intensity_value;
  if (!gdb_py_int_as_long (newvalue, &intensity_value))
    return -1;

  std::optional<ui_file_style::intensity> intensity
    = stylepy_long_to_intensity (intensity_value);
  if (!intensity.has_value ())
    return -1;

  /* Handle unnamed styles.  This is easy, just update the embedded style
     object.  */
  if (style_obj->style_name == nullptr)
    {
      style_obj->style.set_intensity (intensity.value ());
      return 0;
    }

  /* Handle named styles.  This is harder, we need to write to the actual
     parameter.  First though, look up the named style to see if it has an
     intensity.  HAS_INTENSITY will be set true only if the style exists,
     and has an 'intensity' setting.  */
  bool has_intensity = false;
  std::optional<ui_file_style> style
    = stylepy_style_from_name (style_obj->style_name, &has_intensity);
  if (!style.has_value ())
    return -1;
  if (!has_intensity)
    {
      PyErr_Format
	(PyExc_ValueError, "the intensity of style '%s' is not writable.",
	 style_obj->style_name);
      return -1;
    }

  const char *intensity_str = stylepy_intensity_to_string (intensity.value ());
  gdb_assert (intensity_str != nullptr);

  std::string cmd_string
    = string_printf ("set style %s intensity %s",
		     style_obj->style_name, intensity_str);
  try
    {
      execute_command (cmd_string.c_str (), 0);
    }
  catch (const gdb_exception &except)
    {
      return gdbpy_handle_gdb_exception (-1, except);
    }

  return 0;
}



/* Call FETCH_ATTR, passing in SELF, to get a PyObject*, then convert it to
   a Python string, and finally into a C++ managed string.  Will return
   nullptr and set a Python error if something goes wrong.

   The FETCH_ATTR function will be a function that returns an attribute
   from SELF.  */

static gdb::unique_xmalloc_ptr<char>
stylepy_attribute_to_string
  (PyObject *self,
   gdb::function_view<PyObject * (PyObject *, void *)> fetch_attr)
{
  PyObject *attr_obj = fetch_attr (self, nullptr);
  if (attr_obj == nullptr)
    return nullptr;

  PyObject *str_obj = PyObject_Str (attr_obj);
  if (str_obj == nullptr)
    return nullptr;

  return python_string_to_host_string (str_obj);
}

/* __repr__ implementation for gdb.Style.  */

static PyObject *
stylepy_repr (PyObject *self)
{
  style_object *style_obj = (style_object *) self;

  gdb::unique_xmalloc_ptr<char> fg_str
    (stylepy_attribute_to_string (self, stylepy_get_foreground));
  gdb::unique_xmalloc_ptr<char> bg_str
    (stylepy_attribute_to_string (self, stylepy_get_background));

  PyObject *intensity_obj = stylepy_get_intensity (self, nullptr);
  if (intensity_obj == nullptr)
    return nullptr;
  gdb_assert (PyLong_Check (intensity_obj));
  long intensity_value;
  if (!gdb_py_int_as_long (intensity_obj, &intensity_value))
    return nullptr;
  std::optional<ui_file_style::intensity> intensity
    = stylepy_long_to_intensity (intensity_value);
  if (!intensity.has_value ())
    return nullptr;
  const char *intensity_str = stylepy_intensity_to_string (intensity.value ());
  gdb_assert (intensity_str != nullptr);

  if (style_obj->style_name == nullptr)
    return PyUnicode_FromFormat ("<%s fg=%s, bg=%s, intensity=%s>",
				 Py_TYPE (self)->tp_name,
				 fg_str.get (), bg_str.get (),
				 intensity_str);
  else
    return PyUnicode_FromFormat ("<%s name='%s', fg=%s, bg=%s, intensity=%s>",
				 Py_TYPE (self)->tp_name,
				 style_obj->style_name, fg_str.get (),
				 bg_str.get (), intensity_str);
}



/* Style methods.  */

static PyMethodDef stylepy_methods[] =
{
  { "escape_sequence", stylepy_escape_sequence, METH_NOARGS,
    "escape_sequence () -> str.\n\
Return the ANSI escape sequence for this style."},
  { "apply", (PyCFunction) stylepy_apply, METH_VARARGS | METH_KEYWORDS,
    "apply(String) -> String.\n\
Apply this style to the input string.  Return an updated string."},
  {nullptr}
};

/* Attribute get/set Python definitions. */

static gdb_PyGetSetDef style_object_getset[] = {
  { "foreground", stylepy_get_foreground, stylepy_set_foreground,
    "The gdb.Color for the foreground of this style.", NULL },
  { "background", stylepy_get_background, stylepy_set_background,
    "The gdb.Color for the background of this style.", NULL },
  { "intensity", stylepy_get_intensity, stylepy_set_intensity,
    "The Str for the intensity of this style.", NULL },
  { nullptr }  /* Sentinel.  */
};

PyTypeObject style_object_type =
{
  PyVarObject_HEAD_INIT (nullptr, 0)
  "gdb.Style",			  /*tp_name*/
  sizeof (style_object),	  /*tp_basicsize*/
  0,				  /*tp_itemsize*/
  stylepy_dealloc,		  /*tp_dealloc*/
  0,				  /*tp_print*/
  0,				  /*tp_getattr*/
  0,				  /*tp_setattr*/
  0,				  /*tp_compare*/
  stylepy_repr,			  /*tp_repr*/
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
  "GDB style object",	  	  /* tp_doc */
  0,				  /* tp_traverse */
  0,				  /* tp_clear */
  0,				  /* tp_richcompare */
  0,				  /* tp_weaklistoffset */
  0,				  /* tp_iter */
  0,				  /* tp_iternext */
  stylepy_methods,		  /* tp_methods */
  0,				  /* tp_members */
  style_object_getset,		  /* tp_getset */
  0,				  /* tp_base */
  0,				  /* tp_dict */
  0,				  /* tp_descr_get */
  0,				  /* tp_descr_set */
  0,				  /* tp_dictoffset */
  stylepy_init,			  /* tp_init */
  0,				  /* tp_alloc */
  PyType_GenericNew		  /* tp_new */
};

GDBPY_INITIALIZE_FILE (gdbpy_initialize_style);
