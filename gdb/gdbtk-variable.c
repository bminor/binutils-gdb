/* Variable user interface for GDB, the GNU debugger.
   Copyright 1999 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "value.h"
#include "expression.h"
#include "frame.h"
#include "valprint.h"

#include <tcl.h>
#include <tk.h>
#include "gdbtk.h"
#include "gdbtk-wrapper.h"

#include <math.h>

/* Enumeration type defining the return values for valueChanged */
enum value_changed
{
  VALUE_UNCHANGED,         /* the variable's value is unchanged  */
  VALUE_CHANGED,           /* the variable's value has changed   */
  VALUE_OUT_OF_SCOPE       /* the variable is no longer in scope */
};

/* String representations of the value_changed enums */
static char *value_changed_string[] = {
  "VARIABLE_UNCHANGED",
  "VARIABLE_CHANGED",
  "VARIABLE_OUT_OF_SCOPE",
  NULL
};

/* Enumeration for the format types */
enum display_format
{
  FORMAT_NATURAL,          /* What gdb actually calls 'natural' */
  FORMAT_BINARY,           /* Binary display                    */
  FORMAT_DECIMAL,          /* Decimal display                   */
  FORMAT_HEXADECIMAL,      /* Hex display                       */
  FORMAT_OCTAL             /* Octal display                     */
};

/* Mappings of display_format enums to gdb's format codes */
int format_code[] = {0, 't', 'd', 'x', 'o'};

/* String representations of the format codes */
char *format_string[] = {"natural", "binary", "decimal", "hexadecimal", "octal"};

/* Every parent variable keeps a linked list of its children, described
   by the following structure. */
struct variable_child {

  /* Pointer to the child's data */
  struct _gdb_variable *child;

  /* Pointer to the next child */
  struct variable_child *next;
};

/* Every variable in the system has a structure of this type defined
   for it. This structure holds all information necessary to manipulate
   a particular object variable. Members which must be freed are noted. */
struct _gdb_variable {

  /* Alloc'd name of the variable for this object.. If this variable is a
     child, then this name will be the child's source name.
     (bar, not foo.bar) */
  char *name;

  /* The alloc'd real name of this variable. This is used to construct the
     variable's children. It is always a valid expression. */
  char *real_name;

  /* The alloc'd name for this variable's object. This is here for
     convenience when constructing this object's children. */
  char *obj_name;

  /* Alloc'd expression for this variable */
  struct expression *exp;

  /* Block for which this expression is valid */
  struct block *valid_block;

  /* The frame for this expression */
  CORE_ADDR frame;  

  /* The value of this expression */
  value_ptr value;

  /* Did an error occur evaluating the expression or getting its value? */
  int error;

  /* The number of (immediate) children this variable has */
  int num_children;

  /* If this object is a child, this points to its parent. */
  struct _gdb_variable *parent;

  /* A list of this object's children */
  struct variable_child *children;

  /* The format of the output for this object */
  enum display_format format;
};

typedef struct _gdb_variable gdb_variable;

/* This variable will hold the value of the output from gdb
   for commands executed through call_gdb_* */
static Tcl_Obj *fputs_obj;

/*
 * Public functions defined in this file
 */

int gdb_variable_init PARAMS ((Tcl_Interp *));

/*
 * Private functions defined in this file
 */

/* Entries into this file */

static int gdb_variable_command PARAMS ((ClientData, Tcl_Interp *, int,
                                         Tcl_Obj *CONST[]));

static int variable_create PARAMS ((Tcl_Interp *, int, Tcl_Obj *CONST[]));

static void variable_delete PARAMS ((Tcl_Interp *, gdb_variable *));

static void variable_debug PARAMS ((gdb_variable *));

static int variable_obj_command PARAMS ((ClientData, Tcl_Interp *, int,
                                         Tcl_Obj *CONST[]));
static Tcl_Obj *variable_children PARAMS ((Tcl_Interp *, gdb_variable *));

static enum value_changed variable_value_changed PARAMS ((gdb_variable *));

static int variable_format PARAMS ((Tcl_Interp *, int, Tcl_Obj *CONST[],
                                         gdb_variable *));

static int variable_type PARAMS ((Tcl_Interp *, int, Tcl_Obj *CONST[],
                                         gdb_variable *));

static int variable_value PARAMS ((Tcl_Interp *, int, Tcl_Obj *CONST[],
                                         gdb_variable *));

static int variable_editable PARAMS ((gdb_variable *));

/* Helper functions for the above functions. */

static gdb_variable *create_variable PARAMS ((char *, char *, CORE_ADDR));

static void delete_children PARAMS ((Tcl_Interp *, gdb_variable *, int));

static void install_variable PARAMS ((Tcl_Interp *, char *, gdb_variable *));

static void uninstall_variable PARAMS ((Tcl_Interp *, gdb_variable *));

static gdb_variable *child_exists PARAMS ((gdb_variable *, char *));

static gdb_variable *create_child PARAMS ((Tcl_Interp *, gdb_variable *,
                                           char *, int));
static char *name_of_child PARAMS ((gdb_variable *, int));

static int number_of_children PARAMS ((gdb_variable *));

static enum display_format variable_default_display PARAMS ((gdb_variable *));

static void save_child_in_parent PARAMS ((gdb_variable *, gdb_variable *));

