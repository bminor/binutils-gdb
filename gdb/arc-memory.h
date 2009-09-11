/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2009 Free Software Foundation, Inc.

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
/*     This header file defines operations for reading data from, and writing */
/*     data to, target memory.                                                */
/*                                                                            */
/*     The data transferred may be of any length, and its start address may   */
/*     have any alignment.                                                    */
/*                                                                            */
/*     These operations must be supplied with more basic operations which can */
/*     read or write an arbitrary number of complete words of data to or from */
/*     word-aligned addresses.                                                */
/*                                                                            */
/*     Optionally, an operation which can write an arbitrary number of copies */
/*     of a word-sized pattern to memory, starting at a word-aligned address, */
/*     may be supplied for use by the arc_write_pattern operation; this is an */
/*     optimisation for improving performance: if it is not supplied, the     */
/*     arc_write_pattern operation will use repeated calls of the supplied    */
/*     'write_words' operation.                                               */
/*                                                                            */
/*     A pointer to user-defined data may be passed to these operations: it   */
/*     will be passed to the basic operations when they are invoked (this     */
/*     allows any required context data to be passed to those operations).    */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_MEMORY
#define ARC_MEMORY

/* ARC header files */
#include "arc-support.h"


unsigned int arc_read_memory   (TargetOperations *ops, ARC_Address address, ARC_Byte *data,    unsigned int bytes);
unsigned int arc_write_memory  (TargetOperations *ops, ARC_Address address, ARC_Byte *data,    unsigned int bytes);
unsigned int arc_write_pattern (TargetOperations *ops, ARC_Address address, ARC_Word  pattern, unsigned int bytes);


#endif /* ARC_MEMORY */
/******************************************************************************/
