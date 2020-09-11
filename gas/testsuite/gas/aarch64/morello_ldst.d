#as: -march=morello
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

0000000000000000 <.text>:
.*:	a2e47d62 	casa	c4, c2, \[x11\]
.*:	a2e4fd62 	casal	c4, c2, \[x11\]
.*:	a2a47d62 	cas	c4, c2, \[x11\]
.*:	a2a4fd62 	casl	c4, c2, \[x11\]
.*:	a2e47fe2 	casa	c4, c2, \[sp\]
.*:	a2e4ffe2 	casal	c4, c2, \[sp\]
.*:	a2a47fe2 	cas	c4, c2, \[sp\]
.*:	a2a4ffe2 	casl	c4, c2, \[sp\]
.*:	a2a48162 	swpa	c4, c2, \[x11\]
.*:	a2e48162 	swpal	c4, c2, \[x11\]
.*:	a2248162 	swp	c4, c2, \[x11\]
.*:	a2648162 	swpl	c4, c2, \[x11\]
.*:	a2a483e2 	swpa	c4, c2, \[sp\]
.*:	a2e483e2 	swpal	c4, c2, \[sp\]
.*:	a22483e2 	swp	c4, c2, \[sp\]
.*:	a26483e2 	swpl	c4, c2, \[sp\]
