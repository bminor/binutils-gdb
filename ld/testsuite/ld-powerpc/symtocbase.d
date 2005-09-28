#source: symtocbase-1.s
#source: symtocbase-2.s
#as: -a64
#ld: -shared -melf64ppc
#objdump: -dj.data
#target: powerpc64*-*-*

.*:     file format elf64-powerpc

Disassembly of section \.data:

.* <i>:
	\.\.\.
.*:	00 02 00 00 	\.long 0x20000
.*:	00 00 00 00 	\.long 0x0
.*:	00 02 00 00 	\.long 0x20000
.*:	00 00 00 00 	\.long 0x0
.*:	00 03 00 00 	\.long 0x30000
.*:	00 00 00 00 	\.long 0x0
.*:	00 03 00 00 	\.long 0x30000
.*:	00 00 00 00 	\.long 0x0
.*:	00 02 00 00 	\.long 0x20000
.*:	00 00 00 00 	\.long 0x0
.*:	00 03 00 00 	\.long 0x30000
