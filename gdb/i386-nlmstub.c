/* i386-nlmstub.c -- NLM debugging stub for the i386.

   This is originally based on an m68k software stub written by Glenn
   Engel at HP, but has changed quite a bit.  It was modified for the
   i386 by Jim Kingdon, Cygnus Support.  It was modified to run under
   NetWare by Ian Lance Taylor, Cygnus Support.

   This code is intended to produce an NLM (a NetWare Loadable Module)
   to run under NetWare on an i386 platform.  To create the NLM,
   compile this code into an object file using the NLM SDK on any i386
   host, and use the nlmconv program (available in the GNU binutils)
   to transform the resulting object file into an NLM.  */

/****************************************************************************

		THIS SOFTWARE IS NOT COPYRIGHTED

   HP offers the following for use in the public domain.  HP makes no
   warranty with regard to the software or it's performance and the
   user accepts the software "AS IS" with all faults.

   HP DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
   TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

****************************************************************************/

/****************************************************************************
 *
 *    The following gdb commands are supported:
 *
 * command          function                               Return value
 *
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 *
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 *
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 *
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 *
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 *
 * All commands and responses are sent with a packet which includes a
 * checksum.  A packet consists of
 *
 * $<packet info>#<checksum>.
 *
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 *
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 *
 * Example:
 *
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 *
 ****************************************************************************/

#include <dfs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <aio.h>
#include <conio.h>
#include <advanced.h>
#include <debugapi.h>
#include <process.h>

/****************************************************/
/* This information is from Novell.  It is not in any of the standard
   NetWare header files.  */

struct DBG_LoadDefinitionStructure
{
	void *reserved1[4];
	LONG reserved5;
	LONG LDCodeImageOffset;
	LONG LDCodeImageLength;
	LONG LDDataImageOffset;
	LONG LDDataImageLength;
	LONG LDUninitializedDataLength;
	LONG LDCustomDataOffset;
	LONG LDCustomDataSize;
	LONG reserved6[2];
	LONG (*LDInitializationProcedure)(void);
};

#define LO_NORMAL		0x0000
#define LO_STARTUP		0x0001
#define LO_PROTECT		0x0002
#define LO_DEBUG		0x0004
#define LO_AUTO_LOAD  		0x0008

/* Loader returned error codes */
#define LOAD_COULD_NOT_FIND_FILE			1
#define LOAD_ERROR_READING_FILE				2
#define LOAD_NOT_NLM_FILE_FORMAT			3
#define LOAD_WRONG_NLM_FILE_VERSION			4
#define LOAD_REENTRANT_INITIALIZE_FAILURE	5
#define LOAD_CAN_NOT_LOAD_MULTIPLE_COPIES	6
#define LOAD_ALREADY_IN_PROGRESS			7
#define LOAD_NOT_ENOUGH_MEMORY				8
#define LOAD_INITIALIZE_FAILURE				9
#define LOAD_INCONSISTENT_FILE_FORMAT		10
#define LOAD_CAN_NOT_LOAD_AT_STARTUP		11
#define LOAD_AUTO_LOAD_MODULES_NOT_LOADED	12
#define LOAD_UNRESOLVED_EXTERNAL			13
#define LOAD_PUBLIC_ALREADY_DEFINED			14
/****************************************************/

/* The main thread ID.  */
static int mainthread;

/* The LoadDefinitionStructure of the NLM being debugged.  */
static struct DBG_LoadDefinitionStructure *handle;

/* Whether we have connected to gdb.  */
static int talking;

/* The actual first instruction in the program.  */
static unsigned char first_insn;

/* An error message for the main thread to print.  */
static char *error_message;

/* The AIO port handle.  */
static int AIOhandle;

/* The console screen.  */
static int console_screen;

/* BUFMAX defines the maximum number of characters in inbound/outbound
   buffers.  At least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 400

/* remote_debug > 0 prints ill-formed commands in valid packets and
   checksum errors. */
static int remote_debug = 1;

static const char hexchars[] = "0123456789abcdef";

