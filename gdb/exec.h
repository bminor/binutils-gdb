/* Work with executable files, for GDB, the GNU debugger.

   Copyright (C) 2003, 2007, 2008 Free Software Foundation, Inc.

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

#ifndef EXEC_H
#define EXEC_H

#include "target.h"
#include "vec.h"

struct section_table;
struct target_ops;
struct bfd;
struct objfile;
struct inferior;

extern struct target_ops exec_ops;

/* This object represents an executable program. GDB can handle
   arbitrarily many, although the traditional debugging scenario just
   involves one. An executable always has a BFD managing the file
   itself, but may or may not have an associated symbol table.  */

struct exec
  {
    /* The full name of the executable.  */
    char *name;

    /* An abbreviated name, typically formed by pruning the directory
       from the full name.  */
    char *shortname;

    /* The BFD for this executable.  */
    bfd *ebfd;

    /* The main objfile for this executable.  */
    struct objfile *objfile;

    /* The last-modified time, from when the exec was brought in.  */
    long ebfd_mtime;

    /* The "proto-inferior" that we use when setting things up.  */
    struct inferior *inferior;

    /* Pointers to section table for this executable.  */
    struct section_table *sections;
    struct section_table *sections_end;
  };

typedef struct exec *exec_p;
DEF_VEC_P(exec_p);

extern VEC(exec_p) *execs;

extern struct exec *current_exec;

extern struct exec *first_exec;

/* Builds a section table, given args BFD, SECTABLE_PTR, SECEND_PTR.
   Returns 0 if OK, 1 on error.  */

extern int build_section_table (struct bfd *, struct section_table **,
				struct section_table **);

/* Set the loaded address of a section.  */
extern void exec_set_section_address (const char *, int, CORE_ADDR);

/* Return the first exec whose name matches the given string.  */
extern struct exec *find_exec_by_name (char *name);

/* Return the first exec whose name matches the chars between the
   given bounds.  */
extern struct exec *find_exec_by_substr (char *name, char *name_end);

/* Return the number of executables present.  */
extern int number_of_execs ();

extern void exec_file_update (struct exec *exec);

extern struct exec *create_exec (char *filename, bfd *abfd, long mtime);

extern void set_current_exec (struct exec *exec);

extern void maintenance_print_execs (char *, int);

#endif
