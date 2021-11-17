/*
#notarget: cris*-*-elf
#xerror:
#cc: additional_flags=-Wl,-dynamic-linker,/compilercheck.x
#sim: --sysroot=@exedir@
#output: *: could not load ELF interpreter `*' for program `*'\n
 */
#include "hello.c"
