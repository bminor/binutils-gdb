# Vector Single-Width Floating-Point Add/Subtract Instructions

	vfadd.vv v4, v4, v8		# OK
	vfadd.vv v8, v4, v8		# OK
	vfadd.vv v0, v4, v8, v0.t	# vd overlap vm
	vfadd.vf v4, v4, fa1		# OK
	vfadd.vf v0, v4, fa1, v0.t	# vd overlap vm

	vfsub.vv v4, v4, v8
	vfsub.vv v8, v4, v8
	vfsub.vv v0, v4, v8, v0.t
	vfsub.vf v4, v4, fa1
	vfsub.vf v0, v4, fa1, v0.t

	vfrsub.vf v4, v4, fa1		# OK
	vfrsub.vf v0, v4, fa1, v0.t	# vd overlap vm

# Vector Single-Width Floating-Point Multiply/Divide Instructions

	vfmul.vv v4, v4, v8		# OK
	vfmul.vv v8, v4, v8		# OK
	vfmul.vv v0, v4, v8, v0.t	# vd overlap vm
	vfmul.vf v4, v4, fa1		# OK
	vfmul.vf v0, v4, fa1, v0.t	# vd overlap vm

	vfdiv.vv v4, v4, v8
	vfdiv.vv v8, v4, v8
	vfdiv.vv v0, v4, v8, v0.t
	vfdiv.vf v4, v4, fa1
	vfdiv.vf v0, v4, fa1, v0.t

	vfrdiv.vf v4, v4, fa1		# OK
	vfrdiv.vf v0, v4, fa1, v0.t	# vd overlap vm

# Vector Single-Width Floating-Point Fused Multiply-Add Instructions

	vfmacc.vv v4, v4, v8		# OK
	vfmacc.vv v8, v4, v8		# OK
	vfmacc.vv v0, v4, v8, v0.t	# vd overlap vm
	vfmacc.vf v4, fa1, v4		# OK
	vfmacc.vf v0, fa1, v4, v0.t	# vd overlap vm

	vfnmacc.vv v4, v4, v8
	vfnmacc.vv v8, v4, v8
	vfnmacc.vv v0, v4, v8, v0.t
	vfnmacc.vf v4, fa1, v4
	vfnmacc.vf v0, fa1, v4, v0.t

	vfmsac.vv v4, v4, v8
	vfmsac.vv v8, v4, v8
	vfmsac.vv v0, v4, v8, v0.t
	vfmsac.vf v4, fa1, v4
	vfmsac.vf v0, fa1, v4, v0.t

	vfnmsac.vv v4, v4, v8
	vfnmsac.vv v8, v4, v8
	vfnmsac.vv v0, v4, v8, v0.t
	vfnmsac.vf v4, fa1, v4
	vfnmsac.vf v0, fa1, v4, v0.t

	vfmadd.vv v4, v4, v8
	vfmadd.vv v8, v4, v8
	vfmadd.vv v0, v4, v8, v0.t
	vfmadd.vf v4, fa1, v4
	vfmadd.vf v0, fa1, v4, v0.t

	vfnmadd.vv v4, v4, v8
	vfnmadd.vv v8, v4, v8
	vfnmadd.vv v0, v4, v8, v0.t
	vfnmadd.vf v4, fa1, v4
	vfnmadd.vf v0, fa1, v4, v0.t

	vfmsub.vv v4, v4, v8
	vfmsub.vv v8, v4, v8
	vfmsub.vv v0, v4, v8, v0.t
	vfmsub.vf v4, fa1, v4
	vfmsub.vf v0, fa1, v4, v0.t

	vfnmsub.vv v4, v4, v8
	vfnmsub.vv v8, v4, v8
	vfnmsub.vv v0, v4, v8, v0.t
	vfnmsub.vf v4, fa1, v4
	vfnmsub.vf v0, fa1, v4, v0.t

# Vector Floating-Point Square-Root Instruction

	vfsqrt.v v4, v4			# OK
	vfsqrt.v v0, v4, v0.t		# vd overlap vm

# Vector Floating-Point Reciprocal Estimate Instruction

	vfrece7.v v4, v4		# OK
	vfrece7.v v0, v4, v0.t		# vd overlap vm

# Vector Floating-Point Reciprocal Square-Root Estimate Instruction

	vfrsqrte7.v v4, v4		# OK
	vfrsqrte7.v v0, v4, v0.t	# vd overlap vm

# Vector Floating-Point Classify Instruction

	vfclass.v v4, v4		# OK
	vfclass.v v0, v4, v0.t		# vd overlap vm

# Vector Floating-Point MIN/MAX Instructions

	vfmin.vv v4, v4, v8		# OK
	vfmin.vv v8, v4, v8		# OK
	vfmin.vv v0, v4, v8, v0.t	# vd overlap vm
	vfmin.vf v4, v4, fa1		# OK
	vfmin.vf v0, v4, fa1, v0.t	# vd overlap vm

	vfmax.vv v4, v4, v8
	vfmax.vv v8, v4, v8
	vfmax.vv v0, v4, v8, v0.t
	vfmax.vf v4, v4, fa1
	vfmax.vf v0, v4, fa1, v0.t

# Vector Floating-Point Sign-Injection Instructions

	vfneg.v v4, v4			# OK
	vfneg.v v0, v4, v0.t		# vd overlap vm
	vfabs.v v4, v4			# OK
	vfabs.v v0, v4, v0.t		# vd overlap vm

	vfsgnj.vv v4, v4, v8		# OK
	vfsgnj.vv v8, v4, v8		# OK
	vfsgnj.vv v0, v4, v8, v0.t	# vd overlap vm
	vfsgnj.vf v4, v4, fa1		# OK
	vfsgnj.vf v0, v4, fa1, v0.t	# vd overlap vm

	vfsgnjn.vv v4, v4, v8
	vfsgnjn.vv v8, v4, v8
	vfsgnjn.vv v0, v4, v8, v0.t
	vfsgnjn.vf v4, v4, fa1
	vfsgnjn.vf v0, v4, fa1, v0.t

	vfsgnjx.vv v4, v4, v8
	vfsgnjx.vv v8, v4, v8
	vfsgnjx.vv v0, v4, v8, v0.t
	vfsgnjx.vf v4, v4, fa1
	vfsgnjx.vf v0, v4, fa1, v0.t

# Single-Width Floating-Point/Integer Type-Convert Instructions

	vfcvt.xu.f.v v4, v4		# OK
	vfcvt.xu.f.v v0, v4, v0.t	# vd overlap vm
	vfcvt.x.f.v v4, v4
	vfcvt.x.f.v v0, v4, v0.t
	vfcvt.rtz.xu.f.v v4, v4
	vfcvt.rtz.xu.f.v v0, v4, v0.t
	vfcvt.rtz.x.f.v v4, v4
	vfcvt.rtz.x.f.v v0, v4, v0.t
	vfcvt.f.xu.v v4, v4
	vfcvt.f.xu.v v0, v4, v0.t
	vfcvt.f.x.v v4, v4
	vfcvt.f.x.v v0, v4, v0.t
