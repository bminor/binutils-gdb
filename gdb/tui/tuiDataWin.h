#ifndef _TUI_DATAWIN_H
#define _TUI_DATAWIN_H
/*
   ** This header file supports the display of registers/data in the data window.
 */


/*****************************************
** TYPE DEFINITIONS                        **
******************************************/



/*****************************************
** PUBLIC FUNCTION EXTERNAL DECLS        **
******************************************/
extern void tuiEraseDataContent (char *);
extern void tuiDisplayAllData (void);
extern void tuiCheckDataValues (struct frame_info *);
extern void tui_vCheckDataValues (va_list);
extern void tuiDisplayDataFromLine (int);
extern int tuiFirstDataItemDisplayed (void);
extern int tuiFirstDataElementNoInLine (int);
extern void tuiDeleteDataContentWindows (void);
extern void tuiRefreshDataWin (void);
extern void tuiDisplayDataFrom (int, int);
extern void tuiVerticalDataScroll (TuiScrollDirection, int);

#endif
/*_TUI_DATAWIN_H*/
