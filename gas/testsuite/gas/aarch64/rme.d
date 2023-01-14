#name: RME System registers
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:

   0:	d53e60a0 	mrs	x0, mfar_el3
   4:	d53e21c0 	mrs	x0, gpccr_el3
   8:	d53e2180 	mrs	x0, gptbr_el3
   c:	d51e21c0 	msr	gpccr_el3, x0
  10:	d51e2180 	msr	gptbr_el3, x0
