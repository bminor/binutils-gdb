#source: empty-orphan.s
#ld: -T empty-orphan.t
#readelf: -l --wide
#...
Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg +Align
 +LOAD +[x0-9a-f]+ 0x0+ 0x0+ 0x0+ 0x0+ .*
 +LOAD +[x0-9a-f]+ 0x0+ 0x0+ 0x0+ 0x0+ .*
 +LOAD +[x0-9a-f]+ [x0]+70000000 [x0]+70000000 [x0]+(2|4|8|10|20|40|80) .*
#pass
