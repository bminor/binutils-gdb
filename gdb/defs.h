/* Basic definitions for GDB, the GNU debugger.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#define CORE_ADDR unsigned int

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

extern char *savestring ();
extern char *concat ();
extern char *xmalloc (), *xrealloc ();
extern char *alloca ();
extern int parse_escape ();
extern char *reg_names[];

extern int quit_flag;

extern int immediate_quit;

#define QUIT { if (quit_flag) quit (); }

enum command_class
{
  class_run, class_vars, class_stack, class_files, class_support, class_info,
  class_breakpoint, class_alias, class_obscure, class_user,
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

extern void do_cleanups ();
extern void discard_cleanups ();
extern struct cleanup *make_cleanup ();
extern void free_current_contents ();

/* Structure for saved commands lines
   (for breakpoints, defined commands, etc).  */

struct command_line
{
  struct command_line *next;
  char *line;
};

struct command_line *read_command_lines ();

/* String containing the current directory (what getwd would return).  */

char *current_directory;
