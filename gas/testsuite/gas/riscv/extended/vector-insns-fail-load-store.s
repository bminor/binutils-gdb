# Vector Unit-Stride Loads and Stores

	vle8.v v0, (a0), v0.t		# vd overlap vm
	vle8ff.v v0, (a0), v0.t		# vd overlap vm
	vle16.v v0, (a0), v0.t
	vle16ff.v v0, (a0), v0.t
	vle32.v v0, (a0), v0.t
	vle32ff.v v0, (a0), v0.t
	vle64.v v0, (a0), v0.t
	vle64ff.v v0, (a0), v0.t

	vse8.v v0, (a0), v0.t		# vd overlap vm
	vse16.v v0, (a0), v0.t
	vse32.v v0, (a0), v0.t
	vse64.v v0, (a0), v0.t

# Vector Strided Loads and Stores

	vlse8.v v0, (a0), a1, v0.t	# vd overlap vm
	vlse16.v v0, (a0), a1, v0.t
	vlse32.v v0, (a0), a1, v0.t
	vlse64.v v0, (a0), a1, v0.t

	vsse8.v v0, (a0), a1, v0.t
	vsse16.v v0, (a0), a1, v0.t
	vsse32.v v0, (a0), a1, v0.t
	vsse64.v v0, (a0), a1, v0.t

# Vector Ordered Indexed Loads and Stores

	vloxei8.v v4, (a0), v4		# OK
	vloxei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vloxei16.v v4, (a0), v4
	vloxei16.v v0, (a0), v4, v0.t
	vloxei32.v v4, (a0), v4
	vloxei32.v v0, (a0), v4, v0.t
	vloxei64.v v4, (a0), v4
	vloxei64.v v0, (a0), v4, v0.t

	vsoxei8.v v4, (a0), v4
	vsoxei8.v v0, (a0), v4, v0.t
	vsoxei16.v v4, (a0), v4
	vsoxei16.v v0, (a0), v4, v0.t
	vsoxei32.v v4, (a0), v4
	vsoxei32.v v0, (a0), v4, v0.t
	vsoxei64.v v4, (a0), v4
	vsoxei64.v v0, (a0), v4, v0.t

# Vector Unordered Indexed Loads and Stores

	vluxei8.v v4, (a0), v4		# OK
	vluxei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vluxei16.v v4, (a0), v4
	vluxei16.v v0, (a0), v4, v0.t
	vluxei32.v v4, (a0), v4
	vluxei32.v v0, (a0), v4, v0.t
	vluxei64.v v4, (a0), v4
	vluxei64.v v0, (a0), v4, v0.t

	vsuxei8.v v4, (a0), v4
	vsuxei8.v v0, (a0), v4, v0.t
	vsuxei16.v v4, (a0), v4
	vsuxei16.v v0, (a0), v4, v0.t
	vsuxei32.v v4, (a0), v4
	vsuxei32.v v0, (a0), v4, v0.t
	vsuxei64.v v4, (a0), v4
	vsuxei64.v v0, (a0), v4, v0.t

