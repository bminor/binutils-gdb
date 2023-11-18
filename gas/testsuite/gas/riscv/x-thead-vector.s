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

	th.vlseg2b.v v4, (a0)
	th.vlseg2b.v v4, 0(a0)
	th.vlseg2b.v v4, (a0), v0.t
	th.vlseg2h.v v4, (a0)
	th.vlseg2h.v v4, 0(a0)
	th.vlseg2h.v v4, (a0), v0.t
	th.vlseg2w.v v4, (a0)
	th.vlseg2w.v v4, 0(a0)
	th.vlseg2w.v v4, (a0), v0.t
	th.vlseg2bu.v v4, (a0)
	th.vlseg2bu.v v4, 0(a0)
	th.vlseg2bu.v v4, (a0), v0.t
	th.vlseg2hu.v v4, (a0)
	th.vlseg2hu.v v4, 0(a0)
	th.vlseg2hu.v v4, (a0), v0.t
	th.vlseg2wu.v v4, (a0)
	th.vlseg2wu.v v4, 0(a0)
	th.vlseg2wu.v v4, (a0), v0.t
	th.vlseg2e.v v4, (a0)
	th.vlseg2e.v v4, 0(a0)
	th.vlseg2e.v v4, (a0), v0.t
	th.vsseg2b.v v4, (a0)
	th.vsseg2b.v v4, 0(a0)
	th.vsseg2b.v v4, (a0), v0.t
	th.vsseg2h.v v4, (a0)
	th.vsseg2h.v v4, 0(a0)
	th.vsseg2h.v v4, (a0), v0.t
	th.vsseg2w.v v4, (a0)
	th.vsseg2w.v v4, 0(a0)
	th.vsseg2w.v v4, (a0), v0.t
	th.vsseg2e.v v4, (a0)
	th.vsseg2e.v v4, 0(a0)
	th.vsseg2e.v v4, (a0), v0.t

	th.vlseg3b.v v4, (a0)
	th.vlseg3b.v v4, 0(a0)
	th.vlseg3b.v v4, (a0), v0.t
	th.vlseg3h.v v4, (a0)
	th.vlseg3h.v v4, 0(a0)
	th.vlseg3h.v v4, (a0), v0.t
	th.vlseg3w.v v4, (a0)
	th.vlseg3w.v v4, 0(a0)
	th.vlseg3w.v v4, (a0), v0.t
	th.vlseg3bu.v v4, (a0)
	th.vlseg3bu.v v4, 0(a0)
	th.vlseg3bu.v v4, (a0), v0.t
	th.vlseg3hu.v v4, (a0)
	th.vlseg3hu.v v4, 0(a0)
	th.vlseg3hu.v v4, (a0), v0.t
	th.vlseg3wu.v v4, (a0)
	th.vlseg3wu.v v4, 0(a0)
	th.vlseg3wu.v v4, (a0), v0.t
	th.vlseg3e.v v4, (a0)
	th.vlseg3e.v v4, 0(a0)
	th.vlseg3e.v v4, (a0), v0.t
	th.vsseg3b.v v4, (a0)
	th.vsseg3b.v v4, 0(a0)
	th.vsseg3b.v v4, (a0), v0.t
	th.vsseg3h.v v4, (a0)
	th.vsseg3h.v v4, 0(a0)
	th.vsseg3h.v v4, (a0), v0.t
	th.vsseg3w.v v4, (a0)
	th.vsseg3w.v v4, 0(a0)
	th.vsseg3w.v v4, (a0), v0.t
	th.vsseg3e.v v4, (a0)
	th.vsseg3e.v v4, 0(a0)
	th.vsseg3e.v v4, (a0), v0.t

	th.vlseg4b.v v4, (a0)
	th.vlseg4b.v v4, 0(a0)
	th.vlseg4b.v v4, (a0), v0.t
	th.vlseg4h.v v4, (a0)
	th.vlseg4h.v v4, 0(a0)
	th.vlseg4h.v v4, (a0), v0.t
	th.vlseg4w.v v4, (a0)
	th.vlseg4w.v v4, 0(a0)
	th.vlseg4w.v v4, (a0), v0.t
	th.vlseg4bu.v v4, (a0)
	th.vlseg4bu.v v4, 0(a0)
	th.vlseg4bu.v v4, (a0), v0.t
	th.vlseg4hu.v v4, (a0)
	th.vlseg4hu.v v4, 0(a0)
	th.vlseg4hu.v v4, (a0), v0.t
	th.vlseg4wu.v v4, (a0)
	th.vlseg4wu.v v4, 0(a0)
	th.vlseg4wu.v v4, (a0), v0.t
	th.vlseg4e.v v4, (a0)
	th.vlseg4e.v v4, 0(a0)
	th.vlseg4e.v v4, (a0), v0.t
	th.vsseg4b.v v4, (a0)
	th.vsseg4b.v v4, 0(a0)
	th.vsseg4b.v v4, (a0), v0.t
	th.vsseg4h.v v4, (a0)
	th.vsseg4h.v v4, 0(a0)
	th.vsseg4h.v v4, (a0), v0.t
	th.vsseg4w.v v4, (a0)
	th.vsseg4w.v v4, 0(a0)
	th.vsseg4w.v v4, (a0), v0.t
	th.vsseg4e.v v4, (a0)
	th.vsseg4e.v v4, 0(a0)
	th.vsseg4e.v v4, (a0), v0.t

	th.vlseg5b.v v4, (a0)
	th.vlseg5b.v v4, 0(a0)
	th.vlseg5b.v v4, (a0), v0.t
	th.vlseg5h.v v4, (a0)
	th.vlseg5h.v v4, 0(a0)
	th.vlseg5h.v v4, (a0), v0.t
	th.vlseg5w.v v4, (a0)
	th.vlseg5w.v v4, 0(a0)
	th.vlseg5w.v v4, (a0), v0.t
	th.vlseg5bu.v v4, (a0)
	th.vlseg5bu.v v4, 0(a0)
	th.vlseg5bu.v v4, (a0), v0.t
	th.vlseg5hu.v v4, (a0)
	th.vlseg5hu.v v4, 0(a0)
	th.vlseg5hu.v v4, (a0), v0.t
	th.vlseg5wu.v v4, (a0)
	th.vlseg5wu.v v4, 0(a0)
	th.vlseg5wu.v v4, (a0), v0.t
	th.vlseg5e.v v4, (a0)
	th.vlseg5e.v v4, 0(a0)
	th.vlseg5e.v v4, (a0), v0.t
	th.vsseg5b.v v4, (a0)
	th.vsseg5b.v v4, 0(a0)
	th.vsseg5b.v v4, (a0), v0.t
	th.vsseg5h.v v4, (a0)
	th.vsseg5h.v v4, 0(a0)
	th.vsseg5h.v v4, (a0), v0.t
	th.vsseg5w.v v4, (a0)
	th.vsseg5w.v v4, 0(a0)
	th.vsseg5w.v v4, (a0), v0.t
	th.vsseg5e.v v4, (a0)
	th.vsseg5e.v v4, 0(a0)
	th.vsseg5e.v v4, (a0), v0.t

	th.vlseg6b.v v4, (a0)
	th.vlseg6b.v v4, 0(a0)
	th.vlseg6b.v v4, (a0), v0.t
	th.vlseg6h.v v4, (a0)
	th.vlseg6h.v v4, 0(a0)
	th.vlseg6h.v v4, (a0), v0.t
	th.vlseg6w.v v4, (a0)
	th.vlseg6w.v v4, 0(a0)
	th.vlseg6w.v v4, (a0), v0.t
	th.vlseg6bu.v v4, (a0)
	th.vlseg6bu.v v4, 0(a0)
	th.vlseg6bu.v v4, (a0), v0.t
	th.vlseg6hu.v v4, (a0)
	th.vlseg6hu.v v4, 0(a0)
	th.vlseg6hu.v v4, (a0), v0.t
	th.vlseg6wu.v v4, (a0)
	th.vlseg6wu.v v4, 0(a0)
	th.vlseg6wu.v v4, (a0), v0.t
	th.vlseg6e.v v4, (a0)
	th.vlseg6e.v v4, 0(a0)
	th.vlseg6e.v v4, (a0), v0.t
	th.vsseg6b.v v4, (a0)
	th.vsseg6b.v v4, 0(a0)
	th.vsseg6b.v v4, (a0), v0.t
	th.vsseg6h.v v4, (a0)
	th.vsseg6h.v v4, 0(a0)
	th.vsseg6h.v v4, (a0), v0.t
	th.vsseg6w.v v4, (a0)
	th.vsseg6w.v v4, 0(a0)
	th.vsseg6w.v v4, (a0), v0.t
	th.vsseg6e.v v4, (a0)
	th.vsseg6e.v v4, 0(a0)
	th.vsseg6e.v v4, (a0), v0.t

	th.vlseg7b.v v4, (a0)
	th.vlseg7b.v v4, 0(a0)
	th.vlseg7b.v v4, (a0), v0.t
	th.vlseg7h.v v4, (a0)
	th.vlseg7h.v v4, 0(a0)
	th.vlseg7h.v v4, (a0), v0.t
	th.vlseg7w.v v4, (a0)
	th.vlseg7w.v v4, 0(a0)
	th.vlseg7w.v v4, (a0), v0.t
	th.vlseg7bu.v v4, (a0)
	th.vlseg7bu.v v4, 0(a0)
	th.vlseg7bu.v v4, (a0), v0.t
	th.vlseg7hu.v v4, (a0)
	th.vlseg7hu.v v4, 0(a0)
	th.vlseg7hu.v v4, (a0), v0.t
	th.vlseg7wu.v v4, (a0)
	th.vlseg7wu.v v4, 0(a0)
	th.vlseg7wu.v v4, (a0), v0.t
	th.vlseg7e.v v4, (a0)
	th.vlseg7e.v v4, 0(a0)
	th.vlseg7e.v v4, (a0), v0.t
	th.vsseg7b.v v4, (a0)
	th.vsseg7b.v v4, 0(a0)
	th.vsseg7b.v v4, (a0), v0.t
	th.vsseg7h.v v4, (a0)
	th.vsseg7h.v v4, 0(a0)
	th.vsseg7h.v v4, (a0), v0.t
	th.vsseg7w.v v4, (a0)
	th.vsseg7w.v v4, 0(a0)
	th.vsseg7w.v v4, (a0), v0.t
	th.vsseg7e.v v4, (a0)
	th.vsseg7e.v v4, 0(a0)
	th.vsseg7e.v v4, (a0), v0.t

	th.vlseg8b.v v4, (a0)
	th.vlseg8b.v v4, 0(a0)
	th.vlseg8b.v v4, (a0), v0.t
	th.vlseg8h.v v4, (a0)
	th.vlseg8h.v v4, 0(a0)
	th.vlseg8h.v v4, (a0), v0.t
	th.vlseg8w.v v4, (a0)
	th.vlseg8w.v v4, 0(a0)
	th.vlseg8w.v v4, (a0), v0.t
	th.vlseg8bu.v v4, (a0)
	th.vlseg8bu.v v4, 0(a0)
	th.vlseg8bu.v v4, (a0), v0.t
	th.vlseg8hu.v v4, (a0)
	th.vlseg8hu.v v4, 0(a0)
	th.vlseg8hu.v v4, (a0), v0.t
	th.vlseg8wu.v v4, (a0)
	th.vlseg8wu.v v4, 0(a0)
	th.vlseg8wu.v v4, (a0), v0.t
	th.vlseg8e.v v4, (a0)
	th.vlseg8e.v v4, 0(a0)
	th.vlseg8e.v v4, (a0), v0.t
	th.vsseg8b.v v4, (a0)
	th.vsseg8b.v v4, 0(a0)
	th.vsseg8b.v v4, (a0), v0.t
	th.vsseg8h.v v4, (a0)
	th.vsseg8h.v v4, 0(a0)
	th.vsseg8h.v v4, (a0), v0.t
	th.vsseg8w.v v4, (a0)
	th.vsseg8w.v v4, 0(a0)
	th.vsseg8w.v v4, (a0), v0.t
	th.vsseg8e.v v4, (a0)
	th.vsseg8e.v v4, 0(a0)
	th.vsseg8e.v v4, (a0), v0.t

	th.vlsseg2b.v v4, (a0), a1
	th.vlsseg2b.v v4, 0(a0), a1
	th.vlsseg2b.v v4, (a0), a1, v0.t
	th.vlsseg2h.v v4, (a0), a1
	th.vlsseg2h.v v4, 0(a0), a1
	th.vlsseg2h.v v4, (a0), a1, v0.t
	th.vlsseg2w.v v4, (a0), a1
	th.vlsseg2w.v v4, 0(a0), a1
	th.vlsseg2w.v v4, (a0), a1, v0.t
	th.vlsseg2bu.v v4, (a0), a1
	th.vlsseg2bu.v v4, 0(a0), a1
	th.vlsseg2bu.v v4, (a0), a1, v0.t
	th.vlsseg2hu.v v4, (a0), a1
	th.vlsseg2hu.v v4, 0(a0), a1
	th.vlsseg2hu.v v4, (a0), a1, v0.t
	th.vlsseg2wu.v v4, (a0), a1
	th.vlsseg2wu.v v4, 0(a0), a1
	th.vlsseg2wu.v v4, (a0), a1, v0.t
	th.vlsseg2e.v v4, (a0), a1
	th.vlsseg2e.v v4, 0(a0), a1
	th.vlsseg2e.v v4, (a0), a1, v0.t
	th.vssseg2b.v v4, (a0), a1
	th.vssseg2b.v v4, 0(a0), a1
	th.vssseg2b.v v4, (a0), a1, v0.t
	th.vssseg2h.v v4, (a0), a1
	th.vssseg2h.v v4, 0(a0), a1
	th.vssseg2h.v v4, (a0), a1, v0.t
	th.vssseg2w.v v4, (a0), a1
	th.vssseg2w.v v4, 0(a0), a1
	th.vssseg2w.v v4, (a0), a1, v0.t
	th.vssseg2e.v v4, (a0), a1
	th.vssseg2e.v v4, 0(a0), a1
	th.vssseg2e.v v4, (a0), a1, v0.t

	th.vlsseg3b.v v4, (a0), a1
	th.vlsseg3b.v v4, 0(a0), a1
	th.vlsseg3b.v v4, (a0), a1, v0.t
	th.vlsseg3h.v v4, (a0), a1
	th.vlsseg3h.v v4, 0(a0), a1
	th.vlsseg3h.v v4, (a0), a1, v0.t
	th.vlsseg3w.v v4, (a0), a1
	th.vlsseg3w.v v4, 0(a0), a1
	th.vlsseg3w.v v4, (a0), a1, v0.t
	th.vlsseg3bu.v v4, (a0), a1
	th.vlsseg3bu.v v4, 0(a0), a1
	th.vlsseg3bu.v v4, (a0), a1, v0.t
	th.vlsseg3hu.v v4, (a0), a1
	th.vlsseg3hu.v v4, 0(a0), a1
	th.vlsseg3hu.v v4, (a0), a1, v0.t
	th.vlsseg3wu.v v4, (a0), a1
	th.vlsseg3wu.v v4, 0(a0), a1
	th.vlsseg3wu.v v4, (a0), a1, v0.t
	th.vlsseg3e.v v4, (a0), a1
	th.vlsseg3e.v v4, 0(a0), a1
	th.vlsseg3e.v v4, (a0), a1, v0.t
	th.vssseg3b.v v4, (a0), a1
	th.vssseg3b.v v4, 0(a0), a1
	th.vssseg3b.v v4, (a0), a1, v0.t
	th.vssseg3h.v v4, (a0), a1
	th.vssseg3h.v v4, 0(a0), a1
	th.vssseg3h.v v4, (a0), a1, v0.t
	th.vssseg3w.v v4, (a0), a1
	th.vssseg3w.v v4, 0(a0), a1
	th.vssseg3w.v v4, (a0), a1, v0.t
	th.vssseg3e.v v4, (a0), a1
	th.vssseg3e.v v4, 0(a0), a1
	th.vssseg3e.v v4, (a0), a1, v0.t

	th.vlsseg4b.v v4, (a0), a1
	th.vlsseg4b.v v4, 0(a0), a1
	th.vlsseg4b.v v4, (a0), a1, v0.t
	th.vlsseg4h.v v4, (a0), a1
	th.vlsseg4h.v v4, 0(a0), a1
	th.vlsseg4h.v v4, (a0), a1, v0.t
	th.vlsseg4w.v v4, (a0), a1
	th.vlsseg4w.v v4, 0(a0), a1
	th.vlsseg4w.v v4, (a0), a1, v0.t
	th.vlsseg4bu.v v4, (a0), a1
	th.vlsseg4bu.v v4, 0(a0), a1
	th.vlsseg4bu.v v4, (a0), a1, v0.t
	th.vlsseg4hu.v v4, (a0), a1
	th.vlsseg4hu.v v4, 0(a0), a1
	th.vlsseg4hu.v v4, (a0), a1, v0.t
	th.vlsseg4wu.v v4, (a0), a1
	th.vlsseg4wu.v v4, 0(a0), a1
	th.vlsseg4wu.v v4, (a0), a1, v0.t
	th.vlsseg4e.v v4, (a0), a1
	th.vlsseg4e.v v4, 0(a0), a1
	th.vlsseg4e.v v4, (a0), a1, v0.t
	th.vssseg4b.v v4, (a0), a1
	th.vssseg4b.v v4, 0(a0), a1
	th.vssseg4b.v v4, (a0), a1, v0.t
	th.vssseg4h.v v4, (a0), a1
	th.vssseg4h.v v4, 0(a0), a1
	th.vssseg4h.v v4, (a0), a1, v0.t
	th.vssseg4w.v v4, (a0), a1
	th.vssseg4w.v v4, 0(a0), a1
	th.vssseg4w.v v4, (a0), a1, v0.t
	th.vssseg4e.v v4, (a0), a1
	th.vssseg4e.v v4, 0(a0), a1
	th.vssseg4e.v v4, (a0), a1, v0.t

	th.vlsseg5b.v v4, (a0), a1
	th.vlsseg5b.v v4, 0(a0), a1
	th.vlsseg5b.v v4, (a0), a1, v0.t
	th.vlsseg5h.v v4, (a0), a1
	th.vlsseg5h.v v4, 0(a0), a1
	th.vlsseg5h.v v4, (a0), a1, v0.t
	th.vlsseg5w.v v4, (a0), a1
	th.vlsseg5w.v v4, 0(a0), a1
	th.vlsseg5w.v v4, (a0), a1, v0.t
	th.vlsseg5bu.v v4, (a0), a1
	th.vlsseg5bu.v v4, 0(a0), a1
	th.vlsseg5bu.v v4, (a0), a1, v0.t
	th.vlsseg5hu.v v4, (a0), a1
	th.vlsseg5hu.v v4, 0(a0), a1
	th.vlsseg5hu.v v4, (a0), a1, v0.t
	th.vlsseg5wu.v v4, (a0), a1
	th.vlsseg5wu.v v4, 0(a0), a1
	th.vlsseg5wu.v v4, (a0), a1, v0.t
	th.vlsseg5e.v v4, (a0), a1
	th.vlsseg5e.v v4, 0(a0), a1
	th.vlsseg5e.v v4, (a0), a1, v0.t
	th.vssseg5b.v v4, (a0), a1
	th.vssseg5b.v v4, 0(a0), a1
	th.vssseg5b.v v4, (a0), a1, v0.t
	th.vssseg5h.v v4, (a0), a1
	th.vssseg5h.v v4, 0(a0), a1
	th.vssseg5h.v v4, (a0), a1, v0.t
	th.vssseg5w.v v4, (a0), a1
	th.vssseg5w.v v4, 0(a0), a1
	th.vssseg5w.v v4, (a0), a1, v0.t
	th.vssseg5e.v v4, (a0), a1
	th.vssseg5e.v v4, 0(a0), a1
	th.vssseg5e.v v4, (a0), a1, v0.t

	th.vlsseg6b.v v4, (a0), a1
	th.vlsseg6b.v v4, 0(a0), a1
	th.vlsseg6b.v v4, (a0), a1, v0.t
	th.vlsseg6h.v v4, (a0), a1
	th.vlsseg6h.v v4, 0(a0), a1
	th.vlsseg6h.v v4, (a0), a1, v0.t
	th.vlsseg6w.v v4, (a0), a1
	th.vlsseg6w.v v4, 0(a0), a1
	th.vlsseg6w.v v4, (a0), a1, v0.t
	th.vlsseg6bu.v v4, (a0), a1
	th.vlsseg6bu.v v4, 0(a0), a1
	th.vlsseg6bu.v v4, (a0), a1, v0.t
	th.vlsseg6hu.v v4, (a0), a1
	th.vlsseg6hu.v v4, 0(a0), a1
	th.vlsseg6hu.v v4, (a0), a1, v0.t
	th.vlsseg6wu.v v4, (a0), a1
	th.vlsseg6wu.v v4, 0(a0), a1
	th.vlsseg6wu.v v4, (a0), a1, v0.t
	th.vlsseg6e.v v4, (a0), a1
	th.vlsseg6e.v v4, 0(a0), a1
	th.vlsseg6e.v v4, (a0), a1, v0.t
	th.vssseg6b.v v4, (a0), a1
	th.vssseg6b.v v4, 0(a0), a1
	th.vssseg6b.v v4, (a0), a1, v0.t
	th.vssseg6h.v v4, (a0), a1
	th.vssseg6h.v v4, 0(a0), a1
	th.vssseg6h.v v4, (a0), a1, v0.t
	th.vssseg6w.v v4, (a0), a1
	th.vssseg6w.v v4, 0(a0), a1
	th.vssseg6w.v v4, (a0), a1, v0.t
	th.vssseg6e.v v4, (a0), a1
	th.vssseg6e.v v4, 0(a0), a1
	th.vssseg6e.v v4, (a0), a1, v0.t

	th.vlsseg7b.v v4, (a0), a1
	th.vlsseg7b.v v4, 0(a0), a1
	th.vlsseg7b.v v4, (a0), a1, v0.t
	th.vlsseg7h.v v4, (a0), a1
	th.vlsseg7h.v v4, 0(a0), a1
	th.vlsseg7h.v v4, (a0), a1, v0.t
	th.vlsseg7w.v v4, (a0), a1
	th.vlsseg7w.v v4, 0(a0), a1
	th.vlsseg7w.v v4, (a0), a1, v0.t
	th.vlsseg7bu.v v4, (a0), a1
	th.vlsseg7bu.v v4, 0(a0), a1
	th.vlsseg7bu.v v4, (a0), a1, v0.t
	th.vlsseg7hu.v v4, (a0), a1
	th.vlsseg7hu.v v4, 0(a0), a1
	th.vlsseg7hu.v v4, (a0), a1, v0.t
	th.vlsseg7wu.v v4, (a0), a1
	th.vlsseg7wu.v v4, 0(a0), a1
	th.vlsseg7wu.v v4, (a0), a1, v0.t
	th.vlsseg7e.v v4, (a0), a1
	th.vlsseg7e.v v4, 0(a0), a1
	th.vlsseg7e.v v4, (a0), a1, v0.t
	th.vssseg7b.v v4, (a0), a1
	th.vssseg7b.v v4, 0(a0), a1
	th.vssseg7b.v v4, (a0), a1, v0.t
	th.vssseg7h.v v4, (a0), a1
	th.vssseg7h.v v4, 0(a0), a1
	th.vssseg7h.v v4, (a0), a1, v0.t
	th.vssseg7w.v v4, (a0), a1
	th.vssseg7w.v v4, 0(a0), a1
	th.vssseg7w.v v4, (a0), a1, v0.t
	th.vssseg7e.v v4, (a0), a1
	th.vssseg7e.v v4, 0(a0), a1
	th.vssseg7e.v v4, (a0), a1, v0.t

	th.vlsseg8b.v v4, (a0), a1
	th.vlsseg8b.v v4, 0(a0), a1
	th.vlsseg8b.v v4, (a0), a1, v0.t
	th.vlsseg8h.v v4, (a0), a1
	th.vlsseg8h.v v4, 0(a0), a1
	th.vlsseg8h.v v4, (a0), a1, v0.t
	th.vlsseg8w.v v4, (a0), a1
	th.vlsseg8w.v v4, 0(a0), a1
	th.vlsseg8w.v v4, (a0), a1, v0.t
	th.vlsseg8bu.v v4, (a0), a1
	th.vlsseg8bu.v v4, 0(a0), a1
	th.vlsseg8bu.v v4, (a0), a1, v0.t
	th.vlsseg8hu.v v4, (a0), a1
	th.vlsseg8hu.v v4, 0(a0), a1
	th.vlsseg8hu.v v4, (a0), a1, v0.t
	th.vlsseg8wu.v v4, (a0), a1
	th.vlsseg8wu.v v4, 0(a0), a1
	th.vlsseg8wu.v v4, (a0), a1, v0.t
	th.vlsseg8e.v v4, (a0), a1
	th.vlsseg8e.v v4, 0(a0), a1
	th.vlsseg8e.v v4, (a0), a1, v0.t
	th.vssseg8b.v v4, (a0), a1
	th.vssseg8b.v v4, 0(a0), a1
	th.vssseg8b.v v4, (a0), a1, v0.t
	th.vssseg8h.v v4, (a0), a1
	th.vssseg8h.v v4, 0(a0), a1
	th.vssseg8h.v v4, (a0), a1, v0.t
	th.vssseg8w.v v4, (a0), a1
	th.vssseg8w.v v4, 0(a0), a1
	th.vssseg8w.v v4, (a0), a1, v0.t
	th.vssseg8e.v v4, (a0), a1
	th.vssseg8e.v v4, 0(a0), a1
	th.vssseg8e.v v4, (a0), a1, v0.t

	th.vlxseg2b.v v4, (a0), v12
	th.vlxseg2b.v v4, 0(a0), v12
	th.vlxseg2b.v v4, (a0), v12, v0.t
	th.vlxseg2h.v v4, (a0), v12
	th.vlxseg2h.v v4, 0(a0), v12
	th.vlxseg2h.v v4, (a0), v12, v0.t
	th.vlxseg2w.v v4, (a0), v12
	th.vlxseg2w.v v4, 0(a0), v12
	th.vlxseg2w.v v4, (a0), v12, v0.t
	th.vlxseg2bu.v v4, (a0), v12
	th.vlxseg2bu.v v4, 0(a0), v12
	th.vlxseg2bu.v v4, (a0), v12, v0.t
	th.vlxseg2hu.v v4, (a0), v12
	th.vlxseg2hu.v v4, 0(a0), v12
	th.vlxseg2hu.v v4, (a0), v12, v0.t
	th.vlxseg2wu.v v4, (a0), v12
	th.vlxseg2wu.v v4, 0(a0), v12
	th.vlxseg2wu.v v4, (a0), v12, v0.t
	th.vlxseg2e.v v4, (a0), v12
	th.vlxseg2e.v v4, 0(a0), v12
	th.vlxseg2e.v v4, (a0), v12, v0.t
	th.vsxseg2b.v v4, (a0), v12
	th.vsxseg2b.v v4, 0(a0), v12
	th.vsxseg2b.v v4, (a0), v12, v0.t
	th.vsxseg2h.v v4, (a0), v12
	th.vsxseg2h.v v4, 0(a0), v12
	th.vsxseg2h.v v4, (a0), v12, v0.t
	th.vsxseg2w.v v4, (a0), v12
	th.vsxseg2w.v v4, 0(a0), v12
	th.vsxseg2w.v v4, (a0), v12, v0.t
	th.vsxseg2e.v v4, (a0), v12
	th.vsxseg2e.v v4, 0(a0), v12
	th.vsxseg2e.v v4, (a0), v12, v0.t

	th.vlxseg3b.v v4, (a0), v12
	th.vlxseg3b.v v4, 0(a0), v12
	th.vlxseg3b.v v4, (a0), v12, v0.t
	th.vlxseg3h.v v4, (a0), v12
	th.vlxseg3h.v v4, 0(a0), v12
	th.vlxseg3h.v v4, (a0), v12, v0.t
	th.vlxseg3w.v v4, (a0), v12
	th.vlxseg3w.v v4, 0(a0), v12
	th.vlxseg3w.v v4, (a0), v12, v0.t
	th.vlxseg3bu.v v4, (a0), v12
	th.vlxseg3bu.v v4, 0(a0), v12
	th.vlxseg3bu.v v4, (a0), v12, v0.t
	th.vlxseg3hu.v v4, (a0), v12
	th.vlxseg3hu.v v4, 0(a0), v12
	th.vlxseg3hu.v v4, (a0), v12, v0.t
	th.vlxseg3wu.v v4, (a0), v12
	th.vlxseg3wu.v v4, 0(a0), v12
	th.vlxseg3wu.v v4, (a0), v12, v0.t
	th.vlxseg3e.v v4, (a0), v12
	th.vlxseg3e.v v4, 0(a0), v12
	th.vlxseg3e.v v4, (a0), v12, v0.t
	th.vsxseg3b.v v4, (a0), v12
	th.vsxseg3b.v v4, 0(a0), v12
	th.vsxseg3b.v v4, (a0), v12, v0.t
	th.vsxseg3h.v v4, (a0), v12
	th.vsxseg3h.v v4, 0(a0), v12
	th.vsxseg3h.v v4, (a0), v12, v0.t
	th.vsxseg3w.v v4, (a0), v12
	th.vsxseg3w.v v4, 0(a0), v12
	th.vsxseg3w.v v4, (a0), v12, v0.t
	th.vsxseg3e.v v4, (a0), v12
	th.vsxseg3e.v v4, 0(a0), v12
	th.vsxseg3e.v v4, (a0), v12, v0.t

	th.vlxseg4b.v v4, (a0), v12
	th.vlxseg4b.v v4, 0(a0), v12
	th.vlxseg4b.v v4, (a0), v12, v0.t
	th.vlxseg4h.v v4, (a0), v12
	th.vlxseg4h.v v4, 0(a0), v12
	th.vlxseg4h.v v4, (a0), v12, v0.t
	th.vlxseg4w.v v4, (a0), v12
	th.vlxseg4w.v v4, 0(a0), v12
	th.vlxseg4w.v v4, (a0), v12, v0.t
	th.vlxseg4bu.v v4, (a0), v12
	th.vlxseg4bu.v v4, 0(a0), v12
	th.vlxseg4bu.v v4, (a0), v12, v0.t
	th.vlxseg4hu.v v4, (a0), v12
	th.vlxseg4hu.v v4, 0(a0), v12
	th.vlxseg4hu.v v4, (a0), v12, v0.t
	th.vlxseg4wu.v v4, (a0), v12
	th.vlxseg4wu.v v4, 0(a0), v12
	th.vlxseg4wu.v v4, (a0), v12, v0.t
	th.vlxseg4e.v v4, (a0), v12
	th.vlxseg4e.v v4, 0(a0), v12
	th.vlxseg4e.v v4, (a0), v12, v0.t
	th.vsxseg4b.v v4, (a0), v12
	th.vsxseg4b.v v4, 0(a0), v12
	th.vsxseg4b.v v4, (a0), v12, v0.t
	th.vsxseg4h.v v4, (a0), v12
	th.vsxseg4h.v v4, 0(a0), v12
	th.vsxseg4h.v v4, (a0), v12, v0.t
	th.vsxseg4w.v v4, (a0), v12
	th.vsxseg4w.v v4, 0(a0), v12
	th.vsxseg4w.v v4, (a0), v12, v0.t
	th.vsxseg4e.v v4, (a0), v12
	th.vsxseg4e.v v4, 0(a0), v12
	th.vsxseg4e.v v4, (a0), v12, v0.t

	th.vlxseg5b.v v4, (a0), v12
	th.vlxseg5b.v v4, 0(a0), v12
	th.vlxseg5b.v v4, (a0), v12, v0.t
	th.vlxseg5h.v v4, (a0), v12
	th.vlxseg5h.v v4, 0(a0), v12
	th.vlxseg5h.v v4, (a0), v12, v0.t
	th.vlxseg5w.v v4, (a0), v12
	th.vlxseg5w.v v4, 0(a0), v12
	th.vlxseg5w.v v4, (a0), v12, v0.t
	th.vlxseg5bu.v v4, (a0), v12
	th.vlxseg5bu.v v4, 0(a0), v12
	th.vlxseg5bu.v v4, (a0), v12, v0.t
	th.vlxseg5hu.v v4, (a0), v12
	th.vlxseg5hu.v v4, 0(a0), v12
	th.vlxseg5hu.v v4, (a0), v12, v0.t
	th.vlxseg5wu.v v4, (a0), v12
	th.vlxseg5wu.v v4, 0(a0), v12
	th.vlxseg5wu.v v4, (a0), v12, v0.t
	th.vlxseg5e.v v4, (a0), v12
	th.vlxseg5e.v v4, 0(a0), v12
	th.vlxseg5e.v v4, (a0), v12, v0.t
	th.vsxseg5b.v v4, (a0), v12
	th.vsxseg5b.v v4, 0(a0), v12
	th.vsxseg5b.v v4, (a0), v12, v0.t
	th.vsxseg5h.v v4, (a0), v12
	th.vsxseg5h.v v4, 0(a0), v12
	th.vsxseg5h.v v4, (a0), v12, v0.t
	th.vsxseg5w.v v4, (a0), v12
	th.vsxseg5w.v v4, 0(a0), v12
	th.vsxseg5w.v v4, (a0), v12, v0.t
	th.vsxseg5e.v v4, (a0), v12
	th.vsxseg5e.v v4, 0(a0), v12
	th.vsxseg5e.v v4, (a0), v12, v0.t

	th.vlxseg6b.v v4, (a0), v12
	th.vlxseg6b.v v4, 0(a0), v12
	th.vlxseg6b.v v4, (a0), v12, v0.t
	th.vlxseg6h.v v4, (a0), v12
	th.vlxseg6h.v v4, 0(a0), v12
	th.vlxseg6h.v v4, (a0), v12, v0.t
	th.vlxseg6w.v v4, (a0), v12
	th.vlxseg6w.v v4, 0(a0), v12
	th.vlxseg6w.v v4, (a0), v12, v0.t
	th.vlxseg6bu.v v4, (a0), v12
	th.vlxseg6bu.v v4, 0(a0), v12
	th.vlxseg6bu.v v4, (a0), v12, v0.t
	th.vlxseg6hu.v v4, (a0), v12
	th.vlxseg6hu.v v4, 0(a0), v12
	th.vlxseg6hu.v v4, (a0), v12, v0.t
	th.vlxseg6wu.v v4, (a0), v12
	th.vlxseg6wu.v v4, 0(a0), v12
	th.vlxseg6wu.v v4, (a0), v12, v0.t
	th.vlxseg6e.v v4, (a0), v12
	th.vlxseg6e.v v4, 0(a0), v12
	th.vlxseg6e.v v4, (a0), v12, v0.t
	th.vsxseg6b.v v4, (a0), v12
	th.vsxseg6b.v v4, 0(a0), v12
	th.vsxseg6b.v v4, (a0), v12, v0.t
	th.vsxseg6h.v v4, (a0), v12
	th.vsxseg6h.v v4, 0(a0), v12
	th.vsxseg6h.v v4, (a0), v12, v0.t
	th.vsxseg6w.v v4, (a0), v12
	th.vsxseg6w.v v4, 0(a0), v12
	th.vsxseg6w.v v4, (a0), v12, v0.t
	th.vsxseg6e.v v4, (a0), v12
	th.vsxseg6e.v v4, 0(a0), v12
	th.vsxseg6e.v v4, (a0), v12, v0.t

	th.vlxseg7b.v v4, (a0), v12
	th.vlxseg7b.v v4, 0(a0), v12
	th.vlxseg7b.v v4, (a0), v12, v0.t
	th.vlxseg7h.v v4, (a0), v12
	th.vlxseg7h.v v4, 0(a0), v12
	th.vlxseg7h.v v4, (a0), v12, v0.t
	th.vlxseg7w.v v4, (a0), v12
	th.vlxseg7w.v v4, 0(a0), v12
	th.vlxseg7w.v v4, (a0), v12, v0.t
	th.vlxseg7bu.v v4, (a0), v12
	th.vlxseg7bu.v v4, 0(a0), v12
	th.vlxseg7bu.v v4, (a0), v12, v0.t
	th.vlxseg7hu.v v4, (a0), v12
	th.vlxseg7hu.v v4, 0(a0), v12
	th.vlxseg7hu.v v4, (a0), v12, v0.t
	th.vlxseg7wu.v v4, (a0), v12
	th.vlxseg7wu.v v4, 0(a0), v12
	th.vlxseg7wu.v v4, (a0), v12, v0.t
	th.vlxseg7e.v v4, (a0), v12
	th.vlxseg7e.v v4, 0(a0), v12
	th.vlxseg7e.v v4, (a0), v12, v0.t
	th.vsxseg7b.v v4, (a0), v12
	th.vsxseg7b.v v4, 0(a0), v12
	th.vsxseg7b.v v4, (a0), v12, v0.t
	th.vsxseg7h.v v4, (a0), v12
	th.vsxseg7h.v v4, 0(a0), v12
	th.vsxseg7h.v v4, (a0), v12, v0.t
	th.vsxseg7w.v v4, (a0), v12
	th.vsxseg7w.v v4, 0(a0), v12
	th.vsxseg7w.v v4, (a0), v12, v0.t
	th.vsxseg7e.v v4, (a0), v12
	th.vsxseg7e.v v4, 0(a0), v12
	th.vsxseg7e.v v4, (a0), v12, v0.t

	th.vlxseg8b.v v4, (a0), v12
	th.vlxseg8b.v v4, 0(a0), v12
	th.vlxseg8b.v v4, (a0), v12, v0.t
	th.vlxseg8h.v v4, (a0), v12
	th.vlxseg8h.v v4, 0(a0), v12
	th.vlxseg8h.v v4, (a0), v12, v0.t
	th.vlxseg8w.v v4, (a0), v12
	th.vlxseg8w.v v4, 0(a0), v12
	th.vlxseg8w.v v4, (a0), v12, v0.t
	th.vlxseg8bu.v v4, (a0), v12
	th.vlxseg8bu.v v4, 0(a0), v12
	th.vlxseg8bu.v v4, (a0), v12, v0.t
	th.vlxseg8hu.v v4, (a0), v12
	th.vlxseg8hu.v v4, 0(a0), v12
	th.vlxseg8hu.v v4, (a0), v12, v0.t
	th.vlxseg8wu.v v4, (a0), v12
	th.vlxseg8wu.v v4, 0(a0), v12
	th.vlxseg8wu.v v4, (a0), v12, v0.t
	th.vlxseg8e.v v4, (a0), v12
	th.vlxseg8e.v v4, 0(a0), v12
	th.vlxseg8e.v v4, (a0), v12, v0.t
	th.vsxseg8b.v v4, (a0), v12
	th.vsxseg8b.v v4, 0(a0), v12
	th.vsxseg8b.v v4, (a0), v12, v0.t
	th.vsxseg8h.v v4, (a0), v12
	th.vsxseg8h.v v4, 0(a0), v12
	th.vsxseg8h.v v4, (a0), v12, v0.t
	th.vsxseg8w.v v4, (a0), v12
	th.vsxseg8w.v v4, 0(a0), v12
	th.vsxseg8w.v v4, (a0), v12, v0.t
	th.vsxseg8e.v v4, (a0), v12
	th.vsxseg8e.v v4, 0(a0), v12
	th.vsxseg8e.v v4, (a0), v12, v0.t

	th.vlseg2bff.v v4, (a0)
	th.vlseg2bff.v v4, 0(a0)
	th.vlseg2bff.v v4, (a0), v0.t
	th.vlseg2hff.v v4, (a0)
	th.vlseg2hff.v v4, 0(a0)
	th.vlseg2hff.v v4, (a0), v0.t
	th.vlseg2wff.v v4, (a0)
	th.vlseg2wff.v v4, 0(a0)
	th.vlseg2wff.v v4, (a0), v0.t
	th.vlseg2buff.v v4, (a0)
	th.vlseg2buff.v v4, 0(a0)
	th.vlseg2buff.v v4, (a0), v0.t
	th.vlseg2huff.v v4, (a0)
	th.vlseg2huff.v v4, 0(a0)
	th.vlseg2huff.v v4, (a0), v0.t
	th.vlseg2wuff.v v4, (a0)
	th.vlseg2wuff.v v4, 0(a0)
	th.vlseg2wuff.v v4, (a0), v0.t
	th.vlseg2eff.v v4, (a0)
	th.vlseg2eff.v v4, 0(a0)
	th.vlseg2eff.v v4, (a0), v0.t

	th.vlseg3bff.v v4, (a0)
	th.vlseg3bff.v v4, 0(a0)
	th.vlseg3bff.v v4, (a0), v0.t
	th.vlseg3hff.v v4, (a0)
	th.vlseg3hff.v v4, 0(a0)
	th.vlseg3hff.v v4, (a0), v0.t
	th.vlseg3wff.v v4, (a0)
	th.vlseg3wff.v v4, 0(a0)
	th.vlseg3wff.v v4, (a0), v0.t
	th.vlseg3buff.v v4, (a0)
	th.vlseg3buff.v v4, 0(a0)
	th.vlseg3buff.v v4, (a0), v0.t
	th.vlseg3huff.v v4, (a0)
	th.vlseg3huff.v v4, 0(a0)
	th.vlseg3huff.v v4, (a0), v0.t
	th.vlseg3wuff.v v4, (a0)
	th.vlseg3wuff.v v4, 0(a0)
	th.vlseg3wuff.v v4, (a0), v0.t
	th.vlseg3eff.v v4, (a0)
	th.vlseg3eff.v v4, 0(a0)
	th.vlseg3eff.v v4, (a0), v0.t

	th.vlseg4bff.v v4, (a0)
	th.vlseg4bff.v v4, 0(a0)
	th.vlseg4bff.v v4, (a0), v0.t
	th.vlseg4hff.v v4, (a0)
	th.vlseg4hff.v v4, 0(a0)
	th.vlseg4hff.v v4, (a0), v0.t
	th.vlseg4wff.v v4, (a0)
	th.vlseg4wff.v v4, 0(a0)
	th.vlseg4wff.v v4, (a0), v0.t
	th.vlseg4buff.v v4, (a0)
	th.vlseg4buff.v v4, 0(a0)
	th.vlseg4buff.v v4, (a0), v0.t
	th.vlseg4huff.v v4, (a0)
	th.vlseg4huff.v v4, 0(a0)
	th.vlseg4huff.v v4, (a0), v0.t
	th.vlseg4wuff.v v4, (a0)
	th.vlseg4wuff.v v4, 0(a0)
	th.vlseg4wuff.v v4, (a0), v0.t
	th.vlseg4eff.v v4, (a0)
	th.vlseg4eff.v v4, 0(a0)
	th.vlseg4eff.v v4, (a0), v0.t

	th.vlseg5bff.v v4, (a0)
	th.vlseg5bff.v v4, 0(a0)
	th.vlseg5bff.v v4, (a0), v0.t
	th.vlseg5hff.v v4, (a0)
	th.vlseg5hff.v v4, 0(a0)
	th.vlseg5hff.v v4, (a0), v0.t
	th.vlseg5wff.v v4, (a0)
	th.vlseg5wff.v v4, 0(a0)
	th.vlseg5wff.v v4, (a0), v0.t
	th.vlseg5buff.v v4, (a0)
	th.vlseg5buff.v v4, 0(a0)
	th.vlseg5buff.v v4, (a0), v0.t
	th.vlseg5huff.v v4, (a0)
	th.vlseg5huff.v v4, 0(a0)
	th.vlseg5huff.v v4, (a0), v0.t
	th.vlseg5wuff.v v4, (a0)
	th.vlseg5wuff.v v4, 0(a0)
	th.vlseg5wuff.v v4, (a0), v0.t
	th.vlseg5eff.v v4, (a0)
	th.vlseg5eff.v v4, 0(a0)
	th.vlseg5eff.v v4, (a0), v0.t

	th.vlseg6bff.v v4, (a0)
	th.vlseg6bff.v v4, 0(a0)
	th.vlseg6bff.v v4, (a0), v0.t
	th.vlseg6hff.v v4, (a0)
	th.vlseg6hff.v v4, 0(a0)
	th.vlseg6hff.v v4, (a0), v0.t
	th.vlseg6wff.v v4, (a0)
	th.vlseg6wff.v v4, 0(a0)
	th.vlseg6wff.v v4, (a0), v0.t
	th.vlseg6buff.v v4, (a0)
	th.vlseg6buff.v v4, 0(a0)
	th.vlseg6buff.v v4, (a0), v0.t
	th.vlseg6huff.v v4, (a0)
	th.vlseg6huff.v v4, 0(a0)
	th.vlseg6huff.v v4, (a0), v0.t
	th.vlseg6wuff.v v4, (a0)
	th.vlseg6wuff.v v4, 0(a0)
	th.vlseg6wuff.v v4, (a0), v0.t
	th.vlseg6eff.v v4, (a0)
	th.vlseg6eff.v v4, 0(a0)
	th.vlseg6eff.v v4, (a0), v0.t

	th.vlseg7bff.v v4, (a0)
	th.vlseg7bff.v v4, 0(a0)
	th.vlseg7bff.v v4, (a0), v0.t
	th.vlseg7hff.v v4, (a0)
	th.vlseg7hff.v v4, 0(a0)
	th.vlseg7hff.v v4, (a0), v0.t
	th.vlseg7wff.v v4, (a0)
	th.vlseg7wff.v v4, 0(a0)
	th.vlseg7wff.v v4, (a0), v0.t
	th.vlseg7buff.v v4, (a0)
	th.vlseg7buff.v v4, 0(a0)
	th.vlseg7buff.v v4, (a0), v0.t
	th.vlseg7huff.v v4, (a0)
	th.vlseg7huff.v v4, 0(a0)
	th.vlseg7huff.v v4, (a0), v0.t
	th.vlseg7wuff.v v4, (a0)
	th.vlseg7wuff.v v4, 0(a0)
	th.vlseg7wuff.v v4, (a0), v0.t
	th.vlseg7eff.v v4, (a0)
	th.vlseg7eff.v v4, 0(a0)
	th.vlseg7eff.v v4, (a0), v0.t

	th.vlseg8bff.v v4, (a0)
	th.vlseg8bff.v v4, 0(a0)
	th.vlseg8bff.v v4, (a0), v0.t
	th.vlseg8hff.v v4, (a0)
	th.vlseg8hff.v v4, 0(a0)
	th.vlseg8hff.v v4, (a0), v0.t
	th.vlseg8wff.v v4, (a0)
	th.vlseg8wff.v v4, 0(a0)
	th.vlseg8wff.v v4, (a0), v0.t
	th.vlseg8buff.v v4, (a0)
	th.vlseg8buff.v v4, 0(a0)
	th.vlseg8buff.v v4, (a0), v0.t
	th.vlseg8huff.v v4, (a0)
	th.vlseg8huff.v v4, 0(a0)
	th.vlseg8huff.v v4, (a0), v0.t
	th.vlseg8wuff.v v4, (a0)
	th.vlseg8wuff.v v4, 0(a0)
	th.vlseg8wuff.v v4, (a0), v0.t
	th.vlseg8eff.v v4, (a0)
	th.vlseg8eff.v v4, 0(a0)
	th.vlseg8eff.v v4, (a0), v0.t
