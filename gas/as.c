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

#include "ansidecl.h"
#include "libiberty.h"

#define COMMON

#include "as.h"
#include "subsegs.h"
#include "output-file.h"

#include <signal.h>

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

int listing;			/* true if a listing is wanted */

char *myname;			/* argv[0] */
#ifdef BFD_ASSEMBLER
segT reg_section, expr_section;
segT text_section, data_section, bss_section;
#endif


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

void
show_usage (stream)
     FILE *stream;
{
  fprintf (stream, "Usage: %s [option...] [asmfile...]\n", myname);

  fprintf (stream, "\
Options:\n\
-a[sub-option...]	turn on listings\n\
  Sub-options [default hls]:\n\
  d	omit debugging directives\n\
  h	include high-level source\n\
  l	include assembly\n\
  n	omit forms processing\n\
  s	include symbols\n");
  fprintf (stream, "\
-D			produce assembler debugging messages\n\
-f			skip whitespace and comment preprocessing\n\
--help			show this message and exit\n\
-I DIR			add DIR to search list for .include directives\n\
-J			don't warn about signed overflow\n\
-K			warn when differences altered for long displacements\n\
-L			keep local symbols (starting with `L')\n");
  fprintf (stream, "\
-nocpp			ignored\n\
-o OBJFILE		name the object-file output OBJFILE (default a.out)\n\
-R			fold data section into text section\n\
--statistics		print maximum bytes and total seconds used\n\
--version		print assembler version number and exit\n\
-W			suppress warnings\n\
-w			ignored\n\
-X			ignored\n\
-Z			generate object file even after errors\n");

  md_show_usage (stream);
}

/*
 * Since it is easy to do here we interpret the special arg "-"
 * to mean "use stdin" and we set that argv[] pointing to "".
 * After we have munged argv[], the only things left are source file
 * name(s) and ""(s) denoting stdin. These file names are used
 * (perhaps more than once) later.
 *
 * check for new machine-dep cmdline options in
 * md_parse_option definitions in config/tc-*.c
 */

void
parse_args (pargc, pargv)
     int *pargc;
     char ***pargv;
{
  int old_argc, new_argc;
  char **old_argv, **new_argv;

  /* Starting the short option string with '-' is for programs that
     expect options and other ARGV-elements in any order and that care about
     the ordering of the two.  We describe each non-option ARGV-element
     as if it were the argument of an option with character code 1.  */

  char *shortopts;
  extern CONST char *md_shortopts;
#ifdef VMS
  /* -v takes an argument on VMS, so we don't make it a generic option.  */
  CONST char *std_shortopts = "-JKLRWZfa::DI:o:wX";
#else
  CONST char *std_shortopts = "-JKLRWZfa::DI:o:vwX";
#endif

  struct option *longopts;
  extern struct option md_longopts[];
  extern size_t md_longopts_size;
  static const struct option std_longopts[] = {
#define OPTION_HELP (OPTION_STD_BASE)
    {"help", no_argument, NULL, OPTION_HELP},
#define OPTION_NOCPP (OPTION_STD_BASE + 1)
    {"nocpp", no_argument, NULL, OPTION_NOCPP},
#define OPTION_STATISTICS (OPTION_STD_BASE + 2)
    {"statistics", no_argument, NULL, OPTION_STATISTICS},
#define OPTION_VERSION (OPTION_STD_BASE + 3)
    {"version", no_argument, NULL, OPTION_VERSION},
#define OPTION_DUMPCONFIG (OPTION_STD_BASE + 4)
    {"dump-config", no_argument, NULL, OPTION_DUMPCONFIG},
#define OPTION_VERBOSE (OPTION_STD_BASE + 5)
    {"verbose", no_argument, NULL, OPTION_VERBOSE},
  };

  /* Construct the option lists from the standard list and the
     target dependent list.  */
  shortopts = concat (std_shortopts, md_shortopts, (char *) NULL);
  longopts = (struct option *) xmalloc (sizeof (std_longopts) + md_longopts_size);
  memcpy (longopts, std_longopts, sizeof (std_longopts));
  memcpy ((char *) longopts + sizeof (std_longopts),
	  md_longopts, md_longopts_size);

  /* Make a local copy of the old argv.  */
  old_argc = *pargc;
  old_argv = *pargv;

  /* Initialize a new argv that contains no options.  */
  new_argv = (char **) xmalloc (sizeof (char *) * (old_argc + 1));
  new_argv[0] = old_argv[0];
  new_argc = 1;
  new_argv[new_argc] = NULL;

  while (1)
    {
      /* getopt_long_only is like getopt_long, but '-' as well as '--' can
	 indicate a long option.  */
      int longind;
      int optc = getopt_long_only (old_argc, old_argv, shortopts, longopts,
				   &longind);

      if (optc == -1)
	break;

      switch (optc)
	{
	default:
	  /* md_parse_option should return 1 if it recognizes optc,
	     0 if not.  */
	  if (md_parse_option (optc, optarg) != 0)
	    break;
	  /* `-v' isn't included in the general short_opts list, so check for
	     it explicity here before deciding we've gotten a bad argument.  */
	  if (optc == 'v')
	    {
#ifdef VMS
	      /* Telling getopt to treat -v's value as optional can result
		 in it picking up a following filename argument here.  The
		 VMS code in md_parse_option can return 0 in that case,
		 but it has no way of pushing the filename argument back.  */
	      if (optarg && *optarg)
		new_argv[new_argc++] = optarg,  new_argv[new_argc] = NULL;
	      else
#else
	      case 'v':
#endif
	      case OPTION_VERBOSE:
		print_version_id ();
	      break;
	    }
	  /*FALLTHRU*/

	case '?':
	  exit (EXIT_FAILURE);

	case 1:			/* File name.  */
	  if (!strcmp (optarg, "-"))
	    optarg = "";
	  new_argv[new_argc++] = optarg;
	  new_argv[new_argc] = NULL;
	  break;

	case OPTION_HELP:
	  show_usage (stdout);
	  exit (EXIT_SUCCESS);

	case OPTION_NOCPP:
	  break;

	case OPTION_STATISTICS:
	  flag_print_statistics = 1;
	  break;

	case OPTION_VERSION:
	  print_version_id ();
	  exit (EXIT_SUCCESS);

	case OPTION_DUMPCONFIG:
	  fprintf (stderr, "alias = %s\n", TARGET_ALIAS);
	  fprintf (stderr, "canonical = %s\n", TARGET_CANONICAL);
	  fprintf (stderr, "cpu-type = %s\n", TARGET_CPU);
#ifdef TARGET_OBJ_FORMAT
	  fprintf (stderr, "format = %s\n", TARGET_OBJ_FORMAT);
#endif
#ifdef TARGET_FORMAT
	  fprintf (stderr, "bfd-target = %s\n", TARGET_FORMAT);
#endif
	  exit (EXIT_SUCCESS);

	case 'J':
	  flag_signed_overflow_ok = 1;
	  break;

	case 'K':
	  flag_warn_displacement = 1;
	  break;

	case 'L':
	  flag_keep_locals = 1;
	  break;

	case 'R':
	  flag_readonly_data_in_text = 1;
	  break;

	case 'W':
	  flag_no_warnings = 1;
	  break;

	case 'Z':
	  flag_always_generate_output = 1;
	  break;

	case 'a':
	  if (optarg)
	    {
	      while (*optarg)
		{
		  switch (*optarg)
		    {
		    case 'd':
		      listing |= LISTING_NODEBUG;
		      break;
		    case 'h':
		      listing |= LISTING_HLL;
		      break;
		    case 'l':
		      listing |= LISTING_LISTING;
		      break;
		    case 'n':
		      listing |= LISTING_NOFORM;
		      break;
		    case 's':
		      listing |= LISTING_SYMBOLS;
		      break;
		    default:
		      as_fatal ("invalid listing option `%c'", *optarg);
		      break;
		    }
		  optarg++;
		}
	    }
	  if (!listing)
	    listing = LISTING_DEFAULT;
	  break;

	case 'D':
	  /* DEBUG is implemented: it debugs different */
	  /* things from other people's assemblers. */
	  flag_debug = 1;
	  break;

	case 'f':
	  flag_no_comments = 1;
	  break;

	case 'I':
	  {			/* Include file directory */
	    char *temp = strdup (optarg);
	    if (!temp)
	      as_fatal ("virtual memory exhausted");
	    add_include_dir (temp);
	    break;
	  }

	case 'o':
	  out_file_name = strdup (optarg);
	  if (!out_file_name)
	    as_fatal ("virtual memory exhausted");
	  break;

	case 'w':
	  break;

	case 'X':
	  /* -X means treat warnings as errors */
	  break;
	}
    }

  free (shortopts);
  free (longopts);

  *pargc = new_argc;
  *pargv = new_argv;
}

