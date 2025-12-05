/* Support for connecting Guile's stdio to GDB's.
   as well as r/w memory via ports.

   Copyright (C) 2014-2025 Free Software Foundation, Inc.

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

/* See README file in this directory for implementation notes, coding
   conventions, et.al.  */

#include "gdbsupport/gdb_select.h"
#include "ui.h"
#include "target.h"
#include "guile-internal.h"
#include <optional>

#ifdef HAVE_POLL
#if defined (HAVE_POLL_H)
#include <poll.h>
#elif defined (HAVE_SYS_POLL_H)
#include <sys/poll.h>
#endif
#endif

/* A ui-file for sending output to Guile.  */

class ioscm_file_port : public ui_file
{
public:
  /* Return a ui_file that writes to PORT.  */
  explicit ioscm_file_port (SCM port);

  void flush () override;
  void write (const char *buf, long length_buf) override;

private:
  SCM m_port;
};

/* Data for a memory port.  */

struct ioscm_memory_port
{
  /* Bounds of memory range this port is allowed to access: [start, end).
     This means that 0xff..ff is not accessible.  I can live with that.  */
  CORE_ADDR start, end;

  /* (end - start), recorded for convenience.  */
  ULONGEST size;

  /* Think of this as the lseek value maintained by the kernel.
     This value is always in the range [0, size].  */
  ULONGEST current;
};

/* Copies of the original system input/output/error ports.
   These are recorded for debugging purposes.  */
static SCM orig_input_port_scm;
static SCM orig_output_port_scm;
static SCM orig_error_port_scm;

/* This is the stdio port descriptor, scm_ptob_descriptor.  */
static scm_t_port_type *stdio_port_desc;

/* Note: scm_make_port_type takes a char * instead of a const char *.  */
static /*const*/ char stdio_port_desc_name[] = "gdb:stdio-port";

/* Names of each gdb port.  */
static const char input_port_name[] = "gdb:stdin";
static const char output_port_name[] = "gdb:stdout";
static const char error_port_name[] = "gdb:stderr";

/* This is the actual port used from Guile.
   We don't expose these to the user though, to ensure they're not
   overwritten.  */
static SCM input_port_scm;
static SCM output_port_scm;
static SCM error_port_scm;

/* Internal enum for specifying output port.  */
enum oport { GDB_STDOUT, GDB_STDERR };

/* This is the memory port descriptor, scm_ptob_descriptor.  */
static scm_t_port_type *memory_port_desc;

/* Note: scm_make_port_type takes a char * instead of a const char *.  */
static /*const*/ char memory_port_desc_name[] = "gdb:memory-port";

/* The maximum values to use for get_natural_buffer_sizes.  */
static const unsigned natural_buf_size = 16;

/* Keywords used by open-memory.  */
static SCM mode_keyword;
static SCM start_keyword;
static SCM size_keyword;

/* Helper to do the low level work of opening a port.  */

static SCM
ioscm_open_port (scm_t_port_type *port_type, long mode_bits, scm_t_bits stream)
{
  return scm_c_make_port (port_type, mode_bits, stream);
}


/* Support for connecting Guile's stdio ports to GDB's stdio ports.  */

/* Print a string S, length SIZE, but don't escape characters, except
   nul.  */

static void
fputsn_filtered (const char *s, size_t size, struct ui_file *stream)
{
  size_t i;

  for (i = 0; i < size; ++i)
    {
      if (s[i] == '\0')
	gdb_puts ("\\000", stream);
      else
	gdb_putc (s[i], stream);
    }
}

/* Read up to COUNT bytes into bytevector DST at offset START.  Return the
   number of bytes read, zero for the end of file.  */

static size_t
ioscm_read_from_port (SCM port, SCM dst, size_t start, size_t count)
{
  long read;
  char *read_buf;

  /* If we're called on stdout,stderr, punt.  */
  if (! scm_is_eq (port, input_port_scm))
    return 0;

  gdb_flush (gdb_stdout);
  gdb_flush (gdb_stderr);

  read_buf = (char *) SCM_BYTEVECTOR_CONTENTS (dst) + start;
  read = gdb_stdin->read (read_buf, count);
  if (read == -1)
    scm_syserror (FUNC_NAME);

  return (size_t) read;
}

