# Vector Widening Integer Add/Subtract

	# vwcvtu.x.x.v vd,vs,vm = vwaddu.vx vd,vs,x0,vm
	vwcvtu.x.x.v v1, v2		# vd should be multiple of 2
	vwcvtu.x.x.v v2, v2		# vd overlap vs2
	vwcvtu.x.x.v v2, v3		# vd overlap vs2
	vwcvtu.x.x.v v0, v2, v0.t	# vd overlap vm

	# vwcvt.x.x.v vd,vs,vm = vwadd.vx vd,vs,x0,vm
	vwcvt.x.x.v v1, v2
	vwcvt.x.x.v v2, v2
	vwcvt.x.x.v v2, v3
	vwcvt.x.x.v v0, v2, v0.t

	vwaddu.vv v1, v2, v4		# vd should be multiple of 2
	vwaddu.vv v2, v2, v4		# vd overlap vs2
	vwaddu.vv v2, v3, v4		# vd overlap vs2
	vwaddu.vv v4, v2, v4		# vd overlap vs1
	vwaddu.vv v4, v2, v5		# vd overlap vs1
	vwaddu.vv v0, v2, v4, v0.t	# vd overlap vm
	vwaddu.vx v1, v2, a1		# vd should be multiple of 2
	vwaddu.vx v2, v2, a1		# vd overlap vs2
	vwaddu.vx v2, v3, a1		# vd overlap vs2
	vwaddu.vx v0, v2, a1, v0.t	# vd overlap vm
	vwaddu.wv v1, v2, v4		# vd should be multiple of 2
	vwaddu.wv v2, v2, v4		# OK
	vwaddu.wv v2, v3, v4		# vs2 should be multiple of 2
	vwaddu.wv v4, v2, v4		# vd overlap vs1
	vwaddu.wv v4, v2, v5		# vd overlap vs1
	vwaddu.wv v0, v2, v4, v0.t	# vd overlap vm
	vwaddu.wx v1, v2, a1		# vd should be multiple of 2
	vwaddu.wx v2, v2, a1		# OK
	vwaddu.wx v2, v3, a1		# vs2 should be multiple of 2
	vwaddu.wx v0, v2, a1, v0.t	# vd overlap vm

	vwsubu.vv v1, v2, v4
	vwsubu.vv v2, v2, v4
	vwsubu.vv v2, v3, v4
	vwsubu.vv v4, v2, v4
	vwsubu.vv v4, v2, v5
	vwsubu.vv v0, v2, v4, v0.t
	vwsubu.vx v1, v2, a1
	vwsubu.vx v2, v2, a1
	vwsubu.vx v2, v3, a1
	vwsubu.vx v0, v2, a1, v0.t
	vwsubu.wv v1, v2, v4
	vwsubu.wv v2, v2, v4
	vwsubu.wv v2, v3, v4
	vwsubu.wv v4, v2, v4
	vwsubu.wv v4, v2, v5
	vwsubu.wv v0, v2, v4, v0.t
	vwsubu.wx v1, v2, a1
	vwsubu.wx v2, v2, a1
	vwsubu.wx v2, v3, a1
	vwsubu.wx v0, v2, a1, v0.t

	vwadd.vv v1, v2, v4
	vwadd.vv v2, v2, v4
	vwadd.vv v2, v3, v4
	vwadd.vv v4, v2, v4
	vwadd.vv v4, v2, v5
	vwadd.vv v0, v2, v4, v0.t
	vwadd.vx v1, v2, a1
	vwadd.vx v2, v2, a1
	vwadd.vx v2, v3, a1
	vwadd.vx v0, v2, a1, v0.t
	vwadd.wv v1, v2, v4
	vwadd.wv v2, v2, v4
	vwadd.wv v2, v3, v4
	vwadd.wv v4, v2, v4
	vwadd.wv v4, v2, v5
	vwadd.wv v0, v2, v4, v0.t
	vwadd.wx v1, v2, a1
	vwadd.wx v2, v2, a1
	vwadd.wx v2, v3, a1
	vwadd.wx v0, v2, a1, v0.t

	vwsub.vv v1, v2, v4
	vwsub.vv v2, v2, v4
	vwsub.vv v2, v3, v4
	vwsub.vv v4, v2, v4
	vwsub.vv v4, v2, v5
	vwsub.vv v0, v2, v4, v0.t
	vwsub.vx v1, v2, a1
	vwsub.vx v2, v2, a1
	vwsub.vx v2, v3, a1
	vwsub.vx v0, v2, a1, v0.t
	vwsub.wv v1, v2, v4
	vwsub.wv v2, v2, v4
	vwsub.wv v2, v3, v4
	vwsub.wv v4, v2, v4
	vwsub.wv v4, v2, v5
	vwsub.wv v0, v2, v4, v0.t
	vwsub.wx v1, v2, a1
	vwsub.wx v2, v2, a1
	vwsub.wx v2, v3, a1
	vwsub.wx v0, v2, a1, v0.t

