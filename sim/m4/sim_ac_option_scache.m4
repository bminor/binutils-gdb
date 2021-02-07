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
dnl The argument is the default cache size if none is specified.
AC_DEFUN([SIM_AC_OPTION_SCACHE],
[
default_sim_scache="ifelse([$1],,0,[$1])"
AC_ARG_ENABLE(sim-scache,
[AS_HELP_STRING([--enable-sim-scache=size],
		[Specify simulator execution cache size])],
[case "${enableval}" in
  yes)	sim_scache="-DWITH_SCACHE=${default_sim_scache}";;
  no)	sim_scache="-DWITH_SCACHE=0" ;;
  [[0-9]]*) sim_scache="-DWITH_SCACHE=${enableval}";;
  *)	AC_MSG_ERROR("Bad value $enableval passed to --enable-sim-scache");
	sim_scache="";;
esac
if test x"$silent" != x"yes" && test x"$sim_scache" != x""; then
  echo "Setting scache size = $sim_scache" 6>&1
fi],[sim_scache="-DWITH_SCACHE=${default_sim_scache}"])
])
AC_SUBST(sim_scache)
