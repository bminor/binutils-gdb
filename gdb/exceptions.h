/* Exception (throw catch) mechanism, for GDB, the GNU debugger.

   Copyright 1986, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005 Free
   Software Foundation, Inc.

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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/* Reasons for calling throw_exceptions().  NOTE: all reason values
   must be less than zero.  enum value 0 is reserved for internal use
   as the return value from an initial setjmp().  The function
   catch_exceptions() reserves values >= 0 as legal results from its
   wrapped function.  */

enum return_reason
  {
    /* User interrupt.  */
    RETURN_QUIT = -2,
    /* Any other error.  */
    RETURN_ERROR
  };

#define RETURN_MASK(reason)	(1 << (int)(-reason))
#define RETURN_MASK_QUIT	RETURN_MASK (RETURN_QUIT)
#define RETURN_MASK_ERROR	RETURN_MASK (RETURN_ERROR)
#define RETURN_MASK_ALL		(RETURN_MASK_QUIT | RETURN_MASK_ERROR)
typedef int return_mask;

/* Describe all exceptions.  */

enum errors {
  NO_ERROR,
  /* Any generic error, the corresponding text is in
     exception.message.  */
  GENERIC_ERROR,
  NOT_FOUND_ERROR,
  /* Add more errors here.  */
  NR_ERRORS
};

struct exception
{
  enum return_reason reason;
  enum errors error;
  const char *message;
};

/* A pre-defined non-exception.  */
extern const struct exception exception_none;

/* If E is an exception, print it's error message on the specified
   stream.  */
extern void exception_print (struct ui_file *file, const char *pre_print,
			     struct exception e);

/* Throw an exception (as described by "struct exception").  Will
   execute a LONG JUMP to the inner most containing exception handler
   established using catch_exceptions() (or similar).

   Code normally throws an exception using error() et.al.  For various
   reaons, GDB also contains code that throws an exception directly.
   For instance, the remote*.c targets contain CNTRL-C signal handlers
   that propogate the QUIT event up the exception chain.  ``This could
   be a good thing or a dangerous thing.'' -- the Existential
   Wombat.  */

extern NORETURN void throw_exception (struct exception exception) ATTR_NORETURN;
extern NORETURN void throw_reason (enum return_reason reason) ATTR_NORETURN;
extern NORETURN void throw_verror (enum errors, const char *fmt,
				   va_list ap) ATTR_NORETURN;
extern NORETURN void throw_vfatal (const char *fmt, va_list ap) ATTR_NORETURN;
extern NORETURN void throw_error (enum errors error, const char *fmt,
				  ...) ATTR_NORETURN ATTR_FORMAT (printf, 2, 3);

/* Call FUNC(UIOUT, FUNC_ARGS) but wrapped within an exception
   handler.  If an exception (enum return_reason) is thrown using
   throw_exception() than all cleanups installed since
   catch_exceptions() was entered are invoked, the (-ve) exception
   value is then returned by catch_exceptions.  If FUNC() returns
   normally (with a postive or zero return value) then that value is
   returned by catch_exceptions().  It is an internal_error() for
   FUNC() to return a negative value.

   For the period of the FUNC() call: UIOUT is installed as the output
   builder; ERRSTRING is installed as the error/quit message; and a
   new cleanup_chain is established.  The old values are restored
   before catch_exceptions() returns.

   The variant catch_exceptions_with_msg() is the same as
   catch_exceptions() but adds the ability to return an allocated
   copy of the gdb error message.  This is used when a silent error is 
   issued and the caller wants to manually issue the error message.

   FIXME; cagney/2001-08-13: The need to override the global UIOUT
   builder variable should just go away.

   This function superseeds catch_errors().

   This function uses SETJMP() and LONGJUMP().  */

struct ui_out;
typedef int (catch_exceptions_ftype) (struct ui_out *ui_out, void *args);
extern int catch_exceptions (struct ui_out *uiout,
			     catch_exceptions_ftype *func, void *func_args,
			     char *errstring, return_mask mask);
typedef void (catch_exception_ftype) (struct ui_out *ui_out, void *args);
extern int catch_exceptions_with_msg (struct ui_out *uiout,
			     	      catch_exceptions_ftype *func, 
			     	      void *func_args,
			     	      char *errstring, char **gdberrmsg,
				      return_mask mask);

/* This function, in addition, suppresses the printing of the captured
   error message.  It's up to the client to print it.  */

extern struct exception catch_exception (struct ui_out *uiout,
					 catch_exception_ftype *func,
					 void *func_args,
					 return_mask mask);

/* If CATCH_ERRORS_FTYPE throws an error, catch_errors() returns zero
   otherwize the result from CATCH_ERRORS_FTYPE is returned. It is
   probably useful for CATCH_ERRORS_FTYPE to always return a non-zero
   value. It's unfortunate that, catch_errors() does not return an
   indication of the exact exception that it caught - quit_flag might
   help.

   This function is superseeded by catch_exceptions().  */

typedef int (catch_errors_ftype) (void *);
extern int catch_errors (catch_errors_ftype *, void *, char *, return_mask);

/* Template to catch_errors() that wraps calls to command
   functions. */

typedef void (catch_command_errors_ftype) (char *, int);
extern int catch_command_errors (catch_command_errors_ftype *func, char *command, int from_tty, return_mask);

#endif
