#name: Specifying only '-z gcs=implicit' and GCS-unmarked inputs emits no GCS feature without warnings [shared]
#source: gcs.s
#source: gcs2.s
#alltargets: [check_shared_lib_support] *linux*
#as: -march=armv9.4-a+gcs
#ld: -shared -z gcs=implicit
#readelf: -n
