/* Simulator Floating-point support.
   Copyright (C) 1994, 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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



#ifndef SIM_FPU_C
#define SIM_FPU_C

#include "sim-main.h"
#include "sim-fpu.h"
#include "sim-assert.h"

#include <math.h>


/* Floating point number is <SIGN:1><EXP:EXPBITS><FRAC:FRACBITS> */

#define SP_NGARDS    7L
#define SP_GARDROUND 0x3f
#define SP_GARDMASK  0x7f
#define SP_GARDMSB   0x40
#define SP_EXPBITS 8
#define SP_EXPBIAS 127
#define SP_FRACBITS 23
#define SP_EXPMAX (0xff)
#define SP_QUIET_NAN 0x100000L
#define SP_FRAC_NBITS 32
#define SP_FRACHIGH  0x80000000L
#define SP_FRACHIGH2 0xc0000000L

#define DP_NGARDS 8L
#define DP_GARDROUND 0x7f
#define DP_GARDMASK  0xff
#define DP_GARDMSB   0x80
#define DP_EXPBITS 11
#define DP_EXPBIAS 1023
#define DP_FRACBITS 52
#define DP_EXPMAX (0x7ff)
#define DP_QUIET_NAN 0x8000000000000LL
#define DP_FRAC_NBITS 64
#define DP_FRACHIGH  0x8000000000000000LL
#define DP_FRACHIGH2 0xc000000000000000LL

#define EXPMAX (is_double ? DP_EXPMAX : SP_EXPMAX)
#define EXPBITS (is_double ? DP_EXPBITS : SP_EXPBITS)
#define EXPBIAS (is_double ? DP_EXPBIAS : SP_EXPBIAS)
#define FRACBITS (is_double ? DP_FRACBITS : SP_FRACBITS)
#define NGARDS (is_double ? DP_NGARDS : (SP_NGARDS ))
#define SIGNBIT (1LL << (EXPBITS + FRACBITS))
#define FRAC_NBITS (is_double ? DP_FRAC_NBITS : SP_FRAC_NBITS)
#define GARDMASK (is_double ? DP_GARDMASK : SP_GARDMASK)
#define GARDMSB (is_double ? DP_GARDMSB : SP_GARDMSB)
#define GARDROUND (is_double ? DP_GARDROUND : SP_GARDROUND)

/* F_D_BITOFF is the number of bits offset between the MSB of the mantissa
   of a float and of a double. Assumes there are only two float types.
   (double::FRAC_BITS+double::NGARGS-(float::FRAC_BITS-float::NGARDS))
 */
#define F_D_BITOFF (is_double ? 0 : (52+8-(23+7)))


#if 0
#define  (is_double ? DP_ : SP_)
#endif

#define NORMAL_EXPMIN (-(EXPBIAS)+1)

#define IMPLICIT_1 (1LL<<(FRACBITS+NGARDS))
#define IMPLICIT_2 (1LL<<(FRACBITS+1+NGARDS))

#define MAX_SI_INT   (is_double ? LSMASK64 (63) : LSMASK64 (31))
#define MAX_USI_INT  (is_double ? LSMASK64 (64) : LSMASK64 (32))


typedef enum 
{
  sim_fpu_class_snan,
  sim_fpu_class_qnan,
  sim_fpu_class_zero,
  sim_fpu_class_number,
  sim_fpu_class_infinity,
} sim_fpu_class;

typedef struct _sim_ufpu {
  sim_fpu_class class;
  int normal_exp;
  int sign;
  unsigned64 fraction;
  union {
    double d;
    unsigned64 i;
  } val;
} sim_ufpu;


