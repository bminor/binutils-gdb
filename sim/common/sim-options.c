/* Simulator option handling.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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
#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif
#include "libiberty.h"
#include "../libiberty/alloca-conf.h"
#include "sim-options.h"
#include "sim-io.h"

/* This is defined in sim-config.h.  */
#ifndef MAX_NR_PROCESSORS
#define MAX_NR_PROCESSORS 1
#endif

/* Add a set of options to the simulator.
   TABLE is an array of OPTIONS terminated by a NULL `opt.name' entry.
   This is intended to be called by modules in their `install' handler.  */

SIM_RC
sim_add_option_table (sd, table)
     SIM_DESC sd;
     const OPTION *table;
{
  struct option_list *ol = ((struct option_list *)
			    xmalloc (sizeof (struct option_list)));

  /* Note: The list is constructed in the reverse order we're called so
     later calls will override earlier ones (in case that ever happens).
     This is the intended behaviour.  */
  ol->next = STATE_OPTIONS (sd);
  ol->options = table;
  STATE_OPTIONS (sd) = ol;

  return SIM_RC_OK;
}

/* Standard option table.
   Modules may specify additional ones.
   The caller of sim_parse_args may also specify additional options
   by calling sim_add_option_table first.  */

static DECLARE_OPTION_HANDLER (standard_option_handler);

/* FIXME: We shouldn't print in --help output options that aren't usable.
   Some fine tuning will be necessary.  One can either move less general
   options to another table or use a HAVE_FOO macro to ifdef out unavailable
   options.  */

#define OPTION_DEBUG_INSN	(OPTION_START + 0)
#define OPTION_DEBUG_FILE	(OPTION_START + 1)
#define OPTION_TRACE_INSN	(OPTION_START + 2)
#define OPTION_TRACE_DECODE	(OPTION_START + 3)
#define OPTION_TRACE_EXTRACT	(OPTION_START + 4)
#define OPTION_TRACE_LINENUM	(OPTION_START + 5)
#define OPTION_TRACE_MEMORY	(OPTION_START + 6)
#define OPTION_TRACE_MODEL	(OPTION_START + 7)
#define OPTION_TRACE_ALU	(OPTION_START + 8)
#define OPTION_TRACE_FILE	(OPTION_START + 9)
#define OPTION_PROFILE_INSN	(OPTION_START + 10)
#define OPTION_PROFILE_MEMORY	(OPTION_START + 11)
#define OPTION_PROFILE_MODEL	(OPTION_START + 12)
#define OPTION_PROFILE_SIMCACHE	(OPTION_START + 13)
#define OPTION_PROFILE_FILE	(OPTION_START + 14)

