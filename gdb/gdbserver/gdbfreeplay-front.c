/*
 * gdb-freeplay
 *
 * Freely replay a remote debug session logfile for GDB.
 *
 * Parts of gdb-freeplay are adapted from gdbreplay, 
 * by Stu Grossman and Fred Fish.
 */

#include "config.h"
#include <stdio.h>
#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#include <ctype.h>
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#if HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if USE_WIN32API
#include <winsock.h>
#endif

#ifndef HAVE_SOCKLEN_T
typedef int socklen_t;
#endif

/* Version information, from version.c.  */
extern const char version[];
extern const char host_name[];

/* Print the system error message for errno, and also mention STRING
   as the file name for which the error was encountered.
   Then return to command level.  */

static void
perror_with_name (char *string)
{
#ifndef STDC_HEADERS
  extern int errno;
#endif
  const char *err;
  char *combined;

  err = strerror (errno);
  if (err == NULL)
    err = "unknown error";

  combined = (char *) alloca (strlen (err) + strlen (string) + 3);
  strcpy (combined, string);
  strcat (combined, ": ");
  strcat (combined, err);
  fprintf (stderr, "\n%s.\n", combined);
  fflush (stderr);
  exit (1);
}

/*
 * remote_close
 *
 * Close the gdb socket.
 */

static void
remote_close (int remote_desc)
{
#ifdef USE_WIN32API
  closesocket (remote_desc);
#else
  close (remote_desc);
#endif
}

/*
 * remote_open
 *
 * Open a connection to a remote debugger (gdb).
 * NAME is the filename used for communication.  
 *
 * Returns: file descriptor. 
 * Does not return in case of error.
 */

static int
remote_open (char *name)
{
  int remote_desc;

  if (!strchr (name, ':'))
    {
      fprintf (stderr, "%s: Must specify tcp connection as host:addr\n", name);
      fflush (stderr);
      exit (1);
    }
  else
    {
#ifdef USE_WIN32API
      static int winsock_initialized;
#endif
      char *port_str;
      int port;
      struct sockaddr_in sockaddr;
      socklen_t tmp;
      int tmp_desc;

      port_str = strchr (name, ':');

      port = atoi (port_str + 1);

#ifdef USE_WIN32API
      if (!winsock_initialized)
	{
	  WSADATA wsad;

	  WSAStartup (MAKEWORD (1, 0), &wsad);
	  winsock_initialized = 1;
	}
#endif

      tmp_desc = socket (PF_INET, SOCK_STREAM, 0);
      if (tmp_desc < 0)
	perror_with_name ("Can't open socket");

      /* Allow rapid reuse of this port. */
      tmp = 1;
      setsockopt (tmp_desc, SOL_SOCKET, SO_REUSEADDR, (char *) &tmp,
		  sizeof (tmp));

      sockaddr.sin_family = PF_INET;
      sockaddr.sin_port = htons (port);
      sockaddr.sin_addr.s_addr = INADDR_ANY;

      if (bind (tmp_desc, (struct sockaddr *) &sockaddr, sizeof (sockaddr))
	  || listen (tmp_desc, 1))
	perror_with_name ("Can't bind address");

      tmp = sizeof (sockaddr);
      remote_desc = accept (tmp_desc, (struct sockaddr *) &sockaddr, &tmp);
      if (remote_desc == -1)
	perror_with_name ("Accept failed");

      /* Enable TCP keep alive process. */
      tmp = 1;
      setsockopt (tmp_desc, SOL_SOCKET, SO_KEEPALIVE, (char *) &tmp, sizeof (tmp));

      /* Tell TCP not to delay small packets.  This greatly speeds up
         interactive response. */
      tmp = 1;
      setsockopt (remote_desc, IPPROTO_TCP, TCP_NODELAY,
		  (char *) &tmp, sizeof (tmp));

#ifndef USE_WIN32API
      close (tmp_desc);		/* No longer need this */

      signal (SIGPIPE, SIG_IGN);	/* If we don't do this, then gdbreplay simply
					   exits when the remote side dies.  */
#else
      closesocket (tmp_desc);	/* No longer need this */
#endif
    }

#if defined(F_SETFL) && defined (FASYNC)
  fcntl (remote_desc, F_SETFL, FASYNC);
#endif

  fprintf (stderr, "Replay logfile using %s\n", name);
  fflush (stderr);

  return remote_desc;
}

/*
 * freeplay_version
 *
 * Print version information.
 */

static void
freeplay_version (FILE *stream)
{
  fprintf (stream, "GNU gdb-freeplay %s%s\n"
	   "Copyright (C) 2008 Free Software Foundation, Inc.\n"
	   "gdb-freeplay is free software, covered by the GNU General Public License.\n"
	   "\n"
	   "This gdb-freeplay was configured as \"%s\"\n",
	   PKGVERSION, version, host_name);
}

/*
 * freeplay_usage
 *
 * Print usage information.
 */

static void
freeplay_usage (FILE *stream)
{
  fprintf (stream, "Usage:\tgdb-freeplay [--verbose] [--version] <logfile> <host:port>\n");
  if (REPORT_BUGS_TO[0] && stream == stdout)
    fprintf (stream, "Report bugs to \"%s\".\n", REPORT_BUGS_TO);
}

#include "gdbfreeplay.h"

/*
 * TODO:
 *
 * Actually accept packets from gdb, with handshake where appropriate.
 * Parse tfind requests and handle them in the frame cache.
 * Try to match everything else from the frame cache and send cached reply.
 */

int verbose;

extern int
main (int argc, char **argv)
{
  char *default_socket = "localhost:2345";
  int logfile_arg = 1;
  int socket_arg = 2;
  int remote_desc = -1, i;

  for (i = 0; i < argc; i++)
    {
      if (strcmp (argv[i], "--version") == 0)
	{
	  freeplay_version (stdout);
	  logfile_arg++;
	  socket_arg++;
	}
      if (strcmp (argv[i], "--verbose") == 0)
	{
	  verbose = 1;
	  logfile_arg++;
	  socket_arg++;
	}
      else if (strcmp (argv[i], "--help") == 0)
	{
	  freeplay_usage (stdout);
	  exit (0);
	}
      else if (i == logfile_arg)
	{
	  if (gdbfreeplay_open (argv[i]) != PASS)
	    perror_with_name (argv[1]);
	  logfile_arg = 0;
	}
      else if (i == socket_arg)
	{
	  if (verbose)
	    fprintf (stdout, "Open socket: %s\n", argv[i]);
	  remote_desc = remote_open (argv[i]);
	  socket_arg = 0;
	}
    }

  if (logfile_arg)
    {
      freeplay_usage (stderr);
      exit (1);
    }

  if (socket_arg)
    {
      if (verbose)
	fprintf (stdout, "Open default socket: %s\n", default_socket);
      remote_desc = remote_open (default_socket);
    }

  if (remote_desc > 0)
    {
      gdbfreeplay (remote_desc);

      if (verbose)
	fprintf (stdout, "Close remote socket.\n");
      remote_close (remote_desc);
    }

  exit (0);
}

