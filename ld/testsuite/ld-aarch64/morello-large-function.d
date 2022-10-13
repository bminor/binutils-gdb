# Mainly here to check that this actually links.
# This testcase used to complain that the function capability may have
# imprecise bounds, but since such capabilities are given PCC bounds that error
# was invalid.
#
# Even though the only point is to check that the testcase links, we still
# ensure that the dump of the .data section contains a relocation with the
# correct permissions.
#as: -march=morello+c64
#ld: -pie -static
#objdump: -DR -j .data

.*:     file format .*


Disassembly of section \.data:

[0-9a-f]+ <__data_start>:
 *[0-9a-f]+:	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+.*
 *[0-9a-f]+:	.* 	udf	#0
 *[0-9a-f]+:	.*
 *[0-9a-f]+:	04000000 	.*
