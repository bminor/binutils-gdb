/* as.c - GAS main program.
   Copyright (C) 1987, 1990, 1991, 1992, 1994 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

/*
 * Main program for AS; a 32-bit assembler of GNU.
 * Understands command arguments.
 * Has a few routines that don't fit in other modules because they
 * are shared.
 *
 *
 *			bugs
 *
 * : initialisers
 *	Since no-one else says they will support them in future: I
 * don't support them now.
 *
 */

#include <stdio.h>
#include <string.h>

#ifdef _POSIX_SOURCE
#include <sys/types.h>		/* For pid_t in signal.h */
#endif
#include <signal.h>

#define COMMON

#include "as.h"
#include "subsegs.h"
#include "output-file.h"

#ifndef SIGTY
#ifdef __STDC__
#define SIGTY void
#else
#define SIGTY int
#endif /* __STDC__ */
#endif /* SIGTY */

#if 0
/* Not currently used.  */
static SIGTY got_sig PARAMS ((int sig));
#endif
static void perform_an_assembly_pass PARAMS ((int argc, char **argv));

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

int listing;			/* true if a listing is wanted */

char *myname;			/* argv[0] */
#ifdef BFD_ASSEMBLER
segT reg_section, expr_section;
segT text_section, data_section, bss_section;
#endif

/* This is true if the assembler should not produce any timing info. */

static int quiet_flag = 1;


void
print_version_id ()
{
  static int printed;
  if (printed)
    return;
  printed = 1;

  fprintf (stderr, "GNU assembler version %s (%s)", GAS_VERSION, TARGET_ALIAS);
#ifdef BFD_ASSEMBLER
  fprintf (stderr, ", using BFD version %s", BFD_VERSION);
#endif
  fprintf (stderr, "\n");
}

