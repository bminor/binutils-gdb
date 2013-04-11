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
.sleb128 .L1 - .L2
.byte 42
