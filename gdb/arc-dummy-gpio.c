/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey      <richard.stuckey@arc.com>

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
/*     This module implements a dummy set of operations for reading data from */
/*     / writing data to a parallel port using a GPIO (General Purpose Input/ */
/*     Output) driver.                                                        */
/*                                                                            */
/*     It is useful for testing when no hardware target is available.         */
/*                                                                            */
/******************************************************************************/

#include "arc-gpio.h"


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* This is set to TRUE if an I/O error occurs in accessing the port.
   (This never happens in this dummy module).  */
Boolean gpio_port_error;


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialization of the GPIO interface.  */

Boolean
gpio_open (void)
{
    gpio_port_error = FALSE;
    return TRUE;
}


/* Close the GPIO interface.  */

void
gpio_close (void)
{
}


/* Write a byte of data to the given port.  */

void
gpio_write (ParallelPort port, Byte data)
{
}


/* Read a byte of data from the given port.  */

Byte
gpio_read (ParallelPort port)
{
    return 0;
}


/* Write a series of bytes of data to the ports.  */

void
gpio_write_array (GPIO_Pair array[], unsigned int num_elements)
{
}

/******************************************************************************/
