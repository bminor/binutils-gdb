/* Memory-access and commands for remote VxWorks processes, for GDB.
   Copyright (C) 1990-1991  Free Software Foundation, Inc.
   Contributed by Wind River Systems and Cygnus Support.

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
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "wait.h"
#include "target.h"
#include "gdbcore.h"
#include "command.h"
#include "symtab.h"
#include "symfile.h"		/* for struct complaint */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#define free bogon_free		/* Sun claims "int free()" not void */
#include <rpc/rpc.h>
#undef free
#include <sys/time.h>		/* UTek's <rpc/rpc.h> doesn't #incl this */
#include <netdb.h>
#include <ptrace.h>
#include "xdr_ptrace.h"
#include "xdr_ld.h"
#include "xdr_rdb.h"
#include "dbgRpcLib.h"

/* get rid of value.h if possible */
#include <value.h>
#include <symtab.h>
 
extern value call_function_by_hand ();
extern void symbol_file_command ();
extern void add_syms_addr_command ();
extern int stop_soon_quietly;		/* for wait_for_inferior */

static int net_ptrace_clnt_call ();	/* Forward decl */
static enum clnt_stat net_clnt_call ();	/* Forward decl */
extern struct target_ops vx_ops, vx_run_ops;	/* Forward declaration */

/* Saved name of target host and called function for "info files".
   Both malloc'd.  */

static char *vx_host;
static char *vx_running;		/* Called function */

/* Nonzero means target that is being debugged remotely has a floating
   point processor.  */

static int target_has_fp;

/* Default error message when the network is forking up.  */

static const char rpcerr[] = "network target debugging:  rpc error";

CLIENT *pClient;         /* client used in net debugging */
static int ptraceSock = RPC_ANYSOCK;

enum clnt_stat net_clnt_call();
static void parse_args ();

static struct timeval rpcTimeout = { 10, 0 };

static char *skip_white_space ();
static char *find_white_space ();
 
/* Tell the VxWorks target system to download a file.
   The load addresses of the text, data, and bss segments are
   stored in pTextAddr, pDataAddr, and *pBssAddr (respectively).
   Returns 0 for success, -1 for failure.  */

static int
net_load (filename, pTextAddr, pDataAddr, pBssAddr)
    char *filename;
    CORE_ADDR *pTextAddr;
    CORE_ADDR *pDataAddr;
    CORE_ADDR *pBssAddr;
    {
    enum clnt_stat status;
    struct ldfile ldstruct;
    struct timeval load_timeout;
 
    bzero ((char *) &ldstruct, sizeof (ldstruct));

    /* We invoke clnt_call () here directly, instead of through
       net_clnt_call (), because we need to set a large timeout value.
       The load on the target side can take quite a while, easily
       more than 10 seconds.  The user can kill this call by typing
       CTRL-C if there really is a problem with the load.  
       
       Do not change the tv_sec value without checking -- select() imposes
       a limit of 10**8 on it for no good reason that I can see...  */

    load_timeout.tv_sec = 99999999;   /* A large number, effectively inf. */
    load_timeout.tv_usec = 0;
 
    status = clnt_call (pClient, VX_LOAD, xdr_wrapstring, &filename, xdr_ldfile,
			&ldstruct, load_timeout);

    if (status == RPC_SUCCESS)
      {
        if (*ldstruct.name == NULL)	/* load failed on VxWorks side */
          return -1;
	*pTextAddr = ldstruct.txt_addr;
	*pDataAddr = ldstruct.data_addr;
	*pBssAddr = ldstruct.bss_addr;
	return 0;
      }
    else
        return -1;
    }
      
/* returns 0 if successful, errno if RPC failed or VxWorks complains. */

static int
net_break (addr, procnum)
    int addr;
    u_long procnum;
    {
    enum clnt_stat status;
    int break_status;
    Rptrace ptrace_in;  /* XXX This is stupid.  It doesn't need to be a ptrace
                           structure.  How about something smaller? */

    bzero ((char *) &ptrace_in, sizeof (ptrace_in));
    break_status = 0;

    ptrace_in.addr = addr;
    ptrace_in.pid = inferior_pid;

    status = net_clnt_call (procnum, xdr_rptrace, &ptrace_in, xdr_int,
			    &break_status);

    if (status != RPC_SUCCESS)
	return errno;

    if (break_status == -1)
      return ENOMEM;
    return break_status;	/* probably (FIXME) zero */
    }
 
/* returns 0 if successful, errno otherwise */

int
vx_insert_breakpoint (addr)
    int addr;
    {
    return net_break (addr, VX_BREAK_ADD);
    }

/* returns 0 if successful, errno otherwise */

int
vx_remove_breakpoint (addr)
    int addr;
    {
    return net_break (addr, VX_BREAK_DELETE);
    }

/* Call a function on the VxWorks target system.
   ARGS is a vector of values of arguments (NARGS of them).
   FUNCTION is a value, the function to be called.
   Returns a struct value * representing what the function returned.
   May fail to return, if a breakpoint or signal is hit
   during the execution of the function.  */

