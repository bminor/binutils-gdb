/* General utility routines for GDB, the GNU debugger.
   Copyright (C) 1986, 1989, 1990, 1991 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <pwd.h>
#include "defs.h"
#include "param.h"
#include "signals.h"
#include "gdbcmd.h"
#include "terminal.h"
#include <varargs.h>
#include <ctype.h>
#include <string.h>
#include "bfd.h"
#include "target.h"

extern volatile void return_to_top_level ();
extern volatile void exit ();
extern char *gdb_readline ();
extern char *getenv();
extern char *malloc();
extern char *realloc();

/* If this definition isn't overridden by the header files, assume
   that isatty and fileno exist on this system.  */
#ifndef ISATTY
#define ISATTY(FP)	(isatty (fileno (FP)))
#endif

#ifdef MISSING_VPRINTF
#ifdef __GNU_LIBRARY
#undef MISSING_VPRINTF
#else  /* !__GNU_LIBRARY */

#ifndef vfprintf
#define vfprintf(file, format, ap) _doprnt (format, ap, file)
#endif /* vfprintf */

#ifndef vprintf
/* Can't #define it since printcmd.c needs it */
void
vprintf (format, ap)
     char *format; void *ap;
{
  vfprintf (stdout, format, ap);
}
#endif /* vprintf */

#endif /* GNU_LIBRARY */
#endif /* MISSING_VPRINTF */

void error ();
void fatal ();

/* Chain of cleanup actions established with make_cleanup,
   to be executed if an error happens.  */

static struct cleanup *cleanup_chain;

/* Nonzero means a quit has been requested.  */

int quit_flag;

/* Nonzero means quit immediately if Control-C is typed now,
   rather than waiting until QUIT is executed.  */

int immediate_quit;

/* Nonzero means that encoded C++ names should be printed out in their
   C++ form rather than raw.  */

int demangle = 1;

/* Nonzero means that encoded C++ names should be printed out in their
   C++ form even in assembler language displays.  If this is set, but
   DEMANGLE is zero, names are printed raw, i.e. DEMANGLE controls.  */

int asm_demangle = 0;

/* Nonzero means that strings with character values >0x7F should be printed
   as octal escapes.  Zero means just print the value (e.g. it's an
   international character, and the terminal or window can cope.)  */

int sevenbit_strings = 0;

/* Add a new cleanup to the cleanup_chain,
   and return the previous chain pointer
   to be passed later to do_cleanups or discard_cleanups.
   Args are FUNCTION to clean up with, and ARG to pass to it.  */

struct cleanup *
make_cleanup (function, arg)
     void (*function) ();
     int arg;
{
  register struct cleanup *new
    = (struct cleanup *) xmalloc (sizeof (struct cleanup));
  register struct cleanup *old_chain = cleanup_chain;

  new->next = cleanup_chain;
  new->function = function;
  new->arg = arg;
  cleanup_chain = new;

  return old_chain;
}

/* Discard cleanups and do the actions they describe
   until we get back to the point OLD_CHAIN in the cleanup_chain.  */

void
do_cleanups (old_chain)
     register struct cleanup *old_chain;
{
  register struct cleanup *ptr;
  while ((ptr = cleanup_chain) != old_chain)
    {
      (*ptr->function) (ptr->arg);
      cleanup_chain = ptr->next;
      free (ptr);
    }
}

/* Discard cleanups, not doing the actions they describe,
   until we get back to the point OLD_CHAIN in the cleanup_chain.  */

void
discard_cleanups (old_chain)
     register struct cleanup *old_chain;
{
  register struct cleanup *ptr;
  while ((ptr = cleanup_chain) != old_chain)
    {
      cleanup_chain = ptr->next;
      free (ptr);
    }
}

/* Set the cleanup_chain to 0, and return the old cleanup chain.  */
struct cleanup *
save_cleanups ()
{
  struct cleanup *old_chain = cleanup_chain;

  cleanup_chain = 0;
  return old_chain;
}

