/* Tcl/Tk command definitions for gdbtk.
   Copyright 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.

   Written by Stu Grossman <grossman@cygnus.com> of Cygnus Support.

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
#include "symtab.h"
#include "inferior.h"
#include "command.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
#include "target.h"
#include "gdbcore.h"
#include "tracepoint.h"
#include "demangle.h"

#ifdef _WIN32
#include <winuser.h>
#endif

#include <sys/stat.h>

#include <tcl.h>
#include <tk.h>
#include <itcl.h> 
#include <tix.h> 
#include "guitcl.h"
#include "gdbtk.h"

#ifdef IDE
/* start-sanitize-ide */
#include "event.h"
#include "idetcl.h"
#include "ilutk.h"
/* end-sanitize-ide */
#endif

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>
#include "top.h"
#include <sys/ioctl.h>
#include "gdb_string.h"
#include "dis-asm.h"
#include <stdio.h>
#include "gdbcmd.h"

#include "annotate.h"
#include <sys/time.h>

/* This structure filled in call_wrapper and passed to
   the wrapped call function.
   It stores the command pointer and arguments 
   run in the wrapper function. */

struct wrapped_call_args
{
  Tcl_Interp *interp;
  Tcl_ObjCmdProc *func;
  int objc;
  Tcl_Obj *CONST *objv;
  int val;
};

/* These two objects hold boolean true and false,
   and are shared by all the list objects that gdb_listfuncs
   returns. */
   
static Tcl_Obj *mangled, *not_mangled;

/* These two control how the GUI behaves when gdb is either tracing or loading.
   They are used in this file & gdbtk_hooks.c */

int No_Update = 0;
int load_in_progress = 0;

/*
 * This is used in the register fetching routines
 */

#ifndef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(x) (0 != 0)
#endif

#ifndef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(x, y, z, a)
#endif

#ifndef INVALID_FLOAT
#define INVALID_FLOAT(x, y) (0 != 0)
#endif



/* This Structure is used in gdb_disassemble.
   We need a different sort of line table from the normal one cuz we can't
   depend upon implicit line-end pc's for lines to do the
   reordering in this function.  */

struct my_line_entry {
  int line;
  CORE_ADDR start_pc;
  CORE_ADDR end_pc;
};

/* This contains the previous values of the registers, since the last call to
   gdb_changed_register_list.  */

static char old_regs[REGISTER_BYTES];

/*
 * These are routines we need from breakpoint.c.
 * at some point make these static in breakpoint.c and move GUI code there
 */

extern struct breakpoint *set_raw_breakpoint (struct symtab_and_line sal);
extern void set_breakpoint_count (int);
extern int breakpoint_count;


/*
 * Declarations for routines exported from this file
 */

int Gdbtk_Init (Tcl_Interp *interp);

/*
 * Declarations for routines used only in this file.
 */

static int compare_lines PARAMS ((const PTR, const PTR));
static int comp_files PARAMS ((const void *, const void *));
static int call_wrapper PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_actions_command PARAMS ((ClientData, Tcl_Interp *, int,
					Tcl_Obj *CONST objv[]));
static int gdb_changed_register_list PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_clear_file PARAMS ((ClientData, Tcl_Interp *interp, int, Tcl_Obj *CONST []));
static int gdb_cmd PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_confirm_quit PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_disassemble PARAMS ((ClientData, Tcl_Interp *, int, 
				    Tcl_Obj *CONST []));
static int gdb_eval PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_fetch_registers PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_find_file_command PARAMS ((ClientData, Tcl_Interp *, int,
					  Tcl_Obj *CONST objv[]));
static int gdb_force_quit PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static struct symtab *full_lookup_symtab PARAMS ((char *file));
static int gdb_get_args_command PARAMS ((ClientData, Tcl_Interp *, int,
					 Tcl_Obj *CONST objv[]));
static int gdb_get_breakpoint_info PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_get_breakpoint_list PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_get_file_command PARAMS ((ClientData, Tcl_Interp *, int,
					 Tcl_Obj *CONST objv[]));
static int gdb_get_function_command PARAMS ((ClientData, Tcl_Interp *, int,
					     Tcl_Obj *CONST objv[]));
static int gdb_get_line_command PARAMS ((ClientData, Tcl_Interp *, int,
					 Tcl_Obj *CONST objv[]));
static int gdb_get_locals_command PARAMS ((ClientData, Tcl_Interp *, int,
					 Tcl_Obj *CONST objv[]));
static int gdb_get_mem PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_get_trace_frame_num PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_tracepoint_list PARAMS ((ClientData, Tcl_Interp *, int,
					    Tcl_Obj *CONST objv[]));
static int gdb_get_vars_command PARAMS ((ClientData, Tcl_Interp *, int,
					 Tcl_Obj *CONST objv[]));
static int gdb_immediate_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_listfiles PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_listfuncs PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_loadfile PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_load_info PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_loc PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_path_conv PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_prompt_command PARAMS ((ClientData, Tcl_Interp *, int,
				       Tcl_Obj *CONST objv[]));
static int gdb_regnames PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_search PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST
			       objv[]));
static int gdb_set_bp PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_set_bp_addr PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_find_bp_at_line PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_find_bp_at_addr PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_stop PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_target_has_execution_command PARAMS ((ClientData, Tcl_Interp *,
						     int,
						     Tcl_Obj *CONST []));
static int gdb_trace_status PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_tracepoint_exists_command PARAMS ((ClientData, Tcl_Interp *,
						  int,
						  Tcl_Obj *CONST objv[]));
static int gdb_get_tracepoint_info PARAMS ((ClientData, Tcl_Interp *, int,
					    Tcl_Obj *CONST objv[]));
static int gdbtk_dis_asm_read_memory PARAMS ((bfd_vma, bfd_byte *, int, disassemble_info *));
static int get_pc_register PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));
static int gdb_stack PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []));

char * get_prompt PARAMS ((void));
static void get_register PARAMS ((int, void *));
static void get_register_name PARAMS ((int, void *));
static int map_arg_registers PARAMS ((int, Tcl_Obj *CONST [], void (*) (int, void *), void *));
static int perror_with_name_wrapper PARAMS ((char *args));
static void register_changed_p PARAMS ((int, void *));
void TclDebug PARAMS ((const char *fmt, ...));
static int wrapped_call (char *opaque_args);
static void get_frame_name PARAMS ((Tcl_Interp *interp, Tcl_Obj *list, struct frame_info *fi));

/* Gdbtk_Init
 *    This loads all the Tcl commands into the Tcl interpreter.
 *
 * Arguments:
 *    interp - The interpreter into which to load the commands.
 *
 * Result:
 *     A standard Tcl result.
 */

int
Gdbtk_Init (interp)
     Tcl_Interp *interp;
{
  Tcl_CreateObjCommand (interp, "gdb_cmd", call_wrapper, gdb_cmd, NULL);
  Tcl_CreateObjCommand (interp, "gdb_immediate", call_wrapper,
			gdb_immediate_command, NULL);
  Tcl_CreateObjCommand (interp, "gdb_loc", call_wrapper, gdb_loc, NULL);
  Tcl_CreateObjCommand (interp, "gdb_path_conv", call_wrapper, gdb_path_conv, NULL);
  Tcl_CreateObjCommand (interp, "gdb_listfiles", call_wrapper, gdb_listfiles, NULL);
  Tcl_CreateObjCommand (interp, "gdb_listfuncs", call_wrapper, gdb_listfuncs,
			NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_mem", call_wrapper, gdb_get_mem,
			NULL);
  Tcl_CreateObjCommand (interp, "gdb_stop", call_wrapper, gdb_stop, NULL);
  Tcl_CreateObjCommand (interp, "gdb_regnames", call_wrapper, gdb_regnames, NULL);
  Tcl_CreateObjCommand (interp, "gdb_fetch_registers", call_wrapper,
			gdb_fetch_registers, NULL);
  Tcl_CreateObjCommand (interp, "gdb_changed_register_list", call_wrapper,
			gdb_changed_register_list, NULL);
  Tcl_CreateObjCommand (interp, "gdb_disassemble", call_wrapper,
			gdb_disassemble, NULL);
  Tcl_CreateObjCommand (interp, "gdb_eval", call_wrapper, gdb_eval, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_breakpoint_list", call_wrapper,
			gdb_get_breakpoint_list, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_breakpoint_info", call_wrapper,
			gdb_get_breakpoint_info, NULL);
  Tcl_CreateObjCommand (interp, "gdb_clear_file", call_wrapper,
			gdb_clear_file, NULL);
  Tcl_CreateObjCommand (interp, "gdb_confirm_quit", call_wrapper,
			gdb_confirm_quit, NULL);
  Tcl_CreateObjCommand (interp, "gdb_force_quit", call_wrapper,
			gdb_force_quit, NULL);
  Tcl_CreateObjCommand (interp, "gdb_target_has_execution",
			call_wrapper,
			gdb_target_has_execution_command, NULL);
  Tcl_CreateObjCommand (interp, "gdb_is_tracing",
			call_wrapper, gdb_trace_status,
			NULL);
  Tcl_CreateObjCommand (interp, "gdb_load_info", call_wrapper, gdb_load_info, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_locals", call_wrapper, gdb_get_locals_command, 
			NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_args", call_wrapper, gdb_get_args_command,
                         NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_function", call_wrapper, gdb_get_function_command,
                         NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_line", call_wrapper, gdb_get_line_command,
                         NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_file", call_wrapper, gdb_get_file_command,
                         NULL);
  Tcl_CreateObjCommand (interp, "gdb_tracepoint_exists",
                        call_wrapper, gdb_tracepoint_exists_command,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_tracepoint_info",
                        call_wrapper, gdb_get_tracepoint_info,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_actions",
                        call_wrapper, gdb_actions_command,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_prompt",
                        call_wrapper, gdb_prompt_command,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_find_file",
                        call_wrapper, gdb_find_file_command,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_tracepoint_list",
                        call_wrapper, gdb_get_tracepoint_list,  NULL);  
  Tcl_CreateObjCommand (interp, "gdb_pc_reg", call_wrapper, get_pc_register, NULL);
  Tcl_CreateObjCommand (interp, "gdb_loadfile", call_wrapper, gdb_loadfile,  NULL);
  Tcl_CreateObjCommand (gdbtk_interp, "gdb_search", call_wrapper,
			gdb_search,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_set_bp", call_wrapper, gdb_set_bp,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_set_bp_addr", call_wrapper, gdb_set_bp_addr,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_find_bp_at_line", call_wrapper, gdb_find_bp_at_line,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_find_bp_at_addr", call_wrapper, gdb_find_bp_at_addr,  NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_trace_frame_num",
                        call_wrapper, gdb_get_trace_frame_num,  NULL);  
  Tcl_CreateObjCommand (interp, "gdb_stack", call_wrapper, gdb_stack, NULL);

  Tcl_LinkVar (interp, "gdb_selected_frame_level",
               (char *) &selected_frame_level,
               TCL_LINK_INT | TCL_LINK_READ_ONLY);

  /* gdb_context is used for debugging multiple threads or tasks */
  Tcl_LinkVar (interp, "gdb_context_id",
               (char *) &gdb_context,
               TCL_LINK_INT | TCL_LINK_READ_ONLY);

  Tcl_PkgProvide(interp, "Gdbtk", GDBTK_VERSION);
  return TCL_OK;
}

