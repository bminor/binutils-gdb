#ld: -Ttext=0x60
#readelf: -S -l --wide

#...
  \[[ 0-9]+\] \.text[ \t]+PROGBITS[ \t]+0*60[ \t]+.*
#...
Program Headers:
  Type.*
  LOAD[ \t]+0x[0-9a-f]+ 0x[0-9a-f]+ 0x[0-9a-f]+ 0x[0-9a-f]+ 0x[0-9a-f]+ R E 0x[0-9a-f]+

 Section to Segment mapping:
  Segment Sections...
   00     .text *
