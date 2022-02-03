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
#ld: -Ttext-segment 0x100000 -Temit-relocs-morello-7.ld -static
#objdump: -D -j .rela.dyn -j .got -j .text -j .data


.*:     file format .*


Disassembly of section \.text:

0000000000100000 <_start>:
  100000:	c2400c00 	ldr	c0, \[c0, #48\]
  100004:	c2400800 	ldr	c0, \[c0, #32\]
  100008:	c2401000 	ldr	c0, \[c0, #64\]
  10000c:	c2400400 	ldr	c0, \[c0, #16\]

Disassembly of section \.rela\.dyn:

0000000000100010 <__rela_dyn_start>:
  100010:	00101030 	.*
  100014:	00000000 	.*
  100018:	0000e803 	.*
  10001c:	00000000 	.*
  100020:	00000001 	.*
  100024:	00000000 	.*
  100028:	00101020 	.*
  10002c:	00000000 	.*
  100030:	0000e803 	.*
	\.\.\.
  100040:	00101040 	.*
  100044:	00000000 	.*
  100048:	0000e803 	.*
	\.\.\.
  100058:	00101010 	.*
  10005c:	00000000 	.*
  100060:	0000e803 	.*
	\.\.\.

Disassembly of section \.data:

0000000000100070 <data_obj>:
  100070:	0000000a 	.*

0000000000100074 <data_func>:
  100074:	0000000a 	.*

Disassembly of section \.got:

0000000000101000 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
  101010:	00100075 	.*
  101014:	00000000 	.*
  101018:	00000402 	.*
  10101c:	00000000 	.*
  101020:	00100000 	.*
  101024:	00000000 	.*
  101028:	00105001 	.*
  10102c:	00000000 	.*
  101030:	00100000 	.*
  101034:	00000000 	.*
  101038:	00105001 	.*
  10103c:	00000000 	.*
  101040:	00100070 	.*
  101044:	00000000 	.*
  101048:	00000402 	.*
  10104c:	00000000 	.*
