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

#include "defs.h"
#include "symtab.h"
#include "inferior.h"
#include "command.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <connection.h>
#include <genericreq.h>
#include <debuggerreq.h>
#include <debuggerconn.h>
#include <ttyconn.h>
#include <varargs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/filio.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/errno.h>
#include <termios.h>
#include <strings.h>

/* Connection block for debugger<=>kernel communications. */
static Connection *conn = 0;

/* fd for our socket to the kernel. */
static int kerfd;

/* The kernel's ID for this instance of the program. */
static int program_id;

static int instance_id;

/* The fd for the pty associated with the inferior. */
static int inferior_pty = -1;
static int inferior_tty = -1;

static int has_run = 0;

extern int cadillac;

char **pprompt;			/* Pointer to pointer to prompt */

static void
prompt()
{
  fputs_filtered(*pprompt, stdout);
}

/* This routine redirects the output of fputs_filtered to the kernel so that
   the user can see what's going on in his debugger window. */

void
cadillac_fputs(ptr)
     char *ptr;
{
  if (conn)
    CVWriteTranscriptInfo (conn, instance_id, ptr);
  else
    fputs (ptr, stdout);
}

/* Copy all data from the pty to the kernel. */

static void
pty_to_kernel()
{
  CTtyRequest *req;
  char buf[1024];
  int cc;

  while (1)
    {
      cc = read(inferior_pty, buf, sizeof(buf));

      if (cc == 0
	  || (cc < 0
	      && errno == EWOULDBLOCK))
	break;

      if (cc < 0)
	{
	  close(inferior_pty);
	  inferior_pty = -1;
	  perror("pty read error");
	  break;
	}

      req = CWriteTtyRequest(conn, TextIORType);
      CWriteVstringLen(conn, buf, cc);
      CWriteLength(conn);
    }
  CWriteRequestBuffer(conn);
}

/* Copy data from the kernel to the pty. */

static void
kernel_to_pty(data, len)
     char *data;
     int len;
{
  int cc;

  cc = write(inferior_pty, data, len);

  if (cc != len)
    {
      if (cc < 0)
	{
	  close(inferior_pty);
	  inferior_pty = -1;
	  perror("pty write error");
	  return;
	}
      printf("Couldn't write all the data to the pty, wanted %d, got %d\n",
	     len, cc);
    }
}

/* Tell the kernel where we are in the program, and what the stack looks like.
   */

static void
send_status()
{
  static int linecount = 48;
  struct symtab_and_line sal;
  struct symbol *symbol;
  char *funcname, *filename;
  static int foo = 0;
  static int sent_prog_inst = 0;

  if (!has_run)
    return;

  if (inferior_pid == 0)	/* target has died */
    {
      CVWriteProgramTerminatedInfo(conn,
				   instance_id,
				   ""
				   );
      return;
    }

  sal = find_pc_line(stop_pc, 0);
  symbol = find_pc_function(stop_pc);

  funcname = symbol ? symbol->name : "";
  filename = sal.symtab ? sal.symtab->filename : "";

  if (!sent_prog_inst)
    {
      sent_prog_inst = 1;
      CVWriteProgramInstanceInfo(conn,
				 program_id,
				 instance_id,
				 "", /* hostname */
				 "", /* arglist */
				 ""
				 );
    }

  CVWriteStackSizeInfo(conn,
		       instance_id,
		       1, /* XXX - frame depth */
		       CInnerFrameIs0,
		       foo++ ? 1 : 0, /* XXX - frame diff */
		       ""
		       );

  CVWriteStackFrameInfo(conn,
			instance_id,
			sal.line,
			CFileLinePos,
			0,	/* XXX - frame # */
			funcname,
			filename,
			""	/* XXX ? transcript */
			);

  CVWriteProgramStoppedInfo(conn,
			    instance_id,
			    0,	/* XXX - breakpoint # or signal # */
			    CDebuggerCommand,
			    funcname,
			    ""	/* XXX ? transcript */
			    );
}