int 
main (argc, argv)
     int argc;
     char **argv;
{
  int work_argc;		/* variable copy of argc */
  char **work_argv;		/* variable copy of argv */
  char *arg;			/* an arg to program */
  char a;			/* an arg flag (after -) */
  int keep_it;
  long start_time = get_run_time ();

#if 0 /* do we need any of this?? */
  {
    static const int sig[] = {SIGHUP, SIGINT, SIGPIPE, SIGTERM, 0};

    for (a = 0; sig[a] != 0; a++)
      if (signal (sig[a], SIG_IGN) != SIG_IGN)
	signal (sig[a], got_sig);
  }
#endif

  myname = argv[0];
  memset (flagseen, '\0', sizeof (flagseen));	/* aint seen nothing yet */
#ifndef OBJ_DEFAULT_OUTPUT_FILE_NAME
#define OBJ_DEFAULT_OUTPUT_FILE_NAME "a.out"
#endif
  out_file_name = OBJ_DEFAULT_OUTPUT_FILE_NAME;

#ifdef BFD_ASSEMBLER
  bfd_init ();
#endif

  symbol_begin ();
  subsegs_begin ();
  read_begin ();
  input_scrub_begin ();
  frag_init ();
  /*
   * Parse arguments, but we are only interested in flags.
   * When we find a flag, we process it then make it's argv[] NULL.
   * This helps any future argv[] scanners avoid what we processed.
   * Since it is easy to do here we interpret the special arg "-"
   * to mean "use stdin" and we set that argv[] pointing to "".
   * After we have munged argv[], the only things left are source file
   * name(s) and ""(s) denoting stdin. These file names are used
   * (perhaps more than once) later.
   */
  /* FIXME-SOMEDAY this should use getopt. */
  work_argc = argc - 1;		/* don't count argv[0] */
  work_argv = argv + 1;		/* skip argv[0] */
  for (; work_argc--; work_argv++)
    {
      arg = *work_argv;		/* work_argv points to this argument */

      if (*arg != '-')		/* Filename. We need it later. */
	continue;		/* Keep scanning args looking for flags. */
      if (arg[1] == '-' && arg[2] == 0)
	{
	  /* "--" as an argument means read STDIN */
	  /* on this scan, we don't want to think about filenames */
	  *work_argv = "";	/* Code that means 'use stdin'. */
	  continue;
	}
      /* This better be a switch. */
      arg++;			/*->letter. */

      while ((a = *arg) != '\0')
	{			/* scan all the 1-char flags */
	  arg++;		/* arg->after letter. */
	  a &= 0x7F;		/* ascii only please */
	  flagseen[(unsigned char) a] = 1;
	  switch (a)
	    {
	    case 'a':
	      {
		int loop = 1;

		while (loop)
		  {
		    switch (*arg)
		      {
		      case 'l':
			listing |= LISTING_LISTING;
			arg++;
			break;
		      case 's':
			listing |= LISTING_SYMBOLS;
			arg++;
			break;
		      case 'h':
			listing |= LISTING_HLL;
			arg++;
			break;

		      case 'n':
			listing |= LISTING_NOFORM;
			arg++;
			break;
		      case 'd':
			listing |= LISTING_NODEBUG;
			arg++;
			break;
		      default:
			if (!listing)
			  listing = LISTING_DEFAULT;
			loop = 0;
			break;
		      }
		  }
	      }

	      break;


	    case 'f':
	      break;		/* -f means fast - no need for "app" preprocessor. */

	    case 'D':
	      /* DEBUG is implemented: it debugs different */
	      /* things to other people's assemblers. */
	      break;

	    case 'I':
	      {			/* Include file directory */

		char *temp = NULL;
		if (*arg)
		  {
		    temp = strdup (arg);
		    if (!temp)
		      as_fatal ("virtual memory exhausted");
		  }
		else if (work_argc)
		  {
		    *work_argv = NULL;
		    work_argc--;
		    temp = *++work_argv;
		  }
		else
		  as_warn ("%s: I expected a filename after -I", myname);
		add_include_dir (temp);
		arg = "";	/* Finished with this arg. */
		break;
	      }

#ifdef WARN_SIGNED_OVERFLOW_WORD
	      /* Don't warn about signed overflow.  */
	    case 'J':
	      break;
#endif

#ifndef WORKING_DOT_WORD
	    case 'K':
	      break;
#endif

	    case 'L':		/* -L means keep L* symbols */
	      break;

	    case 'o':
	      if (*arg)		/* Rest of argument is object file-name. */
		{
		  out_file_name = strdup (arg);
		  if (!out_file_name)
		    as_fatal ("virtual memory exhausted");
		}
	      else if (work_argc)
		{		/* Want next arg for a file-name. */
		  *work_argv = NULL;	/* This is not a file-name. */
		  work_argc--;
		  out_file_name = *++work_argv;
		}
	      else
		as_warn ("%s: I expected a filename after -o. \"%s\" assumed.",
			 myname, out_file_name);
	      arg = "";		/* Finished with this arg. */
	      break;

	    case 'n':
	      if (*arg && strcmp(arg, "oquiet") == 0)
		quiet_flag = 0;
	      else if (*arg && strcmp(arg, "ocpp") == 0)
		;
	      else
		{
		  as_warn ("Unknown option `-n%s' ignored", arg);
		  arg += strlen (arg);
		  break;
		}

	    case 'R':
	      /* -R means put data into text segment */
	      flag_readonly_data_in_text = 1;
	      break;

	    case 'v':
#ifdef	VMS
	      {
		extern char *compiler_version_string;
		compiler_version_string = arg;
	      }
#else /* not VMS */
	      if (*arg && strcmp (arg, "ersion"))
		{
		  as_warn ("Unknown option `-v%s' ignored", arg);
		  arg += strlen (arg);
		  break;
		}

	      print_version_id ();
#endif /* not VMS */
	      while (*arg)
		arg++;		/* Skip the rest */
	      break;

	    case 'W':
	      /* -W means don't warn about things */
	      flag_suppress_warnings = 1;
	      break;

	    case 'w':
	    case 'X':
	      /* -X means treat warnings as errors */
	      break;
	    case 'Z':
	      /* -Z means attempt to generate object file even after errors. */
	      flag_always_generate_output = 1;
	      break;

	    default:
	      --arg;
	      if (md_parse_option (&arg, &work_argc, &work_argv) == 0)
		as_warn ("%s: I don't understand '%c' flag.", myname, a);
	      if (arg && *arg)
		arg++;
	      break;
	    }
	}
      /*
       * We have just processed a "-..." arg, which was not a
       * file-name. Smash it so the
       * things that look for filenames won't ever see it.
       *
       * Whatever work_argv points to, it has already been used
       * as part of a flag, so DON'T re-use it as a filename.
       */
      *work_argv = NULL;	/* NULL means 'not a file-name' */
    }

#ifdef BFD_ASSEMBLER
  output_file_create (out_file_name);
  assert (stdoutput != 0);
#endif

  /* Here with flags set up in flagseen[]. */
  perform_an_assembly_pass (argc, argv);	/* Assemble it. */
#ifdef TC_I960
  brtab_emit ();
#endif

  if (seen_at_least_1_file ()
      && !((had_warnings () && flag_always_generate_output)
	   || had_errors () > 0))
    keep_it = 1;
  else
    keep_it = 0;

  if (keep_it)
    write_object_file ();

#ifndef NO_LISTING
  listing_print ("");
#endif

#ifndef BFD_ASSEMBLER
  if (keep_it)
#endif
    output_file_close (out_file_name);

  if (!keep_it)
    unlink (out_file_name);

  input_scrub_end ();
#ifdef md_end
  md_end ();
#endif

  if (!quiet_flag)
    {
      extern char **environ;
      char *lim = (char *) sbrk (0);
      long run_time = get_run_time () - start_time;

      fprintf (stderr, "%s: total time in assembly: %d.%06d\n",
	       myname, run_time / 1000000, run_time % 1000000);
      fprintf (stderr, "%s: data size %ld\n",
	       myname, (long) (lim - (char *) &environ));
    }

  if ((had_warnings () && flagseen['Z'])
      || had_errors () > 0)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}


