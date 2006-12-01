#objdump: -sr
# This test is only valid on ELF based ports.
#not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*

.*:     file format.*

RELOCATION RECORDS FOR \[.text\]:
OFFSET   TYPE              VALUE 
00000004 R_ARM_REL32       b

Contents of section .text:
 0000 00000000 (00000004|04000000) 00000000 00000000  .*
# Ignore .ARM.attributes section
#...
