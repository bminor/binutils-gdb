	hfence.vvma
	hfence.vvma	a0
	hfence.vvma	x0, a1
	hfence.vvma	a1, a2
	hfence.gvma
	hfence.gvma	a0
	hfence.gvma	x0, a1
	hfence.gvma	a1, a2

	hlv.b   a0, (a1)
	hlv.b   a0,0(a1)
	hlv.bu  a0, (a1)
	hlv.h   a1, (a2)
	hlv.hu  a1, (a1)
	hlvx.hu a1, (a2)
	hlv.w   a2, (a2)
	hlv.wu  a2, (a3)
	hlvx.wu a2, (a3)
	hlv.d   a3, (a4)
	hsv.b   a0, (a1)
	hsv.b   a0,0(a1)
	hsv.h   a0, (a1)
	hsv.w   a0, (a1)
	hsv.d   a0, (a1)
	csrr    a0, hstatus
	csrw    hstatus, a1
	csrr    a0, hedeleg
	csrw    hedeleg, a1
	csrr    a0, hideleg
	csrw    hideleg, a1
	csrr    a0, hie
	csrw    hie, a1
	csrr    a0, hcounteren
	csrw    hcounteren, a1
	csrr    a0, hgeie
	csrw    hgeie, a1
	csrr    a0, htval
	csrw    htval, a1
	csrr    a0, hip
	csrw    hip, a1
	csrr    a0, hvip
	csrw    hvip, a1
	csrr    a0, htinst
	csrw    htinst, a1
	csrr    a0, hgeip
	csrw    hgeip, a1
	csrr    a0, henvcfg
	csrw    henvcfg, a1
	csrr    a0, hgatp
	csrw    hgatp, a1
	csrr    a0, hcontext
	csrw    hcontext, a1
	csrr    a0, htimedelta
	csrw    htimedelta, a1
	csrr    a0, vsstatus
	csrw    vsstatus, a1
	csrr    a0, vsie
	csrw    vsie, a1
	csrr    a0, vstvec
	csrw    vstvec, a1
	csrr    a0, vsscratch
	csrw    vsscratch, a1
	csrr    a0, vsepc
	csrw    vsepc, a1
	csrr    a0, vscause
	csrw    vscause, a1
	csrr    a0, vstval
	csrw    vstval, a1
	csrr    a0, vsip
	csrw    vsip, a1
	csrr    a0, vsatp
	csrw    vsatp, a1
