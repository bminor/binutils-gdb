From beatty@unh.cs.cmu.edu Sat Jul  4 12:04:01 1987
Received: by PREP.AI.MIT.EDU; Sat, 4 Jul 87 12:03:37 EDT
Message-Id: <8707041603.AA08600@prep.ai.mit.edu>
To: phr@prep.ai.mit.edu (Paul Rubin)
Date: Sat, 4 Jul 87 12:03:01 EDT
From: Derek Beatty <beatty@unh.cs.cmu.edu>
Subject: Re: gdb and X (msg 1 of 3)
Status: R

This is part 1 of 3 parts.  It consists of the cursor I used, and a message
I sent to Zalman Stern at Andrew regarding what I did, and why.  The
code and context diffs will follow in other messages.

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

> 
> The X support I added is minimal; it was inspired by Suntools' dbxtool,
> together with the availability of the V10 implementation of the X V11
> toolkit specification.  Design was guided by simplicity and the facilities
> of the toolkit.  The debugger window provides a view into the code 
> corresponding to the current stack frame, and several buttons for the
> breakpoint, print, step, next, continue, finish, up, and down commands.
> The standard gdb command interface remains available in the tty window from
> which gdb was started.  The breakpoint and print buttons make use of the
> current selection, so you can do simple things like click at text in the
> source window, then click the "Go 'til" button to continue until that
> point.  
> 
> Such an interface is simple to program ( ~ 20 hours, about 700 lines), 
> but it has some drawbacks.  First, I didn't take the time to understand
> the longjmp's in gdb, and I'm not exactly happy with the idea of them
> jumping out of my callback procedures that were invoked by toolkit routines. 
> There's one core dump bug (it shows up when gdb can't find a source
> file) that I haven't tracked down, and it may be related to this.  Second,
> selection in the text window is not particularly graceful: double-clicking
> highlights one word of text, as the toolkit defines a word.  It would
> be much more graceful were double-clicking to highlight a C identifier. 
> Finally, and most seriously, most buttons operate by building textual
> command lines and passing them to gdb's execute_command function.  This
> means that all selected expressions are evaluated and printed in the
> lexical scope corresponding to the current stack frame, although the
> selected text may be in a different lexical scope.  This serious bug would
> require work to fix.
> 
> I wrote the X support out of frustration at not having dbxtool available
> when I work on a vax.  The hope of portability to V11 via the toolkit
> also helped motivate me to write V10 code at this late date.  Finally,
> I'd never written any nontrivial code that ran on a windowing system
> (although that turns out still to be the case). Were I to make a more
> serious effort at this project, I would probably add a general "define-button"
> command akin to gdb's "define" command.
> 
> Look in /usr/beatty/gnu/gdb on vlsi.cs.cmu.edu.  All files I have modified
> are marked, and also have associated backups (.B extensions). Bennet
> Yee has a copy of the toolkit library; see /usr/bsy/Xtlib on f.gp.cs.cmu.edu.
> 
>  -- Derek
> 

 -- Derek Beatty

From beatty@unh.cs.cmu.edu Sat Jul  4 12:12:47 1987
Received: by PREP.AI.MIT.EDU; Sat, 4 Jul 87 12:09:20 EDT
Message-Id: <8707041609.AA08643@prep.ai.mit.edu>
To: phr@PREP.AI.MIT.EDU (Paul Rubin)
Date: Sat, 4 Jul 87 12:07:25 EDT
From: Derek Beatty <beatty@unh.cs.cmu.edu>
Subject: Re: gdb and X (msg 2 of 3)
In-Reply-To: Message from "Paul Rubin" of Jul 4, 87 at 1:22 am
Status: R

The following is "tool.c".  I hereby grant permission to do anything you
like with it.

 -- Derek Beatty

[nosave]
/* 
 * gdb tool for X V10R4 (using V11-compatible toolkit).
 * Derek Beatty 30 June 87.
 */
#include <X/Xlib.h>
#include <X/Xt/Xtlib.h>
#include <stdio.h>

#include "defs.h"
#include "symtab.h"

#include "gdb.cursor"
#include "gdb_mask.cursor"



