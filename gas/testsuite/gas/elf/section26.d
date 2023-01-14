#name: sections 26 (.persistent)
#target: [supports_persistent_section]
#source: section26.s
#readelf: -S --wide

#...
  \[..\] .persistent[ 	]+PROGBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#...
  \[..\] .persistent.foo[ 	]+PROGBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#...
  \[..\] .gnu.linkonce.p.bar[ 	]+PROGBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#pass
