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
    /* Clear xmm0.  */
    "vxorps %%xmm0, %%xmm0, %%xmm0\n\t"

    /* Move the frame pointer (rbp) to xmm0.  */
    "movq %%rbp, %%xmm0\n\t"

    /* CFI: Frame pointer is in xmm0.  */
    ".cfi_register %%rbp, %%xmm0\n\t"

    /* Clobber list: Specify all modified registers  */
    : // No output operands
    : // No input operands
    : "xmm0"
  );

  dummy ();			/* break-here */

  /* Pseudo-epilogue: Restore rbp from xmm0.  */
  __asm__ volatile (
      /* Restore rbp.  */
      "movq %%xmm0, %%rbp\n\t"

      /* Describe CFI: Frame pointer is restored.  */
      ".cfi_restore %%rbp\n\t"

      /* Clobber list: Specify all modified registers  */
      : /* No output operands.  */
      : /* No input operands.  */
      : /* Despite clobbering rbp, gcc doesn't let us list it here.  */
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