/* This routine acts as a top-level for all GDB code called by Tcl/Tk.  It
   handles cleanups, and uses catch_errors to trap calls to return_to_top_level
   (usually via error).
   This is necessary in order to prevent a longjmp out of the bowels of Tk,
   possibly leaving things in a bad state.  Since this routine can be called
   recursively, it needs to save and restore the contents of the result_ptr as
   necessary. */

static int
call_wrapper (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct wrapped_call_args wrapped_args;
  gdbtk_result new_result, *old_result_ptr;
  
  old_result_ptr = result_ptr;
  result_ptr = &new_result;
  result_ptr->obj_ptr = Tcl_NewObj();
  result_ptr->flags = GDBTK_TO_RESULT;

  wrapped_args.func = (Tcl_ObjCmdProc *) clientData;
  wrapped_args.interp = interp;
  wrapped_args.objc = objc;
  wrapped_args.objv = objv;
  wrapped_args.val = TCL_OK;

  if (!catch_errors (wrapped_call, &wrapped_args, "", RETURN_MASK_ALL))
    {
      
      wrapped_args.val = TCL_ERROR;	/* Flag an error for TCL */

      /* Make sure the timer interrupts are turned off.  */

      gdbtk_stop_timer ();

      gdb_flush (gdb_stderr);	/* Flush error output */
      gdb_flush (gdb_stdout);	/* Sometimes error output comes here as well */

      /* If we errored out here, and the results were going to the
	 console, then gdbtk_fputs will have gathered the result into the
	 result_ptr.  We also need to echo them out to the console here */

      gdb_flush (gdb_stderr);	/* Flush error output */
      gdb_flush (gdb_stdout);	/* Sometimes error output comes here as well */

      /* In case of an error, we may need to force the GUI into idle
	 mode because gdbtk_call_command may have bombed out while in
	 the command routine.  */

      running_now = 0;
      Tcl_Eval (interp, "gdbtk_tcl_idle");
      
    }
  
  /* do not suppress any errors -- a remote target could have errored */
  load_in_progress = 0;

  /*
   * Now copy the result over to the true Tcl result.  If GDBTK_TO_RESULT flag
   * bit is set , this just copies a null object over to the Tcl result, which is
   * fine because we should reset the result in this case anyway.
   */
  if (result_ptr->flags & GDBTK_IN_TCL_RESULT)
    {
      Tcl_DecrRefCount(result_ptr->obj_ptr);
    }
  else
    {
      Tcl_SetObjResult (interp, result_ptr->obj_ptr);
    }

  result_ptr = old_result_ptr;

#ifdef _WIN32
  close_bfds ();
#endif

  return wrapped_args.val;
}

/*
 * This is the wrapper that is passed to catch_errors.
 */

static int
wrapped_call (opaque_args)
     char *opaque_args;
{
  struct wrapped_call_args *args = (struct wrapped_call_args *) opaque_args;
  args->val = (*args->func) (args->func, args->interp, args->objc, args->objv);
  return 1;
}

/* This is a convenience function to sprintf something(s) into a
 * new element in a Tcl list object.
 */

static void
#ifdef ANSI_PROTOTYPES
sprintf_append_element_to_obj (Tcl_Obj *objp, char *format, ...)
#else
sprintf_append_element_to_obj (va_alist)
     va_dcl
#endif
{
  va_list args;
  char buf[1024];
  
#ifdef ANSI_PROTOTYPES
  va_start (args, format);
#else
  Tcl_Obj *objp;
  char *format;

  va_start (args);
  dsp = va_arg (args, Tcl_Obj *);
  format = va_arg (args, char *);
#endif

  vsprintf (buf, format, args);

  Tcl_ListObjAppendElement (NULL, objp, Tcl_NewStringObj (buf, -1));
}

/*
 * This section contains the commands that control execution.
 */

/* This implements the tcl command gdb_clear_file.
 *
 * Prepare to accept a new executable file.  This is called when we
 * want to clear away everything we know about the old file, without
 * asking the user.  The Tcl code will have already asked the user if
 * necessary.  After this is called, we should be able to run the
 * `file' command without getting any questions.  
 *
 * Arguments:
 *    None
 * Tcl Result:
 *    None
 */

static int
gdb_clear_file (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  if (objc != 1)
    Tcl_SetStringObj (result_ptr->obj_ptr,
		      "Wrong number of args, none are allowed.", -1);
  
  if (inferior_pid != 0 && target_has_execution)
    {
      if (attach_flag)
        target_detach (NULL, 0);
      else
        target_kill ();
    }

  if (target_has_execution)
    pop_target ();

  symbol_file_command (NULL, 0);

  /* gdb_loc refers to stop_pc, but nothing seems to clear it, so we
     clear it here.  FIXME: This seems like an abstraction violation
     somewhere.  */
  stop_pc = 0;

  return TCL_OK;
}

/* This implements the tcl command gdb_confirm_quit
 * Ask the user to confirm an exit request.
 *
 * Arguments:
 *    None
 * Tcl Result:
 *    A boolean, 1 if the user answered yes, 0 if no.
 */

static int
gdb_confirm_quit (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int ret;

  if (objc != 1)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Wrong number of args, should be none.", -1);
      return TCL_ERROR;
    }
  
  ret = quit_confirm ();
  Tcl_SetBooleanObj (result_ptr->obj_ptr, ret);
  return TCL_OK;
}

/* This implements the tcl command gdb_force_quit
 * Quit without asking for confirmation.
 *
 * Arguments:
 *    None
 * Tcl Result:
 *    None
 */

static int
gdb_force_quit (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  if (objc != 1)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Wrong number of args, should be none.", -1);
      return TCL_ERROR;
    }
  
  quit_force ((char *) NULL, 1);
  return TCL_OK;
}

/* This implements the tcl command gdb_stop
 * It stops the target in a continuable fashion.
 *
 * Arguments:
 *    None
 * Tcl Result:
 *    None
 */

static int
gdb_stop (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  if (target_stop != target_ignore)
      target_stop ();
  else
    quit_flag = 1; /* hope something sees this */

  return TCL_OK;
}


/*
 * This section contains Tcl commands that are wrappers for invoking
 * the GDB command interpreter.
 */


/* This implements the tcl command `gdb_eval'.
 * It uses the gdb evaluator to return the value of
 * an expression in the current language
 *
 * Tcl Arguments:
 *     expression - the expression to evaluate.
 * Tcl Result:
 *     The result of the evaluation.
 */

static int
gdb_eval (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct expression *expr;
  struct cleanup *old_chain=NULL;
  value_ptr val;

  if (objc != 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr,
			"wrong # args, should be \"gdb_eval expression\"", -1);
      return TCL_ERROR;
    }

  expr = parse_expression (Tcl_GetStringFromObj (objv[1], NULL));

  old_chain = make_cleanup (free_current_contents, &expr);

  val = evaluate_expression (expr);

  /*
   * Print the result of the expression evaluation.  This will go to
   * eventually go to gdbtk_fputs, and from there be collected into
   * the Tcl result.
   */
  
  val_print (VALUE_TYPE (val), VALUE_CONTENTS (val), VALUE_ADDRESS (val),
	     gdb_stdout, 0, 0, 0, 0);

  do_cleanups (old_chain);

  return TCL_OK;
}

/* This implements the tcl command "gdb_cmd".
 *  
 * It sends its argument to the GDB command scanner for execution. 
 * This command will never cause the update, idle and busy hooks to be called
 * within the GUI.
 * 
 * Tcl Arguments:
 *    command - The GDB command to execute
 * Tcl Result:
 *    The output from the gdb command (except for the "load" & "while"
 *    which dump their output to the console.
 */

static int
gdb_cmd (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{

  if (objc < 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong # args", -1);
      return TCL_ERROR;
    }

  if (running_now || load_in_progress)
    return TCL_OK;

  No_Update = 1;

  /* for the load instruction (and possibly others later) we
     set turn off the GDBTK_TO_RESULT flag bit so gdbtk_fputs() 
     will not buffer all the data until the command is finished. */

  if ((strncmp ("load ", Tcl_GetStringFromObj (objv[1], NULL), 5) == 0))
    {
      result_ptr->flags &= ~GDBTK_TO_RESULT;
      load_in_progress = 1;
    }

  execute_command (Tcl_GetStringFromObj (objv[1], NULL), 1);

  if (load_in_progress)
    {
      load_in_progress = 0;
      result_ptr->flags |= GDBTK_TO_RESULT;
    }

  bpstat_do_actions (&stop_bpstat);
  
  return TCL_OK;
}

/*
 * This implements the tcl command "gdb_immediate"
 *  
 * It does exactly the same thing as gdb_cmd, except NONE of its outut 
 * is buffered.  This will also ALWAYS cause the busy, update, and idle hooks to
 * be called, contrasted with gdb_cmd, which NEVER calls them.
 * It turns off the GDBTK_TO_RESULT flag, which diverts the result
 * to the console window.
 *
 * Tcl Arguments:
 *    command - The GDB command to execute
 * Tcl Result:
 *    None.
 */

static int
gdb_immediate_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{

  if (objc != 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong # args", -1);
      return TCL_ERROR;
    }

  if (running_now || load_in_progress)
    return TCL_OK;

  No_Update = 0;

  result_ptr->flags &= ~GDBTK_TO_RESULT;  

  execute_command (Tcl_GetStringFromObj (objv[1], NULL), 1);

  bpstat_do_actions (&stop_bpstat);
  
  result_ptr->flags |= GDBTK_TO_RESULT;

  return TCL_OK;
}

/* This implements the tcl command "gdb_prompt"
 *
 * It returns the gdb interpreter's prompt.
 *
 * Tcl Arguments:
 *    None.
 * Tcl Result:
 *    The prompt.
 */

static int
gdb_prompt_command (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  Tcl_SetStringObj (result_ptr->obj_ptr, get_prompt (), -1);
  return TCL_OK;
}


/*
 * This section contains general informational commands.
 */

/* This implements the tcl command "gdb_target_has_execution"
 *
 * Tells whether the target is executing.
 *
 * Tcl Arguments:
 *    None
 * Tcl Result:
 *    A boolean indicating whether the target is executing.
 */

static int
gdb_target_has_execution_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int result = 0;

  if (target_has_execution && inferior_pid != 0)
    result = 1;

  Tcl_SetBooleanObj (result_ptr->obj_ptr, result);
  return TCL_OK;
}

/* This implements the tcl command "gdb_load_info"
 *
 * It returns information about the file about to be downloaded.
 *
 * Tcl Arguments:
 *    filename: The file to open & get the info on.
 * Tcl Result:
 *    A list consisting of the name and size of each section.
 */

