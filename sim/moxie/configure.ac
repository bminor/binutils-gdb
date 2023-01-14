dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_COMMON

AC_CHECK_TOOL(DTC, dtc)

SIM_AC_OPTION_ENDIAN(BIG)
SIM_AC_OPTION_ALIGNMENT(STRICT_ALIGNMENT)

SIM_AC_OUTPUT