/* Number of bytes of registers.  */
#define NUMREGBYTES 64
enum regnames {EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	       PC /* also known as eip */,
	       PS /* also known as eflags */,
	       CS, SS, DS, ES, FS, GS};

/* Register values.  */
static int registers[NUMREGBYTES/4];

/* Read a character from the serial port.  This must busy wait, but
   that's OK because we will be the only thread running anyhow.  */

static int
getDebugChar ()
{
  int err;
  LONG got;
  unsigned char ret;

  do
    {
      err = AIOReadData (AIOhandle, (char *) &ret, 1, &got);
      if (err != 0)
	{
	  error_message = "AIOReadData failed";
	  ResumeThread (mainthread);
	  return -1;
	}
    }
  while (got == 0);

  return ret;
}

/* Write a character to the serial port.  Returns 0 on failure,
   non-zero on success.  */

static int
putDebugChar (c)
     unsigned char c;
{
  int err;
  LONG put;

  err = AIOWriteData (AIOhandle, (char *) &c, 1, &put);
  if (err != 0 || put != 1)
    {
      error_message = "AIOWriteData failed";
      ResumeThread (mainthread);
      return 0;
    }
  return 1;
}

/* Get the registers out of the frame information.  */

static void
frame_to_registers (frame, regs)
     T_TSS_StackFrame *frame;
     int *regs;
{
  regs[EAX] = frame->ExceptionEAX;
  regs[ECX] = frame->ExceptionECX;
  regs[EDX] = frame->ExceptionEDX;
  regs[EBX] = frame->ExceptionEBX;
  regs[ESP] = frame->ExceptionESP;
  regs[EBP] = frame->ExceptionEBP;
  regs[ESI] = frame->ExceptionESI;
  regs[EDI] = frame->ExceptionEDI;
  regs[PC] = frame->ExceptionEIP;
  regs[PS] = frame->ExceptionSystemFlags;
  regs[CS] = frame->ExceptionCS[0];
  regs[SS] = frame->ExceptionSS[0];
  regs[DS] = frame->ExceptionDS[0];
  regs[ES] = frame->ExceptionES[0];
  regs[FS] = frame->ExceptionFS[0];
  regs[GS] = frame->ExceptionGS[0];
}

/* Put the registers back into the frame information.  */

static void
registers_to_frame (regs, frame)
     int *regs;
     T_TSS_StackFrame *frame;
{
  frame->ExceptionEAX = regs[EAX];
  frame->ExceptionECX = regs[ECX];
  frame->ExceptionEDX = regs[EDX];
  frame->ExceptionEBX = regs[EBX];
  frame->ExceptionESP = regs[ESP];
  frame->ExceptionEBP = regs[EBP];
  frame->ExceptionESI = regs[ESI];
  frame->ExceptionEDI = regs[EDI];
  frame->ExceptionEIP = regs[PC];
  frame->ExceptionSystemFlags = regs[PS];
  frame->ExceptionCS[0] = regs[CS];
  frame->ExceptionSS[0] = regs[SS];
  frame->ExceptionDS[0] = regs[DS];
  frame->ExceptionES[0] = regs[ES];
  frame->ExceptionFS[0] = regs[FS];
  frame->ExceptionGS[0] = regs[GS];
}

/* Turn a hex character into a number.  */

static int
hex (ch)
     char ch;
{
  if ((ch >= 'a') && (ch <= 'f'))
    return (ch-'a'+10);
  if ((ch >= '0') && (ch <= '9'))
    return (ch-'0');
  if ((ch >= 'A') && (ch <= 'F'))
    return (ch-'A'+10);
  return (-1);
}

/* Scan for the sequence $<data>#<checksum>.  Returns 0 on failure,
   non-zero on success.  */