STATIC_INLINE_SIM_FPU (unsigned64)
pack_fpu (const sim_ufpu *src, int is_double)
{
  unsigned64 fraction;
  unsigned64 exp;
  int sign;

  switch (src->class)
    {
    default:
      /* create a NaN */
    case sim_fpu_class_qnan:
    case sim_fpu_class_snan:
      sign = 1; /* fixme - always a qNaN */
      exp = EXPMAX;
      fraction = src->fraction;
      break;
    case sim_fpu_class_infinity:
      sign = src->sign;
      exp = EXPMAX;
      fraction = 0;
      break;
    case sim_fpu_class_zero:
      sign = src->sign;
      exp = 0;
      fraction = 0;
      break;
    case sim_fpu_class_number:
      if (src->normal_exp < NORMAL_EXPMIN)
	{
	  /* This number's exponent is too low to fit into the bits
	     available in the number, so we'll store 0 in the exponent and
	     shift the fraction to the right to make up for it.  */

	  int shift = NORMAL_EXPMIN - src->normal_exp;

	  sign = src->sign;
	  exp = 0;

	  if (shift > (FRAC_NBITS - NGARDS))
	    {
	      /* No point shifting, since it's more that 64 out.  */
	      fraction = 0;
	    }
	  else
	    {
	      /* Shift by the value */
	      fraction = src->fraction >> F_D_BITOFF;
	      fraction >>= shift;
	      fraction >>= NGARDS;
	    }
	}
      else if (src->normal_exp > EXPBIAS)
	{
	  /* Infinity */
	  sign = src->sign;
	  exp = EXPMAX;
	  fraction = 0; 
	}
      else
	{
	  sign = src->sign;
	  exp = (src->normal_exp + EXPBIAS);
	  fraction = src->fraction >> F_D_BITOFF;
	  /* IF the gard bits are the all zero, but the first, then we're
	     half way between two numbers, choose the one which makes the
	     lsb of the answer 0.  */
	  if ((fraction & GARDMASK) == GARDMSB)
	    {
	      if (fraction & (1 << NGARDS))
		fraction += GARDROUND + 1;
	    }
	  else
	    {
	      /* Add a one to the guards to round up */
	      fraction += GARDROUND;
	    }
	  if (fraction >= IMPLICIT_2)
	    {
	      fraction >>= 1;
	      exp += 1;
	    }
	  fraction >>= NGARDS;
	}
    }

  return ((sign ? SIGNBIT : 0)
	  | (exp << FRACBITS)
	  | LSMASKED64 (fraction, FRACBITS));
}


STATIC_INLINE_SIM_FPU (void)
unpack_fpu (sim_ufpu *dst, unsigned64 s, int is_double)
{
  unsigned64 fraction = LSMASKED64 (s, FRACBITS);
  unsigned exp = LSMASKED64 (s >> FRACBITS, EXPBITS);

  dst->sign = (s & SIGNBIT) != 0;

  if (exp == 0)
    {
      /* Hmm.  Looks like 0 */
      if (fraction == 0)
	{
	  /* tastes like zero */
	  dst->class = sim_fpu_class_zero;
	}
      else
	{
	  /* Zero exponent with non zero fraction - it's denormalized,
	     so there isn't a leading implicit one - we'll shift it so
	     it gets one.  */
	  dst->normal_exp = exp - EXPBIAS + 1;
	  fraction <<= NGARDS;

	  dst->class = sim_fpu_class_number;
	  while (fraction < IMPLICIT_1)
	    {
	      fraction <<= 1;
	      dst->normal_exp--;
	    }
	  dst->fraction = fraction << F_D_BITOFF;
	}
    }
  else if (exp == EXPMAX)
    {
      /* Huge exponent*/
      if (fraction == 0)
	{
	  /* Attached to a zero fraction - means infinity */
	  dst->class = sim_fpu_class_infinity;
	}
      else
	{
	  /* Non zero fraction, means nan */
	  if (dst->sign)
	    {
	      dst->class = sim_fpu_class_snan;
	    }
	  else
	    {
	      dst->class = sim_fpu_class_qnan;
	    }
	  /* Keep the fraction part as the nan number */
	  dst->fraction = fraction << F_D_BITOFF;
	}
    }
  else
    {
      /* Nothing strange about this number */
      dst->normal_exp = exp - EXPBIAS;
      dst->class = sim_fpu_class_number;
      dst->fraction = ((fraction << NGARDS) | IMPLICIT_1) << F_D_BITOFF;
    }

  /* sanity checks */
  dst->val.i = -1;
  dst->val.i = pack_fpu (dst, 1);
  {
    if (is_double)
      {
	ASSERT (dst->val.i == s);
      }
    else
      {
	unsigned32 val = pack_fpu (dst, 0);
	unsigned32 org = s;
	ASSERT (val == org);
      }
  }
}

STATIC_INLINE_SIM_FPU (sim_fpu)
ufpu2fpu (const sim_ufpu *d)
{
  sim_fpu ans;
  ans.val.i = pack_fpu (d, 1);
  return ans;
}


STATIC_INLINE_SIM_FPU (sim_ufpu)
fpu2ufpu (const sim_fpu *d)
{
  sim_ufpu ans;
  unpack_fpu (&ans, d->val.i, 1);
  return ans;
}

STATIC_INLINE_SIM_FPU (int)
is_ufpu_number (const sim_ufpu *d)
{
  switch (d->class)
    {
    case sim_fpu_class_zero:
    case sim_fpu_class_number:
      return 1;
    default:
      return 0;
    }
}


STATIC_INLINE_SIM_FPU (int)
is_ufpu_nan (const sim_ufpu *d)
{
  switch (d->class)
    {
    case sim_fpu_class_qnan:
    case sim_fpu_class_snan:
      return 1;
    default:
      return 0;
    }
}


STATIC_INLINE_SIM_FPU (int)
is_ufpu_zero (const sim_ufpu *d)
{
  switch (d->class)
    {
    case sim_fpu_class_zero:
      return 1;
    default:
      return 0;
    }
}


