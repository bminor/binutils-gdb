/* sysreg-2.s Test file for ARMv8.2 system registers.  */

	.macro rw_sys_reg sys_reg xreg r w
	.ifc \w, 1
	msr \sys_reg, \xreg
	.endif
	.ifc \r, 1
	mrs \xreg, \sys_reg
	.endif
	.endm

	.text

	rw_sys_reg sys_reg=id_aa64mmfr1_el1 xreg=x5 r=1 w=0
	rw_sys_reg sys_reg=id_aa64mmfr2_el1 xreg=x7 r=1 w=0