static void remove_child_from_parent PARAMS ((gdb_variable *, gdb_variable *));

static struct type *get_type PARAMS ((value_ptr));

static struct type *get_target_type PARAMS ((struct type *));

static Tcl_Obj *get_call_output PARAMS ((void));

static void clear_gdb_output PARAMS ((void));

static int call_gdb_type_print PARAMS ((value_ptr));

static int call_gdb_val_print PARAMS ((value_ptr, int));

static void variable_fputs PARAMS ((const char *, FILE *));

static void null_fputs PARAMS ((const char *, FILE *));

static int my_value_equal PARAMS ((gdb_variable *, value_ptr));

#define INIT_VARIABLE(x) {           \
(x)->name         = NULL;            \
(x)->real_name    = NULL;            \
(x)->obj_name     = NULL;            \
(x)->exp          = NULL;            \
(x)->valid_block  = NULL;            \
(x)->frame        = (CORE_ADDR) 0;   \
(x)->value        = NULL;            \
(x)->error        = 0;               \
(x)->num_children = 0;               \
(x)->parent       = NULL;            \
(x)->children     = NULL;            \
(x)->format       = FORMAT_NATURAL;  \
}

#if defined(FREEIF)
#  undef FREEIF
#endif
#define FREEIF(x) if (x != NULL) free((char *) (x))

/* Initialize the variable code. This function should be called once
   to install and initialize the variable code into the interpreter. */
int
gdb_variable_init (interp)
     Tcl_Interp *interp;
{
  Tcl_Command result;

  result = Tcl_CreateObjCommand (interp, "gdb_variable", call_wrapper,
                                 (ClientData) gdb_variable_command, NULL);
  if (result == NULL)
    return TCL_ERROR;

  return TCL_OK;
}

/* This function defines the "gdb_variable" command which is used to
   create variable objects. Its syntax includes:

     gdb_variable create
     gdb_variable create NAME
     gdb_variable create -expr EXPR
     gdb_variable create NAME -expr EXPR

     NAME = name of object to create. If no NAME, then automatically create
          a name
     EXPR = the gdb expression for which to create a variable. This will
          be the most common usage.
*/
static int
gdb_variable_command (clientData, interp, objc, objv)
     ClientData   clientData;
     Tcl_Interp  *interp;
     int          objc;
     Tcl_Obj *CONST objv[];
{
  static char *commands[] = { "create", NULL };
  enum commands_enum { VARIABLE_CREATE };
  int index, result;

  if (objc < 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "option ?arg...?");
      return TCL_ERROR;
    }

  if (Tcl_GetIndexFromObj (interp, objv[1], commands, "options", 0,
                           &index) != TCL_OK)
    {
      return TCL_ERROR;
    }

  switch ((enum commands_enum) index)
    {
    case VARIABLE_CREATE: 
      result = variable_create (interp, objc - 2, objv + 2);
      break;

    default:
      return TCL_ERROR;
    }

  return result;
}

/* This function implements the actual object command for each
   variable object that is created (and each of its children).

   Currently the following commands are implemented:
   - delete        delete this object and its children
   - valueChanged  has the value of this object changed since the last check?
   - numChildren   how many children does this object have
   - children      create the children and return a list of their objects
   - debug         print out a little debug info for the object
   - name          print out the name of this variable
*/
static int
variable_obj_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  enum commands_enum {
    VARIABLE_DELETE,
    VARIABLE_VALUE_CHANGED,
    VARIABLE_NUM_CHILDREN,
    VARIABLE_CHILDREN,
    VARIABLE_DEBUG,
    VARIABLE_FORMAT,
    VARIABLE_TYPE,
    VARIABLE_VALUE,
    VARIABLE_NAME,
    VARIABLE_EDITABLE
  };
  static char *commands[] = {
    "delete",
    "valueChanged",
    "numChildren",
    "children",
    "debug",
    "format",
    "type",
    "value",
    "name",
    "editable",
    NULL
  };
  gdb_variable *var = (gdb_variable *) clientData;
  int index, result;

  if (objc < 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "option ?arg...?");
      return TCL_ERROR;
    }

  if (Tcl_GetIndexFromObj (interp, objv[1], commands, "options", 0,
                           &index) != TCL_OK)
    return TCL_ERROR;

  result = TCL_OK;
  switch ((enum commands_enum) index)
    {
    case VARIABLE_DELETE:
      if (objc > 2)
        {
          int len;
          char *s = Tcl_GetStringFromObj (objv[2], &len);
          if (*s == 'c' && strncmp (s, "children", len) == 0)
            {
              delete_children (interp, var, 1);
              break;
            }
        }
      variable_delete (interp, var);
      break;

    case VARIABLE_VALUE_CHANGED:
      {
        enum value_changed vc = variable_value_changed (var);
        Tcl_SetObjResult (interp, Tcl_NewStringObj (value_changed_string[vc], -1));
      }
      break;

    case VARIABLE_NUM_CHILDREN:
      Tcl_SetObjResult (interp, Tcl_NewIntObj (var->num_children));
      break;

    case VARIABLE_CHILDREN:
      {
        Tcl_Obj *children = variable_children (interp, var);
        Tcl_SetObjResult (interp, children);
      }
      break;

    case VARIABLE_DEBUG:
      variable_debug (var);
      break;

    case VARIABLE_FORMAT:
      result = variable_format (interp, objc, objv, var);
      break;

    case VARIABLE_TYPE:
      result = variable_type (interp, objc, objv, var);
      break;

    case VARIABLE_VALUE:
      result = variable_value (interp, objc, objv, var);
      break;

    case VARIABLE_NAME:
      Tcl_SetObjResult (interp, Tcl_NewStringObj (var->name, -1));
      break;

    case VARIABLE_EDITABLE:
      Tcl_SetObjResult (interp, Tcl_NewIntObj (variable_editable (var)));
      break;

    default:
      return TCL_ERROR;
    }

  return result;
}

