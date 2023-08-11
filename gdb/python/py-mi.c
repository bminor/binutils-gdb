/* Python interface to MI commands

   Copyright (C) 2023 Free Software Foundation, Inc.

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
#include "ui-out.h"
#include "mi/mi-parse.h"

/* A ui_out subclass that creates a Python object based on the data
   that is passed in.  */

class py_ui_out : public ui_out
{
public:

  py_ui_out ()
    : ui_out (fix_multi_location_breakpoint_output
	      | fix_breakpoint_script_output)
  {
    do_begin (ui_out_type_tuple, nullptr);
  }

  bool can_emit_style_escape () const override
  { return false; }

  bool do_is_mi_like_p () const override
  { return true; }

  /* Return the Python object that was created.  If a Python error
     occurred during the processing, set the Python error and return
     nullptr.  */
  PyObject *result ()
  {
    if (m_error.has_value ())
      {
	m_error->restore ();
	return nullptr;
      }
    return current ().obj.release ();
  }

protected:

  void do_progress_end () override { }
  void do_progress_start () override { }
  void do_progress_notify (const std::string &, const char *, double, double)
    override
  { }

  void do_table_begin (int nbrofcols, int nr_rows, const char *tblid) override
  {
    do_begin (ui_out_type_list, tblid);
  }
  void do_table_body () override
  { }
  void do_table_end () override
  {
    do_end (ui_out_type_list);
  }
  void do_table_header (int width, ui_align align,
			const std::string &col_name,
			const std::string &col_hdr) override
  { }

  void do_begin (ui_out_type type, const char *id) override;
  void do_end (ui_out_type type) override;

  void do_field_signed (int fldno, int width, ui_align align,
			const char *fldname, LONGEST value) override;
  void do_field_unsigned (int fldno, int width, ui_align align,
			  const char *fldname, ULONGEST value) override;

  void do_field_skip (int fldno, int width, ui_align align,
		      const char *fldname) override
  { }

  void do_field_string (int fldno, int width, ui_align align,
			const char *fldname, const char *string,
			const ui_file_style &style) override;
  void do_field_fmt (int fldno, int width, ui_align align,
		     const char *fldname, const ui_file_style &style,
		     const char *format, va_list args) override
    ATTRIBUTE_PRINTF (7, 0);

  void do_spaces (int numspaces) override
  { }

  void do_text (const char *string) override
  { }

  void do_message (const ui_file_style &style,
		   const char *format, va_list args)
    override ATTRIBUTE_PRINTF (3,0)
  { }

  void do_wrap_hint (int indent) override
  { }

  void do_flush () override
  { }

  void do_redirect (struct ui_file *outstream) override
  { }

private:

  /* When constructing Python objects, this class keeps a stack of
     objects being constructed.  Each such object has this type.  */
  struct object_desc
  {
    /* Name of the field (or empty for lists) that this object will
       eventually become.  */
    std::string field_name;
    /* The object under construction.  */
    gdbpy_ref<> obj;
    /* The type of structure being created.  Note that tables are
       treated as lists here.  */
    ui_out_type type;
  };

  /* The stack of objects being created.  */
  std::vector<object_desc> m_objects;

  /* If an error occurred, this holds the exception information for
     use by the 'release' method.  */
  gdb::optional<gdbpy_err_fetch> m_error;

  /* Return a reference to the object under construction.  */
  object_desc &current ()
  { return m_objects.back (); }

  /* Add a new field to the current object under construction.  */
  void add_field (const char *name, const gdbpy_ref<> &obj);
};

void
py_ui_out::add_field (const char *name, const gdbpy_ref<> &obj)
{
  if (obj == nullptr)
    {
      m_error.emplace ();
      return;
    }

  object_desc &desc = current ();
  if (desc.type == ui_out_type_list)
    {
      if (PyList_Append (desc.obj.get (), obj.get ()) < 0)
	m_error.emplace ();
    }
  else
    {
      if (PyDict_SetItemString (desc.obj.get (), name, obj.get ()) < 0)
	m_error.emplace ();
    }
}

void
py_ui_out::do_begin (ui_out_type type, const char *id)
{
  if (m_error.has_value ())
    return;

  gdbpy_ref<> new_obj (type == ui_out_type_list
		       ? PyList_New (0)
		       : PyDict_New ());
  if (new_obj == nullptr)
    {
      m_error.emplace ();
      return;
    }

  object_desc new_desc;
  if (id != nullptr)
    new_desc.field_name = id;
  new_desc.obj = std::move (new_obj);
  new_desc.type = type;

  m_objects.push_back (std::move (new_desc));
}

void
py_ui_out::do_end (ui_out_type type)
{
  if (m_error.has_value ())
    return;

  object_desc new_obj = std::move (current ());
  m_objects.pop_back ();
  add_field (new_obj.field_name.c_str (), new_obj.obj);
}

void
py_ui_out::do_field_signed (int fldno, int width, ui_align align,
			    const char *fldname, LONGEST value)
{
  if (m_error.has_value ())
    return;

  gdbpy_ref<> val = gdb_py_object_from_longest (value);
  add_field (fldname, val);
}

void
py_ui_out::do_field_unsigned (int fldno, int width, ui_align align,
			    const char *fldname, ULONGEST value)
{
  if (m_error.has_value ())
    return;

  gdbpy_ref<> val = gdb_py_object_from_ulongest (value);
  add_field (fldname, val);
}

void
py_ui_out::do_field_string (int fldno, int width, ui_align align,
			    const char *fldname, const char *string,
			    const ui_file_style &style)
{
  if (m_error.has_value ())
    return;

  gdbpy_ref<> val = host_string_to_python_string (string);
  add_field (fldname, val);
}

void
py_ui_out::do_field_fmt (int fldno, int width, ui_align align,
			 const char *fldname, const ui_file_style &style,
			 const char *format, va_list args)
{
  if (m_error.has_value ())
    return;

  std::string str = string_vprintf (format, args);
  do_field_string (fldno, width, align, fldname, str.c_str (), style);
}

/* Implementation of the gdb.execute_mi command.  */

PyObject *
gdbpy_execute_mi_command (PyObject *self, PyObject *args, PyObject *kw)
{
  gdb::unique_xmalloc_ptr<char> mi_command;
  std::vector<gdb::unique_xmalloc_ptr<char>> arg_strings;

  Py_ssize_t n_args = PyTuple_Size (args);
  if (n_args < 0)
    return nullptr;

  for (Py_ssize_t i = 0; i < n_args; ++i)
    {
      /* Note this returns a borrowed reference.  */
      PyObject *arg = PyTuple_GetItem (args, i);
      if (arg == nullptr)
	return nullptr;
      gdb::unique_xmalloc_ptr<char> str = python_string_to_host_string (arg);
      if (str == nullptr)
	return nullptr;
      if (i == 0)
	mi_command = std::move (str);
      else
	arg_strings.push_back (std::move (str));
    }

  py_ui_out uiout;

  try
    {
      scoped_restore save_uiout = make_scoped_restore (&current_uiout, &uiout);
      auto parser = gdb::make_unique<mi_parse> (std::move (mi_command),
						std::move (arg_strings));
      mi_execute_command (parser.get ());
    }
  catch (const gdb_exception &except)
    {
      gdbpy_convert_exception (except);
      return nullptr;
    }

  return uiout.result ();
}
