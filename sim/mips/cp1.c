/*> cp1.c <*/
/* MIPS Simulator FPU (CoProcessor 1) support.
   Copyright (C) 2002 Free Software Foundation, Inc.
   Originally created by Cygnus Solutions, modified substially
   by Broadcom Corporation (SiByte).

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* XXX: The following notice should be removed as soon as is practical:  */
/* Floating Point Support for gdb MIPS simulators

   This file is part of the MIPS sim

		THIS SOFTWARE IS NOT COPYRIGHTED
   (by Cygnus.)

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

   (Originally, this code was in interp.c)
*/

#include "sim-main.h"

/* Within cp1.c we refer to sim_cpu directly.  */
#define CPU cpu
#define SD CPU_STATE(cpu)

/*-- FPU support routines ---------------------------------------------------*/

/* Numbers are held in normalized form. The SINGLE and DOUBLE binary
   formats conform to ANSI/IEEE Std 754-1985.

   SINGLE precision floating:
      seeeeeeeefffffffffffffffffffffff
        s =  1bit  = sign
        e =  8bits = exponent
        f = 23bits = fraction

   SINGLE precision fixed:
      siiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
        s =  1bit  = sign
        i = 31bits = integer

   DOUBLE precision floating:
      seeeeeeeeeeeffffffffffffffffffffffffffffffffffffffffffffffffffff
        s =  1bit  = sign
        e = 11bits = exponent
        f = 52bits = fraction

   DOUBLE precision fixed:
      siiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
        s =  1bit  = sign
        i = 63bits = integer
 */

/* Explicit QNaN values used when value required:  */
#define FPQNaN_SINGLE   (0x7FBFFFFF)
#define FPQNaN_WORD     (0x7FFFFFFF)
#define FPQNaN_DOUBLE   (UNSIGNED64 (0x7FF7FFFFFFFFFFFF))
#define FPQNaN_LONG     (UNSIGNED64 (0x7FFFFFFFFFFFFFFF))

static const char *fpu_format_name (FP_formats fmt);
#ifdef DEBUG
static const char *fpu_rounding_mode_name (int rm);
#endif

uword64
value_fpr (sim_cpu *cpu,
	   address_word cia,
	   int fpr,
	   FP_formats fmt)
{
  uword64 value = 0;
  int err = 0;

  /* Treat unused register values, as fixed-point 64bit values:  */
  if ((fmt == fmt_uninterpreted) || (fmt == fmt_unknown))
    {
#if 1
      /* If request to read data as "uninterpreted", then use the current
	 encoding:  */
      fmt = FPR_STATE[fpr];
#else
      fmt = fmt_long;
#endif
    }

  /* For values not yet accessed, set to the desired format:  */
  if (FPR_STATE[fpr] == fmt_uninterpreted)
    {
      FPR_STATE[fpr] = fmt;
#ifdef DEBUG
      printf ("DBG: Register %d was fmt_uninterpreted. Now %s\n", fpr,
	      fpu_format_name (fmt));
#endif /* DEBUG */
    }
  if (fmt != FPR_STATE[fpr])
    {
      sim_io_eprintf (SD, "FPR %d (format %s) being accessed with format %s - setting to unknown (PC = 0x%s)\n",
		      fpr, fpu_format_name (FPR_STATE[fpr]),
		      fpu_format_name (fmt), pr_addr (cia));
      FPR_STATE[fpr] = fmt_unknown;
    }

  if (FPR_STATE[fpr] == fmt_unknown)
    {
      /* Set QNaN value:  */
      switch (fmt)
	{
	case fmt_single:
	  value = FPQNaN_SINGLE;
	  break;

	case fmt_double:
	  value = FPQNaN_DOUBLE;
	  break;

	case fmt_word:
	  value = FPQNaN_WORD;
	  break;

	case fmt_long:
	  value = FPQNaN_LONG;
	  break;

	default:
	  err = -1;
	  break;
	}
    }
  else if (SizeFGR () == 64)
    {
      switch (fmt)
	{
	case fmt_single:
	case fmt_word:
	  value = (FGR[fpr] & 0xFFFFFFFF);
	  break;

	case fmt_uninterpreted:
	case fmt_double:
	case fmt_long:
	  value = FGR[fpr];
	  break;

	default:
	  err = -1;
	  break;
	}
    }
  else
    {
      switch (fmt)
	{
	case fmt_single:
	case fmt_word:
	  value = (FGR[fpr] & 0xFFFFFFFF);
	  break;

	case fmt_uninterpreted:
	case fmt_double:
	case fmt_long:
	  if ((fpr & 1) == 0)
	    {
	      /* even registers only */
#ifdef DEBUG
	      printf ("DBG: ValueFPR: FGR[%d] = %s, FGR[%d] = %s\n",
		      fpr + 1, pr_uword64 ((uword64) FGR[fpr+1]),
		      fpr, pr_uword64 ((uword64) FGR[fpr]));
#endif
	      value = ((((uword64) FGR[fpr+1]) << 32)
		       | (FGR[fpr] & 0xFFFFFFFF));
	    }
	  else
	    {
	      SignalException (ReservedInstruction, 0);
	    }
	  break;

	default:
	  err = -1;
	  break;
	}
    }

  if (err)
    SignalExceptionSimulatorFault ("Unrecognised FP format in ValueFPR ()");

#ifdef DEBUG
  printf ("DBG: ValueFPR: fpr = %d, fmt = %s, value = 0x%s : PC = 0x%s : SizeFGR () = %d\n",
	  fpr, fpu_format_name (fmt), pr_uword64 (value), pr_addr (cia),
	  SizeFGR ());
#endif /* DEBUG */

  return (value);
}

