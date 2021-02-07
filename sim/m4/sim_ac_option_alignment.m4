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
dnl Specify the alignment restrictions of the target architecture.
dnl Without this option all possible alignment restrictions are accommodated.
dnl arg[1] is hardwired target alignment
dnl arg[2] is default target alignment
AC_DEFUN([SIM_AC_OPTION_ALIGNMENT],
wire_alignment="[$1]"
default_alignment="[$2]"
[
AC_ARG_ENABLE(sim-alignment,
[AS_HELP_STRING([--enable-sim-alignment=align],
		[Specify strict, nonstrict or forced alignment of memory accesses])],
[case "${enableval}" in
  strict | STRICT)       sim_alignment="-DWITH_ALIGNMENT=STRICT_ALIGNMENT";;
  nonstrict | NONSTRICT) sim_alignment="-DWITH_ALIGNMENT=NONSTRICT_ALIGNMENT";;
  forced | FORCED)       sim_alignment="-DWITH_ALIGNMENT=FORCED_ALIGNMENT";;
  yes) if test x"$wire_alignment" != x; then
	 sim_alignment="-DWITH_ALIGNMENT=${wire_alignment}"
       else
         if test x"$default_alignment" != x; then
           sim_alignment="-DWITH_ALIGNMENT=${default_alignment}"
         else
	   echo "No hard-wired alignment for target $target" 1>&6
	   sim_alignment="-DWITH_ALIGNMENT=0"
         fi
       fi;;
  no)  if test x"$default_alignment" != x; then
	 sim_alignment="-DWITH_DEFAULT_ALIGNMENT=${default_alignment}"
       else
         if test x"$wire_alignment" != x; then
	   sim_alignment="-DWITH_DEFAULT_ALIGNMENT=${wire_alignment}"
         else
           echo "No default alignment for target $target" 1>&6
           sim_alignment="-DWITH_DEFAULT_ALIGNMENT=0"
         fi
       fi;;
  *)   AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-alignment"); sim_alignment="";;
esac
if test x"$silent" != x"yes" && test x"$sim_alignment" != x""; then
  echo "Setting alignment flags = $sim_alignment" 6>&1
fi],
[if test x"$default_alignment" != x; then
  sim_alignment="-DWITH_DEFAULT_ALIGNMENT=${default_alignment}"
else
  if test x"$wire_alignment" != x; then
    sim_alignment="-DWITH_ALIGNMENT=${wire_alignment}"
  else
    sim_alignment=
  fi
fi])dnl
])dnl
AC_SUBST(sim_alignment)