#ifdef FIXME
/* FIXME, function calls are really fried.  GO back to manual method. */
value
vx_call_function (function, nargs, args)
     value function;
     int nargs;
     value *args;
{
  register CORE_ADDR sp;
  register int i;
  CORE_ADDR start_sp;
  static REGISTER_TYPE dummy[] = CALL_DUMMY;
  REGISTER_TYPE dummy1[sizeof dummy / sizeof (REGISTER_TYPE)];
  CORE_ADDR old_sp;
  struct type *value_type;
  unsigned char struct_return;
  CORE_ADDR struct_addr;
  struct inferior_status inf_status;
  struct cleanup *old_chain;
  CORE_ADDR funaddr;
  int using_gcc;

  save_inferior_status (&inf_status, 1);
  old_chain = make_cleanup (restore_inferior_status, &inf_status);

  /* PUSH_DUMMY_FRAME is responsible for saving the inferior registers
     (and POP_FRAME for restoring them).  (At least on most machines)
     they are saved on the stack in the inferior.  */
  PUSH_DUMMY_FRAME;

  old_sp = sp = read_register (SP_REGNUM);

#if 1 INNER_THAN 2		/* Stack grows down */
  sp -= sizeof dummy;
  start_sp = sp;
#else				/* Stack grows up */
  start_sp = sp;
  sp += sizeof dummy;
#endif

  funaddr = find_function_addr (function, &value_type);

  {
    struct block *b = block_for_pc (funaddr);
    /* If compiled without -g, assume GCC.  */
    using_gcc = b == NULL || BLOCK_GCC_COMPILED (b);
  }

  /* Are we returning a value using a structure return or a normal
     value return? */

  struct_return = using_struct_return (function, funaddr, value_type,
				       using_gcc);

  /* Create a call sequence customized for this function
     and the number of arguments for it.  */
  bcopy (dummy, dummy1, sizeof dummy);
  FIX_CALL_DUMMY (dummy1, start_sp, funaddr, nargs, args,
		  value_type, using_gcc);

#if CALL_DUMMY_LOCATION == ON_STACK
  write_memory (start_sp, dummy1, sizeof dummy);

#else /* Not on stack.  */
#if CALL_DUMMY_LOCATION == BEFORE_TEXT_END
  /* Convex Unix prohibits executing in the stack segment. */
  /* Hope there is empty room at the top of the text segment. */
  {
    static checked = 0;
    if (!checked)
      for (start_sp = text_end - sizeof dummy; start_sp < text_end; ++start_sp)
	if (read_memory_integer (start_sp, 1) != 0)
	  error ("text segment full -- no place to put call");
    checked = 1;
    sp = old_sp;
    start_sp = text_end - sizeof dummy;
    write_memory (start_sp, dummy1, sizeof dummy);
  }
#else /* After text_end.  */
  {
    int errcode;
    sp = old_sp;
    start_sp = text_end;
    errcode = target_write_memory (start_sp, dummy1, sizeof dummy);
    if (errcode != 0)
      error ("Cannot write text segment -- call_function failed");
  }
#endif /* After text_end.  */
#endif /* Not on stack.  */

#ifdef STACK_ALIGN
  /* If stack grows down, we must leave a hole at the top. */
  {
    int len = 0;

    /* Reserve space for the return structure to be written on the
       stack, if necessary */

    if (struct_return)
      len += TYPE_LENGTH (value_type);
    
    for (i = nargs - 1; i >= 0; i--)
      len += TYPE_LENGTH (VALUE_TYPE (value_arg_coerce (args[i])));
#ifdef CALL_DUMMY_STACK_ADJUST
    len += CALL_DUMMY_STACK_ADJUST;
#endif
#if 1 INNER_THAN 2
    sp -= STACK_ALIGN (len) - len;
#else
    sp += STACK_ALIGN (len) - len;
#endif
  }
#endif /* STACK_ALIGN */

    /* Reserve space for the return structure to be written on the
       stack, if necessary */

    if (struct_return)
      {
#if 1 INNER_THAN 2
	sp -= TYPE_LENGTH (value_type);
	struct_addr = sp;
#else
	struct_addr = sp;
	sp += TYPE_LENGTH (value_type);
#endif
      }

#if defined (REG_STRUCT_HAS_ADDR)
  {
    /* This is a machine like the sparc, where we need to pass a pointer
       to the structure, not the structure itself.  */
    if (REG_STRUCT_HAS_ADDR (using_gcc))
      for (i = nargs - 1; i >= 0; i--)
	if (TYPE_CODE (VALUE_TYPE (args[i])) == TYPE_CODE_STRUCT)
	  {
	    CORE_ADDR addr;
#if !(1 INNER_THAN 2)
	    /* The stack grows up, so the address of the thing we push
	       is the stack pointer before we push it.  */
	    addr = sp;
#endif
	    /* Push the structure.  */
	    sp = value_push (sp, args[i]);
#if 1 INNER_THAN 2
	    /* The stack grows down, so the address of the thing we push
	       is the stack pointer after we push it.  */
	    addr = sp;
#endif
	    /* The value we're going to pass is the address of the thing
	       we just pushed.  */
	    args[i] = value_from_long (builtin_type_long, (LONGEST) addr);
	  }
  }
#endif /* REG_STRUCT_HAS_ADDR.  */

#ifdef PUSH_ARGUMENTS
  PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr);
#else /* !PUSH_ARGUMENTS */
  for (i = nargs - 1; i >= 0; i--)
    sp = value_arg_push (sp, args[i]);
#endif /* !PUSH_ARGUMENTS */

#ifdef CALL_DUMMY_STACK_ADJUST
#if 1 INNER_THAN 2
  sp -= CALL_DUMMY_STACK_ADJUST;
#else
  sp += CALL_DUMMY_STACK_ADJUST;
#endif
#endif /* CALL_DUMMY_STACK_ADJUST */

  /* Store the address at which the structure is supposed to be
     written.  Note that this (and the code which reserved the space
     above) assumes that gcc was used to compile this function.  Since
     it doesn't cost us anything but space and if the function is pcc
     it will ignore this value, we will make that assumption.

     Also note that on some machines (like the sparc) pcc uses a 
     convention like gcc's.  */

  if (struct_return)
    STORE_STRUCT_RETURN (struct_addr, sp);

  /* Write the stack pointer.  This is here because the statements above
     might fool with it.  On SPARC, this write also stores the register
     window into the right place in the new stack frame, which otherwise
     wouldn't happen.  (See write_inferior_registers in sparc-xdep.c.)  */
  write_register (SP_REGNUM, sp);

  /* Figure out the value returned by the function.  */
  {
    char retbuf[REGISTER_BYTES];

    /* Execute the stack dummy routine, calling FUNCTION.
       When it is done, discard the empty frame
       after storing the contents of all regs into retbuf.  */
    run_stack_dummy (start_sp + CALL_DUMMY_START_OFFSET, retbuf);

    do_cleanups (old_chain);

    return value_being_returned (value_type, retbuf, struct_return);
  }
}
/* should return a value of some sort */
 
value
vx_call_function (funcAddr, nargs, args, valueType)
    char *funcAddr;
    int nargs;
    value *args;
    struct type * valueType;
{
    int i;
    func_call funcInfo;
    arg_value *argValue;
    enum clnt_stat status;
    register int len;
    arg_value funcReturn;
    value gdbValue;

    argValue = (arg_value *) xmalloc (nargs * sizeof (arg_value));

    bzero (argValue, nargs * sizeof (arg_value));
    bzero (&funcReturn, sizeof (funcReturn));

