        # Test for eBPF atomic instructions
        .text
        aadd	[%r1+0x1eef], %r2
        aadd32	[%r1+0x1eef], %r2
        aand	[%r1+0x1eef], %r2
        aand32	[%r1+0x1eef], %r2
        aor	[%r1+0x1eef], %r2
        aor32	[%r1+0x1eef], %r2
        axor	[%r1+0x1eef], %r2
        axor32	[%r1+0x1eef], %r2

        afadd	[%r1+0x1eef], %r2
        afadd32	[%r1+0x1eef], %r2
        afand	[%r1+0x1eef], %r2
        afand32	[%r1+0x1eef], %r2
        afor	[%r1+0x1eef], %r2
        afor32	[%r1+0x1eef], %r2
        afxor	[%r1+0x1eef], %r2
        afxor32	[%r1+0x1eef], %r2
