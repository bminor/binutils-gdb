// options.c -- handle command line options for gold

// Copyright 2006, 2007, 2008 Free Software Foundation, Inc.
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

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include "filenames.h"
#include "libiberty.h"

#include "debug.h"
#include "script.h"
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
  int (*special)(int argc, char** argv, char *arg, bool long_option,
		 Command_line*);

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

// We have a separate table for -z options.

struct options::One_z_option
{
  // The name of the option.
  const char* name;

  // The member function in General_options called to record it.
  void (General_options::*set)();
};

// We have a separate table for --debug options.

struct options::One_debug_option
{
  // The name of the option.
  const char* name;

  // The flags to turn on.
  unsigned int debug_flags;
};

class options::Command_line_options
{
 public:
  static const One_option options[];
  static const int options_size;
  static const One_z_option z_options[];
  static const int z_options_size;
  static const One_debug_option debug_options[];
  static const int debug_options_size;
};

} // End namespace gold.

namespace
{

// Handle the special -l option, which adds an input file.

int
library(int argc, char** argv, char* arg, bool long_option,
	gold::Command_line* cmdline)
{
  return cmdline->process_l_option(argc, argv, arg, long_option);
}

// Handle the -R option.  Historically the GNU linker made -R a
// synonym for --just-symbols.  ELF linkers have traditionally made -R
// a synonym for -rpath.  When ELF support was added to the GNU
// linker, -R was changed to switch based on the argument: if the
// argument is an ordinary file, we treat it as --just-symbols,
// otherwise we treat it as -rpath.  We need to be compatible with
// this, because existing build scripts rely on it.

int
handle_r_option(int argc, char** argv, char* arg, bool long_option,
		gold::Command_line* cmdline)
{
  int ret;
  const char* val = cmdline->get_special_argument("R", argc, argv, arg,
						  long_option, &ret);
  struct stat s;
  if (::stat(val, &s) != 0 || S_ISDIR(s.st_mode))
    cmdline->add_to_rpath(val);
  else
    cmdline->add_just_symbols_file(val);
  return ret;
}

// Handle the --just-symbols option.

int
handle_just_symbols_option(int argc, char** argv, char* arg,
			   bool long_option, gold::Command_line* cmdline)
{
  int ret;
  const char* val = cmdline->get_special_argument("just-symbols", argc, argv,
						  arg, long_option, &ret);
  cmdline->add_just_symbols_file(val);
  return ret;
}

// Handle the special -T/--script option, which reads a linker script.

int
invoke_script(int argc, char** argv, char* arg, bool long_option,
	      gold::Command_line* cmdline)
{
  int ret;
  const char* script_name = cmdline->get_special_argument("script", argc, argv,
							  arg, long_option,
							  &ret);
  if (!read_commandline_script(script_name, cmdline))
    gold::gold_fatal(_("unable to parse script file %s"), script_name);
  return ret;
}

// Handle the special --version-script option, which reads a version script.

int
invoke_version_script(int argc, char** argv, char* arg, bool long_option,
                      gold::Command_line* cmdline)
{
  int ret;
  const char* script_name = cmdline->get_special_argument("version-script",
                                                          argc, argv,
							  arg, long_option,
							  &ret);
  if (!read_version_script(script_name, cmdline))
    gold::gold_fatal(_("unable to parse version script file %s"), script_name);
  return ret;
}

// Handle the special --start-group option.

int
start_group(int, char**, char* arg, bool, gold::Command_line* cmdline)
{
  cmdline->start_group(arg);
  return 1;
}

// Handle the special --end-group option.

int
end_group(int, char**, char* arg, bool, gold::Command_line* cmdline)
{
  cmdline->end_group(arg);
  return 1;
}

// Report usage information for ld --help, and exit.

int
help(int, char**, char*, bool, gold::Command_line*)
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
	      len += std::strlen(options[j].help_output);
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

  ::exit(EXIT_SUCCESS);

  return 0;
}

// Report version information.

