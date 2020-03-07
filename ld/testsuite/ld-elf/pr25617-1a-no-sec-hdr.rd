#readelf: -lWSDs

There are no sections in this file.

#...
Program Headers:
  Type +Offset +VirtAddr.*
# On MIPS, the first segment is for .reginfo.
#...
  LOAD .*
#...
  DYNAMIC .*
#...
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 +NOTYPE +LOCAL +DEFAULT +UND +
#...
 +[0-9]+: +[a-f0-9]+ +0 +FUNC +GLOBAL +DEFAULT +UND +_?puts(@.*|)
#...
 +[0-9]+: +[a-f0-9]+ +[0-9]+ +FUNC +GLOBAL +DEFAULT +[0-9]+ +_?test
#pass
