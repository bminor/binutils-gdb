/* Print and select stack frames for GDB, the GNU debugger.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

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

#include <stdio.h>

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"

START_FILE

/* Thie "selected" stack frame is used by default for local and arg access.
   May be zero, for no selected frame.  */

FRAME selected_frame;

/* Level of the selected frame:
   0 for innermost, 1 for its caller, ...
   or -1 for frame specified by address with no defined level.  */

int selected_frame_level;

/* Nonzero means print the full filename and linenumber
   when a frame is printed, and do so in a format programs can parse.  */

int frame_file_full_name = 0;

static void select_calling_frame ();

void print_frame_info ();

/* Print a stack frame briefly.  FRAME should be the frame address
   and LEVEL should be its level in the stack (or -1 for level not defined).
   This prints the level, the function executing, the arguments,
   and the file name and line number.
   If the pc is not at the beginning of the source line,
   the actual pc is printed at the beginning.

   If SOURCE is 1, print the source line as well.
   If SOURCE is -1, print ONLY the source line.  */

static void
print_stack_frame (frame, level, source)
     FRAME frame;
     int level;
     int source;
{
  struct frame_info fi;

  fi = get_frame_info (frame);

  print_frame_info (&fi, level, source, 1);
}

void
print_frame_info (fi, level, source, args)
     struct frame_info *fi;
     register int level;
     int source;
     int args;
{
  register FRAME frame = fi->frame;
  struct symtab_and_line sal;
  struct symbol *func;
  register char *funname = 0;
  int numargs;

  sal = find_pc_line (fi->pc, fi->next_frame);
  func = get_frame_function (frame);
  if (func)
    funname = SYMBOL_NAME (func);
  else
    {
      register int misc_index = find_pc_misc_function (fi->pc);
      if (misc_index >= 0)
	funname = misc_function_vector[misc_index].name;
    }

  if (source >= 0 || !sal.symtab)
    {
      /* This avoids a bug in cc on the sun.  */
      struct frame_info tem;
      tem = *fi;

      if (level >= 0)
	printf ("#%-2d ", level);
      if (fi->pc != sal.pc || !sal.symtab)
	printf ("0x%x in ", fi->pc);
      printf ("%s (", funname ? funname : "??");
      if (args)
	{
	  FRAME_NUM_ARGS (numargs, tem);
	  print_frame_args (func, FRAME_ARGS_ADDRESS (tem), numargs, stdout);
	}
      printf (")");
      if (sal.symtab)
	printf (" (%s line %d)", sal.symtab->filename, sal.line);
      printf ("\n");
    }

  if (source != 0 && sal.symtab)
    {
      int done = 0;
      int mid_statement = source < 0 && fi->pc != sal.pc;
      if (frame_file_full_name)
	done = identify_source_line (sal.symtab, sal.line, mid_statement);
      if (!done)
	{
	  if (mid_statement)
	    printf ("0x%x\t", fi->pc);
	  print_source_lines (sal.symtab, sal.line, sal.line + 1, 1);
	}
      current_source_line = max (sal.line - 5, 1);
    }
  if (source != 0)
    set_default_breakpoint (1, fi->pc, sal.symtab, sal.line);

  fflush (stdout);
}

/* Call here to print info on selected frame, after a trap.  */

void
print_sel_frame (just_source)
     int just_source;
{
  print_stack_frame (selected_frame, -1, just_source ? -1 : 1);
}

/* Print info on the selected frame, including level number
   but not source.  */

print_selected_frame ()
{
  print_stack_frame (selected_frame, selected_frame_level, 0);
}

/* Print verbosely the selected frame or the frame at address ADDR.
   This means absolutely all information in the frame is printed.  */

