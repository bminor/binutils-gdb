/* Parse options for the GNU linker.
   Copyright (C) 1991, 92, 93, 94 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include <stdio.h>
#include <string.h>
#include "getopt.h"
#include "bfdlink.h"
#include "config.h"
#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldgram.h"
#include "ldlex.h"
#include "ldfile.h"

/* Omit args to avoid the possibility of clashing with a system header
   that might disagree about consts.  */
unsigned long strtoul ();

static void set_default_dirlist PARAMS ((char *dirlist_ptr));
static void set_section_start PARAMS ((char *sect, char *valstr));

void
parse_args (argc, argv)
     int argc;
     char **argv;
{
  /* Starting the short option string with '-' is for programs that
     expect options and other ARGV-elements in any order and that care about
     the ordering of the two.  We describe each non-option ARGV-element
     as if it were the argument of an option with character code 1.  */

  const char *shortopts = "-A:B::b:cde:F::G:giL:l:Mm:NnO:o:R:rSsT:tu:VvXxy:";

  static struct option longopts[] =
  {
    {"call_shared", no_argument, NULL, 158},
    {"dc", no_argument, NULL, 'd'},
    {"defsym", required_argument, NULL, 160},
    {"dn", no_argument, NULL, 158},
    {"dp", no_argument, NULL, 'd'},
    {"EB", no_argument, NULL, 150},
    {"EL", no_argument, NULL, 151},
    {"format", required_argument, NULL, 'b'},
    {"help", no_argument, NULL, 164},
    {"Map", required_argument, NULL, 152},
    {"no-keep-memory", no_argument, NULL, 168},
    {"noinhibit-exec", no_argument, NULL, 169},
    {"noinhibit_exec", no_argument, NULL, 169},
    {"non_shared", no_argument, NULL, 158},
    {"oformat", required_argument, NULL, 172},
    {"Qy", no_argument, NULL, 158},
    {"relax", no_argument, NULL, 173},
    {"retain-symbols-file", no_argument, NULL, 174},
    {"sort-common", no_argument, NULL, 175},
    {"sort_common", no_argument, NULL, 175},
    {"stats", no_argument, NULL, 176},
    {"Tbss", required_argument, NULL, 154},
    {"Tdata", required_argument, NULL, 155},
    {"Ttext", required_argument, NULL, 156},
    {"Ur", no_argument, NULL, 157},
    {"version", no_argument, NULL, 178},
    {"warn-common", no_argument, NULL, 177},
    {"YP,", required_argument, NULL, 158},
    {NULL, no_argument, NULL, 0}
  };

  while (1)
    {
      /* getopt_long_only is like getopt_long, but '-' as well as '--' can
	 indicate a long option.  */
      int optc = getopt_long_only (argc, argv, shortopts, longopts,
				   (int *) NULL);
      if (optc == -1)
	break;

      switch (optc)
	{
	default:
	  xexit (1);
	case 1:			/* File name.  */
	  lang_add_input_file (optarg, lang_input_file_is_file_enum,
			       (char *) NULL);
	  break;

	case 'A':
	  ldfile_add_arch (optarg);
	  break;
	case 'B':
	  /* Ignore.  */
	  break;
	case 'b':
	  lang_add_target (optarg);
	  break;
	case 'c':
	  ldfile_open_command_file (optarg);
	  parser_input = input_mri_script;
	  yyparse ();
	  break;
	case 158:		/* call_shared, dn, non_shared, Qy, YP */
	  set_default_dirlist (optarg);
	  break;
	case 'd':
	  command_line.force_common_definition = true;
	  break;
	case 160:		/* defsym */
	  lex_redirect (optarg);
	  parser_input = input_defsym;
	  yyparse ();
	  break;
	case 150:		/* EB */
	  /* FIXME: This is currently ignored.  It means
	     ``produce a big-endian object file''.  It could
	     be used to select an output format.  */
	  break;
	case 151:		/* EL */
	  /* FIXME: This is currently ignored.  It means
	     ``produce a little-endian object file''.  It could
	     be used to select an output format.  */
	  break;
	case 'e':
	  lang_add_entry (optarg);
	  break;
	case 'F':
	  /* Ignore.  */
	  break;
	case 'G':
	  {
	    char *end;
	    g_switch_value = strtoul (optarg, &end, 0);
	    if (*end)
	      einfo ("%P%F: invalid number `%s'", optarg);
	  }
	  break;
	case 'g':
	  /* Ignore.  */
	  break;
	case 164:		/* help */
	  help ();
	  xexit (0);
	  break;
	case 'L':
	  ldfile_add_library_path (optarg);
	  break;
	case 'l':
	  lang_add_input_file (optarg, lang_input_file_is_l_enum,
			       (char *) NULL);
	  break;
	case 'M':
	  config.map_filename = "-";
	  break;
	case 'm':
	  /* Ignore.  Was handled in a pre-parse.   */
	  break;
	case 152:		/* Map */
	  write_map = true;
	  config.map_filename = optarg;
	  break;
	case 'N':
	  config.text_read_only = false;
	  config.magic_demand_paged = false;
	  break;
	case 'n':
	  config.magic_demand_paged = false;
	  break;
	case 168:		/* no-keep-memory */
	  link_info.keep_memory = false;
	  break;
	case 169:		/* noinhibit-exec */
	  force_make_executable = true;
	  break;
	case 'O':
	  /* FIXME "-O<non-digits> <value>" used to set the address of
	     section <non-digits>.  Was this for compatibility with
	     something, or can we create a new option to do that
	     (with a syntax similar to -defsym)?
	     getopt can't handle two args to an option without kludges.  */
	  set_default_dirlist (optarg);
	  break;
	case 'o':
	  lang_add_output (optarg, 0); 
	  break;
	case 172:		/* oformat */
	  lang_add_output_format (optarg, 0);
	  break;
	case 'r':
	  link_info.relocateable = true;
	  config.build_constructors = false;
	  config.magic_demand_paged = false;
	  config.text_read_only = false;
	  break;
	case 'R':
	  lang_add_input_file (optarg,
			       lang_input_file_is_symbols_only_enum,
			       (char *) NULL);
	  break;
	case 173:		/* relax */
	  command_line.relax = true;
	  break;
	case 174:		/* retain-symbols-file */
	  add_keepsyms_file (optarg);
	  break;
	case 'S':
	  link_info.strip = strip_debugger;
	  break;
	case 's':
	  link_info.strip = strip_all;
	  break;
	case 175:		/* sort-common */
	  config.sort_common = true;
	  break;
	case 176:		/* stats */
	  config.stats = true;
	  break;
	case 't':
	  trace_files = true;
	  break;
	case 'T':
	  ldfile_open_command_file (optarg);
	  parser_input = input_script;
	  yyparse ();
	  break;
	case 154:		/* Tbss */
	  set_section_start (".bss", optarg);
	  break;
	case 155:		/* Tdata */
	  set_section_start (".data", optarg);
	  break;
	case 156:		/* Ttext */
	  set_section_start (".text", optarg);
	  break;
	case 157:		/* Ur */
	  link_info.relocateable = true;
	  config.build_constructors = true;
	  config.magic_demand_paged = false;
	  config.text_read_only = false;
	  break;
	case 'u':
	  ldlang_add_undef (optarg);
	  break;
	case 'V':
	  ldversion (1);
	  version_printed = true;
	  trace_file_tries = true;
	  break;
	case 'v':
	  ldversion (0);
	  version_printed = true;
	  break;
	case 178:		/* version */
	  ldversion (0);
	  version_printed = true;
	  break;
	case 177:		/* warn-common */
	  config.warn_common = true;
	  break;
	case 'X':
	  link_info.discard = discard_l;
	  break;
	case 'x':
	  link_info.discard = discard_all;
	  break;
	case 'y':
	  add_ysym (optarg);
	  break;
	}
    }
}

/* Add the (colon-separated) elements of DIRLIST_PTR to the
   library search path.  */

static void
set_default_dirlist (dirlist_ptr)
     char *dirlist_ptr;
{
  char *p;

  while (1)
    {
      p = strchr (dirlist_ptr, ':');
      if (p != NULL)
	*p = 0;
      if (*dirlist_ptr)
	ldfile_add_library_path (dirlist_ptr);
      if (p == NULL)
	break;
      *p = ':';
      dirlist_ptr = p + 1;
    }
}

static void
set_section_start (sect, valstr)
     char *sect, *valstr;
{
  char *end;
  unsigned long val = strtoul (valstr, &end, 16);
  if (*end)
    einfo ("%P%F: invalid hex number `%s'", valstr);
  lang_section_start (sect, exp_intop (val));
}
