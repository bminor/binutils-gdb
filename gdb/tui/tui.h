/* External/Public TUI Header File.
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

#ifndef TUI_H
#define TUI_H

#include <stdarg.h>
#include <string.h>
#include "ansidecl.h"

#if defined(reg)
#undef reg
#endif
#if defined(chtype)
#undef chtype
#endif

/* Opaque data type */
typedef char *Opaque;
typedef
Opaque (*OpaqueFuncPtr) (va_list);
     typedef char **OpaqueList;
     typedef OpaqueList OpaquePtr;

/* Generic function pointer */
     typedef void (*TuiVoidFuncPtr) (va_list);
     typedef int (*TuiIntFuncPtr) (va_list);
/*
   typedef Opaque (*TuiOpaqueFuncPtr) (va_list);
 */
     typedef OpaqueFuncPtr TuiOpaqueFuncPtr;

extern Opaque vcatch_errors (OpaqueFuncPtr, ...);
extern Opaque va_catch_errors (OpaqueFuncPtr, va_list);

extern void strcat_to_buf (char *, int, char *);
extern void strcat_to_buf_with_fmt (char *, int, char *, ...);

/* Types of error returns */
     typedef enum
       {
	 TUI_SUCCESS,
	 TUI_FAILURE
       }
TuiStatus, *TuiStatusPtr;

/* Types of windows */
     typedef enum
       {
	 SRC_WIN = 0,
	 DISASSEM_WIN,
	 DATA_WIN,
	 CMD_WIN,
	 /* This must ALWAYS be AFTER the major windows last */
	 MAX_MAJOR_WINDOWS,
	 /* auxillary windows */
	 LOCATOR_WIN,
	 EXEC_INFO_WIN,
	 DATA_ITEM_WIN,
	 /* This must ALWAYS be next to last */
	 MAX_WINDOWS,
	 UNDEFINED_WIN		/* LAST */
       }
TuiWinType, *TuiWinTypePtr;

/* This is a point definition */
     typedef struct _TuiPoint
       {
	 int x, y;
       }
TuiPoint, *TuiPointPtr;

/* GENERAL TUI FUNCTIONS */
/* tui.c */
extern void tuiInit (char *argv0);
extern void tuiInitWindows (void);
extern void tuiResetScreen (void);
extern void tuiCleanUp (void);
extern void tuiError (char *, int);
extern void tui_vError (va_list);
extern void tuiFree (char *);
extern Opaque tuiGetLowDisassemblyAddress (Opaque, Opaque);
extern Opaque tui_vGetLowDisassemblyAddress (va_list);
extern void tui_vSelectSourceSymtab (va_list);

/* tuiDataWin.c */
extern void tui_vCheckDataValues (va_list);

/* tuiIO.c */
extern void tui_vStartNewLines (va_list);

/* tuiLayout.c */
extern void tui_vAddWinToLayout (va_list);
extern TuiStatus tui_vSetLayoutTo (va_list);

/* tuiSourceWin.c */
extern void tuiDisplayMainFunction (void);
extern void tuiUpdateAllExecInfos (void);
extern void tuiUpdateOnEnd (void);
extern void tui_vAllSetHasBreakAt (va_list);
extern void tui_vUpdateSourceWindowsWithAddr (va_list);

/* tuiStack.c */
extern void tui_vShowFrameInfo (va_list);
extern void tui_vUpdateLocatorFilename (va_list);
#endif /* TUI_H */
