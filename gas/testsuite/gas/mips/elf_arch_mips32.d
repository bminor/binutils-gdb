# name: ELF MIPS32 markings
# source: empty.s
# objdump: -p
# as: -march=mips32

.*:.*file format.*elf.*mips.*
private flags = 5.......: .*\[mips32\].*

