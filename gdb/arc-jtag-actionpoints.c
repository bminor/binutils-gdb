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
/*     This module implements hardware breakpoints and watchpoints on the ARC */
/*     processor using actionpoints.                                          */
/*                                                                            */
/*     See                                                                    */
/*                    ARC 700 System Components Reference                     */
/*                                 5126-016                                   */
/*                                                                            */
/*     for a full description of the processor actionpoints mechanism.        */
/*                                                                            */
/******************************************************************************/

/* gdb header files */
#include "defs.h"
#include "inferior.h"
#include "gdb_assert.h"

/* ARC header files */
#include "arc-jtag-actionpoints.h"
#include "arc-registers.h"
#include "arc-elf32-tdep.h"
#include "arc-jtag.h"
#include "arc-jtag-ops.h"
#include "arc-support.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef enum
{
   NOT_IN_USE,
   SINGLE,
   PAIR_0,
   PAIR_1,
   QUAD_0,
   QUAD_1,
   QUAD_2,
   QUAD_3
} Actionpoint_Usage;


/* This information describes an individual actionpoint.  */
typedef struct arc_actionpoint
{
    Actionpoint_Usage    usage;
    Boolean              triggered;
    Boolean              is_exclude;
    unsigned int         length;
    ARC_RegisterContents match_value;
    ARC_RegisterContents match_mask;
    ARC_RegisterContents control;
    ARC_RegisterContents point;
} ARC_ActionPoint;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define SUCCESS          0
#define FAILURE        (-1)

#define MAX_ACTION_POINTS              8
#define MAX_ACTION_POINTS_IN_GROUP     4
#define HW_BP_SIZE                     1


/* Bit masks for the Actionpoint Control Registers AP_ACx (10 low-order bits only).  */
#define AP_TARGET_INSTRUCTION_ADDRESS      0x000
#define AP_TARGET_INSTRUCTION_DATA         0x001
#define AP_TARGET_LOAD_STORE_ADDRESS       0x002
#define AP_TARGET_LOAD_STORE_DATA          0x003
#define AP_TARGET_AUX_REGISTER_ADDRESS     0x004
#define AP_TARGET_AUX_REGISTER_DATA        0x005
#define AP_TARGET_EXTERNAL_PARAMETER_0     0x006
#define AP_TARGET_EXTERNAL_PARAMETER_1     0x007
#define AP_TARGET_MASK                     0x007
#define AP_TARGET_SHIFT                    0

#define AP_TRANSACTION_TYPE_DISABLED       0x000
#define AP_TRANSACTION_TYPE_WRITE          0x010
#define AP_TRANSACTION_TYPE_READ           0x020
#define AP_TRANSACTION_TYPE_ACCESS         0x030
#define AP_TRANSACTION_TYPE_MASK           0x030
#define AP_TRANSACTION_TYPE_SHIFT          4

#define AP_MODE_TRIGGER_IN_RANGE           0x000
#define AP_MODE_TRIGGER_OUTSIDE_RANGE      0x040
#define AP_MODE_MASK                       0x040

#define AP_ACTION_BREAK                    0x000
#define AP_ACTION_EXCEPTION                0x100
#define AP_ACTION_MASK                     0x100

#define AP_PAIR                            0x080
#define AP_QUAD                            0x200


/* Data defining the actionpoints.  */
static unsigned int       num_actionpoints;
static Boolean            full_target_actionpoints;
static ARC_ActionPoint    actionpoints[MAX_ACTION_POINTS];

/* The h/w numbers of the AMV0, AMM0 and AC0 auxiliary registers.  */
static ARC_RegisterNumber amv0_regnum;
static ARC_RegisterNumber amm0_regnum;
static ARC_RegisterNumber ac0_regnum;


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

/* The N actionpoint auxiliary registers (where N is 0 .. MAX_ACTION_POINTS - 1)
   (fortunately, the numbers of the registers are one contiguous block, so
   a simple addition is sufficient here).  */
#define ARC_HW_AMV_REGNUM(n)     (ARC_RegisterNumber) (amv0_regnum + 3 * (n))
#define ARC_HW_AMM_REGNUM(n)     (ARC_RegisterNumber) (amm0_regnum + 3 * (n))
#define ARC_HW_AC_REGNUM(n)      (ARC_RegisterNumber) (ac0_regnum  + 3 * (n))

/* This will give a value in range 0 .. MAX_ACTION_POINTS - 1.  */
#define AP_INSTANCE(ap)          ((ap) - actionpoints)
#define IN_USE(ap)               ((ap)->usage != NOT_IN_USE)


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

#if 0
/* This function checks that a given number is a power of two, and, if so,
   returns a bitmask corresponding to (2 ** number - 1).  */

static Boolean
is_power_of_two (int number, ARC_Word *mask)
{
    unsigned int power = 1;
    unsigned int i;

    *mask = 0;

    for (i = 0; (i < BITS_IN_WORD) && (power <= (unsigned int) number); i++)
    {
        if (power == (unsigned int) number)
            return TRUE;

        power <<= 1;
        *mask = (*mask << 1) + 1;
    }

    /* not a power */
    return FALSE;
}
#endif


