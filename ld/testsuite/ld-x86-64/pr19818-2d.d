#source: pr19818-2.s
#source: pr19818-1b.s
#as: --x32 -mrelax-relocations=no
#ld: -pie -m elf32_x86_64
#error: .*relocation R_X86_64_PC32 against absolute symbol `foo' can not be used when making a shared object
