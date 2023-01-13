# threadlib.m4 serial 24
dnl Copyright (C) 2005-2019 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

AC_PREREQ([2.60])

dnl gl_PTHREADLIB
dnl -------------
dnl Tests for the libraries needs for using the POSIX threads API.
dnl Sets the variable LIBPTHREAD to the linker options for use in a Makefile.
dnl Sets the variable LIBPMULTITHREAD, for programs that really need
dnl multithread functionality. The difference between LIBPTHREAD and
dnl LIBPMULTITHREAD is that on platforms supporting weak symbols, typically
dnl LIBPTHREAD is empty whereas LIBPMULTITHREAD is not.
dnl Adds to CPPFLAGS the flag -D_REENTRANT or -D_THREAD_SAFE if needed for
dnl multithread-safe programs.
dnl Defines the C macro HAVE_PTHREAD_API if (at least parts of) the POSIX
dnl threads API is available.

dnl gl_THREADLIB
dnl ------------
dnl Tests for a multithreading library to be used.
dnl If the configure.ac contains a definition of the gl_THREADLIB_DEFAULT_NO
dnl (it must be placed before the invocation of gl_THREADLIB_EARLY!), then the
dnl default is 'no', otherwise it is system dependent. In both cases, the user
dnl can change the choice through the options --enable-threads=choice or
dnl --disable-threads.
dnl Defines at most one of the macros USE_ISOC_THREADS, USE_POSIX_THREADS,
dnl USE_ISOC_AND_POSIX_THREADS, USE_WINDOWS_THREADS.
dnl The choice --enable-threads=isoc+posix is available only on platforms that
dnl have both the ISO C and the POSIX threads APIs. It has the effect of using
dnl the ISO C API for most things and the POSIX API only for creating and
dnl controlling threads (because there is no equivalent to pthread_atfork in
dnl the ISO C API).
dnl Sets the variables LIBTHREAD and LTLIBTHREAD to the linker options for use
dnl in a Makefile (LIBTHREAD for use without libtool, LTLIBTHREAD for use with
dnl libtool).
dnl Sets the variables LIBMULTITHREAD and LTLIBMULTITHREAD similarly, for
dnl programs that really need multithread functionality. The difference
dnl between LIBTHREAD and LIBMULTITHREAD is that on platforms supporting weak
dnl symbols, typically LIBTHREAD is empty whereas LIBMULTITHREAD is not.
dnl Adds to CPPFLAGS the flag -D_REENTRANT or -D_THREAD_SAFE if needed for
dnl multithread-safe programs.
dnl Since support for GNU pth was removed, $LTLIBTHREAD and $LIBTHREAD have the
dnl same value, and similarly $LTLIBMULTITHREAD and $LIBMULTITHREAD have the
dnl same value. Only system libraries are needed.

AC_DEFUN([gl_ANYTHREADLIB_EARLY],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  if test -z "$gl_anythreadlib_early_done"; then
    case "$host_os" in
      osf*)
        # On OSF/1, the compiler needs the flag -D_REENTRANT so that it
        # groks <pthread.h>. cc also understands the flag -pthread, but
        # we don't use it because 1. gcc-2.95 doesn't understand -pthread,
        # 2. putting a flag into CPPFLAGS that has an effect on the linker
        # causes the AC_LINK_IFELSE test below to succeed unexpectedly,
        # leading to wrong values of LIBTHREAD and LTLIBTHREAD.
        CPPFLAGS="$CPPFLAGS -D_REENTRANT"
        ;;
    esac
    # Some systems optimize for single-threaded programs by default, and
    # need special flags to disable these optimizations. For example, the
    # definition of 'errno' in <errno.h>.
    case "$host_os" in
      aix* | freebsd*) CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE" ;;
      solaris*) CPPFLAGS="$CPPFLAGS -D_REENTRANT" ;;
    esac
    gl_anythreadlib_early_done=done
  fi
])

AC_DEFUN([gl_THREADLIB_EARLY],
[
  AC_REQUIRE([gl_THREADLIB_EARLY_BODY])
])