/* Write to gdb's stdout or stderr.  */

static size_t
ioscm_write (SCM port, SCM src, size_t start, size_t count)
{
  const char *data = (char *) SCM_BYTEVECTOR_CONTENTS (src) + start;

  /* If we're called on stdin, punt.  */
  if (scm_is_eq (port, input_port_scm))
    return 0;

  gdbscm_gdb_exception exc {};
  try
    {
      if (scm_is_eq (port, error_port_scm))
	fputsn_filtered ((const char *) data, count, gdb_stderr);
      else
	fputsn_filtered ((const char *) data, count, gdb_stdout);
    }
  catch (const gdb_exception &except)
    {
      exc = unpack (except);
    }
  GDBSCM_HANDLE_GDB_EXCEPTION (exc);

  return count;
}

/* Initialize the gdb stdio port type.

   N.B. isatty? will fail on these ports, it is only supported for file
   ports.  IWBN if we could "subclass" file ports.  */

static void
ioscm_init_gdb_stdio_port (void)
{
  stdio_port_desc = scm_make_port_type (stdio_port_desc_name,
					ioscm_read_from_port,
					ioscm_write);

  scm_set_port_read_wait_fd (stdio_port_desc, STDIN_FILENO);
}

#define GDB_STDIO_BUFFER_DEFAULT_SIZE 1024

static void
ioscm_init_stdio_buffers (SCM port, long mode_bits)
{
  if (mode_bits & SCM_BUF0)
    scm_setvbuf (port, scm_from_utf8_symbol ("none"), scm_from_size_t (0));
  else
    scm_setvbuf (port, scm_from_utf8_symbol ("block"),
		 scm_from_size_t (GDB_STDIO_BUFFER_DEFAULT_SIZE));
}

/* Create a gdb stdio port.  */

static SCM
ioscm_make_gdb_stdio_port (int fd)
{
  int is_a_tty = isatty (fd);
  const char *name;
  const char *mode_str;
  long mode_bits;
  SCM port;

  switch (fd)
    {
    case 0:
      name = input_port_name;
      mode_str = is_a_tty ? "r0" : "r";
      break;
    case 1:
      name = output_port_name;
      mode_str = is_a_tty ? "w0" : "w";
      break;
    case 2:
      name = error_port_name;
      mode_str = is_a_tty ? "w0" : "w";
      break;
    default:
      gdb_assert_not_reached ("bad stdio file descriptor");
    }

  mode_bits = scm_mode_bits ((char *) mode_str);
  port = ioscm_open_port (stdio_port_desc, mode_bits, 0);

  scm_set_port_filename_x (port, gdbscm_scm_from_c_string (name));

  ioscm_init_stdio_buffers (port, mode_bits);

  return port;
}

/* (stdio-port? object) -> boolean */

static SCM
gdbscm_stdio_port_p (SCM scm)
{
  return scm_from_bool (SCM_PORTP (scm)
			&& (SCM_PORT_TYPE (scm) == stdio_port_desc));
}

/* GDB's ports are accessed via functions to keep them read-only.  */

/* (input-port) -> port */

static SCM
gdbscm_input_port (void)
{
  return input_port_scm;
}

/* (output-port) -> port */

static SCM
gdbscm_output_port (void)
{
  return output_port_scm;
}

/* (error-port) -> port */

static SCM
gdbscm_error_port (void)
{
  return error_port_scm;
}

/* Support for sending GDB I/O to Guile ports.  */

ioscm_file_port::ioscm_file_port (SCM port)
  : m_port (port)
{}

void
ioscm_file_port::flush ()
{
}

void
ioscm_file_port::write (const char *buffer, long length_buffer)
{
  scm_c_write (m_port, buffer, length_buffer);
}


/* Helper routine for with-{output,error}-to-port.  */

