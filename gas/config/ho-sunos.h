#ifndef __STDC__
#define NO_STDARG
#endif /* not __STDC__ */

#if !defined(__GNUC__) & !defined(__STDC__)
#include <memory.h>
#else
extern int memset();
#endif

/* #include <sys/stdtypes.h> before <stddef.h> when compiling by GCC.  */
#include <sys/stdtypes.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>

/* externs for system libraries. */

extern char *strchr();
extern char *malloc();
extern char *realloc();
extern char *strrchr();
extern int _filbuf();
extern int _flsbuf();
/*extern int abort();*/
extern int bcopy();
extern int bzero();
extern int bzero();
/*extern int exit();*/
extern int fclose();
extern int fprintf();
extern int fread();
extern int free();
extern int perror();
extern int printf();
extern int setvbuf();
extern int sscanf();
extern int strcmp();
extern int strlen();
extern int strncmp();
extern int time();
extern int ungetc();
extern int vfprintf();
extern int vprintf();
extern long atol();

#ifndef tolower
extern int tolower();
#endif /* tolower */

#ifndef toupper
extern int toupper();
#endif /* toupper */

/*
 * Local Variables:
 * fill-column: 80
 * comment-column: 0
 * End:
 */

/* end of ho-sunos.h */
