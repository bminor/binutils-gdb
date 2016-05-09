#name: --set-section-flags test 1 (sections)
#ld: -Tflags1.ld
#objcopy_linked_file: --set-section-flags .post_text_reserve=contents,alloc,load,readonly,code
#readelf: -S --wide
#xfail: "dlx-*-*" "h8300-*-*" "i960-*-*" "ip2k-*-*" "m32r-*-*"
#xfail: "moxie-*-*" "mt-*-*" "visium-*-*" "*-*-nacl*"
#xfail: "*-*-hpux*" "hppa*64*-*-*"
# Fails on the DLX, H8300, I960, IP2K, M32R, MOXIE, MT, VISIUM
#  and all NaCl targets,
#  because the two sections are not merged into one segment.
#  (There is no good reason why they have to be).
# Fails on HPUX systems because the .type pseudo-op behaves differently.
# Fails on hppa64 because a PHDR is always added.

#...
Section Headers:
#...
  \[[ 0-9]+\] \.text.*[ \t]+PROGBITS[ \t0-9a-f]+AX.*
  \[[ 0-9]+\] \.post_text_reserve.*[ \t]+PROGBITS[ \t0-9a-f]+AX.*
#pass
