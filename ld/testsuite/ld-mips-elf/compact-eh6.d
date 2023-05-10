#name: MIPS Compact EH 6
#source: compact-eh3.s
#source: compact-eh3a.s
#as: -EB
#readelf: -x .eh_frame_hdr
#ld: -EB -e main
#

Hex dump of section \'\.eh_frame_hdr\':

  0x[0-9a-f]+ 021b0000 00000005 ffffff[0-9a-f]+ 0000006[08].*
  0x[0-9a-f]+ ffffff[0-9a-f]+ 015d5d01 ffffff[0-9a-f]+ 0000002[9d].*
  0x[0-9a-f]+ ffffff[0-9a-f]+ 0000003[59] ffffff[0-9a-f]+ 015d5d01.*
