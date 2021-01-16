dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
sinclude(../common/acinclude.m4)

SIM_AC_COMMON

dnl Options available in this module
SIM_AC_OPTION_ENDIAN(BIG)
SIM_AC_OPTION_ALIGNMENT(NONSTRICT_ALIGNMENT)
SIM_AC_OPTION_WARNINGS(no)
SIM_AC_OPTION_HARDWARE(yes,,
  m68hc11 m68hc11sio m68hc11eepr m68hc11tim m68hc11spi nvram)

SIM_AC_OUTPUT