dnl The guts of gl_THREADLIB_EARLY. Needs to be expanded only once.

AC_DEFUN([gl_THREADLIB_EARLY_BODY],
[
  dnl Ordering constraints: This macro modifies CPPFLAGS in a way that
  dnl influences the result of the autoconf tests that test for *_unlocked
  dnl declarations, on AIX 5 at least. Therefore it must come early.
  AC_BEFORE([$0], [gl_FUNC_GLIBC_UNLOCKED_IO])dnl
  AC_BEFORE([$0], [gl_ARGP])dnl

  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl _GNU_SOURCE is needed for pthread_rwlock_t on glibc systems.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])
  dnl Check for multithreading.
  m4_ifdef([gl_THREADLIB_DEFAULT_NO],
    [m4_divert_text([DEFAULTS], [gl_use_threads_default=no])],
    [m4_divert_text([DEFAULTS], [gl_use_threads_default=])])
  m4_divert_text([DEFAULTS], [gl_use_winpthreads_default=])
  AC_ARG_ENABLE([threads],
AC_HELP_STRING([--enable-threads={isoc|posix|isoc+posix|windows}], [specify multithreading API])m4_ifdef([gl_THREADLIB_DEFAULT_NO], [], [
AC_HELP_STRING([--disable-threads], [build without multithread safety])]),
    [gl_use_threads=$enableval],
    [if test -n "$gl_use_threads_default"; then
       gl_use_threads="$gl_use_threads_default"
     else
changequote(,)dnl
       case "$host_os" in
         dnl Disable multithreading by default on OSF/1, because it interferes
         dnl with fork()/exec(): When msgexec is linked with -lpthread, its
         dnl child process gets an endless segmentation fault inside execvp().
         osf*) gl_use_threads=no ;;
         dnl Disable multithreading by default on Cygwin 1.5.x, because it has
         dnl bugs that lead to endless loops or crashes. See
         dnl <https://cygwin.com/ml/cygwin/2009-08/msg00283.html>.
         cygwin*)
               case `uname -r` in
                 1.[0-5].*) gl_use_threads=no ;;
                 *)         gl_use_threads=yes ;;
               esac
               ;;
         dnl Obey gl_AVOID_WINPTHREAD on mingw.
         mingw*)
               case "$gl_use_winpthreads_default" in
                 yes) gl_use_threads=posix ;;
                 no)  gl_use_threads=windows ;;
                 *)   gl_use_threads=yes ;;
               esac
               ;;
         *)    gl_use_threads=yes ;;
       esac
changequote([,])dnl
     fi
    ])
  if test "$gl_use_threads" = yes \
     || test "$gl_use_threads" = isoc \
     || test "$gl_use_threads" = posix \
     || test "$gl_use_threads" = isoc+posix; then
    # For using <threads.h> or <pthread.h>:
    gl_ANYTHREADLIB_EARLY
  fi
])

dnl Checks whether the compiler and linker support weak declarations of symbols.

AC_DEFUN([gl_WEAK_SYMBOLS],
[
  AC_CACHE_CHECK([whether imported symbols can be declared weak],
    [gl_cv_have_weak],
    [gl_cv_have_weak=no
     dnl First, test whether the compiler accepts it syntactically.
     AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
          [[extern void xyzzy ();
#pragma weak xyzzy]],
          [[xyzzy();]])],
       [gl_cv_have_weak=maybe])
     if test $gl_cv_have_weak = maybe; then
       dnl Second, test whether it actually works. On Cygwin 1.7.2, with
       dnl gcc 4.3, symbols declared weak always evaluate to the address 0.
       AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
#include <stdio.h>
#pragma weak fputs
int main ()
{
  return (fputs == NULL);
}]])],
         [gl_cv_have_weak=yes],
         [gl_cv_have_weak=no],
         [dnl When cross-compiling, assume that only ELF platforms support
          dnl weak symbols.
          AC_EGREP_CPP([Extensible Linking Format],
            [#ifdef __ELF__
             Extensible Linking Format
             #endif
            ],
            [gl_cv_have_weak="guessing yes"],
            [gl_cv_have_weak="guessing no"])
         ])
     fi
     dnl But when linking statically, weak symbols don't work.
     case " $LDFLAGS " in
       *" -static "*) gl_cv_have_weak=no ;;
     esac
    ])
  case "$gl_cv_have_weak" in
    *yes)
      AC_DEFINE([HAVE_WEAK_SYMBOLS], [1],
        [Define to 1 if the compiler and linker support weak declarations of symbols.])
      ;;
  esac
])

