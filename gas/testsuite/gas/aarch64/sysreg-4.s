/* sysreg-4.s Test file for ARMv8.5 system registers.  */
func:
	cfp rctx, x1
	dvp rctx, x2
	cpp rctx, x3
	dc cvadp, x4
	mrs x5, rndr
	mrs x6, rndrrs
	mrs x7, scxtnum_el0
	mrs x7, scxtnum_el1
	mrs x7, scxtnum_el2
	mrs x7, scxtnum_el3
	mrs x7, scxtnum_el12
	mrs x8, id_pfr2_el1
