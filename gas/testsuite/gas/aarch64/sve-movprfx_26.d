#source: sve-movprfx_26.s
#warning_output: sve-movprfx_26.l
#as: -march=armv8-a+sve -I$srcdir/$subdir
#objdump: -Dr -M notes

.* file format .*

Disassembly of section .*:

0+ <.*>:
[^:]+:	04912420 	movprfx	z0.s, p1/m, z1.s
[^:]+:	65caa440 	fcvt	z0.s, p1/m, z2.d  // note: register size not compatible with previous `movprfx' at operand 1
[^:]+:	04d12420 	movprfx	z0.d, p1/m, z1.d
[^:]+:	65caa440 	fcvt	z0.s, p1/m, z2.d
[^:]+:	04912420 	movprfx	z0.s, p1/m, z1.s
[^:]+:	65cba440 	fcvt	z0.d, p1/m, z2.s  // note: register size not compatible with previous `movprfx' at operand 1
[^:]+:	04d12420 	movprfx	z0.d, p1/m, z1.d
[^:]+:	65cba440 	fcvt	z0.d, p1/m, z2.s
[^:]+:	04112420 	movprfx	z0.b, p1/m, z1.b
[^:]+:	041b8440 	lsl	z0.b, p1/m, z0.b, z2.d
[^:]+:	04d12420 	movprfx	z0.d, p1/m, z1.d
[^:]+:	041b8440 	lsl	z0.b, p1/m, z0.b, z2.d  // note: register size not compatible with previous `movprfx' at operand 1
[^:]+:	d65f03c0 	ret
