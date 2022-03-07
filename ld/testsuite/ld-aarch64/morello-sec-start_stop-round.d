#as: -march=morello+c64
#ld: -static
#objdump: -d -j .data -j __libc_atexit

.*:     file format .*


Disassembly of section \.data:

[0-9a-f]+ <__data_start>:
#record: START_LIBC_ADDR
.*:	([0-9a-f]+) 	.*
.*:	00000000 	.*
.*:	00000008 	.*
.*:	02000000 	.*

Disassembly of section __libc_atexit:

# Use `string tolower` because we know we only have a number so it won't change
# anything.  That's needed because the current record/check implementation
# doesn't have a way to define a replacement which is just the existing
# variable.
#check: START_LIBC string tolower $START_LIBC_ADDR
00000000START_LIBC <__start___libc_atexit>:
.*:	0000002a 	.*
.*:	00000000 	.*
