#name: sections 25 (.noinit)
#target: [supports_noinit_section]
#source: section25.s
#readelf: -S --wide

#...
  \[..\] .noinit[ 	]+NOBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#...
  \[..\] .noinit.foo[ 	]+NOBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#...
  \[..\] .gnu.linkonce.n.bar[ 	]+NOBITS[ 	]+[0-9a-f]+ [0-9a-f]+ [0-9a-f]+ 00  WA .*
#pass
