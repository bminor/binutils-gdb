#name: GNU Property (input without gcs output forced with gcs gcs-report=error)
#source: property-gcs.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs -defsym __mult__=0
#ld: -z gcs -z gcs-report=error
#error: .*property-gcs.*: error: GCS turned on by -z gcs on the output when all inputs do not have GCS in NOTE section.
