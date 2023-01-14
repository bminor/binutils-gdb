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

# Check for the 'make' the user wants to use.
AC_CHECK_PROGS(MAKE, make)
MAKE_IS_GNU=
case "`$MAKE --version 2>&1 | sed 1q`" in
  *GNU*)
    MAKE_IS_GNU=yes
    ;;
esac
AM_CONDITIONAL(GMAKE, test "$MAKE_IS_GNU" = yes)

dnl We don't use gettext, but bfd does.  So we do the appropriate checks
dnl to see if there are intl libraries we should link against.
ALL_LINGUAS=
ZW_GNU_GETTEXT_SISTER_DIR(../../intl)

# Check for common headers.
# NB: You can assume C11 headers exist.
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(sys/time.h sys/times.h sys/resource.h sys/mman.h)
AC_CHECK_HEADERS(fcntl.h fpu_control.h)
AC_CHECK_HEADERS(dlfcn.h sys/stat.h)
AC_CHECK_FUNCS(getrusage time sigaction __setfpucw)
AC_CHECK_FUNCS(mmap munmap lstat truncate ftruncate posix_fallocate)
AC_CHECK_MEMBERS([[struct stat.st_dev], [struct stat.st_ino],
[struct stat.st_mode], [struct stat.st_nlink], [struct stat.st_uid],
[struct stat.st_gid], [struct stat.st_rdev], [struct stat.st_size],
[struct stat.st_blksize], [struct stat.st_blocks], [struct stat.st_atime],
[struct stat.st_mtime], [struct stat.st_ctime]], [], [],
[[#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif]])
AC_CHECK_TYPES(socklen_t, [], [],
[#include <sys/types.h>
#include <sys/socket.h>
])

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

dnl Standard (and optional) simulator options.
dnl Eventually all simulators will support these.
dnl Do not add any here that cannot be supported by all simulators.
dnl Do not add similar but different options to a particular simulator,
dnl all shall eventually behave the same way.


dnl We don't use automake, but we still want to support
dnl --enable-maintainer-mode.
AM_MAINTAINER_MODE


dnl --enable-sim-debug is for developers of the simulator
dnl the allowable values are work-in-progress
AC_MSG_CHECKING([for sim debug setting])
sim_debug="0"
AC_ARG_ENABLE(sim-debug,
[AS_HELP_STRING([--enable-sim-debug=opts],
		[Enable debugging flags (for developers of the sim itself)])],
[case "${enableval}" in
  yes) sim_debug="7";;
  no)  sim_debug="0";;
  *)   sim_debug="($enableval)";;
esac])dnl
if test "$sim_debug" != "0"; then
  AC_DEFINE_UNQUOTED([DEBUG], [$sim_debug], [Sim debug setting])
fi
AC_DEFINE_UNQUOTED([WITH_DEBUG], [$sim_debug], [Sim debug setting])
AC_MSG_RESULT($sim_debug)


dnl --enable-sim-stdio is for users of the simulator
dnl It determines if IO from the program is routed through STDIO (buffered)
AC_MSG_CHECKING([for sim stdio debug behavior])
sim_stdio="0"
AC_ARG_ENABLE(sim-stdio,
[AS_HELP_STRING([--enable-sim-stdio],
		[Specify whether to use stdio for console input/output])],
[case "${enableval}" in
  yes)	sim_stdio="DO_USE_STDIO";;
  no)	sim_stdio="DONT_USE_STDIO";;
  *)	AC_MSG_ERROR([Unknown value $enableval passed to --enable-sim-stdio]);;
esac])dnl
AC_DEFINE_UNQUOTED([WITH_STDIO], [$sim_stdio], [How to route I/O])
AC_MSG_RESULT($sim_stdio)


dnl --enable-sim-trace is for users of the simulator
dnl The argument is either a bitmask of things to enable [exactly what is
dnl up to the simulator], or is a comma separated list of names of tracing
dnl elements to enable.  The latter is only supported on simulators that
dnl use WITH_TRACE.  Default to all tracing but internal debug.
AC_MSG_CHECKING([for sim trace settings])
sim_trace="~TRACE_debug"
AC_ARG_ENABLE(sim-trace,
[AS_HELP_STRING([--enable-sim-trace=opts],
		[Enable tracing of simulated programs])],
[case "${enableval}" in
  yes)	sim_trace="-1";;
  no)	sim_trace="0";;
  [[-0-9]]*)
	sim_trace="'(${enableval})'";;
  [[[:lower:]]]*)
	sim_trace=""
	for x in `echo "$enableval" | sed -e "s/,/ /g"`; do
	  if test x"$sim_trace" = x; then
	    sim_trace="(TRACE_$x"
	  else
	    sim_trace="${sim_trace}|TRACE_$x"
	  fi
	done
	sim_trace="$sim_trace)" ;;
esac])dnl
AC_DEFINE_UNQUOTED([WITH_TRACE], [$sim_trace], [Sim trace settings])
AC_MSG_RESULT($sim_trace)


dnl --enable-sim-profile
dnl The argument is either a bitmask of things to enable [exactly what is
dnl up to the simulator], or is a comma separated list of names of profiling
dnl elements to enable.  The latter is only supported on simulators that
dnl use WITH_PROFILE.
AC_MSG_CHECKING([for sim profile settings])
profile="1"
sim_profile="-1"
AC_ARG_ENABLE(sim-profile,
[AS_HELP_STRING([--enable-sim-profile=opts], [Enable profiling flags])],
[case "${enableval}" in
  yes)	profile="1" sim_profile="-1";;
  no)	profile="0" sim_profile="0";;
  [[-0-9]]*)
	profile="(${enableval})" sim_profile="(${enableval})";;
  [[a-z]]*)
    profile="1"
	sim_profile=""
	for x in `echo "$enableval" | sed -e "s/,/ /g"`; do
	  if test x"$sim_profile" = x; then
	    sim_profile="(PROFILE_$x"
	  else
	    sim_profile="${sim_profile}|PROFILE_$x"
	  fi
	done
	sim_profile="$sim_profile)" ;;
esac])dnl
AC_DEFINE_UNQUOTED([PROFILE], [$profile], [Sim profile settings])
AC_DEFINE_UNQUOTED([WITH_PROFILE], [$sim_profile], [Sim profile settings])
AC_MSG_RESULT($sim_profile)


SIM_AC_OPTION_ASSERT
SIM_AC_OPTION_ENVIRONMENT
SIM_AC_OPTION_INLINE

ACX_PKGVERSION([SIM])
ACX_BUGURL([https://www.gnu.org/software/gdb/bugs/])
AC_DEFINE_UNQUOTED([PKGVERSION], ["$PKGVERSION"], [Additional package description])
AC_DEFINE_UNQUOTED([REPORT_BUGS_TO], ["$REPORT_BUGS_TO"], [Bug reporting address])

dnl Types used by common code
AC_TYPE_SIGNAL

dnl Detect exe extension
AC_EXEEXT
]) dnl End of SIM_AC_COMMON
