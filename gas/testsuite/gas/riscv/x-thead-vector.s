	th.vsetvl a0, a1, a2
	th.vsetvli a0, a1, 0
	th.vsetvli a0, a1, 0x7ff

	th.vlb.v v4, (a0)
	th.vlb.v v4, 0(a0)
	th.vlb.v v4, (a0), v0.t
	th.vlh.v v4, (a0)
	th.vlh.v v4, 0(a0)
	th.vlh.v v4, (a0), v0.t
	th.vlw.v v4, (a0)
	th.vlw.v v4, 0(a0)
	th.vlw.v v4, (a0), v0.t
	th.vlbu.v v4, (a0)
	th.vlbu.v v4, 0(a0)
	th.vlbu.v v4, (a0), v0.t
	th.vlhu.v v4, (a0)
	th.vlhu.v v4, 0(a0)
	th.vlhu.v v4, (a0), v0.t
	th.vlwu.v v4, (a0)
	th.vlwu.v v4, 0(a0)
	th.vlwu.v v4, (a0), v0.t
	th.vle.v v4, (a0)
	th.vle.v v4, 0(a0)
	th.vle.v v4, (a0), v0.t
	th.vsb.v v4, (a0)
	th.vsb.v v4, 0(a0)
	th.vsb.v v4, (a0), v0.t
	th.vsh.v v4, (a0)
	th.vsh.v v4, 0(a0)
	th.vsh.v v4, (a0), v0.t
	th.vsw.v v4, (a0)
	th.vsw.v v4, 0(a0)
	th.vsw.v v4, (a0), v0.t
	th.vse.v v4, (a0)
	th.vse.v v4, 0(a0)
	th.vse.v v4, (a0), v0.t

	th.vlsb.v v4, (a0), a1
	th.vlsb.v v4, 0(a0), a1
	th.vlsb.v v4, (a0), a1, v0.t
	th.vlsh.v v4, (a0), a1
	th.vlsh.v v4, 0(a0), a1
	th.vlsh.v v4, (a0), a1, v0.t
	th.vlsw.v v4, (a0), a1
	th.vlsw.v v4, 0(a0), a1
	th.vlsw.v v4, (a0), a1, v0.t
	th.vlsbu.v v4, (a0), a1
	th.vlsbu.v v4, 0(a0), a1
	th.vlsbu.v v4, (a0), a1, v0.t
	th.vlshu.v v4, (a0), a1
	th.vlshu.v v4, 0(a0), a1
	th.vlshu.v v4, (a0), a1, v0.t
	th.vlswu.v v4, (a0), a1
	th.vlswu.v v4, 0(a0), a1
	th.vlswu.v v4, (a0), a1, v0.t
	th.vlse.v v4, (a0), a1
	th.vlse.v v4, 0(a0), a1
	th.vlse.v v4, (a0), a1, v0.t
	th.vssb.v v4, (a0), a1
	th.vssb.v v4, 0(a0), a1
	th.vssb.v v4, (a0), a1, v0.t
	th.vssh.v v4, (a0), a1
	th.vssh.v v4, 0(a0), a1
	th.vssh.v v4, (a0), a1, v0.t
	th.vssw.v v4, (a0), a1
	th.vssw.v v4, 0(a0), a1
	th.vssw.v v4, (a0), a1, v0.t
	th.vsse.v v4, (a0), a1
	th.vsse.v v4, 0(a0), a1
	th.vsse.v v4, (a0), a1, v0.t

	th.vlxb.v v4, (a0), v12
	th.vlxb.v v4, 0(a0), v12
	th.vlxb.v v4, (a0), v12, v0.t
	th.vlxh.v v4, (a0), v12
	th.vlxh.v v4, 0(a0), v12
	th.vlxh.v v4, (a0), v12, v0.t
	th.vlxw.v v4, (a0), v12
	th.vlxw.v v4, 0(a0), v12
	th.vlxw.v v4, (a0), v12, v0.t
	th.vlxbu.v v4, (a0), v12
	th.vlxbu.v v4, 0(a0), v12
	th.vlxbu.v v4, (a0), v12, v0.t
	th.vlxhu.v v4, (a0), v12
	th.vlxhu.v v4, 0(a0), v12
	th.vlxhu.v v4, (a0), v12, v0.t
	th.vlxwu.v v4, (a0), v12
	th.vlxwu.v v4, 0(a0), v12
	th.vlxwu.v v4, (a0), v12, v0.t
	th.vlxe.v v4, (a0), v12
	th.vlxe.v v4, 0(a0), v12
	th.vlxe.v v4, (a0), v12, v0.t
	th.vsxb.v v4, (a0), v12
	th.vsxb.v v4, 0(a0), v12
	th.vsxb.v v4, (a0), v12, v0.t
	th.vsxh.v v4, (a0), v12
	th.vsxh.v v4, 0(a0), v12
	th.vsxh.v v4, (a0), v12, v0.t
	th.vsxw.v v4, (a0), v12
	th.vsxw.v v4, 0(a0), v12
	th.vsxw.v v4, (a0), v12, v0.t
	th.vsxe.v v4, (a0), v12
	th.vsxe.v v4, 0(a0), v12
	th.vsxe.v v4, (a0), v12, v0.t
	th.vsuxb.v v4, (a0), v12
	th.vsuxb.v v4, 0(a0), v12
	th.vsuxb.v v4, (a0), v12, v0.t
	th.vsuxh.v v4, (a0), v12
	th.vsuxh.v v4, 0(a0), v12
	th.vsuxh.v v4, (a0), v12, v0.t
	th.vsuxw.v v4, (a0), v12
	th.vsuxw.v v4, 0(a0), v12
	th.vsuxw.v v4, (a0), v12, v0.t
	th.vsuxe.v v4, (a0), v12
	th.vsuxe.v v4, 0(a0), v12
	th.vsuxe.v v4, (a0), v12, v0.t

	th.vlbff.v v4, (a0)
	th.vlbff.v v4, 0(a0)
	th.vlbff.v v4, (a0), v0.t
	th.vlhff.v v4, (a0)
	th.vlhff.v v4, 0(a0)
	th.vlhff.v v4, (a0), v0.t
	th.vlwff.v v4, (a0)
	th.vlwff.v v4, 0(a0)
	th.vlwff.v v4, (a0), v0.t
	th.vlbuff.v v4, (a0)
	th.vlbuff.v v4, 0(a0)
	th.vlbuff.v v4, (a0), v0.t
	th.vlhuff.v v4, (a0)
	th.vlhuff.v v4, 0(a0)
	th.vlhuff.v v4, (a0), v0.t
	th.vlwuff.v v4, (a0)
	th.vlwuff.v v4, 0(a0)
	th.vlwuff.v v4, (a0), v0.t
	th.vleff.v v4, (a0)
	th.vleff.v v4, 0(a0)
	th.vleff.v v4, (a0), v0.t
