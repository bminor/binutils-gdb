# Vector Single-Width Integer Add and Subtract

	vneg.v v4, v4			# OK
	vneg.v v0, v4, v0.t		# vd overlap vm

	vadd.vv v4, v4, v8		# OK
	vadd.vv v8, v4, v8		# OK
	vadd.vv v0, v4, v8, v0.t	# vd overlap vm
	vadd.vx v4, v4, a1		# OK
	vadd.vx v0, v4, a1, v0.t	# vd overlap vm
	vadd.vi v4, v4, 15		# OK
	vadd.vi v0, v4, 15, v0.t	# vd overlap vm

	vsub.vv v4, v4, v8		# OK
	vsub.vv v8, v4, v8		# OK
	vsub.vv v0, v4, v8, v0.t	# vd overlap vm
	vsub.vx v4, v4, a1		# OK
	vsub.vx v0, v4, a1, v0.t	# vd overlap vm

	vrsub.vx v4, v4, a1		# OK
	vrsub.vx v0, v4, a1, v0.t	# vd overlap vm
	vrsub.vi v4, v4, 15		# OK
	vrsub.vi v0, v4, 15, v0.t	# vd overlap vm

# Vector Integer Extension

	vzext.vf2 v4, v4		# OK
	vzext.vf2 v0, v4, v0.t		# vd overlap vm
	vsext.vf2 v4, v4
	vsext.vf2 v0, v4, v0.t
	vzext.vf4 v4, v4
	vzext.vf4 v0, v4, v0.t
	vsext.vf4 v4, v4
	vsext.vf4 v0, v4, v0.t
	vzext.vf8 v4, v4
	vzext.vf8 v0, v4, v0.t
	vsext.vf8 v4, v4
	vsext.vf8 v0, v4, v0.t

# Vector Integer Add-with-Carry / Subtract-with-Borrow Instructions

	vadc.vvm v4, v4, v8, v0		# OK
	vadc.vvm v8, v4, v8, v0		# OK
	vadc.vvm v0, v4, v8, v0		# vd overlap vm
	vadc.vxm v4, v4, a1, v0		# OK
	vadc.vxm v0, v4, a1, v0		# vd overlap vm
	vadc.vim v4, v4, 15, v0		# OK
	vadc.vim v0, v4, 15, v0		# vd overlap vm

	vsbc.vvm v4, v4, v8, v0		# OK
	vsbc.vvm v8, v4, v8, v0		# OK
	vsbc.vvm v0, v4, v8, v0		# vd overlap vm
	vsbc.vxm v4, v4, a1, v0		# OK
	vsbc.vxm v0, v4, a1, v0		# vd overlap vm

# Vector Bitwise Logical Instructions

	vnot.v v4, v4			# OK
	vnot.v v0, v4, v0.t		# vd overlap vm

	vand.vv	v4, v4, v8		# OK
	vand.vv v8, v4, v8		# OK
	vand.vv v0, v4, v8, v0.t	# vd overlap vm
	vand.vx v4, v4, a1		# OK
	vand.vx v0, v4, a1, v0.t	# vd overlap vm
	vand.vi v4, v4, 15		# OK
	vand.vi v0, v4, 15, v0.t	# vd overlap vm

	vor.vv	v4, v4, v8
	vor.vv v8, v4, v8
	vor.vv v0, v4, v8, v0.t
	vor.vx v4, v4, a1
	vor.vx v0, v4, a1, v0.t
	vor.vi v4, v4, 15
	vor.vi v0, v4, 15, v0.t

	vxor.vv	v4, v4, v8
	vxor.vv v8, v4, v8
	vxor.vv v0, v4, v8, v0.t
	vxor.vx v4, v4, a1
	vxor.vx v0, v4, a1, v0.t
	vxor.vi v4, v4, 15
	vxor.vi v0, v4, 15, v0.t

# Vector Single-Width Bit Shift Instructions

	vsll.vv v4, v4, v8		# OK
	vsll.vv v8, v4, v8		# OK
	vsll.vv v0, v4, v8, v0.t	# vd overlap vm
	vsll.vx v4, v4, a1		# OK
	vsll.vx v0, v4, a1, v0.t	# vd overlap vm
	vsll.vi v4, v4, 31		# OK
	vsll.vi v0, v4, 31, v0.t	# vd overlap vm

	vsrl.vv v4, v4, v8
	vsrl.vv v8, v4, v8
	vsrl.vv v0, v4, v8, v0.t
	vsrl.vx v4, v4, a1
	vsrl.vx v0, v4, a1, v0.t
	vsrl.vi v4, v4, 31
	vsrl.vi v0, v4, 31, v0.t

	vsra.vv v4, v4, v8
	vsra.vv v8, v4, v8
	vsra.vv v0, v4, v8, v0.t
	vsra.vx v4, v4, a1
	vsra.vx v0, v4, a1, v0.t
	vsra.vi v4, v4, 31
	vsra.vi v0, v4, 31, v0.t