/* Tell the kernel that the target is now running. */

static void
go_busy()
{
  CVWriteProgramBusyInfo(conn,
			 instance_id,
			 "");	/* XXX ? transcript */
  CWriteRequestBuffer(conn);	/* Must take place synchronusly! */
}

/* This wrapper routine is needed because execute_command modifies the command
   string that it's given.  It also echos the commands. */

static void
execute_command_1(va_alist)
     va_dcl
{
  char buf[100];		/* XXX - make buf dynamic! */
  char *cmd;
  va_list args;

  va_start(args);

  cmd = va_arg(args, char *);

  vsprintf(buf, cmd, args);

  printf_filtered("%s\n", buf);
  execute_command(buf, 1);

  va_end(args);
}

#ifdef KERNEL_RECORD
FILE *kerout;

static int
kernel_record(fd, ptr, num)
     int fd, num;
     char *ptr;

{
  fwrite(ptr, num, 1, kerout);
  fflush(kerout);
  return write(fd, ptr, num);
}
#endif

void
cadillac_condition_breakpoint(b)
     struct breakpoint *b;
{
  CVWriteBreakConditionInfo(conn,
			    instance_id,
			    b->number,
			    b->cond_string ? b->cond_string : "",
			    ""	/* transcript */
			    );
}

void
cadillac_commands_breakpoint(b)
     struct breakpoint *b;
{
  struct command_line *l;

  CVWriteBreakCommandBegInfo(conn,
			     instance_id,
			     b->number,
			     ""); /* transcript */

  for (l = b->commands; l; l = l->next)
    CVWriteBreakCommandEntryInfo(conn,
				 instance_id,
				 l->line,
				 ""); /* transcript */

  CVWriteBreakCommandEndInfo(conn,
			     instance_id,
			     ""); /* transcript */
}

static void
breakpoint_notify(b, action)
     struct breakpoint *b;
     int action;
{
  struct symtab *s;
  struct symbol *s1;
  char *funcname = "", *filename = "", *included_in_filename = "";

  if (b->type != bp_breakpoint)
    return;

  s = b->symtab;

  if (s)
    {
      filename = s->filename;
      s1 = find_pc_function(b->address);
      if (s1)
	funcname = SYMBOL_NAME(s1);
    }      

  CVWriteBreakpointInfo (conn,
			 instance_id,
			 b->number,
			 b->line_number,
			 CFileLinePos,
			 CBreakOnInstrAccess,
			 action,
			 b->ignore_count,
			 funcname,
			 filename,
			 "",	/* included_in_filename */
			 ""	/* transcript */
			 );

  if (b->commands)
    cadillac_commands_breakpoint(b);
}

void
cadillac_create_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, CEnableBreakpoint);
}

void
cadillac_delete_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, CDeleteBreakpoint);
}

void
cadillac_enable_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, CEnableBreakpoint);
}

void
cadillac_disable_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, CDisableBreakpoint);
}

void
cadillac_ignore_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, CBreakAttrUnchanged);
}

static int command_line_length = 0;
static char *command_line_text = 0;

int
cadillac_reading()
{
  if (command_line_text)
    return 1;
  else
    return 0;
}

char *
cadillac_command_line_input()
{
  char *p;

  if (command_line_length <= 0)
    return (char *)NULL;

  p = command_line_text;

  while (command_line_length-- > 0)
    {
      if (*command_line_text == '\n')
	{
	  *command_line_text = '\000';
	  command_line_text++;
	  break;
	}
      command_line_text++;
    }

  return p;
}

/* Open up a pty and its associated tty.  Return the fd of the tty. */

