/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Soam Vasani <soam.vasani@codito.com>

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

#define ARC_DEBUG_REG_SH_BIT 	0x40000000

#define RAUX(name, hwregno , desc, gdbregno, version)  ARC_HW_##name##_REGNUM = hwregno , 
#define RBCR(name, hwregno , desc, gdbregno, version)  ARC_HW_##name##_REGNUM = hwregno , 


enum arc_hw_regnums
  {
    #include "arc-regnums-defs.h"
    /* Specific ARCAngel Registers for Caches.  */
    ARC_HW_ICACHE_IVIC = 0x10 , /* Invalidate Cache. */
    ARC_HW_ICACHE_CONTROL = 0x11 , /* Disable ICache. ICache control. */
    ARC_HW_DCACHE_IVIC = 0x47, /* Invalidate Cache. */
    ARC_HW_DCACHE_CONTROL = 0x48 , /* Disable DCache. DCache Control. */
  };

#undef RBCR
#undef RAUX

#define ARC_TARGET_OBJECT_AUXREGS 	-1

#define target_read_aux_reg(readbuf, offset, len)               \
         (current_target.to_xfer_partial(&current_target,        \
                                         ARC_TARGET_OBJECT_AUXREGS, NULL, readbuf, NULL, offset, len))
	  
#define target_write_aux_reg(writebuf, offset, len)             \
         (current_target.to_xfer_partial(&current_target,        \
                                         ARC_TARGET_OBJECT_AUXREGS, NULL, NULL, writebuf, offset, len))


static inline int 
is_arc700 (void)
{
  struct gdbarch_tdep * tdep = gdbarch_tdep (current_gdbarch);
  if(tdep->arc_processor_variant_info->arcprocessorversion == ARC700)
    return 1;
  return 0;
  
}

static inline int 
is_arc600 (void)
{
  struct gdbarch_tdep * tdep = gdbarch_tdep (current_gdbarch);
  if(tdep->arc_processor_variant_info->arcprocessorversion == ARC600)
    return 1;
  return 0;
  
}
 int debug_arc_jtag_target_message;
