#name: FEAT_SVE_BFSCALE instruction test
#as: -march=armv8-a+sve-bfscale+sve2
#objdump: -dr

.*:     file format .*


Disassembly of section .*:

.* <bfscale>:
.*:	65098000 	bfscale	z0.h, p0/m, z0.h, z0.h
.*:	650983e0 	bfscale	z0.h, p0/m, z0.h, z31.h
.*:	65099c00 	bfscale	z0.h, p7/m, z0.h, z0.h
.*:	65099fe0 	bfscale	z0.h, p7/m, z0.h, z31.h
.*:	6509801f 	bfscale	z31.h, p0/m, z31.h, z0.h
.*:	650983ff 	bfscale	z31.h, p0/m, z31.h, z31.h
.*:	65099c1f 	bfscale	z31.h, p7/m, z31.h, z0.h
.*:	65099fff 	bfscale	z31.h, p7/m, z31.h, z31.h
