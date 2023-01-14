#name: PR ld/18176
#as: --64
#ld: -melf_x86_64 -shared -z relro -T pr18176.t -z max-page-size=0x200000 -z common-page-size=0x1000 $NO_DT_RELR_LDFLAGS
#readelf: -l --wide
#target: x86_64-*-linux*

#...
  GNU_RELRO      0x04bcc7 0x000000000024bcc7 0x000000000024bcc7 0x002339 0x002339 R   0x1
#pass
