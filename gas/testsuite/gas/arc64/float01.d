#as: -mcpu=hs6x
#source: float01.s
#objdump: -d --prefix-addresses --show-raw-insn

.*: +file format elf64-.*arc64


Disassembly of section .text:
0x[0-9a-f]+ e310 0062\s+vfhadd	f1,f2,f3
0x[0-9a-f]+ e311 0062\s+vfhsub	f1,f2,f3
0x[0-9a-f]+ e312 0062\s+vfhmul	f1,f2,f3
0x[0-9a-f]+ e313 0062\s+vfhdiv	f1,f2,f3
0x[0-9a-f]+ e314 0062\s+vfhadds	f1,f2,f3
0x[0-9a-f]+ e315 0062\s+vfhsubs	f1,f2,f3
0x[0-9a-f]+ e316 0062\s+vfhmuls	f1,f2,f3
0x[0-9a-f]+ e317 0062\s+vfhdivs	f1,f2,f3
0x[0-9a-f]+ e518 00e4\s+vfhunpkl	f3,f4,f5
0x[0-9a-f]+ e519 00e4\s+vfhunpkm	f3,f4,f5
0x[0-9a-f]+ e51a 00e4\s+vfhpackl	f3,f4,f5
0x[0-9a-f]+ e51b 00e4\s+vfhpackm	f3,f4,f5
0x[0-9a-f]+ e51c 00e4\s+vfhbflyl	f3,f4,f5
0x[0-9a-f]+ e51d 00e4\s+vfhbflym	f3,f4,f5
0x[0-9a-f]+ e51e 00e4\s+vfhaddsub	f3,f4,f5
0x[0-9a-f]+ e51f 00e4\s+vfhsubadd	f3,f4,f5
0x[0-9a-f]+ e010 51a7\s+vfsadd	f6,f7,f8
0x[0-9a-f]+ e011 51a7\s+vfssub	f6,f7,f8
0x[0-9a-f]+ e012 51a7\s+vfsmul	f6,f7,f8
0x[0-9a-f]+ e013 51a7\s+vfsdiv	f6,f7,f8
0x[0-9a-f]+ e014 51a7\s+vfsadds	f6,f7,f8
0x[0-9a-f]+ e015 51a7\s+vfssubs	f6,f7,f8
0x[0-9a-f]+ e016 51a7\s+vfsmuls	f6,f7,f8
0x[0-9a-f]+ e017 51a7\s+vfsdivs	f6,f7,f8
0x[0-9a-f]+ e218 51e9\s+vfsunpkl	f7,f9,f10
0x[0-9a-f]+ e219 51e9\s+vfsunpkm	f7,f9,f10
0x[0-9a-f]+ e21a 51e9\s+vfspackl	f7,f9,f10
0x[0-9a-f]+ e21b 51e9\s+vfspackm	f7,f9,f10
0x[0-9a-f]+ e21c 51e9\s+vfsbflyl	f7,f9,f10
0x[0-9a-f]+ e21d 51e9\s+vfsbflym	f7,f9,f10
0x[0-9a-f]+ e21e 51e9\s+vfsaddsub	f7,f9,f10
0x[0-9a-f]+ e21f 51e9\s+vfssubadd	f7,f9,f10
0x[0-9a-f]+ e610 b2b8\s+vfdadd	f10f11,f24f25,f30f31
0x[0-9a-f]+ e611 b2b8\s+vfdsub	f10f11,f24f25,f30f31
0x[0-9a-f]+ e612 b2b8\s+vfdmul	f10f11,f24f25,f30f31
0x[0-9a-f]+ e613 b2b8\s+vfddiv	f10f11,f24f25,f30f31
0x[0-9a-f]+ e614 b2b8\s+vfdadds	f10f11,f24f25,f30f31
0x[0-9a-f]+ e615 b2b8\s+vfdsubs	f10f11,f24f25,f30f31
0x[0-9a-f]+ e616 b2b8\s+vfdmuls	f10f11,f24f25,f30f31
0x[0-9a-f]+ e617 b2b8\s+vfddivs	f10f11,f24f25,f30f31
0x[0-9a-f]+ e218 a42c\s+vfdunpkl\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e219 a42c\s+vfdunpkm\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21a a42c\s+vfdpackl\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21b a42c\s+vfdpackm\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21c a42c\s+vfdbflyl\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21d a42c\s+vfdbflym\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21e a42c\s+vfdaddsub\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e21f a42c\s+vfdsubadd\s+f16f17,f12f13,f18f19
0x[0-9a-f]+ e040 07ed\s+fhsqrt	f31,f13
0x[0-9a-f]+ e040 47ed\s+fssqrt	f31,f13
0x[0-9a-f]+ e040 87ed\s+fdsqrt	f31,f13
0x[0-9a-f]+ e041 0461\s+vfhsqrt	f17,f1
0x[0-9a-f]+ e041 4461\s+vfssqrt	f17,f1
0x[0-9a-f]+ e041 82bc\s+vfdsqrt	f10f11,f28f29
0x[0-9a-f]+ e042 02ef\s+vfhexch	f11,f15
0x[0-9a-f]+ e042 42ef\s+vfsexch	f11,f15
0x[0-9a-f]+ e042 823a\s+vfdexch	f8f9,f26f27
