	.include "sysreg-test-utils.inc"

.text

	/* CPU Interface registers.  */

	rw_sys_reg icc_apr_el1
	rw_sys_reg icc_apr_el3
	rw_sys_reg icc_cr0_el1
	rw_sys_reg icc_cr0_el3
	rw_sys_reg icc_icsr_el1
	rw_sys_reg icc_pcr_el1
	rw_sys_reg icc_pcr_el3

	rw_sys_reg icc_domhppir_el3 w=0
	rw_sys_reg icc_hapr_el1 w=0
	rw_sys_reg icc_hppir_el1 w=0
	rw_sys_reg icc_hppir_el3 w=0
	rw_sys_reg icc_iaffidr_el1 w=0
	rw_sys_reg icc_idr0_el1 w=0
	rw_sys_reg id_aa64pfr2_el1 w=0

	/* PPI registers.  */

	rw_sys_reg icc_ppi_cactiver0_el1
	rw_sys_reg icc_ppi_cactiver1_el1
	rw_sys_reg icc_ppi_cpendr0_el1
	rw_sys_reg icc_ppi_cpendr1_el1
	rw_sys_reg icc_ppi_domainr0_el3
	rw_sys_reg icc_ppi_domainr1_el3
	rw_sys_reg icc_ppi_domainr2_el3
	rw_sys_reg icc_ppi_domainr3_el3
	rw_sys_reg icc_ppi_enabler0_el1
	rw_sys_reg icc_ppi_enabler1_el1
	rw_sys_reg icc_ppi_hmr0_el1 w=0
	rw_sys_reg icc_ppi_hmr1_el1 w=0
	rw_sys_reg icc_ppi_priorityr0_el1
	rw_sys_reg icc_ppi_priorityr1_el1
	rw_sys_reg icc_ppi_priorityr2_el1
	rw_sys_reg icc_ppi_priorityr3_el1
	rw_sys_reg icc_ppi_priorityr4_el1
	rw_sys_reg icc_ppi_priorityr5_el1
	rw_sys_reg icc_ppi_priorityr6_el1
	rw_sys_reg icc_ppi_priorityr7_el1
	rw_sys_reg icc_ppi_priorityr8_el1
	rw_sys_reg icc_ppi_priorityr9_el1
	rw_sys_reg icc_ppi_priorityr10_el1
	rw_sys_reg icc_ppi_priorityr11_el1
	rw_sys_reg icc_ppi_priorityr12_el1
	rw_sys_reg icc_ppi_priorityr13_el1
	rw_sys_reg icc_ppi_priorityr14_el1
	rw_sys_reg icc_ppi_priorityr15_el1
	rw_sys_reg icc_ppi_sactiver0_el1
	rw_sys_reg icc_ppi_sactiver1_el1
	rw_sys_reg icc_ppi_spendr0_el1
	rw_sys_reg icc_ppi_spendr1_el1
