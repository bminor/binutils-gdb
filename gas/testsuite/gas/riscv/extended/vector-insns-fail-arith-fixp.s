# Vector Single-Width Saturating Add and Subtract

	vsaddu.vv v4, v4, v8		# OK
	vsaddu.vv v8, v4, v8		# OK
	vsaddu.vv v0, v4, v8, v0.t	# vd overlap vm
	vsaddu.vx v4, v4, a1		# OK
	vsaddu.vx v0, v4, a1, v0.t	# vd overlap vm
	vsaddu.vi v4, v4, 15		# OK
	vsaddu.vi v0, v4, 15, v0.t	# vd overlap vm

	vsadd.vv v4, v4, v8
	vsadd.vv v8, v4, v8
	vsadd.vv v0, v4, v8, v0.t
	vsadd.vx v4, v4, a1
	vsadd.vx v0, v4, a1, v0.t
	vsadd.vi v4, v4, 15
	vsadd.vi v0, v4, 15, v0.t

	vssubu.vv v4, v4, v8		# OK
	vssubu.vv v8, v4, v8		# OK
	vssubu.vv v0, v4, v8, v0.t	# vd overlap vm
	vssubu.vx v4, v4, a1		# OK
	vssubu.vx v0, v4, a1, v0.t	# vd overlap vm

	vssub.vv v4, v4, v8
	vssub.vv v8, v4, v8
	vssub.vv v0, v4, v8, v0.t
	vssub.vx v4, v4, a1
	vssub.vx v0, v4, a1, v0.t

# Vector Single-Width Averaging Add and Subtract

	vaaddu.vv v4, v4, v8		# OK
	vaaddu.vv v8, v4, v8		# OK
	vaaddu.vv v0, v4, v8, v0.t	# vd overlap vm
	vaaddu.vx v4, v4, a1		# OK
	vaaddu.vx v0, v4, a1, v0.t	# vd overlap vm

	vaadd.vv v4, v4, v8
	vaadd.vv v8, v4, v8
	vaadd.vv v0, v4, v8, v0.t
	vaadd.vx v4, v4, a1
	vaadd.vx v0, v4, a1, v0.t

	vasubu.vv v4, v4, v8
	vasubu.vv v8, v4, v8
	vasubu.vv v0, v4, v8, v0.t
	vasubu.vx v4, v4, a1
	vasubu.vx v0, v4, a1, v0.t

	vasub.vv v4, v4, v8
	vasub.vv v8, v4, v8
	vasub.vv v0, v4, v8, v0.t
	vasub.vx v4, v4, a1
	vasub.vx v0, v4, a1, v0.t

# Vector Single-Width Fractional Multiply with Rounding and Saturation

	vsmul.vv v4, v4, v8		# OK
	vsmul.vv v8, v4, v8		# OK
	vsmul.vv v0, v4, v8, v0.t	# vd overlap vm
	vsmul.vx v4, v4, a1		# OK
	vsmul.vx v0, v4, a1, v0.t	# vd overlap vm

# Vector Single-Width Scaling Shift Instructions

	vssrl.vv v4, v4, v8		# OK
	vssrl.vv v8, v4, v8		# OK
	vssrl.vv v0, v4, v8, v0.t	# vd overlap vm
	vssrl.vx v4, v4, a1		# OK
	vssrl.vx v0, v4, a1, v0.t	# vd overlap vm
	vssrl.vi v4, v4, 31		# OK
	vssrl.vi v0, v4, 31, v0.t	# vd overlap vm

	vssra.vv v4, v4, v8
	vssra.vv v8, v4, v8
	vssra.vv v0, v4, v8, v0.t
	vssra.vx v4, v4, a1
	vssra.vx v0, v4, a1, v0.t
	vssra.vi v4, v4, 31
	vssra.vi v0, v4, 31, v0.t
