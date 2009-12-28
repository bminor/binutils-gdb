/* Data structures associated with tracepoints in GDB.
   Copyright (C) 1997, 1998, 1999, 2000, 2007, 2008, 2009
   Free Software Foundation, Inc.

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

#if !defined (TRACEPOINT_H)
#define TRACEPOINT_H 1

/* The data structure for an action: */
struct action_line
  {
    struct action_line *next;
    char *action;
  };

enum actionline_type
  {
    BADLINE = -1,
    GENERIC = 0,
    END = 1,
    STEPPING = 2
  };

/* A trace state variable is a value managed by a target being
   traced. A trace state variable (or tsv for short) can be accessed
   and assigned to by tracepoint actions and conditionals, but is not
   part of the program being traced, and it doesn't have to be
   collected. Effectively the variables are scratch space for
   tracepoints.  */

struct trace_state_variable
  {
    /* The variable's name.  The user has to prefix with a dollar sign,
       but we don't store that internally.  */
    const char *name;

    /* An id number assigned by GDB, and transmitted to targets.  */
    int number;

    /* The initial value of a variable is a 64-bit signed integer.  */
    LONGEST initial_value;

    /* 1 if the value is known, else 0.  The value is known during a
       trace run, or in tfind mode if the variable was collected into
       the current trace frame.  */
    int value_known;

    /* The value of a variable is a 64-bit signed integer.  */
    LONGEST value;
  };

extern unsigned long trace_running_p;

/* A hook used to notify the UI of tracepoint operations.  */

extern void (*deprecated_trace_find_hook) (char *arg, int from_tty);
extern void (*deprecated_trace_start_stop_hook) (int start, int from_tty);

int get_traceframe_number (void);
void free_actions (struct breakpoint *);
enum actionline_type validate_actionline (char **, struct breakpoint *);

extern void end_actions_pseudocommand (char *args, int from_tty);
extern void while_stepping_pseudocommand (char *args, int from_tty);

extern struct trace_state_variable *find_trace_state_variable (const char *name);

#endif	/* TRACEPOINT_H */
