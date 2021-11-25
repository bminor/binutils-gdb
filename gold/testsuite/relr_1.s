.data
.balign 8
.globl data
data:

.dc.a .data
.dc.a 0
.dc.a data + 2
.dc.a __ehdr_start + 4
.dc.a __ehdr_start + 9

.byte 0
// Offset is not a multiple of 2. Don't use RELR.
.dc.a __ehdr_start + 10
