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
AC_CHECK_HEADERS(byteswap.h)
AC_CHECK_FUNCS(pread)

AC_CONFIG_FILES(Makefile)
AC_CONFIG_HEADERS(config.h)
AC_OUTPUT
