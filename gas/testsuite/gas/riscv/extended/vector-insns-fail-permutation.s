# Vector Slideup Instructions

	vslideup.vx v4, v4, a1			# vd overlap vs2
	vslideup.vx v0, v4, a1, v0.t		# vd overlap vm
	vslideup.vi v4, v4, 31			# vd overlap vs2
	vslideup.vi v0, v4, 31, v0.t		# vd overlap vm

	vslidedown.vx v4, v4, a1		# OK
	vslidedown.vx v0, v4, a1, v0.t		# vd overlap vm
	vslidedown.vi v4, v4, 31		# OK
	vslidedown.vi v0, v4, 31, v0.t		# vd overlap vm

	vslide1up.vx v4, v4, a1			# vd overlap vs2
	vslide1up.vx v0, v4, a1, v0.t		# vd overlap vm
	vfslide1up.vf v4, v4, fa1		# vd overlap vs2
	vfslide1up.vf v0, v4, fa1, v0.t		# vd overlap vm

	vslide1down.vx v4, v4, a1		# OK
	vslide1down.vx v0, v4, a1, v0.t		# vd overlap vm
	vfslide1down.vf v4, v4, fa1		# OK
	vfslide1down.vf v0, v4, fa1, v0.t	# vd overlap vm

# Vector Register Gather Instructions

	vrgather.vv v4, v4, v8			# vd overlap vs2
	vrgather.vv v8, v4, v8			# vd overlap vs1
	vrgather.vv v0, v4, v8, v0.t		# vd overlap vm
	vrgather.vx v4, v4, a1			# vd overlap vs2
	vrgather.vx v0, v4, a1, v0.t		# vd overlap vm
	vrgather.vi v4, v4, 31			# vd overlap vs2
	vrgather.vi v0, v4, 31, v0.t		# vd overlap vm

	vrgatherei16.vv v4, v4, v8		# vd overlap vs2
	vrgatherei16.vv v8, v4, v8		# vd overlap vs1
	vrgatherei16.vv v0, v4, v8, v0.t	# vd overlap vm

# Vector Compress Instruction

	vcompress.vm v4, v4, v8			# vd overlap vs2
	vcompress.vm v8, v4, v8			# vd overlap vs1

# Whole Vector Register Move

	vmv1r.v v31, v31			# OK, HINT

	vmv2r.v v30, v30			# OK, HINT
	vmv2r.v v30, v31			# vs2 must be aligned to 2
	vmv2r.v v31, v30			# vd must be aligned to 2

	vmv4r.v v28, v28			# OK, HINT
	vmv4r.v v28, v30			# vs2 must be aligned to 4
	vmv4r.v v30, v28			# vd must be aligned to 4

	vmv8r.v v24, v24			# OK, HINT
	vmv8r.v v24, v26			# vs2 must be aligned to 8
	vmv8r.v v26, v24			# vd must be aligned to 8
