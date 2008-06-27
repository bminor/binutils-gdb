/* Main code for remote server for GDB.
   Copyright (C) 1989, 1993, 1994, 1995, 1997, 1998, 1999, 2000, 2002, 2003,
   2004, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

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

#include "server.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

unsigned long cont_thread;
unsigned long general_thread;
unsigned long step_thread;
unsigned long thread_from_wait;
unsigned long old_thread_from_wait;
int server_waiting;

static int extended_protocol;
static int attached;
static int response_needed;
static int exit_requested;

static char **program_argv, **wrapper_argv;

/* Enable miscellaneous debugging output.  The name is historical - it
   was originally used to debug LinuxThreads support.  */
int debug_threads;

int pass_signals[TARGET_SIGNAL_LAST];

jmp_buf toplevel;

const char *gdbserver_xmltarget;

/* The PID of the originally created or attached inferior.  Used to
   send signals to the process when GDB sends us an asynchronous interrupt
   (user hitting Control-C in the client), and to wait for the child to exit
   when no longer debugging it.  */

unsigned long signal_pid;

#ifdef SIGTTOU
/* A file descriptor for the controlling terminal.  */
int terminal_fd;

/* TERMINAL_FD's original foreground group.  */
pid_t old_foreground_pgrp;

/* Set if you want to disable optional thread related packets support
   in gdbserver, for the sake of testing GDB against stubs that don't
   support them.  */
int disable_packet_vCont;
int disable_packet_Tthread;
int disable_packet_qC;
int disable_packet_qfThreadInfo;

/* Hand back terminal ownership to the original foreground group.  */

static void
restore_old_foreground_pgrp (void)
{
  tcsetpgrp (terminal_fd, old_foreground_pgrp);
}
#endif

static int
target_running (void)
{
  return all_threads.head != NULL;
}

static int
start_inferior (char **argv, char *statusptr)
{
  char **new_argv = argv;
  attached = 0;

  if (wrapper_argv != NULL)
    {
      int i, count = 1;

      for (i = 0; wrapper_argv[i] != NULL; i++)
	count++;
      for (i = 0; argv[i] != NULL; i++)
	count++;
      new_argv = alloca (sizeof (char *) * count);
      count = 0;
      for (i = 0; wrapper_argv[i] != NULL; i++)
	new_argv[count++] = wrapper_argv[i];
      for (i = 0; argv[i] != NULL; i++)
	new_argv[count++] = argv[i];
      new_argv[count] = NULL;
    }

#ifdef SIGTTOU
  signal (SIGTTOU, SIG_DFL);
  signal (SIGTTIN, SIG_DFL);
#endif

  signal_pid = create_inferior (new_argv[0], new_argv);

  /* FIXME: we don't actually know at this point that the create
     actually succeeded.  We won't know that until we wait.  */
  fprintf (stderr, "Process %s created; pid = %ld\n", argv[0],
	   signal_pid);
  fflush (stderr);

#ifdef SIGTTOU
  signal (SIGTTOU, SIG_IGN);
  signal (SIGTTIN, SIG_IGN);
  terminal_fd = fileno (stderr);
  old_foreground_pgrp = tcgetpgrp (terminal_fd);
  tcsetpgrp (terminal_fd, signal_pid);
  atexit (restore_old_foreground_pgrp);
#endif

  if (wrapper_argv != NULL)
    {
      struct thread_resume resume_info;
      int sig;

      resume_info.thread = -1;
      resume_info.step = 0;
      resume_info.sig = 0;
      resume_info.leave_stopped = 0;

      sig = mywait (statusptr, 0);
      if (*statusptr != 'T')
	return sig;

      do
	{
	  (*the_target->resume) (&resume_info);

	  sig = mywait (statusptr, 0);
	  if (*statusptr != 'T')
	    return sig;
	}
      while (sig != TARGET_SIGNAL_TRAP);

      return sig;
    }

  /* Wait till we are at 1st instruction in program, return signal
     number (assuming success).  */
  return mywait (statusptr, 0);
}

static int
attach_inferior (int pid, char *statusptr, int *sigptr)
{
  /* myattach should return -1 if attaching is unsupported,
     0 if it succeeded, and call error() otherwise.  */

  if (myattach (pid) != 0)
    return -1;

  attached = 1;

  fprintf (stderr, "Attached; pid = %d\n", pid);
  fflush (stderr);

  /* FIXME - It may be that we should get the SIGNAL_PID from the
     attach function, so that it can be the main thread instead of
     whichever we were told to attach to.  */
  signal_pid = pid;

  *sigptr = mywait (statusptr, 0);

  /* GDB knows to ignore the first SIGSTOP after attaching to a running
     process using the "attach" command, but this is different; it's
     just using "target remote".  Pretend it's just starting up.  */
  if (*statusptr == 'T' && *sigptr == TARGET_SIGNAL_STOP)
    *sigptr = TARGET_SIGNAL_TRAP;

  return 0;
}

extern int remote_debug;

/* Decode a qXfer read request.  Return 0 if everything looks OK,
   or -1 otherwise.  */

static int
decode_xfer_read (char *buf, char **annex, CORE_ADDR *ofs, unsigned int *len)
{
  /* Extract and NUL-terminate the annex.  */
  *annex = buf;
  while (*buf && *buf != ':')
    buf++;
  if (*buf == '\0')
    return -1;
  *buf++ = 0;

  /* After the read marker and annex, qXfer looks like a
     traditional 'm' packet.  */
  decode_m_packet (buf, ofs, len);

  return 0;
}

/* Write the response to a successful qXfer read.  Returns the
   length of the (binary) data stored in BUF, corresponding
   to as much of DATA/LEN as we could fit.  IS_MORE controls
   the first character of the response.  */
static int
write_qxfer_response (char *buf, const void *data, int len, int is_more)
{
  int out_len;

  if (is_more)
    buf[0] = 'm';
  else
    buf[0] = 'l';

  return remote_escape_output (data, len, (unsigned char *) buf + 1, &out_len,
			       PBUFSIZ - 2) + 1;
}

