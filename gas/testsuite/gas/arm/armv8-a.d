#name: Valid v8-a
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> e320f005 	sevl
0[0-9a-f]+ <[^>]+> e1000070 	hlt	0x0000
0[0-9a-f]+ <[^>]+> e100007f 	hlt	0x000f
0[0-9a-f]+ <[^>]+> e10fff70 	hlt	0xfff0
0[0-9a-f]+ <[^>]+> bf50      	sevl
0[0-9a-f]+ <[^>]+> bf50      	sevl
0[0-9a-f]+ <[^>]+> f3af 8005 	sevl.w
0[0-9a-f]+ <[^>]+> f78f 8001 	dcps1
0[0-9a-f]+ <[^>]+> f78f 8002 	dcps2
0[0-9a-f]+ <[^>]+> f78f 8003 	dcps3
0[0-9a-f]+ <[^>]+> ba80      	hlt	0x0000
0[0-9a-f]+ <[^>]+> babf      	hlt	0x003f
