/* Tcl/Tk interface routines.
   Copyright 1994, 1995, 1996, 1997 Free Software Foundation, Inc.

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

#ifdef _WIN32
#include <winuser.h>
#endif

#include <tcl.h>
#include <tk.h>
#include <itcl.h> 
#include <tix.h> 

#ifdef IDE
/* start-sanitize-ide */
#include "guitcl.h"
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

#ifndef WINNT
#ifndef FIOASYNC
#include <sys/stropts.h>
#endif
#endif

#ifdef __CYGWIN32__
#include <sys/time.h>
#endif

#ifdef WINNT
#define GDBTK_PATH_SEP ";"
#else
#define GDBTK_PATH_SEP ":"
#endif

/* Some versions (1.3.79, 1.3.81) of Linux don't support SIOCSPGRP the way
   gdbtk wants to use it... */
#ifdef __linux__
#undef SIOCSPGRP
#endif

int gdbtk_load_hash PARAMS ((char *, unsigned long));
int (*ui_load_progress_hook) PARAMS ((char *, unsigned long));

static void null_routine PARAMS ((int));
static void gdbtk_flush PARAMS ((FILE *));
static void gdbtk_fputs PARAMS ((const char *, FILE *));
static int gdbtk_query PARAMS ((const char *, va_list));
static char *gdbtk_readline PARAMS ((char *));
static void gdbtk_init PARAMS ((char *));
static void tk_command_loop PARAMS ((void));
static void gdbtk_call_command PARAMS ((struct cmd_list_element *, char *, int));
static int gdbtk_wait PARAMS ((int, struct target_waitstatus *));
static void x_event PARAMS ((int));
static void gdbtk_interactive PARAMS ((void));
static void cleanup_init PARAMS ((int));
static void tk_command PARAMS ((char *, int));
static int gdb_disassemble PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int compare_lines PARAMS ((const PTR, const PTR));
static int gdbtk_dis_asm_read_memory PARAMS ((bfd_vma, bfd_byte *, int, disassemble_info *));
static int gdb_path_conv PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_stop PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_confirm_quit PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_force_quit PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_listfiles PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_listfuncs PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int call_wrapper PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_cmd PARAMS ((ClientData, Tcl_Interp *, int, char *argv[]));
static int gdb_fetch_registers PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static void gdbtk_readline_end PARAMS ((void));
static int gdb_changed_register_list PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static void register_changed_p PARAMS ((int, void *));
static int gdb_get_breakpoint_list PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_get_breakpoint_info PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static void breakpoint_notify PARAMS ((struct breakpoint *, const char *));
static void gdbtk_create_breakpoint PARAMS ((struct breakpoint *));
static void gdbtk_delete_breakpoint PARAMS ((struct breakpoint *));
static void gdbtk_modify_breakpoint PARAMS ((struct breakpoint *));
static int gdb_loc PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_eval PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int gdb_sourcelines PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static int map_arg_registers PARAMS ((int, char *[], void (*) (int, void *), void *));
static void get_register_name PARAMS ((int, void *));
static int gdb_regnames PARAMS ((ClientData, Tcl_Interp *, int, char *[]));
static void get_register PARAMS ((int, void *));
static int gdb_target_has_execution_command PARAMS ((ClientData, Tcl_Interp *, int, char *argv[]));
static int gdb_load_info PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_vars_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_function_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_line_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_file_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_tracepoint_exists_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_get_tracepoint_info PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_actions_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static int gdb_prompt_command PARAMS ((ClientData, Tcl_Interp *, int, Tcl_Obj *CONST objv[]));
static void gdbtk_create_tracepoint PARAMS ((struct tracepoint *));
static void gdbtk_delete_tracepoint PARAMS ((struct tracepoint *));
static void tracepoint_notify PARAMS ((struct tracepoint *, const char *));

/* Handle for TCL interpreter */

static Tcl_Interp *interp = NULL;

#ifndef WINNT
static int x_fd;		/* X network socket */
#endif

/* This variable is true when the inferior is running.  Although it's
   possible to disable most input from widgets and thus prevent
   attempts to do anything while the inferior is running, any commands
   that get through - even a simple memory read - are Very Bad, and
   may cause GDB to crash or behave strangely.  So, this variable
   provides an extra layer of defense.  */

static int running_now;

/* This variable determines where memory used for disassembly is read from.
   If > 0, then disassembly comes from the exec file rather than the
   target (which might be at the other end of a slow serial link).  If
   == 0 then disassembly comes from target.  If < 0 disassembly is
   automatically switched to the target if it's an inferior process,
   otherwise the exec file is used.  */

static int disassemble_from_exec = -1;

#ifndef _WIN32

/* Supply malloc calls for tcl/tk.  We do not want to do this on
   Windows, because Tcl_Alloc is probably in a DLL which will not call
   the mmalloc routines.  */

char *
Tcl_Alloc (size)
     unsigned int size;
{
  return xmalloc (size);
}

char *
Tcl_Realloc (ptr, size)
     char *ptr;
     unsigned int size;
{
  return xrealloc (ptr, size);
}

void
Tcl_Free(ptr)
     char *ptr;
{
  free (ptr);
}

#endif /* ! _WIN32 */

static void
null_routine(arg)
     int arg;
{
}

#ifdef _WIN32

/* On Windows, if we hold a file open, other programs can't write to
   it.  In particular, we don't want to hold the executable open,
   because it will mean that people have to get out of the debugging
   session in order to remake their program.  So we close it, although
   this will cost us if and when we need to reopen it.  */

static void
close_bfds ()
{
  struct objfile *o;

  ALL_OBJFILES (o)
    {
      if (o->obfd != NULL)
	bfd_cache_close (o->obfd);
    }

  if (exec_bfd != NULL)
    bfd_cache_close (exec_bfd);
}

#endif /* _WIN32 */

/* The following routines deal with stdout/stderr data, which is created by
   {f}printf_{un}filtered and friends.  gdbtk_fputs and gdbtk_flush are the
   lowest level of these routines and capture all output from the rest of GDB.
   Normally they present their data to tcl via callbacks to the following tcl
   routines:  gdbtk_tcl_fputs, gdbtk_tcl_fputs_error, and gdbtk_flush.  These
   in turn call tk routines to update the display.

   Under some circumstances, you may want to collect the output so that it can
   be returned as the value of a tcl procedure.  This can be done by
   surrounding the output routines with calls to start_saving_output and
   finish_saving_output.  The saved data can then be retrieved with
   get_saved_output (but this must be done before the call to
   finish_saving_output).  */

/* Dynamic string for output. */

static Tcl_DString *result_ptr;

/* Dynamic string for stderr.  This is only used if result_ptr is
   NULL.  */

static Tcl_DString *error_string_ptr;

static void
gdbtk_flush (stream)
     FILE *stream;
{
#if 0
  /* Force immediate screen update */

  Tcl_VarEval (interp, "gdbtk_tcl_flush", NULL);
#endif
}

