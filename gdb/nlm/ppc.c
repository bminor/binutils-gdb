#include <nwdfs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <nwconio.h>
#include <nwadv.h>
#include <nwdbgapi.h>
#include <errno.h>
#include <nwthread.h>
#include "ppc.h"

extern char *mem2hex (void *mem, char *buf, int count, int may_fault);
extern char *hex2mem (char *buf, void *mem, int count, int may_fault);
extern int computeSignal (int exceptionVector);

void
flush_i_cache (void)
{
}

/* Get the registers out of the frame information.  */

void
frame_to_registers (frame, regs)
     struct StackFrame *frame;
     char *regs;
{
  mem2hex (&frame->ExceptionState.CsavedRegs, &regs[GP0_REGNUM * 4 * 2], 4 * 32, 0);

  mem2hex (&frame->ExceptionState.CSavedFPRegs, &regs[FP0_REGNUM * 4 * 2], 4 * 32, 0);

  mem2hex (&frame->ExceptionPC, &regs[PC_REGNUM * 4 * 2], 4 * 1, 0);

  mem2hex (&frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedSRR1, &regs[CR_REGNUM * 4 * 2], 4 * 1, 0);
  mem2hex (&frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedLR, &regs[LR_REGNUM * 4 * 2], 4 * 1, 0);
  mem2hex (&frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedCTR, &regs[CTR_REGNUM * 4 * 2], 4 * 1, 0);
  mem2hex (&frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedXER, &regs[XER_REGNUM * 4 * 2], 4 * 1, 0);
  mem2hex (&frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedMQ, &regs[MQ_REGNUM * 4 * 2], 4 * 1, 0);
}

/* Put the registers back into the frame information.  */

void
registers_to_frame (regs, frame)
     char *regs;
     struct StackFrame *frame;
{
  hex2mem (&regs[GP0_REGNUM * 4 * 2], &frame->ExceptionState.CsavedRegs, 4 * 32, 0);

  hex2mem (&regs[FP0_REGNUM * 4 * 2], &frame->ExceptionState.CSavedFPRegs, 4 * 32, 0);

  hex2mem (&regs[PC_REGNUM * 4 * 2], &frame->ExceptionPC, 4 * 1, 0);

  hex2mem (&regs[CR_REGNUM * 4 * 2], &frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedSRR1, 4 * 1, 0);
  hex2mem (&regs[LR_REGNUM * 4 * 2], &frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedLR, 4 * 1, 0);
  hex2mem (&regs[CTR_REGNUM * 4 * 2], &frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedCTR, 4 * 1, 0);
  hex2mem (&regs[XER_REGNUM * 4 * 2], &frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedXER, 4 * 1, 0);
  hex2mem (&regs[MQ_REGNUM * 4 * 2], &frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedMQ, 4 * 1, 0);
}

extern int ReadByteAltDebugger (char* addr, char *theByte);

extern int WriteByteAltDebugger (char* addr, char theByte);

extern volatile int mem_err;

int
get_char (addr)
     char *addr;
{
  char c;

  if (!ReadByteAltDebugger (addr, &c))
    mem_err = 1;

  return c;
}

void
set_char (addr, val)
     char *addr;
     int val;
{
  if (!WriteByteAltDebugger (addr, val))
    mem_err = 1;
}

int
mem_write (dst, src, len)
     char *dst, *src;
     int len;
{
  while (len-- && !mem_err)
    set_char (dst++, *src++);

  return mem_err;
}

union inst
{
  LONG l;

  struct
    {
      union
	{
	  struct		/* Unconditional branch */
	    {
	      unsigned opcode : 6; /* 18 */
	      signed li : 24;
	      unsigned aa : 1;
	      unsigned lk : 1;
	    } b;
	  struct		/* Conditional branch */
	    {
	      unsigned opcode : 6; /* 16 */
	      unsigned bo : 5;
	      unsigned bi : 5;
	      signed bd : 14;
	      unsigned aa : 1;
	      unsigned lk : 1;
	    } bc;
	  struct		/* Conditional branch to ctr or lr reg */
	    {
	      unsigned opcode : 6; /* 19 */
	      unsigned bo : 5;
	      unsigned bi : 5;
	      unsigned type : 15; /* 528 = ctr, 16 = lr */
	      unsigned lk : 1;
	    } bclr;
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

  opcode = inst.inst.variant.b.opcode;

  target = pc;

  switch (opcode)
    {
    case 18:			/* Unconditional branch */

      if (inst.inst.variant.b.aa) /* Absolute? */
	target = 0;
      target += inst.inst.variant.b.li;

      break;
    case 16:			/* Conditional branch */

      if (!inst.inst.variant.bc.aa) /* Absolute? */
	target = 0;
      target += inst.inst.variant.bc.bd;

      break;
    case 19:			/* Cond. branch via ctr or lr reg */
      switch (inst.inst.variant.bclr.type)
	{
	case 528:		/* ctr */
	  target = (LONG *)frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedCTR;
	  break;
	case 16:		/* lr */
	  target = (LONG *)frame->ExceptionState.u.SpecialRegistersEnumerated.CsavedLR;
	  break;
	}
      break;
    }

