#include "i386.h"

/* Get the registers out of the frame information.  */

static void
frame_to_registers (frame, regs)
     struct StackFrame *frame;
     char *regs;
{
  /* Copy EAX -> EDI */
  mem2hex (&frame->ExceptionEAX, &regs[0 * 4 * 2], 4 * 8, 0);

  /* Copy EIP & PS */
  mem2hex (&frame->ExceptionPC, &regs[8 * 4 * 2], 4 * 2, 0);

  /* Copy CS, SS, DS */
  mem2hex (&frame->ExceptionCS, &regs[10 * 4 * 2], 4 * 3, 0);

  /* Copy ES */
  mem2hex (&frame->ExceptionES, &regs[13 * 4 * 2], 4 * 1, 0);

  /* Copy FS & GS */
  mem2hex (&frame->ExceptionFS, &regs[14 * 4 * 2], 4 * 2, 0);
}

/* Put the registers back into the frame information.  */

static void
registers_to_frame (regs, frame)
     char *regs;
     struct StackFrame *frame;
{
  /* Copy EAX -> EDI */
  hex2mem (&regs[0 * 4 * 2], &frame->ExceptionEAX, 4 * 8, 0);

  /* Copy EIP & PS */
  hex2mem (&regs[8 * 4 * 2], &frame->ExceptionPC, 4 * 2, 0);

  /* Copy CS, SS, DS */
  hex2mem (&regs[10 * 4 * 2], &frame->ExceptionCS, 4 * 3, 0);

  /* Copy ES */
  hex2mem (&regs[13 * 4 * 2], &frame->ExceptionES, 4 * 1, 0);

  /* Copy FS & GS */
  hex2mem (&regs[14 * 4 * 2], &frame->ExceptionFS, 4 * 2, 0);
}

static void
set_step_traps (frame)
     struct StackFrame *frame;
{
  frame->ExceptionSystemFlags |= 0x100;
}

static void
clear_step_traps (frame)
     struct StackFrame *frame;
{
  frame->ExceptionSystemFlags &= ~0x100;
}

static void
do_status (ptr, frame)
     char *ptr;
     struct StackFrame *frame;
{
  int sigval;

  sigval = computeSignal (frame->ExceptionNumber);

  sprintf (ptr, "T%02x", sigval);
  ptr += 3;

  sprintf (ptr, "%02x:", PC_REGNUM);
  ptr = mem2hex (&frame->ExceptionPC, ptr + 3, 4, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", SP_REGNUM);
  ptr = mem2hex (&frame->ExceptionESP, ptr + 3, 4, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", FP_REGNUM);
  ptr = mem2hex (&frame->ExceptionEBP, ptr + 3, 4, 0);
  *ptr++ = ';';

  *ptr = '\000';
}
