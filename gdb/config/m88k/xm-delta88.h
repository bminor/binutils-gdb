/* Host machine description for Motorola Delta 88 system, for GDB.
   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992
   Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define HOST_BYTE_ORDER BIG_ENDIAN

#if !defined (USG)
#define USG 1
#endif

#define TIOCGETC_BROKEN 1
#define TIOCGLTC_BROKEN 1

#include <sys/param.h>
#include <sys/time.h>

/* Required by <sys/ptrace.h>.  */
#include <sys/siginfo.h>

#define HAVE_TERMIO

/*#define USIZE 2048*/
/*#define NBPG NBPC*/
/* Might be defined in <sys/param.h>.  I suspect this define was a relic
   from before when BFD did core files.  */
/* #define UPAGES USIZE */

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

/* Since registers r0 through r31 are stored directly in the struct ptrace_user,
   (for m88k BCS)
   the ptrace_user offsets are sufficient and KERNEL_U_ADDRESS can be 0 */

#define KERNEL_U_ADDR 0

/* Like vprintf, but takes a a pointer to the args, laid out in order,
   rather than a va_list.  */
/* Does this really work, or is it just enough to get this to compile?  */
#define VPRINTF(string, args) \
  { \
    __gnuc_va_list list; \
    list.__va_arg = 0; \
    list.__va_stk = (int *) args; \
    list.__va_reg = (int *) args; \
    vprintf (string, list); \
  }
