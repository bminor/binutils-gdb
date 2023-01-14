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
dnl --enable-sim-hardware is for users of the simulator
dnl arg[1] Enable sim-hw by default? ("yes" or "no")
dnl arg[2] is a space separated list of devices that override the defaults
dnl arg[3] is a space separated list of extra target specific devices.
AC_DEFUN([SIM_AC_OPTION_HARDWARE],
[
if test "[$2]"; then
  hardware="[$2]"
else
  hardware="cfi core pal glue"
fi
hardware="$hardware [$3]"

sim_hw_cflags="-DWITH_HW=1"
sim_hw="$hardware"
sim_hw_objs="\$(SIM_COMMON_HW_OBJS) `echo $sim_hw | sed -e 's/\([[^ ]][[^ ]]*\)/dv-\1.o/g'`"

AC_ARG_ENABLE(sim-hardware,
  [AS_HELP_STRING([--enable-sim-hardware=LIST],
                  [Specify the hardware to be included in the build.])],
  ,[enable_sim_hardware="[$1]"])
case ${enable_sim_hardware} in
  yes|no) ;;
  ,*) hardware="${hardware} `echo ${enableval} | sed -e 's/,/ /'`";;
  *,) hardware="`echo ${enableval} | sed -e 's/,/ /'` ${hardware}";;
  *)  hardware="`echo ${enableval} | sed -e 's/,/ /'`"'';;
esac

if test "$enable_sim_hardware" = no; then
  sim_hw_objs=
  sim_hw_cflags="-DWITH_HW=0"
  sim_hw=
else
  sim_hw_cflags="-DWITH_HW=1"
  # remove duplicates
  sim_hw=""
  sim_hw_objs="\$(SIM_COMMON_HW_OBJS)"
  for i in $hardware ; do
    case " $sim_hw " in
      *" $i "*) ;;
      *) sim_hw="$sim_hw $i" ; sim_hw_objs="$sim_hw_objs dv-$i.o";;
    esac
  done
  # mingw does not support sockser
  case ${host} in
    *mingw*) ;;
    *) # TODO: We don't add dv-sockser to sim_hw as it is not a "real" device
       # that you instatiate.  Instead, other code will call into it directly.
       # At some point, we should convert it over.
       sim_hw_objs="$sim_hw_objs dv-sockser.o"
       AC_DEFINE_UNQUOTED(
         [HAVE_DV_SOCKSER], 1, [Define if dv-sockser is usable.])
       ;;
  esac
  if test x"$silent" != x"yes"; then
    echo "Setting hardware to $sim_hw_cflags, $sim_hw, $sim_hw_objs"
  fi
  dnl Some devices require extra libraries.
  case " $hardware " in
    *" cfi "*) AC_CHECK_LIB(m, log2);;
  esac
fi
])
AC_SUBST(sim_hw_cflags)
AC_SUBST(sim_hw_objs)
AC_SUBST(sim_hw)
