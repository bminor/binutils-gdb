#as: -mcpu=hs6x
#source: stdl.s
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format elf64-.*arc64

Disassembly of section .text:
0x[0-9a-f]+ 1900 0126           	stdl	r4r5,\[r1,0\]
0x[0-9a-f]+ 1c00 70a6 0000 1338 	stdl	r2r3,\[4920@s32\]
0x[0-9a-f]+ 1c00 71a6 7fff ffff 	stdl	r6r7,\[2147483647@s32\]
0x[0-9a-f]+ 1e00 70a6 8000 0000 	stdl	r2r3,\[0x80000000\]
0x[0-9a-f]+ 1c00 7026 ffff fb2e 	stdl	r0r1,\[-1234@s32\]
0x[0-9a-f]+ 1b00 0fa6 8000 0000 	stdl	0x80000000,\[r3,0\]
0x[0-9a-f]+ 1b00 0f26 7fff ffff 	stdl	2147483647@s32,\[r3,0\]
0x[0-9a-f]+ 19ff 0026           	stdl	r0r1,\[r1,255\]
0x[0-9a-f]+ 1c00 80a6           	stdl	r2r3,\[r4,-256\]
0x[0-9a-f]+ 19ff 0f26 ffff 5bf0 	stdl	-42000@s32,\[r1,255\]
0x[0-9a-f]+ 1a00 8fa6 8234 5678 	stdl	0x82345678,\[r2,-256\]
0x[0-9a-f]+ 19ff 002e           	stdl.aw	r0r1,\[r1,255\]
0x[0-9a-f]+ 1c00 812e           	stdl.aw	r4r5,\[r4,-256\]
0x[0-9a-f]+ 1a00 8fae 8234 5678 	stdl.aw	0x82345678,\[r2,-256\]
0x[0-9a-f]+ 19ff 0f2e ffff 5bf0 	stdl.aw	-42000@s32,\[r1,255\]
0x[0-9a-f]+ 19ff 002e           	stdl.aw	r0r1,\[r1,255\]
0x[0-9a-f]+ 1c00 80ae           	stdl.aw	r2r3,\[r4,-256\]
0x[0-9a-f]+ 1a00 8fae 8234 5678 	stdl.aw	0x82345678,\[r2,-256\]
0x[0-9a-f]+ 19ff 0f2e ffff 5bf0 	stdl.aw	-42000@s32,\[r1,255\]
0x[0-9a-f]+ 19ff 0036           	stdl.ab	r0r1,\[r1,255\]
0x[0-9a-f]+ 1c00 81b6           	stdl.ab	r6r7,\[r4,-256\]
0x[0-9a-f]+ 1a00 8fb6 8234 5678 	stdl.ab	0x82345678,\[r2,-256\]
0x[0-9a-f]+ 19ff 0f36 ffff 5bf0 	stdl.ab	-42000@s32,\[r1,255\]
0x[0-9a-f]+ 19ff 003e           	stdl.as	r0r1,\[r1,255\]
0x[0-9a-f]+ 1c00 813e           	stdl.as	r4r5,\[r4,-256\]
0x[0-9a-f]+ 1a00 8fbe 8234 5678 	stdl.as	0x82345678,\[r2,-256\]
0x[0-9a-f]+ 19ff 0f3e ffff 5bf0 	stdl.as	-42000@s32,\[r1,255\]
0x[0-9a-f]+ 1a00 8fbe 8234 5678 	stdl.as	0x82345678,\[r2,-256\]
