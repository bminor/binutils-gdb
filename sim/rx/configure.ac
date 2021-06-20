dnl Process this file with autoconf to produce a configure script.

dnl Copyright (C) 2005-2021 Free Software Foundation, Inc.
dnl Contributed by Red Hat, Inc.
dnl 
dnl This file is part of the GNU simulators.
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
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

AC_MSG_CHECKING([whether sim rx should be cycle accurate])
AC_ARG_ENABLE(sim-rx-cycle-accurate,
[AS_HELP_STRING([--disable-sim-rx-cycle-accurate],
		[Disable cycle accurate simulation (faster runtime)])],
[case "${enableval}" in
yes | no) ;;
*)	AC_MSG_ERROR(bad value ${enableval} given for --enable-sim-rx-cycle-accurate option) ;;
esac])
if test "x${enable_sim_rx_cycle_accurate}" != xno; then
  sim_rx_cycle_accurate_flags="-DCYCLE_ACCURATE"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(sim_rx_cycle_accurate_flags)

SIM_AC_OUTPUT
