#source: ifunc-16-x86.s
#as: --64
#ld: -z now -shared -melf_x86_64
#readelf: -r --wide
#target: x86_64-*-*
#notarget: x86_64-*-nacl*

Relocation section '.rela.dyn' at .*
[ ]+Offset[ ]+Info[ ]+Type[ ]+.*
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_X86_64_GLOB_DAT[ ]+0+[ ]+ifunc \+ 0

Relocation section '.rela.plt' at .*
[ ]+Offset[ ]+Info[ ]+Type[ ]+.*
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_X86_64_IRELATIVE[ ]+[0-9a-f]*
