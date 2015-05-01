# Test SPARC "natural" registers and instructions
        .text
        ba %ncc, 0f
         nop
0:      nop
