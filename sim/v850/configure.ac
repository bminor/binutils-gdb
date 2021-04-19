dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_COMMON

SIM_AC_OPTION_ENDIAN(LITTLE)
SIM_AC_OPTION_ALIGNMENT(,NONSTRICT_ALIGNMENT)
SIM_AC_OPTION_RESERVED_BITS
SIM_AC_OPTION_BITSIZE(32,31)

AC_CHECK_FUNCS_ONCE(chmod utime fork execve execv chown)
AC_CHECK_HEADERS_ONCE(utime.h)

SIM_AC_OUTPUT
