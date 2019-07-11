#source: tls32.s
#as: -a32
#ld: -shared
#objdump: -sj.got
#target: powerpc*-*-*

.*

Contents of section \.got:
.* 00000000 00000000 00000000 00000000  .*
.* 00000000 00000000 00000000 00000000  .*
.* 00000000 (000103dc|dc030100) 00000000 00000000  .*
