/* Energize (formerly known as Cadillac) interface routines.
   Copyright 1991, 1992 Free Software Foundation, Inc.

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
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
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
#ifdef USG
#include <sys/file.h>
#endif
#include <fcntl.h>
#include <sys/filio.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/errno.h>
#include <termios.h>
#include <strings.h>

/* Non-zero means that we're doing the cadillac interface. */
int cadillac = 0;

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

extern int pgrp_inferior;

extern char *source_path;

char **pprompt;			/* Pointer to pointer to prompt */

/* Tell cadillac_command_line_input() where to get its text from */
static int doing_breakcommands_message = 0;

/* Stash command text here */
static char *command_line_text = 0;
static int command_line_length = 0;

/* Flags returned by wait_for_events() */
#define KERNEL_EVENT 1
#define PTY_EVENT 2


/* This routine redirects the output of fputs_filtered to the kernel so that
   the user can see what's going on in his debugger window. */

void
cadillac_fputs(ptr)
     const char *ptr;
{
  if (conn)
    CVWriteTranscriptInfo (conn, instance_id, (char *)ptr);
  else
    fputs (ptr, stdout);
}

void
cadillac_query(query, args)
     char *query;
     va_list args;
{
  char buf[100];

  vsprintf(buf, query, args);

  CVWriteQueryInfo(conn,
		   instance_id,
		   CQueryConfirm,
		   qno_unknown,
		   buf,
		   "");		/* transcript */
}

