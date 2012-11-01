#objdump: -dr --show-raw-insn
#name: microMIPS fixed-size branch delay slots
#as: -mmicromips
#source: micromips-warn-branch-delay.s
#stderr: micromips-warn-branch-delay.l

.*: +file format .*mips.*

Disassembly of section \.text:

[0-9a-f]+ <foo>:
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	0083 1250 	and	v0,v1,a0
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	6043 9000 	swr	v0,0\(v1\)
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	6043 8000 	swl	v0,0\(v1\)
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	0272 8210 	mul	s0,s2,s3
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	001f 8b90 	sltu	s1,ra,zero
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	0220 8910 	add	s1,zero,s1
[ 0-9a-f]+:	45e2      	jalrs	v0
[ 0-9a-f]+:	01b1 8990 	sub	s1,s1,t5
#pass
