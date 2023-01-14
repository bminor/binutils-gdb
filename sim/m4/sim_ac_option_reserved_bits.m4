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
AC_DEFUN([SIM_AC_OPTION_RESERVED_BITS],
[
default_sim_reserved_bits="ifelse([$1],,1,[$1])"
AC_ARG_ENABLE(sim-reserved-bits,
[AS_HELP_STRING([--enable-sim-reserved-bits],
		[Specify whether to check reserved bits in instruction])],
[case "${enableval}" in
  yes)	sim_reserved_bits="-DWITH_RESERVED_BITS=1";;
  no)	sim_reserved_bits="-DWITH_RESERVED_BITS=0";;
  *)	AC_MSG_ERROR("--enable-sim-reserved-bits does not take a value"); sim_reserved_bits="";;
esac
if test x"$silent" != x"yes" && test x"$sim_reserved_bits" != x""; then
  echo "Setting reserved flags = $sim_reserved_bits" 6>&1
fi],[sim_reserved_bits="-DWITH_RESERVED_BITS=${default_sim_reserved_bits}"])dnl
])
AC_SUBST(sim_reserved_bits)
