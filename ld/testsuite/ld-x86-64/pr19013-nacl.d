#source: pr19013.s
#as: --x32
#ld: --oformat elf32-i386-nacl
#objdump: -s -j .rodata
#target: x86_64-*-nacl*

#...
 [0-9a-f]+ 00000203 00414243 4400 +.....ABCD. +
#pass
