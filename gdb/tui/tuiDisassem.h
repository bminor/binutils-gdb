#ifndef _TUI_DISASSEM_H
#define _TUI_DISASSEM_H
/*
   ** This header file supports
 */

/*****************************************
** TYPE DEFINITIONS                        **
******************************************/



/*****************************************
** PUBLIC FUNCTION EXTERNAL DECLS        **
******************************************/
extern TuiStatus tuiSetDisassemContent (struct symtab *, Opaque);
extern void tuiShowDisassem (Opaque);
extern void tuiShowDisassemAndUpdateSource (Opaque);
extern void tuiVerticalDisassemScroll (TuiScrollDirection, int);
extern Opaque tuiGetBeginAsmAddress (void);

#endif
/*_TUI_DISASSEM_H*/
