#source: pr19818-2.s
#as: --64 -mrelax-relocations=no
#ld: -pie -m elf_x86_64 --defsym foo=0xffffffff
#error: .*relocation R_X86_64_PC32 against absolute symbol `foo' can not be used when making a shared object