    for (i = nargs - 1; i >= 0; i--)
	{
	len = TYPE_LENGTH (VALUE_TYPE (args [i]));

	switch (TYPE_CODE (VALUE_TYPE (args[i])))
	    {
	    /* XXX put other types here.  Where's CHAR, etc??? */

	    case TYPE_CODE_FLT:
		argValue[i].type = T_FLOAT;
		break;
	    case TYPE_CODE_INT:
	    case TYPE_CODE_PTR:
	    case TYPE_CODE_ENUM:
	    case TYPE_CODE_FUNC:
		argValue[i].type = T_INT;
		break;

	    case TYPE_CODE_UNDEF:
	    case TYPE_CODE_ARRAY:
	    case TYPE_CODE_STRUCT:
	    case TYPE_CODE_UNION:
	    case TYPE_CODE_VOID:
	    case TYPE_CODE_SET:
	    case TYPE_CODE_RANGE:
	    case TYPE_CODE_PASCAL_ARRAY:
	    case TYPE_CODE_MEMBER:	    /* C++ */
	    case TYPE_CODE_METHOD:	    /* C++ */
	    case TYPE_CODE_REF:		    /* C++ */
	    default:
		error ("No corresponding VxWorks type for %d.  CHECK IT OUT!!!\n",
			TYPE_CODE(VALUE_TYPE(args[i])));
	    } /* switch */
	if (TYPE_CODE(VALUE_TYPE(args[i])) == TYPE_CODE_FUNC)
	    argValue[i].arg_value_u.v_int = VALUE_ADDRESS(args[i]);
	else
	    bcopy (VALUE_CONTENTS (args[i]), (char *) &argValue[i].arg_value_u,
	       	   len);
	}

    /* XXX what should the type of this function addr be?
     * XXX Both in gdb and vxWorks
     */
    funcInfo.func_addr = (int) funcAddr;
    funcInfo.args.args_len = nargs;
    funcInfo.args.args_val = argValue;

    status = net_clnt_call (VX_CALL_FUNC, xdr_func_call, (char *) &funcInfo,
			    xdr_arg_value, &funcReturn);

    free ((char *) argValue);

    if (status == RPC_SUCCESS)
	{
	/* XXX this assumes that vxWorks ALWAYS returns an int, and that
	 * XXX gdb isn't expecting anything more
	 */

	/*******************
	if (funcReturn.type == T_UNKNOWN)
	    return YYYXXX...;
	*******************/
	gdbValue = allocate_value (valueType);
	bcopy (&funcReturn.arg_value_u.v_int, VALUE_CONTENTS (gdbValue),
		sizeof (int));
        return gdbValue;
	}
    else 
	error (rpcerr);
    }
#endif /* FIXME */
 
/* Start an inferior process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.
   Returns process id.  Errors reported with error().
   On VxWorks, we ignore exec_file.  */
 
void
vx_create_inferior (exec_file, args, env)
     char *exec_file;
     char *args;
     char **env;
{
  enum clnt_stat status;
  arg_array passArgs;
  TASK_START taskStart;

  bzero ((char *) &passArgs, sizeof (passArgs));
  bzero ((char *) &taskStart, sizeof (taskStart));

  /* parse arguments, put them in passArgs */

  parse_args (args, &passArgs);

  if (passArgs.arg_array_len == 0)
    error ("You must specify a function name to run, and arguments if any");

  status = net_clnt_call (PROCESS_START, xdr_arg_array, &passArgs,
			  xdr_TASK_START, &taskStart);

  if ((status != RPC_SUCCESS) || (taskStart.status == -1))
    error ("Can't create process on remote target machine");

  /* Save the name of the running function */
  vx_running = savestring (passArgs.arg_array_val[0],
			   strlen (passArgs.arg_array_val[0]));

#ifdef CREATE_INFERIOR_HOOK
  CREATE_INFERIOR_HOOK (pid);
#endif  

  push_target (&vx_run_ops);
  inferior_pid = taskStart.pid;

#if defined (START_INFERIOR_HOOK)
  START_INFERIOR_HOOK ();
#endif

  /* We will get a trace trap after one instruction.
     Insert breakpoints and continue.  */

  init_wait_for_inferior ();

  /* Set up the "saved terminal modes" of the inferior
     based on what modes we are starting it with.  */
  target_terminal_init ();

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  /* remote_start(args); */
  /* trap_expected = 0; */
  stop_soon_quietly = 1;
  wait_for_inferior ();		/* Get the task spawn event */
  stop_soon_quietly = 0;

  /* insert_step_breakpoint ();  FIXME, do we need this?  */
  proceed(-1, -1, 0);
}

/* Fill ARGSTRUCT in argc/argv form with the arguments from the
   argument string ARGSTRING.  */

static void
parse_args (arg_string, arg_struct)
     register char *arg_string;
     arg_array *arg_struct;
{
  register int arg_count = 0;	/* number of arguments */
  register int arg_index = 0;
  register char *p0;
 
  bzero ((char *) arg_struct, sizeof (arg_array));
 
  /* first count how many arguments there are */

  p0 = arg_string;
  while (*p0 != '\0')
    {
      if (*(p0 = skip_white_space (p0)) == '\0')
	break;
      p0 = find_white_space (p0);
      arg_count++;
    }

  arg_struct->arg_array_len = arg_count;
  arg_struct->arg_array_val = (char **) xmalloc ((arg_count + 1)
						 * sizeof (char *));

  /* now copy argument strings into arg_struct.  */

  while (*(arg_string = skip_white_space (arg_string)))
    {
      p0 = find_white_space (arg_string);
      arg_struct->arg_array_val[arg_index++] = savestring (arg_string,
							   p0 - arg_string);
      arg_string = p0;
    }

  arg_struct->arg_array_val[arg_count] = NULL;
}

/* Advance a string pointer across whitespace and return a pointer
   to the first non-white character.  */

static char *
skip_white_space (p)
     register char *p;
{
  while (*p == ' ' || *p == '\t')
    p++;
  return p;
}
    
/* Search for the first unquoted whitespace character in a string.
   Returns a pointer to the character, or to the null terminator
   if no whitespace is found.  */

static char *
find_white_space (p)
     register char *p;
{
  register int c;

  while ((c = *p) != ' ' && c != '\t' && c)
    {
      if (c == '\'' || c == '"')
	{
	  while (*++p != c && *p)
	    {
	      if (*p == '\\')
		p++;
	    }
	  if (!*p)
	    break;
	}
      p++;
    }
  return p;
}
    
/* Poll the VxWorks target system for an event related
   to the debugged task.
   Returns -1 if remote wait failed, task status otherwise.  */

int
net_wait (pEvent)
    RDB_EVENT *pEvent;
{
    int pid;
    enum clnt_stat status;

    bzero ((char *) pEvent, sizeof (RDB_EVENT));

    pid = inferior_pid;
    status = net_clnt_call (PROCESS_WAIT, xdr_int, &pid, xdr_RDB_EVENT, pEvent);

    return (status == RPC_SUCCESS)? pEvent->status: -1;
}
    
/* Suspend the remote task.
   Returns -1 if suspend fails on target system, 0 otherwise.  */

