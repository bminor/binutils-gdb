/* TUI support I/O functions.
   Copyright 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
   Contributed by Hewlett-Packard Company.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "defs.h"
#include "terminal.h"
#include "target.h"
#include "event-loop.h"
#include "command.h"
#include "top.h"
#include "readline/readline.h"
#include "tui.h"
#include "tuiData.h"
#include "tuiIO.h"
#include "tuiCommand.h"
#include "tuiWin.h"
#include "tuiGeneralWin.h"
#include "tui-file.h"
#include "ui-out.h"
#include "cli-out.h"
#include <fcntl.h>
#include <signal.h>

/* This file controls the IO interactions between gdb and curses.
   When the TUI is enabled, gdb has two modes a curses and a standard
   mode.

   In curses mode, the gdb outputs are made in a curses command window.
   For this, the gdb_stdout and gdb_stderr are redirected to the specific
   ui_file implemented by TUI.  The output is handled by tui_puts().
   The input is also controlled by curses with tui_getc().  The readline
   library uses this function to get its input.  Several readline hooks
   are installed to redirect readline output to the TUI (see also the
   note below).

   In normal mode, the gdb outputs are restored to their origin, that
   is as if TUI is not used.  Readline also uses its original getc()
   function with stdin.

   Note: the current readline is not clean in its management of the output.
   Even if we install a redisplay handler, it sometimes writes on a stdout
   file.  It is important to redirect every output produced by readline,
   otherwise the curses window will be garbled.  This is implemented with
   a pipe that TUI reads and readline writes to.  A gdb input handler
   is created so that reading the pipe is handled automatically.
   This will probably not work on non-Unix platforms.  The best fix is
   to make readline clean enougth so that is never write on stdout.  */

/* TUI output files.  */
static struct ui_file *tui_stdout;
static struct ui_file *tui_stderr;
static struct ui_out *tui_out;

/* GDB output files in non-curses mode.  */
static struct ui_file *tui_old_stdout;
static struct ui_file *tui_old_stderr;
static struct ui_out *tui_old_uiout;

/* Readline previous hooks.  */
static Function *tui_old_rl_getc_function;
static VFunction *tui_old_rl_redisplay_function;
static VFunction *tui_old_rl_prep_terminal;
static VFunction *tui_old_rl_deprep_terminal;
static int tui_old_readline_echoing_p;

/* Readline output stream.
   Should be removed when readline is clean.  */
static FILE *tui_rl_outstream;
static FILE *tui_old_rl_outstream;
static int tui_readline_pipe[2];

static unsigned int _tuiHandleResizeDuringIO (unsigned int);


/* Print the string in the curses command window.  */
void
tui_puts (const char *string)
{
  static int tui_skip_line = -1;
  char c;
  WINDOW *w;

  w = cmdWin->generic.handle;
  while ((c = *string++) != 0)
    {
      /* Catch annotation and discard them.  We need two \032 and
         discard until a \n is seen.  */
      if (c == '\032')
        {
          tui_skip_line++;
        }
      else if (tui_skip_line != 1)
        {
          tui_skip_line = -1;
          waddch (w, c);
        }
      else if (c == '\n')
        tui_skip_line = -1;
    }
  getyx (w, cmdWin->detail.commandInfo.curLine,
         cmdWin->detail.commandInfo.curch);
  cmdWin->detail.commandInfo.start_line = cmdWin->detail.commandInfo.curLine;

  /* We could defer the following.  */
  wrefresh (w);
  fflush (stdout);
}

/* Readline callback.
   Redisplay the command line with its prompt after readline has
   changed the edited text.  */
