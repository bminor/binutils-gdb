#ifndef _TUI_WIN_H
#define _TUI_WIN_H
/*
   ** This header file supports
 */

/*****************************************
** TYPE DEFINITIONS                        **
******************************************/



/*****************************************
** PUBLIC FUNCTION EXTERNAL DECLS        **
******************************************/
extern void tuiScrollForward (TuiWinInfoPtr, int);
extern void tuiScrollBackward (TuiWinInfoPtr, int);
extern void tuiScrollLeft (TuiWinInfoPtr, int);
extern void tuiScrollRight (TuiWinInfoPtr, int);
extern void tui_vScroll (va_list);
extern void tuiSetWinFocusTo (TuiWinInfoPtr);
extern void tuiClearWinFocusFrom (TuiWinInfoPtr);
extern void tuiClearWinFocus (void);
extern void tuiResizeAll (void);
extern void tuiRefreshAll (void);
extern void tuiSigwinchHandler (int);

#endif
/*_TUI_WIN_H*/