static SCM
ioscm_with_output_to_port_worker (SCM port, SCM thunk, enum oport oport,
				  const char *func_name)
{
  SCM result;

  SCM_ASSERT_TYPE (gdbscm_is_true (scm_output_port_p (port)), port,
		   SCM_ARG1, func_name, _("output port"));
  SCM_ASSERT_TYPE (gdbscm_is_true (scm_thunk_p (thunk)), thunk,
		   SCM_ARG2, func_name, _("thunk"));

  set_batch_flag_and_restore_page_info save_page_info;

  scoped_restore restore_async = make_scoped_restore (&current_ui->async, 0);

  ui_file_up port_file (new ioscm_file_port (port));

  scoped_restore save_file = make_scoped_restore (oport == GDB_STDERR
						  ? &gdb_stderr : &gdb_stdout);

  {
    std::optional<ui_out_redirect_pop> redirect_popper;
    if (oport == GDB_STDERR)
      gdb_stderr = port_file.get ();
    else
      {
	redirect_popper.emplace (current_uiout, port_file.get ());

	gdb_stdout = port_file.get ();
      }

    result = gdbscm_safe_call_0 (thunk, NULL);
  }

  if (gdbscm_is_exception (result))
    gdbscm_throw (result);

  return result;
}

/* (%with-gdb-output-to-port port thunk) -> object
   This function is experimental.
   IWBN to not include "gdb" in the name, but it would collide with a standard
   procedure, and it's common to import the gdb module without a prefix.
   There are ways around this, but they're more cumbersome.

   This has % in the name because it's experimental, and we want the
   user-visible version to come from module (gdb experimental).  */

static SCM
gdbscm_percent_with_gdb_output_to_port (SCM port, SCM thunk)
{
  return ioscm_with_output_to_port_worker (port, thunk, GDB_STDOUT, FUNC_NAME);
}

/* (%with-gdb-error-to-port port thunk) -> object
   This function is experimental.
   IWBN to not include "gdb" in the name, but it would collide with a standard
   procedure, and it's common to import the gdb module without a prefix.
   There are ways around this, but they're more cumbersome.

   This has % in the name because it's experimental, and we want the
   user-visible version to come from module (gdb experimental).  */

static SCM
gdbscm_percent_with_gdb_error_to_port (SCM port, SCM thunk)
{
  return ioscm_with_output_to_port_worker (port, thunk, GDB_STDERR, FUNC_NAME);
}

/* Support for r/w memory via ports.  */

/* Perform an "lseek" to OFFSET,WHENCE on memory port IOMEM.
   OFFSET must be in the range [0,size].
   The result is non-zero for success, zero for failure.  */

static int
ioscm_lseek_address (ioscm_memory_port *iomem, LONGEST offset, int whence)
{
  CORE_ADDR new_current;

  gdb_assert (iomem->current <= iomem->size);

  switch (whence)
    {
    case SEEK_CUR:
      /* Catch over/underflow.  */
      if ((offset < 0 && iomem->current + offset > iomem->current)
	  || (offset > 0 && iomem->current + offset < iomem->current))
	return 0;
      new_current = iomem->current + offset;
      break;
    case SEEK_SET:
      new_current = offset;
      break;
    case SEEK_END:
      if (offset == 0)
	{
	  new_current = iomem->size;
	  break;
	}
      /* TODO: Not supported yet.  */
      return 0;
    default:
      return 0;
    }

  if (new_current > iomem->size)
    return 0;
  iomem->current = new_current;
  return 1;
}

/* The semantics get weird if the buffer size is larger than the port range,
   so provide a better default buffer size.  */

static void
gdbscm_get_natural_buffer_sizes (SCM port, size_t *read_size,
				 size_t *write_size)
{
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (port);

  size_t size = natural_buf_size;
  if (iomem != NULL && iomem->size < size)
    size = iomem->size;
  *read_size = *write_size = size;
}

/* Read up to COUNT bytes into bytevector DST at offset START.  Return the
   number of bytes read, zero for the end of file.  */