/* Restore the cleanup chain from a previously saved chain.  */
void
restore_cleanups (chain)
     struct cleanup *chain;
{
  cleanup_chain = chain;
}

/* This function is useful for cleanups.
   Do

     foo = xmalloc (...);
     old_chain = make_cleanup (free_current_contents, &foo);

   to arrange to free the object thus allocated.  */

void
free_current_contents (location)
     char **location;
{
  free (*location);
}

/* Print an error message and return to command level.
   The first argument STRING is the error message, used as a fprintf string,
   and the remaining args are passed as arguments to it.  */

/* VARARGS */
void
error (va_alist)
     va_dcl
{
  va_list args;
  char *string;

  va_start (args);
  target_terminal_ours ();
  fflush (stdout);
  string = va_arg (args, char *);
  vfprintf (stderr, string, args);
  fprintf (stderr, "\n");
  va_end (args);
  return_to_top_level ();
}

/* Print an error message and exit reporting failure.
   This is for a error that we cannot continue from.
   The arguments are printed a la printf.  */

/* VARARGS */
void
fatal (va_alist)
     va_dcl
{
  va_list args;
  char *string;

  va_start (args);
  string = va_arg (args, char *);
  fprintf (stderr, "gdb: ");
  vfprintf (stderr, string, args);
  fprintf (stderr, "\n");
  va_end (args);
  exit (1);
}

/* Print an error message and exit, dumping core.
   The arguments are printed a la printf ().  */
/* VARARGS */
void
fatal_dump_core (va_alist)
     va_dcl
{
  va_list args;
  char *string;

  va_start (args);
  string = va_arg (args, char *);
  /* "internal error" is always correct, since GDB should never dump
     core, no matter what the input.  */
  fprintf (stderr, "gdb internal error: ");
  vfprintf (stderr, string, args);
  fprintf (stderr, "\n");
  va_end (args);

  signal (SIGQUIT, SIG_DFL);
  kill (getpid (), SIGQUIT);
  /* We should never get here, but just in case...  */
  exit (1);
}

/* Memory management stuff (malloc friends).  */

#if defined (NO_MALLOC_CHECK)
void
init_malloc ()
{}
#else /* Have mcheck().  */
static void
malloc_botch ()
{
  fatal_dump_core ("Memory corruption");
}

void
init_malloc ()
{
  mcheck (malloc_botch);
}
#endif /* Have mcheck().  */

/* Like malloc but get error if no storage available.  */

#ifdef __STDC__
void *
#else
char *
#endif
xmalloc (size)
     long size;
{
  register char *val;

  /* At least one place (dbxread.c:condense_misc_bunches where misc_count == 0)
     GDB wants to allocate zero bytes.  */
  if (size == 0)
    return NULL;
  
  val = (char *) malloc (size);
  if (!val)
    fatal ("virtual memory exhausted.", 0);
  return val;
}

/* Like realloc but get error if no storage available.  */

#ifdef __STDC__
void *
#else
char *
#endif
xrealloc (ptr, size)
     char *ptr;
     long size;
{
  register char *val = (char *) realloc (ptr, size);
  if (!val)
    fatal ("virtual memory exhausted.", 0);
  return val;
}

/* Print the system error message for errno, and also mention STRING
   as the file name for which the error was encountered.
   Then return to command level.  */

void
perror_with_name (string)
     char *string;
{
  extern int sys_nerr;
  extern char *sys_errlist[];
  char *err;
  char *combined;

  if (errno < sys_nerr)
    err = sys_errlist[errno];
  else
    err = "unknown error";

  combined = (char *) alloca (strlen (err) + strlen (string) + 3);
  strcpy (combined, string);
  strcat (combined, ": ");
  strcat (combined, err);

  /* I understand setting these is a matter of taste.  Still, some people
     may clear errno but not know about bfd_error.  Doing this here is not
     unreasonable. */
  bfd_error = no_error;
  errno = 0;

  error ("%s.", combined);
}

