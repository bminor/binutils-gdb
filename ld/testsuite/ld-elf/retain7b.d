#name: SHF_GNU_RETAIN 7a
#source: retain7.s
#ld: -r
#notarget: ![supports_gnu_osabi] ![check_gc_sections_available]
#readelf: -h

ELF Header:
#...
  OS/ABI:                            UNIX - GNU
#pass