/*
 * Variable object construction/destruction
 */

/* This function is responsible for processing the user's specifications
   and constructing a variable object. */
static int
variable_create (interp, objc, objv)
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  enum create_opts { CREATE_EXPR, CREATE_PC };
  static char *create_options[] = { "-expr", "-pc", NULL };
  gdb_variable *var;
  char *name;
  char obj_name[31];
  int index;
  static int id = 0;
  CORE_ADDR pc = (CORE_ADDR) -1;

  /* REMINDER: This command may be invoked in the following ways:
     gdb_variable create
     gdb_variable create NAME
     gdb_variable create -expr EXPR
     gdb_variable create NAME -expr EXPR

     NAME = name of object to create. If no NAME, then automatically create
          a name
     EXPR = the gdb expression for which to create a variable. This will
          be the most common usage.
  */
  name = NULL;
  if (objc)
    name = Tcl_GetStringFromObj (objv[0], NULL);
  if (name == NULL || *name == '-')
    {
      /* generate a name for this object */
      id++;
      sprintf (obj_name, "var%d", id);
    }
  else
    {
      /* specified name for object */
      strncpy (obj_name, name, 30);
      objv++;
      objc--;
    }

  /* Run through all the possible options for this command */
  name = NULL;
  while (objc > 0)
    {
      if (Tcl_GetIndexFromObj (interp, objv[0], create_options, "options",
                               0, &index) != TCL_OK)
        {
          result_ptr->flags |= GDBTK_IN_TCL_RESULT;
          return TCL_ERROR;
        }

      switch ((enum create_opts) index)
        {
        case CREATE_EXPR:
          name = Tcl_GetStringFromObj (objv[1], NULL);
          objc--;
          objv++;
          break;

        case CREATE_PC:
          {
            char *str;
            str = Tcl_GetStringFromObj (objv[1], NULL);
            pc = parse_and_eval_address (str);
            objc--;
            objv++;
          }
          break;

        default:
          break;
        }

      objc--;
      objv++;
    }

  /* Create the variable */
  var = create_variable (name, name, pc);

  if (var != NULL)
    {
      /* Install a command into the interpreter that represents this
         object */
      install_variable (interp, obj_name, var);
      Tcl_SetObjResult (interp, Tcl_NewStringObj (obj_name, -1));
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;

      return TCL_OK;
    }

  return TCL_ERROR;
}

/* Fill out a gdb_variable structure for the variable being constructed. 
   This function should never fail if real_name is a valid expression.
   (That means no longjmp'ing!) */
static gdb_variable *
create_variable (name, real_name, pc)
     char *name;
     char *real_name;
     CORE_ADDR pc;
{
  gdb_variable *var;
  value_ptr mark;
  struct frame_info *fi, *old_fi;
  struct block *block;
  void (*old_fputs) PARAMS ((const char *, FILE *));
  gdb_result r;

  var  = (gdb_variable *) xmalloc (sizeof (gdb_variable));
  INIT_VARIABLE (var);

  if (name != NULL)
    {
      char *p;

      /* Parse and evaluate the expression, filling in as much
         of the variable's data as possible */

      /* Allow creator to specify context of variable */
      if (pc == (CORE_ADDR) -1)
        block = 0;
      else
        {
          r = GDB_block_for_pc (pc, &block);
          if (r != GDB_OK)
            block = 0;
        }

      p = real_name;
      innermost_block  = NULL;
      r = GDB_parse_exp_1 (&p, block, 0, &(var->exp));
      if (r != GDB_OK)
        {
          FREEIF ((char *) var);
          return NULL;
        }

      /* Don't allow variables to be created for types. */
      if (var->exp->elts[0].opcode == OP_TYPE)
        {
          free_current_contents ((char **) &(var->exp));
          FREEIF (var);
          printf_unfiltered ("Attempt to use a type name as an expression.");
          return NULL;
        }

      var->valid_block = innermost_block;
      var->name        = savestring (name, strlen (name));
      var->real_name   = savestring (real_name, strlen (real_name));

      /* Several of the GDB_* calls can cause messages to be displayed. We swallow
         those here, because we don't need them (the "value" command will
         show them). */
      old_fputs = fputs_unfiltered_hook;
      fputs_unfiltered_hook = null_fputs;

      /* When the PC is different from the current PC (pc == -1), 
         then we must select the appropriate frame before parsing
         the expression, otherwise the value will not be current.
         Since select_frame is so benign, just call it for all cases. */
      r = GDB_block_innermost_frame (var->valid_block, &fi);
      if (r != GDB_OK)
        fi = NULL;
      if (fi)
        var->frame = FRAME_FP (fi);
      old_fi = selected_frame;
      GDB_select_frame (fi, -1);

      mark = value_mark ();
      if (GDB_evaluate_expression (var->exp, &var->value) == GDB_OK)
        {
          release_value (var->value);
          if (VALUE_LAZY (var->value))
            {
              if (GDB_value_fetch_lazy (var->value) != GDB_OK)
                var->error = 1;
              else
                var->error = 0;
            }
        }
      else
        var->error = 1;
      value_free_to_mark (mark);

      /* Reset the selected frame */
      GDB_select_frame (old_fi, -1);

      /* Restore the output hook to normal */
      fputs_unfiltered_hook = old_fputs;

      var->num_children = number_of_children (var);
      var->format = variable_default_display (var);
    }

  return var;
}