/* Print the system error message for ERRCODE, and also mention STRING
   as the file name for which the error was encountered.  */

void
print_sys_errmsg (string, errcode)
     char *string;
     int errcode;
{
  extern int sys_nerr;
  extern char *sys_errlist[];
  char *err;
  char *combined;

  if (errcode < sys_nerr)
    err = sys_errlist[errcode];
  else
    err = "unknown error";

  combined = (char *) alloca (strlen (err) + strlen (string) + 3);
  strcpy (combined, string);
  strcat (combined, ": ");
  strcat (combined, err);

  printf ("%s.\n", combined);
}

/* Control C eventually causes this to be called, at a convenient time.  */

void
quit ()
{
  target_terminal_ours ();
#ifdef HAVE_TERMIO
  ioctl (fileno (stdout), TCFLSH, 1);
#else /* not HAVE_TERMIO */
  ioctl (fileno (stdout), TIOCFLUSH, 0);
#endif /* not HAVE_TERMIO */
#ifdef TIOCGPGRP
  error ("Quit");
#else
  error ("Quit (expect signal %d when inferior is resumed)", SIGINT);
#endif /* TIOCGPGRP */
}

/* Control C comes here */

void
request_quit ()
{
  quit_flag = 1;

#ifdef USG
  /* Restore the signal handler.  */
  signal (SIGINT, request_quit);
#endif

  if (immediate_quit)
    quit ();
}

/* My replacement for the read system call.
   Used like `read' but keeps going if `read' returns too soon.  */

int
myread (desc, addr, len)
     int desc;
     char *addr;
     int len;
{
  register int val;
  int orglen = len;

  while (len > 0)
    {
      val = read (desc, addr, len);
      if (val < 0)
	return val;
      if (val == 0)
	return orglen - len;
      len -= val;
      addr += val;
    }
  return orglen;
}

/* Make a copy of the string at PTR with SIZE characters
   (and add a null character at the end in the copy).
   Uses malloc to get the space.  Returns the address of the copy.  */

char *
savestring (ptr, size)
     char *ptr;
     int size;
{
  register char *p = (char *) xmalloc (size + 1);
  bcopy (ptr, p, size);
  p[size] = 0;
  return p;
}

char *
strsave (ptr)
     char *ptr;
{
  return savestring (ptr, strlen (ptr));
}

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  register int len = strlen (s1) + strlen (s2) + strlen (s3) + 1;
  register char *val = (char *) xmalloc (len);
  strcpy (val, s1);
  strcat (val, s2);
  strcat (val, s3);
  return val;
}

void
print_spaces (n, file)
     register int n;
     register FILE *file;
{
  while (n-- > 0)
    fputc (' ', file);
}

/* Ask user a y-or-n question and return 1 iff answer is yes.
   Takes three args which are given to printf to print the question.
   The first, a control string, should end in "? ".
   It should not say how to answer, because we do that.  */

/* VARARGS */
int
query (va_alist)
     va_dcl
{
  va_list args;
  char *ctlstr;
  register int answer;
  register int ans2;

  /* Automatically answer "yes" if input is not from a terminal.  */
  if (!input_from_terminal_p ())
    return 1;

  while (1)
    {
      va_start (args);
      ctlstr = va_arg (args, char *);
      vfprintf (stdout, ctlstr, args);
      va_end (args);
      printf ("(y or n) ");
      fflush (stdout);
      answer = fgetc (stdin);
      clearerr (stdin);		/* in case of C-d */
      if (answer == EOF)	/* C-d */
        return 1;
      if (answer != '\n')	/* Eat rest of input line, to EOF or newline */
	do 
	  {
	    ans2 = fgetc (stdin);
	    clearerr (stdin);
	  }
        while (ans2 != EOF && ans2 != '\n');
      if (answer >= 'a')
	answer -= 040;
      if (answer == 'Y')
	return 1;
      if (answer == 'N')
	return 0;
      printf ("Please answer y or n.\n");
    }
}

