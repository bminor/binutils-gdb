#readelf: -SW
#name: group section with multiple sections of same name
#source: group1.s

#...
[ 	]*\[.*\][ 	]+\.text[ 	]+PROGBITS.*[ 	]+AX[ 	]+.*
#...
[ 	]*\[.*\][ 	]+\.text[ 	]+PROGBITS.*[ 	]+AXG[ 	]+.*
[ 	]*\[.*\][ 	]+\.foo_group[ 	]+GROUP.*
#pass
