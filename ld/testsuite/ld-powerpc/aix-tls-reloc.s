  .globl bar[TL]
  .csect bar[TL]
  .long 1

  .toc
  .tc gd[TC],bar[TL]
  .tc .gd[TC],bar[TL]@m
  .tc ld[TC],bar[TL]@ld
  .tc mh[TC],mh[TC]@ml
  .tc ie[TC],bar[TL]@ie
  .tc le[TC],bar[TL]@le

  .globl foo
  .globl .foo
  .csect foo[DS],3
foo:
  .if size == 32
  .long	.foo, TOC[tc0], 0
  .else
  .llong .foo, TOC[tc0], 0
  .endif

  .csect foo[PR]
.foo:
  #GD
  .if size == 32
  lwz 4, gd[TC](2)
  lwz 3, .gd[TC](2)
  .else
  ld 4, gd[TC](2)
  ld 3, .gd[TC](2)
  .endif
  bla __tls_get_addr

  #LD
  .if size == 32
  lwz 3, mh[TC](2)
  .else
  ld 3, mh[TC](2)
  .endif
  bla __tls_get_mod
  lwz 4, ld[TC](2)
  add 5,3,4

  #IE
  .if size == 32
  bla __get_tpointer
  lwz 4, ie[TC](2)
  add 5,3,4
  .else
  ld 4, ie[TC](2)
  add 5,4,13
  .endif

  #LE
  .if size == 32
  bla __get_tpointer
  lwz 4, le[TC](2)
  add 5,3,4
  .else
  ld 4, le[TC](2)
  add 5,3,13
  .endif

.rename mh[TC], "_$TLSML" # Symbol for the module handle
