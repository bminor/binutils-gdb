/*  This file is part of the program psim.

    Copyright (C) 1994-1995,1997 Andrew Cagney <cagney@highland.com.au>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include "config.h"
#include "misc.h"
#include "lf.h"
#include "table.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

struct _table {
  size_t size;
  char *buffer;
  char *pos;
  int nr_fields;
  int nr_model_fields;
  int line_nr;
  char *file_name;
  int current_file_line_offset;
  char *current_file_name;
};

extern table *
table_open(const char *file_name,
	   int nr_fields,
	   int nr_model_fields)
{
  int fd;
  struct stat stat_buf;
  table *file;

  /* create a file descriptor */
  file = ZALLOC(table);
  ASSERT(file != NULL);
  file->nr_fields = nr_fields;
  file->nr_model_fields = nr_model_fields;

  /* save the file name */
  file->file_name = (char*)zalloc(strlen(file_name) + 1);
  ASSERT(file->file_name != NULL);
  strcpy(file->file_name, file_name);
  file->current_file_name = file->file_name;

  /* open the file */
  fd = open(file->file_name, O_RDONLY, 0);
  if (fd < 0) {
    perror(file->file_name);
    exit (1);
  }

  /* determine the size */
  if (fstat(fd, &stat_buf) < 0) {
    perror("table_open.fstat");
    exit(1);
  }
  file->size = stat_buf.st_size;

  /* allocate this much memory */
  file->buffer = (char*)zalloc(file->size+1);
  ASSERT(file->buffer != NULL);
  file->pos = file->buffer;

  /* read it in */
  if (read(fd, file->buffer, file->size) < file->size) {
    perror(file->file_name);
    exit(1);
  }
  file->buffer[file->size] = '\0';

  /* set the initial line numbering */
  file->line_nr = 0;
  file->current_file_line_offset = 0;

  /* done */
  close(fd);
  return file;
}


