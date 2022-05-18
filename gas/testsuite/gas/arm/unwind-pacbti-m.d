#objdump: -sr
#name: Unwind information for Armv8.1-M.Mainline PACBTI extension
# This test is only valid on ELF based ports.
#notarget: *-*-pe *-*-wince
# VxWorks needs a special variant of this file.
#skip: *-*-vxworks*

.*:     file format.*

RELOCATION RECORDS FOR \[.ARM.exidx\]:
OFFSET   TYPE              VALUE.
00000000 R_ARM_PREL31      .text
00000000 R_ARM_NONE        __aeabi_unwind_cpp_pr1
00000004 R_ARM_PREL31      .ARM.extab


Contents of section .text:
 0000 (10b54df8 04cd5df8 04cb2de9 f050bde8|e8bd50f0 e92dcb04 f85dcd04 f84db510)  .*
 0010 (f0502de9 0050bde8 005010bd|bd105000 e8bd5000 e92d50f0)  .*
Contents of section .ARM.extab:
 0000 (00840281 b40084b4 b0a8b4a3|a3b4a8b0 b48400b4 81028400) 00000000  .*
Contents of section .ARM.exidx:
 0000 00000000 00000000  .*
Contents of section .ARM.attributes:
 0000 41(290000 00|000000 29)616561 62690001 (1f000000|0000001f)  .*
 0010 05382e31 2d4d2e4d 41494e00 0615074d  .*
 0020 09033202 34024a01 4c01  .*
