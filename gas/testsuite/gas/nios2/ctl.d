#objdump: -dr --prefix-addresses
#name: NIOS2 ctl

# Test the ctl instructions

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> rdctl	r8,ctl31
0+0004 <[^>]*> rdctl	r8,ctl30
0+0008 <[^>]*> rdctl	r8,ctl29
0+000c <[^>]*> rdctl	r8,status
0+0010 <[^>]*> rdctl	r8,bstatus
0+0014 <[^>]*> rdctl	r8,estatus
0+0018 <[^>]*> wrctl	ctl31,r8
0+001c <[^>]*> wrctl	ctl30,r8
0+0020 <[^>]*> wrctl	ctl29,r8
0+0024 <[^>]*> wrctl	status,r8
0+0028 <[^>]*> wrctl	bstatus,r8
0+002c <[^>]*> wrctl	estatus,r8