static void
gdbtk_fputs (ptr, stream)
     const char *ptr;
     FILE *stream;
{
  if (result_ptr)
    Tcl_DStringAppend (result_ptr, (char *) ptr, -1);
  else if (error_string_ptr != NULL && stream == gdb_stderr)
    Tcl_DStringAppend (error_string_ptr, (char *) ptr, -1);
  else
    {
      Tcl_DString str;

      Tcl_DStringInit (&str);

      Tcl_DStringAppend (&str, "gdbtk_tcl_fputs", -1);
      Tcl_DStringAppendElement (&str, (char *)ptr);

      Tcl_Eval (interp, Tcl_DStringValue (&str)); 
      Tcl_DStringFree (&str);
    }
}

static int
gdbtk_query (query, args)
     const char *query;
     va_list args;
{
  char buf[200], *merge[2];
  char *command;
  long val;

  vsprintf (buf, query, args);
  merge[0] = "gdbtk_tcl_query";
  merge[1] = buf;
  command = Tcl_Merge (2, merge);
  Tcl_Eval (interp, command);
  Tcl_Free (command);

  val = atol (interp->result);
  return val;
}

/* VARARGS */
static void
#ifdef ANSI_PROTOTYPES
gdbtk_readline_begin (char *format, ...)
#else
gdbtk_readline_begin (va_alist)
     va_dcl
#endif
{
  va_list args;
  char buf[200], *merge[2];
  char *command;

#ifdef ANSI_PROTOTYPES
  va_start (args, format);
#else
  char *format;
  va_start (args);
  format = va_arg (args, char *);
#endif

  vsprintf (buf, format, args);
  merge[0] = "gdbtk_tcl_readline_begin";
  merge[1] = buf;
  command = Tcl_Merge (2, merge);
  Tcl_Eval (interp, command);
  free (command);
}

static char *
gdbtk_readline (prompt)
     char *prompt;
{
  char *merge[2];
  char *command;
  int result;

#ifdef _WIN32
  close_bfds ();
#endif

  merge[0] = "gdbtk_tcl_readline";
  merge[1] = prompt;
  command = Tcl_Merge (2, merge);
  result = Tcl_Eval (interp, command);
  free (command);
  if (result == TCL_OK)
    {
      return (strdup (interp -> result));
    }
  else
    {
      gdbtk_fputs (interp -> result, gdb_stdout);
      gdbtk_fputs ("\n", gdb_stdout);
      return (NULL);
    }
}

static void
gdbtk_readline_end ()
{
  Tcl_Eval (interp, "gdbtk_tcl_readline_end");
}


static void
#ifdef ANSI_PROTOTYPES
dsprintf_append_element (Tcl_DString *dsp, char *format, ...)
#else
dsprintf_append_element (va_alist)
     va_dcl
#endif
{
  va_list args;
  char buf[1024];

#ifdef ANSI_PROTOTYPES
  va_start (args, format);
#else
  Tcl_DString *dsp;
  char *format;

  va_start (args);
  dsp = va_arg (args, Tcl_DString *);
  format = va_arg (args, char *);
#endif

  vsprintf (buf, format, args);

  Tcl_DStringAppendElement (dsp, buf);
}

static int
gdb_path_conv (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
#ifdef WINNT
  char pathname[256], *ptr;
  if (argc != 2)
    error ("wrong # args");
  cygwin32_conv_to_full_win32_path (argv[1], pathname);
  for (ptr = pathname; *ptr; ptr++)
    {
      if (*ptr == '\\')
	*ptr = '/';
    }
#else
  char *pathname = argv[1];
#endif
  Tcl_DStringAppend (result_ptr, pathname, strlen(pathname));
  return TCL_OK;
}

static int
gdb_get_breakpoint_list (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct breakpoint *b;
  extern struct breakpoint *breakpoint_chain;

  if (argc != 1)
    error ("wrong # args");

  for (b = breakpoint_chain; b; b = b->next)
    if (b->type == bp_breakpoint)
      dsprintf_append_element (result_ptr, "%d", b->number);

  return TCL_OK;
}

static int
gdb_get_breakpoint_info (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
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
  char *funcname, *filename;
  
  if (argc != 2)
    error ("wrong # args");

  bpnum = atoi (argv[1]);

  for (b = breakpoint_chain; b; b = b->next)
    if (b->number == bpnum)
      break;

  if (!b || b->type != bp_breakpoint)
    error ("Breakpoint #%d does not exist", bpnum);

  sal = find_pc_line (b->address, 0);

  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "N/A";
  Tcl_DStringAppendElement (result_ptr, filename);
  find_pc_partial_function (b->address, &funcname, NULL, NULL);
  Tcl_DStringAppendElement (result_ptr, funcname);
  dsprintf_append_element (result_ptr, "%d", sal.line);
  dsprintf_append_element (result_ptr, "0x%lx", b->address);
  Tcl_DStringAppendElement (result_ptr, bptypes[b->type]);
  Tcl_DStringAppendElement (result_ptr, b->enable == enabled ? "1" : "0");
  Tcl_DStringAppendElement (result_ptr, bpdisp[b->disposition]);
  dsprintf_append_element (result_ptr, "%d", b->ignore_count);

  Tcl_DStringStartSublist (result_ptr);
  for (cmd = b->commands; cmd; cmd = cmd->next)
    Tcl_DStringAppendElement (result_ptr, cmd->line);
  Tcl_DStringEndSublist (result_ptr);

  Tcl_DStringAppendElement (result_ptr, b->cond_string);

  dsprintf_append_element (result_ptr, "%d", b->thread);
  dsprintf_append_element (result_ptr, "%d", b->hit_count);

  return TCL_OK;
}

static void
breakpoint_notify(b, action)
     struct breakpoint *b;
     const char *action;
{
  char buf[256];
  int v;
  struct symtab_and_line sal;
  char *filename;

  if (b->type != bp_breakpoint)
    return;

  /* We ensure that ACTION contains no special Tcl characters, so we
     can do this.  */
  sal = find_pc_line (b->address, 0);
  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "N/A";
  sprintf (buf, "gdbtk_tcl_breakpoint %s %d 0x%lx %d {%s}", action, b->number, 
	   (long)b->address, sal.line, filename);

  v = Tcl_Eval (interp, buf);

  if (v != TCL_OK)
    {
      gdbtk_fputs (interp->result, gdb_stdout);
      gdbtk_fputs ("\n", gdb_stdout);
    }
}

static void
gdbtk_create_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify (b, "create");
}

static void
gdbtk_delete_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify (b, "delete");
}

static void
gdbtk_modify_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify (b, "modify");
}

/* This implements the TCL command `gdb_loc', which returns a list consisting
   of the source and line number associated with the current pc. */

