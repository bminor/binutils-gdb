#name: readelf -ws readelf-debug-str-offsets-dw4
#source: readelf-debug-str-offsets-dw4.s
#readelf: -ws

Contents of the .debug_str.dwo section:

  0x00000000 46495253 54005345 434f4e44 00       FIRST.SECOND.

Contents of the .debug_str_offsets.dwo section:

    Length: 0x8
       Index   Offset \[String\]
           0 00000000  FIRST
           1 00000006  SECOND