int
version(int, char**, char* opt, bool, gold::Command_line*)
{
  gold::print_version(opt[0] == 'v' && opt[1] == '\0');
  ::exit(EXIT_SUCCESS);
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
  GENERAL_NOARG('\0', "allow-shlib-undefined",
		N_("Allow unresolved references in shared libraries"),
		NULL, TWO_DASHES,
		&General_options::set_allow_shlib_undefined),
  GENERAL_NOARG('\0', "no-allow-shlib-undefined",
		N_("Do not allow unresolved references in shared libraries"),
		NULL, TWO_DASHES,
		&General_options::set_no_allow_shlib_undefined),
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
#ifdef HAVE_ZLIB_H
# define ZLIB_STR  ",zlib"
#else
# define ZLIB_STR  ""
#endif
  GENERAL_ARG('\0', "compress-debug-sections",
              N_("Compress .debug_* sections in the output file "
                 "(default is none)"),
              N_("--compress-debug-sections=[none" ZLIB_STR "]"),
              TWO_DASHES,
              &General_options::set_compress_debug_sections),
  GENERAL_ARG('\0', "defsym", N_("Define a symbol"),
	      N_("--defsym SYMBOL=EXPRESSION"), TWO_DASHES,
	      &General_options::define_symbol),
  GENERAL_NOARG('\0', "demangle", N_("Demangle C++ symbols in log messages"),
                NULL, TWO_DASHES, &General_options::set_demangle),
  GENERAL_NOARG('\0', "no-demangle",
		N_("Do not demangle C++ symbols in log messages"),
                NULL, TWO_DASHES, &General_options::clear_demangle),
  GENERAL_NOARG('\0', "detect-odr-violations",
                N_("Try to detect violations of the One Definition Rule"),
                NULL, TWO_DASHES, &General_options::set_detect_odr_violations),
  GENERAL_ARG('e', "entry", N_("Set program start address"),
	      N_("-e ADDRESS, --entry ADDRESS"), TWO_DASHES,
	      &General_options::set_entry),
  GENERAL_NOARG('E', "export-dynamic", N_("Export all dynamic symbols"),
                NULL, TWO_DASHES, &General_options::set_export_dynamic),
  GENERAL_NOARG('\0', "eh-frame-hdr", N_("Create exception frame header"),
                NULL, TWO_DASHES, &General_options::set_create_eh_frame_hdr),
  GENERAL_ARG('h', "soname", N_("Set shared library name"),
	      N_("-h FILENAME, -soname FILENAME"), ONE_DASH,
	      &General_options::set_soname),
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
  GENERAL_ARG('\0', "oformat", N_("Set output format (only binary supported)"),
	      N_("--oformat FORMAT"), EXACTLY_TWO_DASHES,
	      &General_options::set_output_format),
  GENERAL_NOARG('r', NULL, N_("Generate relocatable output"), NULL,
		ONE_DASH, &General_options::set_relocatable),
  // -R really means -rpath, but can mean --just-symbols for
  // compatibility with GNU ld.  -rpath is always -rpath, so we list
  // it separately.
  SPECIAL('R', NULL, N_("Add DIR to runtime search path"),
	  N_("-R DIR"), ONE_DASH, &handle_r_option),
  GENERAL_ARG('\0', "rpath", NULL, N_("-rpath DIR"), ONE_DASH,
	      &General_options::add_to_rpath),
  SPECIAL('\0', "just-symbols", N_("Read only symbol values from file"),
	  N_("-R FILE, --just-symbols FILE"), TWO_DASHES,
	  &handle_just_symbols_option),
  GENERAL_ARG('\0', "rpath-link",
              N_("Add DIR to link time shared library search path"),
              N_("--rpath-link DIR"), TWO_DASHES,
              &General_options::add_to_rpath_link),
  GENERAL_NOARG('s', "strip-all", N_("Strip all symbols"), NULL,
		TWO_DASHES, &General_options::set_strip_all),
  GENERAL_NOARG('\0', "strip-debug-gdb",
                N_("Strip debug symbols that are unused by gdb "
                   "(at least versions <= 6.7)"),
		NULL, TWO_DASHES, &General_options::set_strip_debug_gdb),
  // This must come after -Sdebug since it's a prefix of it.
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
  // This must come after -Ttext since it's a prefix of it.
  SPECIAL('T', "script", N_("Read linker script"),
	  N_("-T FILE, --script FILE"), TWO_DASHES,
	  &invoke_script),
  SPECIAL('\0', "version-script", N_("Read version script"),
	  N_("--version-script FILE"), TWO_DASHES,
	  &invoke_version_script),
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

  GENERAL_ARG('z', NULL,
	      N_("Subcommands as follows:\n\
    -z execstack              Mark output as requiring executable stack\n\
    -z noexecstack            Mark output as not requiring executable stack"),
	      N_("-z SUBCOMMAND"), ONE_DASH,
	      &General_options::handle_z_option),

  SPECIAL('(', "start-group", N_("Start a library search group"), NULL,
	  TWO_DASHES, &start_group),
  SPECIAL(')', "end-group", N_("End a library search group"), NULL,
	  TWO_DASHES, &end_group),
  SPECIAL('\0', "help", N_("Report usage information"), NULL,
	  TWO_DASHES, &help),
  SPECIAL('v', "version", N_("Report version information"), NULL,
	  TWO_DASHES, &version),
  GENERAL_ARG('\0', "debug", N_("Turn on debugging (all,task,script)"),
	      N_("--debug=TYPE"), TWO_DASHES,
	      &General_options::handle_debug_option)
};

