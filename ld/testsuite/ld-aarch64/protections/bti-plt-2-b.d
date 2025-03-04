#name: No '-z force-bti' and all input objects with BTI feature emits BTI feature (shared)
#source: bti-plt-1.s
#source: bti-plt-2.s
#target: [check_shared_lib_support]
#as: -mabi=lp64 -defsym __property_bti__=1
#ld: -shared -T bti-plt.ld
#readelf: -n

Displaying notes found in: .note.gnu.property
[ 	]+Owner[ 	]+Data size[ 	]+Description
  GNU                  0x00000010	NT_GNU_PROPERTY_TYPE_0
      Properties: AArch64 feature: BTI
