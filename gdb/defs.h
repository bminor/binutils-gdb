/* Basic definitions for GDB, the GNU debugger.
   Copyright (C) 1986, 1989, 1991 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (DEFS_H)
#define DEFS_H

/* An address in the program being debugged.  Host byte order.  */
typedef unsigned int CORE_ADDR;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/* The character C++ uses to build identifiers that must be unique from
   the program's identifiers (such as $this and $$vptr).  */
#define CPLUS_MARKER '$'	/* May be overridden to '.' for SysV */

/*
 * Allow things in gdb to be declared "const".  If compiling ANSI, it
 * just works.  If compiling with gcc but non-ansi, redefine to __const__.
 * If non-ansi, non-gcc, then eliminate "const" entirely, making those
 * objects be read-write rather than read-only.
 */
#ifndef __STDC__
# ifdef __GNUC__
#  define const __const__
#  define volatile __volatile__
# else
#  define const /*nothing*/
#  define volatile /*nothing*/
# endif /* GNUC */
#endif /* STDC */

extern char *savestring ();
extern char *strsave ();
extern char *concat ();
#ifdef __STDC__
extern void *xmalloc (), *xrealloc ();
#else
extern char *xmalloc (), *xrealloc ();
#endif
extern void free ();
extern int parse_escape ();
extern char *reg_names[];
/* Indicate that these routines do not return to the caller.  */
extern volatile void error(), fatal();

/* Various possibilities for alloca.  */
#ifdef __GNUC__
# define alloca __builtin_alloca
#else
# ifdef sparc
#  include <alloca.h>
# endif
  extern char *alloca ();
# endif

extern int errno;			/* System call error return status */

extern int quit_flag;
extern int immediate_quit;
extern void quit ();

#define QUIT { if (quit_flag) quit (); }

/* Notes on classes: class_alias is for alias commands which are not
   abbreviations of the original command.  */

enum command_class
{
  /* Special args to help_list */
  all_classes = -2, all_commands = -1,
  /* Classes of commands */
  no_class = -1, class_run = 0, class_vars, class_stack,
  class_files, class_support, class_info, class_breakpoint,
  class_alias, class_obscure, class_user
};

/* the cleanup list records things that have to be undone
   if an error happens (descriptors to be closed, memory to be freed, etc.)
   Each link in the chain records a function to call and an
   argument to give it.

   Use make_cleanup to add an element to the cleanup chain.
   Use do_cleanups to do all cleanup actions back to a given
   point in the chain.  Use discard_cleanups to remove cleanups
   from the chain back to a given point, not doing them.  */

struct cleanup
{
  struct cleanup *next;
  void (*function) ();
  int arg;
};

/* From utils.c.  */
extern void do_cleanups ();
extern void discard_cleanups ();
extern struct cleanup *make_cleanup ();
extern struct cleanup *save_cleanups ();
extern void restore_cleanups ();
extern void free_current_contents ();
extern int myread ();
extern int query ();
extern int lines_to_list ();
extern void reinitialize_more_filter ();
extern void fputs_filtered ();
extern void puts_filtered ();
extern void fprintf_filtered ();
extern void printf_filtered ();
extern void print_spaces ();
extern void print_spaces_filtered ();
extern char *n_spaces ();
extern void printchar ();
extern void fprint_symbol ();
extern void fputs_demangled ();
extern void perror_with_name ();
extern void print_sys_errmsg ();

/* From printcmd.c */
extern void print_address_symbolic ();
extern void print_address ();

/* From readline (but not in any readline .h files).  */
extern char *tilde_expand ();

/* Structure for saved commands lines
   (for breakpoints, defined commands, etc).  */

struct command_line
{
  struct command_line *next;
  char *line;
};

extern struct command_line *read_command_lines ();
extern void free_command_lines ();

/* String containing the current directory (what getwd would return).  */

char *current_directory;

/* Default radixes for input and output.  Only some values supported.  */
extern unsigned input_radix;
extern unsigned output_radix;

/* Baud rate specified for communication with serial target systems.  */
char *baud_rate;

#if !defined (UINT_MAX)
#define UINT_MAX 0xffffffff
#endif

#if !defined (LONG_MAX)
#define LONG_MAX 0x7fffffff
#endif

/* Just like CHAR_BIT in <limits.h> but describes the target machine.  */
#if !defined (TARGET_CHAR_BIT)
#define TARGET_CHAR_BIT 8
#endif

#endif /* no DEFS_H */
