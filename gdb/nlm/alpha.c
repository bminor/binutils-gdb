#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <nwtypes.h>
#include <nwdfs.h>
#include <nwconio.h>
#include <nwadv.h>
#include <nwdbgapi.h>
#include <nwthread.h>
#include <aio.h>

#include "alpha.h"

extern char *mem2hex (void *mem, char *buf, int count, int may_fault);
extern char *hex2mem (char *buf, void *mem, int count, int may_fault);
extern int computeSignal (int exceptionVector);

/* Get the registers out of the frame information.  */

void
frame_to_registers (frame, regs)
     struct StackFrame *frame;
     char *regs;
{
  mem2hex (&frame->ExceptionPC, &regs[PC_REGNUM * 8 * 2], 8 * 1, 0);

  mem2hex (&frame->ExceptionRegs[SF_IREG_OFFSET], &regs[V0_REGNUM * 8 * 2], 8 * 64, 0);
}

/* Put the registers back into the frame information.  */

void
registers_to_frame (regs, frame)
     char *regs;
     struct StackFrame *frame;
{
  hex2mem (&regs[PC_REGNUM * 8 * 2], &frame->ExceptionPC, 8 * 1, 0);

  hex2mem (&regs[V0_REGNUM * 8 * 2], &frame->ExceptionRegs[SF_IREG_OFFSET], 8 * 64, 0);
}

union inst
{
  LONG l;

  struct
    {
      union
	{
	  struct
	    {
	      unsigned hint : 16;
	      unsigned rb : 5;
	      unsigned ra : 5;
	      unsigned opcode : 6;
	    } jump;
	  struct
	    {
	      signed disp : 21;
	      unsigned ra : 5;
	      unsigned opcode : 6;
	    } branch;
	} variant;
    } inst;
};

static LONG saved_inst;
static LONG *saved_inst_pc = 0;
static LONG saved_target_inst;
static LONG *saved_target_inst_pc = 0;

void
set_step_traps (frame)
     struct StackFrame *frame;
{
  union inst inst;
  LONG *target;
  int opcode;
  int ra, rb;
  LONG *pc = (LONG *)frame->ExceptionPC;

  inst.l = *pc++;

  opcode = inst.inst.variant.branch.opcode;

  if ((opcode & 0x30) == 0x30)	/* A branch of some sort */
    target = inst.inst.variant.branch.disp + pc;
  else if (opcode == 0x1a)	/* jmp, ret, etc... */
    target = (LONG *)(frame->ExceptionRegs[SF_IREG_OFFSET
					   + inst.inst.variant.jump.rb].lo
		      & ~3);
  else
    target = pc;

  saved_inst = *pc;
  *pc = 0x80;			/* call_pal bpt */
  saved_inst_pc = pc;

  if (target != pc)
    {
      saved_target_inst = *target;
      *target = 0x80;		/* call_pal bpt */
      saved_target_inst_pc = target;
    }
}

/* Remove step breakpoints.  Returns non-zero if pc was at a step breakpoint,
   zero otherwise.  This routine works even if there were no step breakpoints
   set.  */

int
clear_step_traps (frame)
     struct StackFrame *frame;
{
  int retcode;
  LONG *pc = (LONG *)frame->ExceptionPC;

  if (saved_inst_pc == pc || saved_target_inst_pc == pc)
    retcode = 1;
  else
    retcode = 0;

  if (saved_inst_pc)
    {
      *saved_inst_pc = saved_inst;
      saved_inst_pc = 0;
    }

  if (saved_target_inst_pc)
    {
      *saved_target_inst_pc = saved_target_inst;
      saved_target_inst_pc = 0;
    }

  return retcode;
}

void
do_status (ptr, frame)
     char *ptr;
     struct StackFrame *frame;
{
  int sigval;

  sigval = computeSignal (frame->ExceptionNumber);

  sprintf (ptr, "T%02x", sigval);
  ptr += 3;

