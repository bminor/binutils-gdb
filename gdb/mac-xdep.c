/* Top level support for Mac interface to GDB, the GNU debugger.
   Copyright 1994 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Stan Shebs for Cygnus.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"

#include <Values.h>
#include <Types.h>
#include <Resources.h>
#include <QuickDraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Windows.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Desk.h>
#include <ToolUtils.h>
#include <Memory.h>
#include <SegLoad.h>
#include <Files.h>
#include <Folders.h>
#include <OSUtils.h>
#include <OSEvents.h>
#include <DiskInit.h>
#include <Packages.h>
#include <Traps.h>
#include <Lists.h>
#include <GestaltEqu.h>
#include <PPCToolbox.h>
#include <AppleEvents.h>
#include <StandardFile.h>
#include <Sound.h>

#ifdef MPW
#define QD(whatever) (qd.##whatever)
#define QDPat(whatever) (&(qd.##whatever))
#endif

#ifdef THINK_C
#define QD(whatever) (whatever)
#endif

#define p2c(pstr,cbuf)  \
  strncpy(cbuf, ((char *) (pstr) + 1), pstr[0]);  \
  cbuf[pstr[0]] = '\0';

#define pascalify(STR) \
  sprintf(tmpbuf, " %s", STR);  \
  tmpbuf[0] = strlen(STR);

#include "gdbcmd.h"
#include "call-cmds.h"
#include "symtab.h"
#include "inferior.h"
#include "signals.h"
#include "target.h"
#include "breakpoint.h"
#include "gdbtypes.h"
#include "expression.h"
#include "language.h"
#include "terminal.h" /* For job_control.  */

#include "mac-defs.h"

int useWNE;

int hasColorQD;

int inbackground;

Rect dragrect = { -32000, -32000, 32000, 32000 };
Rect sizerect;

int sbarwid = 15;

/* Globals for the console window. */

WindowPtr console_window;

ControlHandle console_v_scrollbar;

Rect console_v_scroll_rect;

TEHandle console_text;

Rect console_text_rect;

mac_init ()
{
  SysEnvRec se;
  int eventloopdone = 0;
  Boolean gotevent;
  Point mouse;
  EventRecord event;
  WindowPtr win;
  RgnHandle cursorRgn;
  int i;
  Handle menubar;
  MenuHandle menu;

  /* Do the standard Mac environment setup. */
  InitGraf (&QD (thePort));
  InitFonts ();
  FlushEvents (everyEvent, 0);
  InitWindows ();
  InitMenus ();
  TEInit ();
  InitDialogs (NULL);
  InitCursor ();

  /* Color Quickdraw is different from Classic QD. */
  SysEnvirons(2, &se);
  hasColorQD = se.hasColorQD;

  sizerect.top = 50;
  sizerect.left = 50;
  sizerect.bottom = 1000;
  sizerect.right  = 1000;
#if 0
  sizerect.bottom = screenBits.bounds.bottom - screenBits.bounds.top;
  sizerect.right  = screenBits.bounds.right  - screenBits.bounds.left;
#endif

  /* Set up the menus. */
  menubar = GetNewMBar (mbMain);
  SetMenuBar (menubar);
  /* Add the DAs etc as usual. */
  menu = GetMHandle (mApple);
  if (menu != nil) {
    AddResMenu (menu, 'DRVR');
  }
  DrawMenuBar ();

  /* Create the main window we're going to play in. */
  if (hasColorQD)
    console_window = GetNewCWindow (wConsole, NULL, (WindowPtr) -1L);
  else
    console_window = GetNewWindow (wConsole, NULL, (WindowPtr) -1L);

  if (1) DebugStr("\pnear beginning");
  SetPort (console_window);
  console_text_rect = console_window->portRect;
  console_text_rect.bottom -= sbarwid - 1;
  console_text_rect.right -= sbarwid - 1;
  console_text = TENew (&console_text_rect, &console_text_rect);
  TESetSelect (0, 32767, console_text);
  TEDelete (console_text);
  TEInsert ("(gdb)", strlen("(gdb)"), console_text);

  console_v_scroll_rect = console_window->portRect;
  console_v_scroll_rect.bottom -= sbarwid - 1;
  console_v_scroll_rect.left = console_v_scroll_rect.right - sbarwid;
  console_v_scrollbar =
    NewControl (console_window, &console_v_scroll_rect,
		"\p", 1, 0, 0, 0, scrollBarProc, 0L);

  ShowWindow (console_window);
  SelectWindow (console_window);
/*  force_update (console_window);  */

  return 1;
}