/* This function determines whether the ARC processor in the connected target
   provides support for actionpoints (that is a configuratiopn option).  */

static Boolean
target_has_actionpoints (void)
{
    ARC_RegisterNumber   ap_build_regnum;
    ARC_RegisterContents ap_build;

    ap_build_regnum = arc_aux_find_register_number("AP_BUILD", ARC_HW_AP_BUILD_REGNUM);
    amv0_regnum     = arc_aux_find_register_number("AMV0",     ARC_HW_AMV0_REGNUM);
    amm0_regnum     = arc_aux_find_register_number("AMM0",     ARC_HW_AMM0_REGNUM);
    ac0_regnum      = arc_aux_find_register_number("AC0",      ARC_HW_AC0_REGNUM);

    num_actionpoints = 0;

    if (arc_read_jtag_aux_register(ap_build_regnum, &ap_build, TRUE))
    {
        DEBUG("AP_BUILD BCR: 0x%08X\n", ap_build);

        /* AP_BUILD returns 0 if actionpoints are not selected in the
           processor configuration.  */
        if (ap_build != 0)
        {
            /* If the processor's implementation of the actionpoint mechanism is
               the one we know about.  */
            if ((ap_build & AP_BUILD_VERSION) == 0x4)
            {
                 unsigned int type = (ap_build & AP_BUILD_TYPE) >> AP_BUILD_TYPE_SHIFT;
                 unsigned int i;

                 /* The low-order two bits of the type field encode the number
                    of actionpoints supported  by the processor.  */
                 switch (type % 4)
                 {
                     case 0 : num_actionpoints = 2; break;
                     case 1 : num_actionpoints = 4; break;
                     case 2 : num_actionpoints = 8; break;
                     default: internal_error (__FILE__, __LINE__, _("invalid AP_BUILD.TYPE: 0x%X"), type);
                 }

                 /* The next bit specifies whether the processor supports full
                    or minimum targets for the actionpoints.  */
                 full_target_actionpoints = ((type & 4) == 0);

                 for (i = 0; i < MAX_ACTION_POINTS; i++)
                 {
                     actionpoints[i].usage      = NOT_IN_USE;
                     actionpoints[i].triggered  = FALSE;
                     actionpoints[i].is_exclude = FALSE;
                 }

                 DEBUG("ARC processor supports %u %s actionpoints\n",
                       num_actionpoints, (full_target_actionpoints) ? "full" : "minimum");

                 return TRUE;
            }
            else
                warning(_("ARC processor actionpoint mechanism version (0x%x) is not supported."),
                        ap_build & AP_BUILD_VERSION);
        }
    }

    DEBUG("ARC processor supports no actionpoints\n");

    return FALSE;
}


/* This function determines the set of actionpoints that would be required to
   cover exactly the memory region specified by (addr,length), by using one
   actionpoint with an inclusive range, and zero or more actionpoints with
   exclusive ranges.

   The set of values and masks for the actionpoint AMV and AMM registers are
   returned in the actionpoint_value and actionpoint_mask arrays - these must
   be able to hold 4 entries.  The value and mask for the inclusive actionpoint
   are returned as the first elements in the arrays.  */

