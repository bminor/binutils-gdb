# name: ELF MIPS4 markings
# source: empty.s
# objdump: -p
# as: -march=mips4

.*:.*file format.*elf.*mips.*
private flags = 3.......: .*\[mips4\].*

