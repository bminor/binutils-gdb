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
the Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

/* Somewhere above, sys/stat.h got included . . . . */
#if !defined(S_ISDIR) && defined(S_IFDIR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

/* Omit args to avoid the possibility of clashing with a system header
   that might disagree about consts.  */
unsigned long strtoul ();

static void set_default_dirlist PARAMS ((char *dirlist_ptr));
static void set_section_start PARAMS ((char *sect, char *valstr));

/* WINDOWS_NT; declare additional functions */
static void set_subsystem PARAMS ((char *subsystem_type));
static void set_stack_heap PARAMS ((char *valstr, boolean for_heap));

void
parse_args (argc, argv)
     int argc;
     char **argv;
{
  int i;
  int ingroup = 0;

  /* Starting the short option string with '-' is for programs that
     expect options and other ARGV-elements in any order and that care about
     the ordering of the two.  We describe each non-option ARGV-element
     as if it were the argument of an option with character code 1.  */

  const char *shortopts =
    "-a:A:B::b:c:de:F::G:giL:l:Mm:NnO:o:R:rSsT:tu:VvXxY:y:z:()";

  /* 150 isn't special; it's just an arbitrary non-ASCII char value.  */

#define OPTION_CALL_SHARED		150
#define OPTION_DEFSYM			(OPTION_CALL_SHARED + 1)
#define OPTION_DYNAMIC_LINKER		(OPTION_DEFSYM + 1)
#define OPTION_EB			(OPTION_DYNAMIC_LINKER + 1)
#define OPTION_EL			(OPTION_EB + 1)
#define OPTION_HEAP			(OPTION_EL + 1)
#define OPTION_EMBEDDED_RELOCS		(OPTION_HEAP + 1)
#define OPTION_EXPORT_DYNAMIC		(OPTION_EMBEDDED_RELOCS + 1)
#define OPTION_HELP			(OPTION_EXPORT_DYNAMIC + 1)
#define OPTION_IGNORE			(OPTION_HELP + 1)
#define OPTION_MAP			(OPTION_IGNORE + 1)
#define OPTION_NO_KEEP_MEMORY		(OPTION_MAP + 1)
#define OPTION_NOINHIBIT_EXEC		(OPTION_NO_KEEP_MEMORY + 1)
#define OPTION_NON_SHARED		(OPTION_NOINHIBIT_EXEC + 1)
#define OPTION_OFORMAT			(OPTION_NON_SHARED + 1)
#define OPTION_RELAX			(OPTION_OFORMAT + 1)
#define OPTION_RETAIN_SYMBOLS_FILE	(OPTION_RELAX + 1)
#define OPTION_RPATH			(OPTION_RETAIN_SYMBOLS_FILE + 1)
#define OPTION_SHARED			(OPTION_RPATH + 1)
#define OPTION_SONAME			(OPTION_SHARED + 1)
#define OPTION_SORT_COMMON		(OPTION_SONAME + 1)
#define OPTION_STACK                    (OPTION_SORT_COMMON + 1) /*WINDOWS_NT*/
#define OPTION_STATS			(OPTION_STACK + 1)
#define OPTION_SUBSYSTEM                (OPTION_STATS + 1) /* WINDOWS_NT */
#define OPTION_TBSS			(OPTION_SUBSYSTEM + 1)
#define OPTION_TDATA			(OPTION_TBSS + 1)
#define OPTION_TTEXT			(OPTION_TDATA + 1)
#define OPTION_TRADITIONAL_FORMAT	(OPTION_TTEXT + 1)
#define OPTION_UR			(OPTION_TRADITIONAL_FORMAT + 1)
#define OPTION_VERBOSE			(OPTION_UR + 1)
#define OPTION_VERSION			(OPTION_VERBOSE + 1)
#define OPTION_WARN_COMMON		(OPTION_VERSION + 1)
#define OPTION_WARN_ONCE		(OPTION_WARN_COMMON + 1)
#define OPTION_SPLIT_BY_RELOC		(OPTION_WARN_ONCE + 1)
#define OPTION_SPLIT_BY_FILE 	    	(OPTION_SPLIT_BY_RELOC + 1)
#define OPTION_WHOLE_ARCHIVE		(OPTION_SPLIT_BY_FILE + 1)
#define OPTION_BASE_FILE		(OPTION_WHOLE_ARCHIVE + 1)
  static struct option longopts[] = {
    {"Bdynamic", no_argument, NULL, OPTION_CALL_SHARED},
    {"Bstatic", no_argument, NULL, OPTION_NON_SHARED},
    {"call_shared", no_argument, NULL, OPTION_CALL_SHARED},
    {"dc", no_argument, NULL, 'd'},
    {"defsym", required_argument, NULL, OPTION_DEFSYM},
    {"dll-verbose", no_argument, NULL, OPTION_VERSION}, /* Linux.  */
    {"dn", no_argument, NULL, OPTION_NON_SHARED},
    {"dp", no_argument, NULL, 'd'},
    {"dy", no_argument, NULL, OPTION_CALL_SHARED},
    {"dynamic-linker", required_argument, NULL, OPTION_DYNAMIC_LINKER},
    {"EB", no_argument, NULL, OPTION_EB},
    {"EL", no_argument, NULL, OPTION_EL},
    {"embedded-relocs", no_argument, NULL, OPTION_EMBEDDED_RELOCS},
    {"end-group", no_argument, NULL, ')'},
    {"export-dynamic", no_argument, NULL, OPTION_EXPORT_DYNAMIC},
    {"format", required_argument, NULL, 'b'},
    {"heap", required_argument, NULL, OPTION_HEAP}, /* WINDOWS_NT */
    {"help", no_argument, NULL, OPTION_HELP},
    {"Map", required_argument, NULL, OPTION_MAP},
    {"no-keep-memory", no_argument, NULL, OPTION_NO_KEEP_MEMORY},
    {"noinhibit-exec", no_argument, NULL, OPTION_NOINHIBIT_EXEC},
    {"noinhibit_exec", no_argument, NULL, OPTION_NOINHIBIT_EXEC},
    {"non_shared", no_argument, NULL, OPTION_NON_SHARED},
    {"oformat", required_argument, NULL, OPTION_OFORMAT},
    {"Qy", no_argument, NULL, OPTION_IGNORE},
    {"qmagic", no_argument, NULL, OPTION_IGNORE}, /* Linux compatibility.  */
    {"relax", no_argument, NULL, OPTION_RELAX},
    {"retain-symbols-file", required_argument, NULL, OPTION_RETAIN_SYMBOLS_FILE},
    {"rpath", required_argument, NULL, OPTION_RPATH},
    {"shared", no_argument, NULL, OPTION_SHARED},
    {"soname", required_argument, NULL, OPTION_SONAME},
    {"sort-common", no_argument, NULL, OPTION_SORT_COMMON},
    {"sort_common", no_argument, NULL, OPTION_SORT_COMMON},
    {"stack", required_argument, NULL, OPTION_STACK}, /* WINDOWS_NT */
    {"start-group", no_argument, NULL, '('},
    {"stats", no_argument, NULL, OPTION_STATS},
    {"static", no_argument, NULL, OPTION_NON_SHARED},
    {"subsystem", required_argument, NULL, OPTION_SUBSYSTEM}, /* WINDOWS_NT */
    {"Tbss", required_argument, NULL, OPTION_TBSS},
    {"Tdata", required_argument, NULL, OPTION_TDATA},
    {"Ttext", required_argument, NULL, OPTION_TTEXT},
    {"traditional-format", no_argument, NULL, OPTION_TRADITIONAL_FORMAT},
    {"Ur", no_argument, NULL, OPTION_UR},
    {"verbose", no_argument, NULL, OPTION_VERBOSE},
    {"version", no_argument, NULL, OPTION_VERSION},
    {"warn-common", no_argument, NULL, OPTION_WARN_COMMON},
    {"warn-once", no_argument, NULL, OPTION_WARN_ONCE},
    {"split-by-reloc", required_argument, NULL, OPTION_SPLIT_BY_RELOC},
    {"split-by-file", no_argument, NULL, OPTION_SPLIT_BY_FILE},
    {"whole-archive", no_argument, NULL, OPTION_WHOLE_ARCHIVE},
    {"base-file", required_argument, NULL, OPTION_BASE_FILE},
    {NULL, no_argument, NULL, 0}
  };

  /* The -G option is ambiguous on different platforms.  Sometimes it
     specifies the largest data size to put into the small data
     section.  Sometimes it is equivalent to --shared.  Unfortunately,
     the first form takes an argument, while the second does not.

     We need to permit the --shared form because on some platforms,
     such as Solaris, gcc -shared will pass -G to the linker.

     To permit either usage, we look through the argument list.  If we
     find -G not followed by a number, we change it into --shared.
     This will work for most normal cases.  */
  for (i = 1; i < argc; i++)
    if (strcmp (argv[i], "-G") == 0
	&& (i + 1 >= argc
	    || ! isdigit (argv[i + 1][0])))
      argv[i] = (char *) "--shared";

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
	case 'a':
	  /* For HP/UX compatibility.  Actually -a shared should mean
             ``use only shared libraries'' but, then, we don't
             currently support shared libraries on HP/UX anyhow.  */
	  if (strcmp (optarg, "archive") == 0)
	    config.dynamic_link = false;
	  else if (strcmp (optarg, "shared") == 0
		   || strcmp (optarg, "default") == 0)
	    config.dynamic_link = true;
	  else
	    einfo ("%P%F: unrecognized -a option `%s'\n", optarg);
	  break;
	case 'A':
	  ldfile_add_arch (optarg);
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
	  command_line.endian = ENDIAN_BIG;
	  break;
	case OPTION_EL:
	  command_line.endian = ENDIAN_LITTLE;
	  break;
	case OPTION_EMBEDDED_RELOCS:
	  command_line.embedded_relocs = true;
	  break;
	case OPTION_EXPORT_DYNAMIC:
	  command_line.export_dynamic = true;
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
	case OPTION_HEAP:  /* WINDOWS_NT */
	  link_info.stack_heap_parameters.heap_defined = true;
	  set_stack_heap (optarg, true);
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
	  lang_add_output_format (optarg, (char *) NULL, (char *) NULL, 0);
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
	  /* The GNU linker traditionally uses -R to mean to include
	     only the symbols from a file.  The Solaris linker uses -R
	     to set the path used by the runtime linker to find
	     libraries.  This is the GNU linker -rpath argument.  We
	     try to support both simultaneously by checking the file
	     named.  If it is a directory, rather than a regular file,
	     we assume -rpath was meant.  */
	  {
	    struct stat s;

	    if (stat (optarg, &s) >= 0
		&& ! S_ISDIR (s.st_mode))
	      {
		lang_add_input_file (optarg,
				     lang_input_file_is_symbols_only_enum,
				     (char *) NULL);
		break;
	      }
	  }
	  /* Fall through.  */
	case OPTION_RPATH:
	  if (command_line.rpath == NULL)
	    command_line.rpath = buystring (optarg);
	  else
	    {
	      char *buf;

	      buf = xmalloc (strlen (command_line.rpath)
			     + strlen (optarg)
			     + 2);
	      sprintf (buf, "%s:%s", command_line.rpath, optarg);
	      free (command_line.rpath);
	      command_line.rpath = buf;
	    }
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
	case OPTION_SHARED:
	  link_info.shared = true;
	  break;
	case OPTION_SONAME:
	  command_line.soname = optarg;
	  break;
	case OPTION_SORT_COMMON:
	  config.sort_common = true;
	  break;
	case OPTION_STACK:  /* WINDOWS_NT */
	  link_info.stack_heap_parameters.stack_defined = true;
	  set_stack_heap (optarg, false);
	  break;
	case OPTION_STATS:
	  config.stats = true;
	  break;
	case OPTION_SUBSYSTEM:  /* WINDOWS_NT */
	  set_subsystem (optarg);
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
	case OPTION_VERBOSE:
	  ldversion (1);
	  version_printed = true;
	  trace_file_tries = true;
	  break;
	case 'v':
	  ldversion (0);
	  version_printed = true;
	  break;
	case 'V':
	  ldversion (1);
	  version_printed = true;
	  break;
	case OPTION_VERSION:
	  ldversion (0);
	  version_printed = true;
	  break;
	case OPTION_WARN_COMMON:
	  config.warn_common = true;
	  break;
	case OPTION_WARN_ONCE:
	  config.warn_once = true;
	  break;
	case OPTION_WHOLE_ARCHIVE:
	  whole_archive = true;
	  break;
	case OPTION_BASE_FILE:
	  link_info.base_file = (PTR) fopen (optarg,"w");
	  if (link_info.base_file == NULL)
	    {
	      fprintf (stderr, "%s: Can't open base file %s\n",
		       program_name, optarg);
	      xexit (1);
	    }
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
	case 'z':
	  /* We accept and ignore this option for Solaris
             compatibility.  Actually, on Solaris, optarg is not
             ignored.  Someday we should handle it correctly.  FIXME.  */
	  break;
	case OPTION_SPLIT_BY_RELOC:
	  config.split_by_reloc = atoi (optarg);
	  break; 
	case OPTION_SPLIT_BY_FILE:
	  config.split_by_file = true;
	  break; 
	case '(':
	  if (ingroup)
	    {
	      fprintf (stderr,
		       "%s: may not nest groups (--help for usage)\n",
		       program_name);
	      xexit (1);
	    }
	  lang_enter_group ();
	  ingroup = 1;
	  break;
	case ')':
	  if (! ingroup)
	    {
	      fprintf (stderr,
		       "%s: group ended before it began (--help for usage)\n",
		       program_name);
	      xexit (1);
	    }
	  lang_leave_group ();
	  ingroup = 0;
	  break;
	}
    }

  if (ingroup)
    lang_leave_group ();
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

