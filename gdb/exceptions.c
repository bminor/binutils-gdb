/* Exception (throw catch) mechanism, for GDB, the GNU debugger.

   Copyright 1986, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
   Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "exceptions.h"
#include <setjmp.h>
#include "breakpoint.h"
#include "target.h"
#include "inferior.h"
#include "annotate.h"
#include "ui-out.h"
#include "gdb_assert.h"

/* One should use catch_errors rather than manipulating these
   directly.  */
#if defined(HAVE_SIGSETJMP)
#define SIGJMP_BUF		sigjmp_buf
#define SIGSETJMP(buf)		sigsetjmp((buf), 1)
#define SIGLONGJMP(buf,val)	siglongjmp((buf), (val))
#else
#define SIGJMP_BUF		jmp_buf
#define SIGSETJMP(buf)		setjmp(buf)
#define SIGLONGJMP(buf,val)	longjmp((buf), (val))
#endif

/* Where to go for throw_exception().  */
static SIGJMP_BUF *catch_return;

/* Return for reason REASON to the nearest containing catch_errors().  */

NORETURN void
throw_exception (enum return_reason reason)
{
  quit_flag = 0;
  immediate_quit = 0;

  /* Perhaps it would be cleaner to do this via the cleanup chain (not sure
     I can think of a reason why that is vital, though).  */
  bpstat_clear_actions (stop_bpstat);	/* Clear queued breakpoint commands */

  disable_current_display ();
  do_cleanups (ALL_CLEANUPS);
  if (target_can_async_p () && !target_executing)
    do_exec_cleanups (ALL_CLEANUPS);
  if (sync_execution)
    do_exec_error_cleanups (ALL_CLEANUPS);

  if (annotation_level > 1)
    switch (reason)
      {
      case RETURN_QUIT:
	annotate_quit ();
	break;
      case RETURN_ERROR:
	annotate_error ();
	break;
      }

  /* Jump to the containing catch_errors() call, communicating REASON
     to that call via setjmp's return value.  Note that REASON can't
     be zero, by definition in defs.h. */

  (NORETURN void) SIGLONGJMP (*catch_return, (int) reason);
}

/* Call FUNC() with args FUNC_UIOUT and FUNC_ARGS, catching any
   errors.  Set FUNC_CAUGHT to an ``enum return_reason'' if the
   function is aborted (using throw_exception() or zero if the
   function returns normally.  Set FUNC_VAL to the value returned by
   the function or 0 if the function was aborted.

   Must not be called with immediate_quit in effect (bad things might
   happen, say we got a signal in the middle of a memcpy to quit_return).
   This is an OK restriction; with very few exceptions immediate_quit can
   be replaced by judicious use of QUIT.

   MASK specifies what to catch; it is normally set to
   RETURN_MASK_ALL, if for no other reason than that the code which
   calls catch_errors might not be set up to deal with a quit which
   isn't caught.  But if the code can deal with it, it generally
   should be RETURN_MASK_ERROR, unless for some reason it is more
   useful to abort only the portion of the operation inside the
   catch_errors.  Note that quit should return to the command line
   fairly quickly, even if some further processing is being done.  */

/* MAYBE: cagney/1999-11-05: catch_errors() in conjunction with
   error() et.al. could maintain a set of flags that indicate the the
   current state of each of the longjmp buffers.  This would give the
   longjmp code the chance to detect a longjmp botch (before it gets
   to longjmperror()).  Prior to 1999-11-05 this wasn't possible as
   code also randomly used a SET_TOP_LEVEL macro that directly
   initialize the longjmp buffers. */

/* MAYBE: cagney/1999-11-05: Should the catch_errors and cleanups code
   be consolidated into a single file instead of being distributed
   between utils.c and top.c? */

