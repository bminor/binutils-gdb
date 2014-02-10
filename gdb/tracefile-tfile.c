/* Trace file TFILE format support in GDB.

   Copyright (C) 1997-2014 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "tracefile.h"
#include "readline/tilde.h"
#include "filestuff.h"
#include "rsp-low.h" /* bin2hex */

/* TFILE trace writer.  */

struct tfile_trace_file_writer
{
  struct trace_file_writer base;

  /* File pointer to tfile trace file.  */
  FILE *fp;
  /* Path name of the tfile trace file.  */
  char *pathname;
};

/* This is the implementation of trace_file_write_ops method
   target_save.  We just call the generic target
   target_save_trace_data to do target-side saving.  */

static int
tfile_target_save (struct trace_file_writer *self,
		   const char *filename)
{
  int err = target_save_trace_data (filename);

  return (err >= 0);
}

/* This is the implementation of trace_file_write_ops method
   dtor.  */

static void
tfile_dtor (struct trace_file_writer *self)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  xfree (writer->pathname);

  if (writer->fp != NULL)
    fclose (writer->fp);
}

/* This is the implementation of trace_file_write_ops method
   start.  It creates the trace file FILENAME and registers some
   cleanups.  */

static void
tfile_start (struct trace_file_writer *self, const char *filename)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  writer->pathname = tilde_expand (filename);
  writer->fp = gdb_fopen_cloexec (writer->pathname, "wb");
  if (writer->fp == NULL)
    error (_("Unable to open file '%s' for saving trace data (%s)"),
	   writer->pathname, safe_strerror (errno));
}

/* This is the implementation of trace_file_write_ops method
   write_header.  Write the TFILE header.  */

static void
tfile_write_header (struct trace_file_writer *self)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;
  int written;

  /* Write a file header, with a high-bit-set char to indicate a
     binary file, plus a hint as what this file is, and a version
     number in case of future needs.  */
  written = fwrite ("\x7fTRACE0\n", 8, 1, writer->fp);
  if (written < 1)
    perror_with_name (writer->pathname);
}

/* This is the implementation of trace_file_write_ops method
   write_regblock_type.  Write the size of register block.  */

static void
tfile_write_regblock_type (struct trace_file_writer *self, int size)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  fprintf (writer->fp, "R %x\n", size);
}

/* This is the implementation of trace_file_write_ops method
   write_status.  */

static void
tfile_write_status (struct trace_file_writer *self,
		    struct trace_status *ts)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  fprintf (writer->fp, "status %c;%s",
	   (ts->running ? '1' : '0'), stop_reason_names[ts->stop_reason]);
  if (ts->stop_reason == tracepoint_error
      || ts->stop_reason == tstop_command)
    {
      char *buf = (char *) alloca (strlen (ts->stop_desc) * 2 + 1);

      bin2hex ((gdb_byte *) ts->stop_desc, buf, strlen (ts->stop_desc));
      fprintf (writer->fp, ":%s", buf);
    }
  fprintf (writer->fp, ":%x", ts->stopping_tracepoint);
  if (ts->traceframe_count >= 0)
    fprintf (writer->fp, ";tframes:%x", ts->traceframe_count);
  if (ts->traceframes_created >= 0)
    fprintf (writer->fp, ";tcreated:%x", ts->traceframes_created);
  if (ts->buffer_free >= 0)
    fprintf (writer->fp, ";tfree:%x", ts->buffer_free);
  if (ts->buffer_size >= 0)
    fprintf (writer->fp, ";tsize:%x", ts->buffer_size);
  if (ts->disconnected_tracing)
    fprintf (writer->fp, ";disconn:%x", ts->disconnected_tracing);
  if (ts->circular_buffer)
    fprintf (writer->fp, ";circular:%x", ts->circular_buffer);
  if (ts->start_time)
    {
      fprintf (writer->fp, ";starttime:%s",
      phex_nz (ts->start_time, sizeof (ts->start_time)));
    }
  if (ts->stop_time)
    {
      fprintf (writer->fp, ";stoptime:%s",
      phex_nz (ts->stop_time, sizeof (ts->stop_time)));
    }
  if (ts->notes != NULL)
    {
      char *buf = (char *) alloca (strlen (ts->notes) * 2 + 1);

      bin2hex ((gdb_byte *) ts->notes, buf, strlen (ts->notes));
      fprintf (writer->fp, ";notes:%s", buf);
    }
  if (ts->user_name != NULL)
    {
      char *buf = (char *) alloca (strlen (ts->user_name) * 2 + 1);

      bin2hex ((gdb_byte *) ts->user_name, buf, strlen (ts->user_name));
      fprintf (writer->fp, ";username:%s", buf);
    }
  fprintf (writer->fp, "\n");
}

/* This is the implementation of trace_file_write_ops method
   write_uploaded_tsv.  */

