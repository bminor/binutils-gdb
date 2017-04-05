/* Python interface to btrace instruction history.

   Copyright 2016-2017 Free Software Foundation, Inc.

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
#include "gdbcore.h"
#include "gdbcmd.h"
#include "gdbthread.h"
#include "btrace.h"
#include "py-record-btrace.h"
#include "disasm.h"

#if defined (IS_PY3K)

#define BTPY_PYSLICE(x) (x)

#else

#define BTPY_PYSLICE(x) ((PySliceObject *) x)

#endif

#define BTPY_REQUIRE_VALID_INSN(obj, iter)				\
    do {								\
      struct thread_info *tinfo = find_thread_ptid (obj->ptid);		\
      if (tinfo == NULL || btrace_is_empty (tinfo))			\
	return PyErr_Format (gdbpy_gdb_error, _("Empty branch trace."));\
      if (0 == btrace_find_insn_by_number (&iter, &tinfo->btrace,	\
					   obj->number))		\
	return PyErr_Format (gdbpy_gdb_error, _("No such instruction."));\
    } while (0)

#define BTPY_REQUIRE_VALID_CALL(obj, iter)				\
    do {								\
      struct thread_info *tinfo = find_thread_ptid (obj->ptid);		\
      if (tinfo == NULL || btrace_is_empty (tinfo))			\
	return PyErr_Format (gdbpy_gdb_error, _("Empty branch trace."));\
      if (0 == btrace_find_call_by_number (&iter, &tinfo->btrace,	\
					   obj->number))		\
	return PyErr_Format (gdbpy_gdb_error, _("No such call segment."));\
    } while (0)

/* This can either be a btrace instruction or a function call segment,
   depending on the chosen type.  */

typedef struct {
  PyObject_HEAD

  /* The thread this object belongs to.  */
  ptid_t ptid;

  /* Instruction number or function call segment number, depending on the type
     of this object.  */
  Py_ssize_t number;
} btpy_object;

/* Python object for btrace record lists.  */

typedef struct {
  PyObject_HEAD

  /* The thread this list belongs to.  */
  ptid_t ptid;

  /* The first index being part of this list.  */
  Py_ssize_t first;

  /* The last index begin part of this list.  */
  Py_ssize_t last;

  /* Stride size.  */
  Py_ssize_t step;

  /* Either &BTPY_CALL_TYPE or &BTPY_INSN_TYPE.  */
  PyTypeObject* element_type;
} btpy_list_object;

/* Python type for btrace instructions.  */

static PyTypeObject btpy_insn_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Python type for btrace function-calls.  */

static PyTypeObject btpy_call_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Python type for btrace lists.  */

static PyTypeObject btpy_list_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Create a new gdb.BtraceInstruction or gdb.BtraceFunctionCall object,
   depending on TYPE.  */

static PyObject *
btpy_new (ptid_t ptid, Py_ssize_t number, PyTypeObject* type)
{
  btpy_object * const obj = PyObject_New (btpy_object, type);

  if (obj == NULL)
    return NULL;

  obj->ptid = ptid;
  obj->number = number;

  return (PyObject *) obj;
}

/* Create a new gdb.BtraceInstruction object.  */

static PyObject *
btpy_insn_new (ptid_t ptid, Py_ssize_t number)
{
  return btpy_new (ptid, number, &btpy_insn_type);
}

/* Create a new gdb.BtraceFunctionCall object.  */

static PyObject *
btpy_call_new (ptid_t ptid, Py_ssize_t number)
{
  return btpy_new (ptid, number, &btpy_call_type);
}

/* Create a new gdb.BtraceList object.  */

static PyObject *
btpy_list_new (ptid_t ptid, Py_ssize_t first, Py_ssize_t last, Py_ssize_t step,
	       PyTypeObject *element_type)
{
  btpy_list_object * const obj = PyObject_New (btpy_list_object,
					       &btpy_list_type);

  if (obj == NULL)
    return NULL;

  obj->ptid = ptid;
  obj->first = first;
  obj->last = last;
  obj->step = step;
  obj->element_type = element_type;

  return (PyObject *) obj;
}

/* Implementation of BtraceInstruction.number [int] and
   BtraceFunctionCall.number [int].  */

