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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <X11/Atoms.h>
#include <X11/TopLevel.h>
#include <X11/VPane.h>
#include <X11/Label.h>
#include <X11/Text.h>
#include <X11/Command.h>
#include <X11/ButtonBox.h>

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

Display *screen_display;

/* The graphics context.  */

GC default_gc;

/* Windows manipulated by this package.  */

static Window icon_window;
static Widget containing_widget;
static Widget source_name_widget;
static Widget source_text_widget;
static Widget exec_name_widget;
static Widget button_box_widget;

/* Source text display.  */

static struct symtab *source_window_symtab = 0;

/* Forward declarations */

static Widget create_text_widget ();

START_FILE

/* Display an appropriate piece of source code in the source window.  */

xgdb_display_source ()
{
  char *filename;
  static Arg labelArgs[1];
  int linenumbers_changed = 0;
  static int new = 1;

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
      static Arg fileArgs[1];
      new = 1;
      source_window_symtab = sal.symtab;

      XtSetArg (fileArgs[0], XtNfile, filename);
      XtSetValues (source_text_widget, fileArgs, XtNumber (fileArgs));

      XtSetArg (labelArgs[0], XtNlabel,
		filename ? filename : "No source displayed.");
      XtSetValues (source_name_widget, labelArgs, XtNumber (labelArgs));
      if (filename) free (filename);
    }

  /* Update display and cursor positions as necessary.
     Cursor should be placed on line sal.line.  */

  {
    static int top_line_number, bottom_line_number;
    int current_top;
    Arg textArgs[1];

    if (! new)
      {
	int new_top;

	/* Get positions of start of display, and caret */
	XtSetArg (textArgs[0], XtNdisplayPosition, NULL);
	XtGetValues (source_text_widget, textArgs, XtNumber (textArgs));
	new_top = source_charpos_line (source_window_symtab,
				       (int) textArgs[0].value);
	bottom_line_number += new_top - top_line_number;
	top_line_number = new_top;
      }

    /* If appropriate, scroll the text display.  */
    if (sal.line < top_line_number
	|| sal.line > bottom_line_number
	|| new)
      {
	/* yes, these magic numbers are ugly, but I don't know how
	 * to get the height of a text widget in a V11-portable way
	 */
	top_line_number = (sal.line > 15) ? sal.line - 15 : 0;
	bottom_line_number = top_line_number + 35;
      
	XtSetArg (textArgs[0], XtNdisplayPosition,
		  source_line_charpos (source_window_symtab, top_line_number));
	XtSetValues (source_text_widget, textArgs, XtNumber (textArgs));
      }

    /* Set the text display cursor position within the text.  */

    XtSetArg (textArgs[0], XtNinsertPosition, 
	      source_line_charpos (source_window_symtab, sal.line));
    XtSetValues (source_text_widget, textArgs, XtNumber (textArgs));
  }
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
    printf ("%s", prompt_string);
}

/* Handlers for buttons.  */

/* Subroutine used by "print" and "print*" buttons.
   STARFLAG is 1 for print*, 0 for print.
   Get the "selection" from X and use it as the operand of a print command.  */

