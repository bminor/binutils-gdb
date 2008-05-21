#source: got-15.s
#as: -mcpu=5206
#ld: -shared --got=negative
#error: .*GOT overflow.*
