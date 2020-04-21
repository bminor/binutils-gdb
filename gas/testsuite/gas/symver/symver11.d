#readelf: -rsW
#name: symver symver11

#...
[0-9a-f]+ +[0-9a-f]+ +R_.* +[0-9a-f]+ +foo *.*
#...
 +[0-9]+: +0+ +1 +OBJECT +GLOBAL +DEFAULT +[0-9]+ +foo
#pass
