dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_COMMON

# For dv-rv and rvdummy.
AC_CHECK_HEADERS_ONCE(m4_flatten([
  sys/param.h
  sys/select.h
  sys/socket.h
]))

SIM_AC_OPTION_ENDIAN(LITTLE)
SIM_AC_OPTION_ALIGNMENT(NONSTRICT_ALIGNMENT)
SIM_AC_OPTION_SCACHE(16384)
SIM_AC_OPTION_WARNINGS(no)
SIM_AC_OPTION_HARDWARE(rv cris cris_900000xx)

# The default model shouldn't matter as long as there's a BFD.
SIM_AC_OPTION_DEFAULT_MODEL(crisv32)
SIM_AC_OPTION_CGEN_MAINT

SIM_AC_OUTPUT