static int
getpacket (buffer)
     char * buffer;
{
  unsigned char checksum;
  unsigned char xmitcsum;
  int i;
  int count;
  int ch;

  do
    {
      /* wait around for the start character, ignore all other characters */
      while ((ch = getDebugChar()) != '$')
	if (ch == -1)
	  return 0;
      checksum = 0;
      xmitcsum = -1;

      count = 0;

      /* now, read until a # or end of buffer is found */
      while (count < BUFMAX)
	{
	  ch = getDebugChar();
	  if (ch == -1)
	    return 0;
	  if (ch == '#')
	    break;
	  checksum = checksum + ch;
	  buffer[count] = ch;
	  count = count + 1;
	}
      buffer[count] = 0;

      if (ch == '#')
	{
	  ch = getDebugChar ();
	  if (ch == -1)
	    return 0;
	  xmitcsum = hex(ch) << 4;
	  ch = getDebugChar ();
	  if (ch == -1)
	    return 0;
	  xmitcsum += hex(ch);
	  if ((remote_debug ) && (checksum != xmitcsum))
	    {
	      fprintf(stderr,"bad checksum.  My count = 0x%x, sent=0x%x. buf=%s\n",
		      checksum,xmitcsum,buffer);
	    }

	  if (checksum != xmitcsum)
	    {
	      /* failed checksum */
	      if (! putDebugChar('-'))
		return 0;
	    }
	  else
	    {
	      /* successful transfer */
	      if (! putDebugChar('+'))
		return 0;
	      /* if a sequence char is present, reply the sequence ID */
	      if (buffer[2] == ':')
		{
		  if (! putDebugChar (buffer[0])
		      || ! putDebugChar (buffer[1]))
		    return 0;
		  /* remove sequence chars from buffer */
		  count = strlen(buffer);
		  for (i=3; i <= count; i++)
		    buffer[i-3] = buffer[i];
		}
	    }
	}
    }
  while (checksum != xmitcsum);

  if (remote_debug)
    ConsolePrintf ("Received packet \"%s\"\r\n", buffer);

  return 1;
}

/* Send the packet in buffer.  Returns 0 on failure, non-zero on
   success.  */

static int
putpacket (buffer)
     char * buffer;
{
  unsigned char checksum;
  int count;
  int ch;

  if (remote_debug)
    ConsolePrintf ("Sending packet \"%s\"\r\n", buffer);

  /*  $<packet info>#<checksum>. */
  do
    {
      if (! putDebugChar('$'))
	return 0;
      checksum = 0;
      count = 0;

      while (ch=buffer[count])
	{
	  if (! putDebugChar(ch))
	    return 0;
	  checksum += ch;
	  count += 1;
	}

      if (! putDebugChar('#')
	  || ! putDebugChar(hexchars[checksum >> 4])
	  || ! putDebugChar(hexchars[checksum % 16]))
	return 0;

      ch = getDebugChar ();
      if (ch == -1)
	return 0;
    }
  while (ch != '+');

  return 1;
}

static char remcomInBuffer[BUFMAX];
static char remcomOutBuffer[BUFMAX];
static short error;

static void
debug_error (format, parm)
     char *format;
     char *parm;
{
  if (remote_debug)
    {
      fprintf (stderr, format, parm);
      fprintf (stderr, "\n");
    }
}

/* This is set if we could get a memory access fault.  */
static int mem_may_fault;

/* Indicate to caller of mem2hex or hex2mem that there has been an
   error.  */
static volatile int mem_err = 0;

/* These are separate functions so that they are so short and sweet
   that the compiler won't save any registers (if there is a fault
   to mem_fault, they won't get restored, so there better not be any
   saved).  */

static int
get_char (addr)
     char *addr;
{
  return *addr;
}

static void
set_char (addr, val)
     char *addr;
     int val;
{
  *addr = val;
}

/* This bit of assembly language just returns from a function.  If a
   memory error occurs within get_char or set_char, the debugger
   handler points EIP at these instructions to get out.  */

extern void just_return ();
asm (".globl just_return");
asm (".globl _just_return");
asm ("just_return:");
asm ("_just_return:");
asm ("leave");
asm ("ret");

/* convert the memory pointed to by mem into hex, placing result in buf */
/* return a pointer to the last char put in buf (null) */
/* If MAY_FAULT is non-zero, then we should set mem_err in response to
   a fault; if zero treat a fault like any other fault in the stub.  */

