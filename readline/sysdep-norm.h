/* System-dependent stuff, for ``normal'' systems */

#ifdef __GNUC__
#define alloca __builtin_alloca
#else
#if defined (sparc) && defined (sun)
#include <alloca.h>
#else
extern char *alloca ();
#endif
#endif
