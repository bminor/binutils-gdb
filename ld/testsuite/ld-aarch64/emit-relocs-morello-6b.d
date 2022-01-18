# Check handling relocations into the got that require a GOT entry.
# This case handles non-PIE binaries.
#
# This testcase uses exact values in order to check that of the two GOT entries
# created, the one that is referenced by the first instruction in _start is
# the one which has the LSB set in its value.
#
# It's difficult to check this in the DejaGNU testsuite without checking for
# specific values that we know are good.  However this is susceptible to
# defaults changing where the .text and .got sections end up.
#
# If this testcase prooves to be too flaky while the linker gets updated then
# we should look harder for some solution, but for now we'll take this
# tradeoff.
#
# Here we have to use a format which dumps the hex of the relocation section
# since objdump does not show us dynamic relocations on a non-dynamic binary.
#source: emit-relocs-morello-6.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x0 -static
#objdump: -D -j .rela.dyn -j .got -j .text


.*:     file format .*


Disassembly of section \.rela\.dyn:

0000000000000000 <__rela_dyn_start>:
   0:	00010060 .*
   4:	00000000 .*
   8:	0000e803 .*
	\.\.\.
  18:	00010050 .*
  1c:	00000000 .*
  20:	0000e803 .*
	\.\.\.

Disassembly of section \.text:

0000000000000030 <_start>:
  30:	c2401800 	ldr	c0, \[c0, #96\]
  34:	c2401400 	ldr	c0, \[c0, #80\]

Disassembly of section \.got:

0000000000010040 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   10050:	00000030 .*
   10054:	00000000 .*
   10058:	00000101 .*
   1005c:	00000000 .*
   10060:	00000031 .*
   10064:	00000000 .*
   10068:	00000c01 .*
   1006c:	00000000 .*
