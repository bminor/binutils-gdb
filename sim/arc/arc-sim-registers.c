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
/*     This module implements an interface which the simulator can use to map */
/*     register numbers as known to gdb to hardware register numbers.         */
/*                                                                            */
/******************************************************************************/

#include "arc-sim-registers.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

/* GDB register numbers */

/* N.B. these numbers seem very strange!
 *
 *      If the gdb numbers for the core registers are 0 (R0) .. 63 (PCL), and
 *      the numbers for the aux registers follow on from that range, then surely
 *      we should have PC_REGNUM = 70 and STATUS32_REGNUM = 71?
 *
 *      And where does 2111 for the last auxiliary register come from???!!!
 *
 *      In the code below, PC is correctly recognized as AUX_FIRST_REGNUM + 6
 *      (i.e. 70), and STATUS32 will be handled by the AUX_FIRST_REGNUM ..
 *      AUX_LAST_REGNUM range - so why are there special cases to map R61
 *      (reserved) and R63 (PCL) onto STATUS32 and PC, respectively?
 *
 *      It might be the case that PCL is being treated the same as PC (although
 *      the ARC documentation says that for the ARCcompact "the PCL register
 *      returns the current instruction address, whereas the PC register returns
 *      the next instruction in sequence") - but then why does the calling code
 *      not just use the *_h_pcl_get / *_h_pcl_set functions?
 *
 *      In any case, PCL is read-only, so mapping it onto the PC would allow
 *      the debugger user to change the value of PCL, which is surely not what
 *      is wanted!
 */
#define STATUS32_REGNUM         61
#define PC_REGNUM               63    /* should this be PCL_REGNUM? */
#define AUX_FIRST_REGNUM        64
#define AUX_LAST_REGNUM       2111


static ARC_RegisterMappingFunction       map     = 0;
static ARC_AuxRegisterConversionFunction convert = 0;


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

void arc_hw_register_mapping(int                gdb_regno,
                             int*               hw_regno,
                             ARC_RegisterClass* reg_class)
{
    /* do we have a mapping? */
    if (map != 0)
        (*map)(gdb_regno, hw_regno, reg_class);
    else
    {
        /* OK, do it ourselves ... */

        *hw_regno  = -1;
        *reg_class = ARC_UNKNOWN_REGISTER;

        if (gdb_regno < 61)
        {
            *hw_regno  = gdb_regno;
            *reg_class = ARC_CORE_REGISTER;
        }
        else if (gdb_regno == AUX_FIRST_REGNUM + 6)
        {
            *reg_class = ARC_PROGRAM_COUNTER;
        }
        else if (AUX_FIRST_REGNUM <= gdb_regno && gdb_regno <= AUX_LAST_REGNUM)
        {
            /* N.B. this assumes that the aux registers are contiguous in the
             *      auxiliary register space, i.e. that there are no gaps, but
             *      that is not the case!
             */
            *hw_regno  = gdb_regno - AUX_FIRST_REGNUM;
            *reg_class = ARC_AUX_REGISTER;
        }
        else /* see comment above */
        {
            switch (gdb_regno)
            {
                case PC_REGNUM:
                    *reg_class = ARC_PROGRAM_COUNTER;
                    break;
                case STATUS32_REGNUM:
                    *hw_regno  = 0xA;
                    *reg_class = ARC_AUX_REGISTER;
                    break;
                default :
                    break;
            }
        }
    }
}


void arc_aux_register_conversion(int gdb_regno, void *buffer)
{
    if (convert != 0)
        convert(gdb_regno, buffer);
}


/* this function allows the debugger to provide a mapping */
void arc_set_register_mapping(ARC_RegisterMappingFunction function)
{
    map = function;
}


void arc_set_aux_register_conversion(ARC_AuxRegisterConversionFunction function)
{
    convert = function;
}

/******************************************************************************/
