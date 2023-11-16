	mrs x3, PMSDSFR_EL1
	msr PMSDSFR_EL1, x3

	mrs x0, ERXGSR_EL1

	msr SCTLR2_EL1, x3
	msr SCTLR2_EL12, x3
	msr SCTLR2_EL2, x3
	msr SCTLR2_EL3, x3
	mrs x3, SCTLR2_EL1
	mrs x3, SCTLR2_EL12
	mrs x3, SCTLR2_EL2
	mrs x3, SCTLR2_EL3

	mrs x3, HDFGRTR2_EL2
	mrs x3, HDFGWTR2_EL2
	mrs x3, HFGRTR2_EL2
	mrs x3, HFGWTR2_EL2
	msr HDFGRTR2_EL2, x3
	msr HDFGWTR2_EL2, x3
	msr HFGRTR2_EL2, x3
	msr HFGWTR2_EL2, x3

	mrs x0, PFAR_EL1
	mrs x0, PFAR_EL2
	mrs x0, PFAR_EL12
	msr PFAR_EL1, x0
	msr PFAR_EL2, x0
	msr PFAR_EL12, x0

	/* AT.  */
	at s1e1a, x1
	at s1e2a, x3
	at s1e3a, x5

	/* FEAT_AIE.  */
	mrs x0, amair2_el1
	mrs x0, amair2_el12
	mrs x0, amair2_el2
	mrs x0, amair2_el3
	mrs x0, mair2_el1
	mrs x0, mair2_el12
	mrs x0, mair2_el2
	mrs x0, mair2_el3

	msr amair2_el1, x0
	msr amair2_el12, x0
	msr amair2_el2, x0
	msr amair2_el3, x0
	msr mair2_el1, x0
	msr mair2_el12, x0
	msr mair2_el2, x0
	msr mair2_el3, x0

	/* FEAT_S1PIE.  */
	mrs x0, pir_el1
	mrs x0, pir_el12
	mrs x0, pir_el2
	mrs x0, pir_el3
	mrs x0, pire0_el1
	mrs x0, pire0_el12
	mrs x0, pire0_el2

	msr pir_el1, x0
	msr pir_el12, x0
	msr pir_el2, x0
	msr pir_el3, x0
	msr pire0_el1, x0
	msr pire0_el12, x0
	msr pire0_el2, x0

	/* FEAT_S2PIE.  */
	mrs x0, s2pir_el2
	msr s2pir_el2, x0

	/* FEAT_S1POE.  */
	mrs x0, por_el0
	mrs x0, por_el1
	mrs x0, por_el12
	mrs x0, por_el2
	mrs x0, por_el3

	msr por_el0, x0
	msr por_el1, x0
	msr por_el12, x0
	msr por_el2, x0
	msr por_el3, x0

	/* FEAT_S21POE.  */
	mrs x0, s2por_el1
	msr s2por_el1, x0

	/* FEAT_TCR2.  */
	mrs x0, tcr2_el1
	mrs x0, tcr2_el12
	mrs x0, tcr2_el2

	msr tcr2_el1, x0
	msr tcr2_el12, x0
	msr tcr2_el2, x0
