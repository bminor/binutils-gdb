# name: rgn-at5
# source: rgn-at5.s
# ld: -T rgn-at5.t -z max-page-size=0x1000
# objdump: -w -h
# xfail: rx-*-*
#   FAILS on the RX because the linker has to set LMA == VMA for the
#   Renesas loader.

.*:     file format .*

Sections:
Idx +Name +Size +VMA +LMA +File off +Algn +Flags
  0 .sec0 +0+2 +0+02000 +0+2000 +0+02000 +.*
  1 .sec1 +0+2 +0+01000 +0+012c +0+01000 +.*
  2 .sec2 +0+2 +0+04000 +0+603c +0+04000 +.*
  3 .sec3 +0+2 +0+05000 +0+412c +0+03000 +.*
  4 .sec4 +0+2 +0+02002 +0+2002 +0+02002 +.*
