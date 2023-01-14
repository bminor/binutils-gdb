dnl Process this file with autoconf to produce a configure script.
dnl NB: The version here is not used.  If gdb ever changes from generating its
dnl version at build time to autoconf time (like bfd et al do), we can switch.
AC_INIT([sim], [0],
  [https://sourceware.org/bugzilla/enter_bug.cgi?product=gdb&component=sim],
  [], [https://sourceware.org/gdb/wiki/Sim/])

SIM_AC_TOOLCHAIN

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE

# If a cpu ever has more than one simulator to choose from, use
# --enable-sim=... to choose.
AC_ARG_ENABLE(sim,
[AS_HELP_STRING([--enable-sim], [Enable the GNU simulator])],
[case "${enableval}" in
yes | no) ;;
*)	AC_MSG_ERROR(bad value ${enableval} given for --enable-sim option) ;;
esac])

AC_ARG_ENABLE([example-sims],
	      [AC_HELP_STRING([--enable-example-sims],
			      [enable example GNU simulators])])

AC_ARG_ENABLE(targets,
[  --enable-targets        alternative target configurations],
[case "${enableval}" in
  yes | "") AC_MSG_ERROR(enable-targets option must specify target names or 'all')
	    ;;
  no)       enable_targets= ;;
  *)        enable_targets=$enableval ;;
esac])

dnl Used to keep track of which target (if any) is the default one.  This is
dnl used when installing files to see if they need to be suffixed.
SIM_PRIMARY_TARGET=
AC_SUBST(SIM_PRIMARY_TARGET)

m4_define([SIM_TARGET], [
  case "${targ}" in
    all|$1)
      if test "${targ}" = "${target}"; then
        SIM_PRIMARY_TARGET=$2
      fi
      AC_CONFIG_SUBDIRS($2)
      $3
      ;;
  esac
])

dnl WHEN ADDING ENTRIES TO THIS MATRIX:
dnl Make sure that the left side always has two dashes.  Otherwise you can get
dnl spurious matches.  Even for unambiguous cases, do this as a convention, else
dnl the table becomes a real mess to understand and maintain.
if test "${enable_sim}" != no; then
  sim_igen=no
  for targ in `echo $target $enable_targets | sed 's/,/ /g'`
  do
    SIM_TARGET([aarch64*-*-*], [aarch64])
    SIM_TARGET([arm*-*-*], [arm])
    SIM_TARGET([avr*-*-*], [avr])
    SIM_TARGET([bfin-*-*], [bfin])
    SIM_TARGET([bpf-*-*], [bpf])
    SIM_TARGET([cr16*-*-*], [cr16])
    SIM_TARGET([cris-*-* | crisv32-*-*], [cris])
    SIM_TARGET([d10v-*-*], [d10v])
    SIM_TARGET([frv-*-*], [frv])
    SIM_TARGET([h8300*-*-*], [h8300])
    SIM_TARGET([iq2000-*-*], [iq2000])
    SIM_TARGET([lm32-*-*], [lm32])
    SIM_TARGET([m32c-*-*], [m32c])
    SIM_TARGET([m32r-*-*], [m32r])
    SIM_TARGET([m68hc11-*-*|m6811-*-*], [m68hc11])
    SIM_TARGET([mcore-*-*], [mcore])
    SIM_TARGET([microblaze-*-*], [microblaze])
    SIM_TARGET([mips*-*-*], [mips], [sim_igen=yes])
    SIM_TARGET([mn10300*-*-*], [mn10300], [sim_igen=yes])
    SIM_TARGET([moxie-*-*], [moxie])
    SIM_TARGET([msp430*-*-*], [msp430])
    SIM_TARGET([or1k-*-* | or1knd-*-*], [or1k])
    SIM_TARGET([pru*-*-*], [pru])
    SIM_TARGET([riscv*-*-*], [riscv])
    SIM_TARGET([rl78-*-*], [rl78])
    SIM_TARGET([rx-*-*], [rx])
    SIM_TARGET([sh*-*-*], [sh])
    SIM_TARGET([sparc-*-rtems*|sparc-*-elf*], [erc32])
    SIM_TARGET([powerpc*-*-*], [ppc])
    SIM_TARGET([ft32-*-*], [ft32])
    SIM_TARGET([v850*-*-*], [v850], [sim_igen=yes])
  done

  if test "x${enable_example_sims}" = xyes; then
    AC_CONFIG_SUBDIRS(example-synacor)
  fi
fi
AM_CONDITIONAL([SIM_ENABLE_IGEN], [test "$sim_igen" = "yes"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
