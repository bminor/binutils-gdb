dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)dnl
AC_INIT(Makefile.in)
AC_CONFIG_HEADER(config.h:config.in)

sinclude(../common/aclocal.m4)

# Bugs in autoconf 2.59 break the call to SIM_AC_COMMON, hack around
# it by inlining the macro's contents.
sinclude(../common/common.m4)

#SIM_AC_OPTION_ENDIAN(BIG_ENDIAN)
SIM_AC_OPTION_ALIGNMENT(STRICT_ALIGNMENT)
SIM_AC_OPTION_HOSTENDIAN
SIM_AC_OPTION_SCACHE(16384)
SIM_AC_OPTION_DEFAULT_MODEL(arc700)
SIM_AC_OPTION_ENVIRONMENT
SIM_AC_OPTION_INLINE()
SIM_AC_OPTION_CGEN_MAINT

  case "${target_alias}" in
  arc*-linux*)
    traps_obj=traps-linux.o
    sim_extra_cflags="-DARC_LINUX"
    ;;
  *)
    traps_obj=traps.o
    sim_extra_cflags="-DARC_ELF"
    ;;
  esac
AC_SUBST(traps_obj)
AC_SUBST(sim_extra_cflags)


SIM_AC_OUTPUT
