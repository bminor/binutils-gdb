.data
.align 4
ct_outer_start:
	.capinit __outer_text_start
	.8byte 0
	.8byte 0
ct_start:
	.capinit text_start
	.8byte 0
	.8byte 0
ct1_start:
	.capinit text1_start
	.8byte 0
	.8byte 0
ct1_end:
	.capinit text1_end
	.8byte 0
	.8byte 0
ct_end:
	.capinit __text_end
	.8byte 0
	.8byte 0
cdynamic:
	.capinit _DYNAMIC
	.8byte 0
	.8byte 0
.text
f2:
	ret
.section .text.1
f1:
	ret
.section .text.2
f3:
	ret