static PyObject *
btpy_number (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;

  return PyInt_FromSsize_t (obj->number);
}

/* Implementation of BtraceInstruction.__hash__ () -> int and
   BtraceFunctionCall.__hash__ () -> int.  */

static Py_hash_t
btpy_hash (PyObject *self)
{
  const btpy_object * const obj = (btpy_object *) self;

  return obj->number;
}

/* Implementation of BtraceInstruction.error [int].  Returns the
   error code for gaps.  */

static PyObject *
btpy_insn_error (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  struct btrace_insn_iterator iter;
  int error;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  error = btrace_insn_get_error (&iter);

  if (error == 0)
    Py_RETURN_NONE;

  return PyInt_FromLong (error);
}

/* Implementation of BtraceInstruction.sal [gdb.Symtab_and_line].
   Return the SAL associated with this instruction.  */

static PyObject *
btpy_insn_sal (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;
  PyObject *result = NULL;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    Py_RETURN_NONE;

  TRY
    {
      result = symtab_and_line_to_sal_object (find_pc_line (insn->pc, 0));
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      GDB_PY_HANDLE_EXCEPTION (except);
    }
  END_CATCH

  return result;
}

/* Implementation of BtraceInstruction.pc [int].  Returns
   the instruction address.  */

static PyObject *
btpy_insn_pc (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    Py_RETURN_NONE;

  return gdb_py_long_from_ulongest (insn->pc);
}

/* Implementation of BtraceInstruction.size [int].  Returns
   the instruction size.  */

static PyObject *
btpy_insn_size (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    Py_RETURN_NONE;

  return PyInt_FromLong (insn->size);
}

/* Implementation of BtraceInstruction.is_speculative [bool].
   Returns if this instruction was executed speculatively.  */

