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
/*     This header file defines operations for reading data from / writing    */
/*     data to a parallel port using a GPIO (General Purpose Input/Output)    */
/*     driver.                                                                */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_GPIO_H
#define ARC_GPIO_H

/* ARC header files */
#include "arc-support.h"


#define GPIO_DEVICE         "/dev/gpio"


typedef enum
{
    DATA_PORT    = 0,
    STATUS_PORT  = 1,
    CONTROL_PORT = 2
} ParallelPort;

typedef unsigned char Byte;

typedef struct
{
    ParallelPort port;
    Byte         data;
} GPIO_Pair;


Boolean gpio_open  (void);
void    gpio_close (void);

void gpio_write (ParallelPort port, Byte data);
Byte gpio_read  (ParallelPort port);

void gpio_write_array (GPIO_Pair array[], unsigned int num_elements);


extern Boolean gpio_port_error;

#endif /* ARC_GPIO_H */
/******************************************************************************/
