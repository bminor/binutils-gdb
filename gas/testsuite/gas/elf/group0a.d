#readelf: -SW
#name: group section
#source: group0.s

#...
[ 	]*\[.*\][ 	]+\.foo[ 	]+PROGBITS.*[ 	]+AXG[ 	]+.*
[ 	]*\[.*\][ 	]+\.bar[ 	]+PROGBITS.*[ 	]+AG[ 	]+.*
[ 	]*\[.*\][ 	]+\.foo_group[ 	]+GROUP.*
#pass
