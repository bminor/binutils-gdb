#name: --gc-sections with note section
#ld: --gc-sections -e _start
#readelf: -S --wide
#target: *-*-linux*
#notarget: *-*-*aout *-*-*oldld

#...
.* .note.ABI-tag[ 	]+NOTE.*
#...