static void
tui_redisplay_readline (void)
{
  int prev_col;
  int height;
  int col, line;
  int c_pos;
  int c_line;
  int in;
  WINDOW *w;
  char *prompt;
  int start_line;
  
  prompt = get_prompt ();
  
  c_pos = -1;
  c_line = -1;
  w = cmdWin->generic.handle;
  start_line = cmdWin->detail.commandInfo.start_line;
  wmove (w, start_line, 0);
  prev_col = 0;
  height = 1;
  for (in = 0; prompt && prompt[in]; in++)
    {
      waddch (w, prompt[in]);
      getyx (w, line, col);
      if (col < prev_col)
        height++;
      prev_col = col;
    }
  for (in = 0; in < rl_end; in++)
    {
      unsigned char c;
      
      c = (unsigned char) rl_line_buffer[in];
      if (in == rl_point)
	{
          getyx (w, c_line, c_pos);
	}

      if (CTRL_CHAR (c) || c == RUBOUT)
	{
          waddch (w, '^');
          waddch (w, CTRL_CHAR (c) ? UNCTRL (c) : '?');
	}
      else
	{
          waddch (w, c);
	}
      if (c == '\n')
        {
          getyx (w, cmdWin->detail.commandInfo.start_line,
                 cmdWin->detail.commandInfo.curch);
        }
      getyx (w, line, col);
      if (col < prev_col)
        height++;
      prev_col = col;
    }
  wclrtobot (w);
  getyx (w, cmdWin->detail.commandInfo.start_line,
         cmdWin->detail.commandInfo.curch);
  if (c_line >= 0)
    {
      wmove (w, c_line, c_pos);
      cmdWin->detail.commandInfo.curLine = c_line;
      cmdWin->detail.commandInfo.curch = c_pos;
    }
  cmdWin->detail.commandInfo.start_line -= height - 1;

  wrefresh (w);
  fflush(stdout);
}

/* Readline callback to prepare the terminal.  It is called once
   each time we enter readline.  There is nothing to do in curses mode.  */
static void
tui_prep_terminal (void)
{
}

/* Readline callback to restore the terminal.  It is called once
   each time we leave readline.  There is nothing to do in curses mode.  */
static void
tui_deprep_terminal (void)
{
}

/* Read readline output pipe and feed the command window with it.
   Should be removed when readline is clean.  */
static void
tui_readline_output (int code, gdb_client_data data)
{
  int size;
  char buf[256];

  size = read (tui_readline_pipe[0], buf, sizeof (buf) - 1);
  if (size > 0 && tui_active)
    {
      buf[size] = 0;
      tui_puts (buf);
    }
}

/* Setup the IO for curses or non-curses mode.
   - In non-curses mode, readline and gdb use the standard input and
   standard output/error directly.
   - In curses mode, the standard output/error is controlled by TUI
   with the tui_stdout and tui_stderr.  The output is redirected in
   the curses command window.  Several readline callbacks are installed
   so that readline asks for its input to the curses command window
   with wgetch().  */
void
tui_setup_io (int mode)
{
  extern int readline_echoing_p;
 
  if (mode)
    {
      /* Redirect readline to TUI.  */
      tui_old_rl_redisplay_function = rl_redisplay_function;
      tui_old_rl_deprep_terminal = rl_deprep_term_function;
      tui_old_rl_prep_terminal = rl_prep_term_function;
      tui_old_rl_getc_function = rl_getc_function;
      tui_old_rl_outstream = rl_outstream;
      tui_old_readline_echoing_p = readline_echoing_p;
      rl_redisplay_function = tui_redisplay_readline;
      rl_deprep_term_function = tui_deprep_terminal;
      rl_prep_term_function = tui_prep_terminal;
      rl_getc_function = tui_getc;
      readline_echoing_p = 0;
      rl_outstream = tui_rl_outstream;
      rl_prompt = 0;

      /* Keep track of previous gdb output.  */
      tui_old_stdout = gdb_stdout;
      tui_old_stderr = gdb_stderr;
      tui_old_uiout = uiout;

      /* Reconfigure gdb output.  */
      gdb_stdout = tui_stdout;
      gdb_stderr = tui_stderr;
      gdb_stdlog = gdb_stdout;	/* for moment */
      gdb_stdtarg = gdb_stderr;	/* for moment */
      uiout = tui_out;

      /* Save tty for SIGCONT.  */
      savetty ();
    }
  else
    {
      /* Restore gdb output.  */
      gdb_stdout = tui_old_stdout;
      gdb_stderr = tui_old_stderr;
      gdb_stdlog = gdb_stdout;	/* for moment */
      gdb_stdtarg = gdb_stderr;	/* for moment */
      uiout = tui_old_uiout;

      /* Restore readline.  */
      rl_redisplay_function = tui_old_rl_redisplay_function;
      rl_deprep_term_function = tui_old_rl_deprep_terminal;
      rl_prep_term_function = tui_old_rl_prep_terminal;
      rl_getc_function = tui_old_rl_getc_function;
      rl_outstream = tui_old_rl_outstream;
      readline_echoing_p = tui_old_readline_echoing_p;

      /* Save tty for SIGCONT.  */
      savetty ();
    }
}

