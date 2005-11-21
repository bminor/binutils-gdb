#mach: crisv10
#ld: --section-start=.text=0
#output: 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 * ixnzvc 0\n
#output: 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 * ixnzvc 1\n
#output: a 0 0 0 0 0 ff004567 0 0 0 0 0 0 0 0 * ixNzvc 3\n
#sim: --cris-trace=basic

 .include "movect10.ms"
