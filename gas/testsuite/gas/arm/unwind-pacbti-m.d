#objdump: -sr
#name: Unwind information for Armv8.1-M.Mainline PACBTI extension
# This test is only valid on ELF based ports.
#notarget: *-*-pe *-*-wince
# VxWorks needs a special variant of this file.
#skip: *-*-vxworks*

.*:     file format.*

RELOCATION RECORDS FOR \[.ARM.exidx\]:
OFFSET   TYPE              VALUE 
00000000 R_ARM_PREL31      .text
00000000 R_ARM_NONE        __aeabi_unwind_cpp_pr0


Contents of section .text:
 0000 (10b54df8 04cd5df8 04cb10bd|b510f84d cd04f85d cb04bd10)  .*
Contents of section .ARM.exidx:
 0000 00000000 (b0a8b480|80b4a8b0)  .*
Contents of section .ARM.attributes:
 0000 41(290000 00|000000 29)616561 62690001 (1f000000|0000001f)  .*
 0010 05382e31 2d4d2e4d 41494e00 0615074d  .*
 0020 09033202 34024a01 4c01               .*
