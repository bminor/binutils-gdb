#source: tls32.s
#as: -a32
#ld: -shared -melf32ppc
#objdump: -sj.got
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Contents of section \.got:
 105c4 4e800021 00010524 00000000 00000000  .*
 105d4 00000000 00000000 00000000 00000000  .*
 105e4 00000000 00000000 00000000 00000000  .*
 105f4 00000000                             .*
