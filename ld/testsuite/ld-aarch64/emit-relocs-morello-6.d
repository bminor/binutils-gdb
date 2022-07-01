# Check handling relocations into the got that require a GOT entry.
# This case handles PIE binaries.
#
# This testcase checks that of the GOT entries created, the ones that are
# referenced by the first and ast instructions are the one which have the LSB
# set in their value.
#source: emit-relocs-morello-6.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x100000 -T emit-relocs-morello-7.ld -pie -static
#objdump: -DR -j .got -j .text -j .data


.*:     file format .*


Disassembly of section \.text:

#record: START_ADDR
([0-9a-f]+) <_start>:
  .*	ldr	c0, \[c0, #48\]
  .*	ldr	c0, \[c0, #32\]
  .*	ldr	c0, \[c0, #64\]
  .*	ldr	c0, \[c0, #16\]

Disassembly of section \.data:

#record: DATA_OBJ_ADDR
([0-9a-f]+) <data_obj>:
  .*:	0000000a 	.*

#record: DATA_FUNC_ADDR
([0-9a-f]+) <data_func>:
  .*:	0000000a 	.*

Disassembly of section \.got:

#record: GOT_ADDR
([0-9a-f]+) <\.got>:
  .*
	\.\.\.
#check: DATA_FUNC aarch64_8digit_addr $DATA_FUNC_ADDR 1
  .*:	DATA_FUNC 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
  .*:	00000000 	.*
  .*:	00000004 	.*
  .*:	02000000 	.*
#check: START_ADDR_NOFUNC aarch64_8digit_addr $START_ADDR
  .*:	START_ADDR_NOFUNC 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
  .*:	00000000 	.*
#record: PCC_BOUNDS
  .*:	(00000[1-9a-f][0-9a-f]+) 	.*
  .*:	04000000 	.*
#check: START_ADDR_FUNC aarch64_8digit_addr $START_ADDR
  .*:	START_ADDR_FUNC 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0x1
  .*:	00000000 	.*
#check: PCC_BOUNDED string tolower $PCC_BOUNDS
  .*:	PCC_BOUNDED 	.*
  .*:	04000000 	.*
#check: DATA_OBJ aarch64_8digit_addr $DATA_OBJ_ADDR
  .*:	DATA_OBJ 	.*
			.*: R_MORELLO_RELATIVE	\*ABS\*
  .*:	00000000 	.*
  .*:	00000004 	.*
  .*:	02000000 	.*
