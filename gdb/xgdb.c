/* Interface from GDB to X windows.
   Copyright (C) 1987 Free Software Foundation, Inc.

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

/* Original version was contributed by Derek Beatty, 30 June 87.  */

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Label.h>
#include <X11/Command.h>
#include <X11/TextP.h>
#include <X11/Box.h>
#include <X11/VPaned.h>

#include <stdio.h>

/* Cursor used in GDB window.  */

#define gdb_width 16
#define gdb_height 16
#define gdb_x_hot 7
#define gdb_y_hot 0
static short gdb_bits[] = {
   0x0000, 0x0140, 0x0220, 0x0220,
   0x23e2, 0x13e4, 0x09c8, 0x0ff8,
   0x0220, 0x3ffe, 0x0630, 0x03e0,
   0x0220, 0x1ffc, 0x2632, 0x01c0};

#define gdb_mask_width 16
#define gdb_mask_height 16
#define gdb_mask_x_hot 7
#define gdb_mask_y_hot 0
static short gdb_mask_bits[] = {
   0x0360, 0x07f0, 0x07f0, 0x77f7,
   0x7fff, 0x7fff, 0x1ffc, 0x1ffc,
   0x7fff, 0x7fff, 0x7fff, 0x0ff8,
   0x3ffe, 0x7fff, 0x7fff, 0x7fff};

/* The X display on which the window appears.  */

static Display *screen_display;

/* Windows manipulated by this package.  */

static Widget main_widget;
static Widget containing_widget;
static Widget title_widget;
static Widget source_name_widget;
static Widget source_text_widget;
static Widget exec_name_widget;
static Widget button_box_widget;

#ifdef VTFD
/* Interaction Window */

static Widget interactive_widget;
XtTextSource PseudoDiskSourceCreate();
XtTextSource TSource;
static int vtfd[2], vifd[2];
#endif

/* Source text display.  */

static struct symtab *source_window_symtab = 0;

/* Forward declarations */

static Widget create_text_widget ();

START_FILE

/* Return number of text lines displayed in text widget W.  */

int /* was XtTextPosition */
XtTextLines (w)
     Widget w;
{
  TextWidget ctx = (TextWidget)w;

  return ctx->text.lt.lines;
}

/* Display an appropriate piece of source code in the source window.  */

xgdb_display_source ()
{
  char *filename;
  Arg args[1];
  Arg labelArgs[1];
  int linenumbers_changed = 0;
  int must_scroll = 0;
  int height = XtTextLines (source_text_widget);

  struct symtab_and_line get_selected_frame_sal ();
  struct symtab_and_line sal;
  struct frame_info fi;

  /* Do nothing if called before we are initialized */

  if (!containing_widget) return;

  /* Get the symtab and line number of the selected frame.  */

  fi = get_frame_info (selected_frame);
  sal = find_pc_line (fi.pc, fi.next_frame);

  /* Strictly this is wrong, but better than a blank display */

  if (sal.symtab == NULL) 
    {
      sal.symtab = current_source_symtab;
      /* current_source_line may be off by a small number like 4 */
      sal.line = current_source_line;
    }

  /* Do a path search and get the exact filename of this source file.
     Also scan it and find its source lines if not already done.  */

  if (sal.symtab)
    linenumbers_changed = get_filename_and_charpos (sal.symtab, sal.line,
						    &filename);

  if (!filename) sal.symtab = NULL;

  /* If the source window may be wrong, destroy it (and make a new one).  */

  if (linenumbers_changed || source_window_symtab != sal.symtab)
    {
      Arg fileArgs[1];
      XtTextSource src;

      must_scroll = 1;
      source_window_symtab = sal.symtab;

      src = XtTextGetSource (source_text_widget);
      XtDiskSourceDestroy (src);
      
      XtSetArg (fileArgs[0], XtNfile, filename);
      src = XtDiskSourceCreate (source_text_widget->core.parent, fileArgs, 1);
      XtTextSetSource (source_text_widget, src, 0);

      XtSetArg (labelArgs[0], XtNlabel,
		filename ? filename : "No source displayed.");
      XtSetValues (source_name_widget, labelArgs, XtNumber (labelArgs));
      if (filename) free (filename);
    }

  /* Update display and cursor positions as necessary.
     Cursor should be placed on line sal.line.  */

  /* Find out where the display is positioned (in case user scrolled it).  */

  if (! must_scroll)
    {
      int top_line_number;

      XtSetArg (args[0], XtNdisplayPosition, NULL);
      XtGetValues (source_text_widget, args, 1);
      top_line_number = source_charpos_line (source_window_symtab,
					     (int) args[0].value);
      /* If desired position is off screen, we must scroll.  */
      if (sal.line < top_line_number
	  || sal.line >= top_line_number + height)
	must_scroll = 1;
    }

  /* If appropriate, scroll the text display.  */

  if (must_scroll)
    {
      int top_line_number = (sal.line > height/3) ? sal.line - height/3 : 1;

      XtSetArg (args[0], XtNdisplayPosition,
		source_line_charpos (source_window_symtab, top_line_number));
      XtSetValues (source_text_widget, args, 1);
    }

  /* Set the text display cursor position within the text.  */

  XtSetArg (args[0], XtNinsertPosition, 
	    source_line_charpos (source_window_symtab, sal.line));
  XtSetValues (source_text_widget, args, 1);
}