mac_command_loop()
{
  SysEnvRec se;
  int eventloopdone = 0;
  Boolean gotevent;
  Point mouse;
  EventRecord event;
  WindowPtr win;
  RgnHandle cursorRgn;
  int i;
  Handle menubar;
  MenuHandle menu;

  /* Figure out if the WaitNextEvent Trap is available. */
  useWNE =
    (NGetTrapAddress (0x60, ToolTrap) != NGetTrapAddress (0x9f, ToolTrap));
  /* Pass WNE an empty region the 1st time thru. */
  cursorRgn = NewRgn ();
  /* Go into the main event-handling loop. */
  while (!eventloopdone)
    {
      /* Use WaitNextEvent if it is available, otherwise GetNextEvent. */
      if (useWNE)
	{
	  get_global_mouse (&mouse);
	  adjust_cursor (mouse, cursorRgn);
	  gotevent = WaitNextEvent (everyEvent, &event, 0L, cursorRgn);
	}
      else
	{
	  SystemTask ();
	  gotevent = GetNextEvent (everyEvent, &event);
	}
      /* First decide if the event is for a dialog or is just any old event. */
      if (FrontWindow () != nil && IsDialogEvent (&event))
	{
	  short itemhit;
	  DialogPtr dialog;
      
	  /* Handle all the modeless dialogs here. */
	  if (DialogSelect (&event, &dialog, &itemhit))
	    {
	    }
	}
      else if (gotevent)
	{
	  /* Make sure we have the right cursor before handling the event. */
	  adjust_cursor (event.where, cursorRgn);
	  do_event (&event);
	}
    }
}

get_global_mouse (mouse)
Point *mouse;
{
  EventRecord evt;
	
  OSEventAvail (0, &evt);
  *mouse = evt.where;
}

adjust_cursor (mouse, region)
Point mouse;
RgnHandle region;
{
}

/* Decipher an event, maybe do something with it. */

