#as: -march=armv8.9-a
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
.*:	d53c9a83 	mrs	x3, pmsdsfr_el1
.*:	d51c9a83 	msr	pmsdsfr_el1, x3
