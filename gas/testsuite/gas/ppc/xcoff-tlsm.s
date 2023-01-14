  .csect foo_tdata[TL],4
  .align 2
foo_tdata:
  .long	1

  .comm foo_tbss[UL],8

  .toc
  .tc foo_tdata[TC],foo_tdata[TL]
  .tc .foo_tdata[TC],foo_tdata[TL]@m
  .tc foo_tbss[TC],foo_tbss[UL]
  .tc .foo_tbss[TC],foo_tbss[UL]@m
