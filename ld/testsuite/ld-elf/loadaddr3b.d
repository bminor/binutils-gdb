#source: loadaddr.s
#ld: -T loadaddr3.t
#objdump: -t
#target: *-*-elf*

#...
0+0000100 l    d  .text	0+0000000 .text
0+0000200 l    d  .data	0+0000000 .data
#...
0+0000101 g       \*ABS\*	0+0000000 data_load
#...
0+0000200 g       .data	0+0000000 data_start
#pass
