#ifndef _TUI_STACK_H
#define _TUI_STACK_H
/*
   ** This header file supports
 */

extern void tuiSetLocatorInfo (char *, char *, int, Opaque,
			       TuiLocatorElementPtr);
extern void tuiUpdateLocatorFilename (char *);
extern void tui_vUpdateLocatorFilename (va_list);
extern void tuiUpdateLocatorInfoFromFrame
  (struct frame_info *, TuiLocatorElementPtr);
extern void tuiUpdateLocatorDisplay (struct frame_info *);
extern void tuiSetLocatorContent (struct frame_info *);
extern void tuiShowLocatorContent (void);
extern void tuiClearLocatorContent (void);
extern void tuiSwitchFilename (char *);
extern void tuiShowFrameInfo (struct frame_info *);
extern void tui_vShowFrameInfo (va_list);
extern void tuiGetLocatorFilename (TuiGenWinInfoPtr, char **);


#endif
/*_TUI_STACK_H*/
