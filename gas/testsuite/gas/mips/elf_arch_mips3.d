# name: ELF MIPS3 markings
# source: empty.s
# objdump: -p
# as: -march=mips3

.*:.*file format.*elf.*mips.*
private flags = 2.......: .*\[mips3\].*

