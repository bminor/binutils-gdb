#source: orphan-region.s
#ld: -T orphan-region.ld
#readelf: -S -l --wide

#...
  \[[ 0-9]+\] \.text[ \t]+PROGBITS[ \t]+0*40000000[ \t]+.*
  \[[ 0-9]+\] \.rodata[ \t]+PROGBITS[ \t]+0*400000[0-9a-f]+[ \t]+.*
  \[[ 0-9]+\] \.moredata[ \t]+PROGBITS[ \t]+0*400000[0-9a-f]+[ \t]+.*
#...
Program Headers:
  Type.*
  LOAD[ \t]+0x[0-9a-f]+ 0x0*40000000 0x0*40000000 0x[0-9a-f]+ 0x[0-9a-f]+ R E 0x[0-9a-f]+

 Section to Segment mapping:
  Segment Sections...
   00     .text .rodata .moredata *
