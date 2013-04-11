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
.sleb128 200+(.L2 - .L1)
.byte 42
.sleb128 200+(.L1 - .L2)
.byte 42
.sleb128 (.L2 - .L1)+200
.byte 42
.sleb128 (.L1 - .L2)+200
.byte 42
