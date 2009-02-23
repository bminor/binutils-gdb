#name: Thumb-Thumb farcall without BLX (PIC veneer)
#source: farcall-thumb-thumb.s
#as: -march=armv4t
#ld: -Ttext 0x1000 --section-start .foo=0x2001014 --pic-veneer
#error: .*\(.text\+0x0\): relocation truncated to fit: R_ARM_THM_CALL against `bar'