/* Install the given variable VAR into the tcl interpreter with
   the object name NAME. */
static void
install_variable (interp, name, var)
     Tcl_Interp *interp;
     char *name;
     gdb_variable *var;
{
  var->obj_name = savestring (name, strlen (name));
  Tcl_CreateObjCommand (interp, name, variable_obj_command, 
                        (ClientData) var, NULL);
}

/* Unistall the object VAR in the tcl interpreter. */
static void
uninstall_variable (interp, var)
     Tcl_Interp *interp;
     gdb_variable *var;
{
  Tcl_DeleteCommand (interp, var->obj_name);
}

/* Delete the variable object VAR and its children */
static void
variable_delete (interp, var)
     Tcl_Interp *interp;
     gdb_variable *var;
{
  /* Delete any children of this variable, too. */
  delete_children (interp, var, 0);

  /* If this variable has a parent, remove it from its parent's list */
  if (var->parent != NULL)
    {
      remove_child_from_parent (var->parent, var);
    }

  uninstall_variable (interp, var);

  /* Free memory associated with this variable */
  FREEIF (var->name);
  FREEIF (var->real_name);
  FREEIF (var->obj_name);
  if (var->exp != NULL)
    free_current_contents ((char **) &var->exp);
  FREEIF (var);
}

/* Silly debugging info */
static void
variable_debug (var)
     gdb_variable *var;
{
  Tcl_Obj *str;

  str = Tcl_NewStringObj ("name=", -1);
  Tcl_AppendStringsToObj (str, var->name, "\nreal_name=", var->real_name,
                          "\nobj_name=", var->obj_name, NULL);
  Tcl_SetObjResult (gdbtk_interp, str);
}

/*
 * Child construction/destruction
 */

/* Delete the children associated with the object VAR. If NOTIFY is set,
   notify the parent object that this child was deleted. This is used as
   a small optimization when deleting variables and their children. If the
   parent is also being deleted, don't bother notifying it that its children
   are being deleted. */
static void
delete_children (interp, var, notify)
     Tcl_Interp *interp;
     gdb_variable *var;
     int notify;
{
  struct variable_child *vc;
  struct variable_child *next;

  for (vc = var->children; vc != NULL; vc = next)
    {
      if (!notify)
        vc->child->parent = NULL;
      variable_delete (interp, vc->child);
      next = vc->next;
      free (vc);
    }
}

/* Return the number of children for a given variable.

   This can get a little complicated, since we would like to make
   certain assumptions about certain types of variables. 

   - struct/union *: dereference first
   - (*)(): do not allow derefencing
   - arrays:
     - declared size = num of children  or
     - -1 if we don't know, i.e., int foo [];
   - if there was an error reported constructing this object,
     assume it has no children (and try this again later)
   - void * and char *  have no children
*/
static int
number_of_children (var)
     gdb_variable *var;
{
  struct type *type;
  struct type *target;
  int children;

  if (var->value != NULL)
    {
      type     = get_type (var->value);
      target   = get_target_type (type);
      children = 0;

      switch (TYPE_CODE (type))
        {
        case TYPE_CODE_ARRAY:
          if (TYPE_LENGTH (type) > 0 && TYPE_LENGTH (target) > 0
              && TYPE_ARRAY_UPPER_BOUND_TYPE (type) != BOUND_CANNOT_BE_DETERMINED)
            children = TYPE_LENGTH (type) / TYPE_LENGTH (target);
          else
            children = -1;
          break;

        case TYPE_CODE_STRUCT:
        case TYPE_CODE_UNION:
          /* If we have a virtual table pointer, omit it. */
          if (TYPE_VPTR_BASETYPE (type) == type
              && !(TYPE_VPTR_FIELDNO (type) < 0))
            children = TYPE_NFIELDS (type) - 1;
          else
            children = TYPE_NFIELDS (type);
          break;

        case TYPE_CODE_PTR:
          /* This is where things get compilcated. All pointers have one child.
             Except, of course, for struct and union ptr, which we automagically
             dereference for the user and function ptrs, which have no children. */
          switch (TYPE_CODE (target))
            {
            case TYPE_CODE_STRUCT:
            case TYPE_CODE_UNION:
              /* If we have a virtual table pointer, omit it. */
              if (TYPE_VPTR_BASETYPE (target) == target
                  && !(TYPE_VPTR_FIELDNO (target) < 0))
                children = TYPE_NFIELDS (target) - 1;
              else
                children = TYPE_NFIELDS (target);
              break;

            case TYPE_CODE_FUNC:
              children = 0;
              break;

            default: 
              /* Don't dereference char* or void*. */
              if (TYPE_NAME (target) != NULL
                  && (STREQ (TYPE_NAME (target), "char")
                      || STREQ (TYPE_NAME (target), "void")))
                children = 0;
              else
                children = 1;
            }
          break;

        default:
          break;
        }
    }
  else
    {
      /* var->value can be null if we tried to access non-existent or
         protected memory. In this case, we simply do not allow any
         children. This will be checked again when we check if its
         value has changed. */
      children = 0;
    }

  return children;
}

