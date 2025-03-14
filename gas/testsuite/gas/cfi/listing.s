	.text
func:
	.cfi_startproc
	.cfi_remember_state
	.nop
	.cfi_escape 0x0a
	.nop
	.cfi_escape 0x02, 0x00, 0x02, 0x00
	.nop
	.cfi_escape 0x03; .cfi_escape 0x00, 0x00
	.cfi_escape 0x04; .cfi_escape 0x00, 0x00, 0x00, 0x00
	.nop
	.cfi_escape 0x0b
	.nop
	.cfi_restore_state
	.nop
	.cfi_endproc

func2:
	.cfi_startproc
	.nop
	.cfi_endproc
