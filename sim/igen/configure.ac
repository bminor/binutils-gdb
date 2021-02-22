dnl Process this file with autoconf to produce a configure script.
AC_INIT(table.h)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

AC_PROG_INSTALL
AC_PROG_CC

SIM_AC_OPTION_WARNINGS

# Put a plausible default for CC_FOR_BUILD in Makefile.
if test "x$cross_compiling" = "xno" -a "x$host" != "xi386-windows"; then
  CC_FOR_BUILD='$(CC)'
  CFLAGS_FOR_BUILD='$(CFLAGS)'
else
  CC_FOR_BUILD=${CC_FOR_BUILD-gcc}
  CFLAGS_FOR_BUILD=${CFLAGS_FOR_BUILD-"-g"}
fi

AC_CANONICAL_SYSTEM

AC_SUBST(CC_FOR_BUILD)
AC_SUBST(CFLAGS_FOR_BUILD)

AC_OUTPUT(Makefile)
