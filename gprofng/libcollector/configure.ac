dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2021 Free Software Foundation, Inc.
dnl
dnl This file is free software; you can redistribute it and/or modify
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
dnl along with this program; see the file COPYING3.  If not see
dnl <http://www.gnu.org/licenses/>.

m4_include([../../bfd/version.m4])
AC_INIT([gprofng], BFD_VERSION)
AC_CONFIG_MACRO_DIRS([../../config ../..])
AC_CONFIG_AUX_DIR(../..)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR(libcol_util.c)

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_AR

LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC

if test "$enable_shared" != "yes"; then
  AC_MSG_ERROR([Cannot set --enable-shared for gprofng/libcollector.])
fi

GPROFNG_VARIANT=unknown
case "${target}" in
  x86_64-*-linux*)
    GPROFNG_VARIANT=amd64-Linux
    ;;
  i?86-*-linux*)
    GPROFNG_VARIANT=intel-Linux
    ;;
  aarch64-*-linux*)
    GPROFNG_VARIANT=aarch64-Linux
    ;;
esac
AC_SUBST(GPROFNG_VARIANT)

ACX_PROG_CC_WARNING_OPTS([-Wno-nonnull-compare], [GPROFNG_NO_NONNULL_COMPARE_CFLAGS])
AC_SUBST(GPROFNG_NO_NONNULL_COMPARE_CFLAGS)

clock_gettime_link=
# At least for glibc, clock_gettime is in librt.  But don't
# pull that in if it still doesn't give us the function we want.  This
# test is copied from libgomp, and modified to not link in -lrt as
# we're using this for test timing only.
if test "$ac_cv_func_clock_gettime" = no; then
  AC_CHECK_LIB(rt, clock_gettime,
    [CLOCK_GETTIME_LINK=-lrt
     AC_DEFINE(HAVE_CLOCK_GETTIME, 1,
	       [Define to 1 if you have the `clock_gettime' function.])])
fi
AC_SUBST(CLOCK_GETTIME_LINK)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([lib-config.h:../common/config.h.in])
AC_OUTPUT