/* forward refs */

static Window createFileText();
/* 
 * Windows manipulated by this package.
 */

static Window 
  icon,
  frame, 
  srcLabelStrip,
  srcText,
  ctlPanel,
  execLabelStrip;

static Cursor curse;

/*
 * Source text display.
 */

static struct symtab *displayedSymtab= 0;

extern struct symtab *current_source_symtab;
extern int current_source_line;

toolDisplaySource()
{
  char *fullName;
  static Arg labelArgs[1];
  int linenumbers_changed= 0;
  static int newWidget= 1;

  struct symtab_and_line get_selected_frame_sal();
  struct symtab_and_line sal;

  /* we could be called before we are initialized */
  if (!frame) return;
  
  sal= get_selected_frame_sal();

  /* strictly this is wrong, but better than a blank display */
  if (sal.symtab==NULL) {
    sal.symtab= current_source_symtab;
    /* current_source_line may be off by a small number like 4 */
    sal.line= current_source_line;
  }

  /*
   * Switch to a new file if necessary.
   */

  if (sal.symtab)
    linenumbers_changed= get_filename_and_charpos(sal.symtab, 
  						sal.line,
  						&fullName);
  if (!fullName) sal.symtab= NULL;
  /* if the display may be wrong, destroy it */
  if (linenumbers_changed || displayedSymtab != sal.symtab) {
    XtVPanedWindowDeletePane( srcText);
    XtSendDestroyNotify( srcText);
    XDestroyWindow( srcText);
    srcText= 0;
  }
  /* if there's no display, create one */
  if (!srcText) {
    newWidget= 1;
    /* if there's no valid display, create a dummy display */
    if (!sal.symtab ) {
      displayedSymtab= NULL;
      srcText= createFileText(frame, "/dev/null");
      XtVPanedWindowAddPane(frame, srcText, 1, 20, 1000, 1);
      /* create /dev/null text widget */
      XtSetArg(labelArgs[0], XtNlabel, "No source displayed.");
      XtLabelSetValues(srcLabelStrip, labelArgs, XtNumber(labelArgs));
     } else {
      displayedSymtab= sal.symtab;
      srcText= createFileText(frame, fullName);
      XtVPanedWindowAddPane(frame, srcText, 1, 20, 1000, 1);
      XtSetArg(labelArgs[0], XtNlabel, fullName);
      XtLabelSetValues(srcLabelStrip, labelArgs, XtNumber(labelArgs));
      /* free filename (maybe: check gdb code!) */
    }
  }

  /*
   * Update display and cursor positions as necessary.
   * Cursor should be placed on line sal.line.
   */

  {
    static int prevTop= 0, highWaterMark= 0;
    int currentTop;
    Arg textArgs[1];

    /* get positions of start of display, and caret */
    XtSetArg(textArgs[0], XtNdisplayPosition, NULL);
    XtTextGetValues(srcText, textArgs, XtNumber(textArgs));
    currentTop= cvtCharToLine(displayedSymtab,
    			      (int) textArgs[0].value);
    
    highWaterMark += currentTop - prevTop;

    if (    sal.line < currentTop
    	 || sal.line > highWaterMark
         || newWidget) {
    
      /* warp the display */

      newWidget= 0;

	/* yes, these magic numbers are ugly, but I don't know how
	 * to get the height of a text widget in a V11-portable way
	 */
      currentTop= (sal.line > 15) ? sal.line - 15 : 0;
      highWaterMark= currentTop + 35;
      
      XtSetArg(textArgs[0], XtNdisplayPosition,
      	       cvtLineToChar(displayedSymtab, currentTop));
      XtTextSetValues(srcText, textArgs, XtNumber(textArgs));
    }
    XtSetArg(textArgs[0], XtNinsertPosition, 
    	     cvtLineToChar(displayedSymtab, sal.line));
    XtTextSetValues(srcText, textArgs, XtNumber(textArgs));

    prevTop=  currentTop;
  }
}

/* return the character position of a line */
int
cvtLineToChar( s, line)
  struct symtab *s;
  int line;
{
  if (!s) return 0;
  if (!s->line_charpos) return 0;
  if (line < 0) line= 0;
  if (line > s->nlines) line= s->nlines;
  return *(s->line_charpos + line-1);
}

