#ld: -e _start --rosegment -z separate-code -z max-page-size=0x1000 -z common-page-size=0x1000
#readelf: -l --wide
#target: x86_64-*-linux* i?86-*-linux-gnu i?86-*-gnu*

#...
 +TLS +0x0+1014 .*
#...
.* \.tbss 
#pass
