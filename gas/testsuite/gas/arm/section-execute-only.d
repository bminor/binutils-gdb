# name: test executable-only section attribute
# as:
# readelf: -t
# This test is only valid on EABI based ports.
# target: *-*-*eabi* *-*-nacl*
There are 10 section headers, starting at offset 0x16c:

Section Headers:
  \[Nr\] Name
       Type            Addr     Off    Size   ES   Lk Inf Al
       Flags
  \[ 0\] 
       NULL            00000000 000000 000000 00   0   0  0
       \[00000000\]: 
  \[ 1\] .text
       PROGBITS        00000000 000034 000000 00   0   0  2
       \[00000006\]: ALLOC, EXEC
  \[ 2\] .data
       PROGBITS        00000000 000034 000000 00   0   0  1
       \[00000003\]: WRITE, ALLOC
  \[ 3\] .bss
       NOBITS          00000000 000034 000000 00   0   0  1
       \[00000003\]: WRITE, ALLOC
  \[ 4\] .text.foo
       PROGBITS        00000000 000034 000010 00   0   0  4
       \[20000006\]: ALLOC, EXEC, ARM_NOREAD
#pass