int
net_quit ()
{
    int pid;
    int quit_status;
    enum clnt_stat status;

    quit_status = 0;

    /* don't let rdbTask suspend itself by passing a pid of 0 */

    if ((pid = inferior_pid) == 0)
	return -1;

    status = net_clnt_call (VX_TASK_SUSPEND, xdr_int, &pid, xdr_int,
			    &quit_status);

    return (status == RPC_SUCCESS)? quit_status: -1;
}

/* Read a register or registers from the remote system.  */

int
vx_read_register (regno)
     int regno;
{
  int status;
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];

  bzero ((char *) &ptrace_in, sizeof (ptrace_in));
  bzero ((char *) &ptrace_out, sizeof (ptrace_out));

  /* FIXME, eventually only get the ones we need.  */
  registers_fetched ();
  
  ptrace_in.pid = inferior_pid;
  ptrace_out.info.more_data = (caddr_t) &inferior_registers;
  status = net_ptrace_clnt_call (PTRACE_GETREGS, &ptrace_in, &ptrace_out);
  if (status)
    error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      return -1;
    }
  
#ifdef I80960

      bcopy ((char *) inferior_registers.r_lreg,
	     &registers[REGISTER_BYTE (R0_REGNUM)], 16 * sizeof (int));
      bcopy ((char *) inferior_registers.r_greg,
	     &registers[REGISTER_BYTE (G0_REGNUM)], 16 * sizeof (int));

      /* Don't assume that a location in registers[] is properly aligned.  */

      bcopy ((char *) &inferior_registers.r_pcw,
	     &registers[REGISTER_BYTE (PCW_REGNUM)], sizeof (int));
      bcopy ((char *) &inferior_registers.r_acw,
	     &registers[REGISTER_BYTE (ACW_REGNUM)], sizeof (int));
      bcopy ((char *) &inferior_registers.r_lreg[2],	/* r2 (RIP) -> IP */
	     &registers[REGISTER_BYTE (IP_REGNUM)], sizeof (int));
      bcopy ((char *) &inferior_registers.r_tcw,
	     &registers[REGISTER_BYTE (TCW_REGNUM)], sizeof (int));

      /* If the target has floating point registers, fetch them.
	 Otherwise, zero the floating point register values in
	 registers[] for good measure, even though we might not
	 need to.  */

      if (target_has_fp)
	{
	  ptrace_in.pid = inferior_pid;
	  ptrace_out.info.more_data = (caddr_t) &inferior_fp_registers;
	  status = net_ptrace_clnt_call (PTRACE_GETFPREGS, &ptrace_in, &ptrace_out);
	  if (status)
	    error (rpcerr);
	  if (ptrace_out.status == -1)
	    {
	      errno = ptrace_out.errno;
	      return -1;
	    }
	  
	  bcopy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
		 REGISTER_RAW_SIZE (FP0_REGNUM) * 4);
	}
      else
	{
	  bzero ((char *) &registers[REGISTER_BYTE (FP0_REGNUM)],
		 REGISTER_RAW_SIZE (FP0_REGNUM) * 4);
	}

#else  /* not 960, thus must be 68000:  FIXME!  */

  bcopy (&inferior_registers, registers, 16 * 4);
  *(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps;
  *(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;

  if (target_has_fp)
    {
      ptrace_in.pid = inferior_pid;
      ptrace_out.info.more_data = (caddr_t) &inferior_fp_registers;
      status = net_ptrace_clnt_call (PTRACE_GETFPREGS, &ptrace_in, &ptrace_out);
      if (status)
	error (rpcerr);
      if (ptrace_out.status == -1)
	{
	  errno = ptrace_out.errno;
	  return -1;
	}
      
      bcopy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	     sizeof inferior_fp_registers.fps_regs);
      bcopy (&inferior_fp_registers.fps_control,
	 &registers[REGISTER_BYTE (FPC_REGNUM)],
	 sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);
    }
  else
    {
      bzero (&registers[REGISTER_BYTE (FP0_REGNUM)],
	     sizeof inferior_fp_registers.fps_regs);
      bzero (&registers[REGISTER_BYTE (FPC_REGNUM)],
	 sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);
    }
#endif  /* various architectures */

  return 0;
}

/* Prepare to store registers.  Since we will store all of them,
   read out their current values now.  */

void
vx_prepare_to_store ()
{
  vx_read_register (-1);
}


/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */
   /* FIXME, look at REGNO to save time here */

vx_write_register (regno)
     int regno;
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];
  int status;
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;

  bzero ((char *) &ptrace_in, sizeof (ptrace_in));
  bzero ((char *) &ptrace_out, sizeof (ptrace_out));

#ifdef I80960

  bcopy (&registers[REGISTER_BYTE (R0_REGNUM)],
	 (char *) inferior_registers.r_lreg, 16 * sizeof (int));
  bcopy (&registers[REGISTER_BYTE (G0_REGNUM)],
	 (char *) inferior_registers.r_greg, 16 * sizeof (int));

  /* Don't assume that a location in registers[] is properly aligned.  */

  bcopy (&registers[REGISTER_BYTE (PCW_REGNUM)],
	 (char *) &inferior_registers.r_pcw, sizeof (int));
  bcopy (&registers[REGISTER_BYTE (ACW_REGNUM)],
	 (char *) &inferior_registers.r_acw, sizeof (int));
  bcopy (&registers[REGISTER_BYTE (TCW_REGNUM)],
	 (char *) &inferior_registers.r_tcw, sizeof (int));

#else  /* not 960 -- assume 68k -- FIXME */

  bcopy (registers, &inferior_registers, 16 * 4);
  inferior_registers.r_ps = *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
  inferior_registers.r_pc = *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];

#endif  /* Different register sets */

  ptrace_in.pid = inferior_pid;
  ptrace_in.info.ttype     = REGS;
  ptrace_in.info.more_data = (caddr_t) &inferior_registers;

  /* XXX change second param to be a proc number */
  status = net_ptrace_clnt_call (PTRACE_SETREGS, &ptrace_in, &ptrace_out);
  if (status)
      error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      return -1;
    }

  /* Store floating point registers if the target has them.  */

  if (target_has_fp)
    {
#ifdef I80960

      bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
	     sizeof inferior_fp_registers.fps_regs);

#else  /* not 960 -- assume 68k -- FIXME */

      bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
	     sizeof inferior_fp_registers.fps_regs);
      bcopy (&registers[REGISTER_BYTE (FPC_REGNUM)],
	 &inferior_fp_registers.fps_control,
         sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);