/* return the line position of a character */
int
cvtCharToLine( s, chr)
    register struct symtab *s;
    register int chr;
{
    register int lineNumber= 0;
    register int *lnp;
    
    if (!s) return 0;
    lnp= s->line_charpos;
    /* files are usually short, so sequential search is Ok */
    while ( lineNumber < s->nlines  && *lnp <= chr) {
    	lineNumber++;
    	lnp++;
    }
    if (lineNumber >= s->nlines)
       lineNumber= s->nlines;
    return lineNumber;
}

/* 
 * title bar at bottom
 */

static char *execFileName;

toolSetExecFile(s)
  char *s;
{
  execFileName= s;
  if (execLabelStrip) {
    static Arg labelArgs[1];

    XtSetArg(labelArgs[0], XtNlabel, execFileName);
    XtLabelSetValues(execLabelStrip, labelArgs, XtNumber(labelArgs));
  }
}

/*
 * Command line into which command are placed for execution.
 * There's some ugly interaction between this and readline in main.c.
 */
extern char *line;
extern int linesize;

/*
 * Do any necessary prompting, etc.
 */
static char *gdbPrompt;

static void
printPrompt()
{
  if (gdbPrompt) {
    printf("%s", gdbPrompt);
    fflush(stdout);
  }
}

/*
 * Callback procedures for control panel.
 */

/* used by "print" and "print*" buttons */
static void printButnProc_1( starflag)
  int starflag;
{
  int selnLen;
  char *seln;

  char *cmd= starflag ? "print * " : "print ";
  register int cmdlen= strlen(cmd);
  
  seln= XFetchBytes(&selnLen);
  if (selnLen) {
    if (selnLen+cmdlen >= linesize-1) {
     linesize= (selnLen+cmdlen > linesize*2-1) ? selnLen+cmdlen+1 : linesize*2;
     line= (char *) xrealloc(line, linesize);
    }
    strcpy(line, cmd);
    strncpy(line+cmdlen, seln, selnLen);
    *(line+cmdlen+selnLen)= '\0';
    execute_command(line, 0);
    free(seln);
  }
  printPrompt();
}

static void printButnProc() 
{
  printButnProc_1( 0);
}

static void printStarButnProc() 
{
  printButnProc_1( 1);
}

