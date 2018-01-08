/* Python interface to inferior events.

   Copyright (C) 2009-2018 Free Software Foundation, Inc.

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

#ifndef GDB_PY_EVENT_H
#define GDB_PY_EVENT_H

#include "py-events.h"
#include "command.h"
#include "python-internal.h"
#include "inferior.h"
#include "py-ref.h"

/* Declare all event types.  */
#define GDB_PY_DEFINE_EVENT_TYPE(name, py_name, doc, base) \
  extern PyTypeObject name##_event_object_type		    \
        CPYCHECKER_TYPE_OBJECT_FOR_TYPEDEF ("event_object");
#include "py-event-types.def"
#undef GDB_PY_DEFINE_EVENT_TYPE

typedef struct
{
  PyObject_HEAD

  PyObject *dict;
} event_object;

extern int emit_continue_event (ptid_t ptid);
extern int emit_exited_event (const LONGEST *exit_code, struct inferior *inf);

/* For inferior function call events, discriminate whether event is
   before or after the call. */

typedef enum
{
  /* Before the call */
  INFERIOR_CALL_PRE,
  /* after the call */
  INFERIOR_CALL_POST,
} inferior_call_kind;

extern int emit_inferior_call_event (inferior_call_kind kind,
				     ptid_t thread, CORE_ADDR addr);
extern int emit_register_changed_event (struct frame_info *frame,
				        int regnum);
extern int emit_memory_changed_event (CORE_ADDR addr, ssize_t len);
extern int evpy_emit_event (PyObject *event,
                            eventregistry_object *registry);

extern gdbpy_ref<> create_event_object (PyTypeObject *py_type);
extern gdbpy_ref<> create_thread_event_object (PyTypeObject *py_type,
					       PyObject *thread = nullptr);
extern int emit_new_objfile_event (struct objfile *objfile);
extern int emit_clear_objfiles_event (void);

extern void evpy_dealloc (PyObject *self);
extern int evpy_add_attribute (PyObject *event,
                               const char *name, PyObject *attr)
  CPYCHECKER_NEGATIVE_RESULT_SETS_EXCEPTION;
int gdbpy_initialize_event_generic (PyTypeObject *type, const char *name)
  CPYCHECKER_NEGATIVE_RESULT_SETS_EXCEPTION;

#endif /* GDB_PY_EVENT_H */
