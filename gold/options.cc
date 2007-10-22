// options.c -- handle command line options for gold

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"

#include <iostream>
#include <sys/stat.h>
#include "filenames.h"
#include "libiberty.h"

#include "options.h"

namespace gold
{

// The information we keep for a single command line option.

struct options::One_option
{
  // The single character option name, or '\0' if this is only a long
  // option.
  char short_option;

  // The long option name, or NULL if this is only a short option.
  const char* long_option;

  // Description of the option for --help output, or NULL if there is none.
  const char* doc;

  // How to print the option name in --help output, or NULL to use the
  // default.
  const char* help_output;

  // Long option dash control.  This is ignored if long_option is
  // NULL.
  enum
    {
      // Long option normally takes one dash; two dashes are also
      // accepted.
      ONE_DASH,
      // Long option normally takes two dashes; one dash is also
      // accepted.
      TWO_DASHES,
      // Long option always takes two dashes.
      EXACTLY_TWO_DASHES
    } dash;

  // Function for special handling, or NULL.  Returns the number of
  // arguments to skip.  This will normally be at least 1, but it may
  // be 0 if this function changes *argv.  ARG points to the location
  // in *ARGV where the option starts, which may be helpful for a
  // short option.
  int (*special)(int argc, char** argv, char *arg, Command_line*);

  // If this is a position independent option which does not take an
  // argument, this is the member function to call to record it.
  void (General_options::*general_noarg)();

  // If this is a position independent function which takes an
  // argument, this is the member function to call to record it.
  void (General_options::*general_arg)(const char*);

  // If this is a position dependent option which does not take an
  // argument, this is the member function to call to record it.
  void (Position_dependent_options::*dependent_noarg)();

  // If this is a position dependent option which takes an argument,
  // this is the member function to record it.
  void (Position_dependent_options::*dependent_arg)(const char*);

  // Return whether this option takes an argument.
  bool
  takes_argument() const
  { return this->general_arg != NULL || this->dependent_arg != NULL; }
};

class options::Command_line_options
{
 public:
  static const One_option options[];
  static const int options_size;
};

} // End namespace gold.