void
store_fpr (sim_cpu *cpu,
	   address_word cia,
	   int fpr,
	   FP_formats fmt,
	   uword64 value)
{
  int err = 0;

#ifdef DEBUG
  printf ("DBG: StoreFPR: fpr = %d, fmt = %s, value = 0x%s : PC = 0x%s : SizeFGR () = %d, \n",
	  fpr, fpu_format_name (fmt), pr_uword64 (value), pr_addr (cia),
	  SizeFGR ());
#endif /* DEBUG */

  if (SizeFGR () == 64)
    {
      switch (fmt)
	{
	case fmt_uninterpreted_32:
	  fmt = fmt_uninterpreted;
	case fmt_single:
	case fmt_word:
	  if (STATE_VERBOSE_P (SD))
	    sim_io_eprintf (SD,
			    "Warning: PC 0x%s: interp.c store_fpr DEADCODE\n",
			    pr_addr (cia));
	  FGR[fpr] = (((uword64) 0xDEADC0DE << 32) | (value & 0xFFFFFFFF));
	  FPR_STATE[fpr] = fmt;
	  break;

	case fmt_uninterpreted_64:
	  fmt = fmt_uninterpreted;
	case fmt_uninterpreted:
	case fmt_double:
	case fmt_long:
	  FGR[fpr] = value;
	  FPR_STATE[fpr] = fmt;
	  break;

	default:
	  FPR_STATE[fpr] = fmt_unknown;
	  err = -1;
	  break;
	}
    }
  else
    {
      switch (fmt)
	{
	case fmt_uninterpreted_32:
	  fmt = fmt_uninterpreted;
	case fmt_single:
	case fmt_word:
	  FGR[fpr] = (value & 0xFFFFFFFF);
	  FPR_STATE[fpr] = fmt;
	  break;

	case fmt_uninterpreted_64:
	  fmt = fmt_uninterpreted;
	case fmt_uninterpreted:
	case fmt_double:
	case fmt_long:
	  if ((fpr & 1) == 0)
	    {
	      /* even register number only */
	      FGR[fpr+1] = (value >> 32);
	      FGR[fpr] = (value & 0xFFFFFFFF);
	      FPR_STATE[fpr + 1] = fmt;
	      FPR_STATE[fpr] = fmt;
	    }
	  else
	    {
	      FPR_STATE[fpr] = fmt_unknown;
	      FPR_STATE[fpr + 1] = fmt_unknown;
	      SignalException (ReservedInstruction, 0);
	    }
	  break;

	default:
	  FPR_STATE[fpr] = fmt_unknown;
	  err = -1;
	  break;
	}
    }

  if (err)
    SignalExceptionSimulatorFault ("Unrecognised FP format in StoreFPR ()");

#ifdef DEBUG
  printf ("DBG: StoreFPR: fpr[%d] = 0x%s (format %s)\n",
	  fpr, pr_uword64 (FGR[fpr]), fpu_format_name (fmt));
#endif /* DEBUG */

  return;
}


