/* Python reference-holding class

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef GDB_PYTHON_REF_H
#define GDB_PYTHON_REF_H

/* An instance of this class either holds a reference to a PyObject,
   or is "NULL".  If it holds a reference, then when the object is
   destroyed, the PyObject is decref'd.

   Normally an instance is constructed using a PyObject*.  This sort
   of initialization lets this class manage the lifetime of that
   reference.

   Assignment and copy construction will make a new reference as
   appropriate.  Assignment from a plain PyObject* is disallowed to
   avoid confusion about whether this acquires a new reference;
   instead use the "reset" method -- which, like the PyObject*
   constructor, transfers ownership.
*/
class gdbpy_ref
{
 public:

  /* Create a new NULL instance.  */
  gdbpy_ref ()
    : m_obj (NULL)
  {
  }

  /* Create a new instance.  OBJ is a reference, management of which
     is now transferred to this class.  */
  explicit gdbpy_ref (PyObject *obj)
    : m_obj (obj)
  {
  }

  /* Copy another instance.  */
  gdbpy_ref (const gdbpy_ref &other)
    : m_obj (other.m_obj)
  {
    Py_XINCREF (m_obj);
  }

  /* Transfer ownership from OTHER.  */
  gdbpy_ref (gdbpy_ref &&other)
    : m_obj (other.m_obj)
  {
    other.m_obj = NULL;
  }

  /* Destroy this instance.  */
  ~gdbpy_ref ()
  {
    Py_XDECREF (m_obj);
  }

  /* Copy another instance.  */
  gdbpy_ref &operator= (const gdbpy_ref &other)
  {
    /* Do nothing on self-assignment.  */
    if (this != &other)
      {
	reset (other.m_obj);
	Py_XINCREF (m_obj);
      }
    return *this;
  }

  /* Transfer ownership from OTHER.  */
  gdbpy_ref &operator= (gdbpy_ref &&other)
  {
    /* Do nothing on self-assignment.  */
    if (this != &other)
      {
	reset (other.m_obj);
	other.m_obj = NULL;
      }
    return *this;
  }

  /* Change this instance's referent.  OBJ is a reference, management
     of which is now transferred to this class.  */
  void reset (PyObject *obj)
  {
    Py_XDECREF (m_obj);
    m_obj = obj;
  }

  /* Return this instance's referent.  In Python terms this is a
     borrowed pointer.  */
  PyObject *get () const
  {
    return m_obj;
  }

  /* Return this instance's referent, and stop managing this
     reference.  The caller is now responsible for the ownership of
     the reference.  */
  PyObject *release ()
  {
    PyObject *result = m_obj;

    m_obj = NULL;
    return result;
  }

 private:

  PyObject *m_obj;
};

inline bool operator== (const gdbpy_ref &self, const gdbpy_ref &other)
{
  return self.get () == other.get ();
}

inline bool operator== (const gdbpy_ref &self, const PyObject *other)
{
  return self.get () == other;
}

inline bool operator== (const PyObject *self, const gdbpy_ref &other)
{
  return self == other.get ();
}

inline bool operator!= (const gdbpy_ref &self, const gdbpy_ref &other)
{
  return self.get () != other.get ();
}

inline bool operator!= (const gdbpy_ref &self, const PyObject *other)
{
  return self.get () != other;
}

inline bool operator!= (const PyObject *self, const gdbpy_ref &other)
{
  return self != other.get ();
}

#endif /* GDB_PYTHON_REF_H */
