/* longjmp-free interface between gdb and gdbtk.
   Copyright (C) 1999 Free Software Foundation, Inc.
 
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
#include "frame.h"
#include "value.h"
#include "gdbtk-wrapper.h"

/*
 * Wrapper functions exported to the world
 */

gdb_result GDB_value_fetch_lazy PARAMS ((value_ptr));

gdb_result GDB_evaluate_expression PARAMS ((struct expression *, value_ptr *));

gdb_result GDB_type_print PARAMS ((value_ptr, char *, GDB_FILE *, int));

gdb_result GDB_val_print PARAMS ((struct type *type, char *valaddr,
                                  CORE_ADDR address, GDB_FILE *stream,
                                  int format, int deref_ref, int recurse,
                                  enum val_prettyprint pretty));

gdb_result GDB_select_frame PARAMS ((struct frame_info *, int));

gdb_result GDB_value_equal PARAMS ((value_ptr, value_ptr, int *));

gdb_result GDB_parse_exp_1 PARAMS ((char **stringptr, struct block *block, int comma,
                                    struct expression **result));

gdb_result GDB_evaluate_type PARAMS ((struct expression *exp, value_ptr *result));

gdb_result GDB_block_for_pc PARAMS ((CORE_ADDR pc, struct block **result));

gdb_result GDB_block_innermost_frame PARAMS ((struct block *block,
                                              struct frame_info **result));

gdb_result GDB_reinit_frame_cache PARAMS ((void));

gdb_result GDB_find_frame_addr_in_frame_chain PARAMS ((CORE_ADDR addr,
                                                       struct frame_info **result));

/*
 * Private functions for this file
 */
static gdb_result call_wrapped_function PARAMS ((catch_errors_ftype *,
                                    struct gdb_wrapper_arguments *));

static int wrap_type_print PARAMS ((char *));

static int wrap_evaluate_expression PARAMS ((char *));

static int wrap_value_fetch_lazy PARAMS ((char *));

static int wrap_val_print PARAMS ((char*));

static int wrap_select_frame PARAMS ((char *));

static int wrap_value_equal PARAMS ((char *));

static int wrap_parse_exp_1 PARAMS ((char *opaque_arg));

static int wrap_evaluate_type PARAMS ((char *opaque_arg));

static int wrap_block_for_pc PARAMS ((char *opaque_arg));

static int wrap_block_innermost_frame PARAMS ((char *opaque_arg));

static int wrap_reinit_frame_cache PARAMS ((char *opaque_arg));

static int wrap_find_frame_addr_in_frame_chain PARAMS ((char *opaque_arg));

static gdb_result
call_wrapped_function (fn, arg)
     catch_errors_ftype *fn;
     struct gdb_wrapper_arguments *arg;
{
  if (!catch_errors (fn, (char *) &arg, "", RETURN_MASK_ERROR))
    {
      /* An error occurred */
      return GDB_ERROR;
    }

  return GDB_OK;
}

gdb_result
GDB_type_print (val, varstring, stream, show)
     value_ptr val;
     char *varstring;
     GDB_FILE *stream;
     int show;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) val;
  args.args[1] = varstring;
  args.args[2] = (char *) stream;
  args.args[3] = (char *) show;
  return call_wrapped_function ((catch_errors_ftype *) wrap_type_print, &args);
}

static int
wrap_type_print (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;
  value_ptr val = (value_ptr) (*args)->args[0];
  char *varstring = (*args)->args[1];
  GDB_FILE *stream = (GDB_FILE *) (*args)->args[2];
  int show = (int) (*args)->args[3];
  type_print (VALUE_TYPE (val), varstring, stream, show);
  return 1;
}

gdb_result
GDB_val_print (type, valaddr, address, stream, format, deref_ref,
               recurse, pretty)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     GDB_FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) type;
  args.args[1] = (char *) valaddr;
  args.args[2] = (char *) address;
  args.args[3] = (char *) stream;
  args.args[4] = (char *) format;
  args.args[5] = (char *) deref_ref;
  args.args[6] = (char *) recurse;
  args.args[7] = (char *) pretty;

  return call_wrapped_function ((catch_errors_ftype *) wrap_val_print, &args);
}

static int
wrap_val_print (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;
  struct type *type;
  char *valaddr;
  CORE_ADDR address;
  GDB_FILE *stream;
  int format;
  int deref_ref;
  int recurse;
  enum val_prettyprint pretty;

  type      = (struct type *) (*args)->args[0];
  valaddr   = (char *) (*args)->args[1];
  address   = (CORE_ADDR) (*args)->args[2];
  stream    = (GDB_FILE *) (*args)->args[3];
  format    = (int) (*args)->args[4];
  deref_ref = (int) (*args)->args[5];
  recurse   = (int) (*args)->args[6];
  pretty    = (enum val_prettyprint) (*args)->args[7];

  val_print (type, valaddr, 0, address, stream, format, deref_ref,
             recurse, pretty);
  return 1;
}

gdb_result
GDB_value_fetch_lazy (value)
     value_ptr value;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) value;
  return call_wrapped_function ((catch_errors_ftype *) wrap_value_fetch_lazy, &args);
}