/* Return a list of all the children of VAR, creating them if necessary. */
static Tcl_Obj *
variable_children (interp, var)
     Tcl_Interp *interp;
     gdb_variable *var;
{
  Tcl_Obj *list;
  gdb_variable *child;
  char *name;
  int i;

  list = Tcl_NewListObj (0, NULL);
  for (i = 0; i < var->num_children; i++)
    {
      /* check if child exists */
      name = name_of_child (var, i);
      child = child_exists (var, name);
      if (child == NULL)
        {
          child = create_child (interp, var, name, i);

          /* name_of_child returns a malloc'd string */
          free (name);
        }
      Tcl_ListObjAppendElement (NULL, list, Tcl_NewStringObj (child->obj_name, -1));
    }

  return list;
}

/* Does a child with the name NAME exist in VAR? If so, return its data.
   If not, return NULL. */
static gdb_variable *
child_exists (var, name)
     gdb_variable *var; /* Parent */
     char *name;        /* name of child */
{
  struct variable_child *vc;

  for (vc = var->children; vc != NULL; vc = vc->next)
    {
      if (STREQ (vc->child->name, name))
        return vc->child;
    }

  return NULL;
}

/* Create and install a child of the parent of the given name */
static gdb_variable *
create_child (interp, parent, name, index)
     Tcl_Interp *interp;
     gdb_variable *parent;
     char *name;
     int index;
{
  struct type *type;
  struct type *target;
  gdb_variable *child;
  char separator[10], prefix[2048], suffix[20];
  char *childs_name;
  char *save_name;
  int deref = 0;
  int len;

  /* name should never be null. For pointer derefs, it should contain "*name".
     For arrays of a known size, the name will simply contain the index into
     the array. */

  separator[0] = '\0';
  prefix[0] = '\0';
  suffix[0] = '\0';;
  save_name = name;

  /* This code must contain a lot of the logic for children based on the parent's
     type. */
  type = get_type (parent->value);
  target = get_target_type (type);

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      sprintf (suffix, "[%s]", name);
      name = "";
      break;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      if (index < TYPE_N_BASECLASSES (type))
        {
          strcpy (prefix, "((");
          strcat (prefix, name);
          strcat (prefix, ")");
          strcpy (suffix, ") ");
          name = "";
        }
      else
        strcpy (separator, ".");
      break;

    case TYPE_CODE_PTR:
      switch (TYPE_CODE (target))
        {
        case TYPE_CODE_STRUCT:
        case TYPE_CODE_UNION:
          if (index < TYPE_N_BASECLASSES (target))
            {
              strcpy (prefix, "(*(");
              strcat (prefix, name);
              strcat (prefix, " *)");
              strcpy (suffix, ")");
              name = "";
            }
          else
            strcpy (separator, "->");
          break;

        default:
          deref = 1;
          break;
        }

    default:
      break;
    }

  /* When we get here, we should know how to construct a legal 
     expression for the child's name */
  len  = strlen (prefix);
  len += strlen (parent->real_name);
  len += strlen (separator);
  len += strlen (name);
  len += strlen (suffix);
  if (deref)
    len += 3;
  childs_name = (char *) xmalloc ((len + 1) * sizeof (char));
  if (deref)
    {
      strcpy (childs_name, "(*");
      strcat (childs_name, parent->real_name);
      strcat (childs_name, suffix);
      strcat (childs_name, ")");
    }
  else
    {
      strcpy (childs_name, prefix);
      strcat (childs_name, parent->real_name);
      strcat (childs_name, separator);
      strcat (childs_name, name);
      strcat (childs_name, suffix);
    }

  /* childs_name now contains a valid expression for the child */
  child = create_variable (save_name, childs_name, (CORE_ADDR) -1);
  child->parent = parent;
  free (childs_name);
  childs_name = (char *) xmalloc ((strlen (parent->obj_name) + strlen (save_name) + 2)
                                  * sizeof (char));
  sprintf (childs_name, "%s.%s", parent->obj_name, save_name);
  install_variable (interp, childs_name, child);
  free (childs_name);

  /* Save a pointer to this child in the parent */
  save_child_in_parent (parent, child);

  return child;
}

/* Save CHILD in the PARENT's data. */
static void
save_child_in_parent (parent, child)
     gdb_variable *parent;
     gdb_variable *child;
{
  struct variable_child *vc;

  /* Insert the child at the top */
  vc = parent->children;
  parent->children =
    (struct variable_child *) xmalloc (sizeof (struct variable_child));

  parent->children->next = vc;
  parent->children->child  = child;
}

/* Remove the CHILD from the PARENT's list of children. */
static void
remove_child_from_parent (parent, child)
     gdb_variable *parent;
     gdb_variable *child;
{
  struct variable_child *vc, *prev;

  /* Find the child in the parent's list */
  prev = NULL;
  for (vc = parent->children; vc != NULL; )
    {
      if (vc->child == child)
        break;
      prev = vc;
      vc = vc->next;
    }

  if (prev == NULL)
    parent->children = vc->next;
  else
    prev->next = vc->next;
  
}