static void
frame_info (addr_exp)
     char *addr_exp;
{
  FRAME frame = addr_exp ? parse_and_eval_address (addr_exp) : selected_frame;
  struct frame_info fi;
  struct frame_saved_regs fsr;
  struct symtab_and_line sal;
  struct symbol *func;
  FRAME calling_frame;
  int i, count;
  char *funname = 0;
  int numargs;

  fi = get_frame_info (frame);
  get_frame_saved_regs (&fi, &fsr);
  sal = find_pc_line (fi.pc, fi.next_frame);
  func = get_frame_function (frame);
  if (func)
    funname = SYMBOL_NAME (func);
  else
    {
      register int misc_index = find_pc_misc_function (fi.pc);
      if (misc_index >= 0)
	funname = misc_function_vector[misc_index].name;
    }
  calling_frame = get_prev_frame (frame);

  if (!addr_exp && selected_frame_level >= 0)
    printf ("Stack level %d, frame at 0x%x:\n pc = 0x%x",
	    selected_frame_level, frame, fi.pc);
  else
    printf ("Stack frame at 0x%x:\n pc = 0x%x",
	    frame, fi.pc);

  if (funname)
    printf (" in %s", funname);
  if (sal.symtab)
    printf (" (%s line %d)", sal.symtab->filename, sal.line);
  printf ("; saved pc 0x%x\n", FRAME_SAVED_PC (frame));
  if (calling_frame)
    printf (" called by frame at 0x%x", calling_frame);
  if (fi.next_frame && calling_frame)
    printf (",");
  if (fi.next_frame)
    printf (" caller of frame at 0x%x", fi.next_frame);
  if (fi.next_frame || calling_frame)
    printf ("\n");
  printf (" Arglist at 0x%x,", FRAME_ARGS_ADDRESS (fi));
  FRAME_NUM_ARGS (i, fi);
  if (i < 0)
    printf (" args: ");
  else if (i == 0)
    printf (" no args.");
  else if (i == 1)
    printf (" 1 arg: ");
  else
    printf (" %d args: ", i);

  FRAME_NUM_ARGS (numargs, fi);
  print_frame_args (func, FRAME_ARGS_ADDRESS (fi), numargs, stdout);
  printf ("\n");
  count = 0;
  for (i = 0; i < NUM_REGS; i++)
    if (fsr.regs[i])
      {
	if (count % 4 != 0)
	  printf (", ");
	else
	  {
	    if (count == 0)
	      printf (" Saved registers:");
	    printf ("\n  ");
	  }
	printf ("%s at 0x%x", reg_names[i], fsr.regs[i]);
	count++;
      }
  if (count)
    printf ("\n");
}

/* Print briefly all stack frames or just the innermost COUNT frames.  */

static void
backtrace_command (count_exp)
     char *count_exp;
{
  struct frame_info fi;
  register int count;
  register FRAME frame;
  register int i;

  if (count_exp)
    count = parse_and_eval_address (count_exp);
  else
    count = -1;

  for (i = 0, frame = get_current_frame (), fi = get_frame_info (frame);
       frame && count--;
       i++, fi = get_prev_frame_info (fi.frame), frame = fi.frame)
    {
      QUIT;
      print_frame_info (&fi, i, 0, 1);
    }
}

/* Print the local variables of a block B active in FRAME.  */

static void
print_block_frame_locals (b, frame, stream)
     struct block *b;
     register FRAME frame;
     register FILE *stream;
{
  int nsyms;
  register int i;
  register struct symbol *sym;

  nsyms = BLOCK_NSYMS (b);

  for (i = 0; i < nsyms; i++)
    {
      sym = BLOCK_SYM (b, i);
      if (SYMBOL_CLASS (sym) == LOC_LOCAL
	  || SYMBOL_CLASS (sym) == LOC_REGISTER
	  || SYMBOL_CLASS (sym) == LOC_STATIC)
	{
	  fprintf (stream, "%s = ", SYMBOL_NAME (sym));
	  print_variable_value (sym, frame, stream);
	  fprintf (stream, "\n");
	  fflush (stream);
	}
    }
}

/* Print on STREAM all the local variables in frame FRAME,
   including all the blocks active in that frame
   at its current pc.

   Returns 1 if the job was done,
   or 0 if nothing was printed because we have no info
   on the function running in FRAME.  */

static int
print_frame_local_vars (frame, stream)
     register FRAME frame;
     register FILE *stream;
{
  register struct block *block = get_frame_block (frame);
  if (block == 0)
    return 0;
  while (block != 0)
    {
      print_block_frame_locals (block, frame, stream);
      /* After handling the function's top-level block, stop.
	 Don't continue to its superblock, the block of
	 per-file symbols.  */
      if (BLOCK_FUNCTION (block))
	break;
      block = BLOCK_SUPERBLOCK (block);
    }
  return 1;
}

