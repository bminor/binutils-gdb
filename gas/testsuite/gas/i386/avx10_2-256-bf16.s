# Check 32bit AVX10.2/256 instructions

	.arch generic32
	.arch .avx10.2/256
	.text
_start:
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	%ymm4, %ymm5, %ymm6
	v\m\()pbf16	%xmm4, %xmm5, %xmm6
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %ymm5, %ymm6{%k7}
	v\m\()pbf16	(%ecx){1to16}, %ymm5, %ymm6
	v\m\()pbf16	4064(%ecx), %ymm5, %ymm6
	v\m\()pbf16	-256(%edx){1to16}, %ymm5, %ymm6{%k7}{z}
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %xmm5, %xmm6{%k7}
	v\m\()pbf16	(%ecx){1to8}, %xmm5, %xmm6
	v\m\()pbf16	2032(%ecx), %xmm5, %xmm6
	v\m\()pbf16	-256(%edx){1to8}, %xmm5, %xmm6{%k7}{z}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	%ymm4, %ymm5, %ymm6
	vf\m\n\(\())nepbf16	%xmm4, %xmm5, %xmm6
	vf\m\n\(\())nepbf16	0x10000000(%esp, %esi, 8), %ymm5, %ymm6{%k7}
	vf\m\n\(\())nepbf16	(%ecx){1to16}, %ymm5, %ymm6
	vf\m\n\(\())nepbf16	4064(%ecx), %ymm5, %ymm6
	vf\m\n\(\())nepbf16	-256(%edx){1to16}, %ymm5, %ymm6{%k7}{z}
	vf\m\n\(\())nepbf16	0x10000000(%esp, %esi, 8), %xmm5, %xmm6{%k7}
	vf\m\n\(\())nepbf16	(%ecx){1to8}, %xmm5, %xmm6
	vf\m\n\(\())nepbf16	2032(%ecx), %xmm5, %xmm6
	vf\m\n\(\())nepbf16	-256(%edx){1to8}, %xmm5, %xmm6{%k7}{z}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	%xmm5, %xmm6
	v\m\()pbf16	%ymm5, %ymm6
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %xmm6{%k7}
	v\m\()pbf16	(%ecx){1to8}, %xmm6
	v\m\()pbf16	2032(%ecx), %xmm6
	v\m\()pbf16	-256(%edx){1to8}, %xmm6{%k7}{z}
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %ymm6{%k7}
	v\m\()pbf16	(%ecx){1to16}, %ymm6
	v\m\()pbf16	4064(%ecx), %ymm6
	v\m\()pbf16	-256(%edx){1to16}, %ymm6{%k7}{z}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	$123, %ymm5, %ymm6
	v\m\()pbf16	$123, %xmm5, %xmm6
	v\m\()pbf16	$123, 0x10000000(%esp, %esi, 8), %xmm6{%k7}
	v\m\()pbf16	$123, (%ecx){1to8}, %xmm6
	v\m\()pbf16	$123, 2032(%ecx), %xmm6
	v\m\()pbf16	$123, -256(%edx){1to8}, %xmm6{%k7}{z}
	v\m\()pbf16	$123, 0x10000000(%esp, %esi, 8), %ymm6{%k7}
	v\m\()pbf16	$123, (%ecx){1to16}, %ymm6
	v\m\()pbf16	$123, 4064(%ecx), %ymm6
	v\m\()pbf16	$123, -256(%edx){1to16}, %ymm6{%k7}{z}
	.endr

	vcmppbf16	$123, %ymm4, %ymm5, %k5
	vcmppbf16	$123, %xmm4, %xmm5, %k5
	vcmppbf16	$123, 0x10000000(%esp, %esi, 8), %xmm5, %k5{%k7}
	vcmppbf16	$123, (%ecx){1to8}, %xmm5, %k5
	vcmppbf16	$123, 2032(%ecx), %xmm5, %k5
	vcmppbf16	$123, -256(%edx){1to8}, %xmm5, %k5{%k7}
	vcmppbf16	$123, 0x10000000(%esp, %esi, 8), %ymm5, %k5{%k7}
	vcmppbf16	$123, (%ecx){1to16}, %ymm5, %k5
	vcmppbf16	$123, 4064(%ecx), %ymm5, %k5
	vcmppbf16	$123, -256(%edx){1to16}, %ymm5, %k5{%k7}

	vcomsbf16	%xmm5, %xmm6
	vcomsbf16	0x10000000(%esp, %esi, 8), %xmm6
	vcomsbf16	(%ecx), %xmm6
	vcomsbf16	254(%ecx), %xmm6
	vcomsbf16	-256(%edx), %xmm6

	vfpclasspbf16	$123, %ymm5, %k5
	vfpclasspbf16	$123, %xmm5, %k5
	vfpclasspbf16x	$123, 0x10000000(%esp, %esi, 8), %k5{%k7}
	vfpclasspbf16	$123, (%ecx){1to8}, %k5
	vfpclasspbf16x	$123, 2032(%ecx), %k5
	vfpclasspbf16	$123, -256(%edx){1to8}, %k5{%k7}
	vfpclasspbf16	$123, (%ecx){1to16}, %k5
	vfpclasspbf16y	$123, 4064(%ecx), %k5
	vfpclasspbf16	$123, -256(%edx){1to16}, %k5{%k7}

