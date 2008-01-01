/* This testcase is part of GDB, the GNU debugger.

   Copyright 2007, 2008 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>

volatile _Decimal32 d32;
volatile _Decimal64 d64;
volatile _Decimal128 d128;

struct decstruct
{
  int int4;
  long long8;
  _Decimal32 dec32;
  _Decimal64 dec64;
  _Decimal128 dec128;
} ds;

static _Decimal32
arg0_32 (_Decimal32 arg0, _Decimal32 arg1, _Decimal32 arg2,
         _Decimal32 arg3, _Decimal32 arg4, _Decimal32 arg5)
{
  return arg0;
}

static _Decimal64
arg0_64 (_Decimal64 arg0, _Decimal64 arg1, _Decimal64 arg2,
         _Decimal64 arg3, _Decimal64 arg4, _Decimal64 arg5)
{
  return arg0;
}

static _Decimal128
arg0_128 (_Decimal128 arg0, _Decimal128 arg1, _Decimal128 arg2,
         _Decimal128 arg3, _Decimal128 arg4, _Decimal128 arg5)
{
  return arg0;
}

int main()
{
  /* An finite 32-bits decimal floating point.  */
  d32 = 1.2345df;		/* Initialize d32.  */

  /* Non-finite 32-bits decimal floating point: infinity and NaN.  */
  d32 = __builtin_infd32();	/* Positive infd32.  */
  d32 = -__builtin_infd32();	/* Negative infd32.  */
  d32 = __builtin_nand32("");

  /* An finite 64-bits decimal floating point.  */
  d64 = 1.2345dd;		/* Initialize d64.  */

  /* Non-finite 64-bits decimal floating point: infinity and NaN.  */
  d64 = __builtin_infd64();	/* Positive infd64.  */
  d64 = -__builtin_infd64();	/* Negative infd64.  */
  d64 = __builtin_nand64("");

  /* An finite 128-bits decimal floating point.  */
  d128 = 1.2345dl;		/* Initialize d128.  */

  /* Non-finite 128-bits decimal floating point: infinity and NaN.  */
  d128 = __builtin_infd128();	/* Positive infd128.  */
  d128 = -__builtin_infd128();	/* Negative infd128.  */
  d128 = __builtin_nand128("");

  /* Functions with decimal floating point as parameter and return value. */
  d32 = arg0_32 (0.1df, 1.0df, 2.0df, 3.0df, 4.0df, 5.0df);
  d64 = arg0_64 (0.1dd, 1.0dd, 2.0dd, 3.0dd, 4.0dd, 5.0dd);
  d128 = arg0_128 (0.1dl, 1.0dl, 2.0dl, 3.0dl, 4.0dl, 5.0dl);

  ds.int4 = 1;
  ds.long8 = 2;
  ds.dec32 = 1.2345df;
  ds.dec64 = 1.2345dd;
  ds.dec128 = 1.2345dl;

  return 0;	/* Exit point.  */
}
