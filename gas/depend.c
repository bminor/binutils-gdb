/* depend.c - Handle dependency tracking.
   Copyright (C) 1997 Free Software Foundation, Inc.

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
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "as.h"

/* The file to write to, or NULL if no dependencies being kept.  */
static char *dep_file = NULL;

struct dependency
{
  char *file;
  struct dependency *next;
};

/* All the files we depend on.  */
static struct dependency *dep_chain = NULL;

/* Current column in output file.  */
static int column = 0;

static void wrap_output PARAMS ((FILE *, char *, int));

/* Number of columns allowable.  */
#define MAX_COLUMNS 72



/* Start saving dependencies, to be written to FILENAME.  If this is
   never called, then dependency tracking is simply skipped.  */

void
start_dependencies (filename)
     char *filename;
{
  dep_file = filename;
}

/* Noticed a new filename, so try to register it.  */

void
register_dependency (filename)
     char *filename;
{
  struct dependency *dep;

  if (dep_file == NULL)
    return;

  for (dep = dep_chain; dep != NULL; dep = dep->next)
    {
      if (! strcmp (filename, dep->file))
	return;
    }

  dep = (struct dependency *) xmalloc (sizeof (struct dependency));
  dep->file = xstrdup (filename);
  dep->next = dep_chain;
  dep_chain = dep;
}

/* Append some output to the file, keeping track of columns and doing
   wrapping as necessary.  */

static void
wrap_output (f, string, spacer)
     FILE *f;
     char *string;
     int spacer;
{
  int len = strlen (string);

  if (len == 0)
    return;

  if (column && MAX_COLUMNS - 1 /*spacer*/ - 2 /*` \'*/ < column + len)
    {
      fprintf (f, " \\\n ");
      column = 0;
      if (spacer == ' ')
	spacer = '\0';
    }

  if (spacer == ' ')
    {
      putc (spacer, f);
      ++column;
    }

  fputs (string, f);
  column += len;

  if (spacer == ':')
    {
      putc (spacer, f);
      ++column;
    }
}

/* Print dependency file.  */

void
print_dependencies ()
{
  FILE *f;
  struct dependency *dep;

  if (dep_file == NULL)
    return;

  f = fopen (dep_file, "w");
  if (f == NULL)
    {
      as_warn ("Can't open `%s' for writing", dep_file);
      return;
    }

  column = 0;
  wrap_output (f, out_file_name, ':');
  for (dep = dep_chain; dep != NULL; dep = dep->next)
    wrap_output (f, dep->file, ' ');

  putc ('\n', f);

  if (fclose (f))
    as_warn ("Can't close %s", dep_file);
}