# Vector Unit-Stride Segment Loads and Stores

	vlseg2e8.v v0, (a0), v0.t	# vd overlap vm
	vsseg2e8.v v0, (a0), v0.t	# vd overlap vm
	vlseg2e8ff.v v0, (a0), v0.t	# vd overlap vm
	vlseg3e8.v v0, (a0), v0.t
	vsseg3e8.v v0, (a0), v0.t
	vlseg3e8ff.v v0, (a0), v0.t
	vlseg4e8.v v0, (a0), v0.t
	vsseg4e8.v v0, (a0), v0.t
	vlseg4e8ff.v v0, (a0), v0.t
	vlseg5e8.v v0, (a0), v0.t
	vsseg5e8.v v0, (a0), v0.t
	vlseg5e8ff.v v0, (a0), v0.t
	vlseg6e8.v v0, (a0), v0.t
	vsseg6e8.v v0, (a0), v0.t
	vlseg6e8ff.v v0, (a0), v0.t
	vlseg7e8.v v0, (a0), v0.t
	vsseg7e8.v v0, (a0), v0.t
	vlseg7e8ff.v v0, (a0), v0.t
	vlseg8e8.v v0, (a0), v0.t
	vsseg8e8.v v0, (a0), v0.t
	vlseg8e8ff.v v0, (a0), v0.t

	vlseg2e16.v v0, (a0), v0.t
	vsseg2e16.v v0, (a0), v0.t
	vlseg2e16ff.v v0, (a0), v0.t
	vlseg3e16.v v0, (a0), v0.t
	vsseg3e16.v v0, (a0), v0.t
	vlseg3e16ff.v v0, (a0), v0.t
	vlseg4e16.v v0, (a0), v0.t
	vsseg4e16.v v0, (a0), v0.t
	vlseg4e16ff.v v0, (a0), v0.t
	vlseg5e16.v v0, (a0), v0.t
	vsseg5e16.v v0, (a0), v0.t
	vlseg5e16ff.v v0, (a0), v0.t
	vlseg6e16.v v0, (a0), v0.t
	vsseg6e16.v v0, (a0), v0.t
	vlseg6e16ff.v v0, (a0), v0.t
	vlseg7e16.v v0, (a0), v0.t
	vsseg7e16.v v0, (a0), v0.t
	vlseg7e16ff.v v0, (a0), v0.t
	vlseg8e16.v v0, (a0), v0.t
	vsseg8e16.v v0, (a0), v0.t
	vlseg8e16ff.v v0, (a0), v0.t

	vlseg2e32.v v0, (a0), v0.t
	vsseg2e32.v v0, (a0), v0.t
	vlseg2e32ff.v v0, (a0), v0.t
	vlseg3e32.v v0, (a0), v0.t
	vsseg3e32.v v0, (a0), v0.t
	vlseg3e32ff.v v0, (a0), v0.t
	vlseg4e32.v v0, (a0), v0.t
	vsseg4e32.v v0, (a0), v0.t
	vlseg4e32ff.v v0, (a0), v0.t
	vlseg5e32.v v0, (a0), v0.t
	vsseg5e32.v v0, (a0), v0.t
	vlseg5e32ff.v v0, (a0), v0.t
	vlseg6e32.v v0, (a0), v0.t
	vsseg6e32.v v0, (a0), v0.t
	vlseg6e32ff.v v0, (a0), v0.t
	vlseg7e32.v v0, (a0), v0.t
	vsseg7e32.v v0, (a0), v0.t
	vlseg7e32ff.v v0, (a0), v0.t
	vlseg8e32.v v0, (a0), v0.t
	vsseg8e32.v v0, (a0), v0.t
	vlseg8e32ff.v v0, (a0), v0.t

	vlseg2e64.v v0, (a0), v0.t
	vsseg2e64.v v0, (a0), v0.t
	vlseg2e64ff.v v0, (a0), v0.t
	vlseg3e64.v v0, (a0), v0.t
	vsseg3e64.v v0, (a0), v0.t
	vlseg3e64ff.v v0, (a0), v0.t
	vlseg4e64.v v0, (a0), v0.t
	vsseg4e64.v v0, (a0), v0.t
	vlseg4e64ff.v v0, (a0), v0.t
	vlseg5e64.v v0, (a0), v0.t
	vsseg5e64.v v0, (a0), v0.t
	vlseg5e64ff.v v0, (a0), v0.t
	vlseg6e64.v v0, (a0), v0.t
	vsseg6e64.v v0, (a0), v0.t
	vlseg6e64ff.v v0, (a0), v0.t
	vlseg7e64.v v0, (a0), v0.t
	vsseg7e64.v v0, (a0), v0.t
	vlseg7e64ff.v v0, (a0), v0.t
	vlseg8e64.v v0, (a0), v0.t
	vsseg8e64.v v0, (a0), v0.t
	vlseg8e64ff.v v0, (a0), v0.t

