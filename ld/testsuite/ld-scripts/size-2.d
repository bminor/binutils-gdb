#source: size-2.s
#ld: -T size-2.t
#readelf: -l

#...
Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x[0-9a-f]+ 0x0+0000 0x0+0000
                 0x[0-9a-f]+ 0x[0-9a-f]+  R      .
  LOAD           0x[0-9a-f]+ 0x0+0000 0x0+0000
                 0x0+0030 0x0+0030  R      [0-9a-f]+
  TLS            0x[0-9a-f]+ 0x0+0008 0x0+0008
                 0x0+0014 0x0+002c  R      [0-9a-f]+

 Section to Segment mapping:
  Segment Sections...
   00     \.text \.tdata \.tbss \.map 
   01     \.text \.tdata \.map 
   02     \.tdata \.tbss \.map 
