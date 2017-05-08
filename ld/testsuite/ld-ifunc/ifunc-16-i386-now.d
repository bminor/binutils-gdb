#source: ifunc-16-x86.s
#ld: -z now -shared -m elf_i386
#as: --32
#readelf: -r --wide
#target: x86_64-*-* i?86-*-*
#notarget: x86_64-*-nacl* i?86-*-nacl*

Relocation section '.rel.dyn' at .*
[ ]+Offset[ ]+Info[ ]+Type[ ]+.*
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_386_GLOB_DAT[ ]+0+[ ]+ifunc

Relocation section '.rel.plt' at .*
[ ]+Offset[ ]+Info[ ]+Type[ ]+.*
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_386_IRELATIVE[ ]*