static unsigned int
map_actionpoints (ARC_Address  addr,
                  unsigned int length,
                  ARC_Address  actionpoint_value[],
                  ARC_Word     actionpoint_mask[])
{
    ARC_Address  first_data = addr;
    ARC_Address  last_data  = first_data + length - 1;
    ARC_Address  include_start;
    ARC_Address  include_end;
    ARC_Word     mask;
    unsigned int power_of_two;
    unsigned int points;

    ENTERARGS("addr 0x%08X, length %d", addr, length);
//  DEBUG("range: %08X .. %08X\n", first_data, last_data);

    /* If the range extends across the midpoint of the address space.  */
    if (((first_data & 0x80000000) == 0) && ((last_data & 0x80000000) != 0))
    {
//      DEBUG("pathological case!\n");

        /* Must cover entire address space.  */
        include_start = 0x00000000;
        include_end   = 0xFFFFFFFF;
        mask          = 0xFFFFFFFF;   // Ignore all bits!
    }
    else
    {
        unsigned int i;

        mask         = 0;
        power_of_two = 1;

        /* Determine what actionpoint would be required to include all of the given
           memory region (this include range may have leading and trailing parts
           that extend beyond the given region).  */
        for (i = 0; i < 32; i++)
        {
            unsigned int include_size = power_of_two;

            include_start = (first_data / include_size) * include_size;
            include_end   = include_start + include_size - 1;

            if (include_start <= first_data && include_end >= last_data)
            {
//              DEBUG("include: 0x%08X .. 0x%08X (mask 0x%08x)\n",
//                     include_start, include_end, mask);
                break;
            }

            mask = (mask << 1) + 1;
            power_of_two <<= 1;
        }
    }

    /* This is the first actionpoint in the list.  */
    actionpoint_value[0] = include_start;
    actionpoint_mask [0] = mask;
    points = 1;


    /* Determine what actionpoints would be required to mask out the leading part
       of the include range.  */
    {
        unsigned int to_be_excluded = first_data - include_start;
        ARC_Address  boundary       = include_start;

        while (to_be_excluded > 0)
        {
            unsigned int j;

            mask         = 0;
            power_of_two = 1;

            for (j = 0; j < 32; j++)
            {
                unsigned int exclude_size  = power_of_two;
                ARC_Address  exclude_start = (first_data / exclude_size - 1) * exclude_size;
                ARC_Address  exclude_end   = exclude_start + exclude_size - 1;

                if (exclude_end < first_data && exclude_start <= boundary)
                {
//                  DEBUG("leading exclude: 0x%08X .. 0x%08X (mask 0x%08x)\n",
//                        exclude_start, exclude_end, mask);

                    to_be_excluded = first_data - exclude_end - 1;
                    boundary       = exclude_end + 1;

                    /* There is no point in returning the details of
                       more than the maximum number of actionpoints that
                       can be linked together (in a quad).  */
                    if (points < MAX_ACTION_POINTS_IN_GROUP)
                    {
                        actionpoint_value[points] = exclude_start;
                        actionpoint_mask [points] = mask;
                    }
                    points++;

                    break;
                }

                mask = (mask << 1) + 1;
                power_of_two <<= 1;
            }
        }
    }

    /* Determine what actionpoints would be required to mask out the trailing
       part of the include range.  */
    {
        unsigned int to_be_excluded = include_end - last_data;
        ARC_Address  boundary       = include_end;

        while (to_be_excluded > 0)
        {
            unsigned int j;

            mask         = 0;
            power_of_two = 1;

            for (j = 0; j < 32; j++)
            {
                unsigned int exclude_size  = power_of_two;
                ARC_Address  exclude_start = (last_data / exclude_size + 1) * exclude_size;
                ARC_Address  exclude_end   = exclude_start + exclude_size - 1;

                if (exclude_start > last_data && exclude_end >= boundary)
                {
//                  DEBUG("trailing exclude: 0x%08X .. 0x%08X (mask 0x%08x)\n",
//                        exclude_start, exclude_end, mask);

                    to_be_excluded = exclude_start - last_data - 1;
                    boundary       = exclude_start - 1;

                    /* There is no point in returning the details of
                       more than the maximum number of actionpoints that
                       can be linked together (in a quad).  */
                    if (points < MAX_ACTION_POINTS_IN_GROUP)
                    {
                        actionpoint_value[points] = exclude_start;
                        actionpoint_mask [points] = mask;
                    }
                    points++;

                    break;
                }

                mask = (mask << 1) + 1;
                power_of_two <<= 1;
            }
        }
    }

    return points;
}


/* Set the given actionpoint on the target by writing to the corresponding set
   of AMV, AMM and AC auxiliary registers.
   Return TRUE if it is set successfully.  */

static Boolean
set_actionpoint_on_target (ARC_ActionPoint *actionpoint)
{
    unsigned int instance = AP_INSTANCE(actionpoint);

    return arc_write_jtag_aux_register(ARC_HW_AMV_REGNUM(instance), actionpoint->match_value, TRUE) &&
           arc_write_jtag_aux_register(ARC_HW_AMM_REGNUM(instance), actionpoint->match_mask,  TRUE) &&
           arc_write_jtag_aux_register(ARC_HW_AC_REGNUM (instance), actionpoint->control,     TRUE);
}


/* Clear the given actionpoint on the target by writing 'DISABLED' to the
   corresponding AC auxiliary register.
   Return TRUE if it is cleared successfully.  */

static Boolean
clear_actionpoint_from_target (ARC_ActionPoint *actionpoint)
{
    return arc_write_jtag_aux_register(ARC_HW_AC_REGNUM (AP_INSTANCE(actionpoint)),
                                       AP_TRANSACTION_TYPE_DISABLED,
                                       TRUE);
}


/* Set the given actionpoint on the target, and update its data structure.
   Return TRUE if it is set successfully.  */

static Boolean
set_actionpoint (ARC_ActionPoint *actionpoint)
{
    Boolean set = set_actionpoint_on_target(actionpoint);

    if (set)
    {
        actionpoint->triggered = FALSE;
        actionpoint->point     = 0;
    }
    else
        actionpoint->usage = NOT_IN_USE;

    return set;
}


/* Insert an actionpoint to cover a range of target memory.

   Parameters:
      bpt        : the information describing a breakpoint (NULL for a watchpoint)
      length     : the length in bytes of the range
      match_value: the value for the actionpoint value   (AMV) aux register
      match_mask : the value for the actionpoint mask    (AMM) aux register
      control    : the value for the actionpoint control (AC)  aux register

   Returns 0 for success, -1 for failure.  */

