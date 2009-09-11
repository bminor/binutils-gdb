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
/*     This header file defines operations for manipulating the ARC processor */
/*     core registers and auxiliary registers.                                */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_REGISTERS_H
#define ARC_REGISTERS_H

/* gdb header files */
#include "defs.h"
#include "gdbarch.h"

/* ARC header files */
#include "arc-support.h"
#include "arc-tdep.h"
#include "arc-architecture.h"


#define REGISTER_DEFINITION_FILE      "arc-registers.xml"


/* ARC aux registers with known (well-defined) numbers.  */
#define ARC_HW_LP_START_REGNUM        (ARC_RegisterNumber) 0x2
#define ARC_HW_LP_END_REGNUM          (ARC_RegisterNumber) 0x3
#define ARC_HW_IDENTITY_REGNUM        (ARC_RegisterNumber) 0x4
#define ARC_HW_DEBUG_REGNUM           (ARC_RegisterNumber) 0x5
#define ARC_HW_PC_REGNUM              (ARC_RegisterNumber) 0x6
#define ARC_HW_STATUS32_REGNUM        (ARC_RegisterNumber) 0xA

/* Specific ARCangel aux registers for caches.  */
#define ARC_HW_IC_IVIC_REGNUM         (ARC_RegisterNumber) 0x10     /* Invalidate ICache.  */
#define ARC_HW_IC_CTRL_REGNUM         (ARC_RegisterNumber) 0x11     /* Disable    ICache.  */
#define ARC_HW_DC_IVDC_REGNUM         (ARC_RegisterNumber) 0x47     /* Invalidate DCache.  */
#define ARC_HW_DC_CTRL_REGNUM         (ARC_RegisterNumber) 0x48     /* Disable    DCache.  */

/* BCRs.  */
#define ARC_HW_MEMSUBSYS_REGNUM       (ARC_RegisterNumber) 0x67
#define ARC_HW_AP_BUILD_REGNUM        (ARC_RegisterNumber) 0x76

/* Auxiliary registers for actionpoint 0 (there are up to 8 sets of these).  */
#define ARC_HW_AMV0_REGNUM            (ARC_RegisterNumber) 0x220
#define ARC_HW_AMM0_REGNUM            (ARC_RegisterNumber) 0x221
#define ARC_HW_AC0_REGNUM             (ARC_RegisterNumber) 0x222


/* Bit masks for use with the cache-related auxiliary registers.  */
#define IC_IVIC_IV                     0x00000001
#define DC_IVDC_IV                     0x00000001
#define IC_CTRL_DC                     0x00000001
#define DC_CTRL_DC                     0x00000001
#define DC_CTRL_IM                     0x00000040


/* Bit masks for use with the auxiliary DEBUG, IDENTITY, STATUS32 and AP_BUILD registers.  */
#define DEBUG_SH                         0x40000000
#define DEBUG_BH                         0x20000000
#define DEBUG_USER                       0x10000000
#define DEBUG_ACTIONPOINT_HALT           0x00000004
#define DEBUG_ACTIONPOINT_STATUS         0x000007F8
#define DEBUG_ACTIONPOINT_STATUS_SHIFT   3
#define DEBUG_FORCE_HALT                 0x00000002
#define DEBUG_INSTRUCTION_STEP           0x00000800
#define DEBUG_SINGLE_STEP                0x00000001
#define DEBUG_LOAD_PENDING               0x80000000
#define IDENTITY_ARCVER                  0x000000FF
#define STATUS32_USER                    0x00000080
#define STATUS32_L                       0x00000100
#define STATUS32_HALT                    0x00000001
#define AP_BUILD_VERSION                 0x000000FF
#define AP_BUILD_TYPE                    0x00000F00
#define AP_BUILD_TYPE_SHIFT              8


typedef enum register_access
{
    READ_ONLY,
    READ_WRITE,
    WRITE_ONLY
} RegisterAccess;