static void
catcher (catch_exceptions_ftype *func,
	 struct ui_out *func_uiout,
	 void *func_args,
	 int *func_val,
	 enum return_reason *func_caught,
	 char *errstring,
	 char **gdberrmsg,
	 return_mask mask)
{
  SIGJMP_BUF *saved_catch;
  SIGJMP_BUF catch;
  struct cleanup *saved_cleanup_chain;
  char *saved_error_pre_print;
  char *saved_quit_pre_print;
  struct ui_out *saved_uiout;

  /* Return value from SIGSETJMP(): enum return_reason if error or
     quit caught, 0 otherwise. */
  int caught;

  /* Return value from FUNC(): Hopefully non-zero. Explicitly set to
     zero if an error quit was caught.  */
  int val;

  /* Override error/quit messages during FUNC. */

  saved_error_pre_print = error_pre_print;
  saved_quit_pre_print = quit_pre_print;

  if (mask & RETURN_MASK_ERROR)
    error_pre_print = errstring;
  if (mask & RETURN_MASK_QUIT)
    quit_pre_print = errstring;

  /* Override the global ``struct ui_out'' builder.  */

  saved_uiout = uiout;
  uiout = func_uiout;

  /* Prevent error/quit during FUNC from calling cleanups established
     prior to here. */

  saved_cleanup_chain = save_cleanups ();

  /* Call FUNC, catching error/quit events. */

  saved_catch = catch_return;
  catch_return = &catch;
  caught = SIGSETJMP (catch);
  if (!caught)
    val = (*func) (func_uiout, func_args);
  else
    {
      val = 0;
      /* If caller wants a copy of the low-level error message, make one.  
         This is used in the case of a silent error whereby the caller
         may optionally want to issue the message.  */
      if (gdberrmsg)
	*gdberrmsg = error_last_message ();
    }
  catch_return = saved_catch;

  /* FIXME: cagney/1999-11-05: A correct FUNC implementation will
     clean things up (restoring the cleanup chain) to the state they
     were just prior to the call.  Unfortunately, many FUNC's are not
     that well behaved.  This could be fixed by adding either a
     do_cleanups call (to cover the problem) or an assertion check to
     detect bad FUNCs code. */

  /* Restore the cleanup chain, the error/quit messages, and the uiout
     builder, to their original states. */

  restore_cleanups (saved_cleanup_chain);

  uiout = saved_uiout;

  if (mask & RETURN_MASK_QUIT)
    quit_pre_print = saved_quit_pre_print;
  if (mask & RETURN_MASK_ERROR)
    error_pre_print = saved_error_pre_print;

  /* Return normally if no error/quit event occurred or this catcher
     can handle this exception.  The caller analyses the func return
     values.  */

  if (!caught || (mask & RETURN_MASK (caught)))
    {
      *func_val = val;
      *func_caught = caught;
      return;
    }

  /* The caller didn't request that the event be caught, relay the
     event to the next containing catch_errors(). */

  throw_exception (caught);
}

int
catch_exceptions (struct ui_out *uiout,
		  catch_exceptions_ftype *func,
		  void *func_args,
		  char *errstring,
		  return_mask mask)
{
  int val;
  enum return_reason caught;
  catcher (func, uiout, func_args, &val, &caught, errstring, NULL, mask);
  gdb_assert (val >= 0);
  gdb_assert (caught <= 0);
  if (caught < 0)
    return caught;
  return val;
}

int
catch_exceptions_with_msg (struct ui_out *uiout,
		  	   catch_exceptions_ftype *func,
		  	   void *func_args,
		  	   char *errstring,
			   char **gdberrmsg,
		  	   return_mask mask)
{
  int val;
  enum return_reason caught;
  catcher (func, uiout, func_args, &val, &caught, errstring, gdberrmsg, mask);
  gdb_assert (val >= 0);
  gdb_assert (caught <= 0);
  if (caught < 0)
    return caught;
  return val;
}

struct catch_errors_args
{
  catch_errors_ftype *func;
  void *func_args;
};

static int
do_catch_errors (struct ui_out *uiout, void *data)
{
  struct catch_errors_args *args = data;
  return args->func (args->func_args);
}

int
catch_errors (catch_errors_ftype *func, void *func_args, char *errstring,
	      return_mask mask)
{
  int val;
  enum return_reason caught;
  struct catch_errors_args args;
  args.func = func;
  args.func_args = func_args;
  catcher (do_catch_errors, uiout, &args, &val, &caught, errstring, 
	   NULL, mask);
  if (caught != 0)
    return 0;
  return val;
}

struct captured_command_args
  {
    catch_command_errors_ftype *command;
    char *arg;
    int from_tty;
  };

static int
do_captured_command (void *data)
{
  struct captured_command_args *context = data;
  context->command (context->arg, context->from_tty);
  /* FIXME: cagney/1999-11-07: Technically this do_cleanups() call
     isn't needed.  Instead an assertion check could be made that
     simply confirmed that the called function correctly cleaned up
     after itself.  Unfortunately, old code (prior to 1999-11-04) in
     main.c was calling SET_TOP_LEVEL(), calling the command function,
     and then *always* calling do_cleanups().  For the moment we
     remain ``bug compatible'' with that old code..  */
  do_cleanups (ALL_CLEANUPS);
  return 1;
}

int
catch_command_errors (catch_command_errors_ftype * command,
		      char *arg, int from_tty, return_mask mask)
{
  struct captured_command_args args;
  args.command = command;
  args.arg = arg;
  args.from_tty = from_tty;
  return catch_errors (do_captured_command, &args, "", mask);
}