static char *
mem2hex (mem, buf, count, may_fault)
     char *mem;
     char *buf;
     int count;
     int may_fault;
{
  int i;
  unsigned char ch;

  mem_may_fault = may_fault;
  for (i = 0; i < count; i++)
    {
      ch = get_char (mem++);
      if (may_fault && mem_err)
	return (buf);
      *buf++ = hexchars[ch >> 4];
      *buf++ = hexchars[ch % 16];
    }
  *buf = 0;
  mem_may_fault = 0;
  return(buf);
}

/* convert the hex array pointed to by buf into binary to be placed in mem */
/* return a pointer to the character AFTER the last byte written */

static char *
hex2mem (buf, mem, count, may_fault)
     char *buf;
     char *mem;
     int count;
     int may_fault;
{
  int i;
  unsigned char ch;

  mem_may_fault = may_fault;
  for (i=0;i<count;i++)
    {
      ch = hex(*buf++) << 4;
      ch = ch + hex(*buf++);
      set_char (mem++, ch);
      if (may_fault && mem_err)
	return (mem);
    }
  mem_may_fault = 0;
  return(mem);
}

/* This function takes the 386 exception vector and attempts to
   translate this number into a unix compatible signal value.  */

static int
computeSignal (exceptionVector)
     int exceptionVector;
{
  int sigval;
  switch (exceptionVector)
    {
    case 0 : sigval = 8; break; /* divide by zero */
    case 1 : sigval = 5; break; /* debug exception */
    case 3 : sigval = 5; break; /* breakpoint */
    case 4 : sigval = 16; break; /* into instruction (overflow) */
    case 5 : sigval = 16; break; /* bound instruction */
    case 6 : sigval = 4; break; /* Invalid opcode */
    case 7 : sigval = 8; break; /* coprocessor not available */
    case 8 : sigval = 7; break; /* double fault */
    case 9 : sigval = 11; break; /* coprocessor segment overrun */
    case 10 : sigval = 11; break; /* Invalid TSS */
    case 11 : sigval = 11; break; /* Segment not present */
    case 12 : sigval = 11; break; /* stack exception */
    case 13 : sigval = 11; break; /* general protection */
    case 14 : sigval = 11; break; /* page fault */
    case 16 : sigval = 7; break; /* coprocessor error */
    default:
      sigval = 7;		/* "software generated"*/
    }
  return (sigval);
}

/**********************************************/
/* WHILE WE FIND NICE HEX CHARS, BUILD AN INT */
/* RETURN NUMBER OF CHARS PROCESSED           */
/**********************************************/
static int
hexToInt(ptr, intValue)
     char **ptr;
     int *intValue;
{
  int numChars = 0;
  int hexValue;

  *intValue = 0;

  while (**ptr)
    {
      hexValue = hex(**ptr);
      if (hexValue >=0)
	{
	  *intValue = (*intValue <<4) | hexValue;
	  numChars ++;
	}
      else
	break;

      (*ptr)++;
    }

  return (numChars);
}

/* This function does all command processing for interfacing to gdb.
   It is called whenever an exception occurs in the module being
   debugged.  */