dnl The guts of gl_PTHREADLIB. Needs to be expanded only once.

AC_DEFUN([gl_PTHREADLIB_BODY],
[
  AC_REQUIRE([gl_ANYTHREADLIB_EARLY])
  if test -z "$gl_threadlib_body_done"; then
    gl_pthread_api=no
    LIBPTHREAD=
    LIBPMULTITHREAD=
    # On OSF/1, the compiler needs the flag -pthread or -D_REENTRANT so that
    # it groks <pthread.h>. It's added above, in gl_ANYTHREADLIB_EARLY.
    AC_CHECK_HEADER([pthread.h],
      [gl_have_pthread_h=yes], [gl_have_pthread_h=no])
    if test "$gl_have_pthread_h" = yes; then
      # Other possible tests:
      #   -lpthreads (FSU threads, PCthreads)
      #   -lgthreads
      # Test whether both pthread_mutex_lock and pthread_mutexattr_init exist
      # in libc. IRIX 6.5 has the first one in both libc and libpthread, but
      # the second one only in libpthread, and lock.c needs it.
      #
      # If -pthread works, prefer it to -lpthread, since Ubuntu 14.04
      # needs -pthread for some reason.  See:
      # https://lists.gnu.org/r/bug-gnulib/2014-09/msg00023.html
      save_LIBS=$LIBS
      for gl_pthread in '' '-pthread'; do
        LIBS="$LIBS $gl_pthread"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#include <pthread.h>
               pthread_mutex_t m;
               pthread_mutexattr_t ma;
             ]],
             [[pthread_mutex_lock (&m);
               pthread_mutexattr_init (&ma);]])],
          [gl_pthread_api=yes
           LIBPTHREAD=$gl_pthread
           LIBPMULTITHREAD=$gl_pthread])
        LIBS=$save_LIBS
        test $gl_pthread_api = yes && break
      done

      # Test for libpthread by looking for pthread_kill. (Not pthread_self,
      # since it is defined as a macro on OSF/1.)
      if test $gl_pthread_api = yes && test -z "$LIBPTHREAD"; then
        # The program links fine without libpthread. But it may actually
        # need to link with libpthread in order to create multiple threads.
        AC_CHECK_LIB([pthread], [pthread_kill],
          [LIBPMULTITHREAD=-lpthread
           # On Solaris and HP-UX, most pthread functions exist also in libc.
           # Therefore pthread_in_use() needs to actually try to create a
           # thread: pthread_create from libc will fail, whereas
           # pthread_create will actually create a thread.
           # On Solaris 10 or newer, this test is no longer needed, because
           # libc contains the fully functional pthread functions.
           case "$host_os" in
             solaris | solaris2.[1-9] | solaris2.[1-9].* | hpux*)
               AC_DEFINE([PTHREAD_IN_USE_DETECTION_HARD], [1],
                 [Define if the pthread_in_use() detection is hard.])
           esac
          ])
      elif test $gl_pthread_api != yes; then
        # Some library is needed. Try libpthread and libc_r.
        AC_CHECK_LIB([pthread], [pthread_kill],
          [gl_pthread_api=yes
           LIBPTHREAD=-lpthread
           LIBPMULTITHREAD=-lpthread])
        if test $gl_pthread_api != yes; then
          # For FreeBSD 4.
          AC_CHECK_LIB([c_r], [pthread_kill],
            [gl_pthread_api=yes
             LIBPTHREAD=-lc_r
             LIBPMULTITHREAD=-lc_r])
        fi
      fi
    fi
    AC_MSG_CHECKING([whether POSIX threads API is available])
    AC_MSG_RESULT([$gl_pthread_api])
    AC_SUBST([LIBPTHREAD])
    AC_SUBST([LIBPMULTITHREAD])
    if test $gl_pthread_api = yes; then
      AC_DEFINE([HAVE_PTHREAD_API], [1],
        [Define if you have the <pthread.h> header and the POSIX threads API.])
    fi
    gl_threadlib_body_done=done
  fi
])