STATIC_INLINE_SIM_FPU (int)
is_ufpu_inf (const sim_ufpu *d)
{
  switch (d->class)
    {
    case sim_fpu_class_infinity:
      return 1;
    default:
      return 0;
    }
}


STATIC_INLINE_SIM_FPU (sim_fpu)
fpu_nan (void)
{
  sim_ufpu tmp;
  tmp.class = sim_fpu_class_snan;
  tmp.fraction = 0;
  tmp.sign = 1;
  tmp.normal_exp = 0;
  return ufpu2fpu (&tmp);
}


STATIC_INLINE_SIM_FPU (signed64)
fpu2i (sim_fpu s, int is_double)
{
  sim_ufpu a = fpu2ufpu (&s);
  unsigned64 tmp;
  if (is_ufpu_zero (&a))
    return 0;
  if (is_ufpu_nan (&a))
    return 0;
  /* get reasonable MAX_SI_INT... */
  if (is_ufpu_inf (&a))
    return a.sign ? MAX_SI_INT : (-MAX_SI_INT)-1;
  /* it is a number, but a small one */
  if (a.normal_exp < 0)
    return 0;
  if (a.normal_exp > (FRAC_NBITS - 2))
    return a.sign ? (-MAX_SI_INT)-1 : MAX_SI_INT;
  if (a.normal_exp > (FRACBITS + NGARDS + F_D_BITOFF))
    tmp = (a.fraction << (a.normal_exp - (FRACBITS + NGARDS)));
  else
    tmp = (a.fraction >> ((FRACBITS + NGARDS + F_D_BITOFF) - a.normal_exp));
  return a.sign ? (-tmp) : (tmp);
}

STATIC_INLINE_SIM_FPU (unsigned64)
fpu2u (sim_fpu s, int is_double)
{
  sim_ufpu a = fpu2ufpu (&s);
  unsigned64 tmp;
  if (is_ufpu_zero (&a))
    return 0;
  if (is_ufpu_nan (&a))
    return 0;
  /* get reasonable MAX_USI_INT... */
  if (is_ufpu_inf (&a))
    return a.sign ? MAX_USI_INT : 0;
  /* it is a negative number */
  if (a.sign)
    return 0;
  /* it is a number, but a small one */
  if (a.normal_exp < 0)
    return 0;
  if (a.normal_exp > (FRAC_NBITS - 1))
    return MAX_USI_INT;
  if (a.normal_exp > (FRACBITS + NGARDS + F_D_BITOFF))
    tmp = (a.fraction << (a.normal_exp - (FRACBITS + NGARDS + F_D_BITOFF)));
  else
    tmp = (a.fraction >> ((FRACBITS + NGARDS + F_D_BITOFF) - a.normal_exp));
  return tmp;
}


/* register <-> sim_fpu */

