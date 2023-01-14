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
dnl The argument is the default model if none is specified.
AC_DEFUN([SIM_AC_OPTION_DEFAULT_MODEL],
[
default_sim_default_model="ifelse([$1],,0,[$1])"
AC_ARG_ENABLE(sim-default-model,
[AS_HELP_STRING([--enable-sim-default-model=model],
		[Specify default model to simulate])],
[case "${enableval}" in
  yes|no) AC_MSG_ERROR("Missing argument to --enable-sim-default-model");;
  *)	sim_default_model="-DWITH_DEFAULT_MODEL='\"${enableval}\"'";;
esac
if test x"$silent" != x"yes" && test x"$sim_default_model" != x""; then
  echo "Setting default model = $sim_default_model" 6>&1
fi],[sim_default_model="-DWITH_DEFAULT_MODEL='\"${default_sim_default_model}\"'"])
])
AC_SUBST(sim_default_model)
