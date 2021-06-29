dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

  case "${target_alias}" in
  m32r*-linux*)
    traps_obj=traps-linux.o
    sim_extra_cflags="-DM32R_LINUX"
    ;;
  *)
    traps_obj=traps.o
    sim_extra_cflags="-DM32R_ELF"
    ;;
  esac
AC_SUBST(traps_obj)
AC_SUBST(sim_extra_cflags)

SIM_AC_OUTPUT
