#ifndef _TUI_SOURCE_H
#define _TUI_SOURCE_H
/*
   ** This header file supports
 */


#include "defs.h"
#if 0
#include "symtab.h"
#include "breakpoint.h"
#endif

extern TuiStatus tuiSetSourceContent (struct symtab *, int, int);
extern void tuiShowSource (struct symtab *, Opaque, int);
extern void tuiShowSourceAsIs (struct symtab *, Opaque, int);
extern int tuiSourceIsDisplayed (char *);
extern void tuiVerticalSourceScroll (TuiScrollDirection, int);


/*******************
** MACROS         **
*******************/
#define m_tuiShowSourceAsIs(s, line, noerror)    tuiUpdateSourceWindowAsIs(srcWin, s, line, noerror)


#endif
/*_TUI_SOURCE_H*/
