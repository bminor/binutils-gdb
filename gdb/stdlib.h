/* Fake stdlib.h supplying the stuff needed by malloc. */

#ifndef __ONEFILE
#include <stddef.h>
#endif

extern void EXFUN(abort, (void));
extern void EXFUN(free, (PTR));
extern PTR EXFUN(malloc, (size_t));
extern PTR EXFUN(realloc, (PTR, size_t));