/* Parse a C escape sequence.  STRING_PTR points to a variable
   containing a pointer to the string to parse.  That pointer
   should point to the character after the \.  That pointer
   is updated past the characters we use.  The value of the
   escape sequence is returned.

   A negative value means the sequence \ newline was seen,
   which is supposed to be equivalent to nothing at all.

   If \ is followed by a null character, we return a negative
   value and leave the string pointer pointing at the null character.

   If \ is followed by 000, we return 0 and leave the string pointer
   after the zeros.  A value of 0 does not mean end of string.  */

int
parse_escape (string_ptr)
     char **string_ptr;
{
  register int c = *(*string_ptr)++;
  switch (c)
    {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'e':
      return 033;
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case '\n':
      return -2;
    case 0:
      (*string_ptr)--;
      return 0;
    case '^':
      c = *(*string_ptr)++;
      if (c == '\\')
	c = parse_escape (string_ptr);
      if (c == '?')
	return 0177;
      return (c & 0200) | (c & 037);
      
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      {
	register int i = c - '0';
	register int count = 0;
	while (++count < 3)
	  {
	    if ((c = *(*string_ptr)++) >= '0' && c <= '7')
	      {
		i *= 8;
		i += c - '0';
	      }
	    else
	      {
		(*string_ptr)--;
		break;
	      }
	  }
	return i;
      }
    default:
      return c;
    }
}

/* Print the character CH on STREAM as part of the contents
   of a literal string whose delimiter is QUOTER.  */

void
printchar (ch, stream, quoter)
     unsigned char ch;
     FILE *stream;
     int quoter;
{
  register int c = ch;

  if (c < 040 || (sevenbit_strings && c >= 0177))
    switch (c)
      {
      case '\n':
	fputs_filtered ("\\n", stream);
	break;
      case '\b':
	fputs_filtered ("\\b", stream);
	break;
      case '\t':
	fputs_filtered ("\\t", stream);
	break;
      case '\f':
	fputs_filtered ("\\f", stream);
	break;
      case '\r':
	fputs_filtered ("\\r", stream);
	break;
      case '\033':
	fputs_filtered ("\\e", stream);
	break;
      case '\007':
	fputs_filtered ("\\a", stream);
	break;
      default:
	fprintf_filtered (stream, "\\%.3o", (unsigned int) c);
	break;
      }
  else
    {
      if (c == '\\' || c == quoter)
	fputs_filtered ("\\", stream);
      fprintf_filtered (stream, "%c", c);
    }
}

/* Number of lines per page or UINT_MAX if paging is disabled.  */
static unsigned int lines_per_page;
/* Number of chars per line or UNIT_MAX is line folding is disabled.  */
static unsigned int chars_per_line;
/* Current count of lines printed on this page, chars on this line.  */
static unsigned int lines_printed, chars_printed;

/* Buffer and start column of buffered text, for doing smarter word-
   wrapping.  When someone calls wrap_here(), we start buffering output
   that comes through fputs_filtered().  If we see a newline, we just
   spit it out and forget about the wrap_here().  If we see another
   wrap_here(), we spit it out and remember the newer one.  If we see
   the end of the line, we spit out a newline, the indent, and then
   the buffered output.

   wrap_column is the column number on the screen where wrap_buffer begins.
     When wrap_column is zero, wrapping is not in effect.
   wrap_buffer is malloc'd with chars_per_line+2 bytes. 
     When wrap_buffer[0] is null, the buffer is empty.
   wrap_pointer points into it at the next character to fill.
   wrap_indent is the string that should be used as indentation if the
     wrap occurs.  */

static char *wrap_buffer, *wrap_pointer, *wrap_indent;
static int wrap_column;