static PyObject *
btpy_insn_is_speculative (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    Py_RETURN_NONE;

  if (insn->flags & BTRACE_INSN_FLAG_SPECULATIVE)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/* Implementation of BtraceInstruction.data [buffer].
   Returns raw instruction data.  */

static PyObject *
btpy_insn_data (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;
  gdb_byte *buffer = NULL;
  PyObject *object;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    Py_RETURN_NONE;

  TRY
    {
      buffer = (gdb_byte *) xmalloc (insn->size);
      read_memory (insn->pc, buffer, insn->size);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      xfree (buffer);
      GDB_PY_HANDLE_EXCEPTION (except);
    }
  END_CATCH

  object = PyBytes_FromStringAndSize ((const char*) buffer, insn->size);
  xfree (buffer);

  if (object == NULL)
    return NULL;

#ifdef IS_PY3K
  return PyMemoryView_FromObject (object);
#else
  return PyBuffer_FromObject (object, 0, Py_END_OF_BUFFER);
#endif

}

/* Implementation of BtraceInstruction.decode [str].  Returns
   the instruction as human readable string.  */

static PyObject *
btpy_insn_decode (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_insn *insn;
  struct btrace_insn_iterator iter;
  string_file strfile;

  BTPY_REQUIRE_VALID_INSN (obj, iter);

  insn = btrace_insn_get (&iter);
  if (insn == NULL)
    {
      int error_code = btrace_insn_get_error (&iter);
      const struct btrace_config *config;

      config = btrace_conf (&find_thread_ptid (obj->ptid)->btrace);
      return PyBytes_FromString (btrace_decode_error (config->format,
						      error_code));
    }

  TRY
    {
      gdb_print_insn (target_gdbarch (), insn->pc, &strfile, NULL);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      gdbpy_convert_exception (except);
      return NULL;
    }
  END_CATCH


  return PyBytes_FromString (strfile.string ().c_str ());
}

/* Implementation of BtraceFunctionCall.level [int].  Returns the
   call level.  */

static PyObject *
btpy_call_level (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  return PyInt_FromLong (iter.btinfo->level + func->level);
}

/* Implementation of BtraceFunctionCall.symbol [gdb.Symbol].  Returns
   the symbol associated with this function call.  */

static PyObject *
btpy_call_symbol (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  if (func->sym == NULL)
    Py_RETURN_NONE;

  return symbol_to_symbol_object (func->sym);
}

/* Implementation of BtraceFunctionCall.instructions [list].
   Return the list of instructions that belong to this function call.  */

static PyObject *
btpy_call_instructions (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;
  unsigned int len;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  len = VEC_length (btrace_insn_s, func->insn);

  /* Gaps count as one instruction.  */
  if (len == 0)
    len = 1;

  return btpy_list_new (obj->ptid, func->insn_offset, func->insn_offset + len,
			1, &btpy_insn_type);
}

/* Implementation of BtraceFunctionCall.up [gdb.BtraceRecordCall].
   Return the caller / returnee of this function.  */

static PyObject *
btpy_call_up (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  if (func->up == NULL)
    Py_RETURN_NONE;

  return btpy_call_new (obj->ptid, func->up->number);
}

/* Implementation of BtraceFunctionCall.prev_sibling [BtraceFunctionCall].
   Return a previous segment of this function.  */

static PyObject *
btpy_call_prev_sibling (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  if (func->segment.prev == NULL)
    Py_RETURN_NONE;

  return btpy_call_new (obj->ptid, func->segment.prev->number);
}

/* Implementation of BtraceFunctionCall.next_sibling [BtraceFunctionCall].
   Return a following segment of this function.  */

static PyObject *
btpy_call_next_sibling (PyObject *self, void *closure)
{
  const btpy_object * const obj = (btpy_object *) self;
  const struct btrace_function *func;
  struct btrace_call_iterator iter;

  BTPY_REQUIRE_VALID_CALL (obj, iter);

  func = btrace_call_get (&iter);
  if (func == NULL)
    Py_RETURN_NONE;

  if (func->segment.next == NULL)
    Py_RETURN_NONE;

  return btpy_call_new (obj->ptid, func->segment.next->number);
}

/* Python rich compare function to allow for equality and inequality checks
   in Python.  */

static PyObject *
btpy_richcompare (PyObject *self, PyObject *other, int op)
{
  const btpy_object * const obj1 = (btpy_object *) self;
  const btpy_object * const obj2 = (btpy_object *) other;

  if (Py_TYPE (self) != Py_TYPE (other))
    {
      Py_INCREF (Py_NotImplemented);
      return Py_NotImplemented;
    }

  switch (op)
  {
    case Py_EQ:
      if (ptid_equal (obj1->ptid, obj2->ptid) && obj1->number == obj2->number)
	Py_RETURN_TRUE;
      else
	Py_RETURN_FALSE;

    case Py_NE:
      if (!ptid_equal (obj1->ptid, obj2->ptid) || obj1->number != obj2->number)
	Py_RETURN_TRUE;
      else
	Py_RETURN_FALSE;

    default:
      break;
  }

  Py_INCREF (Py_NotImplemented);
  return Py_NotImplemented;
}

/* Implementation of BtraceList.__len__ (self) -> int.  */

static Py_ssize_t
btpy_list_length (PyObject *self)
{
  const btpy_list_object * const obj = (btpy_list_object *) self;
  const Py_ssize_t distance = obj->last - obj->first;
  const Py_ssize_t result = distance / obj->step;

  if ((distance % obj->step) == 0)
    return result;

  return result + 1;
}

/* Implementation of
   BtraceList.__getitem__ (self, key) -> BtraceInstruction and
   BtraceList.__getitem__ (self, key) -> BtraceFunctionCall.  */

static PyObject *
btpy_list_item (PyObject *self, Py_ssize_t index)
{
  const btpy_list_object * const obj = (btpy_list_object *) self;
  struct thread_info * const tinfo = find_thread_ptid (obj->ptid);

  if (index < 0 || index >= btpy_list_length (self))
    return PyErr_Format (PyExc_IndexError, _("Index out of range: %zd."),
			 index);

  return btpy_new (obj->ptid, obj->first + (obj->step * index),
		   obj->element_type);
}

/* Implementation of BtraceList.__getitem__ (self, slice) -> BtraceList.  */

static PyObject *
btpy_list_slice (PyObject *self, PyObject *value)
{
  const btpy_list_object * const obj = (btpy_list_object *) self;
  const Py_ssize_t length = btpy_list_length (self);
  Py_ssize_t start, stop, step, slicelength;

  if (PyInt_Check (value))
    {
      Py_ssize_t index = PyInt_AsSsize_t (value);

      /* Emulate Python behavior for negative indices.  */
      if (index < 0)
	index += length;

      return btpy_list_item (self, index);
    }

  if (!PySlice_Check (value))
    return PyErr_Format (PyExc_TypeError, _("Index must be int or slice."));

  if (0 != PySlice_GetIndicesEx (BTPY_PYSLICE (value), length, &start, &stop,
				 &step, &slicelength))
    return NULL;

  return btpy_list_new (obj->ptid, obj->first + obj->step * start,
			obj->first + obj->step * stop, obj->step * step,
			obj->element_type);
}

/* Helper function that returns the position of an element in a BtraceList
   or -1 if the element is not in the list.  */

static LONGEST
btpy_list_position (PyObject *self, PyObject *value)
{
  const btpy_list_object * const list_obj = (btpy_list_object *) self;
  const btpy_object * const obj = (btpy_object *) value;
  Py_ssize_t index = obj->number;

  if (list_obj->element_type != Py_TYPE (value))
    return -1;

  if (!ptid_equal (list_obj->ptid, obj->ptid))
    return -1;

  if (index < list_obj->first || index > list_obj->last)
    return -1;

  index -= list_obj->first;

  if (index % list_obj->step != 0)
    return -1;

  return index / list_obj->step;
}

/* Implementation of "in" operator for BtraceLists.  */

static int
btpy_list_contains (PyObject *self, PyObject *value)
{
  if (btpy_list_position (self, value) < 0)
    return 0;

  return 1;
}

/* Implementation of BtraceLists.index (self, value) -> int.  */

static PyObject *
btpy_list_index (PyObject *self, PyObject *value)
{
  const LONGEST index = btpy_list_position (self, value);

  if (index < 0)
    return PyErr_Format (PyExc_ValueError, _("Not in list."));

  return gdb_py_long_from_longest (index);
}

/* Implementation of BtraceList.count (self, value) -> int.  */

static PyObject *
btpy_list_count (PyObject *self, PyObject *value)
{
  /* We know that if an element is in the list, it is so exactly one time,
     enabling us to reuse the "is element of" check.  */
  return PyInt_FromLong (btpy_list_contains (self, value));
}

/* Python rich compare function to allow for equality and inequality checks
   in Python.  */

static PyObject *
btpy_list_richcompare (PyObject *self, PyObject *other, int op)
{
  const btpy_list_object * const obj1 = (btpy_list_object *) self;
  const btpy_list_object * const obj2 = (btpy_list_object *) other;

  if (Py_TYPE (self) != Py_TYPE (other))
    {
      Py_INCREF (Py_NotImplemented);
      return Py_NotImplemented;
    }

  switch (op)
  {
    case Py_EQ:
      if (ptid_equal (obj1->ptid, obj2->ptid)
	  && obj1->element_type == obj2->element_type
	  && obj1->first == obj2->first
	  && obj1->last == obj2->last
	  && obj1->step == obj2->step)
	Py_RETURN_TRUE;
      else
	Py_RETURN_FALSE;

    case Py_NE:
      if (!ptid_equal (obj1->ptid, obj2->ptid)
	  || obj1->element_type != obj2->element_type
	  || obj1->first != obj2->first
	  || obj1->last != obj2->last
	  || obj1->step != obj2->step)
	Py_RETURN_TRUE;
      else
	Py_RETURN_FALSE;

    default:
      break;
  }

  Py_INCREF (Py_NotImplemented);
  return Py_NotImplemented;
}

/* Implementation of
   BtraceRecord.method [str].  */

PyObject *
recpy_bt_method (PyObject *self, void *closure)
{
  return PyString_FromString ("btrace");
}

/* Implementation of
   BtraceRecord.format [str].  */

PyObject *
recpy_bt_format (PyObject *self, void *closure)
{
  const struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
  const struct btrace_config * config;

  if (tinfo == NULL)
    Py_RETURN_NONE;

  config = btrace_conf (&tinfo->btrace);

  if (config == NULL)
    Py_RETURN_NONE;

  return PyString_FromString (btrace_format_short_string (config->format));
}

/* Implementation of
   BtraceRecord.replay_position [BtraceInstruction].  */

PyObject *
recpy_bt_replay_position (PyObject *self, void *closure)
{
  const struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);

  if (tinfo == NULL)
    Py_RETURN_NONE;

  if (tinfo->btrace.replay == NULL)
    Py_RETURN_NONE;

  return btpy_insn_new (inferior_ptid,
			btrace_insn_number (tinfo->btrace.replay));
}

