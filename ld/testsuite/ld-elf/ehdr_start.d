#source: ehdr_start.s
#ld: -e _start
#nm: -n
#notarget: arm*-*-eabi* cris-*-*

#...
[0-9a-f]*000 [ADRT] __ehdr_start
#pass
