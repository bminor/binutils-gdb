#                                                       -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright 2011
# Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ(2.59)
AC_INIT(.)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_PROG_CC
AC_PROG_RANLIB

AC_CHECK_PROGS([ACLOCAL], [aclocal], [$MISSING aclocal])
AC_CHECK_PROGS([AUTOCONF], [autoconf], [$MISSING autoconf])
AC_CHECK_PROGS([AUTOHEADER], [autoheader], [$MISSING autoheader])

# Check for the 'make' the user wants to use.
AC_CHECK_PROGS(MAKE, make)
MAKE_IS_GNU=
case "`$MAKE --version 2>&1 | sed 1q`" in
  *GNU*)
    MAKE_IS_GNU=yes
    ;;
esac
AM_CONDITIONAL(GMAKE, test "$MAKE_IS_GNU" = yes)
AC_PROG_MAKE_SET

AC_SUBST(COMMON_CPU_OBJ)
AC_SUBST(COMMON_CPU_SRC)

# Add different object files to libcommon.a according to different host_cpu.
case "$host_cpu" in
  *)
    COMMON_CPU_OBJ=""
    COMMON_CPU_SRC=""
    ;;
esac

# Determine whether or not build libcommon.a for gdbserver
AC_ARG_ENABLE(gdbserver,
[  --enable-gdbserver       build libcommon.a for gdbserver],
[], [enable_gdbserver=no])

if test x"$enable_gdbserver" = xyes; then
  GDB_FLAGS="-DGDBSERVER"
  GDB_INCLUDE="-I\$(srcdir)/../gdbserver/"
else
  GDB_FLAGS=""
  GDB_INCLUDE="-I\$(srcdir)/../ -I\$(BFD_DIR)"
fi

AC_SUBST(GDB_FLAGS)
AC_SUBST(GDB_INCLUDE)

AC_EXEEXT

AC_OUTPUT([Makefile])