static int
insert_actionpoint (struct bp_target_info *bpt,
                    unsigned int           length,
                    ARC_RegisterContents   match_value,
                    ARC_RegisterContents   match_mask,
                    ARC_RegisterContents   control)
{
    unsigned int i;

    /* Look for an unused actionpoint.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        /* Got one!  */
        if (!IN_USE(actionpoint))
        {
            /* Record its data.  */
            actionpoint->match_value = match_value;
            actionpoint->match_mask  = match_mask;
            actionpoint->control     = control;
            actionpoint->is_exclude  = FALSE;
            actionpoint->length      = length;

            /* Try to set it on the target.  */
            if (set_actionpoint(actionpoint))
            {
                 /* Now it is in use.  */
                actionpoint->usage = SINGLE;

                /* Is it a breakpoint?  */
                if (bpt)
                {
                    /* We have not actually saved code from the target program.  */
                    bpt->shadow_len  = 0;
                    bpt->placed_size = (int) actionpoint->length;
                }

                return SUCCESS;
            }
        }
    }

    /* Failed: no free actionpoints.  */
//  warning(_("no actionpoints available"));
    return FAILURE;
}


/* Restore the actionpoints on the target according to their current settings.
   If 'clear_unused' is TRUE, any actionpoints which are unused are explicitly
   cleared on the target.

   Returns 0 for success, -1 for failure.  */

static int
restore_actionpoints (Boolean clear_unused)
{
    unsigned int i;

    /* Look at each of the actionpoints.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (IN_USE(actionpoint))
        {
            if (!set_actionpoint_on_target(actionpoint))
            {
                actionpoint->usage = NOT_IN_USE;
                return FAILURE;
            }
        }
        else if (clear_unused)
        {
            if (!clear_actionpoint_from_target (actionpoint))
                return FAILURE;
        }
    }

    return SUCCESS;
}


/* Find a number of unused actionpoints whose numbers (0..7) lie in a contiguous
   range (allowing for wraparound of the numbers, i.e. % 8).

   Parameters:
      required : the number of unused actionpoints required
      from     : set to the number (0..7) of the first actionpoint
      compacted: set to TRUE if the currently used set of actionpoints
                 had to be compacted to give a contiguous range of unused
                 actionpoints

   Returns TRUE if the required number was found, FALSE otherwise.  */

static Boolean
find_unused_actionpoints (unsigned int  required,
                          unsigned int *from,
                          Boolean      *compacted)
{
    unsigned int unused        = 0;
    unsigned int first_unused  = 0;
    unsigned int i;

    /* How many slots are not currently used?  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (!IN_USE(actionpoint))
            unused++;
    }

    DEBUG("%u actionpoints unused, %u required\n", unused, required);

    if (required > unused)
        return FALSE;

    /* When used in pairs or quads, the action points wrap around, e.g. a pair
       might be actionpoints (3, 0), if the target has 4 actionpoints; and a
       quad might be (6, 7, 0, 1), if the target has 8 actionpoints.  */

    /* First try to find 'required' contiguous unused slots.  */
    for (i = 0; i < num_actionpoints + required - 2; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i % num_actionpoints];

        if (IN_USE(actionpoint))
        {
            /* The first unused one MAY be the next one after this one.  */
            first_unused = i + 1;
        }
        else
        {
            DEBUG("%u: AP%u is unused\n", i, i % num_actionpoints);

            if (i - first_unused + 1 >= required)
            {
                /* A sufficiently large sequence of unused actionpoints has been
                   found.  */
                *from = first_unused % num_actionpoints;
                *compacted = FALSE;
                return TRUE;
            }
        }
    }

    DEBUG("compacting array\n");

    /* There are sufficient unused slots, but they are not contiguous - so move
       all the used ones towards the start of the array so that all the unused
       ones are contiguous at the end of the array.  */
    first_unused = MAX_ACTION_POINTS;

    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (IN_USE(actionpoint))
        {
            if (first_unused != MAX_ACTION_POINTS)
            {
                DEBUG("moving %u to %u\n", i, first_unused);

                /* Move the used one into the unused slot.  */
                actionpoints[first_unused] = *actionpoint;

                actionpoint->usage = NOT_IN_USE;

                /* The first unused entry in the array is now the next one after
                   it - this is true whether that next one was the used one that
                   has just been moved, or was the next in a sequence of unused
                   entries.  */
                first_unused++;
            }
        }
        else if (first_unused == MAX_ACTION_POINTS)
        {
            /* This one really is the first unused one we have found.  */
            first_unused = i;
        }
    }

    *from = num_actionpoints - unused;

    DEBUG("from = %u\n", *from);

    *compacted = TRUE;
    return TRUE;
}


/* Insert an actionpoint group to cover a range of target memory.

   Parameters:
      length     : the length in bytes of the range
      number     : the number of actionpoints required
      match_value: the values for the actionpoint value  (AMV) aux registers
      match_mask : the values for the actionpoint mask   (AMM) aux registers
      control    : the value for the actionpoint control (AC)  aux registers

   Returns 0 for success, -1 for failure.  */

