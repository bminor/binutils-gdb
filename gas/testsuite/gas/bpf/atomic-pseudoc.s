        # Test for eBPF atomic pseudo-C instructions.
        .text
	lock *(u64 *)(r1 + 0x1eef) += r2
	lock *(u32 *)(r1 + 0x1eef) += r2
	lock *(u64*)(r1+0x1eef)+=r2
	lock *(u32*)(r1+0x1eef)+=r2
	lock *(u64*)(r1+0x1eef)&=r2
	lock *(u32*)(r1+0x1eef)&=r2
	lock *(u64*)(r1+0x1eef)|=r2
	lock *(u32*)(r1+0x1eef)|=r2
	lock *(u64*)(r1+0x1eef)^=r2
	lock *(u32*)(r1+0x1eef)^=r2
	r2 = atomic_fetch_add((u64*)(r1+0x1eef),r2)
	w2 = atomic_fetch_add((u32*)(r1+0x1eef),w2)
	r2 = atomic_fetch_and((u64*)(r1+0x1eef),r2)
	w2 = atomic_fetch_and((u32*)(r1+0x1eef),w2)
	r2 = atomic_fetch_or((u64*)(r1+0x1eef),r2)
	w2 = atomic_fetch_or((u32*)(r1+0x1eef),w2)
	r2 = atomic_fetch_xor((u64*)(r1+0x1eef),r2)
	w2 = atomic_fetch_xor((u32*)(r1+0x1eef),w2)