char *
cadillac_getpty()
{
  int n, ptyfd, ttyfd;
  static char dev[30];
  struct stat statbuf;
  struct termios termios;

#define HIGHPTY (('z' - 'p') * 16 - 1)

  if (inferior_pty >= 0)		/* Only do this once */
    return dev;

  for (n = 0; n <= HIGHPTY; n++)
    {
      sprintf(dev, "/dev/pty%c%x", n/16 + 'p', n%16);
      if (stat(dev, &statbuf))
	break;
      ptyfd = open(dev, O_RDWR);
      if (ptyfd < 0)
	continue;
      sprintf(dev, "/dev/tty%c%x", n/16 + 'p', n%16);
      ttyfd = open(dev, O_RDWR);
      if (ttyfd < 0) {close(ptyfd); continue;}

      /* Setup pty for non-blocking I/O.  Also make it give us a SIGIO when
	 there's data available.  */

      n = fcntl(ptyfd, F_GETFL, 0);
      fcntl(ptyfd, F_SETFL, n|FNDELAY|FASYNC);
      fcntl(ptyfd, F_SETOWN, getpid());

      tcgetattr(ttyfd, &termios);
      termios.c_oflag &= ~OPOST; /* No post-processing */
      tcsetattr(ttyfd, TCSANOW, &termios);

      inferior_pty = ptyfd;
      inferior_tty = ttyfd;
      return dev;
    }

  error ("getpty: can't get a pty\n");
}

/* Examine a protocol packet from the driver. */

