/* Version of strerror() for systems that need it.
   Copyright (C) 1991, 1992 Free Software Foundation, Inc.

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


/*

NAME

	strerror -- map an error number to an error message string

SYNOPSIS

	#include <string.h>

	char *strerror (int errnum)

DESCRIPTION

	Returns a pointer to a string containing an error message, the
	contents of which are implementation defined.  The implementation
	shall behave as if no library function calls strerror.  The string
	pointed to shall not be modified by the caller and is only guaranteed
	to be valid until a subsequent call to strerror.
	
BUGS

	Requires that the system have sys_errlist and sys_nerr.

*/

#ifndef HAVE_STRERROR

extern int sys_nerr;
extern char *sys_errlist[];

char *
strerror (code)
     int code;
{
  return (((code < 0) || (code >= sys_nerr))
	  ? "(unknown error)"
	  : sys_errlist[code]);
}

#endif /* HAVE_STRERROR */

/* end of strerror.c */