int 
main (argc, argv)
     int argc;
     char **argv;
{
  int keep_it;
  long start_time = get_run_time ();

#ifdef HOST_SPECIAL_INIT
  HOST_SPECIAL_INIT (argc, argv);
#endif

#if 0 /* do we need any of this?? */
  {
    static const int sig[] = {SIGHUP, SIGINT, SIGPIPE, SIGTERM, 0};
    int a;

    for (a = 0; sig[a] != 0; a++)
      if (signal (sig[a], SIG_IGN) != SIG_IGN)
	signal (sig[a], got_sig);
  }
#endif

  myname = argv[0];
  xmalloc_set_program_name (myname);

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
  parse_args (&argc, &argv);

#ifdef BFD_ASSEMBLER
  output_file_create (out_file_name);
  assert (stdoutput != 0);
#endif

#ifdef tc_init_after_args
  tc_init_after_args ();
#endif

  perform_an_assembly_pass (argc, argv);	/* Assemble it. */
#ifdef TC_I960
  brtab_emit ();
#endif
/* start-sanitize-rce */
#ifdef TC_RCE
  dump_literals(0);
#endif
/* end-sanitize-rce */

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

#ifndef OBJ_VMS /* does its own file handling */
#ifndef BFD_ASSEMBLER
  if (keep_it)
#endif
    output_file_close (out_file_name);
#endif

  if (!keep_it)
    unlink (out_file_name);

  input_scrub_end ();
#ifdef md_end
  md_end ();
#endif

  if (flag_print_statistics)
    {
      extern char **environ;
      char *lim = (char *) sbrk (0);
      long run_time = get_run_time () - start_time;

      fprintf (stderr, "%s: total time in assembly: %ld.%06ld\n",
	       myname, run_time / 1000000, run_time % 1000000);
      fprintf (stderr, "%s: data size %ld\n",
	       myname, (long) (lim - (char *) &environ));
    }

  /* Use exit instead of return, because under VMS environments they
     may not place the same interpretation on the value given.  */
  if ((had_warnings () && flag_always_generate_output)
      || had_errors () > 0)
    exit (EXIT_FAILURE);
  exit (EXIT_SUCCESS);
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

#ifdef TE_APOLLO
    seg = subseg_new (".wtext", 0);
#else
    seg = subseg_new (".text", 0);
#endif
    assert (seg == SEG_E0);
    seg = subseg_new (".data", 0);
    assert (seg == SEG_E1);
    seg = subseg_new (".bss", 0);
    assert (seg == SEG_E2);
#ifdef TE_APOLLO
    create_target_segments ();
#endif
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
