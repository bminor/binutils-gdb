# Check 32bit AVX10.2/512 instructions

	.arch generic32
	.arch .avx10.2/512
	.text
_start:
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	%zmm4, %zmm5, %zmm6
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %zmm5, %zmm6{%k7}
	v\m\()pbf16	(%ecx){1to32}, %zmm5, %zmm6
	v\m\()pbf16	8128(%ecx), %zmm5, %zmm6
	v\m\()pbf16	-256(%edx){1to32}, %zmm5, %zmm6{%k7}{z}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	%zmm4, %zmm5, %zmm6
	vf\m\n\(\())nepbf16	0x10000000(%esp, %esi, 8), %zmm5, %zmm6{%k7}
	vf\m\n\(\())nepbf16	(%ecx){1to32}, %zmm5, %zmm6
	vf\m\n\(\())nepbf16	8128(%ecx), %zmm5, %zmm6
	vf\m\n\(\())nepbf16	-256(%edx){1to32}, %zmm5, %zmm6{%k7}{z}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	%zmm5, %zmm6
	v\m\()pbf16	0x10000000(%esp, %esi, 8), %zmm6{%k7}
	v\m\()pbf16	(%ecx){1to32}, %zmm6
	v\m\()pbf16	8128(%ecx), %zmm6
	v\m\()pbf16	-256(%edx){1to32}, %zmm6{%k7}{z}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	$123, %zmm5, %zmm6
	v\m\()pbf16	$123, 0x10000000(%esp, %esi, 8), %zmm6{%k7}
	v\m\()pbf16	$123, (%ecx){1to32}, %zmm6
	v\m\()pbf16	$123, 8128(%ecx), %zmm6
	v\m\()pbf16	$123, -256(%edx){1to32}, %zmm6{%k7}{z}
	.endr

	vcmppbf16	$123, %zmm4, %zmm5, %k5
	vcmppbf16	$123, 0x10000000(%esp, %esi, 8), %zmm5, %k5{%k7}
	vcmppbf16	$123, (%ecx){1to32}, %zmm5, %k5
	vcmppbf16	$123, 8128(%ecx), %zmm5, %k5
	vcmppbf16	$123, -256(%edx){1to32}, %zmm5, %k5{%k7}

	vfpclasspbf16	$123, %zmm5, %k5
	vfpclasspbf16z	$123, 0x10000000(%esp, %esi, 8), %k5{%k7}
	vfpclasspbf16	$123, (%ecx){1to32}, %k5
	vfpclasspbf16z	$123, 8128(%ecx), %k5
	vfpclasspbf16	$123, -256(%edx){1to32}, %k5{%k7}

_intel:
	.intel_syntax noprefix
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	zmm6, zmm5, zmm4
	v\m\()pbf16	zmm6{k7}, zmm5, [esp+esi*8+0x10000000]
	v\m\()pbf16	zmm6, zmm5, [ecx]{1to32}
	v\m\()pbf16	zmm6, zmm5, ZMMWORD PTR [ecx+8128]
	v\m\()pbf16	zmm6{k7}{z}, zmm5, WORD PTR [edx-256]{1to32}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	zmm6, zmm5, zmm4
	vf\m\n\(\())nepbf16	zmm6{k7}, zmm5, ZMMWORD PTR [esp+esi*8+0x10000000]
	vf\m\n\(\())nepbf16	zmm6, zmm5, WORD PTR [ecx]{1to32}
	vf\m\n\(\())nepbf16	zmm6, zmm5, [ecx+8128]
	vf\m\n\(\())nepbf16	zmm6{k7}{z}, zmm5, [edx-256]{1to32}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	zmm6, zmm5
	v\m\()pbf16	zmm6{k7}, [esp+esi*8+0x10000000]
	v\m\()pbf16	zmm6, [ecx]{1to32}
	v\m\()pbf16	zmm6, ZMMWORD PTR [ecx+8128]
	v\m\()pbf16	zmm6{k7}{z}, WORD PTR [edx-256]{1to32}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	zmm6, zmm5, 123
	v\m\()pbf16	zmm6{k7}, ZMMWORD PTR [esp+esi*8+0x10000000], 123
	v\m\()pbf16	zmm6, WORD PTR [ecx]{1to32}, 123
	v\m\()pbf16	zmm6, [ecx+8128], 123
	v\m\()pbf16	zmm6{k7}{z}, [edx-256]{1to32}, 123
	.endr

	vcmppbf16	k5, zmm5, zmm4, 123
	vcmppbf16	k5{k7}, zmm5, [esp+esi*8+0x10000000], 123
	vcmppbf16	k5, zmm5, [ecx]{1to32}, 123
	vcmppbf16	k5, zmm5, ZMMWORD PTR [ecx+8128], 123
	vcmppbf16	k5{k7}, zmm5, WORD PTR [edx-256]{1to32}, 123

	vfpclasspbf16	k5, zmm5, 123
	vfpclasspbf16	k5{k7}, ZMMWORD PTR [esp+esi*8+0x10000000], 123
	vfpclasspbf16	k5, WORD PTR [ecx]{1to32}, 123
	vfpclasspbf16	k5, ZMMWORD PTR [ecx+8128], 123
	vfpclasspbf16	k5{k7}, [edx-256]{1to32}, 123
