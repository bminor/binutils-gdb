#ifndef hosts_std_host_H
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
/*#include <string.h>*/
#else
extern int   fprintf PARAMS ((FILE *,char *,...));
extern int   printf PARAMS((char *,...));
extern char *mktemp ();
extern int   fflush ();
extern int   write ();
extern void  abort ();
extern int   close ();
extern int   fcntl ();
extern int   qsort ();
extern void  exit ();
extern int   fseek ();
extern int   fclose ();
extern void  bcopy ();
extern int   bcmp ();
extern void  bzero ();
extern PTR   memset ();
extern void  puts ();
extern void  fputs ();
extern int   rmdir ();
extern int   getuid ();
extern int   getgid ();
extern int   rename ();
extern void  perror ();
#ifndef DONTDECLARE_MALLOC
extern PTR   malloc ();
extern PTR   realloc ();
#endif

#ifndef __GNUC__
extern PTR   memcpy ();
#else
/* char * memcpy (); */
#endif

#ifdef __STDC__
extern void free ();
#else
extern int free();
#endif

extern char * strchr();
extern char *getenv();
extern char *memchr();
extern char *strrchr();
extern int chmod();
extern int fread();
extern int fstat();
extern int fwrite();
extern int stat();
extern int strtol();

extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
extern int time();
extern int utimes();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();
#endif /* STDC_HEADERS */

#ifndef uint64e_type
#ifdef __GNUC__
#define uint64e_type unsigned long long
#define int64e_type long long
#define uint64_typeLOW(x) (unsigned long)(((x) & 0xffffffff))
#define uint64_typeHIGH(x) (unsigned long)(((x) >> 32) & 0xffffffff)
#else
struct  uint64e_struct {
  unsigned long low, high;
};
#define uint64e_type struct uint64e_struct
#define int64e_type struct uint64e_struct
#define uint64_typeLOW(x) (unsigned long)(((x).low))
#define uint64_typeHIGH(x) (unsigned long)(((x).high))
#endif
#endif

#ifndef int64_type
#define int64_type int64e_type
#endif
#ifndef uint64_type
#define uint64_type uint64e_type
#endif

#ifndef BYTES_IN_PRINTF_INT
#define BYTES_IN_PRINTF_INT 4
#endif

#include "fopen-same.h"
#define hosts_std_host_H
#endif
