/* Cadillac interface routines.
   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <connection.h>
#include <genericreq.h>
#include <debuggerreq.h>
#include <debuggerconn.h>
#include <ttyconn.h>

/* Connection block for debugger<=>kernel communications. */
static Connection *conn = 0;

/* fd for our socket to the kernel. */
static int kerfd;

/* The kernel's ID for this instance of the program. */
static int program_id;


/* This routine redirects the output of fputs_filtered to the kernel so that
   the user can see what's going on in his debugger window. */

void
cadillac_fputs(ptr)
     char *ptr;
{
  CTtyRequest *req;

  if (conn)
    {
      req = CWriteTtyRequest (conn, TextIORType);
      CWriteVstring0 (conn, ptr);
      CWriteLength (conn);
/*      CWriteRequestBuffer (conn);	/* XXX - debugging only */
    }
  else
    fputs (ptr, stdout);
}

/* This wrapper routine is needed because execute_command modifies the command
   string that it's given.  It also echos the commands. */

static void
execute_command_1 (cmd, from_tty)
     char *cmd;
     int from_tty;
{
  char buf[100];

  printf_filtered("%s\n", cmd);
  strcpy(buf, cmd);
  execute_command(buf, from_tty);
}

/* Establish contact with the kernel. */

void
cadillac_initialize(cadillac_id, execarg)
     char *cadillac_id;
     char *execarg;
{
  CTtyRequest *req;
  char *ctmp;
  extern long strtol(char *str, char **ptr, int base);
  char pathname[MAXPATHLEN];

  if (!execarg) execarg = "";

  printf("gdb-debugger pid=%d\n", getpid()); /* XXX - debugging only */
  
  /* First establish the connection with the kernel. */

  kerfd = COpenClientSocket(NULL);
  if (kerfd < 0) {
    printf("COpenClientSocket() failed\n");
    exit(1);
  }

  /* Setup connection buffering. */

  CSetSocketBufferSize (kerfd, 12000);

  /* Generate a new connection control block. */

  conn = NewConnection (0, kerfd, kerfd);
  if (!conn) {
    printf("NewConnection() failed\n");
    exit(1);
  }

  /* Tell the kernel that we are the "debugger". */

  req = CWriteTtyRequest (conn, QueryConnectionRType);
  req->generic.queryconnection.major = 0;
  req->generic.queryconnection.minor = 0;
  req->generic.queryconnection.cadillacId1=strtol(cadillac_id, &ctmp, 16);
  req->generic.queryconnection.cadillacId2 = strtol(++ctmp, NULL, 16);
  req->generic.queryconnection.nProtocols = 1;
  CWriteProtocol (conn, 0, 0, "debugger");
  CWriteLength (conn);

  /* Tell the kernel that we are actually running. */

  /* KROCK ALERT!!!  The kernel doesn't really care about the arguments to
     the program at all!  It only cares that argument 7 be the name of the
     target program.  So, we just fill in the rest of the slots with
     padding.  I hope the kernel never cares about this! */

  req = CWriteTtyRequest (conn, RunningProgramRType);
  req->runningprogram.argc = 8;
  getwd (pathname);
  CWriteVstring0 (conn, pathname);

  CWriteVstring0 (conn, "0");
  CWriteVstring0 (conn, "1");
  CWriteVstring0 (conn, "2");
  CWriteVstring0 (conn, "3");
  CWriteVstring0 (conn, "4");
  CWriteVstring0 (conn, "5");
  CWriteVstring0 (conn, "6");
  CWriteVstring0 (conn, execarg);
  CWriteLength (conn);
}

/* All requests from the Cadillac kernel eventually end up here. */

void
cadillac_main_loop(pprompt)
     char **pprompt;
{
  CTtyRequest *req;
  extern int cadillac;

  /* The actual event loop! */

  while (1)
    {
      register CHeader *head;
      fd_set readfds;
      int numfds;

      fputs_filtered(*pprompt, stdout); /* Output the prompt */

      /* Output all pending requests. */
      CWriteRequestBuffer (conn);

      /* Wait till there's some activity from the kernel. */
      while (1)
	{
	  FD_ZERO (&readfds);
	  FD_SET (kerfd, &readfds);
	  numfds = select (sizeof(readfds)*8, &readfds, 0, 0, 0);
	  if (numfds > 0) break;
	}

      head = (CHeader *)CPeekNextRequest (conn);
      if (head == NULL)
	{
	  fprintf (stderr, "EOF on kernel read!\n");
	  exit (1);
	}

      if (head->reqType < LastTtyRequestRType)
	{
	  CTtyRequest* req = CReadTtyRequest (conn);
	  switch (req->head.reqType)
	    {
	    case AcceptConnectionRType:
	      CSkipRequest (conn);
	      /* Tell the rest of the world that cadillac is now set up */
	      cadillac = 1;
	      break;

	    case RefuseConnectionRType:
	      fprintf (stderr, "Debugger connection refused\n");
	      exit (1);

	    case KillProgramRType:
	      exit (0);

	    default:
	      fprintf_filtered(stderr, "Unknown request type = %d\n",
		      req->head.reqType);
	      CSkipRequest (conn);
	      break;
	    }
	}
      else
	{
	  CVDebuggerRequest *req = CVReadDebuggerRequest (conn);
	  if (!req)
	    {
	      fprintf (stderr, "CVReadDebuggerRequest returned NULL, type = %d\n",
		       head->reqType);
	      return;
	    }

	  switch (req->head.request->reqType)
	    {
	    case OpenProgramInstanceRType:
	      {
		char *arglist, buf[100]; /* XXX - Make buf dynamic! */
		int arglen;
		/* XXX - should notice when program_id changes */
		program_id = req->openProgramInstance.request->programId;
		arglist = req->openProgramInstance.progArglist.text;
		arglen = req->openProgramInstance.progArglist.byteLen;

		execute_command_1("break main", 1);
		if (arglist)
		  {
		    sprintf(buf, "set args %.*s", arglen, arglist);
		    fputs_filtered(*pprompt, stdout); /* Output the prompt */
		    execute_command_1(buf, 1);
		  }
		fputs_filtered(*pprompt, stdout); /* Output the prompt */
		execute_command_1("run", 1);
	      }
	      break;
	    case QuitDebuggerRType:
	      exit (0);
	    case RunRType:
	      execute_command_1("run", 1);
	      break;
	    case ContinueRType:
	      execute_command_1("continue", 1);
	      break;
	    case StepRType:
	      execute_command_1("step", 1);
	      break;
	    case NextRType:
	      execute_command_1("next", 1);
	      break;
	    case ChangeStackFrameRType:
	      printf_filtered("Got to ChangeStackFrame\n");
	      break;
	    case BackTraceRType:
	      execute_command_1("backtrace", 1);
	      break;
	    case FinishRType:
	      execute_command_1("finish", 1);
	      break;
	    case TerminateProgramRType:
	      execute_command_1("quit", 1);
	      break;
	    case UserInputRType:
	      {
		char *text;
		long len;

		text = req->userInput.userInput.text;
		len = req->userInput.userInput.byteLen;

		if (text[len-1] == '\n') text[len-1] = '\000';
		execute_command (text, 1);
		break;
	      }
	    default:
	      fprintf_filtered(stderr, "Unknown request type = %d\n",
		      req->head.request->reqType);
	      break;
	    }
	  CVFreeDebuggerRequest (req);
	}
    }
}