/* CP1 control/status registers */

void
test_fcsr (sim_cpu *cpu,
	   address_word cia)
{
  unsigned int cause;

  cause = (FCSR & fcsr_CAUSE_mask) >> fcsr_CAUSE_shift;
  if ((cause & ((FCSR & fcsr_ENABLES_mask) >> fcsr_ENABLES_shift)) != 0
      || (cause & (1 << UO)))
    {
      SignalExceptionFPE();
    }
}

unsigned_word
value_fcr(sim_cpu *cpu,
	  address_word cia,
	  int fcr)
{
  unsigned32 value = 0;

  switch (fcr)
    {
    case 0:  /* FP Implementation and Revision Register */
      value = FCR0;
      break;
    case 25:  /* FP Condition Codes Register */
      value = (FCR31 & fcsr_FCC_mask) >> fcsr_FCC_shift;
      value = (value & 0x1) | (value >> 1);   /* close FCC gap */
      break;
    case 26:  /* FP Exceptions Register */
      value = FCR31 & (fcsr_CAUSE_mask | fcsr_FLAGS_mask);
      break;
    case 28:  /* FP Enables Register */
      value = FCR31 & (fcsr_ENABLES_mask | fcsr_RM_mask);
      if (FCR31 & fcsr_FS)
	value |= 0x4;                        /* nonstandard FS bit */
      break;
    case 31:  /* FP Control/Status Register */
      value = FCR31 & ~fcsr_ZERO_mask;
      break;
    }

  return (EXTEND32 (value));
}

void
store_fcr(sim_cpu *cpu,
	  address_word cia,
	  int fcr,
	  unsigned_word value)
{
  unsigned32 v;

  v = VL4_8(value);
  switch (fcr)
    {
    case 25:  /* FP Condition Codes Register */
      v = (v << 1) | (v & 0x1);             /* adjust for FCC gap */
      FCR31 &= ~fcsr_FCC_mask;
      FCR31 |= ((v << fcsr_FCC_shift) & fcsr_FCC_mask);
      break;
    case 26:  /* FP Exceptions Register */
      FCR31 &= ~(fcsr_CAUSE_mask | fcsr_FLAGS_mask);
      FCR31 |= (v & (fcsr_CAUSE_mask | fcsr_FLAGS_mask));
      test_fcsr(cpu, cia);
      break;
    case 28:  /* FP Enables Register */
      if (v & 0x4)                         /* nonstandard FS bit */
	v |= fcsr_FS;
      else
	v &= ~fcsr_FS;
      FCR31 &= (fcsr_FCC_mask | fcsr_CAUSE_mask | fcsr_FLAGS_mask);
      FCR31 |= (v & (fcsr_FS | fcsr_ENABLES_mask | fcsr_RM_mask));
      test_fcsr(cpu, cia);
      break;
    case 31:  /* FP Control/Status Register */
      FCR31 = v & ~fcsr_ZERO_mask;
      test_fcsr(cpu, cia);
      break;
    }
}

