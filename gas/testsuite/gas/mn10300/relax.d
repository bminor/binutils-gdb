#objdump: -r 
#name: Relaxation of conditional branches

.*: +file format.*elf32-[am33lin|mn10300].*

RELOCATION RECORDS FOR \[.rlcb\]:
OFFSET   TYPE              VALUE 
0+8003 R_MN10300_PCREL8  .L0._0\+0x00000001
0+8005 R_MN10300_PCREL32  .L2\+0x00000001


RELOCATION RECORDS FOR \[.rsflb\]:
OFFSET   TYPE              VALUE 
0+8004 R_MN10300_PCREL8  .L0._1\+0x00000002
0+8006 R_MN10300_PCREL32  .L4\+0x00000001


