#source: pr18801.s
#as: --32
#ld: -m elf_i386 -pie
#error: read-only segment has dynamic IFUNC relocations; recompile with -fPIE
