. ${srcdir}/emulparams/armelf.sh
. ${srcdir}/emulparams/elf_fbsd.sh
TARGET2_TYPE=got-rel
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
GENERATE_PIE_SCRIPT=yes

unset STACK_ADDR
unset EMBEDDED
