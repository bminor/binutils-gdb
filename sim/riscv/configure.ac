dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_COMMON

SIM_AC_OPTION_ENDIAN(LITTLE)
SIM_AC_OPTION_ALIGNMENT(NONSTRICT_ALIGNMENT)
SIM_AC_OPTION_ENVIRONMENT
SIM_AC_OPTION_WARNINGS

# Select the default model for the target.
riscv_model=
case "${target}" in
riscv32*) riscv_model="RV32G" ;;
riscv*) riscv_model="RV64G" ;;
esac
SIM_AC_OPTION_DEFAULT_MODEL(${riscv_model})

# Select the bitsize of the target.
riscv_addr_bitsize=
case "${target}" in
riscv32*) riscv_addr_bitsize=32 ;;
riscv*) riscv_addr_bitsize=64 ;;
esac
SIM_AC_OPTION_BITSIZE($riscv_addr_bitsize)

SIM_AC_OUTPUT
