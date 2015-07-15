dnl Autoconf configure snippets for common.
dnl Copyright (C) 1995-2015 Free Software Foundation, Inc.
dnl
dnl This file is part of GDB.
dnl 
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

dnl Invoke configury needed by the files in 'common'.
AC_DEFUN([GDB_AC_COMMON], [
  AC_HEADER_STDC
  AC_FUNC_ALLOCA

  dnl Note that this requires codeset.m4, which is included
  dnl by the users of common.m4.
  AM_LANGINFO_CODESET

  AC_CHECK_HEADERS(linux/perf_event.h locale.h memory.h signal.h dnl
		   sys/resource.h sys/socket.h sys/syscall.h dnl
		   sys/un.h sys/wait.h dnl
		   thread_db.h wait.h)

  AC_CHECK_FUNCS([fdwalk getrlimit pipe pipe2 socketpair])

  AC_CHECK_DECLS([strerror, strstr])

  dnl Check if sigsetjmp is available.  Using AC_CHECK_FUNCS won't
  dnl do since sigsetjmp might only be defined as a macro.
AC_CACHE_CHECK([for sigsetjmp], gdb_cv_func_sigsetjmp,
[AC_TRY_COMPILE([
#include <setjmp.h>
], [sigjmp_buf env; while (! sigsetjmp (env, 1)) siglongjmp (env, 1);],
gdb_cv_func_sigsetjmp=yes, gdb_cv_func_sigsetjmp=no)])
if test $gdb_cv_func_sigsetjmp = yes; then
  AC_DEFINE(HAVE_SIGSETJMP, 1, [Define if sigsetjmp is available. ])
fi

  # Assume we'll default to using the included libiberty regex.
  gdb_use_included_regex=yes

  # However, if the system regex is GNU regex, then default to *not*
  # using the included regex.
  AC_CACHE_CHECK(
    [for GNU regex],
    [gdb_cv_have_gnu_regex],
    [AC_TRY_COMPILE(
      [#include <gnu-versions.h>],
      [#define REGEX_INTERFACE_VERSION 1
  #if _GNU_REGEX_INTERFACE_VERSION != REGEX_INTERFACE_VERSION
  # error "Version mismatch"
  #endif],
      gdb_cv_have_gnu_regex=yes,
      gdb_cv_have_gnu_regex=no)])
  if test $gdb_cv_have_gnu_regex = yes; then
    gdb_use_included_regex=no
  fi

  AC_ARG_WITH(included-regex,
    AS_HELP_STRING([--without-included-regex], [don't use included regex; this is the default on systems with version 2 of the GNU C library (use with caution on other system)]),
    gdb_with_regex=$withval,
    gdb_with_regex=$gdb_use_included_regex)
  if test "$gdb_with_regex" = yes; then
    AC_DEFINE(USE_INCLUDED_REGEX, 1,
      [Define to 1 if the regex included in libiberty should be used.])
  fi
])
