.data
obj1:
	.4byte 0
.section .data.1
obj2:
	.4byte 0
.section .data.2
obj3:
	.4byte 0
.section .inspectionsection,"aw"
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
cd_outer_start:
	.capinit __outer_data_start
	.8byte 0
	.8byte 0
cd_start:
	.capinit data_start
	.8byte 0
	.8byte 0
cd1_start:
	.capinit data1_start
	.8byte 0
	.8byte 0
cd1_end:
	.capinit data1_end
	.8byte 0
	.8byte 0
cd_end:
	.capinit __data_end
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
