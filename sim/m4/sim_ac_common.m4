dnl   Copyright (C) 1997-2021 Free Software Foundation, Inc.
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
dnl
dnl SIM_AC_COMMON invokes AC macros used by all simulators and by the common
dnl directory.  It is intended to be invoked before any target specific stuff.
dnl SIM_AC_OUTPUT is a cover function to AC_OUTPUT to generate the Makefile.
dnl It is intended to be invoked last.
dnl
dnl See README-HACKING for more details.
AC_DEFUN([SIM_AC_COMMON],
[dnl
SIM_AC_TOOLCHAIN
AC_CONFIG_HEADERS([config.h:config.in])

# Some of the common include files depend on bfd.h, and bfd.h checks
# that config.h is included first by testing that the PACKAGE macro
# is defined.
PACKAGE=sim
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of this package. ])
AC_SUBST(PACKAGE)

# Dependency checking.
ZW_CREATE_DEPDIR
ZW_PROG_COMPILER_DEPENDENCIES([CC])

dnl We don't use gettext, but bfd does.  So we do the appropriate checks
dnl to see if there are intl libraries we should link against.
ALL_LINGUAS=
ZW_GNU_GETTEXT_SISTER_DIR(../../intl)

# Check for socket libraries
AC_CHECK_LIB(socket, bind)
AC_CHECK_LIB(nsl, gethostbyname)

# BFD conditionally uses zlib, so we must link it in if libbfd does, by
# using the same condition.
AM_ZLIB

# BFD uses libdl when when plugins enabled.
AC_PLUGINS
AM_CONDITIONAL(PLUGINS, test "$plugins" = yes)
LT_INIT([dlopen])
AC_SUBST(lt_cv_dlopen_libs)

dnl We don't use automake, but we still want to support
dnl --enable-maintainer-mode.
AM_MAINTAINER_MODE

SIM_AC_OPTION_INLINE

ACX_PKGVERSION([SIM])
ACX_BUGURL([https://www.gnu.org/software/gdb/bugs/])
AC_DEFINE_UNQUOTED([PKGVERSION], ["$PKGVERSION"], [Additional package description])
AC_DEFINE_UNQUOTED([REPORT_BUGS_TO], ["$REPORT_BUGS_TO"], [Bug reporting address])

dnl Detect exe extension
AC_EXEEXT
]) dnl End of SIM_AC_COMMON