static size_t
gdbscm_memory_port_read (SCM port, SCM dst, size_t start, size_t count)
{
  gdb_byte *read_buf;
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (port);

  /* "current" is the offset of the first byte we want to read.  */
  gdb_assert (iomem->current <= iomem->size);
  if (iomem->current == iomem->size)
    return 0;

  /* Don't read outside the allowed memory range.  */
  if (count > iomem->size - iomem->current)
    count = iomem->size - iomem->current;

  read_buf = (gdb_byte *) SCM_BYTEVECTOR_CONTENTS (dst) + start;
  if (target_read_memory (iomem->start + iomem->current, read_buf,
			  count) != 0)
    gdbscm_memory_error (FUNC_NAME, _("error reading memory"), SCM_EOL);

  iomem->current += count;
  return count;
}

static size_t
gdbscm_memory_port_write (SCM port, SCM src, size_t start, size_t count)
{
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (port);
  const gdb_byte *data =
    (const gdb_byte *) SCM_BYTEVECTOR_CONTENTS (src) + start;

  /* If the request goes past the end of the port's memory range, flag an
     error.  */
  if (count > iomem->size - iomem->current)
    gdbscm_out_of_range_error (FUNC_NAME, 0, scm_from_size_t (count),
			       _("writing beyond end of memory range"));

  if (target_write_memory (iomem->start + iomem->current, data,
			   count) != 0)
    gdbscm_memory_error (FUNC_NAME, _("error writing memory"),
			 SCM_EOL);

  iomem->current += count;

  return count;
}

static scm_t_off
gdbscm_memory_port_seek (SCM port, scm_t_off offset, int whence)
{
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (port);
  int rc;

  rc = ioscm_lseek_address (iomem, offset, whence);
  if (rc == 0)
    gdbscm_out_of_range_error (FUNC_NAME, 0,
			       gdbscm_scm_from_longest (offset),
			       _("bad seek"));

  /* TODO: The Guile API doesn't support 32x64.  We can't fix that here,
     and there's no need to throw an error if the new address can't be
     represented in a scm_t_off.  But we could return something less
     clumsy.  */
  return iomem->current;
}

static void
gdbscm_memory_port_close (SCM port)
{
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (port);
  scm_gc_free (iomem, sizeof (*iomem), "memory port");
  SCM_SETSTREAM (port, NULL);
}

/* "print" method for memory ports.  */

static int
gdbscm_memory_port_print (SCM exp, SCM port, scm_print_state *pstate)
{
  ioscm_memory_port *iomem = (ioscm_memory_port *) SCM_STREAM (exp);

  scm_puts ("#<", port);
  scm_print_port_mode (exp, port);
  /* scm_print_port_mode includes a trailing space.  */
  gdbscm_printf (port, "%s %s-%s", memory_port_desc_name,
		 hex_string (iomem->start), hex_string (iomem->end));
  scm_putc ('>', port);
  return 1;
}

/* Create the port type used for memory.  */

static void
ioscm_init_memory_port_type (void)
{
  memory_port_desc = scm_make_port_type (memory_port_desc_name,
					 gdbscm_memory_port_read,
					 gdbscm_memory_port_write);

  scm_set_port_get_natural_buffer_sizes (memory_port_desc,
					 gdbscm_get_natural_buffer_sizes);
  scm_set_port_seek (memory_port_desc, gdbscm_memory_port_seek);
  scm_set_port_close (memory_port_desc, gdbscm_memory_port_close);
  scm_set_port_print (memory_port_desc, gdbscm_memory_port_print);
}

/* Helper for gdbscm_open_memory to parse the mode bits.
   An exception is thrown if MODE is invalid.  */

static long
ioscm_parse_mode_bits (const char *func_name, const char *mode)
{
  const char *p;
  long mode_bits;

  if (*mode != 'r' && *mode != 'w')
    {
      gdbscm_out_of_range_error (func_name, 0,
				 gdbscm_scm_from_c_string (mode),
				 _("bad mode string"));
    }
  for (p = mode + 1; *p != '\0'; ++p)
    {
      switch (*p)
	{
	case '0':
	case 'b':
	case '+':
	  break;
	default:
	  gdbscm_out_of_range_error (func_name, 0,
				     gdbscm_scm_from_c_string (mode),
				     _("bad mode string"));
	}
    }

  /* Kinda awkward to convert the mode from SCM -> string only to have Guile
     convert it back to SCM, but that's the API we have to work with.  */
  mode_bits = scm_mode_bits ((char *) mode);

  return mode_bits;
}

