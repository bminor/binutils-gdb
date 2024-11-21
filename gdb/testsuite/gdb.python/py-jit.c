/* Copyright (C) 2024-2024 Free Software Foundation, Inc.

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

/* "JIT-ed" function, with the prototype `long (long, long)`.  */
static const unsigned char jit_function_add_code[] = {
  0x48, 0x01, 0xfe,		/* add %rdi,%rsi */
  0x48, 0x89, 0xf0,		/* mov %rsi,%rax */
  0xc3,				/* retq */
};

/* Dummy function to inform the debugger a new code has been installed.  */
void jit_register_code (char * name, uintptr_t code, size_t size)
{}

/* Dummy function to inform the debugger that code has been installed.  */
void jit_unregister_code (uintptr_t code)
{}

int
main (int argc, char **argv)
{
  void *code = mmap (NULL, getpagesize (), PROT_WRITE | PROT_EXEC,
		     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert (code != MAP_FAILED);

  /* "Compile" jit_function_add.  */
  memcpy (code, jit_function_add_code,
	  sizeof (jit_function_add_code));
  jit_register_code ("jit_function_add", (uintptr_t)code, sizeof (jit_function_add_code));

  ((long (*)(long, long))code)(1,5); /* breakpoint 1 line */

  /* "Discard" jit_function_add.  */
  memset(code, 0, sizeof(jit_function_add_code));
  jit_unregister_code ((uintptr_t)code);

  return 0; /* breakpoint 2 line */
}