static LONG
handle_exception (T_StackFrame *old_frame)
{
  T_TSS_StackFrame *frame = (T_TSS_StackFrame *) old_frame;
  int sigval;
  int addr, length;
  char * ptr;
  int newPC;

  /* Apparently the bell can sometimes be ringing at this point, and
     should be stopped.  */
  StopBell ();

  if (remote_debug)
    {
      ConsolePrintf ("vector=%d: %s, sr=0x%x, pc=0x%x, thread=%d\r\n",
		     frame->ExceptionNumber,
		     frame->ExceptionDescription,
		     frame->ExceptionSystemFlags,
		     frame->ExceptionEIP,
		     GetThreadID ());
    }

  /* If the NLM just started, we record the module load information
     and the thread ID, and set a breakpoint at the first instruction
     in the program.  */
  if (frame->ExceptionNumber == START_NLM_EVENT
      && handle == NULL)
    {
      handle = ((struct DBG_LoadDefinitionStructure *)
		frame->ExceptionErrorCode);
      first_insn = *(char *) handle->LDInitializationProcedure;
      *(unsigned char *) handle->LDInitializationProcedure = 0xcc;
      return RETURN_TO_PROGRAM;
    }

  /* After we've reached the initial breakpoint, reset it.  */
  if (frame->ExceptionEIP == (LONG) handle->LDInitializationProcedure + 1
      && *(unsigned char *) handle->LDInitializationProcedure == 0xcc)
    {
      *(char *) handle->LDInitializationProcedure = first_insn;
      frame->ExceptionEIP = (LONG) handle->LDInitializationProcedure;
    }

  /* Pass some events on to the next debugger, in case it will handle
     them.  */
  if (frame->ExceptionNumber == ENTER_DEBUGGER_EVENT
      || frame->ExceptionNumber == KEYBOARD_BREAK_EVENT)
    return RETURN_TO_NEXT_DEBUGGER;

  /* At the moment, we don't care about most of the unusual NetWare
     exceptions.  */
  if (frame->ExceptionNumber != TERMINATE_NLM_EVENT
      && frame->ExceptionNumber > 31)
    return RETURN_TO_PROGRAM;

  /* If we get a GP fault, and mem_may_fault is set, and the
     instruction pointer is near set_char or get_char, then we caused
     the fault ourselves accessing an illegal memory location.  */
  if (mem_may_fault
      && (frame->ExceptionNumber == 11
	  || frame->ExceptionNumber == 13
	  || frame->ExceptionNumber == 14)
      && ((frame->ExceptionEIP >= (long) &set_char
	   && frame->ExceptionEIP < (long) &set_char + 50)
	  || (frame->ExceptionEIP >= (long) &get_char
	      && frame->ExceptionEIP < (long) &get_char + 50)))
    {
      mem_err = 1;
      /* Point the instruction pointer at an assembly language stub
	 which just returns from the function.  */
      frame->ExceptionEIP = (long) &just_return;
      /* Keep going.  This will act as though it returned from
	 set_char or get_char.  The calling routine will check
	 mem_err, and do the right thing.  */
      return RETURN_TO_PROGRAM;
    }

  /* FIXME: How do we know that this exception has anything to do with
     the program we are debugging?  We can check whether the PC is in
     the range of the module we are debugging, but that doesn't help
     much since an error could occur in a library routine.  */

  frame_to_registers (frame, registers);

  /* reply to host that an exception has occurred */
  if (frame->ExceptionNumber == TERMINATE_NLM_EVENT)
    {
      /* There is no way to get the exit status.  */
      remcomOutBuffer[0] = 'W';
      remcomOutBuffer[1] = hexchars[0];
      remcomOutBuffer[2] = hexchars[0];
      remcomOutBuffer[3] = 0;
    }
  else
    {
      sigval = computeSignal (frame->ExceptionNumber);
      remcomOutBuffer[0] = 'N';
      remcomOutBuffer[1] =  hexchars[sigval >> 4];
      remcomOutBuffer[2] =  hexchars[sigval % 16];
      sprintf (remcomOutBuffer + 3, "0x%x;0x%x;0x%x",
	       handle->LDCodeImageOffset,
	       handle->LDDataImageOffset,
	       handle->LDDataImageOffset + handle->LDDataImageLength);
    }

  if (! putpacket(remcomOutBuffer))
    return RETURN_TO_NEXT_DEBUGGER;

  if (frame->ExceptionNumber == TERMINATE_NLM_EVENT)
    {
      ResumeThread (mainthread);
      return RETURN_TO_PROGRAM;
    }

  while (1)
    {
      error = 0;
      remcomOutBuffer[0] = 0;
      if (! getpacket (remcomInBuffer))
	return RETURN_TO_NEXT_DEBUGGER;
      talking = 1;
      switch (remcomInBuffer[0])
	{
	case '?':
	  sigval = computeSignal (frame->ExceptionNumber);
	  remcomOutBuffer[0] = 'N';
	  remcomOutBuffer[1] =  hexchars[sigval >> 4];
	  remcomOutBuffer[2] =  hexchars[sigval % 16];
	  sprintf (remcomOutBuffer + 3, "0x%x;0x%x;0x%x",
		   handle->LDCodeImageOffset,
		   handle->LDDataImageOffset,
		   handle->LDDataImageOffset + handle->LDDataImageLength);
	  break;
	case 'd':
	  remote_debug = !(remote_debug); /* toggle debug flag */
	  break;
	case 'g':
	  /* return the value of the CPU registers */
	  mem2hex((char*) registers, remcomOutBuffer, NUMREGBYTES, 0);
	  break;
	case 'G':
	  /* set the value of the CPU registers - return OK */
	  hex2mem(&remcomInBuffer[1], (char*) registers, NUMREGBYTES, 0);
	  strcpy(remcomOutBuffer,"OK");
	  break;

	case 'm':
	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
	  /* TRY TO READ %x,%x.  IF SUCCEED, SET PTR = 0 */
	  ptr = &remcomInBuffer[1];
	  if (hexToInt(&ptr,&addr))
	    if (*(ptr++) == ',')
	      if (hexToInt(&ptr,&length))
		{
		  ptr = 0;
		  mem_err = 0;
		  mem2hex((char*) addr, remcomOutBuffer, length, 1);
		  if (mem_err)
		    {
		      strcpy (remcomOutBuffer, "E03");
		      debug_error ("memory fault");
		    }
		}

	  if (ptr)
	    {
	      strcpy(remcomOutBuffer,"E01");
	      debug_error("malformed read memory command: %s",remcomInBuffer);
	    }
	  break;

	case 'M':
	  /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
	  /* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */
	  ptr = &remcomInBuffer[1];
	  if (hexToInt(&ptr,&addr))
	    if (*(ptr++) == ',')
	      if (hexToInt(&ptr,&length))
		if (*(ptr++) == ':')
		  {
		    mem_err = 0;
		    hex2mem(ptr, (char*) addr, length, 1);

		    if (mem_err)
		      {
			strcpy (remcomOutBuffer, "E03");
			debug_error ("memory fault");
		      }
		    else
		      {
			strcpy(remcomOutBuffer,"OK");
		      }

		    ptr = 0;
		  }
	  if (ptr)
	    {
	      strcpy(remcomOutBuffer,"E02");
	      debug_error("malformed write memory command: %s",remcomInBuffer);
	    }
	  break;

	case 'c':
	case 's':
	  /* cAA..AA    Continue at address AA..AA(optional) */
	  /* sAA..AA   Step one instruction from AA..AA(optional) */
	  /* try to read optional parameter, pc unchanged if no parm */
	  ptr = &remcomInBuffer[1];
	  if (hexToInt(&ptr,&addr))
	    registers[ PC ] = addr;

	  newPC = registers[ PC];

	  /* clear the trace bit */
	  registers[ PS ] &= 0xfffffeff;

	  /* set the trace bit if we're stepping */
	  if (remcomInBuffer[0] == 's') registers[ PS ] |= 0x100;

	  registers_to_frame (registers, frame);
	  return RETURN_TO_PROGRAM;

	case 'k':
	  /* kill the program */
	  KillMe (handle);
	  ResumeThread (mainthread);
	  return RETURN_TO_PROGRAM;
	}

      /* reply to the request */
      if (! putpacket(remcomOutBuffer))
	return RETURN_TO_NEXT_DEBUGGER;
    }
}

