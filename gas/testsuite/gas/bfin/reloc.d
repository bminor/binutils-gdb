#objdump: -r
#name: reloc
.*: +file format .*

RELOCATION RECORDS FOR \[\.text\]:
OFFSET   TYPE              VALUE 
0*0004 R_pcrel24         _call_data1
0*0008 R_rimm16          .data
0*000a R_expst_push      .text\+0x0000001c
0*000a R_expst_const     .__constant\+0x00000004
0*000a R_expst_sub       .__operator
0*000a R_pcrel12_jump_s  .__operator
0*000c R_expst_push      call_data1
0*000c R_expst_const     .__constant\+0x00000008
0*000c R_expst_add       .__operator
0*000e R_pcrel24         .__operator
0*0012 R_huimm16         .data\+0x00000002
0*0016 R_luimm16         .data\+0x00000004
0*001a R_huimm16         load_extern1


RELOCATION RECORDS FOR \[\.data\]:
OFFSET   TYPE              VALUE 
0*0006 R_byte_data       load_extern1


