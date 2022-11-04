# Checking that our relocations against the symbol __EH_FRAME_BEGIN__ are not
# transformed into relocations against the section symbol .eh_frame.
#as: -march=morello+c64
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

.* <get_eh_frame_begin>:
 *[0-9a-f]+:	........ 	stp	c29, c30, \[csp, #-64\]!
 *[0-9a-f]+:	........ 	adrp	c0, 0 <get_eh_frame_begin>
			4: R_MORELLO_ADR_PREL_PG_HI20	__EH_FRAME_BEGIN__
 *[0-9a-f]+:	........ 	adrp	c0, 0 <get_eh_frame_begin>
			8: R_MORELLO_ADR_PREL_PG_HI20	\.eh_frame
 *[0-9a-f]+:	........ 	add	c0, c0, #0x0
			c: R_AARCH64_ADD_ABS_LO12_NC	__EH_FRAME_BEGIN__
 *[0-9a-f]+:	........ 	add	c0, c0, #0x0
			10: R_AARCH64_ADD_ABS_LO12_NC	\.eh_frame
 *[0-9a-f]+:	........ 	ldp	c29, c30, \[csp\], #64
 *[0-9a-f]+:	........ 	ret	c30

#...