const int options::Command_line_options::options_size =
  sizeof (options) / sizeof (options[0]);

// The -z options.

const options::One_z_option
options::Command_line_options::z_options[] =
{
  { "execstack", &General_options::set_execstack },
  { "noexecstack", &General_options::set_noexecstack },
};

const int options::Command_line_options::z_options_size =
  sizeof(z_options) / sizeof(z_options[0]);

// The --debug options.

const options::One_debug_option
options::Command_line_options::debug_options[] =
{
  { "all", DEBUG_ALL },
  { "task", DEBUG_TASK },
  { "script", DEBUG_SCRIPT }
};

const int options::Command_line_options::debug_options_size =
  sizeof(debug_options) / sizeof(debug_options[0]);

// The default values for the general options.

General_options::General_options(Script_options* script_options)
  : export_dynamic_(false),
    soname_(NULL),
    dynamic_linker_(NULL),
    search_path_(),
    optimization_level_(0),
    output_file_name_("a.out"),
    output_format_(OUTPUT_FORMAT_ELF),
    is_relocatable_(false),
    strip_(STRIP_NONE),
    allow_shlib_undefined_(false),
    symbolic_(false),
    compress_debug_sections_(NO_COMPRESSION),
    detect_odr_violations_(false),
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
    thread_count_final_(0),
    execstack_(EXECSTACK_FROM_INPUT),
    debug_(0),
    script_options_(script_options)
{
  // We initialize demangle_ based on the environment variable
  // COLLECT_NO_DEMANGLE.  The gcc collect2 program will demangle the
  // output of the linker, unless COLLECT_NO_DEMANGLE is set in the
  // environment.  Acting the same way here lets us provide the same
  // interface by default.
  this->demangle_ = getenv("COLLECT_NO_DEMANGLE") == NULL;
}

// Handle the --defsym option.

void
General_options::define_symbol(const char* arg)
{
  this->script_options_->define_symbol(arg);
}

// Handle the --oformat option.  The GNU linker accepts a target name
// with --oformat.  In practice for an ELF target this would be the
// same target as the input files.  That name always start with "elf".
// Non-ELF targets would be "srec", "symbolsrec", "tekhex", "binary",
// "ihex".

void
General_options::set_output_format(const char* arg)
{
  if (strncmp(arg, "elf", 3) == 0)
    this->output_format_ = OUTPUT_FORMAT_ELF;
  else if (strcmp(arg, "binary") == 0)
    this->output_format_ = OUTPUT_FORMAT_BINARY;
  else
    gold_error(_("format '%s' not supported (supported formats: elf, binary)"),
	       arg);
}

// Handle the -z option.

void
General_options::handle_z_option(const char* arg)
{
  const int z_options_size = options::Command_line_options::z_options_size;
  const gold::options::One_z_option* z_options =
    gold::options::Command_line_options::z_options;
  for (int i = 0; i < z_options_size; ++i)
    {
      if (strcmp(arg, z_options[i].name) == 0)
	{
	  (this->*(z_options[i].set))();
	  return;
	}
    }

  fprintf(stderr, _("%s: unrecognized -z subcommand: %s\n"),
	  program_name, arg);
  ::exit(EXIT_FAILURE);
}

// Handle the --debug option.

