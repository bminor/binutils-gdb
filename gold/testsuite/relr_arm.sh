#!/bin/sh
awk 'BEGIN {
  i = 0
  ps[i++] = "\\(RELR\\)"
  ps[i++] = "\\(RELRSZ\\) *8 \\(bytes\\)"
  ps[i++] = "\\(RELRENT\\) *4 \\(bytes\\)"

  ps[i++] = ".relr.dyn.* contains 2 entries"
  ps[i++] = "4 offsets"
  ps[i++] = "00002000"
  ps[i++] = "00002008"
  ps[i++] = "0000200c"
  ps[i++] = "00002010"

  ps[i++] = ".rel.dyn.* contains 1 entry:"
  ps[i++] = "00002015 .*R_ARM_RELATIVE"
  i = 0
}
i in ps && $0 ~ ps[i] {i++}
END { if (i in ps) { print ps[i] " is not found"; exit 1 } }' relr_arm.out