static int
gdb_loc (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  char *filename;
  struct symtab_and_line sal;
  char *funcname;
  CORE_ADDR pc;

  if (!have_full_symbols () && !have_partial_symbols ())
    {
      Tcl_SetResult (interp, "No symbol table is loaded", TCL_STATIC);
      return TCL_ERROR;
    }

  if (argc == 1)
    {
      pc = selected_frame ? selected_frame->pc : stop_pc;
      sal = find_pc_line (pc, 0);
    }
  else if (argc == 2)
    {
      struct symtabs_and_lines sals;
      int nelts;

      sals = decode_line_spec (argv[1], 1);

      nelts = sals.nelts;
      sal = sals.sals[0];
      free (sals.sals);

      if (sals.nelts != 1)
	error ("Ambiguous line spec");

      pc = sal.pc;
    }
  else
    error ("wrong # args");

  if (sal.symtab)
    Tcl_DStringAppendElement (result_ptr, sal.symtab->filename);
  else
    Tcl_DStringAppendElement (result_ptr, "");

  find_pc_partial_function (pc, &funcname, NULL, NULL);
  Tcl_DStringAppendElement (result_ptr, funcname);

  filename = symtab_to_filename (sal.symtab);
  if (filename == NULL)
    filename = "N/A";
  Tcl_DStringAppendElement (result_ptr, filename);

  dsprintf_append_element (result_ptr, "%d", sal.line); /* line number */

  dsprintf_append_element (result_ptr, "0x%s", paddr_nz(pc)); /* PC in current frame */

  dsprintf_append_element (result_ptr, "0x%s", paddr_nz(stop_pc)); /* Real PC */

  return TCL_OK;
}

/* This implements the TCL command `gdb_eval'. */

static int
gdb_eval (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct expression *expr;
  struct cleanup *old_chain;
  value_ptr val;

  if (argc != 2)
    error ("wrong # args");

  expr = parse_expression (argv[1]);

  old_chain = make_cleanup (free_current_contents, &expr);

  val = evaluate_expression (expr);

  val_print (VALUE_TYPE (val), VALUE_CONTENTS (val), VALUE_ADDRESS (val),
	     gdb_stdout, 0, 0, 0, 0);

  do_cleanups (old_chain);

  return TCL_OK;
}

/* gdb_get_mem addr form size num aschar*/
/* dump a block of memory */
/* addr: address of data to dump */
/* form: a char indicating format */
/* size: size of each element; 1,2,4, or 8 bytes*/
/* num: the number of 'size' elements to return */
/* acshar:  an optional ascii character to use in ASCII dump */
/* returns a list of 'num' elements followed by an optional */
/* ASCII dump */
static int
gdb_get_mem (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int size, asize, num, i, j;
  CORE_ADDR addr, saved_addr, ptr;
  int format;
  struct type *val_type;
  value_ptr vptr;
  char c, buff[128], aschar;

  if (argc != 6)
    error ("wrong # args");

  addr = (CORE_ADDR)strtoul(argv[1],(char **)NULL,0);
  saved_addr = addr;
  format = *argv[2];
  size = (int)strtoul(argv[3],(char **)NULL,0);
  num = (int)strtoul(argv[4],(char **)NULL,0);
  aschar = *argv[5];

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

  for (i=0; i < num; i++)
    {
      vptr = value_at (val_type, addr, (asection *)NULL);
      print_scalar_formatted (VALUE_CONTENTS(vptr), val_type, format, asize, gdb_stdout);
      fputs_unfiltered (" ", gdb_stdout);
      addr += size;
    }

  if (aschar)
    {
      val_type = builtin_type_char;
      ptr = saved_addr;
      buff[0] = '"';
      i = 1;
      for (j=0; j < num*size; j++)
	{
	  c = *(char *)VALUE_CONTENTS(value_at (val_type, ptr, (asection *)NULL));
	  if (c < 32 || c > 126)
	    c = aschar;
	  if (c == '"')
	    buff[i++] = '\\';
	  buff[i++] = c;
	  ptr++;
	}
      buff[i++] = '"';
      buff[i] = 0;
      fputs_unfiltered (buff, gdb_stdout);
    }

  return TCL_OK;
}


/* This implements the TCL command `gdb_sourcelines', which returns a list of
   all of the lines containing executable code for the specified source file
   (ie: lines where you can put breakpoints). */

static int
gdb_sourcelines (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct symtab *symtab;
  struct linetable_entry *le;
  int nlines;

  if (argc != 2)
    error ("wrong # args");

  symtab = lookup_symtab (argv[1]);

  if (!symtab)
    error ("No such file");

  /* If there's no linetable, or no entries, then we are done. */

  if (!symtab->linetable
      || symtab->linetable->nitems == 0)
    {
      Tcl_DStringAppendElement (result_ptr, "");
      return TCL_OK;
    }

  le = symtab->linetable->item;
  nlines = symtab->linetable->nitems;

  for (;nlines > 0; nlines--, le++)
    {
      /* If the pc of this line is the same as the pc of the next line, then
	 just skip it.  */
      if (nlines > 1
	  && le->pc == (le + 1)->pc)
	continue;

      dsprintf_append_element (result_ptr, "%d", le->line);
    }

  return TCL_OK;
}

static int
map_arg_registers (argc, argv, func, argp)
     int argc;
     char *argv[];
     void (*func) PARAMS ((int regnum, void *argp));
     void *argp;
{
  int regnum;

  /* Note that the test for a valid register must include checking the
     reg_names array because NUM_REGS may be allocated for the union of the
     register sets within a family of related processors.  In this case, the
     trailing entries of reg_names will change depending upon the particular
     processor being debugged.  */

  if (argc == 0)		/* No args, just do all the regs */
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
  for (; argc > 0; argc--, argv++)
    {
      regnum = atoi (*argv);

      if (regnum >= 0
	  && regnum < NUM_REGS
	  && reg_names[regnum] != NULL
	  && *reg_names[regnum] != '\000')
	func (regnum, argp);
      else
	error ("bad register number");
    }

  return TCL_OK;
}

static void
get_register_name (regnum, argp)
     int regnum;
     void *argp;		/* Ignored */
{
  Tcl_DStringAppendElement (result_ptr, reg_names[regnum]);
}

/* This implements the TCL command `gdb_regnames', which returns a list of
   all of the register names. */

static int
gdb_regnames (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  argc--;
  argv++;

  return map_arg_registers (argc, argv, get_register_name, NULL);
}

#ifndef REGISTER_CONVERTIBLE
#define REGISTER_CONVERTIBLE(x) (0 != 0)
#endif

#ifndef REGISTER_CONVERT_TO_VIRTUAL
#define REGISTER_CONVERT_TO_VIRTUAL(x, y, z, a)
#endif

#ifndef INVALID_FLOAT
#define INVALID_FLOAT(x, y) (0 != 0)
#endif

static void
get_register (regnum, fp)
     int regnum;
     void *fp;
{
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
  int format = (int)fp;

  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    {
      Tcl_DStringAppendElement (result_ptr, "Optimized out");
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

  Tcl_DStringAppend (result_ptr, " ", -1);
}

static int
gdb_fetch_registers (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int format;

  if (argc < 2)
    error ("wrong # args");

  argc--;
  argv++;

  argc--;
  format = **argv++;

  return map_arg_registers (argc, argv, get_register, (void *) format);
}

/* This contains the previous values of the registers, since the last call to
   gdb_changed_register_list.  */

static char old_regs[REGISTER_BYTES];

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

  dsprintf_append_element (result_ptr, "%d", regnum);
}

static int
gdb_changed_register_list (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  argc--;
  argv++;

  return map_arg_registers (argc, argv, register_changed_p, NULL);
}

/* This implements the TCL command `gdb_cmd', which sends its argument into
   the GDB command scanner.  */

