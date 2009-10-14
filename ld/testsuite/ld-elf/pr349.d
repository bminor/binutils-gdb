#source: pr349-1.s
#source: pr349-2.s
#ld: -r
#readelf: -S

#...
.* .abcxyz .*
#...
.* .abcxyz .*
#pass