/* Get the number of lines to print with commands like "list".
   This is based on guessing how many long (i.e. more than chars_per_line
   characters) lines there will be.  To be completely correct, "list"
   and friends should be rewritten to count characters and see where
   things are wrapping, but that would be a fair amount of work.  */
int
lines_to_list ()
{
  /* RMS didn't like the following algorithm.  Let's set it back to
     10 and see if anyone else complains.  */
  /* return lines_per_page == UINT_MAX ? 10 : lines_per_page / 2; */
  return 10;
}

static void 
set_width_command (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  if (!wrap_buffer)
    {
      wrap_buffer = (char *) xmalloc (chars_per_line + 2);
      wrap_buffer[0] = '\0';
    }
  else
    wrap_buffer = (char *) xrealloc (wrap_buffer, chars_per_line + 2);
  wrap_pointer = wrap_buffer;	/* Start it at the beginning */
}

static void
prompt_for_continue ()
{
  immediate_quit++;
  gdb_readline ("---Type <return> to continue---", 0);
  chars_printed = lines_printed = 0;
  immediate_quit--;
}

/* Reinitialize filter; ie. tell it to reset to original values.  */

void
reinitialize_more_filter ()
{
  lines_printed = 0;
  chars_printed = 0;
}

/* Indicate that if the next sequence of characters overflows the line,
   a newline should be inserted here rather than when it hits the end. 
   If INDENT is nonzero, it is a string to be printed to indent the
   wrapped part on the next line.  INDENT must remain accessible until
   the next call to wrap_here() or until a newline is printed through
   fputs_filtered().

   If the line is already overfull, we immediately print a newline and
   the indentation, and disable further wrapping.

   INDENT should not contain tabs, as that
   will mess up the char count on the next line.  FIXME.  */

void
wrap_here(indent)
  char *indent;
{
  if (wrap_buffer[0])
    {
      *wrap_pointer = '\0';
      fputs (wrap_buffer, stdout);
    }
  wrap_pointer = wrap_buffer;
  wrap_buffer[0] = '\0';
  if (chars_printed >= chars_per_line)
    {
      puts_filtered ("\n");
      puts_filtered (indent);
      wrap_column = 0;
    }
  else
    {
      wrap_column = chars_printed;
      wrap_indent = indent;
    }
}

/* Like fputs but pause after every screenful, and can wrap at points
   other than the final character of a line.
   Unlike fputs, fputs_filtered does not return a value.
   It is OK for LINEBUFFER to be NULL, in which case just don't print
   anything.

   Note that a longjmp to top level may occur in this routine
   (since prompt_for_continue may do so) so this routine should not be
   called when cleanups are not in place.  */

