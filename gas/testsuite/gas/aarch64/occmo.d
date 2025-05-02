#name: FEAT_OCCMO Test
#as: -march=armv9.5-a+memtag
#objdump: -dr

.*:     file format .*

Disassembly of section .text:

0+ <.*>:

[^:]*:	d50b7b00 	dc	cvaoc, x0
[^:]*:	d50b7b1e 	dc	cvaoc, x30
[^:]*:	d50b7be0 	dc	cgdvaoc, x0
[^:]*:	d50b7bfe 	dc	cgdvaoc, x30
[^:]*:	d50b7f00 	dc	civaoc, x0
[^:]*:	d50b7f1e 	dc	civaoc, x30
[^:]*:	d50b7fe0 	dc	cigdvaoc, x0
[^:]*:	d50b7ffe 	dc	cigdvaoc, x30
