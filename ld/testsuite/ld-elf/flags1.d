#name: --set-section-flags test 1 (sections)
#ld: -Tflags1.ld
#objcopy_linked_file: --set-section-flags .post_text_reserve=contents,alloc,load,readonly,code 
#readelf: -l --wide

#...
Program Headers:
  Type.*
  LOAD +0x[0-9a-f]+ 0x0*0 0x0*0 0x0*016[1-8] 0x0*016[1-8] RWE 0x[0-9a-f]+

#...
  Segment Sections...
   00[ \t]+.text .post_text_reserve[ \t]*
#pass
