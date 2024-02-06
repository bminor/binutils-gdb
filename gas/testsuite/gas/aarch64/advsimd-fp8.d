#as: -march=armv8.5-a+fp8
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
[ ]*[0-9a-f]+:	2ea17800 	bf1cvtl	v0.8h, v0.8b
[ ]*[0-9a-f]+:	2ea17801 	bf1cvtl	v1.8h, v0.8b
[ ]*[0-9a-f]+:	2ea17820 	bf1cvtl	v0.8h, v1.8b
[ ]*[0-9a-f]+:	2ea17821 	bf1cvtl	v1.8h, v1.8b
[ ]*[0-9a-f]+:	2ea17a30 	bf1cvtl	v16.8h, v17.8b
[ ]*[0-9a-f]+:	2ee17800 	bf2cvtl	v0.8h, v0.8b
[ ]*[0-9a-f]+:	2ee17801 	bf2cvtl	v1.8h, v0.8b
[ ]*[0-9a-f]+:	2ee17820 	bf2cvtl	v0.8h, v1.8b
[ ]*[0-9a-f]+:	2ee17821 	bf2cvtl	v1.8h, v1.8b
[ ]*[0-9a-f]+:	2ee17a30 	bf2cvtl	v16.8h, v17.8b
[ ]*[0-9a-f]+:	2e217800 	f1cvtl	v0.8h, v0.8b
[ ]*[0-9a-f]+:	2e217801 	f1cvtl	v1.8h, v0.8b
[ ]*[0-9a-f]+:	2e217820 	f1cvtl	v0.8h, v1.8b
[ ]*[0-9a-f]+:	2e217821 	f1cvtl	v1.8h, v1.8b
[ ]*[0-9a-f]+:	2e217a30 	f1cvtl	v16.8h, v17.8b
[ ]*[0-9a-f]+:	2e617800 	f2cvtl	v0.8h, v0.8b
[ ]*[0-9a-f]+:	2e617801 	f2cvtl	v1.8h, v0.8b
[ ]*[0-9a-f]+:	2e617820 	f2cvtl	v0.8h, v1.8b
[ ]*[0-9a-f]+:	2e617821 	f2cvtl	v1.8h, v1.8b
[ ]*[0-9a-f]+:	2e617a30 	f2cvtl	v16.8h, v17.8b
[ ]*[0-9a-f]+:	6ea17800 	bf1cvtl2	v0.8h, v0.16b
[ ]*[0-9a-f]+:	6ea17801 	bf1cvtl2	v1.8h, v0.16b
[ ]*[0-9a-f]+:	6ea17820 	bf1cvtl2	v0.8h, v1.16b
[ ]*[0-9a-f]+:	6ea17821 	bf1cvtl2	v1.8h, v1.16b
[ ]*[0-9a-f]+:	6ea17a30 	bf1cvtl2	v16.8h, v17.16b
[ ]*[0-9a-f]+:	6ee17800 	bf2cvtl2	v0.8h, v0.16b
[ ]*[0-9a-f]+:	6ee17801 	bf2cvtl2	v1.8h, v0.16b
[ ]*[0-9a-f]+:	6ee17820 	bf2cvtl2	v0.8h, v1.16b
[ ]*[0-9a-f]+:	6ee17821 	bf2cvtl2	v1.8h, v1.16b
[ ]*[0-9a-f]+:	6ee17a30 	bf2cvtl2	v16.8h, v17.16b
[ ]*[0-9a-f]+:	6e217800 	f1cvtl2	v0.8h, v0.16b
[ ]*[0-9a-f]+:	6e217801 	f1cvtl2	v1.8h, v0.16b
[ ]*[0-9a-f]+:	6e217820 	f1cvtl2	v0.8h, v1.16b
[ ]*[0-9a-f]+:	6e217821 	f1cvtl2	v1.8h, v1.16b
[ ]*[0-9a-f]+:	6e217a30 	f1cvtl2	v16.8h, v17.16b
[ ]*[0-9a-f]+:	6e617800 	f2cvtl2	v0.8h, v0.16b
[ ]*[0-9a-f]+:	6e617801 	f2cvtl2	v1.8h, v0.16b
[ ]*[0-9a-f]+:	6e617820 	f2cvtl2	v0.8h, v1.16b
[ ]*[0-9a-f]+:	6e617821 	f2cvtl2	v1.8h, v1.16b
[ ]*[0-9a-f]+:	6e617a30 	f2cvtl2	v16.8h, v17.16b
[ ]*[0-9a-f]+:	2ec03c00 	fscale	v0.4h, v0.4h, v0.4h
[ ]*[0-9a-f]+:	2ec03c01 	fscale	v1.4h, v0.4h, v0.4h
[ ]*[0-9a-f]+:	2ec03c20 	fscale	v0.4h, v1.4h, v0.4h
[ ]*[0-9a-f]+:	2ec13c00 	fscale	v0.4h, v0.4h, v1.4h
[ ]*[0-9a-f]+:	2ec03c21 	fscale	v1.4h, v1.4h, v0.4h
[ ]*[0-9a-f]+:	2ec13c20 	fscale	v0.4h, v1.4h, v1.4h
[ ]*[0-9a-f]+:	2ec13c21 	fscale	v1.4h, v1.4h, v1.4h
[ ]*[0-9a-f]+:	2ed23e30 	fscale	v16.4h, v17.4h, v18.4h
[ ]*[0-9a-f]+:	6ec03c00 	fscale	v0.8h, v0.8h, v0.8h
[ ]*[0-9a-f]+:	6ec03c01 	fscale	v1.8h, v0.8h, v0.8h
[ ]*[0-9a-f]+:	6ec03c20 	fscale	v0.8h, v1.8h, v0.8h
[ ]*[0-9a-f]+:	6ec13c00 	fscale	v0.8h, v0.8h, v1.8h
[ ]*[0-9a-f]+:	6ec03c21 	fscale	v1.8h, v1.8h, v0.8h
[ ]*[0-9a-f]+:	6ec13c20 	fscale	v0.8h, v1.8h, v1.8h
[ ]*[0-9a-f]+:	6ec13c21 	fscale	v1.8h, v1.8h, v1.8h
[ ]*[0-9a-f]+:	6ed23e30 	fscale	v16.8h, v17.8h, v18.8h
[ ]*[0-9a-f]+:	2ea0fc00 	fscale	v0.2s, v0.2s, v0.2s
[ ]*[0-9a-f]+:	2ea0fc01 	fscale	v1.2s, v0.2s, v0.2s
[ ]*[0-9a-f]+:	2ea0fc20 	fscale	v0.2s, v1.2s, v0.2s
[ ]*[0-9a-f]+:	2ea1fc00 	fscale	v0.2s, v0.2s, v1.2s
[ ]*[0-9a-f]+:	2ea0fc21 	fscale	v1.2s, v1.2s, v0.2s
[ ]*[0-9a-f]+:	2ea1fc20 	fscale	v0.2s, v1.2s, v1.2s
[ ]*[0-9a-f]+:	2ea1fc21 	fscale	v1.2s, v1.2s, v1.2s
[ ]*[0-9a-f]+:	2eb2fe30 	fscale	v16.2s, v17.2s, v18.2s
[ ]*[0-9a-f]+:	6ea0fc00 	fscale	v0.4s, v0.4s, v0.4s
[ ]*[0-9a-f]+:	6ea0fc01 	fscale	v1.4s, v0.4s, v0.4s
[ ]*[0-9a-f]+:	6ea0fc20 	fscale	v0.4s, v1.4s, v0.4s
[ ]*[0-9a-f]+:	6ea1fc00 	fscale	v0.4s, v0.4s, v1.4s
[ ]*[0-9a-f]+:	6ea0fc21 	fscale	v1.4s, v1.4s, v0.4s
[ ]*[0-9a-f]+:	6ea1fc20 	fscale	v0.4s, v1.4s, v1.4s
[ ]*[0-9a-f]+:	6ea1fc21 	fscale	v1.4s, v1.4s, v1.4s
[ ]*[0-9a-f]+:	6eb2fe30 	fscale	v16.4s, v17.4s, v18.4s
[ ]*[0-9a-f]+:	6ee0fc00 	fscale	v0.2d, v0.2d, v0.2d
[ ]*[0-9a-f]+:	6ee0fc01 	fscale	v1.2d, v0.2d, v0.2d
[ ]*[0-9a-f]+:	6ee0fc20 	fscale	v0.2d, v1.2d, v0.2d
[ ]*[0-9a-f]+:	6ee1fc00 	fscale	v0.2d, v0.2d, v1.2d
[ ]*[0-9a-f]+:	6ee0fc21 	fscale	v1.2d, v1.2d, v0.2d
[ ]*[0-9a-f]+:	6ee1fc20 	fscale	v0.2d, v1.2d, v1.2d
[ ]*[0-9a-f]+:	6ee1fc21 	fscale	v1.2d, v1.2d, v1.2d
[ ]*[0-9a-f]+:	6ef2fe30 	fscale	v16.2d, v17.2d, v18.2d
[ ]*[0-9a-f]+:	0e40f400 	fcvtn	v0.8b, v0.4h, v0.4h
[ ]*[0-9a-f]+:	0e40f401 	fcvtn	v1.8b, v0.4h, v0.4h
[ ]*[0-9a-f]+:	0e40f420 	fcvtn	v0.8b, v1.4h, v0.4h
[ ]*[0-9a-f]+:	0e41f400 	fcvtn	v0.8b, v0.4h, v1.4h
[ ]*[0-9a-f]+:	0e40f421 	fcvtn	v1.8b, v1.4h, v0.4h
[ ]*[0-9a-f]+:	0e41f420 	fcvtn	v0.8b, v1.4h, v1.4h
[ ]*[0-9a-f]+:	0e41f421 	fcvtn	v1.8b, v1.4h, v1.4h
[ ]*[0-9a-f]+:	0e52f630 	fcvtn	v16.8b, v17.4h, v18.4h
[ ]*[0-9a-f]+:	4e40f400 	fcvtn	v0.16b, v0.8h, v0.8h
[ ]*[0-9a-f]+:	4e40f401 	fcvtn	v1.16b, v0.8h, v0.8h
[ ]*[0-9a-f]+:	4e40f420 	fcvtn	v0.16b, v1.8h, v0.8h
[ ]*[0-9a-f]+:	4e41f400 	fcvtn	v0.16b, v0.8h, v1.8h
[ ]*[0-9a-f]+:	4e40f421 	fcvtn	v1.16b, v1.8h, v0.8h
[ ]*[0-9a-f]+:	4e41f420 	fcvtn	v0.16b, v1.8h, v1.8h
[ ]*[0-9a-f]+:	4e41f421 	fcvtn	v1.16b, v1.8h, v1.8h
[ ]*[0-9a-f]+:	4e52f630 	fcvtn	v16.16b, v17.8h, v18.8h
[ ]*[0-9a-f]+:	0e00f400 	fcvtn	v0.8b, v0.4s, v0.4s
[ ]*[0-9a-f]+:	0e00f401 	fcvtn	v1.8b, v0.4s, v0.4s
[ ]*[0-9a-f]+:	0e00f420 	fcvtn	v0.8b, v1.4s, v0.4s
[ ]*[0-9a-f]+:	0e01f400 	fcvtn	v0.8b, v0.4s, v1.4s
[ ]*[0-9a-f]+:	0e00f421 	fcvtn	v1.8b, v1.4s, v0.4s
[ ]*[0-9a-f]+:	0e01f420 	fcvtn	v0.8b, v1.4s, v1.4s
[ ]*[0-9a-f]+:	0e01f421 	fcvtn	v1.8b, v1.4s, v1.4s
[ ]*[0-9a-f]+:	0e12f630 	fcvtn	v16.8b, v17.4s, v18.4s
[ ]*[0-9a-f]+:	4e00f400 	fcvtn2	v0.16b, v0.4s, v0.4s
[ ]*[0-9a-f]+:	4e00f401 	fcvtn2	v1.16b, v0.4s, v0.4s
[ ]*[0-9a-f]+:	4e00f420 	fcvtn2	v0.16b, v1.4s, v0.4s
[ ]*[0-9a-f]+:	4e01f400 	fcvtn2	v0.16b, v0.4s, v1.4s
[ ]*[0-9a-f]+:	4e00f421 	fcvtn2	v1.16b, v1.4s, v0.4s
[ ]*[0-9a-f]+:	4e01f420 	fcvtn2	v0.16b, v1.4s, v1.4s
[ ]*[0-9a-f]+:	4e01f421 	fcvtn2	v1.16b, v1.4s, v1.4s
[ ]*[0-9a-f]+:	4e12f630 	fcvtn2	v16.16b, v17.4s, v18.4s