static int
gdb_cmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  Tcl_DString *save_ptr = NULL;

  if (argc != 2)
    error ("wrong # args");

  if (running_now)
    return TCL_OK;

  /* for the load instruction (and possibly others later) we
     set result_ptr to NULL so gdbtk_fputs() will not buffer
     all the data until the command is finished. */

  if (strncmp("load ",argv[1],5) == 0) {
    Tcl_DStringAppend (result_ptr, "", -1);
    save_ptr = result_ptr;
    result_ptr = NULL;
  }

  execute_command (argv[1], 1);

  bpstat_do_actions (&stop_bpstat);
  
  if (save_ptr) 
    result_ptr = save_ptr;

  return TCL_OK;
}

/* Client of call_wrapper - this routine performs the actual call to
   the client function. */

struct wrapped_call_args
{
  Tcl_Interp *interp;
  Tcl_CmdProc *func;
  int argc;
  char **argv;
  int val;
};

static int
wrapped_call (args)
     struct wrapped_call_args *args;
{
  args->val = (*args->func) (args->func, args->interp, args->argc, args->argv);
  return 1;
}

/* This routine acts as a top-level for all GDB code called by tcl/Tk.  It
   handles cleanups, and calls to return_to_top_level (usually via error).
   This is necessary in order to prevent a longjmp out of the bowels of Tk,
   possibly leaving things in a bad state.  Since this routine can be called
   recursively, it needs to save and restore the contents of the jmp_buf as
   necessary.  */

static int
call_wrapper (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct wrapped_call_args wrapped_args;
  Tcl_DString result, *old_result_ptr;
  Tcl_DString error_string, *old_error_string_ptr;

  Tcl_DStringInit (&result);
  old_result_ptr = result_ptr;
  result_ptr = &result;

  Tcl_DStringInit (&error_string);
  old_error_string_ptr = error_string_ptr;
  error_string_ptr = &error_string;

  wrapped_args.func = (Tcl_CmdProc *)clientData;
  wrapped_args.interp = interp;
  wrapped_args.argc = argc;
  wrapped_args.argv = argv;
  wrapped_args.val = 0;

  if (!catch_errors (wrapped_call, &wrapped_args, "", RETURN_MASK_ALL))
    {
      wrapped_args.val = TCL_ERROR;	/* Flag an error for TCL */

      gdb_flush (gdb_stderr);	/* Flush error output */

      gdb_flush (gdb_stdout);	/* Sometimes error output comes here as well */

      /* In case of an error, we may need to force the GUI into idle
	 mode because gdbtk_call_command may have bombed out while in
	 the command routine.  */

      running_now = 0;
      Tcl_Eval (interp, "gdbtk_tcl_idle");
    }

  if (Tcl_DStringLength (&error_string) == 0)
    {
      Tcl_DStringResult (interp, &result);
      Tcl_DStringFree (&error_string);
    }
  else if (Tcl_DStringLength (&result) == 0)
    {
      Tcl_DStringResult (interp, &error_string);
      Tcl_DStringFree (&result);
    }
  else
    {
      Tcl_ResetResult (interp);
      Tcl_AppendResult (interp, Tcl_DStringValue (&result),
			Tcl_DStringValue (&error_string), (char *) NULL);
      Tcl_DStringFree (&result);
      Tcl_DStringFree (&error_string);
    }
  
  result_ptr = old_result_ptr;
  error_string_ptr = old_error_string_ptr;

#ifdef _WIN32
  close_bfds ();
#endif

  return wrapped_args.val;
}

static int
gdb_listfiles (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct objfile *objfile;
  struct partial_symtab *psymtab;
  struct symtab *symtab;

  ALL_PSYMTABS (objfile, psymtab)
    Tcl_DStringAppendElement (result_ptr, psymtab->filename);

  ALL_SYMTABS (objfile, symtab)
    Tcl_DStringAppendElement (result_ptr, symtab->filename);

  return TCL_OK;
}

static int
gdb_listfuncs (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  struct symtab *symtab;
  struct blockvector *bv;
  struct block *b;
  struct symbol *sym;
  int i,j;
         
  if (argc != 2)
    error ("wrong # args");
  
  symtab = lookup_symtab (argv[1]);
  
  if (!symtab)
    error ("No such file");

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
	      Tcl_DStringAppendElement (result_ptr, SYMBOL_NAME(sym));
	    }
	}
    }
  return TCL_OK;
}

static int
gdb_stop (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  if (target_stop)
    target_stop ();
  else
    quit_flag = 1; /* hope something sees this */

  return TCL_OK;
}

/* Prepare to accept a new executable file.  This is called when we
   want to clear away everything we know about the old file, without
   asking the user.  The Tcl code will have already asked the user if
   necessary.  After this is called, we should be able to run the
   `file' command without getting any questions.  */

static int
gdb_clear_file (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
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

  return TCL_OK;
}

/* Ask the user to confirm an exit request.  */

static int
gdb_confirm_quit (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int ret;

  ret = quit_confirm ();
  Tcl_DStringAppendElement (result_ptr, ret ? "1" : "0");
  return TCL_OK;
}

/* Quit without asking for confirmation.  */

static int
gdb_force_quit (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  quit_force ((char *) NULL, 1);
  return TCL_OK;
}

/* This implements the TCL command `gdb_disassemble'.  */

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

/* We need a different sort of line table from the normal one cuz we can't
   depend upon implicit line-end pc's for lines.  This is because of the
   reordering we are about to do.  */

struct my_line_entry {
  int line;
  CORE_ADDR start_pc;
  CORE_ADDR end_pc;
};

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

