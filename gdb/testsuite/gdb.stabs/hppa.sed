# GDB legitimately expects a file name.
# The sun3 assembler bogusly requires that the value of this stab be a
# label.  Placate it.
1i\
	.stabs "weird.c",0x64,0,0,Label0\
Label0:
#
s/N_LSYM/0x80/
s/N_GSYM/0x20/
# Replace a single backslash with a doubled backslash
/\.stabs/s/\\/\\\\/
s/\.begin_common\(.*\)/.stabs \1,0xe2,0,0,0/
s/\.end_common\(.*\)/.stabs \1,0xe4,0,0,0/
s/\.align_it/.align 4/
s/\.globl/.export/
/^#/d
/.if alpha/,/.endif alpha/d
/.if/d
/.endif/d
