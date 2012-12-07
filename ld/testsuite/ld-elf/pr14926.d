#ld: -Ttext=0x60
#readelf: -S --wide

#...
  \[[ 0-9]+\] \.text[ \t]+PROGBITS[ \t]+0*60[ \t]+.*
#pass
