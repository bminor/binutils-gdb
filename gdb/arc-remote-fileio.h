/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Author:
      Richard Stuckey <richard.stuckey@arc.com>

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

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This header file defines facilities for intercepting I/O (and other)   */
/*     operations attempted on an ARC target and performing them on the host, */
/*     using a RPC (Remote Procedure Call) mechanism.                         */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_REMOTE_FILEIO
#define ARC_REMOTE_FILEIO

/* ARC header files */
#include "arc-support.h"


typedef enum
{
    INTERCEPTION_RESUME,
    INTERCEPTION_HALT,
    INTERCEPTION_EXIT,
    INTERCEPTION_INTERRUPT
} InterceptionAction;


typedef enum
{
    INTERCEPTION_RESET,
    INTERCEPTION_ON,
    INTERCEPTION_OFF
} InterceptionState;


void arc_set_IO_interception (TargetOperations *operations,
                              InterceptionState state);

InterceptionAction arc_check_interception_breakpoint (int *exitcode);

#endif /* ARC_REMOTE_FILEIO */
/******************************************************************************/
