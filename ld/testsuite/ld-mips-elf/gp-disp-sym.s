  .global foo
  .text
foo:
  lui    $t0, %hi(_gp_disp)
  addiu   $t0, $t0, %lo(_gp_disp)
