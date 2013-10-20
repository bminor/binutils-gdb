#source: start.s
#source: zerosize1.s
#ld:
#readelf: -s

# Check that xyzzy is not placed in the .text section.

#...
 +[0-9]+: +[0-9a-f]+ +0 +(OBJECT|NOTYPE) +GLOBAL +DEFAULT +ABS xyzzy
#pass
