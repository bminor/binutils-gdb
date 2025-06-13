#name: F8F16MM SVE2 FMMLA instructions
#as: -march=armv8-a+sve2+f8f16mm
#objdump: -dr

.*:     file format .*


Disassembly of section .*:

.* <a>:
.*:	6460e000 	fmmla	z0.h, z0.b, z0.b
.*:	647fe000 	fmmla	z0.h, z0.b, z31.b
.*:	6460e3e0 	fmmla	z0.h, z31.b, z0.b
.*:	647fe3e0 	fmmla	z0.h, z31.b, z31.b
.*:	6460e01f 	fmmla	z31.h, z0.b, z0.b
.*:	647fe01f 	fmmla	z31.h, z0.b, z31.b
.*:	6460e3ff 	fmmla	z31.h, z31.b, z0.b
.*:	647fe3ff 	fmmla	z31.h, z31.b, z31.b