/* What is the name of the INDEX'th child of VAR? */
static char *
name_of_child (var, index)
     gdb_variable *var;
     int index;
{
  struct type *type;
  struct type *target;
  char *name;
  char *string;

  type = get_type (var->value);
  target = get_target_type (type);

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
      {
        /* We never get here unless var->num_children is greater than 0... */
        int len = 1;
        while ((int) pow ((double) 10, (double) len) < index)
          len++;
        name = (char *) xmalloc (1 + len * sizeof (char));
        sprintf (name, "%d", index);
      }
      break;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      string = TYPE_FIELD_NAME (type, index);
      name   = savestring (string, strlen (string));
      break;

    case TYPE_CODE_PTR:
      switch (TYPE_CODE (target))
        {
        case TYPE_CODE_STRUCT:
        case TYPE_CODE_UNION:
          string = TYPE_FIELD_NAME (target, index);
          name   = savestring (string, strlen (string));
          break;

        default:
          name = (char *) xmalloc ((strlen (var->name) + 2) * sizeof (char));
          sprintf (name, "*%s", var->name);
          break;
        }
    }

  return name;
}

/* Has the value of this object changed since the last time we looked?

   There are some special cases:
   - structs/unions/arrays.  The "value" of these never changes. 
      Only their children's values change.
   - if an error occurred with evaluate_expression or fetch_value_lazy,
     then we need to be a little more elaborate with our determination
     of "value changed". Specifically, the value does not change when
     both the previous evaluate fails and the one done here also fails.
*/
static enum value_changed
variable_value_changed (var)
     gdb_variable *var;
{
  value_ptr mark, new_val;
  struct frame_info *fi, *old_fi;
  int within_scope;
  enum value_changed result;
  gdb_result r;

  /* Save the selected stack frame, since we will need to change it
     in order to evaluate expressions. */
  old_fi = selected_frame;

  /* Determine whether the variable is still around. */
  if (var->valid_block == NULL)
    within_scope = 1;
  else
    {
      GDB_reinit_frame_cache ();
      r = GDB_find_frame_addr_in_frame_chain (var->frame, &fi);
      if (r != GDB_OK)
        fi = NULL;
      within_scope = fi != NULL;
      /* FIXME: GDB_select_frame could fail */
      if (within_scope)
        GDB_select_frame (fi, -1);
    }

  result = VALUE_OUT_OF_SCOPE;
  if (within_scope)
    {
      struct type *type = get_type (var->value);

      /* Arrays, struct, classes, unions never change value */
      if (type != NULL && (TYPE_CODE (type) == TYPE_CODE_STRUCT 
                           || TYPE_CODE (type) == TYPE_CODE_UNION
                           || TYPE_CODE (type) == TYPE_CODE_ARRAY))
        result = VALUE_UNCHANGED;
      else
        {
          mark = value_mark ();
          if (GDB_evaluate_expression (var->exp, &new_val) == GDB_OK)
            {
              if (!my_value_equal (var, new_val))
                {
                  /* value changed */                  
                  release_value (new_val);
                  if (var->value == NULL)
                    {
                      /* This can happen if there was an error
                         evaluating the expression (like deref NULL) */
                      var->num_children = number_of_children (var);
                    }
                  value_free (var->value);
                  var->value = new_val;
                  result = VALUE_CHANGED;
                }
              else
                result = VALUE_UNCHANGED;
            }
          else
            {
              /* evaluate expression failed. If we failed before, then
                 the value of this variable has not changed. If we
                 succeed before, then the value did change. */
              if (var->value == NULL)
                result = VALUE_UNCHANGED;
              else
                {
                  var->value = NULL;
                  var->error = 1;
                  result = VALUE_CHANGED;
                }
            }
                  
          value_free_to_mark (mark);
        }
    }

  /* Restore selected frame */
  GDB_select_frame (old_fi, -1);

  return result;
}

static int
variable_format (interp, objc, objv, var)
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
     gdb_variable *var;
{

  if (objc > 2)
    {
      /* Set the format of VAR to given format */
      int len;
      char *fmt = Tcl_GetStringFromObj (objv[2], &len);
      if (STREQN (fmt, "natural", len))
        var->format = FORMAT_NATURAL;
      else if (STREQN (fmt, "binary", len))
        var->format = FORMAT_NATURAL;
      else if (STREQN (fmt, "decimal", len))
        var->format = FORMAT_DECIMAL;
      else if (STREQN (fmt, "hexadecimal", len))
        var->format = FORMAT_HEXADECIMAL;
      else if (STREQN (fmt, "octal", len))
        var->format = FORMAT_OCTAL;
      else
        {
          Tcl_Obj *obj = Tcl_NewStringObj (NULL, 0);
          Tcl_AppendStringsToObj (obj, "unknown display format \"",
                                  fmt, "\": must be: \"natural\", \"binary\""
                                  ", \"decimal\", \"hexadecimal\", or \"octal\"",
                                  NULL);
          Tcl_SetObjResult (interp, obj);
          return TCL_ERROR;
        }
    }
  else
    {
      /* Report the current format */
      Tcl_Obj *fmt;

      fmt = Tcl_NewStringObj (format_string [(int) var->format], -1);
      Tcl_SetObjResult (interp, fmt);
    }

  return TCL_OK;
}

