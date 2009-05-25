$!
$! This file configures binutils for use with openVMS/Alpha
$! We do not use the configure script, since we do not have /bin/sh
$! to execute it.
$!
$! Written by Klaus K"ampf (kkaempf@rmi.de)
$!
$arch_indx = 1 + ((f$getsyi("CPU").ge.128).and.1)      ! vax==1, alpha==2
$arch = f$element(arch_indx,"|","|VAX|Alpha|")
$!
$! Generate config.h
$!
$ create config.h
/* config.h.  Generated automatically by configure.com  */
/* Is the type time_t defined in <time.h>?  */
#define HAVE_TIME_T_IN_TIME_H 1
/* Is the type time_t defined in <sys/types.h>?  */
#define HAVE_TIME_T_IN_TYPES_H 1
/* Does <utime.h> define struct utimbuf?  */
#define HAVE_GOOD_UTIME_H 1
/* Whether fprintf must be declared even if <stdio.h> is included.  */
#define NEED_DECLARATION_FPRINTF 1
/* Do we need to use the b modifier when opening binary files?  */
/* #undef USE_BINARY_FOPEN */
/* Define if you have the utimes function.  */
#define HAVE_UTIMES 1
/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1
/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1
/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1
/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1
/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1
/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1
/* Alloca.  */
#ifdef __DECC
#include <builtins.h>
#define C_alloca(x) __ALLOCA(x)
#endif
$!
$! Add TARGET.
$!
$ if arch .eqs. "Alpha"
$ then
$   target = "vms-alpha"
$ else
$   target = "vms-vax"
$ endif
$!
$ open/append tfile config.h
$ write tfile "#define TARGET """ + target + """"
$ close tfile
$ write sys$output "Created `config.h'"
$!
$ copy makefile.vms-in makefile.vms
$ write sys$output "Created `makefile.vms'"