static int
gdb_load_info (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
   bfd *loadfile_bfd;
   struct cleanup *old_cleanups;
   asection *s;
   Tcl_Obj *ob[2];

   char *filename = Tcl_GetStringFromObj (objv[1], NULL);

   loadfile_bfd = bfd_openr (filename, gnutarget);
   if (loadfile_bfd == NULL)
     {
       Tcl_SetStringObj (result_ptr->obj_ptr, "Open failed", -1);
       return TCL_ERROR;
     }
   old_cleanups = make_cleanup (bfd_close, loadfile_bfd);
   
   if (!bfd_check_format (loadfile_bfd, bfd_object)) 
     {
       Tcl_SetStringObj (result_ptr->obj_ptr, "Bad Object File", -1);
       return TCL_ERROR;
    }

   Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
   
   for (s = loadfile_bfd->sections; s; s = s->next) 
     {
       if (s->flags & SEC_LOAD) 
         {
           bfd_size_type size = bfd_get_section_size_before_reloc (s);
           if (size > 0)
             {
               ob[0] = Tcl_NewStringObj ((char *) bfd_get_section_name (loadfile_bfd, s), -1);
               ob[1] = Tcl_NewLongObj ((long) size);
               Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewListObj (2, ob));
             }
         }
     }
   
   do_cleanups (old_cleanups);
   return TCL_OK;
}


/* gdb_get_locals -
 * This and gdb_get_locals just call gdb_get_vars_command with the right
 * value of clientData.  We can't use the client data in the definition
 * of the command, because the call wrapper uses this instead...
 */

static int
gdb_get_locals_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{

  return gdb_get_vars_command((ClientData) 0, interp, objc, objv);

}

static int
gdb_get_args_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{

  return gdb_get_vars_command((ClientData) 1, interp, objc, objv);

}

/* This implements the tcl commands "gdb_get_locals" and "gdb_get_args"
 *  
 * This function sets the Tcl interpreter's result to a list of variable names
 * depending on clientData. If clientData is one, the result is a list of
 * arguments; zero returns a list of locals -- all relative to the block
 * specified as an argument to the command. Valid commands include
 * anything decode_line_1 can handle (like "main.c:2", "*0x02020202",
 * and "main").
 *
 * Tcl Arguments:
 *   block - the address within which to specify the locals or args.
 * Tcl Result:
 *   A list of the locals or args
 */

static int
gdb_get_vars_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symtabs_and_lines sals;
  struct symbol *sym;
  struct block *block;
  char **canonical, *args;
  int i, nsyms, arguments;

  if (objc != 2)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr,
                        "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " function:line|function|line|*addr\"", NULL);
      return TCL_ERROR;
    }

  arguments = (int) clientData;
  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);
  if (sals.nelts == 0)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr,
                        "error decoding line", -1);
      return TCL_ERROR;
    }

  /* Initialize the result pointer to an empty list. */

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);

  /* Resolve all line numbers to PC's */
  for (i = 0; i < sals.nelts; i++)
    resolve_sal_pc (&sals.sals[i]);
  
  block = block_for_pc (sals.sals[0].pc);
  while (block != 0)
    {
      nsyms = BLOCK_NSYMS (block);
      for (i = 0; i < nsyms; i++)
        {
          sym = BLOCK_SYM (block, i);
          switch (SYMBOL_CLASS (sym)) {
          default:
          case LOC_UNDEF:		  /* catches errors        */
          case LOC_CONST:	      /* constant              */
          case LOC_TYPEDEF:	      /* local typedef         */
          case LOC_LABEL:	      /* local label           */
          case LOC_BLOCK:	      /* local function        */
          case LOC_CONST_BYTES:	  /* loc. byte seq.        */
          case LOC_UNRESOLVED:    /* unresolved static     */
          case LOC_OPTIMIZED_OUT: /* optimized out         */
            break;
          case LOC_ARG:		      /* argument              */
          case LOC_REF_ARG:	      /* reference arg         */
          case LOC_REGPARM:	      /* register arg          */
          case LOC_REGPARM_ADDR:  /* indirect register arg */
          case LOC_LOCAL_ARG:	  /* stack arg             */
          case LOC_BASEREG_ARG:	  /* basereg arg           */
            if (arguments)
              Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
                                        Tcl_NewStringObj (SYMBOL_NAME (sym), -1));
            break;
          case LOC_LOCAL:	      /* stack local           */
          case LOC_BASEREG:	      /* basereg local         */
          case LOC_STATIC:	      /* static                */
          case LOC_REGISTER:      /* register              */
            if (!arguments)
              Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
                                        Tcl_NewStringObj (SYMBOL_NAME (sym), -1));
            break;
          }
        }
      if (BLOCK_FUNCTION (block))
        break;
      else
        block = BLOCK_SUPERBLOCK (block);
    }
  
  return TCL_OK;
}

/* This implements the tcl command "gdb_get_line"
 *
 * It returns the linenumber for a given linespec.  It will take any spec
 * that can be passed to decode_line_1
 *
 * Tcl Arguments:
 *    linespec - the line specification
 * Tcl Result:
 *    The line number for that spec.
 */
static int
gdb_get_line_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symtabs_and_lines sals;
  char *args, **canonical;
  
  if (objc != 2)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"", NULL);
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      Tcl_SetIntObj (result_ptr->obj_ptr, sals.sals[0].line);
      return TCL_OK;
    }

  Tcl_SetStringObj (result_ptr->obj_ptr, "N/A", -1);
  return TCL_OK;
  
}

/* This implements the tcl command "gdb_get_file"
 *
 * It returns the file containing a given line spec.
 *
 * Tcl Arguments:
 *    linespec - The linespec to look up
 * Tcl Result:
 *    The file containing it.
 */

static int
gdb_get_file_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symtabs_and_lines sals;
  char *args, **canonical;
  
  if (objc != 2)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"", NULL);
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, sals.sals[0].symtab->filename, -1);
      return TCL_OK;
    }

  Tcl_SetStringObj (result_ptr->obj_ptr, "N/A", -1);
  return TCL_OK;
}

/* This implements the tcl command "gdb_get_function"
 *
 * It finds the function containing the given line spec.
 *
 * Tcl Arguments:
 *    linespec - The line specification
 * Tcl Result:
 *    The function that contains it, or "N/A" if it is not in a function.
 */
static int
gdb_get_function_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  char *function;
  struct symtabs_and_lines sals;
  char *args, **canonical;

  if (objc != 2)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"", NULL);
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      resolve_sal_pc (&sals.sals[0]);
      find_pc_partial_function (sals.sals[0].pc, &function, NULL, NULL);
      if (function != NULL)
        {
          Tcl_SetStringObj (result_ptr->obj_ptr, function, -1);
          return TCL_OK;
        }
    }
  
  Tcl_SetStringObj (result_ptr->obj_ptr, "N/A", -1);
  return TCL_OK;
}

/* This implements the tcl command "gdb_find_file"
 *
 * It searches the symbol tables to get the full pathname to a file.
 *
 * Tcl Arguments:
 *    filename: the file name to search for.
 * Tcl Result:
 *    The full path to the file, or an empty string if the file is not
 *    found.
 */

static int
gdb_find_file_command (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  char *filename = NULL;
  struct symtab *st;

  if (objc != 2)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "filename");
      return TCL_ERROR;
    }

  st = full_lookup_symtab (Tcl_GetStringFromObj (objv[1], NULL));
  if (st)
    filename = st->fullname;

  if (filename == NULL)
    Tcl_SetStringObj (result_ptr->obj_ptr, "", 0);
  else
    Tcl_SetStringObj (result_ptr->obj_ptr, filename, -1);

  return TCL_OK;
}

/* This implements the tcl command "gdb_listfiles"
 *
 * This lists all the files in the current executible.
 *
 * Note that this currently pulls in all sorts of filenames
 * that aren't really part of the executable.  It would be
 * best if we could check each file to see if it actually
 * contains executable lines of code, but we can't do that
 * with psymtabs.
 *
 * Arguments:
 *    ?pathname? - If provided, only files which match pathname
 *        (up to strlen(pathname)) are included. THIS DOES NOT
 *        CURRENTLY WORK BECAUSE PARTIAL_SYMTABS DON'T SUPPLY
 *        THE FULL PATHNAME!!!
 *
 * Tcl Result:
 *    A list of all matching files.
 */
static int
gdb_listfiles (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  struct objfile *objfile;
  struct partial_symtab *psymtab;
  struct symtab *symtab;
  char *lastfile, *pathname=NULL, **files;
  int files_size;
  int i, numfiles = 0, len = 0;
  
  files_size = 1000;
  files = (char **) xmalloc (sizeof (char *) * files_size);
  
  if (objc > 2)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "Usage: gdb_listfiles ?pathname?");
      return TCL_ERROR;
    }
  else if (objc == 2)
    pathname = Tcl_GetStringFromObj (objv[1], &len);

  ALL_PSYMTABS (objfile, psymtab)
    {
      if (numfiles == files_size)
        {
          files_size = files_size * 2;
          files = (char **) xrealloc (files, sizeof (char *) * files_size);
        }
      if (psymtab->filename)
        {
          if (!len || !strncmp(pathname, psymtab->filename,len)
              || !strcmp(psymtab->filename, basename(psymtab->filename)))
            {
              files[numfiles++] = basename(psymtab->filename);
            }
        }
    }

  ALL_SYMTABS (objfile, symtab)
    {
      if (numfiles == files_size)
        {
          files_size = files_size * 2;
          files = (char **) xrealloc (files, sizeof (char *) * files_size);
        }
      if (symtab->filename && symtab->linetable && symtab->linetable->nitems)
        {
          if (!len || !strncmp(pathname, symtab->filename,len)
              || !strcmp(symtab->filename, basename(symtab->filename)))
            {
              files[numfiles++] = basename(symtab->filename);
            }
        }
    }

  qsort (files, numfiles, sizeof(char *), comp_files);

  lastfile = "";

  /* Discard the old result pointer, in case it has accumulated anything
     and set it to a new list object */
  
  Tcl_SetListObj(result_ptr->obj_ptr, 0, NULL);
  
  for (i = 0; i < numfiles; i++)
    {
      if (strcmp(files[i],lastfile))
        Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewStringObj(files[i], -1));
      lastfile = files[i];
    }

  free (files);
  return TCL_OK;
}

static int
comp_files (file1, file2)
     const void *file1, *file2;
{
  return strcmp(* (char **) file1, * (char **) file2);
}


/* This implements the tcl command "gdb_search"
 *
 *
 * Tcl Arguments:
 *    option - One of "functions", "variables" or "types"
 *    regexp - The regular expression to look for.
 * Then, optionally:
 *    -files fileList
 *    -static 1/0
 * Tcl Result:
 *
 */

