/* General utility routines for GDB, the GNU debugger.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "defs.h"
#include "param.h"
#ifdef HAVE_TERMIO
#include <termio.h>
#endif

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

/* Generally useful subroutines used throughout the program.  */

/* Like malloc but get error if no storage available.  */

char *
xmalloc (size)
     long size;
{
  register char *val = (char *) malloc (size);
  if (!val)
    fatal ("virtual memory exhausted.", 0);
  return val;
}

/* Like realloc but get error if no storage available.  */

char *
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
  extern int errno;
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

void
quit ()
{
  fflush (stdout);
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
  if (immediate_quit)
    quit ();
}

/* Print an error message and return to command level.
   STRING is the error message, used as a fprintf string,
   and ARG is passed as an argument to it.  */

void
error (string, arg1, arg2, arg3)
     char *string;
     int arg1, arg2, arg3;
{
  fflush (stdout);
  fprintf (stderr, string, arg1, arg2, arg3);
  fprintf (stderr, "\n");
  return_to_top_level ();
}

/* Print an error message and exit reporting failure.
   This is for a error that we cannot continue from.
   STRING and ARG are passed to fprintf.  */

void
fatal (string, arg)
     char *string;
     int arg;
{
  fprintf (stderr, "gdb: ");
  fprintf (stderr, string, arg);
  fprintf (stderr, "\n");
  exit (1);
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

int
query (ctlstr, arg1, arg2)
     char *ctlstr;
{
  register int answer;

  /* Automatically answer "yes" if input is not from a terminal.  */
  if (!input_from_terminal_p ())
    return 1;

  while (1)
    {
      printf (ctlstr, arg1, arg2);
      printf ("(y or n) ");
      fflush (stdout);
      answer = fgetc (stdin);
      clearerr (stdin);		/* in case of C-d */
      if (answer != '\n')
	while (fgetc (stdin) != '\n') clearerr (stdin);
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
  if (c < 040 || c >= 0177)
    {
      if (c == '\n')
	fprintf (stream, "\\n");
      else if (c == '\b')
	fprintf (stream, "\\b");
      else if (c == '\t')
	fprintf (stream, "\\t");
      else if (c == '\f')
	fprintf (stream, "\\f");
      else if (c == '\r')
	fprintf (stream, "\\r");
      else if (c == 033)
	fprintf (stream, "\\e");
      else if (c == '\a')
	fprintf (stream, "\\a");
      else
	fprintf (stream, "\\%03o", c);
    }
  else
    {
      if (c == '\\' || c == quoter)
	fputc ('\\', stream);
      fputc (c, stream);
    }
}
