# name: EABI attribute ordering
# source: attr-order.s
# as:
# readelf: -A
# This test is only valid on ELF based ports.
# not-target: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*

Attribute Section: aeabi
File Attributes
  Tag_conformance: "2.07"
  Tag_nodefaults: True
  Tag_CPU_name: "ARM7TDMI"
  Tag_CPU_arch: v4T
  Tag_unknown_63: "val"
  Tag_also_compatible_with: v6-M
  Tag_T2EE_use: Allowed
  Tag_Virtualization_use: Allowed