static int
gdb_disassemble (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  CORE_ADDR pc, low, high;
  int mixed_source_and_assembly;
  static disassemble_info di;
  static int di_initialized;

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

  if (argc != 3 && argc != 4)
    error ("wrong # args");

  if (strcmp (argv[1], "source") == 0)
    mixed_source_and_assembly = 1;
  else if (strcmp (argv[1], "nosource") == 0)
    mixed_source_and_assembly = 0;
  else
    error ("First arg must be 'source' or 'nosource'");

  low = parse_and_eval_address (argv[2]);

  if (argc == 3)
    {
      if (find_pc_partial_function (low, NULL, &low, &high) == 0)
	error ("No function contains specified address");
    }
  else
    high = parse_and_eval_address (argv[3]);

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
    if (strcmp (target_shortname, "child") == 0
	|| strcmp (target_shortname, "procfs") == 0
	|| strcmp (target_shortname, "vxprocess") == 0)
      disassemble_from_exec = 0; /* It's a child process, read inferior mem */
    else
      disassemble_from_exec = 1; /* It's remote, read the exec file */

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

      if (!symtab)
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

static void
tk_command (cmd, from_tty)
     char *cmd;
     int from_tty;
{
  int retval;
  char *result;
  struct cleanup *old_chain;

  /* Catch case of no argument, since this will make the tcl interpreter dump core. */
  if (cmd == NULL)
    error_no_arg ("tcl command to interpret");

  retval = Tcl_Eval (interp, cmd);

  result = strdup (interp->result);

  old_chain = make_cleanup (free, result);

  if (retval != TCL_OK)
    error (result);

  printf_unfiltered ("%s\n", result);

  do_cleanups (old_chain);
}

static void
cleanup_init (ignored)
     int ignored;
{
  if (interp != NULL)
    Tcl_DeleteInterp (interp);
  interp = NULL;
}

/* Come here during long calculations to check for GUI events.  Usually invoked
   via the QUIT macro.  */

static void
gdbtk_interactive ()
{
  /* Tk_DoOneEvent (TK_DONT_WAIT|TK_IDLE_EVENTS); */
}

/* Come here when there is activity on the X file descriptor. */

static void
x_event (signo)
     int signo;
{
  /* Process pending events */

  while (Tcl_DoOneEvent (TCL_DONT_WAIT|TCL_ALL_EVENTS) != 0);
}

#ifdef __CYGWIN32__

/* For Cygwin32, we use a timer to periodically check for Windows
   messages.  FIXME: It would be better to not poll, but to instead
   rewrite the target_wait routines to serve as input sources.
   Unfortunately, that will be a lot of work.  */

static void
gdbtk_start_timer ()
{
  sigset_t nullsigmask;
  struct sigaction action;
  struct itimerval it;

  sigemptyset (&nullsigmask);

  action.sa_handler = x_event;
  action.sa_mask = nullsigmask;
  action.sa_flags = 0;
  sigaction (SIGALRM, &action, NULL);

  it.it_interval.tv_sec = 0;
  /* Check for messages twice a second.  */
  it.it_interval.tv_usec = 500 * 1000;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 500 * 1000;

  setitimer (ITIMER_REAL, &it, NULL);
}

static void
gdbtk_stop_timer ()
{
  sigset_t nullsigmask;
  struct sigaction action;
  struct itimerval it;

  sigemptyset (&nullsigmask);

  action.sa_handler = SIG_IGN;
  action.sa_mask = nullsigmask;
  action.sa_flags = 0;
  sigaction (SIGALRM, &action, NULL);

  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 0;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 0;
  setitimer (ITIMER_REAL, &it, NULL);
}

#endif

/* This hook function is called whenever we want to wait for the
   target.  */

static int
gdbtk_wait (pid, ourstatus)
     int pid;
     struct target_waitstatus *ourstatus;
{
#ifndef WINNT
  struct sigaction action;
  static sigset_t nullsigmask = {0};


#ifndef SA_RESTART
  /* Needed for SunOS 4.1.x */
#define SA_RESTART 0
#endif

  action.sa_handler = x_event;
  action.sa_mask = nullsigmask;
  action.sa_flags = SA_RESTART;
  sigaction(SIGIO, &action, NULL);
#endif /* WINNT */

#ifdef __CYGWIN32__
  gdbtk_start_timer ();
#endif

  pid = target_wait (pid, ourstatus);

#ifdef __CYGWIN32__
  gdbtk_stop_timer ();
#endif

#ifndef WINNT
  action.sa_handler = SIG_IGN;
  sigaction(SIGIO, &action, NULL); 
#endif

  return pid;
}

/* This is called from execute_command, and provides a wrapper around
   various command routines in a place where both protocol messages and
   user input both flow through.  Mostly this is used for indicating whether
   the target process is running or not.
*/

static void
gdbtk_call_command (cmdblk, arg, from_tty)
     struct cmd_list_element *cmdblk;
     char *arg;
     int from_tty;
{
  running_now = 0;
  if (cmdblk->class == class_run || cmdblk->class == class_trace)
    {
      running_now = 1;
      Tcl_Eval (interp, "gdbtk_tcl_busy");
      (*cmdblk->function.cfunc)(arg, from_tty);
      running_now = 0;
      Tcl_Eval (interp, "gdbtk_tcl_idle");
    }
  else
    (*cmdblk->function.cfunc)(arg, from_tty);
}

/* This function is called instead of gdb's internal command loop.  This is the
   last chance to do anything before entering the main Tk event loop. */

static void
tk_command_loop ()
{
  extern GDB_FILE *instream;

  /* We no longer want to use stdin as the command input stream */
  instream = NULL;

  if (Tcl_Eval (interp, "gdbtk_tcl_preloop") != TCL_OK)
    {
      char *msg;

      /* Force errorInfo to be set up propertly.  */
      Tcl_AddErrorInfo (interp, "");

      msg = Tcl_GetVar (interp, "errorInfo", TCL_GLOBAL_ONLY);
#ifdef _WIN32
      MessageBox (NULL, msg, NULL, MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      fputs_unfiltered (msg, gdb_stderr);
#endif
    }

#ifdef _WIN32
  close_bfds ();
#endif

  Tk_MainLoop ();
}

/* gdbtk_init installs this function as a final cleanup.  */

static void
gdbtk_cleanup (dummy)
     PTR dummy;
{
  Tcl_Finalize ();
}

/* Initialize gdbtk.  */

static void
gdbtk_init ( argv0 )
     char *argv0;
{
  struct cleanup *old_chain;
  char *lib, *gdbtk_lib, *gdbtk_lib_tmp, *gdbtk_file;
  int i, found_main;
#ifndef WINNT
  struct sigaction action;
  static sigset_t nullsigmask = {0};
#endif
#ifdef IDE
  /* start-sanitize-ide */
  struct ide_event_handle *h;
  const char *errmsg;
  char *libexecdir;
  /* end-sanitize-ide */
#endif 

  /* If there is no DISPLAY environment variable, Tk_Init below will fail,
     causing gdb to abort.  If instead we simply return here, gdb will
     gracefully degrade to using the command line interface. */

#ifndef WINNT
  if (getenv ("DISPLAY") == NULL)
    return;
#endif

  old_chain = make_cleanup (cleanup_init, 0);

  /* First init tcl and tk. */
  Tcl_FindExecutable (argv0); 
  interp = Tcl_CreateInterp ();

  if (!interp)
    error ("Tcl_CreateInterp failed");

  if (Tcl_Init(interp) != TCL_OK)
    error ("Tcl_Init failed: %s", interp->result);

  make_final_cleanup (gdbtk_cleanup, NULL);

  /* Initialize the Paths variable.  */
#ifdef IDE
  if (ide_initialize_paths (interp, "gdbtcl") != TCL_OK)
    error ("ide_initialize_paths failed: %s", interp->result);

  /* start-sanitize-ide */
  /* Find the directory where we expect to find idemanager.  We ignore
     errors since it doesn't really matter if this fails.  */
  libexecdir = Tcl_GetVar2 (interp, "Paths", "libexecdir", TCL_GLOBAL_ONLY);

  IluTk_Init ();

  h = ide_event_init_from_environment (&errmsg, libexecdir);
  if (h == NULL)
    {
      Tcl_AppendResult (interp, "can't initialize event system: ", errmsg,
			(char *) NULL);
      fprintf(stderr, "WARNING: ide_event_init_client failed: %s\n", interp->result);

      Tcl_SetVar (interp, "GDBTK_IDE", "0", 0);
    }
  else 
    {
      if (ide_create_tclevent_command (interp, h) != TCL_OK)
	error ("ide_create_tclevent_command failed: %s", interp->result);

      if (ide_create_edit_command (interp, h) != TCL_OK)
	error ("ide_create_edit_command failed: %s", interp->result);
      
      if (ide_create_property_command (interp, h) != TCL_OK)
	error ("ide_create_property_command failed: %s", interp->result);

      if (ide_create_build_command (interp, h) != TCL_OK)
	error ("ide_create_build_command failed: %s", interp->result);

      if (ide_create_window_register_command (interp, h, "gdb-restore")
	  != TCL_OK)
	error ("ide_create_window_register_command failed: %s",
	       interp->result);

      if (ide_create_window_command (interp, h) != TCL_OK)
	error ("ide_create_window_command failed: %s", interp->result);

      if (ide_create_exit_command (interp, h) != TCL_OK)
	error ("ide_create_exit_command failed: %s", interp->result);

      if (ide_create_help_command (interp) != TCL_OK)
	error ("ide_create_help_command failed: %s", interp->result);

      /*
	if (ide_initialize (interp, "gdb") != TCL_OK)
	error ("ide_initialize failed: %s", interp->result);
      */

      Tcl_SetVar (interp, "GDBTK_IDE", "1", 0);
      Tcl_SetVar (interp, "IDE", "1", TCL_GLOBAL_ONLY);
    }
  /* end-sanitize-ide */
#else
  Tcl_SetVar (interp, "GDBTK_IDE", "0", 0);
#endif /* IDE */

  /* We don't want to open the X connection until we've done all the
     IDE initialization.  Otherwise, goofy looking unfinished windows
     pop up when ILU drops into the TCL event loop.  */

  if (Tk_Init(interp) != TCL_OK)
    error ("Tk_Init failed: %s", interp->result);

  if (Itcl_Init(interp) == TCL_ERROR) 
    error ("Itcl_Init failed: %s", interp->result);

  if (Tix_Init(interp) != TCL_OK)
    error ("Tix_Init failed: %s", interp->result);

#ifdef __CYGWIN32__
  /* On Windows, create a sizebox widget command */
  if (ide_create_sizebox_command (interp) != TCL_OK)
    error ("sizebox creation failed");
  if (ide_create_winprint_command (interp) != TCL_OK)
    error ("windows print code initialization failed");
  /* start-sanitize-ide */
  /* An interface to ShellExecute.  */
  if (ide_create_shell_execute_command (interp) != TCL_OK)
    error ("shell execute command initialization failed");
#endif

  Tcl_CreateCommand (interp, "gdb_cmd", call_wrapper, gdb_cmd, NULL);
  Tcl_CreateCommand (interp, "gdb_loc", call_wrapper, gdb_loc, NULL);
  Tcl_CreateCommand (interp, "gdb_path_conv", call_wrapper, gdb_path_conv, NULL);
  Tcl_CreateCommand (interp, "gdb_sourcelines", call_wrapper, gdb_sourcelines,
		     NULL);
  Tcl_CreateCommand (interp, "gdb_listfiles", call_wrapper, gdb_listfiles,
		     NULL);
  Tcl_CreateCommand (interp, "gdb_listfuncs", call_wrapper, gdb_listfuncs,
		     NULL);
  Tcl_CreateCommand (interp, "gdb_get_mem", call_wrapper, gdb_get_mem,
		     NULL);
  Tcl_CreateCommand (interp, "gdb_stop", call_wrapper, gdb_stop, NULL);
  Tcl_CreateCommand (interp, "gdb_regnames", call_wrapper, gdb_regnames, NULL);
  Tcl_CreateCommand (interp, "gdb_fetch_registers", call_wrapper,
		     gdb_fetch_registers, NULL);
  Tcl_CreateCommand (interp, "gdb_changed_register_list", call_wrapper,
		     gdb_changed_register_list, NULL);
  Tcl_CreateCommand (interp, "gdb_disassemble", call_wrapper,
		     gdb_disassemble, NULL);
  Tcl_CreateCommand (interp, "gdb_eval", call_wrapper, gdb_eval, NULL);
  Tcl_CreateCommand (interp, "gdb_get_breakpoint_list", call_wrapper,
		     gdb_get_breakpoint_list, NULL);
  Tcl_CreateCommand (interp, "gdb_get_breakpoint_info", call_wrapper,
		     gdb_get_breakpoint_info, NULL);
  Tcl_CreateCommand (interp, "gdb_clear_file", call_wrapper,
		     gdb_clear_file, NULL);
  Tcl_CreateCommand (interp, "gdb_confirm_quit", call_wrapper,
		     gdb_confirm_quit, NULL);
  Tcl_CreateCommand (interp, "gdb_force_quit", call_wrapper,
		     gdb_force_quit, NULL);
  Tcl_CreateCommand (interp, "gdb_target_has_execution",
                     gdb_target_has_execution_command,
                     NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_load_info", gdb_load_info, NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_locals", gdb_get_vars_command, 
                        (ClientData) 0, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_args", gdb_get_vars_command,
                        (ClientData) 1, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_function", gdb_get_function_command,
                        NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_line", gdb_get_line_command,
                        NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_file", gdb_get_file_command,
                        NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_tracepoint_exists",
                        gdb_tracepoint_exists_command, NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_get_tracepoint_info",
                        gdb_get_tracepoint_info, NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_actions",
                        gdb_actions_command, NULL, NULL);
  Tcl_CreateObjCommand (interp, "gdb_prompt",
                        gdb_prompt_command, NULL, NULL);
  
  command_loop_hook = tk_command_loop;
  print_frame_info_listing_hook =
    (void (*) PARAMS ((struct symtab *, int, int, int))) null_routine;
  query_hook = gdbtk_query;
  flush_hook = gdbtk_flush;
  create_breakpoint_hook = gdbtk_create_breakpoint;
  delete_breakpoint_hook = gdbtk_delete_breakpoint;
  modify_breakpoint_hook = gdbtk_modify_breakpoint;
  interactive_hook = gdbtk_interactive;
  target_wait_hook = gdbtk_wait;
  call_command_hook = gdbtk_call_command;
  readline_begin_hook = gdbtk_readline_begin;
  readline_hook = gdbtk_readline;
  readline_end_hook = gdbtk_readline_end;
  ui_load_progress_hook = gdbtk_load_hash;
  create_tracepoint_hook = gdbtk_create_tracepoint;
  delete_tracepoint_hook = gdbtk_delete_tracepoint;

#ifndef WINNT
  /* Get the file descriptor for the X server */

  x_fd = ConnectionNumber (Tk_Display (Tk_MainWindow (interp)));

  /* Setup for I/O interrupts */

  action.sa_mask = nullsigmask;
  action.sa_flags = 0;
  action.sa_handler = SIG_IGN;
  sigaction(SIGIO, &action, NULL);

#ifdef FIOASYNC
  i = 1;
  if (ioctl (x_fd, FIOASYNC, &i))
    perror_with_name ("gdbtk_init: ioctl FIOASYNC failed");

#ifdef SIOCSPGRP
  i = getpid();
  if (ioctl (x_fd, SIOCSPGRP, &i))
    perror_with_name ("gdbtk_init: ioctl SIOCSPGRP failed");

#else
#ifdef F_SETOWN
  i = getpid();
  if (fcntl (x_fd, F_SETOWN, i))
    perror_with_name ("gdbtk_init: fcntl F_SETOWN failed");
#endif	/* F_SETOWN */
#endif	/* !SIOCSPGRP */
#else
#ifndef WINNT
  if (ioctl (x_fd,  I_SETSIG, S_INPUT|S_RDNORM) < 0)
    perror_with_name ("gdbtk_init: ioctl I_SETSIG failed");
#endif

#endif /* ifndef FIOASYNC */
#endif /* WINNT */

  add_com ("tk", class_obscure, tk_command,
	   "Send a command directly into tk.");

  Tcl_LinkVar (interp, "disassemble-from-exec", (char *)&disassemble_from_exec,
	       TCL_LINK_INT);

  /* find the gdb tcl library and source main.tcl */

  gdbtk_lib = getenv ("GDBTK_LIBRARY");
  if (!gdbtk_lib)
    if (access ("gdbtcl/main.tcl", R_OK) == 0)
      gdbtk_lib = "gdbtcl";
    else
      gdbtk_lib = GDBTK_LIBRARY;

  gdbtk_lib_tmp = xstrdup (gdbtk_lib);

  found_main = 0;
  /* see if GDBTK_LIBRARY is a path list */
  lib = strtok (gdbtk_lib_tmp, GDBTK_PATH_SEP);
  do
    {
      if (Tcl_VarEval (interp, "lappend auto_path ", lib, NULL) != TCL_OK)
	{
	  fputs_unfiltered (Tcl_GetVar (interp, "errorInfo", 0), gdb_stderr);
	  error ("");
	}
      if (!found_main)
	{
	  gdbtk_file = concat (lib, "/main.tcl", (char *) NULL);
	  if (access (gdbtk_file, R_OK) == 0)
	    {
	      found_main++;
	      Tcl_SetVar (interp, "GDBTK_LIBRARY", lib, 0);
	    }
	}
     } 
  while ((lib = strtok (NULL, ":")) != NULL);

  free (gdbtk_lib_tmp);

  if (!found_main)
    {
      /* Try finding it with the auto path.  */

      static const char script[] ="\
proc gdbtk_find_main {} {\n\
  global auto_path GDBTK_LIBRARY\n\
  foreach dir $auto_path {\n\
    set f [file join $dir main.tcl]\n\
    if {[file exists $f]} then {\n\
      set GDBTK_LIBRARY $dir\n\
      return $f\n\
    }\n\
  }\n\
  return ""\n\
}\n\
gdbtk_find_main";

      if (Tcl_GlobalEval (interp, (char *) script) != TCL_OK)
	{
	  fputs_unfiltered (Tcl_GetVar (interp, "errorInfo", 0), gdb_stderr);
	  error ("");
	}

      if (interp->result[0] != '\0')
	{
	  gdbtk_file = xstrdup (interp->result);
	  found_main++;
	}
    }

  if (!found_main)
    {
      fputs_unfiltered_hook = NULL; /* Force errors to stdout/stderr */
      if (getenv("GDBTK_LIBRARY"))
	{
	  fprintf_unfiltered (stderr, "Unable to find main.tcl in %s\n",getenv("GDBTK_LIBRARY"));
	  fprintf_unfiltered (stderr, 
			      "Please set GDBTK_LIBRARY to a path that includes the GDB tcl files.\n");
	}
      else
	{
	  fprintf_unfiltered (stderr, "Unable to find main.tcl in %s\n", GDBTK_LIBRARY);
	  fprintf_unfiltered (stderr, "You might want to set GDBTK_LIBRARY\n");	  
	}
      error("");
    }

/* Defer setup of fputs_unfiltered_hook to near the end so that error messages
   prior to this point go to stdout/stderr.  */

  fputs_unfiltered_hook = gdbtk_fputs;

  if (Tcl_EvalFile (interp, gdbtk_file) != TCL_OK)
    {
      char *msg;

      /* Force errorInfo to be set up propertly.  */
      Tcl_AddErrorInfo (interp, "");

      msg = Tcl_GetVar (interp, "errorInfo", TCL_GLOBAL_ONLY);

      fputs_unfiltered_hook = NULL; /* Force errors to stdout/stderr */

#ifdef _WIN32
      MessageBox (NULL, msg, NULL, MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      fputs_unfiltered (msg, gdb_stderr);
#endif

      error ("");
    }

#ifdef IDE
  /* start-sanitize-ide */
  /* Don't do this until we have initialized.  Otherwise, we may get a
     run command before we are ready for one.  */
  if (ide_run_server_init (interp, h) != TCL_OK)
    error ("ide_run_server_init failed: %s", interp->result);
  /* end-sanitize-ide */
#endif

  free (gdbtk_file);

  discard_cleanups (old_chain);
}

static int
gdb_target_has_execution_command (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int result = 0;

  if (target_has_execution && inferior_pid != 0)
    result = 1;

  Tcl_SetIntObj (Tcl_GetObjResult (interp), result);
  return TCL_OK;
}

/* gdb_load_info - returns information about the file about to be downloaded */

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
   Tcl_Obj *res[16];
   int i = 0;

   char *filename = Tcl_GetStringFromObj (objv[1], NULL);

   loadfile_bfd = bfd_openr (filename, gnutarget);
   if (loadfile_bfd == NULL)
     {
       Tcl_SetObjResult (interp, Tcl_NewStringObj ("Open failed", -1));
       return TCL_ERROR;
     }
   old_cleanups = make_cleanup (bfd_close, loadfile_bfd);
   
   if (!bfd_check_format (loadfile_bfd, bfd_object)) 
     {
       Tcl_SetObjResult (interp, Tcl_NewStringObj ("Bad Object File", -1));
       return TCL_ERROR;
    }

   for (s = loadfile_bfd->sections; s; s = s->next) 
     {
       if (s->flags & SEC_LOAD) 
	 {
	   bfd_size_type size = bfd_get_section_size_before_reloc (s);
	   if (size > 0)
	     {
	       ob[0] = Tcl_NewStringObj((char *)bfd_get_section_name(loadfile_bfd, s), -1);
	       ob[1] = Tcl_NewLongObj ((long)size);
	       res[i++] = Tcl_NewListObj (2, ob);
	     }
	 }
     }
   
   Tcl_SetObjResult (interp, Tcl_NewListObj (i, res));
   do_cleanups (old_cleanups);
   return TCL_OK;
}


int
gdbtk_load_hash (section, num)
     char *section;
     unsigned long num;
{
  int result;
  char buf[128];
  sprintf (buf, "download_hash %s %ld", section, num);
  result = Tcl_Eval (interp, buf); 
  return result;
}

/* gdb_get_vars_command -
 *
 * Implements the "gdb_get_locals" and "gdb_get_args" tcl commands. This
 * function sets the Tcl interpreter's result to a list of variable names
 * depending on clientData. If clientData is one, the result is a list of 
 * arguments; zero returns a list of locals -- all relative to the block
 * specified as an argument to the command. Valid commands include
 * anything decode_line_1 can handle (like "main.c:2", "*0x02020202",
 * and "main").
 */
static int
gdb_get_vars_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  Tcl_Obj *result;
  struct symtabs_and_lines sals;
  struct symbol *sym;
  struct block *block;
  char **canonical, *args;
  int i, nsyms, arguments;

  if (objc != 2)
    {
      Tcl_AppendResult (interp,
                        "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " function:line|function|line|*addr\"");
      return TCL_ERROR;
    }

  arguments = (int) clientData;
  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);
  if (sals.nelts == 0)
    {
      Tcl_AppendResult (interp,
                        "error decoding line", NULL);
      return TCL_ERROR;
    }

  /* Initialize a list that will hold the results */
  result = Tcl_NewListObj (0, NULL);

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
          case LOC_STATIC:	      /* static                */
          case LOC_REGISTER:      /* register              */
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
              Tcl_ListObjAppendElement (interp, result,
                                        Tcl_NewStringObj (SYMBOL_NAME (sym), -1));
            break;
          case LOC_LOCAL:	      /* stack local           */
          case LOC_BASEREG:	      /* basereg local         */
            if (!arguments)
              Tcl_ListObjAppendElement (interp, result,
                                        Tcl_NewStringObj (SYMBOL_NAME (sym), -1));
            break;
          }
        }
      if (BLOCK_FUNCTION (block))
        break;
      else
        block = BLOCK_SUPERBLOCK (block);
    }
  
  Tcl_SetObjResult (interp, result);
  return TCL_OK;
}