/* Handle all of the extended 'Q' packets.  */
void
handle_general_set (char *own_buf)
{
  if (strncmp ("QPassSignals:", own_buf, strlen ("QPassSignals:")) == 0)
    {
      int numsigs = (int) TARGET_SIGNAL_LAST, i;
      const char *p = own_buf + strlen ("QPassSignals:");
      CORE_ADDR cursig;

      p = decode_address_to_semicolon (&cursig, p);
      for (i = 0; i < numsigs; i++)
	{
	  if (i == cursig)
	    {
	      pass_signals[i] = 1;
	      if (*p == '\0')
		/* Keep looping, to clear the remaining signals.  */
		cursig = -1;
	      else
		p = decode_address_to_semicolon (&cursig, p);
	    }
	  else
	    pass_signals[i] = 0;
	}
      strcpy (own_buf, "OK");
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
}

static const char *
get_features_xml (const char *annex)
{
  /* gdbserver_xmltarget defines what to return when looking
     for the "target.xml" file.  Its contents can either be
     verbatim XML code (prefixed with a '@') or else the name
     of the actual XML file to be used in place of "target.xml".

     This variable is set up from the auto-generated
     init_registers_... routine for the current target.  */

  if (gdbserver_xmltarget
      && strcmp (annex, "target.xml") == 0)
    {
      if (*gdbserver_xmltarget == '@')
	return gdbserver_xmltarget + 1;
      else
	annex = gdbserver_xmltarget;
    }

#ifdef USE_XML
  {
    extern const char *const xml_builtin[][2];
    int i;

    /* Look for the annex.  */
    for (i = 0; xml_builtin[i][0] != NULL; i++)
      if (strcmp (annex, xml_builtin[i][0]) == 0)
	break;

    if (xml_builtin[i][0] != NULL)
      return xml_builtin[i][1];
  }
#endif

  return NULL;
}

void
monitor_show_help (void)
{
  monitor_output ("The following monitor commands are supported:\n");
  monitor_output ("  set debug <0|1>\n");
  monitor_output ("    Enable general debugging messages\n");  
  monitor_output ("  set remote-debug <0|1>\n");
  monitor_output ("    Enable remote protocol debugging messages\n");
  monitor_output ("  exit\n");
  monitor_output ("    Quit GDBserver\n");
}

/* Subroutine of handle_search_memory to simplify it.  */

static int
handle_search_memory_1 (CORE_ADDR start_addr, CORE_ADDR search_space_len,
			gdb_byte *pattern, unsigned pattern_len,
			gdb_byte *search_buf,
			unsigned chunk_size, unsigned search_buf_size,
			CORE_ADDR *found_addrp)
{
  /* Prime the search buffer.  */

  if (read_inferior_memory (start_addr, search_buf, search_buf_size) != 0)
    {
      warning ("Unable to access target memory at 0x%lx, halting search.",
	       (long) start_addr);
      return -1;
    }

  /* Perform the search.

     The loop is kept simple by allocating [N + pattern-length - 1] bytes.
     When we've scanned N bytes we copy the trailing bytes to the start and
     read in another N bytes.  */

  while (search_space_len >= pattern_len)
    {
      gdb_byte *found_ptr;
      unsigned nr_search_bytes = (search_space_len < search_buf_size
				  ? search_space_len
				  : search_buf_size);

      found_ptr = memmem (search_buf, nr_search_bytes, pattern, pattern_len);

      if (found_ptr != NULL)
	{
	  CORE_ADDR found_addr = start_addr + (found_ptr - search_buf);
	  *found_addrp = found_addr;
	  return 1;
	}

      /* Not found in this chunk, skip to next chunk.  */

      /* Don't let search_space_len wrap here, it's unsigned.  */
      if (search_space_len >= chunk_size)
	search_space_len -= chunk_size;
      else
	search_space_len = 0;

      if (search_space_len >= pattern_len)
	{
	  unsigned keep_len = search_buf_size - chunk_size;
	  CORE_ADDR read_addr = start_addr + keep_len;
	  int nr_to_read;

	  /* Copy the trailing part of the previous iteration to the front
	     of the buffer for the next iteration.  */
	  memcpy (search_buf, search_buf + chunk_size, keep_len);

	  nr_to_read = (search_space_len - keep_len < chunk_size
			? search_space_len - keep_len
			: chunk_size);

	  if (read_inferior_memory (read_addr, search_buf + keep_len,
				    nr_to_read) != 0)
	    {
	      warning ("Unable to access target memory at 0x%lx, halting search.",
		       (long) read_addr);
	      return -1;
	    }

	  start_addr += chunk_size;
	}
    }

  /* Not found.  */

  return 0;
}

/* Handle qSearch:memory packets.  */

static void
handle_search_memory (char *own_buf, int packet_len)
{
  CORE_ADDR start_addr;
  CORE_ADDR search_space_len;
  gdb_byte *pattern;
  unsigned int pattern_len;
  /* NOTE: also defined in find.c testcase.  */
#define SEARCH_CHUNK_SIZE 16000
  const unsigned chunk_size = SEARCH_CHUNK_SIZE;
  /* Buffer to hold memory contents for searching.  */
  gdb_byte *search_buf;
  unsigned search_buf_size;
  int found;
  CORE_ADDR found_addr;
  int cmd_name_len = sizeof ("qSearch:memory:") - 1;

  pattern = malloc (packet_len);
  if (pattern == NULL)
    {
      error ("Unable to allocate memory to perform the search");
      strcpy (own_buf, "E00");
      return;
    }
  if (decode_search_memory_packet (own_buf + cmd_name_len,
				   packet_len - cmd_name_len,
				   &start_addr, &search_space_len,
				   pattern, &pattern_len) < 0)
    {
      free (pattern);
      error ("Error in parsing qSearch:memory packet");
      strcpy (own_buf, "E00");
      return;
    }

  search_buf_size = chunk_size + pattern_len - 1;

  /* No point in trying to allocate a buffer larger than the search space.  */
  if (search_space_len < search_buf_size)
    search_buf_size = search_space_len;

  search_buf = malloc (search_buf_size);
  if (search_buf == NULL)
    {
      free (pattern);
      error ("Unable to allocate memory to perform the search");
      strcpy (own_buf, "E00");
      return;
    }

  found = handle_search_memory_1 (start_addr, search_space_len,
				  pattern, pattern_len,
				  search_buf, chunk_size, search_buf_size,
				  &found_addr);

  if (found > 0)
    sprintf (own_buf, "1,%lx", (long) found_addr);
  else if (found == 0)
    strcpy (own_buf, "0");
  else
    strcpy (own_buf, "E00");

  free (search_buf);
  free (pattern);
}

#define require_running(BUF)			\
  if (!target_running ())			\
    {						\
      write_enn (BUF);				\
      return;					\
    }

/* Handle all of the extended 'q' packets.  */
void
handle_query (char *own_buf, int packet_len, int *new_packet_len_p)
{
  static struct inferior_list_entry *thread_ptr;

  /* Reply the current thread id.  */
  if (strcmp ("qC", own_buf) == 0 && !disable_packet_qC)
    {
      require_running (own_buf);
      thread_ptr = all_threads.head;
      sprintf (own_buf, "QC%x",
	       thread_to_gdb_id ((struct thread_info *)thread_ptr));
      return;
    }

  if (strcmp ("qSymbol::", own_buf) == 0)
    {
      if (target_running () && the_target->look_up_symbols != NULL)
	(*the_target->look_up_symbols) ();

      strcpy (own_buf, "OK");
      return;
    }

  if (!disable_packet_qfThreadInfo)
    {
      if (strcmp ("qfThreadInfo", own_buf) == 0)
	{
	  require_running (own_buf);
	  thread_ptr = all_threads.head;
	  sprintf (own_buf, "m%x", thread_to_gdb_id ((struct thread_info *)thread_ptr));
	  thread_ptr = thread_ptr->next;
	  return;
	}

      if (strcmp ("qsThreadInfo", own_buf) == 0)
	{
	  require_running (own_buf);
	  if (thread_ptr != NULL)
	    {
	      sprintf (own_buf, "m%x", thread_to_gdb_id ((struct thread_info *)thread_ptr));
	      thread_ptr = thread_ptr->next;
	      return;
	    }
	  else
	    {
	      sprintf (own_buf, "l");
	      return;
	    }
	}
    }

  if (the_target->read_offsets != NULL
      && strcmp ("qOffsets", own_buf) == 0)
    {
      CORE_ADDR text, data;

      require_running (own_buf);
      if (the_target->read_offsets (&text, &data))
	sprintf (own_buf, "Text=%lX;Data=%lX;Bss=%lX",
		 (long)text, (long)data, (long)data);
      else
	write_enn (own_buf);
      
      return;
    }

  if (the_target->qxfer_spu != NULL
      && strncmp ("qXfer:spu:read:", own_buf, 15) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *spu_buf;

      require_running (own_buf);
      strcpy (own_buf, "E00");
      if (decode_xfer_read (own_buf + 15, &annex, &ofs, &len) < 0)
	  return;
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      spu_buf = malloc (len + 1);
      if (!spu_buf)
        return;

      n = (*the_target->qxfer_spu) (annex, spu_buf, NULL, ofs, len + 1);
      if (n < 0) 
	write_enn (own_buf);
      else if (n > len)
	*new_packet_len_p = write_qxfer_response
			      (own_buf, spu_buf, len, 1);
      else 
	*new_packet_len_p = write_qxfer_response
			      (own_buf, spu_buf, n, 0);

      free (spu_buf);
      return;
    }

  if (the_target->qxfer_spu != NULL
      && strncmp ("qXfer:spu:write:", own_buf, 16) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *spu_buf;

      require_running (own_buf);
      strcpy (own_buf, "E00");
      spu_buf = malloc (packet_len - 15);
      if (!spu_buf)
        return;
      if (decode_xfer_write (own_buf + 16, packet_len - 16, &annex,
			     &ofs, &len, spu_buf) < 0)
	{
	  free (spu_buf);
	  return;
	}

      n = (*the_target->qxfer_spu) 
	(annex, NULL, (unsigned const char *)spu_buf, ofs, len);
      if (n < 0)
	write_enn (own_buf);
      else
	sprintf (own_buf, "%x", n);

      free (spu_buf);
      return;
    }

  if (the_target->read_auxv != NULL
      && strncmp ("qXfer:auxv:read:", own_buf, 16) == 0)
    {
      unsigned char *data;
      int n;
      CORE_ADDR ofs;
      unsigned int len;
      char *annex;

      require_running (own_buf);

      /* Reject any annex; grab the offset and length.  */
      if (decode_xfer_read (own_buf + 16, &annex, &ofs, &len) < 0
	  || annex[0] != '\0')
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Read one extra byte, as an indicator of whether there is
	 more.  */
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      data = malloc (len + 1);
      n = (*the_target->read_auxv) (ofs, data, len + 1);
      if (n < 0)
	write_enn (own_buf);
      else if (n > len)
	*new_packet_len_p = write_qxfer_response (own_buf, data, len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, data, n, 0);

      free (data);

      return;
    }

  if (strncmp ("qXfer:features:read:", own_buf, 20) == 0)
    {
      CORE_ADDR ofs;
      unsigned int len, total_len;
      const char *document;
      char *annex;

      require_running (own_buf);

      /* Grab the annex, offset, and length.  */
      if (decode_xfer_read (own_buf + 20, &annex, &ofs, &len) < 0)
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Now grab the correct annex.  */
      document = get_features_xml (annex);
      if (document == NULL)
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      total_len = strlen (document);
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;

      if (ofs > total_len)
	write_enn (own_buf);
      else if (len < total_len - ofs)
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  total_len - ofs, 0);

      return;
    }

  if (strncmp ("qXfer:libraries:read:", own_buf, 21) == 0)
    {
      CORE_ADDR ofs;
      unsigned int len, total_len;
      char *document, *p;
      struct inferior_list_entry *dll_ptr;
      char *annex;

      require_running (own_buf);

      /* Reject any annex; grab the offset and length.  */
      if (decode_xfer_read (own_buf + 21, &annex, &ofs, &len) < 0
	  || annex[0] != '\0')
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Over-estimate the necessary memory.  Assume that every character
	 in the library name must be escaped.  */
      total_len = 64;
      for (dll_ptr = all_dlls.head; dll_ptr != NULL; dll_ptr = dll_ptr->next)
	total_len += 128 + 6 * strlen (((struct dll_info *) dll_ptr)->name);

      document = malloc (total_len);
      strcpy (document, "<library-list>\n");
      p = document + strlen (document);

      for (dll_ptr = all_dlls.head; dll_ptr != NULL; dll_ptr = dll_ptr->next)
	{
	  struct dll_info *dll = (struct dll_info *) dll_ptr;
	  char *name;

	  strcpy (p, "  <library name=\"");
	  p = p + strlen (p);
	  name = xml_escape_text (dll->name);
	  strcpy (p, name);
	  free (name);
	  p = p + strlen (p);
	  strcpy (p, "\"><segment address=\"");
	  p = p + strlen (p);
	  sprintf (p, "0x%lx", (long) dll->base_addr);
	  p = p + strlen (p);
	  strcpy (p, "\"/></library>\n");
	  p = p + strlen (p);
	}

      strcpy (p, "</library-list>\n");

      total_len = strlen (document);
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;

      if (ofs > total_len)
	write_enn (own_buf);
      else if (len < total_len - ofs)
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  total_len - ofs, 0);

      free (document);
      return;
    }

  /* Protocol features query.  */
  if (strncmp ("qSupported", own_buf, 10) == 0
      && (own_buf[10] == ':' || own_buf[10] == '\0'))
    {
      sprintf (own_buf, "PacketSize=%x;QPassSignals+", PBUFSIZ - 1);

      /* We do not have any hook to indicate whether the target backend
	 supports qXfer:libraries:read, so always report it.  */
      strcat (own_buf, ";qXfer:libraries:read+");

      if (the_target->read_auxv != NULL)
	strcat (own_buf, ";qXfer:auxv:read+");

      if (the_target->qxfer_spu != NULL)
	strcat (own_buf, ";qXfer:spu:read+;qXfer:spu:write+");

      /* We always report qXfer:features:read, as targets may
	 install XML files on a subsequent call to arch_setup.
	 If we reported to GDB on startup that we don't support
	 qXfer:feature:read at all, we will never be re-queried.  */
      strcat (own_buf, ";qXfer:features:read+");

      return;
    }

  /* Thread-local storage support.  */
  if (the_target->get_tls_address != NULL
      && strncmp ("qGetTLSAddr:", own_buf, 12) == 0)
    {
      char *p = own_buf + 12;
      CORE_ADDR parts[3], address = 0;
      int i, err;

      require_running (own_buf);

      for (i = 0; i < 3; i++)
	{
	  char *p2;
	  int len;

	  if (p == NULL)
	    break;

	  p2 = strchr (p, ',');
	  if (p2)
	    {
	      len = p2 - p;
	      p2++;
	    }
	  else
	    {
	      len = strlen (p);
	      p2 = NULL;
	    }

	  decode_address (&parts[i], p, len);
	  p = p2;
	}

      if (p != NULL || i < 3)
	err = 1;
      else
	{
	  struct thread_info *thread = gdb_id_to_thread (parts[0]);

	  if (thread == NULL)
	    err = 2;
	  else
	    err = the_target->get_tls_address (thread, parts[1], parts[2],
					       &address);
	}

      if (err == 0)
	{
	  sprintf (own_buf, "%llx", address);
	  return;
	}
      else if (err > 0)
	{
	  write_enn (own_buf);
	  return;
	}

      /* Otherwise, pretend we do not understand this packet.  */
    }

  /* Handle "monitor" commands.  */
  if (strncmp ("qRcmd,", own_buf, 6) == 0)
    {
      char *mon = malloc (PBUFSIZ);
      int len = strlen (own_buf + 6);

      if ((len % 2) != 0 || unhexify (mon, own_buf + 6, len / 2) != len / 2)
	{
	  write_enn (own_buf);
	  free (mon);
	  return;
	}
      mon[len / 2] = '\0';

      write_ok (own_buf);

      if (strcmp (mon, "set debug 1") == 0)
	{
	  debug_threads = 1;
	  monitor_output ("Debug output enabled.\n");
	}
      else if (strcmp (mon, "set debug 0") == 0)
	{
	  debug_threads = 0;
	  monitor_output ("Debug output disabled.\n");
	}
      else if (strcmp (mon, "set remote-debug 1") == 0)
	{
	  remote_debug = 1;
	  monitor_output ("Protocol debug output enabled.\n");
	}
      else if (strcmp (mon, "set remote-debug 0") == 0)
	{
	  remote_debug = 0;
	  monitor_output ("Protocol debug output disabled.\n");
	}
      else if (strcmp (mon, "help") == 0)
	monitor_show_help ();
      else if (strcmp (mon, "exit") == 0)
	exit_requested = 1;
      else
	{
	  monitor_output ("Unknown monitor command.\n\n");
	  monitor_show_help ();
	  write_enn (own_buf);
	}

      free (mon);
      return;
    }

  if (strncmp ("qSearch:memory:", own_buf, sizeof ("qSearch:memory:") - 1) == 0)
    {
      require_running (own_buf);
      handle_search_memory (own_buf, packet_len);
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
}

