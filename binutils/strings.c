/* strings -- print the strings of printable characters in files
   Copyright (C) 1993 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Usage: strings [options] file...

   Options:
   --all
   -a
   -		Do not scan only the initialized data section of object files.

   --print-file-name
   -f		Print the name of the file before each string.

   --bytes=min-len
   -n min-len
   -min-len	Print graphic char sequences, MIN-LEN or more bytes long,
		that are followed by a NUL or a newline.  Default is 4.

   --radix={o,x,d}
   -t {o,x,d}	Print the offset within the file before each string,
		in octal/hex/decimal.

   -o		Like -to.  (Some other implementations have -o like -to,
		others like -td.  We chose one arbitrarily.)

   --help
   -h		Print the usage message on the standard output.

   --version
   -v		Print the program version number.

   Written by Richard Stallman <rms@gnu.ai.mit.edu>
   and David MacKenzie <djm@gnu.ai.mit.edu>.  */

#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <bfd.h>

#ifdef isascii
#define isgraphic(c) (isascii (c) && isprint (c))
#else
#define isgraphic(c) (isprint (c))
#endif

#ifndef errno
extern int errno;
#endif

/* The BFD section flags that identify an initialized data section.  */
#define DATA_FLAGS (SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS)

/* Radix for printing addresses (must be 8, 10 or 16).  */
static int address_radix;

/* Minimum length of sequence of graphic chars to trigger output.  */
static int string_min;

/* true means print address within file for each string.  */
static boolean print_addresses;

/* true means print filename for each string.  */
static boolean print_filenames;

/* true means for object files scan only the data section.  */
static boolean datasection_only;

/* true if we found an initialized data section in the current file.  */
static boolean got_a_section;

/* Opened to /dev/null for reading from a BFD.  */
static FILE *devnull;

extern char *program_name;
extern char *program_version;

static struct option long_options[] =
{
  {"all", no_argument, NULL, 'a'},
  {"print-file-name", no_argument, NULL, 'f'},
  {"bytes", required_argument, NULL, 'n'},
  {"radix", required_argument, NULL, 't'},
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {NULL, 0, NULL, 0}
};

char *xmalloc ();
char *xrealloc ();

static boolean strings_file ();
static int integer_arg ();
static void dump_strings ();
static void usage ();

void
main (argc, argv)
     int argc;
     char **argv;
{
  int optc;
  int exit_status = 0;
  boolean files_given = false;

  program_name = argv[0];
  string_min = -1;
  print_addresses = false;
  print_filenames = false;
  datasection_only = true;

  while ((optc = getopt_long (argc, argv, "afhn:ot:v0123456789",
			      long_options, (int *) 0)) != EOF)
    {
      switch (optc)
	{
	case 'a':
	  datasection_only = false;
	  break;

	case 'f':
	  print_filenames = true;
	  break;

	case 'h':
	  usage (stdout);
	  exit (0);

	case 'n':
	  string_min = integer_arg (optarg);
	  if (string_min < 1)
	    {
	      fprintf (stderr, "%s: invalid number %s\n",
		       program_name, optarg);
	      exit (1);
	    }
	  break;

	case 'o':
	  print_addresses = true;
	  address_radix = 8;
	  break;

	case 't':
	  print_addresses = true;
	  if (optarg[1] != '\0')
	    usage ();
	  switch (optarg[0])
	    {
	    case 'o':
	      address_radix = 8;
	      break;

	    case 'd':
	      address_radix = 10;
	      break;

	    case 'x':
	      address_radix = 16;
	      break;

	    default:
	      usage ();
	    }
	  break;

	case 'v':
	  printf ("%s version %s\n", program_name, program_version);
	  exit (0);

	case '?':
	  usage (stderr);

	default:
	  if (string_min < 0)
	    string_min = optc;
	  else
	    string_min = string_min * 10 + optc - '0';
	  break;
	}
    }

  if (string_min < 0)
    string_min = 4;

  bfd_init ();
  devnull = fopen ("/dev/null", "r");
  if (devnull == NULL)
    {
      fprintf (stderr, "%s: ", program_name);
      perror ("/dev/null");
      exit (1);
    }

  for (; optind < argc; ++optind)
    {
      if (!strcmp (argv[optind], "-"))
	datasection_only = false;
      else
	{
	  files_given = true;
	  exit_status |= (strings_file (argv[optind]) == false);
	}
    }

  if (files_given == false)
    usage (stderr);

  exit (exit_status);
}

/* Scan the sections of the file ABFD, whose printable name is FILE.
   If any of them contain initialized data,
   set `got_a_section' and print the strings in them.  */

static void
strings_a_section (abfd, sect, file)
     bfd *abfd;
     asection *sect;
     PTR file;
{
  if ((sect->flags & DATA_FLAGS) == DATA_FLAGS)
    {
      bfd_size_type sz = bfd_get_section_size_before_reloc (sect);
      PTR mem = xmalloc (sz);
      if (bfd_get_section_contents (abfd, sect, mem, (file_ptr) 0, sz))
	{
	  got_a_section = true;
	  dump_strings (file, devnull, sect->filepos, 0, sz, mem);
	}
      free (mem);
    }
}

/* Print the strings in the initialized data section of FILE.
   Return true if successful,
   false if not (such as if FILE is not an object file).  */

