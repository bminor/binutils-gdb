#PROG: objcopy
#readelf: --notes --wide
#objcopy: --merge-notes
#name: v2 gnu build attribute notes (64-bit)
#source: note-3-64.s

#...
Displaying notes found in: .gnu.build.attributes
[ 	]+Owner[ 	]+Data size[ 	]+Description
[ 	]+GA\$<version>2p1[ 	]+0x0000000.[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100 \(file: note_1.s\)
[ 	]+GA\$<tool>gcc 6.3.1 20161221[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\*GOW:0x700[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\*<stack prot>off[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\*FORTIFY:0xff[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\*<PIC>PIC[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\!<short enum>false[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
[ 	]+GA\*<ABI>0x[0-9a-f]+[ 	]+0x00000000[ 	]+NT_GNU_BUILD_ATTRIBUTE_OPEN[ 	]+Applies from offset 0x100
#...