/* Implementation of
   BtraceRecord.begin [BtraceInstruction].  */

PyObject *
recpy_bt_begin (PyObject *self, void *closure)
{
  struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
  struct btrace_insn_iterator iterator;

  if (tinfo == NULL)
    Py_RETURN_NONE;

  btrace_fetch (tinfo);

  if (btrace_is_empty (tinfo))
    Py_RETURN_NONE;

  btrace_insn_begin (&iterator, &tinfo->btrace);
  return btpy_insn_new (inferior_ptid, btrace_insn_number (&iterator));
}

/* Implementation of
   BtraceRecord.end [BtraceInstruction].  */

PyObject *
recpy_bt_end (PyObject *self, void *closure)
{
  struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
  struct btrace_insn_iterator iterator;

  if (tinfo == NULL)
    Py_RETURN_NONE;

  btrace_fetch (tinfo);

  if (btrace_is_empty (tinfo))
    Py_RETURN_NONE;

  btrace_insn_end (&iterator, &tinfo->btrace);
  return btpy_insn_new (inferior_ptid, btrace_insn_number (&iterator));
}

/* Implementation of
   BtraceRecord.instruction_history [list].  */

PyObject *
recpy_bt_instruction_history (PyObject *self, void *closure)
{
  struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
  struct btrace_insn_iterator iterator;
  unsigned long first = 0;
  unsigned long last = 0;

   if (tinfo == NULL)
     Py_RETURN_NONE;

   btrace_fetch (tinfo);

   if (btrace_is_empty (tinfo))
     Py_RETURN_NONE;

   btrace_insn_begin (&iterator, &tinfo->btrace);
   first = btrace_insn_number (&iterator);

   btrace_insn_end (&iterator, &tinfo->btrace);
   last = btrace_insn_number (&iterator);

   return btpy_list_new (inferior_ptid, first, last, 1, &btpy_insn_type);
}

