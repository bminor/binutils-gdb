dnl Autoconf configure snippets for common.
dnl Copyright (C) 1995-2019 Free Software Foundation, Inc.
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
		   sys/resource.h sys/socket.h dnl
		   sys/un.h sys/wait.h dnl
		   thread_db.h wait.h dnl
		   termios.h dnl
		   dlfcn.h)

  AC_CHECK_FUNCS([fdwalk getrlimit pipe pipe2 socketpair sigaction \
		  sigprocmask])

  AC_CHECK_DECLS([strstr])

  # Check for std::thread.  This does not work on some platforms, like
  # mingw and DJGPP.
  AC_LANG_PUSH([C++])
  AX_PTHREAD([threads=yes], [threads=no])
  if test "$threads" = "yes"; then
    save_LIBS="$LIBS"
    LIBS="$PTHREAD_LIBS $LIBS"
    save_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="$PTHREAD_CFLAGS $save_CXXFLAGS"
    AC_CACHE_CHECK([for std::thread],
		   gdb_cv_cxx_std_thread,
		   [AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <thread>
      void callback() { }]],
    [[std::thread t(callback);]])],
				  gdb_cv_cxx_std_thread=yes,
				  gdb_cv_cxx_std_thread=no)])

    # This check must be here, while LIBS includes any necessary
    # threading library.
    AC_CHECK_FUNCS([pthread_sigmask pthread_setname_np])

    LIBS="$save_LIBS"
    CXXFLAGS="$save_CXXFLAGS"
  fi
  if test $gdb_cv_cxx_std_thread = yes; then
    AC_DEFINE(CXX_STD_THREAD, 1,
	      [Define to 1 if std::thread works.])
  fi
  AC_LANG_POP

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
])