#ifdef SIGCONT
/* Catch SIGCONT to restore the terminal and refresh the screen.  */
static void
tui_cont_sig (int sig)
{
  if (tui_active)
    {
      /* Restore the terminal setting because another process (shell)
         might have changed it.  */
      resetty ();

      /* Force a refresh of the screen.  */
      tuiRefreshAll ();

      /* Update cursor position on the screen.  */
      wmove (cmdWin->generic.handle,
             cmdWin->detail.commandInfo.start_line,
             cmdWin->detail.commandInfo.curch);
      wrefresh (cmdWin->generic.handle);
    }
  signal (sig, tui_cont_sig);
}
#endif

/* Initialize the IO for gdb in curses mode.  */
void
tui_initialize_io ()
{
#ifdef SIGCONT
  signal (SIGCONT, tui_cont_sig);
#endif

  /* Create tui output streams.  */
  tui_stdout = tui_fileopen (stdout);
  tui_stderr = tui_fileopen (stderr);
  tui_out = tui_out_new (tui_stdout);

  /* Create the default UI.  It is not created because we installed
     a init_ui_hook.  */
  uiout = cli_out_new (gdb_stdout);

  /* Temporary solution for readline writing to stdout:
     redirect readline output in a pipe, read that pipe and
     output the content in the curses command window.  */
  if (pipe (tui_readline_pipe) != 0)
    {
      fprintf_unfiltered (gdb_stderr, "Cannot create pipe for readline");
      exit (1);
    }
  tui_rl_outstream = fdopen (tui_readline_pipe[1], "w");
  if (tui_rl_outstream == 0)
    {
      fprintf_unfiltered (gdb_stderr, "Cannot redirect readline output");
      exit (1);
    }
  setlinebuf (tui_rl_outstream);

#ifdef O_NONBLOCK
  (void) fcntl (tui_readline_pipe[0], F_SETFL, O_NONBLOCK);
#else
#ifdef O_NDELAY
  (void) fcntl (tui_readline_pipe[0], F_SETFL, O_NDELAY);
#endif
#endif

  add_file_handler (tui_readline_pipe[0], tui_readline_output, 0);
}

/* Get a character from the command window.  This is called from the readline
   package.  */
int
tui_getc (FILE *fp)
{
  int ch;
  WINDOW *w;

  w = cmdWin->generic.handle;

  /* Flush readline output.  */
  tui_readline_output (GDB_READABLE, 0);
  
  ch = wgetch (w);
  ch = _tuiHandleResizeDuringIO (ch);

  /* The \n must be echoed because it will not be printed by readline.  */
  if (ch == '\n')
    {
      /* When hitting return with an empty input, gdb executes the last
         command.  If we emit a newline, this fills up the command window
         with empty lines with gdb prompt at beginning.  Instead of that,
         stay on the same line but provide a visual effect to show the
         user we recognized the command.  */
      if (rl_end == 0)
        {
          wmove (w, cmdWin->detail.commandInfo.curLine, 0);

          /* Clear the line.  This will blink the gdb prompt since
             it will be redrawn at the same line.  */
          wclrtoeol (w);
          wrefresh (w);
          napms (20);
        }
      else
        {
          wmove (w, cmdWin->detail.commandInfo.curLine,
                 cmdWin->detail.commandInfo.curch);
          waddch (w, ch);
        }
    }
  
  if (m_isCommandChar (ch))
    {				/* Handle prev/next/up/down here */
      ch = tuiDispatchCtrlChar (ch);
    }
  
  if (ch == '\n' || ch == '\r' || ch == '\f')
    cmdWin->detail.commandInfo.curch = 0;
#if 0
  else
    tuiIncrCommandCharCountBy (1);
#endif
  if (ch == KEY_BACKSPACE)
    return '\b';
  
  return ch;
}


/* Cleanup when a resize has occured.
   Returns the character that must be processed.  */
static unsigned int
_tuiHandleResizeDuringIO (unsigned int originalCh)
{
  if (tuiWinResized ())
    {
      tuiRefreshAll ();
      dont_repeat ();
      tuiSetWinResizedTo (FALSE);
      return '\n';
    }
  else
    return originalCh;
}
