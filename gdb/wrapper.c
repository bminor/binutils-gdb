/* Longjump free calls to gdb internal routines.
   Copyright 1999 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "value.h"
#include "frame.h"
#include "wrapper.h"

/* Use this struct used to pass arguments to wrapper routines. We assume
   (arbitrarily) that no gdb function takes more than ten arguments. */
struct gdb_wrapper_arguments
  {

    /* Pointer to some result from the gdb function call, if any */
    char *result;

    /* The list of arguments. */
    char *args[10];
  };

int gdb_evaluate_expression PARAMS ((struct expression *, value_ptr *));
int wrap_evaluate_expression PARAMS ((char *));

int gdb_value_fetch_lazy PARAMS ((value_ptr));
int wrap_value_fetch_lazy PARAMS ((char *));

int gdb_value_equal PARAMS ((value_ptr, value_ptr, int *));
int wrap_value_equal PARAMS ((char *));

int gdb_value_ind PARAMS ((value_ptr val, value_ptr * rval));
int wrap_value_ind PARAMS ((char *opaque_arg));

int
gdb_evaluate_expression (exp, value)
     struct expression *exp;
     value_ptr *value;
{
  struct gdb_wrapper_arguments args;
  args.args[0] = (char *) exp;

  if (!catch_errors ((catch_errors_ftype *) wrap_evaluate_expression, &args,
		     "", RETURN_MASK_ERROR))
    {
      /* An error occurred */
      return 0;
    }

  *value = (value_ptr) args.result;
  return 1;
}

int
wrap_evaluate_expression (a)
     char *a;
{
  struct gdb_wrapper_arguments *args = (struct gdb_wrapper_arguments *) a;

  (args)->result =
    (char *) evaluate_expression ((struct expression *) (args)->args[0]);
  return 1;
}

int
gdb_value_fetch_lazy (value)
     value_ptr value;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) value;
  return catch_errors ((catch_errors_ftype *) wrap_value_fetch_lazy, &args,
		       "", RETURN_MASK_ERROR);
}

int
wrap_value_fetch_lazy (a)
     char *a;
{
  struct gdb_wrapper_arguments *args = (struct gdb_wrapper_arguments *) a;

  value_fetch_lazy ((value_ptr) (args)->args[0]);
  return 1;
}

int
gdb_value_equal (val1, val2, result)
     value_ptr val1;
     value_ptr val2;
     int *result;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) val1;
  args.args[1] = (char *) val2;

  if (!catch_errors ((catch_errors_ftype *) wrap_value_equal, &args,
		     "", RETURN_MASK_ERROR))
    {
      /* An error occurred */
      return 0;
    }

  *result = (int) args.result;
  return 1;
}

int
wrap_value_equal (a)
     char *a;
{
  struct gdb_wrapper_arguments *args = (struct gdb_wrapper_arguments *) a;
  value_ptr val1, val2;

  val1 = (value_ptr) (args)->args[0];
  val2 = (value_ptr) (args)->args[1];

  (args)->result = (char *) value_equal (val1, val2);
  return 1;
}

int
gdb_value_ind (val, rval)
     value_ptr val;
     value_ptr *rval;
{
  struct gdb_wrapper_arguments args;

  args.args[0] = (char *) val;

  if (!catch_errors ((catch_errors_ftype *) wrap_value_ind, &args,
		     "", RETURN_MASK_ERROR))
    {
      /* An error occurred */
      return 0;
    }

  *rval = (value_ptr) args.result;
  return 1;
}

int
wrap_value_ind (opaque_arg)
     char *opaque_arg;
{
  struct gdb_wrapper_arguments *args = (struct gdb_wrapper_arguments *) opaque_arg;
  value_ptr val;

  val = (value_ptr) (args)->args[0];
  (args)->result = (char *) value_ind (val);
  return 1;
}
