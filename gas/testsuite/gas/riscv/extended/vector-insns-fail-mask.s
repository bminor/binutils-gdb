# Vector Set-before-first Mask Bit

	vmsbf.m v4, v4		# vd overlap vs2
	vmsbf.m v0, v4, v0.t	# vd overlap vm

# Vector Set-including-first Mask Bit

	vmsif.m v4, v4		# vd overlap vs2
	vmsif.m v0, v4, v0.t	# vd overlap vm

# Vector Set-only-first Mask Bit

	vmsof.m v4, v4		# vd overlap vs2
	vmsof.m v0, v4, v0.t	# vd overlap vm

# Vector Iota Instruction

	viota.m v4, v4		# vd overlap vs2
	viota.m v0, v4, v0.t	# vd overlap vm

# Vector Element Index Instruction

	vid.v v0, v0.t		# vd overlap vm
