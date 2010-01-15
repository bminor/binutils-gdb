/* This program does two things; it generates valid trace files, and
   it can also be traced so as to test trace file creation from
   GDB.  */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

char spbuf[200];

char trbuf[1000];
char *trptr;
char *tfsizeptr;

int testglob = 31415;

int
start_trace_file (char *filename)
{
  int fd;

  fd = open (filename, O_WRONLY|O_CREAT|O_APPEND,
	     S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

  if (fd < 0)
    return fd;

  /* Write a file header, with a high-bit-set char to indicate a
     binary file, plus a hint as what this file is, and a version
     number in case of future needs.  */
  write (fd, "\x7fTRACE0\n", 8);

  return fd;
}

void
finish_trace_file (int fd)
{
  close (fd);
}

void
write_basic_trace_file ()
{
  int fd;

  fd = start_trace_file ("basic.tf");

  /* The next part of the file consists of newline-separated lines
     defining status, tracepoints, etc.  The section is terminated by
     an empty line.  */

  /* Dump the size of the R (register) blocks in traceframes.  */
  snprintf (spbuf, sizeof spbuf, "R %x\n", 500 /* FIXME get from arch */);
  write (fd, spbuf, strlen (spbuf));

  /* Dump trace status, in the general form of the qTstatus reply.  */
  snprintf (spbuf, sizeof spbuf, "status 0;tstop:0;tframes:1;tcreated:1;tfree:100;tsize:1000\n");
  write (fd, spbuf, strlen (spbuf));

  /* Dump tracepoint definitions, in syntax similar to that used
     for reconnection uploads.  */
  snprintf (spbuf, sizeof spbuf, "tp T1:%lx:E:0:0\n",
	    (long) &write_basic_trace_file);
  write (fd, spbuf, strlen (spbuf));
  /* (Note that we would only need actions defined if we wanted to
     test tdump.) */

  /* Empty line marks the end of the definition section.  */
  write (fd, "\n", 1);

  /* Make up a simulated trace buffer.  */
  /* (Encapsulate better if we're going to do lots of this.) */
  trptr = trbuf;
  *((short *) trptr) = 1;
  trptr += sizeof (short);
  tfsizeptr = trptr;
  trptr += sizeof (int);
  *((char *) trptr) = 'M';
  trptr += 1;
  *((long long *) trptr) = (long) &testglob;
  trptr += sizeof (long long);
  *((short *) trptr) = sizeof (testglob);
  trptr += sizeof (short);
  *((int *) trptr) = testglob;
  trptr += sizeof (testglob);
  /* Go back and patch in the frame size.  */
  *((int *) tfsizeptr) = trptr - tfsizeptr - sizeof (int);

  /* Write end of tracebuffer marker.  */
  *((short *) trptr) = 0;
  trptr += sizeof (short);
  *((int *) trptr) = 0;
  trptr += sizeof (int);

  write (fd, trbuf, trptr - trbuf);

  finish_trace_file (fd);
}

void
done_making_trace_files (void)
{
}

int
main (int argc, char **argv, char **envp)
{
  write_basic_trace_file ();

  done_making_trace_files ();

  return 0;
}