dnl The guts of gl_THREADLIB. Needs to be expanded only once.

AC_DEFUN([gl_THREADLIB_BODY],
[
  AC_REQUIRE([gl_THREADLIB_EARLY_BODY])
  gl_threads_api=none
  LIBTHREAD=
  LTLIBTHREAD=
  LIBMULTITHREAD=
  LTLIBMULTITHREAD=
  if test "$gl_use_threads" != no; then
    dnl Check whether the compiler and linker support weak declarations.
    gl_WEAK_SYMBOLS
    if case "$gl_cv_have_weak" in *yes) true;; *) false;; esac; then
      dnl If we use weak symbols to implement pthread_in_use / pth_in_use /
      dnl thread_in_use, we also need to test whether the ISO C 11 thrd_create
      dnl facility is in use.
      AC_CHECK_HEADERS_ONCE([threads.h])
      :
    fi
    if test "$gl_use_threads" = isoc || test "$gl_use_threads" = isoc+posix; then
      AC_CHECK_HEADERS_ONCE([threads.h])
      if test $ac_cv_header_threads_h = yes; then
        gl_have_isoc_threads=
        # Test whether both mtx_lock and cnd_timedwait exist in libc.
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#include <threads.h>
               #include <stddef.h>
               mtx_t m;
               cnd_t c;
             ]],
             [[mtx_lock (&m);
               cnd_timedwait (&c, &m, NULL);]])],
          [gl_have_isoc_threads=yes])
      fi
    fi
    if test "$gl_use_threads" = yes \
       || test "$gl_use_threads" = posix \
       || test "$gl_use_threads" = isoc+posix; then
      gl_PTHREADLIB_BODY
      LIBTHREAD=$LIBPTHREAD LTLIBTHREAD=$LIBPTHREAD
      LIBMULTITHREAD=$LIBPMULTITHREAD LTLIBMULTITHREAD=$LIBPMULTITHREAD
      if test $gl_pthread_api = yes; then
        if test "$gl_use_threads" = isoc+posix && test "$gl_have_isoc_threads" = yes; then
          gl_threads_api='isoc+posix'
          AC_DEFINE([USE_ISOC_AND_POSIX_THREADS], [1],
            [Define if the combination of the ISO C and POSIX multithreading APIs can be used.])
          LIBTHREAD= LTLIBTHREAD=
        else
          gl_threads_api=posix
          AC_DEFINE([USE_POSIX_THREADS], [1],
            [Define if the POSIX multithreading library can be used.])
          if test -n "$LIBMULTITHREAD" || test -n "$LTLIBMULTITHREAD"; then
            if case "$gl_cv_have_weak" in *yes) true;; *) false;; esac; then
              AC_DEFINE([USE_POSIX_THREADS_WEAK], [1],
                [Define if references to the POSIX multithreading library should be made weak.])
              LIBTHREAD= LTLIBTHREAD=
            fi
          fi
        fi
      fi
    fi
    if test $gl_threads_api = none; then
      if test "$gl_use_threads" = isoc && test "$gl_have_isoc_threads" = yes; then
        gl_threads_api=isoc
        AC_DEFINE([USE_ISOC_THREADS], [1],
          [Define if the ISO C multithreading library can be used.])
      fi
    fi
    if test $gl_threads_api = none; then
      case "$gl_use_threads" in
        yes | windows | win32) # The 'win32' is for backward compatibility.
          if { case "$host_os" in
                 mingw*) true;;
                 *) false;;
               esac
             }; then
            gl_threads_api=windows
            AC_DEFINE([USE_WINDOWS_THREADS], [1],
              [Define if the native Windows multithreading API can be used.])
          fi
          ;;
      esac
    fi
  fi
  AC_MSG_CHECKING([for multithread API to use])
  AC_MSG_RESULT([$gl_threads_api])
  AC_SUBST([LIBTHREAD])
  AC_SUBST([LTLIBTHREAD])
  AC_SUBST([LIBMULTITHREAD])
  AC_SUBST([LTLIBMULTITHREAD])
])

