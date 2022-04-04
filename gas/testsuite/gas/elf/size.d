#readelf: -sW
#name: ELF symbol size

#...
 +[0-9]+: 0+ +1 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +foo1
 +[0-9]+: 0+ +2 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar1
 +[0-9]+: 0+ +2 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar2
 +[0-9]+: 0+ +2 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar3
 +[0-9]+: 0+ +3 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +foo2
 +[0-9]+: 0+ +2 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar4
 +[0-9]+: 0+ +1 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar5
 +[0-9]+: 0+ +3 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar6
 +[0-9]+: 0+ +3 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +bar7
#pass