#endif  /* Different register sets */

      ptrace_in.pid = inferior_pid;
      ptrace_in.info.ttype     = FPREGS;
      ptrace_in.info.more_data = (caddr_t) &inferior_fp_registers;

      status = net_ptrace_clnt_call (PTRACE_SETFPREGS, &ptrace_in, &ptrace_out);
      if (status)
	  error (rpcerr);
      if (ptrace_out.status == -1)
	{
	  errno = ptrace_out.errno;
	  return -1;
	}
    }
  return 0;
}

/* Copy LEN bytes to or from remote inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  WRITE is true if writing to the
   inferior.
   Result is the number of bytes written or read (zero if error).  The
   protocol allows us to return a negative count, indicating that we can't
   handle the current address but can handle one N bytes further, but
   vxworks doesn't give us that information.  */

int
vx_xfer_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int status;
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;
  C_bytes data;

  bzero ((char *) &ptrace_in, sizeof (ptrace_in));
  bzero ((char *) &ptrace_out, sizeof (ptrace_out));

  ptrace_in.pid = inferior_pid;		/* XXX pid unnecessary for READDATA */
  ptrace_in.addr = (int) memaddr;	/* Where from */
  ptrace_in.data = len;			/* How many bytes */

  if (write)
    {
      ptrace_in.info.ttype     = DATA;
      ptrace_in.info.more_data = (caddr_t) &data;

      data.bytes = (caddr_t) myaddr;	/* Where from */
      data.len   = len;			/* How many bytes (again, for XDR) */

      /* XXX change second param to be a proc number */
      status = net_ptrace_clnt_call (PTRACE_WRITEDATA, &ptrace_in, &ptrace_out);
    }
  else
    {
      ptrace_out.info.more_data = (caddr_t) &data;
      data.bytes = myaddr;		/* Where to */
      data.len   = len;			/* How many (again, for XDR) */

      /* XXX change second param to be a proc number */
      status = net_ptrace_clnt_call (PTRACE_READDATA, &ptrace_in, &ptrace_out);
    }

  if (status)
      error (rpcerr);
  if (ptrace_out.status == -1)
    {
      return 0;		/* No bytes moved */
    }
  return len;		/* Moved *all* the bytes */
}

void
vx_files_info ()
{
  printf ("\tAttached to host `%s'", vx_host);
  printf (", which has %sfloating point", target_has_fp? "": "no ");
  printf (".\n");
}

void
vx_run_files_info ()
{
  printf ("\tRunning %s VxWorks process 0x%x", 
	  vx_running? "child": "attached",
	  inferior_pid);
  if (vx_running)
    printf (", function `%s'", vx_running);
  printf(".\n");
}

void
vx_resume (step, siggnal)
     int step;
     int siggnal;
{
  int status;
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;

  if (siggnal != 0 && siggnal != stop_signal)
    error ("Cannot send signals to VxWorks processes");

  bzero ((char *) &ptrace_in, sizeof (ptrace_in));
  bzero ((char *) &ptrace_out, sizeof (ptrace_out));

  ptrace_in.pid = inferior_pid;
  ptrace_in.addr = 1;	/* Target side insists on this, or it panics.  */

  /* XXX change second param to be a proc number */
  status = net_ptrace_clnt_call (step? PTRACE_SINGLESTEP: PTRACE_CONT,
				 &ptrace_in, &ptrace_out);
  if (status)
      error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      perror_with_name ("Resuming remote process");
    }
}

void
vx_mourn_inferior ()
{
  pop_target ();		/* Pop back to no-child state */
  generic_mourn_inferior ();
}


/* This function allows the addition of incrementally linked object files.  */

void
vx_load_command (arg_string, from_tty)
     char* arg_string;
     int from_tty;
{
  CORE_ADDR text_addr;
  CORE_ADDR data_addr;
  CORE_ADDR bss_addr;
  
  if (arg_string == 0)
    error ("The load command takes a file name");

  arg_string = tilde_expand (arg_string);
  make_cleanup (free, arg_string);

  dont_repeat ();

  QUIT;
  immediate_quit++;
  if (net_load (arg_string, &text_addr, &data_addr, &bss_addr) == -1)
    error ("Load failed on target machine");
  immediate_quit--;

  /* FIXME, for now we ignore data_addr and bss_addr.  */
  symbol_file_add (arg_string, from_tty, text_addr, 0);
}

#ifdef FIXME  /* Not ready for prime time */
/* Single step the target program at the source or machine level.
   Takes an error exit if rpc fails.
   Returns -1 if remote single-step operation fails, else 0.  */

static int
net_step ()
{
  enum clnt_stat status;
  int step_status;
  SOURCE_STEP source_step;

  source_step.taskId = inferior_pid;

  if (step_range_end)
    {
      source_step.startAddr = step_range_start;
      source_step.endAddr = step_range_end;
    }
  else
    {
      source_step.startAddr = 0;
      source_step.endAddr = 0;
    }

  status = net_clnt_call (VX_SOURCE_STEP, xdr_SOURCE_STEP, &source_step,
			  xdr_int, &step_status);

  if (status == RPC_SUCCESS)
    return step_status;
  else 
    error (rpcerr);
}
#endif

/* Emulate ptrace using RPC calls to the VxWorks target system.
   Returns nonzero (-1) if RPC status to VxWorks is bad, 0 otherwise.  */

static int
net_ptrace_clnt_call (request, pPtraceIn, pPtraceOut)
    enum ptracereq request;
    Rptrace *pPtraceIn;
    Ptrace_return *pPtraceOut;
{
  enum clnt_stat status;

  status = net_clnt_call (request, xdr_rptrace, pPtraceIn, xdr_ptrace_return,
			  pPtraceOut);

  if (status != RPC_SUCCESS)
      return -1;

  return 0;
}

/* Query the target for the name of the file from which VxWorks was
   booted.  pBootFile is the address of a pointer to the buffer to
   receive the file name; if the pointer pointed to by pBootFile is 
   NULL, memory for the buffer will be allocated by XDR.
   Returns -1 if rpc failed, 0 otherwise.  */

int
net_get_boot_file (pBootFile)
     char **pBootFile;
{
  enum clnt_stat status;

  status = net_clnt_call (VX_BOOT_FILE_INQ, xdr_void, (char *) 0,
			  xdr_wrapstring, pBootFile);
  return (status == RPC_SUCCESS) ? 0 : -1;
}

/* Fetch a list of loaded object modules from the VxWorks target.
   Returns -1 if rpc failed, 0 otherwise
   There's no way to check if the returned loadTable is correct.
   VxWorks doesn't check it.  */

