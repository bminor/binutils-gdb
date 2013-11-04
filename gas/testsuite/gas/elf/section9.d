#readelf: -S --wide
#name: section flags

#...
[ 	]*\[.*\][ 	]+\.gnu\.lto_main[ 	]+PROGBITS.*[ 	]+E[   ]+.*
[ 	]*\[.*\][ 	]+\.gnu\.lto_\.pureconst[ 	]+PROGBITS.*[ 	]+E[   ]+.*
[ 	]*\[.*\][ 	]+\.gnu_object_only[ 	]+GNU_OBJECT_ONLY.*[ 	]+E[   ]+.*
#pass
