/* Gdb/Python header for private use by Python module.

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

#ifndef GDB_PYTHON_INTERNAL_H
#define GDB_PYTHON_INTERNAL_H

#include <stdio.h>

/* Python 2.4 doesn't include stdint.h soon enough to get {u,}intptr_t
   needed by pyport.h.  */
#include <stdint.h>

/* /usr/include/features.h on linux systems will define _POSIX_C_SOURCE
   if it sees _GNU_SOURCE (which config.h will define).
   pyconfig.h defines _POSIX_C_SOURCE to a different value than
   /usr/include/features.h does causing compilation to fail.
   To work around this, undef _POSIX_C_SOURCE before we include Python.h.

   Same problem with _XOPEN_SOURCE.  */
#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE

#if HAVE_LIBPYTHON2_4
#include "python2.4/Python.h"
#include "python2.4/frameobject.h"
/* Py_ssize_t is not defined until 2.5.
   Logical type for Py_ssize_t is Py_intptr_t, but that fails in 64-bit
   compilation due to several apparent mistakes in python2.4 API, so we
   use 'int' instead.  */
typedef int Py_ssize_t;
#elif HAVE_LIBPYTHON2_5
#include "python2.5/Python.h"
#include "python2.5/frameobject.h"
#elif HAVE_LIBPYTHON2_6
#include "python2.6/Python.h"
#include "python2.6/frameobject.h"
#elif HAVE_LIBPYTHON2_7
#include "python2.7/Python.h"
#include "python2.7/frameobject.h"
#else
#error "Unable to find usable Python.h"
#endif

/* If Python.h does not define WITH_THREAD, then the various
   GIL-related functions will not be defined.  However,
   PyGILState_STATE will be.  */
#ifndef WITH_THREAD
#define PyGILState_Ensure() ((PyGILState_STATE) 0)
#define PyGILState_Release(ARG) ((void)(ARG))
#define PyEval_InitThreads() 0
#define PyThreadState_Swap(ARG) ((void)(ARG))
#define PyEval_InitThreads() 0
#define PyEval_ReleaseLock() 0
#endif

/* In order to be able to parse symtab_and_line_to_sal_object function 
   a real symtab_and_line structure is needed.  */
#include "symtab.h"

/* Also needed to parse enum var_types. */
#include "command.h"

struct block;
struct value;
struct language_defn;
struct program_space;

extern PyObject *gdb_module;
extern PyTypeObject value_object_type;
extern PyTypeObject block_object_type;
extern PyTypeObject symbol_object_type;

typedef struct
{
  PyObject_HEAD

  /* The thread we represent.  */
  struct thread_info *thread;

  /* The Inferior object to which this thread belongs.  */
  PyObject *inf_obj;
} thread_object;

extern struct cmd_list_element *set_python_list;
extern struct cmd_list_element *show_python_list;

PyObject *gdbpy_history (PyObject *self, PyObject *args);
PyObject *gdbpy_breakpoints (PyObject *, PyObject *);
PyObject *gdbpy_frame_stop_reason_string (PyObject *, PyObject *);
PyObject *gdbpy_lookup_symbol (PyObject *self, PyObject *args, PyObject *kw);
PyObject *gdbpy_selected_frame (PyObject *self, PyObject *args);
PyObject *gdbpy_block_for_pc (PyObject *self, PyObject *args);
PyObject *gdbpy_lookup_type (PyObject *self, PyObject *args, PyObject *kw);
PyObject *gdbpy_create_lazy_string_object (CORE_ADDR address, long length,
					   const char *encoding, struct type *type);
PyObject *gdbpy_inferiors (PyObject *unused, PyObject *unused2);
PyObject *gdbpy_selected_thread (PyObject *self, PyObject *args);
PyObject *gdbpy_string_to_argv (PyObject *self, PyObject *args);
PyObject *gdbpy_get_hook_function (const char *);
PyObject *gdbpy_parameter (PyObject *self, PyObject *args);
PyObject *gdbpy_parameter_value (enum var_types type, void *var);
char *gdbpy_parse_command_name (char *text,
				struct cmd_list_element ***base_list,
				struct cmd_list_element **start_list);

PyObject *symtab_and_line_to_sal_object (struct symtab_and_line sal);
PyObject *symtab_to_symtab_object (struct symtab *symtab);
PyObject *symbol_to_symbol_object (struct symbol *sym);
PyObject *block_to_block_object (struct block *block, struct objfile *objfile);
PyObject *value_to_value_object (struct value *v);
PyObject *type_to_type_object (struct type *);
PyObject *frame_info_to_frame_object (struct frame_info *frame);

