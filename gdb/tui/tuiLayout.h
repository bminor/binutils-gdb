#ifndef TUI_LAYOUT_H
#define TUI_LAYOUT_H

extern void showLayout (TuiLayoutType);
extern void tuiAddWinToLayout (TuiWinType);
extern void tui_vAddWinToLayout (va_list);
extern int tuiDefaultWinHeight (TuiWinType, TuiLayoutType);
extern int tuiDefaultWinViewportHeight (TuiWinType, TuiLayoutType);
extern TuiStatus tuiSetLayout (TuiLayoutType, TuiRegisterDisplayType);
extern TuiStatus tui_vSetLayoutTo (va_list);

#endif /*TUI_LAYOUT_H */
