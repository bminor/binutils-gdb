dnl Process this file with autoconf to produce a configure script.
AC_INIT(table.h)
AC_CONFIG_MACRO_DIRS([../common ../.. ../../config])

AC_PROG_INSTALL
AC_PROG_CC

SIM_AC_OPTION_WARNINGS

# Put a plausible default for CC_FOR_BUILD in Makefile.
if test "x$cross_compiling" = "xno" -a "x$host" != "xi386-windows"; then
  AR_FOR_BUILD='$(AR)'
  AR_FLAGS_FOR_BUILD='$(AR_FLAGS)'
  CC_FOR_BUILD='$(CC)'
  CFLAGS_FOR_BUILD='$(CFLAGS)'
  RANLIB_FOR_BUILD='$(RANLIB)'
else
  AR_FOR_BUILD=${AR_FOR_BUILD-ar}
  AR_FLAGS_FOR_BUILD=${AR_FLAGS_FOR_BUILD-rc}
  CC_FOR_BUILD=${CC_FOR_BUILD-gcc}
  CFLAGS_FOR_BUILD=${CFLAGS_FOR_BUILD-"-g"}
  RANLIB_FOR_BUILD=${RANLIB_FOR_BUILD-ranlib}
fi


AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM

AC_CONFIG_HEADER(config.h:config.in)

# NB: You can assume C11 headers exist.
AC_CHECK_HEADERS(sys/stat.h sys/types.h unistd.h)
AC_HEADER_DIRENT

AC_SUBST(AR_FOR_BUILD)
AC_SUBST(AR_FLAGS_FOR_BUILD)
AC_SUBST(CC_FOR_BUILD)
AC_SUBST(CFLAGS_FOR_BUILD)
AC_SUBST(RANLIB_FOR_BUILD)

AC_SUBST(AR)
AC_SUBST(CFLAGS)
AC_PROG_RANLIB

AC_OUTPUT(Makefile,
[case x$CONFIG_HEADERS in xconfig.h:config.in) echo > stamp-h ;; esac])
