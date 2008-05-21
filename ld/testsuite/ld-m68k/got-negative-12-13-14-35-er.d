#source: got-12.s
#source: got-13.s
#source: got-14.s
#source: got-35.s
#as: -mcpu=5206
#ld: -shared --got=negative
#error: .*relocation truncated to fit.*