void
fputs_filtered (linebuffer, stream)
     char *linebuffer;
     FILE *stream;
{
  char *lineptr;

  if (linebuffer == 0)
    return;
  
  /* Don't do any filtering if it is disabled.  */
  if (stream != stdout
   || (lines_per_page == UINT_MAX && chars_per_line == UINT_MAX))
    {
      fputs (linebuffer, stream);
      return;
    }

  /* Go through and output each character.  Show line extension
     when this is necessary; prompt user for new page when this is
     necessary.  */
  
  lineptr = linebuffer;
  while (*lineptr)
    {
      /* Possible new page.  */
      if (lines_printed >= lines_per_page - 1)
	prompt_for_continue ();

      while (*lineptr && *lineptr != '\n')
	{
	  /* Print a single line.  */
	  if (*lineptr == '\t')
	    {
	      if (wrap_column)
		*wrap_pointer++ = '\t';
	      else
		putc ('\t', stream);
	      /* Shifting right by 3 produces the number of tab stops
	         we have already passed, and then adding one and
		 shifting left 3 advances to the next tab stop.  */
	      chars_printed = ((chars_printed >> 3) + 1) << 3;
	      lineptr++;
	    }
	  else
	    {
	      if (wrap_column)
		*wrap_pointer++ = *lineptr;
	      else
	        putc (*lineptr, stream);
	      chars_printed++;
	      lineptr++;
	    }
      
	  if (chars_printed >= chars_per_line)
	    {
	      unsigned int save_chars = chars_printed;

	      chars_printed = 0;
	      lines_printed++;
	      /* If we aren't actually wrapping, don't output newline --
		 if chars_per_line is right, we probably just overflowed
		 anyway; if it's wrong, let us keep going.  */
	      if (wrap_column)
		putc ('\n', stream);

	      /* Possible new page.  */
	      if (lines_printed >= lines_per_page - 1)
		prompt_for_continue ();

	      /* Now output indentation and wrapped string */
	      if (wrap_column)
		{
		  if (wrap_indent)
		    fputs (wrap_indent, stream);
		  *wrap_pointer = '\0';		/* Null-terminate saved stuff */
		  fputs (wrap_buffer, stream);	/* and eject it */
		  /* FIXME, this strlen is what prevents wrap_indent from
		     containing tabs.  However, if we recurse to print it
		     and count its chars, we risk trouble if wrap_indent is
		     longer than (the user settable) chars_per_line. 
		     Note also that this can set chars_printed > chars_per_line
		     if we are printing a long string.  */
		  chars_printed = strlen (wrap_indent)
				+ (save_chars - wrap_column);
		  wrap_pointer = wrap_buffer;	/* Reset buffer */
		  wrap_buffer[0] = '\0';
		  wrap_column = 0;		/* And disable fancy wrap */
 		}
	    }
	}

      if (*lineptr == '\n')
	{
	  chars_printed = 0;
	  wrap_here ("");	/* Spit out chars, cancel further wraps */
	  lines_printed++;
	  putc ('\n', stream);
	  lineptr++;
	}
    }
}


/* fputs_demangled is a variant of fputs_filtered that
   demangles g++ names.*/

void
fputs_demangled (linebuffer, stream, arg_mode)
     char *linebuffer;
     FILE *stream;
     int arg_mode;
{
#ifdef __STDC__
  extern char *cplus_demangle (const char *, int);
#else
  extern char *cplus_demangle ();
#endif
#define SYMBOL_MAX 1024

#define SYMBOL_CHAR(c) (isascii(c) && (isalnum(c) || (c) == '_' || (c) == '$'))

  char buf[SYMBOL_MAX+1];
# define SLOP 5		/* How much room to leave in buf */
  char *p;

  if (linebuffer == NULL)
    return;

  /* If user wants to see raw output, no problem.  */
  if (!demangle) {
    fputs_filtered (linebuffer, stream);
  }

  p = linebuffer;

  while ( *p != (char) 0 ) {
    int i = 0;

    /* collect non-interesting characters into buf */
    while ( *p != (char) 0 && !SYMBOL_CHAR(*p) && i < (int)sizeof(buf)-SLOP ) {
      buf[i++] = *p;
      p++;
    }
    if (i > 0) {
      /* output the non-interesting characters without demangling */
      buf[i] = (char) 0;
      fputs_filtered(buf, stream);
      i = 0;  /* reset buf */
    }

    /* and now the interesting characters */
    while (i < SYMBOL_MAX
     && *p != (char) 0
     && SYMBOL_CHAR(*p)
     && i < (int)sizeof(buf) - SLOP) {
      buf[i++] = *p;
      p++;
    }
    buf[i] = (char) 0;
    if (i > 0) {
      char * result;
      
      if ( (result = cplus_demangle(buf, arg_mode)) != NULL ) {
	fputs_filtered(result, stream);
	free(result);
      }
      else {
	fputs_filtered(buf, stream);
      }
    }
  }
}

