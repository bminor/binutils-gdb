#objdump: -dr
#as: -march=armv8.2-a

.*:     file .*


Disassembly of section .text:

0000000000000000 <.*>:
   [0-9a-f]+:	d5380725 	mrs	x5, id_aa64mmfr1_el1
   [0-9a-f]+:	d5380747 	mrs	x7, id_aa64mmfr2_el1
   [0-9a-f]+:	d5385305 	mrs	x5, erridr_el1
   [0-9a-f]+:	d5185327 	msr	errselr_el1, x7
  [0-9a-f]+:	d5385327 	mrs	x7, errselr_el1
  [0-9a-f]+:	d5385405 	mrs	x5, erxfr_el1
  [0-9a-f]+:	d5185425 	msr	erxctlr_el1, x5
  [0-9a-f]+:	d5385425 	mrs	x5, erxctlr_el1
  [0-9a-f]+:	d5185445 	msr	erxstatus_el1, x5
  [0-9a-f]+:	d5385445 	mrs	x5, erxstatus_el1
  [0-9a-f]+:	d5185465 	msr	erxaddr_el1, x5
  [0-9a-f]+:	d5385465 	mrs	x5, erxaddr_el1
  [0-9a-f]+:	d5185505 	msr	erxmisc0_el1, x5
  [0-9a-f]+:	d5385505 	mrs	x5, erxmisc0_el1
  [0-9a-f]+:	d5185525 	msr	erxmisc1_el1, x5
  [0-9a-f]+:	d5385525 	mrs	x5, erxmisc1_el1
  [0-9a-f]+:	d53c5265 	mrs	x5, vsesr_el2
  [0-9a-f]+:	d518c125 	msr	disr_el1, x5
  [0-9a-f]+:	d538c125 	mrs	x5, disr_el1
  [0-9a-f]+:	d53cc125 	mrs	x5, vdisr_el2