/* WINDOWS_NT; added routines to get the subsystem type, heap and/or stack
   parameters which may be input from the command line */
static void
set_subsystem (subsystem_type)
  char *subsystem_type;
{
  if (strcmp (subsystem_type, "native") == 0)
    {
      link_info.subsystem = native;
    }
  else if (strcmp (subsystem_type, "windows") == 0)
    {
      link_info.subsystem = windows;
    }
  else if (strcmp (subsystem_type, "console") == 0)
    {
      link_info.subsystem = console;
    }
  else if (strcmp (subsystem_type, "os2") == 0)
    {
      link_info.subsystem = os2;
    }
  else if (strcmp (subsystem_type, "posix") == 0)
    {
      link_info.subsystem = posix;
    }
  else
    einfo ("%P%F: invalid subsystem type `%s'\n", subsystem_type);

}

static void
set_stack_heap (valstr, for_heap)
  char *valstr;
  boolean for_heap;
{
  char *begin_commit, *end;
  bfd_vma reserve = 0;
  bfd_vma commit  = 0;

  /* There may be two values passed in to the -stack or -heap switches like
     this:
       -stack 0x10000,0x100
     where the first parameter is the stack (or heap) reserve and the second
     is commit.  The second parameter is optional. */

  /* get the reserve value */
  reserve = strtoul (valstr, &begin_commit, 16);

  if (strcmp (valstr, begin_commit) == 0)  
    /* the reserve value couldn't be read */
    einfo ("%P%F: invalid hex number for reserve[,commit] '%s'\n", valstr); 
  else if (strcmp (begin_commit, "\0") != 0)  /* check for a commit value */
    {
     begin_commit += 1;  /* increment begin_commit to point past ',' */
     commit = strtoul (begin_commit, &end, 16);
     if (strcmp (end, begin_commit) == 0)
       einfo ("%P%F: invalid hex number for commit '%s'\n", begin_commit);
    }
   
  if (for_heap)
   {
    link_info.stack_heap_parameters.heap_reserve = reserve;     
    link_info.stack_heap_parameters.heap_commit  = commit;     
   }
  else
   {
    link_info.stack_heap_parameters.stack_reserve = reserve;     
    link_info.stack_heap_parameters.stack_commit  = commit;     
   }

#if DUMP_INFO
  printf ("reserve = %8x  commit = %8x\n", reserve, commit);
#endif
}
