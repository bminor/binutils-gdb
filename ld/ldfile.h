/* ldfile.h -
   Copyright 1991, 1992 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

extern char *ldfile_input_filename;
extern unsigned long ldfile_output_machine;
extern enum bfd_architecture ldfile_output_architecture;
extern const char *ldfile_output_machine_name;

/* Structure used to hold the list of directories to search for
   libraries.  */

typedef struct search_dirs 
{
  /* Next directory on list.  */
  struct search_dirs *next;
  /* Name of directory.  */
  const char *name;
  /* true if this is from the command line.  */
  boolean cmdline;
} search_dirs_type;

extern search_dirs_type *search_head;

void ldfile_add_arch PARAMS ((CONST char *));
void ldfile_add_library_path PARAMS ((const char *, boolean cmdline));
void ldfile_open_command_file PARAMS ((char *name));
void ldfile_open_file PARAMS ((struct lang_input_statement_struct *));
FILE *ldfile_find_command_file PARAMS ((char *name, char *extend));

void ldfile_set_output_arch PARAMS ((CONST char *));
