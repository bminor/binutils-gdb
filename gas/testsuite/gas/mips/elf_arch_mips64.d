# name: ELF MIPS64 markings
# source: empty.s
# objdump: -p
# as: -march=mips64

.*:.*file format.*elf.*mips.*
private flags = 6.......: .*\[mips64\].*

