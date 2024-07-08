#name: Test of SVE2.1 TBLQ instruction.
#as: -march=armv9.4-a
#objdump: -dr

[^:]+:     file format .*


[^:]+:

[^:]+:
.*:	4400f800 	tblq	z0.b, {z0.b}, z0.b
.*:	4400f81f 	tblq	z31.b, {z0.b}, z0.b
.*:	44c0f800 	tblq	z0.d, {z0.d}, z0.d
.*:	4400fbe0 	tblq	z0.b, {z31.b}, z0.b
.*:	441ff800 	tblq	z0.b, {z0.b}, z31.b
.*:	44dffbff 	tblq	z31.d, {z31.d}, z31.d
.*:	444ff945 	tblq	z5.h, {z10.h}, z15.h
.*:	4487f861 	tblq	z1.s, {z3.s}, z7.s
