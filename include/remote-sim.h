/* This file defines the interface between the simulator and gdb.
   Copyright (C) 1993, 1994, 1996, 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#if !defined (REMOTE_SIM_H)
#define REMOTE_SIM_H 1

/* This file is used when building stand-alone simulators, so isolate this
   file from gdb.  */

/* Pick up CORE_ADDR_TYPE if defined (from gdb), otherwise use same value as
   gdb does (unsigned int - from defs.h).  */

#ifndef CORE_ADDR_TYPE
typedef unsigned int SIM_ADDR;
#else
typedef CORE_ADDR_TYPE SIM_ADDR;
#endif


/* Semi-opaque type used as result of sim_open and passed back to all
   other routines.  "desc" is short for "descriptor".
   It is up to each simulator to define `sim_state'.  */

typedef struct sim_state *SIM_DESC;


/* Values for `kind' arg to sim_open.  */

typedef enum {
  SIM_OPEN_STANDALONE, /* simulator used standalone (run.c) */
  SIM_OPEN_DEBUG       /* simulator used by debugger (gdb) */
} SIM_OPEN_KIND;


/* Return codes from various functions.  */

typedef enum {
  SIM_RC_FAIL = 0,
  SIM_RC_OK = 1
} SIM_RC;


/* The bfd struct, as an opaque type.  */

struct _bfd;


/* Main simulator entry points.  */


/* Create a simulator instance.
   (This function is called when the simulator is selected from the
   gdb command line.)
   KIND specifies how the simulator will be used.  Currently there are only
   two kinds: standalone and debug.
   CALLBACK specifies a standard host callback (defined in callback.h).
   ARGV is passed from the command line and can be used to select whatever
   run time options the simulator provides.  It is the standard NULL
   terminated array of pointers, with argv[0] being the program name.
   The result is a descriptor that shall be passed to the other
   sim_foo functions.  */

SIM_DESC sim_open PARAMS ((SIM_OPEN_KIND kind, struct host_callback_struct *callback, char **argv));


/* Destory a simulator instance.
   This may involve freeing target memory and closing any open files
   and mmap'd areas.  You cannot assume sim_kill has already been
   called.
   QUITTING is non-zero if we cannot hang on errors.  */

void sim_close PARAMS ((SIM_DESC sd, int quitting));


/* Load program PROG into the simulator.
   If ABFD is non-NULL, the bfd for the file has already been opened.
   The result is a return code indicating success.  */

SIM_RC sim_load PARAMS ((SIM_DESC sd, char *prog, struct _bfd *abfd, int from_tty));


/* Prepare to run the simulated program.
   ARGV and ENV are NULL terminated lists of pointers.  */

SIM_RC sim_create_inferior PARAMS ((SIM_DESC sd, char **argv, char **env));


/* Read LENGTH bytes of the simulated program's memory and store in BUF.
   Result is number of bytes read, or zero if error.  */

int sim_read PARAMS ((SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length));


/* Store LENGTH bytes from BUF in the simulated program's memory.
   Result is number of bytes write, or zero if error.  */

int sim_write PARAMS ((SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length));


/* Fetch register REGNO and store the raw (target endian) value in BUF.  */

void sim_fetch_register PARAMS ((SIM_DESC sd, int regno, unsigned char *buf));


/* Store register REGNO from the raw (target endian) value in BUF.  */

void sim_store_register PARAMS ((SIM_DESC sd, int regno, unsigned char *buf));


/* Print whatever statistics the simulator has collected.
   VERBOSE is currently unused and must always be zero.  */

void sim_info PARAMS ((SIM_DESC sd, int verbose));


/* Run (or resume) the simulated program.  */

void sim_resume PARAMS ((SIM_DESC sd, int step, int siggnal));


/* Asynchronous request to stop the simulation.
   A nonzero return indicates that the simulator is able to handle
   the request */

int sim_stop PARAMS ((SIM_DESC sd));


/* Fetch the REASON why the program stopped.
   SIM_EXITED: The program has terminated. SIGRC indicates the target
   dependant exit status.
   SIM_STOPPED: The program has stopped.  SIGRC indicates the reason:
   program interrupted by user via a sim_stop request (SIGINT); a
   breakpoint instruction (SIGTRAP); a completed step (SIGTRAP); an
   internal error condition (SIGABRT).
   SIM_SIGNALLED: The simulator encountered target code that requires
   the signal SIGRC to be delivered to the simulated program.
   SIM_RUNNING, SIM_POLLING: The return of one of these values
   indicates a problem internal to the simulator. */

enum sim_stop { sim_running, sim_polling, sim_exited, sim_stopped, sim_signalled };

void sim_stop_reason PARAMS ((SIM_DESC sd, enum sim_stop *reason, int *sigrc));


/* Passthru for other commands that the simulator might support.
   Simulators should be prepared to deal with any combination of NULL
   or empty CMD. */

void sim_do_command PARAMS ((SIM_DESC sd, char *cmd));


/* Provide simulator with a default (global) host_callback_struct.
   THIS PROCEDURE IS IS DEPRECIATED.
   GDB and NRUN do not use this interface.
   This procedure does not take a SIM_DESC argument as it is
   used before sim_open. */

void sim_set_callbacks PARAMS ((struct host_callback_struct *));


/* Set the size of the simulator memory array.
   THIS PROCEDURE IS IS DEPRECIATED.
   GDB and NRUN do not use this interface.
   This procedure does not take a SIM_DESC argument as it is
   used before sim_open. */

void sim_size PARAMS ((int i));


/* Run a simulation with tracing enabled.
   THIS PROCEDURE IS IS DEPRECIATED.
   GDB and NRUN do not use this interface.
   This procedure does not take a SIM_DESC argument as it is
   used before sim_open. */

int sim_trace PARAMS ((SIM_DESC sd));


/* Configure the size of the profile buffer.
   THIS PROCEDURE IS IS DEPRECIATED.
   GDB and NRUN do not use this interface.
   This procedure does not take a SIM_DESC argument as it is
   used before sim_open. */

void sim_set_profile_size PARAMS ((int n));


/* Kill the running program.
   THIS PROCEDURE IS IS DEPRECIATED.
   GDB and NRUN do not use this interface.
   This procedure will be replaced as part of the introduction of
   multi-cpu simulators. */

void sim_kill PARAMS ((SIM_DESC sd));


#endif /* !defined (REMOTE_SIM_H) */
