#source: property-or-1.s
#source: property-or-2.s
#source: property-or-3.s
#as:
#ld: -shared
#readelf: -n
#xfail: ![check_shared_lib_support]

Displaying notes found in: .note.gnu.property
[ 	]+Owner[ 	]+Data size[ 	]+Description
  GNU                  0x[0-9a-f]+	NT_GNU_PROPERTY_TYPE_0
      Properties: UINT32_OR \(0xb000ffff\): 0x1103
