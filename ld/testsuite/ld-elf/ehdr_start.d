#source: ehdr_start.s
#ld: -e _start
#nm: -n
#target: *-*-linux* *-*-gnu*

#...
[0-9a-f]*000 [ADRT] __ehdr_start
#pass