/* Start up.  The main thread opens the named serial I/O port, loads
   the named NLM module and then goes to sleep.  The serial I/O port
   is named as a board number and a port number.  It would be more DOS
   like to provide a menu of available serial ports, but I don't want
   to have to figure out how to do that.  */

int
main (argc, argv)
     int argc;
     char **argv;
{
  int hardware, board, port;
  LONG err;
  struct debuggerStructure s;
  char *cmdlin;
  int i;

  /* Create a screen for the debugger.  */
  console_screen = CreateScreen ("System Console", 0);
  if (DisplayScreen (console_screen) != ESUCCESS)
    fprintf (stderr, "DisplayScreen failed\n");

  if (argc < 4)
    {
      fprintf (stderr,
	       "Usage: load gdbserver board port program [arguments]\n");
      exit (1);
    }

  hardware = -1;
  board = strtol (argv[1], (char **) NULL, 0);
  port = strtol (argv[2], (char **) NULL, 0);

  err = AIOAcquirePort (&hardware, &board, &port, &AIOhandle);
  if (err != AIO_SUCCESS)
    {
      switch (err)
	{
	case AIO_PORT_NOT_AVAILABLE:
	  fprintf (stderr, "Port not available\n");
	  break;

	case AIO_BOARD_NUMBER_INVALID:
	case AIO_PORT_NUMBER_INVALID:
	  fprintf (stderr, "No such port\n");
	  break;

	default:
	  fprintf (stderr, "Could not open port: %d\n", err);
	  break;
	}

      exit (1);
    }

  err = AIOConfigurePort (AIOhandle, AIO_BAUD_9600, AIO_DATA_BITS_8,
			  AIO_STOP_BITS_1, AIO_PARITY_NONE,
			  AIO_HARDWARE_FLOW_CONTROL_OFF);
  if (err != AIO_SUCCESS)
    {
      fprintf (stderr, "Could not configure port: %d\n", err);
      AIOReleasePort (AIOhandle);
      exit (1);
    }

  /* Register ourselves as an alternate debugger.  */
  memset (&s, 0, sizeof s);
  s.DDSResourceTag = ((struct ResourceTagStructure *)
		      AllocateResourceTag (GetNLMHandle (),
					   "gdbserver",
					   DebuggerSignature));
  if (s.DDSResourceTag == 0)
    {
      fprintf (stderr, "AllocateResourceTag failed\n");
      AIOReleasePort (AIOhandle);
      exit (1);
    }
  s.DDSdebuggerEntry = handle_exception;
  s.DDSFlags = TSS_FRAME_BIT;

  err = RegisterDebuggerRTag (&s, AT_FIRST);
  if (err != 0)
    {
      fprintf (stderr, "RegisterDebuggerRTag failed\n");
      AIOReleasePort (AIOhandle);
      exit (1);
    }

  /* Get the command line we were invoked with, and advance it past
     our name and the board and port arguments.  */
  cmdlin = getcmd ((char *) NULL);
  for (i = 0; i < 2; i++)
    {
      while (! isspace (*cmdlin))
	++cmdlin;
      while (isspace (*cmdlin))
	++cmdlin;
    }
  
  /* In case GDB is started before us, ack any packets (presumably
     "$?#xx") sitting there.  */
  if (! putDebugChar ('+'))
    {
      fprintf (stderr, "putDebugChar failed\n");
      UnRegisterDebugger (&s);
      AIOReleasePort (AIOhandle);
      exit (1);
    }

  mainthread = GetThreadID ();
  handle = NULL;
  talking = 0;

  if (remote_debug > 0)
    ConsolePrintf ("About to call LoadModule with \"%s\" %d %d\r\n",
		   cmdlin, console_screen, __GetScreenID (console_screen));

  /* Start up the module to be debugged.  */
  err = LoadModule ((struct ScreenStruct *) __GetScreenID (console_screen),
		    cmdlin, LO_DEBUG);
  if (err != 0)
    {
      fprintf (stderr, "LoadModule failed: %d\n", err);
      UnRegisterDebugger (&s);
      AIOReleasePort (AIOhandle);
      exit (1);
    }

  /* Wait for the debugger to wake us up.  */
  if (remote_debug > 0)
    ConsolePrintf ("Suspending main thread (%d)\r\n", mainthread);
  SuspendThread (mainthread);
  if (remote_debug > 0)
    ConsolePrintf ("Resuming main thread (%d)\r\n", mainthread);

  /* If we are woken up, print an optional error message, deregister
     ourselves and exit.  */
  if (error_message != NULL)
    fprintf (stderr, "%s\n", error_message);
  UnRegisterDebugger (&s);
  AIOReleasePort (AIOhandle);
  exit (0);
}