_intel:
	.intel_syntax noprefix
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	ymm6, ymm5, ymm4
	v\m\()pbf16	xmm6, xmm5, xmm4
	v\m\()pbf16	ymm6{k7}, ymm5, [esp+esi*8+0x10000000]
	v\m\()pbf16	ymm6, ymm5, [ecx]{1to16}
	v\m\()pbf16	ymm6, ymm5, YMMWORD PTR [ecx+4064]
	v\m\()pbf16	ymm6{k7}{z}, ymm5, WORD PTR [edx-256]{1to16}
	v\m\()pbf16	xmm6{k7}, xmm5, [esp+esi*8+0x10000000]
	v\m\()pbf16	xmm6, xmm5, WORD PTR [ecx]{1to8}
	v\m\()pbf16	xmm6, xmm5, XMMWORD PTR [ecx+2032]
	v\m\()pbf16	xmm6{k7}{z}, xmm5, [edx-256]{1to8}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	ymm6, ymm5, ymm4
	vf\m\n\(\())nepbf16	xmm6, xmm5, xmm4
	vf\m\n\(\())nepbf16	ymm6{k7}, ymm5, YMMWORD PTR [esp+esi*8+0x10000000]
	vf\m\n\(\())nepbf16	ymm6, ymm5, [ecx]{1to16}
	vf\m\n\(\())nepbf16	ymm6, ymm5, [ecx+4064]
	vf\m\n\(\())nepbf16	ymm6{k7}{z}, ymm5, WORD PTR [edx-256]{1to16}
	vf\m\n\(\())nepbf16	xmm6{k7}, xmm5, [esp+esi*8+0x10000000]
	vf\m\n\(\())nepbf16	xmm6, xmm5, WORD PTR [ecx]{1to8}
	vf\m\n\(\())nepbf16	xmm6, xmm5, XMMWORD PTR [ecx+2032]
	vf\m\n\(\())nepbf16	xmm6{k7}{z}, xmm5, [edx-256]{1to8}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	xmm6, xmm5
	v\m\()pbf16	ymm6, ymm5
	v\m\()pbf16	xmm6{k7}, [esp+esi*8+0x10000000]
	v\m\()pbf16	xmm6, [ecx]{1to8}
	v\m\()pbf16	xmm6, XMMWORD PTR [ecx+2032]
	v\m\()pbf16	xmm6{k7}{z}, WORD PTR [edx-256]{1to8}
	v\m\()pbf16	ymm6{k7}, [esp+esi*8+0x10000000]
	v\m\()pbf16	ymm6, WORD PTR [ecx]{1to16}
	v\m\()pbf16	ymm6, YMMWORD PTR [ecx+4064]
	v\m\()pbf16	ymm6{k7}{z}, [edx-256]{1to16}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	ymm6, ymm5, 123
	v\m\()pbf16	xmm6, xmm5, 123
	v\m\()pbf16	xmm6{k7}, XMMWORD PTR [esp+esi*8+0x10000000], 123
	v\m\()pbf16	xmm6, [ecx]{1to8}, 123
	v\m\()pbf16	xmm6, [ecx+2032], 123
	v\m\()pbf16	xmm6{k7}{z}, WORD PTR [edx-256]{1to8}, 123
	v\m\()pbf16	ymm6{k7}, YMMWORD PTR [esp+esi*8+0x10000000], 123
	v\m\()pbf16	ymm6, [ecx]{1to16}, 123
	v\m\()pbf16	ymm6, [ecx+4064], 123
	v\m\()pbf16	ymm6{k7}{z}, WORD PTR [edx-256]{1to16}, 123
	.endr

	vcmppbf16	k5, ymm5, ymm4, 123
	vcmppbf16	k5, xmm5, xmm4, 123
	vcmppbf16	k5{k7}, xmm5, [esp+esi*8+0x10000000], 123
	vcmppbf16	k5, xmm5, WORD PTR [ecx]{1to8}, 123
	vcmppbf16	k5, xmm5, XMMWORD PTR [ecx+2032], 123
	vcmppbf16	k5{k7}, xmm5, [edx-256]{1to8}, 123
	vcmppbf16	k5{k7}, ymm5, YMMWORD PTR [esp+esi*8+0x10000000], 123
	vcmppbf16	k5, ymm5, WORD PTR [ecx]{1to16}, 123
	vcmppbf16	k5, ymm5, [ecx+4064], 123
	vcmppbf16	k5{k7}, ymm5, [edx-256]{1to16}, 123

	vcomsbf16	xmm6, xmm5
	vcomsbf16	xmm6, WORD PTR [esp+esi*8+0x10000000]
	vcomsbf16	xmm6, WORD PTR [ecx]
	vcomsbf16	xmm6, WORD PTR [ecx+254]
	vcomsbf16	xmm6, WORD PTR [edx-256]

	vfpclasspbf16	k5, ymm5, 123
	vfpclasspbf16	k5, xmm5, 123
	vfpclasspbf16	k5{k7}, XMMWORD PTR [esp+esi*8+0x10000000], 123
	vfpclasspbf16	k5, [ecx]{1to8}, 123
	vfpclasspbf16	k5, XMMWORD PTR [ecx+2032], 123
	vfpclasspbf16	k5{k7}, WORD PTR [edx-256]{1to8}, 123
	vfpclasspbf16	k5, [ecx]{1to16}, 123
	vfpclasspbf16	k5, YMMWORD PTR [ecx+4064], 123
	vfpclasspbf16	k5{k7}, WORD PTR [edx-256]{1to16}, 123
