/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
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
/*     This header file defines the interface to hardware actionpoints in the */
/*     ARC processor.                                                         */
/*                                                                            */
/* Usage:                                                                     */
/*     Once the connection to the target has been established, the function   */
/*     arc_initialize_actionpoint_ops should be called: this will update the  */
/*     debug_ops structure according to the support provided by the ARC       */
/*     processor in the target for hardware breakpoints and watchpoints.      */
/*                                                                            */
/*     The function arc_restore_actionpoints_after_reset should be called     */
/*     after the target board has been reset, in order to re-establish any    */
/*     actionpoints that have been set, as they will have been cleared by     */
/*     the reset; it returns TRUE if successful.                              */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_JTAG_ACTIONPOINTS_H
#define ARC_JTAG_ACTIONPOINTS_H

/* gdb header files */
#include "defs.h"
#include "target.h"

/* ARC header files */
#include "arc-support.h"


Boolean arc_initialize_actionpoint_ops (struct target_ops *debug_ops);

Boolean arc_restore_actionpoints_after_reset (void);

void arc_target_halted (void);

void arc_display_actionpoints (void);


/* For debugging.  */
void arc_dump_actionpoints (const char *message);

#endif /* ARC_JTAG_ACTIONPOINTS_H */
/******************************************************************************/
