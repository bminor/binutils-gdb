/* Support Cygwin paths under MinGW.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Written by CodeSourcery.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or modify it
under the terms of the GNU Library General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If not, write
to the Free Software Foundation, Inc., 51 Franklin Street - Fifth
Floor, Boston, MA 02110-1301, USA.  */

#include <windows.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "libiberty.h"

/* If non-zero, we have attempted to use cygpath.  CYGPATH_PEX may
   still be NULL, if cygpath is unavailable.  */
static int cygpath_initialized;

/* If non-NULL, an instance of cygpath connected via a pipe.  */
static struct pex_obj *cygpath_pex;

/* The input to cygpath.  */
static FILE *cygpath_in;

/* The output from cygpath.  */
static FILE *cygpath_out;

/* CYG_PATH is a pointer to a Cygwin path.  This function converts the
   Cygwin path to a Windows path, storing the result in
   WIN32_PATH.  Returns true if the conversion was successful; false
   otherwise.  */
static bool
cygpath (const char *cyg_path, char win32_path[MAX_PATH + 1])
{
  bool ok;

  if (!cygpath_initialized) 
    {
      const char *argv[] = { "cygpath", "-w", "-f", "-", NULL };
      const char *cygpath_path;
      int err;

      /* If we are unable to invoke cygpath, we do not want to try
	 again.  So, we set the initialized flag at this point; if
	 errors occur during the invocation, it will remain set.  */
      cygpath_initialized = 1;
      /* Check to see if the user wants cygpath support.  */
      cygpath_path = getenv ("CYGPATH");
      if (!cygpath_path)
	/* The user doesn't need to support Cygwin paths.  */
	goto error;
      /* If the environment variable is set to a non-empty string, use
	 that string as the path to cygpath.  */ 
      if (cygpath_path[0] != '\0')
	argv[0] = cygpath_path;
      /* Create the pex object.  */
      cygpath_pex = pex_init (PEX_SEARCH | PEX_USE_PIPES, 
			      "cygpath", NULL);
      if (!cygpath_pex)
	goto error;
      /* Get the FILE we will use to write to the child.  */
      cygpath_in = pex_write_input (cygpath_pex, /*binary=*/0);
      if (!cygpath_in)
	goto error;
      /* Start the child process.  */
      if (pex_run (cygpath_pex, PEX_SEARCH | PEX_USE_PIPES, 
		   argv[0], (char**) argv, 
		   NULL, NULL,
		   &err) != NULL)
	goto error;
      /* Get the FILE we will use to read from the child.  */
      cygpath_out = pex_read_output (cygpath_pex, /*binary=*/1);
      if (!cygpath_out)
	goto error;
    }
  else if (!cygpath_pex) 
    /* We previously tried to use cygpath, but something went wrong.  */
    return false;

  /* Write CYG_PATH to the child, on a line by itself.  */
  if (fprintf (cygpath_in, "%s\n", cyg_path) < 0)
    goto error;
  /* Flush the output.  (We cannot set the stream into line-buffered
     mode with setvbuf because Windows treats _IOLBF as a synonym for
     _IOFBF.)  */
  fflush (cygpath_in);
  /* Read the output.  */
  ok = true;
  while (1)
    {
      size_t pathlen;
      if (!fgets (win32_path, MAX_PATH, cygpath_out))
	goto error;
      pathlen = strlen (win32_path);
      if (pathlen == 0 && ok)
	/* This isn't a well-formed response from cygpath.  */
	goto error;
      if (win32_path[pathlen - 1] == '\n')
	{
	  win32_path[pathlen - 1] = '\0';
	  break;
	}
      /* We didn't reach the end of the line.  There's no point in
	 trying to use this output, since we know the length of
	 paths are limited to MAX_PATH characters, but we read the
	 entire line so that we are still in sync with
	 cygpath.  */
      ok = false;
    }

  return ok;

 error:

  /* Free resources.  */
  if (cygpath_out)
    {
      fclose (cygpath_out);
      cygpath_out = NULL;
    }
  if (cygpath_in)
    {
      fclose (cygpath_in);
      cygpath_in = NULL;
    }
  if (cygpath_pex)
    {
      pex_free (cygpath_pex);
      cygpath_pex = NULL;
    }

  return false;
}

/* Returns the handle for the MVCRT DLL, or NULL if it is not
   available.  */
static HANDLE
msvcrt_dll (void)
{
  static HANDLE dll = INVALID_HANDLE_VALUE;

  /* After we call LoadLibrary, DLL will be either a valid handle or
     NULL, so this check ensures that we only try to load the library
     once.  */
  if (dll == INVALID_HANDLE_VALUE)
    dll = LoadLibrary ("msvcrt.dll");

  return dll;
}

/* Call the underlying MSVCRT fopen with PATH and MODE, and return
   what it returns.  */
static FILE *
msvcrt_fopen (const char *path, const char *mode)
{
  typedef FILE *(fopen_type)(const char *path, 
			     const char *mode);

  static fopen_type *f = NULL;

  /* Get the address of "fopen".  */
  if (!f) 
    {
      HANDLE dll = msvcrt_dll ();
      if (!dll)
	{
	  errno = ENOSYS;
	  return NULL;
	}
      f = (fopen_type *) GetProcAddress (dll, "fopen");
      if (!f)
	{
	  errno = ENOSYS;
	  return NULL;
	}
    }

  /* Call fopen.  */
  return (*f)(path, mode);
}

FILE *
fopen (const char *path, const char *mode)
{
  FILE *f;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  f = msvcrt_fopen (path, mode);
  if (f || errno != ENOENT)
    return f;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    f = msvcrt_fopen (win32_path, mode);
  return f;
}

int 
open (const char *path, int oflag, ...)
{
  int fd;
  char win32_path[MAX_PATH + 1];
  int pmode = 0;

  if ((oflag & _O_CREAT))
    {
      va_list ap;
      va_start (ap, oflag);
      pmode = va_arg (ap, int); 
      va_end (ap);
    }

  /* Assume PATH is a Windows path.  */
  fd = _open (path, oflag, pmode);
  if (fd != -1 || errno != ENOENT)
    return fd;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    fd = _open (win32_path, oflag, pmode);
  return fd;
}

int
stat (const char *path, struct stat *buffer)
{
  int r;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  r = _stat (path, (struct _stat *) buffer);
  if (r != -1 || errno != ENOENT)
    return r;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    r = _stat (win32_path, (struct _stat *) buffer);
  return r;
}

int
access (const char *path, int mode)
{
  int r;
  char win32_path[MAX_PATH + 1];

  /* Assume PATH is a Windows path.  */
  r = _access (path, mode);
  if (r != -1 || errno != ENOENT)
    return r;
  /* Perhaps it is a Cygwin path?  */
  if (cygpath (path, win32_path))
    r = _access (win32_path, mode);
  return r;
}
