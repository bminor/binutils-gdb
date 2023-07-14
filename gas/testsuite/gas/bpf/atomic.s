        # Test for eBPF ADDW and ADDDW instructions
        .text
        aadd	[%r1+0x1eef], %r2
        aadd32	[%r1+0x1eef], %r2
        
