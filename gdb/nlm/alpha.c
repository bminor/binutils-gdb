#include "alpha.h"

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