# Vector Widening Integer Multiply Instructions

	vwmul.vv v1, v2, v4		# vd should be multiple of 2
	vwmul.vv v2, v2, v4		# vd overlap vs2
	vwmul.vv v2, v3, v4		# vd overlap vs2
	vwmul.vv v4, v2, v4		# vd overlap vs1
	vwmul.vv v4, v2, v5		# vd overlap vs1
	vwmul.vv v0, v2, v4, v0.t	# vd overlap vm
	vwmul.vx v1, v2, a1		# vd should be multiple of 2
	vwmul.vx v2, v2, a1		# vd overlap vs2
	vwmul.vx v2, v3, a1		# vd overlap vs2
	vwmul.vx v0, v2, a1, v0.t	# vd overlap vm

	vwmulu.vv v1, v2, v4
	vwmulu.vv v2, v2, v4
	vwmulu.vv v2, v3, v4
	vwmulu.vv v4, v2, v4
	vwmulu.vv v4, v2, v5
	vwmulu.vv v0, v2, v4, v0.t
	vwmulu.vx v1, v2, a1
	vwmulu.vx v2, v2, a1
	vwmulu.vx v2, v3, a1
	vwmulu.vx v0, v2, a1, v0.t

	vwmulsu.vv v1, v2, v4
	vwmulsu.vv v2, v2, v4
	vwmulsu.vv v2, v3, v4
	vwmulsu.vv v4, v2, v4
	vwmulsu.vv v4, v2, v5
	vwmulsu.vv v0, v2, v4, v0.t
	vwmulsu.vx v1, v2, a1
	vwmulsu.vx v2, v2, a1
	vwmulsu.vx v2, v3, a1
	vwmulsu.vx v0, v2, a1, v0.t

# Vector Widening Integer Multiply-Add Instructions

	vwmaccu.vv v1, v2, v4		# vd should be multiple of 2
	vwmaccu.vv v2, v2, v4		# vd overlap vs1
	vwmaccu.vv v2, v3, v4		# vd overlap vs1
	vwmaccu.vv v4, v2, v4		# vd overlap vs2
	vwmaccu.vv v4, v2, v5		# vd overlap vs2
	vwmaccu.vv v0, v2, v4, v0.t	# vd overlap vm
	vwmaccu.vx v1, a1, v2		# vd should be multiple of 2
	vwmaccu.vx v2, a1, v2		# vd overlap vs2
	vwmaccu.vx v2, a1, v3		# vd overlap vs2
	vwmaccu.vx v0, a1, v2, v0.t	# vd overlap vm

	vwmacc.vv v1, v2, v4
	vwmacc.vv v2, v2, v4
	vwmacc.vv v2, v3, v4
	vwmacc.vv v4, v2, v4
	vwmacc.vv v4, v2, v5
	vwmacc.vv v0, v2, v4, v0.t
	vwmacc.vx v1, a1, v2
	vwmacc.vx v2, a1, v2
	vwmacc.vx v2, a1, v3
	vwmacc.vx v0, a1, v2, v0.t

	vwmaccsu.vv v1, v2, v4
	vwmaccsu.vv v2, v2, v4
	vwmaccsu.vv v2, v3, v4
	vwmaccsu.vv v4, v2, v4
	vwmaccsu.vv v4, v2, v5
	vwmaccsu.vv v0, v2, v4, v0.t
	vwmaccsu.vx v1, a1, v2
	vwmaccsu.vx v2, a1, v2
	vwmaccsu.vx v2, a1, v3
	vwmaccsu.vx v0, a1, v2, v0.t

	vwmaccus.vx v1, a1, v2		# vd should be multiple of 2
	vwmaccus.vx v2, a1, v2		# vd overlap vs2
	vwmaccus.vx v2, a1, v3		# vd overlap vs2
	vwmaccus.vx v0, a1, v2, v0.t	# vd overlap vm

