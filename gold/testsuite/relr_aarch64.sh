#!/bin/sh
awk 'BEGIN {
  i = 0
  ps[i++] = "\\(RELR\\)"
  ps[i++] = "\\(RELRSZ\\) *16 \\(bytes\\)"
  ps[i++] = "\\(RELRENT\\) *8 \\(bytes\\)"

  ps[i++] = ".relr.dyn.* contains 2 entries"
  ps[i++] = "4 offsets"
  ps[i++] = "0000000000020000"
  ps[i++] = "0000000000020010"
  ps[i++] = "0000000000020018"
  ps[i++] = "0000000000020020"

  ps[i++] = ".rela.dyn.* contains 1 entry:"
  ps[i++] = "0000000000020029 .*R_AARCH64_RELATIVE.* a"
  i = 0
}
i in ps && $0 ~ ps[i] {i++}
END { if (i in ps) { print ps[i] " is not found"; exit 1 } }' relr_aarch64.out