static void
locals_info ()
{
  print_frame_local_vars (selected_frame, stdout);
}

static int
print_frame_arg_vars (frame, stream)
     register FRAME frame;
     register FILE *stream;
{
  struct symbol *func = get_frame_function (frame);
  register struct block *b;
  int nsyms;
  register int i;
  register struct symbol *sym;

  if (func == 0)
    return 0;

  b = SYMBOL_BLOCK_VALUE (func);
  nsyms = BLOCK_NSYMS (b);

  for (i = 0; i < nsyms; i++)
    {
      sym = BLOCK_SYM (b, i);
      if (SYMBOL_CLASS (sym) == LOC_ARG)
	{
	  fprintf (stream, "%s = ", SYMBOL_NAME (sym));
	  print_variable_value (sym, frame, stream);
	  fprintf (stream, "\n");
	  fflush (stream);
	}
    }

  return 1;
}

static void
args_info ()
{
  print_frame_arg_vars (selected_frame, stdout);
}

/* Select frame FRAME, and note that its stack level is LEVEL.
   LEVEL may be -1 if an actual level number is not known.  */

void
select_frame (frame, level)
     FRAME frame;
     int level;
{
  selected_frame = frame;
  selected_frame_level = level;
}

/* Store the selected frame and its level into *FRAMEP and *LEVELP.  */

void
record_selected_frame (framep, levelp)
     FRAME *framep;
     int *levelp;
{
  *framep = selected_frame;
  *levelp = selected_frame_level;
}

/* Return the symbol-block in which the selected frame is executing.
   Can return zero under various legitimate circumstances.  */

struct block *
get_selected_block ()
{
  if (!have_inferior_p () && !have_core_file_p ())
    return 0;

  if (!selected_frame)
    return get_current_block ();
  return get_frame_block (selected_frame);
}

/* Find a frame a certain number of levels away from FRAME.
   LEVEL_OFFSET_PTR points to an int containing the number of levels.
   Positive means go to earlier frames (up); negative, the reverse.
   The int that contains the number of levels is counted toward
   zero as the frames for those levels are found.
   If the top or bottom frame is reached, that frame is returned,
   but the final value of *LEVEL_OFFSET_PTR is nonzero and indicates
   how much farther the original request asked to go.  */

FRAME
find_relative_frame (frame, level_offset_ptr)
     register FRAME frame;
     register int* level_offset_ptr;
{
  register FRAME prev;
  struct frame_info fi;
  register FRAME frame1, frame2;

  /* Going up is simple: just do get_prev_frame enough times
     or until initial frame is reached.  */
  while (*level_offset_ptr > 0)
    {
      prev = get_prev_frame (frame);
      if (prev == 0)
	break;
      (*level_offset_ptr)--;
      frame = prev;
    }
  /* Going down could be done by iterating get_frame_info to
     find the next frame, but that would be quadratic
     since get_frame_info must scan all the way from the current frame.
     The following algotithm is linear.  */
  if (*level_offset_ptr < 0)
    {
      /* First put frame1 at innermost frame
	 and frame2 N levels up from there.  */
      frame1 = get_current_frame ();
      frame2 = frame1;
      while (*level_offset_ptr < 0 && frame2 != frame)
	{
	  frame2 = get_prev_frame (frame2);
	  (*level_offset_ptr) ++;
	}
      /* Then slide frame1 and frame2 up in synchrony
	 and when frame2 reaches our starting point
	 frame1 must be N levels down from there.  */
      while (frame2 != frame)
	{
	  frame1 = get_prev_frame (frame1);
	  frame2 = get_prev_frame (frame2);
	}
      return frame1;
    }
  return frame;
}

/* The "frame" command.  With no arg, print selected frame briefly.
   With arg LEVEL, select the frame at level LEVEL and print it.
   With arg larger than 100000, use it as address of frame to select.
   If from command file or user-defined command, don't print anything
   if we have an argument.  */

