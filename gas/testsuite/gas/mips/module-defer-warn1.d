# name: .module deferred warnings
# source: module-defer-warn1.s
# objdump: -p
# as: -32 -march=mips2 -mgp64

.*:.*file format.*elf.*mips.*
private flags = 1.......: .*\[mips2\].*
