/*** bucomm.c -- Bin Utils COMmon code.

     We might put this in a library someday so it could be dynamically
     loaded, but for now it's not necessary */

#include "sysdep.h"
#include "bfd.h"
#include <varargs.h>

char *target = NULL;		/* default as late as possible */

/* Yes, this is what atexit is for, but that isn't guaranteed yet.
   And yes, I know this isn't as good, but it does what is needed just fine */
void (*exit_handler) ();

/** Memory hackery */

PROTO (char *, malloc, (unsigned size));
PROTO (char *, realloc, (char *ptr, unsigned size));


/* Error reporting */

char *program_name;

void
bfd_fatal (string)
     char *string;
{
  char *errmsg =  bfd_errmsg (bfd_error);
  
  if (string)
    fprintf (stderr, "%s: %s: %s\n", program_name, string, errmsg);
  else
    fprintf (stderr, "%s: %s\n", program_name, errmsg);

  if (NULL != exit_handler) (*exit_handler) ();
  exit (1);
}

#ifndef NO_STDARG
void
fatal (Format)
     const char *Format;
{
  va_list args;
	
  va_start (args, Format);
  vfprintf (stderr, Format, args);
  va_end (args);
  (void) putc ('\n', stderr);
  if (NULL != exit_handler) (*exit_handler) ();
  exit (1);
}
#else
#ifndef NO_VARARGS
void fatal (va_alist)
     va_dcl
{
	char *Format;
	va_list args;
	
	va_start (args);
	Format = va_arg(args, char *);
	vfprintf (stderr, Format, args);
	va_end (args);
	(void) putc ('\n', stderr);
	if (NULL != exit_handler) (*exit_handler) ();
	exit (1);
} /* fatal() */
#else
/*VARARGS1 */
fatal (Format, args)
     char *Format;
{
  as_where ();
  _doprnt (Format, &args, stderr); /* not terribly portable, but... */
  (void) putc ('\n', stderr);
  if (NULL != exit_handler) (*exit_handler) ();
  exit (1);
}
#endif /* not NO_VARARGS */
#endif /* not NO_STDARG */


/** Display the archive header for an element as if it were an ls -l listing */

/* Mode       User\tGroup\tSize\tDate               Name */

void
print_arelt_descr (abfd, verbose)
     bfd *abfd;
     boolean verbose;
{
  struct stat buf;
  char modebuf[11];
  char timebuf[40];
  long when;
  long current_time = time ((long *) 0);

  if (verbose) {

    if (bfd_stat_arch_elt (abfd, &buf) == 0) { /* if not, huh? */

      mode_string (buf.st_mode, modebuf);
      modebuf[10] = '\0';
      fputs (modebuf, stdout);

      when = buf.st_mtime;
      strcpy (timebuf, ctime (&when));

      /* This code comes from gnu ls.  */
      if ((current_time - when > 6 * 30 * 24 * 60 * 60)
	  || (current_time - when < 0)) {
	/* The file is fairly old or in the future.
	   POSIX says the cutoff is 6 months old;
	   approximate this by 6*30 days.
	   Show the year instead of the time of day.  */
	strcpy (timebuf + 11, timebuf + 19);
      }
      timebuf[16] = 0;

      printf (" %d\t%d\t%ld\t%s ", buf.st_uid, buf.st_gid, buf.st_size, timebuf);
    }
  }

  puts (abfd->filename);
}

/* Like malloc but get fatal error if memory is exhausted.  */
char *
xmalloc (size)
     unsigned size;
{
  register char *result = malloc (size);
  if (result == NULL && size != NULL) fatal ("virtual memory exhausted");

  return result;
}

/* Like realloc but get fatal error if memory is exhausted.  */
char *
xrealloc (ptr, size)
     char *ptr;
     unsigned size;
{
  register char *result = realloc (ptr, size);
  if (result == 0 && size != 0) fatal ("virtual memory exhausted");

  return result;
}
