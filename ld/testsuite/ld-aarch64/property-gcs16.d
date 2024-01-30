#name: GNU Property (input with gcs ouput forced with experimental-gcs=always experimental-gcs-report=none)
#source: property-gcs.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs -defsym __mult__=0 -defsym __property_gcs__=1
#ld: -z experimental-gcs=always -z experimental-gcs-report=none
#readelf: -n

Displaying notes found in: .note.gnu.property
[ 	]+Owner[ 	]+Data size[ 	]+Description
  GNU                  0x00000010	NT_GNU_PROPERTY_TYPE_0
      Properties: AArch64 feature: GCS
