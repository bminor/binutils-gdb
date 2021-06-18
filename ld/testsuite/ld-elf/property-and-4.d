#source: property-and-1.s
#source: property-and-2.s
#source: property-and-3.s
#as:
#ld: -shared
#readelf: -n
#xfail: ![check_shared_lib_support]

Displaying notes found in: .note.gnu.property
[ 	]+Owner[ 	]+Data size[ 	]+Description
  GNU                  0x[0-9a-f]+	NT_GNU_PROPERTY_TYPE_0
      Properties: UINT32_AND \(0xb0007fff\): 0x1
