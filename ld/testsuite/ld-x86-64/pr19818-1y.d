#source: pr19818-1a.s
#as: --64
#ld: -pie -melf_x86_64 --defsym foo=0x100000000
#error: .*relocation truncated to fit: R_X86_64_32 .*
