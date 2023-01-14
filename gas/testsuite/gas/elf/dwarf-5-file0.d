#as: --gdwarf-5
#name: DWARF5 .line 0
#readelf: -wl

#...
 The Directory Table \(offset 0x.*, lines 3, columns 1\):
  Entry	Name
  0	master directory
  1	secondary directory
  2	/tmp

 The File Name Table \(offset 0x.*, lines 3, columns 3\):
  Entry	Dir	MD5				Name
  0	0 0x00000000000000000000000000000000	master source file
  1	1 0x00000000000000000000000000000000	secondary source file
  2	2 0x95828e8bc4f7404dbf7526fb7bd0f192	foo.c
#pass