# Vector Strided Segment Loads and Stores

	vlsseg2e8.v v0, (a0), a1, v0.t		# vd overlap vm
	vssseg2e8.v v0, (a0), a1, v0.t		# vd overlap vm
	vlsseg3e8.v v0, (a0), a1, v0.t
	vssseg3e8.v v0, (a0), a1, v0.t
	vlsseg4e8.v v0, (a0), a1, v0.t
	vssseg4e8.v v0, (a0), a1, v0.t
	vlsseg5e8.v v0, (a0), a1, v0.t
	vssseg5e8.v v0, (a0), a1, v0.t
	vlsseg6e8.v v0, (a0), a1, v0.t
	vssseg6e8.v v0, (a0), a1, v0.t
	vlsseg7e8.v v0, (a0), a1, v0.t
	vssseg7e8.v v0, (a0), a1, v0.t
	vlsseg8e8.v v0, (a0), a1, v0.t
	vssseg8e8.v v0, (a0), a1, v0.t

	vlsseg2e16.v v0, (a0), a1, v0.t
	vssseg2e16.v v0, (a0), a1, v0.t
	vlsseg3e16.v v0, (a0), a1, v0.t
	vssseg3e16.v v0, (a0), a1, v0.t
	vlsseg4e16.v v0, (a0), a1, v0.t
	vssseg4e16.v v0, (a0), a1, v0.t
	vlsseg5e16.v v0, (a0), a1, v0.t
	vssseg5e16.v v0, (a0), a1, v0.t
	vlsseg6e16.v v0, (a0), a1, v0.t
	vssseg6e16.v v0, (a0), a1, v0.t
	vlsseg7e16.v v0, (a0), a1, v0.t
	vssseg7e16.v v0, (a0), a1, v0.t
	vlsseg8e16.v v0, (a0), a1, v0.t
	vssseg8e16.v v0, (a0), a1, v0.t

	vlsseg2e32.v v0, (a0), a1, v0.t
	vssseg2e32.v v0, (a0), a1, v0.t
	vlsseg3e32.v v0, (a0), a1, v0.t
	vssseg3e32.v v0, (a0), a1, v0.t
	vlsseg4e32.v v0, (a0), a1, v0.t
	vssseg4e32.v v0, (a0), a1, v0.t
	vlsseg5e32.v v0, (a0), a1, v0.t
	vssseg5e32.v v0, (a0), a1, v0.t
	vlsseg6e32.v v0, (a0), a1, v0.t
	vssseg6e32.v v0, (a0), a1, v0.t
	vlsseg7e32.v v0, (a0), a1, v0.t
	vssseg7e32.v v0, (a0), a1, v0.t
	vlsseg8e32.v v0, (a0), a1, v0.t
	vssseg8e32.v v0, (a0), a1, v0.t

	vlsseg2e64.v v0, (a0), a1, v0.t
	vssseg2e64.v v0, (a0), a1, v0.t
	vlsseg3e64.v v0, (a0), a1, v0.t
	vssseg3e64.v v0, (a0), a1, v0.t
	vlsseg4e64.v v0, (a0), a1, v0.t
	vssseg4e64.v v0, (a0), a1, v0.t
	vlsseg5e64.v v0, (a0), a1, v0.t
	vssseg5e64.v v0, (a0), a1, v0.t
	vlsseg6e64.v v0, (a0), a1, v0.t
	vssseg6e64.v v0, (a0), a1, v0.t
	vlsseg7e64.v v0, (a0), a1, v0.t
	vssseg7e64.v v0, (a0), a1, v0.t
	vlsseg8e64.v v0, (a0), a1, v0.t
	vssseg8e64.v v0, (a0), a1, v0.t

