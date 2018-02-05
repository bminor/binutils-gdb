#PROG: objcopy
#name: MIPS objcopy .reginfo section size
#as: -32
#objcopy: --rename-section .foo=.reginfo
#error: \A[^\n]*: Incorrect `\.reginfo' section size; expected 24, got 4\n
#error:   [^\n]*: Bad value\Z