static const OPTION standard_options[] =
{
  { {"verbose", no_argument, NULL, 'v'},
      'v', NULL, "Verbose output",
      standard_option_handler },

  { {"debug", no_argument, NULL, 'D'},
      'D', NULL, "Print debugging messages",
      standard_option_handler },
  { {"debug-insn", no_argument, NULL, OPTION_DEBUG_INSN},
      '\0', NULL, "Print instruction debugging messages",
      standard_option_handler },
  { {"debug-file", required_argument, NULL, OPTION_DEBUG_FILE},
      '\0', "FILE NAME", "Specify debugging output file",
      standard_option_handler },

  { {"trace", no_argument, NULL, 't'},
      't', NULL, "Perform tracing",
      standard_option_handler },
  { {"trace-insn", no_argument, NULL, OPTION_TRACE_INSN},
      '\0', NULL, "Perform instruction tracing",
      standard_option_handler },
  { {"trace-decode", no_argument, NULL, OPTION_TRACE_DECODE},
      '\0', NULL, "Perform instruction decoding tracing",
      standard_option_handler },
  { {"trace-extract", no_argument, NULL, OPTION_TRACE_EXTRACT},
      '\0', NULL, "Perform instruction extraction tracing",
      standard_option_handler },
  { {"trace-linenum", no_argument, NULL, OPTION_TRACE_LINENUM},
      '\0', NULL, "Perform line number tracing",
      standard_option_handler },
  { {"trace-memory", no_argument, NULL, OPTION_TRACE_MEMORY},
      '\0', NULL, "Perform memory tracing",
      standard_option_handler },
  { {"trace-model", no_argument, NULL, OPTION_TRACE_MODEL},
      '\0', NULL, "Perform model tracing",
      standard_option_handler },
  { {"trace-alu", no_argument, NULL, OPTION_TRACE_ALU},
      '\0', NULL, "Perform ALU tracing",
      standard_option_handler },
  { {"trace-file", required_argument, NULL, OPTION_TRACE_FILE},
      '\0', "FILE NAME", "Specify tracing output file",
      standard_option_handler },

#ifdef SIM_H8300 /* FIXME: Should be movable to h8300 dir.  */
  { {"h8300h", no_argument, NULL, 'h'},
      'h', NULL, "Indicate the CPU is h8/300h or h8/300s",
      standard_option_handler },
#endif

#ifdef SIM_HAVE_SIMCACHE
  { {"simcache-size", required_argument, NULL, 'c'},
      'c', "SIM CACHE SIZE", "Specify size of simulator instruction cache",
      standard_option_handler },
#endif

#ifdef SIM_HAVE_FLATMEM
  { {"mem-size", required_argument, NULL, 'm'},
      'm', "MEMORY SIZE", "Specify memory size",
      standard_option_handler },
#endif

#ifdef SIM_HAVE_MAX_INSNS
  { {"max-insns", required_argument, NULL, 'M'},
      'M', "MAX INSNS", "Specify maximum instructions to execute",
      standard_option_handler },
#endif

#ifdef SIM_HAVE_PROFILE
  { {"profile", no_argument, NULL, 'p'},
      'p', NULL, "Perform profiling",
      standard_option_handler },
  { {"profile-insn", no_argument, NULL, OPTION_PROFILE_INSN},
      '\0', NULL, "Perform instruction profiling",
      standard_option_handler },
  { {"profile-memory", no_argument, NULL, OPTION_PROFILE_MEMORY},
      '\0', NULL, "Perform memory profiling",
      standard_option_handler },
  { {"profile-model", no_argument, NULL, OPTION_PROFILE_MODEL},
      '\0', NULL, "Perform model profiling",
      standard_option_handler },
  { {"profile-simcache", no_argument, NULL, OPTION_PROFILE_SIMCACHE},
      '\0', NULL, "Perform simulator execution cache profiling",
      standard_option_handler },
  { {"profile-file", required_argument, NULL, OPTION_PROFILE_FILE},
      '\0', "FILE NAME", "Specify profile output file",
      standard_option_handler },
#ifdef SIM_HAVE_PROFILE_PC
  { {"profile-pc-frequency", required_argument, NULL, 'F'},
      'F', "PC PROFILE FREQUENCY", "Turn on PC profiling at specified frequency",
      standard_option_handler },
  { {"profile-pc-size", required_argument, NULL, 'S'},
      'S', "PC PROFILE SIZE", "Specify PC profiling size",
      standard_option_handler },
#endif
#endif /* SIM_HAVE_PROFILE */

  { {"help", no_argument, NULL, 'H'},
      'H', NULL, "Print help information",
      standard_option_handler },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

static SIM_RC
standard_option_handler (sd, opt, arg)
     SIM_DESC sd;
     int opt;
     char *arg;
{
  int i,n;
  unsigned long ul;

  switch (opt)
    {
    case 'v' :
      STATE_VERBOSE_P (sd) = 1;
      break;

    case 'D' :
      if (! WITH_DEBUG)
	sim_io_eprintf (sd, "Debugging not compiled in, `-D' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    for (i = 0; i < MAX_DEBUG_VALUES; ++i)
	      CPU_DEBUG_FLAGS (STATE_CPU (sd, n))[i] = 1;
	}
      break;

    case OPTION_DEBUG_INSN :
      if (! WITH_DEBUG)
	sim_io_eprintf (sd, "Debugging not compiled in, `--debug-insn' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_DEBUG_FLAGS (STATE_CPU (sd, n))[DEBUG_INSN_IDX] = 1;
	}
      break;

    case OPTION_DEBUG_FILE :
      if (! WITH_DEBUG)
	sim_io_eprintf (sd, "Debugging not compiled in, `--debug-file' ignored\n");
      else
	{
	  FILE *f = fopen (arg, "w");

	  if (f == NULL)
	    {
	      sim_io_eprintf (sd, "Unable to open debug output file `%s'\n", arg);
	      return SIM_RC_FAIL;
	    }
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_DEBUG_FILE (STATE_CPU (sd, n)) = f;
	}
      break;

    case 't' :
      if (! WITH_TRACE)
	sim_io_eprintf (sd, "Tracing not compiled in, `-t' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    for (i = 0; i < MAX_TRACE_VALUES; ++i)
	      CPU_TRACE_FLAGS (STATE_CPU (sd, n))[i] = 1;
	}
      break;

    case OPTION_TRACE_INSN :
      if (! (WITH_TRACE & (1 << TRACE_INSN_IDX)))
	sim_io_eprintf (sd, "Instruction tracing not compiled in, `--trace-insn' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_INSN_IDX] = 1;
	}
      break;

    case OPTION_TRACE_DECODE :
      if (! (WITH_TRACE & (1 << TRACE_DECODE_IDX)))
	sim_io_eprintf (sd, "Decode tracing not compiled in, `--trace-decode' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_DECODE_IDX] = 1;
	}
      break;

    case OPTION_TRACE_EXTRACT :
      if (! (WITH_TRACE & (1 << TRACE_EXTRACT_IDX)))
	sim_io_eprintf (sd, "Extract tracing not compiled in, `--trace-extract' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_EXTRACT_IDX] = 1;
	}
      break;

    case OPTION_TRACE_LINENUM :
      if (! (WITH_TRACE & (1 << TRACE_LINENUM_IDX)))
	sim_io_eprintf (sd, "Line number tracing not compiled in, `--trace-linenum' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_LINENUM_IDX] = 1;
	}
      break;

    case OPTION_TRACE_MEMORY :
      if (! (WITH_TRACE & (1 << TRACE_MEMORY_IDX)))
	sim_io_eprintf (sd, "Memory tracing not compiled in, `--trace-memory' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_MEMORY_IDX] = 1;
	}
      break;

    case OPTION_TRACE_MODEL :
      if (! (WITH_TRACE & (1 << TRACE_MODEL_IDX)))
	sim_io_eprintf (sd, "Model tracing not compiled in, `--trace-model' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_MODEL_IDX] = 1;
	}
      break;

    case OPTION_TRACE_ALU :
      if (! (WITH_TRACE & (1 << TRACE_ALU_IDX)))
	sim_io_eprintf (sd, "ALU tracing not compiled in, `--trace-alu' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FLAGS (STATE_CPU (sd, n))[TRACE_ALU_IDX] = 1;
	}
      break;

    case OPTION_TRACE_FILE :
      if (! WITH_TRACE)
	sim_io_eprintf (sd, "Tracing not compiled in, `--trace-file' ignored\n");
      else
	{
	  FILE *f = fopen (arg, "w");

	  if (f == NULL)
	    {
	      sim_io_eprintf (sd, "Unable to open trace output file `%s'\n", arg);
	      return SIM_RC_FAIL;
	    }
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_TRACE_FILE (STATE_CPU (sd, n)) = f;
	}
      break;

#ifdef SIM_H8300 /* FIXME: Can be moved to h8300 dir.  */
    case 'h' :
      set_h8300h (1);
      break;
#endif

#ifdef SIM_HAVE_SIMCACHE
    case 'c':
      n = strtol (arg, NULL, 0);
      if (n <= 0)
	{
	  sim_io_eprintf (sd, "Invalid simulator cache size: %d", n);
	  return SIM_RC_FAIL;
	}
      STATE_SIMCACHE_SIZE (sd) = n;
      break;
#endif

#ifdef SIM_HAVE_FLATMEM
    case 'm':
      ul = strtol (arg, NULL, 0);
      /* 16384: some minimal amount */
      if (! isdigit (arg[0]) || ul < 16384)
	{
	  sim_io_eprintf (sd, "Invalid memory size `%s'", arg);
	  return SIM_RC_FAIL;
	}
      STATE_MEM_SIZE (sd) = ul;
      break;
#endif

#ifdef SIM_HAVE_MAX_INSNS
    case 'M' :
      ul = strtoul (arg, NULL, 0);
      if (! isdigit (arg[0]))
	{
	  sim_io_eprintf (sd, "Invalid maximum instruction count: `%s'", arg);
	  return SIM_RC_FAIL;
	}
      STATE_MAX_INSNS (sd) = ul;
      break;
#endif

#ifdef SIM_HAVE_PROFILE
    case 'p' :
      if (! WITH_PROFILE)
	sim_io_eprintf (sd, "Profile not compiled in, -p option ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    for (i = 0; i < MAX_PROFILE_VALUES; ++i)
	      CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[i] = 1;
	}
      break;

    case OPTION_PROFILE_INSN :
      if (! (WITH_PROFILE & (1 << PROFILE_INSN_IDX)))
	sim_io_eprintf (sd, "Instruction profiling not compiled in, `--profile-insn' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_INSN_IDX] = 1;
	}
      break;

    case OPTION_PROFILE_MEMORY :
      if (! (WITH_PROFILE & (1 << PROFILE_MEMORY_IDX)))
	sim_io_eprintf (sd, "Memory profiling not compiled in, `--profile-memory' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_MEMORY_IDX] = 1;
	}
      break;

    case OPTION_PROFILE_MODEL :
      if (! (WITH_PROFILE & (1 << PROFILE_MODEL_IDX)))
	sim_io_eprintf (sd, "Model profiling not compiled in, `--profile-model' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_MODEL_IDX] = 1;
	}
      break;

    case OPTION_PROFILE_SIMCACHE :
      if (! (WITH_PROFILE & (1 << PROFILE_SIMCACHE_IDX)))
	sim_io_eprintf (sd, "Simulator cache profiling not compiled in, `--profile-simcache' ignored\n");
      else
	{
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_PROFILE_FLAGS (STATE_CPU (sd, n))[PROFILE_SIMCACHE_IDX] = 1;
	}
      break;

    case OPTION_PROFILE_FILE :
      /* FIXME: Might want this to apply to pc profiling only,
	 or have two profile file options.  */
      if (! WITH_PROFILE)
	sim_io_eprintf (sd, "Profiling not compiled in, `--profile-file' ignored\n");
      else
	{
	  FILE *f = fopen (arg, "w");

	  if (f == NULL)
	    {
	      sim_io_eprintf (sd, "Unable to open profile output file `%s'\n", arg);
	      return SIM_RC_FAIL;
	    }
	  for (n = 0; n < MAX_NR_PROCESSORS; ++n)
	    CPU_PROFILE_FILE (STATE_CPU (sd, n)) = f;
	}
      break;

#ifdef SIM_HAVE_PROFILE_PC
    case 'F' :
      STATE_PROFILE_PC_FREQUENCY (sd) = atoi (arg);
      /* FIXME: Validate arg.  */
      break;
    case 'S' :
      STATE_PROFILE_PC_SIZE (sd) = atoi (arg);
      /* FIXME: Validate arg.  */
      break;
#endif
#endif /* SIM_HAVE_PROFILE */

    case 'H':
      sim_print_help (sd);
      if (STATE_OPEN_KIND (sd) == SIM_OPEN_STANDALONE)
	exit (0);
      /* FIXME: 'twould be nice to do something similar if gdb.  */
      break;
    }

  return SIM_RC_OK;
}

/* Initialize common parts before argument processing.
   Called by sim_open.  */

SIM_RC
sim_pre_argv_init (sd, myname)
     SIM_DESC sd;
     const char *myname;
{
  STATE_MY_NAME (sd) = myname + strlen (myname);
  while (STATE_MY_NAME (sd) > myname && STATE_MY_NAME (sd)[-1] != '/')
    --STATE_MY_NAME (sd);

  if (sim_add_option_table (sd, standard_options) != SIM_RC_OK)
    return SIM_RC_FAIL;

  return SIM_RC_OK;
}

/* Return non-zero if arg is a duplicate argument.
   If ARG is NULL, initialize.  */

#define ARG_HASH_SIZE 97
#define ARG_HASH(a) ((256 * (unsigned char) a[0] + (unsigned char) a[1]) % ARG_HASH_SIZE)

static int
dup_arg_p (arg)
     char *arg;
{
  int hash;
  static char **arg_table = NULL;

  if (arg == NULL)
    {
      if (arg_table == NULL)
	arg_table = (char **) xmalloc (ARG_HASH_SIZE * sizeof (char *));
      memset (arg_table, 0, ARG_HASH_SIZE * sizeof (char *));
      return 0;
    }

  hash = ARG_HASH (arg);
  while (arg_table[hash] != NULL)
    {
      if (strcmp (arg, arg_table[hash]) == 0)
	return 1;
      /* We assume there won't be more than ARG_HASH_SIZE arguments so we
	 don't check if the table is full.  */
      if (++hash == ARG_HASH_SIZE)
	hash = 0;
    }
  arg_table[hash] = arg;
  return 0;
}
     
/* Called by sim_open to parse the arguments.  */

SIM_RC
sim_parse_args (sd, argv)
     SIM_DESC sd;
     char **argv;
{
  int i, argc, num_opts;
  char *p, *short_options;
  /* The `val' option struct entry is dynamically assigned for options that
     only come in the long form.  ORIG_VAL is used to get the original value
     back.  */
  unsigned char *orig_val;
  struct option *lp, *long_options;
  const struct option_list *ol;
  const OPTION *opt;
  OPTION_HANDLER **handlers;

  /* Count the number of arguments.  */
  for (argc = 0; argv[argc] != NULL; ++argc)
    continue;

  /* Count the number of options.  */
  num_opts = 0;
  for (ol = STATE_OPTIONS (sd); ol != NULL; ol = ol->next)
    for (opt = ol->options; opt->opt.name != NULL; ++opt)
      ++num_opts;

  /* Initialize duplicate argument checker.  */
  (void) dup_arg_p (NULL);

  /* Build the option table for getopt.  */
  long_options = (struct option *) alloca ((num_opts + 1) * sizeof (struct option));
  lp = long_options;
  short_options = (char *) alloca (num_opts * 3 + 1);
  p = short_options;
#if 0 /* ??? necessary anymore? */
  /* Set '+' as first char so argument permutation isn't done.  This is done
     to workaround a problem with invoking getopt_long in run.c.: optind gets
     decremented when the program name is reached.  */
  *p++ = '+';
#endif
  handlers = (OPTION_HANDLER **) alloca (256 * sizeof (OPTION_HANDLER *));
  memset (handlers, 0, 256 * sizeof (OPTION_HANDLER *));
  orig_val = (unsigned char *) alloca (256);
  for (i = OPTION_START, ol = STATE_OPTIONS (sd); ol != NULL; ol = ol->next)
    for (opt = ol->options; opt->opt.name != NULL; ++opt)
      {
	if (dup_arg_p (opt->opt.name))
	  continue;
	if (opt->shortopt != 0)
	  {
	    *p++ = opt->shortopt;
	    if (opt->opt.has_arg == required_argument)
	      *p++ = ':';
	    else if (opt->opt.has_arg == optional_argument)
	      { *p++ = ':'; *p++ = ':'; }
	  }
	*lp = opt->opt;
	/* Dynamically assign `val' numbers for long options that don't have
	   a short option equivalent.  */
	if (OPTION_LONG_ONLY_P (opt->opt.val))
	  lp->val = i++;
	handlers[(unsigned char) lp->val] = opt->handler;
	orig_val[(unsigned char) lp->val] = opt->opt.val;
	++lp;
      }
  *p = 0;
  lp->name = NULL;

  /* Ensure getopt is initialized.  */
  optind = 0;
  while (1)
    {
      int longind, optc;

      optc = getopt_long (argc, argv, short_options, long_options, &longind);
      if (optc == -1)
	{
	  if (STATE_OPEN_KIND (sd) == SIM_OPEN_STANDALONE)
	    STATE_PROG_ARGV (sd) = argv + optind;
	  break;
	}
      if (optc == '?')
	return SIM_RC_FAIL;

      if ((*handlers[optc]) (sd, orig_val[optc], optarg) == SIM_RC_FAIL)
	return SIM_RC_FAIL;
    }

  return SIM_RC_OK;
}

/* Print help messages for the options.  */

void
sim_print_help (sd)
     SIM_DESC sd;
{
  const struct option_list *ol;
  const OPTION *opt;

  if (STATE_OPEN_KIND (sd) == SIM_OPEN_STANDALONE)
    sim_io_printf (sd, "Usage: %s [options] program [program args]\n",
		   STATE_MY_NAME (sd));

  /* Initialize duplicate argument checker.  */
  (void) dup_arg_p (NULL);

  sim_io_printf (sd, "Options:\n");
  for (ol = STATE_OPTIONS (sd); ol != NULL; ol = ol->next)
    for (opt = ol->options; opt->opt.name != NULL; ++opt)
      {
	int comma, len;
	const OPTION *o;

	if (dup_arg_p (opt->opt.name))
	  continue;

	if (opt->doc == NULL)
	  continue;

	sim_io_printf (sd, "  ");

	comma = 0;
	len = 2;

	o = opt;
	do
	  {
	    if (o->shortopt != '\0')
	      {
		sim_io_printf (sd, "%s-%c", comma ? ", " : "", o->shortopt);
		len += (comma ? 2 : 0) + 2;
		if (o->arg != NULL)
		  {
		    if (o->opt.has_arg != optional_argument)
		      {
			sim_io_printf (sd, " ");
			++len;
		      }
		    sim_io_printf (sd, "%s", o->arg);
		    len += strlen (o->arg);
		  }
		comma = 1;
	      }
	    ++o;
	  }
	while (o->opt.name != NULL && o->doc == NULL);

	o = opt;
	do
	  {
	    if (o->opt.name != NULL)
	      {
		sim_io_printf (sd, "%s--%s",
			       comma ? ", " : "",
			       o->opt.name);
		len += ((comma ? 2 : 0)
			+ 2
			+ strlen (o->opt.name));
		if (o->arg != NULL)
		  {
		    sim_io_printf (sd, " %s", o->arg);
		    len += 1 + strlen (o->arg);
		  }
		comma = 1;
	      }
	    ++o;
	  }
	while (o->opt.name != NULL && o->doc == NULL);

	if (len >= 30)
	  {
	    sim_io_printf (sd, "\n");
	    len = 0;
	  }

	for (; len < 30; len++)
	  sim_io_printf (sd, " ");

	sim_io_printf (sd, "%s\n", opt->doc);
      }

  if (STATE_OPEN_KIND (sd) == SIM_OPEN_STANDALONE)
    {
      sim_io_printf (sd, "\n");
      sim_io_printf (sd, "program args    Arguments to pass to simulated program.\n");
      sim_io_printf (sd, "                Note: Very few simulators support this.\n");
    }
}
