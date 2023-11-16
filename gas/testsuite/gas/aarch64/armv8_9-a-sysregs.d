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
.*:	d538a320 	mrs	x0, amair2_el1
.*:	d53da320 	mrs	x0, amair2_el12
.*:	d53ca320 	mrs	x0, amair2_el2
.*:	d53ea320 	mrs	x0, amair2_el3
.*:	d538a220 	mrs	x0, mair2_el1
.*:	d53da220 	mrs	x0, mair2_el12
.*:	d53ca120 	mrs	x0, mair2_el2
.*:	d53ea120 	mrs	x0, mair2_el3
.*:	d518a320 	msr	amair2_el1, x0
.*:	d51da320 	msr	amair2_el12, x0
.*:	d51ca320 	msr	amair2_el2, x0
.*:	d51ea320 	msr	amair2_el3, x0
.*:	d518a220 	msr	mair2_el1, x0
.*:	d51da220 	msr	mair2_el12, x0
.*:	d51ca120 	msr	mair2_el2, x0
.*:	d51ea120 	msr	mair2_el3, x0
.*:	d538a260 	mrs	x0, pir_el1
.*:	d53da260 	mrs	x0, pir_el12
.*:	d53ca260 	mrs	x0, pir_el2
.*:	d53ea260 	mrs	x0, pir_el3
.*:	d538a240 	mrs	x0, pire0_el1
.*:	d53da240 	mrs	x0, pire0_el12
.*:	d53ca240 	mrs	x0, pire0_el2
.*:	d518a260 	msr	pir_el1, x0
.*:	d51da260 	msr	pir_el12, x0
.*:	d51ca260 	msr	pir_el2, x0
.*:	d51ea260 	msr	pir_el3, x0
.*:	d518a240 	msr	pire0_el1, x0
.*:	d51da240 	msr	pire0_el12, x0
.*:	d51ca240 	msr	pire0_el2, x0
.*:	d53ca2a0 	mrs	x0, s2pir_el2
.*:	d51ca2a0 	msr	s2pir_el2, x0
.*:	d53ba280 	mrs	x0, por_el0
.*:	d538a280 	mrs	x0, por_el1
.*:	d53da280 	mrs	x0, por_el12
.*:	d53ca280 	mrs	x0, por_el2
.*:	d53ea280 	mrs	x0, por_el3
.*:	d51ba280 	msr	por_el0, x0
.*:	d518a280 	msr	por_el1, x0
.*:	d51da280 	msr	por_el12, x0
.*:	d51ca280 	msr	por_el2, x0
.*:	d51ea280 	msr	por_el3, x0
.*:	d538a2a0 	mrs	x0, s2por_el1
.*:	d518a2a0 	msr	s2por_el1, x0
.*:	d5382060 	mrs	x0, tcr2_el1
.*:	d53d2060 	mrs	x0, tcr2_el12
.*:	d53c2060 	mrs	x0, tcr2_el2
.*:	d5182060 	msr	tcr2_el1, x0
.*:	d51d2060 	msr	tcr2_el12, x0
.*:	d51c2060 	msr	tcr2_el2, x0