/* Parse vCont packets.  */
void
handle_v_cont (char *own_buf, char *status, int *signal)
{
  char *p, *q;
  int n = 0, i = 0;
  struct thread_resume *resume_info, default_action;

  /* Count the number of semicolons in the packet.  There should be one
     for every action.  */
  p = &own_buf[5];
  while (p)
    {
      n++;
      p++;
      p = strchr (p, ';');
    }
  /* Allocate room for one extra action, for the default remain-stopped
     behavior; if no default action is in the list, we'll need the extra
     slot.  */
  resume_info = malloc ((n + 1) * sizeof (resume_info[0]));

  default_action.thread = -1;
  default_action.leave_stopped = 1;
  default_action.step = 0;
  default_action.sig = 0;

  p = &own_buf[5];
  i = 0;
  while (*p)
    {
      p++;

      resume_info[i].leave_stopped = 0;

      if (p[0] == 's' || p[0] == 'S')
	resume_info[i].step = 1;
      else if (p[0] == 'c' || p[0] == 'C')
	resume_info[i].step = 0;
      else
	goto err;

      if (p[0] == 'S' || p[0] == 'C')
	{
	  int sig;
	  sig = strtol (p + 1, &q, 16);
	  if (p == q)
	    goto err;
	  p = q;

	  if (!target_signal_to_host_p (sig))
	    goto err;
	  resume_info[i].sig = target_signal_to_host (sig);
	}
      else
	{
	  resume_info[i].sig = 0;
	  p = p + 1;
	}

      if (p[0] == 0)
	{
	  resume_info[i].thread = -1;
	  default_action = resume_info[i];

	  /* Note: we don't increment i here, we'll overwrite this entry
	     the next time through.  */
	}
      else if (p[0] == ':')
	{
	  unsigned int gdb_id = strtoul (p + 1, &q, 16);
	  unsigned long thread_id;

	  if (p == q)
	    goto err;
	  p = q;
	  if (p[0] != ';' && p[0] != 0)
	    goto err;

	  thread_id = gdb_id_to_thread_id (gdb_id);
	  if (thread_id)
	    resume_info[i].thread = thread_id;
	  else
	    goto err;

	  i++;
	}
    }

  resume_info[i] = default_action;

  /* Still used in occasional places in the backend.  */
  if (n == 1 && resume_info[0].thread != -1)
    cont_thread = resume_info[0].thread;
  else
    cont_thread = -1;
  set_desired_inferior (0);

  enable_async_io ();
  (*the_target->resume) (resume_info);

  free (resume_info);

  *signal = mywait (status, 1);
  prepare_resume_reply (own_buf, *status, *signal);
  disable_async_io ();
  return;

err:
  write_enn (own_buf);
  free (resume_info);
  return;
}