static int
insert_actionpoint_group (unsigned int         length,
                          unsigned int         number,
                          ARC_RegisterContents match_value[],
                          ARC_RegisterContents match_mask[],
                          ARC_RegisterContents control)
{
    /* For 2 actionpoints, we can use a pair; for 3 or 4, we must use a quad.  */
    unsigned int required = (number == 2) ? 2 : 4;
    unsigned int first_free;
    Boolean      is_pair  = (required == 2);
    Boolean      compacted;

    gdb_assert(2 <= number && number <= 4);

    /* Try to find the required number of unused actionpoints.  */
    if (find_unused_actionpoints(required, &first_free, &compacted))
    {
        ARC_ActionPoint *actionpoint[MAX_ACTION_POINTS_IN_GROUP];
        unsigned int     i;

        /* Get an array of pointers to the data for those actionpoints.  */
        for (i = 0; i < required; i++)
            actionpoint[i] = &actionpoints[(first_free + i) % num_actionpoints];

        actionpoint[0]->length     = length;
        actionpoint[0]->is_exclude = FALSE;

        /* The Control register for the first actionpoint in the group must be
           set to indicate whether the group is a pair or a quad.  */
        actionpoint[0]->usage       = (is_pair) ? PAIR_0 : QUAD_0;
        actionpoint[0]->match_value = match_value[0];
        actionpoint[0]->match_mask  = match_mask[0];
        actionpoint[0]->control     = control | ((is_pair) ? AP_PAIR : AP_QUAD);

        /* All subsequent actionpoints in the group have exclusive rather than
           inclusive address ranges.  */
        control &= ~AP_MODE_TRIGGER_IN_RANGE;
        control |=  AP_MODE_TRIGGER_OUTSIDE_RANGE;

        for (i = 1; i < number; i++)
        {
            actionpoint[i]->usage       = actionpoint[0]->usage + i;
            actionpoint[i]->match_value = match_value[i];
            actionpoint[i]->match_mask  = match_mask[i];
            actionpoint[i]->control     = control;
            actionpoint[i]->length      = 0;
            actionpoint[i]->is_exclude  = TRUE;
        }

        /* If we are using only 3 of the 4 actionpoints in a quad, the 4th one
           must be disabled (or we could just make it the same as one of the
           other exclusive ones).  */
        if (number == 3)
        {
            ARC_ActionPoint *disabled = actionpoint[3];

            disabled->usage       = QUAD_3;
            disabled->match_value = 0;
            disabled->match_mask  = 0;
            disabled->control     = AP_TRANSACTION_TYPE_DISABLED;
            disabled->length      = 0;
        }

        /* If we had to compact the array of actionpoints in order to get a
           long enough contiguous sequence of unused entries, then set ALL of
           the actionpoints that are now in use, and explicitly clear all that
           are not in use (this is simplest!).  */
        if (compacted)
            return restore_actionpoints(TRUE);

        /* Otherwise, just set the ones for the group, which were previously
           unused.  */
        for (i = 0; i < required; i++)
            if (!set_actionpoint(actionpoint[i]))
                return FAILURE;

        return SUCCESS;
    }

//  warning(_("insufficient actionpoints available"));
    return FAILURE;
}


/* Insert a h/w breakpoint or watchpoint to cover a range of target memory.

   Parameters:
      address : the start address of the range
      control : the value for the actionpoint control (AC) aux register
      length  : the length in bytes of the range
      bpt     : the information describing the breakpoint (NULL for a watchpoint)

   Returns 0 for success, -1 for failure.  */

static int
insert_range (ARC_RegisterContents   address,
              ARC_RegisterContents   control,
              unsigned int           length,
              struct bp_target_info *bpt)
{
    /* At most 4 actionpoints can be connected (as a quad).  */
    ARC_Address  actionpoint_value[MAX_ACTION_POINTS_IN_GROUP];
    ARC_Word     actionpoint_mask [MAX_ACTION_POINTS_IN_GROUP];
    unsigned int actionpoints_needed;

    /* Work out how many actionpoints would be required to exactly cover the
       given memory range.  */
    actionpoints_needed = map_actionpoints(address,
                                           length,
                                           actionpoint_value,
                                           actionpoint_mask);

    if (actionpoints_needed == 1)
        return insert_actionpoint(bpt,
                                  length,
                                  actionpoint_value[0],
                                  actionpoint_mask[0],
                                  control);

    if (actionpoints_needed <= MAX_ACTION_POINTS_IN_GROUP)
        return insert_actionpoint_group(length,
                                        actionpoints_needed,
                                        actionpoint_value,
                                        actionpoint_mask,
                                        control);

    warning (_("break/watchpoint would require %u linked actionpoints, "
               "but at most %u actionpoints may be linked together"),
             actionpoints_needed, MAX_ACTION_POINTS_IN_GROUP);

    return FAILURE;
}


/* Remove an actionpoint from a range of target memory.

   Parameters:
      address : the start address of the range
      length  : the length in bytes of the range

   Returns -1 for failure, 0 for success.  */

