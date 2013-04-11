.text
.globl foo
foo:
.L1:
.byte 0
.byte 0
.byte 0
.L2:

.data
bar:
.sleb128 .L2 - .L1 + (1 << 31)
.byte 42
