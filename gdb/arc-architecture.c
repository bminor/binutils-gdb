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
/*     This module provides support for handling the architectural version of */
/*     of the ARC target processor.                                           */
/*                                                                            */
/* Architectural Checks:                                                      */
/*     Checks upon architectural consistency are (currently) performed at     */
/*     these points:                                                          */
/*                                                                            */
/*        1) after connection to target                                       */
/*        2) after reading (non-default) XML file                             */
/*        3) after blasting JTAG target FPGA                                  */
/*        4) after attaching to JTAG target                                   */
/*        5) after selecting executable file                                  */
/*        6) before downloading program to target                             */
/*                                                                            */
/******************************************************************************/

/* gdb header files.  */
#include "defs.h"
#include "objfiles.h"
#include "arch-utils.h"
#include "gdb-events.h"
#include "gdb_assert.h"

/* ARC header files.  */
#include "arc-architecture.h"
#include "arc-elf32-tdep.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef enum
{
    DEFAULT_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS,
    DEFAULT_SIMULATOR_WITH_AUXILIARY_REGISTERS,
    SELECTED_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS,
    SELECTED_SIMULATOR_WITH_AUXILIARY_REGISTERS,
    TARGET_WITH_NO_AUXILIARY_REGISTERS,
    TARGET_WITH_AUXILIARY_REGISTERS,
    EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS,
    EXECUTABLE_WITH_AUXILIARY_REGISTERS,
    DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS,
    DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS,
    SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS,
    SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS,
    TARGET_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS,
    TARGET_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS,
    NO_ERROR
} Diagnostic;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

/* N.B. the messages MUST correspond to the values in the Diagnostic enumeration
        type above, as a value of that type is used to index the array.  */

static const char *messages[] =
{
    "default simulator architecture is %s but auxiliary registers are not defined",
    "default simulator architecture is %s but auxiliary registers are defined for %s",
    "selected simulator architecture is %s but auxiliary registers are not defined",
    "selected simulator architecture is %s but auxiliary registers are defined for %s",
    "target architecture is %s but auxiliary registers are not defined",
    "target architecture is %s but auxiliary registers are defined for %s",
    "executable file %s is built for %s but auxiliary registers are not defined",
    "executable file %s is built for %s but auxiliary registers are defined for %s",
    "default simulator architecture is %s, executable file %s is built for %s but auxiliary registers are not defined",
    "default simulator architecture is %s, executable file %s is built for %s and auxiliary registers are defined for %s",
    "selected simulator is %s, executable file %s is built for %s but auxiliary registers are not defined",
    "selected simulator is %s, executable file %s is built for %s and auxiliary registers are defined for %s",
    "target architecture is %s, executable file %s is built for %s but auxiliary registers are not defined",
    "target architecture is %s, executable file %s is built for %s and auxiliary registers are defined for %s"
};


/* This is the architecture of the current target.  */
static ARC_ProcessorVersion ARC_processor = NO_ARCHITECTURE;


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Map the BFD architerctural type onto the ARC processor type.  */

static ARC_ProcessorVersion
architecture (const bfd_arch_info_type *arch)
{
    switch (arch->mach)
    {
        case bfd_mach_arc_a5    : return A5;
        case bfd_mach_arc_arc600: return ARC600;
        case bfd_mach_arc_arc700: return ARC700;
        default                 : return UNSUPPORTED_ARCHITECTURE;
    }
}


/* This function performs the architectural consistency check.

   Parameters:
      target              : the architectural version of the target
      auxregs             : the architectural version of the auxiliary registers
      program             : the architectural version of the executable program
      is_builtin_simulator: TRUE if the target is the built-in simulator
      is_default          : TRUE if built-in simulator's version is the default
      objfile_bfd         : a pointer to the BFD for the object file

  If the check fails, an appropriate warning message is output.  */

