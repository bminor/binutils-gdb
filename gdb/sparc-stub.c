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
 *  Header: remcom.c,v 1.34 91/03/09 12:29:49 glenne Exp $
 *
 *  Module name: remcom.c $
 *  Revision: 1.34 $
 *  Date: 91/03/09 12:29:49 $
 *  Contributor:     Lake Stevens Instrument Division$
 *
 *  Description:     low level support for gdb debugger. $
 *
 *  Considerations:  only works on target hardware $
 *
 *  Written by:      Glenn Engel $
 *  ModuleState:     Experimental $
 *
 *  NOTES:           See Below $
 *
 *  Modified for SPARC by Stu Grossman, Cygnus Support.
 *
 *  To enable debugger support, two things need to happen.  One, a
 *  call to set_debug_traps() is necessary in order to allow any breakpoints
 *  or error conditions to be properly intercepted and reported to gdb.
 *  Two, a breakpoint needs to be generated to begin communication.  This
 *  is most easily accomplished by a call to breakpoint().  Breakpoint()
 *  simulates a breakpoint by executing a trap #1.
 *
 *************
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

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <memory.h>

/************************************************************************
 *
 * external low-level support routines
 */

extern putDebugChar();   /* write a single character      */
extern getDebugChar();   /* read and return a single char */

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static int initialized;  /* boolean flag. != 0 means we've been initialized */

static void set_mem_fault_trap();

int remote_debug;
/*  debug >  0 prints ill-formed commands in valid packets & checksum errors */

static const char hexchars[]="0123456789abcdef";

#define NUMREGS 72

/* Number of bytes of registers.  */
#define NUMREGBYTES (NUMREGS * 4)
enum regnames {G0, G1, G2, G3, G4, G5, G6, G7,
		 O0, O1, O2, O3, O4, O5, SP, O7,
		 L0, L1, L2, L3, L4, L5, L6, L7,
		 I0, I1, I2, I3, I4, I5, FP, I7,

		 F0, F1, F2, F3, F4, F5, F6, F7,
		 F8, F9, F10, F11, F12, F13, F14, F15,
		 F16, F17, F18, F19, F20, F21, F22, F23,
		 F24, F25, F26, F27, F28, F29, F30, F31,
		 Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR };

static unsigned long registers[NUMREGS] __attribute__ ((aligned (8)));

/***************************  ASSEMBLY CODE MACROS *************************/
/* 									   */

#define BREAKPOINT() asm("   ta 1");

extern unsigned long rdtbr();

asm("
	.text
!
! FUNCTION
!	_chk4ovflo
!
! DESCRIPTION
!	This code is branched to before each trap (except reset,
!	_win_unf, and _win_ovf) handler.
!	 It checks to see if we've moved into the invalid window
!	 and performs fixup ala _win_ovf.
!
! INPUTS
!	- %l1 = pc at trap time.
!	- %l2 = npc at trap time.
!	- %l7 = return address.
!
! INTERNAL DESCRIPTION
!
! RETURNS
!	- None.
!

	.align 4

_chk4ovflo:
	mov	%psr, %l0		! get the psr
	and	%l0, 0x1F, %l3		! get the cwp
	mov	1, %l4			! compare cwp with the wim
	sll	%l4, %l3, %l3 		! compare
	mov	%wim, %l4		! read the wim
	btst	%l4, %l3
	bz	_retsave		! not invalid window, just return
	nop
					! in line version of _win_ovf
	or	%l0, 0xf20, %l3		! enable traps, disable interrupts.
	mov	%l3, %psr
	mov	%g1, %l0		! Save %g1.
	srl	%l4, 1, %g1		! Next WIM = %g1 = rol(WIM, 1, NWINDOW)
	sll	%l4, 8-1, %l3
	bset	%l3, %g1
	save	%g0, %g0, %g0		! Get into window to be saved.
	mov	%g1, %wim		! Install new wim.
	nop				! must delay three instructions
	nop				! before using these registers, so
	nop				! put nops in just to be safe

	std	%l0, [%sp + 0 * 4]	! save all local registers
	std	%l2, [%sp + 2 * 4]
	std	%l4, [%sp + 4 * 4]
	std	%l6, [%sp + 6 * 4]

	std	%i0, [%sp + 8 * 4]
	std	%i2, [%sp + 10 * 4]
	std	%i4, [%sp + 12 * 4]
	std	%i6, [%sp + 14 * 4]

	restore				! Go back to trap window.
	mov	%l0, %g1		! Restore %g1.

_retsave:
	! It is safe now to allocate a stack frame for this window
	! because all overflow handling will have been accomplished
	! in the event we trapped into the invalid window.
	! ie. all of this window's %o regs (next window's %i regs)
	! will have been safely stored to the stack before we overwrite %sp.

	jmpl	%l7+8, %g0		! Window is valid, just return
	sub	%fp, (16+1+6+1)*4, %sp	! Make room for input & locals
 					! + hidden arg + arg spill
					! + doubleword alignment

