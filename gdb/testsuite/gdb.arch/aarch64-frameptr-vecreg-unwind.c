/* Copyright 2025 Free Software Foundation, Inc.

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

volatile void dummy () {}

long test_function(void)
{
  __asm__ volatile (
    /* Zero d0 (64-bit vector register part of v0). */
    "movi d0, #0\n\t"

    /* Move the frame pointer (x29) to d0 using fmov. */
    "fmov d0, x29\n\t"

    /* Describe CFI: Frame pointer is now in d0. */
    ".cfi_register x29, d0\n\t"

    /* Clobber list: Specify all modified registers. */
    : /* No output operands. */
    : /* No input operands. */
    : "d0"
  );

  dummy ();                     /* break-here */

  __asm__ volatile (
    /* Restore the frame pointer (x29) from d0 using fmov. */
    "fmov x29, d0\n\t"

    /* Describe CFI: Frame pointer is restored. */
    ".cfi_restore x29\n\t"

    /* Clobber list: Specify all modified registers. */
    : /* No output operands. */
    : /* No input operands. */
    : "x29", "d0"
  );

  return 0;
}

int
main ()
{
  long result = test_function ();
  dummy ();
  return 0;
}
