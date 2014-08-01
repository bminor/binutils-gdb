#source: jr.s -mips32r2 -32 -mfp32 -EB RUN_OBJCOPY
#objcopy_objects: -R .MIPS.abiflags
#ld: -melf32btsmip -e 0
#objdump: -p

[^:]*:     file format elf32-tradbigmips

Program Header:
#...
!0x70000003.*
#...
private flags = 70001000: \[abi=O32\] \[mips32r2\] \[not 32bitmode\]

