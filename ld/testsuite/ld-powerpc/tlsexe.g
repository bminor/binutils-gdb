#source: tls.s
#as: -a64
#ld: -melf64ppc tmpdir/libtlslib.so
#objdump: -sj.got
#target: powerpc64*-*-*

.*: +file format elf64-powerpc

Contents of section \.got:
 10010548 00000000 10018548 ffffffff ffff8018  .*
 10010558 00000000 00000000 00000000 00000000  .*
 10010568 00000000 00000000                    .*