/* Display FILENAME in the title bar at bottom of window.  */

xgdb_display_exec_file (filename)
     char *filename;
{
  static Arg labelArgs[1];

  XtSetArg (labelArgs[0], XtNlabel, filename);
  XtSetValues (exec_name_widget, labelArgs, XtNumber (labelArgs));
}

/* Do any necessary prompting, etc.  */

static char *prompt_string;

static void
print_prompt ()
{
  if (prompt_string)
    {
      printf ("%s", prompt_string);
      fflush (stdout);
    }
}

/* Handlers for buttons.  */

/* Subroutine used by "print" and "print*" buttons.
   STARFLAG is 1 for print*, 0 for print.
   Get the "selection" from X and use it as the operand of a print command.  */

static void
print_button (w, starflag, call_data)
     Widget w;
     int starflag;
     caddr_t call_data;
{
  int selected_length;
  char *selected_text;
  char *cmd = starflag ? "print * " : "print ";
  register int cmdlen = strlen (cmd);
  
  selected_text = XFetchBytes (screen_display, &selected_length);
  if (selected_length)
    {
      char *line = xmalloc (cmdlen + selected_length + 1);
      strcpy (line, cmd);
      strncpy (line + cmdlen, selected_text, selected_length);
      line[cmdlen + selected_length] = 0;

      execute_command (line, 0);

      free (selected_text);
      free (line);
    }

  print_prompt ();
}


/* Subroutine used by "stop at" and "go till" buttons.
   Set a breakpoint at the position indicated by the "selection"
   in the source window, and, if RUNFLAG is nonzero, continue.  */

static void
breakpoint_button (w, runflag, call_data)
     Widget w;
     int runflag;
     caddr_t call_data;
{
  XtTextPosition start, finish;
  
  XtTextGetSelectionPos (source_text_widget, &start, &finish);
  if (!source_window_symtab)
    printf ("No source file displayed.\n");
  else
    {
      set_breakpoint (source_window_symtab, 
		      source_charpos_line (source_window_symtab, start),
		      runflag);
      if (runflag)
	{
	  cont_command (0, 1);
	  xgdb_display_source ();
	}
    }
  print_prompt ();
}

/* decide if a character is trash */
static int
garbage (c)
     char c;
{
  if ('a' <= c && c <= 'z') return 0;
  if ('A' <= c && c <= 'Z') return 0;
  if ('0' <= c && c <= '9') return 0;
  if (c == '_') return 0;
  return 1;
}

/* Set a breakpoint at the place specified by the "selection" in X.  */

static void
explicit_breakpoint_button ()
{
  int selected_length;
  char *selected_text;

  selected_text = XFetchBytes (screen_display, &selected_length);
  if (selected_length)
    {
      char *line = (char *) xmalloc (selected_length + 6);
      register char *p, *sp, *end;

      strcpy (line, "break ");

      /* Copy selection but exclude "garbage" characters.  */

      p = selected_text;
      end = p + selected_length;
      sp = line + strlen (line);

      while (garbage (*p) && p != end) p++;
      while (!garbage (*p) && p != end)
	*sp++ = *p++;
      *sp = 0;

      execute_command (line, 0);
      free (selected_text);
      free (line);
    }
  print_prompt ();
}

