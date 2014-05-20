# name: .module command line override
# source: module-override.s
# objdump: -p
# as: -32 -march=mips32r2

.*:.*file format.*elf.*mips.*
private flags = 1.......: .*\[mips2\].*
