#ifndef _TUI_COMMAND_H
#define _TUI_COMMAND_H
/*
   ** This header file supports
 */


/*****************************************
** TYPE DEFINITIONS                        **
******************************************/



/*****************************************
** PUBLIC FUNCTION EXTERNAL DECLS        **
******************************************/

extern unsigned int tuiDispatchCtrlChar (unsigned int);
extern int tuiIncrCommandCharCountBy (int);
extern int tuiDecrCommandCharCountBy (int);
extern int tuiSetCommandCharCountTo (int);
extern int tuiClearCommandCharCount (void);

#endif
/*_TUI_COMMAND_H*/
