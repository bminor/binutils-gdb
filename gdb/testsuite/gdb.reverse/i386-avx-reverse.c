/* This testcase is part of GDB, the GNU debugger.

   Copyright 2023 Free Software Foundation, Inc.

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

/* Architecture tests for intel i386 platform.  */

#include <stdlib.h>

char global_buf0[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
char global_buf1[] = {0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 0, 0, 0, 0};
char *dyn_buf0;
char *dyn_buf1;

int
vmov_test ()
{
  char buf0[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};
  char buf1[] = {0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0};

  /*start vmov_test.  */

  /* Operations on registers.  */
  asm volatile ("mov $0, %rcx");
  asm volatile ("mov $0xbeef, %rax");
  asm volatile ("vmovd %rax, %xmm0");
  asm volatile ("vmovd %xmm0, %rcx");
  asm volatile ("vmovq %xmm0, %xmm15");
  asm volatile ("vmovq %0, %%xmm15": : "m" (buf1));

  /* Operations based on local buffers.  */
  asm volatile ("vmovd %0, %%xmm0": : "m"(buf0));
  asm volatile ("vmovd %%xmm0, %0": "=m"(buf1));
  asm volatile ("vmovq %0, %%xmm0": : "m"(buf0));
  asm volatile ("vmovq %%xmm0, %0": "=m"(buf1));

  /* Operations based on global buffers.  */
  asm volatile ("vmovd %0, %%xmm0": : "m"(global_buf0));
  asm volatile ("vmovd %%xmm0, %0": "=m"(global_buf1));
  asm volatile ("vmovq %0, %%xmm0": : "m"(global_buf0));
  asm volatile ("vmovq %%xmm0, %0": "=m"(global_buf1));

  /* Operations based on dynamic buffers.  */
  asm volatile ("vmovd %0, %%xmm0": : "m"(*dyn_buf0));
  asm volatile ("vmovd %%xmm0, %0": "=m"(*dyn_buf1));
  asm volatile ("vmovq %0, %%xmm0": : "m"(*dyn_buf0));
  asm volatile ("vmovq %%xmm0, %0": "=m"(*dyn_buf1));

  /* Reset all relevant buffers. */
  asm volatile ("vmovq %%xmm15, %0": "=m" (buf1));
  asm volatile ("vmovq %%xmm15, %0": "=m" (global_buf1));
  asm volatile ("vmovq %%xmm15, %0": "=m" (*dyn_buf1));

  /* Quick test for a different xmm register.  */
  asm volatile ("vmovd %0, %%xmm15": "=m" (buf0));
  asm volatile ("vmovd %0, %%xmm15": "=m" (buf1));
  asm volatile ("vmovq %0, %%xmm15": "=m" (buf0));
  asm volatile ("vmovq %0, %%xmm15": "=m" (buf1));

  /* We have a return statement to deal with
     epilogue in different compilers.  */
  return 0; /* end vmov_test */
}

int
main ()
{
  dyn_buf0 = (char *) malloc(sizeof(char) * 16);
  dyn_buf1 = (char *) malloc(sizeof(char) * 16);
  for (int i =0; i < 16; i++)
    {
      dyn_buf0[i] = 0x20 + i;
      dyn_buf1[i] = 0;
    }
  /* Zero relevant xmm registers, se we know what to look for.  */
  asm volatile ("vmovq %0, %%xmm0": : "m" (global_buf1));
  asm volatile ("vmovq %0, %%xmm15": : "m" (global_buf1));

  vmov_test ();
  return 0;	/* end of main */
}