static int
remove_actionpoint (CORE_ADDR address, unsigned int length)
{
    unsigned int i;

    /* Look at all the actionpoints.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (IN_USE(actionpoint) && !actionpoint->is_exclude)
        {
            if (actionpoint->match_value == (ARC_RegisterContents) address &&
                actionpoint->length      == length)
            {
                unsigned int points;
                unsigned int p;

                /* Is this the first of a pair or quad?  */
                if ((actionpoint->control & AP_PAIR) != 0)
                    points = 2;
                else if ((actionpoint->control & AP_QUAD) != 0)
                    points = 4;
                else
                    points = 1;

//              DEBUG("points = %u\n", points);

                for (p = 1; p < points; p++)
                {
                    ARC_ActionPoint *next = &actionpoints[(i + p) % num_actionpoints];

                    if (clear_actionpoint_from_target (next))
                        next->usage = NOT_IN_USE;
                    else
                        return FAILURE;
                }

                if (clear_actionpoint_from_target (actionpoint))
                {
                    actionpoint->usage = NOT_IN_USE;
                    return SUCCESS;
                }

                break;
            }
        }
    }

    /* Failed: could not find actionpoint, or could not clear it.  */
    return FAILURE;
}


/* -------------------------------------------------------------------------- */
/*               local functions called from outside this module              */
/* -------------------------------------------------------------------------- */

/* Check if we can set a hardware watchpoint of type TYPE.  TYPE is
   one of bp_hardware_watchpoint, bp_read_watchpoint, bp_write_watchpoint, or
   bp_hardware_breakpoint.  COUNT is the number of such watchpoints used so far
   (including this one).  OTHERTYPE is the total number of hardware breakpoints
   and watchpoints of other types that are "already" set (0 if type == bp_hardware_breakpoint).

   Result:   0 if hardware watchpoints are not supported
            -1 if there are not enough hardware watchpoints
             1 if there are enough hardware watchpoints

   N.B. this is not what is stated in target.h, but it does conform to the use
        made of this function's result in breakpoint.c!  */

static int
arc_debug_can_use_hw_breakpoint (int type, int count, int othertype)
{
    ENTERARGS("type %d, count %d", type, count);

    if (num_actionpoints == 0)
        return 0;

    /* N.B. this will sometimes give a "false positive" result, i.e. that there
            sufficient actionpoints available when in fact there are not: the
            ARC processor actionpoints can be used for all of the types, but gdb
            assumes that there are separate sets of resources for breakpoints
            and watchpoints, and when asking for a breakpoint does not give the
            number of watchpoints "already" set.

            It is not possible simply to check how many actionpoints are currently
            set, as gdb does not actually set the breakpoints and watchpoints
            until program execution is started or resumed - so when this function
            is called, none are actually set.

            Also, the breakpoints and watchpoints may require pairs or quads of
            actionpoints, rather than single actionpoints, and this will not be
            known until they are set, and their addresses and ranges are known!  */
    return ((int) num_actionpoints >= count + othertype) ? 1 : -1;
}


/* Insert a hardware breakpoint on the target.
   Returns 0 for success, -1 for failure.  */

static int
arc_debug_insert_hw_breakpoint (struct bp_target_info *bpt)
{
    ARC_RegisterContents control = AP_TARGET_INSTRUCTION_ADDRESS |
                                   AP_TRANSACTION_TYPE_READ      |
                                   AP_MODE_TRIGGER_IN_RANGE      |
                                   AP_ACTION_BREAK;

    ENTERARGS("0x%x : %u", (unsigned int) bpt->placed_address, bpt->range);

    /* Is it a range breakpoint?  */
    if (bpt->range)
        return insert_range((ARC_RegisterContents) bpt->placed_address,
                            control,
                            bpt->range,
                            bpt);

    /* No, just a single-instruction breakpoint?  */
    return insert_actionpoint(bpt,
                              HW_BP_SIZE,
                              (ARC_RegisterContents) bpt->placed_address,
                              0,    /* All bits of address.  */
                              control);
}


/* Remove a hardware breakpoint from the target.
   Returns 0 for success, non-zero for failure.  */

static int
arc_debug_remove_hw_breakpoint (struct bp_target_info *bpt)
{
    unsigned int range = (bpt->range) ? bpt->range : HW_BP_SIZE;

    ENTERARGS("0x%x : %u", (unsigned int) bpt->placed_address, range);

    return remove_actionpoint(bpt->placed_address, range);
}


/* Insert a hardware watchpoint on the target.

   Parameters:
      addr  : the start address of the region of memory to be watched
      length: the length in bytes of the region of memory
      type  : 0 => write, 1 => read, 2 => read/write

   Returns 0 for success, -1 for failure.  */

static int
arc_debug_insert_watchpoint (CORE_ADDR addr, int length, int type)
{
    ARC_RegisterContents control = AP_TARGET_LOAD_STORE_ADDRESS |
                                   AP_MODE_TRIGGER_IN_RANGE     |
                                   AP_ACTION_BREAK;

    ENTERARGS("0x%08X:%d %d", (unsigned int) addr, length, type);

    gdb_assert(length > 0);

    switch (type)
    {
        case 0:
            control |= AP_TRANSACTION_TYPE_WRITE;  break;
        case 1:
            control |= AP_TRANSACTION_TYPE_READ;   break;
        case 2:
            control |= AP_TRANSACTION_TYPE_ACCESS; break;
        default:
            internal_error (__FILE__, __LINE__, _("invalid watchpoint type: %d"), type);
    }

    return insert_range((ARC_RegisterContents) addr,
                        control,
                        (unsigned int) length,
                        NULL);
}


