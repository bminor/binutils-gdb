# name: attributes for -march=armv7
# source: blank.s
# as: -march=armv7
# readelf: -A
# This test is only valid on ELF based ports.
# not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*

Attribute Section: aeabi
File Attributes
  Tag_CPU_name: "7"
  Tag_CPU_arch: v7
  Tag_THUMB_ISA_use: Thumb-2
