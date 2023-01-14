.text
        #prefetchit0 (%rcx) PREFETCHIT0/1 apply without RIP-relative addressing, should stay NOPs.
        .byte 0x0f
        .byte 0x18
        .byte 0x39
        #prefetchit1 (%rcx) PREFETCHIT1/1 apply without RIP-relative addressing, should stay NOPs.
        .byte 0x0f
        .byte 0x18
        .byte 0x31
