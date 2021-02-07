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
dnl --enable-sim-endian={yes,no,big,little} is for simulators
dnl that support both big and little endian targets.
dnl arg[1] is hardwired target endianness.
dnl arg[2] is default target endianness.
AC_DEFUN([SIM_AC_OPTION_ENDIAN],
[
wire_endian="[$1]"
default_endian="[$2]"
AC_ARG_ENABLE(sim-endian,
[AS_HELP_STRING([--enable-sim-endian=endian],
		[Specify target byte endian orientation])],
[case "${enableval}" in
  b*|B*) sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_BIG";;
  l*|L*) sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_LITTLE";;
  yes)	 if test x"$wire_endian" != x; then
	   sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_${wire_endian}"
	 else
	  if test x"$default_endian" != x; then
	     sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_${default_endian}"
	   else
	     echo "No hard-wired endian for target $target" 1>&6
	     sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_UNKNOWN"
	   fi
	 fi;;
  no)	 if test x"$default_endian" != x; then
	   sim_endian="-DWITH_DEFAULT_TARGET_BYTE_ORDER=BFD_ENDIAN_${default_endian}"
	 else
	   if test x"$wire_endian" != x; then
	     sim_endian="-DWITH_DEFAULT_TARGET_BYTE_ORDER=BFD_ENDIAN_${wire_endian}"
	   else
	     echo "No default endian for target $target" 1>&6
	     sim_endian="-DWITH_DEFAULT_TARGET_BYTE_ORDER=BFD_ENDIAN_UNKNOWN"
	   fi
	 fi;;
  *)	 AC_MSG_ERROR("Unknown value $enableval for --enable-sim-endian"); sim_endian="";;
esac
if test x"$silent" != x"yes" && test x"$sim_endian" != x""; then
  echo "Setting endian flags = $sim_endian" 6>&1
fi],
[if test x"$default_endian" != x; then
  sim_endian="-DWITH_DEFAULT_TARGET_BYTE_ORDER=BFD_ENDIAN_${default_endian}"
else
  if test x"$wire_endian" != x; then
    sim_endian="-DWITH_TARGET_BYTE_ORDER=BFD_ENDIAN_${wire_endian}"
  else
    sim_endian=
  fi
fi])dnl
])
AC_SUBST(sim_endian)
