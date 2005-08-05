# name: Mixed 16 and 32-bit Thumb conditional instructions
# as: -march=armv6kt2
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <[^>]+> bf05      	ittet	eq
0+002 <[^>]+> 1880      	add.*	r0, r0, r2
0+004 <[^>]+> 4440      	add.*	r0, r8
0+006 <[^>]+> 1888      	add.*	r0, r1, r2
0+008 <[^>]+> eb11 0002 	adds.*	r0, r1, r2
0+00c <[^>]+> 4410      	add.*	r0, r2
0+00e <[^>]+> 4440      	add.*	r0, r8
0+010 <[^>]+> 1880      	adds.*	r0, r0, r2
0+012 <[^>]+> eb10 0008 	adds.*	r0, r0, r8
0+016 <[^>]+> 1888      	adds.*	r0, r1, r2
0+018 <[^>]+> bf0a      	itet	eq
0+01a <[^>]+> 4310      	orr.*	r0, r2
0+01c <[^>]+> ea40 0008 	orr.*	r0, r0, r8
0+020 <[^>]+> ea50 0002 	orrs.*	r0, r0, r2
0+024 <[^>]+> ea40 0002 	orr.*	r0, r0, r2
0+028 <[^>]+> ea40 0008 	orr.*	r0, r0, r8
0+02c <[^>]+> 4310      	orrs.*	r0, r2
