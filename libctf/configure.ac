dnl                                            -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2019 Free Software Foundation, Inc.
dnl
dnl This file is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; see the file COPYING.  If not see
dnl <http://www.gnu.org/licenses/>.
dnl

AC_PREREQ(2.64)
AC_INIT([libctf library], 1.2.0-pre)
AC_CONFIG_SRCDIR(ctf-impl.h)
AC_CONFIG_MACRO_DIR(../config)
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

AC_SYS_LARGEFILE

MISSING=`cd $ac_aux_dir && ${PWDCMD-pwd}`/missing
AC_CHECK_PROGS([ACLOCAL], [aclocal], [$MISSING aclocal])
AC_CHECK_PROGS([AUTOCONF], [autoconf], [$MISSING autoconf])
AC_CHECK_PROGS([AUTOHEADER], [autoheader], [$MISSING autoheader])

# Figure out what compiler warnings we can enable.
# See config/warnings.m4 for details.

ACX_PROG_CC_WARNING_OPTS([-W -Wall -Wno-narrowing -Wwrite-strings \
			  -Wmissing-format-attribute], [warn])
ACX_PROG_CC_WARNING_OPTS([-Wstrict-prototypes -Wmissing-prototypes \
			  -Wold-style-definition], [c_warn])
ACX_PROG_CC_WARNING_ALMOST_PEDANTIC([-Wno-long-long])

# Only enable with --enable-werror-always until existing warnings are
# corrected.
ACX_PROG_CC_WARNINGS_ARE_ERRORS([manual])

AM_MAINTAINER_MODE
ACX_PROG_CC_WARNING_OPTS([-Wall], [ac_libctf_warn_cflags])

AC_FUNC_MMAP
AC_SEARCH_LIBS(dlopen, dl)
AM_ZLIB

# Similar to GDB_AC_CHECK_BFD.
OLD_CFLAGS=$CFLAGS
OLD_LDFLAGS=$LDFLAGS
OLD_LIBS=$LIBS
# Put the old CFLAGS/LDFLAGS last, in case the user's (C|LD)FLAGS
# points somewhere with bfd, with -I/foo/lib and -L/foo/lib.  We
# always want our bfd.
CFLAGS="-I${srcdir}/../include -I../bfd -I${srcdir}/../bfd $CFLAGS"
ZLIBDIR=`echo $zlibdir | sed 's,\$(top_builddir)/,,g'`
LDFLAGS="-L../bfd -L../libiberty $ZLIBDIR $LDFLAGS"
intl=`echo $LIBINTL | sed 's,${top_builddir}/,,g'`
LIBS="-lbfd -liberty -lz $intl $LIBS"
AC_CACHE_CHECK([for ELF support in BFD], ac_cv_libctf_bfd_elf,
[AC_TRY_LINK([#include <stdlib.h>
	     #include "bfd.h"
	     #include "elf-bfd.h"],
	    [(void) bfd_section_from_elf_index (NULL, 0);
	     return 0;],
	    [ac_cv_libctf_bfd_elf=yes],
	    [ac_cv_libctf_bfd_elf=no])])
CFLAGS=$OLD_CFLAGS
LDFLAGS=$OLD_LDFLAGS
LIBS=$OLD_LIBS

if test $ac_cv_libctf_bfd_elf = yes; then
  AC_DEFINE([HAVE_BFD_ELF], 1,
	    [Whether libbfd was configured for an ELF target.])
fi

AC_C_BIGENDIAN
AC_CHECK_HEADERS(byteswap.h endian.h)
AC_CHECK_FUNCS(pread)

dnl Check for qsort_r.  (Taken from gnulib.)
AC_CHECK_FUNCS_ONCE([qsort_r])
if test $ac_cv_func_qsort_r = yes; then
  AC_CACHE_CHECK([for qsort_r signature], [ac_cv_libctf_qsort_r_signature],
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM([[#undef qsort_r
                          #include <stdlib.h>
                          void qsort_r (void *, size_t, size_t,
                                        int (*) (void const *, void const *,
                                                 void *),
                                        void *);
                          void (*p) (void *, size_t, size_t,
                                     int (*) (void const *, void const *,
                                              void *),
                                     void *) = qsort_r;
                        ]])],
       [ac_cv_libctf_qsort_r_signature=GNU],
       [AC_LINK_IFELSE(
          [AC_LANG_PROGRAM([[#undef qsort_r
	                     #include <stdlib.h>
                             void qsort_r (void *, size_t, size_t, void *,
                                           int (*) (void *,
                                                    void const *,
                                                    void const *));
                             void (*p) (void *, size_t, size_t, void *,
                                        int (*) (void *, void const *,
                                                 void const *)) = qsort_r;
                           ]])],
          [ac_cv_libctf_qsort_r_signature=BSD],
          [ac_cv_libctf_qsort_r_signature=unknown])])])
fi

case x$ac_cv_libctf_qsort_r_signature in
  xGNU)     AC_DEFINE([HAVE_QSORT_R_ARG_LAST], 1,
	     [Whether a qsort_r exists with a void *arg as its last arg.]);;
  xBSD)     AC_DEFINE([HAVE_QSORT_R_COMPAR_LAST], 1,
	     [Whether a qsort_r exists with the compar function as its last arg.]);;
  *) ac_cv_libctf_qsort_r_signature=unknown;;
esac

AM_CONDITIONAL(NEED_CTF_QSORT_R, test "${ac_cv_libctf_qsort_r_signature}" = unknown)

AC_CACHE_CHECK([for O_CLOEXEC], [ac_cv_libctf_macro_O_CLOEXEC],
  [AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[#include <fcntl.h>
                       #ifndef O_CLOEXEC
                         choke me;
                       #endif
                     ]],
                     [[return O_CLOEXEC;]])],
    [ac_cv_libctf_macro_O_CLOEXEC=yes],
    [ac_cv_libctf_macro_O_CLOEXEC=no])])

if test $ac_cv_libctf_macro_O_CLOEXEC = yes; then
  AC_DEFINE([HAVE_O_CLOEXEC], 1,
	    [Whether the platform has a definition of O_CLOEXEC.])
fi

AC_CONFIG_FILES(Makefile)
AC_CONFIG_HEADERS(config.h)
AC_OUTPUT
