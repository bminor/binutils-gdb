# name: ELF MIPS5 markings
# source: empty.s
# objdump: -p
# as: -march=mips5

.*:.*file format.*elf.*mips.*
private flags = 4.......: .*\[mips5\].*

