#ifndef TUI_GENERAL_WIN_H
#define TUI_GENERAL_WIN_H

/*
   ** Functions
 */
extern void tuiClearWin (TuiGenWinInfoPtr);
extern void unhighlightWin (TuiWinInfoPtr);
extern void makeVisible (TuiGenWinInfoPtr, int);
extern void makeAllVisible (int);
extern void scrollWinForward (TuiGenWinInfoPtr, int);
extern void scrollWinBackward (TuiGenWinInfoPtr, int);
extern void makeWindow (TuiGenWinInfoPtr, int);
extern TuiWinInfoPtr copyWin (TuiWinInfoPtr);
extern void boxWin (TuiGenWinInfoPtr, int);
extern void highlightWin (TuiWinInfoPtr);
extern void checkAndDisplayHighlightIfNeeded (TuiWinInfoPtr);
extern void refreshAll (TuiWinInfoPtr *);
extern void tuiDelwin (WINDOW * window);
extern void tuiRefreshWin (TuiGenWinInfoPtr);

/*
   ** Macros
 */
#define    m_beVisible(winInfo)   makeVisible((TuiGenWinInfoPtr)(winInfo), TRUE)
#define    m_beInvisible(winInfo) \
                            makeVisible((TuiGenWinInfoPtr)(winInfo), FALSE)
#define    m_allBeVisible()       makeAllVisible(TRUE)
#define m_allBeInvisible()        makeAllVisible(FALSE)

#endif /*TUI_GENERAL_WIN_H */