struct core_register_definition
{
    int            gdb_regno;
    ARC_Word       mask;
    RegisterAccess access;
    Boolean        exists;
};


/* The type 'struct aux_register_definition' is incomplete: it is private to this
   module.  */
typedef struct aux_register_definition  ARC_AuxRegisterDefinition;
typedef struct core_register_definition ARC_CoreRegisterDefinition;


/* This type is essentially private: no access to any of its fields should
   be performed outside of this module.  */
typedef struct _register_info
{
    ARC_ProcessorVersion       processor;
    ARC_AuxRegisterDefinition *aux_registers;
    unsigned int               aux_register_count;
    int                        first_aux_gdb_regno;
    unsigned int               max_name_length;
    int                        PC_number;
    ARC_CoreRegisterDefinition core_registers[ARC_MAX_CORE_REGS];
    unsigned int               core_register_count;
} ARC_RegisterInfo;


typedef void (*ARC_AuxRegisterFunction)(ARC_AuxRegisterDefinition *def, void *data);


extern Boolean arc_pending_register_architecture_change_event;


/* Initialization functions.  */

void arc_initialize_aux_reg_info (ARC_RegisterInfo *info);

void arc_read_default_aux_registers (struct gdbarch *gdbarch);


/* PC-related functions.  */

void arc_aux_pc_guard         (struct gdbarch *gdbarch, Boolean on);

void arc_aux_check_pc_defined (struct gdbarch *gdbarch);

int  arc_aux_pc_number        (struct gdbarch *gdbarch);


/* Output functions.  */

void arc_print_aux_register (ARC_AuxRegisterDefinition *def,
                             ARC_RegisterContents       contents);


/* Search functions.  */

ARC_AuxRegisterDefinition* arc_find_aux_register_by_name       (const char *name);

ARC_AuxRegisterDefinition* arc_find_aux_register_by_gdb_number (int gdb_regno);

ARC_AuxRegisterDefinition* arc_find_aux_register_by_hw_number  (ARC_RegisterNumber hw_regno);

ARC_RegisterNumber      arc_aux_find_register_number (const char        *name,
                                                      ARC_RegisterNumber defaultNumber);

ARC_RegisterNumber      arc_core_register_number (int gdb_regno);

int                     arc_core_register_gdb_number (ARC_RegisterNumber hw_regno);

const char*             arc_aux_register_name_of (ARC_RegisterNumber hw_regno);

Boolean                 arc_is_core_register (int gdb_regno);


/* Accessor functions.  */

int                arc_aux_gdb_register_number (ARC_AuxRegisterDefinition *def);

ARC_RegisterNumber arc_aux_hw_register_number  (ARC_AuxRegisterDefinition *def);

RegisterAccess     arc_aux_register_access     (ARC_AuxRegisterDefinition *def);

Boolean            arc_aux_is_unused           (ARC_AuxRegisterDefinition *def);

Boolean            arc_aux_is_BCR              (ARC_AuxRegisterDefinition *def);

const char*        arc_aux_register_name       (ARC_AuxRegisterDefinition *def);

RegisterAccess     arc_core_register_access    (ARC_RegisterNumber regno);


/* Iterator/summary functions.  */

void arc_all_aux_registers (ARC_AuxRegisterFunction function, void *data);

unsigned int         arc_aux_register_max_name_length  (void);

unsigned int         arc_aux_register_count            (struct gdbarch *gdbarch);

unsigned int         arc_core_register_count           (struct gdbarch *gdbarch);

Boolean              arc_aux_regs_defined              (struct gdbarch *gdbarch);

ARC_ProcessorVersion arc_aux_architecture              (struct gdbarch *gdbarch);

/* Register contents conversion functions.  */

ARC_RegisterContents arc_write_value (ARC_AuxRegisterDefinition *def, ARC_RegisterContents value);

void arc_convert_aux_contents_for_write (int gdb_regno, void *buffer);


#endif /* ARC_REGISTERS_H */
/******************************************************************************/
