#source: reloc-num32.s
#ld: -T $srcdir/$subdir/crx.ld
#objdump: -D

# Test relocation on data R_CRX_NUM32

.*:     file format elf32-crx

Disassembly of section .text:

11014000 <_start>:
11014000:	78 56       	orw	r7, r8
11014002:	34 12       	addcw	\$0x3, r4
