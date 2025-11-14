# Testcase to ensure .cfi_register for RA is not treated as safe to skip.
# Even for ABIs which do not track RA, an explicit manouver of RA like so
# cannot be represented.  The expectation is that such a pattern is caught and
# SFrame FDE is skipped. Such a sequence may be used in longjump implementation
# on x86_64, for instance.
	.cfi_startproc
	.long 0
	.cfi_def_cfa_offset 16
	.cfi_register 16, 9
	.long 0
	.cfi_endproc