  saved_inst = *pc;
  mem_write (pc, breakpoint_insn, BREAKPOINT_SIZE);
  saved_inst_pc = pc;

  if (target != pc)
    {
      saved_target_inst = *target;
      mem_write (target, breakpoint_insn, BREAKPOINT_SIZE);
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
      mem_write (saved_inst_pc, saved_inst, BREAKPOINT_SIZE);
      saved_inst_pc = 0;
    }

  if (saved_target_inst_pc)
    {
      mem_write (saved_target_inst_pc, saved_target_inst, BREAKPOINT_SIZE);
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
  ptr = mem2hex (&frame->ExceptionPC, ptr + 3, 4, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", SP_REGNUM);
  ptr = mem2hex (&frame->ExceptionState.CsavedRegs[SP_REGNUM], ptr + 3, 4, 0);
  *ptr++ = ';';

  sprintf (ptr, "%02x:", LR_REGNUM);
  ptr = mem2hex (&frame->ExceptionState.CsavedRegs[LR_REGNUM], ptr + 3, 4, 0);
  *ptr++ = ';';

  *ptr = '\000';
}

#if 0
/*
 * strtol : convert a string to long.
 *
 * Andy Wilson, 2-Oct-89.
 */

/* FIXME: It'd be nice to configure around these, but the include files are too
   painful.  These macros should at least be more portable than hardwired hex
   constants. */

#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#define	LONG_MAX	((long)(ULONG_MAX >> 1))	/* 0x7FFFFFFF */
#define	LONG_MIN	((long)(~LONG_MAX))		/* 0x80000000 */

extern int errno;

unsigned long strtoul(const char *s, char **ptr, int base);

long
strtol(s, ptr, base)
     const char *s; char **ptr; int base;
{
  int minus=0;
  unsigned long tmp;
  const char *start=s;
  char *eptr;

  if (s==NULL)
    {
      errno = ERANGE;
      if (!ptr)
	*ptr = (char *)start;
      return 0L;
    }
  while (isspace(*s))
	s++;
  if (*s == '-') {
	s++;
	minus = 1;
      }
  else if (*s == '+')
    s++;

  /*
   * let strtoul do the hard work.
   */
  tmp = strtoul(s, &eptr, base);
  if (ptr != NULL)
    *ptr = (char *)((eptr==s) ? (char *)start : eptr);
  if (tmp > (minus ? - (unsigned long) LONG_MIN : (unsigned long) LONG_MAX))
    {
      errno = ERANGE;
      return (minus ? LONG_MIN : LONG_MAX);
    }
  return (minus ? (long) -tmp : (long) tmp);
}

/*
 * strtol : convert a string to long.
 *
 * Andy Wilson, 2-Oct-89.
 */

#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#endif

extern int errno;

unsigned long
strtoul(s, ptr, base)
     const char *s; char **ptr; int base;
{
  unsigned long total = 0;
  unsigned digit;
  const char *start=s;
  int did_conversion=0;
  int overflow = 0;
  int negate = 0;
  unsigned long maxdiv, maxrem;

  if (s==NULL)
    {
      errno = ERANGE;
      if (!ptr)
	*ptr = (char *)start;
      return 0L;
    }

  while (isspace(*s))
    s++;
  if (*s == '+')
    s++;
  else if (*s == '-')
    s++, negate = 1;
  if (base==0 || base==16) /*  the 'base==16' is for handling 0x */
    {
      int tmp;

      /*
       * try to infer base from the string
       */
      if (*s != '0')
        tmp = 10;	/* doesn't start with 0 - assume decimal */
      else if (s[1] == 'X' || s[1] == 'x')
	tmp = 16, s += 2; /* starts with 0x or 0X - hence hex */
      else
	tmp = 8;	/* starts with 0 - hence octal */
      if (base==0)
	base = (int)tmp;
    }

  maxdiv = ULONG_MAX / base;
  maxrem = ULONG_MAX % base;

  while ((digit = *s) != '\0')
    {
      if (digit >= '0' && digit < ('0'+base))
	digit -= '0';
      else
	if (base > 10)
	  {
	    if (digit >= 'a' && digit < ('a'+(base-10)))
	      digit = digit - 'a' + 10;
	    else if (digit >= 'A' && digit < ('A'+(base-10)))
	      digit = digit - 'A' + 10;
	    else
	      break;
	  }
	else
	  break;
      did_conversion = 1;
      if (total > maxdiv
	  || (total == maxdiv && digit > maxrem))
	overflow = 1;
      total = (total * base) + digit;
      s++;
    }
  if (overflow)
    {
      errno = ERANGE;
      if (ptr != NULL)
	*ptr = (char *)s;
      return (ULONG_MAX);
    }
  if (ptr != NULL)
    *ptr = (char *) ((did_conversion) ? (char *)s : (char *)start);
  return negate ? -total : total;
}
#endif

void _exit (int foo) __attribute__ ((noreturn));

void
exit (int foo)
{
  _exit (foo);
}
