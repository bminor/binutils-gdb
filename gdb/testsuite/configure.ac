#                                                       -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright 2002-2015 Free Software Foundation, Inc.
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
AC_INIT(gdb.base)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

ACX_NONCANONICAL_TARGET

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

# Enable gdbtk.
AC_ARG_ENABLE(gdbtk,
[  --enable-gtk            enable gdbtk graphical user interface (GUI)],,
  [if test -d $srcdir/../gdbtk && test -d $srcdir/gdb.gdbtk; then
    enable_gdbtk=yes
  else
    enable_gdbtk=no
  fi])
# We unconditionally disable gdbtk tests on selected platforms.
case $host_os in
  go32* | windows*)
    enable_gdbtk=no ;;
esac

# Add gdbtk tests when appropriate.
if test $enable_gdbtk = yes; then
   AC_CONFIG_SUBDIRS(gdb.gdbtk)
fi

# Enable shared libraries.
AC_ARG_ENABLE(shared,
[  --enable-shared         build shared libraries [deault=yes]],,
  enable_shared=yes)

# If we have shared libraries, try to set RPATH_ENVVAR reasonably,
# such that we can find the shared libraries in the build tree.
if test $enable_shared = no; then
  # The variable `RPATH_ENVVAR' itself is not likely to be used on any
  # platform.
  RPATH_ENVVAR=RPATH_ENVVAR
else
  # The variable `LD_LIBRARY_PATH' is used on most platforms.
  RPATH_ENVVAR=LD_LIBRARY_PATH
  # The following exceptions are taken from Libtool 1.4.3.
  case $host_os in
    aix*)
      if test $host_cpu != ia64; then
        RPATH_ENVVAR=LIBPATH
      fi ;;
    darwin* | rhapsody*)
      RPATH_ENVVAR=DYLD_LIBRARY_PATH ;;
    hpux*)
      RPATH_ENVVAR=SHLIB_PATH ;;
  esac
fi
AC_SUBST(RPATH_ENVVAR)

AC_CHECK_HEADERS(pthread.h)

AC_EXEEXT

if test "${build}" = "${host}" -a "${host}" = "${target}"; then
   case "${host}" in
     *gnu*)
       EXTRA_RULES=read1
       ;;
   esac
fi
AC_SUBST(EXTRA_RULES)

# Transform the name of some programs and generate the lib/pdtrace
# test tool.
AC_ARG_PROGRAM
transform=`echo "$program_transform_name" | sed -e 's/[\\$][\\$]/\\$/g'`
STRIP_TRANSFORM_NAME=`echo strip | sed -e "$transform"`
if test "x$STRIP_TRANSFORM_NAME" = x; then
  STRIP_TRANSFORM_NAME=strip
fi
AC_SUBST(STRIP_TRANSFORM_NAME)
READELF_TRANSFORM_NAME=`echo readelf | sed -e "$transform"`
if test "x$READELF_TRANSFORM_NAME" = x; then
  READELF_TRANSFORM_NAME=readelf
fi
AC_SUBST(READELF_TRANSFORM_NAME)
GAS_TRANSFORM_NAME=`echo as | sed -e "$transform"`
if test "x$GAS_TRANSFORM_NAME" = x; then
  GAS_TRANSFORM_NAME=as
fi
AC_SUBST(GAS_TRANSFORM_NAME)
NM_TRANSFORM_NAME=`echo nm | sed -e "$transform"`
if test "x$NM_TRANSFORM_NAME" = x; then
  NM_TRANSFORM_NAME=nm
fi
AC_SUBST(NM_TRANSFORM_NAME)
AC_CONFIG_FILES([lib/pdtrace], [chmod +x lib/pdtrace])

AC_OUTPUT([Makefile \
  gdb.ada/Makefile \
  gdb.arch/Makefile gdb.asm/Makefile gdb.base/Makefile gdb.btrace/Makefile \
  gdb.cell/Makefile gdb.compile/Makefile gdb.cp/Makefile gdb.disasm/Makefile \
  gdb.dwarf2/Makefile gdb.dlang/Makefile gdb.fortran/Makefile gdb.gdb/Makefile \
  gdb.go/Makefile gdb.server/Makefile gdb.java/Makefile \
  gdb.hp/Makefile gdb.hp/gdb.objdbg/Makefile gdb.hp/gdb.base-hp/Makefile \
  gdb.hp/gdb.aCC/Makefile gdb.hp/gdb.compat/Makefile \
  gdb.hp/gdb.defects/Makefile gdb.guile/Makefile gdb.linespec/Makefile \
  gdb.mi/Makefile gdb.modula2/Makefile gdb.multi/Makefile \
  gdb.objc/Makefile gdb.opencl/Makefile gdb.opt/Makefile gdb.pascal/Makefile \
  gdb.perf/Makefile gdb.python/Makefile gdb.reverse/Makefile gdb.stabs/Makefile \
  gdb.threads/Makefile gdb.trace/Makefile gdb.xml/Makefile])
