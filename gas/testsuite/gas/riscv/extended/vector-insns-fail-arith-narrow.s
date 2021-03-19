# Vector Narrowing Integer Right Shift Instructions

	# vncvt.x.x.w vd,vs,vm = vnsrl.wx vd,vs,x0,vm
	vncvt.x.x.w v2, v2		# vd overlap vs2
	vncvt.x.x.w v2, v3		# vs2 should be multiple of 2
	vncvt.x.x.w v3, v2		# vd overlap vs2
	vncvt.x.x.w v0, v2, v0.t	# vd overlap vm

	vnsrl.wv v2, v2, v4		# vd overlap vs2
	vnsrl.wv v2, v3, v4		# vs2 should be multiple of 2
	vnsrl.wv v3, v2, v4		# vd overlap vs2
	vnsrl.wv v4, v2, v4		# OK
	vnsrl.wv v0, v2, v4, v0.t	# vd overlap vm
	vnsrl.wx v2, v2, a1		# vd overlap vs2
	vnsrl.wx v2, v3, a1		# vs2 should be multiple of 2
	vnsrl.wx v3, v2, a1		# vd overlap vs2
	vnsrl.wx v0, v2, a1, v0.t	# vd overlap vm
	vnsrl.wi v2, v2, 31		# vd overlap vs2
	vnsrl.wi v2, v3, 31		# vs2 should be multiple of 2
	vnsrl.wi v3, v2, 31		# vd overlap vs2
	vnsrl.wi v0, v2, 31, v0.t	# vd overlap vm

	vnsra.wv v2, v2, v4
	vnsra.wv v2, v3, v4
	vnsra.wv v3, v2, v4
	vnsra.wv v4, v2, v4
	vnsra.wv v0, v2, v4, v0.t
	vnsra.wx v2, v2, a1
	vnsra.wx v2, v3, a1
	vnsra.wx v3, v2, a1
	vnsra.wx v0, v2, a1, v0.t
	vnsra.wi v2, v2, 31
	vnsra.wi v2, v3, 31
	vnsra.wi v3, v2, 31
	vnsra.wi v0, v2, 31, v0.t

# Vector Narrowing Fixed-Point Clip Instructions

	vnclipu.wv v2, v2, v4		# vd overlap vs2
	vnclipu.wv v2, v3, v4		# vs2 should be multiple of 2
	vnclipu.wv v3, v2, v4		# vd overlap vs2
	vnclipu.wv v4, v2, v4		# OK
	vnclipu.wv v0, v2, v4, v0.t	# vd overlap vm
	vnclipu.wx v2, v2, a1		# vd overlap vs2
	vnclipu.wx v2, v3, a1		# vs2 should be multiple of 2
	vnclipu.wx v3, v2, a1		# vd overlap vs2
	vnclipu.wx v0, v2, a1, v0.t	# vd overlap vm
	vnclipu.wi v2, v2, 31		# vd overlap vs2
	vnclipu.wi v2, v3, 31		# vs2 should be multiple of 2
	vnclipu.wi v3, v2, 31		# vd overlap vs2
	vnclipu.wi v0, v2, 31, v0.t	# vd overlap vm

	vnclip.wv v2, v2, v4
	vnclip.wv v2, v3, v4
	vnclip.wv v3, v2, v4
	vnclip.wv v4, v2, v4
	vnclip.wv v0, v2, v4, v0.t
	vnclip.wx v2, v2, a1
	vnclip.wx v2, v3, a1
	vnclip.wx v3, v2, a1
	vnclip.wx v0, v2, a1, v0.t
	vnclip.wi v2, v2, 31
	vnclip.wi v2, v3, 31
	vnclip.wi v3, v2, 31
	vnclip.wi v0, v2, 31, v0.t

# Narrowing Floating-Point/Integer Type-Convert Instructions

	vfncvt.xu.f.w v2, v2		# vd overlap vs2
	vfncvt.xu.f.w v2, v3		# vs2 should be multiple of 2
	vfncvt.xu.f.w v3, v2		# vd overlap vs2
	vfncvt.xu.f.w v0, v2, v0.t	# vd overlap vm
	vfncvt.x.f.w v2, v2
	vfncvt.x.f.w v2, v3
	vfncvt.x.f.w v3, v2
	vfncvt.x.f.w v0, v2, v0.t
	vfncvt.rtz.xu.f.w v2, v2
	vfncvt.rtz.xu.f.w v2, v3
	vfncvt.rtz.xu.f.w v3, v2
	vfncvt.rtz.xu.f.w v0, v2, v0.t
	vfncvt.rtz.x.f.w v2, v2
	vfncvt.rtz.x.f.w v2, v3
	vfncvt.rtz.x.f.w v3, v2
	vfncvt.rtz.x.f.w v0, v2, v0.t
	vfncvt.f.xu.w v2, v2
	vfncvt.f.xu.w v2, v3
	vfncvt.f.xu.w v3, v2
	vfncvt.f.xu.w v0, v2, v0.t
	vfncvt.f.x.w v2, v2
	vfncvt.f.x.w v2, v3
	vfncvt.f.x.w v3, v2
	vfncvt.f.x.w v0, v2, v0.t
	vfncvt.f.f.w v2, v2
	vfncvt.f.f.w v2, v3
	vfncvt.f.f.w v3, v2
	vfncvt.f.f.w v0, v2, v0.t
	vfncvt.rod.f.f.w v2, v2
	vfncvt.rod.f.f.w v2, v3
	vfncvt.rod.f.f.w v3, v2
	vfncvt.rod.f.f.w v0, v2, v0.t
