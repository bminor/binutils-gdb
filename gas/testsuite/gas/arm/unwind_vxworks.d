#objdump: -sr
#name: Unwind table generation
# This test is only valid on ELF based ports.
#not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*
# This is the VxWorks variant of this file.
#source: unwind.s
#not-skip: *-*-vxworks*

.*:     file format.*

RELOCATION RECORDS FOR \[.ARM.extab\]:
OFFSET   TYPE              VALUE 
0000000c R_ARM_PREL31      .text\+0x0+c


RELOCATION RECORDS FOR \[.ARM.exidx\]:
OFFSET   TYPE              VALUE 
00000000 R_ARM_PREL31      .text
00000000 R_ARM_NONE        __aeabi_unwind_cpp_pr0
00000008 R_ARM_PREL31      .text.*
00000008 R_ARM_NONE        __aeabi_unwind_cpp_pr1\+0x0+8
0000000c R_ARM_PREL31      .ARM.extab\+0x0+c
00000010 R_ARM_PREL31      .text.*
00000014 R_ARM_PREL31      .ARM.extab.*
00000018 R_ARM_PREL31      .text.*
0000001c R_ARM_PREL31      .ARM.extab.*
00000020 R_ARM_PREL31      .text.*


Contents of section .text:
 0000 (0000a0e3 0100a0e3 0200a0e3 0300a0e3|e3a00000 e3a00001 e3a00002 e3a00003)  .*
 0010 (0420|2004)                                 .*
Contents of section .ARM.extab:
 0000 (449b0181 b0b08086|81019b44 8680b0b0) 00000000 00000000  .*
 0010 (8402b101 b0b0b005 2a000000 00c60181|01b10284 05b0b0b0 0000002a 8101c600)  .*
 0020 (b0b0c1c1|c1c1b0b0) 00000000                    .*
Contents of section .ARM.exidx:
 0000 00000000 (b0b0a880 00000000|80a8b0b0 00000000) 00000000  .*
 0010 00000000 00000000 00000000 00000000  .*
 0020 (00000000 08849780|00000000 80978408)                    .*
