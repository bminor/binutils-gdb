/* Host definitions for GDB running on an alpha under OSF/1
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

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

#if !defined (HOST_BYTE_ORDER)
#define HOST_BYTE_ORDER LITTLE_ENDIAN
#endif

/* Get rid of any system-imposed stack limit if possible.  */
#define	SET_STACK_LIMIT_HUGE

/* The alpha has no siginterrupt routine.  */
#define NO_SIGINTERRUPT

/* The alpha stores the first six float parameters below the va_list.
   We make no distinction and store the first six parameters below
   and above the va_list.  */
#define VA_FLOAT_ARGS_SIZE (6*8)
#define MAKEVA_SIZE(nargs, max_arg_size) \
  return sizeof (makeva_list) + nargs * max_arg_size + VA_FLOAT_ARGS_SIZE;
#define MAKEVA_START(list) \
  list->argindex = VA_FLOAT_ARGS_SIZE;
#define MAKEVA_ARG(list, argaddr, argsize)				  \
{									  \
  memcpy (&list->aligner.arg_bytes[list->argindex], argaddr, argsize);	  \
  if ((list->argindex - VA_FLOAT_ARGS_SIZE) < VA_FLOAT_ARGS_SIZE)	  \
    memcpy (&list->aligner.arg_bytes[list->argindex - VA_FLOAT_ARGS_SIZE],\
	    argaddr, argsize);						  \
  list->argindex += argsize;						  \
}
#ifdef __GNUC__							
#define MAKEVA_END(list)  					\
{ 								\
  va_list ret;							\
  ret.__base = &(list)->aligner.arg_bytes[VA_FLOAT_ARGS_SIZE];	\
  ret.__offset = 0;						\
  return ret;							\
}
#else
#define MAKEVA_END(list)  					\
{ 								\
  va_list ret;							\
  ret.a0 = &(list)->aligner.arg_bytes[VA_FLOAT_ARGS_SIZE];	\
  ret.offset = 0;						\
  return ret;							\
}
#endif

/* HAVE_SGTTY also works, but we have termios, so use it.  */

#define HAVE_TERMIOS
