$! This file is used to build the binutils on openVMS/AXP.
$! You must have GNU make for VMS.
$! Run this script to build nm, size, objdump, and as.
$ @setup
$ set def [.bfd]
$ gmake
$ set def [-.opcodes]
$ gmake
$ set def [-.libiberty]
$ gmake
$ set def [-.binutils]
$ gmake
$ set def [-.gas]
$ @config-gas-axp
$ gmake
$ set def [-]
