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
