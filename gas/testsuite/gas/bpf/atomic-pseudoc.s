        # Test for eBPF ADDW and ADDDW pseudo-C instructions
        .text
	*(u64 *)(r1 + 7919) += r2
	*(u32 *)(r1 + 7919) += r2
