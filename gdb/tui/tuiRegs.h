#ifndef _TUI_REGS_H
#define _TUI_REGS_H
/*
   ** This header file supports the display of registers in the data window.
 */

/*****************************************
** TYPE DEFINITIONS                        **
******************************************/



/*****************************************
** PUBLIC FUNCTION EXTERNAL DECLS        **
******************************************/
extern void tuiCheckRegisterValues (struct frame_info *);
extern void tuiShowRegisters (TuiRegisterDisplayType);
extern void tuiDisplayRegistersFrom (int);
extern int tuiDisplayRegistersFromLine (int, int);
extern int tuiLastRegsLineNo (void);
extern int tuiFirstRegElementInLine (int);
extern int tuiLastRegElementInLine (int);
extern int tuiLineFromRegElementNo (int);
extern void tuiToggleFloatRegs (void);
extern int tuiCalculateRegsColumnCount (TuiRegisterDisplayType);


#endif
/*_TUI_REGS_H*/
