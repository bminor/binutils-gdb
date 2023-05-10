#objdump: -sr
#name: Compact EH EL #7 with personality id and fallback FDE
#source: compact-eh-7.s
#as: -march=from-abi -EL -mno-pdr

.*:     file format.*


RELOCATION RECORDS FOR \[.eh_frame\]:
OFFSET +TYPE +VALUE
0+00001c R_MIPS_PC32       .text.*
#?.*R_MIPS_NONE.*
#?.*R_MIPS_NONE.*


RELOCATION RECORDS FOR \[.eh_frame_entry\]:
OFFSET +TYPE +VALUE
0+000000 R_MIPS_PC32       .text.*
#?.*R_MIPS_NONE.*
#?.*R_MIPS_NONE.*
0+000004 R_MIPS_PC32       .eh_frame.*
#?.*R_MIPS_NONE.*
#?.*R_MIPS_NONE.*


Contents of section .text:
 0000 00000000 00000000.*
Contents of section (.reginfo|.MIPS.options):
 0000 0(000|128)0000 00000000 00000000 00000000  .*
 (0010 00000000 00000000                  |0010 00000000 00000000 00000000 00000000)  .*
#? 0020 00000000 00000000                    .*
Contents of section .MIPS.abiflags:
 .*
 .*
Contents of section .eh_frame:
 0000 10000000 00000000 017a5200 017c1f01  .*
 0010 1b0d1d00 1[48]000000 18000000 00000000  .*
 0020 08000000 00441308 440e0000 (00000000|        )  .*
Contents of section .eh_frame_entry:
 0000 01000000 15000000                    .*
Contents of section .gnu.attributes:
 0000 410f0000 00676e75 00010700 00000401  .*