void
update_fcsr (sim_cpu *cpu,
	     address_word cia,
	     sim_fpu_status status)
{
  FCSR &= ~fcsr_CAUSE_mask;

  if (status != 0)
    {
      unsigned int cause = 0;

      /* map between sim_fpu codes and MIPS FCSR */
      if (status & (sim_fpu_status_invalid_snan
		    | sim_fpu_status_invalid_isi
		    | sim_fpu_status_invalid_idi
		    | sim_fpu_status_invalid_zdz
		    | sim_fpu_status_invalid_imz
		    | sim_fpu_status_invalid_cmp
		    | sim_fpu_status_invalid_sqrt
		    | sim_fpu_status_invalid_cvi))
	cause |= (1 << IO);
      if (status & sim_fpu_status_invalid_div0)
	cause |= (1 << DZ);
      if (status & sim_fpu_status_overflow)
	cause |= (1 << OF);
      if (status & sim_fpu_status_underflow)
	cause |= (1 << UF);
      if (status & sim_fpu_status_inexact)
	cause |= (1 << IR);
#if 0 /* Not yet.  */
      /* Implicit clearing of other bits by unimplemented done by callers. */
      if (status & sim_fpu_status_unimplemented)
	cause |= (1 << UO);
#endif

      FCSR |= (cause << fcsr_CAUSE_shift);
      test_fcsr (cpu, cia);
      FCSR |= ((cause & ~(1 << UO)) << fcsr_FLAGS_shift);
    }
  return;
}


/* Comparison operations.  */

static sim_fpu_status
fp_test(unsigned64 op1,
	unsigned64 op2,
	FP_formats fmt,
	int abs,
	int cond,
	int *condition)
{
  sim_fpu wop1;
  sim_fpu wop2;
  sim_fpu_status status = 0;
  int  less, equal, unordered;

  /* The format type has already been checked:  */
  switch (fmt)
    {
    case fmt_single:
      {
	sim_fpu_32to (&wop1, op1);
	sim_fpu_32to (&wop2, op2);
	break;
      }
    case fmt_double:
      {
	sim_fpu_64to (&wop1, op1);
	sim_fpu_64to (&wop2, op2);
	break;
      }
    default:
      fprintf (stderr, "Bad switch\n");
      abort ();
    }

  if (sim_fpu_is_nan (&wop1) || sim_fpu_is_nan (&wop2))
    {
      if ((cond & (1 << 3)) ||
	  sim_fpu_is_snan (&wop1) || sim_fpu_is_snan (&wop2))
	status = sim_fpu_status_invalid_snan;
      less = 0;
      equal = 0;
      unordered = 1;
    }
  else
    {
      if (abs)
	{
	  status |= sim_fpu_abs (&wop1, &wop1);
	  status |= sim_fpu_abs (&wop2, &wop2);
	}
      equal = sim_fpu_is_eq (&wop1, &wop2);
      less = !equal && sim_fpu_is_lt (&wop1, &wop2);
      unordered = 0;
    }
  *condition = (((cond & (1 << 2)) && less)
		|| ((cond & (1 << 1)) && equal)
		|| ((cond & (1 << 0)) && unordered));
  return status;
}

void
fp_cmp(sim_cpu *cpu,
       address_word cia,
       unsigned64 op1,
       unsigned64 op2,
       FP_formats fmt,
       int abs,
       int cond,
       int cc)
{
  sim_fpu_status status = 0;

  /* The format type should already have been checked: */
  switch (fmt)
    {
    case fmt_single:
    case fmt_double:
      {
	int result;
	status = fp_test(op1, op2, fmt, abs, cond, &result);
	update_fcsr (cpu, cia, status);
	SETFCC (cc, result);
	break;
      }
    default:
      sim_io_eprintf (SD, "Bad switch\n");
      abort ();
    }
}


/* Basic arithmetic operations.  */

