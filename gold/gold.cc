// ld.c -- linker main function

#include "gold.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "options.h"
#include "workqueue.h"
#include "dirsearch.h"
#include "readsyms.h"
#include "symtab.h"
#include "layout.h"

namespace gold
{

const char* program_name;

void
gold_exit(bool status)
{
  exit(status ? EXIT_SUCCESS : EXIT_FAILURE);
}

void
gold_fatal(const char* msg, bool perrno)
{
  fprintf(stderr, "%s: ", program_name);
  if (perrno)
    perror(msg);
  else
    fprintf(stderr, "%s\n", msg);
  gold_exit(false);
}

void
gold_nomem()
{
  // We are out of memory, so try hard to print a reasonable message.
  // Note that we don't try to translate this message, since the
  // translation process itself will require memory.
  write(2, program_name, strlen(program_name));
  const char* const s = ": out of memory\n";
  write(2, s, strlen(s));
  gold_exit(false);
}

void
gold_unreachable()
{
  abort();
}

} // End namespace gold.

namespace
{

using namespace gold;

// Queue up the initial set of tasks for this link job.

void
queue_initial_tasks(const General_options& options,
		    const Dirsearch& search_path,
		    const Command_line::Input_argument_list& inputs,
		    Workqueue* workqueue, Object_list* input_objects,
		    Symbol_table* symtab)
{
  if (inputs.empty())
    gold_fatal(_("no input files"), false);

  // Read the input files.  We have to add the symbols to the symbol
  // table in order.  We do this by creating a separate blocker for
  // each input file.  We associate the blocker with the following
  // input file, to give us a convenient place to delete it.
  Task_token* this_blocker = NULL;
  for (Command_line::Input_argument_list::const_iterator p = inputs.begin();
       p != inputs.end();
       ++p)
    {
      Task_token* next_blocker = new Task_token();
      next_blocker->add_blocker();
      workqueue->queue(new Read_symbols(options, input_objects, symtab,
					search_path, *p, this_blocker,
					next_blocker));
      this_blocker = next_blocker;
    }

  workqueue->queue(new Layout_task(options, input_objects, this_blocker));
}

} // end anonymous namespace.

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

  gold::program_name = argv[0];

  // Handle the command line options.
  gold::Command_line command_line;
  command_line.process(argc - 1, argv + 1);

  // The work queue.
  gold::Workqueue workqueue(command_line.options());

  // The list of input objects.
  Object_list input_objects;

  // The symbol table.
  Symbol_table symtab;

  // Get the search path from the -L options.
  Dirsearch search_path;
  search_path.add(&workqueue, command_line.options().search_path());

  // Queue up the first set of tasks.
  queue_initial_tasks(command_line.options(), search_path,
		      command_line.inputs(), &workqueue, &input_objects,
		      &symtab);

  // Run the main task processing loop.
  workqueue.process();

  gold::gold_exit(true);
}