static void
print_1 (starflag)
     int starflag;
{
  int selected_length;
  char *selected_text;

  char *cmd = starflag ? "print * " : "print ";
  register int cmdlen = strlen (cmd);
  
  selected_text = XFetchBytes (&selected_length);
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

static void
print_button () 
{
  print_1 (0);
}

static void
print_star_button () 
{
  print_1 (1);
}

/* Subroutine used by "stop at" and "go till" buttons.
   Set a breakpoint at the position indicated by the "selection"
   in the source window, and, if RUNFLAG is nonzero, continue.  */

static void
breakpoint_button_1 (runflag)
     int runflag;
{
  XtTextPosition start, finish;
  
  XtTextGetSelectionPos (screen_display, source_text_widget, &start, &finish);
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

static void
breakpoint_button ()
{
  breakpoint_button_1 (0);
}

static void
until_button ()
{
  breakpoint_button_1 (1);
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

/* Various trivial buttons,
   most of which just run one GDB command with no arg.  */

static void
next_button ()
{
  execute_command ("next", 0);
  xgdb_display_source ();
  print_prompt ();
}

static void
step_button ()
{
  execute_command ("step", 0);
  xgdb_display_source ();
  print_prompt ();
}

static void
cont_button ()
{
  execute_command ("cont", 0);
  xgdb_display_source ();
  print_prompt ();
}

static void
finish_button ()
{
  execute_command ("finish", 0);
  xgdb_display_source ();
  print_prompt ();
}

#if 0
static void
deiconify_button ()
{
  XUnmapWindow (screen_display, icon_window);
  XMapWindow (screen_display, containing_widget);
}

static void
iconify_button ()
{
#if 0
  static Arg iconArgs[1];
  XtSetArg (iconArgs[0], XtNlabel, prompt_string);
  XtCommandSetValues (icon_window, iconArgs, XtNumber (iconArgs));
#endif 0
  XUnmapWindow (screen_display, containing_widget);
  XMapWindow (screen_display, icon_window);
}
#endif 0

static void
up_button ()
{
  execute_command ("up", 0);
  xgdb_display_source ();
  print_prompt ();
}

static void
down_button ()
{
  execute_command ("down", 0);
  xgdb_display_source ();
  print_prompt ();
}

/* Define and display all the buttons.  */

static void
addbutton (parent, name, function)
     Widget parent;
     char *name;
     void (*function) ();
{
  static Arg commandArgs[2];

  XtSetArg (commandArgs[0], XtNlabel, name);
  XtSetArg (commandArgs[1], XtNfunction, function);
  XtCreateWidget (name, commandWidgetClass, parent,
		  commandArgs, XtNumber (commandArgs));
}

/* Create the button windows and store them in `buttons'.  */

static void
create_buttons (parent)
     Widget parent;
{
  addbutton (parent, "Brk At", breakpoint_button);
  addbutton (parent, "Brk In", explicit_breakpoint_button);
  addbutton (parent, "Go 'til", until_button);

  addbutton (parent, "Print", print_button);
  addbutton (parent, "Print*", print_star_button);

  addbutton (parent, "Next", next_button);
  addbutton (parent, "Step", step_button);
  addbutton (parent, "Cont", cont_button);
  addbutton (parent, "Finish", finish_button);
  
  addbutton (parent, "Up", up_button);
  addbutton (parent, "Down", down_button);

/*  addbutton (parent, "Iconify", iconify_button); */
}

/* Create a "label window" that just displays the string LABEL.  */

static Widget
create_label (name, label)
     char *name, *label;
{
  static Arg labelArgs[2];
  
  XtSetArg (labelArgs[0], XtNname, name);
  XtSetArg (labelArgs[1], XtNlabel, label);
  return XtCreateWidget ("label1", labelWidgetClass, containing_widget,
			 labelArgs, XtNumber (labelArgs));
}

/* Create a subwindow of PARENT that displays and scrolls the contents
   of file FILENAME.  */

static Widget
create_text_widget (parent, filename)
     Window parent;
     char *filename;
{
  static Arg fileArgs[2];
  
  XtSetArg (fileArgs[0], XtNfile, filename);
  XtSetArg (fileArgs[1], XtNtextOptions, scrollVertical);
  return XtTextDiskCreate (parent, fileArgs, XtNumber (fileArgs));
}

/* Entry point to create the widgets representing our display.  */

int
xgdb_create_window ()
{
  static Arg frameArgs[]= {
      {XtNwidth, (XtArgVal) 600},
      {XtNheight, (XtArgVal) 700},
  };
  
  XrmResourceDataBase db;
  FILE *rdb_file;
  XGCValues dummy;
  
  /* Init and database stuff.  */
  screen_display = XOpenDisplay (NULL);
  if (screen_display == 0)
    {
      fprintf (stderr, "Cannot connect to X server");
      return 0;
    }

  {
    char *dummy1[1];
    dummy1[0] = 0;
    XtInitialize ("gdb", "gdb", 0, 0, 0, dummy1);
  }

  /* should be checking .Xdefaults in $HOME */
  rdb_file = fopen (".Xresources", "r");
  if (rdb_file != NULL)
    {
      XrmGetDataBase (rdb_file, &db);
      XrmSetCurrentDataBase (db);
      fclose (rdb_file);
    }
  
  /* Create the containing_widget.  */

  containing_widget = XtCreateWidget ("frame", vPaneWidgetClass, 0,
				      frameArgs, XtNumber (frameArgs));
  
  default_gc = XCreateGC (screen_display, containing_widget, 0, dummy);

  /* Create source file name window and add to containing_widget */
  source_name_widget
    = create_label ("Source File", "No source file yet.");

  /* Create an empty source-display window and add to containing_widget */
  source_text_widget = create_text_widget (containing_widget, "/dev/null");

  /* Create window full of buttons.  */
  button_box_widget = XtCreateWidget ("Buttons", buttonBoxWidgetClass,
				      containing_widget, NULL, 0);
  create_buttons (button_box_widget);

  /* Create exec file name window and add */
  exec_name_widget = create_label ("Executable", "No executable specified.");

#if 0
  /* Create icon window.  */
  {
    static Arg iconArgs[2];
    void (*compiler_bug) () = deiconify_button;
    XtSetArg (iconArgs[0], XtNlabel, "(gdb)");
    XtSetArg (iconArgs[1], XtNfunction, compiler_bug);
    icon_window = XtCommandCreate (DefaultRootWindow (screen_display),
				   iconArgs, XtNumber (iconArgs));
    XMoveWindow (screen_display, icon_window, 100, 100);	/* HACK */
#if 0
    XSetIconWindow (screen_display, containing_widget, icon_window);
#endif 0
  }
#endif 0

#if 0
  /* Now make the whole thing appear on the display.  */
  {
    Pixmap pm1, pm2;
    XImage image;
    Cursor curse;

    image.width = gdb_width;
    image.height = gdb_height;
    image.xoffset = 0;
    image.format = XYBitmap;
    image.byte_order = LSBFirst;
    image.bitmap_unit = 16;
    image.bitmap_bit_order = LSBFirst;
    image.depth = 1;
    image.bytes_per_line = 2;
    image.bits_per_pixel = 1;

    pm1 = XCreatePixmap (screen_display, DefaultScreen (screen_display),
			 gdb_width, gdb_height, 1);
    pm2 = XCreatePixmap (screen_display, DefaultScreen (screen_display),
			 gdb_width, gdb_height, 1);

    image.data = (char *) gdb_bits;
    XPutImage (screen_display, pm1, default_gc, &image, 0, 0, 0, 0,
	       gdb_width, gdb_height);

    image.data = (char *) gdb_mask_bits;
    XPutImage (screen_display, pm2, default_gc, &image, 0, 0, 0, 0,
	       gdb_width, gdb_height);

    curse = XCreatePixmapCursor (screen_display, pm1, pm2,
				 BlackPixel (screen_display,
					     DefaultScreen (screen_display)),
				 WhitePixel (screen_display,
					     DefaultScreen (screen_display)),
				 gdb_x_hot, gdb_y_hot);

    XFreePixmap (screen_display, pm1);
    XFreePixmap (screen_display, pm2);

    XDefineCursor (screen_display, containing_widget, curse);
    XDefineCursor (screen_display, icon_window, curse);
  }
#endif 0

  XtRealizeWidget (containing_widget);
  XFlush (screen_display);

  return 1;
}

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
  
  while (! (rfds & inmask))
    {
      pend = XPending ();
      if (!pend)
	{
	  rfds = inmask | xmask;
	  /* this isn't right for 4.3 but it works 'cuz of 4.2 compatibility */
	  nfds = select (32, &rfds, 0, 0, (struct timeval *) 0);
	}
      if (pend || rfds & xmask)
	{
	  XNextEvent (screen_display, &ev);
	  XtDispatchEvent (&ev);
	}
    }
}  

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