/* Return the memory object to be used as a "stream" associated with a memory
   port for the START--END range.  */

static ioscm_memory_port *
ioscm_init_memory_port_stream (CORE_ADDR start, CORE_ADDR end)
{
  ioscm_memory_port *iomem;

  gdb_assert (start <= end);

  iomem = (ioscm_memory_port *) scm_gc_malloc_pointerless (sizeof (*iomem),
							   "memory port");

  iomem->start = start;
  iomem->end = end;
  iomem->size = end - start;
  iomem->current = 0;

  return iomem;
}

/* (open-memory [#:mode string] [#:start address] [#:size integer]) -> port
   Return a port that can be used for reading and writing memory.
   MODE is a string, and must be one of "r", "w", or "r+".
   "0" may be appended to MODE to mark the port as unbuffered.
   For compatibility "b" (binary) may also be appended, but we ignore it:
   memory ports are binary only.

   The chunk of memory that can be accessed can be bounded.
   If both START,SIZE are unspecified, all of memory can be accessed
   (except 0xff..ff).  If only START is specified, all of memory from that
   point on can be accessed (except 0xff..ff).  If only SIZE if specified,
   all memory in [0,SIZE) can be accessed.  If both are specified, all memory
   in [START,START+SIZE) can be accessed.

   Note: If it becomes useful enough we can later add #:end as an alternative
   to #:size.  For now it is left out.

   The result is a Scheme port, and its semantics are a bit odd for accessing
   memory (e.g., unget), but we don't try to hide this.  It's a port.

   N.B. Seeks on the port must be in the range [0,size].
   This is for similarity with bytevector ports, and so that one can seek
   to the first byte.  */

static SCM
gdbscm_open_memory (SCM rest)
{
  const SCM keywords[] = {
    mode_keyword, start_keyword, size_keyword, SCM_BOOL_F
  };
  char *mode = NULL;
  CORE_ADDR start = 0;
  CORE_ADDR end;
  int mode_arg_pos = -1, start_arg_pos = -1, size_arg_pos = -1;
  ULONGEST size;
  SCM port;
  long mode_bits;

  gdbscm_parse_function_args (FUNC_NAME, SCM_ARG1, keywords, "#sUU", rest,
			      &mode_arg_pos, &mode,
			      &start_arg_pos, &start,
			      &size_arg_pos, &size);

  scm_dynwind_begin ((scm_t_dynwind_flags) 0);

  if (mode == NULL)
    mode = xstrdup ("r");
  scm_dynwind_free (mode);

  if (size_arg_pos > 0)
    {
      /* For now be strict about start+size overflowing.  If it becomes
	 a nuisance we can relax things later.  */
      if (start + size < start)
	{
	  gdbscm_out_of_range_error (FUNC_NAME, 0,
				scm_list_2 (gdbscm_scm_from_ulongest (start),
					    gdbscm_scm_from_ulongest (size)),
				     _("start+size overflows"));
	}
      end = start + size;
    }
  else
    end = ~(CORE_ADDR) 0;

  mode_bits = ioscm_parse_mode_bits (FUNC_NAME, mode);

  /* Edge case: empty range -> unbuffered.
     There's no need to disallow empty ranges, but we need an unbuffered port
     to get the semantics right.  */
  if (size == 0)
    mode_bits |= SCM_BUF0;

  auto stream = ioscm_init_memory_port_stream (start, end);
  port = ioscm_open_port (memory_port_desc, mode_bits,
			  (scm_t_bits) stream);

  scm_dynwind_end ();

  /* TODO: Set the file name as "memory-start-end"?  */
  return port;
}

/* Return non-zero if OBJ is a memory port.  */

static int
gdbscm_is_memory_port (SCM obj)
{
  return SCM_PORTP (obj) && (SCM_PORT_TYPE (obj) == memory_port_desc);
}

/* (memory-port? obj) -> boolean */

static SCM
gdbscm_memory_port_p (SCM obj)
{
  return scm_from_bool (gdbscm_is_memory_port (obj));
}

