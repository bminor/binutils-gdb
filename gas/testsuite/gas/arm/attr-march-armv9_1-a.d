# name: check for armv9-a command line option settings
# source: blank.s
# as: -march=armv9.1-a
# readelf: -A
# This test is only valid on EABI based ports.
# target: *-*-*eabi* *-*-nacl*

Attribute Section: aeabi
File Attributes
  Tag_CPU_name: "9.1-A"
  Tag_CPU_arch: v9
  Tag_CPU_arch_profile: Application
  Tag_ARM_ISA_use: Yes
  Tag_THUMB_ISA_use: Thumb-2
  Tag_Advanced_SIMD_arch: NEON for ARMv8.1
  Tag_MPextension_use: Allowed
  Tag_Virtualization_use: TrustZone and Virtualization Extensions
