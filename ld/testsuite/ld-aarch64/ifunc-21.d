#source: ifunc-21.s
#ld: -shared -z nocombreloc
#objdump: -d -s -j .got.plt -j .text
#target: aarch64*-*-*

# Ensure the .got.plt slot used is correct

.*:     file format elf64-(little|big)aarch64

Contents of section .text:
 02a0 .*
Contents of section .got.plt:
 103a8 0+ 0+ 0+ 0+  .*
 103b8 0+ 0+ [0-9a-f]+ 0+  .*

Disassembly of section .text:

0+2a0 <ifunc>:
 2a0:	d65f03c0 	ret

0+2a4 <bar>:
 2a4:	90000080 	adrp	x0, 10000 <.*>
 2a8:	f941e000 	ldr	x0, \[x0,#960\]
 2ac:	d65f03c0 	ret

Disassembly of section .got.plt:

.*:
.*
.*
.*
