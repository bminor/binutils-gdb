
#ifndef _DEBUGIFY_H_
#define _DEBUGIFY_H_

#ifdef DEBUGIFY
#include <assert.h>
#ifdef TO_SCREEN
#define DBG(x) OutputDebugString x
#elif TO_GDB
#define DBG(x) printf_unfiltered x
#elif TO_POPUP
#define DBG(x) MessageBox x
#else /* default: TO_FILE "gdb.log" */
#define DBG(x) printf_dbg x
#endif 

#define ASSERT(x) assert(x)

#else /* DEBUGIFY */
#define DBG(x)
#define ASSERT(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REDIRECT
#define printf_unfiltered printf_dbg
#define fputs_unfiltered fputs_dbg
void fputs_dbg (const char *fmt, FILE *fakestream);
#endif /* REDIRECT */

void puts_dbg(const char *fmt);
void printf_dbg(const char *fmt,...);

#ifdef __cplusplus
}
#endif

#endif /* _DEBUGIFY_H_ */

