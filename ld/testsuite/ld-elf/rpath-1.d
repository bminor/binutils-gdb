#source: start.s
#readelf: -d -W
#ld: -shared -rpath=. --disable-new-dtags
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#failif
#...
 +0x[0-9a-f]+ +\(RUNPATH\) +Library runpath: +\[.\]
#...
