/* Mips simulator watchpoint support.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"
#include "sim-options.h"

#include "sim-assert.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <signal.h>

static DECLARE_OPTION_HANDLER (watch_option_handler);

enum {
  OPTION_WATCH_DELETE                      = OPTION_START,

  OPTION_WATCH_PC,
  OPTION_WATCH_CLOCK,
  OPTION_WATCH_CYCLES,

  OPTION_ACTION_PC,
  OPTION_ACTION_CLOCK,
  OPTION_ACTION_CYCLES,
};


static void
delete_watchpoint (SIM_DESC sd, watchpoint_type type)
{
  sim_watch_point *point = &STATE_WATCHPOINTS (sd)->points[type];
  if (point->event != NULL)
    sim_events_deschedule (sd, point->event);
  point->action = invalid_watchpoint_action;
  point->event = NULL;
}


static sim_event_handler handle_watchpoint;

static SIM_RC
schedule_watchpoint (SIM_DESC sd,
		     watchpoint_type type,
		     unsigned long arg,
		     int is_within,
		     int is_command)
{
  sim_watchpoints *watch = STATE_WATCHPOINTS (sd);
  sim_watch_point *point = &watch->points[type];
  if (point->event != NULL)
    sim_events_deschedule (sd, point->event);
  point->arg = arg;
  point->is_within = is_within;
  if (point->action == invalid_watchpoint_action)
    point->action = break_watchpoint_action;
  if (is_command)
    switch (type)
      {
      case pc_watchpoint:
	point->event = sim_events_watch_sim (sd, watch->pc, watch->sizeof_pc,
					     0/* host-endian */,
					     point->is_within,
					     point->arg, point->arg, /* PC == arg? */
					     handle_watchpoint,
					     point);
	return SIM_RC_OK;
      case clock_watchpoint:
	point->event = sim_events_watch_clock (sd,
					       point->arg, /* ms time */
					       handle_watchpoint,
					       point);
	return SIM_RC_OK;
      case cycles_watchpoint:
	point->event = sim_events_schedule (sd, point->arg, /* time */
					    handle_watchpoint,
					    point);
	return SIM_RC_OK;
      default:
	sim_engine_abort (sd, NULL, NULL_CIA,
			  "handle_watchpoint - internal error - bad switch");
	return SIM_RC_FAIL;
      }
  return SIM_RC_OK;
}


static void
handle_watchpoint (SIM_DESC sd, void *data)
{
  sim_watchpoints *watch = STATE_WATCHPOINTS (sd);
  sim_watch_point *point = data;
  watchpoint_type type = point - watch->points;

  switch (point->action)
    {

    case break_watchpoint_action:
      point->event = NULL; /* gone */
      sim_engine_halt (sd, NULL, NULL, NULL_CIA, sim_stopped, SIGINT);
      break;

    case n_interrupt_watchpoint_action:
      /* First reschedule this event */
      schedule_watchpoint (sd, type, point->arg, point->is_within, 1/*is-command*/);
      /* FALL-THROUGH */

    case interrupt_watchpoint_action:
      watch->interrupt_handler (sd, NULL);
      break;

    default:
      sim_engine_abort (sd, NULL, NULL_CIA,
			"handle_watchpoint - internal error - bad switch");

    }
}


static SIM_RC
action_watchpoint (SIM_DESC sd, watchpoint_type type, const char *arg)
{
  sim_watchpoints *watch = STATE_WATCHPOINTS (sd);
  sim_watch_point *point = &watch->points[type];
  if (strcmp (arg, "break") == 0)
    {
      point->action = break_watchpoint_action;
    }
  else if (strcmp (arg, "int") == 0)
    {
      if (watch->interrupt_handler == NULL)
	{
	  sim_io_eprintf (sd, "This simulator does not support int watchpoints\n");
	  return SIM_RC_FAIL;
	}
      point->action = interrupt_watchpoint_action;
    }
  else if (strcmp (arg, "+int") == 0)
    {
      if (watch->interrupt_handler == NULL)
	{
	  sim_io_eprintf (sd, "This simulator does not support int watchpoints\n");
	  return SIM_RC_FAIL;
	}
      point->action = n_interrupt_watchpoint_action;
    }
  else
    {
      sim_io_eprintf (sd, "Interrupts other than `int' currently unsuported\n");
      return SIM_RC_FAIL;
    }
  return SIM_RC_OK;
}