# Vector Ordered Indexed Segment Loads and Stores

	vloxseg2ei8.v v4, (a0), v4		# vd overlap vs2
	vloxseg2ei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vsoxseg2ei8.v v4, (a0), v4		# vd overlap vs2
	vsoxseg2ei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vloxseg3ei8.v v4, (a0), v4
	vloxseg3ei8.v v0, (a0), v4, v0.t
	vsoxseg3ei8.v v4, (a0), v4
	vsoxseg3ei8.v v0, (a0), v4, v0.t
	vloxseg4ei8.v v4, (a0), v4
	vloxseg4ei8.v v0, (a0), v4, v0.t
	vsoxseg4ei8.v v4, (a0), v4
	vsoxseg4ei8.v v0, (a0), v4, v0.t
	vloxseg5ei8.v v4, (a0), v4
	vloxseg5ei8.v v0, (a0), v4, v0.t
	vsoxseg5ei8.v v4, (a0), v4
	vsoxseg5ei8.v v0, (a0), v4, v0.t
	vloxseg6ei8.v v4, (a0), v4
	vloxseg6ei8.v v0, (a0), v4, v0.t
	vsoxseg6ei8.v v4, (a0), v4
	vsoxseg6ei8.v v0, (a0), v4, v0.t
	vloxseg7ei8.v v4, (a0), v4
	vloxseg7ei8.v v0, (a0), v4, v0.t
	vsoxseg7ei8.v v4, (a0), v4
	vsoxseg7ei8.v v0, (a0), v4, v0.t
	vloxseg8ei8.v v4, (a0), v4
	vloxseg8ei8.v v0, (a0), v4, v0.t
	vsoxseg8ei8.v v4, (a0), v4
	vsoxseg8ei8.v v0, (a0), v4, v0.t

	vloxseg2ei16.v v4, (a0), v4
	vloxseg2ei16.v v0, (a0), v4, v0.t
	vsoxseg2ei16.v v4, (a0), v4
	vsoxseg2ei16.v v0, (a0), v4, v0.t
	vloxseg3ei16.v v4, (a0), v4
	vloxseg3ei16.v v0, (a0), v4, v0.t
	vsoxseg3ei16.v v4, (a0), v4
	vsoxseg3ei16.v v0, (a0), v4, v0.t
	vloxseg4ei16.v v4, (a0), v4
	vloxseg4ei16.v v0, (a0), v4, v0.t
	vsoxseg4ei16.v v4, (a0), v4
	vsoxseg4ei16.v v0, (a0), v4, v0.t
	vloxseg5ei16.v v4, (a0), v4
	vloxseg5ei16.v v0, (a0), v4, v0.t
	vsoxseg5ei16.v v4, (a0), v4
	vsoxseg5ei16.v v0, (a0), v4, v0.t
	vloxseg6ei16.v v4, (a0), v4
	vloxseg6ei16.v v0, (a0), v4, v0.t
	vsoxseg6ei16.v v4, (a0), v4
	vsoxseg6ei16.v v0, (a0), v4, v0.t
	vloxseg7ei16.v v4, (a0), v4
	vloxseg7ei16.v v0, (a0), v4, v0.t
	vsoxseg7ei16.v v4, (a0), v4
	vsoxseg7ei16.v v0, (a0), v4, v0.t
	vloxseg8ei16.v v4, (a0), v4
	vloxseg8ei16.v v0, (a0), v4, v0.t
	vsoxseg8ei16.v v4, (a0), v4
	vsoxseg8ei16.v v0, (a0), v4, v0.t

	vloxseg2ei32.v v4, (a0), v4
	vloxseg2ei32.v v0, (a0), v4, v0.t
	vsoxseg2ei32.v v4, (a0), v4
	vsoxseg2ei32.v v0, (a0), v4, v0.t
	vloxseg3ei32.v v4, (a0), v4
	vloxseg3ei32.v v0, (a0), v4, v0.t
	vsoxseg3ei32.v v4, (a0), v4
	vsoxseg3ei32.v v0, (a0), v4, v0.t
	vloxseg4ei32.v v4, (a0), v4
	vloxseg4ei32.v v0, (a0), v4, v0.t
	vsoxseg4ei32.v v4, (a0), v4
	vsoxseg4ei32.v v0, (a0), v4, v0.t
	vloxseg5ei32.v v4, (a0), v4
	vloxseg5ei32.v v0, (a0), v4, v0.t
	vsoxseg5ei32.v v4, (a0), v4
	vsoxseg5ei32.v v0, (a0), v4, v0.t
	vloxseg6ei32.v v4, (a0), v4
	vloxseg6ei32.v v0, (a0), v4, v0.t
	vsoxseg6ei32.v v4, (a0), v4
	vsoxseg6ei32.v v0, (a0), v4, v0.t
	vloxseg7ei32.v v4, (a0), v4
	vloxseg7ei32.v v0, (a0), v4, v0.t
	vsoxseg7ei32.v v4, (a0), v4
	vsoxseg7ei32.v v0, (a0), v4, v0.t
	vloxseg8ei32.v v4, (a0), v4
	vloxseg8ei32.v v0, (a0), v4, v0.t
	vsoxseg8ei32.v v4, (a0), v4
	vsoxseg8ei32.v v0, (a0), v4, v0.t

	vloxseg2ei64.v v4, (a0), v4
	vloxseg2ei64.v v0, (a0), v4, v0.t
	vsoxseg2ei64.v v4, (a0), v4
	vsoxseg2ei64.v v0, (a0), v4, v0.t
	vloxseg3ei64.v v4, (a0), v4
	vloxseg3ei64.v v0, (a0), v4, v0.t
	vsoxseg3ei64.v v4, (a0), v4
	vsoxseg3ei64.v v0, (a0), v4, v0.t
	vloxseg4ei64.v v4, (a0), v4
	vloxseg4ei64.v v0, (a0), v4, v0.t
	vsoxseg4ei64.v v4, (a0), v4
	vsoxseg4ei64.v v0, (a0), v4, v0.t
	vloxseg5ei64.v v4, (a0), v4
	vloxseg5ei64.v v0, (a0), v4, v0.t
	vsoxseg5ei64.v v4, (a0), v4
	vsoxseg5ei64.v v0, (a0), v4, v0.t
	vloxseg6ei64.v v4, (a0), v4
	vloxseg6ei64.v v0, (a0), v4, v0.t
	vsoxseg6ei64.v v4, (a0), v4
	vsoxseg6ei64.v v0, (a0), v4, v0.t
	vloxseg7ei64.v v4, (a0), v4
	vloxseg7ei64.v v0, (a0), v4, v0.t
	vsoxseg7ei64.v v4, (a0), v4
	vsoxseg7ei64.v v0, (a0), v4, v0.t
	vloxseg8ei64.v v4, (a0), v4
	vloxseg8ei64.v v0, (a0), v4, v0.t
	vsoxseg8ei64.v v4, (a0), v4
	vsoxseg8ei64.v v0, (a0), v4, v0.t

