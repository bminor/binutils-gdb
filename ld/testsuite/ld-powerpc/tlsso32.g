#source: tls32.s
#as: -a32
#ld: -shared -melf32ppc
#objdump: -sj.got
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Contents of section \.got:
 10598 4e800021 000104f8 00000000 00000000  .*
 105a8 00000000 00000000 00000000 00000000  .*
 105b8 00000000 00000000 00000000 00000000  .*
 105c8 00000000                             .*
