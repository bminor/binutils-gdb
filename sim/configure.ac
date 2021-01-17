dnl Process this file with autoconf to produce a configure script.
dnl NB: The version here is not used.  If gdb ever changes from generating its
dnl version at build time to autoconf time (like bfd et al do), we can switch.
AC_INIT([sim], [0],
  [https://sourceware.org/bugzilla/enter_bug.cgi?product=gdb&component=sim],
  [], [https://sourceware.org/gdb/wiki/Sim/])

AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(RANLIB, ranlib, :)

AC_ARG_PROGRAM
AC_PROG_CC
AC_PROG_CPP
AC_SUBST(CFLAGS)
AR=${AR-ar}
AC_SUBST(AR)
AC_PROG_RANLIB

# Put a plausible default for CC_FOR_BUILD in Makefile.
if test -z "$CC_FOR_BUILD"; then
  if test "x$cross_compiling" = "xno"; then
    CC_FOR_BUILD='$(CC)'
  else
    CC_FOR_BUILD=gcc
  fi
fi
AC_SUBST(CC_FOR_BUILD)
CFLAGS_FOR_BUILD=${CFLAGS_FOR_BUILD-${CFLAGS}}
AC_SUBST(CFLAGS_FOR_BUILD)

# If a cpu ever has more than one simulator to choose from, use
# --enable-sim=... to choose.
AC_ARG_ENABLE(sim,
[AS_HELP_STRING([--enable-sim], [Enable the GNU simulator])],
[case "${enableval}" in
yes | no) ;;
*)	AC_MSG_ERROR(bad value ${enableval} given for --enable-sim option) ;;
esac])

dnl WHEN ADDING ENTRIES TO THIS MATRIX:
dnl Make sure that the left side always has two dashes.  Otherwise you can get
dnl spurious matches.  Even for unambiguous cases, do this as a convention, else
dnl the table becomes a real mess to understand and maintain.
if test "${enable_sim}" != no; then
  sim_igen=no
  case "${target}" in
    aarch64*-*-*)
      AC_CONFIG_SUBDIRS(aarch64)
      ;;
    arm*-*-*)
      AC_CONFIG_SUBDIRS(arm)
      ;;
    avr*-*-*)
      AC_CONFIG_SUBDIRS(avr)
      ;;
    bfin-*-*)
      AC_CONFIG_SUBDIRS(bfin)
      ;;
    bpf-*-*)
      AC_CONFIG_SUBDIRS(bpf)
      ;;
    cr16*-*-*)
      AC_CONFIG_SUBDIRS(cr16)
      ;;
    cris-*-* | crisv32-*-*)
      AC_CONFIG_SUBDIRS(cris)
      ;;
    d10v-*-*)
      AC_CONFIG_SUBDIRS(d10v)
      ;;
    frv-*-*)
      AC_CONFIG_SUBDIRS(frv)
      ;;
    h8300*-*-*)
      AC_CONFIG_SUBDIRS(h8300)
      ;;
    iq2000-*-*)
      AC_CONFIG_SUBDIRS(iq2000)
      ;;
    lm32-*-*)
      AC_CONFIG_SUBDIRS(lm32)
      ;;
    m32c-*-*)
      AC_CONFIG_SUBDIRS(m32c)
      ;;
    m32r-*-*)
      AC_CONFIG_SUBDIRS(m32r)
      ;;
    m68hc11-*-*|m6811-*-*)
      AC_CONFIG_SUBDIRS(m68hc11)
      ;;
    mcore-*-*)
      AC_CONFIG_SUBDIRS(mcore)
      ;;
    microblaze-*-*)
      AC_CONFIG_SUBDIRS(microblaze)
      ;;
    mips*-*-*)
      AC_CONFIG_SUBDIRS(mips)
      sim_igen=yes
      ;;
    mn10300*-*-*)
      AC_CONFIG_SUBDIRS(mn10300)
      sim_igen=yes
      ;;
    moxie-*-*)
      AC_CONFIG_SUBDIRS(moxie)
      ;;
    msp430*-*-*)
      AC_CONFIG_SUBDIRS(msp430)
      ;;
    or1k-*-* | or1knd-*-*)
      AC_CONFIG_SUBDIRS(or1k)
      ;;
    pru*-*-*)
      AC_CONFIG_SUBDIRS(pru)
      ;;
    riscv*-*-*)
      AC_CONFIG_SUBDIRS(riscv)
      ;;
    rl78-*-*)
      AC_CONFIG_SUBDIRS(rl78)
      ;;
    rx-*-*)
      AC_CONFIG_SUBDIRS(rx)
      ;;
    sh*-*-*)
      AC_CONFIG_SUBDIRS(sh)
      ;;
    sparc-*-rtems*|sparc-*-elf*)
      AC_CONFIG_SUBDIRS(erc32)
      ;;
    powerpc*-*-*)
      AC_CONFIG_SUBDIRS(ppc)
      ;;
    ft32-*-*)
      AC_CONFIG_SUBDIRS(ft32)
      ;;
    v850*-*-*)
      AC_CONFIG_SUBDIRS(v850)
      sim_igen=yes
      ;;
  esac
  if test "$sim_igen" = yes; then
    AC_CONFIG_SUBDIRS(igen)
  fi
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