/* Handle a button by running the command COMMAND.  */

static void
do_command (w, command, call_data)
     Widget w;
     char *command;
     caddr_t call_data;
{
  execute_command (command, 0);
  xgdb_display_source ();
  print_prompt ();
}

static void
redisplay_button()
{
  xgdb_display_source();
}

/* Define and display all the buttons.  */

static void
addbutton (parent, name, function, closure)
     Widget parent;
     char *name;
     void (*function) ();
     caddr_t closure;
{
  static XtCallbackRec Callback[] =
    {
      {NULL, (caddr_t)NULL},
      {NULL, (caddr_t)NULL},
    };
  static Arg commandArgs[] =
    {
      {XtNlabel, (XtArgVal)NULL},
      {XtNcallback, (XtArgVal)Callback},
    };

  Callback[0].callback = (XtCallbackProc)function;
  Callback[0].closure = (caddr_t)closure;
  commandArgs[0].value = (XtArgVal)name;
  XtCreateManagedWidget (name, commandWidgetClass, parent,
			 commandArgs, XtNumber(commandArgs));
}

/* Create the button windows and store them in `buttons'.  */

static void
create_buttons (parent)
     Widget parent;
{
  addbutton (parent, "run", do_command, "run");
  addbutton (parent, "quit", do_command, "quit");

  addbutton (parent, "break in", explicit_breakpoint_button, NULL);
  addbutton (parent, "break at", breakpoint_button, 0);
  addbutton (parent, "go until", breakpoint_button, 1);

  addbutton (parent, "print", print_button, 0);
  addbutton (parent, "print*", print_button, 1);

  addbutton (parent, "next", do_command, "next");
  addbutton (parent, "step", do_command, "step");
  addbutton (parent, "cont", do_command, "cont");
  addbutton (parent, "finish", do_command, "finish");
  
  addbutton (parent, "up", do_command, "up");
  addbutton (parent, "down", do_command, "down");

  addbutton (parent, "redisplay", redisplay_button, NULL);
}

/* Create a "label window" that just displays the string LABEL.  */

static Widget
create_label (name, label)
     char *name, *label;
{
  Arg labelArgs[2];
  Widget w; 
  
  XtSetArg (labelArgs[0], XtNname, name);
  XtSetArg (labelArgs[1], XtNlabel, label);

  w = XtCreateManagedWidget ("label", labelWidgetClass, containing_widget,
			     labelArgs, XtNumber (labelArgs));
  XtPanedSetMinMax (w, w->core.height, w->core.height);
  return w;
}

/* Create a subwindow of PARENT that displays and scrolls the contents
   of file FILENAME.  */

static Widget
create_text_widget (parent, filename)
     Widget parent;
     char *filename;
{
  static Arg fileArgs[3];
  XtTextSource src;
  XtTextSink   sink;

  XtSetArg (fileArgs[0], XtNfile, filename);
  src = XtDiskSourceCreate(parent, fileArgs, 1);
  sink = XtAsciiSinkCreate(parent, NULL, 0);

  XtSetArg (fileArgs[0], XtNtextOptions, scrollVertical);
  XtSetArg (fileArgs[1], XtNtextSource, src);
  XtSetArg (fileArgs[2], XtNtextSink, sink);
  return XtCreateManagedWidget ("disk", textWidgetClass, parent,
				fileArgs, XtNumber (fileArgs));

#if 0 /* This is tucker's method.  */

  /* Create an empty source-display window and add to containing_widget */
  XtSetArg (argl[0], XtNfile, "/dev/null");
  XtSetArg (argl[1], XtNtextOptions, scrollVertical);
  XtSetArg (argl[2], XtNheight, (XtArgVal)sheight);
  source_text_widget = XtCreateManagedWidget (NULL, asciiDiskWidgetClass,
					      containing_widget, argl,
					      XtNumber (argl));

  /* Create NULL disk source */
  XtSetArg (argl[0], XtNfile, "/dev/null");
  NullSource = XtDiskSourceCreate (source_text_widget, argl, ONE);
#endif
}

/* window manager argument parsing */
extern int *win_argc;
extern char **win_argv;

