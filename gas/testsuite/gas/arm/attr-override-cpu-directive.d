# name: EABI attributes .eabi_attribute overrides .cpu
# source: attr-override-cpu-directive.s
# as:
# readelf: -A
# This test is only valid on ELF based ports.
# not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*

Attribute Section: aeabi
File Attributes
  Tag_CPU_name: "custom name"
  Tag_CPU_arch: v7
  Tag_THUMB_ISA_use: \?\?\? \(10\)
