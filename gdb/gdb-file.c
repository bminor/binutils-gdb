/* GDB_FILE - a generic STDIO like output stream.
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.

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

/* Implement the ``struct gdb_file'' object. */

#include "defs.h"
#include "gdb-file.h"

#undef XMALLOC
#define XMALLOC(TYPE) ((TYPE*) xmalloc (sizeof (TYPE)))

static gdb_file_isatty_ftype null_file_isatty;
static gdb_file_write_ftype null_file_write;
static gdb_file_fputs_ftype null_file_fputs;
static gdb_file_flush_ftype null_file_flush;
static gdb_file_delete_ftype null_file_delete;
static gdb_file_rewind_ftype null_file_rewind;
static gdb_file_put_ftype null_file_put;

struct gdb_file
  {
    int *magic;
    gdb_file_flush_ftype *to_flush;
    gdb_file_write_ftype *to_write;
    gdb_file_fputs_ftype *to_fputs;
    gdb_file_delete_ftype *to_delete;
    gdb_file_isatty_ftype *to_isatty;
    gdb_file_rewind_ftype *to_rewind;
    gdb_file_put_ftype *to_put;
    void *to_data;
  };
int gdb_file_magic;

struct gdb_file *
gdb_file_new ()
{
  struct gdb_file *file = xmalloc (sizeof (struct gdb_file));
  file->magic = &gdb_file_magic;
  set_gdb_file_data (file, NULL, null_file_delete);
  set_gdb_file_flush (file, null_file_flush);
  set_gdb_file_write (file, null_file_write);
  set_gdb_file_fputs (file, null_file_fputs);
  set_gdb_file_isatty (file, null_file_isatty);
  set_gdb_file_rewind (file, null_file_rewind);
  set_gdb_file_put (file, null_file_put);
  return file;
}

void
gdb_file_delete (file)
     struct gdb_file *file;
{
  file->to_delete (file);
  free (file);
}

static int
null_file_isatty (file)
     struct gdb_file *file;
{
  return 0;
}

static void
null_file_rewind (file)
     struct gdb_file *file;
{
  return;
}

static void
null_file_put (struct gdb_file *file,
	       gdb_file_put_method_ftype *write,
	       void *dest)
{
  return;
}

static void
null_file_flush (file)
     struct gdb_file *file;
{
  return;
}

static void
null_file_write (struct gdb_file *file,
		 const char *buf,
		 long sizeof_buf)
{
  if (file->to_fputs == null_file_fputs)
    /* Both the write and fputs methods are null. Discard the
       request. */
    return;
  else
    {
      /* The fputs method isn't null, slowly pass the write request
         onto that.  FYI, this isn't as bad as it may look - the
         current (as of 1999-11-07) printf_* function calls fputc and
         fputc does exactly the below.  By having a write function it
         is possible to clean up that code.  */
      int i;
      char b[2];
      b[1] = '\0';
      for (i = 0; i < sizeof_buf; i++)
	{
	  b[0] = buf[i];
	  file->to_fputs (b, file);
	}
      return;
    }
}

static void
null_file_fputs (buf, file)
     const char *buf;
     struct gdb_file *file;
{
  if (file->to_write == null_file_write)
    /* Both the write and fputs methods are null. Discard the
       request. */
    return;
  else
    {
      /* The write method was implemented, use that. */
      file->to_write (file, buf, strlen (buf));
    }
}

static void
null_file_delete (file)
     struct gdb_file *file;
{
  return;
}

void *
gdb_file_data (file)
     struct gdb_file *file;
{
  if (file->magic != &gdb_file_magic)
    internal_error ("gdb_file_data: bad magic number");
  return file->to_data;
}

void
gdb_flush (file)
     struct gdb_file *file;
{
  file->to_flush (file);
}

int
gdb_file_isatty (file)
     struct gdb_file *file;
{
  return file->to_isatty (file);
}

void
gdb_file_rewind (file)
     struct gdb_file *file;
{
  file->to_rewind (file);
}

void
gdb_file_put (struct gdb_file *file,
	      gdb_file_put_method_ftype *write,
	      void *dest)
{
  file->to_put (file, write, dest);
}

void
gdb_file_write (struct gdb_file *file,
		const char *buf,
		long length_buf)
{
  file->to_write (file, buf, length_buf);
}

void
fputs_unfiltered (buf, file)
     const char *buf;
     struct gdb_file *file;
{
  file->to_fputs (buf, file);
}

