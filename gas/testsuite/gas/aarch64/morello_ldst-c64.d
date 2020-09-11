#as: -march=morello+c64 --defsym C64MODE=1
#objdump: -dr
#source: morello_ldst.s

.*:     file format .*


Disassembly of section \.text:

0000000000000000 <.text>:
.*:	a2e47f02 	casa	c4, c2, \[c24\]
.*:	a2e4ff02 	casal	c4, c2, \[c24\]
.*:	a2a47f02 	cas	c4, c2, \[c24\]
.*:	a2a4ff02 	casl	c4, c2, \[c24\]
.*:	a2e47fe2 	casa	c4, c2, \[csp\]
.*:	a2e4ffe2 	casal	c4, c2, \[csp\]
.*:	a2a47fe2 	cas	c4, c2, \[csp\]
.*:	a2a4ffe2 	casl	c4, c2, \[csp\]
.*:	a2a48302 	swpa	c4, c2, \[c24\]
.*:	a2e48302 	swpal	c4, c2, \[c24\]
.*:	a2248302 	swp	c4, c2, \[c24\]
.*:	a2648302 	swpl	c4, c2, \[c24\]
.*:	a2a483e2 	swpa	c4, c2, \[csp\]
.*:	a2e483e2 	swpal	c4, c2, \[csp\]
.*:	a22483e2 	swp	c4, c2, \[csp\]
.*:	a26483e2 	swpl	c4, c2, \[csp\]