/* Remove a hardware watchpoint from the target.

   Parameters:
      addr  : the start address of the region of memory being watched
      length: the length in bytes of the region of memory
      type  : 0 => write, 1 => read, 2 => read/write

   Returns 0 for success, non-zero for failure.  */

static int
arc_debug_remove_watchpoint (CORE_ADDR addr, int length, int type)
{
    ENTERARGS("0x%x:%d %d", (unsigned int) addr, length, type);

    return remove_actionpoint(addr, (unsigned int) length);
}


/* Returns non-zero if the execution of the target program has been stopped by
   the trigger of a hardware watchpoint (i.e. on memory read or write), zero
   otherwise.  */

static int
arc_debug_stopped_by_watchpoint (void)
{
    unsigned int i;

    ENTERMSG;

    /* Look at all of the actionpoints.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (IN_USE(actionpoint) && actionpoint->triggered)
        {
            /* Is it a memory read or write actionpoint?  */
            if ((actionpoint->control & AP_TARGET_LOAD_STORE_ADDRESS) != 0)
            {
                DEBUG("actionpoint %d (load/store) triggered\n", i);
                return 1;
            }
        }
    }

    return 0;
}


/* Get the address of the data that was read/written causing a h/w watchpoint to
   trigger; the address is returned in the '*addr' parameter.
   Returns 0 for failure, non-zero for success.  */

static int
arc_debug_stopped_data_address (struct target_ops *ops, CORE_ADDR *addr)
{
    unsigned int i;

    ENTERMSG;

    /* Look at each of the actionpoints.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        DEBUG("AP%u: in use = %d, triggered = %d\n", i, IN_USE(actionpoint), actionpoint->triggered);

        /* If this actionpoint has been triggered.  */
        if (IN_USE(actionpoint) && actionpoint->triggered)
        {
            /* Is it a memory read or write actionpoint?  */
            if ((actionpoint->control & AP_TARGET_LOAD_STORE_ADDRESS) != 0)
            {
                DEBUG("actionpoint %d (load/store) triggered by access at 0x%08X\n", i, actionpoint->point);

                /* OK, got the data address!  */
                *addr = (CORE_ADDR) actionpoint->point;
                return 1;
            }
        }
    }

   DEBUG("no watchpoint triggered\n");

    return 0;
}


/* Can a h/w watchpoint 'length' bytes long be set at address 'addr' in target memory?  */