/* Print a variable number of ARGS using format FORMAT.  If this
   information is going to put the amount written (since the last call
   to INITIALIZE_MORE_FILTER or the last page break) over the page size,
   print out a pause message and do a gdb_readline to get the users
   permision to continue.

   Unlike fprintf, this function does not return a value.

   We implement three variants, vfprintf (takes a vararg list and stream),
   fprintf (takes a stream to write on), and printf (the usual).

   Note that this routine has a restriction that the length of the
   final output line must be less than 255 characters *or* it must be
   less than twice the size of the format string.  This is a very
   arbitrary restriction, but it is an internal restriction, so I'll
   put it in.  This means that the %s format specifier is almost
   useless; unless the caller can GUARANTEE that the string is short
   enough, fputs_filtered should be used instead.

   Note also that a longjmp to top level may occur in this routine
   (since prompt_for_continue may do so) so this routine should not be
   called when cleanups are not in place.  */

#if !defined(MISSING_VPRINTF) || defined (vsprintf)
/* VARARGS */
void
vfprintf_filtered (stream, format, args)
     va_list args;
#else
void fprintf_filtered (stream, format, arg1, arg2, arg3, arg4, arg5, arg6)
#endif
     FILE *stream;
     char *format;
{
  static char *linebuffer = (char *) 0;
  static int line_size;
  int format_length;

  format_length = strlen (format);

  /* Allocated linebuffer for the first time.  */
  if (!linebuffer)
    {
      linebuffer = (char *) xmalloc (255);
      line_size = 255;
    }

  /* Reallocate buffer to a larger size if this is necessary.  */
  if (format_length * 2 > line_size)
    {
      line_size = format_length * 2;

      /* You don't have to copy.  */
      free (linebuffer);
      linebuffer = (char *) xmalloc (line_size);
    }


  /* This won't blow up if the restrictions described above are
     followed.   */
#if !defined(MISSING_VPRINTF) || defined (vsprintf)
  (void) vsprintf (linebuffer, format, args);
#else
  (void) sprintf (linebuffer, format, arg1, arg2, arg3, arg4, arg5, arg6);
#endif

  fputs_filtered (linebuffer, stream);
}

#if !defined(MISSING_VPRINTF) || defined (vsprintf)
/* VARARGS */
void
fprintf_filtered (va_alist)
     va_dcl
{
  va_list args;
  FILE *stream;
  char *format;

  va_start (args);
  stream = va_arg (args, FILE *);
  format = va_arg (args, char *);

  /* This won't blow up if the restrictions described above are
     followed.   */
  (void) vfprintf_filtered (stream, format, args);
  va_end (args);
}

/* VARARGS */
void
printf_filtered (va_alist)
     va_dcl
{
  va_list args;
  char *format;

  va_start (args);
  format = va_arg (args, char *);

  (void) vfprintf_filtered (stdout, format, args);
  va_end (args);
}
#else
void
printf_filtered (format, arg1, arg2, arg3, arg4, arg5, arg6)
     char *format;
     int arg1, arg2, arg3, arg4, arg5, arg6;
{
  fprintf_filtered (stdout, format, arg1, arg2, arg3, arg4, arg5, arg6);
}
#endif

/* Easy */

void
puts_filtered (string)
     char *string;
{
  fputs_filtered (string, stdout);
}

/* Return a pointer to N spaces and a null.  The pointer is good
   until the next call to here.  */
char *
n_spaces (n)
     int n;
{
  register char *t;
  static char *spaces;
  static int max_spaces;

  if (n > max_spaces)
    {
      if (spaces)
	free (spaces);
      spaces = malloc (n+1);
      for (t = spaces+n; t != spaces;)
	*--t = ' ';
      spaces[n] = '\0';
      max_spaces = n;
    }

  return spaces + max_spaces - n;
}

/* Print N spaces.  */
void
print_spaces_filtered (n, stream)
     int n;
     FILE *stream;
{
  fputs_filtered (n_spaces (n), stream);
}

/* C++ demangler stuff.  */
char *cplus_demangle ();