do_event (evt)
EventRecord *evt;
{
  short part, err, rslt = 0;
  WindowPtr win;
  Boolean hit;
  char key;
  Point pnt;

  switch (evt->what)
    {
    case mouseDown:
      /* See if the click happened in a special part of the screen. */
      part = FindWindow (evt->where, &win);
      switch (part)
	{
	case inMenuBar:
	  adjust_menus ();
	  do_menu_command (MenuSelect (evt->where));
	  break;
	case inSysWindow:
	  SystemClick (evt, win);
	  break;
	case inContent:
	  if (win != FrontWindow ())
	    {
	      /* Bring the clicked-on window to the front. */
	      SelectWindow (win);
	      /* Fix the menu to match the new front window. */
	      adjust_menus ();
	      /* We always want to discard the event now, since clicks in a
		 windows are often irreversible actions. */
	    } else
	      /* Mouse clicks in the front window do something useful. */
	      do_mouse_down (win, evt);
	  break;
	case inDrag:
	  /* Standard drag behavior, no tricks necessary. */
	  DragWindow (win, evt->where, &dragrect);
	  break;
	case inGrow:
	  grow_window (win, evt->where);
	  break;
	case inZoomIn:
	case inZoomOut:
	  zoom_window (win, evt->where, part);
	  break;
	case inGoAway:
	  close_window (win);
	  break;
	}
      break;
    case keyDown:
    case autoKey:
      key = evt->message & charCodeMask;
      /* Check for menukey equivalents. */
      if (evt->modifiers & cmdKey)
	{
	  if (evt->what == keyDown)
	    {
	      adjust_menus ();
	      do_menu_command (MenuKey (key));
	    }
	}
      else
	{
	  if (evt->what == keyDown)
	    {
	      /* Random keypress, interpret it. */
	      do_keyboard_command (key);
	    }
	}
      break;
    case activateEvt:
      activate_window ((WindowPtr) evt->message, evt->modifiers & activeFlag);
      break;
    case updateEvt:
      update_window ((WindowPtr) evt->message);
      break;
    case diskEvt:
      /* Call DIBadMount in response to a diskEvt, so that the user can format
	 a floppy. (from DTS Sample) */
      if (HiWord (evt->message) != noErr)
	{
	  SetPt (&pnt, 50, 50);
	  err = DIBadMount (pnt, evt->message);
	}
      break;
    case app4Evt:
      /* Grab only a single byte. */
      switch ((evt->message >> 24) & 0xFF)
	{
	case 0xfa:
	  break;
	case 1:
	  inbackground = !(evt->message & 1);
	  activate_window (FrontWindow (), !inbackground);
	  break;
	}
      break;
    case kHighLevelEvent:
      AEProcessAppleEvent (evt);
      break;
    case nullEvent:
      rslt = 1;
      break;
    default:
      break;
    }
  return rslt;
}

grow_window (win, where)
WindowPtr win;
Point where;
{
  long winsize;
  int h, v;
  GrafPtr oldport;

  winsize = GrowWindow (win, where, &sizerect);
  if (winsize != 0)
    {
      GetPort (&oldport);
      SetPort (win);
      EraseRect (&win->portRect);
      h = LoWord (winsize);
      v = HiWord (winsize);
      SizeWindow (win, h, v, 1);
      if (win == console_window)
	{
	  MoveControl(console_v_scrollbar, h - sbarwid, 0);
	  SizeControl(console_v_scrollbar, sbarwid + 1, v - sbarwid + 1);
	}
      InvalRect (&win->portRect);
      SetPort (oldport);
    }
}

zoom_window (win, where, part)
WindowPtr win;
Point where;
short part;
{
}

close_window (win)
WindowPtr win;
{
}

do_mouse_down (win, event)
WindowPtr win;
EventRecord *event;
{
  short part;
  Point mouse;
  ControlHandle control;

  if (1 /*is_app_window(win)*/)
    {
      SetPort (win);
      mouse = event->where;
      GlobalToLocal (&mouse);
      part = FindControl(mouse, win, &control);
      if (control == console_v_scrollbar)
	{
	  SysBeep(20);
	}
      else
	{
	  TEClick (mouse, 0, console_text);
	}
    }
}

activate_window (win, activate)
WindowPtr win;
int activate;
{
  if (win == nil) return;
  /* It's convenient to make the activated window also be the
     current GrafPort. */
  if (activate)
    SetPort(win);
  /* Activate the console window's scrollbar. */
  if (win == console_window)
    HiliteControl (console_v_scrollbar, (activate ? 0 : 255));
}

update_window (win)
WindowPtr win;
{
  int controls = 1, growbox = 0;
  GrafPtr oldport;

  /* Set the updating window to be the current grafport. */
  GetPort (&oldport);
  SetPort (win);
/*  recalc_depths();  */
  BeginUpdate (win);
  if (win == console_window)
    {
      draw_console ();
      controls = 1;
      growbox = 1;
    }
  if (controls)
    UpdateControls (win, win->visRgn);
  if (growbox)
    DrawGrowIcon (win);
  EndUpdate (win);
  SetPort (oldport);
}

adjust_menus ()
{
}