/* Entry point to create the widgets representing our display.  */
int
xgdb_create_window ()
{
  int width, height;
  int sheight;
  Arg argl[3];
  
  /* initialize toolkit, setup defaults */
  main_widget = XtInitialize ("gdb", "gdb", NULL, 0, win_argc, win_argv);
  screen_display = XtDisplay (main_widget);

  /* Find out what size the user specified.  */

  XtSetArg (argl[0], XtNwidth, (XtArgVal)&width);
  XtSetArg (argl[1], XtNheight, (XtArgVal)&height);
  XtGetValues (main_widget, argl, XtNumber(argl));

  /* If none specified, set a default size.  */

  if (!width || !height)
    {
      width = 500, height = 700;
      XtSetArg (argl[0], XtNwidth, (XtArgVal)width);
      XtSetArg (argl[1], XtNheight, (XtArgVal)height);
      XtSetValues (main_widget, argl, XtNumber(argl));
    }
  sheight = (float)height / 2.5;

  /* Create the (toplevel) main_widget */
  XtSetArg (argl[0], XtNwidth, (XtArgVal)width);
  XtSetArg (argl[1], XtNheight, (XtArgVal)height);
  containing_widget
    = XtCreateManagedWidget ("vpaned", vPanedWidgetClass,
			     main_widget, argl, XtNumber (argl));
  XtPanedSetRefigureMode (containing_widget, FALSE);
  
  /* Create title */
  {
    char buf[200];
    extern char *version;
    sprintf (buf, "GDB %s", version);
    title_widget =
      create_label ("Title", buf);
  }

  /* Create exec file name window and add */
  exec_name_widget =
    create_label ("Executable", "No executable specified");

  /* Create window full of buttons.  */
  button_box_widget = XtCreateManagedWidget ("buttons", boxWidgetClass,
					     containing_widget, NULL, 0);
  create_buttons (button_box_widget);

  /* Create source file name window and add to containing_widget */
  source_name_widget =
    create_label ("Source File", "No source file yet.");

  /* Create an empty source-display window and add to containing_widget */
  source_text_widget = create_text_widget (containing_widget, "/dev/null");

#ifdef VFTD
  /* Create Fake Text source */
  {
    extern XtTextSource TCreateApAsSource();
    TSource = TCreateApAsSource();
  }

  /* Create interactive box */
  XtSetArg (argl[0], XtNtextSource, (XtArgVal)TSource);
  XtSetArg (argl[1], XtNtextSink,
	    (XtArgVal)XtAsciiSinkCreate(containing_widget, NULL, 0));
  XtSetArg (argl[2], XtNtextOptions,
	    (XtArgVal)(scrollVertical | wordBreak));
  interactive_widget = XtCreateManagedWidget ("gdbWindow", textWidgetClass,
					      containing_widget, argl, THREE);
#endif

  /* Put them one screen */
  XtPanedSetRefigureMode(containing_widget, TRUE);
  XtRealizeWidget (main_widget);

  /* Define GDB cursor */
#if 0
  XDefineCursor (screen_display, XtWindow (main_widget),
		 XCreateFontCursor (screen_display, XC_circle));
#endif

  XFlush (screen_display);
  return 1;
}

#define MAX_XGDB_READ 128

/* xgdb_dispatch -- Loop, dispatching on window events,
   until data is available on FP (which is normally stdin).
   Then return, so the data on FP can be processed.  */

void
xgdb_dispatch (fp)
     FILE *fp;
{
  int inmask = 1 << fileno (fp);
  int xmask = 1 << ConnectionNumber (screen_display);
  int rfds = 0;
  int nfds;
  XEvent ev;
  int pend;
  int nread;
  char buf[1024];
  int ipmask;

#ifdef VTFD
  ipmask = 1 << vtfd[0];
#endif

  while (! (rfds & inmask))
    {
      pend = XPending (screen_display);
      if (!pend)
	{
	  rfds = inmask | xmask | ipmask;
	  /* this isn't right for 4.3 but it works 'cuz of 4.2 compatibility */
	  nfds = select (32, &rfds, 0, 0, (struct timeval *) 0);
	}
      if (pend || rfds & xmask)
	{
	  XNextEvent (screen_display, &ev);
	  XtDispatchEvent (&ev);
	}

#ifdef VTFD
      /* Handle I/O through the command window.  */
      if (pend == 0 && (rfds & ipmask))
	{
	  nread = read (vtfd[0], buf, sizeof(buf));
	  xgdb_write (buf, nread);
	}
      nread = xgdb_read (buf, MAX_XGDB_READ);
      if (pend == 0 && nread > 0)
	{
	  write (vifd[1], buf, nread);
	}
#endif
    }
}  