namespace
{

// Handle the special -l option, which adds an input file.

int
library(int argc, char** argv, char* arg, gold::Command_line* cmdline)
{
  return cmdline->process_l_option(argc, argv, arg);
}

// Handle the special --start-group option.

int
start_group(int, char**, char* arg, gold::Command_line* cmdline)
{
  cmdline->start_group(arg);
  return 1;
}

// Handle the special --end-group option.

int
end_group(int, char**, char* arg, gold::Command_line* cmdline)
{
  cmdline->end_group(arg);
  return 1;
}

// Report usage information for ld --help, and exit.

int
help(int, char**, char*, gold::Command_line*)
{
  printf(_("Usage: %s [options] file...\nOptions:\n"), gold::program_name);

  const int options_size = gold::options::Command_line_options::options_size;
  const gold::options::One_option* options =
    gold::options::Command_line_options::options;
  for (int i = 0; i < options_size; ++i)
    {
      if (options[i].doc == NULL)
	continue;

      printf("  ");
      int len = 2;
      bool comma = false;

      int j = i;
      do
	{
	  if (options[j].help_output != NULL)
	    {
	      if (comma)
		{
		  printf(", ");
		  len += 2;
		}
	      printf(options[j].help_output);
	      len += std::strlen(options[i].help_output);
              comma = true;
	    }
	  else
	    {
	      if (options[j].short_option != '\0')
		{
		  if (comma)
		    {
		      printf(", ");
		      len += 2;
		    }
		  printf("-%c", options[j].short_option);
		  len += 2;
                  comma = true;
		}

	      if (options[j].long_option != NULL)
		{
		  if (comma)
		    {
		      printf(", ");
		      len += 2;
		    }
		  if (options[j].dash == gold::options::One_option::ONE_DASH)
		    {
		      printf("-");
		      ++len;
		    }
		  else
		    {
		      printf("--");
		      len += 2;
		    }
		  printf("%s", options[j].long_option);
		  len += std::strlen(options[j].long_option);
                  comma = true;
		}
	    }
	  ++j;
	}
      while (j < options_size && options[j].doc == NULL);

      if (len >= 30)
	{
	  printf("\n");
	  len = 0;
	}
      for (; len < 30; ++len)
	std::putchar(' ');

      std::puts(options[i].doc);
    }

  ::exit(0);

  return 0;
}

// Report version information.

int
version(int, char**, char* opt, gold::Command_line*)
{
  gold::print_version(opt[0] == 'v' && opt[1] == '\0');
  ::exit(0);
  return 0;
}

// If the default sysroot is relocatable, try relocating it based on
// the prefix FROM.

char*
get_relative_sysroot(const char* from)
{
  char* path = make_relative_prefix(gold::program_name, from,
				    TARGET_SYSTEM_ROOT);
  if (path != NULL)
    {
      struct stat s;
      if (::stat(path, &s) == 0 && S_ISDIR(s.st_mode))
	return path;
      free(path);
    }

  return NULL;
}

// Return the default sysroot.  This is set by the --with-sysroot
// option to configure.

std::string
get_default_sysroot()
{
  const char* sysroot = TARGET_SYSTEM_ROOT;
  if (*sysroot == '\0')
    return "";

  if (TARGET_SYSTEM_ROOT_RELOCATABLE)
    {
      char* path = get_relative_sysroot (BINDIR);
      if (path == NULL)
	path = get_relative_sysroot (TOOLBINDIR);
      if (path != NULL)
	{
	  std::string ret = path;
	  free(path);
	  return ret;
	}
    }

  return sysroot;
}

} // End anonymous namespace.

