/* This file defines the interface between the simulator and gdb.
   Copyright (C) 1993 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (REMOTE_SIM_H)
#define REMOTE_SIM_H 1

/* Main simulator globals ... */

extern int sim_verbose;

/* Main simulator entry points ...

   All functions return 0 for success and non-zero for failure.  */

/* Initialize the simulator.  This function is called when the simulator
   is selected from the command line. ARGS is passed from the command line
   and can be used to select whatever run time options the simulator provides.
   ARGS is the raw character string and must be parsed by the simulator.

   Returns 0 for success, non-zero for failure (FIXME: how do we say what
   kind of failure it was?).  */

int sim_open PARAMS ((char *name));

/* Load program PROG into the simulator.
   We use "void *" instead of "bfd *" to isolate this file from BFD.  */

int sim_load PARAMS ((void *bfd_handle, char *args));

/* Set the arguments and environment for the program loaded into the
   simulator.  ARGV and ENV are NULL terminated lists of pointers.
   If the simulator doesn't support setting arguments, print an error message
   and return non-zero.  */

int sim_set_args PARAMS ((char **argv, char **env));

/* Fetch register REGNO and store the raw value in BUF.  */

int sim_fetch_register PARAMS ((int regno, char *buf));

/* Store register REGNO from BUF (in raw format).  */

int sim_store_register PARAMS ((int regno, char *buf));

/* Kill the running program.
   This may involve closing any open files and deleting any mmap'd areas.  */

int sim_kill PARAMS ((void));

/* Read LENGTH bytes of the simulated program's memory and store in BUF.  */

int sim_read PARAMS ((CORE_ADDR mem, char *buf, int length));

/* Store LENGTH bytes from BUF in the simulated program's memory.  */

int sim_write PARAMS ((CORE_ADDR mem, char *buf, int length));

/* Print some interesting information about the simulator.  */

int sim_info PARAMS ((void));

/* Set the simulated cpu's program counter to PC.  */

int sim_set_pc PARAMS ((CORE_ADDR pc));

/* Fetch why the program stopped.  */

int sim_stop_signal PARAMS ((void));

/* Run (or resume) the program.  */

int sim_resume PARAMS ((int step, int siggnal));

#endif /* !defined (REMOTE_SIM_H) */
