#as: -march=octeon
#objdump: -M reg-names=numeric -dr
#name: MIPS octeon instructions

.*:     file format .*

Disassembly of section .text:

[0-9a-f]+ <sync_insns>:
.*:	0000008f 	synciobdma
#pass