/* (memory-port-range port) -> (start end) */

static SCM
gdbscm_memory_port_range (SCM port)
{
  ioscm_memory_port *iomem;

  SCM_ASSERT_TYPE (gdbscm_is_memory_port (port), port, SCM_ARG1, FUNC_NAME,
		   memory_port_desc_name);

  iomem = (ioscm_memory_port *) SCM_STREAM (port);
  return scm_list_2 (gdbscm_scm_from_ulongest (iomem->start),
		     gdbscm_scm_from_ulongest (iomem->end));
}

/* Initialize gdb ports.  */

static const scheme_function port_functions[] =
{
  { "input-port", 0, 0, 0, as_a_scm_t_subr (gdbscm_input_port),
    "\
Return gdb's input port." },

  { "output-port", 0, 0, 0, as_a_scm_t_subr (gdbscm_output_port),
    "\
Return gdb's output port." },

  { "error-port", 0, 0, 0, as_a_scm_t_subr (gdbscm_error_port),
    "\
Return gdb's error port." },

  { "stdio-port?", 1, 0, 0, as_a_scm_t_subr (gdbscm_stdio_port_p),
    "\
Return #t if the object is a gdb:stdio-port." },

  { "open-memory", 0, 0, 1, as_a_scm_t_subr (gdbscm_open_memory),
    "\
Return a port that can be used for reading/writing inferior memory.\n\
\n\
  Arguments: [#:mode string] [#:start address] [#:size integer]\n\
  Returns: A port object." },

  { "memory-port?", 1, 0, 0, as_a_scm_t_subr (gdbscm_memory_port_p),
    "\
Return #t if the object is a memory port." },

  { "memory-port-range", 1, 0, 0, as_a_scm_t_subr (gdbscm_memory_port_range),
    "\
Return the memory range of the port as (start end)." },

  END_FUNCTIONS
};

static const scheme_function private_port_functions[] =
{
#if 0 /* TODO */
  { "%with-gdb-input-from-port", 2, 0, 0,
    as_a_scm_t_subr (gdbscm_percent_with_gdb_input_from_port),
    "\
Temporarily set GDB's input port to PORT and then invoke THUNK.\n\
\n\
  Arguments: port thunk\n\
  Returns: The result of calling THUNK.\n\
\n\
This procedure is experimental." },
#endif

  { "%with-gdb-output-to-port", 2, 0, 0,
    as_a_scm_t_subr (gdbscm_percent_with_gdb_output_to_port),
    "\
Temporarily set GDB's output port to PORT and then invoke THUNK.\n\
\n\
  Arguments: port thunk\n\
  Returns: The result of calling THUNK.\n\
\n\
This procedure is experimental." },

  { "%with-gdb-error-to-port", 2, 0, 0,
    as_a_scm_t_subr (gdbscm_percent_with_gdb_error_to_port),
    "\
Temporarily set GDB's error port to PORT and then invoke THUNK.\n\
\n\
  Arguments: port thunk\n\
  Returns: The result of calling THUNK.\n\
\n\
This procedure is experimental." },

  END_FUNCTIONS
};

void
gdbscm_initialize_ports (void)
{
  /* Save the original stdio ports for debugging purposes.  */

  orig_input_port_scm = scm_current_input_port ();
  orig_output_port_scm = scm_current_output_port ();
  orig_error_port_scm = scm_current_error_port ();

  /* Set up the stdio ports.  */

  ioscm_init_gdb_stdio_port ();
  input_port_scm = ioscm_make_gdb_stdio_port (0);
  output_port_scm = ioscm_make_gdb_stdio_port (1);
  error_port_scm = ioscm_make_gdb_stdio_port (2);

  /* Set up memory ports.  */

  ioscm_init_memory_port_type ();

  /* Install the accessor functions.  */

  gdbscm_define_functions (port_functions, 1);
  gdbscm_define_functions (private_port_functions, 0);

  /* Keyword args for open-memory.  */

  mode_keyword = scm_from_latin1_keyword ("mode");
  start_keyword = scm_from_latin1_keyword ("start");
  size_keyword = scm_from_latin1_keyword ("size");

}
