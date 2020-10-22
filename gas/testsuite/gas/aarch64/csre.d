#name: CSRE System registers
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

0+ <.*>:
[^:]+:	d5338000 	mrs	x0, csrcr_el0
[^:]+:	d5338020 	mrs	x0, csrptr_el0
[^:]+:	d5338040 	mrs	x0, csridr_el0
[^:]+:	d5338060 	mrs	x0, csrptridx_el0
[^:]+:	d5308000 	mrs	x0, csrcr_el1
[^:]+:	d5358000 	mrs	x0, csrcr_el12
[^:]+:	d5308020 	mrs	x0, csrptr_el1
[^:]+:	d5358020 	mrs	x0, csrptr_el12
[^:]+:	d5308060 	mrs	x0, csrptridx_el1
[^:]+:	d5348000 	mrs	x0, csrcr_el2
[^:]+:	d5348020 	mrs	x0, csrptr_el2
[^:]+:	d5348060 	mrs	x0, csrptridx_el2
[^:]+:	d5138000 	msr	csrcr_el0, x0
[^:]+:	d5138020 	msr	csrptr_el0, x0
[^:]+:	d5108000 	msr	csrcr_el1, x0
[^:]+:	d5158000 	msr	csrcr_el12, x0
[^:]+:	d5108020 	msr	csrptr_el1, x0
[^:]+:	d5158020 	msr	csrptr_el12, x0
[^:]+:	d5148000 	msr	csrcr_el2, x0
[^:]+:	d5148020 	msr	csrptr_el2, x0
