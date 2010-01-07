#name: --gc-sections with note section
#ld: --gc-sections -e _start
#readelf: -S --wide
#target: *-*-linux*

#...
.* .note.ABI-tag[ 	]+NOTE.*
#...
