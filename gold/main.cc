// main.cc -- gold main function.

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

#ifdef HAVE_MALLINFO
#include <malloc.h>
#endif
#include "libiberty.h"

#include "script.h"
#include "options.h"
#include "parameters.h"
#include "errors.h"
#include "dirsearch.h"
#include "workqueue.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"

using namespace gold;

// This function emits the commandline to a hard-coded file in temp.
// This is useful for debugging since ld is typically invoked by gcc,
// so its commandline is not always easy to extract.  You should be
// able to run 'gcc -B... foo.o -o foo' to invoke this linker the
// first time, and then /tmp/ld-run-foo.sh to invoke it on subsequent
// runes.  "/tmp/ld-run-foo.sh debug" will run the linker inside gdb
// (or whatever value the environment variable GDB is set to), for
// even easier debugging.  Since this is a debugging-only tool, and
// creates files, it is only turned on when the user explicitly asks
// for it, by compiling with -DDEBUG.  Do not do this for release
// versions of the linker!

#ifdef DEBUG
#include <stdio.h>
#include <sys/stat.h>    // for chmod()

static std::string
collect_argv(int argc, char** argv)
{
  // This is used by write_debug_script(), which wants the unedited argv.
  std::string args;
  for (int i = 0; i < argc; ++i)
    {
      args.append(" '");
      // Now append argv[i], but with all single-quotes escaped
      const char* argpos = argv[i];
      while (1)
        {
          const int len = strcspn(argpos, "'");
          args.append(argpos, len);
          if (argpos[len] == '\0')
            break;
          args.append("'\"'\"'");
          argpos += len + 1;
        }
      args.append("'");
    }
  return args;
}

static void
write_debug_script(std::string filename_str,
		   const char* argv_0, const char* args)
{
  size_t slash = filename_str.rfind('/');
  if (slash != std::string::npos)
    filename_str = filename_str.c_str() + slash + 1;
  filename_str = std::string("/var/tmp/ld-run-") + filename_str + ".sh";
  const char* filename = filename_str.c_str();
  FILE* fp = fopen(filename, "w");
  if (fp)
    {
      fprintf(fp, "[ \"$1\" = debug ] && PREFIX=\"${GDB-/home/build/static/projects/tools/gdb} --annotate=3 --fullname %s --args\" && shift\n", argv_0);
      fprintf(fp, "$PREFIX%s $*\n", args);
      fclose(fp);
      chmod(filename, 0755);
    }
  else
    filename = "[none]";
  fprintf(stderr, "Welcome to gold!  Commandline written to %s.\n", filename);
  fflush(stderr);
}

#else // !defined(DEBUG)

static inline std::string
collect_argv(int, char**)
{
  return "";
}

static inline void
write_debug_script(std::string, const char*, const char*)
{
}

#endif // !defined(DEBUG)


int
main(int argc, char** argv)
{
#if defined (HAVE_SETLOCALE) && defined (HAVE_LC_MESSAGES)
  setlocale (LC_MESSAGES, "");
#endif
#if defined (HAVE_SETLOCALE)
  setlocale (LC_CTYPE, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  program_name = argv[0];

  // This is used by write_debug_script(), which wants the unedited argv.
  std::string args = collect_argv(argc, argv);

  Errors errors(program_name);

  // Initialize the global parameters, to let random code get to the
  // errors object.
  initialize_parameters(&errors);

  // Options which may be set by the command line or by linker
  // scripts.
  Script_options script_options;

  // Handle the command line options.
  Command_line command_line(&script_options);
  command_line.process(argc - 1, argv + 1);

  long start_time = 0;
  if (command_line.options().print_stats())
    start_time = get_run_time();

  // Store some options in the globally accessible parameters.
  set_parameters_from_options(&command_line.options());

  // Do this as early as possible (since it prints a welcome message).
  write_debug_script(command_line.options().output_file_name(),
                     program_name, args.c_str());

  // The work queue.
  Workqueue workqueue(command_line.options());

  // The list of input objects.
  Input_objects input_objects;

  // The symbol table.  We're going to guess here how many symbols
  // we're going to see based on the number of input files.  Even when
  // this is off, it means at worst we don't quite optimize hashtable
  // resizing as well as we could have (perhap using more memory).
  Symbol_table symtab(command_line.number_of_input_files() * 1024,
                      command_line.options().version_script());

  // The layout object.
  Layout layout(command_line.options(), &script_options);

  // Get the search path from the -L options.
  Dirsearch search_path;
  search_path.initialize(&workqueue, &command_line.options().search_path());

  // Queue up the first set of tasks.
  queue_initial_tasks(command_line.options(), search_path,
		      command_line, &workqueue, &input_objects,
		      &symtab, &layout);

  // Run the main task processing loop.
  workqueue.process(0);

  if (command_line.options().print_stats())
    {
      long run_time = get_run_time() - start_time;
      fprintf(stderr, _("%s: total run time: %ld.%06ld seconds\n"),
	      program_name, run_time / 1000000, run_time % 1000000);
#ifdef HAVE_MALLINFO
      struct mallinfo m = mallinfo();
      fprintf(stderr, _("%s: total space allocated by malloc: %d bytes\n"),
	      program_name, m.arena);
#endif
      File_read::print_stats();
      fprintf(stderr, _("%s: output file size: %lld bytes\n"),
	      program_name, static_cast<long long>(layout.output_file_size()));
      symtab.print_stats();
      layout.print_stats();
    }

  gold_exit(errors.error_count() == 0);
}
