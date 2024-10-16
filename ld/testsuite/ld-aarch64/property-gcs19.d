#name: GNU Property (input without gcs output forced with gcs=never)
#source: property-gcs.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs -defsym __mult__=0
#ld: -z gcs=never
#readelf: -n
