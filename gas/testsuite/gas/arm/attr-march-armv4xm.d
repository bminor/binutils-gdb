# name: attributes for -march=armv4xm
# source: blank.s
# as: -march=armv4xm
# readelf: -A
# This test is only valid on ELF based ports.
# not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*

Attribute Section: aeabi
File Attributes
  Tag_CPU_name: "4XM"
  Tag_CPU_arch: v4
  Tag_ARM_ISA_use: Yes