! Read the TBR.

	.globl _rdtbr
_rdtbr:
	retl
	mov	%tbr, %o0

! This function is called when any SPARC trap (except window overflow or
! underflow) occurs.  It makes sure that the invalid register window is still
! available before jumping into C code.  It will also restore the world if you
! return from handle_exception.

_trap_low:
	set	_registers, %l0

	std	%g0, [%l0 + 0 * 4]	! registers[Gx]
	std	%g2, [%l0 + 2 * 4]
	std	%g4, [%l0 + 4 * 4]
	std	%g6, [%l0 + 6 * 4]

	std	%i0, [%l0 + 8 * 4]	! registers[Ox]
	std	%i2, [%l0 + 10 * 4]
	std	%i4, [%l0 + 12 * 4]
	std	%i6, [%l0 + 14 * 4]
					! F0->F31 not implemented
	mov	%y, %l4
	mov	%psr, %l5
	mov	%wim, %l6
	mov	%tbr, %l7
	std	%l4, [%l0 + 64 * 4]	! Y & PSR
	std	%l6, [%l0 + 66 * 4]	! WIM & TBR
	st	%l1, [%l0 + 68 * 4]	! PC
	st	%l2, [%l0 + 69 * 4]	! NPC

					! CPSR and FPSR not impl

	sethi	%hi(_chk4ovflo), %l7	! Must call this routine via %l7
	jmpl	%l7+%lo(_chk4ovflo), %l7 !  because o regs may not be available yet
	nop
	mov	%psr, %o1
	bset	0xf20, %o1
	mov	%o1, %psr		! Turn on traps, disable interrupts

	call	_handle_exception
	nop
	mov	%o0, %l7		! Save return value

! Reload all of the registers that aren't on the stack

	set	_registers, %l0		! Need to use reg immune from save/rest

	ld	[%l0 + 1 * 4], %g1	! registers[Gx]
	ldd	[%l0 + 2 * 4], %g2
	ldd	[%l0 + 4 * 4], %g4
	ldd	[%l0 + 6 * 4], %g6

	ldd	[%l0 + 8 * 4], %o0	! registers[Ox]
	ldd	[%l0 + 10 * 4], %o2
	ldd	[%l0 + 12 * 4], %o4
	ldd	[%l0 + 14 * 4], %o6

	restore				! Ensure that previous window is valid
	save	%g0, %g0, %g0		!  by causing a window_underflow trap

	ld	[%l0 + 64 * 4], %l3	! registers[Y]
	mov	%l3, %y
	ld	[%l0 + 65 * 4], %l3	! registers[PSR]
	ld	[%l0 + 68 * 4], %l1	! registers[PC]
	ld	[%l0 + 69 * 4], %l2	! registers[NPC]

	tst	%l7			! Did handle_exception tell
	bg	retskip			!  us to skip the next inst?
	nop

	mov	%l3, %psr		! Make sure that traps are disabled
					! for rett
	jmpl	%l1, %g0		! Restore old PC
	rett	%l2			! Restore old nPC

	mov	%l3, %psr		! Make sure that traps are disabled
					! for rett
retskip: ! Come here to skip the next instruction
	jmpl	%l2, %g0		! Old nPC
	rett	%l2+4			! Old nPC+4
");

/* Convert ch from a hex digit to an int */

