/* System-dependent stuff, for SGI Irix 6.  */

#ifdef __GNUC__
#define alloca __builtin_alloca
#else
#include <alloca.h>
#endif

#include <sys/types.h>
#include <sys/stream.h>
#include <sys/dir.h>
typedef struct direct dirent;