/*			perform_an_assembly_pass()
 *
 * Here to attempt 1 pass over each input file.
 * We scan argv[*] looking for filenames or exactly "" which is
 * shorthand for stdin. Any argv that is NULL is not a file-name.
 * We set need_pass_2 TRUE if, after this, we still have unresolved
 * expressions of the form (unknown value)+-(unknown value).
 *
 * Note the un*x semantics: there is only 1 logical input file, but it
 * may be a catenation of many 'physical' input files.
 */
static void 
perform_an_assembly_pass (argc, argv)
     int argc;
     char **argv;
{
  int saw_a_file = 0;
#ifdef BFD_ASSEMBLER
  flagword applicable;
#endif

  need_pass_2 = 0;

#ifndef BFD_ASSEMBLER
#ifdef MANY_SEGMENTS
  {
    unsigned int i;
    for (i = SEG_E0; i < SEG_UNKNOWN; i++)
      segment_info[i].fix_root = 0;
  }
  /* Create the three fixed ones */
  {
    segT seg;

    seg = subseg_new (".text", 0);
    assert (seg == SEG_E0);
    seg = subseg_new (".data", 0);
    assert (seg == SEG_E1);
    seg = subseg_new (".bss", 0);
    assert (seg == SEG_E2);
  }

#else /* not MANY_SEGMENTS */
  text_fix_root = NULL;
  data_fix_root = NULL;
  bss_fix_root = NULL;
#endif /* not MANY_SEGMENTS */
#else /* BFD_ASSEMBLER */
  /* Create the standard sections, and those the assembler uses
     internally.  */
  text_section = subseg_new (".text", 0);
  data_section = subseg_new (".data", 0);
  bss_section = subseg_new (".bss", 0);
  /* @@ FIXME -- we're setting the RELOC flag so that sections are assumed
     to have relocs, otherwise we don't find out in time. */
  applicable = bfd_applicable_section_flags (stdoutput);
  bfd_set_section_flags (stdoutput, text_section,
			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC
				       | SEC_CODE | SEC_READONLY));
  /* @@ FIXME -- SEC_CODE seems to mean code only, rather than code possibly.*/
  bfd_set_section_flags (stdoutput, data_section,
			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC));
  bfd_set_section_flags (stdoutput, bss_section, applicable & SEC_ALLOC);
  seg_info (bss_section)->bss = 1;
  subseg_new (BFD_ABS_SECTION_NAME, 0);
  subseg_new (BFD_UND_SECTION_NAME, 0);
  reg_section = subseg_new ("*GAS `reg' section*", 0);
  expr_section = subseg_new ("*GAS `expr' section*", 0);

#endif /* BFD_ASSEMBLER */

  subseg_set (text_section, 0);

  /* This may add symbol table entries, which requires having an open BFD,
     and sections already created, in BFD_ASSEMBLER mode.  */
  md_begin ();

  argv++;			/* skip argv[0] */
  argc--;			/* skip argv[0] */
  while (argc--)
    {
      if (*argv)
	{			/* Is it a file-name argument? */
	  saw_a_file++;
	  /* argv->"" if stdin desired, else->filename */
	  read_a_source_file (*argv);
	}
      argv++;			/* completed that argv */
    }
  if (!saw_a_file)
    read_a_source_file ("");
}				/* perform_an_assembly_pass() */

#if 0
/* This is not currently used.  */
static SIGTY
got_sig (sig)
     int sig;
{
  static here_before = 0;

  as_bad ("Interrupted by signal %d", sig);
  if (here_before++)
    exit (EXIT_FAILURE);
#if 0 /* If SIGTY is void, this produces warnings.  */
  return ((SIGTY) 0);
#endif
}
#endif

/* end of as.c */
