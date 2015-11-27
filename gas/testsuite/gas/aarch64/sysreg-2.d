#objdump: -dr
#as: -march=armv8.2-a

.*:     file .*


Disassembly of section .text:

0000000000000000 <.*>:
   [0-9a-f]+:	d5380725 	mrs	x5, id_aa64mmfr1_el1
   [0-9a-f]+:	d5380747 	mrs	x7, id_aa64mmfr2_el1