void
General_options::handle_debug_option(const char* arg)
{
  const int debug_options_size =
    options::Command_line_options::debug_options_size;
  const gold::options::One_debug_option* debug_options =
    options::Command_line_options::debug_options;
  for (int i = 0; i < debug_options_size; ++i)
    {
      if (strcmp(arg, debug_options[i].name) == 0)
	{
	  this->set_debug(debug_options[i].debug_flags);
	  return;
	}
    }

  fprintf(stderr, _("%s: unrecognized --debug subcommand: %s\n"),
	  program_name, arg);
  ::exit(EXIT_FAILURE);
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

// The default values for the position dependent options.

Position_dependent_options::Position_dependent_options()
  : do_static_search_(false),
    as_needed_(false),
    include_whole_archive_(false)
{
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

Command_line::Command_line(Script_options* script_options)
  : options_(script_options), position_options_(), inputs_()
{
}

// Process the command line options.  For process_one_option,
// i is the index of argv to process next, and the return value
// is the index of the next option to process (i+1 or i+2, or argc
// to indicate processing is done).  no_more_options is set to true
// if (and when) "--" is seen as an option.

int
Command_line::process_one_option(int argc, char** argv, int i,
                                 bool* no_more_options)
{
  const int options_size = options::Command_line_options::options_size;
  const options::One_option* options = options::Command_line_options::options;
  gold_assert(i < argc);

  if (argv[i][0] != '-' || *no_more_options)
    {
      this->add_file(argv[i], false);
      return i + 1;
    }

  // Option starting with '-'.
  int dashes = 1;
  if (argv[i][1] == '-')
    {
      dashes = 2;
      if (argv[i][2] == '\0')
        {
          *no_more_options = true;
          return i + 1;
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
    	    {
    	      // Restore the '=' we clobbered above.
    	      if (arg != NULL && skiparg == 0)
    	        arg[-1] = '=';
    	      i += options[j].special(argc - i, argv + i, opt, true, this);
    	    }
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
    return i;

  // If we saw two dashes, we needed to have seen a long option.
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
    	          i += options[j].special(argc - i, argv + i, s, false,
                                          this);
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
  return i;
}


void
Command_line::process(int argc, char** argv)
{
  bool no_more_options = false;
  int i = 0;
  while (i < argc)
    i = process_one_option(argc, argv, i, &no_more_options);

  if (this->inputs_.in_group())
    {
      fprintf(stderr, _("%s: missing group end\n"), program_name);
      this->usage();
    }

  // FIXME: We should only do this when configured in native mode.
  this->options_.add_to_search_path_with_sysroot("/lib");
  this->options_.add_to_search_path_with_sysroot("/usr/lib");

  this->options_.add_sysroot();

  // Ensure options don't contradict each other and are otherwise kosher.
  this->normalize_options();
}

// Extract an option argument for a special option.  LONGNAME is the
// long name of the option.  This sets *PRET to the return value for
// the special function handler to skip to the next option.

const char*
Command_line::get_special_argument(const char* longname, int argc, char** argv,
				   const char* arg, bool long_option,
				   int *pret)
{
  if (long_option)
    {
      size_t longlen = strlen(longname);
      gold_assert(strncmp(arg, longname, longlen) == 0);
      arg += longlen;
      if (*arg == '=')
	{
	  *pret = 1;
	  return arg + 1;
	}
      else if (argc > 1)
	{
	  gold_assert(*arg == '\0');
	  *pret = 2;
	  return argv[1];
	}
    }
  else
    {
      if (arg[1] != '\0')
	{
	  *pret = 1;
	  return arg + 1;
	}
      else if (argc > 1)
	{
	  *pret = 2;
	  return argv[1];
	}
    }

  this->usage(_("missing argument"), arg);
}

// Ensure options don't contradict each other and are otherwise kosher.

void
Command_line::normalize_options()
{
  if (this->options_.is_shared() && this->options_.is_relocatable())
    gold_fatal(_("-shared and -r are incompatible"));

  if (this->options_.output_format() != General_options::OUTPUT_FORMAT_ELF
      && (this->options_.is_shared() || this->options_.is_relocatable()))
    gold_fatal(_("binary output format not compatible with -shared or -r"));

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
  Input_file_argument file(name, is_lib, "", false, this->position_options_);
  this->inputs_.add_file(file);
}

// Handle the -l option, which requires special treatment.

int
Command_line::process_l_option(int argc, char** argv, char* arg,
			       bool long_option)
{
  int ret;
  const char* libname = this->get_special_argument("library", argc, argv, arg,
						   long_option, &ret);
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
  ::exit(EXIT_FAILURE);
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
