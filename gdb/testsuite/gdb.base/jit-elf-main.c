/* This test program is part of GDB, the GNU debugger.

   Copyright 2011-2020 Free Software Foundation, Inc.

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

/* Simulate loading of JIT code.  */

#include <elf.h>
#include <link.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jit-protocol.h"

/* ElfW is coming from linux. On other platforms it does not exist.
   Let us define it here. */
#ifndef ElfW
# if (defined  (_LP64) || defined (__LP64__)) 
#   define WORDSIZE 64
# else
#   define WORDSIZE 32
# endif /* _LP64 || __LP64__  */
#define ElfW(type)      _ElfW (Elf, WORDSIZE, type)
#define _ElfW(e,w,t)    _ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)  e##w##t
#endif /* !ElfW  */

static void
usage (void)
{
  fprintf (stderr, "Usage: jit-elf-main libraries...\n");
  exit (1);
}

/* Defined by the .exp file if testing attach.  */
#ifndef ATTACH
#define ATTACH 0
#endif

#ifndef MAIN
#define MAIN main
#endif

/* Must be defined by .exp file when compiling to know
   what address to map the ELF binary to.  */
#ifndef LOAD_ADDRESS
#error "Must define LOAD_ADDRESS"
#endif
#ifndef LOAD_INCREMENT
#error "Must define LOAD_INCREMENT"
#endif

/* Used to spin waiting for GDB.  */
volatile int wait_for_gdb = ATTACH;
#define WAIT_FOR_GDB do {} while (wait_for_gdb)

/* The current process's PID.  GDB retrieves this.  */
int mypid;

int
MAIN (int argc, char *argv[])
{
  int i;
  alarm (300);
  /* Used as backing storage for GDB to populate argv.  */
  char *fake_argv[10];

  mypid = getpid ();
  /* gdb break here 0  */

  if (argc < 2)
    {
      usage ();
      exit (1);
    }

  for (i = 1; i < argc; ++i)
    {
      struct stat st;
      int fd;

      printf ("%s:%d: libname = %s, i = %d\n", __FILE__, __LINE__, argv[i], i);
      if ((fd = open (argv[i], O_RDONLY)) == -1)
	{
	  fprintf (stderr, "open (\"%s\", O_RDONLY): %s\n", argv[i],
		   strerror (errno));
	  exit (1);
	}

      if (fstat (fd, &st) != 0)
	{
	  fprintf (stderr, "fstat (\"%d\"): %s\n", fd, strerror (errno));
	  exit (1);
	}

      void *load_addr = (void *) (size_t) (LOAD_ADDRESS + (i - 1) * LOAD_INCREMENT);
      const void *const addr = mmap (load_addr, st.st_size, PROT_READ|PROT_WRITE,
				     MAP_PRIVATE | MAP_FIXED, fd, 0);
      struct jit_code_entry *const entry = calloc (1, sizeof (*entry));

      if (addr == MAP_FAILED)
	{
	  fprintf (stderr, "mmap: %s\n", strerror (errno));
	  exit (1);
	}

      /* Link entry at the end of the list.  */
      entry->symfile_addr = (const char *)addr;
      entry->symfile_size = st.st_size;
      entry->prev_entry = __jit_debug_descriptor.relevant_entry;
      __jit_debug_descriptor.relevant_entry = entry;

      if (entry->prev_entry != NULL)
	entry->prev_entry->next_entry = entry;
      else
	__jit_debug_descriptor.first_entry = entry;

      /* Notify GDB.  */
      __jit_debug_descriptor.action_flag = JIT_REGISTER;
      __jit_debug_register_code ();
    }

  WAIT_FOR_GDB; i = 0;  /* gdb break here 1 */

  /* Now unregister them all in reverse order.  */
  while (__jit_debug_descriptor.relevant_entry != NULL)
    {
      struct jit_code_entry *const entry =
	__jit_debug_descriptor.relevant_entry;
      struct jit_code_entry *const prev_entry = entry->prev_entry;

      if (prev_entry != NULL)
	{
	  prev_entry->next_entry = NULL;
	  entry->prev_entry = NULL;
	}
      else
	__jit_debug_descriptor.first_entry = NULL;

      /* Notify GDB.  */
      __jit_debug_descriptor.action_flag = JIT_UNREGISTER;
      __jit_debug_register_code ();

      __jit_debug_descriptor.relevant_entry = prev_entry;
      free (entry);
    }

  WAIT_FOR_GDB; return 0;  /* gdb break here 2  */
}