INLINE_SIM_FPU (sim_fpu)
sim_fpu_32to (unsigned32 s)
{
  sim_ufpu tmp;
  unpack_fpu (&tmp, s, 0);
  return ufpu2fpu (&tmp);
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_64to (unsigned64 s)
{
  sim_fpu ans;
  ans.val.i = s;
  return ans;
}


INLINE_SIM_FPU (unsigned32)
sim_fpu_to32 (sim_fpu l)
{
  /* convert to single safely */
  sim_ufpu tmp = fpu2ufpu (&l);
  return pack_fpu (&tmp, 0);
}


INLINE_SIM_FPU (unsigned64)
sim_fpu_to64 (sim_fpu s)
{
  return s.val.i;
}


/* Arithmetic ops */

INLINE_SIM_FPU (sim_fpu)
sim_fpu_add (sim_fpu l,
	     sim_fpu r)
{
  sim_fpu ans;
  ans.val.d = l.val.d + r.val.d;
  return ans;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_sub (sim_fpu l,
	     sim_fpu r)
{
  sim_fpu ans;
  ans.val.d = l.val.d - r.val.d;
  return ans;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_mul (sim_fpu l,
	     sim_fpu r)
{
  sim_fpu ans;
  ans.val.d = l.val.d * r.val.d;
  return ans;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_div (sim_fpu l,
	     sim_fpu r)
{
  const int is_double = 1;
  sim_ufpu a = fpu2ufpu (&l);
  sim_ufpu b = fpu2ufpu (&r);
  unsigned64 bit;
  unsigned64 numerator;
  unsigned64 denominator;
  unsigned64 quotient;

  if (is_ufpu_nan (&a))
    {
      return ufpu2fpu (&a);
    }
  if (is_ufpu_nan (&b))
    {
      return ufpu2fpu (&b);
    }
  if (is_ufpu_inf (&a) || is_ufpu_zero (&a))
    {
      if (a.class == b.class)
	return fpu_nan ();
      return l;
    }
  a.sign = a.sign ^ b.sign;

  if (is_ufpu_inf (&b))
    {
      a.fraction = 0;
      a.normal_exp = 0;
      return ufpu2fpu (&a);
    }
  if (is_ufpu_zero (&b))
    {
      a.class = sim_fpu_class_infinity;
      return ufpu2fpu (&a);
    }

  /* Calculate the mantissa by multiplying both 64bit numbers to get a
     128 bit number */
  {
    /* quotient =
       ( numerator / denominator) * 2^(numerator exponent -  denominator exponent)
     */

    a.normal_exp = a.normal_exp - b.normal_exp;
    numerator = a.fraction;
    denominator = b.fraction;

    if (numerator < denominator)
      {
	/* Fraction will be less than 1.0 */
	numerator *= 2;
	a.normal_exp--;
      }
    bit = IMPLICIT_1;
    quotient = 0;
    /* ??? Does divide one bit at a time.  Optimize.  */
    while (bit)
      {
	if (numerator >= denominator)
	  {
	    quotient |= bit;
	    numerator -= denominator;
	  }
	bit >>= 1;
	numerator *= 2;
      }

    if ((quotient & GARDMASK) == GARDMSB)
      {
	if (quotient & (1 << NGARDS))
	  {
	    /* half way, so round to even */
	    quotient += GARDROUND + 1;
	  }
	else if (numerator)
	  {
	    /* but we really weren't half way, more bits exist */
	    quotient += GARDROUND + 1;
	  }
      }

    a.fraction = quotient;
    return ufpu2fpu (&a);
  }
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_inv (sim_fpu r)
{
  sim_fpu ans;
  ans.val.d = 1 / r.val.d;
  return ans;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_sqrt (sim_fpu r)
{
  sim_fpu ans;
  ans.val.d = sqrt (r.val.d);
  return ans;
}


/* int/long -> sim_fpu */

INLINE_SIM_FPU (sim_fpu)
sim_fpu_i32to (signed32 s)
{
  sim_fpu ans;
  ans.val.d = s;
  return ans;
}


INLINE_SIM_FPU (signed32)
sim_fpu_to32i (sim_fpu s)
{
  return fpu2i (s, 0);
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_u32to (unsigned32 s)
{
  sim_fpu ans;
  ans.val.d = s;
  return ans;
}


INLINE_SIM_FPU (unsigned32)
sim_fpu_to32u (sim_fpu s)
{
  return fpu2u (s, 0);
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_i64to (signed64 s)
{
  sim_fpu ans;
  ans.val.d = s;
  return ans;
}


INLINE_SIM_FPU (signed64)
sim_fpu_to64i (sim_fpu s)
{
  return fpu2i (s, 1);
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_u64to (unsigned64 s)
{
  sim_fpu ans;
  ans.val.d = s;
  return ans;
}


INLINE_SIM_FPU (unsigned64)
sim_fpu_to64u (sim_fpu s)
{
  return fpu2u (s, 1);
}


/* sim_fpu -> host format */

INLINE_SIM_FPU (float)
sim_fpu_2f (sim_fpu f)
{
  return f.val.d;
}


INLINE_SIM_FPU (double)
sim_fpu_2d (sim_fpu s)
{
  return s.val.d;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_f2 (float f)
{
  sim_fpu ans;
  ans.val.d = f;
  return ans;
}


INLINE_SIM_FPU (sim_fpu)
sim_fpu_d2 (double d)
{
  sim_fpu ans;
  ans.val.d = d;
  return ans;
}


/* General */

INLINE_SIM_FPU (int)
sim_fpu_is_nan (sim_fpu d)
{
  sim_ufpu tmp = fpu2ufpu (&d);
  return is_ufpu_nan (&tmp);
}


/* Compare operators */

INLINE_SIM_FPU (int)
sim_fpu_is_lt (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d < r.val.d);
  else
    return 0;
}

INLINE_SIM_FPU (int)
sim_fpu_is_le (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d <= r.val.d);
  else
    return 0;
}

INLINE_SIM_FPU (int)
sim_fpu_is_eq (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d == r.val.d);
  else
    return 0;
}

INLINE_SIM_FPU (int)
sim_fpu_is_ne (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d != r.val.d);
  else
    return 0;
}

INLINE_SIM_FPU (int)
sim_fpu_is_ge (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d >= r.val.d);
  else
    return 0;
}

INLINE_SIM_FPU (int)
sim_fpu_is_gt (sim_fpu l,
	       sim_fpu r)
{
  sim_ufpu tl = fpu2ufpu (&l);
  sim_ufpu tr = fpu2ufpu (&r);
  if (is_ufpu_number (&tl) && is_ufpu_number (&tr))
    return (l.val.d > r.val.d);
  else
    return 0;
}

#endif
