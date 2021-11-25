#!/bin/sh
awk 'BEGIN {
  i = 0
  ps[i++] = "\\(RELR\\)"
  ps[i++] = "\\(RELRSZ\\) *16 \\(bytes\\)"
  ps[i++] = "\\(RELRENT\\) *8 \\(bytes\\)"

  ps[i++] = ".relr.dyn.* contains 2 entries"
  ps[i++] = "4 offsets"
  ps[i++] = "0000000000002000"
  ps[i++] = "0000000000002010"
  ps[i++] = "0000000000002018"
  ps[i++] = "0000000000002020"

  ps[i++] = ".rela.dyn.* contains 1 entry:"
  ps[i++] = "0000000000002029 .*R_X86_64_RELATIVE.* a"
  i = 0
}
i in ps && $0 ~ ps[i] {i++}
END { if (i in ps) { print ps[i] " is not found"; exit 1 } }' relr_x86_64.out
