TEMPLATE_NAME=aix
SCRIPT_NAME=aix
OUTPUT_FORMAT="aixcoff64-rs6000"
ARCH=powerpc
SYSCALL_MASK=0xcc
SYMBOL_MODE_MASK=0x0e
#
# This script is called by genscripts.sh.  genscripts does not produce the
# the correct lib search path for the aixppc64 because the default emulation
# is aixppc. Aix libs can have 32 bit and 64 bit objects in them so it is 
# ok for both the aixppc and aixppc64 linkers to look at the same place.
# $libdir is defined in genscripts.sh before aixppc64.sh is sourced in
# so it will be valid.
LIB_PATH="/lib:/usr/lib:$libdir:/usr/local/lib"
