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
#include "ldver.h"

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

  const char *shortopts = "-A:B::b:cde:F::G:giL:l:Mm:NnO:o:R:rSsT:tu:VvXxY:y:";

  /* 150 isn't special; it's just an arbitrary non-ASCII char value.  */

#define OPTION_CALL_SHARED		150
#define OPTION_DEFSYM			(OPTION_CALL_SHARED + 1)
#define OPTION_DYNAMIC_LINKER		(OPTION_DEFSYM + 1)
#define OPTION_EB			(OPTION_DYNAMIC_LINKER + 1)
#define OPTION_EL			(OPTION_EB + 1)
#define OPTION_HELP			(OPTION_EL + 1)
#define OPTION_IGNORE			(OPTION_HELP + 1)
#define OPTION_MAP			(OPTION_IGNORE + 1)
#define OPTION_NO_KEEP_MEMORY		(OPTION_MAP + 1)
#define OPTION_NOINHIBIT_EXEC		(OPTION_NO_KEEP_MEMORY + 1)
#define OPTION_NON_SHARED		(OPTION_NOINHIBIT_EXEC + 1)
#define OPTION_OFORMAT			(OPTION_NON_SHARED + 1)
#define OPTION_RELAX			(OPTION_OFORMAT + 1)
#define OPTION_RETAIN_SYMBOLS_FILE	(OPTION_RELAX + 1)
#define OPTION_SORT_COMMON		(OPTION_RETAIN_SYMBOLS_FILE + 1)
#define OPTION_STATS			(OPTION_SORT_COMMON + 1)
#define OPTION_TBSS			(OPTION_STATS + 1)
#define OPTION_TDATA			(OPTION_TBSS + 1)
#define OPTION_TTEXT			(OPTION_TDATA + 1)
#define OPTION_TRADITIONAL_FORMAT	(OPTION_TTEXT + 1)
#define OPTION_UR			(OPTION_TRADITIONAL_FORMAT + 1)
#define OPTION_VERSION			(OPTION_UR + 1)
#define OPTION_WARN_COMMON		(OPTION_VERSION + 1)

  static struct option longopts[] = {
    {"call_shared", no_argument, NULL, OPTION_CALL_SHARED},
    {"dc", no_argument, NULL, 'd'},
    {"defsym", required_argument, NULL, OPTION_DEFSYM},
    {"dn", no_argument, NULL, OPTION_NON_SHARED},
    {"dp", no_argument, NULL, 'd'},
    {"dy", no_argument, NULL, OPTION_CALL_SHARED},
    {"dynamic-linker", required_argument, NULL, OPTION_DYNAMIC_LINKER},
    {"EB", no_argument, NULL, OPTION_EB},
    {"EL", no_argument, NULL, OPTION_EL},
    {"format", required_argument, NULL, 'b'},
    {"help", no_argument, NULL, OPTION_HELP},
    {"Map", required_argument, NULL, OPTION_MAP},
    {"no-keep-memory", no_argument, NULL, OPTION_NO_KEEP_MEMORY},
    {"noinhibit-exec", no_argument, NULL, OPTION_NOINHIBIT_EXEC},
    {"noinhibit_exec", no_argument, NULL, OPTION_NOINHIBIT_EXEC},
    {"non_shared", no_argument, NULL, OPTION_NON_SHARED},
    {"oformat", required_argument, NULL, OPTION_OFORMAT},
    {"Qy", no_argument, NULL, OPTION_IGNORE},
    {"relax", no_argument, NULL, OPTION_RELAX},
    {"retain-symbols-file", no_argument, NULL, OPTION_RETAIN_SYMBOLS_FILE},
    {"sort-common", no_argument, NULL, OPTION_SORT_COMMON},
    {"sort_common", no_argument, NULL, OPTION_SORT_COMMON},
    {"stats", no_argument, NULL, OPTION_STATS},
    {"Tbss", required_argument, NULL, OPTION_TBSS},
    {"Tdata", required_argument, NULL, OPTION_TDATA},
    {"Ttext", required_argument, NULL, OPTION_TTEXT},
    {"traditional-format", no_argument, NULL, OPTION_TRADITIONAL_FORMAT},
    {"Ur", no_argument, NULL, OPTION_UR},
    {"version", no_argument, NULL, OPTION_VERSION},
    {"warn-common", no_argument, NULL, OPTION_WARN_COMMON},
    {NULL, no_argument, NULL, 0}
  };

  while (1)
    {
      /* getopt_long_only is like getopt_long, but '-' as well as '--' can
	 indicate a long option.  */
      int longind;
      int optc = getopt_long_only (argc, argv, shortopts, longopts, &longind);

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

	case OPTION_IGNORE:
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
	case OPTION_CALL_SHARED:
	  config.dynamic_link = true;
	  break;
	case OPTION_NON_SHARED:
	  config.dynamic_link = false;
	  break;
	case 'd':
	  command_line.force_common_definition = true;
	  break;
	case OPTION_DEFSYM:
	  lex_redirect (optarg);
	  parser_input = input_defsym;
	  yyparse ();
	  break;
	case OPTION_DYNAMIC_LINKER:
	  command_line.interpreter = optarg;
	  break;
	case OPTION_EB:
	  /* FIXME: This is currently ignored.  It means
	     ``produce a big-endian object file''.  It could
	     be used to select an output format.  */
	  break;
	case OPTION_EL:
	  /* FIXME: This is currently ignored.  It means
	     ``produce a little-endian object file''.  It could
	     be used to select an output format.  */
	  break;
	case 'e':
	  lang_add_entry (optarg, 1);
	  break;
	case 'F':
	  /* Ignore.  */
	  break;
	case 'G':
	  {
	    char *end;
	    g_switch_value = strtoul (optarg, &end, 0);
	    if (*end)
	      einfo ("%P%F: invalid number `%s'\n", optarg);
	  }
	  break;
	case 'g':
	  /* Ignore.  */
	  break;
	case OPTION_HELP:
	  help ();
	  xexit (0);
	  break;
	case 'L':
	  ldfile_add_library_path (optarg, true);
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
	case OPTION_MAP:
	  config.map_filename = optarg;
	  break;
	case 'N':
	  config.text_read_only = false;
	  config.magic_demand_paged = false;
	  break;
	case 'n':
	  config.magic_demand_paged = false;
	  break;
	case OPTION_NO_KEEP_MEMORY:
	  link_info.keep_memory = false;
	  break;
	case OPTION_NOINHIBIT_EXEC:
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
	case OPTION_OFORMAT:
	  lang_add_output_format (optarg, 0);
	  break;
	case 'i':
	case 'r':
	  link_info.relocateable = true;
	  config.build_constructors = false;
	  config.magic_demand_paged = false;
	  config.text_read_only = false;
	  config.dynamic_link = false;
	  break;
	case 'R':
	  lang_add_input_file (optarg,
			       lang_input_file_is_symbols_only_enum,
			       (char *) NULL);
	  break;
	case OPTION_RELAX:
	  command_line.relax = true;
	  break;
	case OPTION_RETAIN_SYMBOLS_FILE:
	  add_keepsyms_file (optarg);
	  break;
	case 'S':
	  link_info.strip = strip_debugger;
	  break;
	case 's':
	  link_info.strip = strip_all;
	  break;
	case OPTION_SORT_COMMON:
	  config.sort_common = true;
	  break;
	case OPTION_STATS:
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
	case OPTION_TBSS:
	  set_section_start (".bss", optarg);
	  break;
	case OPTION_TDATA:
	  set_section_start (".data", optarg);
	  break;
	case OPTION_TTEXT:
	  set_section_start (".text", optarg);
	  break;
	case OPTION_TRADITIONAL_FORMAT:
	  config.traditional_format = true;
	  break;
	case OPTION_UR:
	  link_info.relocateable = true;
	  config.build_constructors = true;
	  config.magic_demand_paged = false;
	  config.text_read_only = false;
	  config.dynamic_link = false;
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
	case OPTION_VERSION:
	  ldversion (0);
	  version_printed = true;
	  break;
	case OPTION_WARN_COMMON:
	  config.warn_common = true;
	  break;
	case 'X':
	  link_info.discard = discard_l;
	  break;
	case 'x':
	  link_info.discard = discard_all;
	  break;
	case 'Y':
	  set_default_dirlist (optarg);
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
	ldfile_add_library_path (dirlist_ptr, true);
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
    einfo ("%P%F: invalid hex number `%s'\n", valstr);
  lang_section_start (sect, exp_intop (val));
}
