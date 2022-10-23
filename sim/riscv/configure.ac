dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

dnl The sim shouldn't be checking $target and changing behavior.  But it is,
dnl and until we clean that up, we need to expand --target for use below.
AC_CANONICAL_SYSTEM

# Select the bitsize of the target.
riscv_addr_bitsize=
case "${target}" in
riscv32*) riscv_addr_bitsize=32 ;;
riscv*) riscv_addr_bitsize=64 ;;
esac
SIM_AC_OPTION_BITSIZE($riscv_addr_bitsize)

SIM_AC_OUTPUT