static void
perform_check (ARC_ProcessorVersion target,
               ARC_ProcessorVersion auxregs,
               ARC_ProcessorVersion program,
               Boolean              is_builtin_simulator,
               bfd_boolean          is_default,
               bfd                 *objfile_bfd)
{
    Diagnostic diagnostic = NO_ERROR;

    /* If we do not yet have an object file (and hence we do not know the
       program's architecture).  */
    if (program == NO_ARCHITECTURE)
    {
        if (target != auxregs)
        {
            if (is_builtin_simulator)
            {
                if (is_default)
                {
                    if (auxregs == NO_ARCHITECTURE)
                        diagnostic = DEFAULT_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS;
                    else
                        diagnostic = DEFAULT_SIMULATOR_WITH_AUXILIARY_REGISTERS;
                }
                else
                {
                    if (auxregs == NO_ARCHITECTURE)
                        diagnostic = SELECTED_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS;
                    else
                        diagnostic = SELECTED_SIMULATOR_WITH_AUXILIARY_REGISTERS;
                }
            }
            else
                if (auxregs == NO_ARCHITECTURE)
                    diagnostic = TARGET_WITH_NO_AUXILIARY_REGISTERS;
                else
                    diagnostic = TARGET_WITH_AUXILIARY_REGISTERS;
        }
    }
    else
    {
        if (program != target || program != auxregs)
        {
            if (is_builtin_simulator)
            {
                if (target == program)
                {
                    if (auxregs == NO_ARCHITECTURE)
                        diagnostic = EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS;
                    else
                        diagnostic = EXECUTABLE_WITH_AUXILIARY_REGISTERS;
                }
                else
                {
                    if (is_default)
                    {
                        if (auxregs == NO_ARCHITECTURE)
                            diagnostic = DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS;
                        else
                            diagnostic = DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS;
                    }
                    else
                    {
                        if (auxregs == NO_ARCHITECTURE)
                            diagnostic = SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS;
                        else
                            diagnostic = SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS;
                    }
                }
            }
            else
                if (auxregs == NO_ARCHITECTURE)
                    diagnostic = TARGET_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS;
                else
                    diagnostic = TARGET_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS;
        }
    }

    if (diagnostic != NO_ERROR)
    {
        const char *message = messages[diagnostic];

        switch (diagnostic)
        {
            case DEFAULT_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS:
            case SELECTED_SIMULATOR_WITH_NO_AUXILIARY_REGISTERS:
            case TARGET_WITH_NO_AUXILIARY_REGISTERS:
                warning(message, arc_version_image(target));
                break;

            case DEFAULT_SIMULATOR_WITH_AUXILIARY_REGISTERS:
            case SELECTED_SIMULATOR_WITH_AUXILIARY_REGISTERS:
            case TARGET_WITH_AUXILIARY_REGISTERS:
                warning(message, arc_version_image(target), arc_version_image(auxregs));
                break;

            case EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS:
                warning(message, objfile_bfd->filename, arc_version_image(program));
                break;

            case EXECUTABLE_WITH_AUXILIARY_REGISTERS:
                warning(message, objfile_bfd->filename, arc_version_image(program), arc_version_image(auxregs));
                break;

            case DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS:
            case SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS:
            case TARGET_AND_EXECUTABLE_WITH_NO_AUXILIARY_REGISTERS:
                warning(message, arc_version_image(target), objfile_bfd->filename, arc_version_image(program));
                break;

            case DEFAULT_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS:
            case SELECTED_SIMULATOR_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS:
            case TARGET_AND_EXECUTABLE_WITH_AUXILIARY_REGISTERS:
                warning(message, arc_version_image(target), objfile_bfd->filename, arc_version_image(program), arc_version_image(auxregs));
                break;

            default:
                break;
        }
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Set the architectural version of the target processor as being unknown.  */

void
arc_architecture_is_unknown (void)
{
    ARC_processor = NO_ARCHITECTURE;
}


/* Return the processor variant that is connected.  */

ARC_ProcessorVersion
arc_get_architecture (ReadRegisterFunction read_aux_register)
{
    ENTERMSG;

    if (ARC_processor == NO_ARCHITECTURE)
    {
        ARC_RegisterNumber   identity_regnum = arc_aux_find_register_number("IDENTITY", ARC_HW_IDENTITY_REGNUM);
        ARC_RegisterContents value;

        if (read_aux_register(identity_regnum, &value, TRUE))
        {
            DEBUG("IDENTITY = 0x%X\n", value);

            /* Get the processor version number.  */
            value &= IDENTITY_ARCVER;

            if ((value >= 0x30) && (value <= 0x3f))
                ARC_processor = ARC700;
            else if ((value >= 0x20) && (value <= 0x2f))
                ARC_processor = ARC600;
            else if ((value >= 0x10) && (value <= 0x1f))
                ARC_processor = A5;
            else if (value <= 0x0f)
                ARC_processor = A4;
            else
                warning(_("unsupported processor version 0x%x"), value);

            DEBUG("target (from IDENTITY) is %s\n", arc_version_image(ARC_processor));
        }
    }

    return ARC_processor;
}


/* Get the target processor architecture, check that is it supported, and
 * update any architecture-related information that gdb requires.  */

void
arc_update_architecture (ReadRegisterFunction read_register)
{
    struct gdbarch_tdep *tdep    = gdbarch_tdep (current_gdbarch);
    ARC_ProcessorVersion version = arc_get_architecture(read_register);

    /* Record the target processor architecture in the targt-dependent
     * variant information.  */
    tdep->processor_variant_info->processor_version = version;

    switch (version)
    {
        case NO_ARCHITECTURE:
            break;
        case ARC700:
            set_gdbarch_decr_pc_after_break (current_gdbarch, 0);
            break;
        case ARC600:
            set_gdbarch_decr_pc_after_break (current_gdbarch, 2);
            break;
        case A5:
            warning(_("A5 debugging is unsupported and may be unreliable."));
            break;
        case A4:
            /* N.B. this will not return.  */
            error(_("A4 debugging is unsupported."));
            break;
        case UNSUPPORTED_ARCHITECTURE:
            break;
    }
}


/* This function checks for architectural consistency; there are three possible
   architectures to be considered:

    1) the architecture for which the program to be debugged has been built
    2) the architecture of the target upon which the program is to be debugged
    3) the architecture for which we have a description of the auxiliary registers

   The 'gdbarch' parameter to this function corresponds to 2).

   A hardware target (such as an ARCangel) has a fixed architecture (e.g. that
   defined by the XBF file used to configure it; and the xISS simulator's
   architecture is defined by the simulator definition (.xis) file; however,
   if the target is the built-in simulator, the architecture depends upon how
   the simulator instance is created: if the instance is created by use of the
   commands

       file <program_file>
       target sim

   then the simulator architecture is taken from the program file; if it is
   created by the commands

       set endian big | little
       set architecture <name>
       target sim

   then the architecture is the named one; if it is created by the commands

       set endian big | little
       target sim

   the architecture is the default ARC architecture as defined in bfd/cpu-arc.c,
   and this is indicated by the 'the_default' flag in the 'bfd_arch_info' struct
   being TRUE.  */

void
arc_check_architecture (struct gdbarch *gdbarch, bfd *objfile_bfd,
                        const char* file, const char* function)
{
    ARC_ProcessorVersion target               = gdbarch_tdep (gdbarch)->processor_variant_info->processor_version;
    ARC_ProcessorVersion auxregs              = arc_aux_architecture(gdbarch);
    ARC_ProcessorVersion program              = NO_ARCHITECTURE;
    bfd_boolean          is_default           = FALSE;
    Boolean              is_builtin_simulator = FALSE;

    ENTERARGS("current target = %s (%sconnected), objfile = %s",
              current_target.to_shortname,
              (arc_target_is_connected) ? "" : "not ",
              (objfile_bfd) ? bfd_get_filename(objfile_bfd) : "<none>");

    DEBUG("architectural check in function '%s' in file '%s'\n", function, file);

    /* If the target is the built-in simulator.  */
    if (strcmp(current_target.to_shortname, "sim") == 0)
    {
        const char               *name = selected_architecture_name();
        const bfd_arch_info_type *arch;

        if (name)
            arch = bfd_scan_arch(name);
        else
            arch = gdbarch_bfd_arch_info(gdbarch);

        is_default           = arch->the_default;
        is_builtin_simulator = TRUE;
    }

    if (objfile_bfd)
        program = architecture(objfile_bfd->arch_info);
    else
        program = NO_ARCHITECTURE;

    DEBUG("target  = %s\n", arc_version_image(target));
    DEBUG("auxregs = %s\n", arc_version_image(auxregs));
    DEBUG("program = %s\n", arc_version_image(program));

    /* If we are connected to a target, we should know its architecture; if we
       are not connected, if we don't know both the program and aux registers
       architectures then there is no point in checking anything (this situation
       could occur if a 'file <program>' or 'arc-reg-read-file <xmlfile>'
       command has been issued before the connection is made).  */
    if (arc_target_is_connected)
        gdb_assert(target != NO_ARCHITECTURE);
    else
        if (program == NO_ARCHITECTURE || auxregs == NO_ARCHITECTURE)
            return;

    /* Check that the architectures are the same.  */
    perform_check(target,
                  auxregs,
                  program,
                  is_builtin_simulator,
                  is_default,
                  objfile_bfd);

    /* Unfortunately, this event can not be sent at the point that it is known
       that the register architecture has changed, as at that point the global
       variable current_gdbarch may have the value NULL, and that could cause
       an error elsewhere where gdbarch_num_regs or gdbarch_num_pseudo_regs is
       used (e.g. in setup_architecture_data in gdbtk/generic/gdbtk-register.c).  */
    if (arc_pending_register_architecture_change_event)
    {
        DEBUG("sending register architecture changed event\n");
        arc_pending_register_architecture_change_event = FALSE;
        reg_architecture_changed_event();
    }
}


/* Get the ARC architectural version from a string.  */

ARC_ProcessorVersion
arc_version (const char *arch)
{
    ARC_ProcessorVersion version;

#define ARCH_IS(ident)    (strcmp(arch, ident) == 0)

    if (ARCH_IS("ARC700"))
        version = ARC700;
    else if (ARCH_IS("ARC600"))
        version = ARC600;
    else if (ARCH_IS("A5"))
        version = A5;
    else if (ARCH_IS("A4"))
        version = A4;
    else
        version = UNSUPPORTED_ARCHITECTURE;

    return version;
}


/* Return a string representation of the ARC architectural version.  */

const char*
arc_version_image (ARC_ProcessorVersion version)
{
    switch (version)
    {
        case NO_ARCHITECTURE         : return _("NONE");
        case ARC700                  : return _("ARC700");
        case ARC600                  : return _("ARC600");
        case A5                      : return _("A5");
        case A4                      : return _("A4");
        case UNSUPPORTED_ARCHITECTURE: return _("UNSUPPORTED");
        default                      : return _("???");
    }
}

/******************************************************************************/