static int
hex(ch)
     unsigned char ch;
{
  if (ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  if (ch >= '0' && ch <= '9')
    return ch-'0';
  if (ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  return -1;
}

/* scan for the sequence $<data>#<checksum>     */

static void
getpacket(buffer)
     char *buffer;
{
  unsigned char checksum;
  unsigned char xmitcsum;
  int i;
  int count;
  unsigned char ch;

  do
    {
      /* wait around for the start character, ignore all other characters */
      while ((ch = getDebugChar()) != '$') ;

      checksum = 0;
      xmitcsum = -1;

      count = 0;

      /* now, read until a # or end of buffer is found */
      while (count < BUFMAX)
	{
	  ch = getDebugChar();
	  if (ch == '#')
	    break;
	  checksum = checksum + ch;
	  buffer[count] = ch;
	  count = count + 1;
	}

      if (count >= BUFMAX)
	continue;

      buffer[count] = 0;

      if (ch == '#')
	{
	  xmitcsum = hex(getDebugChar()) << 4;
	  xmitcsum |= hex(getDebugChar());
#ifdef DEBUG
	  if (remote_debug && checksum != xmitcsum)
	    {
	      fprintf(stderr, "bad checksum.  My count = 0x%x, sent=0x%x. buf=%s\n",
		      checksum,xmitcsum,buffer);
	    }
#endif
#if 1
	  /* Humans shouldn't have to figure out checksums to type to it. */
	  putDebugChar ('+');
	  return;
#endif
	  if (checksum != xmitcsum)
	    putDebugChar('-');	/* failed checksum */
	  else
	    {
	      putDebugChar('+'); /* successful transfer */
	      /* if a sequence char is present, reply the sequence ID */
	      if (buffer[2] == ':')
		{
		  putDebugChar(buffer[0]);
		  putDebugChar(buffer[1]);
		  /* remove sequence chars from buffer */
		  count = strlen(buffer);
		  for (i=3; i <= count; i++)
		    buffer[i-3] = buffer[i];
		}
	    }
	}
    }
  while (checksum != xmitcsum);
}

/* send the packet in buffer.  */

static void
putpacket(buffer)
     unsigned char *buffer;
{
  unsigned char checksum;
  int count;
  unsigned char ch;

  /*  $<packet info>#<checksum>. */
  do
    {
      putDebugChar('$');
      checksum = 0;
      count = 0;

      while (ch = buffer[count])
	{
	  if (! putDebugChar(ch))
	    return;
	  checksum += ch;
	  count += 1;
	}

      putDebugChar('#');
      putDebugChar(hexchars[checksum >> 4]);
      putDebugChar(hexchars[checksum & 0xf]);

    }
  while (getDebugChar() != '+');
}

static unsigned char remcomInBuffer[BUFMAX];
static unsigned char remcomOutBuffer[BUFMAX];
static short error;

static void
debug_error(format, parm)
     char *format;
     char *parm;
{
#ifdef DEBUG
  if (remote_debug)
    fprintf(stderr,format,parm);
#endif
}

/* Address of a routine to RTE to if we get a memory fault.  */
static void (*mem_fault_routine)() = NULL;

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

/* Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null), in case of mem fault,
 * return 0.
 * If MAY_FAULT is non-zero, then we will handle memory faults by returning
 * a 0, else treat a fault like any other fault in the stub.
 */

static unsigned char *
mem2hex(mem, buf, count, may_fault)
     unsigned char *mem;
     unsigned char *buf;
     int count;
     int may_fault;
{
  unsigned char ch;

  set_mem_fault_trap(may_fault);

  while (count-- > 0)
    {
      ch = get_char(mem++);
      if (mem_err)
	return 0;
      *buf++ = hexchars[ch >> 4];
      *buf++ = hexchars[ch & 0xf];
    }

  *buf = 0;

  set_mem_fault_trap(0);

  return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written */

static char *
hex2mem(buf, mem, count, may_fault)
     unsigned char *buf;
     unsigned char *mem;
     int count;
     int may_fault;
{
  int i;
  unsigned char ch;

  set_mem_fault_trap(may_fault);

  for (i=0; i<count; i++)
    {
      ch = hex(*buf++) << 4;
      ch |= hex(*buf++);
      set_char(mem++, ch);
      if (mem_err)
	return 0;
    }

  set_mem_fault_trap(0);

  return mem;
}

/* this function takes the SPARC trap type code and attempts to
   translate this number into a unix compatible signal value */

static int
computeSignal(tt)
     int tt;
{
  int sigval;

  switch (tt)
    {
    case 1:
      sigval = SIGSEGV; break;	/* instruction access error */
    case 2:
      sigval = SIGILL; break;	/* privileged instruction */
    case 3:
      sigval = SIGILL; break;	/* illegal instruction */
    case 4:
      sigval = SIGEMT; break;	/* fp disabled */
    case 36:
      sigval = SIGEMT; break;	/* cp disabled */
    case 7:
      sigval = SIGBUS; break;	/* mem address not aligned */
    case 9:
      sigval = SIGSEGV; break;	/* data access exception */
    case 10:
      sigval = SIGEMT; break;	/* tag overflow */
    case 128+1:			/* ta 1 - normal breakpoint instruction */
    case 255:			/* breakpoint hardware unique to SPARClite */
      sigval = SIGTRAP; break;	/* breakpoint trap */
    default:
      sigval = SIGHUP;		/* "software generated"*/
    }
  return (sigval);
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */

static int
hexToInt(char **ptr, int *intValue)
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

/*
 * This function does all command procesing for interfacing to gdb.  It
 * returns 1 if you should skip the instruction at the trap address, 0
 * otherwise.
 */

static int
handle_exception ()
{
  int tt;			/* Trap type */
  int sigval;
  int addr;
  int length;
  char *ptr;
  int newPC;
  unsigned char *sp;
  unsigned char *com;

/* First, we must force all of the windows to be spilled out */

  asm(" save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	save %g0, -64, %g0
	restore
	restore
	restore
	restore
	restore
	restore
	restore
	restore
");

#if 0
  writez(1, "Got to handle_exception()\r\n ");

  writez(1, "psr = 0x");
  numout(registers[PSR], 16);
  writez(1, " tbr = 0x");
  numout(registers[TBR], 16);
  writez(1, " oldpc = 0x");
  numout(registers[PC], 16);
  writez(1, " oldnpc = 0x");
  numout(registers[NPC], 16);
  writez(1, "\r\n");
#endif

  sp = (unsigned char *)registers[SP];

  tt = (registers[TBR] >> 4) & 0xff;

#ifdef DEBUG
  if (remote_debug)
    printf("tbr=0x%x, tt=%d, psr=0x%x, pc=0x%x, npc=0x%x\n",
	   registers[TBR], (registers[TBR] >> 4) & 0xff, registers[PSR], registers[PC], registers[NPC]);
#endif

  /* reply to host that an exception has occurred */
  sigval = computeSignal(tt);
  com = remcomOutBuffer;

  *com++ = 'T';
  *com++ = hexchars[sigval >> 4];
  *com++ = hexchars[sigval & 0xf];

  *com++ = hexchars[PC >> 4];
  *com++ = hexchars[PC & 0xf];
  com = mem2hex((char *)&registers[PC], com, 4, 0);

  *com++ = hexchars[FP >> 4];
  *com++ = hexchars[FP & 0xf];
  com = mem2hex(sp + (8 + 6) * 4, com, 4, 0); /* FP */

  *com++ = hexchars[SP >> 4];
  *com++ = hexchars[SP & 0xf];
  com = mem2hex((char *)&registers[SP], com, 4, 0);

  *com++ = hexchars[NPC >> 4];
  *com++ = hexchars[NPC & 0xf];
  com = mem2hex((char *)&registers[NPC], com, 4, 0);

  *com++ = 0;

  putpacket(remcomOutBuffer);

  while (1)
    {
      error = 0;
      remcomOutBuffer[0] = 0;

      getpacket(remcomInBuffer);
      switch (remcomInBuffer[0])
	{
	case '?':
	  remcomOutBuffer[0] = 'S';
	  remcomOutBuffer[1] = hexchars[sigval >> 4];
	  remcomOutBuffer[2] = hexchars[sigval & 0xf];
	  remcomOutBuffer[3] = 0;
	  break;

	case 'd':
	  remote_debug = !remote_debug; /* toggle debug flag */
	  break;

	case 'g':		/* return the value of the CPU registers */
	  {
	    com = remcomOutBuffer;
	    com = mem2hex((char *)registers, com, 16 * 4, 0); /* G & O regs */
	    com = mem2hex(sp + 0 * 4, com, 8 * 4, 0); /* L regs */
	    com = mem2hex(sp + 8 * 4, com, 8 * 4, 0); /* I regs */
	    memset(com, '0', 32 * 8); /* Floating point */
	    mem2hex((char *)&registers[Y],
		    com + 32 * 4 * 2,
		    8 * 4,
		    0);		/* Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR */
	  }
	  break;

	case 'G':	   /* set the value of the CPU registers - return OK */
	  {
	    com = &remcomInBuffer[1];
	    hex2mem(com, (char *)registers, 16 * 4, 0); /* G & O regs */
	    hex2mem(com + 16 * 4 * 2, sp + 0 * 4, 8 * 4, 0); /* L regs */
	    hex2mem(com + 24 * 4 * 2, sp + 8 * 4, 8 * 4, 0); /* I regs */
	    hex2mem(com + 64 * 4 * 2, (char *)&registers[Y],
		    8 * 4, 0);	/* Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR */
	    strcpy(remcomOutBuffer,"OK");
	  }
	  break;

	case 'm':	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
	  /* TRY TO READ %x,%x.  IF SUCCEED, SET PTR = 0 */

	  ptr = &remcomInBuffer[1];

	  if (hexToInt(&ptr, &addr)
	      && *ptr++ == ','
	      && hexToInt(&ptr, &length))
	    {
	      if (mem2hex((char *)addr, remcomOutBuffer, length, 1))
		break;

	      strcpy (remcomOutBuffer, "E03");
	      debug_error ("memory fault");
	    }
	  else
	    {
	      strcpy(remcomOutBuffer,"E01");
	      debug_error("malformed read memory command: %s",remcomInBuffer);
	    }
	  break;

	case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
	  /* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */

	  ptr = &remcomInBuffer[1];

	  if (hexToInt(&ptr, &addr)
	      && *ptr++ == ','
	      && hexToInt(&ptr, &length)
	      && *ptr++ == ':')
	    {
	      if (hex2mem(ptr, (char *)addr, length, 1))
		strcpy(remcomOutBuffer, "OK");
	      else
		{
		  strcpy(remcomOutBuffer, "E03");
		  debug_error("memory fault");
		}
	    }
	  else
	    {
	      strcpy(remcomOutBuffer, "E02");
	      debug_error("malformed write memory command: %s",remcomInBuffer);
	    }
	  break;

	case 'c':    /* cAA..AA    Continue at address AA..AA(optional) */
	case 's':    /* sAA..AA   Step one instruction from AA..AA(optional) */
	  /* try to read optional parameter, pc unchanged if no parm */

	  ptr = &remcomInBuffer[1];
	  if (hexToInt(&ptr, &addr))
	    {
	      registers[PC] = addr;
	      registers[NPC] = addr + 4;
	    }

	  return 0;

	  /* kill the program */
	case 'k' :		/* do nothing */
	  break;
	}			/* switch */

      /* reply to the request */
      putpacket(remcomOutBuffer);
    }
}

/* Each entry in the trap vector occupies four words. */

struct trap_entry
{
  unsigned long ti[4];
};

#define NUMTRAPS 256

/* static struct trap_entry oldvec[NUMTRAPS];*/

extern struct trap_entry fltr_proto;
extern struct trap_entry fltr_set_mem_err;
asm ("
	.data
	.globl _fltr_proto
	.align 4
_fltr_proto:			! First level trap routine prototype
	sethi %hi(_trap_low), %l0
	jmpl %lo(_trap_low)+%l0, %g0
	nop
	nop

! Trap handler for memory errors.  This just sets mem_err to be non-zero.  It
! assumes that %l1 is non-zero.  This should be safe, as it is doubtful that
! 0 would ever contain code that could mem fault.  This routine will skip
! past the faulting instruction after setting mem_err.

_fltr_set_mem_err:
	sethi %hi(_mem_err), %l0
	st %l1, [%l0 + %lo(_mem_err)]
	jmpl %l2, %g0
	rett %l2+4

	.text
");

/* this function is used to set up exception handlers for tracing and
   breakpoints */

void
set_debug_traps()
{
  int exception;
  struct trap_entry *tb;	/* Trap vector base address */

  writez(1, "Got to set_debug_traps\r\n");

  tb = (struct trap_entry *)(rdtbr() & ~0xfff);

  writez(1, "tb = 0x");
  numout(tb, 16);
  writez(1, " trap ins = 0x");
  numout(fltr_proto, 16);
  writez(1, "\r\n");

  tb[1] = fltr_proto;		/* instruction access exception */
  tb[2] = fltr_proto;		/* privileged instruction */
  tb[3] = fltr_proto;		/* illegal instruction */
  tb[4] = fltr_proto;		/* fp disabled */
  tb[36] = fltr_proto;		/* cp disabled */
  tb[7] = fltr_proto;		/* mem address not aligned */
  tb[9] = fltr_proto;		/* data access exception */
  tb[10] = fltr_proto;		/* tag overflow */
  tb[128+1] = fltr_proto;	/* breakpoint instruction (ta 1) */
  tb[255] = fltr_proto;		/* hardware breakpoint trap */

  /* In case GDB is started before us, ack any packets (presumably
     "$?#xx") sitting there.  */

  putDebugChar ('+');

  initialized = 1;
}

static void
set_mem_fault_trap(enable)
     int enable;
{
  struct trap_entry *tb;	/* Trap vector base address */

  mem_err = 0;

  tb = (struct trap_entry *)(rdtbr() & ~0xfff);

  if (enable)
    tb[9] = fltr_set_mem_err;
  else
    tb[9] = fltr_proto;
}

/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */

void
breakpoint()
{
  writez(1, "About to do a breakpoint\r\n\n");
  if (initialized)
    BREAKPOINT();
}
