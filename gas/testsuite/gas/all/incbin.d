#as: -I$srcdir/$subdir
#objdump: -s -j .data
#name: incbin

# Test the incbin pseudo-op

.*: .*

Contents of section .data:
 0000 2e646174 610a2e69 6e636269 6e202269  .data..incbin "i
 0010 6e636269 6e2e7322 0a2e696e 6362696e  ncbin.s"..incbin
 0020 2022696e 6362696e 2e73222c 302c3238   "incbin.s",0,28
 0030 0a2e696e 6362696e 2022696e 6362696e  ..incbin "incbin
 0040 2e73222c 31352c39 0a2e7032 616c6967  .s",15,9..p2alig
 0050 6e20340a 2e646174 610a2e69 6e636269  n 4..data..incbi
 0060 6e202269 6e636269 6e2e7322 0a2e696e  n "incbin.s"..in
 0070 696e6362 696e2e73 22000000 00000000  incbin.s".......
