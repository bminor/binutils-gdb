#source: tls32.s
#as: -a32
#ld: -melf32ppc tmpdir/libtlslib32.so
#objdump: -sj.got
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Contents of section \.got:
 1810390 4e800021 018102f0 00000000 00000000  .*
 18103a0 00000000 00000000 00000000           .*