static int
wrap_value_fetch_lazy (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;

  value_fetch_lazy ((value_ptr) (*args)->args[0]);
  return 1;
}

gdb_result
GDB_evaluate_expression (exp, value)
     struct expression *exp;
     value_ptr *value;
{
  struct gdb_wrapper_arguments args;
  gdb_result result;
  args.args[0] = (char *) exp;

  result = call_wrapped_function ((catch_errors_ftype *) wrap_evaluate_expression, &args);
  if (result != GDB_OK)
    return result;

  *value = (value_ptr) args.result;
  return GDB_OK;
}

static int
wrap_evaluate_expression (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;

  (*args)->result = 
    (char *) evaluate_expression ((struct expression *) (*args)->args[0]);
  return 1;
}

gdb_result
GDB_select_frame (fi, level)
     struct frame_info *fi;
     int level;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) fi;
  args.args[1] = (char *) &level;

  return call_wrapped_function ((catch_errors_ftype *) wrap_select_frame, &args);
}

static int
wrap_select_frame (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;
  int level = * (int *) (*args)->args[1];
  struct frame_info *fi = (struct frame_info *) (*args)->args[0];

  select_frame (fi, level);
  return 1;
}

gdb_result
GDB_value_equal (val1, val2, result)
     value_ptr val1;
     value_ptr val2;
     int *result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) val1;
  args.args[1] = (char *) val2;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_value_equal, &args);
  if (r != GDB_OK)
    return r;

  *result = (int) args.result;
  return GDB_OK;
}

static int
wrap_value_equal (a)
     char *a;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) a;
  value_ptr val1, val2;

  val1 = (value_ptr) (*args)->args[0];
  val2 = (value_ptr) (*args)->args[1];

  (*args)->result = (char *) value_equal (val1, val2);
  return 1;
}

gdb_result
GDB_parse_exp_1 (stringptr, block, comma, result)
     char **stringptr;
     struct block *block;
     int comma;
     struct expression **result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) stringptr;
  args.args[1] = (char *) block;
  args.args[2] = (char *) comma;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_parse_exp_1, &args);
  if (r != GDB_OK)
    return r;

  *result = (struct expression *) args.result;
  return GDB_OK;
}

static int
wrap_parse_exp_1 (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) opaque_arg;
  struct block *block;
  char **stringptr;
  int comma;

  stringptr = (char **) (*args)->args[0];
  block     = (struct block *) (*args)->args[1];
  comma     = (int) (*args)->args[2];

  (*args)->result = (char *) parse_exp_1 (stringptr, block, comma);
  return 1;
}

gdb_result
GDB_evaluate_type (exp, result)
     struct expression *exp;
     value_ptr *result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) exp;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_evaluate_type, &args);
  if (r != GDB_OK)
    return r;

  *result = (value_ptr) args.result;
  return GDB_OK;
}

static int
wrap_evaluate_type (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) opaque_arg;
  struct expression *exp;

  exp = (struct expression *) (*args)->args[0];
  (*args)->result = (char *) evaluate_type (exp);
  return 1;
}

gdb_result
GDB_block_for_pc (pc, result)
     CORE_ADDR pc;
     struct block **result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) pc;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_block_for_pc, &args);
  if (r != GDB_OK)
    return r;

  *result = (struct block *) args.result;
  return GDB_OK;
}

static int
wrap_block_for_pc (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) opaque_arg;
  CORE_ADDR pc;

  pc = (CORE_ADDR) (*args)->args[0];
  (*args)->result = (char *) block_for_pc (pc);
  return 1;
}

gdb_result
GDB_block_innermost_frame (block, result)
     struct block *block;
     struct frame_info **result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) block;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_block_innermost_frame, &args);
  if (r != GDB_OK)
    return r;

  *result = (struct frame_info *) args.result;
  return GDB_OK;
}

static int
wrap_block_innermost_frame (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) opaque_arg;
  struct block *block;

  block = (struct block *) (*args)->args[0];
  (*args)->result = (char *) block_innermost_frame (block);
  return 1;
}

gdb_result
GDB_reinit_frame_cache ()
{
  gdb_result r;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_reinit_frame_cache, NULL);
  if (r != GDB_OK)
    return r;

  return GDB_OK;
}

static int
wrap_reinit_frame_cache (opaque_arg)
     char *opaque_arg;
{
  reinit_frame_cache ();
  return 1;
}

gdb_result
GDB_find_frame_addr_in_frame_chain (addr, result)
     CORE_ADDR addr;
     struct frame_info **result;
{
  struct gdb_wrapper_arguments args;
  gdb_result r;

  args.args[0] = (char *) addr;

  r = call_wrapped_function ((catch_errors_ftype *) wrap_find_frame_addr_in_frame_chain, &args);
  if (r != GDB_OK)
    return r;

  *result = (struct frame_info *) args.result;
  return GDB_OK;
}

static int
wrap_find_frame_addr_in_frame_chain (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments **args = (struct gdb_wrapper_arguments **) opaque_arg;
  CORE_ADDR addr;

  addr = (CORE_ADDR) (*args)->args[0];
  (*args)->result = (char *) find_frame_addr_in_frame_chain (addr);
  return 1;
}


/* Local variables: */
/* change-log-default-name: "ChangeLog-gdbtk" */
/* End: */