# Vector Widening Floating-Point Add/Subtract Instructions

	vfwadd.vv v1, v2, v4		# vd should be multiple of 2
	vfwadd.vv v2, v2, v4		# vd overlap vs2
	vfwadd.vv v2, v3, v4		# vd overlap vs2
	vfwadd.vv v4, v2, v4		# vd overlap vs1
	vfwadd.vv v4, v2, v5		# vd overlap vs1
	vfwadd.vv v0, v2, v4, v0.t	# vd overlap vm
	vfwadd.vf v1, v2, fa1		# vd should be multiple of 2
	vfwadd.vf v2, v2, fa1		# vd overlap vs2
	vfwadd.vf v2, v3, fa1		# vd overlap vs2
	vfwadd.vf v0, v2, fa1, v0.t	# vd overlap vm
	vfwadd.wv v1, v2, v4		# vd should be multiple of 2
	vfwadd.wv v2, v2, v4		# OK
	vfwadd.wv v2, v3, v4		# vs2 should be multiple of 2
	vfwadd.wv v4, v2, v4		# vd overlap vs1
	vfwadd.wv v4, v2, v5		# vd overlap vs1
	vfwadd.wv v0, v2, v4, v0.t	# vd overlap vm

	vfwsub.vv v1, v2, v4
	vfwsub.vv v2, v2, v4
	vfwsub.vv v2, v3, v4
	vfwsub.vv v4, v2, v4
	vfwsub.vv v4, v2, v5
	vfwsub.vv v0, v2, v4, v0.t
	vfwsub.vf v1, v2, fa1
	vfwsub.vf v2, v2, fa1
	vfwsub.vf v2, v3, fa1
	vfwsub.vf v0, v2, fa1, v0.t
	vfwsub.wv v1, v2, v4
	vfwsub.wv v2, v2, v4
	vfwsub.wv v2, v3, v4
	vfwsub.wv v4, v2, v4
	vfwsub.wv v4, v2, v5
	vfwsub.wv v0, v2, v4, v0.t

# Vector Widening Floating-Point Multiply

	vfwmul.vv v1, v2, v4		# vd should be multiple of 2
	vfwmul.vv v2, v2, v4		# vd overlap vs2
	vfwmul.vv v2, v3, v4		# vd overlap vs2
	vfwmul.vv v4, v2, v4		# vd overlap vs1
	vfwmul.vv v4, v2, v5		# vd overlap vs1
	vfwmul.vv v0, v2, v4, v0.t	# vd overlap vm
	vfwsub.vf v1, v2, fa1		# vd should be multiple of 2
	vfwsub.vf v2, v2, fa1		# vd overlap vs2
	vfwsub.vf v2, v3, fa1		# vd overlap vs2
	vfwsub.vf v0, v2, fa1, v0.t	# vd overlap vm