static int
kernel_dispatch(interrupt)
     int interrupt;		/* Non-zero means we are at interrupt level
				   and can only do a few commands. */
{
  register CHeader *head;

  head = (CHeader *)CPeekNextRequest (conn);
  if (head == NULL)
    {
      fprintf (stderr, "EOF on kernel read!\n");
      exit (1);
    }

  if (interrupt)
    switch (head->reqType)
      {
      case KillProgramRType:
      case TextIORType:
      case StopRType:
	break;
      default:
	return;
      }

  if (head->reqType < LastTtyRequestRType)
    {
      CTtyRequest* req = CReadTtyRequest (conn);
      switch (req->head.reqType)
	{
	case AcceptConnectionRType:
	  /* Tell the rest of the world that cadillac is now set up */
	  CSkipRequest (conn);
	  break;

	case RefuseConnectionRType:
	  fprintf (stderr, "Debugger connection refused\n");
	  exit (1);

	case KillProgramRType:
	  exit (0);

	case TextIORType:
	  {
	    char *p;
	    ReqLen len;

	    p = CGetVstring(conn, &len);
	    kernel_to_pty(p, len);
	  }
	  break;
	default:
	  fprintf(stderr, "Unknown request type = %d\n",
		  req->head.reqType);
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
	  exit(1);
	}

      switch (req->head.request->reqType)
	{
	case OpenProgramInstanceRType:
	  {
	    char *arglist, buf[100]; /* XXX - Make buf dynamic! */
	    int arglen;
	    /* XXX - should notice when program_id changes */
	    arglist = req->openProgramInstance.progArglist.text;
	    arglen = req->openProgramInstance.progArglist.byteLen;

	    execute_command_1("break main");
	    execute_command_1("enable delete $bpnum");
	    if (arglist)
	      {
		execute_command_1("set args %.*s", arglen, arglist);
	      }
	    execute_command_1("run");
	  }
	  break;
	case QuitDebuggerRType:
	  exit (0);
	case RunRType:
	  if (req->run.request->useArglist == CNewArglist)
	    {
	      execute_command_1("set args %.*s",
				req->run.progArglist.byteLen,
				req->run.progArglist.text);
	    }
	  execute_command_1("run");
	  break;
	case ContinueRType:
	  execute_command_1("continue");
	  break;
	case StepRType:
	  execute_command_1("step %d", req->step.request->stepCount);
	  break;
	case NextRType:
	  execute_command_1("next %d", req->next.request->nextCount);
	  break;
	case ChangeStackFrameRType:
	  switch (req->changeStackFrame.request->frameMovement)
	    {
	    case CToCurrentStackFrame:
	      execute_command_1("frame %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToInnerStackFrame:
	      execute_command_1("down %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToOuterStackFrame:
	      execute_command_1("up %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToAbsoluteStackFrame:
	      printf_filtered("ChangeStackFrame ToAbsolute\n");
	      break;
	    }
	  break;
	case BackTraceRType:
	  /* XXX - deal with limit??? */
	  execute_command_1("backtrace");
	  break;
	case FinishRType:
	  execute_command_1("finish");
	  break;
	case TerminateProgramRType:
	  execute_command_1("kill");
	  break;
	case NewBreakpointRType:
	  {
	    char *tail;
	    int skipped;

	    tail = rindex(req->newBreakpoint.fileName.text, '/');
	    if (!tail)
	      tail = req->newBreakpoint.fileName.text;
	    else
	      tail++;
	    skipped = tail - req->newBreakpoint.fileName.text;
	    execute_command_1("break %.*s:%d",
			      req->newBreakpoint.fileName.byteLen - skipped,
			      tail,
			      req->newBreakpoint.request->fileLinePos);
	  }
	  break;
	case StopRType:
	  {
	    extern int pgrp_inferior;
	    killpg(pgrp_inferior, SIGINT);
	  }
	  break;
	case UserInputRType:
	  {
	    char *text;
	    long len;

	    /* XXX - should really break command up into seperate lines
	       and spoon-feed it to execute_command */

	    text = req->userInput.userInput.text;
	    len = req->userInput.userInput.byteLen;

	    if (text[len-1] == '\n') text[len-1] = '\000';
	    execute_command (text);
	    break;
	  }
	case ChangeBreakpointRType:
	  switch (req->changeBreakpoint.request->breakpointAttr)
	    {
	    case CEnableBreakpoint:
	      execute_command_1("enable %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CDisableBreakpoint:
	      execute_command_1("disable %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CDeleteBreakpoint:
	      execute_command_1("delete %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CBreakAttrUnchanged:
	      execute_command_1("ignore %d %d",
				req->changeBreakpoint.request->breakpointId,
				req->changeBreakpoint.request->ignoreCount);
	      break;
	    default:
	      printf_filtered("Got to ChangeBreakpoint\n");
	      printf_filtered("  breakpointId = %d\n",
			      req->changeBreakpoint.request->breakpointId);
	      printf_filtered("  breakpointType = %d\n",
			      req->changeBreakpoint.request->breakpointType);
	      printf_filtered("  breakpointAttr = %d\n",
			      req->changeBreakpoint.request->breakpointAttr);
	      printf_filtered("  ignoreCount = %d\n",
			      req->changeBreakpoint.request->ignoreCount);
	      break;
	    }
	  break;
	case BreakConditionRType:
	  execute_command_1("condition %d %.*s",
			  req->breakCondition.request->breakpointId,
			  req->breakCondition.condition.byteLen,
			  req->breakCondition.condition.text);
	  break;
	case BreakCommandsRType:
	  /* Put pointers to where cadillac_command_line_input() can find
	     them. */
	  command_line_length = req->breakCommands.commands.byteLen;
	  command_line_text = req->breakCommands.commands.text;
	  execute_command_1("commands %d",
			    req->breakCommands.request->breakpointId);
	  command_line_text = (char *)NULL;
	  command_line_length = 0;
	  break;
	default:
	  fprintf(stderr, "Unknown request type = %d\n",
		  req->head.request->reqType);
	  break;
	}
      free (req); /* Should probably call CVFreeDebuggerRequest() here, but
		     can't do so if interrupt level has mucked with req->
		     request.  CVFreeDebuggerRequest() only ends up calling
		     free() anyway! */
    }
}

#define KERNEL_EVENT 1
#define PTY_EVENT 2

/* Return a bitmask indicating if the kernel or the pty did something
   interesting.  Set poll to non-zero if you don't want to wait.  */

static int
wait_for_events(poll)
     int poll;
{
  fd_set readfds;
  int numfds;
  int eventmask = 0;
  static struct timeval tv = {0};

  /* Output all pending requests. */
  CWriteRequestBuffer(conn);

  /* Wait till there's some activity from the kernel or the pty. */
  do
    {
      FD_ZERO(&readfds);
      FD_SET(kerfd, &readfds);
      if (inferior_pty > 0)
	FD_SET(inferior_pty, &readfds);
      if (poll)
	numfds = select(sizeof(readfds)*8, &readfds, 0, 0, &tv);
      else
	numfds = select(sizeof(readfds)*8, &readfds, 0, 0, 0);
    }
  while (numfds <= 0 && !poll);

  if (FD_ISSET(inferior_pty, &readfds))
    eventmask |= PTY_EVENT;

  if (FD_ISSET(kerfd, &readfds))
    eventmask |= KERNEL_EVENT;

  return eventmask;
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
  int n;

  if (!execarg) execarg = "";

  printf("gdb-debugger pid=%d\n", getpid()); /* XXX - debugging only */
  
  /* First establish the connection with the kernel. */

  kerfd = COpenClientSocket(NULL);
  if (kerfd < 0) {
    printf("COpenClientSocket() failed\n");
    exit(1);
  }

  /* Setup for I/O interrupts when appropriate. */

  n = fcntl(kerfd, F_GETFL, 0);
  fcntl(kerfd, F_SETFL, n|FASYNC);
  fcntl(kerfd, F_SETOWN, getpid());

  /* Setup connection buffering. */

  CSetSocketBufferSize (kerfd, 12000);

  /* Generate a new connection control block. */

  conn = NewConnection (0, kerfd, kerfd);
  if (!conn) {
    printf("NewConnection() failed\n");
    exit(1);
  }

#ifdef KERNEL_RECORD
  kerout = fopen("kernel.output", "+w");

  CReadWriteHooks(conn, conn->previewMethod, conn->readMethod, kernel_record);
#endif

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

  /* Tell the kernel our PID and all that */

  program_id = 1;
  CVWriteDebugProgramInfo(conn,
			  getpid(),
			  program_id,
			  execarg,
			  "");

  /* Tell the rest of the world that Cadillac is now set up. */
  cadillac = 1;
}

/* This is called from execute_command, and provides a wrapper around
   various command routines in a place where both protocol messages and
   user input both flow through.
*/

void
cadillac_call_command(cmdblk, arg, from_tty)
     struct cmd_list_element *cmdblk;
     char *arg;
     int from_tty;
{
  if (cmdblk->class == class_run)
    {
      go_busy();
      has_run = 1;
      (*cmdblk->function.cfunc)(arg, from_tty);
      send_status();
    }
  else
    (*cmdblk->function.cfunc)(arg, from_tty);

  prompt();
}

void
cadillac_new_process()
{
  instance_id = inferior_pid;
}

static void
iosig()
{
  while (1)
    {
      int eventmask;

      eventmask = wait_for_events(1);

      if (eventmask == 0)
	return;

      if (eventmask & PTY_EVENT)
	pty_to_kernel();

      if (eventmask & KERNEL_EVENT)
	kernel_dispatch(1);
    }
}

int
cadillac_wait(status)
     int *status;
{
  int pid;

  signal(SIGIO, iosig);

  pid = wait(status);

  signal(SIGIO, SIG_DFL);
  return pid;
}

static void
null_routine(arg)
     int arg;
{
}

/* All requests from the Cadillac kernel eventually end up here. */

void
cadillac_main_loop(pp)
     char **pp;
{
  CTtyRequest *req;
  struct cleanup *old_chain;

  pprompt = pp;

/* We will come thru here any time there is an error, so send status if
   necessary. */

  send_status();

  prompt();

  /* The actual event loop! */

  while (1)
    {
      int eventmask;

      eventmask = wait_for_events(0);

      if (eventmask & PTY_EVENT)
	pty_to_kernel();

      if (eventmask & KERNEL_EVENT)
	{
	  old_chain = make_cleanup(null_routine, 0);
	  kernel_dispatch(0);
	  bpstat_do_actions(&stop_bpstat);
	  do_cleanups(old_chain);
	}
    }
}