extern table_entry *
table_entry_read(table *file)
{
  int field;
  table_entry *entry;

  /* skip comments/blanks */
  while(1) {
    /* leading white space */
    while (*file->pos != '\0'
	   && *file->pos != '\n'
	   && isspace(*file->pos))
      file->pos++;
    /* cpp line nr directive - # <line-nr> "<file>" */
    if (file->pos[0] == '#'
	&& file->pos[1] == ' '
	&& isdigit(file->pos[2])) {
      file->pos += strlen("# ");
      /* parse the number */
      file->current_file_line_offset = atoi(file->pos) - file->line_nr - 2;
      /* skip to the file name */
      while (file->pos[0] != '0'
	     && file->pos[0] != '"'
	     && file->pos[0] != '\0')
	file->pos++;
      if (file->pos[0] != '"') {
	error("%s:%d: Missing opening quote",
	      file->file_name,
	      file->line_nr);
      }
      /* parse the file name */
      file->pos++;
      file->current_file_name = file->pos;
      while (file->pos[0] != '"'
	     && file->pos[0] != '\0')
	file->pos++;
      if (file->pos[0] != '"') {
	error("%s:%d: Missing closing quote",
	      file->file_name,
	      file->line_nr);
      }
      file->pos[0] = '\0';
      file->pos ++;
      while (file->pos[0] != '\0'
	     && file->pos[0] != '\n')
	file->pos[0]++;
      if (file->pos[0] != '\n')
	error("%s:%d: Missing newline",
	      file->file_name,
	      file->line_nr);
    }
    /* comment - leading // or # - skip */
    else if ((file->pos[0] == '/' && file->pos[1] == '/')
	     || (file->pos[0] == '#')) {
      do {
	file->pos++;
      } while (*file->pos != '\0' && *file->pos != '\n');
    }
    /* end of line? */
    if (*file->pos == '\n') {
      file->pos++;
      file->line_nr++;
    }
    else
      break;
  }
  if (*file->pos == '\0')
    return NULL;

  /* create this new entry */
  entry = (table_entry*)zalloc(sizeof(table_entry)
			       + (file->nr_fields + 1) * sizeof(char*));
  ASSERT(entry != NULL);
  entry->file_name = file->current_file_name;
  entry->nr_fields = file->nr_fields;

  /* break the line into its colon delimitered fields */
  for (field = 0; field < file->nr_fields-1; field++) {
    entry->fields[field] = file->pos;
    while(*file->pos && *file->pos != ':' && *file->pos != '\n')
      file->pos++;
    if (*file->pos == ':') {
      *file->pos = '\0';
      file->pos++;
    }
  }

  /* any trailing stuff not the last field */
  ASSERT(field == file->nr_fields-1);
  entry->fields[field] = file->pos;
  while (*file->pos && *file->pos != '\n') {
    file->pos++;
  }
  if (*file->pos == '\n') {
    *file->pos = '\0';
    file->pos++;
  }
  file->line_nr++;

  /* If following lines being with a double quote ("), add them to the
     list of assembler lines */
  {
    table_assembler_entry **current = &entry->assembler;
    while (*file->pos == '"') {
      char *tmpchp;
      const char *format;
      int strlen_format;
      const char *condition;
      int strlen_condition;

      /* skip over the format string */
      format = file->pos;
      strlen_format = 0;
      do {
	if (file->pos[0] == '\\' && file->pos[1] == '"')
	  file->pos += 2;
	else
	  file->pos += 1;
      } while (*file->pos != '\0' && *file->pos != '\n' && *file->pos != '"');
      if (*file->pos != '"')
	error ("%s:%d: Missing closing quote in assembler line",
	       file->file_name,
	       file->line_nr);
      file->pos++;
      strlen_format = file->pos - format;

      /* skip over the boolean condition */
      condition = NULL;
      strlen_condition = 0;
      if (*file->pos == ':')
	{
	  file->pos++;
	  while (isspace(*file->pos) && *file->pos != '\0' && *file->pos != '\n')
	    file->pos++;
	  condition = file->pos;
	  while (*file->pos != '\0' && *file->pos != '\n')
	    file->pos++;
	  strlen_condition = file->pos - condition;
	}

      /* create the new assembler entry */
      *current = ZALLOC (table_assembler_entry);
      tmpchp = zalloc (strlen_format + 1);
      strncpy (tmpchp, format, strlen_format);
      (*current)->format = tmpchp;
      (*current)->file_name = file->file_name;
      (*current)->line_nr = file->line_nr;
      if (condition != NULL && strlen_condition > 0)
	{
	  tmpchp = zalloc (strlen_condition + 1);
	  strncpy (tmpchp, condition, strlen_condition);
	  (*current)->condition = tmpchp;
	}
      current = &(*current)->next;

      /* end of line? */
      if (*file->pos != '\n')
	error ("%s:%d: Missing eoln in assembler line",
	       file->file_name,
	       file->line_nr);
      file->pos++;
      file->line_nr++;
    }
  }

  /* if following lines begin with a star, add them to the model
     section.  */
  while ((file->nr_model_fields > 0) && (*file->pos == '*')) {
    table_model_entry *model = (table_model_entry*)zalloc(sizeof(table_model_entry)
							  + (file->nr_model_fields + 1) * sizeof(char*));
    if (entry->model_last)
      entry->model_last->next = model;
    else
      entry->model_first = model;
    entry->model_last = model;

    /* break the line into its colon delimitered fields */
    file->pos++;
    for (field = 0; field < file->nr_model_fields-1; field++) {
      model->fields[field] = file->pos;
      while(*file->pos && *file->pos != ':' && *file->pos != '\n')
	file->pos++;
      if (*file->pos == ':') {
	*file->pos = '\0';
	file->pos++;
      }
    }

    /* any trailing stuff not the last field */
    ASSERT(field == file->nr_model_fields-1);
    model->fields[field] = file->pos;
    while (*file->pos && *file->pos != '\n') {
      file->pos++;
    }
    if (*file->pos == '\n') {
      *file->pos = '\0';
      file->pos++;
    }

    file->line_nr++;
    model->line_nr = file->current_file_line_offset + file->line_nr;
  }

  entry->line_nr = file->current_file_line_offset + file->line_nr;

  /* if following lines are tab indented, put in the annex */
  if (*file->pos == '\t') {
    entry->annex = file->pos;
    do {
      do {
	file->pos++;
      } while (*file->pos != '\0' && *file->pos != '\n');
      if (*file->pos == '\n') {
	char *save_pos = ++file->pos;
	int extra_lines = 0;
	file->line_nr++;
	/* Allow tab indented to have blank lines */
	while (*save_pos == '\n') {
	  save_pos++;
	  extra_lines++;
	}
	if (*save_pos == '\t') {
	  file->pos = save_pos;
	  file->line_nr += extra_lines;
	}
      }
    } while (*file->pos != '\0' && *file->pos == '\t');
    if (file->pos[-1] == '\n')
      file->pos[-1] = '\0';
  }
  else
    entry->annex = NULL;

  /* return it */
  return entry;

}


extern void
dump_table_entry(table_entry *entry,
		 int indent)
{
  printf("(table_entry*)%p\n", entry);

  if (entry != NULL) {
    int field;
    char sep;

    sep = ' ';
    dumpf(indent, "(fields");
    for (field = 0; field < entry->nr_fields; field++) {
      printf("%c%s", sep, entry->fields[field]);
      sep = ':';
    }
    printf(")\n");

    dumpf(indent, "(line_nr %d)\n", entry->line_nr);

    dumpf(indent, "(file_name %s)\n", entry->file_name);

    dumpf(indent, "(annex\n%s\n", entry->annex);
    dumpf(indent, " )\n");

  }
}


extern void
table_entry_print_cpp_line_nr(lf *file,
			      table_entry *entry)
{
  lf_print__external_reference(file, entry->line_nr, entry->file_name);
}