void
set_gdb_file_flush (file, flush)
     struct gdb_file *file;
     gdb_file_flush_ftype *flush;
{
  file->to_flush = flush;
}

void
set_gdb_file_isatty (file, isatty)
     struct gdb_file *file;
     gdb_file_isatty_ftype *isatty;
{
  file->to_isatty = isatty;
}

void
set_gdb_file_rewind (file, rewind)
     struct gdb_file *file;
     gdb_file_rewind_ftype *rewind;
{
  file->to_rewind = rewind;
}

void
set_gdb_file_put (file, put)
     struct gdb_file *file;
     gdb_file_put_ftype *put;
{
  file->to_put = put;
}

void
set_gdb_file_write (struct gdb_file *file,
		    gdb_file_write_ftype *write)
{
  file->to_write = write;
}

void
set_gdb_file_fputs (file, fputs)
     struct gdb_file *file;
     gdb_file_fputs_ftype *fputs;
{
  file->to_fputs = fputs;
}

void
set_gdb_file_data (file, data, delete)
     struct gdb_file *file;
     void *data;
     gdb_file_delete_ftype *delete;
{
  file->to_data = data;
  file->to_delete = delete;
}

/* gdb_file utility function for converting a ``struct gdb_file'' into
   a memory buffer''. */

struct accumulated_gdb_file
{
  char *buffer;
  long length;
};

static void
do_gdb_file_xstrdup (void *context, const char *buffer, long length)
{
  struct accumulated_gdb_file *acc = context;
  if (acc->buffer == NULL)
    acc->buffer = xmalloc (length + 1);
  else
    acc->buffer = xrealloc (acc->buffer, acc->length + length + 1);
  memcpy (acc->buffer + acc->length, buffer, length);
  acc->length += length;
  acc->buffer[acc->length] = '\0';
}

char *
gdb_file_xstrdup (struct gdb_file *file,
		  long *length)
{
  struct accumulated_gdb_file acc;
  acc.buffer = NULL;
  acc.length = 0;
  gdb_file_put (file, do_gdb_file_xstrdup, &acc);
  if (acc.buffer == NULL)
    acc.buffer = xstrdup ("");
  *length = acc.length;
  return acc.buffer;
}

/* A pure memory based ``struct gdb_file'' that can be used an output
   buffer. The buffers accumulated contents are available via
   gdb_file_put(). */

struct mem_file
  {
    int *magic;
    char *buffer;
    int sizeof_buffer;
    int length_buffer;
  };

static gdb_file_rewind_ftype mem_file_rewind;
static gdb_file_put_ftype mem_file_put;
static gdb_file_write_ftype mem_file_write;
static gdb_file_delete_ftype mem_file_delete;
static struct gdb_file *mem_file_new PARAMS ((void));
static int mem_file_magic;

static struct gdb_file *
mem_file_new (void)
{
  struct mem_file *stream = XMALLOC (struct mem_file);
  struct gdb_file *file = gdb_file_new ();
  set_gdb_file_data (file, stream, mem_file_delete);
  set_gdb_file_rewind (file, mem_file_rewind);
  set_gdb_file_put (file, mem_file_put);
  set_gdb_file_write (file, mem_file_write);
  stream->magic = &mem_file_magic;
  stream->buffer = NULL;
  stream->sizeof_buffer = 0;
  stream->length_buffer = 0;
  return file;
}

static void
mem_file_delete (struct gdb_file *file)
{
  struct mem_file *stream = gdb_file_data (file);
  if (stream->magic != &mem_file_magic)
    internal_error ("mem_file_delete: bad magic number");
  if (stream->buffer != NULL)
    free (stream->buffer);
  free (stream);
}

struct gdb_file *
mem_fileopen (void)
{
  return mem_file_new ();
}

static void
mem_file_rewind (struct gdb_file *file)
{
  struct mem_file *stream = gdb_file_data (file);
  if (stream->magic != &mem_file_magic)
    internal_error ("mem_file_rewind: bad magic number");
  stream->length_buffer = 0;
}

static void
mem_file_put (struct gdb_file *file,
	      gdb_file_put_method_ftype *write,
	      void *dest)
{
  struct mem_file *stream = gdb_file_data (file);
  if (stream->magic != &mem_file_magic)
    internal_error ("mem_file_put: bad magic number");
  if (stream->length_buffer > 0)
    write (dest, stream->buffer, stream->length_buffer);
}

