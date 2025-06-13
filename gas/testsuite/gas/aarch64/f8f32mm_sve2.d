#name: F8F32MM SVE2 FMMLA instructions
#as: -march=armv8-a+sve2+f8f32mm
#objdump: -dr

.*:     file format .*


Disassembly of section .*:

.* <a>:
.*:	6420e000 	fmmla	z0.s, z0.b, z0.b
.*:	643fe000 	fmmla	z0.s, z0.b, z31.b
.*:	6420e3e0 	fmmla	z0.s, z31.b, z0.b
.*:	643fe3e0 	fmmla	z0.s, z31.b, z31.b
.*:	6420e01f 	fmmla	z31.s, z0.b, z0.b
.*:	643fe01f 	fmmla	z31.s, z0.b, z31.b
.*:	6420e3ff 	fmmla	z31.s, z31.b, z0.b
.*:	643fe3ff 	fmmla	z31.s, z31.b, z31.b