static int
gdb_search (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symbol_search *ss = NULL;
  struct symbol_search *p;
  struct cleanup *old_chain = NULL;
  Tcl_Obj *CONST *switch_objv;
  int index, switch_objc, i;
  namespace_enum space = 0;
  char *regexp;
  int static_only, nfiles;
  Tcl_Obj **file_list;
  char **files;
  static char *search_options[] = { "functions", "variables", "types", (char *) NULL };
  static char *switches[] = { "-files", "-static", (char *) NULL };
  enum search_opts { SEARCH_FUNCTIONS, SEARCH_VARIABLES, SEARCH_TYPES };
  enum switches_opts { SWITCH_FILES, SWITCH_STATIC_ONLY };

  if (objc < 3)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "option regexp ?arg ...?");
          result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (Tcl_GetIndexFromObj (interp, objv[1], search_options, "option", 0,
                           &index) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  /* Unfortunately, we cannot teach search_symbols to search on
     multiple regexps, so we have to do a two-tier search for
     any searches which choose to narrow the playing field. */
  switch ((enum search_opts) index)
    {
    case SEARCH_FUNCTIONS:
      space = FUNCTIONS_NAMESPACE;      break;
    case SEARCH_VARIABLES:
      space = VARIABLES_NAMESPACE;      break;
    case SEARCH_TYPES:
      space = TYPES_NAMESPACE;          break;
    }

  regexp = Tcl_GetStringFromObj (objv[2], NULL);
  /* Process any switches that refine the search */
  switch_objc = objc - 3;
  switch_objv = objv + 3;

  static_only = 0;
  nfiles = 0;
  files = (char **) NULL;
  while (switch_objc > 0)
    {
      if (Tcl_GetIndexFromObj (interp, switch_objv[0], switches,
                               "option", 0, &index) != TCL_OK)
        {
          result_ptr->flags |= GDBTK_IN_TCL_RESULT;
          return TCL_ERROR;
        }

      switch ((enum switches_opts) index)
        {
        case SWITCH_FILES:
          {
            int result;
            if (switch_objc < 2)
              {
                Tcl_WrongNumArgs (interp, 2, objv, "[-files fileList -static 1|0]");
                result_ptr->flags |= GDBTK_IN_TCL_RESULT;
                return TCL_ERROR;
              }
            result = Tcl_ListObjGetElements (interp, switch_objv[1], &nfiles, &file_list);
            if (result != TCL_OK)
              return result;

            files = (char **) xmalloc (nfiles * sizeof (char *));
            for (i = 0; i < nfiles; i++)
              files[i] = Tcl_GetStringFromObj (file_list[i], NULL);
            switch_objc--;
            switch_objv++;
          }
          break;
        case SWITCH_STATIC_ONLY:
          if (switch_objc < 2)
            {
              Tcl_WrongNumArgs (interp, 2, objv, "[-files fileList] [-static 1|0]");
              result_ptr->flags |= GDBTK_IN_TCL_RESULT;
              return TCL_ERROR;
            }              
          if ( Tcl_GetBooleanFromObj (interp, switch_objv[1], &static_only) !=
               TCL_OK) {
            result_ptr->flags |= GDBTK_IN_TCL_RESULT;
            return TCL_ERROR;
          }
          switch_objc--;
          switch_objv++;
        }
      switch_objc--;
      switch_objv++;
    }

  search_symbols (regexp, space, nfiles, files, &ss);
  if (ss != NULL)
    old_chain = make_cleanup (free_search_symbols, ss);

  Tcl_SetListObj(result_ptr->obj_ptr, 0, NULL);  

  for (p = ss; p != NULL; p = p->next)
    {
      Tcl_Obj *elem;

      if (static_only && p->block != STATIC_BLOCK)
        continue;

      elem = Tcl_NewListObj (0, NULL);

      if (p->msymbol == NULL)
        Tcl_ListObjAppendElement (interp, elem, 
                                  Tcl_NewStringObj (SYMBOL_SOURCE_NAME (p->symbol), -1));
      else
        Tcl_ListObjAppendElement (interp, elem,
                                  Tcl_NewStringObj (SYMBOL_SOURCE_NAME (p->msymbol), -1));

      Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, elem);
    }
  
  if (ss != NULL)
    do_cleanups (old_chain);

  return TCL_OK;
}

/* This implements the tcl command gdb_listfuncs
 *  
 * It lists all the functions defined in a given file
 * 
 * Arguments:
 *    file - the file to look in
 * Tcl Result:
 *    A list of two element lists, the first element is
 *    the symbol name, and the second is a boolean indicating
 *    whether the symbol is demangled (1 for yes).
 */

static int
gdb_listfuncs (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symtab *symtab;
  struct blockvector *bv;
  struct block *b;
  struct symbol *sym;
  int i,j;
  Tcl_Obj *funcVals[2];

  if (objc != 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong # args", -1);
    }
  
  symtab = full_lookup_symtab (Tcl_GetStringFromObj (objv[1], NULL));
  if (!symtab)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "No such file", -1);
      return TCL_ERROR;
    }

  if (mangled == NULL)
    {
      mangled = Tcl_NewBooleanObj(1);
      not_mangled = Tcl_NewBooleanObj(0);
      Tcl_IncrRefCount(mangled);
      Tcl_IncrRefCount(not_mangled);
    }

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  
  bv = BLOCKVECTOR (symtab);
  for (i = GLOBAL_BLOCK; i <= STATIC_BLOCK; i++)
    {
      b = BLOCKVECTOR_BLOCK (bv, i);
      /* Skip the sort if this block is always sorted.  */
      if (!BLOCK_SHOULD_SORT (b))
        sort_block_syms (b);
      for (j = 0; j < BLOCK_NSYMS (b); j++)
        {
          sym = BLOCK_SYM (b, j);
          if (SYMBOL_CLASS (sym) == LOC_BLOCK)
            {
	      
              char *name = cplus_demangle (SYMBOL_NAME(sym), 0);
              if (name)
                {
                  /* strip out "global constructors" and "global destructors" */
                  /* because we aren't interested in them. */
                  if (strncmp (name, "global ", 7))
                    {
                      funcVals[0] = Tcl_NewStringObj(name, -1);
                      funcVals[1] = mangled;	  
                    }
                  else
                    continue;

                }
              else
                {
                  funcVals[0] = Tcl_NewStringObj(SYMBOL_NAME(sym), -1);
                  funcVals[1] = not_mangled;
                }
              Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                                        Tcl_NewListObj (2, funcVals));
            }
        }
    }
  return TCL_OK;
}


/*
 * This section contains all the commands that act on the registers:
 */

/* This is a sort of mapcar function for operations on registers */ 
	    
static int
map_arg_registers (objc, objv, func, argp)
     int objc;
     Tcl_Obj *CONST objv[];
     void (*func) PARAMS ((int regnum, void *argp));
     void *argp;
{
  int regnum;

  /* Note that the test for a valid register must include checking the
     reg_names array because NUM_REGS may be allocated for the union of the
     register sets within a family of related processors.  In this case, the
     trailing entries of reg_names will change depending upon the particular
     processor being debugged.  */

  if (objc == 0)		/* No args, just do all the regs */
    {
      for (regnum = 0;
           regnum < NUM_REGS
             && reg_names[regnum] != NULL
             && *reg_names[regnum] != '\000';
           regnum++)
        func (regnum, argp);

      return TCL_OK;
    }

  /* Else, list of register #s, just do listed regs */
  for (; objc > 0; objc--, objv++)
    {
      if (Tcl_GetIntFromObj (NULL, *objv, &regnum) != TCL_OK)
        {
          result_ptr->flags |= GDBTK_IN_TCL_RESULT;
          return TCL_ERROR;
        }

      if (regnum >= 0
          && regnum < NUM_REGS
          && reg_names[regnum] != NULL
          && *reg_names[regnum] != '\000')
        func (regnum, argp);
      else
        {
          Tcl_SetStringObj (result_ptr->obj_ptr, "bad register number", -1);
          return TCL_ERROR;
        }
    }

  return TCL_OK;
}
	    
/* This implements the TCL command `gdb_regnames', which returns a list of
   all of the register names. */

static int
gdb_regnames (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  objc--;
  objv++;

  return map_arg_registers (objc, objv, get_register_name, NULL);
}

static void
get_register_name (regnum, argp)
     int regnum;
     void *argp;		/* Ignored */
{
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
                            Tcl_NewStringObj (reg_names[regnum], -1));
}

/* This implements the tcl command gdb_fetch_registers
 * Pass it a list of register names, and it will
 * return their values as a list.
 *
 * Tcl Arguments:
 *    format: The format string for printing the values
 *    args: the registers to look for
 * Tcl Result:
 *    A list of their values.
 */

static int
gdb_fetch_registers (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int format, result;

  if (objc < 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr,
                        "wrong # args, should be gdb_fetch_registers format ?register1 register2 ...?", -1);
    }
  objc -= 2;
  objv++;
  format = *(Tcl_GetStringFromObj(objv[0], NULL));  
  objv++;
  
  
  result_ptr->flags |= GDBTK_MAKES_LIST; /* Output the results as a list */
  result = map_arg_registers (objc, objv, get_register, (void *) format);
  result_ptr->flags &= ~GDBTK_MAKES_LIST;
  
  return result; 
}

static void
get_register (regnum, fp)
     int regnum;
     void *fp;
{
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
  int format = (int)fp;

  if (format == 'N')
    format = 0;

  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    {
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				Tcl_NewStringObj ("Optimized out", -1));
      return;
    }

  /* Convert raw data to virtual format if necessary.  */

  if (REGISTER_CONVERTIBLE (regnum))
    {
      REGISTER_CONVERT_TO_VIRTUAL (regnum, REGISTER_VIRTUAL_TYPE (regnum),
                                   raw_buffer, virtual_buffer);
    }
  else
    memcpy (virtual_buffer, raw_buffer, REGISTER_VIRTUAL_SIZE (regnum));

  if (format == 'r')
    {
      int j;
      printf_filtered ("0x");
      for (j = 0; j < REGISTER_RAW_SIZE (regnum); j++)
        {
          register int idx = TARGET_BYTE_ORDER == BIG_ENDIAN ? j
            : REGISTER_RAW_SIZE (regnum) - 1 - j;
          printf_filtered ("%02x", (unsigned char)raw_buffer[idx]);
        }
    }
  else
    val_print (REGISTER_VIRTUAL_TYPE (regnum), virtual_buffer, 0,
               gdb_stdout, format, 1, 0, Val_pretty_default);

}

/* This implements the tcl command get_pc_reg
 * It returns the value of the PC register
 *
 * Tcl Arguments:
 *    None
 * Tcl Result:
 *    The value of the pc register.
 */

static int
get_pc_register (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  char buff[64];
  
  sprintf (buff, "0x%llx",(long long) read_register (PC_REGNUM));
  Tcl_SetStringObj(result_ptr->obj_ptr, buff, -1);
  return TCL_OK;
}

/* This implements the tcl command "gdb_changed_register_list"
 * It takes a list of registers, and returns a list of
 * the registers on that list that have changed since the last
 * time the proc was called.
 *
 * Tcl Arguments:
 *    A list of registers.
 * Tcl Result:
 *    A list of changed registers.
 */

static int
gdb_changed_register_list (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  objc--;
  objv++;

  return map_arg_registers (objc, objv, register_changed_p, NULL);
}

static void
register_changed_p (regnum, argp)
     int regnum;
     void *argp;		/* Ignored */
{
  char raw_buffer[MAX_REGISTER_RAW_SIZE];

  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    return;

  if (memcmp (&old_regs[REGISTER_BYTE (regnum)], raw_buffer,
              REGISTER_RAW_SIZE (regnum)) == 0)
    return;

  /* Found a changed register.  Save new value and return its number. */

  memcpy (&old_regs[REGISTER_BYTE (regnum)], raw_buffer,
          REGISTER_RAW_SIZE (regnum));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj(regnum));
}

/*
 * This section contains the commands that deal with tracepoints:
 */

/* return a list of all tracepoint numbers in interpreter */
static int
gdb_get_tracepoint_list (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  struct tracepoint *tp;

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);

  ALL_TRACEPOINTS (tp)
    Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->number));

  return TCL_OK;
}

