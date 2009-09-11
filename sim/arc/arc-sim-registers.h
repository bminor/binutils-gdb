/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This header file defines an interface which the simulator can use to   */
/*     map register numbers as known to gdb to hardware register numbers.     */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_SIM_REGISTERS_H
#define ARC_SIM_REGISTERS_H


typedef enum
{
    ARC_UNKNOWN_REGISTER,
    ARC_CORE_REGISTER,
    ARC_AUX_REGISTER,
    ARC_PROGRAM_COUNTER
} ARC_RegisterClass;


typedef void (*ARC_RegisterMappingFunction)(int gdb_regno, int *hw_regno, ARC_RegisterClass *regclass);

typedef void (*ARC_AuxRegisterConversionFunction)(int gdb_regno, void *buffer);


void arc_hw_register_mapping(int                gdb_regno,
                             int               *hw_regno,
                             ARC_RegisterClass *reg_class);

void arc_aux_register_conversion(int gdb_regno, void *buffer);

void arc_set_register_mapping(ARC_RegisterMappingFunction function);

void arc_set_aux_register_conversion(ARC_AuxRegisterConversionFunction function);


#endif /* ARC_SIM_REGISTERS_H */
/******************************************************************************/