static unsigned64
fp_unary(sim_cpu *cpu,
	 address_word cia,
	 int (*sim_fpu_op)(sim_fpu *, const sim_fpu *),
	 unsigned64 op,
	 FP_formats fmt)
{
  sim_fpu wop;
  sim_fpu ans;
  unsigned64 result = 0;

  /* The format type has already been checked: */
  switch (fmt)
    {
    case fmt_single:
      {
	unsigned32 res;
	sim_fpu_32to (&wop, op);
	(*sim_fpu_op) (&ans, &wop);
	sim_fpu_to32 (&res, &ans);
	result = res;
	break;
      }
    case fmt_double:
      {
	unsigned64 res;
	sim_fpu_64to (&wop, op);
	(*sim_fpu_op) (&ans, &wop);
	sim_fpu_to64 (&res, &ans);
	result = res;
	break;
      }
    default:
      sim_io_eprintf (SD, "Bad switch\n");
      abort ();
    }

  return result;
}

static unsigned64
fp_binary(sim_cpu *cpu,
	  address_word cia,
	  int (*sim_fpu_op)(sim_fpu *, const sim_fpu *, const sim_fpu *),
	  unsigned64 op1,
	  unsigned64 op2,
	  FP_formats fmt)
{
  sim_fpu wop1;
  sim_fpu wop2;
  sim_fpu ans;
  unsigned64 result = 0;

  /* The format type has already been checked: */
  switch (fmt)
    {
    case fmt_single:
      {
	unsigned32 res;
	sim_fpu_32to (&wop1, op1);
	sim_fpu_32to (&wop2, op2);
	(*sim_fpu_op) (&ans, &wop1, &wop2);
	sim_fpu_to32 (&res, &ans);
	result = res;
	break;
      }
    case fmt_double:
      {
	unsigned64 res;
	sim_fpu_64to (&wop1, op1);
	sim_fpu_64to (&wop2, op2);
	(*sim_fpu_op) (&ans, &wop1, &wop2);
	sim_fpu_to64 (&res, &ans);
	result = res;
	break;
      }
    default:
      sim_io_eprintf (SD, "Bad switch\n");
      abort ();
    }

  return result;
}


unsigned64
fp_abs(sim_cpu *cpu,
       address_word cia,
       unsigned64 op,
       FP_formats fmt)
{
  return fp_unary(cpu, cia, &sim_fpu_abs, op, fmt);
}

unsigned64
fp_neg(sim_cpu *cpu,
       address_word cia,
       unsigned64 op,
       FP_formats fmt)
{
  return fp_unary(cpu, cia, &sim_fpu_neg, op, fmt);
}

unsigned64
fp_add(sim_cpu *cpu,
       address_word cia,
       unsigned64 op1,
       unsigned64 op2,
       FP_formats fmt)
{
  return fp_binary(cpu, cia, &sim_fpu_add, op1, op2, fmt);
}

unsigned64
fp_sub(sim_cpu *cpu,
       address_word cia,
       unsigned64 op1,
       unsigned64 op2,
       FP_formats fmt)
{
  return fp_binary(cpu, cia, &sim_fpu_sub, op1, op2, fmt);
}

unsigned64
fp_mul(sim_cpu *cpu,
       address_word cia,
       unsigned64 op1,
       unsigned64 op2,
       FP_formats fmt)
{
  return fp_binary(cpu, cia, &sim_fpu_mul, op1, op2, fmt);
}

unsigned64
fp_div(sim_cpu *cpu,
       address_word cia,
       unsigned64 op1,
       unsigned64 op2,
       FP_formats fmt)
{
  return fp_binary(cpu, cia, &sim_fpu_div, op1, op2, fmt);
}

unsigned64
fp_recip(sim_cpu *cpu,
         address_word cia,
         unsigned64 op,
         FP_formats fmt)
{
  return fp_unary(cpu, cia, &sim_fpu_inv, op, fmt);
}

unsigned64
fp_sqrt(sim_cpu *cpu,
        address_word cia,
        unsigned64 op,
        FP_formats fmt)
{
  return fp_unary(cpu, cia, &sim_fpu_sqrt, op, fmt);
}


/* Conversion operations.  */

