#as: -I$srcdir/$subdir
#objdump: -s -j .data
#name: incbin

# Test the incbin pseudo-op

.*: .*

Contents of section .data:
 0000 202e6461 74610a20 2e696e63 62696e20   .data. .incbin 
 0010 22696e63 62696e2e 73220a20 2e696e63  "incbin.s". .inc
 0020 62696e20 22696e63 62696e2e 73222c30  bin "incbin.s",0
 0030 2c32380a 202e696e 6362696e 2022696e  ,28. .incbin "in
 0040 6362696e 2e73222c 31352c39 0a202e70  cbin.s",15,9. .p
 0050 32616c69 676e2034 0a202e64 6174610a  2align 4. .data.
 0060 202e696e 6362696e 2022696e 6362696e   .incbin "incbin
 0070 2e73220a 20202269 6e636269 6e2e0000  .s".  "incbin...
