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
.*:	d518cd00 	msr	icc_ppi_cactiver0_el1, x0
.*:	d538cd00 	mrs	x0, icc_ppi_cactiver0_el1
.*:	d518cd20 	msr	icc_ppi_cactiver1_el1, x0
.*:	d538cd20 	mrs	x0, icc_ppi_cactiver1_el1
.*:	d518cd80 	msr	icc_ppi_cpendr0_el1, x0
.*:	d538cd80 	mrs	x0, icc_ppi_cpendr0_el1
.*:	d518cda0 	msr	icc_ppi_cpendr1_el1, x0
.*:	d538cda0 	mrs	x0, icc_ppi_cpendr1_el1
.*:	d51ec880 	msr	icc_ppi_domainr0_el3, x0
.*:	d53ec880 	mrs	x0, icc_ppi_domainr0_el3
.*:	d51ec8a0 	msr	icc_ppi_domainr1_el3, x0
.*:	d53ec8a0 	mrs	x0, icc_ppi_domainr1_el3
.*:	d51ec8c0 	msr	icc_ppi_domainr2_el3, x0
.*:	d53ec8c0 	mrs	x0, icc_ppi_domainr2_el3
.*:	d51ec8e0 	msr	icc_ppi_domainr3_el3, x0
.*:	d53ec8e0 	mrs	x0, icc_ppi_domainr3_el3
.*:	d518cac0 	msr	icc_ppi_enabler0_el1, x0
.*:	d538cac0 	mrs	x0, icc_ppi_enabler0_el1
.*:	d518cae0 	msr	icc_ppi_enabler1_el1, x0
.*:	d538cae0 	mrs	x0, icc_ppi_enabler1_el1
.*:	d538ca00 	mrs	x0, icc_ppi_hmr0_el1
.*:	d538ca20 	mrs	x0, icc_ppi_hmr1_el1
.*:	d518ce00 	msr	icc_ppi_priorityr0_el1, x0
.*:	d538ce00 	mrs	x0, icc_ppi_priorityr0_el1
.*:	d518ce20 	msr	icc_ppi_priorityr1_el1, x0
.*:	d538ce20 	mrs	x0, icc_ppi_priorityr1_el1
.*:	d518ce40 	msr	icc_ppi_priorityr2_el1, x0
.*:	d538ce40 	mrs	x0, icc_ppi_priorityr2_el1
.*:	d518ce60 	msr	icc_ppi_priorityr3_el1, x0
.*:	d538ce60 	mrs	x0, icc_ppi_priorityr3_el1
.*:	d518ce80 	msr	icc_ppi_priorityr4_el1, x0
.*:	d538ce80 	mrs	x0, icc_ppi_priorityr4_el1
.*:	d518cea0 	msr	icc_ppi_priorityr5_el1, x0
.*:	d538cea0 	mrs	x0, icc_ppi_priorityr5_el1
.*:	d518cec0 	msr	icc_ppi_priorityr6_el1, x0
.*:	d538cec0 	mrs	x0, icc_ppi_priorityr6_el1
.*:	d518cee0 	msr	icc_ppi_priorityr7_el1, x0
.*:	d538cee0 	mrs	x0, icc_ppi_priorityr7_el1
.*:	d518cf00 	msr	icc_ppi_priorityr8_el1, x0
.*:	d538cf00 	mrs	x0, icc_ppi_priorityr8_el1
.*:	d518cf20 	msr	icc_ppi_priorityr9_el1, x0
.*:	d538cf20 	mrs	x0, icc_ppi_priorityr9_el1
.*:	d518cf40 	msr	icc_ppi_priorityr10_el1, x0
.*:	d538cf40 	mrs	x0, icc_ppi_priorityr10_el1
.*:	d518cf60 	msr	icc_ppi_priorityr11_el1, x0
.*:	d538cf60 	mrs	x0, icc_ppi_priorityr11_el1
.*:	d518cf80 	msr	icc_ppi_priorityr12_el1, x0
.*:	d538cf80 	mrs	x0, icc_ppi_priorityr12_el1
.*:	d518cfa0 	msr	icc_ppi_priorityr13_el1, x0
.*:	d538cfa0 	mrs	x0, icc_ppi_priorityr13_el1
.*:	d518cfc0 	msr	icc_ppi_priorityr14_el1, x0
.*:	d538cfc0 	mrs	x0, icc_ppi_priorityr14_el1
.*:	d518cfe0 	msr	icc_ppi_priorityr15_el1, x0
.*:	d538cfe0 	mrs	x0, icc_ppi_priorityr15_el1
.*:	d518cd40 	msr	icc_ppi_sactiver0_el1, x0
.*:	d538cd40 	mrs	x0, icc_ppi_sactiver0_el1
.*:	d518cd60 	msr	icc_ppi_sactiver1_el1, x0
.*:	d538cd60 	mrs	x0, icc_ppi_sactiver1_el1
.*:	d518cdc0 	msr	icc_ppi_spendr0_el1, x0
.*:	d538cdc0 	mrs	x0, icc_ppi_spendr0_el1
.*:	d518cde0 	msr	icc_ppi_spendr1_el1, x0
.*:	d538cde0 	mrs	x0, icc_ppi_spendr1_el1
