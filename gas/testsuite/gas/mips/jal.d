#objdump: -dr
#name: jal

# Test the jal macro.

.*: +file format .*mips.*

Disassembly of section .text:
0+0000 <[^>]*> jalr \$t9
...
0+0008 <[^>]*> jalr \$a0,\$t9
...
0+0010 <[^>]*> jal 0+ <text_label>
[ 	]*RELOC: 0+0010 (JMPADDR|R_MIPS_26) .text
...
0+0018 <[^>]*> jal 0+ <text_label>
[ 	]*RELOC: 0+0018 (JMPADDR|R_MIPS_26) external_text_label
...
0+0020 <[^>]*> j 0+ <text_label>
[ 	]*RELOC: 0+0020 (JMPADDR|R_MIPS_26) .text
...
0+0028 <[^>]*> j 0+ <text_label>
[ 	]*RELOC: 0+0028 (JMPADDR|R_MIPS_26) external_text_label
...