/* returns -1 if not found, tracepoint # if found */
int
tracepoint_exists (char * args)
{
  struct tracepoint *tp;
  char **canonical;
  struct symtabs_and_lines sals;
  char  *file = NULL;
  int    result = -1;

  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      resolve_sal_pc (&sals.sals[0]);
      file = xmalloc (strlen (sals.sals[0].symtab->dirname)
                      + strlen (sals.sals[0].symtab->filename) + 1);
      if (file != NULL)
        {
          strcpy (file, sals.sals[0].symtab->dirname);
          strcat (file, sals.sals[0].symtab->filename);

          ALL_TRACEPOINTS (tp)
            {
              if (tp->address == sals.sals[0].pc)
                result = tp->number;
#if 0
              /* Why is this here? This messes up assembly traces */
              else if (tp->source_file != NULL
                       && strcmp (tp->source_file, file) == 0
                       && sals.sals[0].line == tp->line_number)
                result = tp->number;
#endif                
            }
        }
    }
  if (file != NULL)
    free (file);
  return result;
}

static int
gdb_tracepoint_exists_command (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  char * args;

  if (objc != 2)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " function:line|function|line|*addr\"", NULL);
      return TCL_ERROR;
    }
  
  args = Tcl_GetStringFromObj (objv[1], NULL);
  
  Tcl_SetIntObj (result_ptr->obj_ptr, tracepoint_exists (args));
  return TCL_OK;
}

static int
gdb_get_tracepoint_info (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj  *CONST objv[];
{
  struct symtab_and_line sal;
  int tpnum;
  struct tracepoint *tp;
  struct action_line *al;
  Tcl_Obj *action_list;
  char *filename, *funcname;
  char tmp[19];
  
  if (objc != 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong # args", -1);
      return TCL_ERROR;
    }
  
  if (Tcl_GetIntFromObj (NULL, objv[1], &tpnum) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  ALL_TRACEPOINTS (tp)
    if (tp->number == tpnum)
      break;

  if (tp == NULL)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Tracepoint #%d does not exist", -1);
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);
  sal = find_pc_line (tp->address, 0);
  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "N/A";
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr,
                            Tcl_NewStringObj (filename, -1));
  find_pc_partial_function (tp->address, &funcname, NULL, NULL);
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewStringObj (funcname, -1));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (sal.line));
  sprintf (tmp, "0x%lx", tp->address);
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewStringObj (tmp, -1));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->enabled));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->pass_count));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->step_count));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->thread));
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, Tcl_NewIntObj (tp->hit_count));

  /* Append a list of actions */
  action_list = Tcl_NewObj ();
  for (al = tp->actions; al != NULL; al = al->next)
    {
      Tcl_ListObjAppendElement (interp, action_list,
                                Tcl_NewStringObj (al->action, -1));
    }
  Tcl_ListObjAppendElement (interp, result_ptr->obj_ptr, action_list);

  return TCL_OK;
}


static int
gdb_trace_status (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int result = 0;
 
  if (trace_running_p)
    result = 1;
 
  Tcl_SetIntObj (result_ptr->obj_ptr, result);
  return TCL_OK;
}



static int
gdb_get_trace_frame_num (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  if (objc != 1)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"", NULL);
      return TCL_ERROR;
    }
 
  Tcl_SetIntObj (result_ptr->obj_ptr, get_traceframe_number ());
  return TCL_OK;
  
}
 
/* This implements the tcl command gdb_actions
 * It sets actions for a given tracepoint.
 *
 * Tcl Arguments:
 *    number: the tracepoint in question
 *    actions: the actions to add to this tracepoint
 * Tcl Result:
 *    None.
 */

static int
gdb_actions_command (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  struct tracepoint *tp;
  Tcl_Obj **actions;
  int      nactions, i, len;
  char *number, *args, *action;
  long step_count;
  struct action_line *next = NULL, *temp;
  enum actionline_type linetype;

  if (objc != 3)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "wrong # args: should be: \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " number actions\"", NULL);
      return TCL_ERROR;
    }

  args = number = Tcl_GetStringFromObj (objv[1], NULL);
  tp = get_tracepoint_by_number (&args);
  if (tp == NULL)
    {
      Tcl_AppendStringsToObj (result_ptr->obj_ptr, "Tracepoint \"", number, "\" does not exist", NULL);
      return TCL_ERROR;
    }

  /* Free any existing actions */
  if (tp->actions != NULL)
    free_actions (tp);

  step_count = 0;

  Tcl_ListObjGetElements (interp, objv[2], &nactions, &actions);

  /* Add the actions to the tracepoint */
  for (i = 0; i < nactions; i++)
    {
      temp = xmalloc (sizeof (struct action_line));
      temp->next = NULL;
      action = Tcl_GetStringFromObj (actions[i], &len);
      temp->action = savestring (action, len);

      linetype = validate_actionline (&(temp->action), tp);

      if (linetype == BADLINE) 
       {
         free (temp);
         continue;
       }

      if (next == NULL)
        {
          tp->actions = temp;
          next = temp;
        }
      else
        {
          next->next = temp;
          next = temp;
        }
    }
  
  return TCL_OK;
}

/*
 * This section has commands that handle source disassembly.
 */

/* This implements the tcl command gdb_disassemble
 *
 * Arguments:
 *    source_with_assm - must be "source" or "nosource"
 *    low_address - the address from which to start disassembly
 *    ?hi_address? - the address to which to disassemble, defaults
 *                   to the end of the function containing low_address.
 * Tcl Result:
 *    The disassembled code is passed to fputs_unfiltered, so it
 *    either goes to the console if result_ptr->obj_ptr is NULL or to
 *    the Tcl result.
 */

static int
gdb_disassemble (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  CORE_ADDR pc, low, high;
  int mixed_source_and_assembly;
  static disassemble_info di;
  static int di_initialized;
  char *arg_ptr;

  if (objc != 3 && objc != 4)
    error ("wrong # args");

  if (! di_initialized)
    {
      INIT_DISASSEMBLE_INFO_NO_ARCH (di, gdb_stdout,
                                     (fprintf_ftype) fprintf_unfiltered);
      di.flavour = bfd_target_unknown_flavour;
      di.memory_error_func = dis_asm_memory_error;
      di.print_address_func = dis_asm_print_address;
      di_initialized = 1;
    }

  di.mach = tm_print_insn_info.mach;
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    di.endian = BFD_ENDIAN_BIG;
  else
    di.endian = BFD_ENDIAN_LITTLE;

  arg_ptr = Tcl_GetStringFromObj (objv[1], NULL);
  if (*arg_ptr == 's' && strcmp (arg_ptr, "source") == 0)
    mixed_source_and_assembly = 1;
  else if (*arg_ptr == 'n' && strcmp (arg_ptr, "nosource") == 0)
    mixed_source_and_assembly = 0;
  else
    error ("First arg must be 'source' or 'nosource'");

  low = parse_and_eval_address (Tcl_GetStringFromObj (objv[2], NULL));

  if (objc == 3)
    {
      if (find_pc_partial_function (low, NULL, &low, &high) == 0)
        error ("No function contains specified address");
    }
  else
    high = parse_and_eval_address (Tcl_GetStringFromObj (objv[3], NULL));

  /* If disassemble_from_exec == -1, then we use the following heuristic to
     determine whether or not to do disassembly from target memory or from the
     exec file:

     If we're debugging a local process, read target memory, instead of the
     exec file.  This makes disassembly of functions in shared libs work
     correctly.

     Else, we're debugging a remote process, and should disassemble from the
     exec file for speed.  However, this is no good if the target modifies its
     code (for relocation, or whatever).
   */

  if (disassemble_from_exec == -1)
    {
      if (strcmp (target_shortname, "child") == 0
          || strcmp (target_shortname, "procfs") == 0
          || strcmp (target_shortname, "vxprocess") == 0)
        disassemble_from_exec = 0; /* It's a child process, read inferior mem */
      else
        disassemble_from_exec = 1; /* It's remote, read the exec file */
    }

  if (disassemble_from_exec)
    di.read_memory_func = gdbtk_dis_asm_read_memory;
  else
    di.read_memory_func = dis_asm_read_memory;

  /* If just doing straight assembly, all we need to do is disassemble
     everything between low and high.  If doing mixed source/assembly, we've
     got a totally different path to follow.  */

  if (mixed_source_and_assembly)
    {				/* Come here for mixed source/assembly */
      /* The idea here is to present a source-O-centric view of a function to
         the user.  This means that things are presented in source order, with
         (possibly) out of order assembly immediately following.  */
      struct symtab *symtab;
      struct linetable_entry *le;
      int nlines;
      int newlines;
      struct my_line_entry *mle;
      struct symtab_and_line sal;
      int i;
      int out_of_order;
      int next_line;

      symtab = find_pc_symtab (low); /* Assume symtab is valid for whole PC range */

      if (!symtab || !symtab->linetable)
        goto assembly_only;

      /* First, convert the linetable to a bunch of my_line_entry's.  */

      le = symtab->linetable->item;
      nlines = symtab->linetable->nitems;

      if (nlines <= 0)
        goto assembly_only;

      mle = (struct my_line_entry *) alloca (nlines * sizeof (struct my_line_entry));

      out_of_order = 0;
      
      /* Copy linetable entries for this function into our data structure, creating
         end_pc's and setting out_of_order as appropriate.  */

      /* First, skip all the preceding functions.  */

      for (i = 0; i < nlines - 1 && le[i].pc < low; i++) ;

      /* Now, copy all entries before the end of this function.  */

      newlines = 0;
      for (; i < nlines - 1 && le[i].pc < high; i++)
        {
          if (le[i].line == le[i + 1].line
              && le[i].pc == le[i + 1].pc)
            continue;		/* Ignore duplicates */

          mle[newlines].line = le[i].line;
          if (le[i].line > le[i + 1].line)
            out_of_order = 1;
          mle[newlines].start_pc = le[i].pc;
          mle[newlines].end_pc = le[i + 1].pc;
          newlines++;
        }

      /* If we're on the last line, and it's part of the function, then we need to
         get the end pc in a special way.  */

      if (i == nlines - 1
          && le[i].pc < high)
        {
          mle[newlines].line = le[i].line;
          mle[newlines].start_pc = le[i].pc;
          sal = find_pc_line (le[i].pc, 0);
          mle[newlines].end_pc = sal.end;
          newlines++;
        }

      /* Now, sort mle by line #s (and, then by addresses within lines). */

      if (out_of_order)
        qsort (mle, newlines, sizeof (struct my_line_entry), compare_lines);

      /* Now, for each line entry, emit the specified lines (unless they have been
         emitted before), followed by the assembly code for that line.  */

      next_line = 0;		/* Force out first line */
      for (i = 0; i < newlines; i++)
        {
          /* Print out everything from next_line to the current line.  */

          if (mle[i].line >= next_line)
            {
              if (next_line != 0)
                print_source_lines (symtab, next_line, mle[i].line + 1, 0);
              else
                print_source_lines (symtab, mle[i].line, mle[i].line + 1, 0);

              next_line = mle[i].line + 1;
            }

          for (pc = mle[i].start_pc; pc < mle[i].end_pc; )
            {
              QUIT;
              fputs_unfiltered ("    ", gdb_stdout);
              print_address (pc, gdb_stdout);
              fputs_unfiltered (":\t    ", gdb_stdout);
              pc += (*tm_print_insn) (pc, &di);
              fputs_unfiltered ("\n", gdb_stdout);
            }
        }
    }
  else
    {
    assembly_only:
      for (pc = low; pc < high; )
        {
          QUIT;
          fputs_unfiltered ("    ", gdb_stdout);
          print_address (pc, gdb_stdout);
          fputs_unfiltered (":\t    ", gdb_stdout);
          pc += (*tm_print_insn) (pc, &di);
          fputs_unfiltered ("\n", gdb_stdout);
        }
    }

  gdb_flush (gdb_stdout);

  return TCL_OK;
}