static int
arc_debug_region_ok_for_hw_watchpoint (CORE_ADDR addr, int length)
{
    /* As far as we know, we can set a h/w watchpoint anywhere!  */
    return 1;
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* This function is called after a reset of the target has been performed (which
   clears all the aux registers associated with actionpoints).  It attempts to
   restore all actionpoints to their pre-reset settings.

   Returns TRUE if the actionpoints are restored, FALSE otherwise.  */

Boolean
arc_restore_actionpoints_after_reset (void)
{
    return (restore_actionpoints(FALSE) == SUCCESS);
}


/* If the debug target supports actionpoints, set up the function pointers in
   the given target operations structure to point to the functions which
   implement the associated operations.

   Returns TRUE if actionpoints are supported, FALSE otherwise.  */

Boolean
arc_initialize_actionpoint_ops (struct target_ops *debug_ops)
{
    if (target_has_actionpoints())
    {
        debug_ops->to_can_use_hw_breakpoint       = arc_debug_can_use_hw_breakpoint;
        debug_ops->to_insert_hw_breakpoint        = arc_debug_insert_hw_breakpoint;
        debug_ops->to_remove_hw_breakpoint        = arc_debug_remove_hw_breakpoint;
        debug_ops->to_insert_watchpoint           = arc_debug_insert_watchpoint;
        debug_ops->to_remove_watchpoint           = arc_debug_remove_watchpoint;
        debug_ops->to_stopped_by_watchpoint       = arc_debug_stopped_by_watchpoint;
        debug_ops->to_stopped_data_address        = arc_debug_stopped_data_address;
        debug_ops->to_region_ok_for_hw_watchpoint = arc_debug_region_ok_for_hw_watchpoint;

        /* This is the default, but just to make it clear that watchpoints must
           be cleared before execution can resume.  */
        debug_ops->to_have_continuable_watchpoint = 0;

        return TRUE;
    }

    return FALSE;
}


/* Display all the target actionpoints.  */

void
arc_display_actionpoints (void)
{
    unsigned int i;

    char *targets[8] =
    {
        _("Instruction Address"),
        _("Instruction Data"),
        _("Load/Store Address"),
        _("Load/Store Data"),
        _("Aux Register Address"),
        _("Aux Register Data"),
        _("Ext Parameter 0"),
        _("Ext Parameter 1")
    };

    char *transactions[4] =
    {
        _("disabled"),
        _("write"),
        _("read"),
        _("read/write")
    };

    char *explanations[8] =
    {
        _("execution of instruction at address"),
        _("execution of instruction"),
        _("load or store of data at address"),
        _("load or store of data"),
        _("read or write of auxiliary register"),
        _("read or write of auxiliary register contents"),
        _("value"),
        _("value")
    };


    /* Look at each of the actionpoints in turn.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        if (IN_USE(actionpoint))
        {
            ARC_RegisterContents control = actionpoint->control;
            const unsigned int   targ    = (control & AP_TARGET_MASK          ) >> AP_TARGET_SHIFT;
            const unsigned int   trans   = (control & AP_TRANSACTION_TYPE_MASK) >> AP_TRANSACTION_TYPE_SHIFT;
            const char          *target  = targets     [targ];
            const char          *type    = transactions[trans];
            const char          *mode    = ((control & AP_MODE_MASK) ==
                                             AP_MODE_TRIGGER_OUTSIDE_RANGE) ? _("outside range") : _("in range");
            const char          *action  = ((control & AP_ACTION_MASK) ==
                                             AP_ACTION_BREAK) ? _("break") : _("raise exception");
            const char          *usage;

            switch (actionpoint->usage)
            {
                case SINGLE: usage = _("         "); break;
                case PAIR_0: usage = _(" (Pair 0)"); break;
                case PAIR_1: usage = _(" (Pair 1)"); break;
                case QUAD_0: usage = _(" (Quad 0)"); break;
                case QUAD_1: usage = _(" (Quad 1)"); break;
                case QUAD_2: usage = _(" (Quad 2)"); break;
                case QUAD_3: usage = _(" (Quad 3)"); break;
                default:
                    internal_error (__FILE__, __LINE__, _("invalid AP usage: %u"), actionpoint->usage);
                    return;
            }

            printf_filtered(_("AP %u%s :: "), i, usage);

            printf_filtered(                     _("value    : %08X\n"),                 actionpoint->match_value);
            printf_filtered(    _("                 mask     : %08X\n"),                 actionpoint->match_mask);
            if ((control & AP_TRANSACTION_TYPE_MASK) == AP_TRANSACTION_TYPE_DISABLED)
                printf_filtered(_("                 control  : %08X disabled\n"),        actionpoint->control);
            else
                printf_filtered(_("                 control  : %08X %s, %s on %s %s\n"), actionpoint->control, target, action, type, mode);
            if (actionpoint->triggered)
            {
                const char *explain = explanations[targ];

                printf_filtered(_("                 triggered by %s %08x\n"), explain,   actionpoint->point);
            }
        }
        else
        {
            printf_filtered(_("AP %u          :: not in use\n"), i);
        }
    }
}


/* This function is called as soon as execution of the target program has halted.
   It checks whether the halt is due to an actionpoint trigger, and, if so,
   identifies the actionpoint that has been triggered and finds the address (code
   or data) at which memory access (read, write or execute) has caused the trigger.  */

void
arc_target_halted (void)
{
    ARC_RegisterContents debug;

    ENTERMSG;

    if (arc_read_jtag_aux_register(arc_debug_regnum, &debug, TRUE))
    {
        /* If the bit indicating that an actionpoint has halted the processor is
           set.  */
        if ((debug & DEBUG_ACTIONPOINT_HALT) != 0)
        {
            /* Get the Actionpoints Status Register from the DEBUG register:
               this contains one bit for each actionpoint in the processor
               configuration.  */
            unsigned int ASR = (debug & DEBUG_ACTIONPOINT_STATUS) >>
                                DEBUG_ACTIONPOINT_STATUS_SHIFT;
            unsigned int i;

            /* Now look at each of the actionpoints.  */
            for (i = 0; i < num_actionpoints; i++)
            {
                ARC_ActionPoint *actionpoint = &actionpoints[i];

                actionpoint->triggered = FALSE;

                /* Is the ASR bit for this actionpoint set?  */
                if ((ASR & 1) != 0)
                {
                    if (IN_USE(actionpoint))
                    {
                        actionpoint->triggered = TRUE;

                        /* The AMV register for this action point has been
                           updated with the address to which access has caused
                           the actionpoint to trigger.  */
                        (void) arc_read_jtag_aux_register(ARC_HW_AMV_REGNUM(AP_INSTANCE(actionpoint)),
                                                          &actionpoint->point,
                                                          TRUE);
                    }
                    else
                        internal_error (__FILE__, __LINE__, _("actionpoint %u triggered but not set"), i);
                }

                ASR >>= 1;
            }
        }
    }
}


/* For debugging - just give the values.  */

void
arc_dump_actionpoints (const char *message)
{
    unsigned int i;

    DEBUG("%s\n", message);

    /* Look at each of the actionpoints in turn.  */
    for (i = 0; i < num_actionpoints; i++)
    {
        ARC_ActionPoint *actionpoint = &actionpoints[i];

        DEBUG("slot %u:: ", i);

        if (IN_USE(actionpoint))
        {
            DEBUG(         "value    : %08X\n", actionpoint->match_value);
            DEBUG("         mask     : %08X\n", actionpoint->match_mask);
            DEBUG("         control  : %08X\n", actionpoint->control);
            DEBUG("         triggered: %u\n",   actionpoint->triggered);
            DEBUG("         point    : %08x\n", actionpoint->point);
        }
        else
        {
            DEBUG("not in use\n");
        }
    }
}

/******************************************************************************/