/* Attach to a new program.  Return 1 if successful, 0 if failure.  */
int
handle_v_attach (char *own_buf, char *status, int *signal)
{
  int pid;

  pid = strtol (own_buf + 8, NULL, 16);
  if (pid != 0 && attach_inferior (pid, status, signal) == 0)
    {
      prepare_resume_reply (own_buf, *status, *signal);
      return 1;
    }
  else
    {
      write_enn (own_buf);
      return 0;
    }
}

/* Run a new program.  Return 1 if successful, 0 if failure.  */
static int
handle_v_run (char *own_buf, char *status, int *signal)
{
  char *p, **pp, *next_p, **new_argv;
  int i, new_argc;

  new_argc = 0;
  for (p = own_buf + strlen ("vRun;"); p && *p; p = strchr (p, ';'))
    {
      p++;
      new_argc++;
    }

  new_argv = malloc ((new_argc + 2) * sizeof (char *));
  i = 0;
  for (p = own_buf + strlen ("vRun;"); *p; p = next_p)
    {
      next_p = strchr (p, ';');
      if (next_p == NULL)
	next_p = p + strlen (p);

      if (i == 0 && p == next_p)
	new_argv[i] = NULL;
      else
	{
	  new_argv[i] = malloc (1 + (next_p - p) / 2);
	  unhexify (new_argv[i], p, (next_p - p) / 2);
	  new_argv[i][(next_p - p) / 2] = '\0';
	}

      if (*next_p)
	next_p++;
      i++;
    }
  new_argv[i] = NULL;

  if (new_argv[0] == NULL)
    {
      if (program_argv == NULL)
	{
	  write_enn (own_buf);
	  return 0;
	}

      new_argv[0] = strdup (program_argv[0]);
    }

  /* Free the old argv.  */
  if (program_argv)
    {
      for (pp = program_argv; *pp != NULL; pp++)
	free (*pp);
      free (program_argv);
    }
  program_argv = new_argv;

  *signal = start_inferior (program_argv, status);
  if (*status == 'T')
    {
      prepare_resume_reply (own_buf, *status, *signal);
      return 1;
    }
  else
    {
      write_enn (own_buf);
      return 0;
    }
}

