#source: tls32.s
#as: -a32
#ld: -melf32ppc tmpdir/libtlslib32.so
#objdump: -sj.tdata
#target: powerpc*-*-*

.*: +file format elf32-powerpc

Contents of section \.tdata:
 18102d4 12345678 23456789 3456789a 456789ab  .*
 18102e4 56789abc 6789abcd 789abcde           .*
