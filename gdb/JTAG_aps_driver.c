/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

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
/*     This module contains a test driver for the JTAG actionpoints module of */
/*     the ARC port of gdb.                                                   */
/*                                                                            */
/* Usage:                                                                     */
/*         <driver>  [ -c ] [ -d ] [ -s ] [ -r <count> ]  [ -a N ]            */
/*                                                                            */
/*         where -c specifies target connection & disconnection only          */
/*               -d switches on JTAG operation debuggging                     */
/*               -s simulates the JTAG target                                 */
/*               -r specifies the JTAG operation retry count                  */
/*               -a 2 | 4 | 8 specifies the number of simulated actionpoints  */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

/* gdb header files */
#include "defs.h"
#include "breakpoint.h"

/* ARC header files */
#include "arc-jtag.h"
#include "arc-jtag-ops.h"
#include "arc-jtag-actionpoints.h"
#include "arc-registers.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

// complete the type here
struct aux_register_definition
{
    const char*        name;
    ARC_RegisterNumber number;
};


typedef enum
{
    CLEAR_USER_BIT,
    RESTORE_USER_BIT
} Status32Action;


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* global debug flag */
Boolean arc_debug_target;

ARC_RegisterNumber arc_debug_regnum = ARC_HW_DEBUG_REGNUM;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define SUCCESS          0
#define FAILURE        (-1)

// what should this be?
#define DATA_AREA            0x00001000
#define DATA_AREA_LENGTH           1024

#define BP_ADDRESS     (DATA_AREA + 0x0400)

#define MAX_ACTION_POINTS        8

#define WRITE_WATCHPOINT         0
#define READ_WATCHPOINT          1
#define ACCESS_WATCHPOINT        2

#define AP_BUILD              0x76


static Boolean           test     = TRUE;
static Boolean           simulate = FALSE;
static unsigned int      num_actionpoints = 2;
static struct target_ops operations;

static const ARC_AuxRegisterDefinition registers[] =
{
    { "DEBUG",    ARC_HW_DEBUG_REGNUM    },
    { "IDENTITY", ARC_HW_IDENTITY_REGNUM },
    { "PC",       ARC_HW_PC_REGNUM       },
    { "STATUS32", ARC_HW_STATUS32_REGNUM },
    { "AP_BUILD", ARC_HW_AP_BUILD_REGNUM },
    { "AMV0",     ARC_HW_AMV0_REGNUM     },
    { "AMM0",     ARC_HW_AMM0_REGNUM     },
    { "AC0",      ARC_HW_AC0_REGNUM      }
};


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#define CHECK(status, expected) \
{ \
    if ((status) != (expected)) \
    { \
        failed("%d line: status %d != %d", __LINE__, status, expected); \
    } \
}


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

static void failed(const char* fmt, ...)
{
    va_list ap;

    fprintf(stderr, "*** FAILED: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");

//  exit(EXIT_FAILURE);
}


