#source: pr19617.s
#ld: --dynamic-list-data
#readelf : --dyn-syms --wide

#failif
#...
 +[0-9]+: +[a-f0-9]+ +0 +FUNC +GLOBAL +DEFAULT +[0-9]+ +start
#...