namespace gold
{

// Helper macros used to specify the options.  We could also do this
// using constructors, but then g++ would generate code to initialize
// the array.  We want the array to be initialized statically so that
// we get better startup time.

#define GENERAL_NOARG(short_option, long_option, doc, help, dash, func)	\
  { short_option, long_option, doc, help, options::One_option::dash, \
      NULL, func, NULL, NULL, NULL }
#define GENERAL_ARG(short_option, long_option, doc, help, dash, func)	\
  { short_option, long_option, doc, help, options::One_option::dash, \
      NULL, NULL, func, NULL, NULL }
#define POSDEP_NOARG(short_option, long_option, doc, help, dash, func)	\
  { short_option, long_option, doc, help, options::One_option::dash, \
      NULL,  NULL, NULL, func, NULL }
#define POSDEP_ARG(short_option, long_option, doc, help, dash, func)	\
  { short_option, long_option, doc, help, options::One_option::dash, \
      NULL, NULL, NULL, NULL, func }
#define SPECIAL(short_option, long_option, doc, help, dash, func)	\
  { short_option, long_option, doc, help, options::One_option::dash, \
      func, NULL, NULL, NULL, NULL }

// Here is the actual list of options which we accept.

const options::One_option
options::Command_line_options::options[] =
{
  POSDEP_NOARG('\0', "as-needed",
	       N_("Only set DT_NEEDED for dynamic libs if used"),
	       NULL, TWO_DASHES, &Position_dependent_options::set_as_needed),
  POSDEP_NOARG('\0', "no-as-needed",
	       N_("Always DT_NEEDED for dynamic libs (default)"),
	       NULL, TWO_DASHES, &Position_dependent_options::clear_as_needed),
  POSDEP_NOARG('\0', "Bdynamic",
	       N_("-l searches for shared libraries"),
	       NULL, ONE_DASH,
	       &Position_dependent_options::set_dynamic_search),
  POSDEP_NOARG('\0', "Bstatic",
	       N_("-l does not search for shared libraries"),
	       NULL, ONE_DASH,
	       &Position_dependent_options::set_static_search),
  GENERAL_NOARG('\0', "Bsymbolic", N_("Bind defined symbols locally"),
		NULL, ONE_DASH, &General_options::set_symbolic),
  GENERAL_NOARG('E', "export-dynamic", N_("Export all dynamic symbols"),
                NULL, TWO_DASHES, &General_options::set_export_dynamic),
  GENERAL_NOARG('\0', "eh-frame-hdr", N_("Create exception frame header"),
                NULL, TWO_DASHES, &General_options::set_create_eh_frame_hdr),
  GENERAL_ARG('I', "dynamic-linker", N_("Set dynamic linker path"),
	      N_("-I PROGRAM, --dynamic-linker PROGRAM"), TWO_DASHES,
	      &General_options::set_dynamic_linker),
  SPECIAL('l', "library", N_("Search for library LIBNAME"),
	  N_("-lLIBNAME, --library LIBNAME"), TWO_DASHES,
	  &library),
  GENERAL_ARG('L', "library-path", N_("Add directory to search path"),
	      N_("-L DIR, --library-path DIR"), TWO_DASHES,
	      &General_options::add_to_search_path),
  GENERAL_ARG('m', NULL, N_("Ignored for compatibility"), NULL, ONE_DASH,
	      &General_options::ignore),
  GENERAL_ARG('o', "output", N_("Set output file name"),
	      N_("-o FILE, --output FILE"), TWO_DASHES,
	      &General_options::set_output_file_name),
  GENERAL_ARG('O', NULL, N_("Optimize output file size"),
	      N_("-O level"), ONE_DASH,
	      &General_options::set_optimization_level),
  GENERAL_NOARG('r', NULL, N_("Generate relocatable output"), NULL,
		ONE_DASH, &General_options::set_relocatable),
  GENERAL_ARG('R', "rpath", N_("Add DIR to runtime search path"),
              N_("-R DIR, -rpath DIR"), ONE_DASH,
              &General_options::add_to_rpath),
  GENERAL_ARG('\0', "rpath-link",
              N_("Add DIR to link time shared library search path"),
              N_("--rpath-link DIR"), TWO_DASHES,
              &General_options::add_to_rpath_link),
  GENERAL_NOARG('s', "strip-all", N_("Strip all symbols"), NULL,
		TWO_DASHES, &General_options::set_strip_all),
  GENERAL_NOARG('S', "strip-debug", N_("Strip debugging information"), NULL,
		TWO_DASHES, &General_options::set_strip_debug),
  GENERAL_NOARG('\0', "shared", N_("Generate shared library"),
		NULL, ONE_DASH, &General_options::set_shared),
  GENERAL_NOARG('\0', "static", N_("Do not link against shared libraries"),
		NULL, ONE_DASH, &General_options::set_static),
  GENERAL_NOARG('\0', "stats", N_("Print resource usage statistics"),
		NULL, TWO_DASHES, &General_options::set_stats),
  GENERAL_ARG('\0', "sysroot", N_("Set target system root directory"),
	      N_("--sysroot DIR"), TWO_DASHES, &General_options::set_sysroot),
  GENERAL_ARG('\0', "Ttext", N_("Set the address of the .text section"),
              N_("-Ttext ADDRESS"), ONE_DASH,
              &General_options::set_text_segment_address),
  GENERAL_NOARG('\0', "threads", N_("Run the linker multi-threaded"),
		NULL, TWO_DASHES, &General_options::set_threads),
  GENERAL_NOARG('\0', "no-threads", N_("Do not run the linker multi-threaded"),
		NULL, TWO_DASHES, &General_options::clear_threads),
  GENERAL_ARG('\0', "thread-count", N_("Number of threads to use"),
	      N_("--thread-count COUNT"), TWO_DASHES,
	      &General_options::set_thread_count),
  GENERAL_ARG('\0', "thread-count-initial",
	      N_("Number of threads to use in initial pass"),
	      N_("--thread-count-initial COUNT"), TWO_DASHES,
	      &General_options::set_thread_count_initial),
  GENERAL_ARG('\0', "thread-count-middle",
	      N_("Number of threads to use in middle pass"),
	      N_("--thread-count-middle COUNT"), TWO_DASHES,
	      &General_options::set_thread_count_middle),
  GENERAL_ARG('\0', "thread-count-final",
	      N_("Number of threads to use in final pass"),
	      N_("--thread-count-final COUNT"), TWO_DASHES,
	      &General_options::set_thread_count_final),
  POSDEP_NOARG('\0', "whole-archive",
               N_("Include all archive contents"),
               NULL, TWO_DASHES,
               &Position_dependent_options::set_whole_archive),
  POSDEP_NOARG('\0', "no-whole-archive",
               N_("Include only needed archive contents"),
               NULL, TWO_DASHES,
               &Position_dependent_options::clear_whole_archive),
  SPECIAL('(', "start-group", N_("Start a library search group"), NULL,
	  TWO_DASHES, &start_group),
  SPECIAL(')', "end-group", N_("End a library search group"), NULL,
	  TWO_DASHES, &end_group),
  SPECIAL('\0', "help", N_("Report usage information"), NULL,
	  TWO_DASHES, &help),
  SPECIAL('v', "version", N_("Report version information"), NULL,
	  TWO_DASHES, &version)
};

const int options::Command_line_options::options_size =
  sizeof (options) / sizeof (options[0]);

// The default values for the general options.

General_options::General_options()
  : export_dynamic_(false),
    dynamic_linker_(NULL),
    search_path_(),
    optimization_level_(0),
    output_file_name_("a.out"),
    is_relocatable_(false),
    strip_(STRIP_NONE),
    symbolic_(false),
    create_eh_frame_hdr_(false),
    rpath_(),
    rpath_link_(),
    is_shared_(false),
    is_static_(false),
    print_stats_(false),
    sysroot_(),
    text_segment_address_(-1U),   // -1 indicates value not set by user
    threads_(false),
    thread_count_initial_(0),
    thread_count_middle_(0),
    thread_count_final_(0)
{
}

// The default values for the position dependent options.

Position_dependent_options::Position_dependent_options()
  : do_static_search_(false),
    as_needed_(false),
    include_whole_archive_(false)
{
}

// Add the sysroot, if any, to the search paths.

void
General_options::add_sysroot()
{
  if (this->sysroot_.empty())
    {
      this->sysroot_ = get_default_sysroot();
      if (this->sysroot_.empty())
	return;
    }

  const char* sysroot = this->sysroot_.c_str();
  char* canonical_sysroot = lrealpath(sysroot);

  for (Dir_list::iterator p = this->search_path_.begin();
       p != this->search_path_.end();
       ++p)
    p->add_sysroot(sysroot, canonical_sysroot);

  free(canonical_sysroot);
}

// Search_directory methods.

// This is called if we have a sysroot.  Apply the sysroot if
// appropriate.  Record whether the directory is in the sysroot.

void
Search_directory::add_sysroot(const char* sysroot,
			      const char* canonical_sysroot)
{
  gold_assert(*sysroot != '\0');
  if (this->put_in_sysroot_)
    {
      if (!IS_DIR_SEPARATOR(this->name_[0])
	  && !IS_DIR_SEPARATOR(sysroot[strlen(sysroot) - 1]))
	this->name_ = '/' + this->name_;
      this->name_ = sysroot + this->name_;
      this->is_in_sysroot_ = true;
    }
  else
    {
      // Check whether this entry is in the sysroot.  To do this
      // correctly, we need to use canonical names.  Otherwise we will
      // get confused by the ../../.. paths that gcc tends to use.
      char* canonical_name = lrealpath(this->name_.c_str());
      int canonical_name_len = strlen(canonical_name);
      int canonical_sysroot_len = strlen(canonical_sysroot);
      if (canonical_name_len > canonical_sysroot_len
	  && IS_DIR_SEPARATOR(canonical_name[canonical_sysroot_len]))
	{
	  canonical_name[canonical_sysroot_len] = '\0';
	  if (FILENAME_CMP(canonical_name, canonical_sysroot) == 0)
	    this->is_in_sysroot_ = true;
	}
      free(canonical_name);
    }
}

// Input_arguments methods.

// Add a file to the list.

void
Input_arguments::add_file(const Input_file_argument& file)
{
  if (!this->in_group_)
    this->input_argument_list_.push_back(Input_argument(file));
  else
    {
      gold_assert(!this->input_argument_list_.empty());
      gold_assert(this->input_argument_list_.back().is_group());
      this->input_argument_list_.back().group()->add_file(file);
    }
}

// Start a group.

void
Input_arguments::start_group()
{
  gold_assert(!this->in_group_);
  Input_file_group* group = new Input_file_group();
  this->input_argument_list_.push_back(Input_argument(group));
  this->in_group_ = true;
}

// End a group.

void
Input_arguments::end_group()
{
  gold_assert(this->in_group_);
  this->in_group_ = false;
}

// Command_line options.

Command_line::Command_line()
  : options_(), position_options_(), inputs_()
{
}

// Process the command line options.

void
Command_line::process(int argc, char** argv)
{
  const int options_size = options::Command_line_options::options_size;
  const options::One_option* options =
    options::Command_line_options::options;
  bool no_more_options = false;
  int i = 0;
  while (i < argc)
    {
      if (argv[i][0] != '-' || no_more_options)
	{
	  this->add_file(argv[i], false);
	  ++i;
	  continue;
	}

      // Option starting with '-'.
      int dashes = 1;
      if (argv[i][1] == '-')
	{
	  dashes = 2;
	  if (argv[i][2] == '\0')
	    {
	      no_more_options = true;
	      continue;
	    }
	}

      // Look for a long option match.
      char* opt = argv[i] + dashes;
      char first = opt[0];
      int skiparg = 0;
      char* arg = strchr(opt, '=');
      bool argument_with_equals = arg != NULL;
      if (arg != NULL)
	{
	  *arg = '\0';
	  ++arg;
	}
      else if (i + 1 < argc)
	{
	  arg = argv[i + 1];
	  skiparg = 1;
	}

      int j;
      for (j = 0; j < options_size; ++j)
	{
	  if (options[j].long_option != NULL
	      && (dashes == 2
		  || (options[j].dash
		      != options::One_option::EXACTLY_TWO_DASHES))
	      && first == options[j].long_option[0]
	      && strcmp(opt, options[j].long_option) == 0)
	    {
	      if (options[j].special)
		i += options[j].special(argc - 1, argv + i, opt, this);
	      else
		{
		  if (!options[j].takes_argument())
		    {
		      if (argument_with_equals)
			this->usage(_("unexpected argument"), argv[i]);
		      arg = NULL;
		      skiparg = 0;
		    }
		  else
		    {
		      if (arg == NULL)
			this->usage(_("missing argument"), argv[i]);
		    }
		  this->apply_option(options[j], arg);
		  i += skiparg + 1;
		}
	      break;
	    }
	}
      if (j < options_size)
	continue;

      // If we saw two dashes, we need to see a long option.
      if (dashes == 2)
	this->usage(_("unknown option"), argv[i]);

      // Look for a short option match.  There may be more than one
      // short option in a given argument.
      bool done = false;
      char* s = argv[i] + 1;
      ++i;
      while (*s != '\0' && !done)
	{
	  char opt = *s;
	  int j;
	  for (j = 0; j < options_size; ++j)
	    {
	      if (options[j].short_option == opt)
		{
		  if (options[j].special)
		    {
		      // Undo the argument skip done above.
		      --i;
		      i += options[j].special(argc - i, argv + i, s, this);
		      done = true;
		    }
		  else
		    {
		      arg = NULL;
		      if (options[j].takes_argument())
			{
			  if (s[1] != '\0')
			    {
			      arg = s + 1;
			      done = true;
			    }
			  else if (i < argc)
			    {
			      arg = argv[i];
			      ++i;
			    }
			  else
			    this->usage(_("missing argument"), opt);
			}
		      this->apply_option(options[j], arg);
		    }
		  break;
		}
	    }

	  if (j >= options_size)
	    this->usage(_("unknown option"), *s);

	  ++s;
	}
    }

  if (this->inputs_.in_group())
    {
      fprintf(stderr, _("%s: missing group end"), program_name);
      this->usage();
    }

  // FIXME: We should only do this when configured in native mode.
  this->options_.add_to_search_path_with_sysroot("/lib");
  this->options_.add_to_search_path_with_sysroot("/usr/lib");

  this->options_.add_sysroot();

  // Ensure options don't contradict each other and are otherwise kosher.
  this->normalize_options();
}

// Ensure options don't contradict each other and are otherwise kosher.

void
Command_line::normalize_options()
{
  // If the user specifies both -s and -r, convert the -s as -S.
  // -r requires us to keep externally visible symbols!
  if (this->options_.strip_all() && this->options_.is_relocatable())
    {
      // Clears the strip_all() status, replacing it with strip_debug().
      this->options_.set_strip_debug();
    }

  // FIXME: we can/should be doing a lot more sanity checking here.
}


// Apply a command line option.

void
Command_line::apply_option(const options::One_option& opt,
			   const char* arg)
{
  if (arg == NULL)
    {
      if (opt.general_noarg)
	(this->options_.*(opt.general_noarg))();
      else if (opt.dependent_noarg)
	(this->position_options_.*(opt.dependent_noarg))();
      else
	gold_unreachable();
    }
  else
    {
      if (opt.general_arg)
	(this->options_.*(opt.general_arg))(arg);
      else if (opt.dependent_arg)
	(this->position_options_.*(opt.dependent_arg))(arg);
      else
	gold_unreachable();
    }
}

// Add an input file or library.

void
Command_line::add_file(const char* name, bool is_lib)
{
  Input_file_argument file(name, is_lib, "", this->position_options_);
  this->inputs_.add_file(file);
}

// Handle the -l option, which requires special treatment.

int
Command_line::process_l_option(int argc, char** argv, char* arg)
{
  int ret;
  const char* libname;
  if (arg[1] != '\0')
    {
      ret = 1;
      libname = arg + 1;
    }
  else if (argc > 1)
    {
      ret = 2;
      libname = argv[argc + 1];
    }
  else
    this->usage(_("missing argument"), arg);

  this->add_file(libname, true);

  return ret;
}

// Handle the --start-group option.

void
Command_line::start_group(const char* arg)
{
  if (this->inputs_.in_group())
    this->usage(_("may not nest groups"), arg);
  this->inputs_.start_group();
}

// Handle the --end-group option.

void
Command_line::end_group(const char* arg)
{
  if (!this->inputs_.in_group())
    this->usage(_("group end without group start"), arg);
  this->inputs_.end_group();
}

// Report a usage error.  */

void
Command_line::usage()
{
  fprintf(stderr,
	  _("%s: use the --help option for usage information\n"),
	  program_name);
  ::exit(1);
}

void
Command_line::usage(const char* msg, const char *opt)
{
  fprintf(stderr,
	  _("%s: %s: %s\n"),
	  program_name, opt, msg);
  this->usage();
}

void
Command_line::usage(const char* msg, char opt)
{
  fprintf(stderr,
	  _("%s: -%c: %s\n"),
	  program_name, opt, msg);
  this->usage();
}

} // End namespace gold.
