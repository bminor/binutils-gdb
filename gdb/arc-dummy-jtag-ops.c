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
/*     This module implements a dummy set of operations for debug access to   */
/*     an ARC processor via its JTAG interface.                               */
/*                                                                            */
/*     It is useful for testing when no hardware target is available.         */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <stdio.h>

/* ARC header files */
#include "arc-jtag-ops.h"
#include "arc-registers.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define AMV1_REG        (ARC_HW_AMV0_REGNUM + 3)


/* The number of actionpoints supported by the target. Set this to different
   values (2, 4 or 8) when testing.  */
static unsigned int num_actionpoints = 8;


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

JTAG_Operations arc_jtag_ops;


/* -------------------------------------------------------------------------- */
/*                               main operations                              */
/* -------------------------------------------------------------------------- */

/* These are the functions that are called from outside this module via the
   pointers in the arc_jtag_ops global object.

   N.B. none of these functions are called from within this module.  */


/* Read a processor core register.  */

static JTAG_OperationStatus
jtag_read_core_reg (ARC_RegisterNumber regnum, ARC_RegisterContents *contents)
{
    return JTAG_SUCCESS;
}


/* Write a processor core register.  */

static JTAG_OperationStatus
jtag_write_core_reg (ARC_RegisterNumber regnum, ARC_RegisterContents contents)
{
    return JTAG_SUCCESS;
}


/* Read a processor auxiliary register.  */

static JTAG_OperationStatus
jtag_read_aux_reg (ARC_RegisterNumber regnum, ARC_RegisterContents *contents)
{
    /* Return a "useful" value for some specific registers.  */

    if (regnum == ARC_HW_PC_REGNUM)
        *contents = 0x00001008;
    else if (regnum == ARC_HW_AP_BUILD_REGNUM)
    {
        if (num_actionpoints == 2)
            *contents = 0x00000004;
        else if (num_actionpoints == 4)
            *contents = 0x00000104;
        else
            *contents = 0x00000204;
    }
    else if (regnum == ARC_HW_DEBUG_REGNUM)
    {
        /* Fake trigger of AP 1.  */
        *contents = DEBUG_ACTIONPOINT_HALT |
                    (1 << (DEBUG_ACTIONPOINT_STATUS_SHIFT + 1));
    }
    else if (regnum == AMV1_REG)
    {
        *contents = 0x4008;
    }
    else if (regnum == ARC_HW_IDENTITY_REGNUM)
    {
        *contents = 0x31;
    }
    else if (regnum == ARC_HW_STATUS32_REGNUM)
    {
        *contents = STATUS32_HALT;
    }
    else
        *contents = 0;

//  DEBUG("regnum = %x, contents = 0x%08X", regnum, *contents);

    return JTAG_SUCCESS;
}


/* Write a processor auxiliary register.  */

static JTAG_OperationStatus
jtag_write_aux_reg (ARC_RegisterNumber regnum, ARC_RegisterContents contents)
{
//  printf(_("AUX: regnum = %d, contents = 0x%08X\n"), regnum, contents);
    return JTAG_SUCCESS;
}


/* Read a word of data from memory; the given address must be word-aligned.
   Returns number of bytes read.  */

static unsigned int
jtag_read_word (ARC_Address addr, ARC_Word *data)
{
    *data = 0;
    return BYTES_IN_WORD;
}


/* Write a word of data to memory; the given address must be word-aligned.
   Returns number of bytes written.  */

static unsigned int
jtag_write_word (ARC_Address addr, ARC_Word data)
{
    return BYTES_IN_WORD;
}


/* Read a number of words of data from target memory starting at the given address.
   Returns number of bytes read.  */

static unsigned int
jtag_read_chunk (ARC_Address addr, ARC_Byte *data, unsigned int words)
{
    return words * BYTES_IN_WORD;
}


/* Write a number of words of data to target memory starting at the given address.
   Returns number of bytes written.  */

static unsigned int
jtag_write_chunk (ARC_Address addr, ARC_Byte *data, unsigned int words)
{
    return words * BYTES_IN_WORD;
}


/* Write a number of copies of a word-sized pattern of data to memory starting
   at the given address.
   Returns number of bytes written.  */

static unsigned int
jtag_write_pattern (ARC_Address addr, ARC_Word pattern, unsigned int words)
{
    return words * BYTES_IN_WORD;
}


/* Open the JTAG interface.
   Returns TRUE for success.  */

static Boolean
jtag_open (void)
{
    arc_jtag_ops.status = JTAG_OPENED;
    return TRUE;
}


/* Close the JTAG interface.  */

static void
jtag_close (void)
{
    arc_jtag_ops.status = JTAG_CLOSED;
}


/* Reset the target JTAG controller.  */

static void
jtag_reset (void)
{
}


/* Reset the target board.  */

static void
jtag_reset_board (void)
{
}


/* Check that the JTAG interface is open.
   If it is closed, 'error' is called.  */

static void
jtag_check_open (void)
{
    if (arc_jtag_ops.status == JTAG_CLOSED)
        error(_("JTAG connection is closed. "
                "Use command 'target " ARC_TARGET_NAME "' first."));
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialize the module. This function is called from the gdb core on start-up.  */

void
_initialize_arc_jtag_ops (void)
{
    arc_jtag_ops.status              = JTAG_CLOSED;
    arc_jtag_ops.state_machine_debug = FALSE;
    arc_jtag_ops.retry_count         = 50;

    arc_jtag_ops.open                 = jtag_open;
    arc_jtag_ops.close                = jtag_close;
    arc_jtag_ops.check_open           = jtag_check_open;
    arc_jtag_ops.reset_board          = jtag_reset_board;
    arc_jtag_ops.reset                = jtag_reset;
    arc_jtag_ops.memory_read_word     = jtag_read_word;
    arc_jtag_ops.memory_write_word    = jtag_write_word;
    arc_jtag_ops.memory_read_chunk    = jtag_read_chunk;
    arc_jtag_ops.memory_write_chunk   = jtag_write_chunk;
    arc_jtag_ops.memory_write_pattern = jtag_write_pattern;
    arc_jtag_ops.read_aux_reg         = jtag_read_aux_reg;
    arc_jtag_ops.write_aux_reg        = jtag_write_aux_reg;
    arc_jtag_ops.read_core_reg        = jtag_read_core_reg;
    arc_jtag_ops.write_core_reg       = jtag_write_core_reg;
}

/******************************************************************************/
