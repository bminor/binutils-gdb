#name: Test of system registers for Generic Interrupt Controller version 5
#as: -menable-sysreg-checking -I$srcdir/$subdir -march=armv9.5-a+gcie
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
.*:	d519c000 	msr	icc_apr_el1, x0
.*:	d539c000 	mrs	x0, icc_apr_el1
.*:	d51ec800 	msr	icc_apr_el3, x0
.*:	d53ec800 	mrs	x0, icc_apr_el3
.*:	d519c020 	msr	icc_cr0_el1, x0
.*:	d539c020 	mrs	x0, icc_cr0_el1
.*:	d51ec900 	msr	icc_cr0_el3, x0
.*:	d53ec900 	mrs	x0, icc_cr0_el3
.*:	d518ca80 	msr	icc_icsr_el1, x0
.*:	d538ca80 	mrs	x0, icc_icsr_el1
.*:	d519c040 	msr	icc_pcr_el1, x0
.*:	d539c040 	mrs	x0, icc_pcr_el1
.*:	d51ec820 	msr	icc_pcr_el3, x0
.*:	d53ec820 	mrs	x0, icc_pcr_el3
.*:	d53ec840 	mrs	x0, icc_domhppir_el3
.*:	d539c060 	mrs	x0, icc_hapr_el1
.*:	d538ca60 	mrs	x0, icc_hppir_el1
.*:	d53ec920 	mrs	x0, icc_hppir_el3
.*:	d538caa0 	mrs	x0, icc_iaffidr_el1
.*:	d538ca40 	mrs	x0, icc_idr0_el1
.*:	d5380440 	mrs	x0, id_aa64pfr2_el1