static const OPTION watch_options[] =
{
  { {"watch-delete", required_argument, NULL, OPTION_WATCH_DELETE },
      '\0', "all|pc|cycles|clock", "Delete a watchpoint",
      watch_option_handler },
  { {"delete-watch", required_argument, NULL, OPTION_WATCH_DELETE },
      '\0', "all|pc|cycles|clock", NULL,
      watch_option_handler },

  { {"watch-pc", required_argument, NULL, OPTION_WATCH_PC },
      '\0', "[!] VALUE", "Watch the PC (break)",
      watch_option_handler },
  { {"watch-clock", required_argument, NULL, OPTION_WATCH_CLOCK },
      '\0', "TIME-IN-MS", "Watch the clock (break)",
      watch_option_handler },
  { {"watch-cycles", required_argument, NULL, OPTION_WATCH_CYCLES },
      '\0', "CYCLES", "Watch the cycles (break)",
      watch_option_handler },

  { {"action-pc", required_argument, NULL, OPTION_ACTION_PC },
      '\0', "break|int|+int", "Action taken by PC watchpoint",
      watch_option_handler },
  { {"action-clock", required_argument, NULL, OPTION_ACTION_CLOCK },
      '\0', "break|int|+int", "Action taken by CLOCK watchpoint",
      watch_option_handler },
  { {"action-cycles", required_argument, NULL, OPTION_ACTION_CYCLES },
      '\0', "break|int|+int", "Action taken by CYCLES watchpoint",
      watch_option_handler },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};


static SIM_RC
watch_option_handler (sd, opt, arg, is_command)
     SIM_DESC sd;
     int opt;
     char *arg;
     int is_command;
{
  switch (opt)
    {

    case OPTION_WATCH_DELETE:
      if (strcmp (arg, "all") == 0
	  || strcmp (arg, "pc") == 0)
	{
	  delete_watchpoint (sd, pc_watchpoint);
	  return SIM_RC_OK;
	}
      if (strcmp (arg, "all") == 0
	  || strcmp (arg, "clock") == 0)
	{
	  delete_watchpoint (sd, clock_watchpoint);
	  return SIM_RC_OK;
	}
      if (strcmp (arg, "all") == 0
	  || strcmp (arg, "cycles") == 0)
	{
	  delete_watchpoint (sd, cycles_watchpoint);
	  return SIM_RC_OK;
	}
      sim_io_eprintf (sd, "Unknown watchpoint type `%s'\n", arg);
      return SIM_RC_FAIL;

    case OPTION_WATCH_PC:
      if (STATE_WATCHPOINTS (sd)->pc == NULL)
	{
	  sim_io_eprintf (sd, "PC watchpoints are not supported for this simulator\n");
	  return SIM_RC_FAIL;
	}
      if (arg[0] == '!')
	return schedule_watchpoint (sd, pc_watchpoint, strtoul (arg + 1, NULL, 0),
				    0 /* !is_within */, is_command);
      else
	return schedule_watchpoint (sd, pc_watchpoint, strtoul (arg, NULL, 0),
				    1 /* is_within */, is_command);

    case OPTION_WATCH_CLOCK:
      return schedule_watchpoint (sd, clock_watchpoint, strtoul (arg, NULL, 0), 0, is_command);

    case OPTION_WATCH_CYCLES:
      return schedule_watchpoint (sd, cycles_watchpoint, strtoul (arg, NULL, 0), 0, is_command);

    case OPTION_ACTION_PC:
      return action_watchpoint (sd, pc_watchpoint, arg);

    case OPTION_ACTION_CLOCK:
      return action_watchpoint (sd, clock_watchpoint, arg);

    case OPTION_ACTION_CYCLES:
      return action_watchpoint (sd, cycles_watchpoint, arg);


    default:
      sim_io_eprintf (sd, "Unknown watch option %d\n", opt);
      return SIM_RC_FAIL;

    }

}

static SIM_RC
sim_watchpoint_init (SIM_DESC sd)
{
  /* schedule any watchpoints enabled by command line options */
  sim_watchpoints *watch = STATE_WATCHPOINTS (sd);
  watchpoint_type type;
  for (type = 0; type < nr_watchpoint_types; type++)
    {
      if (watch->points[type].action != invalid_watchpoint_action)
	schedule_watchpoint (sd, type,
			     watch->points[type].arg,
			     watch->points[type].is_within,
			     1 /*is-command*/);
    }
  return SIM_RC_OK;
}


SIM_RC
sim_watchpoint_install (SIM_DESC sd)
{
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  sim_add_option_table (sd, watch_options);
  sim_module_add_init_fn (sd, sim_watchpoint_init);
  return SIM_RC_OK;
}