/* Implementation of
   BtraceRecord.function_call_history [list].  */

PyObject *
recpy_bt_function_call_history (PyObject *self, void *closure)
{
    struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
    struct btrace_call_iterator iterator;
    unsigned long first = 0;
    unsigned long last = 0;

    if (tinfo == NULL)
      Py_RETURN_NONE;

    btrace_fetch (tinfo);

    if (btrace_is_empty (tinfo))
      Py_RETURN_NONE;

    btrace_call_begin (&iterator, &tinfo->btrace);
    first = btrace_call_number (&iterator);

    btrace_call_end (&iterator, &tinfo->btrace);
    last = btrace_call_number (&iterator);

    return btpy_list_new (inferior_ptid, first, last, 1, &btpy_call_type);
}

/* Implementation of BtraceRecord.goto (self, BtraceInstruction) -> None.  */

PyObject *
recpy_bt_goto (PyObject *self, PyObject *args)
{
  struct thread_info * const tinfo = find_thread_ptid (inferior_ptid);
  const btpy_object *obj;

  if (tinfo == NULL || btrace_is_empty (tinfo))
	return PyErr_Format (gdbpy_gdb_error, _("Empty branch trace."));

  if (!PyArg_ParseTuple (args, "O", &obj))
    return NULL;

  if (Py_TYPE (obj) != &btpy_insn_type)
    return PyErr_Format (PyExc_TypeError, _("Argument must be instruction."));

  TRY
    {
      struct btrace_insn_iterator iter;

      btrace_insn_end (&iter, &tinfo->btrace);

      if (btrace_insn_number (&iter) == obj->number)
	target_goto_record_end ();
      else
	target_goto_record (obj->number);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      GDB_PY_HANDLE_EXCEPTION (except);
    }
  END_CATCH

  Py_RETURN_NONE;
}

/* BtraceInstruction members.  */

