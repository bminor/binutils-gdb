	vsetvl a0, a1, a2
	vsetvli a0, a1, e8
	vsetvli a0, a1, e16, m2
	vsetvli a0, a1, e32, m2, ta
	vsetvli a0, a1, e64, m2, ta, ma
	vsetvli a0, a1, e128
	vsetvli a0, a1, e256, m2
	vsetvli a0, a1, e512, m2, ta
	vsetvli a0, a1, e1024, m2, ta, ma

	vlm.v v4, (a0)
	vle1.v v4, (a0)		# Alias of vlm.v
	vsm.v v4, (a0)
	vse1.v v4, (a0)		# Alias of vsm.v

	vle8.v v4, (a0)
	vse8.v v4, (a0)
	vle16.v v4, (a0)
	vse16.v v4, (a0)
	vle32.v v4, (a0)
	vse32.v v4, (a0)
	vle64.v v4, (a0)
	vse64.v v4, (a0)

	vlse8.v v4, (a0), a1
	vsse8.v v4, (a0), a1
	vlse16.v v4, (a0), a1
	vsse16.v v4, (a0), a1
	vlse32.v v4, (a0), a1
	vsse32.v v4, (a0), a1
	vlse64.v v4, (a0), a1
	vsse64.v v4, (a0), a1

	vloxei8.v v4, (a0), v12
	vsoxei8.v v4, (a0), v12
	vluxei8.v v4, (a0), v12
	vsuxei8.v v4, (a0), v12
	vloxei16.v v4, (a0), v12
	vsoxei16.v v4, (a0), v12
	vluxei16.v v4, (a0), v12
	vsuxei16.v v4, (a0), v12
	vloxei32.v v4, (a0), v12
	vsoxei32.v v4, (a0), v12
	vluxei32.v v4, (a0), v12
	vsuxei32.v v4, (a0), v12
	vloxei64.v v4, (a0), v12
	vsoxei64.v v4, (a0), v12
	vluxei64.v v4, (a0), v12
	vsuxei64.v v4, (a0), v12

	vle8ff.v v4, (a0)
	vle16ff.v v4, (a0)
	vle32ff.v v4, (a0)
	vle64ff.v v4, (a0)

	vlseg2e8.v v4, (a0)
	vsseg2e8.v v4, (a0)
	vlseg3e8.v v4, (a0)
	vsseg3e8.v v4, (a0)
	vlseg4e8.v v4, (a0)
	vsseg4e8.v v4, (a0)
	vlseg5e8.v v4, (a0)
	vsseg5e8.v v4, (a0)
	vlseg6e8.v v4, (a0)
	vsseg6e8.v v4, (a0)
	vlseg7e8.v v4, (a0)
	vsseg7e8.v v4, (a0)
	vlseg8e8.v v4, (a0)
	vsseg8e8.v v4, (a0)
	vlseg2e16.v v4, (a0)
	vsseg2e16.v v4, (a0)
	vlseg3e16.v v4, (a0)
	vsseg3e16.v v4, (a0)
	vlseg4e16.v v4, (a0)
	vsseg4e16.v v4, (a0)
	vlseg5e16.v v4, (a0)
	vsseg5e16.v v4, (a0)
	vlseg6e16.v v4, (a0)
	vsseg6e16.v v4, (a0)
	vlseg7e16.v v4, (a0)
	vsseg7e16.v v4, (a0)
	vlseg8e16.v v4, (a0)
	vsseg8e16.v v4, (a0)
	vlseg2e32.v v4, (a0)
	vsseg2e32.v v4, (a0)
	vlseg3e32.v v4, (a0)
	vsseg3e32.v v4, (a0)
	vlseg4e32.v v4, (a0)
	vsseg4e32.v v4, (a0)
	vlseg5e32.v v4, (a0)
	vsseg5e32.v v4, (a0)
	vlseg6e32.v v4, (a0)
	vsseg6e32.v v4, (a0)
	vlseg7e32.v v4, (a0)
	vsseg7e32.v v4, (a0)
	vlseg8e32.v v4, (a0)
	vsseg8e32.v v4, (a0)
	vlseg2e64.v v4, (a0)
	vsseg2e64.v v4, (a0)
	vlseg3e64.v v4, (a0)
	vsseg3e64.v v4, (a0)
	vlseg4e64.v v4, (a0)
	vsseg4e64.v v4, (a0)
	vlseg5e64.v v4, (a0)
	vsseg5e64.v v4, (a0)
	vlseg6e64.v v4, (a0)
	vsseg6e64.v v4, (a0)
	vlseg7e64.v v4, (a0)
	vsseg7e64.v v4, (a0)
	vlseg8e64.v v4, (a0)
	vsseg8e64.v v4, (a0)

	vlsseg2e8.v v4, (a0), a1
	vssseg2e8.v v4, (a0), a1
	vlsseg3e8.v v4, (a0), a1
	vssseg3e8.v v4, (a0), a1
	vlsseg4e8.v v4, (a0), a1
	vssseg4e8.v v4, (a0), a1
	vlsseg5e8.v v4, (a0), a1
	vssseg5e8.v v4, (a0), a1
	vlsseg6e8.v v4, (a0), a1
	vssseg6e8.v v4, (a0), a1
	vlsseg7e8.v v4, (a0), a1
	vssseg7e8.v v4, (a0), a1
	vlsseg8e8.v v4, (a0), a1
	vssseg8e8.v v4, (a0), a1
	vlsseg2e16.v v4, (a0), a1
	vssseg2e16.v v4, (a0), a1
	vlsseg3e16.v v4, (a0), a1
	vssseg3e16.v v4, (a0), a1
	vlsseg4e16.v v4, (a0), a1
	vssseg4e16.v v4, (a0), a1
	vlsseg5e16.v v4, (a0), a1
	vssseg5e16.v v4, (a0), a1
	vlsseg6e16.v v4, (a0), a1
	vssseg6e16.v v4, (a0), a1
	vlsseg7e16.v v4, (a0), a1
	vssseg7e16.v v4, (a0), a1
	vlsseg8e16.v v4, (a0), a1
	vssseg8e16.v v4, (a0), a1
	vlsseg2e32.v v4, (a0), a1
	vssseg2e32.v v4, (a0), a1
	vlsseg3e32.v v4, (a0), a1
	vssseg3e32.v v4, (a0), a1
	vlsseg4e32.v v4, (a0), a1
	vssseg4e32.v v4, (a0), a1
	vlsseg5e32.v v4, (a0), a1
	vssseg5e32.v v4, (a0), a1
	vlsseg6e32.v v4, (a0), a1
	vssseg6e32.v v4, (a0), a1
	vlsseg7e32.v v4, (a0), a1
	vssseg7e32.v v4, (a0), a1
	vlsseg8e32.v v4, (a0), a1
	vssseg8e32.v v4, (a0), a1
	vlsseg2e64.v v4, (a0), a1
	vssseg2e64.v v4, (a0), a1
	vlsseg3e64.v v4, (a0), a1
	vssseg3e64.v v4, (a0), a1
	vlsseg4e64.v v4, (a0), a1
	vssseg4e64.v v4, (a0), a1
	vlsseg5e64.v v4, (a0), a1
	vssseg5e64.v v4, (a0), a1
	vlsseg6e64.v v4, (a0), a1
	vssseg6e64.v v4, (a0), a1
	vlsseg7e64.v v4, (a0), a1
	vssseg7e64.v v4, (a0), a1
	vlsseg8e64.v v4, (a0), a1
	vssseg8e64.v v4, (a0), a1

	vloxseg2ei8.v v4, (a0), v12
	vsoxseg2ei8.v v4, (a0), v12
	vloxseg3ei8.v v4, (a0), v12
	vsoxseg3ei8.v v4, (a0), v12
	vloxseg4ei8.v v4, (a0), v12
	vsoxseg4ei8.v v4, (a0), v12
	vloxseg5ei8.v v4, (a0), v12
	vsoxseg5ei8.v v4, (a0), v12
	vloxseg6ei8.v v4, (a0), v12
	vsoxseg6ei8.v v4, (a0), v12
	vloxseg7ei8.v v4, (a0), v12
	vsoxseg7ei8.v v4, (a0), v12
	vloxseg8ei8.v v4, (a0), v12
	vsoxseg8ei8.v v4, (a0), v12
	vloxseg2ei16.v v4, (a0), v12
	vsoxseg2ei16.v v4, (a0), v12
	vloxseg3ei16.v v4, (a0), v12
	vsoxseg3ei16.v v4, (a0), v12
	vloxseg4ei16.v v4, (a0), v12
	vsoxseg4ei16.v v4, (a0), v12
	vloxseg5ei16.v v4, (a0), v12
	vsoxseg5ei16.v v4, (a0), v12
	vloxseg6ei16.v v4, (a0), v12
	vsoxseg6ei16.v v4, (a0), v12
	vloxseg7ei16.v v4, (a0), v12
	vsoxseg7ei16.v v4, (a0), v12
	vloxseg8ei16.v v4, (a0), v12
	vsoxseg8ei16.v v4, (a0), v12
	vloxseg2ei32.v v4, (a0), v12
	vsoxseg2ei32.v v4, (a0), v12
	vloxseg3ei32.v v4, (a0), v12
	vsoxseg3ei32.v v4, (a0), v12
	vloxseg4ei32.v v4, (a0), v12
	vsoxseg4ei32.v v4, (a0), v12
	vloxseg5ei32.v v4, (a0), v12
	vsoxseg5ei32.v v4, (a0), v12
	vloxseg6ei32.v v4, (a0), v12
	vsoxseg6ei32.v v4, (a0), v12
	vloxseg7ei32.v v4, (a0), v12
	vsoxseg7ei32.v v4, (a0), v12
	vloxseg8ei32.v v4, (a0), v12
	vsoxseg8ei32.v v4, (a0), v12
	vloxseg2ei64.v v4, (a0), v12
	vsoxseg2ei64.v v4, (a0), v12
	vloxseg3ei64.v v4, (a0), v12
	vsoxseg3ei64.v v4, (a0), v12
	vloxseg4ei64.v v4, (a0), v12
	vsoxseg4ei64.v v4, (a0), v12
	vloxseg5ei64.v v4, (a0), v12
	vsoxseg5ei64.v v4, (a0), v12
	vloxseg6ei64.v v4, (a0), v12
	vsoxseg6ei64.v v4, (a0), v12
	vloxseg7ei64.v v4, (a0), v12
	vsoxseg7ei64.v v4, (a0), v12
	vloxseg8ei64.v v4, (a0), v12
	vsoxseg8ei64.v v4, (a0), v12

	vluxseg2ei8.v v4, (a0), v12
	vsuxseg2ei8.v v4, (a0), v12
	vluxseg3ei8.v v4, (a0), v12
	vsuxseg3ei8.v v4, (a0), v12
	vluxseg4ei8.v v4, (a0), v12
	vsuxseg4ei8.v v4, (a0), v12
	vluxseg5ei8.v v4, (a0), v12
	vsuxseg5ei8.v v4, (a0), v12
	vluxseg6ei8.v v4, (a0), v12
	vsuxseg6ei8.v v4, (a0), v12
	vluxseg7ei8.v v4, (a0), v12
	vsuxseg7ei8.v v4, (a0), v12
	vluxseg8ei8.v v4, (a0), v12
	vsuxseg8ei8.v v4, (a0), v12
	vluxseg2ei16.v v4, (a0), v12
	vsuxseg2ei16.v v4, (a0), v12
	vluxseg3ei16.v v4, (a0), v12
	vsuxseg3ei16.v v4, (a0), v12
	vluxseg4ei16.v v4, (a0), v12
	vsuxseg4ei16.v v4, (a0), v12
	vluxseg5ei16.v v4, (a0), v12
	vsuxseg5ei16.v v4, (a0), v12
	vluxseg6ei16.v v4, (a0), v12
	vsuxseg6ei16.v v4, (a0), v12
	vluxseg7ei16.v v4, (a0), v12
	vsuxseg7ei16.v v4, (a0), v12
	vluxseg8ei16.v v4, (a0), v12
	vsuxseg8ei16.v v4, (a0), v12
	vluxseg2ei32.v v4, (a0), v12
	vsuxseg2ei32.v v4, (a0), v12
	vluxseg3ei32.v v4, (a0), v12
	vsuxseg3ei32.v v4, (a0), v12
	vluxseg4ei32.v v4, (a0), v12
	vsuxseg4ei32.v v4, (a0), v12
	vluxseg5ei32.v v4, (a0), v12
	vsuxseg5ei32.v v4, (a0), v12
	vluxseg6ei32.v v4, (a0), v12
	vsuxseg6ei32.v v4, (a0), v12
	vluxseg7ei32.v v4, (a0), v12
	vsuxseg7ei32.v v4, (a0), v12
	vluxseg8ei32.v v4, (a0), v12
	vsuxseg8ei32.v v4, (a0), v12
	vluxseg2ei64.v v4, (a0), v12
	vsuxseg2ei64.v v4, (a0), v12
	vluxseg3ei64.v v4, (a0), v12
	vsuxseg3ei64.v v4, (a0), v12
	vluxseg4ei64.v v4, (a0), v12
	vsuxseg4ei64.v v4, (a0), v12
	vluxseg5ei64.v v4, (a0), v12
	vsuxseg5ei64.v v4, (a0), v12
	vluxseg6ei64.v v4, (a0), v12
	vsuxseg6ei64.v v4, (a0), v12
	vluxseg7ei64.v v4, (a0), v12
	vsuxseg7ei64.v v4, (a0), v12
	vluxseg8ei64.v v4, (a0), v12
	vsuxseg8ei64.v v4, (a0), v12

	vlseg2e8ff.v v4, (a0)
	vlseg3e8ff.v v4, (a0)
	vlseg4e8ff.v v4, (a0)
	vlseg5e8ff.v v4, (a0)
	vlseg6e8ff.v v4, (a0)
	vlseg7e8ff.v v4, (a0)
	vlseg8e8ff.v v4, (a0)
	vlseg2e16ff.v v4, (a0)
	vlseg3e16ff.v v4, (a0)
	vlseg4e16ff.v v4, (a0)
	vlseg5e16ff.v v4, (a0)
	vlseg6e16ff.v v4, (a0)
	vlseg7e16ff.v v4, (a0)
	vlseg8e16ff.v v4, (a0)
	vlseg2e32ff.v v4, (a0)
	vlseg3e32ff.v v4, (a0)
	vlseg4e32ff.v v4, (a0)
	vlseg5e32ff.v v4, (a0)
	vlseg6e32ff.v v4, (a0)
	vlseg7e32ff.v v4, (a0)
	vlseg8e32ff.v v4, (a0)
	vlseg2e64ff.v v4, (a0)
	vlseg3e64ff.v v4, (a0)
	vlseg4e64ff.v v4, (a0)
	vlseg5e64ff.v v4, (a0)
	vlseg6e64ff.v v4, (a0)
	vlseg7e64ff.v v4, (a0)
	vlseg8e64ff.v v4, (a0)

	vl1r.v v3, (a0)
	vl1re8.v v3, (a0)
	vl1re16.v v3, (a0)
	vl1re32.v v3, (a0)
	vl1re64.v v3, (a0)
	vl2r.v v2, (a0)
	vl2re8.v v2, (a0)
	vl2re16.v v2, (a0)
	vl2re32.v v2, (a0)
	vl2re64.v v2, (a0)
	vl4r.v v4, (a0)
	vl4re8.v v4, (a0)
	vl4re16.v v4, (a0)
	vl4re32.v v4, (a0)
	vl4re64.v v4, (a0)
	vl8r.v v8, (a0)
	vl8re8.v v8, (a0)
	vl8re16.v v8, (a0)
	vl8re32.v v8, (a0)
	vl8re64.v v8, (a0)

	vs1r.v v3, (a1)
	vs2r.v v2, (a1)
	vs4r.v v4, (a1)
	vs8r.v v8, (a1)

	vamoaddei8.v v4, (a1), v8, v4
	vamoswapei8.v v4, (a1), v8, v4
	vamoxorei8.v v4, (a1), v8, v4
	vamoandei8.v v4, (a1), v8, v4
	vamoorei8.v v4, (a1), v8, v4
	vamominei8.v v4, (a1), v8, v4
	vamomaxei8.v v4, (a1), v8, v4
	vamominuei8.v v4, (a1), v8, v4
	vamomaxuei8.v v4, (a1), v8, v4
	vamoaddei8.v v4, 0(a1), v8, v4
	vamoswapei8.v v4, 0(a1), v8, v4
	vamoxorei8.v v4, 0(a1), v8, v4
	vamoandei8.v v4, 0(a1), v8, v4
	vamoorei8.v v4, 0(a1), v8, v4
	vamominei8.v v4, 0(a1), v8, v4
	vamomaxei8.v v4, 0(a1), v8, v4
	vamominuei8.v v4, 0(a1), v8, v4
	vamomaxuei8.v v4, 0(a1), v8, v4

	vamoaddei16.v v4, (a1), v8, v4
	vamoswapei16.v v4, (a1), v8, v4
	vamoxorei16.v v4, (a1), v8, v4
	vamoandei16.v v4, (a1), v8, v4
	vamoorei16.v v4, (a1), v8, v4
	vamominei16.v v4, (a1), v8, v4
	vamomaxei16.v v4, (a1), v8, v4
	vamominuei16.v v4, (a1), v8, v4
	vamomaxuei16.v v4, (a1), v8, v4
	vamoaddei16.v v4, 0(a1), v8, v4
	vamoswapei16.v v4, 0(a1), v8, v4
	vamoxorei16.v v4, 0(a1), v8, v4
	vamoandei16.v v4, 0(a1), v8, v4
	vamoorei16.v v4, 0(a1), v8, v4
	vamominei16.v v4, 0(a1), v8, v4
	vamomaxei16.v v4, 0(a1), v8, v4
	vamominuei16.v v4, 0(a1), v8, v4
	vamomaxuei16.v v4, 0(a1), v8, v4

	vamoaddei32.v v4, (a1), v8, v4
	vamoswapei32.v v4, (a1), v8, v4
	vamoxorei32.v v4, (a1), v8, v4
	vamoandei32.v v4, (a1), v8, v4
	vamoorei32.v v4, (a1), v8, v4
	vamominei32.v v4, (a1), v8, v4
	vamomaxei32.v v4, (a1), v8, v4
	vamominuei32.v v4, (a1), v8, v4
	vamomaxuei32.v v4, (a1), v8, v4
	vamoaddei32.v v4, 0(a1), v8, v4
	vamoswapei32.v v4, 0(a1), v8, v4
	vamoxorei32.v v4, 0(a1), v8, v4
	vamoandei32.v v4, 0(a1), v8, v4
	vamoorei32.v v4, 0(a1), v8, v4
	vamominei32.v v4, 0(a1), v8, v4
	vamomaxei32.v v4, 0(a1), v8, v4
	vamominuei32.v v4, 0(a1), v8, v4
	vamomaxuei32.v v4, 0(a1), v8, v4

	vamoaddei64.v v4, (a1), v8, v4
	vamoswapei64.v v4, (a1), v8, v4
	vamoxorei64.v v4, (a1), v8, v4
	vamoandei64.v v4, (a1), v8, v4
	vamoorei64.v v4, (a1), v8, v4
	vamominei64.v v4, (a1), v8, v4
	vamomaxei64.v v4, (a1), v8, v4
	vamominuei64.v v4, (a1), v8, v4
	vamomaxuei64.v v4, (a1), v8, v4
	vamoaddei64.v v4, 0(a1), v8, v4
	vamoswapei64.v v4, 0(a1), v8, v4
	vamoxorei64.v v4, 0(a1), v8, v4
	vamoandei64.v v4, 0(a1), v8, v4
	vamoorei64.v v4, 0(a1), v8, v4
	vamominei64.v v4, 0(a1), v8, v4
	vamomaxei64.v v4, 0(a1), v8, v4
	vamominuei64.v v4, 0(a1), v8, v4
	vamomaxuei64.v v4, 0(a1), v8, v4