PyObject *pspace_to_pspace_object (struct program_space *);
PyObject *pspy_get_printers (PyObject *, void *);

PyObject *objfile_to_objfile_object (struct objfile *);
PyObject *objfpy_get_printers (PyObject *, void *);

thread_object *create_thread_object (struct thread_info *tp);
thread_object *find_thread_object (ptid_t ptid);
PyObject *find_inferior_object (int pid);

struct block *block_object_to_block (PyObject *obj);
struct symbol *symbol_object_to_symbol (PyObject *obj);
struct value *value_object_to_value (PyObject *self);
struct value *convert_value_from_python (PyObject *obj);
struct type *type_object_to_type (PyObject *obj);
struct symtab *symtab_object_to_symtab (PyObject *obj);
struct symtab_and_line *sal_object_to_symtab_and_line (PyObject *obj);

void gdbpy_initialize_auto_load (void);
void gdbpy_initialize_values (void);
void gdbpy_initialize_frames (void);
void gdbpy_initialize_symtabs (void);
void gdbpy_initialize_commands (void);
void gdbpy_initialize_symbols (void);
void gdbpy_initialize_symtabs (void);
void gdbpy_initialize_blocks (void);
void gdbpy_initialize_types (void);
void gdbpy_initialize_functions (void);
void gdbpy_initialize_pspace (void);
void gdbpy_initialize_objfile (void);
void gdbpy_initialize_breakpoints (void);
void gdbpy_initialize_lazy_string (void);
void gdbpy_initialize_parameters (void);
void gdbpy_initialize_thread (void);
void gdbpy_initialize_inferior (void);

struct cleanup *make_cleanup_py_decref (PyObject *py);

struct cleanup *ensure_python_env (struct gdbarch *gdbarch,
				   const struct language_defn *language);

extern struct gdbarch *python_gdbarch;
extern const struct language_defn *python_language;

/* Use this after a TRY_EXCEPT to throw the appropriate Python
   exception.  */
#define GDB_PY_HANDLE_EXCEPTION(Exception)				\
    do {								\
      if (Exception.reason < 0)						\
	return PyErr_Format (Exception.reason == RETURN_QUIT		\
			     ? PyExc_KeyboardInterrupt : PyExc_RuntimeError, \
			     "%s", Exception.message);			\
    } while (0)

/* Use this after a TRY_EXCEPT to throw the appropriate Python
   exception.  This macro is for use inside setter functions.  */
#define GDB_PY_SET_HANDLE_EXCEPTION(Exception)				\
    do {								\
      if (Exception.reason < 0)						\
        {								\
	  PyErr_Format (Exception.reason == RETURN_QUIT			\
			? PyExc_KeyboardInterrupt : PyExc_RuntimeError, \
			"%s", Exception.message);			\
	  return -1;							\
	}								\
    } while (0)

void gdbpy_print_stack (void);

void source_python_script_for_objfile (struct objfile *objfile,
				       FILE *stream, const char *file);

PyObject *python_string_to_unicode (PyObject *obj);
char *unicode_to_target_string (PyObject *unicode_str);
char *python_string_to_target_string (PyObject *obj);
PyObject *python_string_to_target_python_string (PyObject *obj);
char *python_string_to_host_string (PyObject *obj);
PyObject *target_string_to_unicode (const gdb_byte *str, int length);
int gdbpy_is_string (PyObject *obj);
char *gdbpy_obj_to_string (PyObject *obj);
char *gdbpy_exception_to_string (PyObject *ptype, PyObject *pvalue);

int gdbpy_is_lazy_string (PyObject *result);
gdb_byte *gdbpy_extract_lazy_string (PyObject *string,
				     struct type **str_type, 
				     long *length, char **encoding);

int gdbpy_is_value_object (PyObject *obj);

/* Note that these are declared here, and not in python.h with the
   other pretty-printer functions, because they refer to PyObject.  */
PyObject *apply_varobj_pretty_printer (PyObject *print_obj,
				       struct value **replacement);
PyObject *gdbpy_get_varobj_pretty_printer (struct value *value);
char *gdbpy_get_display_hint (PyObject *printer);
PyObject *gdbpy_default_visualizer (PyObject *self, PyObject *args);

extern PyObject *gdbpy_doc_cst;
extern PyObject *gdbpy_children_cst;
extern PyObject *gdbpy_to_string_cst;
extern PyObject *gdbpy_display_hint_cst;
extern PyObject *gdbpy_enabled_cst;

extern PyObject *gdbpy_gdberror_exc;

int get_addr_from_python (PyObject *obj, CORE_ADDR *addr);

#endif /* GDB_PYTHON_INTERNAL_H */
