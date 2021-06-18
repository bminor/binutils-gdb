dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_COMMON

SIM_AC_OPTION_SCACHE(16384)
SIM_AC_OPTION_HARDWARE(rv cris cris_900000xx)

# The default model shouldn't matter as long as there's a BFD.
SIM_AC_OPTION_DEFAULT_MODEL(crisv32)
SIM_AC_OPTION_CGEN_MAINT

SIM_AC_OUTPUT
