# Check 32bit AVX10.2 instructions

	.text
_start:
	.irp m, pd, ph, ps
	vcmp\m	$123, {sae}, %ymm4, %ymm5, %k5
	vcmp\m	$123, {sae}, %ymm4, %ymm5, %k5{%k7}
	vgetexp\m	{sae}, %ymm5, %ymm6
	vgetexp\m	{sae}, %ymm5, %ymm6{%k7}
	vgetexp\m	{sae}, %ymm5, %ymm6{%k7}{z}
	vsqrt\m	{rn-sae}, %ymm5, %ymm6
	vsqrt\m	{rd-sae}, %ymm5, %ymm6{%k7}
	vsqrt\m	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp a, add, div, mul, scalef, sub
	.irp m, pd, ph, ps
	v\a\m	{rn-sae}, %ymm4, %ymm5, %ymm6
	v\a\m	{rd-sae}, %ymm4, %ymm5, %ymm6{%k7}
	v\a\m	{rz-sae}, %ymm4, %ymm5, %ymm6{%k7}{z}
	.endr
	.endr

	.irp a, max, min
	.irp m, pd, ph, ps
	v\a\m	{sae}, %ymm4, %ymm5, %ymm6
	v\a\m	{sae}, %ymm4, %ymm5, %ymm6{%k7}
	v\a\m	{sae}, %ymm4, %ymm5, %ymm6{%k7}{z}
	.endr
	.endr

	.irp a, getmant, reduce, rndscale
	.irp m, pd, ph, ps
	v\a\m	$123, {sae}, %ymm5, %ymm6
	v\a\m	$123, {sae}, %ymm5, %ymm6{%k7}
	v\a\m	$123, {sae}, %ymm5, %ymm6{%k7}{z}
	.endr
	.endr

	.irp a, madd, maddsub, msub, msubadd, nmadd, nmsub
	.irp n, 132, 213, 231
	.irp m, pd, ph, ps
	vf\a\n\m	{rn-sae}, %ymm4, %ymm5, %ymm6
	vf\a\n\m	{rd-sae}, %ymm4, %ymm5, %ymm6{%k7}
	vf\a\n\m	{rz-sae}, %ymm4, %ymm5, %ymm6{%k7}{z}
	.endr
	.endr
	.endr

	.irp a, fixupimm, range
	.irp m, pd, ps
	v\a\m	$123, {sae}, %ymm4, %ymm5, %ymm6
	v\a\m	$123, {sae}, %ymm4, %ymm5, %ymm6{%k7}
	v\a\m	$123, {sae}, %ymm4, %ymm5, %ymm6{%k7}{z}
	.endr
	.endr

	.irp a, cmadd, cmul, madd, mul
	vf\a\()cph	{rn-sae}, %ymm4, %ymm5, %ymm6
	vf\a\()cph	{rd-sae}, %ymm4, %ymm5, %ymm6{%k7}
	vf\a\()cph	{rz-sae}, %ymm4, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp n, dq, udq
	vcvt\n\()2ph	{rn-sae}, %ymm5, %xmm6
	vcvt\n\()2ph	{rd-sae}, %ymm5, %xmm6{%k7}
	vcvt\n\()2ph	{rz-sae}, %ymm5, %xmm6{%k7}{z}

	vcvt\n\()2ps	{rn-sae}, %ymm5, %ymm6
	vcvt\n\()2ps	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvt\n\()2ps	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, dq, ph, ps, udq
	vcvtpd2\m	{rn-sae}, %ymm5, %xmm6
	vcvtpd2\m	{rd-sae}, %ymm5, %xmm6{%k7}
	vcvtpd2\m	{rz-sae}, %ymm5, %xmm6{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvtpd2\m	{rn-sae}, %ymm5, %ymm6
	vcvtpd2\m	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvtpd2\m	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvtph2\m	{rn-sae}, %xmm5, %ymm6
	vcvtph2\m	{rd-sae}, %xmm5, %ymm6{%k7}
	vcvtph2\m	{rz-sae}, %xmm5, %ymm6{%k7}{z}
	.endr

	.irp m, pd, ps, psx
	vcvtph2\m	{sae}, %xmm5, %ymm6
	vcvtph2\m	{sae}, %xmm5, %ymm6{%k7}
	vcvtph2\m	{sae}, %xmm5, %ymm6{%k7}{z}
	.endr

	.irp m, uw, w
	vcvtph2\m	{rn-sae}, %ymm5, %ymm6
	vcvtph2\m	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvtph2\m	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, dq, udq
	vcvtps2\m	{rn-sae}, %ymm5, %ymm6
	vcvtps2\m	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvtps2\m	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	vcvtps2pd	{sae}, %xmm5, %ymm6
	vcvtps2pd	{sae}, %xmm5, %ymm6{%k7}
	vcvtps2pd	{sae}, %xmm5, %ymm6{%k7}{z}

	vcvtps2phx	{rn-sae}, %ymm5, %xmm6
	vcvtps2phx	{rd-sae}, %ymm5, %xmm6{%k7}
	vcvtps2phx	{rz-sae}, %ymm5, %xmm6{%k7}{z}

	.irp m, qq, uqq
	vcvtps2\m	{rn-sae}, %xmm5, %ymm6
	vcvtps2\m	{rd-sae}, %xmm5, %ymm6{%k7}
	vcvtps2\m	{rz-sae}, %xmm5, %ymm6{%k7}{z}
	.endr

	.irp n, qq, uqq
	vcvt\n\()2pd	{rn-sae}, %ymm5, %ymm6
	vcvt\n\()2pd	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvt\n\()2pd	{rz-sae}, %ymm5, %ymm6{%k7}{z}

	.irp m, ph, ps
	vcvt\n\()2\m	{rn-sae}, %ymm5, %xmm6
	vcvt\n\()2\m	{rd-sae}, %ymm5, %xmm6{%k7}
	vcvt\n\()2\m	{rz-sae}, %ymm5, %xmm6{%k7}{z}
	.endr
	.endr

	.irp m, dq, udq
	vcvttpd2\m	{sae}, %ymm5, %xmm6
	vcvttpd2\m	{sae}, %ymm5, %xmm6{%k7}
	vcvttpd2\m	{sae}, %ymm5, %xmm6{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvttpd2\m	{sae}, %ymm5, %ymm6
	vcvttpd2\m	{sae}, %ymm5, %ymm6{%k7}
	vcvttpd2\m	{sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvttph2\m	{sae}, %xmm5, %ymm6
	vcvttph2\m	{sae}, %xmm5, %ymm6{%k7}
	vcvttph2\m	{sae}, %xmm5, %ymm6{%k7}{z}
	.endr

	.irp m, uw, w
	vcvttph2\m	{sae}, %ymm5, %ymm6
	vcvttph2\m	{sae}, %ymm5, %ymm6{%k7}
	vcvttph2\m	{sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, dq, udq
	vcvttps2\m	{sae}, %ymm5, %ymm6
	vcvttps2\m	{sae}, %ymm5, %ymm6{%k7}
	vcvttps2\m	{sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvttps2\m	{sae}, %xmm5, %ymm6
	vcvttps2\m	{sae}, %xmm5, %ymm6{%k7}
	vcvttps2\m	{sae}, %xmm5, %ymm6{%k7}{z}
	.endr

	.irp n, uw, w
	vcvt\n\()2ph	{rn-sae}, %ymm5, %ymm6
	vcvt\n\()2ph	{rd-sae}, %ymm5, %ymm6{%k7}
	vcvt\n\()2ph	{rz-sae}, %ymm5, %ymm6{%k7}{z}
	.endr

	.intel_syntax noprefix
	.irp m, pd, ph, ps
	vcmp\m	k5, ymm5, ymm4{sae}, 123
	vcmp\m	k5{k7}, ymm5, ymm4{sae}, 123
	vgetexp\m	ymm6, ymm5{sae}
	vgetexp\m	ymm6{k7}, ymm5{sae}
	vgetexp\m	ymm6{k7}{z}, ymm5{sae}
	vsqrt\m	ymm6, ymm5{rn-sae}
	vsqrt\m	ymm6{k7}, ymm5{rd-sae}
	vsqrt\m	ymm6{k7}{z}, ymm5{rz-sae}
	.endr

	.irp a, add, div, mul, scalef, sub
	.irp m, pd, ph, ps
	v\a\m	ymm6, ymm5, ymm4{rn-sae}
	v\a\m	ymm6{k7}, ymm5, ymm4{rd-sae}
	v\a\m	ymm6{k7}{z}, ymm5, ymm4{rz-sae}
	.endr
	.endr

	.irp a, max, min
	.irp m, pd, ph, ps
	v\a\m	ymm6, ymm5, ymm4{sae}
	v\a\m	ymm6{k7}, ymm5, ymm4{sae}
	v\a\m	ymm6{k7}{z}, ymm5, ymm4{sae}
	.endr
	.endr

	.irp a, getmant, reduce, rndscale
	.irp m, pd, ph, ps
	v\a\m	ymm6, ymm5{sae}, 123
	v\a\m	ymm6{k7}, ymm5{sae}, 123
	v\a\m	ymm6{k7}{z}, ymm5{sae}, 123
	.endr
	.endr

	.irp a, madd, maddsub, msub, msubadd, nmadd, nmsub
	.irp n, 132, 213, 231
	.irp m, pd, ph, ps
	vf\a\n\m	ymm6, ymm5, ymm4{rn-sae}
	vf\a\n\m	ymm6{k7}, ymm5, ymm4{rd-sae}
	vf\a\n\m	ymm6{k7}{z}, ymm5, ymm4{rz-sae}
	.endr
	.endr
	.endr

	.irp a, fixupimm, range
	.irp m, pd, ps
	v\a\m	ymm6, ymm5, ymm4{sae}, 123
	v\a\m	ymm6{k7}, ymm5, ymm4{sae}, 123
	v\a\m	ymm6{k7}{z}, ymm5, ymm4{sae}, 123
	.endr
	.endr

	.irp a, cmadd, cmul, madd, mul
	vf\a\()cph	ymm6, ymm5, ymm4{rn-sae}
	vf\a\()cph	ymm6{k7}, ymm5, ymm4{rd-sae}
	vf\a\()cph	ymm6{k7}{z}, ymm5, ymm4{rz-sae}
	.endr

	.irp n, dq, udq
	vcvt\n\()2ph	xmm6, ymm5{rn-sae}
	vcvt\n\()2ph	xmm6{k7}, ymm5{rd-sae}
	vcvt\n\()2ph	xmm6{k7}{z}, ymm5{rz-sae}

	vcvt\n\()2ps	ymm6, ymm5{rn-sae}
	vcvt\n\()2ps	ymm6{k7}, ymm5{rd-sae}
	vcvt\n\()2ps	ymm6{k7}{z}, ymm5{rz-sae}
	.endr

	.irp m, dq, ph, ps, udq
	vcvtpd2\m	xmm6, ymm5{rn-sae}
	vcvtpd2\m	xmm6{k7}, ymm5{rd-sae}
	vcvtpd2\m	xmm6{k7}{z}, ymm5{rz-sae}
	.endr

	.irp m, qq, uqq
	vcvtpd2\m	ymm6, ymm5{rn-sae}
	vcvtpd2\m	ymm6{k7}, ymm5{rd-sae}
	vcvtpd2\m	ymm6{k7}{z}, ymm5{rz-sae}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvtph2\m	ymm6, xmm5{rn-sae}
	vcvtph2\m	ymm6{k7}, xmm5{rd-sae}
	vcvtph2\m	ymm6{k7}{z}, xmm5{rz-sae}
	.endr

	.irp m, pd, ps, psx
	vcvtph2\m	ymm6, xmm5{sae}
	vcvtph2\m	ymm6{k7}, xmm5{sae}
	vcvtph2\m	ymm6{k7}{z}, xmm5{sae}
	.endr

	.irp m, uw, w
	vcvtph2\m	ymm6, ymm5{rn-sae}
	vcvtph2\m	ymm6{k7}, ymm5{rd-sae}
	vcvtph2\m	ymm6{k7}{z}, ymm5{rz-sae}
	.endr

	.irp m, dq, udq
	vcvtps2\m	ymm6, ymm5{rn-sae}
	vcvtps2\m	ymm6{k7}, ymm5{rd-sae}
	vcvtps2\m	ymm6{k7}{z}, ymm5{rz-sae}
	.endr

	vcvtps2pd	ymm6, xmm5{sae}
	vcvtps2pd	ymm6{k7}, xmm5{sae}
	vcvtps2pd	ymm6{k7}{z}, xmm5{sae}

	vcvtps2phx	xmm6, ymm5{rn-sae}
	vcvtps2phx	xmm6{k7}, ymm5{rd-sae}
	vcvtps2phx	xmm6{k7}{z}, ymm5{rz-sae}

	.irp m, qq, uqq
	vcvtps2\m	ymm6, xmm5{rn-sae}
	vcvtps2\m	ymm6{k7}, xmm5{rd-sae}
	vcvtps2\m	ymm6{k7}{z}, xmm5{rz-sae}
	.endr

	.irp n, qq, uqq
	vcvt\n\()2pd	ymm6, ymm5{rn-sae}
	vcvt\n\()2pd	ymm6{k7}, ymm5{rd-sae}
	vcvt\n\()2pd	ymm6{k7}{z}, ymm5{rz-sae}

	.irp m, ph, ps
	vcvt\n\()2\m	xmm6, ymm5{rn-sae}
	vcvt\n\()2\m	xmm6{k7}, ymm5{rd-sae}
	vcvt\n\()2\m	xmm6{k7}{z}, ymm5{rz-sae}
	.endr
	.endr

	.irp m, dq, udq
	vcvttpd2\m	xmm6, ymm5{sae}
	vcvttpd2\m	xmm6{k7}, ymm5{sae}
	vcvttpd2\m	xmm6{k7}{z}, ymm5{sae}
	.endr

	.irp m, qq, uqq
	vcvttpd2\m	ymm6, ymm5{sae}
	vcvttpd2\m	ymm6{k7}, ymm5{sae}
	vcvttpd2\m	ymm6{k7}{z}, ymm5{sae}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvttph2\m	ymm6, xmm5{sae}
	vcvttph2\m	ymm6{k7}, xmm5{sae}
	vcvttph2\m	ymm6{k7}{z}, xmm5{sae}
	.endr

	.irp m, uw, w
	vcvttph2\m	ymm6, ymm5{sae}
	vcvttph2\m	ymm6{k7}, ymm5{sae}
	vcvttph2\m	ymm6{k7}{z}, ymm5{sae}
	.endr

	.irp m, dq, udq
	vcvttps2\m	ymm6, ymm5{sae}
	vcvttps2\m	ymm6{k7}, ymm5{sae}
	vcvttps2\m	ymm6{k7}{z}, ymm5{sae}
	.endr

	.irp m, qq, uqq
	vcvttps2\m	ymm6, xmm5{sae}
	vcvttps2\m	ymm6{k7}, xmm5{sae}
	vcvttps2\m	ymm6{k7}{z}, xmm5{sae}
	.endr

	.irp n, uw, w
	vcvt\n\()2ph	ymm6, ymm5{rn-sae}
	vcvt\n\()2ph	ymm6{k7}, ymm5{rd-sae}
	vcvt\n\()2ph	ymm6{k7}{z}, ymm5{rz-sae}
	.endr