static int
gdb_get_line_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  Tcl_Obj *result;
  struct symtabs_and_lines sals;
  char *args, **canonical;
  
  if (objc != 2)
    {
      Tcl_AppendResult (interp, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"");
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      Tcl_SetObjResult (interp, Tcl_NewIntObj (sals.sals[0].line));
      return TCL_OK;
    }

    Tcl_SetResult (interp, "N/A", TCL_STATIC);
    return TCL_OK;
}

static int
gdb_get_file_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  Tcl_Obj *result;
  struct symtabs_and_lines sals;
  char *args, **canonical;
  
  if (objc != 2)
    {
      Tcl_AppendResult (interp, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"");
      return TCL_ERROR;
    }

  args = Tcl_GetStringFromObj (objv[1], NULL);
  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);  
  if (sals.nelts == 1)
    {
      Tcl_SetResult (interp, sals.sals[0].symtab->filename, TCL_VOLATILE);
      return TCL_OK;
    }

    Tcl_SetResult (interp, "N/A", TCL_STATIC);
    return TCL_OK;
}

static int
gdb_get_function_command (clientData, interp, objc, objv)
     ClientData clientData;
     Tcl_Interp *interp;
     int objc;
     Tcl_Obj *CONST objv[];
{
  Tcl_Obj *result;
  char *function;
  struct symtabs_and_lines sals;
  char *args, **canonical;

  if (objc != 2)
    {
      Tcl_AppendResult (interp, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " linespec\"");
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
          Tcl_SetResult (interp, function, TCL_VOLATILE);
          return TCL_OK;
        }
    }
  
  Tcl_SetResult (interp, "N/A", TCL_STATIC);
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
  struct command_line *cmd;
  int tpnum;
  struct tracepoint *tp;
  struct action_line *al;
  Tcl_Obj *list, *action_list;
  char tmp[19];
  
  if (objc != 2)
    error ("wrong # args");

  Tcl_GetIntFromObj (NULL, objv[1], &tpnum);

  ALL_TRACEPOINTS (tp)
    if (tp->number == tpnum)
      break;

  if (tp == NULL)
    error ("Tracepoint #%d does not exist", tpnum);

  list = Tcl_NewListObj (0, NULL);
  if (tp->source_file != NULL)
    Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj (tp->source_file, -1));
  else
    Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj ("N/A", -1));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->line_number));
  /* the function part is not currently used by the frontend */
  Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj ("function", -1));
  sprintf (tmp, "0x%08x", tp->address);
  Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj (tmp, -1));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->enabled));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->pass_count));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->step_count));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->thread));
  Tcl_ListObjAppendElement (interp, list, Tcl_NewIntObj (tp->hit_count));

  /* Append a list of actions */
  action_list = Tcl_NewListObj (0, NULL);
  for (al = tp->actions; al != NULL; al = al->next)
    {
      Tcl_ListObjAppendElement (interp, action_list,
                                Tcl_NewStringObj (al->action, -1));
    }
  Tcl_ListObjAppendElement (interp, list, action_list);

  Tcl_SetObjResult (interp, list);
  return TCL_OK;
}

