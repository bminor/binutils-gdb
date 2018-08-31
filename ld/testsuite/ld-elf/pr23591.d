#source: pr23591a.s
#source: pr23591b.s
#source: pr23591c.s
#ld: -e _start
#readelf: -sW

#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +GLOBAL +HIDDEN +[0-9]+ +___?start___sancov_cntrs
#pass