# Vector Widening Floating-Point Fused Multiply-Add Instructions
	vfwmacc.vv v1, v2, v4		# vd should be multiple of 2
	vfwmacc.vv v2, v2, v4		# vd overlap vs1
	vfwmacc.vv v2, v3, v4		# vd overlap vs1
	vfwmacc.vv v4, v2, v4		# vd overlap vs2
	vfwmacc.vv v4, v2, v5		# vd overlap vs2
	vfwmacc.vv v0, v2, v4, v0.t	# vd overlap vm
	vfwmacc.vf v1, fa1, v2		# vd should be multiple of 2
	vfwmacc.vf v2, fa1, v2		# vd overlap vs2
	vfwmacc.vf v2, fa1, v3		# vd overlap vs2
	vfwmacc.vf v0, fa1, v2, v0.t	# vd overlap vm

	vfwnmacc.vv v1, v2, v4
	vfwnmacc.vv v2, v2, v4
	vfwnmacc.vv v2, v3, v4
	vfwnmacc.vv v4, v2, v4
	vfwnmacc.vv v4, v2, v5
	vfwnmacc.vv v0, v2, v4, v0.t
	vfwnmacc.vf v1, fa1, v2
	vfwnmacc.vf v2, fa1, v2
	vfwnmacc.vf v2, fa1, v3
	vfwnmacc.vf v0, fa1, v2, v0.t

	vfwmsac.vv v1, v2, v4
	vfwmsac.vv v2, v2, v4
	vfwmsac.vv v2, v3, v4
	vfwmsac.vv v4, v2, v4
	vfwmsac.vv v4, v2, v5
	vfwmsac.vv v0, v2, v4, v0.t
	vfwmsac.vf v1, fa1, v2
	vfwmsac.vf v2, fa1, v2
	vfwmsac.vf v2, fa1, v3
	vfwmsac.vf v0, fa1, v2, v0.t

	vfwnmsac.vv v1, v2, v4
	vfwnmsac.vv v2, v2, v4
	vfwnmsac.vv v2, v3, v4
	vfwnmsac.vv v4, v2, v4
	vfwnmsac.vv v4, v2, v5
	vfwnmsac.vv v0, v2, v4, v0.t
	vfwnmsac.vf v1, fa1, v2
	vfwnmsac.vf v2, fa1, v2
	vfwnmsac.vf v2, fa1, v3
	vfwnmsac.vf v0, fa1, v2, v0.t

# Widening Floating-Point/Integer Type-Convert Instructions

	vfwcvt.xu.f.v v1, v2		# vd should be multiple of 2
	vfwcvt.xu.f.v v2, v2		# vd overlap vs2
	vfwcvt.xu.f.v v2, v3		# vd overlap vs2
	vfwcvt.xu.f.v v0, v2, v0.t	# vd overlap vm
	vfwcvt.x.f.v v1, v2
	vfwcvt.x.f.v v2, v2
	vfwcvt.x.f.v v2, v3
	vfwcvt.x.f.v v0, v2, v0.t
	vfwcvt.rtz.xu.f.v v1, v2
	vfwcvt.rtz.xu.f.v v2, v2
	vfwcvt.rtz.xu.f.v v2, v3
	vfwcvt.rtz.xu.f.v v0, v2, v0.t
	vfwcvt.rtz.x.f.v v1, v2
	vfwcvt.rtz.x.f.v v2, v2
	vfwcvt.rtz.x.f.v v2, v3
	vfwcvt.rtz.x.f.v v0, v2, v0.t
	vfwcvt.f.xu.v v1, v2
	vfwcvt.f.xu.v v2, v2
	vfwcvt.f.xu.v v2, v3
	vfwcvt.f.xu.v v0, v2, v0.t
	vfwcvt.f.x.v v1, v2
	vfwcvt.f.x.v v2, v2
	vfwcvt.f.x.v v2, v3
	vfwcvt.f.x.v v0, v2, v0.t
	vfwcvt.f.f.v v1, v2
	vfwcvt.f.f.v v2, v2
	vfwcvt.f.f.v v2, v3
	vfwcvt.f.f.v v0, v2, v0.t
