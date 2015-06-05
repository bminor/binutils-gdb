#source: ppc476-shared.s
#as: -a32
#ld: -melf32ppc -shared --ppc476-workaround -T ppc476-shared.lnk
#objdump: -R
#target: powerpc*-*-*

.*:     file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0001000[02] R_PPC_ADDR16_LO   \.text\+0x000200f4
0002000[02] R_PPC_ADDR16_HA   \.text\+0x000200f4
