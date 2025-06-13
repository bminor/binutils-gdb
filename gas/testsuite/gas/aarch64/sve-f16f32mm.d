#name: SVE_F16F32MM FMMLA instructions
#as: -march=armv8-a+sve-f16f32mm
#objdump: -dr

.*:     file format .*


Disassembly of section .*:

.* <a>:
.*:	6420e400 	fmmla	z0.s, z0.h, z0.h
.*:	643fe400 	fmmla	z0.s, z0.h, z31.h
.*:	6420e7e0 	fmmla	z0.s, z31.h, z0.h
.*:	643fe7e0 	fmmla	z0.s, z31.h, z31.h
.*:	6420e41f 	fmmla	z31.s, z0.h, z0.h
.*:	643fe41f 	fmmla	z31.s, z0.h, z31.h
.*:	6420e7ff 	fmmla	z31.s, z31.h, z0.h
.*:	643fe7ff 	fmmla	z31.s, z31.h, z31.h
