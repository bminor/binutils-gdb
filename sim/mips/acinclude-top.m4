dnl TODO: Rename this to acinclude.m4 once mips/configure is removed.
dnl
dnl Copyright (C) 2005-2022 Free Software Foundation, Inc.
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
dnl NB: This file is included in sim/configure, so keep settings namespaced.

dnl DEPRECATED
dnl
dnl Instead of defining a `subtarget' macro, code should be checking the value
dnl of {STATE,CPU}_ARCHITECTURE to identify the architecture dnl in question.
AC_MSG_CHECKING([mips subtarget])
SIM_MIPS_SUBTARGET=
AS_CASE([${target}],
  [mips64vr*-*-*],  [SIM_MIPS_SUBTARGET="-DTARGET_ENABLE_FR=1"],
  [mips*tx39*],     [SIM_MIPS_SUBTARGET="-DSUBTARGET_R3900=1"],
  [mips*-sde-elf*], [SIM_MIPS_SUBTARGET="-DTARGET_ENABLE_FR=1"],
  [mips*-mti-elf*], [SIM_MIPS_SUBTARGET="-DTARGET_ENABLE_FR=1"],
  [mipsisa32*-*-*], [SIM_MIPS_SUBTARGET="-DTARGET_ENABLE_FR=1"],
  [mipsisa64*-*-*], [SIM_MIPS_SUBTARGET="-DTARGET_ENABLE_FR=1"])
AC_MSG_RESULT([${SIM_MIPS_SUBTARGET:-none}])
AC_SUBST(SIM_MIPS_SUBTARGET)

dnl Select the bitsize of the target.
AC_MSG_CHECKING([mips bitsize])
SIM_MIPS_BITSIZE=64
AS_CASE([${target}],
  [mips*-sde-elf*], [SIM_MIPS_BITSIZE=64],
  [mips*-mti-elf*], [SIM_MIPS_BITSIZE=64],
  [mips64*-*-*],    [SIM_MIPS_BITSIZE=64],
  [mips16*-*-*],    [SIM_MIPS_BITSIZE=64],
  [mipsisa32*-*-*], [SIM_MIPS_BITSIZE=32],
  [mipsisa64*-*-*], [SIM_MIPS_BITSIZE=64],
  [mips*-*-*],      [SIM_MIPS_BITSIZE=32])
AC_MSG_RESULT([$SIM_MIPS_BITSIZE])
AC_SUBST(SIM_MIPS_BITSIZE)
