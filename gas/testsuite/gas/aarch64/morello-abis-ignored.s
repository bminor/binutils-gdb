  .arch morello+c64
  .type foo, @function
foo:
  mov x0, #0
  ret c30
  .arch morello
  .type bar,@function
bar:
  mov x0, #0
  ret c30
  .arch morello+c64
  .type baz, @function
baz:
  mov x0, #0
  ret c30