int
net_get_symbols (pLoadTable)
     ldtabl *pLoadTable;		/* return pointer to ldtabl here */
{
  enum clnt_stat status;

  bzero ((char *) pLoadTable, sizeof (struct ldtabl));

  status = net_clnt_call (VX_STATE_INQ, xdr_void, 0, xdr_ldtabl, pLoadTable);
  return (status == RPC_SUCCESS) ? 0 : -1;
}

/* Look up a symbol in the VxWorks target's symbol table.
   Returns status of symbol read on target side (0=success, -1=fail)
   Returns -1 and complain()s if rpc fails.  */

struct complaint cant_contact_target =
  {"Lost contact with VxWorks target", 0, 0};

int
vx_lookup_symbol (name, pAddr)
     char *name;		/* symbol name */
     CORE_ADDR *pAddr;
{
  enum clnt_stat status;
  SYMBOL_ADDR symbolAddr;

  *pAddr = 0;
  bzero ((char *) &symbolAddr, sizeof (symbolAddr));

  status = net_clnt_call (VX_SYMBOL_INQ, xdr_wrapstring, &name,
			  xdr_SYMBOL_ADDR, &symbolAddr);
  if (status != RPC_SUCCESS) {
      complain (&cant_contact_target, 0);
      return -1;
  }

  *pAddr = symbolAddr.addr;
  return symbolAddr.status;
}

/* Check to see if the VxWorks target has a floating point coprocessor.
   Returns 1 if target has floating point processor, 0 otherwise.
   Calls error() if rpc fails.  */

int
net_check_for_fp ()
{
  enum clnt_stat status;
  bool_t fp = 0;	/* true if fp processor is present on target board */

  status = net_clnt_call (VX_FP_INQUIRE, xdr_void, 0, xdr_bool, &fp);
  if (status != RPC_SUCCESS)
      error (rpcerr);

   return (int) fp;
}

/* Establish an RPC connection with the VxWorks target system.
   Calls error () if unable to establish connection.  */

void
net_connect (host)
     char *host;
{
  struct sockaddr_in destAddr;
  struct hostent *destHost;

  /* get the internet address for the given host */

  if ((destHost = (struct hostent *) gethostbyname (host)) == NULL)
      error ("Invalid hostname.  Couldn't find remote host address.");

  bzero (&destAddr, sizeof (destAddr));

  destAddr.sin_addr.s_addr = * (u_long *) destHost->h_addr;
  destAddr.sin_family      = AF_INET;
  destAddr.sin_port        = 0;	/* set to actual port that remote
			           ptrace is listening on.  */

  /* Create a tcp client transport on which to issue
     calls to the remote ptrace server.  */

  ptraceSock = RPC_ANYSOCK;
  pClient = clnttcp_create (&destAddr, RDBPROG, RDBVERS, &ptraceSock, 0, 0);
  /* FIXME, here is where we deal with different version numbers of the proto */
  
  if (pClient == NULL)
    {
      clnt_pcreateerror ("\tnet_connect");
      error ("Couldn't connect to remote target.");
    }
}

/* Sleep for the specified number of milliseconds 
 * (assumed to be less than 1000).
 * If select () is interrupted, returns immediately;
 * takes an error exit if select () fails for some other reason.
 */

static void
sleep_ms (ms)
     long ms;
{
  struct timeval select_timeout;
  int status;

  select_timeout.tv_sec = 0;
  select_timeout.tv_usec = ms * 1000;

  status = select (0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &select_timeout);

  if (status < 0 && errno != EINTR)
    perror_with_name ("select");
}

/* Wait for control to return from inferior to debugger.
   If inferior gets a signal, we may decide to start it up again
   instead of returning.  That is why there is a loop in this function.
   When this function actually returns it means the inferior
   should be left stopped and GDB should read more commands.  */

/* For network debugging with VxWorks.
 * VxWorks knows when tasks hit breakpoints, receive signals, exit, etc,
 * so vx_wait() receives this information directly from
 * VxWorks instead of trying to figure out what happenned via a wait() call.
 */

static int
vx_wait (status)
     int *status;
{
  register int pid;
  WAITTYPE w;
  RDB_EVENT rdbEvent;
  int quit_failed;

  do
    {
      /* If CTRL-C is hit during this loop,
	 suspend the inferior process.  */

      quit_failed = 0;
      if (quit_flag)
	{
	  quit_failed = (net_quit () == -1);
	  quit_flag = 0;
	}

      /* If a net_quit () or net_wait () call has failed,
	 allow the user to break the connection with the target.
	 We can't simply error () out of this loop, since the 
	 data structures representing the state of the inferior
	 are in an inconsistent state.  */

      if (quit_failed || net_wait (&rdbEvent) == -1)
	{
	  terminal_ours ();
	  if (query ("Can't %s.  Disconnect from target system? ",
		     (quit_failed) ? "suspend remote task"
		                   : "get status of remote task"))
	    {
	      target_mourn_inferior();
	      error ("Use the \"target\" command to reconnect.");
	    }
	  else
	    {
	      terminal_inferior ();
	      continue;
	    }
	}
      
      pid = rdbEvent.taskId;
      if (pid == 0)
	{
	  sleep_ms (200);	/* FIXME Don't kill the network too badly */
	}
      else if (pid != inferior_pid)
	fatal ("Bad pid for debugged task: 0x%x\n", pid);
    } while (pid == 0);

  /* FIXME, eventually do more then SIGTRAP on everything...  */
  switch (rdbEvent.eventType)
    {
    case EVENT_EXIT:
      WSETEXIT (w, 0);
      /* FIXME is it possible to distinguish between a
	 XXX   normal vs abnormal exit in VxWorks? */
      break;

    case EVENT_START:		/* Task was just started. */
      WSETSTOP (w, SIGTRAP);
      break;

    case EVENT_STOP:
      WSETSTOP (w, SIGTRAP);
      /* XXX was it stopped by a signal?  act accordingly */
      break;

    case EVENT_BREAK:		/* Breakpoint was hit. */
      WSETSTOP (w, SIGTRAP);
      break;

    case EVENT_SUSPEND:		/* Task was suspended, probably by ^C. */
      WSETSTOP (w, SIGINT);
      break;

    case EVENT_BUS_ERR:		/* Task made evil nasty reference. */
      WSETSTOP (w, SIGBUS);
      break;

    case EVENT_ZERO_DIV:	/* Division by zero */
      WSETSTOP (w, SIGFPE);	/* Like Unix, call it a float exception. */

    case EVENT_SIGNAL:
      /* The target is not running Unix, and its
	 faults/traces do not map nicely into Unix signals.
	 Make sure they do not get confused with Unix signals
	 by numbering them with values higher than the highest
	 legal Unix signal.  code in the arch-dependent PRINT_RANDOM_SIGNAL
	 routine will interpret the value for wait_for_inferior.  */
      WSETSTOP (w, rdbEvent.sigType + NSIG);
      break;
    } /* switch */
  *status = *(int *)&w;		/* Grumble union wait crap Grumble */
  return pid;
}