uword64
convert (sim_cpu *cpu,
	 address_word cia,
	 int rm,
	 uword64 op,
	 FP_formats from,
	 FP_formats to)
{
  sim_fpu wop;
  sim_fpu_round round;
  unsigned32 result32;
  unsigned64 result64;

#ifdef DEBUG
#if 0 /* FIXME: doesn't compile */
  printf ("DBG: Convert: mode %s : op 0x%s : from %s : to %s : (PC = 0x%s)\n",
	  fpu_rounding_mode_name (rm), pr_addr (op), fpu_format_name (from),
	  fpu_format_name (to), pr_addr (IPC));
#endif
#endif /* DEBUG */

  switch (rm)
    {
    case FP_RM_NEAREST:
      /* Round result to nearest representable value. When two
	 representable values are equally near, round to the value
	 that has a least significant bit of zero (i.e. is even).  */
      round = sim_fpu_round_near;
      break;
    case FP_RM_TOZERO:
      /* Round result to the value closest to, and not greater in
	 magnitude than, the result.  */
      round = sim_fpu_round_zero;
      break;
    case FP_RM_TOPINF:
      /* Round result to the value closest to, and not less than,
	 the result.  */
      round = sim_fpu_round_up;
      break;

    case FP_RM_TOMINF:
      /* Round result to the value closest to, and not greater than,
	 the result.  */
      round = sim_fpu_round_down;
      break;
    default:
      round = 0;
      fprintf (stderr, "Bad switch\n");
      abort ();
    }

  /* Convert the input to sim_fpu internal format */
  switch (from)
    {
    case fmt_double:
      sim_fpu_64to (&wop, op);
      break;
    case fmt_single:
      sim_fpu_32to (&wop, op);
      break;
    case fmt_word:
      sim_fpu_i32to (&wop, op, round);
      break;
    case fmt_long:
      sim_fpu_i64to (&wop, op, round);
      break;
    default:
      fprintf (stderr, "Bad switch\n");
      abort ();
    }

  /* Convert sim_fpu format into the output */
  /* The value WOP is converted to the destination format, rounding
     using mode RM. When the destination is a fixed-point format, then
     a source value of Infinity, NaN or one which would round to an
     integer outside the fixed point range then an IEEE Invalid
     Operation condition is raised.  */
  switch (to)
    {
    case fmt_single:
      sim_fpu_round_32 (&wop, round, 0);
      sim_fpu_to32 (&result32, &wop);
      result64 = result32;
      break;
    case fmt_double:
      sim_fpu_round_64 (&wop, round, 0);
      sim_fpu_to64 (&result64, &wop);
      break;
    case fmt_word:
      sim_fpu_to32i (&result32, &wop, round);
      result64 = result32;
      break;
    case fmt_long:
      sim_fpu_to64i (&result64, &wop, round);
      break;
    default:
      result64 = 0;
      fprintf (stderr, "Bad switch\n");
      abort ();
    }

#ifdef DEBUG
  printf ("DBG: Convert: returning 0x%s (to format = %s)\n",
	  pr_addr (result64), fpu_format_name (to));
#endif /* DEBUG */

  return (result64);
}

static const char *
fpu_format_name (FP_formats fmt)
{
  switch (fmt)
    {
    case fmt_single:
      return "single";
    case fmt_double:
      return "double";
    case fmt_word:
      return "word";
    case fmt_long:
      return "long";
    case fmt_unknown:
      return "<unknown>";
    case fmt_uninterpreted:
      return "<uninterpreted>";
    case fmt_uninterpreted_32:
      return "<uninterpreted_32>";
    case fmt_uninterpreted_64:
      return "<uninterpreted_64>";
    default:
      return "<format error>";
    }
}

#ifdef DEBUG
static const char *
fpu_rounding_mode_name (int rm)
{
  switch (rm)
    {
    case FP_RM_NEAREST:
      return "Round";
    case FP_RM_TOZERO:
      return "Trunc";
    case FP_RM_TOPINF:
      return "Ceil";
    case FP_RM_TOMINF:
      return "Floor";
    default:
      return "<rounding mode error>";
    }
}
#endif /* DEBUG */
