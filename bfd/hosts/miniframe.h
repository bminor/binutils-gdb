#include "hosts/std-host.h"

#define USE_UTIME

#define NEED_MKDIR
#define NEED_RMDIR

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024
#endif	/* MAXPATHLEN */

#define bcmp(b1,b2,len)		memcmp(b1,b2,len)
#define bcopy(src,dst,len)	memcpy(dst,src,len)
#define bzero(s,n)		memset(s,0,n)