/* Handle all of the extended 'v' packets.  */
void
handle_v_requests (char *own_buf, char *status, int *signal,
		   int packet_len, int *new_packet_len)
{
  if (!disable_packet_vCont)
    {
      if (strncmp (own_buf, "vCont;", 6) == 0)
	{
	  require_running (own_buf);
	  handle_v_cont (own_buf, status, signal);
	  return;
	}

      if (strncmp (own_buf, "vCont?", 6) == 0)
	{
	  strcpy (own_buf, "vCont;c;C;s;S");
	  return;
	}
    }

  if (strncmp (own_buf, "vFile:", 6) == 0
      && handle_vFile (own_buf, packet_len, new_packet_len))
    return;

  if (strncmp (own_buf, "vAttach;", 8) == 0)
    {
      if (target_running ())
	{
	  fprintf (stderr, "Already debugging a process\n");
	  write_enn (own_buf);
	  return;
	}
      handle_v_attach (own_buf, status, signal);
      return;
    }

  if (strncmp (own_buf, "vRun;", 5) == 0)
    {
      if (target_running ())
	{
	  fprintf (stderr, "Already debugging a process\n");
	  write_enn (own_buf);
	  return;
	}
      handle_v_run (own_buf, status, signal);
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
  return;
}

void
myresume (char *own_buf, int step, int *signalp, char *statusp)
{
  struct thread_resume resume_info[2];
  int n = 0;
  int sig = *signalp;

  set_desired_inferior (0);

  if (step || sig || (cont_thread != 0 && cont_thread != -1))
    {
      resume_info[0].thread
	= ((struct inferior_list_entry *) current_inferior)->id;
      resume_info[0].step = step;
      resume_info[0].sig = sig;
      resume_info[0].leave_stopped = 0;
      n++;
    }
  resume_info[n].thread = -1;
  resume_info[n].step = 0;
  resume_info[n].sig = 0;
  resume_info[n].leave_stopped = (cont_thread != 0 && cont_thread != -1);

  enable_async_io ();
  (*the_target->resume) (resume_info);
  *signalp = mywait (statusp, 1);
  prepare_resume_reply (own_buf, *statusp, *signalp);
  disable_async_io ();
}

static void
gdbserver_version (void)
{
  printf ("GNU gdbserver %s%s\n"
	  "Copyright (C) 2007 Free Software Foundation, Inc.\n"
	  "gdbserver is free software, covered by the GNU General Public License.\n"
	  "This gdbserver was configured as \"%s\"\n",
	  PKGVERSION, version, host_name);
}

static void
gdbserver_usage (FILE *stream)
{
  fprintf (stream, "Usage:\tgdbserver [OPTIONS] COMM PROG [ARGS ...]\n"
	   "\tgdbserver [OPTIONS] --attach COMM PID\n"
	   "\tgdbserver [OPTIONS] --multi COMM\n"
	   "\n"
	   "COMM may either be a tty device (for serial debugging), or \n"
	   "HOST:PORT to listen for a TCP connection.\n"
	   "\n"
	   "Options:\n"
	   "  --debug\t\tEnable debugging output.\n"
	   "  --version\t\tDisplay version information and exit.\n"
	   "  --wrapper WRAPPER --\tRun WRAPPER to start new programs.\n");
  if (REPORT_BUGS_TO[0] && stream == stdout)
    fprintf (stream, "Report bugs to \"%s\".\n", REPORT_BUGS_TO);
}

static void
gdbserver_show_disableable (FILE *stream)
{
  fprintf (stream, "Disableable packets:\n"
	   "  vCont       \tAll vCont packets\n"
	   "  qC          \tQuerying the current thread\n"
	   "  qfThreadInfo\tThread listing\n"
	   "  Tthread     \tPassing the thread specifier in the T stop reply packet\n"
	   "  threads     \tAll of the above\n");
}


#undef require_running
#define require_running(BUF)			\
  if (!target_running ())			\
    {						\
      write_enn (BUF);				\
      break;					\
    }

int
main (int argc, char *argv[])
{
  char ch, status, *own_buf;
  unsigned char *mem_buf;
  int i = 0;
  int signal;
  unsigned int len;
  CORE_ADDR mem_addr;
  int bad_attach;
  int pid;
  char *arg_end, *port;
  char **next_arg = &argv[1];
  int multi_mode = 0;
  int attach = 0;
  int was_running;

  while (*next_arg != NULL && **next_arg == '-')
    {
      if (strcmp (*next_arg, "--version") == 0)
	{
	  gdbserver_version ();
	  exit (0);
	}
      else if (strcmp (*next_arg, "--help") == 0)
	{
	  gdbserver_usage (stdout);
	  exit (0);
	}
      else if (strcmp (*next_arg, "--attach") == 0)
	attach = 1;
      else if (strcmp (*next_arg, "--multi") == 0)
	multi_mode = 1;
      else if (strcmp (*next_arg, "--wrapper") == 0)
	{
	  next_arg++;

	  wrapper_argv = next_arg;
	  while (*next_arg != NULL && strcmp (*next_arg, "--") != 0)
	    next_arg++;

	  if (next_arg == wrapper_argv || *next_arg == NULL)
	    {
	      gdbserver_usage (stderr);
	      exit (1);
	    }

	  /* Consume the "--".  */
	  *next_arg = NULL;
	}
      else if (strcmp (*next_arg, "--debug") == 0)
	debug_threads = 1;
      else if (strcmp (*next_arg, "--disable-packet") == 0)
	{
	  gdbserver_show_disableable (stdout);
	  exit (0);
	}
      else if (strncmp (*next_arg,
			"--disable-packet=",
			sizeof ("--disable-packet=") - 1) == 0)
	{
	  char *packets, *tok;

	  packets = *next_arg += sizeof ("--disable-packet=") - 1;
	  for (tok = strtok (packets, ",");
	       tok != NULL;
	       tok = strtok (NULL, ","))
	    {
	      if (strcmp ("vCont", tok) == 0)
		disable_packet_vCont = 1;
	      else if (strcmp ("Tthread", tok) == 0)
		disable_packet_Tthread = 1;
	      else if (strcmp ("qC", tok) == 0)
		disable_packet_qC = 1;
	      else if (strcmp ("qfThreadInfo", tok) == 0)
		disable_packet_qfThreadInfo = 1;
	      else if (strcmp ("threads", tok) == 0)
		{
		  disable_packet_vCont = 1;
		  disable_packet_Tthread = 1;
		  disable_packet_qC = 1;
		  disable_packet_qfThreadInfo = 1;
		}
	      else
		{
		  fprintf (stderr, "Don't know how to disable \"%s\".\n\n",
			   tok);
		  gdbserver_show_disableable (stderr);
		  exit (1);
		}
	    }
	}
      else
	{
	  fprintf (stderr, "Unknown argument: %s\n", *next_arg);
	  exit (1);
	}

      next_arg++;
      continue;
    }

  if (setjmp (toplevel))
    {
      fprintf (stderr, "Exiting\n");
      exit (1);
    }

  port = *next_arg;
  next_arg++;
  if (port == NULL || (!attach && !multi_mode && *next_arg == NULL))
    {
      gdbserver_usage (stderr);
      exit (1);
    }

  bad_attach = 0;
  pid = 0;

  /* --attach used to come after PORT, so allow it there for
       compatibility.  */
  if (*next_arg != NULL && strcmp (*next_arg, "--attach") == 0)
    {
      attach = 1;
      next_arg++;
    }

  if (attach
      && (*next_arg == NULL
	  || (*next_arg)[0] == '\0'
	  || (pid = strtoul (*next_arg, &arg_end, 0)) == 0
	  || *arg_end != '\0'
	  || next_arg[1] != NULL))
    bad_attach = 1;

  if (bad_attach)
    {
      gdbserver_usage (stderr);
      exit (1);
    }

  initialize_async_io ();
  initialize_low ();

  own_buf = malloc (PBUFSIZ + 1);
  mem_buf = malloc (PBUFSIZ);

  if (pid == 0 && *next_arg != NULL)
    {
      int i, n;

      n = argc - (next_arg - argv);
      program_argv = malloc (sizeof (char *) * (n + 1));
      for (i = 0; i < n; i++)
	program_argv[i] = strdup (next_arg[i]);
      program_argv[i] = NULL;

      /* Wait till we are at first instruction in program.  */
      signal = start_inferior (program_argv, &status);

      /* We are now (hopefully) stopped at the first instruction of
	 the target process.  This assumes that the target process was
	 successfully created.  */
    }
  else if (pid != 0)
    {
      if (attach_inferior (pid, &status, &signal) == -1)
	error ("Attaching not supported on this target");

      /* Otherwise succeeded.  */
    }
  else
    {
      status = 'W';
      signal = 0;
    }

  /* Don't report shared library events on the initial connection,
     even if some libraries are preloaded.  Avoids the "stopped by
     shared library event" notice on gdb side.  */
  dlls_changed = 0;

  if (setjmp (toplevel))
    {
      fprintf (stderr, "Killing inferior\n");
      kill_inferior ();
      exit (1);
    }

  if (status == 'W' || status == 'X')
    was_running = 0;
  else
    was_running = 1;

  if (!was_running && !multi_mode)
    {
      fprintf (stderr, "No program to debug.  GDBserver exiting.\n");
      exit (1);
    }

  while (1)
    {
      remote_open (port);

    restart:
      if (setjmp (toplevel) != 0)
	{
	  /* An error occurred.  */
	  if (response_needed)
	    {
	      write_enn (own_buf);
	      putpkt (own_buf);
	    }
	}

      disable_async_io ();
      while (!exit_requested)
	{
	  unsigned char sig;
	  int packet_len;
	  int new_packet_len = -1;

	  response_needed = 0;
	  packet_len = getpkt (own_buf);
	  if (packet_len <= 0)
	    break;
	  response_needed = 1;

	  i = 0;
	  ch = own_buf[i++];
	  switch (ch)
	    {
	    case 'q':
	      handle_query (own_buf, packet_len, &new_packet_len);
	      break;
	    case 'Q':
	      handle_general_set (own_buf);
	      break;
	    case 'D':
	      require_running (own_buf);
	      fprintf (stderr, "Detaching from inferior\n");
	      if (detach_inferior () != 0)
		write_enn (own_buf);
	      else
		{
		  write_ok (own_buf);

		  if (extended_protocol)
		    {
		      /* Treat this like a normal program exit.  */
		      signal = 0;
		      status = 'W';
		    }
		  else
		    {
		      putpkt (own_buf);
		      remote_close ();

		      /* If we are attached, then we can exit.  Otherwise, we
			 need to hang around doing nothing, until the child
			 is gone.  */
		      if (!attached)
			join_inferior ();

		      exit (0);
		    }
		}
	      break;
	    case '!':
	      extended_protocol = 1;
	      write_ok (own_buf);
	      break;
	    case '?':
	      prepare_resume_reply (own_buf, status, signal);
	      break;
	    case 'H':
	      if (own_buf[1] == 'c' || own_buf[1] == 'g' || own_buf[1] == 's')
		{
		  unsigned long gdb_id, thread_id;

		  require_running (own_buf);
		  gdb_id = strtoul (&own_buf[2], NULL, 16);
		  if (gdb_id == 0 || gdb_id == -1)
		    thread_id = gdb_id;
		  else
		    {
		      thread_id = gdb_id_to_thread_id (gdb_id);
		      if (thread_id == 0)
			{
			  write_enn (own_buf);
			  break;
			}
		    }

		  if (own_buf[1] == 'g')
		    {
		      general_thread = thread_id;
		      set_desired_inferior (1);
		    }
		  else if (own_buf[1] == 'c')
		    cont_thread = thread_id;
		  else if (own_buf[1] == 's')
		    step_thread = thread_id;

		  write_ok (own_buf);
		}
	      else
		{
		  /* Silently ignore it so that gdb can extend the protocol
		     without compatibility headaches.  */
		  own_buf[0] = '\0';
		}
	      break;
	    case 'g':
	      require_running (own_buf);
	      set_desired_inferior (1);
	      registers_to_string (own_buf);
	      break;
	    case 'G':
	      require_running (own_buf);
	      set_desired_inferior (1);
	      registers_from_string (&own_buf[1]);
	      write_ok (own_buf);
	      break;
	    case 'm':
	      require_running (own_buf);
	      decode_m_packet (&own_buf[1], &mem_addr, &len);
	      if (read_inferior_memory (mem_addr, mem_buf, len) == 0)
		convert_int_to_ascii (mem_buf, own_buf, len);
	      else
		write_enn (own_buf);
	      break;
	    case 'M':
	      require_running (own_buf);
	      decode_M_packet (&own_buf[1], &mem_addr, &len, mem_buf);
	      if (write_inferior_memory (mem_addr, mem_buf, len) == 0)
		write_ok (own_buf);
	      else
		write_enn (own_buf);
	      break;
	    case 'X':
	      require_running (own_buf);
	      if (decode_X_packet (&own_buf[1], packet_len - 1,
				   &mem_addr, &len, mem_buf) < 0
		  || write_inferior_memory (mem_addr, mem_buf, len) != 0)
		write_enn (own_buf);
	      else
		write_ok (own_buf);
	      break;
	    case 'C':
	      require_running (own_buf);
	      convert_ascii_to_int (own_buf + 1, &sig, 1);
	      if (target_signal_to_host_p (sig))
		signal = target_signal_to_host (sig);
	      else
		signal = 0;
	      myresume (own_buf, 0, &signal, &status);
	      break;
	    case 'S':
	      require_running (own_buf);
	      convert_ascii_to_int (own_buf + 1, &sig, 1);
	      if (target_signal_to_host_p (sig))
		signal = target_signal_to_host (sig);
	      else
		signal = 0;
	      myresume (own_buf, 1, &signal, &status);
	      break;
	    case 'c':
	      require_running (own_buf);
	      signal = 0;
	      myresume (own_buf, 0, &signal, &status);
	      break;
	    case 's':
	      require_running (own_buf);
	      signal = 0;
	      myresume (own_buf, 1, &signal, &status);
	      break;
	    case 'Z':
	      {
		char *lenptr;
		char *dataptr;
		CORE_ADDR addr = strtoul (&own_buf[3], &lenptr, 16);
		int len = strtol (lenptr + 1, &dataptr, 16);
		char type = own_buf[1];

		if (the_target->insert_watchpoint == NULL
		    || (type < '2' || type > '4'))
		  {
		    /* No watchpoint support or not a watchpoint command;
		       unrecognized either way.  */
		    own_buf[0] = '\0';
		  }
		else
		  {
		    int res;

		    require_running (own_buf);
		    res = (*the_target->insert_watchpoint) (type, addr, len);
		    if (res == 0)
		      write_ok (own_buf);
		    else if (res == 1)
		      /* Unsupported.  */
		      own_buf[0] = '\0';
		    else
		      write_enn (own_buf);
		  }
		break;
	      }
	    case 'z':
	      {
		char *lenptr;
		char *dataptr;
		CORE_ADDR addr = strtoul (&own_buf[3], &lenptr, 16);
		int len = strtol (lenptr + 1, &dataptr, 16);
		char type = own_buf[1];

		if (the_target->remove_watchpoint == NULL
		    || (type < '2' || type > '4'))
		  {
		    /* No watchpoint support or not a watchpoint command;
		       unrecognized either way.  */
		    own_buf[0] = '\0';
		  }
		else
		  {
		    int res;

		    require_running (own_buf);
		    res = (*the_target->remove_watchpoint) (type, addr, len);
		    if (res == 0)
		      write_ok (own_buf);
		    else if (res == 1)
		      /* Unsupported.  */
		      own_buf[0] = '\0';
		    else
		      write_enn (own_buf);
		  }
		break;
	      }
	    case 'k':
	      response_needed = 0;
	      if (!target_running ())
		/* The packet we received doesn't make sense - but we
		   can't reply to it, either.  */
		goto restart;

	      fprintf (stderr, "Killing inferior\n");
	      kill_inferior ();

	      /* When using the extended protocol, we wait with no
		 program running.  The traditional protocol will exit
		 instead.  */
	      if (extended_protocol)
		{
		  status = 'X';
		  signal = TARGET_SIGNAL_KILL;
		  was_running = 0;
		  goto restart;
		}
	      else
		{
		  exit (0);
		  break;
		}
	    case 'T':
	      {
		unsigned long gdb_id, thread_id;

		require_running (own_buf);
		gdb_id = strtoul (&own_buf[1], NULL, 16);
		thread_id = gdb_id_to_thread_id (gdb_id);
		if (thread_id == 0)
		  {
		    write_enn (own_buf);
		    break;
		  }

		if (mythread_alive (thread_id))
		  write_ok (own_buf);
		else
		  write_enn (own_buf);
	      }
	      break;
	    case 'R':
	      response_needed = 0;

	      /* Restarting the inferior is only supported in the
	         extended protocol.  */
	      if (extended_protocol)
		{
		  if (target_running ())
		    kill_inferior ();
		  fprintf (stderr, "GDBserver restarting\n");

		  /* Wait till we are at 1st instruction in prog.  */
		  if (program_argv != NULL)
		    signal = start_inferior (program_argv, &status);
		  else
		    {
		      status = 'X';
		      signal = TARGET_SIGNAL_KILL;
		    }
		  goto restart;
		}
	      else
		{
		  /* It is a request we don't understand.  Respond with an
		     empty packet so that gdb knows that we don't support this
		     request.  */
		  own_buf[0] = '\0';
		  break;
		}
	    case 'v':
	      /* Extended (long) request.  */
	      handle_v_requests (own_buf, &status, &signal,
				 packet_len, &new_packet_len);
	      break;

	    default:
	      /* It is a request we don't understand.  Respond with an
	         empty packet so that gdb knows that we don't support this
	         request.  */
	      own_buf[0] = '\0';
	      break;
	    }

	  if (new_packet_len != -1)
	    putpkt_binary (own_buf, new_packet_len);
	  else
	    putpkt (own_buf);

	  response_needed = 0;

	  if (was_running && (status == 'W' || status == 'X'))
	    {
	      was_running = 0;

	      if (status == 'W')
		fprintf (stderr,
			 "\nChild exited with status %d\n", signal);
	      if (status == 'X')
		fprintf (stderr, "\nChild terminated with signal = 0x%x (%s)\n",
			 target_signal_to_host (signal),
			 target_signal_to_name (signal));

	      if (extended_protocol)
		goto restart;
	      else
		{
		  fprintf (stderr, "GDBserver exiting\n");
		  exit (0);
		}
	    }

	  if (status != 'W' && status != 'X')
	    was_running = 1;
	}

      /* If an exit was requested (using the "monitor exit" command),
	 terminate now.  The only other way to get here is for
	 getpkt to fail; close the connection and reopen it at the
	 top of the loop.  */

      if (exit_requested)
	{
	  remote_close ();
	  if (attached && target_running ())
	    detach_inferior ();
	  else if (target_running ())
	    kill_inferior ();
	  exit (0);
	}
      else
	{
	  fprintf (stderr, "Remote side has terminated connection.  "
			   "GDBserver will reopen the connection.\n");
	  remote_close ();
	}
    }
}
