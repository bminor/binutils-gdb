#name: GNU Property (input without gcs ouput forced with experimental-gcs experimental-gcs-report=error)
#source: property-gcs.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs -defsym __mult__=0
#ld: -z experimental-gcs -z experimental-gcs-report=error
#error: .*property-gcs.*: error: GCS turned on by -z experimental-gcs on the output when all inputs do not have GCS in NOTE section.