void
mem_file_write (struct gdb_file *file,
		const char *buffer,
		long length_buffer)
{
  struct mem_file *stream = gdb_file_data (file);
  if (stream->magic != &mem_file_magic)
    internal_error ("mem_file_write: bad magic number");
  if (stream->buffer == NULL)
    {
      stream->length_buffer = length_buffer;
      stream->sizeof_buffer = length_buffer;
      stream->buffer = xmalloc (stream->sizeof_buffer);
      memcpy (stream->buffer, buffer, length_buffer);
    }
  else
    {
      int new_length = stream->length_buffer + length_buffer;
      if (new_length >= stream->sizeof_buffer)
	{
	  stream->sizeof_buffer = new_length;
	  stream->buffer = xrealloc (stream->buffer, stream->sizeof_buffer);
	}
      memcpy (stream->buffer + stream->length_buffer, buffer, length_buffer);
      stream->length_buffer = new_length;
    }
}

/* ``struct gdb_file'' implementation that maps directly onto
   <stdio.h>'s FILE. */

static gdb_file_write_ftype stdio_file_write;
static gdb_file_fputs_ftype stdio_file_fputs;
static gdb_file_isatty_ftype stdio_file_isatty;
static gdb_file_delete_ftype stdio_file_delete;
static struct gdb_file *stdio_file_new PARAMS ((FILE * file, int close_p));
static gdb_file_flush_ftype stdio_file_flush;

static int stdio_file_magic;

struct stdio_file
  {
    int *magic;
    FILE *file;
    int close_p;
  };

static struct gdb_file *
stdio_file_new (file, close_p)
     FILE *file;
     int close_p;
{
  struct gdb_file *gdb_file = gdb_file_new ();
  struct stdio_file *stdio = xmalloc (sizeof (struct stdio_file));
  stdio->magic = &stdio_file_magic;
  stdio->file = file;
  stdio->close_p = close_p;
  set_gdb_file_data (gdb_file, stdio, stdio_file_delete);
  set_gdb_file_flush (gdb_file, stdio_file_flush);
  set_gdb_file_write (gdb_file, stdio_file_write);
  set_gdb_file_fputs (gdb_file, stdio_file_fputs);
  set_gdb_file_isatty (gdb_file, stdio_file_isatty);
  return gdb_file;
}

static void
stdio_file_delete (file)
     struct gdb_file *file;
{
  struct stdio_file *stdio = gdb_file_data (file);
  if (stdio->magic != &stdio_file_magic)
    internal_error ("stdio_file_delete: bad magic number");
  if (stdio->close_p)
    {
      fclose (stdio->file);
    }
  free (stdio);
}

static void
stdio_file_flush (file)
     struct gdb_file *file;
{
  struct stdio_file *stdio = gdb_file_data (file);
  if (stdio->magic != &stdio_file_magic)
    internal_error ("stdio_file_flush: bad magic number");
  fflush (stdio->file);
}

static void
stdio_file_write (struct gdb_file *file, const char *buf, long length_buf)
{
  struct stdio_file *stdio = gdb_file_data (file);
  if (stdio->magic != &stdio_file_magic)
    internal_error ("stdio_file_write: bad magic number");
  fwrite (buf, length_buf, 1, stdio->file);
}

static void
stdio_file_fputs (linebuffer, file)
     const char *linebuffer;
     struct gdb_file *file;
{
  struct stdio_file *stdio = gdb_file_data (file);
  if (stdio->magic != &stdio_file_magic)
    internal_error ("stdio_file_fputs: bad magic number");
  fputs (linebuffer, stdio->file);
}

static int
stdio_file_isatty (file)
     struct gdb_file *file;
{
  struct stdio_file *stdio = gdb_file_data (file);
  if (stdio->magic != &stdio_file_magic)
    internal_error ("stdio_file_isatty: bad magic number");
  return (isatty (fileno (stdio->file)));
}

/* Like fdopen().  Create a gdb_file from a previously opened FILE. */

struct gdb_file *
stdio_fileopen (file)
     FILE *file;
{
  return stdio_file_new (file, 0);
}

GDB_FILE *
gdb_fopen (name, mode)
     char *name;
     char *mode;
{
  FILE *f = fopen (name, mode);
  if (f == NULL)
    return NULL;
  return stdio_file_new (f, 1);
}