/* This is the memory_read_func for gdb_disassemble when we are
   disassembling from the exec file. */

static int
gdbtk_dis_asm_read_memory (memaddr, myaddr, len, info)
     bfd_vma memaddr;
     bfd_byte *myaddr;
     int len;
     disassemble_info *info;
{
  extern struct target_ops exec_ops;
  int res;

  errno = 0;
  res = xfer_memory (memaddr, myaddr, len, 0, &exec_ops);

  if (res == len)
    return 0;
  else
    if (errno == 0)
      return EIO;
    else
      return errno;
}

/* This will be passed to qsort to sort the results of the disassembly */

static int
compare_lines (mle1p, mle2p)
     const PTR mle1p;
     const PTR mle2p;
{
  struct my_line_entry *mle1, *mle2;
  int val;

  mle1 = (struct my_line_entry *) mle1p;
  mle2 = (struct my_line_entry *) mle2p;

  val =  mle1->line - mle2->line;

  if (val != 0)
    return val;

  return mle1->start_pc - mle2->start_pc;
}

/* This implements the TCL command `gdb_loc',
 *
 * Arguments:
 *    ?symbol? The symbol or address to locate - defaults to pc
 * Tcl Return:
 *    a list consisting of the following:                                  
 *       basename, function name, filename, line number, address, current pc
 */

static int
gdb_loc (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  char *filename;
  struct symtab_and_line sal;
  char *funcname, *fname;
  CORE_ADDR pc;

  if (!have_full_symbols () && !have_partial_symbols ())
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "No symbol table is loaded", -1);
      return TCL_ERROR;
    }
  
  if (objc == 1)
    {
      if (selected_frame && (selected_frame->pc != stop_pc))
        {
          /* Note - this next line is not correct on all architectures. */
          /* For a graphical debugger we really want to highlight the */
          /* assembly line that called the next function on the stack. */
          /* Many architectures have the next instruction saved as the */
          /* pc on the stack, so what happens is the next instruction is hughlighted. */
          /* FIXME */
          pc = selected_frame->pc;
          sal = find_pc_line (selected_frame->pc,
                              selected_frame->next != NULL
                              && !selected_frame->next->signal_handler_caller
                              && !frame_in_dummy (selected_frame->next));
        }
      else
        {
          pc = stop_pc;
          sal = find_pc_line (stop_pc, 0);
        }
    }
  else if (objc == 2)
    {
      struct symtabs_and_lines sals;
      int nelts;

      sals = decode_line_spec (Tcl_GetStringFromObj (objv[1], NULL), 1);

      nelts = sals.nelts;
      sal = sals.sals[0];
      free (sals.sals);

      if (sals.nelts != 1)
        {
          Tcl_SetStringObj (result_ptr->obj_ptr, "Ambiguous line spec", -1);
          return TCL_ERROR;
        }
      pc = sal.pc;
    }
  else
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong # args", -1);
      return TCL_ERROR;
    }

  if (sal.symtab)
    Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			      Tcl_NewStringObj (sal.symtab->filename, -1));
  else
    Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewStringObj ("", 0));

  find_pc_partial_function (pc, &funcname, NULL, NULL);
  fname = cplus_demangle (funcname, 0);
  if (fname)
    {
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				Tcl_NewStringObj (fname, -1));
      free (fname);
    }
  else
    Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			      Tcl_NewStringObj (funcname, -1));
  
  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "";

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (filename, -1));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj(sal.line)); /* line number */
  sprintf_append_element_to_obj (result_ptr->obj_ptr, "0x%s", paddr_nz(pc)); /* PC in current frame */
  sprintf_append_element_to_obj (result_ptr->obj_ptr, "0x%s", paddr_nz(stop_pc)); /* Real PC */
  return TCL_OK;
}

/* This implements the Tcl command 'gdb_get_mem', which 
 * dumps a block of memory 
 * Arguments:
 *   gdb_get_mem addr form size num aschar
 *
 *   addr: address of data to dump
 *   form: a char indicating format
 *   size: size of each element; 1,2,4, or 8 bytes
 *   num: the number of bytes to read 
 *   acshar: an optional ascii character to use in ASCII dump
 * 
 * Return:
 * a list of elements followed by an optional ASCII dump 
 */

static int
gdb_get_mem (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int size, asize, i, j, bc;
  CORE_ADDR addr;
  int nbytes, rnum, bpr;
  long tmp;
  char format, c, buff[128], aschar, *mbuf, *mptr, *cptr, *bptr;
  struct type *val_type;

  if (objc < 6 || objc > 7)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr,
			"addr format size bytes bytes_per_row ?ascii_char?", -1);
      return TCL_ERROR; 
    }

  if (Tcl_GetIntFromObj (interp, objv[3], &size) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }
  else if (size <= 0)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Invalid size, must be > 0", -1);
      return TCL_ERROR;
    }
  
  if (Tcl_GetIntFromObj (interp, objv[4], &nbytes) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }
  else if (size <= 0)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Invalid number of bytes, must be > 0", 
			-1);
      return TCL_ERROR;
    }
  
  if (Tcl_GetIntFromObj (interp, objv[5], &bpr) != TCL_OK)
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }
  else if (size <= 0)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Invalid bytes per row, must be > 0", -1);
      return TCL_ERROR;
    }
  
  if (Tcl_GetLongFromObj (interp, objv[1], &tmp) != TCL_OK)
    return TCL_OK;

  addr = (CORE_ADDR) tmp;

  format = *(Tcl_GetStringFromObj (objv[2], NULL));
  mbuf = (char *)malloc (nbytes+32);
  if (!mbuf)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Out of memory.", -1);
      return TCL_ERROR;
    }
  
  memset (mbuf, 0, nbytes+32);
  mptr = cptr = mbuf;

  rnum = target_read_memory_partial (addr, mbuf, nbytes, NULL);

  if (objc == 7)
    aschar = *(Tcl_GetStringFromObj(objv[6], NULL)); 
  else
    aschar = 0;

  switch (size) {
  case 1:
    val_type = builtin_type_char;
    asize = 'b';
    break;
  case 2:
    val_type = builtin_type_short;
    asize = 'h';
    break;
  case 4:
    val_type = builtin_type_int;
    asize = 'w';
    break;
  case 8:
    val_type = builtin_type_long_long;
    asize = 'g';
    break;
  default:
    val_type = builtin_type_char;
    asize = 'b';
  }

  bc = 0;        /* count of bytes in a row */
  buff[0] = '"'; /* buffer for ascii dump */
  bptr = &buff[1];   /* pointer for ascii dump */
  
  result_ptr->flags |= GDBTK_MAKES_LIST; /* Build up the result as a list... */
   
  for (i=0; i < nbytes; i+= size)
    {
      if ( i >= rnum)
        {
          fputs_unfiltered ("N/A ", gdb_stdout);
          if (aschar)
            for ( j = 0; j < size; j++)
              *bptr++ = 'X';
        }
      else
        {
          print_scalar_formatted (mptr, val_type, format, asize, gdb_stdout);

          if (aschar)
            {
              for ( j = 0; j < size; j++)
                {
                  c = *cptr++;
                  if (c < 32 || c > 126)
                    c = aschar;
                  if (c == '"')
                    *bptr++ = '\\';
                  *bptr++ = c;
                }
            }
        }

      mptr += size;
      bc += size;

      if (aschar && (bc >= bpr))
        {
          /* end of row. print it and reset variables */
          bc = 0;
          *bptr++ = '"';
          *bptr++ = ' ';
          *bptr = 0;
          fputs_unfiltered (buff, gdb_stdout);
          bptr = &buff[1];
        }
    }
  
  result_ptr->flags &= ~GDBTK_MAKES_LIST;
	    
  free (mbuf);
  return TCL_OK;
}



/* This implements the tcl command "gdb_loadfile"
 * It loads a c source file into a text widget.
 *
 * Tcl Arguments:
 *    widget: the name of the text widget to fill
 *    filename: the name of the file to load
 *    linenumbers: A boolean indicating whether or not to display line numbers.
 * Tcl Result:
 *
 */

/* In this routine, we will build up a "line table", i.e. a
 * table of bits showing which lines in the source file are executible.
 * LTABLE_SIZE is the number of bytes to allocate for the line table.
 *
 * Its size limits the maximum number of lines 
 * in a file to 8 * LTABLE_SIZE.  This memory is freed after 
 * the file is loaded, so it is OK to make this very large. 
 * Additional memory will be allocated if needed. */