/* What is the default display for this variable? We assume that
   everything is "natural". Any exceptions? */
static enum display_format
variable_default_display (var)
     gdb_variable *var;
{
  return FORMAT_NATURAL;
}

/* This function returns the type of a variable in the interpreter (or an error)
   and returns either TCL_OK or TCL_ERROR as appropriate. */
static int
variable_type (interp, objc, objv, var)
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
     gdb_variable *var;
{
  int result;
  value_ptr val;
  char *first, *last, *string;
  Tcl_RegExp regexp;
  gdb_result r;

  if (var->value != NULL)
    val = var->value;
  else
    {
      r = GDB_evaluate_type (var->exp, &val);
      if (r != GDB_OK)
        return TCL_ERROR;
    }

  result = call_gdb_type_print (val);
  if (result == TCL_OK)
    {
      string = strdup (Tcl_GetStringFromObj (get_call_output (), NULL));
      first = string;

      /* gdb will print things out like "struct {...}" for anonymous structs.
         In gui-land, we don't want the {...}, so we strip it here. */
      regexp = Tcl_RegExpCompile (interp, "{...}");
      if (Tcl_RegExpExec (interp, regexp, string, first))
        {
          /* We have an anonymous struct/union/class/enum */
          Tcl_RegExpRange (regexp, 0, &first, &last);
          if (*(first - 1) == ' ')
            first--;
          *first = '\0';
        }

      Tcl_SetObjResult (interp, Tcl_NewStringObj (string, -1));
      FREEIF (string);
      return TCL_OK;
    }

  Tcl_SetObjResult (interp, get_call_output ());
  return result;
}

/* This function returns the value of a variable in the interpreter (or an error)
   and returns either TCL_OK or TCL_ERROR as appropriate. */
static int
variable_value (interp, objc, objv, var)
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
     gdb_variable *var;
{
  int result;
  struct type *type;
  value_ptr val;
  Tcl_Obj *str;
  gdb_result r;
  int real_addressprint;

  /* If we set the value of the variable, objv[2] will contain the
     variable's new value. We need to first construct a legal expression
     for this -- ugh! */
  if (objc > 2)
    {
      /* Does this cover all the bases? */
      struct expression *exp;
      value_ptr value;
      int saved_input_radix = input_radix;

      if (VALUE_LVAL (var->value) != not_lval && var->value->modifiable)
        {
          char *s;

          input_radix = 10;   /* ALWAYS reset to decimal temporarily */
          s = Tcl_GetStringFromObj (objv[2], NULL);
          r = GDB_parse_exp_1 (&s, 0, 0, &exp);
          if (r != GDB_OK)
            return TCL_ERROR;
          if (GDB_evaluate_expression (exp, &value) != GDB_OK)
            return TCL_ERROR;

          val = value_assign (var->value, value);
          value_free (var->value);
          release_value (val);
          var->value = val;
          input_radix = saved_input_radix;
        }

      return TCL_OK;
    }

  if (var->value != NULL)
    val = var->value;
  else
    {
      /* This can happen if we attempt to get the value of a struct
         member when the parent is an invalid pointer.

         GDB reports the error as the error derived from accessing the
         parent, but we don't have access to that here... */
      Tcl_SetObjResult (interp, Tcl_NewStringObj ("???", -1));
      return TCL_ERROR;
    }

  /* C++: addressprint causes val_print to print the
     address of the reference, too. So clear it to get
     the real value -- BUT ONLY FOR C++ REFERENCE TYPES! */
  real_addressprint = addressprint;

  /* BOGUS: if val_print sees a struct/class, it will print out its
     children instead of "{...}" */
  type = get_type (val);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      str = Tcl_NewStringObj ("{...}", -1);
      break;

    case TYPE_CODE_ARRAY:
      {
        char number[256];
        str = Tcl_NewStringObj (NULL, 0);
        sprintf (number, "%d", var->num_children);
        Tcl_AppendStringsToObj (str, "[", number, "]", NULL);
      }
      break;

    case TYPE_CODE_REF:
      /* Clear addressprint so that the actual value is printed */
      addressprint = 0;

      /* fall through */
    default:
      result = call_gdb_val_print (val, format_code[(int) var->format]);
      Tcl_SetObjResult (interp, get_call_output ());

      /* Restore addressprint */
      addressprint = real_addressprint;
      return result;
    }

  /* We only get here if we encountered one of the "special types" above */

  /* Restore addressprint */
  addressprint = real_addressprint;

  Tcl_SetObjResult (interp, str);
  return TCL_OK;
}

/* Is this variable editable? Use the variable's type to make
   this determination. */
static int
variable_editable (var)
     gdb_variable *var;
{
  struct type *type;
  int result;
  gdb_result r;

  type = get_type (var->value);
  if (type == NULL)
    {
      value_ptr val;
      r = GDB_evaluate_type (var->exp, &val);
      if (r != GDB_OK)
        return 0;
      type = get_type (val);
    }

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_ARRAY:
    case TYPE_CODE_FUNC:
    case TYPE_CODE_MEMBER:
    case TYPE_CODE_METHOD:
      result = 0;
      break;

    default:
      result = 1;
      break;
    }

  return result;
}

/*
 * Call stuff. These functions are used to capture the output of gdb commands
 * without going through the tcl interpreter.
 */

