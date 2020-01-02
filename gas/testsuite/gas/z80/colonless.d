#name: colonless labels
#source: colonless.s -colonless
#objdump: -d

.*:[     ]+file format (coff)|(elf32)\-z80


Disassembly of section \.text:

00000000 <start>:
[   ]+0:[ 	]+3e 00[       	]+ld a,0x00
[   ]+2:[ 	]+28 0c[       	]+jr z,0x0010
[   ]+4:[ 	]+3e 01[       	]+ld a,0x01
[   ]+6:[ 	]+3e 02[       	]+ld a,0x02
[   ]+8:[ 	]+3e 03[       	]+ld a,0x03
[   ]+a:[ 	]+18 f4[       	]+jr 0x0000
[   ]+c:[ 	]+18 f4[       	]+jr 0x0002
[   ]+e:[ 	]+18 f6[       	]+jr 0x0006

00000010 <finish>:
[  ]+10:[ 	]+c9[          	]+ret