static void
frame_command (level_exp, from_tty)
     char *level_exp;
     int from_tty;
{
  register int i;
  register FRAME frame;
  unsigned int level, level1;

  if (level_exp)
    {
      level1 = level = parse_and_eval_address (level_exp);
      if (level > 100000)
	{
	  select_frame (level, -1);
	  frame_info (0);
	  return;
	}

      frame = find_relative_frame (get_current_frame (), &level1);
      if (level1 != 0)
	error ("Stack level %d is out of range.", level);
      select_frame (frame, level);
      if (! from_tty)
	return;
    }

  print_stack_frame (selected_frame, selected_frame_level, 1);
}

/* Select the frame up one or COUNT stack levels
   from the previously selected frame, and print it briefly.  */

static void
up_command (count_exp)
     char *count_exp;
{
  register FRAME frame;
  int count = 1, count1;
  if (count_exp)
    count = parse_and_eval_address (count_exp);
  count1 = count;
  
  frame = find_relative_frame (selected_frame, &count1);
  if (count1 != 0 && count_exp == 0)
    error ("Initial frame selected; you cannot go up.");
  select_frame (frame, selected_frame_level + count - count1);

  print_stack_frame (selected_frame, selected_frame_level, 1);
}

/* Select the frame down one or COUNT stack levels
   from the previously selected frame, and print it briefly.  */

static void
down_command (count_exp)
     char *count_exp;
{
  register FRAME frame;
  int count = -1, count1;
  if (count_exp)
    count = - parse_and_eval_address (count_exp);
  count1 = count;
  
  frame = find_relative_frame (selected_frame, &count1);
  if (count1 != 0 && count_exp == 0)
    error ("Bottom (i.e., innermost) frame selected; you cannot go down.");
  select_frame (frame, selected_frame_level + count - count1);

  print_stack_frame (selected_frame, selected_frame_level, 1);
}

static void
return_command (retval_exp, from_tty)
     char *retval_exp;
     int from_tty;
{
  struct symbol *thisfun = get_frame_function (selected_frame);

  /* If interactive, require confirmation.  */

  if (from_tty)
    {
      if (thisfun != 0)
	{
	  if (!query ("Make %s return now? ", SYMBOL_NAME (thisfun)))
	    error ("Not confirmed.");
	}
      else
	if (!query ("Make selected stack frame return now? "))
	  error ("Not confirmed.");
    }

  /* Do the real work.  Pop until the specified frame is current.  */

  while (selected_frame != get_current_frame ())
    POP_FRAME;

  /* Then pop that frame.  */

  POP_FRAME;

  /* Compute the return value (if any) and store in the place
     for return values.  */

  if (retval_exp)
    set_return_value (parse_and_eval (retval_exp));

  /* If interactive, print the frame that is now current.  */

  if (from_tty)
    frame_command ("0", 1);
}

static
initialize ()
{
  add_com ("return", class_stack, return_command,
	   "Make selected stack frame return to its caller.\n\
Control remains in the debugger, but when you continue\n\
execution will resume in the frame above the one now selected.\n\
If an argument is given, it is an expression for the value to return.");

  add_com ("up", class_stack, up_command,
	   "Select and print stack frame that called this one.\n\
An argument says how many frames up to go.");

  add_com ("down", class_stack, down_command,
	   "Select and print stack frame called by this one.\n\
An argument says how many frames down to go.");
  add_com_alias ("do", "down", class_stack, 1);

  add_com ("frame", class_stack, frame_command,
	   "Select and print a stack frame.\n\
With no argument, print the selected stack frame.  (See also \"info frame\").\n\
An argument specifies the frame to select.\n\
It can be a stack frame number or the address of the frame.\n\
With argument, nothing is printed if input is coming from\n\
a command file or a user-defined command.");

  add_com_alias ("f", "frame", class_stack, 1);

  add_com ("backtrace", class_stack, backtrace_command,
	   "Print backtrace of all stack frames, or innermost COUNT frames.");
  add_com_alias ("bt", "backtrace", class_stack, 0);
  add_com_alias ("where", "backtrace", class_alias, 0);
  add_info ("stack", backtrace_command,
	    "Backtrace of the stack, or innermost COUNT frames.");
  add_info_alias ("s", "stack", 1);
  add_info ("frame", frame_info,
	    "All about selected stack frame, or frame at ADDR.");
  add_info_alias ("f", "frame", 1);
  add_info ("locals", locals_info,
	    "Local variables of current stack frame.");
  add_info ("args", args_info,
	    "Argument variables of current stack frame.");
}

END_FILE