#define LTABLE_SIZE 20000
static int
gdb_loadfile (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  char *file, *widget;
  int linenumbers, ln, lnum, ltable_size;
  FILE *fp;
  char *ltable;
  struct symtab *symtab;
  struct linetable_entry *le;
  long mtime = 0;
  struct stat st;
  Tcl_DString text_cmd_1, text_cmd_2, *cur_cmd;
  char line[1024], line_num_buf[16];
  int prefix_len_1, prefix_len_2, cur_prefix_len, widget_len;

 
  if (objc != 4)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "widget filename linenumbers");
      return TCL_ERROR; 
    }

  widget = Tcl_GetStringFromObj (objv[1], NULL);
  if ( Tk_NameToWindow (interp, widget, Tk_MainWindow (interp)) == NULL)
    {
      return TCL_ERROR;
    }
  
  file  = Tcl_GetStringFromObj (objv[2], NULL);
  Tcl_GetBooleanFromObj (interp, objv[3], &linenumbers);

  symtab = full_lookup_symtab (file);
  if (!symtab)
    {
      Tcl_SetStringObj ( result_ptr->obj_ptr, "File not found in symtab", -1);
      fclose (fp);
      return TCL_ERROR;
    }

  file = symtab_to_filename ( symtab );
  if ((fp = fopen ( file, "r" )) == NULL)
    {
      Tcl_SetStringObj ( result_ptr->obj_ptr, "Can't open file for reading", -1);
      return TCL_ERROR;
    }

  if (stat (file, &st) < 0)
    {
      catch_errors (perror_with_name_wrapper, "gdbtk: get time stamp", "",
                    RETURN_MASK_ALL);
      return TCL_ERROR;
    }

  if (symtab && symtab->objfile && symtab->objfile->obfd)
      mtime = bfd_get_mtime(symtab->objfile->obfd);
  else if (exec_bfd)
      mtime = bfd_get_mtime(exec_bfd);
 
  if (mtime && mtime < st.st_mtime)
     gdbtk_ignorable_warning("Source file is more recent than executable.\n");


  /* Source linenumbers don't appear to be in order, and a sort is */
  /* too slow so the fastest solution is just to allocate a huge */
  /* array and set the array entry for each linenumber */

  ltable_size = LTABLE_SIZE;
  ltable = (char *)malloc (LTABLE_SIZE);
  if (ltable == NULL)
    {
      Tcl_SetStringObj ( result_ptr->obj_ptr, "Out of memory.", -1);
      fclose (fp);
      return TCL_ERROR;
    }

  memset (ltable, 0, LTABLE_SIZE);

  if (symtab->linetable && symtab->linetable->nitems)
    {
      le = symtab->linetable->item;
      for (ln = symtab->linetable->nitems ;ln > 0; ln--, le++)
        {
          lnum = le->line >> 3;
          if (lnum >= ltable_size)
            {
              char *new_ltable;
              new_ltable = (char *)realloc (ltable, ltable_size*2);
              memset (new_ltable + ltable_size, 0, ltable_size);
              ltable_size *= 2;
              if (new_ltable == NULL)
                {
                  Tcl_SetStringObj ( result_ptr->obj_ptr, "Out of memory.", -1);
                  free (ltable);
                  fclose (fp);
                  return TCL_ERROR;
                }
              ltable = new_ltable;
            }
          ltable[lnum] |= 1 << (le->line % 8);
        }
    }
    
  Tcl_DStringInit(&text_cmd_1);
  Tcl_DStringInit(&text_cmd_2);
  
  ln = 1;

  widget_len = strlen (widget);
  line[0] = '\t';
  
  Tcl_DStringAppend (&text_cmd_1, widget, widget_len);
  Tcl_DStringAppend (&text_cmd_2, widget, widget_len);
  
  if (linenumbers)
    {
      Tcl_DStringAppend (&text_cmd_1, " insert end {-\t", -1);
      prefix_len_1 = Tcl_DStringLength(&text_cmd_1);

      Tcl_DStringAppend (&text_cmd_2, " insert end { \t", -1);
      prefix_len_2 = Tcl_DStringLength(&text_cmd_2);
      
      while (fgets (line + 1, 980, fp))
        {
          sprintf (line_num_buf, "%d", ln);
          if (ltable[ln >> 3] & (1 << (ln % 8)))
            {
              cur_cmd = &text_cmd_1;
              cur_prefix_len = prefix_len_1;
              Tcl_DStringAppend (cur_cmd, line_num_buf, -1);
              Tcl_DStringAppend (cur_cmd, "} break_tag", 11);
            }
          else
            {
              cur_cmd = &text_cmd_2;
              cur_prefix_len = prefix_len_2;
              Tcl_DStringAppend (cur_cmd, line_num_buf, -1);
              Tcl_DStringAppend (cur_cmd, "} \"\"", 4);
            }

          Tcl_DStringAppendElement (cur_cmd, line);
          Tcl_DStringAppend (cur_cmd, " source_tag", 11);

          Tcl_Eval(interp, Tcl_DStringValue(cur_cmd));
          Tcl_DStringSetLength(cur_cmd, cur_prefix_len);
          ln++;
        }
    }
  else
    {
      Tcl_DStringAppend (&text_cmd_1, " insert end {- } break_tag", -1);
      prefix_len_1 = Tcl_DStringLength(&text_cmd_1);
      Tcl_DStringAppend (&text_cmd_2, " insert end {  } \"\"", -1);
      prefix_len_2 = Tcl_DStringLength(&text_cmd_2);

      while (fgets (line + 1, 980, fp))
        {
          if (ltable[ln >> 3] & (1 << (ln % 8)))
            {
              cur_cmd = &text_cmd_1;
              cur_prefix_len = prefix_len_1;
            }
          else
            {
              cur_cmd = &text_cmd_2;
              cur_prefix_len = prefix_len_2;
            }

          Tcl_DStringAppendElement (cur_cmd, line);
          Tcl_DStringAppend (cur_cmd, " source_tag", 11);

          Tcl_Eval(interp, Tcl_DStringValue(cur_cmd));
          Tcl_DStringSetLength(cur_cmd, cur_prefix_len);

          ln++;
        }
    }

  Tcl_DStringFree (&text_cmd_1);
  Tcl_DStringFree (&text_cmd_2);
  free (ltable);
  fclose (fp);
  return TCL_OK;
}

/*
 *  This section contains commands for manipulation of breakpoints.
 */


/* set a breakpoint by source file and line number */
/* flags are as follows: */
/* least significant 2 bits are disposition, rest is */
/* type (normally 0).

enum bptype {
  bp_breakpoint,		 Normal breakpoint 
  bp_hardware_breakpoint,	Hardware assisted breakpoint
}

Disposition of breakpoint.  Ie: what to do after hitting it.
enum bpdisp {
  del,				Delete it
  del_at_next_stop,		Delete at next stop, whether hit or not
  disable,			Disable it 
  donttouch			Leave it alone 
  };
*/

/* This implements the tcl command "gdb_set_bp"
 * It sets breakpoints, and runs the Tcl command
 *     gdbtk_tcl_breakpoint create
 * to register the new breakpoint with the GUI.
 *
 * Tcl Arguments:
 *    filename: the file in which to set the breakpoint
 *    line:     the line number for the breakpoint
 *    type:     the type of the breakpoint
 *    thread:	optional thread number
 * Tcl Result:
 *    The return value of the call to gdbtk_tcl_breakpoint.
 */

static int
gdb_set_bp (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];

{
  struct symtab_and_line sal;
  int line, flags, ret, thread = -1;
  struct breakpoint *b;
  char buf[64];
  Tcl_DString cmd;

  if (objc != 4 && objc != 5)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "filename line type [thread]");
      return TCL_ERROR; 
    }
  
  sal.symtab = full_lookup_symtab (Tcl_GetStringFromObj( objv[1], NULL));
  if (sal.symtab == NULL)
    return TCL_ERROR;

  if (Tcl_GetIntFromObj( interp, objv[2], &line) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (Tcl_GetIntFromObj( interp, objv[3], &flags) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (objc == 5)
    {
      if (Tcl_GetIntFromObj( interp, objv[4], &thread) == TCL_ERROR)
	{
	  result_ptr->flags = GDBTK_IN_TCL_RESULT;
	  return TCL_ERROR;
	}
    }

  sal.line = line;
  if (!find_line_pc (sal.symtab, sal.line, &sal.pc))
    return TCL_ERROR;

  sal.section = find_pc_overlay (sal.pc);
  b = set_raw_breakpoint (sal);
  set_breakpoint_count (breakpoint_count + 1);
  b->number = breakpoint_count;
  b->type = flags >> 2;
  b->disposition = flags & 3;
  b->thread = thread;

  /* FIXME: this won't work for duplicate basenames! */
  sprintf (buf, "%s:%d", basename (Tcl_GetStringFromObj ( objv[1], NULL)), line);
  b->addr_string = strsave (buf);

  /* now send notification command back to GUI */

  Tcl_DStringInit (&cmd);

  Tcl_DStringAppend (&cmd, "gdbtk_tcl_breakpoint create ", -1);
  sprintf (buf, "%d", b->number);
  Tcl_DStringAppendElement(&cmd, buf);
  sprintf (buf, "0x%lx", (long)sal.pc);
  Tcl_DStringAppendElement (&cmd, buf);
  Tcl_DStringAppendElement (&cmd, Tcl_GetStringFromObj (objv[2], NULL));
  Tcl_DStringAppendElement (&cmd, Tcl_GetStringFromObj (objv[1], NULL));

  ret = Tcl_Eval (interp, Tcl_DStringValue (&cmd));
  Tcl_DStringFree (&cmd);
  return ret;
}

/* This implements the tcl command "gdb_set_bp_addr"
 * It sets breakpoints, and runs the Tcl command
 *     gdbtk_tcl_breakpoint create
 * to register the new breakpoint with the GUI.
 *
 * Tcl Arguments:
 *    addr: the address at which to set the breakpoint
 *    type:     the type of the breakpoint
 *    thread:	optional thread number
 * Tcl Result:
 *    The return value of the call to gdbtk_tcl_breakpoint.
 */

static int
gdb_set_bp_addr (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];

{
  struct symtab_and_line sal;
  int line, flags, ret, thread = -1;
  long addr;
  struct breakpoint *b;
  char buf[64];
  Tcl_DString cmd;

  if (objc != 4 && objc != 3)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "addr type [thread]");
      return TCL_ERROR; 
    }
  
  if (Tcl_GetLongFromObj( interp, objv[1], &addr) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (Tcl_GetIntFromObj( interp, objv[2], &flags) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (objc == 4)
    {
      if (Tcl_GetIntFromObj( interp, objv[3], &thread) == TCL_ERROR)
	{
	  result_ptr->flags = GDBTK_IN_TCL_RESULT;
	  return TCL_ERROR;
	}
    }

  sal = find_pc_line (addr, 0);
  sal.pc = addr;
  b = set_raw_breakpoint (sal);
  set_breakpoint_count (breakpoint_count + 1);
  b->number = breakpoint_count;
  b->type = flags >> 2;
  b->disposition = flags & 3;
  b->thread = thread;

  sprintf (buf, "*(0x%lx)",addr);
  b->addr_string = strsave (buf);

  /* now send notification command back to GUI */

  Tcl_DStringInit (&cmd);

  Tcl_DStringAppend (&cmd, "gdbtk_tcl_breakpoint create ", -1);
  sprintf (buf, "%d", b->number);
  Tcl_DStringAppendElement(&cmd, buf);
  sprintf (buf, "0x%lx", addr);
  Tcl_DStringAppendElement (&cmd, buf);
  sprintf (buf, "%d", b->line_number);
  Tcl_DStringAppendElement (&cmd, buf);
  Tcl_DStringAppendElement (&cmd, b->source_file);

  ret = Tcl_Eval (interp, Tcl_DStringValue (&cmd));
  Tcl_DStringFree (&cmd);
  return ret;
}

/* This implements the tcl command "gdb_find_bp_at_line"
 *
 * Tcl Arguments:
 *    filename: the file in which to find the breakpoint
 *    line:     the line number for the breakpoint
 * Tcl Result:
 *    It returns a list of breakpoint numbers
 */

static int
gdb_find_bp_at_line(clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];

{
  struct symtab *s;
  int line;
  struct breakpoint *b;
  extern struct breakpoint *breakpoint_chain;

  if (objc != 3)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "filename line");
      return TCL_ERROR; 
    }
  
  s = full_lookup_symtab (Tcl_GetStringFromObj( objv[1], NULL));
  if (s == NULL)
    return TCL_ERROR;
  
  if (Tcl_GetIntFromObj( interp, objv[2], &line) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr ,0 ,NULL);
  for (b = breakpoint_chain; b; b = b->next)
    if (b->line_number == line && !strcmp(b->source_file, s->filename))
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				Tcl_NewIntObj (b->number));
  
  return TCL_OK;
}


/* This implements the tcl command "gdb_find_bp_at_addr"
 *
 * Tcl Arguments:
 *    addr:     address
 * Tcl Result:
 *    It returns a list of breakpoint numbers
 */

static int
gdb_find_bp_at_addr(clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];

{
  long addr;
  struct breakpoint *b;
  extern struct breakpoint *breakpoint_chain;

  if (objc != 2)
    {
      Tcl_WrongNumArgs(interp, 1, objv, "address");
      return TCL_ERROR; 
    }
  
  if (Tcl_GetLongFromObj( interp, objv[1], &addr) == TCL_ERROR)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr ,0 ,NULL);
  for (b = breakpoint_chain; b; b = b->next)
    if (b->address == (CORE_ADDR)addr)
      Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
				Tcl_NewIntObj (b->number));

  return TCL_OK;
}

