/* External/Public TUI Header File */

#ifndef TUI_H
#define TUI_H
#if defined (HAVE_NCURSES_H)
#include <ncurses.h>
#elif defined (HAVE_CURSES_H)
#include <curses.h>
#endif

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif

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

/* Generic window information */
     typedef struct _TuiGenWinInfo
       {
	 WINDOW *handle;	/* window handle */
	 TuiWinType type;	/* type of window */
	 int width;		/* window width */
	 int height;		/* window height */
	 TuiPoint origin;	/* origin of window */
	 OpaquePtr content;	/* content of window */
	 int contentSize;	/* Size of content (# of elements) */
	 int contentInUse;	/* Can it be used, or is it already used? */
	 int viewportHeight;	/* viewport height */
	 int lastVisibleLine;	/* index of last visible line */
	 int isVisible;		/* whether the window is visible or not */
       }
TuiGenWinInfo, *TuiGenWinInfoPtr;

/* GENERAL TUI FUNCTIONS */
/* tui.c */
extern void tuiInit (char *argv0);
extern void tuiInitWindows (void);
extern void tuiResetScreen (void);
extern void tuiCleanUp (void);
extern void tuiError (char *, int);
extern void tui_vError (va_list);
extern void tuiFree (char *);
extern Opaque tuiDo (TuiOpaqueFuncPtr, ...);
extern Opaque tuiDoAndReturnToTop (TuiOpaqueFuncPtr, ...);
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
