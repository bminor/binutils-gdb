/* OBSOLETE /* Top level support for Mac interface to GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1994, 1995, 1998, 2000, 2001 Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by Cygnus Support.  Written by Stan Shebs. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE  */
/* OBSOLETE #include <readline/readline.h> */
/* OBSOLETE #include <readline/history.h> */
/* OBSOLETE  */
/* OBSOLETE #include <Types.h> */
/* OBSOLETE #include <Resources.h> */
/* OBSOLETE #include <QuickDraw.h> */
/* OBSOLETE #include <Fonts.h> */
/* OBSOLETE #include <Events.h> */
/* OBSOLETE #include <Windows.h> */
/* OBSOLETE #include <Menus.h> */
/* OBSOLETE #include <TextEdit.h> */
/* OBSOLETE #include <Dialogs.h> */
/* OBSOLETE #include <Desk.h> */
/* OBSOLETE #include <ToolUtils.h> */
/* OBSOLETE #include <Memory.h> */
/* OBSOLETE #include <SegLoad.h> */
/* OBSOLETE #include <Files.h> */
/* OBSOLETE #include <Folders.h> */
/* OBSOLETE #include <OSUtils.h> */
/* OBSOLETE #include <OSEvents.h> */
/* OBSOLETE #include <DiskInit.h> */
/* OBSOLETE #include <Packages.h> */
/* OBSOLETE #include <Traps.h> */
/* OBSOLETE #include <Lists.h> */
/* OBSOLETE #include <Gestalt.h> */
/* OBSOLETE #include <PPCToolbox.h> */
/* OBSOLETE #include <AppleEvents.h> */
/* OBSOLETE #include <StandardFile.h> */
/* OBSOLETE #include <Sound.h> */
/* OBSOLETE  */
/* OBSOLETE #ifdef MPW */
/* OBSOLETE #define QD(whatever) (qd.##whatever) */
/* OBSOLETE #define QDPat(whatever) (&(qd.##whatever)) */
/* OBSOLETE #endif /* MPW */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef THINK_C */
/* OBSOLETE #define QD(whatever) (whatever) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #define p2c(pstr,cbuf)  \ */
/* OBSOLETE   strncpy(cbuf, ((char *) (pstr) + 1), pstr[0]);  \ */
/* OBSOLETE   cbuf[pstr[0]] = '\0'; */
/* OBSOLETE  */
/* OBSOLETE #define pascalify(STR) \ */
/* OBSOLETE   sprintf(tmpbuf, " %s", STR);  \ */
/* OBSOLETE   tmpbuf[0] = strlen(STR); */
/* OBSOLETE  */
/* OBSOLETE #include "gdbcmd.h" */
/* OBSOLETE #include "call-cmds.h" */
/* OBSOLETE #include "symtab.h" */
/* OBSOLETE #include "inferior.h" */
/* OBSOLETE #include <signal.h> */
/* OBSOLETE #include "target.h" */
/* OBSOLETE #include "breakpoint.h" */
/* OBSOLETE #include "gdbtypes.h" */
/* OBSOLETE #include "expression.h" */
/* OBSOLETE #include "language.h" */
/* OBSOLETE  */
/* OBSOLETE #include "mac-defs.h" */
/* OBSOLETE  */
/* OBSOLETE int debug_openp = 0; */
/* OBSOLETE  */
/* OBSOLETE /* This is true if we are running as a standalone application.  */ */
/* OBSOLETE  */
/* OBSOLETE int mac_app; */
/* OBSOLETE  */
/* OBSOLETE /* This is true if we are using WaitNextEvent.  */ */
/* OBSOLETE  */
/* OBSOLETE int use_wne; */
/* OBSOLETE  */
/* OBSOLETE /* This is true if we have Color Quickdraw.  */ */
/* OBSOLETE  */
/* OBSOLETE int has_color_qd; */
/* OBSOLETE  */
/* OBSOLETE /* This is true if we are using Color Quickdraw. */ */
/* OBSOLETE  */
/* OBSOLETE int use_color_qd; */
/* OBSOLETE  */
/* OBSOLETE int inbackground; */
/* OBSOLETE  */
/* OBSOLETE Rect dragrect = */
/* OBSOLETE {-32000, -32000, 32000, 32000}; */
/* OBSOLETE Rect sizerect; */
/* OBSOLETE  */
/* OBSOLETE int sbarwid = 15; */
/* OBSOLETE  */
/* OBSOLETE /* Globals for the console window. */ */
/* OBSOLETE  */
/* OBSOLETE WindowPtr console_window; */
/* OBSOLETE  */
/* OBSOLETE ControlHandle console_v_scrollbar; */
/* OBSOLETE  */
/* OBSOLETE Rect console_v_scroll_rect; */
/* OBSOLETE  */
/* OBSOLETE TEHandle console_text; */
/* OBSOLETE  */
/* OBSOLETE Rect console_text_rect; */
/* OBSOLETE  */
/* OBSOLETE /* This will go away eventually. */ */
/* OBSOLETE gdb_has_a_terminal (void) */
/* OBSOLETE { */
/* OBSOLETE   return 1; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE mac_init (void) */
/* OBSOLETE { */
/* OBSOLETE   SysEnvRec se; */
/* OBSOLETE   int eventloopdone = 0; */
/* OBSOLETE   char *str; */
/* OBSOLETE   Boolean gotevent; */
/* OBSOLETE   Point mouse; */
/* OBSOLETE   EventRecord event; */
/* OBSOLETE   WindowPtr win; */
/* OBSOLETE   RgnHandle cursorRgn; */
/* OBSOLETE   int i; */
/* OBSOLETE   Handle menubar; */
/* OBSOLETE   MenuHandle menu; */
/* OBSOLETE   Handle siow_resource; */
/* OBSOLETE  */
/* OBSOLETE   mac_app = 0; */
/* OBSOLETE  */
/* OBSOLETE   str = getenv ("DEBUG_GDB"); */
/* OBSOLETE   if (str != NULL && str[0] != '\0') */
/* OBSOLETE     { */
/* OBSOLETE       if (strcmp (str, "openp") == 0) */
/* OBSOLETE 	debug_openp = 1; */
/* OBSOLETE     } */
/* OBSOLETE  */
/* OBSOLETE   /* Don't do anything if we`re running under MPW. */ */
/* OBSOLETE   if (!StandAlone) */
/* OBSOLETE     return; */
/* OBSOLETE  */
/* OBSOLETE   /* Don't do anything if we're using SIOW. */ */
/* OBSOLETE   /* This test requires that the siow 0 resource, as defined in */
/* OBSOLETE      {RIncludes}siow.r, not be messed with.  If it is, then the */
/* OBSOLETE      standard Mac setup below will step on SIOW's Mac setup and */
/* OBSOLETE      most likely crash the machine. */ */
/* OBSOLETE   siow_resource = GetResource ('siow', 0); */
/* OBSOLETE   if (siow_resource != nil) */
/* OBSOLETE     return; */
/* OBSOLETE  */
/* OBSOLETE   mac_app = 1; */
/* OBSOLETE  */
/* OBSOLETE   /* Do the standard Mac environment setup. */ */
/* OBSOLETE   InitGraf (&QD (thePort)); */
/* OBSOLETE   InitFonts (); */
/* OBSOLETE   FlushEvents (everyEvent, 0); */
/* OBSOLETE   InitWindows (); */
/* OBSOLETE   InitMenus (); */
/* OBSOLETE   TEInit (); */
/* OBSOLETE   InitDialogs (NULL); */
/* OBSOLETE   InitCursor (); */
/* OBSOLETE  */
/* OBSOLETE   /* Color Quickdraw is different from Classic QD. */ */
/* OBSOLETE   SysEnvirons (2, &se); */
/* OBSOLETE   has_color_qd = se.hasColorQD; */
/* OBSOLETE   /* Use it if we got it. */ */
/* OBSOLETE   use_color_qd = has_color_qd; */
/* OBSOLETE  */
/* OBSOLETE   sizerect.top = 50; */
/* OBSOLETE   sizerect.left = 50; */
/* OBSOLETE   sizerect.bottom = 1000; */
/* OBSOLETE   sizerect.right = 1000; */
/* OBSOLETE #if 0 */
/* OBSOLETE   sizerect.bottom = screenBits.bounds.bottom - screenBits.bounds.top; */
/* OBSOLETE   sizerect.right = screenBits.bounds.right - screenBits.bounds.left; */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE   /* Set up the menus. */ */
/* OBSOLETE   menubar = GetNewMBar (mbMain); */
/* OBSOLETE   SetMenuBar (menubar); */
/* OBSOLETE   /* Add the DAs etc as usual. */ */
/* OBSOLETE   menu = GetMHandle (mApple); */
/* OBSOLETE   if (menu != nil) */
/* OBSOLETE     { */
/* OBSOLETE       AddResMenu (menu, 'DRVR'); */
/* OBSOLETE     } */
/* OBSOLETE   DrawMenuBar (); */
/* OBSOLETE  */
/* OBSOLETE   new_console_window (); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE new_console_window (void) */
/* OBSOLETE { */
/* OBSOLETE   /* Create the main window we're going to play in. */ */
/* OBSOLETE   if (has_color_qd) */
/* OBSOLETE     console_window = GetNewCWindow (wConsole, NULL, (WindowPtr) - 1L); */
/* OBSOLETE   else */
/* OBSOLETE     console_window = GetNewWindow (wConsole, NULL, (WindowPtr) - 1L); */
/* OBSOLETE  */
/* OBSOLETE   SetPort (console_window); */
/* OBSOLETE   console_text_rect = console_window->portRect; */
/* OBSOLETE   /* Leave 8 pixels of blank space, for aesthetic reasons and to */
/* OBSOLETE      make it easier to select from the beginning of a line. */ */
/* OBSOLETE   console_text_rect.left += 8; */
/* OBSOLETE   console_text_rect.bottom -= sbarwid - 1; */
/* OBSOLETE   console_text_rect.right -= sbarwid - 1; */
/* OBSOLETE   console_text = TENew (&console_text_rect, &console_text_rect); */
/* OBSOLETE   TESetSelect (0, 40000, console_text); */
/* OBSOLETE   TEDelete (console_text); */
/* OBSOLETE   TEAutoView (1, console_text); */
/* OBSOLETE  */
/* OBSOLETE   console_v_scroll_rect = console_window->portRect; */
/* OBSOLETE   console_v_scroll_rect.bottom -= sbarwid - 1; */
/* OBSOLETE   console_v_scroll_rect.left = console_v_scroll_rect.right - sbarwid; */
/* OBSOLETE   console_v_scrollbar = */
/* OBSOLETE     NewControl (console_window, &console_v_scroll_rect, */
/* OBSOLETE 		"\p", 1, 0, 0, 0, scrollBarProc, 0L); */
/* OBSOLETE  */
/* OBSOLETE   ShowWindow (console_window); */
/* OBSOLETE   SelectWindow (console_window); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE mac_command_loop (void) */
/* OBSOLETE { */
/* OBSOLETE   SysEnvRec se; */
/* OBSOLETE   int eventloopdone = 0; */
/* OBSOLETE   Boolean gotevent; */
/* OBSOLETE   Point mouse; */
/* OBSOLETE   EventRecord event; */
/* OBSOLETE   WindowPtr win; */
/* OBSOLETE   RgnHandle cursorRgn; */
/* OBSOLETE   int i, tm; */
/* OBSOLETE   Handle menubar; */
/* OBSOLETE   MenuHandle menu; */
/* OBSOLETE  */
/* OBSOLETE   /* Figure out if the WaitNextEvent Trap is available.  */ */
/* OBSOLETE   use_wne = */
/* OBSOLETE     (NGetTrapAddress (0x60, ToolTrap) != NGetTrapAddress (0x9f, ToolTrap)); */
/* OBSOLETE   /* Pass WaitNextEvent an empty region the first time through.  */ */
/* OBSOLETE   cursorRgn = NewRgn (); */
/* OBSOLETE   /* Go into the main event-handling loop.  */ */
/* OBSOLETE   while (!eventloopdone) */
/* OBSOLETE     { */
/* OBSOLETE       /* Use WaitNextEvent if it is available, otherwise GetNextEvent.  */ */
/* OBSOLETE       if (use_wne) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  get_global_mouse (&mouse); */
/* OBSOLETE 	  adjust_cursor (mouse, cursorRgn); */
/* OBSOLETE 	  tm = GetCaretTime (); */
/* OBSOLETE 	  gotevent = WaitNextEvent (everyEvent, &event, tm, cursorRgn); */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  SystemTask (); */
/* OBSOLETE 	  gotevent = GetNextEvent (everyEvent, &event); */
/* OBSOLETE 	} */
/* OBSOLETE       /* First decide if the event is for a dialog or is just any old event. */ */
/* OBSOLETE       if (FrontWindow () != nil && IsDialogEvent (&event)) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  short itemhit; */
/* OBSOLETE 	  DialogPtr dialog; */
/* OBSOLETE  */
/* OBSOLETE 	  /* Handle all the modeless dialogs here. */ */
/* OBSOLETE 	  if (DialogSelect (&event, &dialog, &itemhit)) */
/* OBSOLETE 	    { */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE       else if (gotevent) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  /* Make sure we have the right cursor before handling the event. */ */
/* OBSOLETE 	  adjust_cursor (event.where, cursorRgn); */
/* OBSOLETE 	  do_event (&event); */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  do_idle (); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Collect the global coordinates of the mouse pointer.  */ */
/* OBSOLETE  */
/* OBSOLETE get_global_mouse (Point *mouse) */
/* OBSOLETE { */
/* OBSOLETE   EventRecord evt; */
/* OBSOLETE  */
/* OBSOLETE   OSEventAvail (0, &evt); */
/* OBSOLETE   *mouse = evt.where; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Change the cursor's appearance to be appropriate for the given mouse */
/* OBSOLETE    location.  */ */
/* OBSOLETE  */
/* OBSOLETE adjust_cursor (Point mouse, RgnHandle region) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Decipher an event, maybe do something with it.  */ */
/* OBSOLETE  */
/* OBSOLETE do_event (EventRecord *evt) */
/* OBSOLETE { */
/* OBSOLETE   short part, err, rslt = 0; */
/* OBSOLETE   WindowPtr win; */
/* OBSOLETE   Boolean hit; */
/* OBSOLETE   char key; */
/* OBSOLETE   Point pnt; */
/* OBSOLETE  */
/* OBSOLETE   switch (evt->what) */
/* OBSOLETE     { */
/* OBSOLETE     case mouseDown: */
/* OBSOLETE       /* See if the click happened in a special part of the screen. */ */
/* OBSOLETE       part = FindWindow (evt->where, &win); */
/* OBSOLETE       switch (part) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case inMenuBar: */
/* OBSOLETE 	  adjust_menus (); */
/* OBSOLETE 	  do_menu_command (MenuSelect (evt->where)); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inSysWindow: */
/* OBSOLETE 	  SystemClick (evt, win); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inContent: */
/* OBSOLETE 	  if (win != FrontWindow ()) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      /* Bring the clicked-on window to the front. */ */
/* OBSOLETE 	      SelectWindow (win); */
/* OBSOLETE 	      /* Fix the menu to match the new front window. */ */
/* OBSOLETE 	      adjust_menus (); */
/* OBSOLETE 	      /* We always want to discard the event now, since clicks in a */
/* OBSOLETE 	         windows are often irreversible actions. */ */
/* OBSOLETE 	    } */
/* OBSOLETE 	  else */
/* OBSOLETE 	    /* Mouse clicks in the front window do something useful. */ */
/* OBSOLETE 	    do_mouse_down (win, evt); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inDrag: */
/* OBSOLETE 	  /* Standard drag behavior, no tricks necessary. */ */
/* OBSOLETE 	  DragWindow (win, evt->where, &dragrect); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inGrow: */
/* OBSOLETE 	  grow_window (win, evt->where); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inZoomIn: */
/* OBSOLETE 	case inZoomOut: */
/* OBSOLETE 	  zoom_window (win, evt->where, part); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inGoAway: */
/* OBSOLETE 	  close_window (win); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case keyDown: */
/* OBSOLETE     case autoKey: */
/* OBSOLETE       key = evt->message & charCodeMask; */
/* OBSOLETE       /* Check for menukey equivalents. */ */
/* OBSOLETE       if (evt->modifiers & cmdKey) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (evt->what == keyDown) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      adjust_menus (); */
/* OBSOLETE 	      do_menu_command (MenuKey (key)); */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  if (evt->what == keyDown) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      /* Random keypress, interpret it. */ */
/* OBSOLETE 	      do_keyboard_command (key); */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case activateEvt: */
/* OBSOLETE       activate_window ((WindowPtr) evt->message, evt->modifiers & activeFlag); */
/* OBSOLETE       break; */
/* OBSOLETE     case updateEvt: */
/* OBSOLETE       update_window ((WindowPtr) evt->message); */
/* OBSOLETE       break; */
/* OBSOLETE     case diskEvt: */
/* OBSOLETE       /* Call DIBadMount in response to a diskEvt, so that the user can format */
/* OBSOLETE          a floppy. (from DTS Sample) */ */
/* OBSOLETE       if (HiWord (evt->message) != noErr) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  SetPt (&pnt, 50, 50); */
/* OBSOLETE 	  err = DIBadMount (pnt, evt->message); */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case app4Evt: */
/* OBSOLETE       /* Grab only a single byte. */ */
/* OBSOLETE       switch ((evt->message >> 24) & 0xFF) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case 0xfa: */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case 1: */
/* OBSOLETE 	  inbackground = !(evt->message & 1); */
/* OBSOLETE 	  activate_window (FrontWindow (), !inbackground); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case kHighLevelEvent: */
/* OBSOLETE       AEProcessAppleEvent (evt); */
/* OBSOLETE       break; */
/* OBSOLETE     case nullEvent: */
/* OBSOLETE       do_idle (); */
/* OBSOLETE       rslt = 1; */
/* OBSOLETE       break; */
/* OBSOLETE     default: */
/* OBSOLETE       break; */
/* OBSOLETE     } */
/* OBSOLETE   return rslt; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Do any idle-time activities. */ */
/* OBSOLETE  */
/* OBSOLETE do_idle (void) */
/* OBSOLETE { */
/* OBSOLETE   TEIdle (console_text); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE grow_window (WindowPtr win, Point where) */
/* OBSOLETE { */
/* OBSOLETE   long winsize; */
/* OBSOLETE   int h, v; */
/* OBSOLETE   GrafPtr oldport; */
/* OBSOLETE  */
/* OBSOLETE   winsize = GrowWindow (win, where, &sizerect); */
/* OBSOLETE   /* Only do anything if it actually changed size. */ */
/* OBSOLETE   if (winsize != 0) */
/* OBSOLETE     { */
/* OBSOLETE       GetPort (&oldport); */
/* OBSOLETE       SetPort (win); */
/* OBSOLETE       if (win == console_window) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  EraseRect (&win->portRect); */
/* OBSOLETE 	  h = LoWord (winsize); */
/* OBSOLETE 	  v = HiWord (winsize); */
/* OBSOLETE 	  SizeWindow (win, h, v, 1); */
/* OBSOLETE 	  resize_console_window (); */
/* OBSOLETE 	} */
/* OBSOLETE       SetPort (oldport); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE zoom_window (WindowPtr win, Point where, short part) */
/* OBSOLETE { */
/* OBSOLETE   ZoomWindow (win, part, (win == FrontWindow ())); */
/* OBSOLETE   if (win == console_window) */
/* OBSOLETE     { */
/* OBSOLETE       resize_console_window (); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE resize_console_window (void) */
/* OBSOLETE { */
/* OBSOLETE   adjust_console_sizes (); */
/* OBSOLETE   adjust_console_scrollbars (); */
/* OBSOLETE   adjust_console_text (); */
/* OBSOLETE   InvalRect (&console_window->portRect); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE close_window (WindowPtr win) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE pascal void */
/* OBSOLETE v_scroll_proc (ControlHandle control, short part) */
/* OBSOLETE { */
/* OBSOLETE   int oldval, amount = 0, newval; */
/* OBSOLETE   int pagesize = ((*console_text)->viewRect.bottom - (*console_text)->viewRect.top) / (*console_text)->lineHeight; */
/* OBSOLETE   if (part) */
/* OBSOLETE     { */
/* OBSOLETE       oldval = GetCtlValue (control); */
/* OBSOLETE       switch (part) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case inUpButton: */
/* OBSOLETE 	  amount = 1; */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inDownButton: */
/* OBSOLETE 	  amount = -1; */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inPageUp: */
/* OBSOLETE 	  amount = pagesize; */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case inPageDown: */
/* OBSOLETE 	  amount = -pagesize; */
/* OBSOLETE 	  break; */
/* OBSOLETE 	default: */
/* OBSOLETE 	  /* (should freak out) */ */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       SetCtlValue (control, oldval - amount); */
/* OBSOLETE       newval = GetCtlValue (control); */
/* OBSOLETE       amount = oldval - newval; */
/* OBSOLETE       if (amount) */
/* OBSOLETE 	TEScroll (0, amount * (*console_text)->lineHeight, console_text); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE do_mouse_down (WindowPtr win, EventRecord * event) */
/* OBSOLETE { */
/* OBSOLETE   short part, value; */
/* OBSOLETE   Point mouse; */
/* OBSOLETE   ControlHandle control; */
/* OBSOLETE  */
/* OBSOLETE   if (1 /*is_app_window(win) */ ) */
/* OBSOLETE     { */
/* OBSOLETE       SetPort (win); */
/* OBSOLETE       mouse = event->where; */
/* OBSOLETE       GlobalToLocal (&mouse); */
/* OBSOLETE       part = FindControl (mouse, win, &control); */
/* OBSOLETE       if (control == console_v_scrollbar) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  switch (part) */
/* OBSOLETE 	    { */
/* OBSOLETE 	    case inThumb: */
/* OBSOLETE 	      value = GetCtlValue (control); */
/* OBSOLETE 	      part = TrackControl (control, mouse, nil); */
/* OBSOLETE 	      if (part) */
/* OBSOLETE 		{ */
/* OBSOLETE 		  value -= GetCtlValue (control); */
/* OBSOLETE 		  if (value) */
/* OBSOLETE 		    TEScroll (0, value * (*console_text)->lineHeight, */
/* OBSOLETE 			      console_text); */
/* OBSOLETE 		} */
/* OBSOLETE 	      break; */
/* OBSOLETE 	    default: */
/* OBSOLETE #if 0				/* don't deal with right now */ */
/* OBSOLETE #if 1				/* universal headers */ */
/* OBSOLETE 	      value = TrackControl (control, mouse, (ControlActionUPP) v_scroll_proc); */
/* OBSOLETE #else */
/* OBSOLETE 	      value = TrackControl (control, mouse, (ProcPtr) v_scroll_proc); */
/* OBSOLETE #endif */
/* OBSOLETE #endif */
/* OBSOLETE 	      break; */
/* OBSOLETE 	    } */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  TEClick (mouse, 0, console_text); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE scroll_text (int hlines, int vlines) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE activate_window (WindowPtr win, int activate) */
/* OBSOLETE { */
/* OBSOLETE   Rect grow_rect; */
/* OBSOLETE  */
/* OBSOLETE   if (win == nil) */
/* OBSOLETE     return; */
/* OBSOLETE   /* It's convenient to make the activated window also be the */
/* OBSOLETE      current GrafPort. */ */
/* OBSOLETE   if (activate) */
/* OBSOLETE     SetPort (win); */
/* OBSOLETE   /* Activate the console window's scrollbar. */ */
/* OBSOLETE   if (win == console_window) */
/* OBSOLETE     { */
/* OBSOLETE       if (activate) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  TEActivate (console_text); */
/* OBSOLETE 	  /* Cause the grow icon to be redrawn at the next update. */ */
/* OBSOLETE 	  grow_rect = console_window->portRect; */
/* OBSOLETE 	  grow_rect.top = grow_rect.bottom - sbarwid; */
/* OBSOLETE 	  grow_rect.left = grow_rect.right - sbarwid; */
/* OBSOLETE 	  InvalRect (&grow_rect); */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  TEDeactivate (console_text); */
/* OBSOLETE 	  DrawGrowIcon (console_window); */
/* OBSOLETE 	} */
/* OBSOLETE       HiliteControl (console_v_scrollbar, (activate ? 0 : 255)); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE update_window (WindowPtr win) */
/* OBSOLETE { */
/* OBSOLETE   int controls = 1, growbox = 0; */
/* OBSOLETE   GrafPtr oldport; */
/* OBSOLETE  */
/* OBSOLETE   /* Set the updating window to be the current grafport. */ */
/* OBSOLETE   GetPort (&oldport); */
/* OBSOLETE   SetPort (win); */
/* OBSOLETE /*  recalc_depths();  */ */
/* OBSOLETE   BeginUpdate (win); */
/* OBSOLETE   if (win == console_window) */
/* OBSOLETE     { */
/* OBSOLETE       draw_console (); */
/* OBSOLETE       controls = 1; */
/* OBSOLETE       growbox = 1; */
/* OBSOLETE     } */
/* OBSOLETE   if (controls) */
/* OBSOLETE     UpdateControls (win, win->visRgn); */
/* OBSOLETE   if (growbox) */
/* OBSOLETE     DrawGrowIcon (win); */
/* OBSOLETE   EndUpdate (win); */
/* OBSOLETE   SetPort (oldport); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE adjust_menus (void) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE do_menu_command (long which) */
/* OBSOLETE { */
/* OBSOLETE   short menuid, menuitem; */
/* OBSOLETE   short itemHit; */
/* OBSOLETE   Str255 daname; */
/* OBSOLETE   short daRefNum; */
/* OBSOLETE   Boolean handledbyda; */
/* OBSOLETE   WindowPtr win; */
/* OBSOLETE   short ditem; */
/* OBSOLETE   int i; */
/* OBSOLETE   char cmdbuf[300]; */
/* OBSOLETE  */
/* OBSOLETE   cmdbuf[0] = '\0'; */
/* OBSOLETE   menuid = HiWord (which); */
/* OBSOLETE   menuitem = LoWord (which); */
/* OBSOLETE   switch (menuid) */
/* OBSOLETE     { */
/* OBSOLETE     case mApple: */
/* OBSOLETE       switch (menuitem) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case miAbout: */
/* OBSOLETE 	  Alert (128, nil); */
/* OBSOLETE 	  break; */
/* OBSOLETE #if 0 */
/* OBSOLETE 	case miHelp: */
/* OBSOLETE 	  /* (should pop up help info) */ */
/* OBSOLETE 	  break; */
/* OBSOLETE #endif */
/* OBSOLETE 	default: */
/* OBSOLETE 	  GetItem (GetMHandle (mApple), menuitem, daname); */
/* OBSOLETE 	  daRefNum = OpenDeskAcc (daname); */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case mFile: */
/* OBSOLETE       switch (menuitem) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case miFileNew: */
/* OBSOLETE 	  if (console_window == FrontWindow ()) */
/* OBSOLETE 	    { */
/* OBSOLETE 	      close_window (console_window); */
/* OBSOLETE 	    } */
/* OBSOLETE 	  new_console_window (); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miFileOpen: */
/* OBSOLETE 	  SysBeep (20); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miFileQuit: */
/* OBSOLETE 	  ExitToShell (); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     case mEdit: */
/* OBSOLETE       /* handledbyda = SystemEdit(menuitem-1); */ */
/* OBSOLETE       switch (menuitem) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case miEditCut: */
/* OBSOLETE 	  TECut (console_text); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miEditCopy: */
/* OBSOLETE 	  TECopy (console_text); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miEditPaste: */
/* OBSOLETE 	  TEPaste (console_text); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miEditClear: */
/* OBSOLETE 	  TEDelete (console_text); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       /* All of these operations need the same postprocessing. */ */
/* OBSOLETE       adjust_console_sizes (); */
/* OBSOLETE       adjust_console_scrollbars (); */
/* OBSOLETE       adjust_console_text (); */
/* OBSOLETE       break; */
/* OBSOLETE     case mDebug: */
/* OBSOLETE       switch (menuitem) */
/* OBSOLETE 	{ */
/* OBSOLETE 	case miDebugTarget: */
/* OBSOLETE 	  sprintf (cmdbuf, "target %s", "remote"); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miDebugRun: */
/* OBSOLETE 	  sprintf (cmdbuf, "run"); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miDebugContinue: */
/* OBSOLETE 	  sprintf (cmdbuf, "continue"); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miDebugStep: */
/* OBSOLETE 	  sprintf (cmdbuf, "step"); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	case miDebugNext: */
/* OBSOLETE 	  sprintf (cmdbuf, "next"); */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE       break; */
/* OBSOLETE     } */
/* OBSOLETE   HiliteMenu (0); */
/* OBSOLETE   /* Execute a command if one had been given.  Do here because a command */
/* OBSOLETE      may longjmp before we get a chance to unhilite the menu. */ */
/* OBSOLETE   if (strlen (cmdbuf) > 0) */
/* OBSOLETE     execute_command (cmdbuf, 0); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE char commandbuf[1000]; */
/* OBSOLETE  */
/* OBSOLETE do_keyboard_command (int key) */
/* OBSOLETE { */
/* OBSOLETE   int startpos, endpos, i, len; */
/* OBSOLETE   char *last_newline; */
/* OBSOLETE   char buf[10], *text_str, *command, *cmd_start; */
/* OBSOLETE   CharsHandle text; */
/* OBSOLETE  */
/* OBSOLETE   if (key == '\015' || key == '\003') */
/* OBSOLETE     { */
/* OBSOLETE       text = TEGetText (console_text); */
/* OBSOLETE       HLock ((Handle) text); */
/* OBSOLETE       text_str = *text; */
/* OBSOLETE       startpos = (*console_text)->selStart; */
/* OBSOLETE       endpos = (*console_text)->selEnd; */
/* OBSOLETE       if (startpos != endpos) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  len = endpos - startpos; */
/* OBSOLETE 	  cmd_start = text_str + startpos; */
/* OBSOLETE 	} */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  for (i = startpos - 1; i >= 0; --i) */
/* OBSOLETE 	    if (text_str[i] == '\015') */
/* OBSOLETE 	      break; */
/* OBSOLETE 	  last_newline = text_str + i; */
/* OBSOLETE 	  len = (text_str + startpos) - 1 - last_newline; */
/* OBSOLETE 	  cmd_start = last_newline + 1; */
/* OBSOLETE 	} */
/* OBSOLETE       if (len > 1000) */
/* OBSOLETE 	len = 999; */
/* OBSOLETE       if (len < 0) */
/* OBSOLETE 	len = 0; */
/* OBSOLETE       strncpy (commandbuf + 1, cmd_start, len); */
/* OBSOLETE       commandbuf[1 + len] = 0; */
/* OBSOLETE       command = commandbuf + 1; */
/* OBSOLETE       HUnlock ((Handle) text); */
/* OBSOLETE       commandbuf[0] = strlen (command); */
/* OBSOLETE  */
/* OBSOLETE       /* Insert a newline and recalculate before doing any command. */ */
/* OBSOLETE       key = '\015'; */
/* OBSOLETE       TEKey (key, console_text); */
/* OBSOLETE       TEInsert (buf, 1, console_text); */
/* OBSOLETE       adjust_console_sizes (); */
/* OBSOLETE       adjust_console_scrollbars (); */
/* OBSOLETE       adjust_console_text (); */
/* OBSOLETE  */
/* OBSOLETE       if (strlen (command) > 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  execute_command (command, 0); */
/* OBSOLETE 	  bpstat_do_actions (&stop_bpstat); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       /* A self-inserting character.  This includes delete.  */ */
/* OBSOLETE       TEKey (key, console_text); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Draw all graphical stuff in the console window.  */ */
/* OBSOLETE  */
/* OBSOLETE draw_console (void) */
/* OBSOLETE { */
/* OBSOLETE   SetPort (console_window); */
/* OBSOLETE   TEUpdate (&(console_window->portRect), console_text); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Cause an update of a given window's entire contents.  */ */
/* OBSOLETE  */
/* OBSOLETE force_update (WindowPtr win) */
/* OBSOLETE { */
/* OBSOLETE   GrafPtr oldport; */
/* OBSOLETE  */
/* OBSOLETE   if (win == nil) */
/* OBSOLETE     return; */
/* OBSOLETE   GetPort (&oldport); */
/* OBSOLETE   SetPort (win); */
/* OBSOLETE   EraseRect (&win->portRect); */
/* OBSOLETE   InvalRect (&win->portRect); */
/* OBSOLETE   SetPort (oldport); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE adjust_console_sizes (void) */
/* OBSOLETE { */
/* OBSOLETE   Rect tmprect; */
/* OBSOLETE  */
/* OBSOLETE   tmprect = console_window->portRect; */
/* OBSOLETE   /* Move and size the scrollbar. */ */
/* OBSOLETE   MoveControl (console_v_scrollbar, tmprect.right - sbarwid, 0); */
/* OBSOLETE   SizeControl (console_v_scrollbar, sbarwid + 1, tmprect.bottom - sbarwid + 1); */
/* OBSOLETE   /* Move and size the text. */ */
/* OBSOLETE   tmprect.left += 7; */
/* OBSOLETE   tmprect.right -= sbarwid; */
/* OBSOLETE   tmprect.bottom -= sbarwid; */
/* OBSOLETE   InsetRect (&tmprect, 1, 1); */
/* OBSOLETE   (*console_text)->destRect = tmprect; */
/* OBSOLETE   /* Fiddle bottom of viewrect to be even multiple of text lines. */ */
/* OBSOLETE   tmprect.bottom = tmprect.top */
/* OBSOLETE     + ((tmprect.bottom - tmprect.top) / (*console_text)->lineHeight) */
/* OBSOLETE     * (*console_text)->lineHeight; */
/* OBSOLETE   (*console_text)->viewRect = tmprect; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE adjust_console_scrollbars (void) */
/* OBSOLETE { */
/* OBSOLETE   int lines, newmax, value; */
/* OBSOLETE  */
/* OBSOLETE   (*console_v_scrollbar)->contrlVis = 0; */
/* OBSOLETE   lines = (*console_text)->nLines; */
/* OBSOLETE   newmax = lines - (((*console_text)->viewRect.bottom */
/* OBSOLETE 		     - (*console_text)->viewRect.top) */
/* OBSOLETE 		    / (*console_text)->lineHeight); */
/* OBSOLETE   if (newmax < 0) */
/* OBSOLETE     newmax = 0; */
/* OBSOLETE   SetCtlMax (console_v_scrollbar, newmax); */
/* OBSOLETE   value = ((*console_text)->viewRect.top - (*console_text)->destRect.top) */
/* OBSOLETE     / (*console_text)->lineHeight; */
/* OBSOLETE   SetCtlValue (console_v_scrollbar, value); */
/* OBSOLETE   (*console_v_scrollbar)->contrlVis = 0xff; */
/* OBSOLETE   ShowControl (console_v_scrollbar); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Scroll the TE record so that it is consistent with the scrollbar(s). */ */
/* OBSOLETE  */
/* OBSOLETE adjust_console_text (void) */
/* OBSOLETE { */
/* OBSOLETE   TEScroll (((*console_text)->viewRect.left */
/* OBSOLETE 	     - (*console_text)->destRect.left) */
/* OBSOLETE 	    - 0 /* get h scroll value */ , */
/* OBSOLETE 	    ((((*console_text)->viewRect.top - (*console_text)->destRect.top) */
/* OBSOLETE 	      / (*console_text)->lineHeight) */
/* OBSOLETE 	     - GetCtlValue (console_v_scrollbar)) */
/* OBSOLETE 	    * (*console_text)->lineHeight, */
/* OBSOLETE 	    console_text); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Readline substitute. */ */
/* OBSOLETE  */
/* OBSOLETE char * */
/* OBSOLETE readline (char *prrompt) */
/* OBSOLETE { */
/* OBSOLETE   return gdb_readline (prrompt); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE char *rl_completer_word_break_characters; */
/* OBSOLETE  */
/* OBSOLETE char *rl_completer_quote_characters; */
/* OBSOLETE  */
/* OBSOLETE int (*rl_completion_entry_function) (); */
/* OBSOLETE  */
/* OBSOLETE int rl_point; */
/* OBSOLETE  */
/* OBSOLETE char *rl_line_buffer; */
/* OBSOLETE  */
/* OBSOLETE char *rl_readline_name; */
/* OBSOLETE  */
/* OBSOLETE /* History substitute. */ */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE add_history (char *buf) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE stifle_history (int n) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE unstifle_history (void) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE read_history (char *name) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE write_history (char *name) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE history_expand (char *x, char **y) */
/* OBSOLETE { */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE extern HIST_ENTRY * */
/* OBSOLETE history_get (int xxx) */
/* OBSOLETE { */
/* OBSOLETE   return NULL; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int history_base; */
/* OBSOLETE  */
/* OBSOLETE char * */
/* OBSOLETE filename_completion_function (char *text, char *name) */
/* OBSOLETE { */
/* OBSOLETE   return "?"; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE char * */
/* OBSOLETE tilde_expand (char *str) */
/* OBSOLETE { */
/* OBSOLETE   return xstrdup (str); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Modified versions of standard I/O. */ */
/* OBSOLETE  */
/* OBSOLETE #undef fprintf */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE hacked_fprintf (FILE * fp, const char *fmt,...) */
/* OBSOLETE { */
/* OBSOLETE   int ret; */
/* OBSOLETE   va_list ap; */
/* OBSOLETE  */
/* OBSOLETE   va_start (ap, fmt); */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       char buf[1000]; */
/* OBSOLETE  */
/* OBSOLETE       ret = vsprintf (buf, fmt, ap); */
/* OBSOLETE       TEInsert (buf, strlen (buf), console_text); */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     ret = vfprintf (fp, fmt, ap); */
/* OBSOLETE   va_end (ap); */
/* OBSOLETE   return ret; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef printf */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE hacked_printf (const char *fmt,...) */
/* OBSOLETE { */
/* OBSOLETE   int ret; */
/* OBSOLETE   va_list ap; */
/* OBSOLETE  */
/* OBSOLETE   va_start (ap, fmt); */
/* OBSOLETE   ret = hacked_vfprintf (stdout, fmt, ap); */
/* OBSOLETE   va_end (ap); */
/* OBSOLETE   return ret; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef vfprintf */
/* OBSOLETE  */
/* OBSOLETE int */
/* OBSOLETE hacked_vfprintf (FILE * fp, const char *format, va_list args) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       char buf[1000]; */
/* OBSOLETE       int ret; */
/* OBSOLETE  */
/* OBSOLETE       ret = vsprintf (buf, format, args); */
/* OBSOLETE       TEInsert (buf, strlen (buf), console_text); */
/* OBSOLETE       if (strchr (buf, '\n')) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  adjust_console_sizes (); */
/* OBSOLETE 	  adjust_console_scrollbars (); */
/* OBSOLETE 	  adjust_console_text (); */
/* OBSOLETE 	} */
/* OBSOLETE       return ret; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     return vfprintf (fp, format, args); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef fputs */
/* OBSOLETE  */
/* OBSOLETE hacked_fputs (const char *s, FILE * fp) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       TEInsert (s, strlen (s), console_text); */
/* OBSOLETE       if (strchr (s, '\n')) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  adjust_console_sizes (); */
/* OBSOLETE 	  adjust_console_scrollbars (); */
/* OBSOLETE 	  adjust_console_text (); */
/* OBSOLETE 	} */
/* OBSOLETE       return 0; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     return fputs (s, fp); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef fputc */
/* OBSOLETE  */
/* OBSOLETE hacked_fputc (const char c, FILE * fp) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       char buf[1]; */
/* OBSOLETE  */
/* OBSOLETE       buf[0] = c; */
/* OBSOLETE       TEInsert (buf, 1, console_text); */
/* OBSOLETE       if (c == '\n') */
/* OBSOLETE 	{ */
/* OBSOLETE 	  adjust_console_sizes (); */
/* OBSOLETE 	  adjust_console_scrollbars (); */
/* OBSOLETE 	  adjust_console_text (); */
/* OBSOLETE 	} */
/* OBSOLETE       return c; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     return fputc (c, fp); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef putc */
/* OBSOLETE  */
/* OBSOLETE hacked_putc (const char c, FILE * fp) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       char buf[1]; */
/* OBSOLETE  */
/* OBSOLETE       buf[0] = c; */
/* OBSOLETE       TEInsert (buf, 1, console_text); */
/* OBSOLETE       if (c == '\n') */
/* OBSOLETE 	{ */
/* OBSOLETE 	  adjust_console_sizes (); */
/* OBSOLETE 	  adjust_console_scrollbars (); */
/* OBSOLETE 	  adjust_console_text (); */
/* OBSOLETE 	} */
/* OBSOLETE       return c; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     return fputc (c, fp); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef fflush */
/* OBSOLETE  */
/* OBSOLETE hacked_fflush (FILE * fp) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdout || fp == stderr)) */
/* OBSOLETE     { */
/* OBSOLETE       adjust_console_sizes (); */
/* OBSOLETE       adjust_console_scrollbars (); */
/* OBSOLETE       adjust_console_text (); */
/* OBSOLETE       return 0; */
/* OBSOLETE     } */
/* OBSOLETE   return fflush (fp); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE #undef fgetc */
/* OBSOLETE  */
/* OBSOLETE hacked_fgetc (FILE * fp) */
/* OBSOLETE { */
/* OBSOLETE   if (mac_app && (fp == stdin)) */
/* OBSOLETE     { */
/* OBSOLETE       /* Catch any attempts to use this.  */ */
/* OBSOLETE       DebugStr ("\pShould not be reading from stdin!"); */
/* OBSOLETE       return '\n'; */
/* OBSOLETE     } */
/* OBSOLETE   return fgetc (fp); */
/* OBSOLETE } */