static void run_tests(Boolean before_reset)
{
static struct bp_target_info bpt[MAX_ACTION_POINTS + 1];

    int          status;
    int          result;
    CORE_ADDR    addr;
    unsigned int i;
    unsigned int max_actionpoints;

    if (before_reset)
    {
        memset(&bpt, sizeof(bpt), 0);

        for (i = 0; i <= MAX_ACTION_POINTS; i++)
            bpt[i].placed_address = BP_ADDRESS + 4 * i;
    }
    else
    {
        arc_dump_actionpoints("after reset");

        /* remove the actionpoints that were set before the reset */
        status = operations.to_remove_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
        status = operations.to_remove_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);
    }


    if (operations.to_can_use_hw_breakpoint(bp_hardware_breakpoint, 8, 0) == 1)
        max_actionpoints = 8;
    else if (operations.to_can_use_hw_breakpoint(bp_hardware_breakpoint, 4, 0) == 1)
        max_actionpoints = 4;
    else if (operations.to_can_use_hw_breakpoint(bp_hardware_breakpoint, 2, 0) == 1)
        max_actionpoints = 2;
    else
    {
        printf("can not use h/w breakpoints\n");
        return;
    }

    printf("target supports %d actionpoints\n", max_actionpoints);


    /* N breakpoints that each require 1 actionpoint can be set */
    for (i = 0; i < max_actionpoints + 1; i++)
    {
        status = operations.to_insert_hw_breakpoint(&bpt[i]);
        CHECK(status, (i < max_actionpoints) ? SUCCESS : FAILURE);
    }
    for (i = 0; i < max_actionpoints + 1; i++)
    {
        status = operations.to_remove_hw_breakpoint(&bpt[i]);
        CHECK(status, (i < max_actionpoints) ? SUCCESS : FAILURE);
    }


    if (operations.to_region_ok_for_hw_watchpoint(DATA_AREA, DATA_AREA_LENGTH))
    {
        /* N watchpoints that each require 1 actionpoint can be set */
        for (i = 0; i < max_actionpoints + 1; i++)
        {
            status = operations.to_insert_watchpoint(DATA_AREA, 4, WRITE_WATCHPOINT);
            CHECK(status, (i < max_actionpoints) ? SUCCESS : FAILURE);
        }
        for (i = 0; i < max_actionpoints + 1; i++)
        {
            status = operations.to_remove_watchpoint(DATA_AREA, 4, WRITE_WATCHPOINT);
            CHECK(status, (i < max_actionpoints) ? SUCCESS : FAILURE);
        }

        if (max_actionpoints == 8)
        {
            /* this requires 2 actionpoints (a pair) */
            status = operations.to_insert_watchpoint(DATA_AREA,  6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            arc_display_actionpoints();
            status = operations.to_remove_watchpoint(DATA_AREA,  6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this requires 3 actionpoints (a quad) */
            status = operations.to_insert_watchpoint(DATA_AREA,  5, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            arc_display_actionpoints();
            status = operations.to_remove_watchpoint(DATA_AREA,  5, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this requires 4 actionpoints (a quad) */
            status = operations.to_insert_watchpoint(DATA_AREA,  9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this requires another 4 actionpoints (another quad) */
            status = operations.to_insert_watchpoint(DATA_AREA, 25, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            status = operations.to_remove_watchpoint(DATA_AREA,  9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 25, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this would require 5 actionpoints! */
            status = operations.to_insert_watchpoint(DATA_AREA, 17, ACCESS_WATCHPOINT);    CHECK(status, FAILURE);

            /* this will tie up actionpoints 0, 2 and 6 */
            status = operations.to_insert_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[2]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[3]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[4]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[5]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[6]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[3]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[4]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[5]);    CHECK(status, SUCCESS);

            arc_display_actionpoints();

            arc_dump_actionpoints("before quad shuffle");

            /* this requires 4 actionpoints (a quad) - this should cause the
             * actionpoints to be shuffled so that a quad is available
             */
            status = operations.to_insert_watchpoint(DATA_AREA, 9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            arc_dump_actionpoints("after quad shuffle");

            arc_display_actionpoints();

            /* release the tied-up actionpoints */
            status = operations.to_remove_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[2]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[6]);    CHECK(status, SUCCESS);

            arc_dump_actionpoints("before quad shuffle");

            /* this requires another 4 actionpoints (a quad) - this should
             * cause the actionpoints to be shuffled so that the other quad is
             * available
             */
            status = operations.to_insert_watchpoint(DATA_AREA, 36, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            arc_dump_actionpoints("after quad shuffle");

            /* release the two quads */
            status = operations.to_remove_watchpoint(DATA_AREA,  9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 36, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
        }
        else if (max_actionpoints == 4)
        {
            /* this requires 2 actionpoints (a pair) */
            status = operations.to_insert_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this requires 3 actionpoints (a quad) */
            status = operations.to_insert_watchpoint(DATA_AREA, 5, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 5, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this requires 4 actionpoints (a quad) */
            status = operations.to_insert_watchpoint(DATA_AREA, 9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 9, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this would require 5 actionpoints! */
            status = operations.to_insert_watchpoint(DATA_AREA, 17, ACCESS_WATCHPOINT);    CHECK(status, FAILURE);

            /* this will tie up actionpoints 0 and 2 */
            status = operations.to_insert_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);
            status = operations.to_insert_hw_breakpoint(&bpt[2]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);

            arc_dump_actionpoints("before quad shuffle");

            /* this requires 2 actionpoints (a pair) - this should cause the
             * actionpoints to be shuffled so that a pair is available
             */
            status = operations.to_insert_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            arc_dump_actionpoints("after quad shuffle");

            status = operations.to_remove_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
            status = operations.to_remove_hw_breakpoint(&bpt[2]);    CHECK(status, SUCCESS);
        }
        else if (max_actionpoints == 2)
        {
            /* this requires 2 actionpoints (a pair) */
            status = operations.to_insert_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);
            status = operations.to_remove_watchpoint(DATA_AREA, 6, ACCESS_WATCHPOINT);    CHECK(status, SUCCESS);

            /* this would require 3 actionpoints! */
            status = operations.to_insert_watchpoint(DATA_AREA, 5, ACCESS_WATCHPOINT);    CHECK(status, FAILURE);
        }

        result = operations.to_stopped_by_watchpoint();
        printf("%sstopped by watchpoint\n", (result) ? "" : "not ");

        result = operations.to_stopped_data_address(&operations, &addr);
        if (result)
            printf("stopped by data access at address 0x%08X\n", (unsigned int) addr);
    }
    else
        printf("area %0x08X .. %0x08X does not allow watchpoints\n", DATA_AREA, DATA_AREA + DATA_AREA_LENGTH - 1);

    arc_dump_actionpoints("before reset");

    /* leave some actionpoints set to ensure that there are some to re-establish
     * after target board reset
     */
    if (before_reset)
    {
        status = operations.to_insert_hw_breakpoint(&bpt[0]);    CHECK(status, SUCCESS);
        status = operations.to_insert_hw_breakpoint(&bpt[1]);    CHECK(status, SUCCESS);
    }
}


static void process_options(int argc, char** argv)
{
    int c;

    while ((c = getopt (argc, argv, "sdcr:a:")) != -1)
    {
        switch (c)
        {
            case 'd':
                arc_jtag_ops.state_machine_debug = TRUE;
                break;
            case 'r':
                arc_jtag_ops.retry_count = atoi(optarg);
                break;
            case 'a':
                num_actionpoints = atoi(optarg);
                if (!(num_actionpoints == 2 || num_actionpoints == 4 || num_actionpoints == 8))
                {
                   fprintf(stderr, "Usage: %s [ -d ]\n", argv[0]);
                   exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                test = FALSE;
                break;
            case 's':
                simulate = TRUE;
                break;
            default:
                fprintf(stderr, "Usage: %s [ -dcs ] [ -r <count> ] [ -a <actionpoints> ]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

extern void _initialize_arc_jtag_ops(void);


int main(int argc, char** argv)
{
    Boolean opened;

    printf("Starting test of ARC JTAG actionpoints...\n");

    _initialize_arc_jtag_ops();

    process_options(argc, argv);

    opened = ((simulate) ? TRUE : arc_jtag_ops.open());

    if (opened)
    {
        printf("ARC processor is connected\n");

        /* this can be done only after connection */
        if (arc_initialize_actionpoint_ops(&operations))
        {
            if (test)
            {
                run_tests(TRUE);
                printf("resetting board...\n");
                if (!simulate)
                    arc_jtag_ops.reset_board();
                printf("board reset\n");
                if (!arc_restore_actionpoints_after_reset())
                    failed("could not restore actionpoints after reset");
                run_tests(FALSE);
            }
        }
        else
            printf("processor does not support actionpoints\n");

        if (!simulate)
            arc_jtag_ops.close();
    }

    printf("Finished test of ARC JTAG actionpoints\n");

    return 0;
}


/* N.B. these functions are found in arc-jtag.c and arc-registers.c, but are
 *      included here in order to avoid including that module in the built test
 *      driver, as that would also pull in a lot of the gdb modules!
 *
 *      Also, this allows the AUX register read/write operations to be simulated,
 *      thus making it possible to test much of the logic of the arc-jtag-actionpoints
 *      module without a real JTAG target.
 */

void arc_change_status32(Status32Action action)
{
    static ARC_RegisterContents status32;

    if (action == CLEAR_USER_BIT)
    {
        /* Get processor out of user mode. */

        if (arc_read_jtag_aux_register(ARC_HW_STATUS32_REGNUM, &status32, FALSE))
        {
            /* if the User bit is actually set */
            if (status32 & STATUS32_USER)
                if (!arc_write_jtag_aux_register(ARC_HW_STATUS32_REGNUM,
                                                 status32 & ~STATUS32_USER, FALSE))
                    warning(_("Can not clear User bit in STATUS32 register"));
        }
        else
            warning(_("Can not read STATUS32 register"));
    }
    else
    {
        /* if the User bit was actually cleared */
        if (status32 & STATUS32_USER)
            if (!arc_write_jtag_aux_register(ARC_HW_STATUS32_REGNUM, status32, FALSE))
                warning(_("Can not restore User bit in STATUS32 register"));
    }
}


Boolean arc_read_jtag_aux_register (ARC_RegisterNumber    hwregno,
                                    ARC_RegisterContents* contents,
                                    Boolean               warn_on_failure)
{
    if (simulate)
    {
        if (hwregno == AP_BUILD)
        {
            if (num_actionpoints == 2)
                *contents = 0x00000004;
            else if (num_actionpoints == 4)
                *contents = 0x00000104;
            else
                *contents = 0x00000204;
        }
        else
            *contents = 0;
        return TRUE;
    }

    return (arc_jtag_ops.read_aux_reg(hwregno, contents) == JTAG_SUCCESS);
}


Boolean arc_write_jtag_aux_register (ARC_RegisterNumber   hwregno,
                                     ARC_RegisterContents contents,
                                     Boolean              warn_on_failure)
{
    if (simulate)
    {
//      printf("AUX: hwregno = %d, contents = 0x%08X\n", hwregno, contents);
        return TRUE;
    }

    return (arc_jtag_ops.write_aux_reg(hwregno, contents) == JTAG_SUCCESS);
}


ARC_AuxRegisterDefinition*
arc_find_aux_register_by_name(const char* name)
{
    unsigned int i;

    for (i = 0; i < ELEMENTS_IN_ARRAY(registers); i++)
    {
        const ARC_AuxRegisterDefinition* def = &registers[i];

        if (strcmp(name, def->name) == 0)
            return (ARC_AuxRegisterDefinition*) def;
    }

    return NULL;
}


ARC_RegisterNumber arc_aux_hw_register_number(ARC_AuxRegisterDefinition* def)
{
    return def->number;
}


ARC_RegisterNumber arc_aux_find_register_number(const char*        name,
                                                ARC_RegisterNumber defaultNumber)
{
    ARC_AuxRegisterDefinition* def = arc_find_aux_register_by_name(name);

    return (def) ? def->number : defaultNumber;
}

/******************************************************************************/
