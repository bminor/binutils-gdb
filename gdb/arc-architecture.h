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
/*     This header file defines an enumeration type for representing the      */
/*     architectural version of an ARC processor, and associated functions.   */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_ARCHITECTURE_H
#define ARC_ARCHITECTURE_H

/* gdb header files */
#include "defs.h"
#include "gdbarch.h"
#include "bfd/bfd-in2.h"

/* ARC header files */
#include "arc-support.h"


typedef enum arc_processor_version
{
    NO_ARCHITECTURE,
    ARC700,
    ARC600,
    A5,
    A4,
    UNSUPPORTED_ARCHITECTURE
} ARC_ProcessorVersion;


#define ARCHITECTURE_CHECK(arch, bfd)      arc_check_architecture (arch, bfd, __FILE__, __FUNCTION__)


void arc_architecture_is_unknown (void);

ARC_ProcessorVersion arc_get_architecture (ReadRegisterFunction read_aux_register);

void arc_update_architecture (ReadRegisterFunction read_register);

void arc_check_architecture (struct gdbarch *gdbarch, bfd *objfile_bfd,
                             const char* file, const char* function);

ARC_ProcessorVersion arc_version (const char *arch);

const char* arc_version_image (ARC_ProcessorVersion version);


#endif /* ARC_ARCHITECTURE_H */
/******************************************************************************/
