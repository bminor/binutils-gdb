#source: tls.s
#as: -a64
#ld: -melf64ppc tmpdir/libtlslib.so
#objdump: -sj.got
#target: powerpc64*-*-*

.*: +file format elf64-powerpc

Contents of section \.got:
 100105f8 00000000 100185f8 00000000 00000001  .*
 10010608 00000000 00000000 ffffffff ffff8018  .*
 10010618 00000000 00000000 00000000 00000000  .*
