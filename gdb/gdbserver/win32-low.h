/* Internal interfaces for the Win32 specific target code for gdbserver.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include <windows.h>

/* Thread information structure used to track extra information about
   each thread.  */
typedef struct win32_thread_info
{
  DWORD tid;
  HANDLE h;
  int suspend_count;
  CONTEXT context;
} win32_thread_info;

struct win32_target_ops
{
  /* An array of offset mappings into a Win32 Context structure.
     This is a one-to-one mapping which is indexed by gdb's register
     numbers.  It retrieves an offset into the context structure where
     the 4 byte register is located.
     An offset value of -1 indicates that Win32 does not provide this
     register in it's CONTEXT structure.  In this case regptr will return
     a pointer into a dummy register.  */
  const int *regmap;

  /* The number of elements of regmap.  */
  int num_regs;

  void (*initial_stuff) (void);

  void (*store_debug_registers) (win32_thread_info *);
  void (*load_debug_registers) (win32_thread_info *);

  /* Fetch register(s) from gdbserver regcache data.  */
  void (*fetch_inferior_registers) (win32_thread_info *th, int r);

  void (*single_step) (win32_thread_info *th);

  const unsigned char *breakpoint;
  int breakpoint_len;

  /* What string to report to GDB when it asks for the architecture,
     or NULL not to answer.  */
  const char *arch_string;
};

extern struct win32_target_ops the_low_target;

/* in win32-low.c */

/* Return a pointer into a CONTEXT field indexed by gdb register number.
   Return a pointer to an dummy register holding zero if there is no
   corresponding CONTEXT field for the given register number.  */
extern char * regptr (CONTEXT* c, int r);

/* Map the Windows error number in ERROR to a locale-dependent error
   message string and return a pointer to it.  Typically, the values
   for ERROR come from GetLastError.

   The string pointed to shall not be modified by the application,
   but may be overwritten by a subsequent call to strwinerror

   The strwinerror function does not change the current setting
   of GetLastError.  */
extern char * strwinerror (DWORD error);

/* in wincecompat.c */

extern void to_back_slashes (char *);