static boolean
strings_object_file (file)
     char *file;
{
  bfd *abfd = bfd_openr (file, NULL);

  if (abfd == NULL)
    {
      if (bfd_error != system_call_error)
	{
	  /* Out of memory, or an invalid target is specified by the
	     GNUTARGET environment variable.  */
	  fprintf (stderr, "%s: ", program_name);
	  bfd_perror (file);
	}
      return false;
    }

  /* For some reason, without this call, the BFD has no sections.
     This call is only for the side effect of reading in the sections.  */
  bfd_check_format (abfd, bfd_object);

  got_a_section = false;
  bfd_map_over_sections (abfd, strings_a_section, file);

  if (!bfd_close (abfd))
    {
      fprintf (stderr, "%s: ", program_name);
      bfd_perror (file);
      return false;
    }

  return got_a_section;
}

/* Print the strings in FILE.  Return true if ok, false if an error occurs.  */

static boolean
strings_file (file)
     char *file;
{
  /* If we weren't told to scan the whole file,
     try to open it as an object file and only look at
     initialized data sections.  If that fails, fall back to the
     whole file.  */
  if (!datasection_only || !strings_object_file (file))
    {
      FILE *stream;

      stream = fopen (file, "r");
      if (stream == NULL)
	{
	  fprintf (stderr, "%s: ", program_name);
	  perror (file);
	  return false;
	}

      dump_strings (file, stream, (file_ptr) 0, 0, 0, (char *) 0);

      if (fclose (stream) == EOF)
	{
	  fprintf (stderr, "%s: ", program_name);
	  perror (file);
	  return false;
	}
    }

  return true;
}

/* Find the strings in file FILENAME, read from STREAM.
   Assume that STREAM is positioned so that the next byte read
   is at address ADDRESS in the file.
   Stop reading at address STOP_POINT in the file, if nonzero.

   Optionally the caller can supply a buffer of characters
   to be processed before the data in STREAM.
   MAGIC is the address of the buffer and
   MAGICCOUNT is how many characters are in it.
   Those characters come at address ADDRESS and the data in STREAM follow.  */

static void
dump_strings (filename, stream, address, stop_point, magiccount, magic)
     char *filename;
     FILE *stream;
     file_ptr address;
     int stop_point;
     int magiccount;
     char *magic;
{
  int bufsize = 100;
  char *buf = (char *) xmalloc (bufsize);

  while (1)
    {
      int i;
      int c;

      /* See if the next `string_min' chars are all graphic chars.  */
    tryline:
      if (stop_point && address >= stop_point)
	break;
      for (i = 0; i < string_min; i++)
	{
	  if (magiccount)
	    {
	      magiccount--;
	      c = *magic++;
	    }
	  else
	    {
	      c = getc (stream);
	      if (c < 0)
		return;
	    }
	  address++;
	  if (!isgraphic (c))
	    /* Found a non-graphic.  Try again starting with next char.  */
	    goto tryline;
	  buf[i] = c;
	}

      /* We found a run of `string_min' graphic characters.
	 Now see if it is terminated with a NUL byte or a newline.   */
      while (1)
	{
	  if (i == bufsize)
	    {
	      bufsize *= 2;
	      buf = (char *) xrealloc (buf, bufsize);
	    }
	  if (magiccount)
	    {
	      magiccount--;
	      c = *magic++;
	    }
	  else
	    {
	      c = getc (stream);
	      if (c < 0)
		return;
	    }
	  address++;
	  if (c == '\0' || c == '\n')
	    break;		/* It is; print this string.  */
	  if (!isgraphic (c))
	    goto tryline;	/* It isn't; give up on this string.  */
	  buf[i++] = c;		/* The string continues; store it all.  */
	}

      /* If we get here, the string is all graphics and properly terminated,
	 so print it.  It is all in `buf' and `i' is its length.  */
      buf[i] = '\0';
      if (print_filenames)
	printf ("%s: ", filename);
      if (print_addresses)
	switch (address_radix)
	  {
	  case 8:
	    printf ("%7lo ", address - i - 1);
	    break;

	  case 10:
	    printf ("%7ld ", address - i - 1);
	    break;

	  case 16:
	    printf ("%7lx ", address - i - 1);
	    break;
	  }

      for (i = 0; (c = buf[i]) != '\0'; i++)
	switch (c)
	  {
	  case '\n':
	    printf ("\\n");
	    break;
	  case '\t':
	    printf ("\\t");
	    break;
	  case '\f':
	    printf ("\\f");
	    break;
	  case '\b':
	    printf ("\\b");
	    break;
	  case '\r':
	    printf ("\\r");
	    break;
	  default:
	    putchar (c);
	  }
      putchar ('\n');
    }
}

/* Parse string S as an integer, using decimal radix by default,
   but allowing octal and hex numbers as in C.  */

static int
integer_arg (s)
     char *s;
{
  int value;
  int radix = 10;
  char *p = s;
  int c;

  if (*p != '0')
    radix = 10;
  else if (*++p == 'x')
    {
      radix = 16;
      p++;
    }
  else
    radix = 8;

  value = 0;
  while (((c = *p++) >= '0' && c <= '9')
	 || (radix == 16 && (c & ~40) >= 'A' && (c & ~40) <= 'Z'))
    {
      value *= radix;
      if (c >= '0' && c <= '9')
	value += c - '0';
      else
	value += (c & ~40) - 'A';
    }

  if (c == 'b')
    value *= 512;
  else if (c == 'B')
    value *= 1024;
  else
    p--;

  if (*p)
    {
      fprintf (stderr, "%s: invalid integer argument %s\n", program_name, s);
      exit (1);
    }
  return value;
}

static void
usage (stream)
     FILE *stream;
{
  fprintf (stream, "\
Usage: %s [-afhov] [-n min-len] [-min-len] [-t {o,x,d}] [-]\n\
       [--all] [--print-file-name] [--bytes=min-len] [--radix={o,x,d}]\n\
       [--help] [--version] file...\n",
	   program_name);
  exit (1);
}
