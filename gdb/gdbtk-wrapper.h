/* longjmp-free interface between gdb and gdbtk.
   Copyright 1999 Free Software Foundation, Inc.

This file is part of GDB.  It contains routines to safely call common gdb
functions without the fear of longjmp'ing.
 
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

#ifndef GDBTK_WRAPPER_H
#define GDBTK_WRAPPER_H
/* Use this struct used to pass arguments to wrapper routines. We assume
   (arbitrarily) that no gdb function takes more than ten arguments. */
struct gdb_wrapper_arguments {

  /* Pointer to some result from the gdb function call, if any */
  char *result;

  /* The list of arguments. */
  char *args[10];
};

/* Whenever any gdb function wrapper is called, its return status is: */
typedef enum gdb_wrapper_status { GDB_OK, GDB_ERROR } gdb_result;

/* This list of functions which have been wrapped. Please keep this list
   in alphabetical order, using "GDB_" to prefix the actual name of the
   function. */
extern gdb_result GDB_evaluate_expression PARAMS ((struct expression *expr, value_ptr *val));
extern gdb_result GDB_select_frame PARAMS ((struct frame_info *fi, int level));
extern gdb_result GDB_type_print PARAMS ((value_ptr val, char *varstring, GDB_FILE *stream, int show));
extern gdb_result GDB_val_print PARAMS ((struct type *type, char *valaddr,
                                  CORE_ADDR address, GDB_FILE *stream,
                                  int format, int deref_ref, int recurse,
                                  enum val_prettyprint pretty));
extern gdb_result GDB_value_fetch_lazy PARAMS ((value_ptr value));
extern gdb_result GDB_value_equal PARAMS ((value_ptr val1, value_ptr val2, int *result));
extern gdb_result GDB_parse_exp_1 PARAMS ((char **stringptr, struct block *block, int comma,
                                           struct expression **result));
extern gdb_result GDB_evaluate_type PARAMS ((struct expression *exp, value_ptr *result));
extern gdb_result GDB_block_for_pc PARAMS ((CORE_ADDR pc, struct block **result));
extern gdb_result GDB_block_innermost_frame PARAMS ((struct block *block,
                                              struct frame_info **result));
extern gdb_result GDB_reinit_frame_cache PARAMS ((void));
extern gdb_result GDB_find_frame_addr_in_frame_chain PARAMS ((CORE_ADDR addr,
                                                       struct frame_info **result));
#endif /* GDBTK_WRAPPER_H */

/* Local variables: */
/* change-log-default-name: "ChangeLog-gdbtk" */
/* End: */