/* This implements the tcl command gdb_get_breakpoint_info
 *
 *
 * Tcl Arguments:
 *   breakpoint_number
 * Tcl Result:
 *   A list with {file, function, line_number, address, type, enabled?,
 *                disposition, ignore_count, {list_of_commands}, thread, hit_count}
 */

static int
gdb_get_breakpoint_info (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct symtab_and_line sal;
  static char *bptypes[] = {"breakpoint", "hardware breakpoint", "until",
			      "finish", "watchpoint", "hardware watchpoint",
			      "read watchpoint", "access watchpoint",
			      "longjmp", "longjmp resume", "step resume",
			      "through sigtramp", "watchpoint scope",
			      "call dummy" };
  static char *bpdisp[] = {"delete", "delstop", "disable", "donttouch"};
  struct command_line *cmd;
  int bpnum;
  struct breakpoint *b;
  extern struct breakpoint *breakpoint_chain;
  char *funcname, *fname, *filename;
  Tcl_Obj *new_obj;

  if (objc != 2)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "wrong number of args, should be \"breakpoint\"", -1);
      return TCL_ERROR;
    }

  if ( Tcl_GetIntFromObj(NULL, objv[1], &bpnum) != TCL_OK)
    {
      result_ptr->flags = GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  for (b = breakpoint_chain; b; b = b->next)
    if (b->number == bpnum)
      break;

  if (!b || b->type != bp_breakpoint)
    {
      Tcl_SetStringObj (result_ptr->obj_ptr, "Breakpoint #%d does not exist", -1);
      return TCL_ERROR;
    }

  sal = find_pc_line (b->address, 0);

  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "";

  Tcl_SetListObj (result_ptr->obj_ptr ,0 ,NULL);
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
	  Tcl_NewStringObj (filename, -1));

  find_pc_partial_function (b->address, &funcname, NULL, NULL);
  fname = cplus_demangle (funcname, 0);
  if (fname)
    {
      new_obj = Tcl_NewStringObj (fname, -1);
      free (fname);
    }
  else
    new_obj = Tcl_NewStringObj (funcname, -1);

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, new_obj);
  
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj (b->line_number));
  sprintf_append_element_to_obj (result_ptr->obj_ptr, "0x%lx", b->address);
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (bptypes[b->type], -1));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewBooleanObj(b->enable == enabled));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (bpdisp[b->disposition], -1));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj (b->ignore_count));

  new_obj = Tcl_NewObj();
  for (cmd = b->commands; cmd; cmd = cmd->next)
    Tcl_ListObjAppendElement (NULL, new_obj,
			      Tcl_NewStringObj (cmd->line, -1));
  Tcl_ListObjAppendElement(NULL, result_ptr->obj_ptr, new_obj);
			      
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr,
			    Tcl_NewStringObj (b->cond_string, -1));

  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj (b->thread));
  Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, Tcl_NewIntObj (b->hit_count));

  return TCL_OK;
}


/* This implements the tcl command gdb_get_breakpoint_list
 * It builds up a list of the current breakpoints.
 *
 * Tcl Arguments:
 *    None.
 * Tcl Result:
 *    A list of breakpoint numbers.
 */

static int
gdb_get_breakpoint_list (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  struct breakpoint *b;
  extern struct breakpoint *breakpoint_chain;
  Tcl_Obj *new_obj;

  if (objc != 1)
    error ("wrong number of args, none are allowed");

  for (b = breakpoint_chain; b; b = b->next)
    if (b->type == bp_breakpoint)
      {
        new_obj = Tcl_NewIntObj (b->number);
        Tcl_ListObjAppendElement (NULL, result_ptr->obj_ptr, new_obj);
      }

  return TCL_OK;
}

/* The functions in this section deal with stacks and backtraces. */

/* This implements the tcl command gdb_stack.
 * It builds up a list of stack frames.
 *
 * Tcl Arguments:
 *    start  - starting stack frame
 *    count - number of frames to inspect
 * Tcl Result:
 *    A list of function names
 */

static int
gdb_stack (clientData, interp, objc, objv)     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  int start, count;

  if (objc < 3)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "start count");
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  if (Tcl_GetIntFromObj (NULL, objv[1], &start))
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }
  if (Tcl_GetIntFromObj (NULL, objv[2], &count))
    {
      result_ptr->flags |= GDBTK_IN_TCL_RESULT;
      return TCL_ERROR;
    }

  Tcl_SetListObj (result_ptr->obj_ptr, 0, NULL);

  if (target_has_stack)
    {
      struct frame_info *top;
      struct frame_info *fi;

      /* Find the outermost frame */
      fi = get_current_frame ();
      while (fi != NULL)
        {
          top = fi;
          fi = get_prev_frame (fi);
        }

      /* top now points to the top (outermost frame) of the
         stack, so point it to the requested start */
      start = -start;
      top   = find_relative_frame (top, &start);

      /* If start != 0, then we have asked to start outputting
         frames beyond the innermost stack frame */
      if (start == 0)
        {
          fi = top; 
          while (fi && count--)
            {
              get_frame_name (interp, result_ptr->obj_ptr, fi);
              fi = get_next_frame (fi);
            }
        }
    }

  return TCL_OK;
}

/* A helper function for get_stack which adds information about
 * the stack frame FI to the caller's LIST.
 *
 * This is stolen from print_frame_info in stack.c.
 */
static void
get_frame_name (interp, list, fi)
     Tcl_Interp *interp;
     Tcl_Obj *list;
     struct frame_info *fi;
{
  struct symtab_and_line sal;
  struct symbol *func = NULL;
  register char *funname = 0;
  enum language funlang = language_unknown;
  Tcl_Obj *objv[1];

  if (frame_in_dummy (fi))
    {
      objv[0] = Tcl_NewStringObj ("<function called from gdb>\n", -1);
      Tcl_ListObjAppendElement (interp, list, objv[0]);
      return;
    }
  if (fi->signal_handler_caller)
    {
      objv[0] = Tcl_NewStringObj ("<signal handler called>\n", -1);
      Tcl_ListObjAppendElement (interp, list, objv[0]);
      return;
    }

  sal =
    find_pc_line (fi->pc,
                  fi->next != NULL
                  && !fi->next->signal_handler_caller
                  && !frame_in_dummy (fi->next));
  
  func = find_pc_function (fi->pc);
  if (func)
    {
      struct minimal_symbol *msymbol = lookup_minimal_symbol_by_pc (fi->pc);
      if (msymbol != NULL
          && (SYMBOL_VALUE_ADDRESS (msymbol) 
              > BLOCK_START (SYMBOL_BLOCK_VALUE (func))))
        {
          func = 0;
          funname = SYMBOL_NAME (msymbol);
          funlang = SYMBOL_LANGUAGE (msymbol);
        }
      else
        {
          funname = SYMBOL_NAME (func);
          funlang = SYMBOL_LANGUAGE (func);
        }
    }
  else
    {
      struct minimal_symbol *msymbol = lookup_minimal_symbol_by_pc (fi->pc);
      if (msymbol != NULL)
        {
          funname = SYMBOL_NAME (msymbol);
          funlang = SYMBOL_LANGUAGE (msymbol);
        }
    }
  
  if (sal.symtab)
    {
      char *name = NULL;

      if (funlang == language_cplus)
        name = cplus_demangle (funname, 0);
      if (name == NULL)
        name = funname;

      objv[0] = Tcl_NewStringObj (name, -1);
      Tcl_ListObjAppendElement (interp, list, objv[0]);
    }
  else
    {
#if 0
      /* we have no convenient way to deal with this yet... */
      if (fi->pc != sal.pc || !sal.symtab)
        {
          print_address_numeric (fi->pc, 1, gdb_stdout);
          printf_filtered (" in ");
        }
      printf_symbol_filtered (gdb_stdout, funname ? funname : "??", funlang,
                               DMGL_ANSI);
#endif
      objv[0] = Tcl_NewStringObj (funname != NULL ? funname : "??", -1);
#ifdef PC_LOAD_SEGMENT
      /* If we couldn't print out function name but if can figure out what
         load segment this pc value is from, at least print out some info
         about its load segment. */
      if (!funname)
        {
          Tcl_AppendStringsToObj (objv[0], " from ", PC_LOAD_SEGMENT (fi->pc),
                                  (char *) NULL);
        }
#endif
#ifdef PC_SOLIB
      if (!funname)
        {
          char *lib = PC_SOLIB (fi->pc);
          if (lib)
            {
              Tcl_AppendStringsToObj (objv[0], " from ", lib, (char *) NULL);
            }
        }
#endif
      Tcl_ListObjAppendElement (interp, list, objv[0]);
    }
}


/*
 * This section contains a bunch of miscellaneous utility commands
 */

/* This implements the tcl command gdb_path_conv
 *
 * On Windows, it canonicalizes the pathname,
 * On Unix, it is a no op.
 *
 * Arguments:
 *    path
 * Tcl Result:
 *    The canonicalized path.
 */

static int
gdb_path_conv (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  if (objc != 2)
    error ("wrong # args");
  
#ifdef WINNT
  {
    char pathname[256], *ptr;

    cygwin32_conv_to_full_win32_path (Tcl_GetStringFromObj(objv[1], NULL), pathname);
    for (ptr = pathname; *ptr; ptr++)
      {
	if (*ptr == '\\')
	  *ptr = '/';
      }
    Tcl_SetStringObj (result_ptr->obj_ptr, pathname, -1);
  }
#else
  Tcl_SetStringObj (result_ptr->obj_ptr, Tcl_GetStringFromObj (objv[1], NULL), -1);
#endif

  return TCL_OK;
}

/*
 * This section has utility routines that are not Tcl commands.
 */

static int
perror_with_name_wrapper (args)
  char * args;
{
  perror_with_name (args);
  return 1;
}

/* The lookup_symtab() in symtab.c doesn't work correctly */
/* It will not work will full pathnames and if multiple */
/* source files have the same basename, it will return */
/* the first one instead of the correct one.  This version */
/* also always makes sure symtab->fullname is set. */

static struct symtab *
full_lookup_symtab(file)
     char *file;
{
  struct symtab *st;
  struct objfile *objfile;
  char *bfile, *fullname;
  struct partial_symtab *pt;

  if (!file)
    return NULL;

  /* first try a direct lookup */
  st = lookup_symtab (file);
  if (st)
    {
      if (!st->fullname)
	  symtab_to_filename(st);
      return st;
    }
  
  /* if the direct approach failed, try */
  /* looking up the basename and checking */
  /* all matches with the fullname */
  bfile = basename (file);
  ALL_SYMTABS (objfile, st)
    {
      if (!strcmp (bfile, basename(st->filename)))
        {
          if (!st->fullname)
            fullname = symtab_to_filename (st);
          else
            fullname = st->fullname;

          if (!strcmp (file, fullname))
            return st;
        }
    }
  
  /* still no luck?  look at psymtabs */
  ALL_PSYMTABS (objfile, pt)
    {
      if (!strcmp (bfile, basename(pt->filename)))
        {
          st = PSYMTAB_TO_SYMTAB (pt);
          if (st)
            {
              fullname = symtab_to_filename (st);
              if (!strcmp (file, fullname))
                return st;
            }
        }
    }
  return NULL;
}
