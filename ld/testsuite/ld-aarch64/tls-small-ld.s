// Test file for AArch64 LD -- reloc 518

	.text
func:
	add	x1, x2, x3
	// BFD_RELOC_AARCH64_TLSLD_ADR_PAGE21
	adrp	x0, :tlsldm:dummy