static void nextButnProc()
{
  strcpy(line, "next");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

static void stepButnProc()
{
  strcpy(line, "step");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

static void contButnProc()
{
  strcpy(line, "cont");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

static void finButnProc()
{
  strcpy(line, "finish");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

/* used by "stop at" and "go till" buttons */
static void stopAtButnProc_1( gotillFlag)
  int gotillFlag;
{
  XtTextPosition start, finish;
  static int lineNumber; 
  
  XtTextGetSelectionPos(srcText, &start, &finish);
  if (!displayedSymtab)
    printf("No source file displayed.\n");
  else {
    break_command_for_tool( displayedSymtab, 
    			    cvtCharToLine(displayedSymtab, start),
    			    gotillFlag);
    if (gotillFlag) {
      strcpy(line, "cont");
      execute_command(line, 0);
      toolDisplaySource();
    }
  }
  printPrompt();
}

static void stopAtButnProc()
{
  stopAtButnProc_1( 0);
}

static void untilButnProc()
{
  stopAtButnProc_1( 1);
}

/* decide if a character is trash */
static int
garbage(c)
  char c;
{
  if ('a' <= c && c <= 'z') return 0;
  if ('A' <= c && c <= 'Z') return 0;
  if ('0' <= c && c <= '9') return 0;
  if (c == '_') return 0;
  return 1;
}

static void stopInButnProc()
{
  static int selnLen;
  static char *seln;
  char *sp, *selnp;

  seln= XFetchBytes(&selnLen);
  if (selnLen) {
    if (selnLen+6 >= linesize-1) {
      linesize= (selnLen+6 > linesize*2-1) ? selnLen+7 : linesize*2;
      line= (char *) xrealloc(line, linesize);
    }
    strcpy(line, "break ");
    /* copy selection but not garbage */
    selnp= seln;
    sp= line+strlen(line);
    while (garbage(*selnp) && selnLen) selnp++, selnLen--;
    while (!garbage(*selnp) && selnLen) {
      *sp++= *selnp++;
      selnLen--;
    }
    *sp= '\0';
    execute_command(line, 0);
    free(seln);
  }
  printPrompt();
}

static void deIconifyButnProc()
{
  XUnmapWindow(icon);
  XMapWindow(frame);
}

static void iconifyButnProc()
{
  static Arg iconArgs[1];
  XtSetArg(iconArgs[0], XtNlabel, gdbPrompt);
  XtCommandSetValues(icon, iconArgs, XtNumber(iconArgs));
  XUnmapWindow(frame);
  XMapWindow(icon);
}

static void upButnProc()
{
  strcpy(line, "up");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

static void downButnProc()
{
  strcpy(line, "down");
  execute_command(line, 0);
  toolDisplaySource();
  printPrompt();
}

#define addbutton(w) XtSetArg(buttons[buttoncount], XtNwindow, w); \
		     buttoncount++;
static Arg buttons[20];
static int buttoncount= 0;

/*
 * Create control panel buttons.
 */
static createButtons(parent)
  Window parent;
{
  static Window button;
  static Arg commandArgs[2];

#define crButn(label,fn)	\
	XtSetArg(commandArgs[0], XtNlabel, label);\
	XtSetArg(commandArgs[1], XtNfunction, fn);\
	button= XtCommandCreate(parent, commandArgs, XtNumber(commandArgs));\
	addbutton(button);

  crButn("Brk At", stopAtButnProc);
  crButn("Brk In", stopInButnProc);
  crButn("Go 'til", untilButnProc);

  crButn("Print", printButnProc);
  crButn("Print*", printStarButnProc);

  crButn("Next", nextButnProc);
  crButn("Step", stepButnProc);
  crButn("Cont", contButnProc);
  crButn("Finish", finButnProc);
  
  crButn("Up", upButnProc);
  crButn("Down", downButnProc);

  crButn("Iconify", iconifyButnProc);
#undef crButn
}

static Window createLabel(parent, name, label)
  Window parent;
  char *name, *label;
{
  static Arg labelArgs[2];
  
  XtSetArg(labelArgs[0], XtNname, name);
  XtSetArg(labelArgs[1], XtNlabel, label);
  return XtLabelCreate(frame, labelArgs, XtNumber(labelArgs));
}

static Window createFileText( parent, filename)
  Window parent;
  char *filename;
{
  static Arg fileArgs[2];
  
  XtSetArg(fileArgs[0], XtNfile, filename);
  XtSetArg(fileArgs[1], XtNtextOptions, scrollVertical);
  return XtTextDiskCreate(parent, fileArgs, XtNumber(fileArgs));
}

/***************** Externally referenced routine **************/
int createTool()
{
  static Arg frameArgs[]= {
      {XtNwidth, (XtArgVal) 600},
      {XtNheight, (XtArgVal) 700},
  };
  
  ResourceDataBase db;
  FILE *rdbFile;
  
  /*
   * init and database stuff... this is wrong but what the heck
   */
  if (XOpenDisplay("") == NULL)
    return 0;
  printf("Initializing tool..."); fflush(stdout);
  XtInitialize();
  /* should be checking .Xdefaults in $HOME */
  if ((rdbFile= fopen(".Xresources", "r")) != NULL) {
    XtGetDataBase(rdbFile, &db);
    XtSetCurrentDataBase(db);
    fclose(rdbFile);
  }
  
  /*
   * create the frame
   */
  frame= XtVPanedWindowCreate(RootWindow, frameArgs, XtNumber(frameArgs));
  
  /* create source label strip and add to frame */
  srcLabelStrip= createLabel(frame, "Source File", "No source file yet.");
  XtVPanedWindowAddPane(frame, srcLabelStrip, 0, 15, 15, 0);

  /* create text widget and add to frame */
  srcText= createFileText(frame, "/dev/null");
  XtVPanedWindowAddPane(frame, srcText, 1, 20, 1000, 1);

  /* create button box */
  ctlPanel= XtButtonBoxCreate(frame, NULL, 0);
  createButtons( ctlPanel);
  XtButtonBoxAddButton(ctlPanel, buttons, buttoncount);
  XtVPanedWindowAddPane(frame, ctlPanel, 2, 30, 30, 0);

  /* create exec label strip and add */
  execLabelStrip= createLabel(frame, "Executable", 
     execFileName ? execFileName : "No executable specified.");
  XtVPanedWindowAddPane(frame, execLabelStrip, 3, 15, 15, 0);
  
  
  /* create icon */
  {
    static Arg iconArgs[2];
    XtSetArg(iconArgs[0], XtNlabel, "(gdb)");
    XtSetArg(iconArgs[1], XtNfunction, deIconifyButnProc);
    icon= XtCommandCreate(RootWindow, iconArgs, XtNumber(iconArgs));
    XMoveWindow(icon, 100, 100);	/* HACK */
    XSetIconWindow(frame, icon);
  }
  
  /* throw it onto the display */
  curse= XCreateCursor(gdb_width, gdb_height, gdb_bits, gdb_mask_bits,
  		gdb_x_hot, gdb_y_hot,
  		BlackPixel, WhitePixel, GXcopy);
  XDefineCursor(frame, curse);
  XDefineCursor(icon, curse);
  XMapWindow(frame);
  XMapSubwindows(frame);
  XFlush();
  printf("done\n");
  return 1;
}

/**************** Externally referenced routine. ***********/
/* toolDispatcher -- dispatch events until data is available on fp */
toolDispatcher(fp, prompt)
  FILE *fp;
  char *prompt;
{
  int inMask= 1 << fileno(fp);
  int xMask= 1 << dpyno();
  int rfds= 0;
  int nfds;
  XEvent ev;
  int pend;
  
  gdbPrompt= prompt;
  
  while (! (rfds & inMask)) {
    pend= XPending();
    if (!pend) {
      rfds= inMask | xMask;
      /* this isn't right for 4.3 but it works 'cuz of 4.2 compatibility */
      nfds= select( 32, &rfds, 0, 0, (struct timeval *) 0);
    }
    if (pend || rfds & xMask) {
      XNextEvent(&ev);
      XtDispatchEvent(&ev);
    }
  }
}  

From beatty@unh.cs.cmu.edu Sat Jul  4 12:17:44 1987
Received: by PREP.AI.MIT.EDU; Sat, 4 Jul 87 12:15:18 EDT
Message-Id: <8707041615.AA08691@prep.ai.mit.edu>
To: phr@PREP.AI.MIT.EDU (Paul Rubin)
Date: Sat, 4 Jul 87 12:14:08 EDT
From: Derek Beatty <beatty@unh.cs.cmu.edu>
Subject: Re: gdb and X (msg 3 of 3)
In-Reply-To: Message from "Paul Rubin" of Jul 4, 87 at 1:22 am
Status: R

Context diffs follow.  The original files are from GDB 2.1 (emacs distribution
18.40).

 -- Derek Beatty
[nosave]
*** /usr/misc/.gdb/src/core.c	Fri Mar 27 12:20:14 1987
--- core.c	Sat Jul  4 11:12:16 1987
***************
*** 1,3
  /* Work with core dump and executable files, for GDB.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  

--- 1,5 -----
+ /* modified by Beatty 1 Jul 87 for gdb tool. */
+ 
  /* Work with core dump and executable files, for GDB.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  
***************
*** 257,262
      }
    else if (from_tty)
      printf ("No exec file now.\n");
  }
  
  /* If we have both a core file and an exec file,

--- 259,267 -----
      }
    else if (from_tty)
      printf ("No exec file now.\n");
+ #ifdef TOOL
+     toolSetExecFile( filename ? filename : "No executable specified.\n");
+ #endif /* def TOOL */
  }
  
  /* If we have both a core file and an exec file,
*** /usr/misc/.gdb/src/breakpoint.c	Fri Mar 27 12:20:11 1987
--- breakpoint.c	Wed Jul  1 11:27:31 1987
***************
*** 1,3
  /* Everything about breakpoints, for GDB.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  

--- 1,5 -----
+ /* modified by Beatty 1 Jul 87 for gdbtool */
+ 
  /* Everything about breakpoints, for GDB.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  
***************
*** 513,518
  	break;
        }
  }
  
  /* Set a breakpoint according to ARG (function, linenum or *address)
     and make it temporary if TEMPFLAG is nonzero.  */

--- 515,571 -----
  	break;
        }
  }
+ 
+ #ifdef TOOL
+ /* set a breakpoint from a symtab and line */
+ void break_command_for_tool( s, line, tempflag)
+   struct symtab *s;
+   int line;
+   int tempflag;
+ {
+   register struct breakpoint *b;
+   struct symtab_and_line sal;
+   
+   sal.symtab= s;
+   sal.line= line;
+   sal.pc= find_line_pc( sal.symtab, sal.line);
+   if (sal.pc==0) {
+     error("No line %d in file \"%s\".\n", sal.line, sal.symtab->filename);
+   } else {
+     b= set_raw_breakpoint( sal);
+     b->number= ++breakpoint_count;
+     b->cond= 0;
+     if (tempflag)
+       b->enable= temporary;
+ 
+   printf ("Breakpoint %d at 0x%x", b->number, b->address);
+   if (b->symtab)
+     printf (": file %s, line %d.", b->symtab->filename, b->line_number);
+   printf ("\n");
+ 
+     {
+       int others = 0;
+       ALL_BREAKPOINTS (b)
+ 	if (b->address == sal.pc && b->number != breakpoint_count)
+ 	  others++;
+       if (others > 0)
+ 	{
+ 	  printf ("Note: breakpoint%s ", (others > 1) ? "s" : "");
+ 	  ALL_BREAKPOINTS (b)
+ 	    if (b->address == sal.pc && b->number != breakpoint_count)
+ 	      {
+ 		others--;
+ 		printf ("%d%s%s ",
+ 			b->number,
+ 			(b->enable == disabled) ? " (disabled)" : "",
+ 			(others > 1) ? "," : ((others == 1) ? " and" : ""));
+ 	      }
+ 	  printf (" also set at pc 0x%x\n", sal.pc);
+ 	}
+     }
+   }
+ }
+ #endif /* def TOOL */
  
  /* Set a breakpoint according to ARG (function, linenum or *address)
     and make it temporary if TEMPFLAG is nonzero.  */
*** /usr/misc/.gdb/src/main.c	Fri Mar 27 12:20:45 1987
--- main.c	Sat Jul  4 11:13:32 1987
***************
*** 1,3
  /* Top level for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  

--- 1,5 -----
+ /* modified by Beatty 30 june 87 for gdb tool */
+ 
  /* Top level for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  
***************
*** 42,47
  
  FILE *instream;
  
  void free_command_lines ();
  char *read_line ();
  static void initialize_main ();

--- 44,54 -----
  
  FILE *instream;
  
+ #ifdef TOOL
+ /* flag indicating whether we are running in a window system */
+ int isaTool= 0;
+ #endif /* def TOOL */
+ 
  void free_command_lines ();
  char *read_line ();
  static void initialize_main ();
***************
*** 214,219
  
    while (1)
      {
        if (!setjmp (to_top_level))
  	command_loop ();
        clearerr (stdin);		/* Don't get hung if C-d is typed.  */

--- 221,232 -----
  
    while (1)
      {
+ 
+ #ifdef TOOL
+       if (!isaTool)
+         isaTool= createTool();
+ #endif /* def TOOL */
+ 
        if (!setjmp (to_top_level))
  	command_loop ();
        clearerr (stdin);		/* Don't get hung if C-d is typed.  */
***************
*** 270,275
  	printf ("%s", prompt);
        fflush (stdout);
  
        quit_flag = 0;
        execute_command (read_line (instream == stdin), instream == stdin);
        /* Do any commands attached to breakpoint we stopped at.  */

--- 283,294 -----
  	printf ("%s", prompt);
        fflush (stdout);
  
+ #ifdef TOOL
+       toolDisplaySource();
+       if (isaTool) toolDispatcher(instream, 
+       				  instream==stdin ? prompt : NULL);
+ #endif /* def TOOL */
+ 
        quit_flag = 0;
        execute_command (read_line (instream == stdin), instream == stdin);
        /* Do any commands attached to breakpoint we stopped at.  */
***************
*** 320,325
  
    while (1)
      {
        c = fgetc (instream);
        if (c == -1 || c == '\n')
  	break;

--- 339,345 -----
  
    while (1)
      {
+ 
        c = fgetc (instream);
        if (c == -1 || c == '\n')
  	break;
***************
*** 765,770
  GDB is free software and you are welcome to distribute copies of it\n\
   under certain conditions; type \"info copying\" to see the conditions.\n",
  	  version);
  }
  
  static void

--- 785,793 -----
  GDB is free software and you are welcome to distribute copies of it\n\
   under certain conditions; type \"info copying\" to see the conditions.\n",
  	  version);
+ #ifdef TOOL
+   printf( "(CMU X support is available in this version.)\n");
+ #endif
  }
  
  static void
*** /usr/misc/.gdb/src/source.c	Fri Mar 27 12:20:50 1987
--- source.c	Wed Jul  1 17:56:58 1987
***************
*** 1,3
  /* List lines of source files for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  

--- 1,5 -----
+ /* modified 1 July 87 by Beatty for gdbtool */
+ 
  /* List lines of source files for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  
***************
*** 295,300
    s->nlines = nlines;
    s->line_charpos = (int *) xrealloc (line_charpos, nlines * sizeof (int));
  }
  
  /* Print source lines from the file of symtab S,
     starting with line number LINE and stopping before line number STOPLINE.  */

--- 297,328 -----
    s->nlines = nlines;
    s->line_charpos = (int *) xrealloc (line_charpos, nlines * sizeof (int));
  }
+ 
+ #ifdef TOOL
+ /* Get full pathname and line number positions for a symtab 
+  *	return nonzero if line numbers may have changed
+  *	set full pathname to NULL if no file found
+  */
+ int
+ get_filename_and_charpos(s, line, fullname)
+   struct symtab *s;
+   int line;
+   char **fullname;
+ {
+   register int desc, linenums_changed= 0;
+   
+   desc= openp(source_path, 0, s->filename, O_RDONLY, 0, fullname);
+   if (desc < 0) {
+     *fullname= NULL;
+     return 0;
+   }  
+   if (s->line_charpos==0) linenums_changed= 1;
+   if (linenums_changed) find_source_lines(s, desc);
+   close(desc);
+   return linenums_changed;
+ }
+ #endif /* def TOOL */
+ 
  
  /* Print source lines from the file of symtab S,
     starting with line number LINE and stopping before line number STOPLINE.  */
*** /usr/misc/.gdb/src/stack.c	Fri Mar 27 12:20:51 1987
--- stack.c	Wed Jul  1 17:27:34 1987
***************
*** 1,3
  /* Print and select stack frames for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  

--- 1,5 -----
+ /* modified by Beatty 1 Jul 87 for gdbtool */
+ 
  /* Print and select stack frames for GDB, the GNU debugger.
     Copyright (C) 1986, 1987 Free Software Foundation, Inc.
  
***************
*** 42,47
  static void select_calling_frame ();
  
  void print_frame_info ();
  
  /* Print a stack frame briefly.  FRAME should be the frame address
     and LEVEL should be its level in the stack (or -1 for level not defined).

--- 44,62 -----
  static void select_calling_frame ();
  
  void print_frame_info ();
+ 
+ #ifdef TOOL
+ /* get symtab and line of selected frame, for tool display */
+ struct symtab_and_line
+ get_selected_frame_sal()
+ {
+   struct frame_info fi;
+ 
+   fi= get_frame_info( selected_frame);
+   return find_pc_line(fi.pc, fi.next_frame);
+ }
+ 
+ #endif /* TOOL */
  
  /* Print a stack frame briefly.  FRAME should be the frame address
     and LEVEL should be its level in the stack (or -1 for level not defined).
End of context diffs.  The presence of this line verifies that this message
has not been truncated.