static void
gdbtk_create_tracepoint (tp)
  struct tracepoint *tp;
{
  tracepoint_notify (tp, "create");
}

static void
gdbtk_delete_tracepoint (tp)
  struct tracepoint *tp;
{
  tracepoint_notify (tp, "delete");
}

static void
tracepoint_notify(tp, action)
     struct tracepoint *tp;
     const char *action;
{
  char buf[256];
  char *source;
  int v;

  /* We ensure that ACTION contains no special Tcl characters, so we
     can do this.  */
  if (tp->source_file != NULL)
    source = tp->source_file;
  else
    source = "N/A";
  sprintf (buf, "gdbtk_tcl_tracepoint %s %d 0x%lx %d {%s}", action, tp->number, 
	   (long)tp->address, tp->line_number, source);

  v = Tcl_Eval (interp, buf);

  if (v != TCL_OK)
    {
      gdbtk_fputs (interp->result, gdb_stdout);
      gdbtk_fputs ("\n", gdb_stdout);
    }
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
              else if (tp->source_file != NULL
                       && strcmp (tp->source_file, file) == 0
                       && sals.sals[0].line == tp->line_number)
                
                result = tp->number;
            }
        }
    }
  if (file != NULL)
    free (file);
  return result;
}

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
  struct action_line *next = NULL, *temp;

  if (objc != 3)
    {
      Tcl_AppendResult (interp, "wrong # args: should be: \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " number actions\"");
      return TCL_ERROR;
    }

  args = number = Tcl_GetStringFromObj (objv[1], NULL);
  tp = get_tracepoint_by_number (&args);
  if (tp == NULL)
    {
      Tcl_AppendResult (interp, "Tracepoint \"", number, "\" does not exist");
      return TCL_ERROR;
    }

  /* Free any existing actions */
  for (temp = tp->actions; temp != NULL; temp = temp->next)
    {
      if (temp->action)
        free (temp->action);
      free (temp);
    }

  Tcl_ListObjGetElements (interp, objv[2], &nactions, &actions);
  for (i = 0; i < nactions; i++)
    {
      temp = xmalloc (sizeof (struct action_line));
      temp->next = NULL;
      action = Tcl_GetStringFromObj (actions[i], &len);
      temp->action = savestring (action, len);
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
      Tcl_AppendResult (interp, "wrong # of args: should be \"",
                        Tcl_GetStringFromObj (objv[0], NULL),
                        " function:line|function|line|*addr\"");
      return TCL_ERROR;
    }
  
  args = Tcl_GetStringFromObj (objv[1], NULL);
  
  Tcl_SetObjResult (interp, Tcl_NewIntObj (tracepoint_exists (args)));
  return TCL_OK;
}

/* Return the prompt to the interpreter */
static int
gdb_prompt_command (clientData, interp, objc, objv)
  ClientData clientData;
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj *CONST objv[];
{
  Tcl_SetResult (interp, get_prompt (), TCL_VOLATILE);
  return TCL_OK;
}

/* Come here during initialize_all_files () */

void
_initialize_gdbtk ()
{
  if (use_windows)
    {
      /* Tell the rest of the world that Gdbtk is now set up. */

      init_ui_hook = gdbtk_init;
    }
}