/* Retrieve gdb output in the buffer since last call. */
static Tcl_Obj *
get_call_output ()
{
  /* Clear the error flags, in case we errored. */
  if (result_ptr != NULL)
    result_ptr->flags &= ~GDBTK_ERROR_ONLY;
  return fputs_obj;
}

/* Clear the output of the buffer. */
static void
clear_gdb_output ()
{
  if (fputs_obj != NULL)
    Tcl_DecrRefCount (fputs_obj);

  fputs_obj = Tcl_NewStringObj (NULL, -1);
  Tcl_IncrRefCount (fputs_obj);
}

/* Call the gdb command "type_print", retaining its output in the buffer. */
static int
call_gdb_type_print (val)
     value_ptr val;
{
  void (*old_hook) PARAMS ((const char *, FILE *));
  int result;

  /* Save the old hook and install new hook */
  old_hook = fputs_unfiltered_hook;
  fputs_unfiltered_hook = variable_fputs;

  /* Call our command with our args */
  clear_gdb_output ();


  if (GDB_type_print (val, "", gdb_stdout, -1) == GDB_OK)
    result = TCL_OK;
  else
    result = TCL_ERROR;

  /* Restore fputs hook */
  fputs_unfiltered_hook = old_hook;

  return result;
}

/* Call the gdb command "val_print", retaining its output in the buffer. */
static int
call_gdb_val_print (val, format)
     value_ptr val;
     int format;
{
  void (*old_hook) PARAMS ((const char *, FILE *));
  gdb_result r;
  int result;

  /* Save the old hook and install new hook */
  old_hook = fputs_unfiltered_hook;
  fputs_unfiltered_hook = variable_fputs;

  /* Call our command with our args */
  clear_gdb_output ();

  if (VALUE_LAZY (val))
    {
      r = GDB_value_fetch_lazy (val);
      if (r != GDB_OK)
        {
          fputs_unfiltered_hook = old_hook;
          return TCL_ERROR;
        }
    }
  r = GDB_val_print (VALUE_TYPE (val), VALUE_CONTENTS_RAW (val), VALUE_ADDRESS (val),
                     gdb_stdout, format, 1, 0, 0);
  if (r == GDB_OK)
    result = TCL_OK;
  else
    result = TCL_ERROR;

  /* Restore fputs hook */
  fputs_unfiltered_hook = old_hook;

  return result;
}

/* The fputs_unfiltered_hook function used to save the output from one of the
   call commands in this file. */
static void
variable_fputs (text, stream)
     const char *text;
     FILE *stream;
{
  /* Just append everything to the fputs_obj... Issues with stderr/stdout? */
  Tcl_AppendToObj (fputs_obj, (char *) text, -1);
}

/* Empty handler for the fputs_unfiltered_hook. Set the hook to this function
   whenever the output is irrelevent. */
static void
null_fputs (text, stream)
     const char *text;
     FILE *stream;
{
  return;
}

/*
 * Special wrapper-like stuff to supplement the generic wrappers
 */

/* This returns the type of the variable. This skips past typedefs
   and returns the real type of the variable. */
static struct type *
get_type (val)
     value_ptr val;
{
  struct type *type = NULL;

  if (val != NULL)
    {
      type = VALUE_TYPE (val);
      while (type != NULL && TYPE_CODE (type) == TYPE_CODE_TYPEDEF)
        type = TYPE_TARGET_TYPE (type);
    }

  return type;
}

/* This returns the target type (or NULL) of TYPE, also skipping
   past typedefs, just like get_type (). */
static struct type *
get_target_type (type)
     struct type *type;
{
  if (type != NULL)
    {
      type = TYPE_TARGET_TYPE (type);
      while (type != NULL && TYPE_CODE (type) == TYPE_CODE_TYPEDEF)
        type = TYPE_TARGET_TYPE (type);
    }

  return type;
}

/* This function is a special wrap. This call never "fails".*/
static int
my_value_equal (var, val2)
     gdb_variable *var;
     value_ptr val2;
{
  int err1, err2, r;

  /* This is bogus, but unfortunately necessary. We must know
   exactly what caused an error -- reading var->val (which we
   get from var->error and/or val2, so that we can really determine
   if we think that something has changed. */
  err1 = var->error;
  err2 = 0;
  if (VALUE_LAZY (val2) && GDB_value_fetch_lazy (val2) != GDB_OK)
    err2 = 1;

  /* Another special case: NULL values. If both are null, say
     they're equal. */
  if (var->value == NULL && val2 == NULL)
    return 1;
  else if (var->value == NULL || val2 == NULL)
    return 0;

  if (GDB_value_equal (var->value, val2, &r) != GDB_OK)
    {
      /* An error occurred, this could have happened if
         either val1 or val2 errored. ERR1 and ERR2 tell
         us which of these it is. If both errored, then
         we assume nothing has changed. If one of them is
         valid, though, then something has changed. */
      if (err1 == err2)
        {
          /* both the old and new values caused errors, so
             we say the value did not change */
          /* This is indeterminate, though. Perhaps we should
             be safe and say, yes, it changed anyway?? */
          return 1;
        }
      else
        {
          /* err2 replaces var->error since this new value
             WILL replace the old one. */
          var->error = err2;
          return 0;
        }
    }

  return r;
}

/* Local variables: */
/* change-log-default-name: "ChangeLog-gdbtk" */
/* End: */