static int
symbol_stub (arg)
     int arg;
{
  char *bootFile = (char *)arg;
  symbol_file_command (bootFile, 0);
  return 1;
}

static int
add_symbol_stub (arg)
     int arg;
{
  struct ldfile *pLoadFile = (struct ldfile *)arg;

  printf("\t%s: ", pLoadFile->name);
  symbol_file_add (pLoadFile->name, 0, pLoadFile->txt_addr, 0);
  printf ("ok\n");
  return 1;
}
/* Target command for VxWorks target systems.

   Used in vxgdb.  Takes the name of a remote target machine
   running vxWorks and connects to it to initialize remote network
   debugging.  */

static void
vx_open (args, from_tty)
     char *args;
     int from_tty;
{
  extern int close ();
  char *bootFile;
  extern char *source_path;
  struct ldtabl loadTable;
  struct ldfile *pLoadFile;
  int i;
  extern CLIENT *pClient;

  if (!args)
    error_no_arg ("target machine name");

  target_preopen (from_tty);
  
  unpush_target (&vx_ops);
  printf ("Attaching remote machine across net...\n");
  fflush (stdout);

  /* Allow the user to kill the connect attempt by typing ^C.
     Wait until the call to target_has_fp () completes before
     disallowing an immediate quit, since even if net_connect ()
     is successful, the remote debug server might be hung.  */

  immediate_quit++;

  net_connect (args);
  target_has_fp = net_check_for_fp ();
  printf_filtered ("Connected to %s.\n", args);

  immediate_quit--;

  push_target (&vx_ops);

  /* Save a copy of the target host's name.  */
  vx_host = savestring (args, strlen (args));

  /* Find out the name of the file from which the target was booted
     and load its symbol table.  */

  printf_filtered ("Looking in Unix path for all loaded modules:\n");
  bootFile = NULL;
  if (!net_get_boot_file (&bootFile))
    {
      if (*bootFile) {
	printf_filtered ("\t%s: ", bootFile);
	if (catch_errors (symbol_stub, (int)bootFile,
		"Error reading symbols from boot file"))
	  puts_filtered ("ok\n");
      } else if (from_tty)
	printf ("VxWorks kernel symbols not loaded.\n");
    }
  else
    error ("Can't retrieve boot file name from target machine.");

  clnt_freeres (pClient, xdr_wrapstring, &bootFile);

  if (net_get_symbols (&loadTable) != 0)
    error ("Can't read loaded modules from target machine");

  i = 0-1;
  while (++i < loadTable.tbl_size)
    {
      QUIT;	/* FIXME, avoids clnt_freeres below:  mem leak */
      pLoadFile = &loadTable.tbl_ent [i];
#ifdef WRS_ORIG
  {
    register int desc;
    struct cleanup *old_chain;
    char *fullname = NULL;

    desc = openp (source_path, 0, pLoadFile->name, O_RDONLY, 0, &fullname);
    if (desc < 0)
	perror_with_name (pLoadFile->name);
    old_chain = make_cleanup (close, desc);
    add_file_at_addr (fullname, desc, pLoadFile->txt_addr, pLoadFile->data_addr,
		      pLoadFile->bss_addr);
    do_cleanups (old_chain);
  }
#else
      /* Botches, FIXME:
	 (1)  Searches the PATH, not the source path.
	 (2)  data and bss are assumed to be at the usual offsets from text.  */
      catch_errors (add_symbol_stub, (int)pLoadFile, (char *)0);
#endif
    }
  printf_filtered ("Done.\n");

  clnt_freeres (pClient, xdr_ldtabl, &loadTable);
}

/* attach_command --
   takes a task started up outside of gdb and ``attaches'' to it.
   This stops it cold in its tracks and allows us to start tracing it.  */

static void
vx_attach (args, from_tty)
     char *args;
     int from_tty;
{
  int pid;
  char *cptr = 0;
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;
  int status;

  dont_repeat();

  if (!args)
    error_no_arg ("process-id to attach");

  pid = strtol (args, &cptr, 0);
  if ((cptr == args) || (*cptr != '\0'))
    error ("Invalid process-id -- give a single number in decimal or 0xhex");

  if (from_tty)
      printf ("Attaching pid 0x%x.\n", pid);

  bzero ((char *)&ptrace_in,  sizeof (ptrace_in));
  bzero ((char *)&ptrace_out, sizeof (ptrace_out));
  ptrace_in.pid = pid;

  status = net_ptrace_clnt_call (PTRACE_ATTACH, &ptrace_in, &ptrace_out);
  if (status == -1)
    error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      perror_with_name ("Attaching remote process");
    }

  /* It worked... */
  push_target (&vx_run_ops);
  inferior_pid = pid;
  vx_running = 0;

#if defined (START_INFERIOR_HOOK)
  START_INFERIOR_HOOK ();
#endif

  mark_breakpoints_out ();

  /* Set up the "saved terminal modes" of the inferior
     based on what modes we are starting it with.  */
  target_terminal_init ();

  /* Install inferior's terminal modes.  */
  target_terminal_inferior ();

  /* We will get a task spawn event immediately.  */
  init_wait_for_inferior ();
  clear_proceed_status ();
  stop_soon_quietly = 1;
  wait_for_inferior ();
  stop_soon_quietly = 0;
  normal_stop ();
}


/* detach_command --
   takes a program previously attached to and detaches it.
   The program resumes execution and will no longer stop
   on signals, etc.  We better not have left any breakpoints
   in the program or it'll die when it hits one.  For this
   to work, it may be necessary for the process to have been
   previously attached.  It *might* work if the program was
   started via the normal ptrace (PTRACE_TRACEME).  */

static void
vx_detach (args, from_tty)
     char *args;
     int from_tty;
{
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;
  int signal = 0;
  int status;

  if (args)
    error ("Argument given to VxWorks \"detach\".");

  if (from_tty)
      printf ("Detaching pid 0x%x.\n", inferior_pid);

  if (args)		/* FIXME, should be possible to leave suspended */
    signal = atoi (args);
  
  bzero ((char *)&ptrace_in,  sizeof (ptrace_in));
  bzero ((char *)&ptrace_out, sizeof (ptrace_out));
  ptrace_in.pid = inferior_pid;

  status = net_ptrace_clnt_call (PTRACE_DETACH, &ptrace_in, &ptrace_out);
  if (status == -1)
    error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      perror_with_name ("Detaching VxWorks process");
    }

  inferior_pid = 0;
  pop_target ();	/* go back to non-executing VxWorks connection */
}

