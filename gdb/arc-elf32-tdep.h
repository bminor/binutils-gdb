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
/*     This header file defines an initialization function for the arc_elf32  */
/*     architecture, and various operations which are useful for the various  */
/*     arc-elf32 targets supported.                                           */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_ELF32_TDEP_H
#define ARC_ELF32_TDEP_H

/* gdb header files */
#include "defs.h"
#include "gdbarch.h"
#include "target.h"
#include "breakpoint.h"

/* ARC header files */
#include "arc-registers.h"
#include "arc-architecture.h"


/* Complete the structure definition here.  */
struct arc_variant_info
{
    ARC_ProcessorVersion processor_version;
    ARC_RegisterInfo     registers;
};


typedef enum
{
    REGISTER_IS_READ_ONLY,
    ERROR_ON_READING_REGISTER,
    ERROR_ON_WRITING_REGISTER
} RegisterError;


typedef void (*ProcessorControlFunction)(void);


struct gdbarch* arc_elf32_initialize (struct gdbarch      *gdbarch,
                                      struct gdbarch_list *arches);

void arc_check_pc_defined (struct gdbarch *gdbarch);

void arc_elf32_find_register_numbers (void);

void arc_elf32_load_program (char *filename, int from_tty);

void arc_elf32_create_inferior (char              *exec_file,
                                char              *args,
                                char             **env,
                                struct target_ops *target_ops);

void arc_elf32_execute (struct target_waitstatus *status,
                        ProcessorControlFunction  run_processor,
                        ProcessorControlFunction  start_processor,
                        ProcessorControlFunction  stop_processor);

void arc_elf32_close (Boolean resume);

void arc_elf32_fetch_registers (struct regcache *regcache, int gdb_regno);
void arc_elf32_store_registers (struct regcache *regcache, int gdb_regno);

int arc_elf32_insert_breakpoint (struct bp_target_info *bpt);
int arc_elf32_remove_breakpoint (struct bp_target_info *bpt);

LONGEST arc_elf32_xfer_partial (struct target_ops *ops,
                                enum target_object object,
                                const char        *annex,
                                gdb_byte          *readbuf,
                                const gdb_byte    *writebuf,
                                ULONGEST           offset,
                                LONGEST            len);

void arc_elf32_core_warning (RegisterError      error,
                             ARC_RegisterNumber hw_regno);

void arc_elf32_aux_warning (RegisterError      error,
                            ARC_RegisterNumber hw_regno);


extern ARC_RegisterNumber arc_debug_regnum;
extern ARC_RegisterNumber arc_pc_regnum;
extern ARC_RegisterNumber arc_status32_regnum;

extern Boolean arc_program_is_loaded;
extern Boolean arc_target_is_connected;


#endif /* ARC_ELF32_TDEP_H */
/******************************************************************************/