# Vector Unordered Indexed Segment Loads and Stores

	vluxseg2ei8.v v4, (a0), v4		# vd overlap vs2
	vluxseg2ei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vsuxseg2ei8.v v4, (a0), v4		# vd overlap vs2
	vsuxseg2ei8.v v0, (a0), v4, v0.t	# vd overlap vm
	vluxseg3ei8.v v4, (a0), v4
	vluxseg3ei8.v v0, (a0), v4, v0.t
	vsuxseg3ei8.v v4, (a0), v4
	vsuxseg3ei8.v v0, (a0), v4, v0.t
	vluxseg4ei8.v v4, (a0), v4
	vluxseg4ei8.v v0, (a0), v4, v0.t
	vsuxseg4ei8.v v4, (a0), v4
	vsuxseg4ei8.v v0, (a0), v4, v0.t
	vluxseg5ei8.v v4, (a0), v4
	vluxseg5ei8.v v0, (a0), v4, v0.t
	vsuxseg5ei8.v v4, (a0), v4
	vsuxseg5ei8.v v0, (a0), v4, v0.t
	vluxseg6ei8.v v4, (a0), v4
	vluxseg6ei8.v v0, (a0), v4, v0.t
	vsuxseg6ei8.v v4, (a0), v4
	vsuxseg6ei8.v v0, (a0), v4, v0.t
	vluxseg7ei8.v v4, (a0), v4
	vluxseg7ei8.v v0, (a0), v4, v0.t
	vsuxseg7ei8.v v4, (a0), v4
	vsuxseg7ei8.v v0, (a0), v4, v0.t
	vluxseg8ei8.v v4, (a0), v4
	vluxseg8ei8.v v0, (a0), v4, v0.t
	vsuxseg8ei8.v v4, (a0), v4
	vsuxseg8ei8.v v0, (a0), v4, v0.t

	vluxseg2ei16.v v4, (a0), v4
	vluxseg2ei16.v v0, (a0), v4, v0.t
	vsuxseg2ei16.v v4, (a0), v4
	vsuxseg2ei16.v v0, (a0), v4, v0.t
	vluxseg3ei16.v v4, (a0), v4
	vluxseg3ei16.v v0, (a0), v4, v0.t
	vsuxseg3ei16.v v4, (a0), v4
	vsuxseg3ei16.v v0, (a0), v4, v0.t
	vluxseg4ei16.v v4, (a0), v4
	vluxseg4ei16.v v0, (a0), v4, v0.t
	vsuxseg4ei16.v v4, (a0), v4
	vsuxseg4ei16.v v0, (a0), v4, v0.t
	vluxseg5ei16.v v4, (a0), v4
	vluxseg5ei16.v v0, (a0), v4, v0.t
	vsuxseg5ei16.v v4, (a0), v4
	vsuxseg5ei16.v v0, (a0), v4, v0.t
	vluxseg6ei16.v v4, (a0), v4
	vluxseg6ei16.v v0, (a0), v4, v0.t
	vsuxseg6ei16.v v4, (a0), v4
	vsuxseg6ei16.v v0, (a0), v4, v0.t
	vluxseg7ei16.v v4, (a0), v4
	vluxseg7ei16.v v0, (a0), v4, v0.t
	vsuxseg7ei16.v v4, (a0), v4
	vsuxseg7ei16.v v0, (a0), v4, v0.t
	vluxseg8ei16.v v4, (a0), v4
	vluxseg8ei16.v v0, (a0), v4, v0.t
	vsuxseg8ei16.v v4, (a0), v4
	vsuxseg8ei16.v v0, (a0), v4, v0.t

	vluxseg2ei32.v v4, (a0), v4
	vluxseg2ei32.v v0, (a0), v4, v0.t
	vsuxseg2ei32.v v4, (a0), v4
	vsuxseg2ei32.v v0, (a0), v4, v0.t
	vluxseg3ei32.v v4, (a0), v4
	vluxseg3ei32.v v0, (a0), v4, v0.t
	vsuxseg3ei32.v v4, (a0), v4
	vsuxseg3ei32.v v0, (a0), v4, v0.t
	vluxseg4ei32.v v4, (a0), v4
	vluxseg4ei32.v v0, (a0), v4, v0.t
	vsuxseg4ei32.v v4, (a0), v4
	vsuxseg4ei32.v v0, (a0), v4, v0.t
	vluxseg5ei32.v v4, (a0), v4
	vluxseg5ei32.v v0, (a0), v4, v0.t
	vsuxseg5ei32.v v4, (a0), v4
	vsuxseg5ei32.v v0, (a0), v4, v0.t
	vluxseg6ei32.v v4, (a0), v4
	vluxseg6ei32.v v0, (a0), v4, v0.t
	vsuxseg6ei32.v v4, (a0), v4
	vsuxseg6ei32.v v0, (a0), v4, v0.t
	vluxseg7ei32.v v4, (a0), v4
	vluxseg7ei32.v v0, (a0), v4, v0.t
	vsuxseg7ei32.v v4, (a0), v4
	vsuxseg7ei32.v v0, (a0), v4, v0.t
	vluxseg8ei32.v v4, (a0), v4
	vluxseg8ei32.v v0, (a0), v4, v0.t
	vsuxseg8ei32.v v4, (a0), v4
	vsuxseg8ei32.v v0, (a0), v4, v0.t

	vluxseg2ei64.v v4, (a0), v4
	vluxseg2ei64.v v0, (a0), v4, v0.t
	vsuxseg2ei64.v v4, (a0), v4
	vsuxseg2ei64.v v0, (a0), v4, v0.t
	vluxseg3ei64.v v4, (a0), v4
	vluxseg3ei64.v v0, (a0), v4, v0.t
	vsuxseg3ei64.v v4, (a0), v4
	vsuxseg3ei64.v v0, (a0), v4, v0.t
	vluxseg4ei64.v v4, (a0), v4
	vluxseg4ei64.v v0, (a0), v4, v0.t
	vsuxseg4ei64.v v4, (a0), v4
	vsuxseg4ei64.v v0, (a0), v4, v0.t
	vluxseg5ei64.v v4, (a0), v4
	vluxseg5ei64.v v0, (a0), v4, v0.t
	vsuxseg5ei64.v v4, (a0), v4
	vsuxseg5ei64.v v0, (a0), v4, v0.t
	vluxseg6ei64.v v4, (a0), v4
	vluxseg6ei64.v v0, (a0), v4, v0.t
	vsuxseg6ei64.v v4, (a0), v4
	vsuxseg6ei64.v v0, (a0), v4, v0.t
	vluxseg7ei64.v v4, (a0), v4
	vluxseg7ei64.v v0, (a0), v4, v0.t
	vsuxseg7ei64.v v4, (a0), v4
	vsuxseg7ei64.v v0, (a0), v4, v0.t
	vluxseg8ei64.v v4, (a0), v4
	vluxseg8ei64.v v0, (a0), v4, v0.t
	vsuxseg8ei64.v v4, (a0), v4
	vsuxseg8ei64.v v0, (a0), v4, v0.t

# Vector Load/Store Whole Register Instructions

	vl1r.v v31, (a0)		# OK

	vl2r.v v31, (a0)		# vd must be aligned to 2
	vl2re8.v v31, (a0)
	vl2re16.v v31, (a0)
	vl2re32.v v31, (a0)
	vl2re64.v v31, (a0)

	vl4r.v v30, (a0)		# vd must be aligned to 4
	vl4re8.v v30, (a0)
	vl4re16.v v30, (a0)
	vl4re32.v v30, (a0)
	vl4re64.v v30, (a0)

	vl8r.v v26, (a0)		# vd must be aligned to 8
	vl8re8.v v26, (a0)
	vl8re16.v v26, (a0)
	vl8re32.v v26, (a0)
	vl8re64.v v26, (a0)

	vs2r.v v31, (a0)		# vs3 must be aligned to 2
	vs4r.v v30, (a0)		# vs3 must be aligned to 4
	vs8r.v v26, (a0)		# vs3 must be aligned to 8
