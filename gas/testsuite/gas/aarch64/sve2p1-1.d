#name: Test of SVE2.1 min max instructions.
#as: -march=armv9.4-a+sve2p1
#objdump: -dr

[^:]+:     file format .*


[^:]+:

[^:]+:
.*:	04052200 	addqv	v0.16b, p0, z16.b
.*:	04452501 	addqv	v1.8h, p1, z8.h
.*:	04852882 	addqv	v2.4s, p2, z4.s
.*:	04c52c44 	addqv	v4.2d, p3, z2.d
.*:	04c53028 	addqv	v8.2d, p4, z1.d
.*:	04853c10 	addqv	v16.4s, p7, z0.s
.*:	041e2200 	andqv	v0.16b, p0, z16.b
.*:	045e2501 	andqv	v1.8h, p1, z8.h
.*:	049e2882 	andqv	v2.4s, p2, z4.s
.*:	04de2c44 	andqv	v4.2d, p3, z2.d
.*:	04de3028 	andqv	v8.2d, p4, z1.d
.*:	049e3c10 	andqv	v16.4s, p7, z0.s
.*:	040c2200 	smaxqv	v0.16b, p0, z16.b
.*:	044c2501 	smaxqv	v1.8h, p1, z8.h
.*:	048c2882 	smaxqv	v2.4s, p2, z4.s
.*:	04cc2c44 	smaxqv	v4.2d, p3, z2.d
.*:	04cc3028 	smaxqv	v8.2d, p4, z1.d
.*:	048c3c10 	smaxqv	v16.4s, p7, z0.s
.*:	040d2200 	umaxqv	v0.16b, p0, z16.b
.*:	044d2501 	umaxqv	v1.8h, p1, z8.h
.*:	048d2882 	umaxqv	v2.4s, p2, z4.s
.*:	04cd2c44 	umaxqv	v4.2d, p3, z2.d
.*:	04cd3028 	umaxqv	v8.2d, p4, z1.d
.*:	048d3c10 	umaxqv	v16.4s, p7, z0.s
.*:	040e2200 	sminqv	v0.16b, p0, z16.b
.*:	044e2501 	sminqv	v1.8h, p1, z8.h
.*:	048e2882 	sminqv	v2.4s, p2, z4.s
.*:	04ce2c44 	sminqv	v4.2d, p3, z2.d
.*:	04ce3028 	sminqv	v8.2d, p4, z1.d
.*:	048e3c10 	sminqv	v16.4s, p7, z0.s
.*:	040f2200 	uminqv	v0.16b, p0, z16.b
.*:	044f2501 	uminqv	v1.8h, p1, z8.h
.*:	048f2882 	uminqv	v2.4s, p2, z4.s
.*:	04cf2c44 	uminqv	v4.2d, p3, z2.d
.*:	04cf3028 	uminqv	v8.2d, p4, z1.d
.*:	048f3c10 	uminqv	v16.4s, p7, z0.s
.*:	0530268a 	dupq	z10.b, z20.b\[0\]
.*:	053f268a 	dupq	z10.b, z20.b\[15\]
.*:	0521268a 	dupq	z10.h, z20.h\[0\]
.*:	052f268a 	dupq	z10.h, z20.h\[7\]
.*:	0522268a 	dupq	z10.s, z20.s\[0\]
.*:	052e268a 	dupq	z10.s, z20.s\[3\]
.*:	0524268a 	dupq	z10.d, z20.d\[0\]
.*:	052c268a 	dupq	z10.d, z20.d\[1\]
.*:	041d2200 	eorqv	v0.16b, p0, z16.b
.*:	045d2501 	eorqv	v1.8h, p1, z8.h
.*:	049d2882 	eorqv	v2.4s, p2, z4.s
.*:	04dd2c44 	eorqv	v4.2d, p3, z2.d
.*:	04dd3028 	eorqv	v8.2d, p4, z1.d
.*:	049d3c10 	eorqv	v16.4s, p7, z0.s
.*:	056a27c0 	extq	z0.b, z0.b, z10.b\[15\]
.*:	056f25c1 	extq	z1.b, z1.b, z15.b\[7\]
.*:	056524c2 	extq	z2.b, z2.b, z5.b\[3\]
.*:	056c2444 	extq	z4.b, z4.b, z12.b\[1\]
.*:	05672508 	extq	z8.b, z8.b, z7.b\[4\]
.*:	05612610 	extq	z16.b, z16.b, z1.b\[8\]
