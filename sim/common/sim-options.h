/* Header file for simulator argument handling.
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

#ifndef SIM_OPTIONS_H
#define SIM_OPTIONS_H

#include "getopt.h"

/* ARGV option support.

   Options for the standalone simulator are parsed by sim_open since
   sim_open handles the large majority of them and it also parses the
   options when invoked by gdb [or any external program].  */

typedef SIM_RC (OPTION_HANDLER) PARAMS ((SIM_DESC, int, char *));

/* Declare option handlers with a macro so it's usable on k&r systems.  */
#define DECLARE_OPTION_HANDLER(fn) SIM_RC fn PARAMS ((SIM_DESC, int, char *))

typedef struct {
  /* The long option information.  */
  struct option opt;
  /* The short option with the same meaning ('\0' if none).  */
  char shortopt;
  /* The name of the argument (NULL if none).  */
  const char *arg;
  /* The documentation string.  If this is NULL, this is a synonym for
     the previous option.  */
  const char *doc;
  /* A function to process the option.  */
  OPTION_HANDLER *handler;
} OPTION;

/* All options that don't have a short form equivalent begin with this for
   `val'.  130 isn't special, just some non-ASCII value to begin at.
   Modules needn't worry about collisions here, the code dynamically assigned
   the actual numbers used and then passes the original value to the option
   handler.  */
#define OPTION_START 130

/* Non-zero if an option whose `val' entry is O is using OPTION_START.  */
#define OPTION_LONG_ONLY_P(o) ((unsigned char) (o) >= OPTION_START)

/* List of options added by various modules.  */
typedef struct option_list {
  struct option_list *next;
  const OPTION *options;
} OPTION_LIST;

/* Add a set of options to the simulator.
   TABLE is an array of OPTIONS terminated by a NULL `opt.name' entry.  */
SIM_RC sim_add_option_table PARAMS ((SIM_DESC sd, const OPTION *table));

/* Initialize common parts before argument processing.
   Called by sim_open.  */
SIM_RC sim_pre_argv_init PARAMS ((SIM_DESC sd, const char *myname));

/* Called by sim_open to parse the arguments.  */
SIM_RC sim_parse_args PARAMS ((SIM_DESC sd, char **argv));

/* Print help messages for the options.  */
void sim_print_help PARAMS ((SIM_DESC sd));

#endif /* SIM_OPTIONS_H */
