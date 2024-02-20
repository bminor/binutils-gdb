#source: tls.s
#ld: -no-pie tmpdir/tlslib.so
#readelf: -Wr

Relocation section '.rela.plt' at offset 0x[0-9a-f]+ contains 1 entry:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
[0-9a-f]+ +[0-9a-f]+ R_RISCV_JUMP_SLOT +[0-9a-f]+ __tls_get_addr \+ 0
