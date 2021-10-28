#readelf: -u
#source: unwind-pacbti-m.s
#name: Unwind table information for Armv8.1-M.Mainline PACBTI extension
# This test is only valid on ELF based ports.
#notarget: *-*-pe *-*-wince
# VxWorks needs a special variant of this file.
#skip: *-*-vxworks*

Unwind section '.ARM.exidx' at offset 0x40 contains 1 entry:

0x0 <foo>: 0x80b4a8b0
  Compact model index: 0
  0xb4      pop {ra_auth_code}
  0xa8      pop {r4, r14}
  0xb0      finish