void
cadillac_acknowledge_query(ack)
     char *ack;
{
  CVWriteQueryInfo(conn,
		   instance_id,
		   CQueryAcknowleged,
		   0,
		   ack,
		   "");		/* transcript */
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

static char *
full_filename(symtab)
     struct symtab *symtab;
{
  int pathlen;
  char *filename;

  if (!symtab)
    return NULL;

  if (symtab->fullname)
    return savestring(symtab->fullname, strlen(symtab->fullname));

  if (symtab->dirname)
    pathlen = strlen(symtab->dirname);
  else
    pathlen = 0;
  if (symtab->filename)
    pathlen += strlen(symtab->filename);

  filename = xmalloc(pathlen+1);

  if (symtab->dirname)
    strcpy(filename, symtab->dirname);
  else
    *filename = '\000';
  if (symtab->filename)
    strcat(filename, symtab->filename);

  return filename;
}

/* Tell the cadillac kernel how high the stack is so that frame numbers (which
   are relative to the current stack height make sense.

   Calculate the number of frames on the stack, and the number of subroutine
   invocations that haven't changed since the last call to this routine.  The
   second number is calculated by comparing the PCs of the current stack frames
   to the PCs of the previous set of stack frames.  The screw here is that a
   subroutine may call several different procedures, which means that the PC
   in its frame changes, even though you are still in the same subroutine.  We
   resolve this by converting the frames PC into the PC at the start of the
   function (for efficiency, this is done only if the simple comparison test
   fails). */

struct pclist
{
  CORE_ADDR pc;
  struct pclist *next;
};

/* Non-zero means that Cadillac kernel already knows how high the stack is. */
static int stack_info_valid = 0;

static void
send_stack_info()
{
  struct pclist *pclist = 0, *pli, *opli;
  static struct pclist *old_pclist;
  FRAME frame;
  int height, similar;

  if (stack_info_valid)
    return;

  height = 0;
  similar = 0;

/* First, calculate the stack height, and build the new pclist */

  for (frame = get_current_frame();
       frame != 0;
       frame = get_prev_frame(frame))
    {
      (height)++;
      pli = (struct pclist *)xmalloc(sizeof(struct pclist));

      pli->pc = frame->pc;
      pli->next = pclist;
      pclist = pli;
    }

/* Now, figure out how much of the stack hasn't changed */

  for (pli = pclist, opli = old_pclist;
       pli != 0 && opli != 0;
       pli = pli->next, opli = opli->next, (similar)++)
    {
      if ((pli->pc != opli->pc)
	  && (get_pc_function_start(pli->pc)
	      != get_pc_function_start(opli->pc)))
	break;
    }

/* Free up all elements of the old pclist */

  opli = old_pclist;

  while (opli)
    {
      pli = opli->next;
      free (opli);
      opli = pli;
    }

  old_pclist = pclist;		/* Install the new pclist */

  CVWriteStackSizeInfo(conn,
		       instance_id,
		       height,	/* Frame depth */
		       CInnerFrameIs0,
		       similar, /* Frame diff */
		       ""	/* Transcript */
		       );

  stack_info_valid = 1;
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
  filename = full_filename(sal.symtab);

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

  send_stack_info();

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

  if (filename)
    free(filename);
}

/* Call this to output annotated function names.  Names will be demangled if
   necessary.  arg_mode contains flags that are passed on to cplus_demangle. */

void
cadillac_annotate_function(funcname, arg_mode, level)
     char *funcname;
     int arg_mode;
     int level;
{
  extern int demangle;
  char *demangled_name = NULL;


  if (funcname == NULL)
    return;

  if (demangle)
    {
      demangled_name = cplus_demangle(funcname, arg_mode);

      if (demangled_name)
	funcname = demangled_name;
    }

  send_stack_info();

  if (level < 0) level = 0;

  CVWriteBackTraceEntryInfo(conn,
			    instance_id,
			    level, /* frameNo */
			    funcname);

  if (demangled_name)
    free(demangled_name);
}

/* Call this just prior to printing out the name & value of a variable.  This
   tells the kernel where to annotate the output. */

/* The args are:
   expression - A text handle on what GDB can use to reference this value.
   	        This can be a symbol name, or a convenience var, etc...
   symbol - Used to determine the scope of the data.  May be NULL.
   type - Determines if we have a pointer ref, and the print name of the type.
          Used in ShowValue message.
   valaddr - The address in target memory of the data.
   field - The field name of the struct or union element being referenced.
*/

static char cum_expr[200];	/* Cumulative expression */
static char *expr_stack[100] = {cum_expr}; /* Pointers to end of expressions */
static char **last_expr = expr_stack;	/* Current expr stack pointer */

void
cadillac_start_variable_annotation(expression, symbol, type, valaddr, field)
     char *expression;
     struct symbol *symbol;
     struct type *type;
     CORE_ADDR valaddr;
     char *field;
{
  int ref_type;
  int stor_cl;
  enum type_code type_code;
  enum address_class sym_class;
  char *type_cast;

  send_stack_info();

  strcpy(*last_expr++, expression);
  *last_expr = *(last_expr-1) + strlen(expression);

  switch (TYPE_CODE(type))
    {
    case TYPE_CODE_ARRAY:
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
      ref_type = CValueValueRef;
      break;
    case TYPE_CODE_PTR:
      ref_type = CValuePointerRef;
      break;
    default:
      ref_type = CValueUndefRef;
      break;
    }

/* Make sure that pointer points at something we understand */

  if (ref_type == CValuePointerRef)
    switch (TYPE_CODE(TYPE_TARGET_TYPE(type)))
      {
      case TYPE_CODE_PTR:
      case TYPE_CODE_ARRAY:
      case TYPE_CODE_STRUCT:
      case TYPE_CODE_UNION:
      case TYPE_CODE_ENUM:
      case TYPE_CODE_INT:
      case TYPE_CODE_FLT:
	break;
      default:
	ref_type = CValueUndefRef;
	break;
      }

  if (symbol)
    {
      sym_class = SYMBOL_CLASS(symbol);

      switch (sym_class)
	{
	case LOC_CONST:
	case LOC_CONST_BYTES:
	  stor_cl = CValueStorStaticConst;
	  break;
	case LOC_STATIC:
	  stor_cl = CValueStorStaticVar;
	  break;
	case LOC_REGISTER:
	case LOC_REGPARM:
	  stor_cl = CValueStorRegister;
	  break;
	case LOC_ARG:
	case LOC_REF_ARG:
	case LOC_LOCAL:
	case LOC_LOCAL_ARG:
	  stor_cl = CValueStorLocalVar;
	  break;
	default:
	  stor_cl = CValueStorUndef;
	  break;
	}
    }
  else
    stor_cl = CValueStorUndef;

  type_cast = TYPE_NAME(type);

  CVWriteValueBeginInfo(conn,
			instance_id,
			valaddr,
			ref_type,
		        stor_cl,
		        0,	/* XXX - frameno */
			cum_expr,
			field,
			type_cast,
			"");	/* transcript */
}

void
cadillac_end_variable_annotation()
{
  last_expr--;			/* Pop the expr stack */
  **last_expr = '\000';		/* Cut off the last part of the expr */

  CVWriteValueEndInfo(conn,
		      instance_id,
		      "");	/* transcript */
}

/* Tell the kernel that the target is now running. */

static void
go_busy()
{
  CVWriteProgramBusyInfo(conn,
			 instance_id,
			 "");	/* XXX ? transcript */
  CWriteRequestBuffer(conn);	/* Must take place synchronusly! */
  stack_info_valid = 0;
}


void
cadillac_symbol_file(objfile)
     struct objfile *objfile;
{
  CVWriteSymbolTableInfo(conn,
			 objfile->name,
			 "");	/* Transcript */
}

/* execute_command_1(echo, queue, cmd, args) - echo - non-zero means echo the
   command.  queue - non-zero means don't execute it now, just save it away for
   later.  cmd - string containing printf control sequences.  args - list of
   arguments needed by those control sequences.
 */

/* Linked list of queued up commands */
static struct command_line *queued_commands = 0;
static struct command_line *last_queued_command = 0;

/* Call this routine to save a command for later.  The command string is
   copied into freshly malloc'ed memory. */

static void
queue_command(cmd)
     char *cmd;
{
  char *buf;
  struct command_line *cl;
  unsigned long s;

  s = (strlen(cmd) + 1) + 7 & ~(unsigned long)7;

  buf = (char *)xmalloc(s + sizeof(struct command_line));
  cl = (struct command_line *)(buf + s);
  cl->next = 0;
  cl->line = buf;

  strncpy(cl->line, cmd, s);

  if (queued_commands)
    last_queued_command->next = cl;
  else
    queued_commands = cl;

  last_queued_command = cl;
}

/* Call this procedure to take a command off of the command queue.  It returns
   a pointer to a buf which the caller is responsible for freeing.  NULL is
   returned if there are no commands queued. */

static char *
dequeue_command()
{
  struct command_line *cl;
  char *cmd;

  cl = queued_commands;

  if (!cl)
    return NULL;

  queued_commands = cl->next;

  return cl->line;
}

static void
execute_command_1(va_alist)
     va_dcl
{
  char buf[100];		/* XXX - make buf dynamic! */
  
  int echo;
  int queue;
  char *cmd;
  va_list args;

  va_start(args);

  echo = va_arg(args, int);
  queue = va_arg(args, int);
  cmd = va_arg(args, char *);

  vsprintf(buf, cmd, args);

  if (queue)
    queue_command(buf);
  else
    {
      if (echo)
	printf_filtered("%s\n", buf);
      execute_command(buf, 1);
    }

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
  struct symbol *sym;
  char *funcname = "";
  char *filename;
  char *included_in_filename = "";

  if (b->type != bp_breakpoint)
    return;

  filename = full_filename(b->symtab);

  sym = find_pc_function(b->address);
  if (sym)
    funcname = SYMBOL_NAME(sym);

  CVWriteBreakpointInfo (conn,
			 instance_id,
			 b->number,
			 b->line_number,
			 CFileLinePos,
			 CBreakOnInstrAccess,
			 action,
			 b->ignore_count,
			 funcname,
			 filename ? filename : "",
			 "",	/* included_in_filename */
			 ""	/* transcript */
			 );

  if (b->commands)
    cadillac_commands_breakpoint(b);

  cadillac_condition_breakpoint(b);

  if (filename)
    free(filename);
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

/* Open up a pty and its associated tty.  Return the fd of the tty. */

static void
getpty()
{
  int n, ptyfd, ttyfd;
  static char dev[30];
  struct stat statbuf;
  struct termios termios;

#define HIGHPTY (('z' - 'p') * 16 - 1)

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
      return;
    }

  error ("getpty: can't get a pty\n");
}

/* Examine a protocol packet from the driver. */

static int
kernel_dispatch(queue)
     int queue;			/* Non-zero means we should just queue up
				   commands. */
{
  register CHeader *head;

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

	    execute_command_1(1, queue, "break main");
	    execute_command_1(1, queue, "enable delete $bpnum");
	    if (arglist)
	      {
		execute_command_1(1, queue, "set args %.*s", arglen, arglist);
	      }
	    execute_command_1(1, queue, "run");
	  }
	  break;
	case SearchPathRType:
	  directory_command(req->searchPath.searchPath.text, 0);
	  break;
	case QuitDebuggerRType:
	  execute_command_1(1, queue, "quit");
	  break;
	case RunRType:
	  if (req->run.request->useArglist == CNewArglist)
	    {
	      execute_command_1(1, queue, "set args %.*s",
				req->run.progArglist.byteLen,
				req->run.progArglist.text);
	    }
	  execute_command_1(1, queue, "run");
	  break;
	case ContinueRType:
	  execute_command_1(1, queue, "continue");
	  break;
	case StepRType:
	  execute_command_1(1, queue, "step %d", req->step.request->stepCount);
	  break;
	case NextRType:
	  execute_command_1(1, queue, "next %d", req->next.request->nextCount);
	  break;
	case ChangeStackFrameRType:
	  switch (req->changeStackFrame.request->frameMovement)
	    {
	    case CToCurrentStackFrame:
	      execute_command_1(1, queue, "frame %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToInnerStackFrame:
	      execute_command_1(1, queue, "down %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToOuterStackFrame:
	      execute_command_1(1, queue, "up %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    case CToAbsoluteStackFrame:
	      execute_command_1(1, queue, "frame %d",
				req->changeStackFrame.request->frameNo);
	      break;
	    }
	  break;
	case BackTraceRType:
	  /* XXX - deal with limit??? */
	  execute_command_1(1, queue, "backtrace");
	  break;
	case FinishRType:
	  execute_command_1(1, queue, "finish");
	  break;
	case TerminateProgramRType:
	  execute_command_1(1, queue, "kill");
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
	    execute_command_1(1, queue, "break %.*s:%d",
			      req->newBreakpoint.fileName.byteLen - skipped,
			      tail,
			      req->newBreakpoint.request->fileLinePos);
	  }
	  break;
	case StopRType:
	  killpg(pgrp_inferior, SIGINT);
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

	    while (*text == ' ' || *text == '\t') text++;

	    if (strcmp(text, "]*[") == 0) /* XXX - What does this mean??? */
	      break;

	    if (*text != '\000')
	      execute_command_1(0, queue, "%s", text);
	    else
	      print_prompt();	/* User just typed a blank line */
	  }
	  break;
	case QueryResponseRType:
	  {
	    char *resp;

	    if (req->queryResponse.request->response)
	      resp = "y";
	    else
	      resp = "n";
	    execute_command_1(1, 1, resp);
	    printf_filtered("%s\n", resp);
	  }
	  break;
	case ChangeBreakpointRType:
	  switch (req->changeBreakpoint.request->breakpointAttr)
	    {
	    case CBreakAttrUnchanged:
	      execute_command_1(1, queue, "ignore %d %d",
				req->changeBreakpoint.request->breakpointId,
				req->changeBreakpoint.request->ignoreCount);
	      break;
	    case CEnableBreakpoint:
	      execute_command_1(1, queue, "enable %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CDisableBreakpoint:
	      execute_command_1(1, queue, "disable %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CDeleteBreakpoint:
	      execute_command_1(1, queue, "delete %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CEnableDisableBreakpoint:
	      execute_command_1(1, queue, "enable once %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    case CEnableDeleteBreakpoint:
	      execute_command_1(1, queue, "enable delete %d",
				req->changeBreakpoint.request->breakpointId);
	      break;
	    default:
	      printf_filtered("ChangeBreakpointRType: unknown breakpointAttr\n");
	      printf_filtered("  breakpointAttr = %d\n",
			      req->changeBreakpoint.request->breakpointAttr);
	      printf_filtered("  breakpointId = %d\n",
			      req->changeBreakpoint.request->breakpointId);
	      printf_filtered("  breakpointType = %d\n",
			      req->changeBreakpoint.request->breakpointType);
	      printf_filtered("  ignoreCount = %d\n",
			      req->changeBreakpoint.request->ignoreCount);
	      break;
	    }
	  break;
	case BreakConditionRType:
	  execute_command_1(1, queue, "condition %d %.*s",
			  req->breakCondition.request->breakpointId,
			  req->breakCondition.condition.byteLen,
			  req->breakCondition.condition.text);
	  break;
	case BreakCommandsRType:
	  /* Put pointers to where cadillac_command_line_input() can find
	     them. */
	  doing_breakcommands_message = 1;
	  command_line_length = req->breakCommands.commands.byteLen;
	  command_line_text = req->breakCommands.commands.text;
	  execute_command_1(1, queue, "commands %d",
			    req->breakCommands.request->breakpointId);
	  command_line_text = (char *)NULL;
	  command_line_length = 0;
	  doing_breakcommands_message = 0;
	  break;
	case ShowValueRType:
	  {
	    char expr[100], *p = expr;

	    expr[0] = 0;

	    if (req->showValue.request->ref_type == CValuePointerRef)
	      strcat(expr, "* ");

	    if (req->showValue.type_cast.byteLen)
	      {
		strcat(expr, "(");
		strncat(expr, req->showValue.type_cast.text,
			req->showValue.type_cast.byteLen);
		strcat(expr, ") ");
	      }

	    if (req->showValue.field.byteLen)
	      strcat(expr, "(");

	    strncat(expr, req->showValue.expression.text,
		    req->showValue.expression.byteLen);

	    if (req->showValue.field.byteLen)
	      {
		strcat(expr, ")");

		strncat(expr, req->showValue.field.text,
			req->showValue.field.byteLen);
	      }

	    execute_command_1(1, queue, "print %s", expr);
	  }
	  break;
	case SetValueRType:
	  {
	    char expr[100], *p = expr;

	    expr[0] = 0;

	    if (req->setValue.request->ref_type == CValuePointerRef)
	      strcat(expr, "* ");

#if 0
	    if (req->setValue.type_cast.byteLen)
	      {
		strcat(expr, "(");
		strncat(expr, req->setValue.type_cast.text,
			req->setValue.type_cast.byteLen);
		strcat(expr, ") ");
	      }
#endif
	    if (req->setValue.field.byteLen)
	      strcat(expr, "(");

	    strncat(expr, req->setValue.expression.text,
		    req->setValue.expression.byteLen);

	    if (req->setValue.field.byteLen)
	      {
		strcat(expr, ")");

		strncat(expr, req->setValue.field.text,
			req->setValue.field.byteLen);
	      }

	    execute_command_1(1, queue, "print %s = (%s) %s", expr,
			      req->setValue.type_cast.text,
			      req->setValue.value.text);
	  }
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

  FD_ZERO(&readfds);

  /* Wait till there's some activity from the kernel or the pty. */
  do
    {
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

/* This is called from read_command_lines() to provide the text for breakpoint
   commands, which is supplied in a BreakCommands message.  Each call to this
   routine supplies a single line of text, with the newline removed. */

/* This routine may be invoked in two different contexts.  In the first, it
   is being called as a result of the BreakCommands message.  In this case,
   all of the command text is immediately available.  In the second case, it is
   called as a result of the user typing the 'command' command.  The command
   text then needs to be glommed out of UserInput messages (and possibly other
   messages as well).  The most 'straighforward' way of doing this is to
   basically simulate the main loop, but just accumulate the command text
   instead of sending it to execute_command().  */

char *
cadillac_command_line_input()
{
  char *p;

  if (doing_breakcommands_message)
    {
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

      printf_filtered("%s\n", p);
      return p;
    }
  else
    {
      /* We come here when the user has typed the 'command' or 'define' command
	 to the GDB window.  We are basically deep inside of the 'command'
	 command processing routine right now, and will be called to get a new
	 line of input.  We expect that kernel_dispatch will queue up only one
	 command at a time. */

      int eventmask;
      static char buf[100];
      
      eventmask = wait_for_events(0);

      if (eventmask & PTY_EVENT)
	pty_to_kernel();

      if (eventmask & KERNEL_EVENT)
	kernel_dispatch(1);	/* Queue up commands */

/* Note that command has been echoed by the time we get here */

      p = dequeue_command();

      if (p)
	{
	  strncpy(buf, p, sizeof(buf));
	  free(p);
	  return buf;
	}
      else
	return NULL;
    }
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

  setsid();			/* Drop controlling tty, become pgrp master */
  getpty();			/* Setup the pty */
  dup2(inferior_tty, 0);	/* Attach all GDB I/O to the pty */
  dup2(inferior_tty, 1);
  dup2(inferior_tty, 2);
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

  print_prompt();
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
cadillac_main_loop()
{
  CTtyRequest *req;
  struct cleanup *old_chain;

  doing_breakcommands_message = 0;

/* We will come thru here any time there is an error, so send status if
   necessary. */

  send_status();

  print_prompt();

  /* The actual event loop! */

  while (1)
    {
      int eventmask;
      char *cmd;

      old_chain = make_cleanup(null_routine, 0);

/* First, empty out the command queue, then check for new requests. */

      while (cmd = dequeue_command())
	{
	  execute_command_1(1, 0, cmd);
	  free(cmd);
	}

      eventmask = wait_for_events(0);

      if (eventmask & PTY_EVENT)
	pty_to_kernel();

      if (eventmask & KERNEL_EVENT)
	kernel_dispatch(0);

      bpstat_do_actions(&stop_bpstat);
      do_cleanups(old_chain);
    }
}
