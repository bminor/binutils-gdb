# name: ELF MIPS1 markings
# source: empty.s
# objdump: -p
# as: -march=mips1

.*:.*file format.*elf.*mips.*
# Note: objdump omits leading zeros, so must check for the fact that
# flags are _not_ 8 chars long.
private flags = (.......|......|.....|....|...|..|.): .*\[mips1\].*

