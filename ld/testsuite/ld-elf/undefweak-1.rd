#source: undefweak-1a.s
#source: undefweak-1b.s
#as:
#ld: -r
#readelf: -sW

#failif
#...
.*: 0+ +0 +FUNC +WEAK +DEFAULT +UND +bar
#pass
