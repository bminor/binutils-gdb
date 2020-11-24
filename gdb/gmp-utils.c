/* Copyright (C) 2019-2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "gmp-utils.h"

/* See gmp-utils.h.  */

std::string
gmp_string_printf (const char *fmt, ...)
{
  va_list vp;

  va_start (vp, fmt);
  int size = gmp_vsnprintf (NULL, 0, fmt, vp);
  va_end (vp);

  std::string str (size, '\0');

  /* C++11 and later guarantee std::string uses contiguous memory and
     always includes the terminating '\0'.  */
  va_start (vp, fmt);
  gmp_vsprintf (&str[0], fmt, vp);
  va_end (vp);

  return str;
}

/* See gmp-utils.h.  */

void
gdb_mpz::read (gdb::array_view<const gdb_byte> buf, enum bfd_endian byte_order,
	       bool unsigned_p)
{
  mpz_import (val, 1 /* count */, -1 /* order */, buf.size () /* size */,
	      byte_order == BFD_ENDIAN_BIG ? 1 : -1 /* endian */,
	      0 /* nails */, buf.data () /* op */);

  if (!unsigned_p)
    {
      /* The value was imported as if it was a positive value,
	 as mpz_import does not handle signs. If the original value
	 was in fact negative, we need to adjust VAL accordingly.  */
      gdb_mpz max;

      mpz_ui_pow_ui (max.val, 2, buf.size () * TARGET_CHAR_BIT - 1);
      if (mpz_cmp (val, max.val) >= 0)
	mpz_submul_ui (val, max.val, 2);
    }
}

/* See gmp-utils.h.  */

void
gdb_mpz::write (gdb::array_view<gdb_byte> buf, enum bfd_endian byte_order,
		bool unsigned_p) const
{
  gdb_mpz exported_val (val);

  if (mpz_cmp_ui (val, 0) < 0)
    {
      /* mpz_export does not handle signed values, so create a positive
	 value whose bit representation as an unsigned of the same length
	 would be the same as our negative value.  */
      gdb_mpz neg_offset;

      mpz_ui_pow_ui (neg_offset.val, 2, buf.size () * TARGET_CHAR_BIT);
      mpz_add (exported_val.val, exported_val.val, neg_offset.val);
    }

  /* Start by clearing the buffer, as mpz_export only writes as many
     bytes as it needs (including none, if the value to export is zero.  */
  memset (buf.data (), 0, buf.size ());
  mpz_export (buf.data (), NULL /* count */, -1 /* order */,
	      buf.size () /* size */,
	      byte_order == BFD_ENDIAN_BIG ? 1 : -1 /* endian */,
	      0 /* nails */, exported_val.val);
}

/* See gmp-utils.h.  */

gdb_mpz
gdb_mpq::get_rounded () const
{
  /* Work with a positive number so as to make the "floor" rounding
     always round towards zero.  */

  gdb_mpq abs_val (val);
  mpq_abs (abs_val.val, abs_val.val);

  /* Convert our rational number into a quotient and remainder,
     with "floor" rounding, which in our case means rounding
     towards zero.  */

  gdb_mpz quotient, remainder;
  mpz_fdiv_qr (quotient.val, remainder.val,
	       mpq_numref (abs_val.val), mpq_denref (abs_val.val));

  /* Multiply the remainder by 2, and see if it is greater or equal
     to abs_val's denominator.  If yes, round to the next integer.  */

  mpz_mul_ui (remainder.val, remainder.val, 2);
  if (mpz_cmp (remainder.val, mpq_denref (abs_val.val)) >= 0)
    mpz_add_ui (quotient.val, quotient.val, 1);

  /* Re-apply the sign if needed.  */
  if (mpq_sgn (val) < 0)
    mpz_neg (quotient.val, quotient.val);

  return quotient;
}

/* See gmp-utils.h.  */

void
gdb_mpq::read_fixed_point (gdb::array_view<const gdb_byte> buf,
			   enum bfd_endian byte_order, bool unsigned_p,
			   const gdb_mpq &scaling_factor)
{
  gdb_mpz vz;
  vz.read (buf, byte_order, unsigned_p);

  mpq_set_z (val, vz.val);
  mpq_mul (val, val, scaling_factor.val);
}

/* See gmp-utils.h.  */

void
gdb_mpq::write_fixed_point (gdb::array_view<gdb_byte> buf,
			    enum bfd_endian byte_order, bool unsigned_p,
			    const gdb_mpq &scaling_factor) const
{
  gdb_mpq unscaled (val);

  mpq_div (unscaled.val, unscaled.val, scaling_factor.val);

  gdb_mpz unscaled_z = unscaled.get_rounded ();
  unscaled_z.write (buf, byte_order, unsigned_p);
}

/* A wrapper around xrealloc that we can then register with GMP
   as the "realloc" function.  */

static void *
xrealloc_for_gmp (void *ptr, size_t old_size, size_t new_size)
{
  return xrealloc (ptr, new_size);
}

/* A wrapper around xfree that we can then register with GMP
   as the "free" function.  */

static void
xfree_for_gmp (void *ptr, size_t size)
{
  xfree (ptr);
}

void _initialize_gmp_utils ();

void
_initialize_gmp_utils ()
{
  /* Tell GMP to use GDB's memory management routines.  */
  mp_set_memory_functions (xmalloc, xrealloc_for_gmp, xfree_for_gmp);
}