  sprintf (ptr, "%02x:", PC_REGNUM);
  ptr = mem2hex (&frame->ExceptionPC, ptr + 3, 8, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", SP_REGNUM);
  ptr = mem2hex (&frame->ExceptionRegs[SF_IREG_OFFSET + SP_REGNUM], ptr + 3, 8, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", RA_REGNUM);
  ptr = mem2hex (&frame->ExceptionRegs[SF_IREG_OFFSET + RA_REGNUM], ptr + 3, 8, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", FP_REGNUM);
  ptr = mem2hex (&frame->ExceptionRegs[SF_IREG_OFFSET + FP_REGNUM], ptr + 3, 8, 0);
  *ptr++ = ';';

  *ptr = '\000';
}

/* This section provides stubs and equivalent interfaces for all functions that
   the debugger stub needs, but aren't yet implemented (such as the AIO nlm). */

#include <nwtypes.h>
#include <errno.h>
#include <stdio.h>
#include <aio.h>

#define CONST const

#define com1Rbr	0x3f8		/* Receiver Buffer - Read	*/
#define com1Thr	0x3f8		/* Transmitter Holding - Write	*/
#define com1Ier	0x3f9		/* Interrupt Enable		*/
#define com1Iir	0x3fa		/* Interrupt Identification	*/
#define com1Lcr	0x3fb		/* Line Control			*/
#define com1Mcr	0x3fc		/* Modem Control		*/
#define com1Lsr	0x3fd		/* Line Status			*/
#define com1Msr	0x3fe		/* Modem Status			*/
#define com1Scr	0x3ff		/* Scratch			*/
#define com1Dll	0x3f8		/* Divisor Latch - lsb		*/
#define com1Dlm	0x3f9		/* Divisor Latch - msb		*/

#define com2Rbr	0x2f8		/* Receiver Buffer - Read	*/
#define com2Thr	0x2f8		/* Transmitter Holding - Write	*/
#define com2Ier	0x2f9		/* Interrupt Enable		*/
#define com2Iir	0x2fa		/* Interrupt Identification	*/
#define com2Lcr	0x2fb		/* Line Control			*/
#define com2Mcr	0x2fc		/* Modem Control		*/
#define com2Lsr	0x2fd		/* Line Status			*/
#define com2Msr	0x2fe		/* Modem Status			*/
#define com2Scr	0x2ff		/* Scratch			*/
#define com2Dll	0x2f8		/* Divisor Latch - lsb		*/
#define com2Dlm	0x2f9		/* Divisor Latch - msb		*/

#define COM1 0x8000
#define COM2 0x9000

static ULONG
uart_getchar (void)
{
  while ((inVti(com1Lsr) & 1) == 0);

  return inVti (com1Rbr);
}

static void  
uart_putchar (char c)
{ 
  while ((inVti(com1Lsr) & 0x20) == 0);

  outVti (com1Thr,c);
}

static int
uart_init (int baud)
{
  int i;
  int baudconst;

  baudconst = 115200 / baud;

  outVti (com1Lcr, 0x87);
  outVti (com1Dlm, 0);
  outVti (com1Dll, baudconst);
  outVti (com1Lcr, 0x07);
  outVti (com1Mcr, 0x0F);
  outVti (com1Ier, 0x0);
}

int
AIOReadData (int portHandle, char  *buffer, LONG length, LONG *numberBytesRead)
{
  ULONG c;

  while (1)
    {
      c = uart_getchar ();
      if ((c & ~0377) == COM1)
	break;
    }

  *buffer = c;
  *numberBytesRead = 1;

  return AIO_SUCCESS;
}

int
AIOWriteData (int portHandle, char *buffer, LONG length,
	      LONG *numberBytesWritten)
	
{
  *numberBytesWritten = length;

  while (length-- > 0)
    uart_putchar (*buffer++);

  return AIO_SUCCESS;
}

int
AIOAcquirePort (int *hardwareType, int *boardNumber, int *portNumber,
		int *portHandle)
{
  return AIO_SUCCESS;
}

int
AIOConfigurePort (int portHandle, BYTE bitRate,	BYTE dataBits, BYTE stopBits,
		  BYTE parityMode, BYTE flowCtrlMode)
{
  uart_init (9600);

  return AIO_SUCCESS;
}

int
AIOGetPortConfiguration (int portHandle, AIOPORTCONFIG *pPortConfig,
			     AIODVRCONFIG *pDvrConfig)
{
  fprintf (stderr, "AIOGetPortConfiguration stubbed out\n");
  exit (1);
}

int
AIOReleasePort (int portHandle)
{
  return AIO_SUCCESS;
}
	
int
AIOSetExternalControl (int portHandle, int requestType, int requestValue)
{
  return AIO_SUCCESS;
}

int
AIOGetExternalStatus (int portHandle, LONG *extStatus, LONG *chgdExtStatus)
{
  fprintf (stderr, "AIOGetExternalStatus stubbed out\n");
  exit (1);
}

void
StopBell ()
{
}

int
Breakpoint (int __arg)
{
  fprintf (stderr, "Breakpoint() stubbed out\n");
  exit (1);
}
