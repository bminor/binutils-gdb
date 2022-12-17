#name: non-contiguous-arm7
#source: non-contiguous-arm7.s
#ld: --enable-non-contiguous-regions -T non-contiguous-arm7.ld
#error: \A.*unresolvable R_ARM_ABS32 relocation against symbol .MY_BUF..*\Z