struct gdb_PyGetSetDef btpy_insn_getset[] =
{
  { "number", btpy_number, NULL, "instruction number", NULL},
  { "error", btpy_insn_error, NULL, "error number for gaps", NULL},
  { "sal", btpy_insn_sal, NULL, "associated symbol and line", NULL},
  { "pc", btpy_insn_pc, NULL, "instruction address", NULL},
  { "data", btpy_insn_data, NULL, "raw instruction data", NULL},
  { "decoded", btpy_insn_decode, NULL, "decoded instruction or error message \
if the instruction is a gap", NULL},
  { "size", btpy_insn_size, NULL, "instruction size in byte", NULL},
  { "is_speculative", btpy_insn_is_speculative, NULL, "if the instruction was \
executed speculatively", NULL},
  {NULL}
};

/* BtraceFunctionCall members.  */

static gdb_PyGetSetDef btpy_call_getset[] =
{
  { "number", btpy_number, NULL, "function call number", NULL},
  { "level", btpy_call_level, NULL, "call stack level", NULL},
  { "symbol", btpy_call_symbol, NULL, "associated line and symbol", NULL},
  { "instructions", btpy_call_instructions, NULL, "list of instructions in \
this function segment", NULL},
  { "up", btpy_call_up, NULL, "caller or returned-to function segment", NULL},
  { "prev_sibling", btpy_call_prev_sibling, NULL, "previous segment of this \
function", NULL},
  { "next_sibling", btpy_call_next_sibling, NULL, "next segment of this \
function", NULL},
  {NULL}
};

/* BtraceList methods.  */

struct PyMethodDef btpy_list_methods[] =
{
  { "count", btpy_list_count, METH_O, "count number of occurences"},
  { "index", btpy_list_index, METH_O, "index of entry"},
  {NULL}
};

/* BtraceList sequence methods.  */

static PySequenceMethods btpy_list_sequence_methods =
{
  NULL
};

/* BtraceList mapping methods.  Necessary for slicing.  */

static PyMappingMethods btpy_list_mapping_methods =
{
  NULL
};

/* Sets up the btrace record API.  */

int
gdbpy_initialize_btrace (void)
{
  btpy_insn_type.tp_new = PyType_GenericNew;
  btpy_insn_type.tp_flags = Py_TPFLAGS_DEFAULT;
  btpy_insn_type.tp_basicsize = sizeof (btpy_object);
  btpy_insn_type.tp_name = "gdb.BtraceInstruction";
  btpy_insn_type.tp_doc = "GDB btrace instruction object";
  btpy_insn_type.tp_getset = btpy_insn_getset;
  btpy_insn_type.tp_richcompare = btpy_richcompare;
  btpy_insn_type.tp_hash = btpy_hash;

  btpy_call_type.tp_new = PyType_GenericNew;
  btpy_call_type.tp_flags = Py_TPFLAGS_DEFAULT;
  btpy_call_type.tp_basicsize = sizeof (btpy_object);
  btpy_call_type.tp_name = "gdb.BtraceFunctionCall";
  btpy_call_type.tp_doc = "GDB btrace call object";
  btpy_call_type.tp_getset = btpy_call_getset;
  btpy_call_type.tp_richcompare = btpy_richcompare;
  btpy_call_type.tp_hash = btpy_hash;

  btpy_list_type.tp_new = PyType_GenericNew;
  btpy_list_type.tp_flags = Py_TPFLAGS_DEFAULT;
  btpy_list_type.tp_basicsize = sizeof (btpy_list_object);
  btpy_list_type.tp_name = "gdb.BtraceObjectList";
  btpy_list_type.tp_doc = "GDB btrace list object";
  btpy_list_type.tp_methods = btpy_list_methods;
  btpy_list_type.tp_as_sequence = &btpy_list_sequence_methods;
  btpy_list_type.tp_as_mapping = &btpy_list_mapping_methods;
  btpy_list_type.tp_richcompare = btpy_list_richcompare;

  btpy_list_sequence_methods.sq_item = btpy_list_item;
  btpy_list_sequence_methods.sq_length = btpy_list_length;
  btpy_list_sequence_methods.sq_contains = btpy_list_contains;

  btpy_list_mapping_methods.mp_subscript = btpy_list_slice;

  if (PyType_Ready (&btpy_insn_type) < 0
      || PyType_Ready (&btpy_call_type) < 0
      || PyType_Ready (&btpy_list_type) < 0)
    return -1;
  else
    return 0;
}