#ifdef VTFD

static int output_size;
static int used_size;
static char *output_string;

static void
xgdb_init_text ()
{
  Arg args[2];

  output_size = 1000;
  output_string = (char *) xmalloc (output_size);
  used_size = 0;

  XtSetArg (args[0], XtNstring, (XtArgVal) output_string);
  XtSetArg (args[1], XtNlength, (XtArgVal) output_size);
  TSource
    = XtStringSourceCreate (toplevel, args, 2);

  XtSetArg (args[0], XtNtextSource, TSource);
  XtSetValues (interaction_widget, Args, 1);
}

static void
xgdb_grow_text (size)
     int size;
{
  if (output_size < used_size + size + 200)
    {
      Arg args[2];

      XtStringSourceDestroy (TSource);

      output_size = (used_size + size + 1010 + 512) / 1010 * 1010;
      output_string = xrealloc (output_string, output_size);

      XtSetArg (args[0], XtNstring, (XtArgVal) output_string);
      XtSetArg (args[1], XtNlength, (XtArgVal) output_size);
      TSource
	= XtStringSourceCreate (toplevel, args, 2);

      XtSetArg (args[0], XtNtextSource, TSource);
      XtSetValues (interaction_widget, Args, 1);
    }
}

/*VARARGS*/
xgdb_printf (fmt, arg1, arg2, arg3, arg4)
     char *fmt;
{
  char buf[1024];
  XtTextBlock text;
  XtTextPosition pos;

/* ??? This will crash on the wrong data.  */
  pos = (*TSource->Scan)(TSource, 0, XtstAll, XtsdRight, 1, 0);
  sprintf (buf, fmt, arg1, arg2, arg3, arg4);
  text.length = strlen (buf);
  text.ptr = buf;
  xgdb_grow_text (text.length);
  used_size += text.length;
  XtTextReplace (interactive_widget, pos, pos, &text);
  XtTextSetInsertionPoint (interactive_widget, pos + text.length);
  XFlush (screen_display);
}

int
xgdb_write (buf, len)
    char *buf;
    int len;
{
  XtTextBlock text;
  XtTextPosition pos;

  pos = (*TSource->Scan)(TSource, 0, XtstAll, XtsdRight, 1, 0);
  text.length = len;
  text.ptr = buf;
  xgdb_grow_text (text.length);
  used_size += text.length;
  XtTextReplace (interactive_widget, pos, pos, &text);
  XtTextSetInsertionPoint (interactive_widget, pos + text.length);
  XFlush (screen_display);
}

int
xgdb_read (buf, maxlen)
     char *buf;
     int maxlen;
{
  XtTextBlock text;
  XtTextPosition endpos;
  int length = 0;

  xgdb_grow_text (maxlen);
  endpos = XtTextGetInsertionPoint (interactive_widget);
  length = endpos - used_size;
  if (length > 0)
    {
      (*TSource->Read) (TSource, lastpos, &text, maxlen - 10);
      length = text.length;
      strncpy(buf, text.ptr, length);
      buf[length] = NULL;
      used_size += length;
    }
  return length;
}
#endif /* VTFD */

/* If we use an X window, the GDB command loop is told to call this function
   before reading a command from stdin.
   PROMPT is saved for later use so buttons can print a prompt-string.  */

void
xgdb_window_hook (infile, prompt)
     FILE *infile;
     char *prompt;
{
  prompt_string = prompt;
  xgdb_display_source ();
  xgdb_dispatch (infile);
}

static
initialize ()
{
  extern void (*window_hook) ();
  extern int inhibit_windows;

  if (getenv ("DISPLAY") && ! inhibit_windows)
    if (xgdb_create_window ())
      window_hook = xgdb_window_hook;

  specify_exec_file_hook (xgdb_display_exec_file);
}

END_FILE

