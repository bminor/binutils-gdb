#name: Valid v8-a+cryptov1
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> f3b00300 	aese.8	q0, q0
0[0-9a-f]+ <[^>]+> f3b0e30e 	aese.8	q7, q7
0[0-9a-f]+ <[^>]+> f3f00320 	aese.8	q8, q8
0[0-9a-f]+ <[^>]+> f3f0e32e 	aese.8	q15, q15
0[0-9a-f]+ <[^>]+> f3b00340 	aesd.8	q0, q0
0[0-9a-f]+ <[^>]+> f3b0e34e 	aesd.8	q7, q7
0[0-9a-f]+ <[^>]+> f3f00360 	aesd.8	q8, q8
0[0-9a-f]+ <[^>]+> f3f0e36e 	aesd.8	q15, q15
0[0-9a-f]+ <[^>]+> f3b00380 	aesmc.8	q0, q0
0[0-9a-f]+ <[^>]+> f3b0e38e 	aesmc.8	q7, q7
0[0-9a-f]+ <[^>]+> f3f003a0 	aesmc.8	q8, q8
0[0-9a-f]+ <[^>]+> f3f0e3ae 	aesmc.8	q15, q15
0[0-9a-f]+ <[^>]+> f3b003c0 	aesimc.8	q0, q0
0[0-9a-f]+ <[^>]+> f3b0e3ce 	aesimc.8	q7, q7
0[0-9a-f]+ <[^>]+> f3f003e0 	aesimc.8	q8, q8
0[0-9a-f]+ <[^>]+> f3f0e3ee 	aesimc.8	q15, q15
0[0-9a-f]+ <[^>]+> ffb0 0300 	aese.8	q0, q0
0[0-9a-f]+ <[^>]+> ffb0 e30e 	aese.8	q7, q7
0[0-9a-f]+ <[^>]+> fff0 0320 	aese.8	q8, q8
0[0-9a-f]+ <[^>]+> fff0 e32e 	aese.8	q15, q15
0[0-9a-f]+ <[^>]+> ffb0 0340 	aesd.8	q0, q0
0[0-9a-f]+ <[^>]+> ffb0 e34e 	aesd.8	q7, q7
0[0-9a-f]+ <[^>]+> fff0 0360 	aesd.8	q8, q8
0[0-9a-f]+ <[^>]+> fff0 e36e 	aesd.8	q15, q15
0[0-9a-f]+ <[^>]+> ffb0 0380 	aesmc.8	q0, q0
0[0-9a-f]+ <[^>]+> ffb0 e38e 	aesmc.8	q7, q7
0[0-9a-f]+ <[^>]+> fff0 03a0 	aesmc.8	q8, q8
0[0-9a-f]+ <[^>]+> fff0 e3ae 	aesmc.8	q15, q15
0[0-9a-f]+ <[^>]+> ffb0 03c0 	aesimc.8	q0, q0
0[0-9a-f]+ <[^>]+> ffb0 e3ce 	aesimc.8	q7, q7
0[0-9a-f]+ <[^>]+> fff0 03e0 	aesimc.8	q8, q8
0[0-9a-f]+ <[^>]+> fff0 e3ee 	aesimc.8	q15, q15