static void
tfile_write_uploaded_tsv (struct trace_file_writer *self,
			  struct uploaded_tsv *utsv)
{
  char *buf = "";
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  if (utsv->name)
    {
      buf = (char *) xmalloc (strlen (utsv->name) * 2 + 1);
      bin2hex ((gdb_byte *) (utsv->name), buf, strlen (utsv->name));
    }

  fprintf (writer->fp, "tsv %x:%s:%x:%s\n",
	   utsv->number, phex_nz (utsv->initial_value, 8),
	   utsv->builtin, buf);

  if (utsv->name)
    xfree (buf);
}

#define MAX_TRACE_UPLOAD 2000

/* This is the implementation of trace_file_write_ops method
   write_uploaded_tp.  */

static void
tfile_write_uploaded_tp (struct trace_file_writer *self,
			 struct uploaded_tp *utp)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;
  int a;
  char *act;
  char buf[MAX_TRACE_UPLOAD];

  fprintf (writer->fp, "tp T%x:%s:%c:%x:%x",
	   utp->number, phex_nz (utp->addr, sizeof (utp->addr)),
	   (utp->enabled ? 'E' : 'D'), utp->step, utp->pass);
  if (utp->type == bp_fast_tracepoint)
    fprintf (writer->fp, ":F%x", utp->orig_size);
  if (utp->cond)
    fprintf (writer->fp,
	     ":X%x,%s", (unsigned int) strlen (utp->cond) / 2,
	     utp->cond);
  fprintf (writer->fp, "\n");
  for (a = 0; VEC_iterate (char_ptr, utp->actions, a, act); ++a)
    fprintf (writer->fp, "tp A%x:%s:%s\n",
	     utp->number, phex_nz (utp->addr, sizeof (utp->addr)), act);
  for (a = 0; VEC_iterate (char_ptr, utp->step_actions, a, act); ++a)
    fprintf (writer->fp, "tp S%x:%s:%s\n",
	     utp->number, phex_nz (utp->addr, sizeof (utp->addr)), act);
  if (utp->at_string)
    {
      encode_source_string (utp->number, utp->addr,
			    "at", utp->at_string, buf, MAX_TRACE_UPLOAD);
      fprintf (writer->fp, "tp Z%s\n", buf);
    }
  if (utp->cond_string)
    {
      encode_source_string (utp->number, utp->addr,
			    "cond", utp->cond_string,
			    buf, MAX_TRACE_UPLOAD);
      fprintf (writer->fp, "tp Z%s\n", buf);
    }
  for (a = 0; VEC_iterate (char_ptr, utp->cmd_strings, a, act); ++a)
    {
      encode_source_string (utp->number, utp->addr, "cmd", act,
			    buf, MAX_TRACE_UPLOAD);
      fprintf (writer->fp, "tp Z%s\n", buf);
    }
  fprintf (writer->fp, "tp V%x:%s:%x:%s\n",
	   utp->number, phex_nz (utp->addr, sizeof (utp->addr)),
	   utp->hit_count,
	   phex_nz (utp->traceframe_usage,
		    sizeof (utp->traceframe_usage)));
}

/* This is the implementation of trace_file_write_ops method
   write_definition_end.  */

static void
tfile_write_definition_end (struct trace_file_writer *self)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  fprintf (writer->fp, "\n");
}

/* This is the implementation of trace_file_write_ops method
   write_raw_data.  */

static void
tfile_write_raw_data (struct trace_file_writer *self, gdb_byte *buf,
		      LONGEST len)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;

  if (fwrite (buf, len, 1, writer->fp) < 1)
    perror_with_name (writer->pathname);
}

/* This is the implementation of trace_file_write_ops method
   end.  */

static void
tfile_end (struct trace_file_writer *self)
{
  struct tfile_trace_file_writer *writer
    = (struct tfile_trace_file_writer *) self;
  uint32_t gotten = 0;

  /* Mark the end of trace data.  */
  if (fwrite (&gotten, 4, 1, writer->fp) < 1)
    perror_with_name (writer->pathname);
}

/* Operations to write trace buffers into TFILE format.  */

static const struct trace_file_write_ops tfile_write_ops =
{
  tfile_dtor,
  tfile_target_save,
  tfile_start,
  tfile_write_header,
  tfile_write_regblock_type,
  tfile_write_status,
  tfile_write_uploaded_tsv,
  tfile_write_uploaded_tp,
  tfile_write_definition_end,
  tfile_write_raw_data,
  NULL,
  tfile_end,
};

/* Return a trace writer for TFILE format.  */

struct trace_file_writer *
tfile_trace_file_writer_new (void)
{
  struct tfile_trace_file_writer *writer
    = xmalloc (sizeof (struct tfile_trace_file_writer));

  writer->base.ops = &tfile_write_ops;
  writer->fp = NULL;
  writer->pathname = NULL;

  return (struct trace_file_writer *) writer;
}