do_menu_command (which)
long which;
{
  short menuid, menuitem;
  short itemHit;
  Str255 daname;
  short daRefNum;
  Boolean handledbyda;
  WindowPtr win;
  short ditem;
  int i;

  menuid = HiWord (which);
  menuitem = LoWord (which);
  switch (menuid)
    {
    case mApple:
      switch (menuitem)
	{
	case miAbout:
/*	  Alert(aAbout, nil); */
	  break;
	default:
	  GetItem (GetMHandle (mApple), menuitem, daname);
	  daRefNum = OpenDeskAcc (daname);
	}
      break;
    case mFile:
      switch (menuitem)
	{
	case miFileQuit:
	  ExitToShell ();
	  break;
	}
      break;
    case mEdit:
      /* handledbyda = SystemEdit(menuitem-1); */
      switch (menuitem)
	{
	case miEditCut:
	  break;
	case miEditCopy:
	  break;
	case miEditPaste:
	  break;
	case miEditClear:
	  break;
	}
      break;
    }
  HiliteMenu (0);
}

char commandbuf[1000];

do_keyboard_command (key)
char key;
{
  int startpos, endpos, i;
  char buf[10], *text_str, *command;
  CharsHandle text;

  if (key == '\015' || key == '\003')
    {
      /* (should) Interpret the line as a command. */
      text = TEGetText (console_text);
      HLock ((Handle) text);
      startpos = (*console_text)->selStart;
      endpos = (*console_text)->selEnd;
      if (startpos != endpos)
	{
	  strncpy (commandbuf + 1, *text + startpos, endpos - startpos);
	  commandbuf[1 + endpos - startpos] = 0;
	  command = commandbuf + 1;
	}
      else
	{
	  for (i = startpos; i > 0; --i)
	    {
	      strncpy (buf, *text + i, 5);
	      buf[5] = 0;
	      if (strncmp (buf, "(gdb)") == 0)
		break;
	    }
	  if (i > 0)
	    {
	      strncpy (commandbuf + 1, *text + i + 5, startpos - i);
	      commandbuf[1 + startpos - i] = '\0';
	    }
	  else
	    {
	      SysBeep (20);
	      commandbuf[1] = '\0';
	    }
	  command = commandbuf + 1;
	}
      HUnlock ((Handle) text);
      commandbuf[0] = strlen(command);
      DebugStr(commandbuf);

      /* Insert a newline and redraw before doing the command. */
      buf[0] = '\015';
      TEInsert (buf, 1, console_text);
      TESetSelect (100000, 100000, console_text);
      draw_console ();

      execute_command (commandbuf, 0);
      bpstat_do_actions (&stop_bpstat);
    }
  else if (0 /* editing chars... */)
    {
    }
  else
    {
      /* A self-inserting character. */
      buf[0] = key;
      TEInsert (buf, 1, console_text);
      TESetSelect (100000, 100000, console_text);
      draw_console ();
    }
}

draw_console ()
{
  GrafPtr oldport;

  GetPort (&oldport);
  SetPort (console_window);
  TEUpdate (&(console_window->portRect), console_text);
  SetPort (oldport);
/*	adjust_help_scrollbar();  */
}

/* Cause an update of a window's entire contents. */

force_update (win)
WindowPtr win;
{
  GrafPtr oldport;

  if (win == nil) return;
  GetPort (&oldport);
  SetPort (win);
  EraseRect (&win->portRect);
  InvalRect (&win->portRect);
  SetPort (oldport);
}

adjust_console_scrollbars ()
{
  int lines, newmax, value;

  lines = (*console_text)->nLines;
  newmax = lines - (((*console_text)->viewRect.bottom - (*console_text)->viewRect.top)
		    / (*console_text)->lineHeight);
  if (newmax < 0) newmax = 0;
  SetCtlMax(console_v_scrollbar, newmax);
  value = ((*console_text)->viewRect.top - (*console_text)->destRect.top)
    / (*console_text)->lineHeight;
  SetCtlValue(console_v_scrollbar, value);
}
