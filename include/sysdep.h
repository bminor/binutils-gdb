/* All the system include files boiled into one place (sort of).
   Copyright 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of BFD and the GNU Binutils.  Please don't use it for
other programs; better configuration mechanisms than this are needed.

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

/* All the system include files boiled into one place. 

   One day, everyone will have the same set of include files..

   This is ugly, but if you can think of a better way of doing this,
   tell me.  --steve@cygnus.com */

#ifndef _SYSDEP_H
#define _SYSDEP_H

/*
 The including makefile must define HOST_SYS to be one of these.
 Each combination of Machine and OS (and maybe OS Version) must
 have a different number.
 */

#define SUN4_SYS 2
#define POSIX_SYS 3
#define AIX_SYS 4
#define VAX_ULTRIX_SYS 5
#define i386_SYSV_SYS 6
#define SUN3_SYS 7
#define UNKNOWN_SYS 8
#define DGUX_SYS 9
#define DEC3100_SYS 10
#define HP9000_SYS 11
#define APOLLO400_SYS 12
#define DOS_SYS 13
#define	VAX_BSD_SYS 14
#define	TAHOE_BSD_SYS 15
#define RTBSD_SYS 16  /* IBM RT/PC running bsd Unix */

#include <ansidecl.h>

#if __STDC__
#define PROTO(type, name, arglist) type name arglist
#else
#define PROTO(type, name, arglist) type name ()
#define NO_STDARG
#endif

#ifndef HOST_SYS
#define HOST_SYS = Hey_you_HOST_SYS_has_not_been_defined.
#endif

#if HOST_SYS==SUN4_SYS
#define HOST_IS_SUN4 1
#include <sys/h-sun4.h>
#endif

#if HOST_SYS==DGUX_SYS
#include <sys/h-dgux.h>
#endif

#if HOST_SYS==POSIX_SYS
#define HOST_IS_POSIX 1
#endif 

#if HOST_SYS==AIX_SYS
#define HOST_IS_AIX 1
#include <sys/h-rs6000.h>
#endif

#if HOST_SYS==VAX_ULTRIX_SYS
#define HOST_IS_VAX_ULTRIX 1
#include <sys/h-vaxult.h>
#endif

#if HOST_SYS==i386_SYSV_SYS
#define HOST_IS_i386_SYSV 1
#define USG 
#include <sys/h-i386v.h>
#endif

#if HOST_SYS==SUN3_SYS
#define HOST_IS_SUN3 1
#include <sys/h-sun3.h>
#endif

#if HOST_SYS==DEC3100_SYS
#define HOST_IS_DEC3100 1
#include <sys/h-dec3100.h>
#endif

#if HOST_SYS==HP9000_SYS
#define HOST_IS_HP9000 1
#define USG 
#include <sys/h-hp9000.h>
#endif

#if HOST_SYS==APOLLO400_SYS
#define HOST_IS_APOLLO400 1
#include <sys/h-ap400.h>
#endif
 
#if HOST_SYS==DOS_SYS
#define HOST_IS_DOS 1
#include <sys/h_dos.h>
#endif

#if HOST_SYS == VAX_BSD_SYS
#include <sys/h-vaxbsd.h>
#endif

#if HOST_SYS == TAHOE_BSD_SYS
#include <sys/h-tahoebsd.h>
#endif

#if HOST_SYS == RTBSD_SYS	
#include <sys/h-rtbsd.h>
#endif

#endif 
