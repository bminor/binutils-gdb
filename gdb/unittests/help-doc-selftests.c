/* Self tests for help doc for GDB, the GNU debugger.

   Copyright (C) 2019 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "cli/cli-cmds.h"
#include "cli/cli-decode.h"
#include "gdbsupport/selftest.h"

namespace selftests {
namespace help_doc_tests {

static unsigned int nr_failed_invariants;

/* Report a broken invariant and increments nr_failed_invariants.  */

static void
broken_doc_invariant (const char *prefix, const char *name, const char *msg)
{
  fprintf_filtered (gdb_stdout,
		    "help doc broken invariant: command '%s%s' help doc %s\n",
		    prefix, name, msg);
  nr_failed_invariants++;
}

/* Recursively walk the commandlist structures, and check doc invariants:
   - The first line of the doc must end with a '.'.
   - the doc must not end with a new line.
  If an invariant is not respected, produce a message and increment
  nr_failed_invariants.
  Note that we do not call SELF_CHECK in this function, as we want
  all commands to be checked before making the test fail.  */

static void
check_doc (struct cmd_list_element *commandlist, const char *prefix)
{
  struct cmd_list_element *c;

  /* Walk through the commands.  */
  for (c = commandlist; c; c = c->next)
    {
      /* Checks the doc has a first line terminated with a '.'.  */
      const char *p = c->doc;

      /* Position p on the first LF, or on terminating null byte.  */
      while (*p && *p != '\n')
	p++;
      if (p == c->doc)
	broken_doc_invariant
	  (prefix, c->name,
	   "is missing the first line terminated with a '.' character");
      else if (*(p-1) != '.')
	broken_doc_invariant
	  (prefix, c->name,
	   "first line is not terminated with a '.' character");

      /* Checks the doc is not terminated with a new line.  */
      if (c->doc[strlen (c->doc) - 1] == '\n')
	broken_doc_invariant
	  (prefix, c->name,
	   "has a superfluous trailing end of line");

      /* Check if this command has subcommands and is not an
	 abbreviation.  We skip checking subcommands of abbreviations
	 in order to avoid duplicates in the output.  */
      if (c->prefixlist != NULL && !c->abbrev_flag)
	{
	  /* Recursively call ourselves on the subcommand list,
	     passing the right prefix in.  */
	  check_doc (*c->prefixlist, c->prefixname);
	}
    }
}

static void
help_doc_invariants_tests ()
{
  nr_failed_invariants = 0;
  check_doc (cmdlist, "");
  SELF_CHECK (nr_failed_invariants == 0);
}

} /* namespace help_doc_tests */
} /* namespace selftests */

void
_initialize_help_doc_selftests ()
{
  selftests::register_test
    ("help_doc_invariants",
     selftests::help_doc_tests::help_doc_invariants_tests);
}