# Vector Integer Min/Max Instructions

	vminu.vv v4, v4, v8		# OK
	vminu.vv v8, v4, v8		# OK
	vminu.vv v0, v4, v8, v0.t	# vd overlap vm
	vminu.vx v4, v4, a1		# OK
	vminu.vx v0, v4, a1, v0.t	# vd overlap vm

	vmin.vv v4, v4, v8
	vmin.vv v8, v4, v8
	vmin.vv v0, v4, v8, v0.t
	vmin.vx v4, v4, a1
	vmin.vx v0, v4, a1, v0.t

	vmaxu.vv v4, v4, v8
	vmaxu.vv v8, v4, v8
	vmaxu.vv v0, v4, v8, v0.t
	vmaxu.vx v4, v4, a1
	vmaxu.vx v0, v4, a1, v0.t

	vmax.vv v4, v4, v8
	vmax.vv v8, v4, v8
	vmax.vv v0, v4, v8, v0.t
	vmax.vx v4, v4, a1
	vmax.vx v0, v4, a1, v0.t

# Vector Single-Width Integer Multiply Instructions

	vmul.vv v4, v4, v8		# OK
	vmul.vv v8, v4, v8		# OK
	vmul.vv v0, v4, v8, v0.t	# vd overlap vm
	vmul.vx v4, v4, a1		# OK
	vmul.vx v0, v4, a1, v0.t	# vd overlap vm

	vmulh.vv v4, v4, v8
	vmulh.vv v8, v4, v8
	vmulh.vv v0, v4, v8, v0.t
	vmulh.vx v4, v4, a1
	vmulh.vx v0, v4, a1, v0.t

	vmulhu.vv v4, v4, v8
	vmulhu.vv v8, v4, v8
	vmulhu.vv v0, v4, v8, v0.t
	vmulhu.vx v4, v4, a1
	vmulhu.vx v0, v4, a1, v0.t

	vmulhsu.vv v4, v4, v8
	vmulhsu.vv v8, v4, v8
	vmulhsu.vv v0, v4, v8, v0.t
	vmulhsu.vx v4, v4, a1
	vmulhsu.vx v0, v4, a1, v0.t

# Vector Integer Divide Instructions

	vdivu.vv v4, v4, v8		# OK
	vdivu.vv v8, v4, v8		# OK
	vdivu.vv v0, v4, v8, v0.t	# vd overlap vm
	vdivu.vx v4, v4, a1		# OK
	vdivu.vx v0, v4, a1, v0.t	# vd overlap vm

	vdiv.vv v4, v4, v8
	vdiv.vv v8, v4, v8
	vdiv.vv v0, v4, v8, v0.t
	vdiv.vx v4, v4, a1
	vdiv.vx v0, v4, a1, v0.t

	vremu.vv v4, v4, v8
	vremu.vv v8, v4, v8
	vremu.vv v0, v4, v8, v0.t
	vremu.vx v4, v4, a1
	vremu.vx v0, v4, a1, v0.t

	vrem.vv v4, v4, v8
	vrem.vv v8, v4, v8
	vrem.vv v0, v4, v8, v0.t
	vrem.vx v4, v4, a1
	vrem.vx v0, v4, a1, v0.t

# Vector Single-Width Integer Multiply-Add Instructions

	vmacc.vv v4, v4, v8		# OK
	vmacc.vv v8, v4, v8		# OK
	vmacc.vv v0, v4, v8, v0.t	# vd overlap vm
	vmacc.vx v4, a1, v4		# OK
	vmacc.vx v0, a1, v4, v0.t	# vd overlap vm

	vnmsac.vv v4, v4, v8
	vnmsac.vv v8, v4, v8
	vnmsac.vv v0, v4, v8, v0.t
	vnmsac.vx v4, a1, v4
	vnmsac.vx v0, a1, v4, v0.t

	vmadd.vv v4, v4, v8
	vmadd.vv v8, v4, v8
	vmadd.vv v0, v4, v8, v0.t
	vmadd.vx v4, a1, v4
	vmadd.vx v0, a1, v4, v0.t

	vnmsub.vv v4, v4, v8
	vnmsub.vv v8, v4, v8
	vnmsub.vv v0, v4, v8, v0.t
	vnmsub.vx v4, a1, v4
	vnmsub.vx v0, a1, v4, v0.t
