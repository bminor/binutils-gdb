#name: '-z gcs=never -z gcs-report=error' and shared libraries without GCS feature reports no warning/error.
#source: gcs.s
#source: gcs2.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs  -defsym __property_gcs__=1
#ld: -z gcs=never -z gcs-report=error -L./tmpdir -lnogcs-so -lbti-plt-so -lgcs-so2
#readelf: -n
