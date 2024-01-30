#name: GNU Property (input with gcs output forced with experimental-gcs=never)
#source: property-gcs.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs -defsym __mult__=0 -defsym __property_gcs__=1
#ld: -z experimental-gcs=never
#readelf: -n