/* vx_kill -- takes a running task and wipes it out.  */

static void
vx_kill (args, from_tty)
     char *args;
     int from_tty;
{
  Rptrace ptrace_in;
  Ptrace_return ptrace_out;
  int status;

  if (args)
    error ("Argument given to VxWorks \"kill\".");

  if (from_tty)
      printf ("Killing pid 0x%x.\n", inferior_pid);

  bzero ((char *)&ptrace_in,  sizeof (ptrace_in));
  bzero ((char *)&ptrace_out, sizeof (ptrace_out));
  ptrace_in.pid = inferior_pid;

  status = net_ptrace_clnt_call (PTRACE_KILL, &ptrace_in, &ptrace_out);
  if (status == -1)
    error (rpcerr);
  if (ptrace_out.status == -1)
    {
      errno = ptrace_out.errno;
      perror_with_name ("Killing VxWorks process");
    }

  /* If it gives good status, the process is *gone*, no events remain.  */
  inferior_pid = 0;
  pop_target ();	/* go back to non-executing VxWorks connection */
}

/* Clean up from the VxWorks process target as it goes away.  */

void
vx_proc_close (quitting)
     int quitting;
{
  inferior_pid = 0;		/* No longer have a process.  */
  if (vx_running)
    free (vx_running);
  vx_running = 0;
}

/* Cross-net conversion of floats to and from extended form.
   (This is needed because different target machines have different
    extended floating point formats.)  */

/* Convert from an extended float to a double.

   The extended float is stored as raw data pointed to by FROM.
   Return the converted value as raw data in the double pointed to by TO.
*/

static void
vx_convert_to_virtual (regno, from, to)
    int regno;
    char *from;
    char *to;
{
  enum clnt_stat status;

  if (REGISTER_CONVERTIBLE (regno)) 
    {
      if (!target_has_fp) {
	*(double *)to = 0.0;	/* Skip the trouble if no float anyway */
	return;
      }

      status = net_clnt_call (VX_CONV_FROM_68881, xdr_ext_fp, from,
			      xdr_double, to);

      if (status == RPC_SUCCESS)
	  return;
      else
	  error (rpcerr);
    }
  else
    bcopy (from, to, REGISTER_VIRTUAL_SIZE (regno));
}


/* The converse:  convert from a double to an extended float.

   The double is stored as raw data pointed to by FROM.
   Return the converted value as raw data in the extended
   float pointed to by TO.
*/

static void
vx_convert_from_virtual (regno, from, to)
    int regno;
    char *from;
    char *to;
{
  enum clnt_stat status;

  if (REGISTER_CONVERTIBLE (regno)) 
    {
      if (!target_has_fp) {
	bzero (to, REGISTER_RAW_SIZE (FP0_REGNUM));	/* Shrug */
	return;
      }

      status = net_clnt_call (VX_CONV_TO_68881, xdr_double, from,
			      xdr_ext_fp, to);
      if (status == RPC_SUCCESS)
	  return;
      else
	  error (rpcerr);
    }
  else
    bcopy (from, to, REGISTER_VIRTUAL_SIZE (regno));
}

/* Make an RPC call to the VxWorks target.
   Returns RPC status.  */

static enum clnt_stat
net_clnt_call (procNum, inProc, in, outProc, out)
    enum ptracereq procNum;
    xdrproc_t inProc;
    char *in;
    xdrproc_t outProc;
    char *out;
{
  enum clnt_stat status;
  
  status = clnt_call (pClient, procNum, inProc, in, outProc, out, rpcTimeout);

  if (status != RPC_SUCCESS)
      clnt_perrno (status);

  return status;
}

/* Clean up before losing control.  */

void
vx_close (quitting)
     int quitting;
{
  if (pClient)
    clnt_destroy (pClient);	/* The net connection */
  pClient = 0;

  if (vx_host)
    free (vx_host);		/* The hostname */
  vx_host = 0;
}

/* A vxprocess target should be started via "run" not "target".  */
/*ARGSUSED*/
static void
vx_proc_open (name, from_tty)
     char *name;
     int from_tty;
{
  error ("Use the \"run\" command to start a VxWorks process.");
}

/* Target ops structure for accessing memory and such over the net */

struct target_ops vx_ops = {
	"vxworks", "VxWorks target memory via RPC over TCP/IP",
	"Use VxWorks target memory.  \n\
Specify the name of the machine to connect to.",
	vx_open, vx_close, vx_attach, 0, /* vx_detach, */
	0, 0, /* resume, wait */
	0, 0, /* read_reg, write_reg */
	0, vx_convert_to_virtual, vx_convert_from_virtual,  /* prep_to_store, */
	vx_xfer_memory, vx_files_info,
	0, 0, /* insert_breakpoint, remove_breakpoint */
	0, 0, 0, 0, 0,	/* terminal stuff */
	0, /* vx_kill, */
	vx_load_command, add_syms_addr_command,
	0,  /* call_function */
	vx_lookup_symbol,
	vx_create_inferior, 0,  /* mourn_inferior */
	core_stratum, 0, /* next */
	1, 1, 0, 0, 0,	/* all mem, mem, stack, regs, exec */
	OPS_MAGIC,		/* Always the last thing */
};

/* Target ops structure for accessing VxWorks child processes over the net */

struct target_ops vx_run_ops = {
	"vxprocess", "VxWorks process",
	"VxWorks process, started by the \"run\" command.",
	vx_proc_open, vx_proc_close, 0, vx_detach, /* vx_attach */
	vx_resume, vx_wait,
	vx_read_register, vx_write_register,
	vx_prepare_to_store, vx_convert_to_virtual, vx_convert_from_virtual,
	vx_xfer_memory, vx_run_files_info,
	vx_insert_breakpoint, vx_remove_breakpoint,
	0, 0, 0, 0, 0,	/* terminal stuff */
	vx_kill,
	vx_load_command, add_syms_addr_command,
	call_function_by_hand,  /* FIXME, calling fns is maybe botched? */
	vx_lookup_symbol,
	0, vx_mourn_inferior,
	process_stratum, 0, /* next */
	0, 1, 1, 1, 1,	/* all mem, mem, stack, regs, exec */
			/* all_mem is off to avoid spurious msg in "i files" */
	OPS_MAGIC,		/* Always the last thing */
};
/* ==> Remember when reading at end of file, there are two "ops" structs here. */

void
_initialize_vx ()
{
  add_target (&vx_ops);
  add_target (&vx_run_ops);
}
