        # eBPF v3 atomic instructions
        .text
        xadddw	[%r1+0x1eef], %r2
        xaddw	[%r1+0x1eef], %r2
        xordw	[%r1+0x1eef], %r2
        xorw	[%r1+0x1eef], %r2
        xanddw	[%r1+0x1eef], %r2
        xandw	[%r1+0x1eef], %r2
        xxordw	[%r1+0x1eef], %r2
        xxorw	[%r1+0x1eef], %r2
        xfadddw	[%r1+0x1eef], %r2
        xfaddw	[%r1+0x1eef], %r2
        xfordw	[%r1+0x1eef], %r2
        xforw	[%r1+0x1eef], %r2
        xfanddw	[%r1+0x1eef], %r2
        xfandw	[%r1+0x1eef], %r2
        xfxordw	[%r1+0x1eef], %r2
        xfxorw	[%r1+0x1eef], %r2
        xchgdw	[%r1+0x1eef], %r2
        xchgw	[%r1+0x1eef], %r2
        xcmpdw	[%r1+0x1eef], %r2
        xcmpw	[%r1+0x1eef], %r2
