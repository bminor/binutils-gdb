dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_OPTION_SCACHE(16384)

dnl Enable making unknown traps dump out registers
AC_MSG_CHECKING([whether sim frv should dump cpu state on unknown traps])
AC_ARG_ENABLE(sim-frv-trapdump,
[AS_HELP_STRING([--enable-sim-frv-trapdump],
		[Make unknown traps dump the registers])],
[case "${enableval}" in
yes|no) ;;
*) AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-trapdump");;
esac])
if test "x${enable_sim_frv_trapdump}" != xno; then
  SIM_FRV_TRAPDUMP_FLAGS="-DTRAPDUMP=1"
  AC_MSG_RESULT([yes])
else
  SIM_FRV_TRAPDUMP_FLAGS="-DTRAPDUMP=0"
  AC_MSG_RESULT([no])
fi
AC_SUBST(SIM_FRV_TRAPDUMP_FLAGS)

SIM_AC_OUTPUT
