#as: -march=armv8.9-a
#objdump: -dr

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
.*:	d53c9a83 	mrs	x3, pmsdsfr_el1
.*:	d51c9a83 	msr	pmsdsfr_el1, x3
.*:	d5385340 	mrs	x0, erxgsr_el1
.*:	d5181063 	msr	sctlr2_el1, x3
.*:	d51d1063 	msr	sctlr2_el12, x3
.*:	d51c1063 	msr	sctlr2_el2, x3
.*:	d51e1063 	msr	sctlr2_el3, x3
.*:	d5381063 	mrs	x3, sctlr2_el1
.*:	d53d1063 	mrs	x3, sctlr2_el12
.*:	d53c1063 	mrs	x3, sctlr2_el2
.*:	d53e1063 	mrs	x3, sctlr2_el3
.*:	d53c3103 	mrs	x3, hdfgrtr2_el2
.*:	d53c3123 	mrs	x3, hdfgwtr2_el2
.*:	d53c3143 	mrs	x3, hfgrtr2_el2
.*:	d53c3163 	mrs	x3, hfgwtr2_el2
.*:	d51c3103 	msr	hdfgrtr2_el2, x3
.*:	d51c3123 	msr	hdfgwtr2_el2, x3
.*:	d51c3143 	msr	hfgrtr2_el2, x3
.*:	d51c3163 	msr	hfgwtr2_el2, x3
.*:	d53860a0 	mrs	x0, pfar_el1
.*:	d53c60a0 	mrs	x0, pfar_el2
.*:	d53d60a0 	mrs	x0, pfar_el12
.*:	d51860a0 	msr	pfar_el1, x0
.*:	d51c60a0 	msr	pfar_el2, x0
.*:	d51d60a0 	msr	pfar_el12, x0
.*:	d5087941 	at	s1e1a, x1
.*:	d50c7943 	at	s1e2a, x3
.*:	d50e7945 	at	s1e3a, x5