AC_DEFUN([gl_PTHREADLIB],
[
  AC_REQUIRE([gl_ANYTHREADLIB_EARLY])
  gl_PTHREADLIB_BODY
])

AC_DEFUN([gl_THREADLIB],
[
  AC_REQUIRE([gl_THREADLIB_EARLY])
  AC_REQUIRE([gl_THREADLIB_BODY])
])


dnl gl_DISABLE_THREADS
dnl ------------------
dnl Sets the gl_THREADLIB default so that threads are not used by default.
dnl The user can still override it at installation time, by using the
dnl configure option '--enable-threads'.

AC_DEFUN([gl_DISABLE_THREADS], [
  m4_divert_text([INIT_PREPARE], [gl_use_threads_default=no])
])


dnl gl_AVOID_WINPTHREAD
dnl -------------------
dnl Sets the gl_THREADLIB default so that on mingw, a dependency to the
dnl libwinpthread DLL (mingw-w64 winpthreads library) is avoided.
dnl The user can still override it at installation time, by using the
dnl configure option '--enable-threads'.

AC_DEFUN([gl_AVOID_WINPTHREAD], [
  m4_divert_text([INIT_PREPARE], [gl_use_winpthreads_default=no])
])


dnl Survey of platforms:
dnl
dnl Platform           Available  Compiler    Supports   test-lock
dnl                    flavours   option      weak       result
dnl ---------------    ---------  ---------   --------   ---------
dnl Linux 2.4/glibc    posix      -lpthread       Y      OK
dnl
dnl GNU Hurd/glibc     posix
dnl
dnl Ubuntu 14.04       posix      -pthread        Y      OK
dnl
dnl FreeBSD 5.3        posix      -lc_r           Y
dnl                    posix      -lkse ?         Y
dnl                    posix      -lpthread ?     Y
dnl                    posix      -lthr           Y
dnl
dnl FreeBSD 5.2        posix      -lc_r           Y
dnl                    posix      -lkse           Y
dnl                    posix      -lthr           Y
dnl
dnl FreeBSD 4.0,4.10   posix      -lc_r           Y      OK
dnl
dnl NetBSD 1.6         --
dnl
dnl OpenBSD 3.4        posix      -lpthread       Y      OK
dnl
dnl Mac OS X 10.[123]  posix      -lpthread       Y      OK
dnl
dnl Solaris 7,8,9      posix      -lpthread       Y      Sol 7,8: 0.0; Sol 9: OK
dnl
dnl HP-UX 11           posix      -lpthread       N (cc) OK
dnl                                               Y (gcc)
dnl
dnl IRIX 6.5           posix      -lpthread       Y      0.5
dnl
dnl AIX 4.3,5.1        posix      -lpthread       N      AIX 4: 0.5; AIX 5: OK
dnl
dnl OSF/1 4.0,5.1      posix      -pthread (cc)   N      OK
dnl                               -lpthread (gcc) Y
dnl
dnl Cygwin             posix      -lpthread       Y      OK
dnl
dnl Mingw              windows                    N      OK
dnl
dnl BeOS 5             --
dnl
dnl The test-lock result shows what happens if in test-lock.c EXPLICIT_YIELD is
dnl turned off:
dnl   OK if all three tests terminate OK,
dnl   0.5 if the first test terminates OK but the second one loops endlessly,
dnl   0.0 if the first test already loops endlessly.