/* Print NAME on STREAM, demangling if necessary.  */
void
fprint_symbol (stream, name)
     FILE *stream;
     char *name;
{
  char *demangled;
  if ((!demangle) || NULL == (demangled = cplus_demangle (name, 1)))
    fputs_filtered (name, stream);
  else
    {
      fputs_filtered (demangled, stream);
      free (demangled);
    }
}

#if !defined (USG_UTILS)
#define USG_UTILS defined (USG)
#endif

#if USG_UTILS
bcopy (from, to, count)
char *from, *to;
{
	memcpy (to, from, count);
}

bcmp (from, to, count)
{
	return (memcmp (to, from, count));
}

bzero (to, count)
char *to;
{
	while (count--)
		*to++ = 0;
}

getwd (buf)
char *buf;
{
  getcwd (buf, MAXPATHLEN);
}

char *
index (s, c)
     char *s;
{
  char *strchr ();
  return strchr (s, c);
}

char *
rindex (s, c)
     char *s;
{
  char *strrchr ();
  return strrchr (s, c);
}
#endif /* USG_UTILS.  */

#if !defined (QUEUE_MISSING)
#define QUEUE_MISSING defined (USG)
#endif

#if QUEUE_MISSING
/* Queue routines */

struct queue {
	struct queue *forw;
	struct queue *back;
};

insque (item, after)
struct queue *item;
struct queue *after;
{
	item->forw = after->forw;
	after->forw->back = item;

	item->back = after;
	after->forw = item;
}

remque (item)
struct queue *item;
{
	item->forw->back = item->back;
	item->back->forw = item->forw;
}
#endif /* QUEUE_MISSING */

/* Simple implementation of strstr, since some implementations lack it. */
char *
strstr (in, find)
     const char *in, *find;
{
  register char *p = in - 1;

  while (0 != (p = strchr (p+1, *find))) {
    if (strcmp (p, find))
      return p;
  }
  return 0;
}

void
_initialize_utils ()
{
  struct cmd_list_element *c;

  c = add_set_cmd ("width", class_support, var_uinteger, 
		  (char *)&chars_per_line,
		  "Set number of characters gdb thinks are in a line.",
		  &setlist);
  add_show_from_set (c, &showlist);
  c->function = set_width_command;

  add_show_from_set
    (add_set_cmd ("height", class_support,
		  var_uinteger, (char *)&lines_per_page,
		  "Set number of lines gdb thinks are in a page.", &setlist),
     &showlist);
  
  /* These defaults will be used if we are unable to get the correct
     values from termcap.  */
  lines_per_page = 24;
  chars_per_line = 80;
  /* Initialize the screen height and width from termcap.  */
  {
    char *termtype = getenv ("TERM");

    /* Positive means success, nonpositive means failure.  */
    int status;

    /* 2048 is large enough for all known terminals, according to the
       GNU termcap manual.  */
    char term_buffer[2048];

    if (termtype)
      {
	status = tgetent (term_buffer, termtype);
	if (status > 0)
	  {
	    int val;
	    
	    val = tgetnum ("li");
	    if (val >= 0)
	      lines_per_page = val;
	    else
	      /* The number of lines per page is not mentioned
		 in the terminal description.  This probably means
		 that paging is not useful (e.g. emacs shell window),
		 so disable paging.  */
	      lines_per_page = UINT_MAX;
	    
	    val = tgetnum ("co");
	    if (val >= 0)
	      chars_per_line = val;
	  }
      }
  }

  set_width_command ((char *)NULL, 0, c);

  add_show_from_set
    (add_set_cmd ("demangle", class_support, var_boolean, 
		  (char *)&demangle,
		"Set demangling of encoded C++ names when displaying symbols.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("sevenbit-strings", class_support, var_boolean, 
		  (char *)&sevenbit_strings,
   "Set printing of 8-bit characters in strings as \\nnn.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("asm-demangle", class_support, var_boolean, 
		  (char *)&asm_demangle,
	"Set demangling of C++ names in disassembly listings.",
		  &setlist),
     &showlist);
}
