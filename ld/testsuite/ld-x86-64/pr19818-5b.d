#source: pr19818-5.s
#as: --x32
#ld: -pie -melf32_x86_64
#error: .*relocation R_X86_64_32S against symbol `.data' can not be used when making a shared object; recompile with -fPIC
